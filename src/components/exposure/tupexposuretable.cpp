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

#include "tupexposuretable.h"

/**
 * @author Jorge Cuadrado
 */

/////////////////
class TUPI_EXPORT TupExposureVerticalHeader : public QHeaderView
{
    public:
        TupExposureVerticalHeader(QWidget * parent = 0);
        ~TupExposureVerticalHeader();
        void paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const;
};

TupExposureVerticalHeader::TupExposureVerticalHeader(QWidget * parent) : QHeaderView(Qt::Vertical, parent)
{
    setFixedWidth(25);
}

TupExposureVerticalHeader::~TupExposureVerticalHeader()
{
}

void TupExposureVerticalHeader::paintSection(QPainter * painter, const QRect & rect, int logicalIndex) const
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
    QFont font = this->font();
    font.setPointSize(7);
    // QFont font("Arial", 7, QFont::Normal, false);
    QFontMetrics fm(font);

    int x = rect.normalized().x() + ((rect.normalized().width() - fm.width(text))/2);
    int y = rect.normalized().bottomLeft().y() - (1 + (rect.normalized().height() - fm.height())/2);

    painter->setFont(font);
    painter->setPen(QPen(Qt::black, 1, Qt::SolidLine));
    painter->drawText(x, y, text);
}

/////////////////

class TupExposureItemDelegate : public QItemDelegate
{
    public:
        TupExposureItemDelegate(QObject * parent = 0);
        ~TupExposureItemDelegate();
        virtual void paint(QPainter * painter, const QStyleOptionViewItem & option, const QModelIndex & index) const;
};

TupExposureItemDelegate::TupExposureItemDelegate(QObject * parent) :  QItemDelegate(parent)
{
}

TupExposureItemDelegate::~TupExposureItemDelegate()
{
}

void TupExposureItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem & option, const QModelIndex & index) const
{
    QItemDelegate::paint(painter, option, index);
    TupExposureTable *table = qobject_cast<TupExposureTable *>(index.model()->parent());
    QTableWidgetItem *item = table->itemFromIndex(index);

    if (item) {
        /* SQA: Useful code
        if (item->data(TupExposureTable::IsLocked).toBool()) {
             QPixmap pixmap(THEME_DIR + "icons/padlock.png");
             painter->drawPixmap(1, 5, pixmap);
        } else 
        */

        int x = option.rect.topLeft().x() + 2;
        int y = option.rect.topLeft().y() + 2;
        int w = option.rect.bottomRight().x() - x - 2;
        int h = option.rect.bottomRight().y() - y - 2;

        if ((item->data(TupExposureTable::IsEmpty).toInt() == TupExposureTable::Empty) && !item->data(TupExposureTable::IsLocked).toBool()) {
            QPen pen(QColor(100, 100, 100, 30));
            pen.setStyle(Qt::DashLine);
            painter->setPen(pen);
            painter->drawRect(x, y, w, h);
        }

        if ((item->data(TupExposureTable::IsEmpty).toInt() == TupExposureTable::Used) && !item->data(TupExposureTable::IsLocked).toBool()) {
            QPainterPath path(QPointF(x, y));
            path.lineTo(x, y + h - 3);
            path.lineTo(x + 3, y + h);
            path.lineTo(x + w, y + h);
            path.lineTo(x + w, y + 4);
            path.lineTo(x + w - 8, y); 

            painter->fillPath(path, QBrush(QColor(0, 102, 255, 80)));
        }
    }
}

////////////////////////

struct TupExposureTable::Private
{
    TupExposureHeader *header;
    QMenu *menu;
    bool removingLayer;
    bool removingFrame;
    bool isLocalRequest;
};

TupExposureTable::TupExposureTable(QWidget * parent) : QTableWidget(parent), k(new Private)
{
    k->isLocalRequest = false;

    TupExposureVerticalHeader *verticalHeader = new TupExposureVerticalHeader(this);
    setVerticalHeader(verticalHeader);

    setItemDelegate(new TupExposureItemDelegate(this));
    k->removingLayer = false;
    k->removingFrame = false;

    QTableWidgetItem *prototype = new QTableWidgetItem();
    prototype->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEditable);
    prototype->setTextAlignment(Qt::AlignCenter);
    prototype->setData(IsEmpty, Unset);

    setItemPrototype(prototype);

    setRowCount(100);

    for (int i=0; i < 100; i++)
         setRowHeight(i, 20);

    k->header = new TupExposureHeader(this);

    connect(k->header, SIGNAL(visibilityChanged(int, bool)), this, SIGNAL(layerVisibilityChanged(int, bool)));
    connect(k->header, SIGNAL(nameChanged(int, const QString &)), this, SIGNAL(layerNameChanged(int, const QString & )));
    connect(k->header, SIGNAL(sectionMoved(int, int, int)), this, SLOT(requestLayerMove(int, int, int)));
    connect(k->header, SIGNAL(selectionChanged(int)), this, SLOT(updateLayerSelection(int)));

    setHorizontalHeader(k->header);

    connect(this, SIGNAL(cellClicked(int, int)), this, SLOT(markUsedFrames(int, int)));
    connect(this, SIGNAL(currentCellChanged(int, int, int, int)), this, SLOT(requestFrameSelection(int, int, int, int)));

    setSelectionBehavior(QAbstractItemView::SelectItems);
    setSelectionMode(QAbstractItemView::SingleSelection);

    k->menu = 0;
}

