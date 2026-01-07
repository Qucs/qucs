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
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>

#include "schematic.h"
#include <QDebug>


/* *******************************************************************
   *****                                                         *****
   *****              Actions handling the nodes                 *****
   *****                                                         *****
   ******************************************************************* */

// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
std::shared_ptr<Node> Schematic::insertNode(int x, int y, const std::shared_ptr<Element> &e)
{
    // check if new node lies upon existing node
    auto pn = Nodes->begin();
    for( ; pn != Nodes->end(); ++pn)  // check every node
        if(pn->cx() == x) if(pn->cy() == y)
            {
		pn->connect(e);
                break;
            }

    if(pn != Nodes->end()) {
      return pn.ref();
    }

    // create new node, if no existing one lies at this position
    std::shared_ptr<Node> newNode(new Node(x, y));
    Nodes->append(newNode);
    newNode->connect(e);

    // check if the new node lies upon an existing wire
    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
    {
        if(pw->x1() == x)
        {
            if(pw->y1() > y) continue;
            if(pw->y2() < y) continue;
        }
        else if(pw->y1() == y)
        {
            if(pw->x1() > x) continue;
            if(pw->x2() < x) continue;
        }
        else continue;

        // split the wire into two wires
        splitWire(pw.ref(), newNode);
        break;
    }

    return newNode;
}

