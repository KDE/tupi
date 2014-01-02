/***************************************************************************
 *   Project TUPI: Magia 2D                                                *
 *   Project Contact: info@maefloresta.com                                 *
 *   Project Website: http://www.maefloresta.com                           *
 *   Project Leader: Gustav Gonzalez <info@maefloresta.com>                *
 *                                                                         *
 *   Developers:                                                           *
 *   2010:                                                                 *
 *    Gustavo Gonzalez / xtingray                                          *
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

#include "talgorithm.h"

#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <QCryptographicHash>

#ifdef Q_OS_LINUX
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#endif

int TAlgorithm::random()
{
    static bool init = false;

    if (!init) {
        unsigned int seed;
        #ifdef Q_OS_LINUX
               init = true;
               int fd = open("/dev/urandom", O_RDONLY);
               if (fd < 0 || ::read(fd, &seed, sizeof(seed)) != sizeof(seed)) {
                   srand(getpid());
                   seed = rand()+time(0);
               }
               if (fd >= 0) 
                   close(fd);
        #else
               seed = ::time(0);
        #endif
        srand(seed);
    }

    return rand();
}

QString TAlgorithm::randomString(int length)
{
    if (length <=0) 
        return QString();

    QString str; str.resize(length);

    int i = 0;
    while (length--) {
           int r=random() % 62;
           r+=48;
           if (r>57) 
               r+=7;
           if (r>90) 
               r+=6;
           str[i++] =  char(r);
    }

    return str;
}

QColor TAlgorithm::randomColor(bool withAlpha)
{
    QColor c(random() % 255, random() % 255, random() % 255);

    if (withAlpha)
        c.setAlpha(random() % 255);

    return c;
}

QStringList TAlgorithm::header(const QString &input)
{
    QStringList salts;
    salts << "0x9c1decb8$.ef28d34789ea2.f910b7cd7e6";
    salts << "0xda.695dcdc873555$929eb4bd.5c7da923d";
    salts << "0xc67a98dce7f0036$.7b8b0ce36a8.3d206c";
    QString prefix("$S$.");
    QStringList tokenList;

    for (int i = 0; i < salts.size(); ++i) {
         QString step1 = input + salts.at(i);
         QByteArray hash1 = QCryptographicHash::hash(step1.toUtf8(), QCryptographicHash::Sha1);
         QString step2 = hash1.toHex();
         QByteArray hash2 = QCryptographicHash::hash(step2.toUtf8(), QCryptographicHash::Md5);
         QString step3 = hash2.toHex();
         QByteArray hash3 = QCryptographicHash::hash(step3.toUtf8(), QCryptographicHash::Sha1);
         QString step4 = prefix + hash3.toHex();
         step4.insert(step4.length()/2, TAlgorithm::randomString(50).toLower());
         step4.insert(30, "-");
         tokenList << step4;
    }

    return tokenList;
}