void TupExposureTable::requestFrameRenaming(QTableWidgetItem * item)
{
    QModelIndex  index = indexFromItem(item);
    emit frameRenamed(index.column(), index.row(), item->text());
}

void TupExposureTable::requestFrameSelection(int currentSelectedRow, int currentColumn, int previousRow, int previousColumn)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupExposureTable::requestFrameSelection()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (!k->removingLayer) { 
        if (k->removingFrame) {
            k->removingFrame = false;
            
            if ((previousColumn != currentColumn) || (columnCount() == 1))
                 k->header->updateSelection(currentColumn);

            if (currentSelectedRow >= framesCountAtCurrentLayer()) {
                selectionModel()->clearSelection();
                // TODO: Check why cell selection is awkward
                selectFrame(currentLayer(), framesCountAtCurrentLayer() - 1);
            } else {
                if (previousRow != currentSelectedRow) 
                    emit frameSelected(currentLayer(), currentRow());
            }

            return;
        } else {
            QTableWidgetItem *frame = item(currentSelectedRow, currentColumn);
            if (frame) {
                if (previousColumn != currentColumn || previousRow != currentSelectedRow)
                    emit frameSelected(currentLayer(), currentRow());

                if ((previousColumn != currentColumn) || (columnCount() == 1))
                    k->header->updateSelection(currentColumn);
            } 
        }
    } else { // A layer is being removed
        k->removingLayer = false;
        selectFrame(currentColumn, currentSelectedRow);

        if (previousColumn == 0)
            k->header->updateSelection(0);
        else
            k->header->updateSelection(currentColumn);
    }
}

void TupExposureTable::requestLayerMove(int logicalIndex, int oldVisualIndex, int newVisualIndex)
{
    Q_UNUSED(logicalIndex);

    if (!k->header->sectionIsMoving()) {
        int distance = newVisualIndex - oldVisualIndex;
        if (distance > 0) {
            if (distance > 1)
                newVisualIndex = oldVisualIndex + 1;
        } else {
            distance = abs(distance);
            if (distance > 1)
                newVisualIndex = oldVisualIndex - 1;
        }

        k->isLocalRequest = true;
        emit layerMoved(oldVisualIndex, newVisualIndex);
    }
}

void TupExposureTable::updateLayerSelection(int layerIndex)
{
    selectFrame(layerIndex, currentRow());
}

TupExposureTable::~TupExposureTable()
{
    delete k;
}

QString TupExposureTable::frameName(int layerIndex, int frameIndex)
{
    QTableWidgetItem *frame = item(frameIndex , layerIndex);
    if (frame)
        return frame->text();

    return "";
}

void TupExposureTable::setFrameName(int layerIndex, int frameIndex, const QString &name)
{
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    QFont font = this->font();
    font.setPointSize(7);
    frame->setFont(font);
    // frame->setFont(QFont("Arial", 7, QFont::Normal, false));

    if (frame) {
        if (frame->text() != name)
            frame->setText(name);
    }
}

void TupExposureTable::setLayerName(int layerIndex, const QString & name)
{
    k->header->setSectionTitle(k->header->logicalIndex(layerIndex), name);
}

bool TupExposureTable::frameIsLocked(int layerIndex, int frameIndex)
{
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame) {
        return frame->data(IsLocked).toBool();
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::frameIsLocked() - Layer: " + QString::number(layerIndex) + QString(", Frame: ") + QString::number(frameIndex) + QString(" doesn't exist");
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
    return false;
}

TupExposureTable::FrameType TupExposureTable::frameState(int layerIndex, int frameIndex)
{ 
    TupExposureTable::FrameType type = TupExposureTable::Unset;
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame)
        type = TupExposureTable::FrameType(frame->data(TupExposureTable::IsEmpty).toInt()); 

    return type;
}

