/***************************************************************************
                          qucsdoc.h  -  description
                             -------------------
    begin                : Wed Sep 3 2003
    copyright            : (C) 2003 by Michael Margraf
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

class QucsApp;

/**
  *@author Michael Margraf
  */

class QucsDoc {
public: 
  QucsDoc(QucsApp*, const QString&);
  ~QucsDoc();

  void setName(const QString&);
  void setChanged(bool, bool fillStack=false);

  void paint(QPainter*);
  void paintGrid(QPainter*, int, int, int, int);

  int   insertWireNode1(Wire*);
  bool  connectHWires1(Wire*);
  bool  connectVWires1(Wire*);
  int   insertWireNode2(Wire*);
  bool  connectHWires2(Wire*);
  bool  connectVWires2(Wire*);
  int   insertWire(Wire*);

  Node* insertNode(int, int, Element*);
  void  insertRawComponent(Component*, bool num=false);
  void  insertComponent(Component*);
  void  insertNodeLabel(WireLabel*);

  Component* searchSelSubcircuit();
  void       sizeOfAll(int&, int&, int&, int&);
  Component* selectedComponent(int, int);
  Diagram*   selectedDiagram(int, int);
  Node*      selectedNode(int, int);
  Wire*      selectedWire(int, int);
  Painting*  selectedPainting(int, int);
  void       selectWireLine(Element*, Node*, bool);
//  Wire*      selectWireLabel(int x, int y);
  Element*   selectElement(int, int, bool);
  int        selectElements(int, int, int, int, bool);
  void       deselectElements(Element*);
  bool  activateComponent(int, int);
  bool  activateComponents();
  void  activateComps(int, int, int, int);
  void  NewMovingWires(QPtrList<Element>*, Node*);
  void  copySelectedElements(QPtrList<Element>*);

  void  setComponentNumber(Component*);
  void  oneLabel(Node*);
  Element* getWireLabel(Node*);
  void  setCompPorts(Component*);
  bool  copyCompsWires(int&, int&, int&, int&);
  bool  rotateElements();
  bool  mirrorXComponents();
  bool  mirrorYComponents();
  bool  oneTwoWires(Node*);
  void  deleteComp(Component*);
  void  deleteWire(Wire*);
  bool  deleteElements();
  Marker*  setMarker(int, int);
  bool  MarkerLeftRight(bool);
  bool  MarkerUpDown(bool);

  bool  giveNodeNames(QTextStream*);
  bool  createSubNetlist(QTextStream*);
  bool  createNetlist(QFile*);

  QString copySelected(bool);
  bool    paste(QTextStream*, QPtrList<Element>*);
  bool    load();
  int     save();
  bool    undo();
  bool    redo();

  void    reloadGraphs();
  void    setOnGrid(int&, int&);


  QucsFile  File;   // class to perform  load, save, copy, paste

  QString DocName;
  bool    DocChanged;

  QucsApp *App;
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
  int PosX, PosY;    // upper left corner of visible area (only for remembering during seeing another open document)
  int ViewX1, ViewY1, ViewX2, ViewY2;  // size of the document area
  int UsedX1, UsedY1, UsedX2, UsedY2;  // document area used by elements

  QPtrList<Element> ElementCache;
  QPtrList<QString> UndoStack;
};


#endif
