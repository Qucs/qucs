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
".	c #595963","+	c #E6E6F1","@	c #465460","#	c #FEFEFF",
"$	c #DEDEEE","%	c #43535F","&	c #D1D1E6","*	c #5E5E66",
"=	c #FFFFFF","-	c #C5C5DF",";	c #FCF8F9",">	c #BDBDDA",
",	c #BFBFDC","'	c #C4C4DF",")	c #FBF7F7","!	c #D6D6E9",
"~	c #CBCBE3","{	c #B5B5D6","]	c #BCBCDA","^	c #C6C6E0",
"/	c #CFCFE5","(	c #CEC9DC","_	c #D8D8EA",":	c #DADAEB",
"<	c #313134","[	c #807FB3","}	c #AEAED1","|	c #B7B7D7",
"1	c #E2E2EF","2	c #9393C0","3	c #E3E3F0","4	c #DDD5E1",
"5	c #E8E8F3","6	c #2F2F31","7	c #7B7BAF","8	c #8383B5",
"9	c #151518","0	c #000000","a	c #C0C0DC","b	c #8E8FBD",
"c	c #8989BA","d	c #E7EEF6","e	c #282829","f	c #6867A1",
"g	c #7373A9","h	c #A7A7CD","i	c #8080B3","j	c #7B7CB0",
"k	c #7070A8","l	c #6D6DA5","m	c #6E6EA6","n	c #6969A2",
"o	c #7A79AF","p	c #DCDCEC","q	c #60609A","r	c #7777AC",
"s	c #5D5D98","t	c #7676AB","u	c #484785","v	c #575793",
"w	c #50506A","x	c #8787B8","y	c #53536E","z	c #07070E",
"A	c #666688",
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



