/***************************************************************************
                          marker.h  -  description
                             -------------------
    begin                : Sat Apr 10 2004
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

#ifndef MARKER_H
#define MARKER_H


#include "../element.h"

#include <qpainter.h>
#include <qptrlist.h>


class Diagram;

/**
  *@author Michael Margraf
  */

class Marker : public Element {
public:
  Marker(Diagram *Diag_, int _cx=0, int _cy=0, double _xpos=0.0,
         double _yrpos=0.0, double _yipos=0.0, int _gNum=0);
  ~Marker();

  void    createText();
  void    paint(QPainter *p, int x0, int y0);
  void    paintScheme(QPainter *p);
  void    setCenter(int x, int y, bool relative);
  void    Bounding(int& _x1, int& _y1, int& _x2, int& _y2);
  QString save();
  bool    load(const QString& Line);
  int     getSelected(int x, int y);

  Diagram *Diag;    // the corresponding diagram
  int    GraphNum;  // number of diagram graph the marker is pointing to
  double xpos;      // x axis value the marker is pointing to
  double yr, yi;

  QString Text;     // the string to be displayed in the marker text
  int lookNfeel;    // different marker designs possible

  int  Precision;   // number of digits to show
};

#endif
