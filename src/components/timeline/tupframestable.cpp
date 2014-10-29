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

void TupFramesTableItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
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
    /*
    struct LayerItem {
        LayerItem() : lastItem(-1), sound(false) {};
        int lastItem;
        bool sound;
    };
    */
    
    int rectWidth;
    int rectHeight;

    int sceneIndex;
    int layerIndex;
    int frameIndex;

    // QList<LayerItem> layers;
    TupTimeLineRuler *ruler;
    TupLayerHeader *layerColumn;

    bool removingLayer;
    bool removingFrame;
};

TupFramesTable::TupFramesTable(int sceneIndex, QWidget *parent) : QTableWidget(0, 200, parent), k(new Private)
{
    k->sceneIndex = sceneIndex;
    k->frameIndex = 0;
    k->layerIndex = 0;
    k->ruler = new TupTimeLineRuler;

    k->removingLayer = false;
    k->removingFrame = false;

    k->layerColumn = new TupLayerHeader;
    connect(k->layerColumn, SIGNAL(nameChanged(int, const QString &)), this, SIGNAL(layerNameChanged(int, const QString &)));

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
    setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(requestFrameSelection(int, int, int, int)));
    // connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)), this, SLOT(requestFrameSelection(QTableWidgetItem *, QTableWidgetItem *)));

    setHorizontalHeader(k->ruler);

    connect(k->ruler, SIGNAL(selectionChanged(int)), this, SLOT(frameSelectionFromRuler(int)));

    setVerticalHeader(k->layerColumn);

    connect(k->layerColumn, SIGNAL(selectionChanged(int)), this, SLOT(frameSelectionFromLayerHeader(int)));
    connect(k->layerColumn, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(visibilityChanged(int, bool)));

    setItemSize(10, 25);
    
    horizontalHeader()->setSectionResizeMode(QHeaderView::Custom);
    // verticalHeader()->setSectionResizeMode(QHeaderView::Custom);
    k->layerColumn->setSectionResizeMode(QHeaderView::Custom);
}

void TupFramesTable::frameSelectionFromRuler(int frameIndex)
{
    emit frameSelected(0, frameIndex);
}

void TupFramesTable::frameSelectionFromLayerHeader(int layerIndex)
{
    tError() << "TupFramesTable::frameSelectionFromLayerHeader() - layerIndex -> " << layerIndex;

    emit frameSelected(layerIndex, currentColumn());
}

/*
void TupFramesTable::requestFrameSelection(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    Q_UNUSED(previous);

    TupFramesTableItem *item = dynamic_cast<TupFramesTableItem *>(current);
    
    if (item) {
        if (item->isUsed()) {
            tError() << "TupFramesTable::requestFrameSelection() - Just tracing 1";
            emit frameChanged(k->sceneIndex, verticalHeader()->visualIndex(this->row(item)), this->column(item));
        } else {
            #ifdef K_DEBUG
                QString msg = "TupFramesTable::requestFrameSelection <- item exists but is unset right now";
                #ifdef Q_OS_WIN32
                    qDebug() << msg;
                #else
                    tFatal() << msg;
                #endif
            #endif  
	    }
    } else { 
        tError() << "TupFramesTable::requestFrameSelection() - Called from -> currentItemChanged()";
        tError() << "";
        emit frameRequest(TupProjectActionBar::InsertFrame, currentColumn(), currentRow(), k->sceneIndex);
    }
}
*/

void TupFramesTable::setItemSize(int w, int h)
{
    k->rectHeight = h;
    k->rectWidth = w;
    
    fixSize();
}

bool TupFramesTable::isSoundLayer(int index)
{
    // if (index < 0 && index >= k->layers.count())
    if (index < 0 && index >= rowCount())
        return false;
    
    // return k->layers[index].sound;

    return k->layerColumn->isSound(index);
}

void TupFramesTable::insertLayer(int index, const QString &name)
{
    tError() << "TupFramesTable::insertLayer() - Inserting layer at index: " << index;

    insertRow(index);

    /*    
    Private::LayerItem layer;
    layer.sound = false;
    k->layers.insert(index, layer);
    */

    k->layerColumn->insertSection(index, name);
    
    fixSize();
}

void TupFramesTable::insertSoundLayer(int index, const QString &name)
{
    insertRow(index);

    /*
    Private::LayerItem layer;
    layer.sound = true;
    k->layers.insert(index, layer);
    */

    k->layerColumn->insertSection(index, name);
    
    fixSize();
}

void TupFramesTable::removeCurrentLayer()
{
    // int pos = verticalHeader()->logicalIndex(currentRow());
    // removeLayer(pos);

    removeLayer(currentRow());
}

void TupFramesTable::removeLayer(int index)
{
    tError() << "TupFramesTable::removeLayer() - Removing layer at: " << index;
    tError() << "TupFramesTable::removeLayer() - rowCount(): " << rowCount();

    // setUpdatesEnabled(false);
    k->removingLayer = true;

    removeRow(index);
    k->layerColumn->removeSection(index);

    // setUpdatesEnabled(true);

    /*
    if (rowCount() > 1) {
        removeRow(index);
        k->layerColumn->removeSection(index);
    } else {
        for(int i=0; i<= k->layerColumn->lastFrame(0); i++) 
            setAttribute(0, i, TupFramesTableItem::IsUsed, false);

        k->layerColumn->resetLastFrame(0);
    }
    */
}

