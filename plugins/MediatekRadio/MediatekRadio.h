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

public:
	explicit MediatekRadio(QObject *parent = nullptr);
	~MediatekRadio();

	Q_INVOKABLE QByteArray startRadio(int freq);
	Q_INVOKABLE QByteArray stopRadio();
	Q_INVOKABLE void tune(int freq);
	Q_INVOKABLE int getFrequency() const;
	Q_INVOKABLE int seekUp();
	Q_INVOKABLE int seekDown();
	Q_INVOKABLE bool isRadioRunning() const;
	Q_INVOKABLE bool isSpeakerOutput() const;
	Q_INVOKABLE void setSpeakerOutput(bool speaker);

signals:
	void runningChanged();
	void frequencyChanged();
	void speakerOutputChanged();

private:
	int seek(bool up);
	void setFrequency(int freq);
	void setMute(bool mute);
	int getRssi();
	bool isHeadsetConnected();
	void routeAudio();
	void restoreAudio();

	QSettings settings;
	QString paSource;
	QString paSink;
	QString savedSourcePort;
	QString savedSinkPort;
	QString loopbackModule;
	bool running = false;
	bool speakerOutput = false;
	int frequency;
	int fd = -1;
};

#endif
