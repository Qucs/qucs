/***************************************************************************
                          graph.h  -  description
                             -------------------
    begin                : Thu Oct 2 2003
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

#ifndef GRAPH_H
#define GRAPH_H


#include "../element.h"

#include <qcolor.h>
#include <qpainter.h>
#include <qptrlist.h>


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

  void    paint(QPainter *p, int x0, int y0);
  QString save();
  bool    load(const QString& Line);
  int     getSelected(int x, int y);

  QPtrList<DataX>  cPointsX;
  double *cPointsY;
  int    *Points, countX1, countX2;     // data in screen coordinates
  QString Var;
  QColor  Color;
  int     Thick;
  
};

#endif
