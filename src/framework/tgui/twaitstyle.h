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

#ifndef TUPISTYLE_H
#define TUPISTYLE_H

#include "tglobal.h"

#include <QPlastiqueStyle>

/**
 * @if english
 * Tupi style
 * @elseif spanish
 * Estilo Tupi 
 * @endif
 */

class T_GUI_EXPORT TWaitStyle : public QPlastiqueStyle
{
    public:
        TWaitStyle();

        void drawHoverRect(QPainter *painter, const QRect &rect, const QBrush &brush) const;

        void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                           QPainter *painter, const QWidget *widget = 0) const;
        void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget) const;
        void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                QPainter *painter, const QWidget *widget) const;
        QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                               const QSize &size, const QWidget *widget) const;

        QRect subElementRect(SubElement element, const QStyleOption *option, const QWidget *widget) const;
        QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                             SubControl sc, const QWidget *widget) const;

        int pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const;

        void polish(QPalette &palette);
        void polish(QWidget *widget);
        void unpolish(QWidget *widget);
};

#endif
