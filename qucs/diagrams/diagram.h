/***************************************************************************
                          diagram.h  -  description
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

#ifndef DIAGRAM_H
#define DIAGRAM_H

#include "graph.h"
#include "marker.h"
#include "element.h"
#include "viewpainter.h"

#include <qfile.h>


/**
  *@author Michael Margraf
  */

struct Axis {
  double  min, max; // least and greatest values of all graph data
  double  low, up;  // the limits of the diagram
  bool    log;      // in "rectdiagram": logarithmic or linear
  QString Label;
  int     numGraphs;  // counts number of graphs using this axis
  bool    GridOn;
};


class Diagram : public Element {
public:
  Diagram(int _cx=0, int _cy=0);
  virtual ~Diagram();

  virtual Diagram* newOne();
  virtual int  calcDiagram();
  virtual void calcCoordinate(double, double, double, int*, int*, Axis*) {};
  void    calcData(Graph*, int);
  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    paintScheme(QPainter*);
  void    paint(ViewPainter*);
  void    Bounding(int&, int&, int&, int&);
  bool    getSelected(int, int);
  bool    ResizeTouched(int, int);
  QString save();
  bool    load(const QString&, QTextStream*);

  void updateGraphData();
  void loadGraphData(const QString&);
  void recalcGraphData();
  bool loadVarData(const QString&);
  int  loadIndepVarData(const QString&, const QString&);

  void createSmithChart(Axis*, int Mode=7);
  void createPolarDiagram(Axis*, int Mode=3);

  QString Name; // identity of diagram type (e.g. Polar), used for saving etc.
  QPen    GridPen;

  QPtrList<Graph>  Graphs;
  QPtrList<Arc>    Arcs;
  QPtrList<Line>   Lines;
  QPtrList<Text>   Texts;

  int x3, y3;
  Axis  xAxis, ylAxis, yrAxis;   // axes (x, y left, y right)
  int State;  // to remember which resize area was touched

  int limit_x1, limit_x2, step_x;   // if not auto-scale
  int limit_y1, limit_y2, step_y;
  int limit_z1, limit_z2, step_z;
};

#endif
