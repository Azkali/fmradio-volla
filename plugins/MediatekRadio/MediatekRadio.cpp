/*
 * Copyright (C) 2020 venji10 <bennisteinir@gmail.com>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#include <stdio.h>

#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <initializer_list>


#include "MediatekRadio.h"
#include "common.cpp"

#define FM_DEV_NAME "/dev/fm"

#define FM_BAND_UE      1 // US/Europe band  87.5MHz ~ 108MHz (DEFAULT)
#define FM_BAND_JAPAN   2 // Japan band      76MHz   ~ 90MHz
#define FM_BAND_JAPANW  3 // Japan wideband  76MHZ   ~ 108MHz
#define FM_BAND_SPECIAL 4 // special   band  between 76MHZ   and  108MHz

// ---------------------------------------------------------------------------
// PulseAudio helpers
//
// Node names differ between pulseaudio-modules-droid releases:
//   legacy (Halium 9/10):  source.droid          / sink.droid
//   newer  (Halium 11+):   source.primary_input  / sink.primary_output
// so probe them once instead of hardcoding.
// ---------------------------------------------------------------------------

// Run a shell command and return its stdout, one line per element.
static std::vector<std::string> runLines(const std::string &cmd)
{
	std::vector<std::string> lines;
	FILE *fp = popen(cmd.c_str(), "r");
	if (!fp)
		return lines;

	char buf[256];
	while (fgets(buf, sizeof buf, fp)) {
		buf[strcspn(buf, "\n")] = '\0';
		lines.emplace_back(buf);
	}
	pclose(fp);
	return lines;
}

// Pick the first existing PulseAudio node name from a candidate list.
// kind is "sources" or "sinks".
static std::string findNode(const char *kind, std::initializer_list<const char *> candidates)
{
	const auto names = runLines(std::string("/usr/bin/pactl list short ") + kind + " | /usr/bin/awk '{ print $2 }'");
	for (const char *c : candidates)
		for (const auto &n : names)
			if (n == c)
				return n;
	// nothing matched; fall back to the first candidate so the error is visible in logs
	return *candidates.begin();
}

static const std::string &paSource()
{
	static const std::string s = findNode("sources", {"source.primary_input", "source.droid"});
	return s;
}

static const std::string &paSink()
{
	static const std::string s = findNode("sinks", {"sink.primary_output", "sink.droid"});
	return s;
}

static int sh(const std::string &cmd)
{
	return system(cmd.c_str());
}

MediatekRadio::MediatekRadio() {

}

// check if headset/headphones are connected, they act as antenna
bool MediatekRadio::isHeadsetAvailable() {

	isHeadset = false;

	const auto ports = runLines(
		"/usr/bin/pactl list sinks | "
		"/usr/bin/awk '/^Sink #/ { droid = 0 } /Name: " + paSink() + "$/ { droid = 1 } droid && /Active Port:/ { print $3 }'");

	for (const auto &port : ports) {
		if (port == "output-wired_headset") {
			isHeadset = true;   // headset with mic: restore input-wired_headset on stop
			return true;
		}
		if (port == "output-wired_headphone")
			return true;
	}
	return false;
}

// This is needed to route the FM input to the headphones
void MediatekRadio::preparePulseAudio() {

	sh("pacmd set-source-port " + paSource() + " input-fm_tuner");
	sh("pactl load-module module-loopback source=" + paSource() + " sink=" + paSink());

}

bool MediatekRadio::isRadioRunning() {

	return radioRunning;

}

// Volume is always at 100% without this
void MediatekRadio::startVolumeUpdater() {

	sh("touch ~/.radioRunning");
	sh("while ( test -f ~/.radioRunning ) do "
	   "pactl set-source-volume " + paSource() + " "
	   "$(printf \"%.*f\\n\" 0 $(echo print $(dbus-send --session --type=method_call --print-reply "
	   "--dest=org.ayatana.indicator.sound /org/ayatana/indicator/sound org.gtk.Actions.DescribeAll "
	   "| grep -A5 \"string \\\"volume\\\"\" | grep double | cut -b 49-52)*65536 | perl)); "
	   "sleep 0.5; done &");

}

void MediatekRadio::stopVolumeUpdater() {

	sh("rm -f ~/.radioRunning");
	sh("pactl set-source-volume " + paSource() + " 65536"); // 100%

}

// Start the radio
QByteArray MediatekRadio::startRadio(int freq) {

	if(isHeadsetAvailable()) {

		int ret = 0;

		if((ret = COM_open_dev(FM_DEV_NAME, &idx)) < 0) {
			printf("error opening device: %d\n", ret);
			return "Error";
		}

		if((ret = COM_pwr_up(idx, FM_BAND_UE, freq)) < 0) {
			printf("error powering up: %d\n", ret);
			return "Error";
		}

		preparePulseAudio();

		radioRunning = true;
		startVolumeUpdater();

		return "Stop radio";
	} else {
		return "Headset not available";
	}

}

// Stop the radio
QByteArray MediatekRadio::stopRadio() {

	int ret = 0;

	if((ret = COM_pwr_down(idx, 0)) < 0) {
		printf("error powering down: %d\n", ret);
		return "Error";
	}

	if((ret = COM_close_dev(idx)) < 0) {
		printf("error closing device; %d\n", ret);
		return "Error";
	}

	ret = system("pactl unload-module module-loopback");

	if(isHeadset) {
		ret = sh("pacmd set-source-port " + paSource() + " input-wired_headset && pacmd set-sink-port " + paSink() + " output-wired_headset");
	} else {
		ret = sh("pacmd set-source-port " + paSource() + " input-builtin_mic && pacmd set-sink-port " + paSink() + " output-wired_headphone");
	}

	stopVolumeUpdater();

	radioRunning = false;

	return "Start radio";

}

// Tune to different frequency
void MediatekRadio::tune(int freq) {

	if(isRadioRunning()) {
		int ret;

		if((ret = COM_tune(idx, freq, FM_BAND_UE)) < 0) {
			printf("error tuning: %d\n", ret);
		}
	} else {
		printf("tune: radio not running\n");
	}

	frequency = freq;

}

void MediatekRadio::mute() {

	int ret;

	if((ret = COM_set_mute(idx, 1)) < 0) {
		printf("error muting: %d\n", ret);
	}

}

void MediatekRadio::unmute() {

	int ret;

	if((ret = COM_set_mute(idx, 0)) < 0) {
		printf("error unmuting: %d\n", ret);
	}


}

int MediatekRadio::getRssi() {

	int ret;
	int rssi;

	if((ret = COM_get_rssi(idx, &rssi)) < 0) {
		printf("error getting rssi: %d\n", ret);
		return -100;
	}

	return rssi;

}

int MediatekRadio::seekUp() {

	if(radioRunning) {

		mute();

		bool foundStation = false;
		int tmp = frequency;

		for(int i = frequency + 10; i < 10800; i += 10) {
			tune(i);
			if(getRssi() > -75) {
				foundStation = true;
				break;
			}
		}

		if(!foundStation) {
			tune(tmp);
		}
		unmute();

	}

	return frequency;

}

int MediatekRadio::seekDown() {

	if(radioRunning) {

		mute();

		bool foundStation = false;
		int tmp = frequency;

		for(int i = frequency - 10; i > 8750; i -= 10) {
			tune(i);
			if(getRssi() > -75) {
				foundStation = true;
				break;
			}
		}

		if(!foundStation) {
			tune(tmp);
		}
		unmute();

	}

	return frequency;

}

int MediatekRadio::getFrequency() {

	return frequency;

}

MediatekRadio::~MediatekRadio() {

	stopRadio();

}
