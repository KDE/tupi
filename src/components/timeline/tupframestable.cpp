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

#include "tupframestable.h"

////////// TupFramesTableItemDelegate ///////////

class TupFramesTableItemDelegate : public QItemDelegate
{
    public:
        TupFramesTableItemDelegate(QObject * parent = 0);
        ~TupFramesTableItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TupFramesTableItemDelegate::TupFramesTableItemDelegate(QObject * parent) : QItemDelegate(parent) // QAbstractItemDelegate(parent)
{
}

TupFramesTableItemDelegate::~TupFramesTableItemDelegate()
{
}

void TupFramesTableItemDelegate::paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    Q_ASSERT(index.isValid());

    QItemDelegate::paint(painter, option, index);
    TupFramesTable *table = qobject_cast<TupFramesTable *>(index.model()->parent());
    TupFramesTableItem *item = dynamic_cast<TupFramesTableItem *>(table->itemFromIndex(index));
    
    // draw the background color
    QVariant value = index.data(Qt::BackgroundColorRole);
    
    if (value.isValid()) {
        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        if (!sound)
            painter->fillRect(option.rect, value.value<QColor>());
        
        painter->restore();
    } else {
        painter->save();
        
        bool sound = table->isSoundLayer(index.row());
        
        if (!sound) {
            int counter = index.column() + 1;
            if (counter == 1 || counter % 5 == 0) 
                painter->fillRect(option.rect, QColor(230, 230, 230));
            else 
                painter->fillRect(option.rect, Qt::white);
        } else {

        }
        
        painter->restore();
    }
    
    // Selection!
    if (option.showDecorationSelected && (option.state & QStyle::State_Selected)) {
        painter->save();
        painter->fillRect(option.rect, QColor(0, 136, 0, 180));
        painter->restore();
    }
    
    // Draw attributes
    int offset = option.rect.width() - 5;

    if (item && index.isValid()) {
        if (item->isUsed()) {
            painter->save();
            painter->setBrush(Qt::black);
            painter->setRenderHint(QPainter::Antialiasing, true);
            
            if (!item->isSound()) {
                if (item->isLocked()) {
                    painter->setPen(QPen(Qt::red, 1, Qt::SolidLine));
                    painter->setBrush(Qt::red);
                    // painter->drawEllipse(option.rect.left(), option.rect.bottom() - offset, offset, offset);
                } 
                painter->drawEllipse(option.rect.x() + ((option.rect.width() - offset)/2), 
                                     option.rect.y() + ((option.rect.height() + offset)/2), 
                                     offset, offset);
            } else {
                painter->setBrush(QColor(0, 136, 0));
                painter->drawRect(option.rect.x() + ((option.rect.width() - offset)/2), 
                                  option.rect.y() + ((option.rect.height() + offset)/2), 
                                  offset, offset);
            }
            
            painter->restore();
        }

/*
        if (item->isLocked()) {
            painter->save();
            painter->setBrush(Qt::red);
            // painter->drawEllipse(option.rect.left(), option.rect.bottom() - offset, offset, offset);
            painter->restore();
        }
*/
    }
}

////////// TupFramesTableItem ////////

TupFramesTableItem::TupFramesTableItem()
{
}

TupFramesTableItem::~TupFramesTableItem()
{
}

bool TupFramesTableItem::isUsed()
{
    return data(IsUsed).toBool();
}

bool TupFramesTableItem::isLocked()
{
    return data(IsLocked).toBool();
}

bool TupFramesTableItem::isSound()
{
    QVariant data = this->data(IsSound);
    
    if (data.canConvert<bool>())
        return data.toBool();

    return false;
}

//// TupFramesTable

struct TupFramesTable::Private
{
    struct LayerItem {
        LayerItem() : lastItem(-1), sound(false) {};
        int lastItem;
        bool sound;
    };
    
    int rectWidth;
    int rectHeight;

    int sceneIndex;
    int layerIndex;
    int frameIndex;

    QList<LayerItem> layers;
    TupTimeLineRuler *ruler;
    TupLayerHeader *layerColumn;
};

TupFramesTable::TupFramesTable(int sceneIndex, QWidget *parent) : QTableWidget(0, 200, parent), k(new Private)
{
    k->sceneIndex = sceneIndex;
    k->frameIndex = 0;
    k->layerIndex = 0;
    k->ruler = new TupTimeLineRuler;
    k->layerColumn = new TupLayerHeader;

    setup();
}

TupFramesTable::~TupFramesTable()
{
    delete k;
}

void TupFramesTable::setup()
{
    setItemDelegate(new TupFramesTableItemDelegate(this));
    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);
    
    setHorizontalHeader(k->ruler);

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(emitRequestSelectFrame(int, int, int, int)));
    connect(k->ruler, SIGNAL(logicalSectionSelected(int)), this, SLOT(emitFrameSelectionFromRuler(int)));
    connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, 
            SLOT(emitFrameSelected(QTableWidgetItem *, QTableWidgetItem *)));

    setVerticalHeader(k->layerColumn);

    connect(k->layerColumn, SIGNAL(logicalSectionSelected(int)), this, SLOT(emitFrameSelectionFromLayerHeader(int)));

    setItemSize(10, 25);
    
    horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    verticalHeader()->setSectionResizeMode(QHeaderView::Custom);
}

void TupFramesTable::emitFrameSelectionFromRuler(int frameIndex)
{
    emit emitSelection(0, frameIndex);
}

void TupFramesTable::emitFrameSelectionFromLayerHeader(int layerIndex)
{
    emit emitSelection(layerIndex, currentColumn());
}

