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


/**
  *@author Michael Margraf
  */

class Element {
public: 
	Element();
	~Element();

  bool isSelected;
  bool isWire;    // is it wire or component ?
  int  x1, y1, x2, y2;  // boundings
};

#endif
