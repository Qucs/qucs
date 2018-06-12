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


#include "element.h"

#include <cmath>
#include <complex>
#include <QColor>
#include <QDateTime>

#include <assert.h>

typedef std::complex<double> cplx_t;

typedef enum{
  GRAPHSTYLE_INVALID = -1,
  GRAPHSTYLE_SOLID = 0,
  GRAPHSTYLE_DASH,
  GRAPHSTYLE_DOT,
  GRAPHSTYLE_LONGDASH,
  GRAPHSTYLE_STAR,
  GRAPHSTYLE_CIRCLE,
  GRAPHSTYLE_ARROW,
  GRAPHSTYLE_COUNT,
} graphstyle_t;

inline graphstyle_t toGraphStyle(int x){
  if (x<0){
    return GRAPHSTYLE_INVALID;
  }else if(x<GRAPHSTYLE_COUNT){
    return graphstyle_t(x);
  }else{
    return GRAPHSTYLE_INVALID;
  }
}

class Diagram;
class Marker;
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

struct Axis;

/*!
 * a graph (in the usual sense)
 * FIXME: this is a COMPLEX graph in one variable,
 *        there are other graphs...
 *        should be made more explicit.
 */
class /*Complex*/Graph /*: public GraphBase*/ {
public:
  class ScrPt{
    float ScrX;
    float ScrY;

    double indep; // top level indep value (sweep)
    cplx_t dep; // top level dep value
  public:
    ScrPt() : ScrX(0){}
    ~ScrPt(){}

    void setStrokeEnd();
    void setBranchEnd();
    void setGraphEnd();
    void setScrX(float); // screen horizontal coordinate
    void setScrY(float); // screen vertical coordinate
    void setScr(float,float); // both @ once.
    void setIndep(double);
    void setDep(cplx_t);
//    void attachCoords(double*);

    bool isPt() const; // indicate if this is a point on the screen
    bool isStrokeEnd() const;
    bool isBranchEnd() const;
    bool isGraphEnd() const;

    float getScrX() const;
    float getScrY() const;
    double getIndep() const;
    cplx_t getDep() const;
  };
  typedef QVector<ScrPt> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;

//  iterator begin(){return b;}
//  iterator end(){return e;}
  const_iterator begin() const{return b;}
  const_iterator end() const{return e;}
public:
  Graph() : b(), e() {} // required by GraphDeque::Graphs.resize()...
  Graph(const_iterator B, const_iterator E) : b(B), e(E) {}
  const_iterator findSample(double&) const;
  ~Graph(){}
private:
  /*const*/ const_iterator b;
  /*const*/ const_iterator e;
  // SimOutputData* origin;
};

/*!
 * prepare data for plotting purposes in Diagram.
 * iterating yields Graphs.
 *
 * also stores markers.
 */
class GraphDeque : public Element /*why Element?*/ {
  GraphDeque(const GraphDeque&){assert(false/*unreachable*/);}
public:
  GraphDeque(const Diagram*, const QString& _Line="");
  ~GraphDeque();

  typedef QVector<Graph> container;
  typedef container::iterator iterator;
  typedef container::const_iterator const_iterator;

  // just a sketch. (how about markers in between sampling points?)
  typedef std::pair<GraphDeque::const_iterator, Graph::const_iterator> MarkerPos;

  int loadDatFile(const QString& filename);
  int loadIndepVarData(const QString&, char* datfilecontent, DataX* where);

  void    paint(ViewPainter*, int, int);
  void    paintLines(ViewPainter*, int, int);
  QString save();
  bool    load(const QString&);
  GraphDeque::const_iterator getSelected(int, int) const;
  GraphDeque* sameNewOne();
  
private: // tmp hack
  DataX* mutable_axis(uint i) { if(i<(uint)cPointsX.size()) return cPointsX.at(i); return NULL;}
public:
  unsigned numAxes() const { return cPointsX.size(); }
  DataX const* axis(uint i) const { if(i<(uint)cPointsX.size()) return cPointsX.at(i); return NULL;}
  double const* coords(uint i) const { // ++i;
                                       if(i<(uint)cPointsX.size())
                                         if(cPointsX.at(i))
					   return cPointsX.at(i)->Points;
					     return NULL;}
  size_t count(uint i) const { if(axis(i)) return axis(i)->count; return 0; }
  QString axisName(unsigned i) const {if(axis(i))return axis(i)->Var; return "";}
  bool isEmpty() const { return !cPointsX.size(); }
  QVector<DataX*>& mutable_axes(){return cPointsX;} // HACK

  void clear(){ScrPoints.resize(0); Graphs.resize(0); invalidateMarkers();}
  void resizeScrPoints(size_t s){assert(s>=(unsigned)ScrPoints.size()); ScrPoints.resize(s);}
  iterator begin(){return Graphs.begin();}
  iterator end(){return Graphs.end();}
  const_iterator begin() const{return Graphs.begin();}
  const_iterator end() const{return Graphs.end();}

// private: not yet
  Graph::iterator _begin(){return ScrPoints.begin();}
  Graph::iterator _end(){return ScrPoints.end();}
  Graph::const_iterator _begin()const{return ScrPoints.begin();}
  Graph::const_iterator _end()const{return ScrPoints.end();}

  QDateTime lastLoaded;  // when it was loaded into memory
  int     yAxisNo;       // which y axis is used
  double *cPointsY;
  int     countY;    // number of curves
  QString Var;
  QColor  Color;
  int     Thick;
  graphstyle_t Style;
  QList<Marker *> Markers;
  double *gy;

  // for tabular diagram
  int  Precision;   // number of digits to show
  int  numMode;     // real/imag or polar (deg/rad)

private: // painting
  void drawLines(int, int, ViewPainter*) const;
  void drawStarSymbols(int, int, ViewPainter*) const;
  void drawCircleSymbols(int, int, ViewPainter*) const;
  void drawArrowSymbols(int, int, ViewPainter*) const;
public: // marker related
  void createMarkerText() const;
  MarkerPos findSample(std::vector<double>&) const;
  void samplePos(const_iterator, std::vector<double>& VarPos) const;
  Diagram const* parentDiagram() const{return diagram;}
  void invalidateMarkers();
private:
  QVector<DataX*>  cPointsX;
  Graph::container ScrPoints; // data in screen coordinates
  Diagram const* diagram;
  container Graphs;
public: // BUG. used by Diagram
  void push_back(const Graph& g);
  const Graph& back() const {return Graphs.back();}
};

#endif

// vim:ts=8:sw=2:noet
