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

#include "wire.h"
#include "diagrams/diagram.h"
#include "paintings/painting.h"
#include "components/component.h"

#include "qucsfile.h"

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

  int   insertWireNode1(Wire *w);
  bool  connectHWires1(Wire *w);
  bool  connectVWires1(Wire *w);
  int   insertWireNode2(Wire *w);
  bool  connectHWires2(Wire *w);
  bool  connectVWires2(Wire *w);
  int   insertWire(Wire *w);

  Node* insertNode(int x, int y, Element *e);
  void  insertRawComponent(Component *c, bool num=false);
  void  insertComponent(Component *c);
  void  insertNodeLabel(WireLabel *pl);

  void       sizeOfAll(int& xmin, int& ymin, int& xmax, int& ymax);
  Component* selectedComponent(int x, int y);
  Diagram*   selectedDiagram(int x, int y);
  Node*      selectedNode(int x, int y);
  Wire*      selectedWire(int x, int y);
  Painting*  selectedPainting(int x, int y);
  void       selectWireLine(Element *pe, Node *pn, bool ctrl);
  Wire*      selectWireLabel(int x, int y);
  Element*   selectElement(int x, int y, bool flag);
  int        selectElements(int x1, int y1, int x2, int y2, bool flag);
  void       deselectElements(Element *e);
  bool  activateComponent(int x, int y);
  bool  activateComponents();
  void  activateComps(int x1, int y1, int x2, int y2);
  Component* searchSelSubcircuit();
  void  NewMovingWires(QPtrList<Element> *p, Node *pn);
  void  copySelectedElements(QPtrList<Element> *p);

  void  setComponentNumber(Component *c);
  void  oneLabel(Node *n1);
  Element* getWireLabel(Node *pn);
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
  bool    load();
  int     save();

  void    reloadGraphs();
  void    setOnGrid(int& x, int& y);


  QucsFile  File;   // class to perform  load, save, copy, paste

  QString DocName;
  bool    DocChanged;

  QTab    *Tab;
  QTabBar *Bar;

  QPtrList<Wire>      Wires;
  QPtrList<Node>      Nodes;
  QPtrList<Diagram>   Diags;
  QPtrList<Painting>  Paints;
  QPtrList<Component> Comps;

  bool    SimOpenDpl;  // open data display after simulation ?
  QString DataSet;     // name of the default dataset
  QString DataDisplay; // name of the default data display
  int     GridX, GridY;
  bool    GridOn;
  double  Scale;
  int PosX, PosY;    // upper left corner of the visible area (only for remembering during seeing another open document)
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // size of the document area used by elements

  QPtrList<Element> ElementCache;
};


#endif
