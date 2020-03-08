/*
 * schematicscene.cpp - implement schematic scene
 *
 * Copyright (C) 2006, Michael Margraf, michael.margraf@alumni.tu-berlin.de
 * Copyright (C) 2014, Yodalee, lc85301@gmail.com
 * Copyright (C) 2017-2019, Guilherme Brondani Torri, guitorri@gmail.com
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdlib.h>
#include <limits.h>

#include "schematicscene.h"
#include "schematicview.h"
#include "undocommands.h"
#include "components.h"
#include "qucs.h"
#include "misc.h"
#include "qt_compat.h"
#include "frame.h"

#include <QDebug>
#include <QPainter>
#include <QUndoStack>
#include <QGraphicsSceneMouseEvent>

// just dummies for empty lists
WireList      SymbolWires;
NodeList      SymbolNodes;
DiagramList   SymbolDiags;
ComponentList SymbolComps;


SchematicScene::SchematicScene(QObject *parent) :
  QGraphicsScene(parent)
{
  GridX  = GridY  = 10;

  undoStack = new QUndoStack(this);

  schematicFrame = new Frame();

  /// \todo remove deprecated
  SymbolPaints.setAutoDelete(true);

}

SchematicScene::~SchematicScene()
{
}

/*!
 * \brief SchematicScene::drawBackground of scene
 * \param painter
 * \param rect
 */
void SchematicScene::drawBackground(QPainter *painter, const QRectF &rect)
{

  // Draw origin when visible
  if(rect.contains(QPointF(0, 0))) {
    painter->drawLine(QLine(-3.0, 0.0, 3.0, 0.0));
    painter->drawLine(QLine(0.0, -3.0, 0.0, 3.0));
  }
  /// \todo getter and setter
  int GridX = 10;
  int GridY = 10;

  // Get associated view, assume single view
  SchematicView *v = static_cast<SchematicView *>(views().at(0));

  // When scaling, adjust visible grid spacing
  float scale = v->Scale;
  if(scale < 1) {
      if(scale > 0.5) {
          GridX *= 4;
          GridY *= 4;
      } else {
          GridX *= 16;
          GridY *= 16;
      }
  }

  // Grid limits
  qreal left = int(rect.left()) - (int(rect.left()) % GridX);
  qreal top = int(rect.top()) - (int(rect.top()) % GridY);
  qreal right = int(rect.right()) - (int(rect.right()) % GridX);
  qreal bottom = int(rect.bottom()) - (int(rect.bottom()) % GridY);

  // Draw grid
  painter->setBrush(Qt::NoBrush);
  for (int x=left; x<=right; x+=GridX)
      for (int y=top; y<=bottom; y+=GridY)
          painter->drawPoint(x, y);
}

void SchematicScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
  TODO("trace");
  // need to figure out a way to get the element selected for insert
  QPointF pos = event->scenePos();
  SchematicView *view = static_cast<SchematicView *>(views().at(0));
  Element *chosen = view->App->chosenElement;

  if (chosen) {
    Component *comp = static_cast<Component *>(chosen);

    // insert copy
    Component *copy = comp->newOne();
    undoStack->push( new AddItemCommand(copy, pos, this) );
  }

  // propagate to be able to drag items
  QGraphicsScene::mousePressEvent(event);
}

void SchematicScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
  QPointF pos = event->scenePos();
  SchematicView *view = static_cast<SchematicView *>(views().at(0));
  Element *chosen = view->App->chosenElement;

  if (chosen){
    if (!this->items().contains(chosen)) {
      addItem(chosen);
    }
    int x = pos.x();
    int y = pos.y();
    int gx = x;
    int gy = y;
    setOnGrid(gx, gy);
    chosen->setPos(gx,gy);
    chosen->drawScheme = true;
  }
  update();

  // propagate to reach hoverEnterEvent
  QGraphicsScene::mouseMoveEvent(event);
}

void SchematicScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
  TODO("trace");

}

void SchematicScene::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  TODO("trace");
}


// from schematic_element

/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the nodes                 *****
   *****                                                         *****
   ******************************************************************* */

// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
Node* SchematicScene::insertNode(int x, int y, Element *e)
{
    Node *pn;
    // check if new node lies upon existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())  // check every node
        if(pn->cx == x) if(pn->cy == y)
            {
                pn->Connections.append(e);
                break;
            }

    if(pn == 0)   // create new node, if no existing one lies at this position
    {
        pn = new Node(x, y);
        Nodes->append(pn);
        pn->Connections.append(e);  // connect schematic node to component node
    }
    else return pn;   // return, if node is not new

    // check if the new node lies upon an existing wire
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        if(pw->x1 == x)
        {
            if(pw->y1 > y) continue;
            if(pw->y2 < y) continue;
        }
        else if(pw->y1 == y)
        {
            if(pw->x1 > x) continue;
            if(pw->x2 < x) continue;
        }
        else continue;

        // split the wire into two wires
        splitWire(pw, pn);
        return pn;
    }

    return pn;
}

// ---------------------------------------------------
Node* SchematicScene::selectedNode(int x, int y)
{
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next()) // test nodes
        if(pn->getSelected(x, y))
            return pn;

    return 0;
}


/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the wires                 *****
   *****                                                         *****
   ******************************************************************* */

// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int SchematicScene::insertWireNode1(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) // check every node
        if(pn->cx == w->x1) if(pn->cy == w->y1) break;

    if(pn != 0)
    {
        pn->Connections.append(w);
        w->Port1 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = Wires->first(); ptr2 != 0; ptr2 = Wires->next())
    {
        if(ptr2->x1 == w->x1)
        {
            if(ptr2->y1 > w->y1) continue;
            if(ptr2->y2 < w->y1) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                if(ptr2->y2 >= w->y2)
                {
                    delete w;    // new wire lies within an existing wire
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->Port2->Connections.count() == 1)
                    {
                        w->y1 = ptr2->y1;
                        w->Port1 = ptr2->Port1;
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->pOwner = w;
                        }
                        ptr2->Port1->Connections.removeRef(ptr2);  // two -> one wire
                        ptr2->Port1->Connections.append(w);
                        Nodes->removeRef(ptr2->Port2);
                        Wires->removeRef(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->y1 = ptr2->y2;
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->Connections.append(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else if(ptr2->y1 == w->y1)
        {
            if(ptr2->x1 > w->x1) continue;
            if(ptr2->x2 < w->x1) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                if(ptr2->x2 >= w->x2)
                {
                    delete w;   // new wire lies within an existing wire
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->Port2->Connections.count() == 1)
                    {
                        w->x1 = ptr2->x1;
                        w->Port1 = ptr2->Port1;
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->pOwner = w;
                        }
                        ptr2->Port1->Connections.removeRef(ptr2); // two -> one wire
                        ptr2->Port1->Connections.append(w);
                        Nodes->removeRef(ptr2->Port2);
                        Wires->removeRef(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->x1 = ptr2->x2;
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->Connections.append(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else continue;

        pn = new Node(w->x1, w->y1);   // create new node
        Nodes->append(pn);
        pn->Connections.append(w);  // connect schematic node to the new wire
        w->Port1 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x1, w->y1);   // create new node
    Nodes->append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port1 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool SchematicScene::connectHWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->ElemType != isWire) continue;
        if(!pw->isHorizontal()) continue;
        if(pw->x1 < w->x1)
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->pOwner = w;
                w->Label->ElemType = isHWireLabel;
            }
            w->x1 = pw->x1;
            w->Port1 = pw->Port1;      // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port1->Connections.removeRef(pw);
            w->Port1->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        if(pw->x2 >= w->x2)    // new wire lies within an existing one ?
        {
            w->Port1->Connections.removeRef(w); // second node not yet made
            delete w;
            return false;
        }
        if(pw->Port2->Connections.count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port1->Connections.removeRef(pw);
            Nodes->removeRef(pw->Port2);
            Wires->removeRef(pw);
            return true;
        }
        w->x1 = pw->x2;    // shorten new wire according to an existing one
        w->Port1->Connections.removeRef(w);
        w->Port1 = pw->Port2;
        w->Port1->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool SchematicScene::connectVWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->ElemType != isWire) continue;
        if(pw->isHorizontal()) continue;
        if(pw->y1 < w->y1)
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->pOwner = w;
                w->Label->ElemType = isVWireLabel;
            }
            w->y1 = pw->y1;
            w->Port1 = pw->Port1;         // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port1->Connections.removeRef(pw);
            w->Port1->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        if(pw->y2 >= w->y2)    // new wire lies complete within an existing one ?
        {
            w->Port1->Connections.removeRef(w); // second node not yet made
            delete w;
            return false;
        }
        if(pw->Port2->Connections.count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port1->Connections.removeRef(pw);
            Nodes->removeRef(pw->Port2);
            Wires->removeRef(pw);
            return true;
        }
        w->y1 = pw->y2;    // shorten new wire according to an existing one
        w->Port1->Connections.removeRef(w);
        w->Port1 = pw->Port2;
        w->Port1->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int SchematicScene::insertWireNode2(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())  // check every node
        if(pn->cx == w->x2) if(pn->cy == w->y2) break;

    if(pn != 0)
    {
        pn->Connections.append(w);
        w->Port2 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = Wires->first(); ptr2 != 0; ptr2 = Wires->next())
    {
        if(ptr2->x1 == w->x2)
        {
            if(ptr2->y1 > w->y2) continue;
            if(ptr2->y2 < w->y2) continue;

            // (if new wire lies within an existing wire, was already check before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->Port1->Connections.count() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->pOwner = w;
                    }
                    w->y2 = ptr2->y2;
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
                    ptr2->Port2->Connections.append(w);
                    Nodes->removeRef(ptr2->Port1);
                    Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->y2 = ptr2->y1;
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->Connections.append(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else if(ptr2->y1 == w->y2)
        {
            if(ptr2->x1 > w->x2) continue;
            if(ptr2->x2 < w->x2) continue;

            // (if new wire lies within an existing wire, was already check before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->Port1->Connections.count() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->pOwner = w;
                    }
                    w->x2 = ptr2->x2;
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
                    ptr2->Port2->Connections.append(w);
                    Nodes->removeRef(ptr2->Port1);
                    Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->x2 = ptr2->x1;
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->Connections.append(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else continue;

        pn = new Node(w->x2, w->y2);   // create new node
        Nodes->append(pn);
        pn->Connections.append(w);  // connect schematic node to the new wire
        w->Port2 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x2, w->y2);   // create new node
    Nodes->append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port2 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool SchematicScene::connectHWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->ElemType != isWire) continue;
        if(!pw->isHorizontal()) continue;
        if(pw->x2 > w->x2)
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->x2 = pw->x2;
            w->Port2 = pw->Port2;      // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port2->Connections.removeRef(pw);
            w->Port2->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)

        if(pw->Port1->Connections.count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port2->Connections.removeRef(pw);
            Nodes->removeRef(pw->Port1);
            Wires->removeRef(pw);
            return true;
        }
        w->x2 = pw->x1;    // shorten new wire according to an existing one
        w->Port2->Connections.removeRef(w);
        w->Port2 = pw->Port1;
        w->Port2->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool SchematicScene::connectVWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->ElemType != isWire) continue;
        if(pw->isHorizontal()) continue;
        if(pw->y2 > w->y2)
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->y2 = pw->y2;
            w->Port2 = pw->Port2;     // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port2->Connections.removeRef(pw);
            w->Port2->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)

        if(pw->Port1->Connections.count() < 2)
        {
            // existing wire lies within the new one
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            pw->Port2->Connections.removeRef(pw);
            Nodes->removeRef(pw->Port1);
            Wires->removeRef(pw);
            return true;
        }
        w->y2 = pw->y1;    // shorten new wire according to an existing one
        w->Port2->Connections.removeRef(w);
        w->Port2 = pw->Port1;
        w->Port2->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a vertical or horizontal wire into the schematic and connects
