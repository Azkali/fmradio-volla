/*
 * Copyright (C) 2026 Azkali Manad <a.ffcc7@gmail.com>
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

#include <QDebug>
#include <QProcess>
#include <QProcessEnvironment>
#include <QStandardPaths>

#include "MediatekRadio.h"
#include "common.h"

#define FM_DEV_NAME "/dev/fm"

// The driver ignores the band, its range is fixed to 87.5 - 108 MHz
#define FM_BAND_UE      1

// Frequencies are in 10 kHz units
static const int kBandMin = 8750;
static const int kBandMax = 10800;
static const int kStep = 10;
static const int kSeekRssi = -75;
static const int kDefaultFrequency = 10250;

// Run pactl with a C locale so its output can be parsed
static QString pactl(const QStringList &args)
{
	QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
	env.insert("LC_ALL", "C");

	QProcess process;
	process.setProcessEnvironment(env);
	process.start("/usr/bin/pactl", args);
	process.waitForFinished();

	return QString::fromUtf8(process.readAllStandardOutput()).trimmed();
}

// Node names differ between pulseaudio-modules-droid releases:
//   Halium 9/10: source.droid / sink.droid
//   Halium 11+:  source.primary_input / sink.primary_output
static QString findNode(const QString &kind, const QStringList &candidates)
{
	QStringList names;
	for (const QString &line : pactl({"list", "short", kind}).split('\n'))
		names << line.section('\t', 1, 1);

	for (const QString &candidate : candidates)
		if (names.contains(candidate))
			return candidate;

	return candidates.first();
}

// Node blocks start with an unindented "Sink #N" or "Source #N" line
static QString activePort(const QString &kind, const QString &node)
{
	bool inNode = false;

	for (const QString &line : pactl({"list", kind}).split('\n')) {
		const QString field = line.trimmed();

		if (!line.startsWith('\t'))
			inNode = false;
		else if (field == "Name: " + node)
			inNode = true;
		else if (inNode && field.startsWith("Active Port: "))
			return field.section(' ', 2);
	}

	return QString();
}

// Wrap around at the band edges
static int wrapFrequency(int freq)
{
	if (freq > kBandMax)
		return kBandMin;
	if (freq < kBandMin)
		return kBandMax;

	return freq;
}

MediatekRadio::MediatekRadio(QObject *parent)
	: QObject(parent),
	  settings(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation) + "/radio.conf",
		   QSettings::IniFormat),
	  paSource(findNode("sources", {"source.primary_input", "source.droid"})),
	  paSink(findNode("sinks", {"sink.primary_output", "sink.droid"})),
	  frequency(settings.value("frequency", kDefaultFrequency).toInt())
{
}

MediatekRadio::~MediatekRadio()
{
	stopRadio();
}

// Wired headsets and headphones act as the antenna
bool MediatekRadio::isHeadsetConnected()
{
	const QString port = activePort("sinks", paSink);

	return port == "output-wired_headset" || port == "output-wired_headphone";
}

void MediatekRadio::routeAudio()
{
	savedSourcePort = activePort("sources", paSource);
	savedSinkPort = activePort("sinks", paSink);

	pactl({"set-source-port", paSource, "input-fm_tuner"});
	if (speakerOutput)
		pactl({"set-sink-port", paSink, "output-speaker"});

	// The multimedia role makes the loopback follow the system media volume
	loopbackModule = pactl({"load-module", "module-loopback",
				"source=" + paSource, "sink=" + paSink,
				"sink_input_properties=media.role=multimedia"});
}

void MediatekRadio::restoreAudio()
{
	if (!loopbackModule.isEmpty())
		pactl({"unload-module", loopbackModule});
	loopbackModule.clear();

	if (savedSourcePort.isEmpty() || savedSourcePort == "input-fm_tuner")
		savedSourcePort = savedSinkPort == "output-wired_headset" ? "input-wired_headset" : "input-builtin_mic";

	pactl({"set-source-port", paSource, savedSourcePort});
	if (!savedSinkPort.isEmpty())
		pactl({"set-sink-port", paSink, savedSinkPort});
}

QByteArray MediatekRadio::startRadio(int freq)
{
	if (running)
		return "Stop radio";

	if (!isHeadsetConnected())
		return "Headset not available";

	int ret = COM_open_dev(FM_DEV_NAME, &fd);
	if (ret < 0) {
		qWarning() << "Failed to open" << FM_DEV_NAME << ret;
		return "Error";
	}

	ret = COM_pwr_up(fd, FM_BAND_UE, freq);
	if (ret < 0) {
		qWarning() << "Failed to power up:" << ret;
		COM_close_dev(fd);
		fd = -1;
		return "Error";
	}

	setFrequency(freq);
	routeAudio();

	running = true;
	emit runningChanged();

	return "Stop radio";
}

QByteArray MediatekRadio::stopRadio()
{
	if (!running)
		return "Start radio";

	restoreAudio();

	int ret = COM_pwr_down(fd, 0);
	if (ret < 0)
		qWarning() << "Failed to power down:" << ret;

	COM_close_dev(fd);
	fd = -1;

	running = false;
	emit runningChanged();

	return "Start radio";
}

bool MediatekRadio::isRadioRunning() const
{
	return running;
}

void MediatekRadio::tune(int freq)
{
	if (running && COM_tune(fd, freq, FM_BAND_UE) < 0) {
		qWarning() << "Failed to tune to" << freq;
		return;
	}

	setFrequency(freq);
}

int MediatekRadio::getFrequency() const
{
	return frequency;
}

void MediatekRadio::setFrequency(int freq)
{
	if (frequency == freq)
		return;

	frequency = freq;
	settings.setValue("frequency", freq);
	emit frequencyChanged();
}

int MediatekRadio::seekUp()
{
	return seek(true);
}

int MediatekRadio::seekDown()
{
	return seek(false);
}

int MediatekRadio::stepUp()
{
	return step(true);
}

int MediatekRadio::stepDown()
{
	return step(false);
}

// Move by 0.1 MHz
int MediatekRadio::step(bool up)
{
	tune(wrapFrequency(frequency + (up ? kStep : -kStep)));

	return frequency;
}

int MediatekRadio::seek(bool up)
{
	if (!running)
		return frequency;

	setMute(true);

	const int delta = up ? kStep : -kStep;
	int found = frequency;

	for (int freq = frequency + delta; freq >= kBandMin && freq <= kBandMax; freq += delta) {
		if (COM_tune(fd, freq, FM_BAND_UE) == 0 && getRssi() > kSeekRssi) {
			found = freq;
			break;
		}
	}

	tune(found);
	setMute(false);

	return frequency;
}

void MediatekRadio::setMute(bool mute)
{
	int ret = COM_set_mute(fd, mute);
	if (ret < 0)
		qWarning() << "Failed to set mute:" << ret;
}

int MediatekRadio::getRssi()
{
	int rssi;

	int ret = COM_get_rssi(fd, &rssi);
	if (ret < 0) {
		qWarning() << "Failed to get rssi:" << ret;
		return -100;
	}

	return rssi;
}

bool MediatekRadio::isSpeakerOutput() const
{
	return speakerOutput;
}

void MediatekRadio::setSpeakerOutput(bool speaker)
{
	if (speakerOutput == speaker)
		return;

	speakerOutput = speaker;
	if (running)
		pactl({"set-sink-port", paSink, speaker ? "output-speaker" : savedSinkPort});

	emit speakerOutputChanged();
}
