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
    QPixmap viewIconOn;
    QPixmap viewIconOff;
    int currentLayer;
    QList<TimeLineLayerItem> layers;
    QLineEdit *editor;
    int editorSection;
};

TupLayerHeader::TupLayerHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent), k(new Private)
{
    setSectionsClickable(true);
    // setSectionsMovable(true);

    setFixedWidth(115);
    // SQA: This code will be disabled until the "Lock layer" feature is implemented
    // setFixedWidth(140);
    // k->lockIcon = QPixmap(THEME_DIR + "icons/padlock.png");
    k->viewIconOn = QPixmap(THEME_DIR + "icons/show_layer.png");
    k->viewIconOff = QPixmap(THEME_DIR + "icons/hide_layer.png");

    k->editor = new QLineEdit(this);
    k->editor->setFocusPolicy(Qt::ClickFocus);
    k->editor->setInputMask("");
    connect(k->editor, SIGNAL(editingFinished()), this, SLOT(hideTitleEditor()));
    k->editor->hide();

    connect(this, SIGNAL(sectionDoubleClicked(int)), this, SLOT(showTitleEditor(int)));
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
    if (k->layers[logicalIndex].isVisible)
        painter->drawPixmap(QPointF(rect.x() + 90, viewY + y), k->viewIconOn, viewRect);
    else
        painter->drawPixmap(QPointF(rect.x() + 90, viewY + y), k->viewIconOff, viewRect);

    painter->restore();
}

void TupLayerHeader::mousePressEvent(QMouseEvent *event)
{
    QPoint point = event->pos();
    emit selectionHasChanged(logicalIndexAt(point));

    int section = logicalIndexAt(point);
    int y = sectionViewportPosition(section);
    QRect rect(90, y, 20, sectionSize(section));

    if (rect.contains(point))
        emit visibilityHasChanged(section, !k->layers[section].isVisible);
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

void TupLayerHeader::setLayerVisibility(int layerIndex, bool visibility)
{
    k->layers[layerIndex].isVisible = visibility;
    updateSection(layerIndex);
}

void TupLayerHeader::setLayerName(int layerIndex, const QString &name)
{
    k->layers[layerIndex].title = name;
    updateSection(layerIndex);
}

void TupLayerHeader::showTitleEditor(int index)
{
    if (index >= 0) {
        QFont font("Arial", 8, QFont::Normal, false);
        k->editor->setFont(font);
        int x = sectionViewportPosition(index);
        k->editor->setGeometry(x, 0, width(), sectionSize(index));
        k->editorSection = index;
        k->editor->setText(k->layers[index].title);
        k->editor->show();
        k->editor->setFocus();
    }
}

void TupLayerHeader::hideTitleEditor()
{
    k->editor->hide();

    if (k->editorSection != -1 && k->editor->isModified())
        emit nameHasChanged(k->editorSection, k->editor->text());

    k->editorSection = -1;
}