// the ports that hit together. Returns whether the beginning and ending
// (the ports of the wire) are connected or not.
int SchematicScene::insertWire(Wire *w)
{
    int  tmp, con = 0;
    bool ok;

    // change coordinates if necessary (port 1 coordinates must be less
    // port 2 coordinates)
    if(w->x1 > w->x2)
    {
        tmp = w->x1;
        w->x1 = w->x2;
        w->x2 = tmp;
    }
    else if(w->y1 > w->y2)
    {
        tmp = w->y1;
        w->y1 = w->y2;
        w->y2 = tmp;
    }
    else con = 0x100;



    tmp = insertWireNode1(w);
    if(tmp == 0) return 3;  // no new wire and no open connection
    if(tmp > 1) con |= 2;   // insert node and remember if it remains open

    if(w->isHorizontal()) ok = connectHWires1(w);
    else ok = connectVWires1(w);
    if(!ok) return 3;




    tmp = insertWireNode2(w);
    if(tmp == 0) return 3;  // no new wire and no open connection
    if(tmp > 1) con |= 1;   // insert node and remember if it remains open

    if(w->isHorizontal()) ok = connectHWires2(w);
    else ok = connectVWires2(w);
    if(!ok) return 3;



    // change node 1 and 2
    if(con > 255) con = ((con >> 1) & 1) | ((con << 1) & 2);

    Wires->append(w);    // add wire to the schematic




    int  n1, n2;
    Wire *pw, *nw;
    Node *pn, *pn2;
    Element *pe;
    // ................................................................
    // Check if the new line covers existing nodes.
    // In order to also check new appearing wires -> use "for"-loop
    for(pw = Wires->current(); pw != 0; pw = Wires->next())
        for(pn = Nodes->first(); pn != 0; )    // check every node
        {
            if(pn->cx == pw->x1)
            {
                if(pn->cy <= pw->y1)
                {
                    pn = Nodes->next();
                    continue;
                }
                if(pn->cy >= pw->y2)
                {
                    pn = Nodes->next();
                    continue;
                }
            }
            else if(pn->cy == pw->y1)
            {
                if(pn->cx <= pw->x1)
                {
                    pn = Nodes->next();
                    continue;
                }
                if(pn->cx >= pw->x2)
                {
                    pn = Nodes->next();
                    continue;
                }
            }
            else
            {
                pn = Nodes->next();
                continue;
            }

            n1 = 2;
            n2 = 3;
            pn2 = pn;
            // check all connections of the current node
            for(pe=pn->Connections.first(); pe!=0; pe=pn->Connections.next())
            {
                if(pe->ElemType != isWire) continue;
                nw = (Wire*)pe;
                // wire lies within the new ?
                if(pw->isHorizontal() != nw->isHorizontal()) continue;

                pn  = nw->Port1;
                pn2 = nw->Port2;
                n1  = pn->Connections.count();
                n2  = pn2->Connections.count();
                if(n1 == 1)
                {
                    Nodes->removeRef(pn);     // delete node 1 if open
                    pn2->Connections.removeRef(nw);   // remove connection
                    pn = pn2;
                }

                if(n2 == 1)
                {
                    pn->Connections.removeRef(nw);   // remove connection
                    Nodes->removeRef(pn2);     // delete node 2 if open
                    pn2 = pn;
                }

                if(pn == pn2)
                {
                    if(nw->Label)
                    {
                        pw->Label = nw->Label;
                        pw->Label->pOwner = pw;
                    }
                    Wires->removeRef(nw);    // delete wire
                    Wires->findRef(pw);      // set back to current wire
                }
                break;
            }
            if(n1 == 1) if(n2 == 1) continue;

            // split wire into two wires
            if((pw->x1 != pn->cx) || (pw->y1 != pn->cy))
            {
                nw = new Wire(pw->x1, pw->y1, pn->cx, pn->cy, pw->Port1, pn);
                pn->Connections.append(nw);
                Wires->append(nw);
                Wires->findRef(pw);
                pw->Port1->Connections.append(nw);
            }
            pw->Port1->Connections.removeRef(pw);
            pw->x1 = pn2->cx;
            pw->y1 = pn2->cy;
            pw->Port1 = pn2;
            pn2->Connections.append(pw);

            pn = Nodes->next();
        }

    if (Wires->containsRef (w))  // if two wire lines with different labels ...
        oneLabel(w->Port1);       // ... are connected, delete one label
    return con | 0x0200;   // sent also end flag
}

// ---------------------------------------------------
// Follows a wire line and selects it.
void SchematicScene::selectWireLine(Element *pe, Node *pn, bool ctrl)
{
    Node *pn_1st = pn;
    while(pn->Connections.count() == 2)
    {
        if(pn->Connections.first() == pe)  pe = pn->Connections.last();
        else  pe = pn->Connections.first();

        if(pe->ElemType != isWire) break;
        if(ctrl) pe->ElemSelected ^= ctrl;
        else pe->ElemSelected = true;

        if(((Wire*)pe)->Port1 == pn)  pn = ((Wire*)pe)->Port2;
        else  pn = ((Wire*)pe)->Port1;
        if(pn == pn_1st) break;  // avoid endless loop in wire loops
    }
}

// ---------------------------------------------------
Wire* SchematicScene::selectedWire(int x, int y)
{
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
        if(pw->getSelected(x, y))
            return pw;

    return 0;
}

// ---------------------------------------------------
// Splits the wire "*pw" into two pieces by the node "*pn".
Wire* SchematicScene::splitWire(Wire *pw, Node *pn)
{
    Wire *newWire = new Wire(pn->cx, pn->cy, pw->x2, pw->y2, pn, pw->Port2);
    newWire->ElemSelected = pw->ElemSelected;

    pw->x2 = pn->cx;
    pw->y2 = pn->cy;
    pw->Port2 = pn;

    newWire->Port2->Connections.prepend(newWire);
    pn->Connections.prepend(pw);
    pn->Connections.prepend(newWire);
    newWire->Port2->Connections.removeRef(pw);
    Wires->append(newWire);

    if(pw->Label)
        if((pw->Label->cx > pn->cx) || (pw->Label->cy > pn->cy))
        {
            newWire->Label = pw->Label;   // label goes to the new wire
            pw->Label = 0;
            newWire->Label->pOwner = newWire;
        }

    return newWire;
}

// ---------------------------------------------------
// If possible, make one wire out of two wires.
bool SchematicScene::oneTwoWires(Node *n)
{
    Wire *e3;
    Wire *e1 = (Wire*)n->Connections.getFirst();  // two wires -> one wire
    Wire *e2 = (Wire*)n->Connections.getLast();

    if(e1->ElemType == isWire) if(e2->ElemType == isWire)
            if(e1->isHorizontal() == e2->isHorizontal())
            {
                if(e1->x1 == e2->x2) if(e1->y1 == e2->y2)
                    {
                        e3 = e1;
                        e1 = e2;
                        e2 = e3;    // e1 must have lesser coordinates
                    }
                if(e2->Label)     // take over the node name label ?
                {
                    e1->Label = e2->Label;
                    e1->Label->pOwner = e1;
                }
                else if(n->Label)
                {
                    e1->Label = n->Label;
                    e1->Label->pOwner = e1;
                    if(e1->isHorizontal())
                        e1->Label->ElemType = isHWireLabel;
                    else
                        e1->Label->ElemType = isVWireLabel;
                }

                e1->x2 = e2->x2;
                e1->y2 = e2->y2;
                e1->Port2 = e2->Port2;
                Nodes->removeRef(n);    // delete node (is auto delete)
                e1->Port2->Connections.removeRef(e2);
                e1->Port2->Connections.append(e1);
                Wires->removeRef(e2);
                return true;
            }
    return false;
}

// ---------------------------------------------------
// Deletes the wire 'w'.
void SchematicScene::deleteWire(Wire *w)
{
    if(w->Port1->Connections.count() == 1)
    {
        if(w->Port1->Label) delete w->Port1->Label;
        Nodes->removeRef(w->Port1);     // delete node 1 if open
    }
    else
    {
        w->Port1->Connections.removeRef(w);   // remove connection
        if(w->Port1->Connections.count() == 2)
            oneTwoWires(w->Port1);  // two wires -> one wire
    }

    if(w->Port2->Connections.count() == 1)
    {
        if(w->Port2->Label) delete w->Port2->Label;
        Nodes->removeRef(w->Port2);     // delete node 2 if open
    }
    else
    {
        w->Port2->Connections.removeRef(w);   // remove connection
        if(w->Port2->Connections.count() == 2)
            oneTwoWires(w->Port2);  // two wires -> one wire
    }

    if(w->Label)
    {
        delete w->Label;
        w->Label = 0;
    }
    Wires->removeRef(w);
}

