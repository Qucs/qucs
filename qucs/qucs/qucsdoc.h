/***************************************************************************
                          qucsdoc.h  -  description
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

#ifndef QUCSDOC_H
#define QUCSDOC_H

#include "components/components.h"
#include "wire.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"

#include <qstring.h>
#include <qpainter.h>
#include <qptrlist.h>
#include <qfile.h>
#include <qtabbar.h>


/**
  *@author Michael Margraf
  */

class QucsDoc {
public: 
	QucsDoc(QTabBar *b, const QString& _Name);
	~QucsDoc();

  void setName(const QString& _Name);
  void setChanged(bool c);

  void paint(QPainter *p);
  void paintGrid(QPainter *p, int StartX, int StartY, int Width, int Height);

  int      insertWireNode1(Wire *w);
  bool     connectHWires1(Wire *w);
  bool     connectVWires1(Wire *w);
  int      insertWireNode2(Wire *w);
  bool     connectHWires2(Wire *w);
  bool     connectVWires2(Wire *w);
  int      insertWire(Wire *w);

  void       sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax);
  Component* selectedComponent(int x, int y);
  Diagram*   selectedDiagram(int x, int y);
  Wire*      selectedWire(int x, int y);
  Painting*  selectedPainting(int x, int y);
  void       selectWireLine(Element *pe, Node *pn, bool ctrl);
  Element*   selectElement(int x, int y, bool flag);
  int        selectComponents(int x1, int y1, int x2, int y2, bool flag);
  void       deselectElements(Element *e);
  bool  activateComponent(int x, int y);
  bool  activateComponents();
  void  activateComps(int x1, int y1, int x2, int y2);
  Component* searchSelSubcircuit();
  void  NewMovingWires(QPtrList<Element> *p, Node *pn);
  void  copySelectedElements(QPtrList<Element> *p);
  Node* insertNode(int x, int y, Element *e);
  void  insertRawComponent(Component *c, int pos=-1);
  void  insertComponent(Component *c);
  int   getComponentPos(Component *c);
  void  oneLabel(Node *n1);
  Wire* getWireLabel(Wire *w);
  void  setCompPorts(Component *pc);
  bool  copyCompsWires(int& x1, int& y1, int& x2, int& y2);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  bool  oneTwoWires(Node *n);
  void  deleteComp(Component *c);
  void  deleteWire(Wire *w);
  bool  deleteElements();

  bool  giveNodeNames(QTextStream *stream);
  bool  createSubNetlist(QTextStream *stream);
  bool  createNetlist(QFile *NetlistFile);

  QString copySelected(bool cut);
  bool    paste(QTextStream *stream, QPtrList<Element> *pe);

  void  reloadGraphs();
  int   save();
  bool  load();
  bool  loadProperties(QTextStream *stream);
  bool  loadComponents(QTextStream *stream, bool insert=true);
  bool  loadWires(QTextStream *stream, bool insert=true);
  bool  loadDiagrams(QTextStream *stream, bool insert=true);
  bool  loadPaintings(QTextStream *stream, bool insert=true);

  void  setOnGrid(int& x, int& y);

  QString DocName;
  bool    DocChanged;

  QTab    *Tab;
  QTabBar *Bar;

  QPtrList<Wire>      Wires;
  QPtrList<Node>      Nodes;
  QPtrList<Component> Comps;
  QPtrList<Diagram>   Diags;
  QPtrList<Painting>  Paints;

  bool SimOpenDpl;  // open data display after simulation ?
  QString DataSet;  // name of the default dataset
  QString DataDisplay; // name of the default data display
  int  GridX, GridY;
  bool GridOn;
  double Scale;
  int PosX, PosY;    // upper left corner of the visible area
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area

  QPtrList<Element> *Cache;
  QPtrList<Element> ElementCache;
};


#endif