void TupExposureTable::updateFrameState(int layerIndex, int frameIndex, TupExposureTable::FrameType value)
{
    QTableWidgetItem *frame = item(frameIndex, layerIndex);
    if (frame) {
        frame->setData(IsEmpty, value);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::updateFrameState() - Error: No frame at [" + QString::number(layerIndex) + ", " + QString::number(frameIndex) + "]";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupExposureTable::selectFrame(int layerIndex, int frameIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupExposureTable::selectFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    if (k->header->currentSectionIndex() != layerIndex)
        k->header->updateSelection(layerIndex);

    setCurrentCell(frameIndex, layerIndex);
}

void TupExposureTable::setMenu(QMenu *menu)
{
    k->menu = menu;
}

int TupExposureTable::currentLayer() const
{
    return k->header->visualIndex(currentColumn());
}

int TupExposureTable::currentFrame() const
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupExposureTable::currentFrame()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    QTableWidgetItem *frame = currentItem();

    if (frame) {
        if (frame->data(TupExposureTable::IsEmpty).toInt() != Unset)
            return currentRow();
    }

    return 0;
}

void TupExposureTable::insertLayer(int index, const QString & name)
{
    insertColumn(index);
    setColumnWidth(index, 70);
    k->header->insertSection(index, name);
}

void TupExposureTable::insertFrame(int layerIndex, int frameIndex, const QString & name, bool external)
{
    QTableWidgetItem *frame = new QTableWidgetItem;
    QFont font = this->font();
    font.setPointSize(7);
    frame->setFont(font);
    // frame->setFont(QFont("Arial", 7, QFont::Normal, false));
    frame->setSizeHint(QSize(65, 10));
    frame->setText(name);
    frame->setData(IsEmpty, Empty);
    frame->setTextAlignment(Qt::AlignCenter);

    int logicalIndex = k->header->logicalIndex(layerIndex);

    k->header->setLastFrame(logicalIndex, k->header->lastFrame(logicalIndex) + 1);

    setItem(k->header->lastFrame(logicalIndex)-1, logicalIndex, frame);

    for (int i = k->header->lastFrame(logicalIndex)-1; i > frameIndex; i--)
         exchangeFrame(layerIndex, i , layerIndex, i-1, external);

    if (k->header->lastFrame(logicalIndex) == rowCount()) {
        setRowCount(k->header->lastFrame(logicalIndex) + 100);
        int last = k->header->lastFrame(logicalIndex);
        for (int i=last; i < last + 100; i++)
             setRowHeight(i, 20);
    }
}

void TupExposureTable::setLockFrame(int layerIndex, int frameIndex, bool locked)
{
    int logicalIndex = k->header->logicalIndex(layerIndex);
    QTableWidgetItem * frame = item(frameIndex, logicalIndex);

    if (frame) {
        if (frame->data(TupExposureTable::IsEmpty).toInt() != Unset) {
            if (locked)
                frame->setBackgroundColor(QColor(255, 0, 0, 90));
            else
                frame->setBackgroundColor(QColor(255, 255, 255));

            frame->setData(IsLocked, locked);
        }
    }
}

void TupExposureTable::setLockLayer(int layerIndex, bool locked)
{
    k->header->setLockFlag(layerIndex, locked);
}

void TupExposureTable::setLayerVisibility(int visualIndex, bool visibility)
{
    k->header->setSectionVisibility(k->header->logicalIndex(visualIndex), visibility);
}

void TupExposureTable::removeLayer(int layerIndex)
{
    setUpdatesEnabled(false);
    k->removingLayer = true;

    k->header->removeSection(layerIndex);
    removeColumn(layerIndex);

    setUpdatesEnabled(true);
}

void TupExposureTable::removeFrame(int layerIndex, int frameIndex, bool fromMenu)
{
    k->removingFrame = fromMenu;

    QTableWidgetItem *item = takeItem(frameIndex, layerIndex);

    if (item) {
        k->header->setLastFrame(layerIndex, k->header->lastFrame(layerIndex)-1);
    } else {
        #ifdef K_DEBUG
            QString msg = "TupExposureTable::removeFrame() - No item available at [" + QString::number(layerIndex) + ", " + QString::number(frameIndex) + "]";
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }
}

void TupExposureTable::exchangeFrame(int oldPosLayer, int oldPosFrame, int newPosLayer, int newPosFrame, bool external)
{
    QTableWidgetItem *oldItem = takeItem(oldPosFrame, oldPosLayer);
    QTableWidgetItem *newItem = takeItem(newPosFrame, newPosLayer);

    setItem(newPosFrame, newPosLayer, oldItem);
    setItem(oldPosFrame, oldPosLayer, newItem);

    if (!external) 
        setCurrentItem(oldItem);
}

