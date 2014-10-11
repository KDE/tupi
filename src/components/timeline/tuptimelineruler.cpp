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

#include "tuptimelineruler.h"

TupTimeLineRuler::TupTimeLineRuler(QWidget *parent) : QHeaderView(Qt::Horizontal, parent)
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[TupTimeLineRuler()]";
        #else
            TINIT;
        #endif
    #endif

    setHighlightSections(true);
    setStyleSheet("QHeaderView { background-color: #CCCCCC; }");
}

TupTimeLineRuler::~TupTimeLineRuler()
{
    #ifdef K_DEBUG
        #ifdef Q_OS_WIN32
            qDebug() << "[~TupTimeLineRuler()]";
        #else
            TEND;
        #endif
    #endif
}

void TupTimeLineRuler::paintSection(QPainter *painter, const QRect & rect, int logicalIndex) const
{
    if (!model() || !rect.isValid())
        return;

    painter->save();

    QModelIndex currentSelection = currentIndex(); 
    int column = currentSelection.row(); 

    if (selectionModel()->isSelected(model()->index(column, logicalIndex))) {
        QBrush brush(QColor(0, 135, 0, 80));
        QRect green = QRect();
        painter->fillRect(rect, brush);
    }

    int x = rect.bottomRight().x() - 1;
    int topY = rect.topRight().y();
    int bottomY = rect.bottomRight().y();
    painter->drawLine(x, bottomY, x, bottomY - 6);
    painter->drawLine(x, topY, x, topY + 4);

    logicalIndex++;

    if (logicalIndex == 1 || logicalIndex % 5 == 0) {
        QFont label("Arial", 7, QFont::Normal, false);
        QFontMetrics fm(label);

        QString number = QString::number(logicalIndex);
	
        painter->setFont(label);	
        painter->drawText((int)(rect.center().x() - (fm.width(number)/2)), 
                          (int)(rect.center().y() + (fm.height()/2)) - 2, number);
    }

    QPen pen = painter->pen();
    pen.setWidth(4);
    painter->setPen(pen); 
    painter->drawLine(rect.bottomLeft(), rect.bottomRight());
    painter->restore();

}

void TupTimeLineRuler::updateSelected(int logical)
{
    select(logical);

    emit logicalSectionSelected(logical);
}

void TupTimeLineRuler::select(int logical)
{
    selectionModel()->select(model()->index(0, logical), QItemSelectionModel::ClearAndSelect);
    viewport()->update(QRect(sectionViewportPosition(logical), 0, sectionSize(logical),viewport()->height()));
}

/*
void TupTimeLineRuler::mouseMoveEvent(QMouseEvent *e)
{
    if (e->buttons() & Qt::LeftButton)
        updateSelected(logicalIndexAt(e->pos()));

    QHeaderView::mouseMoveEvent(e);
}
*/

void TupTimeLineRuler::mousePressEvent(QMouseEvent *event)
{
    emit logicalSectionSelected(logicalIndexAt(event->pos()));
}

