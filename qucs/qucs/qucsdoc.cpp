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

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "qucsdoc.h"
#include "diagrams/diagrams.h"
#include "paintings/paintings.h"

#include <qmessagebox.h>
#include <qfileinfo.h>
#include <qimage.h>
#include <qiconset.h>

#include <limits.h>


// icon for unsaved files (diskette)
static const char *smallsave_xpm[] = {
"16 17 66 1", " 	c None",
".	c #595963","+	c #E6E6F1","@	c #465460","#	c #FEFEFF","$	c #DEDEEE","%	c #43535F","&	c #D1D1E6",
"*	c #5E5E66","=	c #FFFFFF","-	c #C5C5DF",";	c #FCF8F9",">	c #BDBDDA",",	c #BFBFDC","'	c #C4C4DF",
")	c #FBF7F7","!	c #D6D6E9","~	c #CBCBE3","{	c #B5B5D6","]	c #BCBCDA","^	c #C6C6E0","/	c #CFCFE5",
"(	c #CEC9DC","_	c #D8D8EA",":	c #DADAEB","<	c #313134","[	c #807FB3","}	c #AEAED1","|	c #B7B7D7",
"1	c #E2E2EF","2	c #9393C0","3	c #E3E3F0","4	c #DDD5E1","5	c #E8E8F3","6	c #2F2F31","7	c #7B7BAF",
"8	c #8383B5","9	c #151518","0	c #000000","a	c #C0C0DC","b	c #8E8FBD","c	c #8989BA","d	c #E7EEF6",
"e	c #282829","f	c #6867A1","g	c #7373A9","h	c #A7A7CD","i	c #8080B3","j	c #7B7CB0","k	c #7070A8",
"l	c #6D6DA5","m	c #6E6EA6","n	c #6969A2","o	c #7A79AF","p	c #DCDCEC","q	c #60609A","r	c #7777AC",
"s	c #5D5D98","t	c #7676AB","u	c #484785","v	c #575793","w	c #50506A","x	c #8787B8","y	c #53536E",
"z	c #07070E","A	c #666688",
"        .       ",
"       .+.      ",
"      .+@#.     ",
"     .$%###.    ",
"    .&*####=.   ",
"   .-.#;#####.  ",
"  .>,'.#)!!!!~. ",
" .{].'^./(!_:<[.",
".}|.1./2.3456789",
"0a.$11.bc.defg9 ",
" 011h11.ij9kl9  ",
"  0_1h1h.mno9   ",
"   0p12h9qr9    ",
"    0hh9st9     ",
"     09uv9w     ",
"      0x9y      ",
"       zA       "};



QucsDoc::QucsDoc(QTabBar *b, const QString& _Name)
{
  GridX  = 10;
  GridY  = 10;
  GridOn = true;
  Scale=1.0;
  ViewX1=ViewY1=0;
  ViewX2=ViewY2=800;
  PosX=PosY=0;

  DocName = _Name;
  if(_Name.isEmpty()) Tab = new QTab(QObject::tr("untitled"));
  else {
    QFileInfo Info(DocName);
    Tab = new QTab(Info.fileName());
    DataSet = Info.baseName()+".dat";   // name of the default dataset
    if(Info.extension(false) == "sch")
      DataDisplay = Info.baseName()+".dpl";   // name of the default data display
    else {
      DataDisplay = Info.baseName()+".sch";   // name of the default data display
      GridOn = false;     // data display without grid (per default)
    }
  }
  SimOpenDpl = true;
  
  Bar = b;
  if(Bar != 0) {
    Bar->addTab(Tab);  // create tab in TabBar
//    Bar->setCurrentTab(Tab);     // make it the current  ===>  Qt 3.1 CRASHES !!!???!!!
    Bar->repaint();
  }

  DocChanged = false;

  Comps.setAutoDelete(true);
  Wires.setAutoDelete(true);
  Nodes.setAutoDelete(true);
  Diags.setAutoDelete(true);
  Paints.setAutoDelete(true);
}

QucsDoc::~QucsDoc()
{
  if(Bar != 0) Bar->removeTab(Tab);    // delete tab in TabBar
}

// ---------------------------------------------------
void QucsDoc::setName(const QString& _Name)
{
  DocName = _Name;

  QFileInfo Info(DocName);
  Tab->setText(Info.fileName());  // remove path from file name and show it in TabBar

  DataSet = Info.baseName()+".dat";   // name of the default dataset
  if(Info.extension(false) == "sch")
    DataDisplay = Info.baseName()+".dpl";   // name of the default data display
  else
    DataDisplay = Info.baseName()+".sch";   // name of the default data display
}

// ---------------------------------------------------
// Sets the document to be changed or not to be changed.
void QucsDoc::setChanged(bool c)
{
  if((!DocChanged) & c) {
    Tab->setIconSet(QPixmap(smallsave_xpm));
    if(Bar != 0) {
      Bar->layoutTabs();
      Bar->repaint();
    }
  }
  else if(DocChanged & (!c)) {
    Tab->setIconSet(QIconSet(0));   // no icon in TabBar
    if(Bar != 0) {
      Bar->layoutTabs();
      Bar->repaint();
    }
  }
  DocChanged = c;
}

// ---------------------------------------------------
void QucsDoc::paint(QPainter *p)
{
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())
    pc->paint(p);   // paint all components

  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next())
    pw->paint(p);   // paint all wires

  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next())
    pn->paint(p);   // paint all nodes

  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next())
    pd->paint(p);   // paint all diagrams

  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next())
    pp->paint(p);   // paint all paintings
}

// ---------------------------------------------------
// Sets an arbitrary coordinate onto the next grid coordinate.
void QucsDoc::setOnGrid(int& x, int& y)
{
  if(x<0) x -= (GridX >> 1) - 1;
  else x += GridX >> 1;
  x -= x % GridX;
  
  if(y<0) y -= (GridY >> 1) - 1;
  else y += GridY >> 1;
  y -= y % GridY;
}