// ---------------------------------------------------
int SchematicScene::copyWires(int& x1, int& y1, int& x2, int& y2,
                         QList<Element *> *ElementCache)
{
    int count=0;
    Node *pn;
    Wire *pw;
    WireLabel *pl;
    for(pw = Wires->first(); pw != 0; )  // find bounds of all selected wires
        if(pw->ElemSelected)
        {
            if(pw->x1 < x1) x1 = pw->x1;
            if(pw->x2 > x2) x2 = pw->x2;
            if(pw->y1 < y1) y1 = pw->y1;
            if(pw->y2 > y2) y2 = pw->y2;

            count++;
            ElementCache->append(pw);

            // rescue non-selected node labels
            pn = pw->Port1;
            if(pn->Label)
                if(pn->Connections.count() < 2)
                {
                    ElementCache->append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->pOwner = (Node*)pw;
                    pn->Label = 0;
                }
            pn = pw->Port2;
            if(pn->Label)
                if(pn->Connections.count() < 2)
                {
                    ElementCache->append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->pOwner = (Node*)pw;
                    pn->Label = 0;
                }

            pl = pw->Label;
            pw->Label = 0;
            deleteWire(pw);
            pw->Label = pl;    // restore wire label
            pw = Wires->current();
        }
        else pw = Wires->next();

    return count;
}


/* *******************************************************************
   *****                                                         *****
   *****                  Actions with markers                   *****
   *****                                                         *****
   ******************************************************************* */

Marker* SchematicScene::setMarker(int x, int y)
{
  // only diagrams ...
  foreach (auto const pd, filterItems<Diagram>(this)) {
    if(Marker* m=pd->setMarker(x,y)){
      TODO("callback to set changed");
      //setChanged(true, true);
      return m;
    }
  }
  return NULL;
}

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
void SchematicScene::markerLeftRight(bool left, Q3PtrList<Element> *Elements)
{
    Marker *pm;
    bool acted = false;
    for(pm = (Marker*)Elements->first(); pm!=0; pm = (Marker*)Elements->next())
    {
        if(pm->moveLeftRight(left))
            acted = true;
    }

    if(acted) {
        TODO("callback to set changed");
        //setChanged(true, true, 'm');
    }
}

// ---------------------------------------------------
// Moves the marker pointer up/down on the more-dimensional graph.
void SchematicScene::markerUpDown(bool up, Q3PtrList<Element> *Elements)
{
    Marker *pm;
    bool acted = false;
    for(pm = (Marker*)Elements->first(); pm!=0; pm = (Marker*)Elements->next())
    {
        if(pm->moveUpDown(up))
            acted = true;
    }

    if(acted) {
        TODO("callback to set changed");
        //setChanged(true, true, 'm');
    }
}


/* *******************************************************************
   *****                                                         *****
   *****               Actions with all elements                 *****
   *****                                                         *****
   ******************************************************************* */