void TupFramesTable::moveLayer(int index, int newIndex)
{
    if (index < 0 || index >= rowCount() || newIndex < 0 || newIndex >= rowCount()) 
        return;

    tError() << "TupFramesTable::moveLayer() - Moving Layer from -> " << index << " to -> " << newIndex;
    k->layerColumn->moveHeaderSection(index, newIndex);
    for (int frameIndex = 0; frameIndex < k->layerColumn->lastFrame(index); frameIndex++)
         exchangeFrame(frameIndex, index, newIndex);

    blockSignals(true); 
    selectFrame(newIndex, currentColumn());
    blockSignals(false);

    // tError() << "TupFramesTable::moveLayer() - Requesting new selection...";
    // tError() << "TupFramesTable::moveLayer() - layer index: " << newIndex;
    // emit frameSelected(newIndex, currentColumn());

    // emit frameSelected(k->layerColumn->logicalIndex(newIndex), currentColumn());
}

void TupFramesTable::exchangeFrame(int frameIndex, int currentLayer, int newLayer)
{
    QTableWidgetItem * oldItem  = takeItem(frameIndex, currentLayer);
    QTableWidgetItem * newItem  = takeItem(frameIndex, newLayer);

    setItem(frameIndex, newLayer, oldItem);
    setItem(frameIndex, currentLayer, newItem);

    // if (!external)
    //     setCurrentItem(oldItem);
}

void TupFramesTable::setLayerVisibility(int layerIndex, bool isVisible)
{
    k->layerColumn->setSectionVisibility(layerIndex, isVisible);
}

void TupFramesTable::setLayerName(int layerIndex, const QString &name)
{
    k->layerColumn->setSectionTitle(layerIndex, name);
}

int TupFramesTable::currentLayer()
{
    return currentRow();
}

int TupFramesTable::layersTotal()
{
    return rowCount();
}

int TupFramesTable::lastFrameByLayer(int index)
{
    if (index < 0 || index >= rowCount())
        return -1;

    return k->layerColumn->lastFrame(index);
}

// FRAMES

void TupFramesTable::insertFrame(int layerIndex, const QString &name)
{
    Q_UNUSED(name);

    tError() << "TupFramesTable::insertFrame() - Inserting frame at layer index -> " << layerIndex;

    if (layerIndex < 0 || layerIndex >= rowCount())
        return;

    k->layerColumn->updateLastFrame(layerIndex, true);
  
    int lastFrame = k->layerColumn->lastFrame(layerIndex); 

    setAttribute(layerIndex, lastFrame, TupFramesTableItem::IsUsed, true);
    setAttribute(layerIndex, lastFrame, TupFramesTableItem::IsSound, false);

    // viewport()->update();
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

void TupFramesTable::removeFrame(int index, int position)
{
    Q_UNUSED(position);

    // if (index < 0 || index >= k->layers.count())
    if (index < 0 || index >= rowCount())
        return;

    k->removingFrame = true;
    
    // index = verticalHeader()->logicalIndex(index);
    // setAttribute(index, k->layers[index].lastItem, TupFramesTableItem::IsUsed, false);

    setAttribute(index, k->layerColumn->lastFrame(index), TupFramesTableItem::IsUsed, false);

    // k->layers[index].lastItem--;

    k->layerColumn->updateLastFrame(index, false);
    viewport()->update();
}

void TupFramesTable::lockFrame(int index, int position, bool lock)
{
    // if (index < 0 || index >= k->layers.count())

    if (index < 0 || index >= rowCount())
        return;
    
    // index = verticalHeader()->logicalIndex(index);

    setAttribute(index, position, TupFramesTableItem::IsLocked, lock);
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
         k->layerColumn->resizeSection(row, k->rectHeight);
         // verticalHeader()->resizeSection(row, k->rectHeight);
}

void TupFramesTable::requestFrameSelection(int currentSelectedRow, int currentSelectedColumn, int previousRow, int previousColumn)
{
    if (!k->removingLayer) {
        if (k->removingFrame) {
            /*
            k->removingFrame = false;

            if (previousRow != currentSelectedRow)
                k->layerColumn->updateSelection(currentSelectedRow);

            if ((previousColumn != currentSelectedColumn) || (columnCount() == 1))
                emit frameSelected(currentColumn(), currentRow());

            return;
            */
        } else {
            tError() << "TupFramesTable::requestFrameSelection() - Tracing usual selection...";
            if (previousColumn != currentSelectedColumn || previousRow != currentSelectedRow) 
                emit frameSelected(currentRow(), currentColumn());
        }
    } else { // A layer is being removed
        k->removingLayer = false;

        if (previousRow != 0) {
            if (previousRow != rowCount() - 1) {
                blockSignals(true);
                setCurrentItem(item(previousRow - 1, currentColumn())); 
                k->layerColumn->updateSelection(previousRow - 1);
                blockSignals(false);
            }
        }
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

void TupFramesTable::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Right) {
        int limit = columnCount()-1;
        int next = currentColumn()+1;
        if (next <= limit) 
            setCurrentCell(currentRow(), next);
    }    

    if (event->key() == Qt::Key_Left) {
        int next = currentColumn()-1;
        if (next >= 0) 
            setCurrentCell(currentRow(), next);
    }

    if (event->key() == Qt::Key_Up) {
        int next = currentRow()-1;
        if (next >= 0) 
            setCurrentCell(next, currentColumn());
    }

    if (event->key() == Qt::Key_Down) {
        int limit = rowCount()-1;
        int next = currentRow()+1;
        if (next <= limit)
            setCurrentCell(next, currentColumn());
    }
}

void TupFramesTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void TupFramesTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void TupFramesTable::selectFrame(int layerIndex, int frameIndex)
{
    blockSignals(true);
    setCurrentCell(layerIndex, frameIndex);
    updateLayerHeader(layerIndex);
    blockSignals(false);
}
