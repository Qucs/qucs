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


#include <qcolor.h>
#include <qpainter.h>
//#include <qpoint.h>
#include <qptrlist.h>


/**
  *@author Michael Margraf
  */

struct cPoint {
  cPoint(double _x, double _yr, double _yi)
       : x(_x), yr(_yr), yi(_yi) {};
  double  x, yr, yi;
};


class Graph {
public: 
	Graph(const QString& _Line="");
	~Graph();

  void    paint(QPainter *p, int cx, int cy);
  QString save();
  bool    load(const QString& Line);

  int  count, *Points;
  QPtrList<cPoint> cPoints;
  QString Line;
//  QRgb    Color;
  QColor  Color;
  int     Thick;
  
};

#endif