void SchematicScene::highlightWireLabels ()
{
    WireLabel *pltestinner = 0;
    WireLabel *pltestouter = 0;

    // First set highlighting for all wire and nodes labels to false
    for(Wire *pwouter = Wires->last(); pwouter != 0; pwouter = Wires->prev())
    {
        pltestouter = pwouter->Label; // test any label associated with the wire
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    for(Node *pnouter = Nodes->last(); pnouter != 0; pnouter = Nodes->prev())
    {
        pltestouter = pnouter->Label; // test any label associated with the node
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    // Then test every wire's label to see if we need to highlight it
    // and matching labels on wires and nodes
    for(auto it=Wires->begin(); it!=Wires->end(); ++it) {
	Wire* pwouter = *it;

        // get any label associated with the wire
        pltestouter = pwouter->Label;
        if (pltestouter)
        {
            if (pltestouter->ElemSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
		for(auto iit=Wires->begin(); iit!=Wires->end(); ++iit) {
		    Wire* pwinner = *iit;

                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one wire has this label, do not highlight it
                        if (pltestinner != pltestouter)
                        {
                            if (pltestouter->Name == pltestinner->Name)
                            {
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }
                        }
                    }
                }
                // Search for matching labels on nodes

		for(auto iit=Nodes->begin(); iit!=Nodes->end(); ++iit) {
		    Node* pninner = *iit;

                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        if (pltestouter->Name == pltestinner->Name)
                        {
                            // node label matches wire label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
    // Same as above but for nodes labels:
    // test every node label to see if we need to highlight it
    // and matching labels on wires and nodes
    for(auto iit=Nodes->begin(); iit!=Nodes->end(); ++iit) {
	Node* pnouter = *iit;

        // get any label associated with the node
        pltestouter = pnouter->Label;
        if (pltestouter)
        {
            if (pltestouter->ElemSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
		for(auto iit=Wires->begin(); iit!=Wires->end(); ++iit) {
		    Wire* pwinner = *iit;

                    pltestinner = pwinner->Label; // test any label associated with the wire
                    if (pltestinner)
                    {
                        if (pltestouter->Name == pltestinner->Name)
                        {
                            // wire label matches node label
                            pltestinner->setHighlighted (true);
                            hiLightOuter = true;
                        }
                    }
                }
                // Search for matching labels on nodes
		for(auto iit=Nodes->begin(); iit!=Nodes->end(); ++iit) {
		    Node* pninner = *iit;

                    pltestinner = pninner->Label; // test any label associated with the node
                    if (pltestinner)
                    {
                        // Highlight the label if it has the same name as the selected label
                        // if only one node has this label, do not highlight it
                        if (pltestinner != pltestouter)
                        {
                            if (pltestouter->Name == pltestinner->Name)
                            {
                                pltestinner->setHighlighted (true);
                                hiLightOuter = true;
                            }
                        }
                    }
                }
                // highlight if at least two different wires/nodes with the same label found
                pltestouter->setHighlighted (hiLightOuter);
            }
        }
    }
}

void SchematicScene::drawBias(QPainter *painter, const QRectF &rect)
{
    if(showBias > 0) {  // show DC bias points in schematic ?
      int x, y, z;
      for(Node* pn = Nodes->first(); pn != 0; pn = Nodes->next()) {
        if(pn->Name.isEmpty()) continue;
        x = pn->cx;
        y = pn->cy + 4;
        z = pn->x1;
        if(z & 1) x -= painter->fontMetrics().width(pn->Name);
        if(!(z & 2)) {
          /// \todo fix linespacing
          //y -= (p->LineSpacing>>1) + 4;
          y -= 4;
          if(z & 1) x -= 4;
          else x += 4;
        }
        if(z & 0x10)
          painter->setPen(Qt::darkGreen);  // green for currents
        else
          painter->setPen(Qt::blue);   // blue for voltages

        painter->drawText(x, y, pn->Name);
      }
    }
}

// ---------------------------------------------------
// Deselects all elements except 'e'.
void SchematicScene::deselectElements(Element *e)
{
    // test all components
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
        if(e != pc)  pc->ElemSelected = false;

    // test all wires
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        if(e != pw)  pw->ElemSelected = false;
        if(pw->Label) if(pw->Label != e)  pw->Label->ElemSelected = false;
    }

    // test all node labels
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
        if(pn->Label) if(pn->Label != e)  pn->Label->ElemSelected = false;

    // test all diagrams
    foreach (auto pd, filterItems<Diagram>(this)) {
        if(e != pd)  pd->ElemSelected = false;

        // test graphs of diagram
        foreach(Graph *pg, pd->Graphs)
        {
            if(e != pg) pg->ElemSelected = false;

            // test markers of graph
            foreach(Marker *pm, pg->Markers)
                if(e != pm) pm->ElemSelected = false;
        }

    }

    // test all paintings
    foreach (auto pp, filterItems<Painting>(this))
        if(e != pp)  pp->ElemSelected = false;
}

// ---------------------------------------------------
// Selects elements that lie within the rectangle x1/y1, x2/y2.
int SchematicScene::selectElements(int x1, int y1, int x2, int y2, bool flag)
{
    int  z=0;   // counts selected elements
    int  cx1, cy1, cx2, cy2;

    // exchange rectangle coordinates to obtain x1 < x2 and y1 < y2
    cx1 = (x1 < x2) ? x1 : x2;
    cx2 = (x1 > x2) ? x1 : x2;
    cy1 = (y1 < y2) ? y1 : y2;
    cy2 = (y1 > y2) ? y1 : y2;
    x1 = cx1;
    x2 = cx2;
    y1 = cy1;
    y2 = cy2;

    // test all components
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pc->ElemSelected = true;
                        z++;
                        continue;
                    }
        if(pc->ElemSelected &= flag) z++;
    }


    Wire *pw;
    for(pw = Wires->first(); pw != 0; pw = Wires->next())   // test all wires
    {
        if(pw->x1 >= x1) if(pw->x2 <= x2) if(pw->y1 >= y1) if(pw->y2 <= y2)
                    {
                        pw->ElemSelected = true;
                        z++;
                        continue;
                    }
        if(pw->ElemSelected &= flag) z++;
    }


    // test all wire labels *********************************
    WireLabel *pl=0;
    for(pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        if(pw->Label)
        {
            pl = pw->Label;
            if(pl->x1 >= x1) if((pl->x1+pl->x2) <= x2)
                    if(pl->y1 >= y1) if((pl->y1+pl->y2) <= y2)
                        {
                            pl->ElemSelected = true;
                            z++;
                            continue;
                        }
            if(pl->ElemSelected &= flag) z++;
        }
    }


    // test all node labels *************************************
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    {
        pl = pn->Label;
        if(pl)
        {
            if(pl->x1 >= x1) if((pl->x1+pl->x2) <= x2)
                    if((pl->y1-pl->y2) >= y1) if(pl->y1 <= y2)
                        {
                            pl->ElemSelected = true;
                            z++;
                            continue;
                        }
            if(pl->ElemSelected &= flag) z++;
        }
    }


    // test all diagrams *******************************************
    foreach (auto const pd, filterItems<Diagram>(this)) {
        // test graphs of diagram
        foreach(Graph *pg, pd->Graphs)
        {
            if(pg->ElemSelected &= flag) z++;

            // test markers of graph
            foreach(Marker *pm, pg->Markers)
            {
                pm->Bounding(cx1, cy1, cx2, cy2);
                if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                            {
                                pm->ElemSelected = true;
                                z++;
                                continue;
                            }
                if(pm->ElemSelected &= flag) z++;
            }
        }

        // test diagram itself
        pd->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pd->ElemSelected = true;
                        z++;
                        continue;
                    }
        if(pd->ElemSelected &= flag) z++;
    }

    // test all paintings *******************************************
    foreach (auto pp, filterItems<Painting>(this)) {
        pp->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pp->ElemSelected = true;
                        z++;
                        continue;
                    }
        if(pp->ElemSelected &= flag) z++;
    }

    return z;
}

// ---------------------------------------------------
// Selects all markers.
void SchematicScene::selectMarkers()
{
    foreach (auto const pd, filterItems<Diagram>(this))
        foreach(Graph *pg, pd->Graphs)
            foreach(Marker *pm, pg->Markers)
                pm->ElemSelected = true;
}

// ---------------------------------------------------
// For moving elements: If the moving element is connected to a not
// moving element, insert two wires. If the connected element is already
// a wire, use this wire. Otherwise create new wire.
void SchematicScene::newMovingWires(Q3PtrList<Element> *p, Node *pn, int pos)
{
    Element *pe;

    if(pn->State & 8)  // Were new wires already inserted ?
        return;
    pn->State |= 8;

    for (;;)
    {
        if(pn->State & 16)  // node was already worked on
            break;

        pe = pn->Connections.getFirst();
        if(pe == 0)  return;

        if(pn->Connections.count() > 1)
            break;
        if(pe->ElemType != isWire)  // is it connected to exactly one wire ?
            break;

        // .................................................
        long  mask = 1, invMask = 3;
        Wire *pw2=0, *pw = (Wire*)pe;

        Node *pn2 = pw->Port1;
        if(pn2 == pn) pn2 = pw->Port2;

        if(pn2->Connections.count() == 2) // two existing wires connected ?
            if((pn2->State & (8+4)) == 0)
            {
                Element *pe2 = pn2->Connections.getFirst();
                if(pe2 == pe) pe2 = pn2->Connections.getLast();
                // connected wire connected to exactly one wire ?
                if(pe2->ElemType == isWire)
                    pw2  = (Wire*)pe2;
            }

        // .................................................
        // reuse one wire
        p->insert(pos, pw);
        pw->Port1->Connections.removeRef(pw);   // remove connection 1
        pw->Port1->State |= 16+4;
        pw->Port2->Connections.removeRef(pw);   // remove connection 2
        pw->Port2->State |= 16+4;
        Wires->take(Wires->findRef(pw));

        if(pw->isHorizontal()) mask = 2;

        if(pw2 == 0)    // place new wire between component and old wire
        {
            pn = pn2;
            mask ^= 3;
            invMask = 0;
        }

        if(pw->Port1 != pn)
        {
            pw->Port1->State |= mask;
            pw->Port1 = (Node*)(uintptr_t)mask;
            pw->Port2->State |= invMask;
            pw->Port2 = (Node*)(uintptr_t)invMask;  // move port 2 completely
        }
        else
        {
            pw->Port1->State |= invMask;
            pw->Port1 = (Node*)(uintptr_t)invMask;
            pw->Port2->State |= mask;
            pw->Port2 = (Node*)(uintptr_t)mask;
        }

        invMask ^= 3;
        // .................................................
        // create new wire ?
        if(pw2 == 0)
        {
            if(pw->Port1 != (Node*)(uintptr_t)mask)
                p->insert(pos,
                          new Wire(pw->x2, pw->y2, pw->x2, pw->y2, (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            else
                p->insert(pos,
                          new Wire(pw->x1, pw->y1, pw->x1, pw->y1, (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            return;
        }


        // .................................................
        // reuse a second wire
        p->insert(pos, pw2);
        pw2->Port1->Connections.removeRef(pw2);   // remove connection 1
        pw2->Port1->State |= 16+4;
        pw2->Port2->Connections.removeRef(pw2);   // remove connection 2
        pw2->Port2->State |= 16+4;
        Wires->take(Wires->findRef(pw2));

        if(pw2->Port1 != pn2)
        {
            pw2->Port1 = (Node*)0;
            pw2->Port2->State |= mask;
            pw2->Port2 = (Node*)(uintptr_t)mask;
        }
        else
        {
            pw2->Port1->State |= mask;
            pw2->Port1 = (Node*)(uintptr_t)mask;
            pw2->Port2 = (Node*)0;
        }
        return;
    }

    // only x2 moving
    p->insert(pos, new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)0, (Node*)1));
    // x1, x2, y2 moving
    p->insert(pos, new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)1, (Node*)3));
}

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
// BUG: does not (only) copy, as the name suggests.
//      cannot be used to make copies.
// returns the number of "copied" _Markers_ only
int SchematicScene::copySelectedElements(Q3PtrList<Element> *p)
{
    int i, count = 0;
    Component *pc;
    Wire      *pw;
    Diagram   *pd;
    Element   *pe;
    Node      *pn;


    // test all components *********************************
    // Insert components before wires in order to prevent short-cut removal.
    for(pc = Components->first(); pc != 0; )
        if(pc->ElemSelected)
        {
            p->append(pc);
            count++;

            // delete all port connections
            foreach(Port *pp, pc->Ports)
            {
                pp->Connection->Connections.removeRef((Element*)pc);
                pp->Connection->State = 4;
            }

            Components->take();   // take component out of the document
            pc = Components->current();
        }
        else pc = Components->next();

    // test all wires and wire labels ***********************
    for(pw = Wires->first(); pw != 0; )
    {
        if(pw->Label) if(pw->Label->ElemSelected)
                p->append(pw->Label);

        if(pw->ElemSelected)
        {
            p->append(pw);

            pw->Port1->Connections.removeRef(pw);   // remove connection 1
            pw->Port1->State = 4;
            pw->Port2->Connections.removeRef(pw);   // remove connection 2
            pw->Port2->State = 4;
            Wires->take();
            pw = Wires->current();
        }
        else pw = Wires->next();
    }

    // ..............................................
    // Inserts wires, if a connection to a not moving element is found.
    // The order of the "for"-loops is important to guarantee a stable
    // operation: components, new wires, old wires
    pc = (Component*)p->first();
    for(i=0; i<count; i++)
    {
        foreach(Port *pp, pc->Ports)
            newMovingWires(p, pp->Connection, count);

        p->findRef(pc);   // back to the real current pointer
        pc = (Component*)p->next();
    }

    for(pe = (Element*)pc; pe != 0; pe = p->next())  // new wires
        if(pe->ElemSelected)
            break;

    for(pw = (Wire*)pe; pw != 0; pw = (Wire*)p->next())
        if(pw->ElemType == isWire)    // not working on labels
        {
            newMovingWires(p, pw->Port1, count);
            newMovingWires(p, pw->Port2, count);
            p->findRef(pw);   // back to the real current pointer
        }


    // ..............................................
    // delete the unused nodes
    for(pn = Nodes->first(); pn!=0; )
    {
        if(pn->State & 8)
            if(pn->Connections.count() == 2)
                if(oneTwoWires(pn))    // if possible, connect two wires to one
                {
                    pn = Nodes->current();
                    continue;
                }

        if(pn->Connections.count() == 0)
        {
            if(pn->Label)
            {
                pn->Label->ElemType = isMovingLabel;
                if(pn->State & 1)
                {
                    if(!(pn->State & 2)) pn->Label->ElemType = isHMovingLabel;
                }
                else if(pn->State & 2) pn->Label->ElemType = isVMovingLabel;
                p->append(pn->Label);    // do not forget the node labels
            }
            Nodes->remove();
            pn = Nodes->current();
            continue;
        }

        pn->State = 0;
        pn = Nodes->next();
    }

    // test all node labels
    // do this last to avoid double copying
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())
        if(pn->Label) if(pn->Label->ElemSelected)
                p->append(pn->Label);


    // test all paintings **********************************
    for(Painting *ppa = Paintings->first(); ppa != 0; )
        if(ppa->ElemSelected)
        {
            p->append(ppa);
            Paintings->take();
            ppa = Paintings->current();
        }
        else ppa = Paintings->next();

    count = 0;  // count markers now
    // test all diagrams **********************************
    for(pd = Diagrams->first(); pd != 0; )
        if(pd->ElemSelected)
        {
            p->append(pd);
            Diagrams->take();
            pd = Diagrams->current();
        }
        else
        {
            foreach(Graph *pg, pd->Graphs)
            {
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->ElemSelected)
                    {
                        count++;
                        p->append(pm);
                    }
                }
            }

            pd = Diagrams->next();
        }

    return count;
}

// ---------------------------------------------------
bool SchematicScene::copyComps2WiresPaints(int& x1, int& y1, int& x2, int& y2,
                                      QList<Element *> *ElementCache)
{
    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    copyLabels(x1, y1, x2, y2, ElementCache);   // must be first of all !
    copyComponents2(x1, y1, x2, y2, ElementCache);
    copyWires(x1, y1, x2, y2, ElementCache);
    copyPaintings(x1, y1, x2, y2, ElementCache);

    if(y1 == INT_MAX) return false;  // no element selected
    return true;
}

// ---------------------------------------------------
// Used in "aligning()", "distributeHorizontal()", "distributeVertical()".
int SchematicScene::copyElements(int& x1, int& y1, int& x2, int& y2,
                            QList<Element *> *ElementCache)
{
    int bx1, by1, bx2, by2;
    Wires->setAutoDelete(false);
    Components->setAutoDelete(false);

    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    // take components and wires out of list, check their boundings
    int number = copyComponents(x1, y1, x2, y2, ElementCache);
    number += copyWires(x1, y1, x2, y2, ElementCache);

    Wires->setAutoDelete(true);
    Components->setAutoDelete(true);

    // find upper most selected diagram
    for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev())
        if(pd->ElemSelected)
        {
            pd->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache->append(pd);
            number++;
        }
    // find upper most selected painting
    for(Painting *pp = Paintings->last(); pp != 0; pp = Paintings->prev())
        if(pp->ElemSelected)
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache->append(pp);
            number++;
        }

    return number;
}

