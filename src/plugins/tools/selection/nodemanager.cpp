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

#include "nodemanager.h"
#include "tupgraphicobject.h"

/**
 * This class handles the methods required to manipulate a set of nodes.
 * 
 * @author Jorge Cuadrado
*/

NodeManager::NodeManager(QGraphicsItem * parent, QGraphicsScene  *scene, int zValue): 
                         m_parent(parent), m_scene(scene), m_anchor(0,0), m_press(false), m_rotation(0)
{
    QRectF rect = parent->sceneBoundingRect();
    Node *topLeft = new Node(Node::TopLeft, Node::Scale, rect.topLeft(), this, parent, zValue);
    Node *topRight = new Node(Node::TopRight, Node::Scale, rect.topRight(), this, parent, zValue);
    Node *bottomLeft = new Node(Node::BottomLeft,Node::Scale, rect.bottomLeft(),this, parent, zValue);
    Node *bottomRight = new Node(Node::BottomRight,Node::Scale, rect.bottomRight(),this, parent, zValue);
    Node *center = new Node(Node::Center, Node::Scale, rect.center(), this,parent, zValue);

    m_nodes.insert(Node::TopLeft, topLeft);
    m_nodes.insert(Node::TopRight, topRight);
    m_nodes.insert(Node::BottomLeft, bottomLeft);
    m_nodes.insert(Node::BottomRight, bottomRight);
    m_nodes.insert(Node::Center, center);

    m_proportional = false;
    
    beginToEdit();
}

NodeManager::~NodeManager()
{
    clear();
}

void NodeManager::clear()
{
    foreach (Node *node, m_nodes) {
             if (node) {
                 if (QGraphicsScene *scene = node->scene())
                     scene->removeItem(node);
             }
             delete node;
             node = 0;
    }
    m_nodes.clear();
}

void NodeManager::syncNodes(const QRectF &rect)
{
    if (m_nodes.isEmpty())
        return;
    
    QHash<Node::TypeNode, Node *>::iterator it = m_nodes.begin();
    while (it != m_nodes.end()) {
           if ((*it)) {
               switch (it.key()) {
                       case Node::TopLeft:
                       {
                            if ((*it)->scenePos() != rect.topLeft())
                                (*it)->setPos(rect.topLeft());
                            break;
                       }
                       case Node::TopRight:
                       {
                            if ((*it)->scenePos() != rect.topRight())
                                (*it)->setPos(rect.topRight());
                            break;
                       }
                       case Node::BottomRight:
                       {
                            if ((*it)->scenePos() != rect.bottomRight())
                                (*it)->setPos(rect.bottomRight());
                            break;
                       }
                       case Node::BottomLeft:
                       {
                            if ((*it)->scenePos() != rect.bottomLeft())
                                (*it)->setPos(rect.bottomLeft() );
                            break;
                       }
                       case Node::Center:
                       {
                            if ((*it)->scenePos() != rect.center())
                                (*it)->setPos(rect.center());
                            break;
                       }
               }
           }
           ++it;
    }
}

void NodeManager::syncNodesFromParent()
{
    if (m_parent)
        syncNodes(m_parent->sceneBoundingRect());
}

QGraphicsItem *NodeManager::parentItem() const
{
    return m_parent;
}

bool NodeManager::isModified() const
{
    return !((m_parent->matrix() == m_origMatrix) && (m_parent->pos() == m_origPos));
}

void NodeManager::beginToEdit()
{
    m_origMatrix = m_parent->matrix();
    m_origPos = m_parent->pos();
}

void NodeManager::restoreItem()
{
    m_parent->setMatrix(m_origMatrix);
    m_parent->setPos(m_origPos);
}

void NodeManager::setAnchor(const QPointF& point)
{
    m_anchor = point;
}

QPointF NodeManager::anchor() const
{
    return m_anchor;
}

void NodeManager::scale(float sx, float sy)
{
    QMatrix m;
    m.translate(m_anchor.x(),m_anchor.y());
    m.scale(sx,sy);
    m.translate(-m_anchor.x(),-m_anchor.y());
    m_parent->setMatrix(m, true);

    syncNodesFromParent();
}

void NodeManager::rotate(double a)
{
    QMatrix m = m_parent->matrix();
    m.translate(m_anchor.x(),m_anchor.y());
    m.rotate(m_rotation-a);
    m.translate(-m_anchor.x(),-m_anchor.y());
    m_parent->setMatrix(m);
    m_parent->setData(TupGraphicObject::Rotate, m_rotation - a);

    syncNodesFromParent();
    m_rotation = a;
}

void NodeManager::show()
{
    foreach (Node *node, m_nodes) {
             if (!node->scene())
                 m_scene->addItem(node);
    }
}

void NodeManager::setPress(bool press)
{
    m_press = press;
}

bool NodeManager::isPress()
{
    return m_press;
}

void NodeManager::toggleAction()
{
    foreach (Node *node, m_nodes) {
             if (node->actionNode() == Node::Scale) {
                 node->setAction(Node::Rotate);
             } else if (node->actionNode() == Node::Rotate) {
                        node->setAction(Node::Scale);
             }
    }
}

void NodeManager::setActionNode(Node::ActionNode action)
{
    foreach (Node *node, m_nodes)
             node->setAction(action);
}

void NodeManager::resizeNodes(qreal factor)
{
    foreach (Node *node, m_nodes)
             node->resize(factor);
}

void NodeManager::setVisible(bool visible)
{
    foreach (Node *node, m_nodes)
             node->setVisible(visible);
}

double NodeManager::rotation()
{
    return m_rotation;
}

void NodeManager::setProportion(bool flag)
{
    m_proportional = flag;
}

bool NodeManager::proportionalScale()
{
    return m_proportional;
}