QucsDoc::QucsDoc(QTabBar *b, const QString& _Name) : File(this)
{
  GridX  = 10;
  GridY  = 10;
  GridOn = true;
  Scale=1.0;
  ViewX1=ViewY1=0;
  ViewX2=ViewY2=800;
  PosX=PosY=0;
  UsedX1=UsedY1=UsedX2=UsedY2=0;

  DocName = _Name;
  if(_Name.isEmpty()) Tab = new QTab(QObject::tr("untitled"));
  else {
    QFileInfo Info(DocName);
    Tab = new QTab(Info.fileName());
    DataSet = Info.baseName()+".dat";   // name of the default dataset
    if(Info.extension(false) == "sch")
      DataDisplay = Info.baseName()+".dpl"; // name of default data display
    else {
      DataDisplay = Info.baseName()+".sch"; // name of default schematic
      GridOn = false;     // data display without grid (per default)
    }
  }
  SimOpenDpl = true;

  Bar = b;
  if(Bar != 0) {
    Bar->addTab(Tab);  // create tab in TabBar
//    Bar->setCurrentTab(Tab);  // make it the current  ===>  Qt 3.1 CRASHES !!!???!!!
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
  Tab->setText(Info.fileName());  // remove path from file name, show it in TabBar

  DataSet = Info.baseName()+".dat";   // name of default dataset
  if(Info.extension(false) == "sch")
    DataDisplay = Info.baseName()+".dpl";   // name of default data display
  else
    DataDisplay = Info.baseName()+".sch";   // name of default data display
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
// Inserts a port into the schematic and connects it to another node if
// the coordinates are identical. The node is returned.
Node* QucsDoc::insertNode(int x, int y, Element *e)
{
  Node *pn;
  // check if new node lies upon existing node
  for(pn = Nodes.first(); pn != 0; pn = Nodes.next())  // check every node
    if(pn->cx == x) if(pn->cy == y) {
      pn->Connections.append(e);
      break;
    }

  if(pn == 0) { // create new node, if no existing one lies at this position
    pn = new Node(x, y);
    Nodes.append(pn);
    pn->Connections.append(e);  // connect schematic node to component node
  }
  else return pn;   // return, if node is not new

  // check if the new node lies upon an existing wire
  Wire *nw;
  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next()) {
    if(pw->x1 == x) {
      if(pw->y1 > y) continue;
      if(pw->y2 < y) continue;
    }
    else if(pw->y1 == y) {
      if(pw->x1 > x) continue;
      if(pw->x2 < x) continue;
    }
    else continue;

    // split the wire into two wires
    nw = new Wire(x, y, pw->x2, pw->y2, pn, pw->Port2);

    pw->x2 = x;
    pw->y2 = y;
    pw->Port2 = pn;

    nw->Port2->Connections.append(nw);
    pn->Connections.append(pw);
    pn->Connections.append(nw);
    nw->Port2->Connections.removeRef(pw);
    Wires.append(nw);
    return pn;
  }

  return pn;
}

// ---------------------------------------------------
// Finds the correct number for power sources and subcircuit ports and
// sets them accordingly.
void QucsDoc::setComponentNumber(Component *c)
{
  if(c->Sign != "Port")
    if(c->Sign != "Pac") return;  // number ports and power sources only

  int n=1;
  QString s, cSign = c->Sign;
  Component *pc;
  // power sources and subcirciut ports have to be numbered
  do {
    s  = QString::number(n);
    // look for existing ports and their numbers
    for(pc = Comps.first(); pc != 0; pc = Comps.next())
      if(pc->Sign == cSign)
        if(pc->Props.getFirst()->Value == s) break;

    n++;
  } while(pc);     // found not used component number
  c->Props.getFirst()->Value = s; // set new number
}

// ---------------------------------------------------
void QucsDoc::insertRawComponent(Component *c, bool num)
{
  // connect every node of component to corresponding schematic node
  for(Port *ptr = c->Ports.first(); ptr != 0; ptr = c->Ports.next())
    ptr->Connection = insertNode(ptr->x+c->cx, ptr->y+c->cy, c);

  if(num) setComponentNumber(c);
  Comps.append(c);

  // a ground symbol erases an existing label on the wire line
  if(c->Sign == "GND") {
    c->Sign = "x";    // prevent that this ground is found as label
    Element *pe = getWireLabel(c->Ports.getFirst()->Connection);
    if(pe) {
      if(pe->Type == isWire) {
        delete ((Wire*)pe)->Label;
        ((Wire*)pe)->Label = 0;
      }
      else if(pe->Type == isNode) {
        delete ((Node*)pe)->Label;
        ((Node*)pe)->Label = 0;
      }
    }
    c->Sign = "GND";    // rebuild component sign
  }

  setChanged(true);
}

// ---------------------------------------------------
void QucsDoc::insertComponent(Component *c)
{
  // connect every node of the component to corresponding schematic node
  for(Port *ptr = c->Ports.first(); ptr != 0; ptr = c->Ports.next())
    ptr->Connection = insertNode(ptr->x+c->cx, ptr->y+c->cy, c);

  bool ok;
  QString s;
  int  max=1, len = c->Name.length(), z;
  if(c->Name.isEmpty()) {
    // a ground symbol erases an existing label on the wire line
    if(c->Sign == "GND") {
      c->Sign = "x";    // prevent that this ground is found as label
      Element *pe = getWireLabel(c->Ports.getFirst()->Connection);
      if(pe) {
        if(pe->Type == isWire) {
          delete ((Wire*)pe)->Label;
          ((Wire*)pe)->Label = 0;
        }
        else if(pe->Type == isNode) {
          delete ((Node*)pe)->Label;
          ((Node*)pe)->Label = 0;
        }
      }
      c->Sign = "GND";    // rebuild component sign
    }
  }
  else {
    // determines the name by looking for names with the same
    // prefix and increment the number
    for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())
      if(pc->Name.left(len) == c->Name) {
        s = pc->Name.right(pc->Name.length()-len);
        z = s.toInt(&ok);
        if(ok) if(z >= max) max = z + 1;
      }
    c->Name += QString::number(max);  // create name with new number
  }


  setChanged(true);
  setComponentNumber(c); // important for power sources and subcircuit ports
  Comps.append(c);   // insert component at appropriate position
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
    if(pn->cx == w->x1) if(pn->cy == w->y1) break;

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
        if(ptr2->y2 >= w->y2) {
	  delete w;    // new wire lies within an existing wire
	  return 0; }
        else {
	  // one part of the wire lies within an existing wire
	  // the other part not
          if(ptr2->Port2->Connections.count() == 1) {
            w->y1 = ptr2->y1;
            w->Port1 = ptr2->Port1;
	    if(ptr2->Label) {
	      w->Label = ptr2->Label;
	      w->Label->pWire = w;
	    }
            ptr2->Port1->Connections.removeRef(ptr2);  // two -> one wire
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
        else {
	  // one part of the wire lies within an existing wire
	  // the other part not
          if(ptr2->Port2->Connections.count() == 1) {
            w->x1 = ptr2->x1;
            w->Port1 = ptr2->Port1;
	    if(ptr2->Label) {
	      w->Label = ptr2->Label;
	      w->Label->pWire = w;
	    }
            ptr2->Port1->Connections.removeRef(ptr2); // two -> one wire
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

    pw->Port2->Connections.prepend(pw);  // new connections not at end !!!
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
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      w->x1 = pw->x1;
      w->Port1 = pw->Port1;      // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port1->Connections.removeRef(pw);
      w->Port1->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    if(pw->x2 >= w->x2) {  // new wire lies within an existing one ?
      w->Port1->Connections.removeRef(w); // second node not yet made
      delete w;
      return false;
    }
    if(pw->Port2->Connections.count() < 2) {
      // existing wire lies within the new one
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      pw->Port1->Connections.removeRef(pw);
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
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
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
      // existing wire lies within the new one
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      pw->Port1->Connections.removeRef(pw);
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
    if(pn->cx == w->x2) if(pn->cy == w->y2) break;

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
          // one part of the wire lies within an existing wire
          // the other part not
          if(ptr2->Port1->Connections.count() == 1) {
	    if(ptr2->Label) {
	      w->Label = ptr2->Label;
	      w->Label->pWire = w;
	    }
            w->y2 = ptr2->y2;
            w->Port2 = ptr2->Port2;
            ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
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
          // one part of the wire lies within an existing wire
          // the other part not
          if(ptr2->Port1->Connections.count() == 1) {
	    if(ptr2->Label) {
	      w->Label = ptr2->Label;
	      w->Label->pWire = w;
	    }
            w->x2 = ptr2->x2;
            w->Port2 = ptr2->Port2;
            ptr2->Port2->Connections.removeRef(ptr2);  // two -> one wire
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
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      w->x2 = pw->x2;
      w->Port2 = pw->Port2;      // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port2->Connections.removeRef(pw);
      w->Port2->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    // (if new wire lies complete within an existing one, was already check before)

    if(pw->Port1->Connections.count() < 2) {
      // existing wire lies within the new one
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      pw->Port2->Connections.removeRef(pw);
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
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      w->y2 = pw->y2;
      w->Port2 = pw->Port2;     // new wire lengthens an existing one
      Nodes.removeRef(n);
      w->Port2->Connections.removeRef(pw);
      w->Port2->Connections.append(w);
      Wires.removeRef(pw);
      return true;
    }
    // (if new wire lies complete within an existing one, was already check before)

    if(pw->Port1->Connections.count() < 2) {
      // existing wire lies within the new one
      if(pw->Label) {
        w->Label = pw->Label;
	w->Label->pWire = w;
      }
      pw->Port2->Connections.removeRef(pw);
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




  int  n1, n2;
  Wire *pw, *nw;
  Node *pn, *pn2;
  // ................................................................
  // check if the new line covers existing nodes
  for(pw = Wires.current(); pw != 0; pw = Wires.next()) // in order to also check new appearing wires
    for(pn = Nodes.first(); pn != 0; ) {  // check every node
      if(pn->cx == pw->x1) {
        if(pn->cy <= pw->y1) { pn = Nodes.next(); continue; }
        if(pn->cy >= pw->y2) { pn = Nodes.next(); continue; }
      }
      else if(pn->cy == pw->y1) {
        if(pn->cx <= pw->x1) { pn = Nodes.next(); continue; }
        if(pn->cx >= pw->x2) { pn = Nodes.next(); continue; }
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
          pn2->Connections.removeRef(nw);   // remove connection
          pn = pn2;
        }

        if(n2 == 1) {
          pn->Connections.removeRef(nw);   // remove connection
          Nodes.removeRef(pn2);     // delete node 2 if open
          pn2 = pn;
        }

        if(pn == pn2) {
	  if(nw->Label) {
	    pw->Label = nw->Label;
	    pw->Label->pWire = pw;
	  }
          Wires.removeRef(nw);    // delete wire
          Wires.findRef(pw);      // set back to current wire
        }
        break;
      }
      if(n1 == 1) if(n2 == 1) continue;

      // split wire into two wires
      if((pw->x1 != pn->cx) || (pw->y1 != pn->cy)) {
        nw = new Wire(pw->x1, pw->y1, pn->cx, pn->cy, pw->Port1, pn);
        pn->Connections.append(nw);
        Wires.append(nw);
        Wires.findRef(pw);
        pw->Port1->Connections.append(nw);
      }
      pw->Port1->Connections.removeRef(pw);
      pw->x1 = pn2->cx;
      pw->y1 = pn2->cy;
      pw->Port1 = pn2;
      pn2->Connections.append(pw);

      pn = Nodes.next();
    }

  oneLabel(w->Port1); // if two wire lines with different labels are connected, delete one label
  setChanged(true);
  return con;
}

// ---------------------------------------------------
// Inserts a node label.
void QucsDoc::insertNodeLabel(WireLabel *pl)
{
  Node *pn;
  // check if node lies upon existing node
  for(pn = Nodes.first(); pn != 0; pn = Nodes.next())  // check every node
    if(pn->cx == pl->cx) if(pn->cy == pl->cy) break;

  if(!pn) {   // create new node, if no existing one lies at this position
    pn = new Node(pl->cx, pl->cy);
    Nodes.append(pn);
  }

  if(pn->Label) delete pn->Label;
  pn->Label = pl;
  pn->Label->Type = isNodeLabel;
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
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())  // test all components
    if(pc->getSelected(x, y))
      return pc;

  return 0;
}

// ---------------------------------------------------
Diagram* QucsDoc::selectedDiagram(int x, int y)
{
  for(Diagram *ptr1 = Diags.first(); ptr1 != 0; ptr1 = Diags.next())  // test all diagrams
    if(ptr1->getSelected(x, y))
      return ptr1;

  return 0;
}

// ---------------------------------------------------
Node* QucsDoc::selectedNode(int x, int y)
{
  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next())    // test all nodes
    if(pn->getSelected(x, y))
      return pn;

  return 0;
}

// ---------------------------------------------------
Wire* QucsDoc::selectedWire(int x, int y)
{
  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next())    // test all wires
    if(pw->getSelected(x, y))
      return pw;

  return 0;
}

// ---------------------------------------------------
Painting* QucsDoc::selectedPainting(int x, int y)
{
  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next())  // test all paintings
    if(pp->getSelected(x,y))
      return pp;

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
// Checks if pressed on a wire label.
/*Wire* QucsDoc::selectWireLabel(int x, int y)
{
  for(Wire *pw = Wires.last(); pw != 0; pw = Wires.prev())    // test all wires
    if(pw->Label)
      if(pw->Label->getSelected(x, y))
        return pw;
  return 0;
}*/

// ---------------------------------------------------
bool QucsDoc::setMarker(int x, int y)
{
  int n, *p;
  double *px, *py;
  // test all diagrams
  for(Diagram *pd = Diags.last(); pd != 0; pd = Diags.prev())
    if(pd->getSelected(x, y)) {

      // test all graphs of the diagram
      for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next()) {
        n = pg->getSelected(x-pd->cx, pd->cy-y);
        if(n > 0) {
          px = pg->cPointsX + n-1;
          py = pg->cPointsY + 2*(n-1);
          p = pg->Points + 2*(n-1);
          Marker *pm = new Marker(pd, *p, *(p+1),
	                          *px, *py, *(py+1), pd->Graphs.at());
          pd->Markers.append(pm);
          setChanged(true);
          return true;
        }
      }
    }

  return false;
}

// ---------------------------------------------------
// Moves the marker pointer left/right on the graph.
bool QucsDoc::MarkerLeftRight(bool left)
{
  int n;
  double *px;
  bool acted = false;
  for(Diagram *pd = Diags.last(); pd != 0; pd = Diags.prev())
    // test all markers of the diagram
    for(Marker *pm = pd->Markers.first(); pm!=0; pm = pd->Markers.next()) {
      if(pm->isSelected) {
        Graph *pg = pd->Graphs.at(pm->GraphNum);
        px = pg->cPointsX;
        for(n=0; n<pg->count; n++) {
          if(pm->xpos <= *px) break;
          px++;
        }
        if(n == pg->count) px--;

        if(left) {
          if(px <= pg->cPointsX) continue;
          px--;  // one position to the left
	  n--;
        }
        else {
          if(px >= (pg->cPointsX+pg->count)) continue;
          px++;  // one position to the right
	  n++;
        }
        pm->xpos = *px;
        pm->yr = *(pg->cPointsY + 2*n);
	pm->yi = *(pg->cPointsY + 2*n+1);
        pm->createText();
        int *pi = pg->Points + 2*n;
        pm->cx = *pi;
        pm->cy = *(pi+1);
        acted = true;
      }
    }
  return acted;
}

// ---------------------------------------------------
// Selects the element that contains the coordinates x/y.
// Returns the pointer to the element.
// If 'flag' is true, the element can be deselected.
Element* QucsDoc::selectElement(int x, int y, bool flag)
{
  Element *pe_1st=0, *pe_sel=0;
  for(Component *pc = Comps.last(); pc != 0; pc = Comps.prev())    // test all components
    if(pc->getSelected(x, y)) {
      if(flag) { pc->isSelected ^= flag; return pc; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pc->isSelected = true; return pc; }
      if(pe_1st == 0) pe_1st = pc;   // give access to elements that lie beneath
      if(pc->isSelected) pe_sel = pc;
    }

  WireLabel *pl;
  for(Wire *pw = Wires.last(); pw != 0; pw = Wires.prev()) {    // test all wires and wire labels
    if(pw->getSelected(x, y)) {
      if(flag) { pw->isSelected ^= flag; return pw; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pw->isSelected = true; return pw; }
      if(pe_1st == 0) pe_1st = pw;   // give access to elements that lie beneath
      if(pw->isSelected) pe_sel = pw;
    }
    pl = pw->Label;
    if(pl) if(pl->getSelected(x, y)) {
      if(flag) { pl->isSelected ^= flag; return pl; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pl->isSelected = true; return pl; }
      if(pe_1st == 0) pe_1st = pl;   // give access to elements that lie beneath
      if(pl->isSelected) pe_sel = pl;
    }
  }

  for(Node *pn = Nodes.last(); pn != 0; pn = Nodes.prev()) {    // test all node labels
    pl = pn->Label;
    if(pl) if(pl->getSelected(x, y)) {
      if(flag) { pl->isSelected ^= flag; return pl; }
      if(pe_sel != 0) { pe_sel->isSelected = false; pl->isSelected = true; return pl; }
      if(pe_1st == 0) pe_1st = pl;   // give access to elements that lie beneath
      if(pl->isSelected) pe_sel = pl;
    }
  }

  for(Diagram *pd = Diags.last(); pd != 0; pd = Diags.prev()) {   // test all diagrams
    for(Marker *pm = pd->Markers.first(); pm != 0; pm = pd->Markers.next()) // test markers of diagram
      if(pm->getSelected(x-pd->cx, pd->cy-y) > 0) {
        if(flag) { pm->isSelected ^= flag; return pm; }
        if(pe_sel != 0) { pe_sel->isSelected = false; pm->isSelected = true; return pm; }
        if(pe_1st == 0) pe_1st = pm;   // give access to elements that lie beneath
        if(pm->isSelected) pe_sel = pm;
      }

    if(pd->getSelected(x, y)) {

      for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next()) // test graphs of diagram
        if(pg->getSelected(x-pd->cx, pd->cy-y) > 0) {
          if(flag) { pg->isSelected ^= flag; return pg; }
          if(pe_sel != 0) { pe_sel->isSelected = false; pg->isSelected = true; return pg; }
          if(pe_1st == 0) pe_1st = pg;   // give access to elements that lie beneath
          if(pg->isSelected) pe_sel = pg;
        }

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

  for(Wire *pw = Wires.first(); pw != 0; pw = Wires.next()) {     // test all wires
    if(e != pw)  pw->isSelected = false;
    if(pw->Label) if(pw->Label != e)  pw->Label->isSelected = false;
  }

  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next())       // test all node labels
    if(pn->Label) if(pn->Label != e)  pn->Label->isSelected = false;

  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next()) {   // test all diagrams
    if(e != pd)  pd->isSelected = false;

    for(Marker *pm = pd->Markers.first(); pm != 0; pm = pd->Markers.next()) // test markers of diagram
      if(e != pm) pm->isSelected = false;

    for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next()) // test graphs of diagram
      if(e != pg) pg->isSelected = false;
  }

  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next()) // test all paintings
    if(e != pp)  pp->isSelected = false;
}