// ---------------------------------------------------
void QucsDoc::paintGrid(QPainter *p, int cX, int cY, int Width, int Height)
{
  if(!GridOn) return;
  
  int x1 = int(cX/Scale)+ViewX1;
  if(x1<0) x1 -= GridX - 1;
  else x1 += GridX;
  x1 -= x1 % (GridX << 1);
  x1 -= ViewX1; x1 = int(x1*Scale);

  int y1 = int(cY/Scale)+ViewY1;
  if(y1<0) y1 -= GridY - 1;
  else y1 += GridY;
  y1 -= y1 % (GridY << 1);
  y1 -= ViewY1; y1 = int(y1*Scale);

  int x2 = x1+Width, y2 = y1+Height;
  int dx = int(double(2*GridX)*Scale);
  int dy = int(double(2*GridY)*Scale);

  p->setPen(QPen(QPen::black,1));
  for(int x=x1; x<x2; x+=dx)
    for(int y=y1; y<y2; y+=dy)
      p->drawPoint(x,y);    // paint grid
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
      if(ptr2->y1 > y) continue;
      if(ptr2->y2 < y) continue;
    }
    else if(ptr2->y1 == y) {
      if(ptr2->x1 > x) continue;
      if(ptr2->x2 < x) continue;
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
void QucsDoc::insertRawComponent(Component *c, int pos)
{
  Node *p;
  // connect every node of component
  for(Port *ptr = c->Ports.first(); ptr != 0; ptr = c->Ports.next()) {
    p = insertNode(ptr->x+c->cx, ptr->y+c->cy, c);
    ptr->Connection = p;  // connect component node to schematic node
  }

  if(pos < 0) Comps.append(c);
  else Comps.insert(pos, c);   // insert component at wanted position
  setChanged(true);
}

// ---------------------------------------------------
// Finds the correct position and number for power sources and subcircuit ports.
int QucsDoc::getComponentPos(Component *c)
{
  int n=0;
  QString s, cSign = c->Sign;
  // power sources and subcirciut ports have to be numbered
  if((cSign == "Pac") || (cSign == "Port")) {
    s  = c->Props.getFirst()->Value;
    // look for existing ports and their numbers
    for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {
      if(pc->Sign == cSign)
        if(pc->Props.getFirst()->Value > s) break;
        else {
          if(pc->Props.getFirst()->Value < s) { n++; continue; }
          c->Props.getFirst()->Value = s = QString::number(s.toInt()+1); // create new number
        }
      n++;
    }
    return n;
  }
  return Comps.count();
}

// ---------------------------------------------------
void QucsDoc::insertComponent(Component *c)
{
  Component *pc;
  Node *p;
  // connect every node of the component
  for(Port *ptr = c->Ports.first(); ptr != 0; ptr = c->Ports.next()) {
    p = insertNode(ptr->x+c->cx, ptr->y+c->cy, c);
    ptr->Connection = p;  // connect component node to schematic node
  }

  bool ok;
  QString s;
  int  max=1, len = c->Name.length(), z;
  // determines the name by looking for names with the same prefix and increment the number
  if(!c->Name.isEmpty()) {
    for(pc = Comps.first(); pc != 0; pc = Comps.next())
      if(pc->Name.left(len) == c->Name) {
        s = pc->Name.right(pc->Name.length()-len);
        z = s.toInt(&ok);
        if(ok) if(z >= max) max = z + 1;
      }
    c->Name += QString::number(max);    // create component name with new number
  }


  setChanged(true);
  max = getComponentPos(c); // important for power sources and subcircuit ports
  Comps.insert(max, c);   // insert component at appropriate position
}

// ---------------------------------------------------
// Inserts a port into the schematic and connects it to another node if the
// coordinates are identical. If 0 is returned, no new wire is inserted.
// If 2 is returned, the wire line ended.
int QucsDoc::insertWireNode1(Wire *w)
{
  Node *pn;
  // check if new node lies upon an existing node
  for(pn = Nodes.first(); pn != 0; pn = Nodes.next())  // check every node
    if(pn->x == w->x1) if(pn->y == w->y1) break;

  if(pn != 0) {
    pn->Connections.append(w);
    w->Port1 = pn;
    return 2;   // node is not new
  }


  
  Wire *pw;
  // check if the new node lies upon an existing wire
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {
    if(ptr2->x1 == w->x1) {
      if(ptr2->y1 > w->y1) continue;
      if(ptr2->y2 < w->y1) continue;

      if(ptr2->isHorizontal() == w->isHorizontal())   // (ptr2-wire is vertical)
        if(ptr2->y2 >= w->y2) { delete w; return 0; }  // new wire lies within an existing wire
        else {  // one part of the wire lies within an existing wire the other part not
          if(ptr2->Port2->Connections.count() == 1) {
            w->y1 = ptr2->y1;
            w->Port1 = ptr2->Port1;
            ptr2->Port1->Connections.removeRef(ptr2);   // two wires -> one wire
            ptr2->Port1->Connections.append(w);
            Nodes.removeRef(ptr2->Port2);
            Wires.removeRef(ptr2);
            return 2;
          }
          else {
            w->y1 = ptr2->y2;
            w->Port1 = ptr2->Port2;
            ptr2->Port2->Connections.append(w);   // shorten new wire
            return 2;
          }
        }
    }
    else if(ptr2->y1 == w->y1) {
      if(ptr2->x1 > w->x1) continue;
      if(ptr2->x2 < w->x1) continue;

      if(ptr2->isHorizontal() == w->isHorizontal())   // (ptr2-wire is horizontal)
        if(ptr2->x2 >= w->x2) { delete w; return 0; }  // new wire lies within an existing wire
        else {  // one part of the wire lies within an existing wire the other part not
          if(ptr2->Port2->Connections.count() == 1) {
            w->x1 = ptr2->x1;
            w->Port1 = ptr2->Port1;
            ptr2->Port1->Connections.removeRef(ptr2);   // two wires -> one wire
            ptr2->Port1->Connections.append(w);
            Nodes.removeRef(ptr2->Port2);
            Wires.removeRef(ptr2);
            return 2;
          }
          else {
            w->x1 = ptr2->x2;
            w->Port1 = ptr2->Port2;
            ptr2->Port2->Connections.append(w);   // shorten new wire
            return 2;
          }
        }
    }
    else continue;

    pn = new Node(w->x1, w->y1);   // create new node
    Nodes.append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port1 = pn;

    // split the wire into two wires
    pw = new Wire(w->x1, w->y1, ptr2->x2, ptr2->y2, pn, ptr2->Port2);

    ptr2->x2 = w->x1;
    ptr2->y2 = w->y1;
    ptr2->Port2 = pn;

    pw->Port2->Connections.prepend(pw);   // add new connections not at the end !!!
    pn->Connections.prepend(ptr2);
    pn->Connections.prepend(pw);
    pw->Port2->Connections.removeRef(ptr2);
    Wires.append(pw);
    return 2;
  }

  pn = new Node(w->x1, w->y1);   // create new node
  Nodes.append(pn);
  pn->Connections.append(w);  // connect schematic node to the new wire
  w->Port1 = pn;
  return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool QucsDoc::connectHWires1(Wire *w)
{
  Wire *pw;
  Node *n = w->Port1;
  
  pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
  for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev()) {
    if(pw->Type != isWire) continue;
    if(!pw->isHorizontal()) continue;
    if(pw->x1 < w->x1) {
      if(n->Connections.count() != 2) continue;
      w->Name  = pw->Name; w->nx = pw->nx; w->ny = pw->ny;
                 w->delta = pw->delta;   // do not move node name label
      w->x1 = pw->x1;
      w->Port1 = pw->Port1;         // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port1->Connections.removeRef(pw);
      w->Port1->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    if(pw->x2 >= w->x2) {  // new wire lies complete within an existing one ?
      w->Port1->Connections.removeRef(w); // second node not yet made
      delete w;
      return false;
    }
    if(pw->Port2->Connections.count() < 2) {
      pw->Port1->Connections.removeRef(pw); // existing wire lies within the new one
      Nodes.removeRef(pw->Port2);
      Wires.removeRef(pw);
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
bool QucsDoc::connectVWires1(Wire *w)
{
  Wire *pw;
  Node *n = w->Port1;

  pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
  for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev()) {
    if(pw->Type != isWire) continue;
    if(pw->isHorizontal()) continue;
    if(pw->y1 < w->y1) {
      if(n->Connections.count() != 2) continue;
      w->Name  = pw->Name; w->nx = pw->nx; w->ny = pw->ny;
                 w->delta = pw->delta;   // do not move node name label
      w->y1 = pw->y1;
      w->Port1 = pw->Port1;         // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port1->Connections.removeRef(pw);
      w->Port1->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    if(pw->y2 >= w->y2) {  // new wire lies complete within an existing one ?
      w->Port1->Connections.removeRef(w); // second node not yet made
      delete w;
      return false;
    }
    if(pw->Port2->Connections.count() < 2) {
      pw->Port1->Connections.removeRef(pw); // existing wire lies within the new one
      Nodes.removeRef(pw->Port2);
      Wires.removeRef(pw);
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
int QucsDoc::insertWireNode2(Wire *w)
{
  Node *pn;
  // check if new node lies upon an existing node
  for(pn = Nodes.first(); pn != 0; pn = Nodes.next())  // check every node
    if(pn->x == w->x2) if(pn->y == w->y2) break;

  if(pn != 0) {
    pn->Connections.append(w);
    w->Port2 = pn;
    return 2;   // node is not new
  }



  Wire *pw;
  // check if the new node lies upon an existing wire
  for(Wire *ptr2 = Wires.first(); ptr2 != 0; ptr2 = Wires.next()) {
    if(ptr2->x1 == w->x2) {
      if(ptr2->y1 > w->y2) continue;
      if(ptr2->y2 < w->y2) continue;

      // (if new wire lies within an existing wire, was already check before)
      if(ptr2->isHorizontal() == w->isHorizontal())   // (ptr2-wire is vertical)
      // one part of the wire lies within an existing wire the other part not
          if(ptr2->Port1->Connections.count() == 1) {
            w->y2 = ptr2->y2;
            w->Port2 = ptr2->Port2;
            ptr2->Port2->Connections.removeRef(ptr2);   // two wires -> one wire
            ptr2->Port2->Connections.append(w);
            Nodes.removeRef(ptr2->Port1);
            Wires.removeRef(ptr2);
            return 2;
          }
          else {
            w->y2 = ptr2->y1;
            w->Port2 = ptr2->Port1;
            ptr2->Port1->Connections.append(w);   // shorten new wire
            return 2;
          }
    }
    else if(ptr2->y1 == w->y2) {
      if(ptr2->x1 > w->x2) continue;
      if(ptr2->x2 < w->x2) continue;

      // (if new wire lies within an existing wire, was already check before)
      if(ptr2->isHorizontal() == w->isHorizontal())   // (ptr2-wire is horizontal)
      // one part of the wire lies within an existing wire the other part not
          if(ptr2->Port1->Connections.count() == 1) {
            w->x2 = ptr2->x2;
            w->Port2 = ptr2->Port2;
            ptr2->Port2->Connections.removeRef(ptr2);   // two wires -> one wire
            ptr2->Port2->Connections.append(w);
            Nodes.removeRef(ptr2->Port1);
            Wires.removeRef(ptr2);
            return 2;
          }
          else {
            w->x2 = ptr2->x1;
            w->Port2 = ptr2->Port1;
            ptr2->Port1->Connections.append(w);   // shorten new wire
            return 2;
          }
    }
    else continue;

    pn = new Node(w->x2, w->y2);   // create new node
    Nodes.append(pn);
    pn->Connections.append(w);  // connect schematic node to the new wire
    w->Port2 = pn;

    // split the wire into two wires
    pw = new Wire(w->x2, w->y2, ptr2->x2, ptr2->y2, pn, ptr2->Port2);

    ptr2->x2 = w->x2;
    ptr2->y2 = w->y2;
    ptr2->Port2 = pn;

    pw->Port2->Connections.prepend(pw);   // add new connections not at the end !!!
    pn->Connections.prepend(ptr2);
    pn->Connections.prepend(pw);
    pw->Port2->Connections.removeRef(ptr2);
    Wires.append(pw);
    return 2;
  }

  pn = new Node(w->x2, w->y2);   // create new node
  Nodes.append(pn);
  pn->Connections.append(w);  // connect schematic node to the new wire
  w->Port2 = pn;
  return 1;
}

// ---------------------------------------------------
// if possible, connect two horizontal wires to one
bool QucsDoc::connectHWires2(Wire *w)
{
  Wire *pw;
  Node *n = w->Port2;

  pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
  for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev()) {
    if(pw->Type != isWire) continue;
    if(!pw->isHorizontal()) continue;
    if(pw->x2 > w->x2) {
      if(n->Connections.count() != 2) continue;
      w->Name  = pw->Name; w->nx = pw->nx; w->ny = pw->ny;
                 w->delta = pw->delta;   // do not move node name label
      w->x2 = pw->x2;
      w->Port2 = pw->Port2;         // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port2->Connections.removeRef(pw);
      w->Port2->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    // (if new wire lies complete within an existing one, was already check before)
    
    if(pw->Port1->Connections.count() < 2) {
      pw->Port2->Connections.removeRef(pw); // existing wire lies within the new one
      Nodes.removeRef(pw->Port1);
      Wires.removeRef(pw);
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
bool QucsDoc::connectVWires2(Wire *w)
{
  Wire *pw;
  Node *n = w->Port2;

  pw = (Wire*)n->Connections.last();  // last connection is the new wire itself
  for(pw = (Wire*)n->Connections.prev(); pw!=0; pw = (Wire*)n->Connections.prev()) {
    if(pw->Type != isWire) continue;
    if(pw->isHorizontal()) continue;
    if(pw->y2 > w->y2) {
      if(n->Connections.count() != 2) continue;
      w->Name  = pw->Name; w->nx = pw->nx; w->ny = pw->ny;
                 w->delta = pw->delta;   // do not move node name label
      w->y2 = pw->y2;
      w->Port2 = pw->Port2;         // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port2->Connections.removeRef(pw);
      w->Port2->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    // (if new wire lies complete within an existing one, was already check before)

    if(pw->Port1->Connections.count() < 2) {
      pw->Port2->Connections.removeRef(pw); // existing wire lies within the new one
      Nodes.removeRef(pw->Port1);
      Wires.removeRef(pw);
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
// Inserts a vertical or horizontal wire into the schematic and connects the ports that hit together.
// Returns whether the beginning and ending (the ports of the wire) are connected or not.
int QucsDoc::insertWire(Wire *w)
{
  int  tmp, con = 0;
  bool ok;

  // change coordinates if necessary (port 1 coordinates must be less port 2 coordinates)
  if(w->x1 > w->x2) { tmp = w->x1; w->x1 = w->x2; w->x2 = tmp; }
  else
  if(w->y1 > w->y2) { tmp = w->y1; w->y1 = w->y2; w->y2 = tmp; }
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

  

  
  if(con > 255) con = ((con >> 1) & 1) | ((con << 1) & 2); // change node 1 and 2

  Wires.append(w);    // add wire to the schematic




//QMessageBox::critical(0, "Error", "0");
  int  n1, n2;
  Wire *pw, *nw;
  Node *pn, *pn2;
  // ................................................................
  // check if the new line covers existing nodes
  for(pw = Wires.current(); pw != 0; pw = Wires.next()) // in order to also check new appearing wires
    for(pn = Nodes.first(); pn != 0; ) {  // check every node
      if(pn->x == pw->x1) {
        if(pn->y <= pw->y1) { pn = Nodes.next(); continue; }
        if(pn->y >= pw->y2) { pn = Nodes.next(); continue; }
      }
      else if(pn->y == pw->y1) {
        if(pn->x <= pw->x1) { pn = Nodes.next(); continue; }
        if(pn->x >= pw->x2) { pn = Nodes.next(); continue; }
      }
      else { pn = Nodes.next(); continue; }

      n1 = 2; n2 = 3;
      pn2 = pn;
      // check all connections of the current node
      for(Element *pe = pn->Connections.first(); pe != 0; pe = pn->Connections.next()) {
        if(pe->Type != isWire) continue;
        nw = (Wire*)pe;
        if(pw->isHorizontal() != nw->isHorizontal()) continue;  // wire lies within the new ?

        pn  = nw->Port1;
        pn2 = nw->Port2;
        n1  = pn->Connections.count();
        n2  = pn2->Connections.count();
        if(n1 == 1) {
          Nodes.removeRef(pn);     // delete node 1 if open
          pn = pn2;
          pn->Connections.removeRef(nw);   // remove connection
        }

        if(n2 == 1) {
          pn->Connections.removeRef(nw);   // remove connection
          Nodes.removeRef(pn2);     // delete node 2 if open
          pn2 = pn;
        }

        if(pn == pn2) {
          Wires.removeRef(nw);    // delete wire
          Wires.findRef(pw);      // set back to current wire
        }
        break;
      }
      if(n1 == 1) if(n2 == 1) continue;

      // split wire into two wires
      if((pw->x1 != pn->x) || (pw->y1 != pn->y)) {
        nw = new Wire(pw->x1, pw->y1, pn->x, pn->y, pw->Port1, pn);
        pn->Connections.append(nw);
        Wires.append(nw);
        Wires.findRef(pw);
        pw->Port1->Connections.append(nw);
      }
      pw->Port1->Connections.removeRef(pw);
      pw->x1 = pn2->x;
      pw->y1 = pn2->y;
      pw->Port1 = pn2;
      pn2->Connections.append(pw);

      pn = Nodes.next();
    }

  oneLabel(w->Port1); // if two wire lines with different labels are connected, delete one label
  setChanged(true);
  return con;
}

// ---------------------------------------------------
Component* QucsDoc::searchSelSubcircuit()
{
  Component *sub=0;
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {  // test all components
    if(!pc->isSelected) continue;
    if(pc->Sign.left(3) != "Sub") continue;

    if(sub != 0) return 0;    // more than one subcircuit selected
    sub = pc;
  }
  return sub;
}

// ---------------------------------------------------
Component* QucsDoc::selectedComponent(int x, int y)
{
  int  x1, y1, x2, y2;

  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {  // test all components
    pc->Bounding(x1, y1, x2, y2);
    if((x >= x1) & (x <= x2) & (y >= y1) & (y <= y2)) return pc;
  }

  return 0;
}

// ---------------------------------------------------
Diagram* QucsDoc::selectedDiagram(int x, int y)
{
  int  x1, y1, x2, y2;

  for(Diagram *ptr1 = Diags.first(); ptr1 != 0; ptr1 = Diags.next()) {  // test all diagrams
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
Painting* QucsDoc::selectedPainting(int x, int y)
{
  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next()) {  // test all paintings
    if(pp->getSelected(x,y)) return pp;
  }

  return 0;
}

// ---------------------------------------------------
// Follows a wire line and selects it.
void QucsDoc::selectWireLine(Element *pe, Node *pn, bool ctrl)
{
  Node *pn_1st = pn;
  while(pn->Connections.count() == 2) {
    if(pn->Connections.first() == pe)  pe = pn->Connections.last();
    else  pe = pn->Connections.first();

    if(pe->Type != isWire) break;
    if(ctrl) pe->isSelected ^= ctrl;
    else pe->isSelected = true;
    
    if(((Wire*)pe)->Port1 == pn)  pn = ((Wire*)pe)->Port2;
    else  pn = ((Wire*)pe)->Port1;
    if(pn == pn_1st) break;  // avoid endless loop in wire loops
  }
}

// ---------------------------------------------------
// Selects the element that contains the coordinates x/y.
// Returns the pointer to the element.
// If 'flag' is true, the element can be deselected.
Element* QucsDoc::selectElement(int x, int y, bool flag)
{
  int  x1, y1, x2, y2;
  
  Element *pe_1st=0, *pe_sel=0;
  for(Component *pc = Comps.last(); pc != 0; pc = Comps.prev()) {    // test all components
    pc->Bounding(x1, y1, x2, y2);
    if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2) {
      if(flag) { pc->isSelected ^= flag; return pc; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pc->isSelected = true; return pc; }
      if(pe_1st == 0) pe_1st = pc;   // give access to elements that lie beneath
      if(pc->isSelected) pe_sel = pc;
    }
  }

  x1 = x; y1 = y;
  setOnGrid(x1, y1);
  for(Wire *pw = Wires.last(); pw != 0; pw = Wires.prev()) {    // test all wires
    if(pw->x1 <= x1) if(pw->x2 >= x1) if(pw->y1 <= y1) if(pw->y2 >= y1) {
      if(flag) { pw->isSelected ^= flag; return pw; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pw->isSelected = true; return pw; }
      if(pe_1st == 0) pe_1st = pw;   // give access to elements that lie beneath
      if(pw->isSelected) pe_sel = pw;
    }
  }

  for(Diagram *pd = Diags.last(); pd != 0; pd = Diags.prev()) {    // test all diagrams
    pd->Bounding(x1, y1, x2, y2);
    if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2) {
      if(flag) { pd->isSelected ^= flag; return pd; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pd->isSelected = true; return pd; }
      if(pe_1st == 0) pe_1st = pd;   // give access to elements that lie beneath
      if(pd->isSelected) pe_sel = pd;
    }
  }

  for(Painting *pp = Paints.last(); pp != 0; pp = Paints.prev())    // test all paintings
    if(pp->getSelected(x, y)) {
      if(flag) { pp->isSelected ^= flag; return pp; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pp->isSelected = true; return pp; }
      if(pe_1st == 0) pe_1st = pp;   // give access to elements that lie beneath
      if(pp->isSelected) pe_sel = pp;
    }

  if(pe_1st != 0) pe_1st->isSelected = true;
  return pe_1st;
}

// ---------------------------------------------------
// Deselects all elements except 'e'.
void QucsDoc::deselectElements(Element *e)
{
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())  // test all components
    if(e != pc)  pc->isSelected = false;

  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next())       // test all wires
    if(e != pw)  pw->isSelected = false;

  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next())    // test all diagrams
    if(e != pd)  pd->isSelected = false;

  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next()) // test all paintings
    if(e != pp)  pp->isSelected = false;
}

// ---------------------------------------------------
// For moving elements: If the moving element is connected to a not moving element,
// insert two wires. If the connected element is already a wire, use this wire.
// Otherwise create new wire.
void QucsDoc::NewMovingWires(QPtrList<Element> *p, Node *pn)
{
  if(pn->Connections.count() < 1) return;   // return, if connected node is also moving
  Element *pe = pn->Connections.getFirst();
  if(pe == (Element*)1) return;   // return, if it was already treated this way
  pn->Connections.prepend((Element*)1);  // to avoid doubling
  
  if(pn->Connections.count() == 2)    // 2, because of prepend (Element*)1
    if(pe->Type == isWire) {    // is it connected to exactly one wire ?

      // .................................................
      Wire *pw2=0, *pw = (Wire*)pe;

      Node *pn2 = pw->Port1;
      if(pn2 == pn) pn2 = pw->Port2;

      if(pn2->Connections.count() == 2) {   // two existing wires connected ?
        Element *pe2 = pn2->Connections.getFirst();
        if(pe2 == pe) pe2 = pn2->Connections.getLast();
        if(pe2 != (Element*)1)
          if(pe2->Type == isWire)   // the connected wire is again connected to exactly one wire ?
            pw2  = (Wire*)pe2;
      }
      
      // .................................................
      // reuse one wire
      p->append(pw);
      pw->Port1->Connections.removeRef(pw);   // remove connection 1
      pw->Port2->Connections.removeRef(pw);   // remove connection 2
      Wires.take(Wires.findRef(pw));
      int mask = 1;
      if(pw->isHorizontal()) mask = 2;

      if(pw->Port1 != pn) {
        pw->Port1 = (Node*)mask;
        pw->Port2 = (Node*)3;
      }
      else {
        pw->Port1 = (Node*)3;
        pw->Port2 = (Node*)mask;
      }

      // .................................................
      // create new wire ?
      if(pw2 == 0) {
        if(pw->Port1 == (Node*)3)
          p->append(new Wire(pw->x2, pw->y2, pw->x2, pw->y2, (Node*)mask, (Node*)0));
        else
          p->append(new Wire(pw->x1, pw->y1, pw->x1, pw->y1, (Node*)mask, (Node*)0));
        return;
      }

      
      // .................................................
      // reuse a second wire
      p->append(pw2);
      pw2->Port1->Connections.removeRef(pw2);   // remove connection 1
      pw2->Port2->Connections.removeRef(pw2);   // remove connection 2
      Wires.take(Wires.findRef(pw2));

      if(pw2->Port1 != pn2) {
        pw2->Port1 = (Node*)0;
        pw2->Port2 = (Node*)mask;
      }
      else {
        pw2->Port1 = (Node*)mask;
        pw2->Port2 = (Node*)0;
      }
      return;
    }
                  
  p->append(new Wire(pn->x, pn->y, pn->x, pn->y, (Node*)0, (Node*)1));  // only x2 moving
  p->append(new Wire(pn->x, pn->y, pn->x, pn->y, (Node*)1, (Node*)3));  // x1, x2, y2 moving
}

// ---------------------------------------------------
// For moving of elements: Copies all selected elements into the
// list 'p' and deletes them from the document.
void QucsDoc::copySelectedElements(QPtrList<Element> *p)
{
  Port      *pp;
  Component *pc;
  Wire      *pw;
  Diagram   *pd;
  Element   *pe;
  Node      *pn;

  for(pc = Comps.first(); pc != 0; )  // test all components
    if(pc->isSelected) {
      p->append(pc);

      for(pp = pc->Ports.first(); pp!=0; pp = pc->Ports.next()) {
        pp->Connection->Connections.removeRef((Element*)pc);   // delete all port connections
/*        if(pp->Connection->Connections.count() != 1) continue;
        pe1 = pp->Connection->Connections.getfirst();
        if(pe1->Type != isWire) continue;
        if(pe1->isSelected) continue;   they are all gone
        if(pe1->isSelected = true;*/
      }
      Comps.take();   // take component out of the document

      pc = Comps.current();
    }
    else pc = Comps.next();

  for(pw = Wires.first(); pw != 0; )       // test all wires
    if(pw->isSelected) {
      p->append(pw);

      pw->Port1->Connections.removeRef(pw);   // remove connection 1
      pw->Port2->Connections.removeRef(pw);   // remove connection 2
      Wires.take();

      pw = Wires.current();
    }
    else pw = Wires.next();

  for(pd = Diags.first(); pd != 0; )    // test all diagrams
    if(pd->isSelected) {
      p->append(pd);
      Diags.take();
      pd = Diags.current();
    }
    else pd = Diags.next();

  for(Painting *pp = Paints.first(); pp != 0; )    // test all paintings
    if(pp->isSelected) {
      p->append(pp);
      Paints.take();
      pp = Paints.current();
    }
    else pp = Paints.next();

  // ..............................................
  // Inserts wires, if a connection to a not moving element is found.
  for(pe = p->last(); pe!=0; pe = p->prev())  // go backwards in order not to test the new insertions
    switch(pe->Type) {
      case isComponent:
          pc = (Component*)pe;
          for(pp = pc->Ports.first(); pp!=0; pp = pc->Ports.next())
            NewMovingWires(p, pp->Connection);

          p->findRef(pe);   // back to the real current pointer
          break;
      case isWire:
          pw = (Wire*)pe;
          NewMovingWires(p, pw->Port1);
          NewMovingWires(p, pw->Port2);
          p->findRef(pe);   // back to the real current pointer
          break;
      default:   ;  // avoid compiler warning
    }


  // ..............................................
  // delete the unused nodes
  for(pn = Nodes.first(); pn!=0; ) {
    if(pn->Connections.getFirst() == (Element*)1) {
      pn->Connections.removeFirst();  // delete tag
      if(pn->Connections.count() == 2)
        if(oneTwoWires(pn)) {  // if possible, connect two wires to one
          pn = Nodes.current();
          continue;
        }
    }

    if(pn->Connections.count() == 0) {
      Nodes.remove();
      pn = Nodes.current();
      continue;
    }

    pn = Nodes.next();
  }
}

// ---------------------------------------------------
// Selects components that lie within the rectangle x1/y1, x2/y2.
int QucsDoc::selectComponents(int x1, int y1, int x2, int y2, bool flag)
{
  int  z=0;   // counts selected elements
  int  cx1, cy1, cx2, cy2;
  if(x1 > x2) { cx1 = x1; x1  = x2; x2  = cx1; }
  if(y1 > y2) { cy1 = y1; y1  = y2; y2  = cy1; }

  
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {    // test all components
    pc->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      pc->isSelected = true;  z++;
      continue;
    }
    if(pc->isSelected &= flag) z++;
  }

  
  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next()) {    // test all wires
    if(pw->x1 >= x1) if(pw->x2 <= x2) if(pw->y1 >= y1) if(pw->y2 <= y2) {
      pw->isSelected = true;  z++;
      continue;
    }
    if(pw->isSelected &= flag) z++;
  }


  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next()) {    // test all diagrams
    pd->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      pd->isSelected = true;  z++;
      continue;
    }
    if(pd->isSelected &= flag) z++;
  }

  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next()) {    // test all diagrams
    pd->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      pd->isSelected = true;  z++;
      continue;
    }
    if(pd->isSelected &= flag) z++;
  }

  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next()) {    // test all paintings
    pp->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      pp->isSelected = true;  z++;
      continue;
    }
    if(pp->isSelected &= flag) z++;
  }

  return z;
}

// ---------------------------------------------------
// Activates/deactivates components that lie within the rectangle x1/y1, x2/y2.
void QucsDoc::activateComps(int x1, int y1, int x2, int y2)
{
  int  cx1, cy1, cx2, cy2;
  if(x1 > x2) { cx1 = x1; x1  = x2; x2  = cx1; }
  if(y1 > y2) { cy1 = y1; y1  = y2; y2  = cy1; }

  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {    // test all components
    pc->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2)
      pc->isActive ^= true;    // change "active status"
  }
}

// ---------------------------------------------------
// Activate/deactivate component, if x/y lies within its boundings.
bool QucsDoc::activateComponent(int x, int y)
{
  int  x1, y1, x2, y2;
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {    // test all components
    pc->Bounding(x1, y1, x2, y2);
    if(x >= x1) if(x <= x2) if(y >= y1) if(y <= y2) {
      pc->isActive ^= true;    // change "active status"
      setChanged(true);
      return true;
    }
  }
  return false;
}

// ---------------------------------------------------
// Activates/deactivates all selected elements.
bool QucsDoc::activateComponents()
{
  bool sel = false;

  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())    // all selected component
    if(pc->isSelected) {
      pc->isActive ^= true;    // change "active status"
      sel = true;
    }

  setChanged(sel);
  return sel;
}

