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
#include "../element.h"

#include <qfile.h>


/**
  *@author Michael Margraf
  */


class Diagram : public Element {
public: 
  Diagram(int _cx=0, int _cy=0);
  virtual ~Diagram();

  virtual Diagram* newOne();
  virtual void calcDiagram();
  virtual void calcCoordinate(double, double, double, int*, int*) {};
  void    calcData(Graph*);
  void    setCenter(int, int, bool relative=false);
  void    paintScheme(QPainter*);
  void    paint(QPainter*);
  void    Bounding(int&, int&, int&, int&);
  bool    getSelected(int, int);
  bool    ResizeTouched(int, int);
  QString save();
  bool    load(const QString&, QTextStream*);

  void updateGraphData();
  void loadGraphData(const QString&);
  bool loadVarData(const QString&);
  int  loadIndepVarData(const QString&, const QString&);

  QString Name; // identity of diagram type (e.g. Polar), used for saving etc.
  bool    GridOn;
  QPen    GridPen;
  QString xLabel, yLabel;
  bool    xlog, ylog;   // in "rectdiagram": x-/y-axis logarithmic or linear

  QPtrList<Graph>  Graphs;
  QPtrList<Marker> Markers;
  QPtrList<Arc>    Arcs;
  QPtrList<Line>   Lines;
  QPtrList<Text>   Texts;

  double xmin, ymin, xmax, ymax; // least and greatest values of all graph data
  double xup, xlow, yup, ylow;   // the limits of the diagram
  int State;
};

#endif
