/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez                                                     *
 *                                                                         *
 *   KTooN's versions:                                                     *
 *                                                                         *
 *   2006:                                                                 *
 *    David Cuadrado                                                       *
 *    Jorge Cuadrado                                                       *
 *   2003:                                                                 *
 *    Fernado Roldan                                                       *
 *    Simena Dinas                                                         *
 *                                                                         *
 *   Copyright (C) 2010 Gustav Gonzalez - http://www.maefloresta.com       *
 *   License:                                                              *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 ***************************************************************************/

#ifndef TUPLIPSYNC_H
#define TUPLIPSYNC_H

#include "tupabstractserializable.h"

#include <QDomDocument>
#include <QDomElement>
#include <QStringList>
#include <QPoint>
#include <QTextStream>

class TUPI_EXPORT TupPhoneme : public QObject, public TupAbstractSerializable
{
    public:
        TupPhoneme();
        TupPhoneme(const QString &value, int duration);
        ~TupPhoneme();

        void setDuration(int duration);
        int duration();
        void setValue(const QString &value);
        QString value() const;

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        int frames;
        QString phoneme;
};

class TUPI_EXPORT TupPhrase : public QObject, public TupAbstractSerializable
{
    public:
        TupPhrase(int index);
        ~TupPhrase();
        void setInitFrame(int index);
        int initFrame();
        void addPhoneme(TupPhoneme *phoneme);

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        int frameIndex; 
        QList<TupPhoneme *> phonemes;
};

class TUPI_EXPORT TupVoice : public QObject, public TupAbstractSerializable
{
    public:
        TupVoice();
        TupVoice(const QString &label, QPoint pos);
        ~TupVoice();
        void setVoiceTitle(const QString &label);
        QString voiceTitle() const;
        void setMouthPos(QPoint pos);
        QPoint mouthPos();
        void setText(const QString &content);
        QString text() const;
        void addPhrase(TupPhrase *phrase);

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        QString title;
        QPoint point;
        QString script;
        QList<TupPhrase *> phrases;
};

class TUPI_EXPORT TupLipSync : public QObject, public TupAbstractSerializable
{
    public:
        TupLipSync();
        TupLipSync(const QString &name, const QString &soundFile);
        ~TupLipSync();
        void addPhrase(TupPhrase *phrase);
        QString name() const;
        void setName(const QString &title);
        QString soundFile() const;
        void setSoundFile(const QString &file);
        int framesTotal();
        void setFramesTotal(int framesTotal);
        void addVoice(TupVoice *voice);

        virtual void fromXml(const QString &xml);
        virtual QDomElement toXml(QDomDocument &doc) const;

    private:
        struct Private;
        Private *const k;
};

#endif
