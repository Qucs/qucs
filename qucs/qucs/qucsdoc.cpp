/***************************************************************************
                          qucsdoc.cpp  -  description
                             -------------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : margraf@mwt.ee.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "config.h"
#include "qucsdoc.h"
#include "component.h"

#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qiconset.h>


QucsDoc::QucsDoc(QTabBar *b, const QString& _Name)
{
  GridX  = 10;
  GridY  = 10;
  GridOn = true;

  DocName = _Name;
  if(_Name.isEmpty()) Tab = new QTab("untitled");
  else {
    QFileInfo Info(DocName);
    Tab = new QTab(Info.fileName());
  }

  Bar = b;
  Bar->addTab(Tab);  // create tab in TabBar
//  Bar->setCurrentTab(Tab);     // make it the current  ===>  CRASHES !!!???!!!
  Bar->repaint();

  DocChanged = false;

  Comps.setAutoDelete(true);
  Wires.setAutoDelete(true);
  Nodes.setAutoDelete(true);
}

QucsDoc::~QucsDoc()
{
  Bar->removeTab(Tab);    // delete tab in TabBar
}

// ---------------------------------------------------
void QucsDoc::setName(const QString& _Name)
{
  DocName = _Name;

  QFileInfo Info(DocName);
  Tab->setText(Info.fileName());  // remove path from file name and show it in TabBar
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
void QucsDoc::setChanged(bool c)
{
  if((!DocChanged) & c) {
    Tab->setIconSet(QIconSet(QImage("bitmaps/smallsave.png")));
    Bar->layoutTabs();
    Bar->repaint();
  }
  else if(DocChanged & (!c)) {
    Tab->setIconSet(QIconSet(0));   // no icon in TabBar
    Bar->layoutTabs();
    Bar->repaint();
  }
  DocChanged = c;
}

// ---------------------------------------------------
void QucsDoc::paint(QPainter *p)
{
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next())
    ptr1->paint(p);   // paint all components

  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next())
    ptr2->paint(p);   // paint all wires

  for(Node *ptr3 = Nodes.first(); ptr3 != 0; ptr3 = Nodes.next())
    ptr3->paint(p);   // paint all nodes
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void QucsDoc::setOnGrid(int& x, int& y)
{
  x -= x % GridX - (GridX >> 1);
  y -= y % GridY - (GridY >> 1);
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. The node is returned.
Node* QucsDoc::insertNode(int x, int y, Element *e)
{
  Node *ptr1;
  // check if new node lies upon existing node
  for(ptr1 = Nodes.first(); ptr1 != 0; ptr1 = Nodes.next()) {  // check every node
    if((ptr1->x == x) && (ptr1->y == y)) {
      ptr1->Connections.append(e);
      break;
    }
  }
  if(ptr1 == 0) {   // create new node, if no existing one lies at this position
    ptr1 = new Node(x, y);
    Nodes.append(ptr1);
    ptr1->Connections.append(e);  // connect schematic node to component node
  }
  else return ptr1;   // return, if node is not new

  // check if the new node lies upon an existing wire
  Wire *nw;
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {
    if(ptr2->x1 == x) {
      if((ptr2->y1 > y) || (ptr2->y2 < y)) continue;
    }
    else if(ptr2->y1 == y) {
      if((ptr2->x1 > x) || (ptr2->x2 < x)) continue;
    }
    else continue;

    // split the wire into two wires
    nw = new Wire(x, y, ptr2->x2, ptr2->y2, ptr1, ptr2->Port2);

    ptr2->x2 = x;
    ptr2->y2 = y;
    ptr2->Port2 = ptr1;

    nw->Port2->Connections.append(nw);
    ptr1->Connections.append(ptr2);
    ptr1->Connections.append(nw);
    nw->Port2->Connections.removeRef(ptr2);
    Wires.append(nw);
    return ptr1;
  }

  return ptr1;
}

// ---------------------------------------------------
void QucsDoc::insertComponent(Component *c)
{
  Node *p;
  // connect every node of component
  for(Port *ptr = c->Ports.first(); ptr != 0; ptr = c->Ports.next()) {
    p = insertNode(ptr->x+c->cx, ptr->y+c->cy, c);
    ptr->Connection = p;  // connect component node to schematic node
  }

  bool ok;
  QString s;
  int  max=1, len = c->Name.length(), z;
  // determines the name by looking for names with the same prefix and increment the number
  if(!c->Name.isEmpty()) {
    for(Component *ptr2 = Comps.first(); ptr2 != 0; ptr2 = Comps.next())
      if(ptr2->Name.left(len) == c->Name) {
        s = ptr2->Name.right(ptr2->Name.length()-len);
        z = s.toInt(&ok);
        if(ok) if(z >= max) max = z + 1;
      }
    c->Name += QString::number(max);    // create component name with new number
  }
  
  Comps.append(c);
  setChanged(true);
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. The node is returned.
Node* QucsDoc::insertWireNode1(Wire *e)
{
  Node *ptr1;
  // check if new node lies upon existing node
  for(ptr1 = Nodes.first(); ptr1 != 0; ptr1 = Nodes.next()) {  // check every node
    if((ptr1->x == e->x1) && (ptr1->y == e->y1)) {
      ptr1->Connections.append(e);
      break;
    }
  }
  if(ptr1 != 0) return ptr1;   // return, if node is not new

  // check if the new node lies upon an existing wire
  Wire *nw;
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {
    if(ptr2->x1 == e->x1) {
      if((ptr2->y1 > e->y1) || (ptr2->y2 < e->y1)) continue;

      if(ptr2->isHorizontal() == e->isHorizontal())
        if(ptr2->y2 >= e->y2) return 0;   // new wire lies within an existing wire
        else {
          e->y1 = ptr2->y2;
          ptr2->Port2->Connections.append(e);
          return ptr2->Port2;
        }
    }
    else if(ptr2->y1 == e->y1) {
      if((ptr2->x1 > e->x1) || (ptr2->x2 < e->x1)) continue;

      if(ptr2->isHorizontal() == e->isHorizontal())
        if(ptr2->x2 >= e->x2) return 0;   // new wire lies within an existing wire
        else {
          e->x1 = ptr2->x2;
          ptr2->Port2->Connections.append(e);
          return ptr2->Port2;
        }
    }
    else continue;

    ptr1 = new Node(e->x1, e->y1);   // create new node
    Nodes.append(ptr1);
    ptr1->Connections.append(e);  // connect schematic node to component node

    // split the wire into two wires
    nw = new Wire(e->x1, e->y1, ptr2->x2, ptr2->y2, ptr1, ptr2->Port2);

    ptr2->x2 = e->x1;
    ptr2->y2 = e->y1;
    ptr2->Port2 = ptr1;

    nw->Port2->Connections.append(nw);
    ptr1->Connections.append(ptr2);
    ptr1->Connections.append(nw);
    nw->Port2->Connections.removeRef(ptr2);
    Wires.append(nw);
    return ptr1;
  }

  ptr1 = new Node(e->x1, e->y1);   // create new node
  Nodes.append(ptr1);
  ptr1->Connections.append(e);  // connect schematic node to component node

  return ptr1;
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. The node is returned.
Node* QucsDoc::insertWireNode2(Wire *e)
{
  Node *ptr1;
  // check if new node lies upon existing node
  for(ptr1 = Nodes.first(); ptr1 != 0; ptr1 = Nodes.next()) {  // check every node
    if((ptr1->x == e->x2) && (ptr1->y == e->y2)) {
      ptr1->Connections.append(e);
      break;
    }
  }
  if(ptr1 != 0) return ptr1;   // return, if node is not new

  // check if the new node lies upon an existing wire
  Wire *nw;
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {
    if(ptr2->x1 == e->x2) {
      if((ptr2->y1 > e->y2) || (ptr2->y2 < e->y2)) continue;

      if(ptr2->isHorizontal() == e->isHorizontal()) {
        e->y2 = ptr2->y1;
        ptr2->Port1->Connections.append(e);
        return ptr2->Port1;
      }
    }
    else if(ptr2->y1 == e->y2) {
      if((ptr2->x1 > e->x2) || (ptr2->x2 < e->x2)) continue;

      if(ptr2->isHorizontal() == e->isHorizontal()) {
        e->x2 = ptr2->x1;
        ptr2->Port1->Connections.append(e);
        return ptr2->Port1;
      }
    }
    else continue;

    ptr1 = new Node(e->x2, e->y2);   // create new node
    Nodes.append(ptr1);
    ptr1->Connections.append(e);  // connect schematic node to component node

    // split the wire into two wires
    nw = new Wire(e->x2, e->y2, ptr2->x2, ptr2->y2, ptr1, ptr2->Port2);

    ptr2->x2 = e->x2;
    ptr2->y2 = e->y2;
    ptr2->Port2 = ptr1;

    nw->Port2->Connections.append(nw);
    ptr1->Connections.append(ptr2);
    ptr1->Connections.append(nw);
    nw->Port2->Connections.removeRef(ptr2);
    Wires.append(nw);
    return ptr1;
  }

  ptr1 = new Node(e->x2, e->y2);   // create new node
  Nodes.append(ptr1);
  ptr1->Connections.append(e);  // connect schematic node to component node

  return ptr1;
}

// ---------------------------------------------------
// Inserts a vertical or horizontal wire into the schematic and connects the ports that hit together.
// Returns whether the beginning and ending (the ports of the wire) are connected or not.
int QucsDoc::insertWire(Wire *w)
{
  int con = 0;
  int tmp;

  // change coordinates if necessary (port 1 coordinates must be less port 2 coordinates)
  if(w->x1 > w->x2) { tmp = w->x1; w->x1 = w->x2; w->x2 = tmp; }
  else
  if(w->y1 > w->y2) { tmp = w->y1; w->y1 = w->y2; w->y2 = tmp; }
  else con = 0x100;

  Node *ptr, *n;
  ptr = insertWireNode1(w);
  if(ptr == 0) return 3;
  if(ptr->Connections.count() > 1) con |= 2; // insert node and remember if it remains open
  w->Port1 = ptr;

  Wire *e, *nw;
  if(ptr->Connections.count() == 2) {   // if possible, connect two wires to one
    e = (Wire*)ptr->Connections.first();
    if(e->isWire)
      if(e->isHorizontal() == w->isHorizontal()) {
        w->Name  = e->Name; w->nx = e->nx; w->ny = e->ny;
                 w->delta = e->delta;   // do not move node name label
        w->x1 = e->x1;
        w->y1 = e->y1;
        w->Port1 = e->Port1;
        Nodes.removeRef(ptr);
        w->Port1->Connections.removeRef(e);
        w->Port1->Connections.append(w);
        Wires.removeRef(e);
      }
  }

  ptr = insertWireNode2(w);
  if(ptr == 0) return 3;
  if(ptr->Connections.count() > 1) con |= 1; // insert node and remember if it remains open
  w->Port2 = ptr;

  if(ptr->Connections.count() == 2) {   // if possible, connect two wires to one
    e = (Wire*)ptr->Connections.first();
    if(e->isWire)
      if(e->isHorizontal() == w->isHorizontal()) {
        w->Name  = e->Name; w->nx = e->nx; w->ny = e->ny;
                 w->delta = e->delta + w->x2-w->x1 + w->y2-w->y1;   // do not move node name label
        w->x2 = e->x2;
        w->y2 = e->y2;
        w->Port2 = e->Port2;
        Nodes.removeRef(ptr);
        w->Port2->Connections.removeRef(e);
        w->Port2->Connections.append(w);
        Wires.removeRef(e);
      }
  }

  if(con > 255) con = ((con >> 1) & 1) | ((con << 1) & 2); // change node 1 and 2


  Wires.append(w);
  n = Nodes.first();
  // check if the new line covers existing nodes
  for(e = Wires.current(); e != 0; e = Wires.next())
    for(ptr = n; ptr != 0; ptr = Nodes.next()) {  // check every node
      if(ptr->x == e->x1) {
        if((ptr->y > e->y1) && (ptr->y < e->y2)) {
          nw = new Wire(e->x1, e->y1, ptr->x, ptr->y, e->Port1, ptr);
          ptr->Connections.append(nw);
          Wires.append(nw);
          Wires.findRef(e);
          e->Port1->Connections.removeRef(e);
          e->Port1->Connections.append(nw);
          e->x1 = ptr->x;
          e->y1 = ptr->y;
          e->Port1 = ptr;
          ptr->Connections.append(e);
          n = ptr;
        }
      }
      else if(ptr->y == e->y1)
        if((ptr->x > e->x1) && (ptr->x < e->x2)) {
          nw = new Wire(e->x1, e->y1, ptr->x, ptr->y, e->Port1, ptr);
          ptr->Connections.append(nw);
          Wires.append(nw);
          Wires.findRef(e);
          e->Port1->Connections.removeRef(e);
          e->Port1->Connections.append(nw);
          e->x1 = ptr->x;
          e->y1 = ptr->y;
          e->Port1 = ptr;
          ptr->Connections.append(e);
          n = ptr;
        }
    }


  setChanged(true);
  return con;
}

// ---------------------------------------------------
Component* QucsDoc::selectedComponent(int x, int y)
{
  int  x1, y1, x2, y2;

  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {  // test all components
    ptr1->Bounding(x1, y1, x2, y2);
    if((x >= x1) & (x <= x2) & (y >= y1) & (y <= y2)) return ptr1;
  }

  return 0;
}

// ---------------------------------------------------
Wire* QucsDoc::selectedWire(int x, int y)
{
  setOnGrid(x, y);
  for(Wire *ptr = Wires.first(); ptr != 0; ptr = Wires.next())    // test all wires
    if(ptr->x1 <= x) if(ptr->x2 >= x) if(ptr->y1 <= y) if(ptr->y2 >= y)
      return ptr;

  return 0;
}

// ---------------------------------------------------
// Selects components that contains the coordinates x/y.
// If "flag" is true, components that are selected stay selected.
// Returns true, if at least one component has been selected.
bool QucsDoc::selectComponent(int x, int y, bool flag)
{
  int  x1, y1, x2, y2;
  bool isSel = false;
  
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {    // test all components
    ptr1->Bounding(x1, y1, x2, y2);
    if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2) {
      ptr1->isSelected = true;
      isSel = true;
      continue;
    }
    ptr1->isSelected &= flag;
  }

  setOnGrid(x, y);
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {    // test all wires
    if(ptr2->x1 <= x) if(ptr2->x2 >= x) if(ptr2->y1 <= y) if(ptr2->y2 >= y) {
      ptr2->isSelected = true;
      isSel = true;
      continue;
    }
    ptr2->isSelected &= flag;
  }

  return isSel;
}

// ---------------------------------------------------
// Selects components that lie within the rectangle x1/y1, x2/y2.
void QucsDoc::selectComponents(int x1, int y1, int x2, int y2, bool flag)
{
  int  cx1, cy1, cx2, cy2;
  if(x1 > x2) { cx1 = x1; x1  = x2; x2  = cx1; }
  if(y1 > y2) { cy1 = y1; y1  = y2; y2  = cy1; }

  
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {    // test all components
    ptr1->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      ptr1->isSelected = true;
      continue;
    }
    ptr1->isSelected &= flag;
  }

  
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {    // test all wires
    if(ptr2->x1 >= x1) if(ptr2->x2 <= x2) if(ptr2->y1 >= y1) if(ptr2->y2 <= y2) {
      ptr2->isSelected = true;
      continue;
    }
    ptr2->isSelected &= flag;
  }
}

// ---------------------------------------------------
void QucsDoc::activateComponent(int x, int y)
{
  int  x1, y1, x2, y2;
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {    // test all components
    ptr1->Bounding(x1, y1, x2, y2);
    if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2) {
      ptr1->isActive ^= 1;    // change "active status"
      setChanged(true);
    }
  }
}

// ---------------------------------------------------
// Rotates all selected components.
bool QucsDoc::rotateComponents()
{
  int  x1, y1, x2, y2;
  Component *ptr1;
   
  for(ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {  // find first selected component
    if(ptr1->isSelected) {
      x1 = ptr1->cx; x2 = x1;
      y1 = ptr1->cy; y2 = y1;
      break;
    }
  }
  if(ptr1 == 0) return false;   // no component selected, I leave
  for(ptr1 = Comps.next(); ptr1 != 0; ptr1 = Comps.next()) {  // find bounds of all selected components
    if(ptr1->isSelected) {
      if(ptr1->cx < x1) x1 = ptr1->cx;
      else if(ptr1->cx > x2) x2 = ptr1->cx;
      if(ptr1->cy < y1) y1 = ptr1->cy;
      else if(ptr1->cy > y2) y2 = ptr1->cy;
    }
  }

  x1 = (x1+x2) / 2;   // center for rotation
  y1 = (y1+y2) / 2;
  setOnGrid(x1, y1);

  
  for(ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {    // test all components
    if(ptr1->isSelected) {
      ptr1->rotate();   // rotate component !before! rotating its center
      x2 = x1 - ptr1->cx;
      ptr1->setCenter(ptr1->cy - y1 + x1, x2 + y1);
    }
  }

  setChanged(true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components.
bool QucsDoc::mirrorXComponents()
{
  int  x1, y1, x2, y2;
  Component *ptr1;

  for(ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {  // find first selected component
    if(ptr1->isSelected) {
      x1 = ptr1->cx; x2 = x1;
      y1 = ptr1->cy; y2 = y1;
      break;
    }
  }
  if(ptr1 == 0) return false;   // no component selected, I leave
  for(ptr1 = Comps.next(); ptr1 != 0; ptr1 = Comps.next()) {  // find bounds of all selected components
    if(ptr1->isSelected) {
      if(ptr1->cx < x1) x1 = ptr1->cx;
      else if(ptr1->cx > x2) x2 = ptr1->cx;
      if(ptr1->cy < y1) y1 = ptr1->cy;
      else if(ptr1->cy > y2) y2 = ptr1->cy;
    }
  }

  y1 = (y1+y2) / 2;   // axis for mirroring
  setOnGrid(x1, y1);


  for(ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {    // test all components
    if(ptr1->isSelected) {
      ptr1->mirrorX();   // mirror component !before! mirroring its center
      ptr1->setCenter(ptr1->cx, y1 - ptr1->cy + y1);
    }
  }

  setChanged(true);
  return true;
}

// ---------------------------------------------------
// Deletes all selected components.
bool QucsDoc::deleteComponents()
{
  bool sel = false;
  Wire *e1, *e2, *e3;
  Port *pn;
  
  Component *ptr1 = Comps.first();
  while(ptr1 != 0) {    // all selected component
    if(ptr1->isSelected) {
      for(pn = ptr1->Ports.first(); pn!=0; pn = ptr1->Ports.next())   // delete all port connections
        switch(pn->Connection->Connections.count()) {
          case 1  : Nodes.removeRef(pn->Connection);     // delete all open nodes
                    break;
          case 3  : pn->Connection->Connections.removeRef(ptr1);  // delete connection to component
                    e1 = (Wire*)pn->Connection->Connections.first();  // two wires -> one wire
                    e2 = (Wire*)pn->Connection->Connections.last();
                    if(e1->isWire) if(e2->isWire)
                      if(e1->isHorizontal() == e2->isHorizontal()) {
                        if(e1->x1 == e2->x2) if(e1->y1 == e2->y2) {
                          e3 = e1; e1 = e2; e2 = e3;    // e1 must have lesser coordinates
                        }
                        if(!e2->Name.isEmpty()) {   // take over the node name label ?
                          e1->Name = e2->Name;
                          e1->nx = e2->nx;
                          e1->ny = e2->ny;
                          e1->delta = e2->delta + e1->x2-e1->x1 + e1->y2-e1->y1;
                        }
                        e1->x2 = e2->x2;
                        e1->y2 = e2->y2;
                        e1->Port2 = e2->Port2;
                        Nodes.removeRef(pn->Connection);    // delete node (is auto delete)
                        e1->Port2->Connections.removeRef(e2);
                        e1->Port2->Connections.append(e1);
                        Wires.removeRef(e2);
                      }
                    break;
          default : pn->Connection->Connections.removeRef(ptr1);   // remove connection
                    break;
        }

      Comps.remove();   // delete component, if selected
      ptr1 = Comps.current();
      sel = true;
    }
    else ptr1 = Comps.next();
  }

  Wire *ptr2 = Wires.first();
  while(ptr2 != 0) {    // all selected wires
    if(ptr2->isSelected) {
      if(ptr2->Port1->Connections.count() == 1)
        Nodes.removeRef(ptr2->Port1);     // delete node 1 if open
      else ptr2->Port1->Connections.removeRef(ptr2);   // remove connection
      
      if(ptr2->Port2->Connections.count() == 1)
        Nodes.removeRef(ptr2->Port2);     // delete node 2 if open
      else ptr2->Port2->Connections.removeRef(ptr2);   // remove connection

      Wires.remove();
      ptr2 = Wires.current();
      sel = true;
    }
    else ptr2 = Wires.next();
  }

  setChanged(sel);
  return sel;
}

// ---------------------------------------------------
bool QucsDoc::save()
{
  QFile file(DocName);
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::critical(0, "Error", "Cannot save document!");
    return false;
  }
  
  QTextStream stream(&file);

  stream << "<Content=Qucs Schematic>\n";
  stream << "<Version=" << VERSION << ">\n";

  stream << "<Properties>\n";
  stream << "   <GridX=" << GridX << ">\n";
  stream << "   <GridY=" << GridY << ">\n";
  stream << "   <GridOn=" << GridOn << ">\n";
  stream << "<\\Properties>\n";
    
  stream << "<Components>\n";
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next())  // save all components
    stream << "   " << ptr1->save() << "\n";
  stream << "<\\Components>\n";
    
  stream << "<Wires>\n";
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next())  // save all wires
    stream << "   " << ptr2->save() << "\n";
  stream << "<\\Wires>\n";
    
  stream << "<Diagrams>\n";
  stream << "<\\Diagrams>\n";

  file.close();
  setChanged(false);
  return true;
}

// ---------------------------------------------------
bool QucsDoc::loadProperties(QTextStream *stream)
{
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<\\Properties>") return true;
  }

  QMessageBox::critical(0, "Error", "File Format Error:\n'Property' field is not closed!");
  return false;
}

bool QucsDoc::loadComponents(QTextStream *stream)
{
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<\\Components>") return true;

    Line = Line.stripWhiteSpace();
    cstr = Line.section(' ',0,0);    // component type
         if(cstr == "<R") c = new Resistor();
    else if(cstr == "<Rus") c = new ResistorUS();
    else if(cstr == "<C") c = new Capacitor();
    else if(cstr == "<L") c = new Inductor();
    else if(cstr == "<GND") c = new Ground();
    else if(cstr == "<Tr") c = new Transformer();
    else if(cstr == "<sTr") c = new symTrafo();
    else if(cstr == "<V") c = new Volt_dc();
    else if(cstr == "<I") c = new Ampere_dc();
    else {
      QMessageBox::critical(0, "Error", "File Format Error:\nUnknown component!");
      return false;
    }

    if(!c->load(Line)) {
      QMessageBox::critical(0, "Error", "File Format Error:\nWrong 'component' line format!");
      return false;
    }
    Line = c->Name;
    insertComponent(c);
    c->Name = Line;   // do not overwrite the name
  }

  QMessageBox::critical(0, "Error", "File Format Error:\n'Component' field is not closed!");
  return false;
}

bool QucsDoc::loadWires(QTextStream *stream)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<\\Wires>") return true;

    Line = Line.stripWhiteSpace();
    w = new Wire();
    if(!w->load(Line)) {
      QMessageBox::critical(0, "Error", "File Format Error:\nWrong 'wire' line format!");
      return false;
    }
//QMessageBox::information(0, "Info", w->Name+"  "+QString::number(w->x1)+","+QString::number(w->y1));
    insertWire(w);
//QMessageBox::information(0, "Info", w->Name+"  "+QString::number(w->x1)+","+QString::number(w->y1));
  }

  QMessageBox::critical(0, "Error", "File Format Error:\n'Wire' field is not closed!");
  return false;
}

bool QucsDoc::loadDiagrams(QTextStream *stream)
{
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<\\Diagrams>") return true;
  }

  QMessageBox::critical(0, "Error", "File Format Error:\n'Diagram' field is not closed!");
  return false;
}

bool QucsDoc::load()
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, "Error", "Cannot load document: "+DocName);
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  // read header **************************
  if(stream.atEnd()) {
    file.close();
    QMessageBox::critical(0, "Error", "Document is empty!");
    return false;
  }
  Line = stream.readLine();
  if(Line != "<Content=Qucs Schematic>") {  // wrong file type ?
    file.close();
    QMessageBox::critical(0, "Error", "Wrong document type!");
    return false;
  }

  if(stream.atEnd()) {
    file.close();
    QMessageBox::critical(0, "Error", "Wrong document type!");
    return false;
  }
  QString s = "<Version=" VERSION ">";
  Line = stream.readLine();
  if(Line != s) {  // wrong file type
    file.close();
    QMessageBox::critical(0, "Error", "Wrong document version!");
    return false;
  }

  // read content *************************
  while(!stream.atEnd()) {
    Line = stream.readLine();
    if(Line == "<Properties>") { if(!loadProperties(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Components>") { if(!loadComponents(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Wires>") { if(!loadWires(&stream)) { file.close(); return false; } }
    else
    if(Line == "<Diagrams>") { if(!loadDiagrams(&stream)) { file.close(); return false; } }
    else {
      QMessageBox::critical(0, "Error", "File Format Error:\nUnknown field!");
      file.close();
      return false;
    }
  }

  file.close();
  setChanged(false);
  return true;
}

// ---------------------------------------------------
// Creates the file "netlist.net" in the project directory. Returns "true"
// if successful.
bool QucsDoc::createNetlist(QFile *NetlistFile)
{
  if(!NetlistFile->open(IO_WriteOnly)) return false;

  QTextStream stream(NetlistFile);
  stream << "% Qucs  " << DocName << "\n";    // first line docu


  // ................................................
  Node *p1, *p2;
  Wire *pw;
  Element *pe;

  // delete the node names not given by the user
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next())
    if(!p1->isNamed) p1->Name = "";

  // set the wire names to the connected node
  for(pw = Wires.first(); pw != 0; pw = Wires.next())
    if(!pw->Name.isEmpty()) pw->Port1->Name = pw->Name;

  // give the ground nodes the name "0"
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())
    if(pc->Sign == "GND") pc->Ports.first()->Connection->Name = "0";

    
  QPtrList<Node> Cons;
  // work on named nodes first in order to preserve the user given names
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next()) {
    if(p1->Name.isEmpty()) continue;
    Cons.append(p1);
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
        if(pe->isWire) {
          pw = (Wire*)pe;
          if(p2 != pw->Port1) {
            if(pw->Port1->Name.isEmpty()) {
              pw->Port1->Name = p1->Name;
              Cons.append(pw->Port1);
              Cons.findRef(p2);
            }
          }
          else {
            if(pw->Port2->Name.isEmpty()) {
              pw->Port2->Name = p1->Name;
              Cons.append(pw->Port2);
              Cons.findRef(p2);
            }
          }
        }
    Cons.clear();
  }


  int z=0;
  // give names to the remaining (unnamed) nodes
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next()) {   // work on all nodes
    if(!p1->Name.isEmpty()) continue;    // already named ?
    p1->Name = "_net" + QString::number(z++);   // create node name
    Cons.append(p1);
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())   // create list with connections to the node
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
        if(pe->isWire) {
          pw = (Wire*)pe;
          if(p2 != pw->Port1) {
            if(pw->Port1->Name.isEmpty()) {
              pw->Port1->Name = p1->Name;
              Cons.append(pw->Port1);
              Cons.findRef(p2);   // back to current Connection
            }
          }
          else {
            if(pw->Port2->Name.isEmpty()) {
              pw->Port2->Name = p1->Name;
              Cons.append(pw->Port2);
              Cons.findRef(p2);
            }
          }
        }
    Cons.clear();
  }


  // .................................................
  QString s;
  // write all components with node names into the netlist file
  for(Component *ptr1 = Comps.first(); ptr1 != 0; ptr1 = Comps.next()) {
    s = ptr1->NetList();
    if(!s.isEmpty()) stream << s << "\n";
  }
  NetlistFile->close();

  return true;
}
