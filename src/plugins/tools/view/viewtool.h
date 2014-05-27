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

#ifndef VIEWTOOL_H
#define VIEWTOOL_H

#include "tglobal.h"
#include "tuptoolplugin.h"
#include "zoomconfigurator.h"
#include "tupgraphicsscene.h"

#ifdef K_DEBUG
#include "tdebug.h"
#endif

#include <QObject>
#include <QLabel>
#include <QGraphicsRectItem>
#include <QSize>
#include <QKeySequence>
#include <QImage>
#include <QPaintDevice>
#include <QGraphicsView>

// class TupGraphicsScene;

/**
 * @author Jorge Cuadrado
*/

class TUPI_EXPORT ViewTool: public TupToolPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.maefloresta.tupi.TupToolInterface" FILE "viewtool.json")

    public:
        ViewTool();
        ~ViewTool();

        virtual void init(TupGraphicsScene *scene);
        virtual QStringList keys() const;
        
        virtual void press(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void move(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);
        virtual void release(const TupInputDeviceInformation *input, TupBrushManager *brushManager, TupGraphicsScene *scene);

        virtual QMap<QString, TAction *> actions() const;
        int toolType() const;
        
        virtual QWidget *configurator();
        virtual void aboutToChangeScene(TupGraphicsScene *scene);
        virtual void aboutToChangeTool();
        virtual void saveConfig();
        virtual void keyPressEvent(QKeyEvent *event);
        virtual QCursor cursor() const;

        virtual void autoZoom();
        virtual void setProjectSize(const QSize size);

    signals:
        void closeHugeCanvas();
        void callForPlugin(int menu, int index);

    private:
        void setupActions();

    private:
        struct Private;
        Private *const k;

    /*
    private:
        QMap<QString, TAction *> m_actions;
        QGraphicsRectItem *m_rect;
        bool added;
        QPointF firstPoint; 
        TupGraphicsScene *m_scene;
        ZoomConfigurator *m_configurator;
    */
};

#endif
