/***************************************************************************
                               diagram.h
                              -----------
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

#include <QFile>
#include <QTextStream>
#include <QList>

#define MIN_SCROLLBAR_SIZE 8

#define INVALID_STR QObject::tr(" <invalid>")

// ------------------------------------------------------------
// Enlarge memory block if neccessary.
#define  FIT_MEMORY_SIZE  \
  if(p >= p_end) {     \
    int pos = p - g->begin(); \
    assert(pos<Size); \
    Size += 256;        \
    g->resizeScrPoints(Size); \
    p = g->begin() + pos; \
    p_end = g->begin() + (Size - 9); \
  }

struct Axis {
  double  min, max; // least and greatest values of all graph data
  double  low, up;  // the limits of the diagram
  bool    log;      // in "rectdiagram": logarithmic or linear
  QString Label;
  int     numGraphs;  // counts number of graphs using this axis
  bool    GridOn;

  bool   autoScale;    // manual limits or auto-scale ?
  double limit_min, limit_max, step;   // if not auto-scale
};


class Diagram : public Element {
public:
  Diagram(int _cx=0, int _cy=0);
  virtual ~Diagram();

  virtual Diagram* newOne();
  virtual int  calcDiagram() { return 0; }
  virtual void calcCoordinate
               (const double*, const double*, const double*, float*, float*, Axis const*) const {}
  void calcCoordinateP (const double*x, const double*y, const double*z, Graph::iterator& p, Axis const* A) const;
  virtual void calcCoordinatePh(const double*, float*, float*, Axis const*, Axis const*) const{}
  virtual void finishMarkerCoordinates(float&, float&) const;
  virtual void calcLimits() {}
  virtual QString extraMarkerText(Marker const*) const {return "";}
  
  virtual QRectF boundingRect() const;
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* item, QWidget* widget);
  void paintDiagram(QPainter* painter);
  void paintMarkers(QPainter* p, bool paintAll = true);

  void    setCenter(int, int, bool relative=false);
  void    getCenter(int&, int&);
  void    Bounding(int&, int&, int&, int&);
  bool    getSelected(int, int);
  bool    resizeTouched(float, float, float);
  QString save();
  bool    load(const QString&, QTextStream*);

  void getAxisLimits(Graph*);
  void updateGraphData();
  void loadGraphData(const QString&);
  void recalcGraphData();
  bool sameDependencies(Graph const*, Graph const*) const;
  int  checkColumnWidth(const QString&, const QFontMetrics&, int, int, int);

  virtual bool insideDiagram(float, float) const;
  bool insideDiagramP(Graph::iterator const& ) const;
  Marker* setMarker(int x, int y);

  bool insideDiagramPh(Graph::iterator const& , float*, float*) const;
  bool newcoordinate(Graph::iterator const& , float*, float*) const;
  /* PHASOR AND WAVEAC RELATED
  void phasorscale();
  void findaxisA(Graph*);
  bool findmatch(Graph* , int);
  void findfreq(Graph*);
  void setlimitsphasor(Axis* ,Axis*);
  double wavevalX(int) const;*/

  QString Name; // identity of diagram type (e.g. Polar), used for saving etc.
  QPen    GridPen;

  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;

  QString sfreq;
  double *freq=nullptr;
  int nfreqt,nfreqa;
  int x3, y3, sc;
  Axis  xAxis, yAxis, zAxis;   // axes (x, y left, y right)

  /* PHASOR DIAGRAM RELATED
  Axis  xAxisV, yAxisV, zAxisV, xAxisI, yAxisI, zAxisI, xAxisP, yAxisP, zAxisP, xAxisZ, yAxisZ,
 zAxisZ, *xAxisA, *yAxisA, *zAxisA;*/
  int State;  // to remember which resize area was touched

  bool hideLines;       // for "Rect3D": hide invisible lines ?
  int rotX, rotY, rotZ; // for "Rect3D": rotation around x, y and z axis

protected:
  void calcSmithAxisScale(Axis*, int&, int&);
  void createSmithChart(Axis*, int Mode=7);
  void calcPolarAxisScale(Axis*, double&, double&, double&);
  void createPolarDiagram(Axis*, int Mode=3);

  bool calcAxisScale(Axis*, double&, double&, double&, double&, double);
  bool calcAxisLogScale(Axis*, int&, double&, double&, double&, int);
  bool calcYAxis(Axis*, int);
  virtual void createAxisLabels();

  int  regionCode(float, float) const;
  virtual void clip(Graph::iterator &) const;
  void rectClip(Graph::iterator &) const;

  virtual void calcData(Graph*);

private:
  int Bounding_x1, Bounding_x2, Bounding_y1, Bounding_y2;
};

#endif
