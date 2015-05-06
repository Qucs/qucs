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
#include "../sim/data.h"

#include <cmath>
#include <assert.h>
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


#if 0
struct DataX {
  DataX(const QString& Var_, double *Points_=0, int count_=0)
       : Var(Var_), Points(Points_), count(count_), Min(INFINITY), Max(-INFINITY) {};
 ~DataX() { if(Points) delete[] Points; };
  QString Var;
  double *Points;
  int     count;
  const double* end() const{return Points+count;}

public:
  const double& min()const {return Min;}
  const double& max()const {return Max;}
public: // only called from Graph. cleanup later.
  void setLimit(const double& x){
	  if (Min>x) Min=x;
	  if (Max<x) Max=x;
  }
private:
  double Min;
  double Max;
};

class SimOutputData {
public:
  typedef std::pair<double,std::complex<double> > valuetype;
  class const_iterator{
    friend class SimOutputData; // need to access constructor.
  protected:
    const_iterator(double const* x, double const* y) : seekx(x), seeky(y) {};
  public:
    const_iterator& operator++(){ ++seekx; ++seeky; ++seeky; return *this;}
    valuetype operator*(){
      return valuetype(*seekx,std::complex<double>(*seeky,seeky[1]));
    }
    const valuetype* operator->() const{
      _v = valuetype(*seekx,std::complex<double>(*seeky,seeky[1]));
      return &_v;
    }
    bool operator==(const const_iterator& p)const { return seekx==p.seekx; }
    bool operator!=(const const_iterator& p)const { return seekx!=p.seekx; }
  private:
    double const* seekx;
    double const* seeky;
    static valuetype _v; // bit of a hack. lets see...
  };

  SimOutputData(const QString& filename, const QString& varname);
  int refresh();
  int loadIndepVarData(const QString&, char* datfilecontent, DataX* where);

  bool isEmpty() const { return !numAxes(); }
  unsigned numAxes() const { return CPointsX.count();}
  double const* coords(uint i) const;
  // double *cPointsY() const { return CPointsY; }
  int countY() const { return CountY; }
  static void attach(SimOutputData*, SimOutputData**);
  static void detach(SimOutputData**);
  const_iterator begin() const {return const_iterator(CPointsX[0]->Points, CPointsY);}
  const_iterator end() const {return const_iterator(CPointsX[0]->end(), NULL);}

public: // obsolete interface. don't use.
  DataX* axis(uint i) { if (i<axis_count) return CPointsX.at(i); return NULL;}
  double *cPointsY() const { return CPointsY; }
private:
  unsigned axis_count;
  QVector<DataX*> CPointsX;
  double *CPointsY;
  int CountY;    // number of curves
  QString Var;
  QString FileName;
  QDateTime lastLoaded;  // when it was loaded into memory
  unsigned attach_count;
public:
  const double& min()const {return Min;}
  const double& max()const {return Max;}
public:
  void setLimit(const double& x){
	  if (Min>x) Min=x;
	  if (Max<x) Max=x;
  }
private: // limits on "y"-axis (ordinate)
  double Min;
  double Max;
};
#endif

struct Axis;

class SimOutputDat : public SimOutputData {
public:
  SimOutputDat(const QString& filename, const QString& varname);
  SimOutputData const* refresh();
  int loadIndepVarData(const QString&, char* datfilecontent, DataX* where);

  bool isEmpty() const { return !numAxes(); }
  unsigned numAxes() const { return CPointsX.count();}
  DataX* axis(uint i) { if (i<axis_count) return CPointsX.at(i); return NULL;}
  double *cPointsY() const { return CPointsY; }
  int countY() const { return CountY; }
private:
  void clear();
private:
  unsigned axis_count;
  QList<DataX*>  CPointsX;
  double *CPointsY;
  int CountY;    // number of curves
  QString Var;
  QString fileName;
  QDateTime lastLoaded;  // when it was loaded into memory
};


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
  QString var() const {return Var;}
  QColor color() const {return Color;}
  int thick() const {return Thick;}
  graphstyle_t style() const {return Style;}
  int yAxisNo() const {return YAxisNo;}
  double *cPointsY() const { return CPointsY; }
  int countY() const { return CountY; }
  int precision() const{return Precision;}
  int numMode() const{return NumMode;}

public: // modify
  void setColor(QColor c) {Color = c;}
  void setThick(int c) {Thick = c;}
  void setStyle(graphstyle_t s) {Style = s;}
  void setPrecision(int p) {Precision = p;}
  void setNumMode(int m) {NumMode = m;}
  void setYAxisNo(int x) {YAxisNo = x;}

  void clear(){ScrPoints.resize(0); Graphs.resize(0); invalidateMarkers();}
  void resizeScrPoints(size_t s){assert(s>=(unsigned)ScrPoints.size()); ScrPoints.resize(s);}
  iterator begin(){return Graphs.begin();}
  iterator end(){return Graphs.end();}
  const_iterator begin() const{return Graphs.begin();}
  const_iterator end() const{return Graphs.end();}
  QList<Marker *> const& markers()const{return Markers;}

// private: not yet
  Graph::iterator _begin(){return ScrPoints.begin();}
  Graph::iterator _end(){return ScrPoints.end();}
  Graph::const_iterator _begin()const{return ScrPoints.begin();}
  Graph::const_iterator _end()const{return ScrPoints.end();}

private:
  QDateTime lastLoaded;  // when it was loaded into memory
  int YAxisNo;       // which y axis is used
  double *CPointsY;
  int     CountY;    // number of curves
  QString Var;
  QColor  Color;
  int     Thick;
  graphstyle_t Style;
public: // BUG
  QList<Marker *> Markers;

private: // FIXME: for tabular diagram
  int Precision;   // number of digits to show
  int NumMode;     // real/imag or polar (deg/rad)

private: // painting
  void drawLines(int, int, ViewPainter*) const;
  void drawStarSymbols(int, int, ViewPainter*) const;
  void drawCircleSymbols(int, int, ViewPainter*) const;
  void drawArrowSymbols(int, int, ViewPainter*) const;
public: // marker related
  Marker* newMarker(const_iterator const& p,
      GraphDeque const *pg_,
      int cx_, int cy_);
  Marker const* newMarker(
      GraphDeque::const_iterator const& p,
      GraphDeque const *pg,
      QString const& s);
  void createMarkerText() const;
  void moveMarkers(int x, int y);
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
public: // bookkeeping
  void attach(SimOutputData* d);
  bool has_data() const{return data;}
private:
  SimOutputData* data;
};

#endif
// vim:ts=8:sw=2:noet