// ---------------------------------------------------
// Deletes all selected elements.
bool SchematicScene::deleteElements()
{
    bool sel = false;

    Component *pc = Components->first();
    while(pc != 0)      // all selected component
        if(pc->ElemSelected)
        {
            deleteComp(pc);
            pc = Components->current();
            sel = true;
        }
        else pc = Components->next();

    Wire *pw = Wires->first();
    while(pw != 0)        // all selected wires and their labels
    {
        if(pw->Label)
            if(pw->Label->ElemSelected)
            {
                delete pw->Label;
                pw->Label = 0;
                sel = true;
            }

        if(pw->ElemSelected)
        {
            deleteWire(pw);
            pw = Wires->current();
            sel = true;
        }
        else pw = Wires->next();
    }

    // all selected labels on nodes ***************************
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
        if(pn->Label)
            if(pn->Label->ElemSelected)
            {
                delete pn->Label;
                pn->Label = 0;
                sel = true;
            }

    Diagram *pd = Diagrams->first();
    while(pd != 0)      // test all diagrams
        if(pd->ElemSelected)
        {
            Diagrams->remove();
            pd = Diagrams->current();
            sel = true;
        }
        else
        {
            bool wasGraphDeleted = false;
            // all graphs of diagram

            QMutableListIterator<Graph *> ig(pd->Graphs);
            Graph *pg;

            while (ig.hasNext())
            {
                pg = ig.next();
                // all markers of diagram
                QMutableListIterator<Marker *> im(pg->Markers);
                Marker *pm;
                while (im.hasNext())
                {
                    pm = im.next();
                    if(pm->ElemSelected)
                    {
                        im.remove();
                        sel = true;
                    }
                }

                if(pg->ElemSelected)
                {
                    ig.remove();
                    sel = wasGraphDeleted = true;
                }
            }
            if(wasGraphDeleted)
                pd->recalcGraphData();  // update diagram (resize etc.)

            pd = Diagrams->next();
        } //else


    Painting *pp = Paintings->first();
    while(pp != 0)      // test all paintings
    {
        if(pp->ElemSelected)
            if(pp->Name.at(0) != '.')    // do not delete "PortSym", "ID_text"
            {
                sel = true;
                Paintings->remove();
                pp = Paintings->current();
                continue;
            }
        pp = Paintings->next();
    }

    if(sel)
    {
        TODO("callback to set sizeofAll and setchanged");
        // equivalent to fit() and update()?
        //sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);   // set new document size
        //setChanged(sel, true);
    }
    return sel;
}

// ---------------------------------------------------
/*!
 * \brief Schematic::aligning align selected elements.
 * \param Mode: top, bottom, left, right, center vertical, center horizontal
 * \return True if aligned
 */
bool SchematicScene::aligning(int Mode)
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2, *bx=0, *by=0, *ax=0, *ay=0;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;


    ax = ay = &x2;  // = 0
    switch(Mode)
    {
    case 0:  // align top
        bx = &x1;
        by = &by1;
        y2 = 1;
        break;
    case 1:  // align bottom
        bx = &x1;
        y1 = y2;
        by = &by2;
        y2 = 1;
        break;
    case 2:  // align left
        by = &y1;
        bx = &bx1;
        y2 = 1;
        break;
    case 3:  // align right
        by = &y1;
        x1 = x2;
        bx = &bx2;
        y2 = 1;
        break;
    case 4:  // center horizontally
        x1 = (x2+x1) / 2;
        by = &x2;  // = 0
        ax = &bx1;
        bx = &bx2;
        y1 = 0;
        y2 = 2;
        break;
    case 5:  // center vertically
        y1 = (y2+y1) / 2;
        bx = &x2;  // = 0
        ay = &by1;
        by = &by2;
        x1 = 0;
        y2 = 2;
        break;
    }
    x2 = 0;

    Wire      *pw;
    Component *pc;
    Element   *pe;
    // re-insert elements
    // Go backwards in order to insert node labels before its component.
    QListIterator<Element *> elementCacheIter(ElementCache);
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious()) {
        pe = elementCacheIter.previous();
        switch(pe->ElemType)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pc = (Component*)pe;
            pc->Bounding(bx1, by1, bx2, by2);
            pc->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            insertRawComponent(pc);
            break;

        case isWire:
            pw = (Wire*)pe;
            bx1 = pw->x1;
            by1 = pw->y1;
            bx2 = pw->x2;
            by2 = pw->y2;
            pw->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
//        if(pw->Label) {  }
            insertWire(pw);
            break;

        case isDiagram:
            // Should the axis label be counted for ? I guess everyone
            // has a different opinion.
//        ((Diagram*)pe)->Bounding(bx1, by1, bx2, by2);

            // Take size without axis label.
            bx1 = ((Diagram*)pe)->cx;
            by2 = ((Diagram*)pe)->cy;
            bx2 = bx1 + ((Diagram*)pe)->x2;
            by1 = by2 - ((Diagram*)pe)->y2;
            ((Diagram*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isPainting:
            ((Painting*)pe)->Bounding(bx1, by1, bx2, by2);
            ((Painting*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->ElemType & isComponent)
            {
                pc = (Component*)(((WireLabel*)pe)->pOwner);
                pc->Bounding(bx1, by1, bx2, by2);
            }
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                bx1 = pw->x1;
                by1 = pw->y1;
                bx2 = pw->x2;
                by2 = pw->y2;
            }
            ((WireLabel*)pe)->cx += x1-((*bx)+(*ax))/y2;
            ((WireLabel*)pe)->cy += y1-((*by)+(*ay))/y2;
            insertNodeLabel((WireLabel*)pe);
            break;

        default:
            ;
        }
    }

    ElementCache.clear();
    if(count < 2) return false;

    TODO("callback to set changed");
    //setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
bool SchematicScene::distributeHorizontal()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    Element *pe;
    WireLabel *pl;
    // Node labels are not counted for, so put them to the end.
    /*  for(pe = ElementCache.last(); pe != 0; pe = ElementCache.prev())
        if(pe->Type == isNodeLabel) {
          ElementCache.append(pe);
          ElementCache.removeRef(pe);
        }*/

    // using bubble sort to get elements x ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe=elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(bx1 > bx2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    Wire *pw;
    int x = x2;
    int dx=0;
    if(count > 1) dx = (x2-x1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        switch(pe->ElemType)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->cx = x;
            insertRawComponent((Component*)pe);
            break;

        case isWire:
            pw = (Wire*)pe;
            if(pw->isHorizontal())
            {
                x1 = pw->x2 - pw->x1;
                pw->x1 = x - (x1 >> 1);
                pw->x2 = pw->x1 + x1;
            }
            else  pw->x1 = pw->x2 = x;
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pe->cx = x - (pe->x2 >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(x, by1, false);
            break;

        case isNodeLabel:
            pl = (WireLabel*)pe;
            if(((Element*)(pl->pOwner))->ElemType & isComponent)
                pe->cx += x - ((Component*)(pl->pOwner))->cx;
            else
            {
                pw = (Wire*)(pl->pOwner);
                if(pw->isHorizontal())
                {
                    x1 = pw->x2 - pw->x1;
                    pe->cx += x - (x1 >> 1) - pw->x1;
                }
                else  pe->cx += x - pw->x1;
            }
            insertNodeLabel(pl);
            x += dx;
            break;

        default:
            ;
        }
        x -= dx;
    }

    ElementCache.clear();
    if(count < 2) return false;

    TODO("callback to set changed");
    //setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeVertical sort selection vertically.
 * \return True if sorted
 */
bool SchematicScene::distributeVertical()
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;

    // using bubble sort to get elements y ordered
    QListIterator<Element *> elementCacheIter(ElementCache);
    Element *pe;
    if(count > 1)
        for(int i = count-1; i>0; i--)
        {
            pe = ElementCache.first();
            for(int j=0; j<i; j++)
            {
                pe->getCenter(bx1, by1);
                pe = elementCacheIter.peekNext();
                pe->getCenter(bx2, by2);
                if(by1 > by2)    // change two elements ?
                {
                    ElementCache.replace(j+1, elementCacheIter.peekPrevious());
                    ElementCache.replace(j, pe);
                    pe = elementCacheIter.next();
                }
            }
        }

    ElementCache.last()->getCenter(x2, y2);
    ElementCache.first()->getCenter(x1, y1);
    Wire *pw;
    int y  = y2;
    int dy=0;
    if(count > 1) dy = (y2-y1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        switch(pe->ElemType)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->cy = y;
            insertRawComponent((Component*)pe);
            break;

        case isWire:
            pw = (Wire*)pe;
            if(pw->isHorizontal())  pw->y1 = pw->y2 = y;
            else
            {
                y1 = pw->y2 - pw->y1;
                pw->y1 = y - (y1 >> 1);
                pw->y2 = pe->y1 + y1;
            }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pe->cy = y + (pe->y2 >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(bx1, y, false);
            break;

        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->ElemType & isComponent)
                pe->cy += y - ((Component*)(((WireLabel*)pe)->pOwner))->cy;
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                if(!pw->isHorizontal())
                {
                    y1 = pw->y2 - pw->y1;
                    pe->cy += y - (y1 >> 1) - pw->y1;
                }
                else  pe->cy += y - pw->y1;
            }
            insertNodeLabel((WireLabel*)pe);
            y += dy;
            break;

        default:
            ;
        }
        y -= dy;
    }

    ElementCache.clear();
    if(count < 2) return false;

    TODO("callback to set changed");
    //setChanged(true, true);
    return true;
}


/* *******************************************************************
   *****                                                         *****
   *****                Actions with components                  *****
   *****                                                         *****
   ******************************************************************* */

// Finds the correct number for power sources, subcircuit ports and
// digital sources and sets them accordingly.
void SchematicScene::setComponentNumber(Component *c)
{
    Property *pp = c->Props.getFirst();
    if(!pp) return;
    if(pp->Name != "Num") return;

    int n=1;
    QString s = pp->Value;
    QString cSign = c->obsolete_model_hack();
    Component *pc;
    // First look, if the port number already exists.
    for(pc = Components->first(); pc != 0; pc = Components->next())
        if(pc->obsolete_model_hack() == cSign)
            if(pc->Props.getFirst()->Value == s) break;
    if(!pc) return;   // was port number not yet in use ?

    // Find the first free number.
    do
    {
        s  = QString::number(n);
        // look for existing ports and their numbers
        for(pc = Components->first(); pc != 0; pc = Components->next())
            if(pc->obsolete_model_hack() == cSign)
                if(pc->Props.getFirst()->Value == s) break;

        n++;
    }
    while(pc);     // found not used component number
    pp->Value = s; // set new number
}