// ---------------------------------------------------
// Test, if wire connects wire line with more than one label and delete all further labels.
void QucsDoc::oneLabel(Node *n1)
{
  Wire *pw;
  Node *pn;
  bool named=false;   // wire line already named ?
  QPtrList<Node> Cons;

  for(pn = Nodes.first(); pn!=0; pn = Nodes.next())
    if(!pn->isNamed) pn->Name = "";   // erase all node names

  n1->Name = "x";  // mark Node as already checked
  Cons.append(n1);
  for(pn = Cons.first(); pn!=0; pn = Cons.next())
    for(Element *pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next()) {
      if(pe->Type != isWire) continue;
      pw = (Wire*)pe;

      if(pn != pw->Port1) {
        if(!pw->Port1->Name.isEmpty()) continue;
        pw->Port1->Name = "x";  // mark Node as already checked
        Cons.append(pw->Port1);
        Cons.findRef(pn);
      }
      else {
        if(!pw->Port2->Name.isEmpty()) continue;
        pw->Port2->Name = "x";  // mark Node as already checked
        Cons.append(pw->Port2);
        Cons.findRef(pn);
      }

      if(!pw->Name.isEmpty()) {
        if(named) pw->Name = "";    // erase double names
        else named = true;
      }
    }
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the labeled wire.
Wire* QucsDoc::getWireLabel(Wire *w)
{
  Wire *pw;
  Node *pn;
  QPtrList<Node> Cons;

  if(!w->Name.isEmpty()) return w;
  
  for(pn = Nodes.first(); pn!=0; pn = Nodes.next())
    if(!pn->isNamed) pn->Name = "";   // erase all node names

  Cons.append(w->Port1);
  w->Port1->Name = "x";  // mark Node as already checked
  for(pn = Cons.first(); pn!=0; pn = Cons.next())
    for(Element *pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next()) {
      if(pe->Type != isWire) continue;
      
      pw = (Wire*)pe;
      if(!pw->Name.isEmpty()) return pw;

      if(pn != pw->Port1) {
        if(!pw->Port1->Name.isEmpty()) continue;
        pw->Port1->Name = "x";  // mark Node as already checked
        Cons.append(pw->Port1);
        Cons.findRef(pn);
      }
      else {
        if(!pw->Port2->Name.isEmpty()) continue;
        pw->Port2->Name = "x";  // mark Node as already checked
        Cons.append(pw->Port2);
        Cons.findRef(pn);
      }
    }
  return 0;
}

// ---------------------------------------------------
void QucsDoc::sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax)
{
  xmin=INT_MAX;
  ymin=INT_MAX;
  xmax=INT_MIN;
  ymax=INT_MIN;
  Component *pc;
  Diagram *pd;
  Wire *pw;
  Painting *pp;

  if(Comps.isEmpty())
    if(Wires.isEmpty())
      if(Diags.isEmpty())
        if(Paints.isEmpty()) {
          xmin = xmax = 0;
          ymin = ymax = 0;
          return;
      }

  int x1, y1, x2, y2;
  for(pc = Comps.first(); pc != 0; pc = Comps.next()) {  // find bounds of all components
    pc->entireBounds(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  for(pw = Wires.first(); pw != 0; pw = Wires.next()) {  // find bounds of all wires
    if(pw->x1 < xmin) xmin = pw->x1;
    if(pw->x2 > xmax) xmax = pw->x2;
    if(pw->y1 < ymin) ymin = pw->y1;
    if(pw->y2 > ymax) ymax = pw->y2;
  }

  for(pd = Diags.first(); pd != 0; pd = Diags.next()) {  // find bounds of all diagrams
    pd->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  for(pp = Paints.first(); pp != 0; pp = Paints.next()) {  // find bounds of all Paintings
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

// ---------------------------------------------------
// Sets the component ports.
void QucsDoc::setCompPorts(Component *pc)
{
  for(Port *pp = pc->Ports.first(); pp!=0; pp = pc->Ports.next()) {
    pp->Connection->Connections.removeRef((Element*)pc);   // delete all port connections
    switch(pp->Connection->Connections.count()) {
      case 0: Nodes.removeRef(pp->Connection);
              break;
      case 2: oneTwoWires(pp->Connection);   // if possible, connect two wires to one
      default: ;
    }
    // connect component node to schematic node
    pp->Connection = insertNode(pp->x+pc->cx, pp->y+pc->cy, pc);
  }
}

// ---------------------------------------------------
bool QucsDoc::copyCompsWires(int& x1, int& y1, int& x2, int& y2)
{
  x1=INT_MAX;
  y1=INT_MAX;
  x2=INT_MIN;
  y2=INT_MIN;
  Component *pc;
  Wire *pw;
  Painting *pp;

  for(pc = Comps.first(); pc != 0; ) {   // find bounds of all selected components
    if(pc->isSelected) if(pc->Ports.count() > 0) {  // do not copy components without ports
      if(pc->cx < x1) x1 = pc->cx;
      if(pc->cx > x2) x2 = pc->cx;
      if(pc->cy < y1) y1 = pc->cy;
      if(pc->cy > y2) y2 = pc->cy;

      ElementCache.append(pc);
      deleteComp(pc);
      pc = Comps.current();
      continue;
    }
    pc = Comps.next();
  }

  for(pw = Wires.first(); pw != 0; )  // find bounds of all selected wires
    if(pw->isSelected) {
      if(pw->x1 < x1) x1 = pw->x1;
      if(pw->x2 > x2) x2 = pw->x2;
      if(pw->y1 < y1) y1 = pw->y1;
      if(pw->y2 > y2) y2 = pw->y2;

      ElementCache.append(pw);
      deleteWire(pw);
      pw = Wires.current();
    }
    else pw = Wires.next();

  int bx1, by1, bx2, by2;
  for(pp = Paints.first(); pp != 0; )  // find bounds of all selected paintings
    if(pp->isSelected) {
      pp->Bounding(bx1, by1, bx2, by2);
      if(bx1 < x1) x1 = bx1;
      if(bx2 > x2) x2 = bx2;
      if(by1 < y1) y1 = by1;
      if(by2 > y2) y2 = by2;

      ElementCache.append(pp);
      Paints.take();
      pp = Paints.current();
    }
    else pp = Paints.next();

  if(y1 == INT_MAX) return false;   // no element selected
  return true;
}

// ---------------------------------------------------
// Rotates all selected components around their midpoint.
bool QucsDoc::rotateElements()
{
  Wires.setAutoDelete(false);
  Comps.setAutoDelete(false);

  int x1, y1, x2, y2, tmp;
  if(!copyCompsWires(x1, y1, x2, y2)) return false;
  Wires.setAutoDelete(true);
  Comps.setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // center for rotation
  y1 = (y1+y2) >> 1;
  setOnGrid(x1, y1);

  
  Component *pc;
  Wire *pw;
  Painting *pp;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent: pc = (Component*)pe;
                        pc->rotate();   //rotate component !before! rotating its center
                        x2 = x1 - pc->cx;
                        pc->setCenter(pc->cy - y1 + x1, x2 + y1);
                        insertRawComponent(pc);
                        break;
      case isWire:      pw = (Wire*)pe;
                        x2 = pw->x1;
                        pw->x1 = pw->y1 - y1 + x1;
                        pw->y1 = x1 - x2 + y1;
                        x2 = pw->x2;
                        pw->x2 = pw->y2 - y1 + x1;
                        pw->y2 = x1 - x2 + y1;
                        insertWire(pw);
                        break;
      case isPainting:  pp = (Painting*)pe;
                        pp->rotate();   // rotate painting !before! rotating its center
                        pp->getCenter(x2, y2);
                        tmp = x1 - x2;
                        pp->setCenter(y2 - y1 + x1, tmp + y1);
                        Paints.append(pp);
                        break;
      default:          ;
    }

  ElementCache.clear();

  setChanged(true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool QucsDoc::mirrorXComponents()
{
  Wires.setAutoDelete(false);
  Comps.setAutoDelete(false);

  int x1, y1, x2, y2;
  if(!copyCompsWires(x1, y1, x2, y2)) return false;
  Wires.setAutoDelete(true);
  Comps.setAutoDelete(true);

  y1 = (y1+y2) >> 1;   // axis for mirroring
  setOnGrid(y2, y1);


  Wire *pw;
  Component *pc;
  Painting *pp;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent: pc = (Component*)pe;
                        pc->mirrorX();   // mirror component !before! mirroring its center
                        pc->setCenter(pc->cx, (y1<<1) - pc->cy);
                        insertRawComponent(pc);
                        break;
      case isWire:      pw = (Wire*)pe;
                        pw->y1 = (y1<<1) - pw->y1;
                        pw->y2 = (y1<<1) - pw->y2;
                        insertWire(pw);
                        break;
      case isPainting:  pp = (Painting*)pe;
                        pp->getCenter(x2, y2);
                        pp->mirrorX();   // mirror painting !before! mirroring its center
                        pp->setCenter(x2, (y1<<1) - y2);
                        Paints.append(pp);
                        break;
      default:          ;
    }

  ElementCache.clear();
  setChanged(true);
  return true;
}

// ---------------------------------------------------
// Mirrors all selected components. First copy them to 'ElementCache', then mirror and insert again.
bool QucsDoc::mirrorYComponents()
{
  Wires.setAutoDelete(false);
  Comps.setAutoDelete(false);

  int x1, y1, x2, y2;
  if(!copyCompsWires(x1, y1, x2, y2)) return false;
  Wires.setAutoDelete(true);
  Comps.setAutoDelete(true);

  x1 = (x1+x2) >> 1;   // axis for mirroring
  setOnGrid(x1, x2);


  Wire *pw;
  Component *pc;
  Painting *pp;
  // re-insert elements
  for(Element *pe = ElementCache.first(); pe != 0; pe = ElementCache.next())
    switch(pe->Type) {
      case isComponent: pc = (Component*)pe;
                        pc->mirrorY();   // mirror component !before! mirroring its center
                        pc->setCenter((x1<<1) - pc->cx, pc->cy);
                        insertRawComponent(pc);
                        break;
      case isWire:      pw = (Wire*)pe;
                        pw->x1 = (x1<<1) - pw->x1;
                        pw->x2 = (x1<<1) - pw->x2;
                        insertWire(pw);
                        break;
      case isPainting:  pp = (Painting*)pe;
                        pp->getCenter(x2, y2);
                        pp->mirrorY();   // mirror painting !before! mirroring its center
                        pp->setCenter((x1<<1) - x2, y2);
                        Paints.append(pp);
                        break;
      default:          ;
    }

  ElementCache.clear();
  setChanged(true);
  return true;
}

// ---------------------------------------------------
// If possible, make one wire out of two wires.
bool QucsDoc::oneTwoWires(Node *n)
{
  Wire *e3;
  Wire *e1 = (Wire*)n->Connections.first();  // two wires -> one wire
  Wire *e2 = (Wire*)n->Connections.last();

  if(e1->Type == isWire) if(e2->Type == isWire)
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
      Nodes.removeRef(n);    // delete node (is auto delete)
      e1->Port2->Connections.removeRef(e2);
      e1->Port2->Connections.append(e1);
      Wires.removeRef(e2);
      return true;
    }
  return false;
}

// ---------------------------------------------------
// Deletes the component 'c'.
void QucsDoc::deleteComp(Component *c)
{
  Port *pn;

  for(pn = c->Ports.first(); pn!=0; pn = c->Ports.next())   // delete all port connections
    switch(pn->Connection->Connections.count()) {
      case 1  : Nodes.removeRef(pn->Connection);     // delete all open nodes
                pn->Connection = 0;
                break;
      case 3  : pn->Connection->Connections.removeRef(c);  // delete connection to component
                oneTwoWires(pn->Connection);    // two wires -> one wire
                break;
      default : pn->Connection->Connections.removeRef(c);   // remove connection
                break;
    }

  Comps.removeRef(c);   // delete component
}

// ---------------------------------------------------
// Deletes the wire 'w'.
void QucsDoc::deleteWire(Wire *w)
{
  if(w->Port1->Connections.count() == 1)
    Nodes.removeRef(w->Port1);     // delete node 1 if open
  else {
    w->Port1->Connections.removeRef(w);   // remove connection
    if(w->Port1->Connections.count() == 2) oneTwoWires(w->Port1);  // two wires -> one wire
  }

  if(w->Port2->Connections.count() == 1)
    Nodes.removeRef(w->Port2);     // delete node 2 if open
  else {
    w->Port2->Connections.removeRef(w);   // remove connection
    if(w->Port2->Connections.count() == 2) oneTwoWires(w->Port2);  // two wires -> one wire
  }

  Wires.removeRef(w);
}

// ---------------------------------------------------
// Deletes all selected elements.
bool QucsDoc::deleteElements()
{
  bool sel = false;
  
  Component *pc = Comps.first();
  while(pc != 0)      // all selected component
    if(pc->isSelected) {
      deleteComp(pc);
      pc = Comps.current();
      sel = true;
    }
    else pc = Comps.next();

  Wire *pw = Wires.first();
  while(pw != 0)      // all selected wires
    if(pw->isSelected) {
      deleteWire(pw);
      pw = Wires.current();
      sel = true;
    }
    else pw = Wires.next();

  Diagram *pd = Diags.first();
  while(pd != 0)      // test all diagrams
    if(pd->isSelected) {
      Diags.remove();
      pd = Diags.current();
      sel = true;
    }
    else pd = Diags.next();

  Painting *pp = Paints.first();
  while(pp != 0)      // test all paintings
    if(pp->isSelected) {
      Paints.remove();
      pp = Paints.current();
      sel = true;
    }
    else pp = Paints.next();

  setChanged(sel);
  return sel;
}

// ---------------------------------------------------
// Updates the graph data of all diagrams.
void QucsDoc::reloadGraphs()
{
  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next())
    pd->loadGraphData(DataSet);  // load graphs from data files
}

// ---------------------------------------------------
QString QucsDoc::copySelected(bool cut)
{
  int z=0;  // counts selected elements
  Component *pc;
  Wire *pw;
  Diagram *pd;
  Painting *pp;
  
  QString s("<Qucs Schematic " VERSION ">\n");

  // Build element document.
  s += "<Components>\n";
  for(pc = Comps.first(); pc != 0; pc = Comps.next())
    if(pc->isSelected) {
      s += pc->save()+"\n";  z++; }
  s += "</Components>\n";

  s += "<Wires>\n";
  for(pw = Wires.first(); pw != 0; pw = Wires.next())
    if(pw->isSelected) {
      s += pw->save()+"\n";  z++; }
  s += "</Wires>\n";

  s += "<Diagrams>\n";
  for(pd = Diags.first(); pd != 0; pd = Diags.next())
    if(pd->isSelected) {
      s += pd->save()+"\n";  z++; }
  s += "</Diagrams>\n";

  s += "<Paintings>\n";
  for(pp = Paints.first(); pp != 0; pp = Paints.next())
    if(pp->isSelected) {
      s += pp->save()+"\n";  z++; }
  s += "</Paintings>\n";

  if(z == 0) return "";   // return empty if no selection

  if(cut) deleteElements();   // delete selected elements if wanted
  return s;
}

// ---------------------------------------------------
// Paste from clipboard.
bool QucsDoc::paste(QTextStream *stream, QPtrList<Element> *pe)
{
  QString Line;

  Cache = pe;
  
  Line = stream->readLine();
  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
//    QMessageBox::critical(0, "Error", "Clipboard content is not a Qucs schematic!");
    return false;
  }

  QString s = VERSION;
  Line = Line.mid(16, Line.length()-17);
  if(Line != s) {  // wrong version number ?
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Wrong document version: ")+Line);
    return false;
  }

  // read content *************************
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "<Components>") { if(!loadComponents(stream, false)) return false; }
    else
    if(Line == "<Wires>") { if(!loadWires(stream, false)) return false; }
    else
    if(Line == "<Diagrams>") { if(!loadDiagrams(stream, false)) return false; }
    else
    if(Line == "<Paintings>") { if(!loadPaintings(stream, false)) return false; }
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Clipboard Format Error:\nUnknown field!"));
      return false;
    }
  }

  return true;
}

