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

#ifndef TUPLIBRARYWIDGET_H
#define TUPLIBRARYWIDGET_H

#include "tupmodulewidgetbase.h"
#include "tupitempreview.h"
#include "timagebutton.h"
#include "tupitemmanager.h"
#include "tupnewitemdialog.h"
#include "tuplibraryobject.h"

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QMap>
#include <QDir>
#include <QMouseEvent>

class TupLibrary;
typedef QMap<QString, TupLibraryObject *> LibraryObjects;

/**
 * @author David Cuadrado
*/

class TupLibraryWidget : public TupModuleWidgetBase
{
    Q_OBJECT

    public:
        TupLibraryWidget(QWidget *parent = 0);
        ~TupLibraryWidget();
        void resetGUI();
        void setLibrary(TupLibrary *library);
        void setNetworking(bool isNetworked);

    protected:
        virtual void sceneResponse(TupSceneResponse *response);
        virtual void libraryResponse(TupLibraryResponse *response);
        virtual void frameResponse(TupFrameResponse *response);

    private slots:
        void addFolder();
        void previewItem(QTreeWidgetItem *item);
        void insertObjectInWorkspace();
        void removeCurrentGraphic();
        void cloneObject(QTreeWidgetItem *item);
        void exportObject(QTreeWidgetItem *item);
        void createRasterObject();
        void createVectorObject();
        void renameObject(QTreeWidgetItem *item);
        void importGraphicObject();
        void refreshItem(QTreeWidgetItem *item);
        void updateLibrary(QString node, QString target);
        void activeRefresh(QTreeWidgetItem *item);
        void openInkscapeToEdit(QTreeWidgetItem *item);
        void openGimpToEdit(QTreeWidgetItem *item);
        void openKritaToEdit(QTreeWidgetItem *item);
        void openMyPaintToEdit(QTreeWidgetItem *item);
        void updateItemFromSaveAction();

    public slots:
        void importBitmap();
        void importSvg();
        void importBitmapArray();
        void importSvgArray();
        void importSound();

    signals:
        void requestCurrentGraphic();

    private:
        void callExternalEditor(QTreeWidgetItem *item, TupNewItemDialog::ThirdParty software);
        void executeSoftware(TupNewItemDialog::ThirdParty software, QString &path);
        void updateItem(const QString &name, const QString &extension, TupLibraryObject *object);
        bool itemNameEndsWithDigit(QString &name);
        int getItemNameIndex(QString &name) const;
        QString nameForClonedItem(QString &name, QString &extension, int index, QString &path) const;
        QString nameForClonedItem(QString &smallId, QString &extension, QString &path) const;
        QString verifyNameAvailability(QString &name, QString &extension, bool isCloningAction);

        struct Private;
        Private *const k;
};

#endif
