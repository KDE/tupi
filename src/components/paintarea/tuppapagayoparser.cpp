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

#include "tuppapagayoparser.h" 

LipsyncPhoneme::LipsyncPhoneme()
{
    fText = "";
    fFrame = 0;
    fTop = fBottom = 0;
}

LipsyncPhoneme::~LipsyncPhoneme()
{
}

LipsyncWord::LipsyncWord()
{
    fText = "";
    fStartFrame = 0;
    fEndFrame = 0;
    fTop = fBottom = 0;
}

LipsyncWord::~LipsyncWord()
{
    while (!fPhonemes.isEmpty())
           delete fPhonemes.takeFirst();
}

LipsyncPhrase::LipsyncPhrase()
{
    fText = "";
    fStartFrame = 0;
    fEndFrame = 0;
    fTop = fBottom = 0;
}

LipsyncPhrase::~LipsyncPhrase()
{
    while (!fWords.isEmpty())
           delete fWords.takeFirst();
}

struct TupPapagayoParser::Private
{
    bool isValid;
    QList<LipsyncPhrase *> fPhrases;
};

TupPapagayoParser::TupPapagayoParser(const QString &file) : QObject(), k(new Private)
{
    k->isValid = true;
    QFile input(file);

    if (input.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream stream(&input);
        int i = 0;
        int voicesNumber = 0;
        QString soundFile = "";
        QString line;
        while (i < 5) {
               line = stream.readLine(); 
               switch (i) {
                   case 0:
                   {
                       tError() << "TupPapagayoParser() - Line: " << line;
                       if (!line.startsWith("lipsync version")) {
                           k->isValid = false;      
                           return;
                       }
                   } 
                   break;
                   case 1:
                   {
                       // Load audio file
                       soundFile = line;
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
            QString voiceName = stream.readLine().trimmed();
            QString text = stream.readLine().trimmed();
            int numPhrases = stream.readLine().toInt();
            int numPhonemes;
            int numWords;
            QString str;
            for (int p = 0; p < numPhrases; p++) {
                LipsyncPhrase *phrase = new LipsyncPhrase;
                phrase->fText = stream.readLine().trimmed();
                phrase->fStartFrame = stream.readLine().toInt();
                phrase->fEndFrame = stream.readLine().toInt();
                numWords = stream.readLine().toInt();
                for (int w = 0; w < numWords; w++)
                {
                        LipsyncWord *word = new LipsyncWord;
                        str = stream.readLine().trimmed();
                        QStringList strList = str.split(' ', QString::SkipEmptyParts);
                        if (strList.size() >= 4)
                        {
                                word->fText = strList.at(0);
                                word->fStartFrame = strList.at(1).toInt();
                                word->fEndFrame = strList.at(2).toInt();
                                numPhonemes = strList.at(3).toInt();
                        }
                        for (int ph = 0; ph < numPhonemes; ph++)
                        {
                                LipsyncPhoneme *phoneme = new LipsyncPhoneme;
                                str = stream.readLine().trimmed();
                                QStringList strList = str.split(' ', QString::SkipEmptyParts);
                                if (strList.size() >= 2)
                                {
                                        phoneme->fFrame = strList.at(0).toInt();
                                        phoneme->fText = strList.at(1);
                                }
                                word->fPhonemes << phoneme;
                        } // for ph
                        phrase->fWords << word;
                } // for w
                k->fPhrases << phrase;
            }
        }
    } else {
        k->isValid = false;
        #ifdef K_DEBUG
            QString msg = "TupPapagayoParser() - Fatal Error: Insufficient permissions to load file! -> " + file;
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

TupPapagayoParser::~TupPapagayoParser()
{
}

bool TupPapagayoParser::fileIsValid()
{
    return k->isValid;
}
