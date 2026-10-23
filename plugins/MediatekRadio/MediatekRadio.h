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

#ifndef MEDIATEKRADIO_H
#define MEDIATEKRADIO_H

#include <QObject>
#include <QSettings>
#include <QString>

class MediatekRadio : public QObject {
	Q_OBJECT
	Q_PROPERTY(bool running READ isRadioRunning NOTIFY runningChanged)
	Q_PROPERTY(int frequency READ getFrequency NOTIFY frequencyChanged)
	Q_PROPERTY(bool speakerOutput READ isSpeakerOutput WRITE setSpeakerOutput NOTIFY speakerOutputChanged)
	Q_PROPERTY(bool headsetAntenna READ isHeadsetAntenna WRITE setHeadsetAntenna NOTIFY headsetAntennaChanged)
	Q_PROPERTY(bool mono READ isMono WRITE setMono NOTIFY monoChanged)
	Q_PROPERTY(Region region READ getRegion WRITE setRegion NOTIFY regionChanged)
	Q_PROPERTY(QString devicePath READ getDevicePath CONSTANT)
	Q_PROPERTY(QString chipName READ getChipName NOTIFY runningChanged)

public:
	// Both use 87.5 - 108 MHz, they only differ in de-emphasis
	enum Region {
		RegionWorld,
		RegionAmericas
	};
	Q_ENUM(Region)

	explicit MediatekRadio(QObject *parent = nullptr);
	~MediatekRadio();

	Q_INVOKABLE QByteArray startRadio(int freq);
	Q_INVOKABLE QByteArray stopRadio();
	Q_INVOKABLE void tune(int freq);
	Q_INVOKABLE int getFrequency() const;
	Q_INVOKABLE int seekUp();
	Q_INVOKABLE int seekDown();
	Q_INVOKABLE int stepUp();
	Q_INVOKABLE int stepDown();
	Q_INVOKABLE bool isRadioRunning() const;
	Q_INVOKABLE bool isSpeakerOutput() const;
	Q_INVOKABLE void setSpeakerOutput(bool speaker);

	bool isHeadsetAntenna() const;
	void setHeadsetAntenna(bool headset);
	bool isMono() const;
	void setMono(bool enabled);
	Region getRegion() const;
	void setRegion(Region newRegion);
	QString getDevicePath() const;
	QString getChipName() const;

signals:
	void runningChanged();
	void frequencyChanged();
	void speakerOutputChanged();
	void headsetAntennaChanged();
	void monoChanged();
	void regionChanged();

private:
	bool powerUp(int freq);
	int seek(bool up);
	int step(bool up);
	void setFrequency(int freq);
	void setMute(bool mute);
	void applyAntenna();
	void applyMono();
	bool isHeadsetConnected();
	void routeAudio();
	void restoreAudio();

	QSettings settings;
	QString paSource;
	QString paSink;
	QString savedSourcePort;
	QString savedSinkPort;
	QString loopbackModule;
	QString chipName;
	bool running = false;
	bool speakerOutput = false;
	bool headsetAntenna;
	bool mono;
	Region region;
	int frequency;
	int fd = -1;
};

#endif