// ---------------------------------------------------
// Selects elements that lie within the rectangle x1/y1, x2/y2.
int QucsDoc::selectElements(int x1, int y1, int x2, int y2, bool flag)
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


  Wire *pw;
  for(pw = Wires.first(); pw != 0; pw = Wires.next()) {    // test all wires
    if(pw->x1 >= x1) if(pw->x2 <= x2) if(pw->y1 >= y1) if(pw->y2 <= y2) {
      pw->isSelected = true;  z++;
      continue;
    }
    if(pw->isSelected &= flag) z++;
  }


  // test all wire labels *********************************
  WireLabel *pl=0;
  for(pw = Wires.first(); pw != 0; pw = Wires.next()) {
    if(pw->Label) {
      pl = pw->Label;
      if(pl->x1 >= x1) if((pl->x1+pl->x2) <= x2)
        if((pl->y1-pl->y2) >= y1) if(pl->y1 <= y2) {
          pl->isSelected = true;  z++;
          continue;
        }
      if(pl->isSelected &= flag) z++;
    }
  }


  // test all node labels *************************************
  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next()) {
    if(pn->Label) {
      pl = pn->Label;
      if(pl->x1 >= x1) if((pl->x1+pl->x2) <= x2)
        if((pl->y1-pl->y2) >= y1) if(pl->y1 <= y2) {
          pl->isSelected = true;  z++;
          continue;
        }
      if(pl->isSelected &= flag) z++;
    }
  }


  // test all diagrams *******************************************
  for(Diagram *pd = Diags.first(); pd != 0; pd = Diags.next()) {
    // test graphs of diagram
    for(Graph *pg = pd->Graphs.first(); pg != 0; pg = pd->Graphs.next())
      if(pg->isSelected &= flag) z++;

    // test markers of diagram
    for(Marker *pm = pd->Markers.first(); pm != 0; pm = pd->Markers.next()) {
      pm->Bounding(cx1, cy1, cx2, cy2);
      if(pd->cx+cx1 >= x1) if(pd->cx+cx2 <= x2)
        if(pd->cy-cy1 >= y1) if(pd->cy-cy2 <= y2) {
          pm->isSelected = true;  z++;
          continue;
        }
      if(pm->isSelected &= flag) z++;
    }

    // test diagram itself
    pd->Bounding(cx1, cy1, cx2, cy2);
    if(cx1 >= x1) if(cx2 <= x2) if(cy1 >= y1) if(cy2 <= y2) {
      pd->isSelected = true;  z++;
      continue;
    }
    if(pd->isSelected &= flag) z++;
  }

  // test all paintings *******************************************
  for(Painting *pp = Paints.first(); pp != 0; pp = Paints.next()) {
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
      if(pw->Port1->Connections.getFirst() !=  (Element*)1)
        pw->Port1->Connections.prepend((Element*)1);  // to control node afterwards
      if(pw->Port2->Connections.getFirst() !=  (Element*)1)
        pw->Port2->Connections.prepend((Element*)1);  // to control node afterwards
      Wires.take(Wires.findRef(pw));
      int mask = 1;
      if(pw->isHorizontal()) mask = 2;

      if(pw->Port1 != pn) {
        pw->Port1 = (Node*)mask;
        pw->Port2 = (Node*)3;    // move port 2 completely
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
      if(pw2->Port1->Connections.getFirst() !=  (Element*)1)
        pw2->Port1->Connections.prepend((Element*)1);  // to control node afterwards
      if(pw2->Port2->Connections.getFirst() !=  (Element*)1)
        pw2->Port2->Connections.prepend((Element*)1);  // to control node afterwards
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

  p->append(new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)0, (Node*)1));  // only x2 moving
  p->append(new Wire(pn->cx, pn->cy, pn->cx, pn->cy, (Node*)1, (Node*)3));  // x1, x2, y2 moving
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

  // test all wires and wire labels ***********************
  for(pw = Wires.first(); pw != 0; ) {
    if(pw->Label) if(pw->Label->isSelected)
      p->append(pw->Label);

    if(pw->isSelected) {
      p->append(pw);

      pw->Port1->Connections.removeRef(pw);   // remove connection 1
      pw->Port2->Connections.removeRef(pw);   // remove connection 2
      Wires.take();

      pw = Wires.current();
    }
    else pw = Wires.next();
  }

  // test all diagrams **********************************
  for(pd = Diags.first(); pd != 0; )
    if(pd->isSelected) {
      p->append(pd);
      Diags.take();
      pd = Diags.current();
    }
    else {
      for(Marker *pm = pd->Markers.first(); pm != 0; )
        if(pm->isSelected) {
          p->append(pm);
          pd->Markers.take();
          pm = pd->Markers.current();
        }
        else pm = pd->Markers.next();

      pd = Diags.next();
    }

  for(Painting *pp = Paints.first(); pp != 0; )   // test all paintings
    if(pp->isSelected) {
      p->append(pp);
      Paints.take();
      pp = Paints.current();
    }
    else pp = Paints.next();

  // ..............................................
  // Inserts wires, if a connection to a not moving element is found.
  // Go backwards in order not to test the new insertions.
  for(pe = p->last(); pe!=0; pe = p->prev())
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
      if(pn->Label) {
        p->append(pn->Label);    // do not forget the node labels
        pn->Label->Type = isMovingLabel;
      }
      Nodes.remove();
      pn = Nodes.current();
      continue;
    }

    pn = Nodes.next();
  }

  // test all node labels
  // do this last to avoid double copying
  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next())
    if(pn->Label) if(pn->Label->isSelected)
      p->append(pn->Label);

}

