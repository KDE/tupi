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

#ifndef TUPHELPWIDGET_H
#define TUPHELPWIDGET_H

#include "tupmodulewidgetbase.h"

#include <QWidget>
#include <QMap>
#include <QTreeWidget>
#include <QDir>

// class KHelpWidgetManager;

/**
 * @author David Alejandro Cuadrado Cabrera
*/

class TupHelpWidget : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupHelpWidget(const QString &path, QWidget *parent = 0);
        ~TupHelpWidget();
        QString helpPath() const;

    private slots:
        void tryToLoadPage(QTreeWidgetItem *, QTreeWidgetItem *);
        void loadPage(const QString &path);

    signals:
        void pageLoaded(const QString &content);

    private:
        QDir *m_helpPath;
        QMap<QTreeWidgetItem *, QString> m_files;
};

#endif
