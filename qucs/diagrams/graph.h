/***************************************************************************
                          graph.h  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#ifndef GRAPH_H
#define GRAPH_H


#include "marker.h"
#include "element.h"

#include <qcolor.h>
#include <qptrlist.h>

class Diagram;
class ViewPainter;

/**
  *@author Michael Margraf
  */

struct DataX {
  DataX(const QString& Var_, double *Points_=0, int count_=0)
       : Var(Var_), Points(Points_), count(count_) {};
  ~DataX() { if(Points) delete[] Points; };
  QString Var;
  double *Points;
  int     count;
};


class Graph : public Element {
public:
  Graph(const QString& _Line="");
  ~Graph();

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  int     getSelected(int, int);
  Graph*  sameNewOne();

  int     yAxisNo;   // which y axis is used
  QPtrList<DataX>  cPointsX;
  double *cPointsY;
  int    *Points, countY; // data in screen coordinates, countY = curves number
  QString Var;
  QColor  Color;
  int     Thick;
  int     Style;
  QPtrList<Marker> Markers;

  // for tabular diagram
  int  Precision;   // number of digits to show
  int  numMode;     // real/imag or polar (deg/rad)
};

#endif
