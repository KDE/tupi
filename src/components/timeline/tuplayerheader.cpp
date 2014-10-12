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

#include "tuplayerheader.h"

struct TupLayerHeader::Private
{
    QPixmap lockIcon;
    QPixmap viewIcon;
};

TupLayerHeader::TupLayerHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent), k(new Private)
{
    setFixedWidth(200);
    k->lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");
    k->viewIcon = QPixmap(THEME_DIR + "icons/show_hide_all_layers.png");
}

TupLayerHeader::~TupLayerHeader()
{
}

void TupLayerHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    Q_UNUSED(logicalIndex);

    if (!rect.isValid())
        return;

    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Vertical;
    headerOption.position = QStyleOptionHeader::Middle;
    headerOption.text = "";

    QStyle::State state = QStyle::State_None;

    if (isEnabled())
        state |= QStyle::State_Enabled;

    if (window()->isActiveWindow())
        state |= QStyle::State_Active;

    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);

    QString text;
    text = text.setNum(logicalIndex + 1);
    QFont font("Arial", 7, QFont::Normal, false);
    QFontMetrics fm(font);

    int x = rect.normalized().x() + ((rect.normalized().width() - fm.width(text))/2);
    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(5, y, text);

    painter->drawPixmap(QPointF(rect.x() + (rect.width()-10)/2, rect.y() + 6), k->lockIcon, QRectF(0, 0, 10, 13));
    painter->drawPixmap(QPointF(rect.x() + (rect.width()-10)/2 + 30, rect.y() + 5), k->viewIcon, QRectF(0, 0, 16, 16));
}

void TupLayerHeader::mousePressEvent(QMouseEvent *event)
{
    emit logicalSectionSelected(logicalIndexAt(event->pos()));
}
