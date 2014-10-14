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
    int currentLayer;
    QList<TimeLineLayerItem> layers;
};

TupLayerHeader::TupLayerHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent), k(new Private)
{
    setFixedWidth(115);

    // SQA: This code will be disabled until the "Lock layer" feature is implemented
    // setFixedWidth(140);
    // k->lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");

    k->viewIcon = QPixmap(THEME_DIR + "icons/show_hide_all_layers.png");
}

TupLayerHeader::~TupLayerHeader()
{
}

void TupLayerHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
{
    Q_UNUSED(logicalIndex);

    if (!model() || !rect.isValid())
        return;

    painter->save();

    QStyleOptionHeader headerOption;
    headerOption.rect = rect;
    headerOption.orientation = Qt::Vertical;
    headerOption.position = QStyleOptionHeader::Middle;
    headerOption.text = "";

    style()->drawControl(QStyle::CE_HeaderSection, &headerOption, painter);

    if (k->currentLayer == logicalIndex) {
        QColor color(0, 136, 0, 40);
        painter->fillRect(rect, color);
    }

    QFont font("Arial", 7, QFont::Normal, false);
    QFontMetrics fm(font);

    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);
    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(10, y, k->layers[logicalIndex].title);

    y = rect.y();

    // SQA: This code will be disabled until the "Lock layer" feature is implemented
    /* 
    QRectF lockRect = QRectF(0, 0, 11, 12);
    int lockY = (rect.height() - lockRect.height())/2;
    painter->drawPixmap(QPointF(rect.x() + 90, lockY + y), k->lockIcon, lockRect);
    */

    QRectF viewRect = QRectF(0, 0, 13, 7); 
    int viewY = (rect.height() - viewRect.height())/2;
    painter->drawPixmap(QPointF(rect.x() + 90, viewY + y), k->viewIcon, viewRect);

    painter->restore();
}

void TupLayerHeader::mousePressEvent(QMouseEvent *event)
{
    emit logicalSectionSelected(logicalIndexAt(event->pos()));
}

void TupLayerHeader::updateSelection(int layerIndex)
{
    k->currentLayer = layerIndex;
    updateSection(layerIndex);
}

void TupLayerHeader::insertLayer(int index, const QString &name)
{
    TimeLineLayerItem layer;
    layer.title = name;
    layer.lastFrame = 0;
    layer.isVisible = true;
    layer.isLocked = false;

    k->layers.insert(index, layer);
}