void TupExposureTable::moveLayer(int oldPosLayer, int newPosLayer)
{
    k->header->moveHeaderSection(oldPosLayer, newPosLayer, k->isLocalRequest);
    if (k->isLocalRequest)
        k->isLocalRequest = false;

    for (int frameIndex = 0; frameIndex < k->header->lastFrame(oldPosLayer); frameIndex++)
         exchangeFrame(oldPosLayer, frameIndex, newPosLayer, frameIndex, true);

    blockSignals(true);
    selectFrame(newPosLayer, currentRow());
    blockSignals(false);
}

void TupExposureTable::markUsedFrames(int frameIndex, int layerIndex)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupExposureTable::markUsedFrames()]";
        #else
            T_FUNCINFO;
        #endif
    #endif

    int layer = k->header->visualIndex(layerIndex);
    int lastFrame = k->header->lastFrame(layerIndex); 

    if (frameIndex >= lastFrame) {
        for (int column=0; column<columnCount(); column++) {
             int used = usedFrames(column); 
             if (lastFrame >= used) {
                 for (int frame=used; frame <= frameIndex; frame++)
                      emit frameUsed(column, frame);
             }
        }

        emit frameSelected(layer, frameIndex);
    } 
}

int TupExposureTable::usedFrames(int column) const
{
    return k->header->lastFrame(column);
}

bool TupExposureTable::edit(const QModelIndex & index, EditTrigger trigger, QEvent *event)
{
    QTableWidgetItem *item = itemFromIndex(index);

    if (item) {
        if (item->data(TupExposureTable::IsEmpty).toInt() != Unset)
            QTableWidget::edit(index, trigger, event);
        else
            return false;
    }

    return false;
}

void TupExposureTable::mousePressEvent(QMouseEvent * event)
{
    int frame = rowAt(event->y());
    if (event->button() == Qt::RightButton) {
        if (k->header->lastFrame(currentLayer()) >= frame) {
            if (k->menu) {
                clearFocus();
                k->menu->exec(event->globalPos());
            }
        } else {
            return;
        }
    }

    QTableWidget::mousePressEvent(event);
}

void TupExposureTable::commitData(QWidget *editor)
{
    QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor);
    QTableWidget::commitData(0); // Don't rename

    if (lineEdit)
        emit frameRenamed(currentLayer(), currentFrame(), lineEdit->text());
}

int TupExposureTable::layersCount()
{
    return k->header->sectionsTotal();
}

int TupExposureTable::framesCount()
{
    return k->header->lastFrame(0);
}

int TupExposureTable::framesCountAtCurrentLayer()
{
    return k->header->lastFrame(currentLayer());
}

void TupExposureTable::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Up) {
        int row = currentRow()-1;
        if (row > -1) {
            setCurrentCell(row, currentColumn());
        } else {
            #ifdef K_DEBUG
                QString msg = "TupExposureTable::keyPressEvent() - Warning: wrong frame index [ " + QString::number(row) + " ]";
                #ifdef Q_OS_WIN32
                    qWarning() << msg;
                #else
                    tWarning() << msg;
                #endif
            #endif
        }

        return;
    }

    if (event->key() == Qt::Key_Down) {
        int framesCount = k->header->lastFrame(currentLayer());
        int next = currentRow()+1;
        if (next >= framesCount)
            markUsedFrames(next, currentColumn());
        else
            setCurrentCell(next, currentColumn());

        return;
    }

    if (event->key() == Qt::Key_Right) {
        int limit = columnCount()-1;
        int next = currentColumn()+1;
        if (next <= limit)
            setCurrentCell(currentRow(), next);
        return;
    }

    if (event->key() == Qt::Key_Left) {
        int column = currentColumn()-1;
        if (column > -1)
            setCurrentCell(currentRow(), column);
        return;
    }   
}

// SQA : Verify if this method is required
void TupExposureTable::notifyCellClicked(int frame, int layer) 
{
    emit cellClicked(frame, layer);
}

void TupExposureTable::enterEvent(QEvent *event)
{
    if (!hasFocus())
        setFocus();

    QTableWidget::enterEvent(event);
}

void TupExposureTable::leaveEvent(QEvent *event)
{
    if (hasFocus())
        clearFocus();

    QTableWidget::leaveEvent(event);
}

void TupExposureTable::reset()
{
    int cols = columnCount();
    if (cols > 1) {
        for (int i=1; i < cols; i++)
             removeLayer(i);
    }

    int rows = rowCount();
    for (int i=1; i < rows;  i++)
         takeItem(i, 0);

    k->header->setLastFrame(0, 1);
}

