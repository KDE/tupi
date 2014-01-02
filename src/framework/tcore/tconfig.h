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

#ifndef TCONFIG_H
#define TCONFIG_H

#include <QObject>
#include <QDir>
#include <QHash>
#include <QDomDocument>
#include <QVariant>

#include "tglobal.h"

class TConfig;

/**
 * @author David Cuadrado
 * this is a dom config handler
*/

class T_CORE_EXPORT TConfig : public QObject
{
    Q_OBJECT

    public:
        ~TConfig();

    protected:
        explicit TConfig();
        void init();

    public:
        void beginGroup(const QString & prefix);
        void endGroup();

        void setValue(const QString & key, const QVariant & value);

        QVariant value(const QString & key, const QVariant & defaultValue = QVariant()) const;

        static TConfig *instance();

        bool firstTime();
        bool isOk();
        QDomDocument document();
        QString currentGroup();

        void sync();

    private:
        QDomElement find(const QDomElement &element, const QString &key) const;

    private:
        static TConfig *m_instance;

        class Private;
        Private * const k;
};

#define TCONFIG TConfig::instance()

#endif
