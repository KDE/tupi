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

#include "tupwebhunter.h"

QString TupWebHunter::BROWSER_FINGERPRINT = QString("Tupi_Browser 1.0");

struct TupWebHunter::Private
{
    DataType type;
    QString url;
    QString currency;
};

TupWebHunter::TupWebHunter(DataType type, const QString &url, QList<QString> params) : k(new Private)
{
    k->type = type;
    k->url = url;

    if (k->type == Currency) {
        QString money1 = params.at(0);
        QString money2 = params.at(1); 
        k->url.replace("1", money1); 
        k->url.replace("2", money2);
        k->currency = money2;
    }
}

void TupWebHunter::start()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(closeRequest(QNetworkReply*)));

    QNetworkRequest request;
    request.setUrl(QUrl(k->url));
    // request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toAscii());
    request.setRawHeader("User-Agent", BROWSER_FINGERPRINT.toLatin1());

    QNetworkReply *reply = manager->get(request);
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(slotError(QNetworkReply::NetworkError)));
}

TupWebHunter::~TupWebHunter()
{
}

void TupWebHunter::closeRequest(QNetworkReply *reply)
{
    QByteArray array = reply->readAll();
    QString answer(array);

    if (k->type == Currency) {
        answer = answer.mid(answer.indexOf("\n"), answer.length()).trimmed();

        QDomDocument doc;
        if (doc.setContent(answer)) {
            QDomElement root = doc.documentElement();
            if (!root.text().isNull())
                emit dataReady(k->currency + ":" + root.text());
        }
    }
}

void TupWebHunter::slotError(QNetworkReply::NetworkError error)
{
    if (k->type == Currency)
        emit dataReady(k->currency + ":UNAVAILABLE");
    else
        emit dataReady(tr("Information Temporarily Unavailable"));

    switch (error) {
            case QNetworkReply::HostNotFoundError:
                 { 
                 #ifdef K_DEBUG
                     QString msg = "TupWebHunter::slotError() - Network Error: Host not found";
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::TimeoutError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupWebHunter::slotError() - Network Error: Time out!";
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::ConnectionRefusedError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupWebHunter::slotError() - Network Error: Connection Refused!";
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::ContentNotFoundError:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupWebHunter::slotError() - Network Error: Content not found!";
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
                 }
            break;
            case QNetworkReply::UnknownNetworkError:
            default:
                 {
                 #ifdef K_DEBUG
                     QString msg = "TupWebHunter::slotError() - Network Error: Unknown Network error!";
                     #ifdef Q_OS_WIN32
                         qDebug() << msg;
                     #else
                         tError() << msg;
                     #endif
                 #endif
                 }
            break;
    }
}
