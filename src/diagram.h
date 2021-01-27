/***************************************************************************
    copyright            : (C) 2003 Michael Margraf
                               2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef DIAGRAM_H
#define DIAGRAM_H

//#include "graph.h"
#include "../legacy/marker.h"
#include "element.h"
#include "viewpainter.h"

#include <QFile>
#include <QTextStream>
#include <QList>

#define MIN_SCROLLBAR_SIZE 8

#define INVALID_STR QObject::tr(" <invalid>")

// BUG obsolete_paintings
class Arc;
class Line;
class Text;

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

class QMouseEvent;
class istream_t;
class FontMetrics;
class Graph;

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


// yikes.
#include "graph.h"
typedef Graph::iterator GraphIterator;

class Diagram : public Element {
protected:
	Diagram(Diagram const& d);
public:
  Diagram(int _cx=0, int _cy=0);
  virtual ~Diagram();
  virtual Diagram* newOne(){ // BUG: pure. legacy interface.
	  unreachable();
	  return nullptr;
  }
  Element* clone() const{
	  Diagram const* e=this;
	  Diagram* E=const_cast<Diagram*>(e);
	  return E->newOne();
  }
  virtual int  calcDiagram() { return 0; };
  virtual void calcCoordinate
               (const double*, const double*, const double*, float*, float*, Axis const*) const {}
  void calcCoordinateP (const double*x, const double*y, const double*z, GraphIterator& p, Axis const* A) const;
  virtual void calcCoordinatePh(const double*, float*, float*, Axis const*, Axis const*) const{};
  virtual void finishMarkerCoordinates(float&, float&) const;
  virtual void calcLimits() {}
  virtual QString extraMarkerText(Marker const*) const {return "";}
  
private:
  virtual void paint(ViewPainter*) const override;
//  pos_t center() const override;
  void    getCenter(int&, int&); //override; //remove this.
  QDialog* schematicWidget(QucsDoc* Doc) const override;
  virtual QRectF boundingRect() const; // { unreachable(); return QRectF(); }

protected:
  rect_t bounding_rect() const{
	  auto br = boundingRect();
	  return rect_t(br);
  }

public:
  QString const& name() const{ incomplete(); return Name;}
//  QString const& typeName() const{return Name;}
  void setName(QString const& n){ Name = n; }


public: // ??!
  virtual void paintDiagram(ViewPainter* p);
  void paintMarkers(ViewPainter* p, bool paintAll = true);
//  void    setCenter(int, int, bool relative=false);
  void    paintScheme(SchematicDoc*) const;
  void    Bounding(int&, int&, int&, int&);
  bool    getSelected(int, int);
  bool    resizeTouched(float, float, float);
  QString save();
  bool    load(const QString&, istream_t&);

  void getAxisLimits(Graph*);
  void updateGraphData();
  void loadGraphData(const QString&);
  void recalcGraphData();
  bool sameDependencies(Graph const*, Graph const*) const;
  int  checkColumnWidth(const QString&, const FontMetrics&, int, int, int);

  virtual bool insideDiagram(float, float) const;
  bool insideDiagramP(GraphIterator const& ) const;
  Marker* setMarker(int x, int y);

private: // internals
public: // BUG/incomplete
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
  virtual void clip(GraphIterator &) const;
  void rectClip(GraphIterator &) const;

  virtual void calcData(Graph*);

public: // from mouseactions.cpp
  virtual bool scrollTo(int, int, int){return false;}
  virtual int scroll(int){return 0;}
  virtual int xAxis_limit_min() const{
	  return xAxis.limit_min;
  }

  // returns drawn, for now.
  bool pressElement(SchematicDoc* Doc, Element*& selElem, QMouseEvent* Event);

protected: // FIXME
  int x2;
  int y2;
private:
  int Bounding_x1, Bounding_x2, Bounding_y1, Bounding_y2;
protected:
  QString Name; // the label, but sometimes the type. yikes.
}; // Diagram

#endif