// ---------------------------------------------------
Node* Schematic::selectedNode(int x, int y)
{
    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn) // test nodes
        if(pn->getSelected(x, y))
            return pn.operator->();

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
int Schematic::insertWireNode1(const std::shared_ptr<Wire> &w)
{
    auto pn = Nodes->begin();
    // check if new node lies upon an existing node
    for( ; pn != Nodes->end(); ++pn) // check every node
        if(pn->cx() == w->x1()) if(pn->cy() == w->y1()) break;

    if(pn != Nodes->end())
    {
        pn->appendConnection(std::static_pointer_cast<Element>(w));
        w->ports(0) = pn.operator->();
        return 2;   // node is not new
    }



    // check if the new node lies upon an existing wire
    for(auto ptr2 = Wires->begin(); ptr2 != Wires->end(); ++ptr2)
    {
        if(ptr2->x1() == w->x1())
        {
            if(ptr2->y1() > w->y1()) continue;
            if(ptr2->y2() < w->y1()) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                if(ptr2->y2() >= w->y2())
                {
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->ports(1)->refcount() == 1) {
                        w->set_y1(ptr2->y1());  // shorten new wire
                        w->ports(0) = ptr2->ports(0);
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->setOwner(w);
                        }
                        w->ports(0)->removeConnection(ptr2.ref());  // two -> one wire
                        w->ports(0)->appendConnection(w);
                        Nodes->erase(ptr2->ports(1));
                        Wires->erase(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->set_y1(ptr2->y2());  // shorten new wire
                        w->ports(0) = ptr2->ports(1);
                        ptr2->ports(1)->appendConnection(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else if(ptr2->y1() == w->y1())
        {
            if(ptr2->x1() > w->x1()) continue;
            if(ptr2->x2() < w->x1()) continue;

            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                if(ptr2->x2() >= w->x2())
                {
                    return 0;
                }
                else
                {
                    // one part of the wire lies within an existing wire
                    // the other part not
                    if(ptr2->ports(1)->refcount() == 1) {
                        w->set_x1(ptr2->x1());  // shorten new wire
                        w->ports(0) = ptr2->ports(0);
                        if(ptr2->Label)
                        {
                            w->Label = ptr2->Label;
                            w->Label->setOwner(w);
                        }
                        ptr2->ports(0)->removeConnection(ptr2.ref()); // two -> one wire
                        ptr2->ports(0)->appendConnection(w);
                        Nodes->erase(ptr2->ports(1));
                        Wires->erase(ptr2);
                        return 2;
                    }
                    else
                    {
                        w->set_x1(ptr2->x2());  // shorten new wire
                        w->ports(0) = ptr2->ports(1);
                        ptr2->ports(1)->connect(w);   // shorten new wire
                        return 2;
                    }
                }
            }
        }
        else continue;

        std::shared_ptr<Node> newNode(new Node(w->x1(), w->y1()));   // create new node
        Nodes->append(newNode);
        newNode->appendConnection(w);  // connect schematic node to the new wire
        w->ports(0) = newNode.get();

        // split the wire into two wires
        splitWire(ptr2.ref(), newNode);
        return 2;
    }

    std::shared_ptr<Node> newNode(new Node(w->x1(), w->y1()));   // create new node
    Nodes->append(newNode);
    newNode->appendConnection(w);  // connect schematic node to the new wire
    w->ports(0) = newNode.get();
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires1(const std::shared_ptr<Wire> &w)
{
    Node *n = w->ports(0);

    auto pw = n->connections().end();
    --pw;  // last connection is the new wire itself
    for( ; pw != n->connections().begin(); )
    {
        --pw;
        auto pws = pw->lock();
        if(pws->Type != isWire) continue;
        auto wire = std::dynamic_pointer_cast<Wire>(pws);
        if(!wire->isHorizontal()) continue;
        if(wire->x1() < w->x1())
        {
            if(n->refcount() != 2) continue;
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->setOwner(w);
                w->Label->Type = isHWireLabel;
            }
            w->set_x1(wire->x1());  // new wire lengthens an existing one
            w->ports(0) = wire->ports(0);
            Nodes->erase(n);
            w->ports(0)->removeConnection(wire);
            w->ports(0)->appendConnection(w);
            Wires->erase(wire);
            return true;
        }
        if(pws->x2() >= w->x2())    // new wire lies within an existing one ?
        {
            w->ports(0)->removeConnection(w); // second node not yet made
            return false;
        }
        if(wire->ports(1)->refcount() < 2)
        {
            // existing wire lies within the new one
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            wire->ports(0)->removeConnection(wire);
            Nodes->erase(wire->ports(1));
            Wires->erase(wire);
            return true;
        }
        w->set_x1(wire->x2());    // shorten new wire according to an existing one
        w->ports(0)->removeConnection(w);
        w->ports(0) = wire->ports(1);
        w->ports(0)->appendConnection(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires1(const std::shared_ptr<Wire> &w)
{
    Node *n = w->ports(0);

    auto pw = n->connections().end();
    --pw;  // last connection is the new wire itself
    for( ; pw != n->connections().begin(); )
    {
        --pw;
        auto pws = pw->lock();
        if(pws->Type != isWire) continue;
        auto wire = std::dynamic_pointer_cast<Wire>(pws);
        if(wire->isHorizontal()) continue;
        if(wire->y1() < w->y1())
        {
            if(n->refcount() != 2) continue;
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            else if(n->Label)
            {
                w->Label = n->Label;
                w->Label->setOwner(w);
                w->Label->Type = isVWireLabel;
            }
            w->set_y1(wire->y1());
            w->ports(0) = wire->ports(0);
            Nodes->erase(n);
            w->ports(0)->removeConnection(wire);
            w->ports(0)->appendConnection(w);
            Wires->erase(wire);
            return true;
        }
        if(wire->y2() >= w->y2())    // new wire lies complete within an existing one ?
        {
            w->ports(0)->removeConnection(w); // second node not yet made
            return false;
        }
        if(wire->ports(1)->refcount() < 2)
        {
            // existing wire lies within the new one
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            wire->ports(0)->removeConnection(wire);
            Nodes->erase(wire->ports(1));
            Wires->erase(wire);
            return true;
        }
        w->set_y1(wire->y2());    // shorten new wire according to an existing one
        w->ports(0)->removeConnection(w);
        w->ports(0) = wire->ports(1);
        w->ports(0)->appendConnection(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int Schematic::insertWireNode2(const std::shared_ptr<Wire> &w)
{
    // check if new node lies upon an existing node
    auto pn = Nodes->begin();
    for( ; pn != Nodes->end(); ++pn)  // check every node
        if(pn->cx() == w->x2()) if(pn->cy() == w->y2()) break;

    if(pn != Nodes->end())
    {
        pn->appendConnection(w);
        w->ports(1) = pn.operator->();
        return 2;   // node is not new
    }

    // check if the new node lies upon an existing wire
    for(auto ptr2 = Wires->begin(); ptr2 != Wires->end(); ++ptr2)
    {
        if(ptr2->x1() == w->x2())
        {
            if(ptr2->y1() > w->y2()) continue;
            if(ptr2->y2() < w->y2()) continue;

            // (if new wire lies within an existing wire, was already check before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is vertical
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->ports(0)->refcount() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->setOwner(w);
                    }
                    w->set_y2(ptr2->y2());  // shorten new wire
                    w->ports(1) = ptr2->ports(1);
                    ptr2->ports(1)->removeConnection(ptr2.ref());  // two -> one wire
                    ptr2->ports(1)->appendConnection(w);
                    Nodes->erase(ptr2->ports(0));
                    Wires->erase(ptr2);
                    return 2;
                }
                else
                {
                    w->set_y2(ptr2->y1());  // shorten new wire
                    w->ports(1) = ptr2->ports(0);
                    ptr2->ports(0)->appendConnection(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else if(ptr2->y1() == w->y2())
        {
            if(ptr2->x1() > w->x2()) continue;
            if(ptr2->x2() < w->x2()) continue;

            // (if new wire lies within an existing wire, was already checked before)
            if(ptr2->isHorizontal() == w->isHorizontal())   // ptr2-wire is horizontal
            {
                // one part of the wire lies within an existing wire
                // the other part not
                if(ptr2->ports(0)->refcount() == 1)
                {
                    if(ptr2->Label)
                    {
                        w->Label = ptr2->Label;
                        w->Label->setOwner(w);
                    }
                    w->set_x2(ptr2->x2());  // shorten new wire
                    w->ports(1) = ptr2->ports(1);
                    ptr2->ports(1)->removeConnection(ptr2.ref());  // two -> one wire
                    ptr2->ports(1)->appendConnection(w);
                    Nodes->erase(ptr2->ports(0));
                    Wires->erase(ptr2);
                    return 2;
                }
                else
                {
                    w->set_x2(ptr2->x1());  // shorten new wire
                    w->ports(1) = ptr2->ports(0);
                    ptr2->ports(0)->appendConnection(w);   // shorten new wire
                    return 2;
                }
            }
        }
        else continue;

        std::shared_ptr<Node> newNode(new Node(w->x2(), w->y2()));   // create new node
        Nodes->append(newNode);
        newNode->connect(w);  // connect schematic node to the new wire
        w->ports(1) = newNode.get();

        // split the wire into two wires
        splitWire(ptr2.ref(), newNode);
        return 2;
    }

    std::shared_ptr<Node> newNode(new Node(w->x2(), w->y2()));   // create new node
    Nodes->append(newNode);
    newNode->connect(w);  // connect schematic node to the new wire
    w->ports(1) = newNode.get();
    return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool Schematic::connectHWires2(const std::shared_ptr<Wire> &w)
{
    Node *n = w->ports(1);

    auto pw = n->connections().end();
    --pw;  // last connection is the new wire itself
    for( ; pw != n->connections().begin(); )
    {
        --pw;
        auto pws = pw->lock();
        if(pws->Type != isWire) continue;
        auto wire = std::dynamic_pointer_cast<Wire>(pws);
        if(!wire->isHorizontal()) continue;
        if(wire->x2() > w->x2())
        {
            if(n->refcount() != 2) continue;
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            w->set_x2(wire->x2());  // new wire lengthens an existing one
            w->ports(1) = wire->ports(1);      // new wire lengthens an existing one
            Nodes->erase(n);
            w->ports(1)->removeConnection(wire);
            w->ports(1)->appendConnection(w);
            Wires->erase(wire);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)


        {
            // existing wire lies within the new one
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            wire->ports(1)->removeConnection(wire);
            Nodes->erase(wire->ports(0));
            Wires->erase(wire);
            return true;
        }
        w->set_x2(wire->x1());    // shorten new wire according to an existing one
        w->ports(1)->removeConnection(w);
        w->ports(1) = wire->ports(0);
        w->ports(1)->appendConnection(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// if possible, connect two vertical wires to one
bool Schematic::connectVWires2(const std::shared_ptr<Wire> &w)
{
    Node *n = w->ports(1);

    auto pw = n->connections().end();
    --pw;  // last connection is the new wire itself
    for( ; pw != n->connections().begin(); )
    {
        --pw;
        auto pws = pw->lock();
        if(pws->Type != isWire) continue;
        auto wire = std::dynamic_pointer_cast<Wire>(pws);
        if(wire->isHorizontal()) continue;
        if(wire->y2() > w->y2())
        {
            if(n->refcount() != 2) continue;
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            w->set_y2(wire->y2());  // new wire lengthens an existing one
            w->ports(1) = wire->ports(1);     // new wire lengthens an existing one
            Nodes->erase(n);
            w->ports(1)->removeConnection(wire);
            w->ports(1)->appendConnection(w);
            Wires->erase(wire);
            return true;
        }
        // (if new wire lies complete within an existing one, was already
        // checked before)

        if(wire->ports(0)->refcount() < 2)
        {
            // existing wire lies within the new one
            if(wire->Label)
            {
                w->Label = wire->Label;
                w->Label->setOwner(w);
            }
            wire->ports(1)->removeConnection(wire);
            Nodes->erase(wire->ports(0));
            Wires->erase(wire);
            return true;
        }
        w->set_y2(wire->y1());    // shorten new wire according to an existing one
        w->ports(1)->removeConnection(w);
        w->ports(1) = wire->ports(0);
        w->ports(1)->appendConnection(w);
        return true;
    }

    return true;
}

// ---------------------------------------------------
// Inserts a vertical or horizontal wire into the schematic and connects
// the ports that hit together. Returns whether the beginning and ending
// (the ports of the wire) are connected or not.
int Schematic::insertWire(const std::shared_ptr<Wire> &w)
{
    int  tmp, con = 0;
    bool ok;

    // change coordinates if necessary (port 1 coordinates must be less
    // port 2 coordinates)
    if(w->x1() > w->x2())
    {
        tmp = w->x1();
        w->set_x1(w->x2());
        w->set_x2(tmp);
    }
    else if(w->y1() > w->y2())
    {
        tmp = w->y1();
        w->set_y1(w->y2());
        w->set_y2(tmp);
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

    QSet<Wire *> wiresToDelete;
    QSet<Node *> nodesToDelete;

    // ................................................................
    // Check if the new line covers existing nodes.
    // In order to also check new appearing wires -> use "for"-loop
    for(auto pw = --Wires->end(); pw != Wires->end(); ++pw) {
        for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)    // check every node
        {
            if(pn->cx() == pw->x1())
            {
                if(pn->cy() <= pw->y1())
                {
                    continue;
                }
                if(pn->cy() >= pw->y2())
                {
                    continue;
                }
            }
            else if(pn->cy() == pw->y1())
            {
                if(pn->cx() <= pw->x1())
                {
                    continue;
                }
                if(pn->cx() >= pw->x2())
                {
                    continue;
                }
            }
            else
            {
                continue;
            }

            n1 = 2;
            n2 = 3;
            Node *pn1 = pn.operator->();
            Node *pn2 = pn1;
            // check all connections of the current node
            for(auto pe = pn->connections().begin(); pe != pn->connections().end(); ++pe)
            {
                auto pes = pe->lock();
                if(pes->Type != isWire) continue;
                auto nw = std::dynamic_pointer_cast<Wire>(pes);
                // wire lies within the new ?
                if (pw->isHorizontal() != nw->isHorizontal()) continue;

                pn1 = nw->ports(0);
                pn2 = nw->ports(1);
                n1  = pn1->refcount();
                n2  = pn2->refcount();
                if(n1 == 1)
                {
                    nodesToDelete.insert(pn1);  // delete node 1 if open
                    pn2->removeConnection(nw);  // remove connection
                    pn1 = pn2;
                }

                if(n2 == 1)
                {
                    pn1->removeConnection(nw);  // remove connection
                    nodesToDelete.insert(pn2);  // delete node 2 if open
                    pn2 = pn1;
                }

                if(pn1 == pn2)
                {
                    if(nw->Label)
                    {
                        pw->Label = nw->Label;
                        pw->Label->setOwner(pw.ref());
                    }
                    wiresToDelete.insert(nw.get());
                }
                break;
            }

            if (n1 == 1 && n2 == 1) continue;

            // split wire into two wires
            if((pw->x1() != pn1->cx()) || (pw->y1() != pn1->cy()))
            {
                std::shared_ptr<Wire> newWire (new Wire(pw->x1(), pw->y1(), pn->cx(), pn->cy(), pw->ports(0), pn1));
                pn1->appendConnection(newWire);
                Wires->append(newWire);
                pw->ports(0)->appendConnection(newWire);
            }

            pw->ports(0)->removeConnection(pw.ref());
            pw->set_x1(pn2->cx());
            pw->set_y1(pn2->cy());
            pw->ports(0) = pn2;
            pn2->appendConnection(pw.ref());

        }
    }

    //  remove the obsolete wires
    for (auto i = wiresToDelete.begin(); i != wiresToDelete.end(); ++i) {
      Wires->erase(*i);
    }

    //  remove the obsolete nodes
    for (auto i = nodesToDelete.begin(); i != nodesToDelete.end(); ++i) {
      Nodes->erase(*i);
    }

    if (Wires->find(w.get()) != Wires->end())  // if two wire lines with different labels ...
        oneLabel(w->ports(0));       // ... are connected, delete one label
    return con | 0x0200;   // sent also end flag
}

// ---------------------------------------------------
// Follows a wire line and selects it.
void Schematic::selectWireLine(const std::shared_ptr<Element> &_pe, Node *pn, bool ctrl)
{
    std::shared_ptr<Element> pe = _pe;
    //  as we assign pn from ports(0) or ports(1), we have to use pointers
    Node *pn_1st = pn;
    while(pn->refcount() == 2)
    {
        if(pn->connections().front().lock() == pe)
          pe = pn->connections().back().lock();
        else
          pe = pn->connections().front().lock();

        if(pe->Type != isWire) break;
        if(ctrl) pe->isSelected ^= ctrl;
        else pe->isSelected = true;

        auto pw = std::dynamic_pointer_cast<Wire>(pe);
        if(pw->ports(0) == pn)  pn = pw->ports(1);
        else  pn = pw->ports(0);
        if(pn == pn_1st) break;  // avoid endless loop in wire loops
    }
}

// ---------------------------------------------------
std::shared_ptr<Wire> Schematic::selectedWire(int x, int y)
{
    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
        if(pw->getSelected(x, y))
            return pw.ref();

    return 0;
}

// ---------------------------------------------------
// Splits the wire "*pw" into two pieces by the node "*pn".
std::shared_ptr<Wire> Schematic::splitWire(const std::shared_ptr<Wire> &pw, const std::shared_ptr<Node> &pn)
{
    std::shared_ptr<Wire> newWire(new Wire(pn->cx(), pn->cy(), pw->x2(), pw->y2(), pn.get(), pw->ports(1)));
    newWire->isSelected = pw->isSelected;

    pw->set_x2(pn->cx());
    pw->set_y2(pn->cy());
    pw->ports(1) = pn.get();

    newWire->ports(1)->connect(newWire);
    pn->connect(pw);
    pn->connect(newWire);
    newWire->ports(1)->removeConnection(pw);
    Wires->append(newWire);

    if(pw->Label)
        if((pw->Label->cx() > pn->cx()) || (pw->Label->cy() > pn->cy()))
        {
            newWire->Label = pw->Label;   // label goes to the new wire
            pw->Label = 0;
            newWire->Label->setOwner(newWire);
        }

    return newWire;
}

// ---------------------------------------------------
// If possible, make one wire out of two wires.
bool Schematic::oneTwoWires(const std::shared_ptr<Node> &n)
{
    auto e1 = n->connections().front().lock();  // two wires -> one wire
    auto e2 = n->connections().back().lock();

    if(e1->Type == isWire && e2->Type == isWire)
    {
        auto w1 = std::static_pointer_cast<Wire> (e1);
        auto w2 = std::static_pointer_cast<Wire> (e2);
        if(w1->isHorizontal() == w2->isHorizontal())
        {
            if(e1->x1() == e2->x2() && e1->y1() == e2->y2())
            {
                w1.swap(w2);  // e1 must have lesser coordinates
            }
            if(w2->Label)     // take over the node name label ?
            {
                w1->Label = w2->Label;
                w1->Label->setOwner(w1);
            }
            else if(n->Label)
            {
                w1->Label = n->Label;
                w1->Label->setOwner(w1);
                if(w1->isHorizontal())
                    w1->Label->Type = isHWireLabel;
                else
                    w1->Label->Type = isVWireLabel;
            }

            w1->set_x2(w2->x2());
            w1->set_y2(w2->y2());
            w1->ports(1) = w2->ports(1);
            Nodes->erase(n);    // delete node (is auto delete)
            w1->ports(1)->removeConnection(w2);
            w1->ports(1)->appendConnection(w1);
            Wires->erase(w2);
            return true;
        }
    }
    return false;
}

// ---------------------------------------------------
// Deletes the wire 'w'.
void Schematic::deleteWire(const WireList::iterator &w)
{
    if(w->ports(0)->refcount() == 1)
    {
        Nodes->erase(w->ports(0));     // delete node 1 if open
    }
    else
    {
        w->ports(0)->removeConnection(w.ref());   // remove connection
        if(w->ports(0)->refcount() == 2) {
            auto n1 = Nodes->find(w->ports(0));
            assert(n1 != Nodes->end());
            oneTwoWires(n1.ref());  // two wires -> one wire
        }
    }

    if(w->ports(1)->refcount() == 1)
    {
        Nodes->erase(w->ports(1));     // delete node 2 if open
    }
    else
    {
        w->ports(1)->removeConnection(w.ref());   // remove connection
        if(w->ports(1)->refcount() == 2) {
            auto n2 = Nodes->find(w->ports(1));
            assert(n2 != Nodes->end());
            oneTwoWires(n2.ref());  // two wires -> one wire
        }
    }

    Wires->erase(w);
}

// ---------------------------------------------------
int Schematic::copyWires(int& x1, int& y1, int& x2, int& y2,
                         SharedObjectList<Element> &ElementCache)
{
    int count=0;
    Node *pn;
    for(auto pwn = Wires->begin(); pwn != Wires->end(); ) { // find bounds of all selected wires
        auto pw = pwn;
        ++pwn;
        if(pw->isSelected)
        {
            if(pw->x1() < x1) x1 = pw->x1();
            if(pw->x2() > x2) x2 = pw->x2();
            if(pw->y1() < y1) y1 = pw->y1();
            if(pw->y2() > y2) y2 = pw->y2();

            count++;
            ElementCache.append(pw.ref());

            // rescue non-selected node labels
            pn = pw->ports(0);
            if(pn->Label)
                if(pn->refcount() < 2)
                {
                    ElementCache.append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->setOwner(pw.ref());
                    pn->Label = 0;
                }
            pn = pw->ports(1);
            if(pn->Label)
                if(pn->refcount() < 2)
                {
                    ElementCache.append(pn->Label);

                    // Don't set pn->Label->pOwner=0 , so text position stays unchanged.
                    // But remember its wire.
                    pn->Label->setOwner(pw.ref());
                    pn->Label = 0;
                }

            deleteWire(pw);
        }
    }

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
  for(auto pd = Diagrams->end(); pd != Diagrams->begin(); ) {
    --pd;
    if(Marker* m=pd->setMarker(x,y)){
      setChanged(true, true);
      return m;
    }
  }
  return NULL;
}

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
void Schematic::markerLeftRight(bool left, SharedObjectList<Element> &Elements)
{
    bool acted = false;
    for (auto i = Elements.begin(); i != Elements.end(); ++i) {
        Marker* pm = dynamic_cast<Marker*>(i.operator->());
        if(pm && pm->moveLeftRight(left))
            acted = true;
    }

    if(acted)
        setChanged(true, true, 'm');
}

// ---------------------------------------------------
// Moves the marker pointer up/down on the more-dimensional graph.
void Schematic::markerUpDown(bool up, SharedObjectList<Element> &Elements)
{
    bool acted = false;
    for (auto i = Elements.begin(); i != Elements.end(); ++i) {
        Marker* pm = dynamic_cast<Marker*>(i.operator->());
        if(pm && pm->moveUpDown(up))
            acted = true;
    }

    if(acted)
        setChanged(true, true, 'm');
}


/* *******************************************************************
   *****                                                         *****
   *****               Actions with all elements                 *****
   *****                                                         *****
   ******************************************************************* */

/* Selects the element that contains the coordinates x/y.
   Returns the pointer to the element.

   If 'flag' is true, the element can be deselected. If
   'flag' is false the element cannot be deselected. The
   purpose of this is to prevent deselection in cases such
   as right-clicking on a selected element to get a context
   menu.
*/
//  FIXME: use weak_ptr for return value
std::shared_ptr<Element> Schematic::selectElement(float fX, float fY, bool flag, int *index)
{
    int n, x = int(fX), y = int(fY);
    std::shared_ptr<Element> pe_1st;
    std::shared_ptr<Element> pe_sel;
    float Corr = textCorr(); // for selecting text

    // test all nodes and their labels
    for(auto pn = Nodes->end(); pn != Nodes->begin(); )
    {
        --pn;
        if(!flag)
        {
            // The element cannot be deselected
            if(index)
            {
                // 'index' is only true if called from MouseActions::MPressSelect()
                if(pn->getSelected(x, y))
                {
                    // Return the node pointer, as the selection cannot change
                    return pn.ref();
                }
            }
        }

        auto pl = pn->Label; // Get any wire label associated with the Node
        if(pl)
        {
            if(pl->getSelected(x, y))
            {
                if(flag)
                {
                    // The element can be deselected, so toggle its isSelected member
                    // TODO: I don't see a need for the xor here, a simple ! on the current value
                    // would be clearer and have the same effect?
                    pl->isSelected ^= flag;
                    return pl;
                }
                if(pe_sel)
                {
                    // There is another currently
                    pe_sel->isSelected = false;
                    return pl;
                }
                if(pe_1st == 0)
                {
                    // give access to elements lying beneath by storing this label.
                    // If no label pointer (or other element) has previously been
                    // stored, the current label pointer is stored here.
                    // pe_1st is returned if no other selected element
                    pe_1st = pl;
                }
                if(pl->isSelected)
                {
                    // if current label is already selected, store a pointer to it.
                    // This can be used to cycle through
                    pe_sel = pl;
                }
            }
        }
    }

    // test all wires and wire labels
    for(auto pw = Wires->end(); pw != Wires->begin(); )
    {
        --pw;
        if(pw->getSelected(x, y))
        {
            if(flag)
            {
                // The element can be deselected
                pw->isSelected ^= flag;
                return pw.ref();
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pw.ref();
            }
            if(pe_1st == 0)
            {
                pe_1st = pw.ref();   // give access to elements lying beneath
            }
            if(pw->isSelected)
            {
                pe_sel = pw.ref();
            }
        }
        auto pl = pw->Label; // test any label associated with the wire
        if(pl)
        {
            if(pl->getSelected(x, y))
            {
                if(flag)
                {
                    // The element can be deselected
                    pl->isSelected ^= flag;
                    return pl;
                }
                if(pe_sel)
                {
                    pe_sel->isSelected = false;
                    return pl;
                }
                if(pe_1st == 0)
                {
                    // give access to elements lying beneath
                    pe_1st = pl;
                }
                if(pl->isSelected)
                {
                    pe_sel = pl;
                }
            }
        }
    }

    // test all components
    for(auto pc = Components->end(); pc != Components->begin(); )
    {
        --pc;
        if(pc->getSelected(x, y))
        {
            if(flag)
            {
                // The element can be deselected
                pc->isSelected ^= flag;
                return pc.ref();
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pc.ref();
            }
            if(pe_1st == 0)
            {
                pe_1st = pc.ref();
            }  // give access to elements lying beneath
            if(pc->isSelected)
            {
                pe_sel = pc.ref();
            }
        }
        else
        {
            n = pc->getTextSelected(x, y, Corr);
            if(n >= 0)     // was property text clicked ?
            {
                pc->Type = isComponentText;
                if(index)  *index = n;
                return pc.ref();
            }
        }
    }

    Corr = 5.0 / Scale;  // size of line select and area for resizing
    // test all diagrams
    for(auto pd = Diagrams->end(); pd != Diagrams->begin(); )
    {
        --pd;
        for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
        {
            // test markers of graphs
            for(auto pm = pg->Markers.begin(); pm != pg->Markers.end(); ++pm)
            {
                if(pm->getSelected(x-pd->cx(), y-pd->cy()))
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pm->isSelected ^= flag;
                        return pm.ref();
                    }
                    if(pe_sel)
                    {
                        pe_sel->isSelected = false;
                        return pm.ref();
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pm.ref();   // give access to elements beneath
                    }
                    if(pm->isSelected)
                    {
                        pe_sel = pm.ref();
                    }
                }
            }
        }

        // resize area clicked ?
        if(pd->isSelected)
        {
            if(pd->resizeTouched(fX, fY, Corr))
            {
                if(pe_1st == 0)
                {
                    pd->Type = isDiagramResize;
                    return pd.ref();
                }
            }
        }

        if(pd->getSelected(x, y))
        {
            if(pd->Name[0] == 'T')     // tabular, timing diagram or truth table ?
            {
                if(pd->Name[1] == 'i')
                {
                    if(y > pd->cy())
                    {
                        if(x < pd->cx()+pd->xAxis.numGraphs) continue;
                        pd->Type = isDiagramHScroll;
                        return pd.ref();
                    }
                }
                else
                {
                    if(x < pd->cx())        // clicked on scroll bar ?
                    {
                        pd->Type = isDiagramVScroll;
                        return pd.ref();
                    }
                }
            }

            // test graphs of diagram
            for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
            {
                if(pg->getSelected(x-pd->cx(), pd->cy()-y) >= 0)
                {
                    if(flag)
                    {
                        // The element can be deselected
                        pg->isSelected ^= flag;
                        return pg.ref();
                    }
                    if(pe_sel)
                    {
                        pe_sel->isSelected = false;
                        return pg.ref();
                    }
                    if(pe_1st == 0)
                    {
                        pe_1st = pg.ref();   // access to elements lying beneath
                    }
                    if(pg->isSelected)
                    {
                        pe_sel = pg.ref();
                    }
                }
            }

            if(flag)
            {
                // The element can be deselected
                pd->isSelected ^= flag;
                return pd.ref();
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pd.ref();
            }
            if(pe_1st == 0)
            {
                pe_1st = pd.ref();    // give access to elements lying beneath
            }
            if(pd->isSelected)
            {
                pe_sel = pd.ref();
            }
        }
    }

    // test all paintings
    for(auto pp = Paintings->end(); pp != Paintings->begin(); )
    {
        --pp;
        if(pp->isSelected)
        {
            if(pp->resizeTouched(fX, fY, Corr))
            {
                if(pe_1st == 0)
                {
                    pp->Type = isPaintingResize;
                    return pp.ref();
                }
            }
        }

        if(pp->getSelected(fX, fY, Corr))
        {
            if(flag)
            {
                // The element can be deselected
                pp->isSelected ^= flag;
                return pp.ref();
            }
            if(pe_sel)
            {
                pe_sel->isSelected = false;
                return pp.ref();
            }
            if(pe_1st == 0)
            {
                pe_1st = pp.ref();    // give access to elements lying beneath
            }
            if(pp->isSelected)
            {
                pe_sel = pp.ref();
            }
        }
    }

    return pe_1st;
}

void Schematic::highlightWireLabels ()
{
    // First set highlighting for all wire and nodes labels to false
    for(auto pwouter = Wires->end(); pwouter != Wires->begin(); )
    {
        --pwouter;
        auto pltestouter = pwouter->Label; // test any label associated with the wire
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }

    for(auto pnouter = Nodes->end(); pnouter != Nodes->begin(); )
    {
        --pnouter;
        auto pltestouter = pnouter->Label; // test any label associated with the node
        if (pltestouter)
        {
            pltestouter->setHighlighted (false);
        }
    }
	
    // Then test every wire's label to see if we need to highlight it
    // and matching labels on wires and nodes
    for (auto pwouter = Wires->begin(); pwouter != Wires->end(); ++pwouter) {
        // get any label associated with the wire
        auto pltestouter = pwouter->Label;
        if (pltestouter)
        {
            if (pltestouter->isSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
                for (auto pwinner = Wires->begin(); pwinner != Wires->end(); ++pwinner) {
                    auto pltestinner = pwinner->Label; // test any label associated with the wire
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
                for (auto pninner = Nodes->begin(); pninner != Nodes->end(); ++pninner) {
                    auto pltestinner = pninner->Label; // test any label associated with the node
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
    for (auto pnouter = Nodes->begin(); pnouter != Nodes->end(); ++pnouter) {
        // get any label associated with the node
        auto pltestouter = pnouter->Label;
        if (pltestouter)
        {
            if (pltestouter->isSelected)
            {
                bool hiLightOuter = false;
                // Search for matching labels on wires
                for (auto pwinner = Wires->begin(); pwinner != Wires->end(); ++pwinner) {
                    auto pltestinner = pwinner->Label; // test any label associated with the wire
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
                for (auto pninner = Nodes->begin(); pninner != Nodes->end(); ++pninner) {
                    auto pltestinner = pninner->Label; // test any label associated with the node
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
void Schematic::deselectElements(const std::shared_ptr<Element> &e)
{
    // test all components
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
        if(e != pc.ref())  pc->isSelected = false;

    // test all wires
    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
    {
        if(e != pw.ref())  pw->isSelected = false;
        if(pw->Label) if(pw->Label != e)  pw->Label->isSelected = false;
    }

    // test all node labels
    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
        if(pn->Label) if(pn->Label != e)  pn->Label->isSelected = false;

    // test all diagrams
    for(auto pd = Diagrams->begin(); pd != Diagrams->end(); ++pd)
    {
        if(e != pd.ref())  pd->isSelected = false;

        // test graphs of diagram
        for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
        {
            if(e != pg.ref()) pg->isSelected = false;

            // test markers of graph
            for(auto pm = pg->Markers.begin(); pm != pg->Markers.end(); ++pm)
                if(e != pm.ref()) pm->isSelected = false;
        }

    }

    // test all paintings
    for(auto pp = Paintings->begin(); pp != Paintings->end(); ++pp)
        if(e != pp.ref())  pp->isSelected = false;
}

// ---------------------------------------------------
// Selects elements that lie within the rectangle x1()/y1(), x2()/y2().
int Schematic::selectElements(int x1, int y1, int x2, int y2, bool flag)
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
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
    {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pc->isSelected = true;
                        z++;
                        continue;
                    }
        if(pc->isSelected &= flag) z++;
    }


    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
    {
        if(pw->x1() >= x1) if(pw->x2() <= x2) if(pw->y1() >= y1) if(pw->y2() <= y2)
                    {
                        pw->isSelected = true;
                        z++;
                        continue;
                    }
        if(pw->isSelected &= flag) z++;
    }


    // test all wire labels *********************************
    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
    {
        if(pw->Label)
        {
            auto pl = pw->Label;
            if(pl->x1() >= x1) if((pl->x1()+pl->x2()) <= x2)
                    if(pl->y1() >= y1) if((pl->y1()+pl->y2()) <= y2)
                        {
                            pl->isSelected = true;
                            z++;
                            continue;
                        }
            if(pl->isSelected &= flag) z++;
        }
    }


    // test all node labels *************************************
    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
    {
        auto pl = pn->Label;
        if(pl)
        {
            if(pl->x1() >= x1) if((pl->x1()+pl->x2()) <= x2)
                    if((pl->y1()-pl->y2()) >= y1) if(pl->y1() <= y2)
                        {
                            pl->isSelected = true;
                            z++;
                            continue;
                        }
            if(pl->isSelected &= flag) z++;
        }
    }


    // test all diagrams *******************************************
    for(auto pd = Diagrams->begin(); pd != Diagrams->end(); ++pd)
    {
        // test graphs of diagram
        for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
        {
            if(pg->isSelected &= flag) z++;

            // test markers of graph
            for(auto pm = pg->Markers.begin(); pm != pg->Markers.end(); ++pm)
            {
                pm->Bounding(cx1, cy1, cx2, cy2);
                if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                            {
                                pm->isSelected = true;
                                z++;
                                continue;
                            }
                if(pm->isSelected &= flag) z++;
            }
        }

        // test diagram itself
        pd->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pd->isSelected = true;
                        z++;
                        continue;
                    }
        if(pd->isSelected &= flag) z++;
    }

    // test all paintings *******************************************
    for(auto pp = Paintings->begin(); pp != Paintings->end(); ++pp)
    {
        pp->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        pp->isSelected = true;
                        z++;
                        continue;
                    }
        if(pp->isSelected &= flag) z++;
    }

    return z;
}

// ---------------------------------------------------
// Selects all markers.
void Schematic::selectMarkers()
{
    for(auto pd = Diagrams->begin(); pd != Diagrams->end(); ++pd)
        for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
            for(auto pm = pg->Markers.begin(); pm != pg->Markers.end(); ++pm)
                pm->isSelected = true;
}

// ---------------------------------------------------
// For moving elements: If the moving element is connected to a not
// moving element, insert two wires. If the connected element is already
// a wire, use this wire. Otherwise create new wire.
void Schematic::newMovingWires(SharedObjectList<Element> &p, Node *pn, int pos)
{
    std::shared_ptr<Element> pe;

    if(pn->State & 8)  // Were new wires already inserted ?
        return;
    pn->State |= 8;

    for (;;)
    {
        if(pn->State & 16)  // node was already worked on
            break;

        if(!pn->connections().size()) return;

        pe = pn->connections().front().lock();
        if(!pe)  return;

        if(pn->refcount() > 1)
            break;
        if(pe->Type != isWire)  // is it connected to exactly one wire ?
            break;

        // .................................................
        long  mask = 1, invMask = 3;
        std::shared_ptr<Wire> pw2;
        std::shared_ptr<Wire> pw = std::dynamic_pointer_cast<Wire>(pe);

        Node *pn2 = pw->ports(0);
        if(pn2 == pn) pn2 = pw->ports(1);

        if(pn2->refcount() == 2) // two existing wires connected ?
            if((pn2->State & (8+4)) == 0)
            {
                std::shared_ptr<Element> pe2 = pn2->connections().front().lock();
                if(pe2 == pe) pe2 = pn2->connections().back().lock();
                // connected wire connected to exactly one wire ?
                if(pe2->Type == isWire)
                    pw2  = std::dynamic_pointer_cast<Wire>(pe2);
            }

        // .................................................
        // reuse one wire

        auto wi = Wires->find(pw.get());
        assert(wi != Wires->end());
        p.insert(p.begin() + pos, wi.ref());
        Wires->erase(wi);
        pw->ports(0)->removeConnection(pw);  // remove connection 1
        pw->ports(0)->State |= 16+4;
        pw->ports(1)->removeConnection(pw);  // remove connection 2
        pw->ports(1)->State |= 16+4;

        if(pw->isHorizontal()) mask = 2;

        if(pw2 == 0)    // place new wire between component and old wire
        {
            pn = pn2;
            mask ^= 3;
            invMask = 0;
        }

        if(pw->ports(0) != pn)
        {
            pw->ports(0)->State |= mask;
            pw->ports(0) = (Node*)(uintptr_t)mask;
            pw->ports(1)->State |= invMask;
            pw->ports(1) = (Node*)(uintptr_t)invMask;  // move port 2 completely
        }
        else
        {
            pw->ports(0)->State |= invMask;
            pw->ports(0) = (Node*)(uintptr_t)invMask;
            pw->ports(1)->State |= mask;
            pw->ports(1) = (Node*)(uintptr_t)mask;
        }

        invMask ^= 3;
        // .................................................
        // create new wire ?
        if(pw2 == 0)
        {
            if(pw->ports(0) != (Node*)(uintptr_t)mask)
                p.insert(p.begin() + pos,
                         new Wire(pw->x2(), pw->y2(), pw->x2(), pw->y2(), (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            else
                p.insert(p.begin() + pos,
                         new Wire(pw->x1(), pw->y1(), pw->x1(), pw->y1(), (Node*)(uintptr_t)mask, (Node*)(uintptr_t)invMask));
            return;
        }


        // .................................................
        // reuse a second wire
        auto iw2 = Wires->find(pw2.get());
        assert(iw2 != Wires->end());
        p.insert(p.begin() + pos, iw2.ref());
        Wires->erase(iw2);
        pw2->ports(0)->removeConnection(pw2);  // remove connection 1
        pw2->ports(0)->State |= 16+4;
        pw2->ports(1)->removeConnection(pw2);  // remove connection 2
        pw2->ports(1)->State |= 16+4;

        if(pw2->ports(0) != pn2)
        {
            pw2->ports(0) = (Node*)0;
            pw2->ports(1)->State |= mask;
            pw2->ports(1) = (Node*)(uintptr_t)mask;
        }
        else
        {
            pw2->ports(0)->State |= mask;
            pw2->ports(0) = (Node*)(uintptr_t)mask;
            pw2->ports(1) = (Node*)0;
        }
        return;
    }

    // only x2 moving
    p.insert(p.begin() + pos, new Wire(pn->cx(), pn->cy(), pn->cx(), pn->cy(), (Node*)0, (Node*)1));
    // x1, x2, y2 moving
    p.insert(p.begin() + pos, new Wire(pn->cx(), pn->cy(), pn->cx(), pn->cy(), (Node*)1, (Node*)3));
}

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
// BUG: does not (only) copy, as the name suggests.
//      cannot be used to make copies.
// returns the number of "copied" _Markers_ only
int Schematic::copySelectedElements(SharedObjectList<Element> &p)
{
    size_t count = 0;


    // test all components *********************************
    // Insert components before wires in order to prevent short-cut removal.
    for(auto pcn = Components->begin(); pcn != Components->end(); ) {
        auto pc = pcn;
        ++pcn;
        if(pc->isSelected)
        {
            p.append(pc.ref());
            count++;

            // delete all port connections
            for (auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp) {
                auto pcc = pp->getConnection();
                pcc->removeConnection(pc.ref());
                pcc->State = 4;
            }

            Components->erase(pc);   // take component out of the document
        }
    }

    // test all wires and wire labels ***********************
    for(auto pwn = Wires->begin(); pwn != Wires->end(); )
    {
        auto pw = pwn;
        ++pwn;
        if(pw->Label) if(pw->Label->isSelected)
                p.append(pw->Label);

        if(pw->isSelected)
        {
            p.append(pw.ref());

            pw->ports(0)->removeConnection(pw.ref());   // remove connection 1
            pw->ports(0)->State = 4;
            pw->ports(1)->removeConnection(pw.ref());   // remove connection 2
            pw->ports(1)->State = 4;
            Wires->erase(pw);
        }
    }

    // ..............................................
    // Inserts wires, if a connection to a not moving element is found.
    // The order of the "for"-loops is important to guarantee a stable
    // operation: components, new wires, old wires
    auto i = p.begin();
    for(size_t j = 0; j < count; ++j, ++i)
    {
        auto pc = std::dynamic_pointer_cast<Component>(i.ref());
        for(auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp)
            newMovingWires(p, pp->getConnection().get(), count);
    }

    for( ; i != p.end(); ++i) {
      if (i->isSelected) {
        break;
      }
    }

    QVector<Wire *> wiresToHandle;
    for ( ; i != p.end(); ++i) {
        if (i->Type == isWire) {
            wiresToHandle.append ((Wire *)i.operator->());
        }
    }
    foreach (Wire *pw, wiresToHandle) {
        newMovingWires(p, pw->ports(0), count);
        newMovingWires(p, pw->ports(1), count);
    }

    // ..............................................
    // delete the unused nodes
    for(auto pnn = Nodes->begin(); pnn != Nodes->end(); )
    {
        auto pn = pnn;
        ++pnn;
        if(pn->State & 8)
            if(pn->refcount() == 2)
                if(oneTwoWires(pn.ref()))    // if possible, connect two wires to one
                {
                    continue;
                }

        if(pn->refcount() == 0)
        {
            if(pn->Label)
            {
                pn->Label->Type = isMovingLabel;
                if(pn->State & 1)
                {
                    if(!(pn->State & 2)) pn->Label->Type = isHMovingLabel;
                }
                else if(pn->State & 2) pn->Label->Type = isVMovingLabel;
                p.append(pn->Label);    // do not forget the node labels
            }
            Nodes->erase(pn);
            continue;
        }

        pn->State = 0;
    }

    // test all node labels
    // do this last to avoid double copying
    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
        if(pn->Label) if(pn->Label->isSelected)
                p.append(pn->Label);


    // test all paintings **********************************
    for(auto ppan = Paintings->begin(); ppan != Paintings->end(); ) {
        auto ppa = ppan;
        ++ppan;
        if(ppa->isSelected)
        {
            p.append(ppa.ref());
            Paintings->erase(ppa);
        }
    }

    count = 0;  // count markers now
    // test all diagrams **********************************
    for(auto pdn = Diagrams->begin(); pdn != Diagrams->end(); ) {
        auto pd = pdn;
        ++pdn;
        if(pd->isSelected)
        {
            p.append(pd.ref());
            Diagrams->erase(pd);
        }
        else
        {
            for(auto pg = pd->Graphs.begin(); pg != pd->Graphs.end(); ++pg)
            {
                for(auto pm = pg->Markers.begin(); pm != pg->Markers.end(); ++pm)
                {
                    if(pm->isSelected)
                    {
                        count++;
                        p.append(pm.ref());
                    }
                }
            }
        }
    }

    return count;
}

// ---------------------------------------------------
bool Schematic::copyComps2WiresPaints(int& x1, int& y1, int& x2, int& y2,
                                  SharedObjectList<Element> &ElementCache)
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
                            SharedObjectList<Element> &ElementCache)
{
    int bx1, by1, bx2, by2;

    x1=INT_MAX;
    y1=INT_MAX;
    x2=INT_MIN;
    y2=INT_MIN;
    // take components and wires out of list, check their boundings
    int number = copyComponents(x1, y1, x2, y2, ElementCache);
    number += copyWires(x1, y1, x2, y2, ElementCache);

    // find upper most selected diagram
    for(auto pd = Diagrams->end(); pd != Diagrams->begin(); ) {
        --pd;
        if(pd->isSelected)
        {
            pd->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache.append(pd.ref());
            number++;
        }
    }

    // find upper most selected painting
    for(auto pp = Paintings->end(); pp != Paintings->begin(); ) {
        --pp;
        if(pp->isSelected)
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;
            ElementCache.append(pp.ref());
            number++;
        }
    }

    return number;
}

// ---------------------------------------------------
// Deletes all selected elements.
bool Schematic::deleteElements()
{
    bool sel = false;

    for (auto pcn = Components->begin(); pcn != Components->end(); )
    {
        auto pc = pcn;
        ++pcn;
        if(pc->isSelected)
        {
            deleteComp(pc);
            sel = true;
        }
    }

    for (auto pwn = Wires->begin(); pwn != Wires->end(); )
    {
        auto pw = pwn;
        ++pwn;
        if(pw->Label)
            if(pw->Label->isSelected)
            {
                pw->Label = 0;
                sel = true;
            }

        if(pw->isSelected)
        {
            deleteWire(pw);
            sel = true;
        }
    }

    // all selected labels on nodes ***************************
    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn) {
        if(pn->Label)
            if(pn->Label->isSelected)
            {
                pn->Label = 0;
                sel = true;
            }
    }

    for (auto pdn = Diagrams->begin(); pdn != Diagrams->end(); )
    {
        auto pd = pdn;
        ++pdn;
        if(pd->isSelected)
        {
            Diagrams->erase(pd);
            sel = true;
        }
        else
        {
            bool wasGraphDeleted = false;
            // all graphs of diagram

            auto ign = pd->Graphs.begin();

            for (; ign != pd->Graphs.end(); )
            {
                auto ig = ign++;
                // all markers of diagram
                for(auto pmn = ig->Markers.begin(); pmn != ig->Markers.end(); )
                {
                    auto pm = pmn;
                    ++pmn;
                    if(pm->isSelected)
                    {
                        ig->Markers.erase(pm);
                        sel = true;
                    }
                }
                if(ig->isSelected)
                {
                    pd->Graphs.erase(ig);
                    sel = wasGraphDeleted = true;
                }
            }
            if(wasGraphDeleted)
                pd->recalcGraphData();  // update diagram (resize etc.)

        } //else
    }

    for (auto ppn = Paintings->begin(); ppn != Paintings->end(); )
    {
        auto pp = ppn;
        ++ppn;
        if(pp->isSelected)
            if(pp->Name.at(0) != '.')    // do not delete "PortSym", "ID_text"
            {
                sel = true;
                Paintings->erase(pp);
            }
    }

    if(sel)
    {
        sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);   // set new document size
        setChanged(sel, true);
    }
    return sel;
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
    SharedObjectList<Element> ElementCache;
    int count = copyElements(x1, y1, x2, y2, ElementCache);
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

    // re-insert elements
    // Go backwards in order to insert node labels before its component.
    for (auto pe = ElementCache.end(); pe != ElementCache.begin(); ) {
        --pe;
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            {
                auto pc = std::dynamic_pointer_cast<Component>(pe.ref());
                pc->Bounding(bx1, by1, bx2, by2);
                pc->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
                insertRawComponent(pc);
            }
            break;

        case isWire:
            {
                auto pw = std::dynamic_pointer_cast<Wire>(pe.ref());
                bx1 = pw->x1();
                by1 = pw->y1();
                bx2 = pw->x2();
                by2 = pw->y2();
                pw->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
                insertWire(pw);
            }
            break;

        case isDiagram:
            {
                auto pd = std::dynamic_pointer_cast<Diagram>(pe.ref());
                // Should the axis label be counted for ? I guess everyone
                // has a different opinion.
    //        ((Diagram*)pe)->Bounding(bx1, by1, bx2, by2);

                // Take size without axis label.
                bx1 = pd->cx();
                by2 = pd->cy();
                bx2 = bx1 + pd->x2();
                by1 = by2 - pd->y2();
                pd->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            }
            break;

        case isPainting:
            {
                auto pp = std::dynamic_pointer_cast<Painting>(pe.ref());
                pp->Bounding(bx1, by1, bx2, by2);
                pp->setCenter(x1-((*bx)+(*ax))/y2, y1-((*by)+(*ay))/y2, true);
            }
            break;

        case isNodeLabel:
            {
                auto pwl = std::dynamic_pointer_cast<WireLabel>(pe.ref());
                if(((Element*)(pwl->pOwner))->Type & isComponent)
                {
                    Component *pc = (Component*)(pwl->pOwner);
                    pc->Bounding(bx1, by1, bx2, by2);
                }
                else
                {
                    Wire *pw = (Wire*)(pwl->pOwner);
                    bx1 = pw->x1();
                    by1 = pw->y1();
                    bx2 = pw->x2();
                    by2 = pw->y2();
                }
                pwl->set_cx(pwl->cx() + x1-((*bx)+(*ax))/y2);
                pwl->set_cy(pwl->cy() + y1-((*by)+(*ay))/y2);
                insertNodeLabel(pwl);
            }
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

namespace
{
    struct SortElementPointerByX
    {
        bool operator() (const std::shared_ptr<Element> &a, const std::shared_ptr<Element> &b) const
        {
            int bx1, by1, bx2, by2;
            a->getCenter(bx1, by1);
            b->getCenter(bx2, by2);
            return bx1 < bx2 || (bx1 == bx2 && by1 < by2);
        }
    };

    struct SortElementPointerByY
    {
        bool operator() (const std::shared_ptr<Element> &a, const std::shared_ptr<Element> &b) const
        {
            int bx1, by1, bx2, by2;
            a->getCenter(bx1, by1);
            b->getCenter(bx2, by2);
            return by1 < by2 || (by1 == by2 && bx1 < bx2);
        }
    };
}

/*!
 * \brief Schematic::distributeHorizontal sort selection horizontally
 * \return True if sorted
 */
bool Schematic::distributeHorizontal()
{
    int x1, y1, x2, y2;
    SharedObjectList<Element> ElementCache;
    int count = copyElements(x1, y1, x2, y2, ElementCache);
    if(count < 1) return false;

    //  sort the elements by x position
    std::vector<std::shared_ptr<Element> > sortedElements;
    sortedElements.reserve(ElementCache.size());
    for (auto i = ElementCache.begin(); i != ElementCache.end(); ++i) {
       sortedElements.push_back(i.ref());
    }
    ElementCache.clear();

    std::sort(sortedElements.begin(), sortedElements.end(), SortElementPointerByX());

    sortedElements.back()->getCenter(x2, y2);
    sortedElements.front()->getCenter(x1, y1);
    int x = x2;
    int dx=0;
    if(count > 1) dx = (x2-x1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    for (auto el = sortedElements.end(); el != sortedElements.begin(); ) {
        --el;
        const std::shared_ptr<Element> &pe = *el;
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->set_cx(x);
            insertRawComponent(std::dynamic_pointer_cast<Component>(pe));
            break;

        case isWire:
            {
                auto pw = std::dynamic_pointer_cast<Wire>(pe);
                if(pw->isHorizontal())
                {
                    int x1 = pw->x2() - pw->x1();
                    pw->set_x1(x - (x1 >> 1));
                    pw->set_x2(pw->x1() + x1);
                }
                else  {
                  pw->set_x1(x);
                  pw->set_x2(x);
                }
                insertWire(pw);
            }
            break;

        case isDiagram:
            pe->set_cx(x - (pe->x2() >> 1));
            break;

        case isPainting:
            {
                int bx1, by1;
                pe->getCenter(bx1, by1);
                pe->setCenter(x, by1, false);
            }
            break;

        case isNodeLabel:
            {
                auto pl = std::dynamic_pointer_cast<WireLabel>(pe);
                if(((Element*)(pl->pOwner))->Type & isComponent)
                    pe->set_cx(pe->cx() + x - ((Component*)(pl->pOwner))->cx());
                else
                {
                    Wire *pw = (Wire*)(pl->pOwner);
                    if(pw->isHorizontal())
                    {
                        int x1 = pw->x2() - pw->x1();
                        pe->set_cx(pe->cx() + x - (x1 >> 1) - pw->x1());
                    }
                    else  pe->set_cx(pe->cx() + x - pw->x1());
                }
                insertNodeLabel(pl);
                x += dx;
            }
            break;

        default:
            ;
        }
        x -= dx;
    }

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
    SharedObjectList<Element> ElementCache;
    int count = copyElements(x1, y1, x2, y2, ElementCache);
    if(count < 1) return false;

    //  sort the elements by x position
    std::vector<std::shared_ptr<Element> > sortedElements;
    sortedElements.reserve(ElementCache.size());
    for (auto i = ElementCache.begin(); i != ElementCache.end(); ++i) {
       sortedElements.push_back(i.ref());
    }
    ElementCache.clear();

    std::sort(sortedElements.begin(), sortedElements.end(), SortElementPointerByY());

    sortedElements.front()->getCenter(x2, y2);
    sortedElements.back()->getCenter(x1, y1);
    int y  = y2;
    int dy=0;
    if(count > 1) dy = (y2-y1)/(count-1);
    // re-insert elements and put them at right position
    // Go backwards in order to insert node labels before its component.
    for (auto el = sortedElements.end(); el != sortedElements.begin(); )
    {
        --el;
        const std::shared_ptr<Element> &pe = *el;
        switch(pe->Type)
        {
        case isComponent:
        case isAnalogComponent:
        case isDigitalComponent:
            pe->set_cy(y);
            insertRawComponent(std::dynamic_pointer_cast<Component>(pe));
            break;

        case isWire:
            {
                auto pw = std::dynamic_pointer_cast<Wire>(pe);
                if(pw->isHorizontal())  {
                  pw->set_y1(y);
                  pw->set_y2(y);
                }
                else
                {
                    int y1 = pw->y2() - pw->y1();
                    pw->set_y1(y - (y1 >> 1));
                    pw->set_y2(pe->y1() + y1);
                }
                insertWire(pw);
            }
            break;

        case isDiagram:
            pe->set_cy(y + (pe->y2() >> 1));
            break;

        case isPainting:
            {
                int bx1, by1;
                pe->getCenter(bx1, by1);
                pe->setCenter(bx1, y, false);
            }
            break;

        case isNodeLabel:
            {
                auto pl = std::dynamic_pointer_cast<WireLabel>(pe);
                if(((Element*)(pl->pOwner))->Type & isComponent)
                    pe->set_cy(pe->cy() + y - ((Component*)(pl->pOwner))->cy());
                else
                {
                    Wire *pw = (Wire*)(pl->pOwner);
                    if(!pw->isHorizontal())
                    {
                        int y1 = pw->y2() - pw->y1();
                        pe->set_cy(pe->cy() + y - (y1 >> 1) - pw->y1());
                    }
                    else  pe->set_cy(pe->cy() + y - pw->y1());
                }
                insertNodeLabel(pl);
                y += dy;
            }
            break;

        default:
            ;
        }
        y -= dy;
    }

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
void Schematic::setComponentNumber(const std::shared_ptr<Component> &c)
{
    if (c->Props.empty()) {
      return;
    }

    qucs::Property &pp = c->Props.front();
    if(pp.Name != "Num") return;

    int n=1;
    QString s = pp.Value;
    QString cSign = c->obsolete_model_hack();
    auto pc = Components->begin();
    // First look, if the port number already exists.
    for( ; pc != Components->end(); ++pc)
        if(pc->obsolete_model_hack() == cSign)
            if(pc->Props.front().Value == s) break;
    if (pc == Components->end()) return;   // was port number not yet in use ?

    // Find the first free number.
    do
    {
        s  = QString::number(n);
        // look for existing ports and their numbers
        for(pc = Components->begin(); pc != Components->end(); ++pc)
            if(pc->obsolete_model_hack() == cSign)
                if(pc->Props.front().Value == s) break;

        n++;
    }
    while(pc != Components->end());     // found not used component number
    pp.Value = s; // set new number
}

// ---------------------------------------------------
void Schematic::insertComponentNodes(const std::shared_ptr<Component> &c, bool noOptimize)
{
    // simulation components do not have ports
    if (c->Ports.empty()) return;

    // connect every node of the component to corresponding schematic node
    for (auto pp = c->Ports.begin(); pp != c->Ports.end(); ++pp) {
        pp->Connection = insertNode(pp->x+c->cx(), pp->y+c->cy(), c);
    }

    if(noOptimize)  return;

    // if component over wire then delete this wire
    auto pp = c->Ports.begin();
    // omit the first element
    ++pp;
    while (pp != c->Ports.end())
    {
        auto pn = pp->getConnection();
        for(auto i = pn->connections().begin(); i != pn->connections().end(); ++i)
        {
            auto pe = i->lock();
            if(pe->Type == isWire)
            {
                std::list<std::weak_ptr<Element> > const* pL;
                std::shared_ptr<Wire> pw = std::dynamic_pointer_cast<Wire>(pe);
                if (pw->ports(0) == pn.get()){
                  pL = &pw->ports(1)->connections();
		}else{
                  pL = &pw->ports(0)->connections();
		}

                for(auto pe1 = pL->begin(); pe1 != pL->end(); ++pe1)
                    if(std::shared_ptr<Element>(*pe1) == c)
                    {
                        auto pw = Wires->find((Wire*)pe.get());
                        assert(pw != Wires->end());
                        deleteWire(pw);
                        break;
                    }
            }
        }
        ++pp;
    }
}

// ---------------------------------------------------
// Used for example in moving components.
void Schematic::insertRawComponent(const ComponentList::holder &c, bool noOptimize)
{
    // connect every node of component to corresponding schematic node
    insertComponentNodes(c, noOptimize);
    // Check if this component already exists in the list. If it does, then
    // we shouldn't add it.
    if(Components->find(c) == Components->end())
        Components->append(c);

    // a ground symbol erases an existing label on the wire line
    if(c->obsolete_model_hack() == "GND") { // BUG.
        c->gnd_obsolete_model_override_hack("x");
        auto pe = getWireLabel(c->Ports.front().getConnection().get());
        if(pe) if((pe->Type & isComponent) == 0)
            {
                std::dynamic_pointer_cast<Conductor>(pe)->Label = 0;
            }
        c->gnd_obsolete_model_override_hack("GND");
    }
}

// ---------------------------------------------------
void Schematic::recreateComponent(const std::shared_ptr<Component> &Comp)
{
    std::list<std::shared_ptr<WireLabel> > plMem;
    int PortCount = Comp->Ports.size();

    if(PortCount > 0)
    {
        // Save the labels whose node is not connected to somewhere else.
        // Otherwise the label would be deleted.
        for (auto pp = Comp->Ports.begin(); pp != Comp->Ports.end(); ++pp) {
            auto pcc = pp->getConnection();
            if(pcc->refcount() < 2)
            {
                plMem.push_back(pcc->Label);
                pcc->Label = 0;
            }
            else plMem.push_back(0);
        }
    }


    int x = Comp->tx(), y = Comp->ty();
    int x1 = Comp->x1(), x2 = Comp->x2(), y1 = Comp->y1(), y2 = Comp->y2();
    QString tmp = Comp->name();    // is sometimes changed by "recreate"
    Comp->recreate(this);   // to apply changes to the schematic symbol
    Comp->obsolete_name_override_hack(tmp);
    if(x < x1)
        x += Comp->x1() - x1;
    else if(x > x2)
        x += Comp->x2() - x2;
    if(y < y1)
        y += Comp->y1() - y1;
    else if(y > y2)
        y += Comp->y2() - y2;
    Comp->set_qucs_text_position(x, y);


    if(PortCount > 0)
    {
        // restore node labels
        auto pl = plMem.begin();
        for (auto pp = Comp->Ports.begin(); pp != Comp->Ports.end(); ++pp)
        {
            if(*pl != 0)
            {
                auto pcc = pp->getConnection();
                (*pl)->set_cx(pcc->cx());
                (*pl)->set_cy(pcc->cy());
                placeNodeLabel(*pl);
            }
            ++pl;
            if((--PortCount) < 1)  break;
        }
    }
}

// ---------------------------------------------------
void Schematic::insertComponent(const std::shared_ptr<Component> &c)
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
            auto pe = getWireLabel(c->Ports.front().getConnection().get());
            if(pe && (pe->Type & isComponent) == 0)
                std::dynamic_pointer_cast<Conductor>(pe)->Label = 0;
            c->gnd_obsolete_model_override_hack("GND");
        }
    }
    else
    {
        // determines the name by looking for names with the same
        // prefix and increment the number
        for(auto pc = Components->begin(); pc != Components->end(); ++pc)
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


    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
    {
        pc->Bounding(cx1, cy1, cx2, cy2);
        if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.size() > 1)
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
                                    oneLabel(pc->Ports.front().getConnection().get());
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
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
    {
        pc->Bounding(x1, y1, x2, y2);
        if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2)
                    {
                        a = pc->isActive - 1;

                        if(pc->Ports.size() > 1)
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
                                    oneLabel(pc->Ports.front().getConnection().get());
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
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
        if(pc->isSelected)
        {
            a = pc->isActive - 1;

            if(pc->Ports.size() > 1)
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
                        oneLabel(pc->Ports.front().getConnection().get());
		    }
            }
            sel = true;
        }

    if(sel) setChanged(true, true);
    return sel;
}

// ---------------------------------------------------
// Sets the component ports anew. Used after rotate, mirror etc.
void Schematic::setCompPorts(std::shared_ptr<Component> &pc)
{
    std::list<std::shared_ptr<WireLabel> > LabelCache;

    for (auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp)
    {
        auto pcc = pp->getConnection();
        pcc->removeConnection(pc);  // delete connections
        switch(pcc->refcount())
        {
        case 0:
            {
                auto pl = pcc->Label;
                if(pl)
                {
                    LabelCache.push_back(pl);
                    pl->set_cx(pp->x + pc->cx());
                    pl->set_cy(pp->y + pc->cy());
                }
                Nodes->erase(pcc);
            }
            break;
        case 2:
            oneTwoWires(pcc); // try to connect two wires to one
        default:
            ;
        }
    }

    // Re-connect component node to schematic node. This must be done completely
    // after the first loop in order to avoid problems with node labels.
    for (auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp) {
        pp->Connection = insertNode(pp->x+pc->cx(), pp->y+pc->cy(), pc);
    }

    for(auto pl = LabelCache.begin(); pl != LabelCache.end(); ++pl) {
        insertNodeLabel(*pl);
    }
}

// ---------------------------------------------------
// Returns a pointer of the component on whose text x/y points.
std::shared_ptr<Component> Schematic::selectCompText(int x_, int y_, int& w, int& h)
{
    int a, b, dx, dy;
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
    {
        a = pc->cx() + pc->tx();
        if(x_ < a)  continue;
        b = pc->cy() + pc->ty();
        if(y_ < b)  continue;

        pc->textSize(dx, dy);
        if(x_ > a+dx)  continue;
        if(y_ > b+dy)  continue;

        w = dx;
        h = dy;
        return pc.ref();
    }

    return 0;
}

// ---------------------------------------------------
Component* Schematic::searchSelSubcircuit()
{
    Component *sub=0;
    // test all components
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
    {
        if(!pc->isSelected) continue;

        if(pc->obsolete_model_hack() != "Sub"){
            if(pc->obsolete_model_hack() != "VHDL")
                if(pc->obsolete_model_hack() != "Verilog") continue;
	}

        if(sub != 0) return 0;    // more than one subcircuit selected
        sub = pc.operator->();
    }
    return sub;
}

// ---------------------------------------------------
std::shared_ptr<Component> Schematic::selectedComponent(int x, int y)
{
    // test all components
    for(auto pc = Components->begin(); pc != Components->end(); ++pc)
        if(pc->getSelected(x, y))
            return pc.ref();

    return std::shared_ptr<Component>();
}

// ---------------------------------------------------
// Deletes the component 'c'.
void Schematic::deleteComp(const ComponentList::iterator &c)
{
    // delete all port connections
    for (auto pn = c->Ports.begin(); pn != c->Ports.end(); ++pn) {
        auto pcc = pn->getConnection();
        switch(pcc->refcount())
        {
        case 1  :
            Nodes->erase(pcc);  // delete open nodes
            pn->Connection = std::weak_ptr<Node>();  //  TODO: should be automatic, but we leave that here for safety
            break;
        case 3  :
            pcc->removeConnection(c.ref());// delete connection
            oneTwoWires(pcc);  // two wires -> one wire
            break;
        default :
            pcc->removeConnection(c.ref());// remove connection
            break;
        }
    }

    Components->erase(c);
}

// ---------------------------------------------------
int Schematic::copyComponents(int& x1, int& y1, int& x2, int& y2,
                           SharedObjectList<Element> &ElementCache)
{
    int bx1, by1, bx2, by2, count=0;
    // find bounds of all selected components
    for(auto pcn = Components->begin(); pcn != Components->end(); )
    {
        auto pc = pcn;
        ++pcn;
        if(pc->isSelected)
        {
            pc->Bounding(bx1, by1, bx2, by2);  // is needed because of "distribute
            if(bx1 < x1) x1 = bx1;             // uniformly"
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            count++;
            ElementCache.append(pc.ref());

            // rescue non-selected node labels
            for (auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp) {
                auto pcc = pp->getConnection();
                if(pcc->Label)
                    if(pcc->refcount() < 2)
                    {
                        ElementCache.append(pcc->Label);

                        // Don't set pp->Connection->Label->pOwner=0,
                        // so text position stays unchanged, but
                        // remember component for align/distribute.
                        pcc->Label->pOwner = (Node*)pc.operator->();

                        pcc->Label = 0;
                    }
            }

            deleteComp(pc);
        }
    }
    return count;
}

// ---------------------------------------------------
void Schematic::copyComponents2(int& x1, int& y1, int& x2, int& y2,
                            SharedObjectList<Element> &ElementCache)
{
    // find bounds of all selected components
    for(auto pcn = Components->begin(); pcn != Components->end(); )
    {
        auto pc = pcn;
        ++pcn;
        if(pc->isSelected)
        {
            // is better for unsymmetrical components
            if(pc->cx() < x1)  x1 = pc->cx();
            if(pc->cx() > x2)  x2 = pc->cx();
            if(pc->cy() < y1)  y1 = pc->cy();
            if(pc->cy() > y2)  y2 = pc->cy();

            ElementCache.append(pc.ref());

            // rescue non-selected node labels
            for (auto pp = pc->Ports.begin(); pp != pc->Ports.end(); ++pp) {
                auto pcc = pp->getConnection();
                if(pcc->Label)
                    if(pcc->refcount() < 2)
                    {
                        ElementCache.append(pcc->Label);
                        pcc->Label = 0;
                        // Don't set pcc->Label->pOwner=0,
                        // so text position stays unchanged.
                    }
            }

            deleteComp(pc);
        }
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
    std::shared_ptr<WireLabel> pl;
    bool named=false;   // wire line already named ?
    QVector<Node *> Cons;

    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
        pn->set_y1(0);   // mark all nodes as not checked

    Cons.append(n1);
    n1->set_y1(1);  // mark Node as already checked
    for(int i = 0; i < Cons.size(); ++i)
    {
        Node *pn = Cons[i];
        if(pn->Label)
        {
            if(named)
            {
                pn->Label = 0;    // erase double names
            }
            else
            {
                named = true;
                pl = pn->Label;
            }
        }

        for(auto j = pn->connections().begin(); j != pn->connections().end(); ++j)
        {
            auto pe = j->lock();
            if(pe->Type != isWire)
            {
                std::shared_ptr<Component> pc = std::dynamic_pointer_cast<Component>(pe);
                if(pc->isActive == COMP_IS_ACTIVE)
                    if(pc->obsolete_model_hack() == "GND")
                    {
                        named = true;
                        if(pl)
                            pl->pOwner->Label = 0;
                        pl = 0;
                    }
                continue;
            }

            std::shared_ptr<Wire> pw = std::dynamic_pointer_cast<Wire>(pe);

            Node *pNode;
            if(pn != pw->ports(0))
                pNode = pw->ports(0);
            else
                pNode = pw->ports(1);

            if(pNode->y1()) continue;
            pNode->set_y1(1);  // mark Node as already checked
            Cons.append(pNode);

            if(pw->Label)
            {
                if(named)
                {
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
int Schematic::placeNodeLabel(const std::shared_ptr<WireLabel> &pl)
{
    int x = pl->cx();
    int y = pl->cy();

    // check if new node lies upon an existing node
    auto pn = Nodes->begin();
    for( ; pn != Nodes->end(); ++pn)
        if(pn->cx() == x) if(pn->cy() == y) break;

    if(pn == Nodes->end())  return -1;

    auto pe = getWireLabel(pn.operator->());
    if(pe)      // name found ?
    {
        if(pe->Type & isComponent)
        {
            return -2;  // ground potential
        }

        std::dynamic_pointer_cast<Conductor>(pe)->Label = 0;
    }

    pn->Label = pl;   // insert node label
    pl->Type = isNodeLabel;
    pl->setOwner(pn.ref());
    return 0;
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
std::shared_ptr<Element> Schematic::getWireLabel(Node *pn_)
{
    QVector<Node *> Cons;

    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
        pn->set_y1(0);   // mark all nodes as not checked

    Cons.append(pn_);
    pn_->set_y1(1);  // mark Node as already checked
    for(int i = 0; i < Cons.size(); ++i)
    {
        Node *pn = Cons[i];
        if(pn->Label)
        {
            //  TOOD: this is required as we cannot make ports(0) and ports(1) weak pointers ...
            auto i = Nodes->find(pn);
            assert(i != Nodes->end());
            return i.ref();
        }
        else
        {
            for(auto j = pn->connections().begin(); j != pn->connections().end(); ++j)
            {
                auto pe = j->lock();
                if(pe->Type != isWire)
                {
                    auto pc = std::dynamic_pointer_cast<Component>(pe);
                    if(pc->isActive == COMP_IS_ACTIVE)
                        if(pc->obsolete_model_hack() == "GND") return pe;
                    continue;
                }

                auto pw = std::dynamic_pointer_cast<Wire>(pe);
                if(pw->Label) return pw;

                Node *pNode;
                if(pn != pw->ports(0))
                    pNode = pw->ports(0);
                else
                    pNode = pw->ports(1);

                if(pNode->y1()) continue;
                pNode->set_y1(1);  // mark Node as already checked
                Cons.append(pNode);
            }
        }
    }
    return 0;   // no wire label found
}

// ---------------------------------------------------
// Inserts a node label.
void Schematic::insertNodeLabel(const std::shared_ptr<WireLabel> &pl)
{
    if(placeNodeLabel(pl) != -1)
        return;

    // Go on, if label don't lie on existing node.

    auto pw = selectedWire(pl->cx(), pl->cy());
    if(pw)    // lies label on existing wire ?
    {
        if(getWireLabel(pw->ports(0)) == 0)  // wire not yet labeled ?
            pw->setName(pl->Name, pl->initValue, 0, pl->cx(), pl->cy());
        return;
    }


    std::shared_ptr<Node> pn(new Node(pl->cx(), pl->cy()));
    Nodes->append(pn);

    pn->Label = pl;
    pl->Type  = isNodeLabel;
    pl->setOwner(pn);
}

// ---------------------------------------------------
void Schematic::copyLabels(int& x1, int& y1, int& x2, int& y2,
                           SharedObjectList<Element> &ElementCache)
{
    // find bounds of all selected wires
    for(auto pw = Wires->begin(); pw != Wires->end(); ++pw)
    {
        auto pl = pw->Label;
        if(pl && pl->isSelected)
        {
            if(pl->x1() < x1) x1 = pl->x1();
            if(pl->y1()-pl->y2() < y1) y1 = pl->y1()-pl->y2();
            if(pl->x1()+pl->x2() > x2) x2 = pl->x1()+pl->x2();
            if(pl->y1() > y2) y2 = pl->y1();
            ElementCache.append(pl);
        }
    }

    for(auto pn = Nodes->begin(); pn != Nodes->end(); ++pn)
    {
        auto pl = pn->Label;
        if(pl && pl->isSelected)
        {
            if(pl->x1() < x1) x1 = pl->x1();
            if(pl->y1()-pl->y2() < y1) y1 = pl->y1()-pl->y2();
            if(pl->x1()+pl->x2() > x2) x2 = pl->x1()+pl->x2();
            if(pl->y1() > y2) y2 = pl->y1();
            ElementCache.append(pl);
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

Painting* Schematic::selectedPainting(float fX, float fY)
{
    float Corr = 5.0 / Scale; // size of line select

    for(auto pp = Paintings->begin(); pp != Paintings->end(); ++pp)
        if(pp->getSelected(fX, fY, Corr))
            return pp.operator->();

    return 0;
}

// ---------------------------------------------------
void Schematic::copyPaintings(int& x1, int& y1, int& x2, int& y2,
                              SharedObjectList<Element> &ElementCache)
{
    int bx1, by1, bx2, by2;
    // find boundings of all selected paintings
    for(auto ppn = Paintings->begin(); ppn != Paintings->end(); ) {
        auto pp = ppn;
        ++ppn;
        if(pp->isSelected)
        {
            pp->Bounding(bx1, by1, bx2, by2);
            if(bx1 < x1) x1 = bx1;
            if(bx2 > x2) x2 = bx2;
            if(by1 < y1) y1 = by1;
            if(by2 > y2) y2 = by2;

            ElementCache.append(pp.ref());
            Paintings->erase(pp);
        }
    }
}

// vim:ts=8:sw=2:noet
