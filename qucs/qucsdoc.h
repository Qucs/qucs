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

#include "component.h"
#include "wire.h"
#include "diagram.h"

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

  Node*      insertWireNode1(Wire *e);
  Node*      insertWireNode2(Wire *e);
  int        insertWire(Wire *w);
  Component* selectedComponent(int x, int y);
  Diagram*   selectedDiagram(int x, int y);
  Wire*      selectedWire(int x, int y);
  Element*   selectElement(int x, int y, bool flag);
  void       deselectElements(Element *e);
  void  NewMovingWires(QPtrList<Element> *p, Node *pn);
  void  copySelectedElements(QPtrList<Element> *p);
  int   selectComponents(int x1, int y1, int x2, int y2, bool flag);
  bool  activateComponent(int x, int y);
  bool  activateComponents();
  void  activateComps(int x1, int y1, int x2, int y2);
  Node* insertNode(int x, int y, Element *e);
  void  insertRawComponent(Component *c);
  void  insertComponent(Component *c);
  bool  rotateComponents();
  bool  mirrorXComponents();
  void  deleteComp(Component *c);
  void  deleteWire(Wire *w);
  bool  deleteElements();
  bool  createNetlist(QFile *NetlistFile);

  QString copySelected(bool cut);
  bool    paste(QTextStream *stream, QPtrList<Element> *pe);

  void  reloadGraphs();
  bool  save();
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

  bool SimOpenDpl;  // open data display after simulation ?
  QString DataSet;  // name of the default dataset
  QString DataDisplay; // name of the default data display
  int  GridX, GridY;
  bool GridOn;

  QPtrList<Element> *Cache;
};


#endif
