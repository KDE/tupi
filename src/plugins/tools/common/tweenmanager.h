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

#ifndef TWEENMANAGER_H
#define TWEENMANAGER_H

#include "tglobal.h"
#include "timagebutton.h"
#include "tosd.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QBoxLayout>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMenu>
#include <QDir>

/**
 * @author Gustav Gonzalez 
*/

// class QListWidgetItem;

class TUPI_EXPORT TweenManager: public QWidget 
{
    Q_OBJECT

    public:
        TweenManager(QWidget *parent = 0);
        ~TweenManager();
        void loadTweenList(QList<QString> tweenList);
        void resetUI();
        QString currentTweenName() const;
        int listSize(); 
        void removeItemFromList();
        void updateTweenName(const QString &name);

    signals:
        void addNewTween(const QString &name);
        void editCurrentTween(const QString &name);
        void removeCurrentTween(const QString &name);
        void getTweenData(const QString &name);

    private slots:
        void addTween();
        void editTween();
        void editTween(QListWidgetItem *item);
        void removeTween();
        void showMenu(const QPoint &point);
        void updateTweenData(QListWidgetItem *item);

    private:
        bool itemExists(const QString &name);

        struct Private;
        Private *const k;
};

#endif
