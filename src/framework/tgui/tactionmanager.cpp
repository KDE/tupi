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

#include "tactionmanager.h"

/**
 * Construye un manejador de acciones.
 * @param parent widget que contine el manejador de acciones
 */
TActionManager::TActionManager(QObject *parent) : QObject(parent)
{
    setObjectName("TActionManager" + parent->objectName());
}

/**
 * Destructor
 */
TActionManager::~TActionManager()
{
}

/**
 * Inserta una accion al manejador
 * @param action accion para adicionar 
 * @return 
 */
bool TActionManager::insert(QAction *action, const QString &_id, const QString &container)
{
    QString id = _id.toLower();
    if (id.isEmpty() || container.isEmpty())
        return false;

    QAction *a = (m_actionContainer[container])[id];
    if (a == action) {
        #ifdef K_DEBUG
            QString msg = "TActionManager::insert() - Fatal Error: Cannot insert action with id -> " + id;;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif

        return false;
    }

    action->setParent(this);
    m_actionContainer[container].insert(id, action);

    return true;
}

/**
 * Remueve una accion del manejador
 * @param action para remover
 */
void TActionManager::remove(QAction* action, const QString &container)
{
    delete take(action, container);
}

/**
 * Remueve una accion del manejador retornando dicha accion.
 * @param action para remover
 * @return la accion removida o cero si esta no estaba en el manejador
 */
QAction *TActionManager::take(QAction* action, const QString &container)
{
    QAction *a = 0;
    QString id = m_actionContainer[container].key(action);

    if (! container.isEmpty()) {
        if (m_actionContainer[container].contains(id))
            a = m_actionContainer[container].take(id);
    } else {
            foreach (QString key, m_actionContainer.keys()) {
                     if (m_actionContainer[key].contains(id)) {
                         a = m_actionContainer[key].take(id);
                         break;
                     }
            }
    }

    if (!a || a != action)
        return 0;

    return a;
}

/**
 * Busca una accion en el manejardor.
 * @param id asociado a la accion
 * @return la accion requeriada
 */
QAction *TActionManager::find(const QString &_id, const QString &container) const
{
    QAction *action = 0;
    QString id = _id.toLower();

    if (!container.isEmpty()) {
        if (m_actionContainer[container].contains(id))
            action = m_actionContainer[container][id];
    } else {
        foreach (QString key, m_actionContainer.keys()) {
                 if (m_actionContainer[key].contains(id)) {
                     action = m_actionContainer[key][id];
                     break;
                 }
        }
    }

    if (action == 0) {
        #ifdef K_DEBUG
            QString msg = "TActionManager::find() - Fatal Error: Returning NULL action: " + id + " in " + container;
            #ifdef Q_OS_WIN32
                qDebug() << msg;
            #else
                tError() << msg;
            #endif
        #endif
    }

    return action;
}

/**
 * Retorna la accion asociada a id
 * @param id 
 */
QAction *TActionManager::operator[](const QString &id) const
{
    return find(id);
}

/**
 * Retorna la accion asociada a id
 * @param id
 */
void TActionManager::enable(const QString &id, bool flag)
{
   QAction *action = find(id);
   if (action != 0)
       action->setEnabled(flag);
}

/**
 * Exec the action defined by id 
 * @param id
 */
void TActionManager::exec(const QString &id)
{
   QAction *action = find(id);
   if (action != 0) {
       action->toggle();
       action->setChecked(true);
   } 
}

QMenuBar *TActionManager::setupMenuBar(QMenuBar *menuBar, const QStringList &containers, bool clear)
{
    if (menuBar) {
        if (clear)
            menuBar->clear();
    } else {
        menuBar = new QMenuBar(0);
    }

    foreach (QString container, containers)
             menuBar->addMenu(setupMenu(0, container, clear));

    return menuBar;
}

QMenu *TActionManager::setupMenu(QMenu *menu, const QString &container, bool clear)
{
    if (!menu)
        menu = new QMenu(container);

    if (clear)
        menu->clear();

    foreach (QAction *a, m_actionContainer[container]) {
             if (a)
                 menu->addAction(a);
    }

    return menu;
}

QToolBar *TActionManager::setupToolBar(QToolBar *toolBar, const QString &container, bool clear)
{
    if (!toolBar)
        toolBar = new QToolBar();

    if (clear)
        toolBar->clear();

    foreach (QAction *a, m_actionContainer[container]) {
             if (a)
                 toolBar->addAction(a);
    }

    if (m_actionContainer.count() == 0)
        toolBar->hide();
     else
        toolBar->show();

    return toolBar;
}
