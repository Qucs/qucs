/***************************************************************************
                          element.h  -  description
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

#ifndef ELEMENT_H
#define ELEMENT_H

#include <qpainter.h>


/**
  *@author Michael Margraf
  */

enum ElementType {isDummy, isComponent, isWire, isDiagram, isPainting};
  
class Element {
public: 
	Element();
	virtual ~Element();

  virtual void paintScheme(QPainter *p);
  virtual void setCenter(int x, int y, bool relative=false);

  bool isSelected;

  ElementType  Type;    // whether it is Component, Wire, ...
  int  cx, cy, x1, y1, x2, y2;  // center and relative boundings
};

#endif
