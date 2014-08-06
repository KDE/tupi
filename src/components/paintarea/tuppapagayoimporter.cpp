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

#include "tuppapagayoimporter.h" 

struct TupPapagayoImporter::Private
{
    bool isValid;
    int framesTotal;
    TupLipSync *lipsync;
};

TupPapagayoImporter::TupPapagayoImporter(const QString &file) : QObject(), k(new Private)
{
    k->framesTotal = 0;
    k->isValid = true;
    QFile input(file);

    QFileInfo info(file);
    QString name = info.fileName().toLower();
    k->lipsync = new TupLipSync();
    k->lipsync->setName(name);

    if (input.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&input);
        int i = 0;
        int voicesNumber = 0;
        QString line;
        while (i < 5) {
               line = stream.readLine(); 
               switch (i) {
                   case 0:
                   {
                       if (!line.startsWith("lipsync version")) {
                           k->isValid = false;      
                           return;
                       }
                   } 
                   break;
                   case 1:
                   {
                       // Load audio file
                       k->lipsync->setSoundFile(line.trimmed());
                   }
                   break;
                   case 4:
                   {
                       // Total of voices 
                       voicesNumber = line.toInt();
                   }
                   break;
               }
               i++;
        }

        for(int j=0; j<voicesNumber; j++) {
            TupVoice *voice = new TupVoice(); 
            voice->setVoiceTitle(stream.readLine().trimmed());
            voice->setText(stream.readLine().trimmed());
            int numPhrases = stream.readLine().toInt();
            int numPhonemes = 0;
            int numWords;
            QString str;
            for (int p = 0; p < numPhrases; p++) {
                 QString text = stream.readLine().trimmed();
                 int initFrame = stream.readLine().toInt();
                 int endFrame = stream.readLine().toInt();
                 TupPhrase *phrase = new TupPhrase(initFrame);
                 numWords = stream.readLine().toInt();
                 for (int w = 0; w < numWords; w++) {
                      QString str = stream.readLine().trimmed();
                      QStringList strList = str.split(' ', QString::SkipEmptyParts);
                      QString word; 
                      int firstFrame = 0;
                      int lastFrame = 0;
                      if (strList.size() >= 4) {
                          word = strList.at(0);   
                          firstFrame = strList.at(1).toInt();
                          lastFrame = strList.at(2).toInt();
                          numPhonemes = strList.at(3).toInt();
                      }
                      QList<int> frames;
                      QList<QString> blocks;
                      for (int ph = 0; ph < numPhonemes; ph++) {
                           str = stream.readLine().trimmed();
                           QStringList strList = str.split(' ', QString::SkipEmptyParts);
                           if (strList.size() >= 2) {
                               frames << strList.at(0).toInt();
                               blocks << strList.at(1);
                           }
                      } // for ph

                      for (int ph = 0; ph < numPhonemes-1; ph++) {
                           int total = frames.at(ph+1) - frames.at(ph);
                           TupPhoneme *phoneme = new TupPhoneme(blocks.at(ph), total);
                           phrase->addPhoneme(phoneme);
                      } // for ph

                      int total = (lastFrame - frames.at(numPhonemes-1)) + 1;
                      TupPhoneme *phoneme = new TupPhoneme(blocks.at(numPhonemes-1), total);
                      phrase->addPhoneme(phoneme);

                      if (w == numWords - 1) {
                          if (lastFrame > k->framesTotal)
                              k->framesTotal = lastFrame;
                      }
                 } // for w
                 voice->addPhrase(phrase); 
            }
            k->lipsync->addVoice(voice);
        }
        k->framesTotal++;
        k->lipsync->setFramesTotal(k->framesTotal);
    } else {
        k->isValid = false;
        #ifdef K_DEBUG
            QString msg = "TupPapagayoImporter() - Fatal Error: Insufficient permissions to load file! -> " + file;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
        return;
    }

    input.close();
}

TupPapagayoImporter::~TupPapagayoImporter()
{
}

bool TupPapagayoImporter::fileIsValid()
{
    return k->isValid;
}

QString TupPapagayoImporter::file2Text() const
{
    QDomDocument document;
    QDomElement root = k->lipsync->toXml(document);

    QString xml;
    {
      QTextStream ts(&xml);
      ts << root;
    }

    return xml;
}

int TupPapagayoImporter::framesTotal()
{
    return k->framesTotal;
}