// ---------------------------------------------------
int QucsDoc::save()
{
  QFile file(DocName);
  if(!file.open(IO_WriteOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Cannot save document!"));
    return -1;
  }
  
  QTextStream stream(&file);

  stream << "<Qucs Schematic " << VERSION << ">\n";

  stream << "<Properties>\n";
  stream << "   <View=" << ViewX1<<","<<ViewY1<<","<<ViewX2<<","<<ViewY2<<","<<Scale<<",";
                 stream << PosX<<","<<PosY << ">\n";
  stream << "   <Grid=" << GridX<<","<<GridY<<","<<GridOn << ">\n";
  stream << "   <DataSet=" << DataSet << ">\n";
  stream << "   <DataDisplay=" << DataDisplay << ">\n";
  stream << "   <OpenDisplay=" << SimOpenDpl << ">\n";
  stream << "</Properties>\n";
    
  int z=0;
  stream << "<Components>\n";
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {  // save all components
    stream << pc->save() << "\n";
    if(pc->Sign == "Port") z++;
  }
  stream << "</Components>\n";
    
  stream << "<Wires>\n";
  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next())  // save all wires
    stream << pw->save() << "\n";
  stream << "</Wires>\n";
    
  stream << "<Diagrams>\n";
  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next())  // save all diagrams
    stream << pd->save() << "\n";
  stream << "</Diagrams>\n";

  stream << "<Paintings>\n";
  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next())  // save all paintings
    stream << pp->save() << "\n";
  stream << "</Paintings>\n";

  file.close();
  setChanged(false);
  return z;
}

