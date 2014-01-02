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

#ifndef TPATHHELPER_H
#define TPATHHELPER_H

#include "tglobal.h"

#include <QPainterPath>
#include <QChar>
#include <QStringList>

/**
 * @author David Cuadrado
*/

class T_CORE_EXPORT TPathHelper
{
    public:
        /**
         * @if english
         * Default constructor
         * @elseif spanish
         * Constructor por defecto
         * @endif
         * @return 
         */
        TPathHelper();
        
        /**
         * Destructor
         * @return 
         */
        ~TPathHelper();
        
        /**
         * @if english
         * Adjusts a QPainterPath to rect
         * @elseif spanish
         * Ajusta un QPainterPath a un rectangulo
         * @endif
         * @param p 
         * @param rect 
         * @param offset 
         * @return 
         */
        static QPainterPath toRect(const QPainterPath &p, const QRect &rect, float offset = 10.0f);
        
        /**
         * @if english
         * Adjust a QPainterPath list to rect
         * @elseif spanish
         * Ajusta una lista de QPainterPath a un rectangulo
         * @endif
         * @param l 
         * @param rect 
         * @param offset 
         * @return 
         */
        static QList<QPainterPath> toRect(const QList<QPainterPath> &l, const QRect &rect, float offset = 10.0f);
        
        /**
         * @if english
         * Builds a QPainterPath from a QStringList
         * @elseif spanish
         * Construye un QPainterPath desde un QStringList
         * @endif
         * @param polygonsStr 
         * @param sep 
         * @return 
         */
        static QPainterPath buildPath(const QStringList &polygonsStr, QChar sep);
        static QPainterPath buildPath(const QString &svgpath);
        
        static QPainterPath fromElements(const QList<QPainterPath::Element>& elements);
        static QList<QPainterPath> toSubpaths(const QPainterPath & path);
};

#endif