// ---------------------------------------------------
// Activates/deactivates components that lie within the
// rectangle x1/y1, x2/y2.
void QucsDoc::activateComps(int x1, int y1, int x2, int y2)
{
  int  cx1, cy1, cx2, cy2;
  if(x1 > x2) { cx1 = x1; x1  = x2; x2  = cx1; }
  if(y1 > y2) { cy1 = y1; y1  = y2; y2  = cy1; }

  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {
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
  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next()) {
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

  for(Component *pc = Comps.first(); pc != 0; pc = Comps.next())
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
    pn->Name = "";   // erase all node names

  Cons.append(n1);
  n1->Name = "x";  // mark Node as already checked
  for(pn = Cons.first(); pn!=0; pn = Cons.next()) {
    if(pn->Label) {
      if(named) {
        delete pn->Label;
        pn->Label = 0;    // erase double names
      }
      else named = true;
    }

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

      if(pw->Label) {
        if(named) {
          delete pw->Label;
          pw->Label = 0;    // erase double names
        }
        else named = true;
      }
    }
  }
}

// ---------------------------------------------------
// Test, if wire line is already labeled and returns a pointer to the
// labeled element.
Element* QucsDoc::getWireLabel(Node *pn_)
{
  Wire *pw;
  Node *pn;
  QPtrList<Node> Cons;

  for(pn = Nodes.first(); pn!=0; pn = Nodes.next())
    pn->Name = "";   // erase all node names

  Cons.append(pn_);
  pn_->Name = "x";  // mark Node as already checked
  for(pn = Cons.first(); pn!=0; pn = Cons.next())
    if(pn->Label) return pn;
    else
      for(Element *pe = pn->Connections.first(); pe!=0; pe = pn->Connections.next()) {
        if(pe->Type != isWire) {
          if(((Component*)pe)->Sign == "GND") return pe;
          continue;
        }

        pw = (Wire*)pe;
        if(pw->Label) return pw;

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
  // find boundings of all components
  for(pc = Comps.first(); pc != 0; pc = Comps.next()) {
    pc->entireBounds(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  // find boundings of all wires
  for(pw = Wires.first(); pw != 0; pw = Wires.next()) {
    if(pw->x1 < xmin) xmin = pw->x1;
    if(pw->x2 > xmax) xmax = pw->x2;
    if(pw->y1 < ymin) ymin = pw->y1;
    if(pw->y2 > ymax) ymax = pw->y2;

    if(pw->Label != 0) {     // check position of wire label
      if(pw->Label->x1 < xmin) xmin = pw->x1;
      if((pw->Label->x1+pw->Label->x2) > xmax)
        xmax = pw->Label->x1+pw->Label->x2;
      if(pw->Label->y1 > ymax)
        ymax = pw->Label->y1;
      if((pw->Label->y1+pw->Label->y2) < ymin)
        ymin = pw->Label->y1+pw->Label->y2;
    }
  }

  // find boundings of all diagrams
  for(pd = Diags.first(); pd != 0; pd = Diags.next()) {
    pd->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }

  // find boundings of all Paintings
  for(pp = Paints.first(); pp != 0; pp = Paints.next()) {
    pp->Bounding(x1, y1, x2, y2);
    if(x1 < xmin) xmin = x1;
    if(x2 > xmax) xmax = x2;
    if(y1 < ymin) ymin = y1;
    if(y2 > ymax) ymax = y2;
  }
}

// ---------------------------------------------------
// Sets the component ports anew. Used after rotate, mirror etc.
void QucsDoc::setCompPorts(Component *pc)
{
  WireLabel *pl=0;
  for(Port *pp = pc->Ports.first(); pp!=0; pp = pc->Ports.next()) {
    pp->Connection->Connections.removeRef((Element*)pc);   // delete all port connections
    switch(pp->Connection->Connections.count()) {
      case 0: pl = pp->Connection->Label;
              Nodes.removeRef(pp->Connection);
              break;
      case 2: oneTwoWires(pp->Connection);   // if possible, connect two wires to one
              pl = 0;
      default: ;
    }
    // connect component node to schematic node
    pp->Connection = insertNode(pp->x+pc->cx, pp->y+pc->cy, pc);
    if(pl) {
      if(!getWireLabel(pp->Connection)) {
        pl->cx = pp->Connection->cx;
        pl->cy = pp->Connection->cy;
        pp->Connection->Label = pl;   // restore node label if unlabeled
      }
      else delete pl;
    }
  }
}

// ---------------------------------------------------
bool QucsDoc::copyCompsWires(int& x1, int& y1, int& x2, int& y2)
{
  x1=INT_MAX;
  y1=INT_MAX;
  x2=INT_MIN;
  y2=INT_MIN;
  Wire *pw;
  Painting *pp;
  Component *pc;
  WireLabel *pl;

  // find bounds of all selected components
  for(pc = Comps.first(); pc != 0; ) {
    if(pc->isSelected) if(pc->Ports.count() > 0) {
      if(pc->cx < x1) x1 = pc->cx;  // do not copy components without ports
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
      pl = pw->Label;
      pw->Label = 0;
      deleteWire(pw);
      pw->Label = pl;    // restore wire label
      pw = Wires.current();
    }
    else pw = Wires.next();

  int bx1, by1, bx2, by2;
  // find boundings of all selected paintings
  for(pp = Paints.first(); pp != 0; )
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
      case isComponent:
           pc = (Component*)pe;
           pc->rotate();   //rotate component !before! rotating its center
           x2 = x1 - pc->cx;
           pc->setCenter(pc->cy - y1 + x1, x2 + y1);
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
           if(pw->Label) {
             x2 = pw->Label->cx;
             pw->Label->cx = pw->Label->cy - y1 + x1;
             pw->Label->cy = x1 - x2 + y1;
             if(pw->Label->Type == isHWireLabel)
	       pw->Label->Type = isVWireLabel;
             else pw->Label->Type = isHWireLabel;
           }
           insertWire(pw);
           break;

      case isPainting:
           pp = (Painting*)pe;
           pp->rotate();   // rotate painting !before! rotating its center
           pp->getCenter(x2, y2);
           tmp = x1 - x2;
           pp->setCenter(y2 - y1 + x1, tmp + y1);
           Paints.append(pp);
           break;
      default: ;
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
                        if(pw->Label)
                          pw->Label->cy = (y1<<1) - pw->Label->cy;
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
                        if(pw->Label)
                          pw->Label->cx = (x1<<1) - pw->Label->cx;
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
      if(e2->Label) {   // take over the node name label ?
        e1->Label = e2->Label;
	e1->Label->pWire = e1;
      }
      else if(n->Label) {
             e1->Label = n->Label;
	     e1->Label->pWire = e1;
	     e1->Label->pNode = 0;
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
      case 1  : if(pn->Connection->Label) delete pn->Connection->Label;
                Nodes.removeRef(pn->Connection);     // delete all open nodes (auto-delete)
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
  if(w->Port1->Connections.count() == 1) {
    if(w->Port1->Label) delete w->Port1->Label;
    Nodes.removeRef(w->Port1);     // delete node 1 if open
  }
  else {
    w->Port1->Connections.removeRef(w);   // remove connection
    if(w->Port1->Connections.count() == 2) oneTwoWires(w->Port1);  // two wires -> one wire
  }

  if(w->Port2->Connections.count() == 1) {
    if(w->Port1->Label) delete w->Port1->Label;
    Nodes.removeRef(w->Port2);     // delete node 2 if open
  }
  else {
    w->Port2->Connections.removeRef(w);   // remove connection
    if(w->Port2->Connections.count() == 2) oneTwoWires(w->Port2);  // two wires -> one wire
  }

  if(w->Label) delete w->Label;
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
  while(pw != 0) {      // all selected wires and their labels
    if(pw->Label)
      if(pw->Label->isSelected) {
        delete pw->Label;
        pw->Label = 0;
        sel = true;
      }

    if(pw->isSelected) {
      deleteWire(pw);
      pw = Wires.current();
      sel = true;
    }
    else pw = Wires.next();
  }

  // all selected labels on nodes ***************************
  for(Node *pn = Nodes.first(); pn != 0; pn = Nodes.next())
    if(pn->Label)
      if(pn->Label->isSelected) {
        delete pn->Label;
        pn->Label = 0;
        sel = true;
      }

  Diagram *pd = Diags.first();
  while(pd != 0)      // test all diagrams
    if(pd->isSelected) {
      Diags.remove();
      pd = Diags.current();
      sel = true;
    }
    else {
      for(Marker *pm = pd->Markers.first(); pm != 0; )   // all markers of diagram
        if(pm->isSelected) {
          pd->Markers.remove();
          pm = pd->Markers.current();
          sel = true;
        }
        else  pm = pd->Markers.next();

      for(Graph *pg = pd->Graphs.first(); pg != 0; )   // all graphs of diagram
        if(pg->isSelected) {
          pd->Graphs.remove();
          pg = pd->Graphs.current();
          sel = true;
        }
        else  pg = pd->Graphs.next();

      pd = Diags.next();
    }

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
// Performs copy or cut functions for clipboard.
QString QucsDoc::copySelected(bool cut)
{
  QString s = File.createClipboardFile();
  if(cut) deleteElements();   // delete selected elements if wanted
  return s;
}

// ---------------------------------------------------
// Performs paste function from clipboard
bool QucsDoc::paste(QTextStream *stream, QPtrList<Element> *pe)
{
  return File.pasteFromClipboard(stream, pe);
}

// ---------------------------------------------------
// Loads this Qucs document.
bool QucsDoc::load()
{
  if(!File.load()) return false;
  setChanged(false);
  sizeOfAll(UsedX1, UsedY1, UsedX2, UsedY2);
  return true;
}

// ---------------------------------------------------
// Saves this Qucs document.
int QucsDoc::save()
{
  setChanged(false);
  return File.save();
}

// ---------------------------------------------------
// Follows the wire lines in order to determine the node names for
// each component.
bool QucsDoc::giveNodeNames(QTextStream *stream)
{
  Node *p1, *p2;
  Wire *pw;
  Element *pe;

  // delete the node names
  for(p1 = Nodes.first(); p1 != 0; p1 = Nodes.next())
    if(p1->Label) p1->Name = p1->Label->Name;
    else p1->Name = "";

  // set the wire names to the connected node
  for(pw = Wires.first(); pw != 0; pw = Wires.next())
    if(pw->Label != 0) pw->Port1->Name = pw->Label->Name;

  // give the ground nodes the name "gnd", and insert subcircuits
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
