/***************************************************************************
                          node.h  -  description
                             -------------------
    begin                : Sat Sep 20 2003
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

#ifndef NODE_H
#define NODE_H

#include "element.h"

#include <qpainter.h>
#include <qptrlist.h>

/**
  *@author Michael Margraf
  */

class Node {
public: 
	Node(int _x, int _y);
	~Node();

  void    paint(QPainter *p);
  
  int x, y;
  QPtrList<Element> Connections;

  bool isNamed;   // name given by the user ?
  QString Name;
};

#endif
