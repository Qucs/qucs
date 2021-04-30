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

#ifndef QUCS_DIAGRAM_H
#define QUCS_DIAGRAM_H

#include "element.h"
#include "viewpainter.h"
#include "exception.h"

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

namespace qucs{

class Graph;

// Element?
struct Axis {
  void fit_in_data(double lo, double hi);

  double  min, max; // least and greatest values of all graph data
  double  low, up;  // the limits of the diagram
  bool    log;      // in "rectdiagram": logarithmic or linear
  QString Label;
  int     numGraphs;  // counts number of graphs using this axis
  bool    GridOn;

  bool   autoScale;    // manual limits or auto-scale ?
  double limit_min, limit_max, step;   // if not auto-scale

};

}

// yikes.
#include "graph.h"

/*--------------------------------------------------------------------------*/
namespace qucs {
/*--------------------------------------------------------------------------*/
typedef qucs::Graph::iterator GraphIterator;
/*--------------------------------------------------------------------------*/
class Diagram : public Element {
public:
	typedef std::pair<float, float> diag_coordinate_t;

protected:
	Diagram(Diagram const& d);
public:
  Diagram();
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
  virtual diag_coordinate_t calcCoordinate(double const& x, double const& y) const = 0;
  virtual void finishMarkerCoordinates(float&, float&) const;
  virtual void calcLimits() {}
  virtual QString extraMarkerText(Marker const*) const {return "";}
  
private:
//   virtual void paint(ViewPainter*) const override; // not necessarily
//  pos_t center() const override;
//  void    getCenter(int&, int&); //override; //remove this.
  Widget* schematicWidget(Doc* Doc) const override;

protected:
  rect_t bounding_rect() const;
  void prepare() override;

public:
//  QString const& name() const{ incomplete(); return Name;}
//  QString const& typeName() const{return Name;}
//  void setName(QString const& n){ Name = n; }

public:
  QString save() const; // obsolete callback

public: // Parameters
	virtual unsigned param_count() const;
//	virtual bool paramIsPrintable()const;
	virtual void set_param_by_name(std::string const&, std::string const&){unreachable();}
	virtual void set_param_by_index(index_t, std::string const&);
	virtual std::string param_by_name(std::string const& n)const;
	virtual std::string param_value(index_t i) const;
	virtual std::string param_name(index_t n) const;

public:
	ElementList* scope() override{ return _subckt; }

private:
	bool makes_own_scope() const override{ untested(); return true;}

public: // ??!
	// void paint(ViewPainter* p) override;
	void paintDiagram(ViewPainter* p);
	void paintMarkers(ViewPainter* p, bool paintAll = true);
//  void    setCenter(int, int, bool relative=false);
//  void    paintScheme(SchematicDoc*) const;
	// bool    resizeTouched(float, float, float);
	bool    load(const QString&, istream_t&); // TODO: remove.
	// bool sameDependencies(Graph const*, Graph const*) const;

	// virtual bool insideDiagram(float, float) const;
	// bool insideDiagramP(GraphIterator const& ) const;
	// Marker* setMarker(int x, int y);

private: // internals
public: // BUG/incomplete
  QPen    GridPen;

//  QString sfreq;
//  double *freq=nullptr;
  int nfreqt,nfreqa;
  int x3, y3, sc;
  Axis  xAxis, yAxis, zAxis;   // axes (x, y left, y right)

  int State;  // to remember which resize area was touched


protected:
//  void calcSmithAxisScale(Axis*, int&, int&);
//  void createSmithChart(Axis*, int Mode=7);

  // bool calcYAxis(Axis*, int);
  virtual void createAxisLabels();

//  int  regionCode(float, float) const;
//  virtual void clip(GraphIterator &) const;
//  void rectClip(GraphIterator &) const;

  virtual void calcData(Graph*);

  ElementList* subckt(){return _subckt;}

public:
  ElementList const* subckt() const{return _subckt;}

private:
	void new_subckt();

public: // from mouseactions.cpp
//  virtual bool scrollTo(int, int, int){return false;}
//  virtual int scroll(int){return 0;}
//  virtual int xAxis_limit_min() const{
//	  return xAxis.limit_min;
//  }

  // returns drawn, for now.
//  bool pressElement(SchematicDoc* Doc, Element*& selElem, QMouseEvent* Event);

protected: // FIXME
  int _width;
  int _height;

protected:
  QString Name; // the label, but sometimes the type. yikes.
private:
  ElementList* _subckt{nullptr};
}; // Diagram
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
#endif