// ---------------------------------------------------
bool QucsDoc::loadProperties(QTextStream *stream)
{
  bool ok = true;
  QString Line, cstr, nstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Properties>") return true;
    Line = Line.stripWhiteSpace();

    if(Line.at(0) != '<') {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    if(Line.at(Line.length()-1) != '>') {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong property field limiter!"));
      return false;
    }
    Line = Line.mid(1, Line.length()-2);   // cut off start and end character

    cstr = Line.section('=',0,0);    // property type
    nstr = Line.section('=',1,1);    // property value
         if(cstr == "View") { ViewX1 = nstr.section(',',0,0).toInt(&ok); if(ok) {
                              ViewY1 = nstr.section(',',1,1).toInt(&ok); if(ok) {
                              ViewX2 = nstr.section(',',2,2).toInt(&ok); if(ok) {
                              ViewY2 = nstr.section(',',3,3).toInt(&ok); if(ok) {
                              Scale  = nstr.section(',',4,4).toDouble(&ok); if(ok) {
                              PosX   = nstr.section(',',5,5).toInt(&ok); if(ok) {
                              PosY   = nstr.section(',',6,6).toInt(&ok); }}}}}} }
    else if(cstr == "Grid") { GridX = nstr.section(',',0,0).toInt(&ok); if(ok) {
                              GridY = nstr.section(',',1,1).toInt(&ok); if(ok) {
                              if(nstr.section(',',2,2).toInt(&ok) == 0) GridOn = false;
                              else GridOn = true; }} }
    else if(cstr == "DataSet") DataSet = nstr;
    else if(cstr == "DataDisplay") DataDisplay = nstr;
    else if(cstr == "OpenDisplay") if(nstr.toInt(&ok) == 0) SimOpenDpl = false;
                                   else SimOpenDpl = true;
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown property: ")+cstr);
      return false;
    }
    if(!ok) {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nNumber expected in property field!"));
      return false;
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Property' field is not closed!"));
  return false;
}

bool QucsDoc::loadComponents(QTextStream *stream, bool insert)
{
  QString Line, cstr;
  Component *c;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Components>") return true;

    Line = Line.stripWhiteSpace();
    cstr = Line.section(' ',0,0);    // component type
         if(cstr == "<R") c = new Resistor();
    else if(cstr == "<Rus") c = new ResistorUS();
    else if(cstr == "<C") c = new Capacitor();
    else if(cstr == "<L") c = new Inductor();
    else if(cstr == "<GND") c = new Ground();
    else if(cstr == "<Tr") c = new Transformer();
    else if(cstr == "<sTr") c = new symTrafo();
    else if(cstr == "<Vdc") c = new Volt_dc();
    else if(cstr == "<Vac") c = new Volt_ac();
    else if(cstr == "<Pac") c = new Source_ac();
    else if(cstr == "<Idc") c = new Ampere_dc();
    else if(cstr == "<VCCS") c = new VCCS();
    else if(cstr == "<CCCS") c = new CCCS();
    else if(cstr == "<VCVS") c = new VCVS();
    else if(cstr == "<CCVS") c = new CCVS();
    else if(cstr == "<Port") c = new SubCirPort();
    else if(cstr.left(7) == "<SPfile") { c = new SParamFile(cstr.mid(7).toInt()); }
    else if(cstr.left(4) == "<Sub") { c = new Subcircuit(cstr.mid(4).toInt()); }
    else if(cstr == "<DCBlock") c = new dcBlock();
    else if(cstr == "<DCFeed") c = new dcFeed();
    else if(cstr == "<BiasT") c = new BiasT();
    else if(cstr == "<Attenuator") c = new Attenuator();
    else if(cstr == "<Isolator") c = new Isolator();
    else if(cstr == "<Circulator") c = new Circulator();
    else if(cstr == "<TLIN") c = new TLine();
    else if(cstr == "<SUBST") c = new Substrate();
    else if(cstr == "<MLIN") c = new MSline();
    else if(cstr == "<MSTEP") c = new MSstep();
    else if(cstr == "<MCORN") c = new MScorner();
    else if(cstr == "<MTEE") c = new MStee();
    else if(cstr == "<MCROSS") c = new MScross();
    else if(cstr == "<Diode") c = new Diode();
    else if(cstr == "<.DC") c = new DC_Sim();
    else if(cstr == "<.AC") c = new AC_Sim();
    else if(cstr == "<.TR") c = new TR_Sim();
    else if(cstr == "<.SP") c = new SP_Sim();
    else if(cstr == "<.HB") c = new HB_Sim();
    else if(cstr == "<.SW") c = new Param_Sweep();
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown component!"));
      return false;
    }
    if(!c->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'component' line format!"));
      delete c;
      return false;
    }

    if(insert)
      insertRawComponent(c, getComponentPos(c));   // insert component at appropriate position ...
      // (... for power sources and ports (only necessary if file was manipulated outside qucs) )
    else {
      int z;
      for(z=c->Name.length()-1; z>=0; z--)  // cut off number of component name
        if(!c->Name.at(z).isDigit()) break;
      c->Name = c->Name.left(z+1);
      Cache->append((Element*)c);
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Component' field is not closed!"));
  return false;
}

bool QucsDoc::loadWires(QTextStream *stream, bool insert)
{
  Wire *w;
  QString Line;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Wires>") return true;

    Line = Line.stripWhiteSpace();
    w = new Wire();
    if(!w->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'wire' line format!"));
      return false;
    }
    if(insert) insertWire(w);
    else {
      w->Port1 = (Node*)4;    // move all ports (later on)
      w->Port2 = (Node*)4;
      Cache->append((Element*)w);
    }
  }

  QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Wire' field is not closed!"));
  return false;
}

