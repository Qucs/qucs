/***************************************************************************
                                 graph.h
                                ---------
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

#include <QColor>
#include <Q3PtrList>
#include <QDateTime>


// meaning of the values in a graph "Points" list
#define STROKEEND   -2
#define BRANCHEND   -10
#define GRAPHEND    -100

#define GRAPHSTYLE_SOLID    0
#define GRAPHSTYLE_DASH     1
#define GRAPHSTYLE_DOT      2
#define GRAPHSTYLE_LONGDASH 3
#define GRAPHSTYLE_STAR     4
#define GRAPHSTYLE_CIRCLE   5
#define GRAPHSTYLE_ARROW    6

class Diagram;
class ViewPainter;


struct DataX {
  DataX(const QString& Var_, double *Points_=0, int count_=0)
       : Var(Var_), Points(Points_), count(count_), Min(INFINITY), Max(-INFINITY) {};
 ~DataX() { if(Points) delete[] Points; };
  QString Var;
  double *Points;
  int     count;

public:
  const double& min()const {return Min;}
  const double& max()const {return Max;}
public: // only called from Graph. cleanup later.
  const double& min(const double& x){if (Min<x) Min=x; return Min;}
  const double& max(const double& x){if (Max>x) Max=x; return Max;}
private:
  double Min;
  double Max;
};

class Axis;

class Graph : public Element {
public:
  Graph(const QString& _Line="");
 ~Graph();

  int loadDatFile(const QString& filename);
  int loadIndepVarData(const QString&, char* datfilecontent);

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  int     getSelected(int, int);
  Graph*  sameNewOne();
  unsigned numAxes() const { return cPointsX.count(); }
  DataX* axis(uint i) { return cPointsX.at(i); }
  bool isEmpty() const { return cPointsX.isEmpty(); }
  Q3PtrList<DataX>& mutable_axes(){return cPointsX;} // HACK

  QDateTime lastLoaded;  // when it was loaded into memory
  int     yAxisNo;       // which y axis is used
  double *cPointsY;
  float  *ScrPoints; // data in screen coordinates
  int     countY;    // number of curves
  QString Var;
  QColor  Color;
  int     Thick;
  int     Style;
  QList<Marker *> Markers;

  // for tabular diagram
  int  Precision;   // number of digits to show
  int  numMode;     // real/imag or polar (deg/rad)
private:
  Q3PtrList<DataX>  cPointsX;
};

#endif
