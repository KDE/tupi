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

#include "tuplipsync.h" 

TupPhoneme::TupPhoneme() : QObject()
{
}

TupPhoneme::TupPhoneme(const QString &value, int duration) : QObject()
{
    phoneme = value;
    frames = duration;
}

TupPhoneme::~TupPhoneme()
{
}

void TupPhoneme::setDuration(int duration)
{
    frames = duration;
}

int TupPhoneme::duration()
{
    return frames;
}

void TupPhoneme::setValue(const QString &value)
{
    phoneme = value;
}

QString TupPhoneme::value() const
{
    return phoneme;
}

void TupPhoneme::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement e = document.documentElement();
        if (!e.isNull()) {
            if (e.tagName() == "phoneme") {
                phoneme = e.attribute("value");
                frames = e.attribute("duration").toInt();
            }
        }
    }
}

QDomElement TupPhoneme::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("phoneme");
    root.setAttribute("value", phoneme);
    root.setAttribute("duration", frames);

    return root;
}

TupPhrase::TupPhrase(int index) : QObject()
{
    frameIndex = index;
}

TupPhrase::~TupPhrase()
{
}

void TupPhrase::setInitFrame(int index)
{
    frameIndex = index;
}

int TupPhrase::initFrame()
{
    return frameIndex;
}

void TupPhrase::addPhoneme(TupPhoneme *phoneme)
{
    if (phoneme)
        phonemes << phoneme;
}

void TupPhrase::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement root = document.documentElement();
        frameIndex = root.attribute("initFrame").toInt();
        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "phoneme") {
                       QString value = e.attribute("value");
                       int duration = e.attribute("duration").toInt();

                       TupPhoneme *phoneme = new TupPhoneme(value, duration);
                       phonemes << phoneme;
                   }
               }

               n = n.nextSibling();
        }
    }
}

QDomElement TupPhrase::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("phrase");
    root.setAttribute("initFrame", frameIndex);

    int total = phonemes.size();
    for(int i=0; i<total; i++) {
        TupPhoneme *phoneme = phonemes.at(i);
        root.appendChild(phoneme->toXml(doc));
    }

    return root;
}

TupVoice::TupVoice()
{
}

TupVoice::TupVoice(const QString &label, QPoint pos) 
{
    title = label;
    point = pos;
}

TupVoice::~TupVoice()
{
}

void TupVoice::setVoiceTitle(const QString &label)
{
    title = label;
}

QString TupVoice::voiceTitle() const
{
    return title;
}

void TupVoice::setMouthPos(QPoint pos)
{
    point = pos;
}

QPoint TupVoice::mouthPos()
{
    return point;
}

void TupVoice::setText(const QString &content)
{
    script = content;
}

QString TupVoice::text() const
{
    return script;
}

void TupVoice::addPhrase(TupPhrase *phrase)
{
    if (phrase)
        phrases << phrase;
}

void TupVoice::fromXml(const QString &xml)
{
    QDomDocument document;

    if (document.setContent(xml)) {
        QDomElement root = document.documentElement();
        QDomNode n = root.firstChild();

        while (!n.isNull()) {
               QDomElement e = n.toElement();
               if (!e.isNull()) {
                   if (e.tagName() == "phrase") {
                       int initFrame = e.attribute("initFrame").toInt();
                       TupPhrase *phrase = new TupPhrase(initFrame);
                       QString newDoc;
                       {
                           QTextStream ts(&newDoc);
                           ts << n;
                       }

                       phrase->fromXml(newDoc);
                       phrases << phrase;
                   }
               }
               n = n.nextSibling();
        }
    }
}

QDomElement TupVoice::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("voice");
    root.setAttribute("name", title);
    root.setAttribute("origin", QString::number(point.x()) + "," + QString::number(point.y()));

    int total = phrases.size();
    for(int i=0; i<total; i++) {
        TupPhrase *phrase = phrases.at(i);
        root.appendChild(phrase->toXml(doc));
    }

    return root;
}

struct TupLipSync::Private
{
    QString name;
    QString soundFile;
    int framesTotal;
    QList<TupVoice *> voices;
};

TupLipSync::TupLipSync() : QObject(), k(new Private)
{
}

TupLipSync::TupLipSync(const QString &name, const QString &soundFile) : QObject(), k(new Private)
{
    k->name = name;
    k->soundFile = soundFile;
}

TupLipSync::~TupLipSync()
{
}

QString TupLipSync::name() const
{
    return k->name;
}

void TupLipSync::setName(const QString &title)
{
    k->name = title;
}

QString TupLipSync::soundFile() const
{
    return k->soundFile;
}

void TupLipSync::setSoundFile(const QString &file)
{
    k->soundFile = file;
}

int TupLipSync::framesTotal()
{
    return k->framesTotal;
}

void TupLipSync::setFramesTotal(int framesTotal)
{
    k->framesTotal = framesTotal;
}

void TupLipSync::addVoice(TupVoice *voice)
{
    if (voice)
        k->voices << voice;
}

void TupLipSync::fromXml(const QString &xml)
{
    QDomDocument document;
   
    if (! document.setContent(xml)) {
        #ifdef K_DEBUG
            QString msg = "TupLipSync::fromXml() - File corrupted!";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
                qWarning() << "Content:";
                qWarning() << xml;
            #else
                tError() << msg;
                tWarning() << "Content:";
                tWarning() << xml;
            #endif
        #endif

        return;
    }

    QDomElement root = document.documentElement();
    k->name = root.attribute("name");
    k->soundFile = root.attribute("soundFile");
    k->framesTotal = root.attribute("framesTotal").toInt();

    QDomNode n = root.firstChild();

    while (!n.isNull()) {
           QDomElement e = n.toElement();
           if (!e.isNull()) {
               if (e.tagName() == "voice") {
                   QString name = e.attribute("name");
                   QStringList xy = e.attribute("origin").split(",");
                   QPoint point = QPoint(xy.at(0).toInt(), xy.at(1).toInt());
                   TupVoice *voice = new TupVoice(name, point);

                   QString newDoc;
                   {
                       QTextStream ts(&newDoc);
                       ts << n;
                   }

                   voice->fromXml(newDoc);
                   k->voices << voice;
               }
           }
           n = n.nextSibling();
    }
}

QDomElement TupLipSync::toXml(QDomDocument &doc) const
{
    QDomElement root = doc.createElement("lipsync");
    root.setAttribute("name", k->name);
    root.setAttribute("soundFile", k->soundFile);
    root.setAttribute("framesTotal", k->framesTotal);

    int total = k->voices.size();
    for(int i=0; i<total; i++) {
        TupVoice *voice = k->voices.at(i);
        root.appendChild(voice->toXml(doc));
    }

    return root;
}