bool QucsDoc::loadDiagrams(QTextStream *stream, bool insert)
{
  Diagram *d;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Diagrams>") return true;
    Line = Line.stripWhiteSpace();

    cstr = Line.section(' ',0,0);    // diagram type
         if(cstr == "<Rect") d = new RectDiagram();
    else if(cstr == "<Polar") d = new PolarDiagram();
    else if(cstr == "<Tab") d = new TabDiagram();
    else if(cstr == "<Smith") d = new SmithDiagram();
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown diagram!"));
      return false;
    }

    if(!d->load(Line, stream)) {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'diagram' line format!"));
      delete d;
      return false;
    }
    if(insert) Diags.append(d);
    else Cache->append((Element*)d);
  }

  QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Diagram' field is not closed!"));
  return false;
}

bool QucsDoc::loadPaintings(QTextStream *stream, bool insert)
{
  Painting *p;
  QString Line, cstr;
  while(!stream->atEnd()) {
    Line = stream->readLine();
    if(Line == "</Paintings>") return true;
    Line = Line.stripWhiteSpace();

    cstr = Line.section(' ',0,0);    // painting type
         if(cstr == "<Rectangle") p = new Rectangle();
    else if(cstr == "<Text") p = new GraphicText();
    else if(cstr == "<Line") p = new GraphicLine();
    else if(cstr == "<Arrow") p = new Arrow();
    else if(cstr == "<Ellipse") p = new Ellipse();
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nUnknown painting!"));
      return false;
    }

    if(!p->load(Line)) {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\nWrong 'painting' line format!"));
      delete p;
      return false;
    }
    if(insert) Paints.append(p);
    else Cache->append((Element*)p);
  }

  QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Format Error:\n'Painting' field is not closed!"));
  return false;
}