void TupFramesTable::emitFrameSelected(QTableWidgetItem *current, QTableWidgetItem *prev)
{
    Q_UNUSED(prev);

    TupFramesTableItem *item = dynamic_cast<TupFramesTableItem *>(current);
    
    if (item) {
        if (item->isUsed()) {
            emit emitRequestChangeFrame(k->sceneIndex, verticalHeader()->visualIndex(this->row(item)), this->column(item));
        } else {
            #ifdef K_DEBUG
                QString msg = "TupFramesTable::emitFrameSelected <- item exists but is unset right now";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal() << msg;
                #endif
            #endif  
	    }
    } else { 
        emit frameRequest(TupProjectActionBar::InsertFrame, currentColumn(), currentRow(), k->sceneIndex);
    }
}

void TupFramesTable::setItemSize(int w, int h)
{
    k->rectHeight = h;
    k->rectWidth = w;
    
    fixSize();
}

bool TupFramesTable::isSoundLayer(int row)
{
    if (row < 0 && row >= k->layers.count())
        return false;
    
    return k->layers[row].sound;
}

void TupFramesTable::insertLayer(int index, const QString &name)
{
    tError() << "TupFramesTable::insertLayer() - Inserting layer at index: " << index;

    insertRow(index);
    
    Private::LayerItem layer;
    layer.sound = false;
    k->layers.insert(index, layer);

    k->layerColumn->insertLayer(index, name);
    
    fixSize();
}

void TupFramesTable::insertSoundLayer(int layerPos, const QString &name)
{
    Q_UNUSED(name);

    insertRow(layerPos);
    
    Private::LayerItem layer;
    layer.sound = true;
    k->layers.insert(layerPos, layer);
    
    fixSize();
}

void TupFramesTable::removeCurrentLayer()
{
    int pos = verticalHeader()->logicalIndex(currentRow());
    removeLayer(pos);
}

void TupFramesTable::removeLayer(int pos)
{
    pos = verticalHeader()->logicalIndex(pos);
    removeRow( pos );
    k->layers.removeAt(pos);
}

void TupFramesTable::moveLayer(int position, int newPosition)
{
    if (position < 0 || position >= rowCount() || newPosition < 0 || newPosition >= rowCount()) 
        return;
    
    blockSignals(true);
    verticalHeader()->moveSection(position, newPosition);
    blockSignals(false);
}

int TupFramesTable::currentLayer()
{
    return currentRow();
}

int TupFramesTable::layersTotal()
{
    return rowCount();
}

int TupFramesTable::lastFrameByLayer(int layerPos)
{
    int pos = verticalHeader()->logicalIndex(layerPos);

    if (pos < 0 || pos > k->layers.count())
        return -1;

    return k->layers[pos].lastItem;
}

// FRAMES

void TupFramesTable::insertFrame(int layerPos, const QString &name)
{
    Q_UNUSED(name);

    if (layerPos < 0 || layerPos >= k->layers.count()) 
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    
    k->layers[layerPos].lastItem++;
    
    if (k->layers[layerPos].lastItem >= columnCount())
        insertColumn(k->layers[layerPos].lastItem);
    
    setAttribute(layerPos, k->layers[layerPos].lastItem, TupFramesTableItem::IsUsed, true);
    setAttribute(layerPos, k->layers[layerPos].lastItem, TupFramesTableItem::IsSound, k->layers[layerPos].sound);
    
    viewport()->update();
}

/*
void TupFramesTable::setCurrentFrame(TupFramesTableItem *item)
{
    setCurrentItem(item);
}
*/

void TupFramesTable::updateLayerHeader(int layerIndex)
{
    k->layerColumn->updateSelection(layerIndex); 
}

void TupFramesTable::selectFrame(int index)
{
    setCurrentItem(item(currentRow(), index));
}

void TupFramesTable::removeFrame(int layerPos, int position)
{
    Q_UNUSED(position);

    if (layerPos < 0 || layerPos >= k->layers.count())
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    setAttribute(layerPos, k->layers[layerPos].lastItem, TupFramesTableItem::IsUsed, false);
    k->layers[layerPos].lastItem--;
    viewport()->update();
}

void TupFramesTable::lockFrame(int layerPos, int position, bool lock)
{
    if (layerPos < 0 || layerPos >= k->layers.count())
        return;
    
    layerPos = verticalHeader()->logicalIndex(layerPos);
    setAttribute(layerPos, position, TupFramesTableItem::IsLocked, lock);
    viewport()->update();
}

void TupFramesTable::setAttribute(int row, int col, TupFramesTableItem::Attributes att, bool value)
{
    QTableWidgetItem *item = this->item(row, col);
    
    if (!item) {
        item = new TupFramesTableItem;
        setItem(row, col, item);
    }
    
    item->setData(att, value);
}

void TupFramesTable::fixSize()
{
    for (int column = 0; column < columnCount(); column++)
        horizontalHeader()->resizeSection(column, k->rectWidth);

    for (int row = 0; row < rowCount(); row++)
         verticalHeader()->resizeSection(row, k->rectHeight);
}

void TupFramesTable::emitRequestSelectFrame(int currentRow, int currentColumn, int previousRow, int previousColumn)
{
     Q_UNUSED(previousRow);
     Q_UNUSED(previousColumn);

     if (k->frameIndex != currentColumn || k->layerIndex != currentRow) {
         k->frameIndex = currentColumn;
         k->layerIndex = currentRow;
         emit emitSelection(currentRow, currentColumn);
     }
}

void TupFramesTable::mousePressEvent(QMouseEvent *event)
{
    int frameIndex = columnAt(event->x());

    int total = columnCount();
    if ((frameIndex >= total - 11) && (frameIndex <= total - 1)) {
        int newTotal = total + 100;
        for (int i=total; i < newTotal; i++) {
             insertColumn(i);
        }
        fixSize();
    }

    QTableWidget::mousePressEvent(event);
}