// ---------------------------------------------------
void SchematicScene::insertComponentNodes(Component *c, bool noOptimize)
{
    // simulation components do not have ports
    if (c->Ports.empty()) return;

    // connect every node of the component to corresponding schematic node
    foreach(Port *pp, c->Ports)
        pp->Connection = insertNode(pp->x+c->cx, pp->y+c->cy, c);

    if(noOptimize)  return;

    Node    *pn;
    Element *pe, *pe1;
    Q3PtrList<Element> *pL;
    // if component over wire then delete this wire
    QListIterator<Port *> iport(c->Ports);
    // omit the first element
    Port *pp = iport.next();
    while (iport.hasNext())
    {
        pp = iport.next();
        pn = pp->Connection;
        for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
            if(pe->ElemType == isWire)
            {
                if(((Wire*)pe)->Port1 == pn)  pL = &(((Wire*)pe)->Port2->Connections);
                else  pL = &(((Wire*)pe)->Port1->Connections);

                for(pe1 = pL->first(); pe1!=0; pe1 = pL->next())
                    if(pe1 == c)
                    {
                        deleteWire((Wire*)pe);
                        break;
                    }
            }
    }
}

// ---------------------------------------------------
// Used for example in moving components.
void SchematicScene::insertRawComponent(Component *c, bool noOptimize)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, noOptimize);
    Components->append(c);

    // a ground symbol erases an existing label on the wire line
    if(c->obsolete_model_hack() == "GND") { // BUG.
        c->gnd_obsolete_model_override_hack("x");
        Element *pe = getWireLabel(c->Ports.first()->Connection);
        if(pe) if((pe->ElemType & isComponent) == 0)
            {
                delete ((Conductor*)pe)->Label;
                ((Conductor*)pe)->Label = 0;
            }
        c->gnd_obsolete_model_override_hack("GND");
    }
}

// ---------------------------------------------------
void SchematicScene::recreateComponent(Component *Comp)
{

    WireLabel **plMem=0, **pl;
    int PortCount = Comp->Ports.count();

    if(PortCount > 0)
    {
        // Save the labels whose node is not connected to somewhere else.
        // Otherwise the label would be deleted.
        pl = plMem = (WireLabel**)malloc(PortCount * sizeof(WireLabel*));
        foreach(Port *pp, Comp->Ports)
            if(pp->Connection->Connections.count() < 2)
            {
                *(pl++) = pp->Connection->Label;
                pp->Connection->Label = 0;
            }
            else  *(pl++) = 0;
    }


    int x = Comp->tx, y = Comp->ty;
    int x1 = Comp->x1, x2 = Comp->x2, y1 = Comp->y1, y2 = Comp->y2;
    QString tmp = Comp->name();    // is sometimes changed by "recreate"
    // only MultiViewComponent is usin the recreate(this) with argument
    Comp->recreate(this);   // to apply changes to the schematic symbol
    Comp->obsolete_name_override_hack(tmp);
    if(x < x1)
        x += Comp->x1 - x1;
    else if(x > x2)
        x += Comp->x2 - x2;
    if(y < y1)
        y += Comp->y1 - y1;
    else if(y > y2)
        y += Comp->y2 - y2;
    Comp->tx = x;
    Comp->ty = y;


    if(PortCount > 0)
    {
        // restore node labels
        pl = plMem;
        foreach(Port *pp, Comp->Ports)
        {
            if(*pl != 0)
            {
                (*pl)->cx = pp->Connection->cx;
                (*pl)->cy = pp->Connection->cy;
                placeNodeLabel(*pl);
            }
            pl++;
            if((--PortCount) < 1)  break;
        }
        for( ; PortCount > 0; PortCount--)
        {
            delete (*pl);  // delete not needed labels
            pl++;
        }
        free(plMem);
    }
}

// ---------------------------------------------------
void SchematicScene::insertComponent(Component *c)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, false);

    bool ok;
    QString s;
    int  max=1, len = c->name().length(), z;
    if(c->name().isEmpty()) { // BUG
        // a ground symbol erases an existing label on the wire line
        if(c->obsolete_model_hack() == "GND") { // BUG
            c->gnd_obsolete_model_override_hack("x");
            Element *pe = getWireLabel(c->Ports.first()->Connection);
            if(pe) if((pe->ElemType & isComponent) == 0)
                {
                    delete ((Conductor*)pe)->Label;
                    ((Conductor*)pe)->Label = 0;
                }
            c->gnd_obsolete_model_override_hack("GND");
        }
    }
    else
    {
        // determines the name by looking for names with the same
        // prefix and increment the number
        for(Component *pc = Components->first(); pc != 0; pc = Components->next())
            if(pc->name().left(len) == c->name())
            {
                s = pc->name().right(pc->name().length()-len);
                z = s.toInt(&ok);
                if(ok) if(z >= max) max = z + 1;
            }
        c->obsolete_name_override_hack(
	    c->name() + QString::number(max));  // create name with new number
    }

    setComponentNumber(c); // important for power sources and subcircuit ports
    Components->append(c);
}

// ---------------------------------------------------
/// \bug avoid linear search, pass rubberband selection or arbitrary list of selected items
void SchematicScene::activateCompsWithinRect(int x1, int y1, int x2, int y2)
{
    bool changed = false;
    int  cx1, cy1, cx2, cy2, a;
    // exchange rectangle coordinates to obtain x1 < x2 and y1 < y2
    cx1 = (x1 < x2) ? x1 : x2;
    cx2 = (x1 > x2) ? x1 : x2;
    cy1 = (y1 < y2) ? y1 : y2;
    cy2 = (y1 > y2) ? y1 : y2;
    x1 = cx1;
    x2 = cx2;
    y1 = cy1;
    y2 = cy2;


    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status"
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->obsolete_model_hack() == "GND"){
				  // if existing, delete label on wire line
                                    oneLabel(pc->Ports.first()->Connection);
				}
                        }
                        changed = true;
                    }
    }

    if(changed) {
        TODO("callback to set changed");
        //setChanged(true, true);
    }
}

// ---------------------------------------------------
/// \bug avoid linear search, pass item under mouse event directly
bool SchematicScene::activateSpecifiedComponent(int x, int y)
{
    int x1, y1, x2, y2, a;
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        pc->Bounding(x1, y1, x2, y2);
        if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.count() > 1)
                        {
                            if(a < 0)  a = 2;
                            pc->isActive = a;    // change "active status"
                        }
                        else
                        {
                            a &= 1;
                            pc->isActive = a;    // change "active status"
                            if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                                if(pc->obsolete_model_hack() == "GND"){
				  // if existing, delete label on wire line
                                    oneLabel(pc->Ports.first()->Connection);
				}
                        }
                        TODO("callback to set changed");
                        //setChanged(true, true);
                        return true;
                    }
    }
    return false;
}

// ---------------------------------------------------
bool SchematicScene::activateSelectedComponents()
{
    int a;
    bool sel = false;
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
        if(pc->ElemSelected)
        {
            a = pc->isActive - 1;

            if(pc->Ports.count() > 1)
            {
                if(a < 0)  a = 2;
                pc->isActive = a;    // change "active status"
            }
            else
            {
                a &= 1;
                pc->isActive = a;    // change "active status"
                if(a == COMP_IS_ACTIVE)  // only for active (not shorten)
                    if(pc->obsolete_model_hack() == "GND"){
		      // if existing, delete label on wire line
                        oneLabel(pc->Ports.first()->Connection);
		    }
            }
            sel = true;
        }

    if(sel) {
        TODO("callback to set changed");
        //setChanged(true, true);
    }
    return sel;
}

// ---------------------------------------------------
// Sets the component ports anew. Used after rotate, mirror etc.
void SchematicScene::setCompPorts(Component *pc)
{
    WireLabel *pl;
    Q3PtrList<WireLabel> LabelCache;

    foreach(Port *pp, pc->Ports)
    {
        pp->Connection->Connections.removeRef((Element*)pc);// delete connections
        switch(pp->Connection->Connections.count())
        {
        case 0:
            pl = pp->Connection->Label;
            if(pl)
            {
                LabelCache.append(pl);
                pl->cx = pp->x + pc->cx;
                pl->cy = pp->y + pc->cy;
            }
            Nodes->removeRef(pp->Connection);
            break;
        case 2:
            oneTwoWires(pp->Connection); // try to connect two wires to one
        default:
            ;
        }
    }

    // Re-connect component node to schematic node. This must be done completely
    // after the first loop in order to avoid problems with node labels.
    foreach(Port *pp, pc->Ports)
        pp->Connection = insertNode(pp->x+pc->cx, pp->y+pc->cy, pc);

    for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next())
        insertNodeLabel(pl);
}

// ---------------------------------------------------
// Returns a pointer of the component on whose text x/y points.
/// \bug replace with itemAt, find parent of label
Component* SchematicScene::selectCompText(int x_, int y_, int& w, int& h)
{
    int a, b, dx, dy;
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        a = pc->cx + pc->tx;
        if(x_ < a)  continue;
        b = pc->cy + pc->ty;
        if(y_ < b)  continue;

        pc->textSize(dx, dy);
        if(x_ > a+dx)  continue;
        if(y_ > b+dy)  continue;

        w = dx;
        h = dy;
        return pc;
    }

    return 0;
}

// ---------------------------------------------------
Component* SchematicScene::searchSelSubcircuit()
{
    Component *sub=0;
    // test all components
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        if(!pc->ElemSelected) continue;
        if(pc->obsolete_model_hack() != "Sub")
            if(pc->obsolete_model_hack() != "VHDL")
                if(pc->obsolete_model_hack() != "Verilog") continue;

        if(sub != 0) return 0;    // more than one subcircuit selected
        sub = pc;
    }
    return sub;
}


// ---------------------------------------------------
// Deletes the component 'c'.
void SchematicScene::deleteComp(Component *c)
{
    // delete all port connections
    foreach(Port *pn, c->Ports)
        switch(pn->Connection->Connections.count())
        {
        case 1  :
            if(pn->Connection->Label) delete pn->Connection->Label;
            Nodes->removeRef(pn->Connection);  // delete open nodes
            pn->Connection = 0;		  //  (auto-delete)
            break;
        case 3  :
            pn->Connection->Connections.removeRef(c);// delete connection
            oneTwoWires(pn->Connection);  // two wires -> one wire
            break;
        default :
            pn->Connection->Connections.removeRef(c);// remove connection
            break;
        }

    Components->removeRef(c);   // delete component
}

// ---------------------------------------------------
int SchematicScene::copyComponents(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    Component *pc;
    int bx1, by1, bx2, by2, count=0;
    // find bounds of all selected components
    for(pc = Components->first(); pc != 0; )
    {
        if(pc->ElemSelected)
        {
            pc->Bounding(bx1, by1, bx2, by2);  // is needed because of "distribute
            if(bx1 < x1) x1 = bx1;             // uniformly"
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            count++;
            ElementCache->append(pc);

            // rescue non-selected node labels
            foreach(Port *pp, pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->Connections.count() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);

                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged, but
                        // remember component for align/distribute.
                        pp->Connection->Label->pOwner = (Node*)pc;

                        pp->Connection->Label = 0;
                    }

            deleteComp(pc);
            pc = Components->current();
            continue;
        }
        pc = Components->next();
    }
    return count;
}