// ......................................................................
bool QucsDoc::load()
{
  QFile file(DocName);
  if(!file.open(IO_ReadOnly)) {
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Cannot load document: ")+DocName);
    return false;
  }

  QString Line;
  QTextStream stream(&file);

  // read header **************************
  if(stream.atEnd()) {
    file.close();
//    QMessageBox::critical(0, "Error", "Document is empty!");
//    return false;
    return true;
  }
  Line = stream.readLine();
  if(Line.left(16) != "<Qucs Schematic ") {  // wrong file type ?
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Wrong document type!"));
    return false;
  }

  QString s = VERSION;
  Line = Line.mid(16, Line.length()-17);
  if(Line != s) {  // wrong version number ?
    file.close();
    QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("Wrong document version: ")+Line);
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
    else
    if(Line == "<Paintings>") { if(!loadPaintings(&stream)) { file.close(); return false; } }
    else {
      QMessageBox::critical(0, QObject::tr("Error"), QObject::tr("File Format Error:\nUnknown field!"));
      file.close();
      return false;
    }
  }

  file.close();
  setChanged(false);
  return true;
}

// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component.
bool QucsDoc::giveNodeNames(QTextStream *stream)
{
  Node *p1, *p2;
  Wire *pw;
  Element *pe;

  // delete the node names not given by the user
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next())
    if(!p1->isNamed) p1->Name = "";

  // set the wire names to the connected node
  for(pw = Wires.first(); pw != 0; pw = Wires.next())
    if(!pw->Name.isEmpty()) pw->Port1->Name = pw->Name;

  // give the ground nodes the name "0", and insert subcircuits
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())
    if(pc->Sign == "GND") pc->Ports.first()->Connection->Name = "gnd";
    else if(pc->Sign.left(3) == "Sub") {
           QucsDoc *d = new QucsDoc(0, pc->Props.getFirst()->Value);
           if(!d->load()) {    // load document if possible
             delete d;
             return false;
           }
           d->createSubNetlist(stream);
         }


  QPtrList<Node> Cons;
  // work on named nodes first in order to preserve the user given names
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next()) {
    if(p1->Name.isEmpty()) continue;
    Cons.append(p1);
    for(p2 = Cons.first(); p2 != 0; p2 = Cons.next())
      for(pe = p2->Connections.first(); pe != 0; pe = p2->Connections.next())
        if(pe->Type == isWire) {
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
        if(pe->Type == isWire) {
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

  return true;
}

// ---------------------------------------------------
// Write the netlist as subcircuit to the text stream 'NetlistFile'.
bool QucsDoc::createSubNetlist(QTextStream *stream)
{
  if(!giveNodeNames(stream)) return false;

  QStringList sl;
  Component *pc;
  for(pc = Comps.first(); pc != 0; pc = Comps.next())
    if(pc->Sign == "Port")
      sl.append(pc->Props.first()->Value + ":" + pc->Ports.getFirst()->Connection->Name);
  sl.sort();
  
//  QTextStream stream(NetlistFile);
  (*stream) << "\nsubcircuit " << DocName << "  " << sl.join(" ") << "\n";

  
  QString s;
  // write all components with node names into the netlist file
  for(pc = Comps.first(); pc != 0; pc = Comps.next()) {
    s = pc->NetList();
    if(!s.isEmpty()) (*stream) << "   " << s << "\n";
  }

  (*stream) << "end subcircuit\n\n";
  return true;
}

// ---------------------------------------------------
// Creates the file "netlist.net" in the project directory. Returns "true"
// if successful.
bool QucsDoc::createNetlist(QFile *NetlistFile)
{
  if(!NetlistFile->open(IO_WriteOnly)) return false;

  QTextStream stream(NetlistFile);
  stream << "# Qucs " << VERSION << "  " << DocName << "\n";    // first line is docu


  if(!giveNodeNames(&stream)) {
    NetlistFile->close();
    return false;
  }

  // .................................................
  QString s;
  // write all components with node names into the netlist file
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {
    s = pc->NetList();
    if(!s.isEmpty()) stream << s << "\n";
  }
  NetlistFile->close();

  return true;
}
