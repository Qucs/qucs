/***************************************************************************
                          node.h  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#ifndef NODE_H
#define NODE_H

#include "element.h"
#include "wirelabel.h"

#include <qpainter.h>
#include <qptrlist.h>

/**
  *@author Michael Margraf
  */

class Node : public Element {
public:
  Node(int _x, int _y);
  ~Node();

  void  paint(QPainter *p);
  bool  getSelected(int x_, int y_);
  void  setName(const QString& Name_, int x_=0, int y_=0);

  QPtrList<Element> Connections;

  WireLabel *Label;     // label for node name
  QString Name;   // node name used by creation of netlist
  int State;	// remember some things during some operations
};

#endif
