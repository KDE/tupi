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

#include "tupcommunicationparser.h"

struct TupCommunicationParser::Private
{
    QString message;
    QString login;
    int state;
};

TupCommunicationParser::TupCommunicationParser() : TupXmlParserBase(), k(new Private())
{
    
}

TupCommunicationParser::~TupCommunicationParser()
{
    
}

bool TupCommunicationParser::startTag(const QString &tag, const QXmlAttributes &atts)
{
    if (root() == "communication_chat" || root() == "communication_wall") {
        if (tag == "message") {
            k->message = atts.value("text");
            k->login = atts.value("from");
        }
    } else if (root() == "communication_notice") {
        if (tag == "notice") {
            k->login = atts.value("login");
            k->state = atts.value("state").toInt();
        }
    }
    
    return true;
}

bool TupCommunicationParser::endTag(const QString &)
{
    return true;
}

void TupCommunicationParser::text(const QString &)
{
}

QString TupCommunicationParser::message() const
{
    return k->message;
}

QString TupCommunicationParser::login() const
{
    return k->login;
}

int TupCommunicationParser::state()
{
    return k->state;
}