// ---------------------------------------------------
void SchematicScene::copyComponents2(int& x1, int& y1, int& x2, int& y2,
                                QList<Element *> *ElementCache)
{
    Component *pc;
    // find bounds of all selected components
    for(pc = Components->first(); pc != 0; )
    {
        if(pc->ElemSelected)
        {
            // is better for unsymmetrical components
            if(pc->cx < x1)  x1 = pc->cx;
            if(pc->cx > x2)  x2 = pc->cx;
            if(pc->cy < y1)  y1 = pc->cy;
            if(pc->cy > y2)  y2 = pc->cy;

            ElementCache->append(pc);

            // rescue non-selected node labels
            foreach(Port *pp, pc->Ports)
                if(pp->Connection->Label)
                    if(pp->Connection->Connections.count() < 2)
                    {
                        ElementCache->append(pp->Connection->Label);
                        pp->Connection->Label = 0;
                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged.
                    }

            deleteComp(pc);
            pc = Components->current();
            continue;
        }
        pc = Components->next();
    }
}


/* *******************************************************************
   *****                                                         *****
   *****                  Actions with labels                    *****
   *****                                                         *****
   ******************************************************************* */

// Test, if wire connects wire line with more than one label and delete
// all further labels. Also delete all labels if wire line is grounded.
void SchematicScene::oneLabel(Node *n1)
{
    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    WireLabel *pl = 0;
    bool named=false;   // wire line already named ?
    Q3PtrList<Node> Cons;

    for(pn = Nodes->first(); pn!=0; pn = Nodes->next())
        pn->y1 = 0;   // mark all nodes as not checked

    Cons.append(n1);
    n1->y1 = 1;  // mark Node as already checked
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
    {
        if(pn->Label)
        {
            if(named)
            {
                delete pn->Label;
                pn->Label = 0;    // erase double names
            }
            else
            {
                named = true;
                pl = pn->Label;
            }
        }

        for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
        {
            if(pe->ElemType != isWire)
            {
                if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                    if(((Component*)pe)->obsolete_model_hack() == "GND")
                    {
                        named = true;
                        if(pl)
                        {
                            pl->pOwner->Label = 0;
                            delete pl;
                        }
                        pl = 0;
                    }
                continue;
            }
            pw = (Wire*)pe;

            if(pn != pw->Port1) pNode = pw->Port1;
            else pNode = pw->Port2;

            if(pNode->y1) continue;
            pNode->y1 = 1;  // mark Node as already checked
            Cons.append(pNode);
            Cons.findRef(pn);

            if(pw->Label)
            {
                if(named)
                {
                    delete pw->Label;
                    pw->Label = 0;    // erase double names
                }
                else
                {
                    named = true;
                    pl = pw->Label;
                }
            }
        }
    }
}

// ---------------------------------------------------
int SchematicScene::placeNodeLabel(WireLabel *pl)
{
    Node *pn;
    int x = pl->cx;
    int y = pl->cy;

    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())
        if(pn->cx == x) if(pn->cy == y) break;

    if(!pn)  return -1;

    Element *pe = getWireLabel(pn);
    if(pe)      // name found ?
    {
        if(pe->ElemType & isComponent)
        {
            delete pl;
            return -2;  // ground potential
        }

        delete ((Conductor*)pe)->Label;
        ((Conductor*)pe)->Label = 0;
    }

    pn->Label = pl;   // insert node label
    pl->ElemType = isNodeLabel;
    pl->pOwner = pn;
    return 0;
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
Element* SchematicScene::getWireLabel(Node *pn_)
{
    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    Q3PtrList<Node> Cons;

    for(pn = Nodes->first(); pn!=0; pn = Nodes->next())
        pn->y1 = 0;   // mark all nodes as not checked

    Cons.append(pn_);
    pn_->y1 = 1;  // mark Node as already checked
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
        if(pn->Label) return pn;
        else
            for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
            {
                if(pe->ElemType != isWire)
                {
                    if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                        if(((Component*)pe)->obsolete_model_hack() == "GND") return pe;
                    continue;
                }

                pw = (Wire*)pe;
                if(pw->Label) return pw;

                if(pn != pw->Port1) pNode = pw->Port1;
                else pNode = pw->Port2;

                if(pNode->y1) continue;
                pNode->y1 = 1;  // mark Node as already checked
                Cons.append(pNode);
                Cons.findRef(pn);
            }
    return 0;   // no wire label found
}

// ---------------------------------------------------
// Inserts a node label.
void SchematicScene::insertNodeLabel(WireLabel *pl)
{
    if(placeNodeLabel(pl) != -1)
        return;

    // Go on, if label don't lie on existing node.

    Wire *pw = selectedWire(pl->cx, pl->cy);
    if(pw)    // lies label on existing wire ?
    {
        if(getWireLabel(pw->Port1) == 0)  // wire not yet labeled ?
            pw->setName(pl->Name, pl->initValue, 0, pl->cx, pl->cy);

        delete pl;
        return;
    }


    Node *pn = new Node(pl->cx, pl->cy);
    Nodes->append(pn);

    pn->Label = pl;
    pl->ElemType  = isNodeLabel;
    pl->pOwner = pn;
}

// ---------------------------------------------------
void SchematicScene::copyLabels(int& x1, int& y1, int& x2, int& y2,
                           QList<Element *> *ElementCache)
{
    WireLabel *pl;
    // find bounds of all selected wires
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        pl = pw->Label;
        if(pl) if(pl->ElemSelected)
            {
                if(pl->x1 < x1) x1 = pl->x1;
                if(pl->y1-pl->y2 < y1) y1 = pl->y1-pl->y2;
                if(pl->x1+pl->x2 > x2) x2 = pl->x1+pl->x2;
                if(pl->y1 > y2) y2 = pl->y1;
                ElementCache->append(pl);
            }
    }

    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    {
        pl = pn->Label;
        if(pl) if(pl->ElemSelected)
            {
                if(pl->x1 < x1) x1 = pl->x1;
                if(pl->y1-pl->y2 < y1) y1 = pl->y1-pl->y2;
                if(pl->x1+pl->x2 > x2) x2 = pl->x1+pl->x2;
                if(pl->y1 > y2) y2 = pl->y1;
                ElementCache->append(pl);
                pl->pOwner->Label = 0;   // erase connection
                pl->pOwner = 0;
            }
    }
}







// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool SchematicScene::rotateElements()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1=INT_MAX, y1=INT_MAX;
  int x2=INT_MIN, y2=INT_MIN;
  QList<Element *> ElementCache;
  copyLabels(x1, y1, x2, y2, &ElementCache);   // must be first of all !
  copyComponents(x1, y1, x2, y2, &ElementCache);
  copyWires(x1, y1, x2, y2, &ElementCache);
  copyPaintings(x1, y1, x2, y2, &ElementCache);
  if(y1 == INT_MAX) return false;   // no element selected

  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // center for rotation
  y1 = (y1+y2) >> 1;
  //setOnGrid(x1, y1);


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->rotate();   //rotate component !before! rotating its center
        pc->setCenter(pc->cy - y1 + x1, x1 - pc->cx + y1);
        insertRawComponent(pc);
        break;

      case isWire:
        pw = (Wire*)pe;
        x2 = pw->x1;
        pw->x1 = pw->y1 - y1 + x1;
        pw->y1 = x1 - x2 + y1;
        x2 = pw->x2;
        pw->x2 = pw->y2 - y1 + x1;
        pw->y2 = x1 - x2 + y1;
        pl = pw->Label;
        if(pl) {
          x2 = pl->cx;
          pl->cx = pl->cy - y1 + x1;
          pl->cy = x1 - x2 + y1;
          if(pl->ElemType == isHWireLabel)
            pl->ElemType = isVWireLabel;
          else pl->ElemType = isHWireLabel;
        }
        insertWire(pw);
        break;

      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	x2 = pl->x1;
	pl->x1 = pl->y1 - y1 + x1;
	pl->y1 = x1 - x2 + y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0) {
	  x2 = pl->x1;
	  pl->x1 = pl->y1 - y1 + x1;
	  pl->y1 = x1 - x2 + y1;
	}
	x2 = pl->cx;
	pl->cx = pl->cy - y1 + x1;
	pl->cy = x1 - x2 + y1;
	insertNodeLabel(pl);
	break;

      case isPainting:
        pp = (Painting*)pe;
        pp->rotate();   // rotate painting !before! rotating its center
        pp->getCenter(x2, y2);
        //qDebug("pp->getCenter(x2, y2): (%i,%i)\n", x2, y2);
        //qDebug("(x1,y1) (x2,y2): (%i,%i) (%i,%i)\n", x1,y1,x2,y2);
        pp->setCenter(y2-y1 + x1, x1-x2 + y1);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();

  TODO("callback to set changed");
  //setChanged(true, true);
  return true;
}


