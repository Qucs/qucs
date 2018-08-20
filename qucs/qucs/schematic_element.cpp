/***************************************************************************
                            schematic_element.cpp
                           -----------------------
    begin                : Sat Mar 3 2006
    copyright            : (C) 2006 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <stdlib.h>
#include <limits.h>

#include "schematic.h"
#include "mouseactions.h"
#include "qt_compat.h"
#include <QDebug>

#include "trace.h"


/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the nodes                 *****
   *****                                                         *****
   ******************************************************************* */

// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
Node* Schematic::insertNode(int x, int y, Element *e)
{
    Node *pn;
    // check if new node lies upon existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()){
        // check every node
        if(pn->cx_() == x) if(pn->cy_() == y) {
            pn->Connections.append(e);
            break;
        }
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
        if(pw->x1_() == x) {
            if(pw->y1_() > y) continue;
            if(pw->y2_() < y) continue;
        } else if(pw->y1_() == y) {
            if(pw->x1_() > x) continue;
            if(pw->x2_() < x) continue;
        }else{
            continue;
        }

        // split the wire into two wires
        splitWire(pw, pn);
        return pn;
    }

    return pn;
}

// ---------------------------------------------------
Node* Schematic::selectedNode(int x, int y)
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
int Schematic::insertWireNode1(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()) // check every node
        if(pn->cx_() == w->x1_()) if(pn->cy_() == w->y1_()) break;

    if(pn != 0)
    {
        pn->Connections.append(w);
        w->Port1 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = Wires->first(); ptr2 != 0; ptr2 = Wires->next())
    {
        if(ptr2->x1_() == w->x1_()) {
            if(ptr2->y1_() > w->y1_()) continue;
            if(ptr2->y2_() < w->y1_()) continue;

            if(ptr2->isHorizontal() == w->isHorizontal()){
                // ptr2-wire is vertical
                if(ptr2->y2_() >= w->y2_()) {
                    delete w;    // new wire lies within an existing wire
                    return 0;
                } else {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->Port2->Connections.count() == 1) {
                        w->y1__() = ptr2->y1_();
                        w->Port1 = ptr2->Port1;
                        if(ptr2->Label) {
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
                        w->y1__() = ptr2->y2_();
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->Connections.append(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else if(ptr2->y1_() == w->y1_())
        {
            if(ptr2->x1_() > w->x1_()) continue;
            if(ptr2->x2_() < w->x1_()) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                if(ptr2->x2_() >= w->x2_())
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
                        w->x1__() = ptr2->x1_();
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
                        w->x1__() = ptr2->x2_();
                        w->Port1 = ptr2->Port2;
                        ptr2->Port2->Connections.append(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else continue;

        pn = new Node(w->x1_(), w->y1_());   // create new node
        Nodes->append(pn);
        pn->Connections.append(w);  // connect schematic node to the new wire
        w->Port1 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x1_(), w->y1_());   // create new node
    Nodes->append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port1 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->Type != isWire) continue;
        if(!pw->isHorizontal()) continue;
        if(pw->x1_() < w->x1_())
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
                w->Label->Type = isHWireLabel;
            }
            w->x1__() = pw->x1_();
            w->Port1 = pw->Port1;      // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port1->Connections.removeRef(pw);
            w->Port1->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        if(pw->x2_() >= w->x2_())    // new wire lies within an existing one ?
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
        w->x1__() = pw->x2_();    // shorten new wire according to an existing one
        w->Port1->Connections.removeRef(w);
        w->Port1 = pw->Port2;
        w->Port1->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires1(Wire *w)
{
    Wire *pw;
    Node *n = w->Port1;

    pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->Type != isWire) continue;
        if(pw->isHorizontal()) continue;
        if(pw->y1_() < w->y1_())
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
                w->Label->Type = isVWireLabel;
            }
            w->y1__() = pw->y1_();
            w->Port1 = pw->Port1;         // new wire lengthens an existing one
            Nodes->removeRef(n);
            w->Port1->Connections.removeRef(pw);
            w->Port1->Connections.append(w);
            Wires->removeRef(pw);
            return true;
        }
        if(pw->y2_() >= w->y2_())    // new wire lies complete within an existing one ?
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
        w->y1__() = pw->y2_();    // shorten new wire according to an existing one
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
int Schematic::insertWireNode2(Wire *w)
{
    Node *pn;
    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next()){
        if(pn->cx_() == w->x2_()) if(pn->cy_() == w->y2_()) break;
    }

    if(pn != 0)
    {
        pn->Connections.append(w);
        w->Port2 = pn;
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(Wire *ptr2 = Wires->first(); ptr2 != 0; ptr2 = Wires->next())
    {
        if(ptr2->x1_() == w->x2_())
        {
            if(ptr2->y1_() > w->y2_()) continue;
            if(ptr2->y2_() < w->y2_()) continue;

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
                    w->y2__() = ptr2->y2_();
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
                    ptr2->Port2->Connections.append(w);
                    Nodes->removeRef(ptr2->Port1);
                    Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->y2__() = ptr2->y1_();
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->Connections.append(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else if(ptr2->y1_() == w->y2_())
        {
            if(ptr2->x1_() > w->x2_()) continue;
            if(ptr2->x2_() < w->x2_()) continue;

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
                    w->x2__() = ptr2->x2_();
                    w->Port2 = ptr2->Port2;
                    ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
                    ptr2->Port2->Connections.append(w);
                    Nodes->removeRef(ptr2->Port1);
                    Wires->removeRef(ptr2);
                    return 2;
                }
                else
                {
                    w->x2__() = ptr2->x1_();
                    w->Port2 = ptr2->Port1;
                    ptr2->Port1->Connections.append(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else continue;

        pn = new Node(w->x2_(), w->y2_());   // create new node
        Nodes->append(pn);
        pn->Connections.append(w);  // connect schematic node to the new wire
        w->Port2 = pn;

        // split the wire into two wires
        splitWire(ptr2, pn);
        return 2;
    }

    pn = new Node(w->x2_(), w->y2_());   // create new node
    Nodes->append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port2 = pn;
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->Type != isWire) continue;
        if(!pw->isHorizontal()) continue;
        if(pw->x2_() > w->x2_())
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->x2__() = pw->x2_();
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
        w->x2__() = pw->x1_();    // shorten new wire according to an existing one
        w->Port2->Connections.removeRef(w);
        w->Port2 = pw->Port1;
        w->Port2->Connections.append(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires2(Wire *w)
{
    Wire *pw;
    Node *n = w->Port2;

    pw = (Wire*)n->Connections.last(); // last connection is the new wire itself
    for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev())
    {
        if(pw->Type != isWire) continue;
        if(pw->isHorizontal()) continue;
        if(pw->y2_() > w->y2_())
        {
            if(n->Connections.count() != 2) continue;
            if(pw->Label)
            {
                w->Label = pw->Label;
                w->Label->pOwner = w;
            }
            w->y2__() = pw->y2_();
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
        w->y2__() = pw->y1_();    // shorten new wire according to an existing one
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
int Schematic::insertWire(Wire *w)
{
    int  tmp, con = 0;
    bool ok;

    // change coordinates if necessary (port 1 coordinates must be less
    // port 2 coordinates)
    if(w->x1_() > w->x2_())
    {
        tmp = w->x1_();
        w->x1__() = w->x2_();
        w->x2__() = tmp;
    }
    else if(w->y1_() > w->y2_())
    {
        tmp = w->y1_();
        w->y1__() = w->y2_();
        w->y2__() = tmp;
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
    assert(Nodes);
    assert(Wires);
    // this looks like it is not a loop at all.
    // check: what are the findRef calls really doing, and why?
    for(pw=Wires->current(); pw!=nullptr; pw=Wires->next()){
        for(pn = Nodes->first(); pn != 0; ){
	    // check every node
            if(pn->cx_() == pw->x1_()) {
                if(pn->cy_() <= pw->y1_()) {
                    pn = Nodes->next();
                    continue;
                }else if(pn->cy_() >= pw->y2_()) {
                    pn = Nodes->next();
                    continue;
                }
            } else if(pn->cy_() == pw->y1_()){
                if(pn->cx_() <= pw->x1_()) {
                    pn = Nodes->next();
                    continue;
                }else if(pn->cx_() >= pw->x2_())
                {
                    pn = Nodes->next();
                    continue;
                }else{
		  // do more stuff below.
		}
            }else{
                pn = Nodes->next();
                continue;
            }

            n1 = 2;
            n2 = 3;
            pn2 = pn;
            // check all connections of the current node
            for(pe=pn->Connections.first(); pe!=0; pe=pn->Connections.next())
            {
                if(pe->Type != isWire) continue;
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
                    Wires->findRef(pw);      // is this a hidden loop condition?!
                }
                break;
            }
            if(n1 == 1) if(n2 == 1) continue;

            // split wire into two wires
            if((pw->x1_() != pn->cx_()) || (pw->y1_() != pn->cy_())) {
                nw = new Wire(pw->x1_(), pw->y1_(), pn->cx_(), pn->cy_(), pw->Port1, pn);
                pn->Connections.append(nw);
                Wires->append(nw);
                Wires->findRef(pw); // hidden loop conditional?
                pw->Port1->Connections.append(nw);
            }
            pw->Port1->Connections.removeRef(pw);
            pw->x1__() = pn2->cx_();
            pw->y1__() = pn2->cy_();
            pw->Port1 = pn2;
            pn2->Connections.append(pw);

            pn = Nodes->next();
        }
    }

    if (Wires->containsRef (w))  // if two wire lines with different labels ...
        oneLabel(w->Port1);       // ... are connected, delete one label
    return con | 0x0200;   // sent also end flag
}

// other place.
//
// ---------------------------------------------------
// Follows a wire line and selects it.
void Schematic::selectWireLine(ElementGraphics *g, Node *pn, bool ctrl)
{
    Element* pe=element(g);
    Node *pn_1st = pn;
    while(pn->Connections.count() == 2)
    {
        if(pn->Connections.first() == element(pe)){
	    pe = pn->Connections.last();
	}else{
	    pe = pn->Connections.first();
	}

        if(pe->Type != isWire) break;
        if(ctrl) pe->toggleSelected();
        else pe->setSelected();

        if(((Wire*)pe)->Port1 == pn)  pn = ((Wire*)pe)->Port2;
        else  pn = ((Wire*)pe)->Port1;
        if(pn == pn_1st) break;  // avoid endless loop in wire loops
    }
}

// ---------------------------------------------------
Wire* Schematic::selectedWire(int x, int y)
{
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
        if(pw->getSelected(x, y))
            return pw;

    return 0;
}

// ---------------------------------------------------
// Splits the wire "*pw" into two pieces by the node "*pn".
Wire* Schematic::splitWire(Wire *pw, Node *pn)
{
    Wire *newWire = new Wire(pn->cx_(), pn->cy_(), pw->x2_(), pw->y2_(), pn, pw->Port2);
    newWire->setSelected(pw->isSelected());

    pw->x2__() = pn->cx_();
    pw->y2__() = pn->cy_();
    pw->Port2 = pn;

    newWire->Port2->Connections.prepend(newWire);
    pn->Connections.prepend(pw);
    pn->Connections.prepend(newWire);
    newWire->Port2->Connections.removeRef(pw);
    Wires->append(newWire);

    if(pw->Label)
        if((pw->Label->cx_() > pn->cx_()) || (pw->Label->cy_() > pn->cy_()))
        {
            newWire->Label = pw->Label;   // label goes to the new wire
            pw->Label = 0;
            newWire->Label->pOwner = newWire;
        }

    return newWire;
}

// ---------------------------------------------------
// If possible, make one wire out of two wires.
bool Schematic::oneTwoWires(Node *n)
{
    Wire *e3;
    Wire *e1 = (Wire*)n->Connections.getFirst();  // two wires -> one wire
    Wire *e2 = (Wire*)n->Connections.getLast();

    if(e1->Type == isWire) if(e2->Type == isWire)
            if(e1->isHorizontal() == e2->isHorizontal())
            {
                if(e1->x1_() == e2->x2_()) if(e1->y1_() == e2->y2_())
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
                        e1->Label->Type = isHWireLabel;
                    else
                        e1->Label->Type = isVWireLabel;
                }

                e1->x2__() = e2->x2_();
                e1->y2__() = e2->y2_();
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
void Schematic::deleteWire(Wire *w)
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
int Schematic::copyWires(int& x1, int& y1, int& x2, int& y2,
                         QList<Element *> *ElementCache)
{
    int count=0;
    Node *pn;
    Wire *pw;
    WireLabel *pl;
    for(pw = Wires->first(); pw != 0; )  // find bounds of all selected wires
        if(pw->isSelected())
        {
            if(pw->x1_() < x1) x1 = pw->x1_();
            if(pw->x2_() > x2) x2 = pw->x2_();
            if(pw->y1_() < y1) y1 = pw->y1_();
            if(pw->y2_() > y2) y2 = pw->y2_();

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

Marker* Schematic::setMarker(int x, int y)
{
  // only diagrams ...
  for(Diagram *pd = Diagrams->last(); pd != 0; pd = Diagrams->prev()){
    if(Marker* m=pd->setMarker(x,y)){
      setChanged(true, true);
      return m;
    }
  }
  return NULL;
}

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
void Schematic::markerLeftRight(bool left, Q3PtrList<ElementGraphics> *Elements)
{
    bool acted = false;
    for(auto i : *Elements) {
        Marker* pm = prechecked_cast<Marker*>(i);
        assert(pm);
        if(pm->moveLeftRight(left))
            acted = true;
    }

    if(acted)  setChanged(true, true, 'm');
}

// ---------------------------------------------------
// Move the marker pointer up/down on the more-dimensional graph.
void Schematic::markerUpDown(bool up, Q3PtrList<ElementGraphics> *Elements)
{
    Marker *pm;
    bool acted = false;
    for(pm = (Marker*)Elements->first(); pm!=0; pm = (Marker*)Elements->next())
    {
        if(pm->moveUpDown(up))
            acted = true;
    }

    if(acted)  setChanged(true, true, 'm');
}


/* *******************************************************************
   *****                                                         *****
   *****               Actions with all elements                 *****
   *****                                                         *****
   ******************************************************************* */

/* Selects a mouse action. onluy called from mouseaction.cpp
   Returns a mouse action.

   If 'flag' is true, the element can be deselected. If
   'flag' is false the element cannot be deselected. The
   purpose of this is to prevent deselection in cases such
   as right-clicking on a selected element to get a context
   menu.
*/
ElementMouseAction MouseActions::selectElement(Schematic* Doc,
	QPoint const& xy, bool flag, int *index)
{ untested();
   // THIS IS MISLEADING. it is also used to generate mouse actions.
   // we need something that produces actions, not Elements
   // actions will be needed to implement the undo stack... etcpp
#ifndef USE_SCROLLVIEW
    ElementGraphics *pe_sel=nullptr;
    ElementGraphics *pe_1st=nullptr;
    assert(Doc);
   auto scenepos=Doc->mapToScene(xy);

   ElementGraphics* e=Doc->itemAt(scenepos);
   if(e){ untested();
   }else{ untested();
   }

   // now add the mouseaction hacks...
   if(component(e)){ untested();
       ElementMouseAction pc(e);
       if(flag) { untested();
	   // The element can be deselected
	   pc->setSelected(!pc->isSelected());
	   return ElementMouseAction(pc);
       }else if(pe_sel) {
	   pe_sel->setSelected(false);
	   return ElementMouseAction(pc);
       }else if(pe_1st == 0) {
	   incomplete();
	   // give access to elements lying beneath
	   pe_1st = element(pc);
       } if(pc->isSelected()) {
	   incomplete();
	   pe_sel = element(pc);
       }else{
       }
   }else if(wire(e)){ untested();
       return ElementMouseAction(e);
   }else{ untested();
	   qDebug() << "nothing at" << xy << scenepos;
       return ElementMouseAction(nullptr);
   }
    return ElementMouseAction(pe_1st);
#else

    int n;

    QPointF pos=Doc->mapToScene(xy);
    float fX=pos.x();
    float fY=pos.y();
    int x=int(fX);
    int y=int(fY);
    Element *pe_1st = 0;
    Element *pe_sel = 0;
    WireLabel *pl = 0;
    float Corr = Doc->textCorr(); // ??

    // test all nodes and their labels
    for(Node *pn = Doc->nodes().last(); pn != 0; pn = Doc->nodes().prev()) {
        if(flag) {
            // The element can be deselected
	}else if(!index) {
	    // 'index' is only true if called from MouseActions::MPressSelect()
	}else if(pn->getSelected(x, y)) {
	    // Return the node pointer, as the selection cannot change
	    return ElementMouseAction(pn);
        }

        pl = pn->Label; // Get any wire label associated with the Node
        if(!pl) {
	}else if(pl->getSelected(x, y)) {
                if(flag) {
                    // The element can be deselected, so toggle its isSelected member
                    // TODO: I don't see a need for the xor here, a simple ! on the current value
                    // would be clearer and have the same effect?
                    pl->toggleSelected();
                    return ElementMouseAction(pl);
                }else if(pe_sel) {
                    // There is another currently
                    pe_sel->setSelected(false);
                    return ElementMouseAction(pl);
                }else if(pe_1st == 0) {
                    // give access to elements lying beneath by storing this label.
                    // If no label pointer (or other element) has previously been
                    // stored, the current label pointer is stored here.
                    // pe_1st is returned if no other selected element
                    pe_1st = pl;
                }else{
		}

                if(pl->isSelected()) {
                    // if current label is already selected, store a pointer to it.
                    // This can be used to cycle through
                    pe_sel = pl;
                }
            }
    }

    // test all wires and wire labels
    for(Wire *pw=Doc->wires().last(); pw != 0; pw = Doc->wires().prev()) {
        if(pw->getSelected(x, y)) {
            if(flag) {
                // The element can be deselected
                pw->toggleSelected();
                return ElementMouseAction(pw);
            }else if(pe_sel) {
                pe_sel->setSelected(false);
                return ElementMouseAction(pw);
            }else if(pe_1st == 0) {
                pe_1st = pw;   // give access to elements lying beneath
            }else{
	    }
            if(pw->isSelected()) {
                pe_sel = pw;
            }
        }else{
	}
        pl = pw->Label; // test any label associated with the wire
        if(pl) {
            if(pl->getSelected(x, y)) {
                if(flag) {
                    // The element can be deselected
                    pl->toggleSelected();
                    return ElementMouseAction(pl);
                }else if(pe_sel) {
                    pe_sel->setSelected(false);
                    return ElementMouseAction(pl);
                }
                if(pe_1st == 0) {
                    // give access to elements lying beneath
                    pe_1st = pl;
                }
                if(pl->isSelected()) {
                    pe_sel = pl;
                }
            }
        }
    }

    // test all components
    for(Component *pc=Doc->components().last(); pc!=nullptr; pc=Doc->components().prev()) {
        if(pc->getSelected(x, y)) {
            if(flag) {
                // The element can be deselected
                pc->toggleSelected();
                return ElementMouseAction(pc);
            }else if(pe_sel) {
                pe_sel->setSelected(false);
                return ElementMouseAction(pc);
            }else if(pe_1st == 0) {
		// give access to elements lying beneath
                pe_1st = pc;
            } if(pc->isSelected()) {
                pe_sel = pc;
            }else{
	    }
        }else{
            n = pc->getTextSelected(x, y, Corr);
	    // was property text clicked ?
            if(n >= 0) {
                pc->Type = isComponentText;
                if(index)  *index = n;
                return ElementMouseAction(pc);
            }
        }
    }

    Corr = 5.0 / Doc->Scale;  // size of line select and area for resizing
    // test all diagrams
    for(Diagram *pd = Doc->diagrams().last(); pd!=nullptr; pd=Doc->diagrams().prev()) {

        foreach(Graph *pg, pd->Graphs) {
            // test markers of graphs
            foreach(Marker *pm, pg->Markers)
            {
                if(pm->getSelected(x-pd->cx_(), y-pd->cy_()))
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pm->toggleSelected();
                        return ElementMouseAction(pm);
                    }
                    if(pe_sel)
                    {
                        pe_sel->setSelected(false);
                        return ElementMouseAction(pm);
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pm;   // give access to elements beneath
                    }
                    if(pm->isSelected())
                    {
                        pe_sel = pm;
                    }
                }
            }
        }

        // resize area clicked ?
        if(!pd->isSelected()){
          // why do we visit non-selected diagrams?!
        }else if(!pd->resizeTouched(fX, fY, Corr)){
          // check: what does it mean?
        }else if(pe_1st == 0) {
	  ElementMouseAction A(pd);
	  A->setObsoleteType(isDiagramResize);
	  return A;
        }else{
        }

        if(pd->getSelected(x, y))
        {
            if(pd->Name[0] == 'T')     // tabular, timing diagram or truth table ?
            {
                if(pd->Name[1] == 'i')
                {
                    if(y > pd->cy_())
                    {
                        if(x < pd->cx_()+pd->xAxis.numGraphs) continue;
                        ElementMouseAction A(pd);
                        A->setObsoleteType(isDiagramHScroll);
                        return A;
                    }
                }
                else
                {
                    if(x < pd->cx_())        // clicked on scroll bar ?
                    {
                        ElementMouseAction A(pd);
                        A->setObsoleteType(isDiagramVScroll);
                        return A;
                    }
                }
            }

            // test graphs of diagram
            foreach(Graph *pg, pd->Graphs)
            {
                if(pg->getSelected(x-pd->cx_(), pd->cy_()-y) >= 0)
                {
                    if(flag) {
                        // The element can be deselected
                        pg->toggleSelected();

                        ElementMouseAction A(pg);
                        return A;
                    }else if(pe_sel) {
			pe_sel->setSelected(false);
                        return ElementMouseAction(pg);
                    }else if(pe_1st == 0) {
                        pe_1st = pg;   // access to elements lying beneath
                    }
                    if(pg->isSelected())
                    {
                        pe_sel = pg;
                    }
                }
            }

            if(flag)
            {
                // The element can be deselected
                pd->toggleSelected();
                return ElementMouseAction(pd);
            }
            if(pe_sel)
            {
                pe_sel->setSelected(false);
                return ElementMouseAction(pd);
            }
            if(pe_1st == 0)
            {
                pe_1st = pd;    // give access to elements lying beneath
            }
            if(pd->isSelected())
            {
                pe_sel = pd;
            }
        }
    }

    // test all paintings
    for(Painting *pp = Doc->paintings().last(); pp!=nullptr; pp=Doc->paintings().prev())
    {
        if(pp->isSelected())
        {
            if(pp->resizeTouched(fX, fY, Corr))
            {
                if(pe_1st == 0)
                {
                    pp->Type = isPaintingResize;
                    return ElementMouseAction(pp);
                }
            }
        }

        if(pp->getSelected(fX, fY, Corr))
        {
            if(flag)
            {
                // The element can be deselected
                pp->toggleSelected();
                return ElementMouseAction(pp);
            }
            if(pe_sel)
            {
                pe_sel->setSelected(false);
                return ElementMouseAction(pp);
            }
            if(pe_1st == 0)
            {
                pe_1st = pp;    // give access to elements lying beneath
            }
            if(pp->isSelected())
            {
                pe_sel = pp;
            }
        }
    }

    return ElementMouseAction(pe_1st);
#endif
}

void Schematic::highlightWireLabels ()
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
            if (pltestouter->isSelected()) {
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
            if (pltestouter->isSelected()) {
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

// ---------------------------------------------------
// Deselects all elements except 'e'.
// bug: why do we not have a list of selected elements?!
void MouseActions::deselectElements(Schematic* Doc, ElementMouseAction e)
{
#ifndef USE_SCROLLVIEW
incomplete();
#else
    // test all components
    for(auto* pc : Doc->components()){
        if(e != pc)  pc->setSelected(false);
    }

    // test all wires
    for(auto *pw : Doc->wires()) {
        if(e != pw)  pw->setSelected(false);
        if(pw->Label) if(e != pw->Label)  pw->Label->setSelected(false);
    }

    // test all node labels
    for(auto *pn : Doc->nodes()){
        if(pn->Label) if(e != pn->Label)  pn->Label->setSelected(false);
    }

    // test all diagrams
    for(auto *pd : Doc->diagrams())
    {
        if(e != pd)  pd->setSelected(false);

        // test graphs of diagram
        foreach(Graph *pg, pd->Graphs)
        {
            if(e != pg) pg->setSelected(false);

            // test markers of graph
            foreach(Marker *pm, pg->Markers)
                if(e != pm) pm->setSelected(false);
        }

    }

    // test all paintings
    for(auto *pp : Doc->paintings()){
        if(e != pp)  pp->setSelected(false);
    }
#endif
}

// ---------------------------------------------------
// flags elements that lie within the rectangle x1/y1, x2/y2.
// return the number of elements selected.
int Schematic::selectElements(int x1, int y1, int x2, int y2, bool flag)
{
#if 0 && QT_VERSION >= 0x050000
    //do something smarter
    incomplete();
    return 0;
#endif
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
                        pc->setSelected();
                        z++;
                        continue;
                    }
        if(!flag){
	  pc->setSelected(false);
	}else if(pc->isSelected()){
	  z++;
	}
    }


    Wire *pw;
    for(pw = Wires->first(); pw != 0; pw = Wires->next())   // test all wires
    {
        if(  (pw->x1_() >= x1)
	  && (pw->x2_() <= x2)
	  && (pw->y1_() >= y1)
	  && (pw->y2_() <= y2) ) {
	    pw->setSelected(false);
            z++;
        }else{
	    if(!flag){
		pw->setSelected(false);
	    }else if(pw->isSelected()){
		z++;
	    }
	}
    }


    // test all wire labels *********************************
    WireLabel *pl=0;
    for(pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        if(pw->Label)
        {
            pl = pw->Label;
            if(pl->x1_() >= x1) if((pl->x1_()+pl->x2_()) <= x2)
                    if(pl->y1_() >= y1) if((pl->y1_()+pl->y2_()) <= y2)
                        {
                            pl->setSelected();
                            z++;
                            continue;
                        }
	    if(!flag){
	      pl->setSelected(false);
	    }else if(pl->isSelected()){
	      z++;
	    }
        }
    }


    // test all node labels *************************************
    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    {
        pl = pn->Label;
        if(pl)
        {
            if(pl->x1_() >= x1) if((pl->x1_()+pl->x2_()) <= x2)
                    if((pl->y1_()-pl->y2_()) >= y1) if(pl->y1_() <= y2)
                        {
                            pl->setSelected();
                            z++;
                            continue;
                        }
	    if(!flag){
	      pl->setSelected(false);
	    }else if(pl->isSelected()){
	      z++;
	    }
        }
    }


    // test all diagrams *******************************************
    for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
    {
        // test graphs of diagram
        foreach(Graph *pg, pd->Graphs)
        {
	    if(!flag){
	      pg->setSelected(false);
	    }else if(pg->isSelected()){
	      z++;
	    }

            // test markers of graph
            foreach(Marker *pm, pg->Markers)
            {
                pm->Bounding(cx1, cy1, cx2, cy2);
                if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                            {
                                pm->setSelected();
                                z++;
                                continue;
                            }
		if(!flag){
		  pm->setSelected(false);
		}else if(pm->isSelected()){
		  z++;
		}
            }
        }

        // test diagram itself
        pd->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pd->setSelected();
                        z++;
                        continue;
                    }
	if(!flag){
	  pd->setSelected(false);
	}else if(pd->isSelected()){
	  z++;
	}
    }

    // test all paintings *******************************************
    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
    {
        pp->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pp->setSelected();
                        z++;
                        continue;
                    }
	if(!flag){
	  pp->setSelected(false);
	}else if(pp->isSelected()){
	  z++;
	}
    }

    return z;
}

// ---------------------------------------------------
// Selects all markers.
void Schematic::selectMarkers()
{
    for(Diagram *pd = Diagrams->first(); pd != 0; pd = Diagrams->next())
        foreach(Graph *pg, pd->Graphs)
            foreach(Marker *pm, pg->Markers)
                pm->setSelected();
}

// ---------------------------------------------------
// For moving elements: If the moving element is connected to a not
// moving element, insert two wires. If the connected element is already
// a wire, use this wire. Otherwise create new wire.
void Schematic::newMovingWires(Q3PtrList<Element> *p, Node *pn, int pos)
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
        if(pe->Type != isWire)  // is it connected to exactly one wire ?
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
                if(pe2->Type == isWire)
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
                          new Wire(pw->x2_(), pw->y2_(), pw->x2_(), pw->y2_(), (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            else
                p->insert(pos,
                          new Wire(pw->x1_(), pw->y1_(), pw->x1_(), pw->y1_(), (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
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
    p->insert(pos, new Wire(pn->cx_(), pn->cy_(), pn->cx_(), pn->cy_(), (Node*)0, (Node*)1));
    // x1, x2, y2 moving
    p->insert(pos, new Wire(pn->cx_(), pn->cy_(), pn->cx_(), pn->cy_(), (Node*)1, (Node*)3));
}

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
// returns the number of "copied" _Markers_ only
Q3PtrList<ElementGraphics> Schematic::cropSelectedElements()
{
    Q3PtrList<ElementGraphics> P;
    auto p=&P;

#ifndef USE_SCROLLVIEW
    assert(scene());
    for(auto i : scene()->selectedItems()){
	if(auto x=dynamic_cast<ElementGraphics*>(i)){
	    p->append(x);
	}else{
	    unreachable();
	}
    }

    return P;
#else // does not work with ElementGraphics
    int i, count = 0;
    Component *pc;
    Wire      *pw;
    Diagram   *pd;
    Element   *pe;
    Node      *pn;

    // test all components *********************************
    // Insert components before wires in order to prevent short-cut removal.
    for(pc = Components->first(); pc != 0; )
        if(pc->isSelected())
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
        if(pw->Label) if(pw->Label->isSelected())
                p->append(pw->Label);

        if(pw->isSelected())
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
        if(pe->isSelected())
            break;

    for(pw = (Wire*)pe; pw != 0; pw = (Wire*)p->next())
        if(pw->Type == isWire)    // not working on labels
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
                pn->Label->Type = isMovingLabel;
                if(pn->State & 1)
                {
                    if(!(pn->State & 2)) pn->Label->Type = isHMovingLabel;
                }
                else if(pn->State & 2) pn->Label->Type = isVMovingLabel;
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
        if(pn->Label) if(pn->Label->isSelected())
                p->append(pn->Label);


    // test all paintings **********************************
    for(Painting *ppa = Paintings->first(); ppa != 0; )
        if(ppa->isSelected())
        {
            p->append(ppa);
            Paintings->take();
            ppa = Paintings->current();
        }
        else ppa = Paintings->next();

    count = 0;  // count markers now
    // test all diagrams **********************************
    for(pd = Diagrams->first(); pd != 0; )
        if(pd->isSelected())
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
                    if(pm->isSelected())
                    {
                        count++;
                        p->append(pm);
                    }
                }
            }

            pd = Diagrams->next();
        }

    return P;
#endif
}

// ---------------------------------------------------
bool Schematic::copyComps2WiresPaints(int& x1, int& y1, int& x2, int& y2,
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
int Schematic::copyElements(int& x1, int& y1, int& x2, int& y2,
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
        if(pd->isSelected())
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
        if(pp->isSelected())
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
// return what?
bool Schematic::deleteElements()
{
#ifndef USE_SCROLLVIEW
    // todo: elements seem to be entangled, see below old code.
//    scene()->selectionChanged();
    for(auto ge : scene()->selectedItems()){ untested();
	qDebug() << "select" << ge << ge->isSelected();
	if(auto e=dynamic_cast<ElementGraphics*>(ge)){
	    scene()->removeItem(ge);

	    // todo: forward delete to schematicModel (or so) container
	    if(auto c=component(e)){ untested();
		deleteComp(c);
	    }else if(auto w=wire(e)){ untested();
		deleteWire(w);
	    }else{
		incomplete();
	    }
	}else{
	    unreachable();
	    incomplete();
	}
    }
    return true; // ?
#else
    bool sel = false;

    Component *pc = Components->first();
    while(pc != 0)      // all selected component
        if(pc->isSelected())
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
            if(pw->Label->isSelected())
            {
                delete pw->Label;
                pw->Label = 0;
                sel = true;
            }

        if(pw->isSelected())
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
            if(pn->Label->isSelected())
            {
                delete pn->Label;
                pn->Label = 0;
                sel = true;
            }

    Diagram *pd = Diagrams->first();
    while(pd != 0)      // test all diagrams
        if(pd->isSelected())
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
                    if(pm->isSelected())
                    {
                        im.remove();
                        sel = true;
                    }
                }

                if(pg->isSelected())
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
        if(pp->isSelected())
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
	sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
        setChanged(sel, true);
    }
    return sel;
#endif
}

// ---------------------------------------------------
/*!
 * \brief Schematic::aligning align selected elements.
 * \param Mode: top, bottom, left, right, center vertical, center horizontal
 * \return True if aligned
 */
bool Schematic::aligning(int Mode)
{
    int x1, y1, x2, y2;
    int bx1, by1, bx2, by2, *bx=0, *by=0, *ax=0, *ay=0;
    QList<Element *> ElementCache;
    int count = copyElements(x1, y1, x2, y2, &ElementCache);
    if(count < 1) return false;


    ax = ay = &x2;  // = 0
    switch(Mode) {
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
	auto W=dynamic_cast<WireLabel*>(pe);

        switch(pe->Type) {
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
            bx1 = pw->x1_();
            by1 = pw->y1_();
            bx2 = pw->x2_();
            by2 = pw->y2_();
            pw->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
//        if(pw->Label) {  }
            insertWire(pw);
            break;

        case isDiagram:
            // Should the axis label be counted for ? I guess everyone
            // has a different opinion.
//        ((Diagram*)pe)->Bounding(bx1, by1, bx2, by2);

            // Take size without axis label.
            bx1 = ((Diagram*)pe)->cx_();
            by2 = ((Diagram*)pe)->cy_();
            bx2 = bx1 + ((Diagram*)pe)->x2_();
            by1 = by2 - ((Diagram*)pe)->y2_();
            ((Diagram*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isPainting:
            ((Painting*)pe)->Bounding(bx1, by1, bx2, by2);
            ((Painting*)pe)->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            break;

        case isNodeLabel:
            if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
            {
                pc = (Component*)(((WireLabel*)pe)->pOwner);
                pc->Bounding(bx1, by1, bx2, by2);
            }
            else
            {
                pw = (Wire*)(((WireLabel*)pe)->pOwner);
                bx1 = pw->x1_();
                by1 = pw->y1_();
                bx2 = pw->x2_();
                by2 = pw->y2_();
            }
	    assert(W);
            W->cx__() += x1-((*bx)+(*ax))/y2;
            W->cx__() += y1-((*by)+(*ay))/y2;
            insertNodeLabel(W);
            break;

        default:
            ;
        }
    }

    ElementCache.clear();
    if(count < 2) return false;

    setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
bool Schematic::distributeHorizontal()
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
    int x = x2;
    int dx=0;
    if(count > 1) dx = (x2-x1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();

    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
	auto C=dynamic_cast<Component*>(pe);
	auto pw=dynamic_cast<Wire*>(pe);
	auto pd=dynamic_cast<Diagram*>(pe);

        switch(pe->Type) {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            assert(C);

            C->setCenter(x, C->cy_());
            insertRawComponent(C);
            break;

        case isWire:
	    assert(pw);
            if(pw->isHorizontal()) {
                x1 = pw->x2_() - pw->x1_();
                pw->x1__() = x - (x1 >> 1);
                pw->x2__() = pw->x1_() + x1;
            }else{
	      pw->x1__() = pw->x2__() = x;
	    }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
            pd->cx__() = x - (pe->x2_() >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(x, by1, false);
            break;

        case isNodeLabel:
            pl = (WireLabel*)pe;
            if(auto oc=dynamic_cast<Component*>(pl->pOwner)){
		//pe->cx += x - ((Component*)(pl->pOwner))->cx;
		pl->setCenter(x - oc->cx_(), 0, true);
	
	    }else if((pw=dynamic_cast<Wire*>(pl->pOwner))){
                if(pw->isHorizontal()) {
                    x1 = pw->x2_() - pw->x1_();
                    pl->cx__() += x - (x1 >> 1) - pw->x1_();
                } else {
		  pl->cx__() += x - pw->x1_();
		}
            }else{
	      // unreachable();
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

    setChanged(true, true);
    return true;
}

/*!
 * \brief Schematic::distributeVertical sort selection vertically.
 * \return True if sorted
 */
bool Schematic::distributeVertical()
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
    int y  = y2;
    int dy=0;
    if(count > 1) dy = (y2-y1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    elementCacheIter.toBack();
    while (elementCacheIter.hasPrevious())
    {
        pe = elementCacheIter.previous();
        auto C=dynamic_cast<Component*>(pe);
        auto pw=dynamic_cast<Wire*>(pe);
        auto pd=dynamic_cast<Diagram*>(pe);
	auto pl=dynamic_cast<WireLabel*>(pe);

        switch(pe->Type) { // FIXME.
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            assert(C);
            C->setCenter(C->cx_(), y);
            insertRawComponent(C);
            break;

        case isWire:
            if(pw->isHorizontal())  pw->y1__() = pw->y2__() = y;
            else
            {
                y1 = pw->y2_() - pw->y1_();
                pw->y1__() = y - (y1 >> 1);
                pw->y2__() = pe->y1_() + y1;
            }
//        if(pw->Label) {	}
            insertWire(pw);
            break;

        case isDiagram:
	    assert(pd);
            pd->cx__() = y + (pe->y2_() >> 1);
            break;

        case isPainting:
            pe->getCenter(bx1, by1);
            pe->setCenter(bx1, y, false);
            break;

        case isNodeLabel:
            // if(((Element*)(((WireLabel*)pe)->pOwner))->Type & isComponent)
            if(auto oc=dynamic_cast<Component*>(pl->pOwner)){
	        oc->setCenter(y - oc->cx_(), 0, true);
	    }else if((pw=dynamic_cast<Wire*>(pl->pOwner))){
                if(!pw->isHorizontal())
                {
                    y1 = pw->y2_() - pw->y1_();
                    pl->cx__() += y - (y1 >> 1) - pw->y1_();
                }else{
                    pl->cx__() += y - pw->y1_();
		}
            }else{
	      // unreachable
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

    setChanged(true, true);
    return true;
}


/* *******************************************************************
   *****                                                         *****
   *****                Actions with components                  *****
   *****                                                         *****
   ******************************************************************* */

// Finds the correct number for power sources, subcircuit ports and
// digital sources and sets them accordingly.
void Schematic::setComponentNumber(Component *c)
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
void Schematic::insertComponentNodes(Component *c, bool noOptimize)
{
    // simulation components do not have ports
    if (c->Ports.empty()) return;

    // connect every node of the component to corresponding schematic node
    foreach(Port *pp, c->Ports)
        pp->Connection = insertNode(pp->x+c->cx_(), pp->y+c->cy_(), c);

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
            if(pe->Type == isWire)
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
void Schematic::insertRawComponent(Component *c, bool noOptimize)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, noOptimize);
    Components->append(c);

#ifndef USE_SCROLLVIEW
    // huh, where is the original EG?
    scene()->addItem(new ElementGraphics(c));
#endif

    // a ground symbol erases an existing label on the wire line
    if(c->obsolete_model_hack() == "GND") { // BUG.
        c->gnd_obsolete_model_override_hack("x");
        Element *pe = getWireLabel(c->Ports.first()->Connection);
        if(pe) if((pe->Type & isComponent) == 0)
            {
                delete ((Conductor*)pe)->Label;
                ((Conductor*)pe)->Label = 0;
            }
        c->gnd_obsolete_model_override_hack("GND");
    }
}

// ---------------------------------------------------
void Schematic::recreateComponent(Component *Comp)
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
    int x1 = Comp->x1_(), x2 = Comp->x2_(), y1 = Comp->y1_(), y2 = Comp->y2_();
    QString tmp = Comp->name();    // is sometimes changed by "recreate"
    Comp->recreate(this);   // to apply changes to the schematic symbol
    Comp->obsolete_name_override_hack(tmp);
    if(x < x1)
        x += Comp->x1_() - x1;
    else if(x > x2)
        x += Comp->x2_() - x2;
    if(y < y1)
        y += Comp->y1_() - y1;
    else if(y > y2)
        y += Comp->y2_() - y2;
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
                (*pl)->cx__() = pp->Connection->cx_();
                (*pl)->cx__() = pp->Connection->cx_();
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
void Schematic::insertComponent(Component *c)
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
            if(pe) if((pe->Type & isComponent) == 0)
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
void Schematic::activateCompsWithinRect(int x1, int y1, int x2, int y2)
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

    if(changed)  setChanged(true, true);
}

// ---------------------------------------------------
bool Schematic::activateSpecifiedComponent(int x, int y)
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
                        setChanged(true, true);
                        return true;
                    }
    }
    return false;
}

// ---------------------------------------------------
bool Schematic::activateSelectedComponents()
{
    int a;
    bool sel = false;
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
        if(pc->isSelected())
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

    if(sel) setChanged(true, true);
    return sel;
}

// ---------------------------------------------------
// Sets the component ports anew. Used after rotate, mirror etc.
void Schematic::setCompPorts(Component *pc)
{
    WireLabel *pl;
    Q3PtrList<WireLabel> LabelCache;

    foreach(Port *pp, pc->Ports)
    {
        pp->Connection->Connections.removeRef((Element*)pc);// delete connections
        switch(pp->Connection->Connections.count()) {
        case 0:
            pl = pp->Connection->Label;
            if(pl)
            {
                LabelCache.append(pl);
                pl->cx__() = pp->x + pc->cx_();
                pl->cx__() = pp->y + pc->cx_();
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
        pp->Connection = insertNode(pp->x+pc->cx_(), pp->y+pc->cx_(), pc);

    for(pl = LabelCache.first(); pl != 0; pl = LabelCache.next())
        insertNodeLabel(pl);
}

// ---------------------------------------------------
// Returns a pointer of the component on whose text x/y points.
Component* MouseActions::selectCompText(Schematic* Doc, int x_, int y_, int& w, int& h)
{
    int a, b, dx, dy;
    for(auto *pc : Doc->components()) {
        a = pc->cx_() + pc->tx;
        if(x_ < a)  continue;
        b = pc->cx_() + pc->ty;
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
Component* Schematic::searchSelSubcircuit()
{
    Component *sub=0;
    // test all components
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
    {
        if(!pc->isSelected()) continue;

        if(pc->obsolete_model_hack() != "Sub"){
            if(pc->obsolete_model_hack() != "VHDL")
                if(pc->obsolete_model_hack() != "Verilog") continue;
	}

        if(sub != 0) return 0;    // more than one subcircuit selected
        sub = pc;
    }
    return sub;
}

// ---------------------------------------------------
Component* Schematic::selectedComponent(int x, int y)
{
    // test all components
    for(Component *pc = Components->first(); pc != 0; pc = Components->next())
        if(pc->getSelected(x, y))
            return pc;

    return 0;
}

// ---------------------------------------------------
// Deletes the component 'c'.
void Schematic::deleteComp(Component *c)
{
    // delete all port connections
    foreach(Port *pn, c->Ports)
        switch(pn->Connection->Connections.count()) {
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
int Schematic::copyComponents(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    Component *pc;
    int bx1, by1, bx2, by2, count=0;
    // find bounds of all selected components
    for(pc = Components->first(); pc != 0; )
    {
        if(pc->isSelected())
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
void Schematic::copyComponents2(int& x1, int& y1, int& x2, int& y2,
                                QList<Element *> *ElementCache)
{
    Component *pc;
    // find bounds of all selected components
    for(pc = Components->first(); pc != 0; )
    {
        if(pc->isSelected())
        {
            // is better for unsymmetrical components
            if(pc->cx_() < x1)  x1 = pc->cx_();
            if(pc->cx_() > x2)  x2 = pc->cx_();
            if(pc->cx_() < y1)  y1 = pc->cx_();
            if(pc->cx_() > y2)  y2 = pc->cx_();

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
void Schematic::oneLabel(Node *n1)
{
    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    WireLabel *pl = 0;
    bool named=false;   // wire line already named ?
    Q3PtrList<Node> Cons;

    for(pn = Nodes->first(); pn!=0; pn = Nodes->next()){
        pn->markUnChecked(); // y1__() = 0;
    }

    Cons.append(n1);
    n1->markChecked(); // y1_() = 1; 
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
            if(pe->Type != isWire)
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

            if(pNode->y1_()) continue;
            pNode->markChecked(); // y1_() = 1;
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
int Schematic::placeNodeLabel(WireLabel *pl)
{
    Node *pn;
    int x = pl->cx_();
    int y = pl->cx_();

    // check if new node lies upon an existing node
    for(pn = Nodes->first(); pn != 0; pn = Nodes->next())
        if(pn->cx_() == x) if(pn->cy_() == y) break;

    if(!pn)  return -1;

    Element *pe = getWireLabel(pn);
    if(pe)      // name found ?
    {
        if(pe->Type & isComponent)
        {
            delete pl;
            return -2;  // ground potential
        }

        delete ((Conductor*)pe)->Label;
        ((Conductor*)pe)->Label = 0;
    }

    pn->Label = pl;   // insert node label
    pl->Type = isNodeLabel;
    pl->pOwner = pn;
    return 0;
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
Element* Schematic::getWireLabel(Node *pn_)
{
    Wire *pw;
    Node *pn, *pNode;
    Element *pe;
    Q3PtrList<Node> Cons;

    for(pn = Nodes->first(); pn!=0; pn = Nodes->next()){
        pn->markUnChecked();
    }

    Cons.append(pn_);
    pn_->markChecked();
    for(pn = Cons.first(); pn!=0; pn = Cons.next())
        if(pn->Label) return pn;
        else
            for(pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next())
            {
                if(pe->Type != isWire)
                {
                    if(((Component*)pe)->isActive == COMP_IS_ACTIVE)
                        if(((Component*)pe)->obsolete_model_hack() == "GND") return pe;
                    continue;
                }

                pw = (Wire*)pe;
                if(pw->Label) return pw;

                if(pn != pw->Port1) pNode = pw->Port1;
                else pNode = pw->Port2;

                if(pNode->y1_()) continue;
		pNode->markChecked();
                Cons.append(pNode);
                Cons.findRef(pn);
            }
    return 0;   // no wire label found
}

// ---------------------------------------------------
// Inserts a node label.
void Schematic::insertNodeLabel(WireLabel *pl)
{
    if(placeNodeLabel(pl) != -1)
        return;

    // Go on, if label don't lie on existing node.

    Wire *pw = selectedWire(pl->cx_(), pl->cy_());
    if(pw)    // lies label on existing wire ?
    {
        if(getWireLabel(pw->Port1) == 0)  // wire not yet labeled ?
            pw->setName(pl->Name, pl->initValue, 0, pl->cx_(), pl->cx_());

        delete pl;
        return;
    }


    Node *pn = new Node(pl->cx_(), pl->cy_());
    Nodes->append(pn);

    pn->Label = pl;
    pl->Type  = isNodeLabel;
    pl->pOwner = pn;
}

// ---------------------------------------------------
void Schematic::copyLabels(int& x1, int& y1, int& x2, int& y2,
                           QList<Element *> *ElementCache)
{
    WireLabel *pl;
    // find bounds of all selected wires
    for(Wire *pw = Wires->first(); pw != 0; pw = Wires->next())
    {
        pl = pw->Label;
        if(pl) if(pl->isSelected())
            {
                if(pl->x1_() < x1) x1 = pl->x1_();
                if(pl->y1_()-pl->y2_() < y1) y1 = pl->y1_()-pl->y2_();
                if(pl->x1_()+pl->x2_() > x2) x2 = pl->x1_()+pl->x2_();
                if(pl->y1_() > y2) y2 = pl->y1_();
                ElementCache->append(pl);
            }
    }

    for(Node *pn = Nodes->first(); pn != 0; pn = Nodes->next())
    {
        pl = pn->Label;
        if(pl) if(pl->isSelected())
            {
                if(pl->x1_() < x1) x1 = pl->x1_();
                if(pl->y1_()-pl->y2_() < y1) y1 = pl->y1_()-pl->y2_();
                if(pl->x1_()+pl->x2_() > x2) x2 = pl->x1_()+pl->x2_();
                if(pl->y1_() > y2) y2 = pl->y1_();
                ElementCache->append(pl);
                pl->pOwner->Label = 0;   // erase connection
                pl->pOwner = 0;
            }
    }
}


/* *******************************************************************
   *****                                                         *****
   *****                Actions with paintings                   *****
   *****                                                         *****
   ******************************************************************* */

// don't use this.
Painting* Schematic::selectedPainting(float fX, float fY)
{
    float Corr = 5.0 / Scale; // size of line select

    for(Painting *pp = Paintings->first(); pp != 0; pp = Paintings->next())
        if(pp->getSelected(fX, fY, Corr))
            return pp;

    return 0;
}

// ---------------------------------------------------
void Schematic::copyPaintings(int& x1, int& y1, int& x2, int& y2,
                              QList<Element *> *ElementCache)
{
    Painting *pp;
    int bx1, by1, bx2, by2;
    // find boundings of all selected paintings
    for(pp = Paintings->first(); pp != 0; )
        if(pp->isSelected())
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

// vim:ts=8:sw=4:noet
