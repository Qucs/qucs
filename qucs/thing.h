/***************************************************************************
                          thing.h  -  description
                             -------------------
    begin                : Sun Sep 28 2003
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

#ifndef THING_H
#define THING_H

#include <qpainter.h>

/**
  *@author Michael Margraf
  */

class Thing {
public: 
	Thing();
	virtual ~Thing();

  virtual void paint(QPainter *p);
  virtual bool containPoint(int x, int y);

  bool selected;
};

#endif