// ---------------------------------------------------
// Mirrors all selected components.
// First copy them to 'ElementCache', then mirror and insert again.
bool SchematicScene::mirrorXComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  y1 = (y1+y2) >> 1;   // axis for mirroring
  setOnGrid(y2, y1);
  y1 <<= 1;


  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
	pc = (Component*)pe;
	pc->mirrorX();   // mirror component !before! mirroring its center
	pc->setCenter(pc->cx, y1 - pc->cy);
	insertRawComponent(pc);
	break;
      case isWire:
	pw = (Wire*)pe;
	pw->y1 = y1 - pw->y1;
	pw->y2 = y1 - pw->y2;
	pl = pw->Label;
	if(pl)  pl->cy = y1 - pl->cy;
	insertWire(pw);
	break;
      case isHWireLabel:
      case isVWireLabel:
	pl = (WireLabel*)pe;
	pl->y1 = y1 - pl->y1;
	break;
      case isNodeLabel:
	pl = (WireLabel*)pe;
	if(pl->pOwner == 0)
	  pl->y1 = y1 - pl->y1;
	pl->cy = y1 - pl->cy;
	insertNodeLabel(pl);
	break;
      case isPainting:
	pp = (Painting*)pe;
	pp->getCenter(x2, y2);
	pp->mirrorX();   // mirror painting !before! mirroring its center
	pp->setCenter(x2, y1 - y2);
	Paintings->append(pp);
	break;
      default: ;
    }

  ElementCache.clear();
  TODO("callback to set changed");
  //setChanged(true, true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool SchematicScene::mirrorYComponents()
{
  Wires->setAutoDelete(false);
  Components->setAutoDelete(false);

  int x1, y1, x2, y2;
  QList<Element *> ElementCache;
  if(!copyComps2WiresPaints(x1, y1, x2, y2, &ElementCache))
    return false;
  Wires->setAutoDelete(true);
  Components->setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // axis for mirroring
  setOnGrid(x1, x2);
  x1 <<= 1;

  Wire *pw;
  Painting  *pp;
  Component *pc;
  WireLabel *pl;
  // re-insert elements
  foreach(Element *pe, ElementCache)
    switch(pe->ElemType) {
      case isComponent:
      case isAnalogComponent:
      case isDigitalComponent:
        pc = (Component*)pe;
        pc->mirrorY();   // mirror component !before! mirroring its center
        pc->setCenter(x1 - pc->cx, pc->cy);
        insertRawComponent(pc);
        break;
      case isWire:
        pw = (Wire*)pe;
        pw->x1 = x1 - pw->x1;
        pw->x2 = x1 - pw->x2;
        pl = pw->Label;
        if(pl)  pl->cx = x1 - pl->cx;
        insertWire(pw);
        break;
      case isHWireLabel:
      case isVWireLabel:
        pl = (WireLabel*)pe;
        pl->x1 = x1 - pl->x1;
        break;
      case isNodeLabel:
        pl = (WireLabel*)pe;
        if(pl->pOwner == 0)
          pl->x1 = x1 - pl->x1;
        pl->cx = x1 - pl->cx;
        insertNodeLabel(pl);
        break;
      case isPainting:
        pp = (Painting*)pe;
        pp->getCenter(x2, y2);
        pp->mirrorY();   // mirror painting !before! mirroring its center
        pp->setCenter(x1 - x2, y2);
        Paintings->append(pp);
        break;
      default: ;
    }

  ElementCache.clear();
  TODO("callback to set changed");
  //setChanged(true, true);
  return true;
}


/* *******************************************************************
   *****                                                         *****
   *****                Actions with paintings                   *****
   *****                                                         *****
   ******************************************************************* */

// ---------------------------------------------------
void SchematicScene::copyPaintings(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    Painting *pp;
    int bx1, by1, bx2, by2;
    // find boundings of all selected paintings
    for(pp = Paintings->first(); pp != 0; )
        if(pp->ElemSelected)
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            ElementCache->append(pp);
            Paintings->take();
            pp = Paintings->current();
        }
        else pp = Paintings->next();
}



// ---------------------------------------------------
// Sets selected elements on grid.
/// \bug pass list of selected instead of searching for them.
bool SchematicScene::elementsOnGrid()
{
  int x, y, No;
  bool count = false;
  WireLabel *pl, *pLabel;
  Q3PtrList<WireLabel> LabelCache;

  // test all components
  Components->setAutoDelete(false);
  for(Component *pc = Components->last(); pc != 0; pc = Components->prev())
    if(pc->ElemSelected) {

      // rescue non-selected node labels
      foreach(Port *pp, pc->Ports)
        if(pp->Connection->Label)
          if(pp->Connection->Connections.count() < 2) {
            LabelCache.append(pp->Connection->Label);
            pp->Connection->Label->pOwner = 0;
            pp->Connection->Label = 0;
          }

      x = pc->cx;
      y = pc->cy;
      No = Components->at();
      deleteComp(pc);
      setOnGrid(pc->cx, pc->cy);
      insertRawComponent(pc);
      Components->at(No);   // restore current list position
      pc->ElemSelected = false;
      count = true;

      x -= pc->cx;
      y -= pc->cy;    // re-insert node labels and correct position
      for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next()) {
        pl->cx -= x;
        pl->cy -= y;
        insertNodeLabel(pl);
      }
      LabelCache.clear();
    }
  Components->setAutoDelete(true);

  Wires->setAutoDelete(false);
  // test all wires and wire labels
  for(Wire *pw = Wires->last(); pw != 0; pw = Wires->prev()) {
    pl = pw->Label;
    pw->Label = 0;

    if(pw->ElemSelected) {
      // rescue non-selected node label
      pLabel = 0;
      if(pw->Port1->Label) {
        if(pw->Port1->Connections.count() < 2) {
            pLabel = pw->Port1->Label;
            pw->Port1->Label = 0;
        }
      }
      else if(pw->Port2->Label) {
        if(pw->Port2->Connections.count() < 2) {
            pLabel = pw->Port2->Label;
            pw->Port2->Label = 0;
        }
      }

      No = Wires->at();
      deleteWire(pw);
      setOnGrid(pw->x1, pw->y1);
      setOnGrid(pw->x2, pw->y2);
      insertWire(pw);
      Wires->at(No);   // restore current list position
      pw->ElemSelected = false;
      count = true;
      if(pl)
        setOnGrid(pl->cx, pl->cy);

      if(pLabel) {
        setOnGrid(pLabel->cx, pLabel->cy);
        insertNodeLabel(pLabel);
      }
    }

    if(pl) {
      pw->Label = pl;
      if(pl->ElemSelected) {
        setOnGrid(pl->x1, pl->y1);
        pl->ElemSelected = false;
        count = true;
      }
    }
  }
  Wires->setAutoDelete(true);

  // test all node labels
  for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    if(pn->Label)
      if(pn->Label->ElemSelected) {
        setOnGrid(pn->Label->x1, pn->Label->y1);
        pn->Label->ElemSelected = false;
        count = true;
      }

  // test all diagrams
  for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev()) {
    if(pd->ElemSelected) {
      setOnGrid(pd->cx, pd->cy);
      pd->ElemSelected = false;
      count = true;
    }

    foreach(Graph *pg,pd->Graphs)
      // test markers of diagram
      foreach(Marker *pm, pg->Markers)
        if(pm->ElemSelected) {
	  x = pm->x1 + pd->cx;
	  y = pm->y1 + pd->cy;
	  setOnGrid(x, y);
	  pm->x1 = x - pd->cx;
	  pm->y1 = y - pd->cy;
	  pm->ElemSelected = false;
	  count = true;
        }
  }

  // test all paintings
  for(Painting *pa = Paintings->last(); pa != 0; pa = Paintings->prev())
    if(pa->ElemSelected) {
      setOnGrid(pa->cx, pa->cy);
      pa->ElemSelected = false;
      count = true;
    }

  if(count) {
      TODO("callback to set changed");
      //setChanged(true, true);
  }
  return count;
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void SchematicScene::setOnGrid(int& x, int& y)
{
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;

  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;
}

// ---------------------------------------------------
// Updates the graph data of all diagrams (load from data files).
void SchematicScene::reloadGraphs()
{
  QFileInfo Info(DocName);
  foreach (auto const pd, filterItems<Diagram>(this))
    pd->loadGraphData(Info.path()+QDir::separator()+DataSet);
}

/*!
 * @brief SchematicScene::simpleInsertComponent
 *
 * Inserts a component without performing logic for wire optimization.
 * Used when loading from a schematic file.
 *
 * @param c is pointing to the component to be inserted.
 */
void SchematicScene::simpleInsertComponent(Component *c)
{
  Node *pn;
  int x, y;
  // connect every node of component
  foreach(Port *pp, c->Ports) {
    x = pp->x+c->cx;
    y = pp->y+c->cy;

    // check if new node lies upon existing node
    foreach (auto pn, filterItems<Node>(this)) {
      if(pn->cx == x) if(pn->cy == y) {
	if (!pn->DType.isEmpty()) {
	  pp->Type = pn->DType;
	}
	if (!pp->Type.isEmpty()) {
	  pn->DType = pp->Type;
	}
	break;
      }
    }

    if(pn == 0) { // create new node, if no existing one lies at this position
      pn = new Node(x, y);
      // add Node to scene
      addItem(pn);
    }
    /// \todo fix note to component connection
    /*
    pn->Connections.append(c);  // connect schematic node to component node
    if (!pp->Type.isEmpty()) {
      pn->DType = pp->Type;
    }

    pp->Connection = pn;  // connect component node to schematic node
    */
  }

  // add Component to scene
  addItem(c);
  /// \todo check performance, no need to update scene on every addIte
  update();
}

/*!
 * \brief SchematicScene::simpleInsertWire
 * \param pw
 * Inserts a wire without performing logic for optimizing.
 * \todo similar to insertNode
 *
 * - check for node 1 overlap
 * - check for zero length wire
 * - add node label (at wire corner) to scene
 * - check for node 2 overlap
 * - add nodes to list
 * - add nodes to scene
 * - add wire to list
 * - add wire to scene
 * - add wire label to scene
 *
 */
void SchematicScene::simpleInsertWire(Wire *pw)
{
  TODO("fix append to DocNodes, DocWires");
  Node *pn1 = 0;
  Node *pn2 = 0;

  // check if first wire node lies upon existing node
  foreach (pn1, filterItems<Node>(this)) {
    if(pn1->cx == pw->x1) if(pn1->cy == pw->y1) break;
  }

  // create new node, if no existing one lies at this position
  if(!pn1) {
    pn1 = new Node(pw->x1, pw->y1);
  }

  // check for zero length wire
  if(pw->x1 == pw->x2) if(pw->y1 == pw->y2) {
    pn1->Label = pw->Label;   // wire with length zero are just node labels
    if (pn1->Label) {
      pn1->Label->ElemType = isNodeLabel;
      pn1->Label->pOwner = pn1;
      // add WireLabel to scene
      addItem(pn1->Label);
    }
    delete pw;           // delete wire because this is not a wire
    return;
  }
  pn1->Connections.append(pw);  // connect schematic node to component node
  pw->Port1 = pn1;

  // check if second wire node lies upon existing node
  foreach (pn2, filterItems<Node>(this)) {
    if(pn2->cx == pw->x2) if(pn2->cy == pw->y2) break;
  }

  if(!pn2) {   // create new node, if no existing one lies at this position
    pn2 = new Node(pw->x2, pw->y2);
  }
  pn2->Connections.append(pw);  // connect schematic node to component node
  pw->Port2 = pn2;

  addItem(pn1);
  addItem(pn2);
  addItem(pw);

  if(pw->Label) {
    addItem(pw->Label);
  }
}
