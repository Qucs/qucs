/***************************************************************************
                          graph.cpp  -  description
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
#include "graph.h"
#include "marker.h"
#include "viewpainter.h"

#include <stdlib.h>
#include <iostream>

#include <QPainter>
#include <QDebug>

class Diagram;

GraphDeque::GraphDeque(Diagram const* d, const QString& _Line) :
  Element(),
  Style(GRAPHSTYLE_SOLID),
  diagram(d)
{
  Type = isGraph;

  Var    = _Line;
  countY = 0;    // no points in graph
  Thick  = numMode = 0;
  Color  = 0x0000ff;   // blue
  Precision  = 3;
  isSelected = false;
  yAxisNo = 0;   // left y axis

  cPointsY = 0;
  gy=NULL;
}

GraphDeque::~GraphDeque()
{
  if(cPointsY != 0)
    delete[] cPointsY;
}

// ---------------------------------------------------------------------
void GraphDeque::createMarkerText() const
{
  for(auto pm : Markers) {
    qDebug() << "marker" << pm->varPos().size();
    assert(pm);

// no. maybe data has just reloaded...
//    assert(pm->splPos().first>=begin());
//    assert(pm->splPos().first<=end());

    if(pm->splPos().second<_begin()){
      qDebug() << "bug, bogus marker";
    }else if(pm->splPos().second>=_end()){
      qDebug() << "bug, bogus marker end";
    }else{
    }
    pm->createText();
  }
}

// ---------------------------------------------------------------------
void GraphDeque::paint(ViewPainter *p, int x0, int y0)
{
  if(!ScrPoints.size())
    return;

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Thick*p->PrintScale+4));
    paintLines(p, x0, y0);

    p->Painter->setPen(QPen(Qt::white, Thick*p->PrintScale, Qt::SolidLine));
    paintLines(p, x0, y0);
    return;
  }

  // **** not selected ****
  p->Painter->setPen(QPen(QColor(Color), Thick*p->PrintScale, Qt::SolidLine));
  paintLines(p, x0, y0);
}

// ---------------------------------------------------------------------
void GraphDeque::paintLines(ViewPainter *p, int x0, int y0)
{
  switch(Style) {
    case GRAPHSTYLE_STAR:
      drawStarSymbols(x0, y0, p);
      break;
    case GRAPHSTYLE_CIRCLE:
      drawCircleSymbols(x0, y0, p);
      break;
    case GRAPHSTYLE_ARROW:
      drawArrowSymbols(x0, y0, p);
      break;
    default:
      drawLines(x0, y0, p);
  }
}
/* PHASOR DIAGRAM CODE
// ---------------------------------------------------------------------
//paint function for phasor diagram
void Graph::paintvect(ViewPainter *p, int x0, int y0)
{
    if(!ScrPoints.size())
    return;

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Thick*p->PrintScale+4));
    drawvect(x0, y0, p);

    p->Painter->setPen(QPen(Qt::white, Thick*p->PrintScale, Qt::SolidLine));
    drawvect(x0, y0, p);
    return;
  }

  // **** not selected ****
  p->Painter->setPen(QPen(QColor(Color), Thick*p->PrintScale, Qt::SolidLine));
  drawvect(x0, y0, p);
}*/

// ---------------------------------------------------------------------
QString GraphDeque::save()
{
  QString s = "\t<\""+Var+"\" "+Color.name()+
	      " "+QString::number(Thick)+" "+QString::number(Precision)+
	      " "+QString::number(numMode)+" "+QString::number(Style)+
	      " "+QString::number(yAxisNo)+">";

  foreach(Marker *pm, Markers)
    s += "\n\t  "+pm->save();

  return s;
}

// ---------------------------------------------------------------------
bool GraphDeque::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  Var = s.section('"',1,1);  // Var

  QString n;
  n  = s.section(' ',1,1);    // Color
  Color.setNamedColor(n);
  if(!Color.isValid()) return false;

  n  = s.section(' ',2,2);    // Thick
  Thick = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // Precision
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);    // numMode
  numMode = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);    // Style
  int st = n.toInt(&ok);
  if(!ok) return false;
  Style = toGraphStyle(st);
  if(Style==GRAPHSTYLE_INVALID) return false;

  n  = s.section(' ',6,6);    // yAxisNo
  if(n.isEmpty()) return true;   // backward compatible
  yAxisNo = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// -----------------------------------------------------------------------
/*!
 * Checks if the coordinates x/y point to a graph within this deque.
 * returns corresponding iterator, end() upon a miss.
 *
 * x/y are relative to diagram cx/cy. 5 is the precision the user must point
 * onto the graph.
 *
 * FIXME: should return reference to hit sample point or some context.
 */
GraphDeque::const_iterator GraphDeque::getSelected(int x, int y) const
{
  auto z = begin();
  auto pp = ScrPoints.begin();
  if(pp == ScrPoints.end()){
    return end();
  }

  int A;
  int dx, dx2, x1;
  int dy, dy2, y1;

  if(pp->isStrokeEnd()) {
    if(pp->isBranchEnd()) z++;
    pp++;
    if(pp->isBranchEnd()) {
      if(pp->isGraphEnd()) { // not even one point ?
        return end();
      }
      z++;
      pp++;
      if(pp->isGraphEnd()) { // not even one point ?
        return end();
      }
    }
  }

  if(Style >= GRAPHSTYLE_STAR || gy!=NULL) {
    // for graph symbols
    while(!pp->isGraphEnd()) {
      if(!pp->isStrokeEnd()) {
        dx  = x - int((pp)->getScrX());
        dy  = y - int((pp++)->getScrY());
        if(dx < -5) continue;
        if(dx >  5) continue;
        if(dy < -5) continue;
        if(dy >  5) continue;
        return z;
      }
      else {
        assert(z!=end());
        z++;   // next graph
        pp++;
      }
    }
    return end();
  }

  // for graph lines
  while(!pp->isGraphEnd()) {
    while(!pp->isBranchEnd()) {
      x1 = int(pp->getScrX());
      y1 = int((pp++)->getScrY());
      dx  = x - x1;
      dy  = y - y1;

      if(pp->isPt()){
        dx2 = int(pp->getScrX());
      }else if(pp->isBranchEnd()) {
        break;
      }else if(pp->isStrokeEnd()) {
        pp++;
        if(pp->isPt()){
          dx2 = int(pp->getScrX());  // go on as graph can also be selected between strokes
        }
        if(pp->isBranchEnd()) break;
      }
      if(dx < -5) { if(x < dx2-5) continue; } // point between x coordinates ?
      else { if(x > 5) if(x > dx2+5) continue; }

      dy2 = int(pp->getScrY());
      if(dy < -5) { if(y < dy2-5) continue; } // point between y coordinates ?
      else { if(y > 5) if(y > dy2+5) continue; }

      dx2 -= x1;
      dy2 -= y1;

      A  = dx2*dy - dx*dy2;    // calculate the rectangle area spanned
      A *= A;                  // avoid the need for square root
      A -= 25*(dx2*dx2 + dy2*dy2);  // substract selectable area

      if(A <= 0)  return z; // x/y lies on the graph line
    }
    pp++;
    assert(z!=end());
    z++;
  }

  return end();
}

/* PHASOR DIAGRAM CODE
// -----------------------------------------------------------------------
//it's a select function for phasordiagram that with the 2 points of the vector
//creates a linear equation and find if the point is in that equation
int Graph::getSelectedP(int x, int y)
{

  float f1,f2,f3,f4;
  float xn,yn;
  float d1,b1;
  auto pp = ScrPoints.begin();
  if(pp == ScrPoints.end()) return -1;

  if(pp->isStrokeEnd())
    pp++;
  while(!pp->isGraphEnd()) {
    if(!pp->isBranchEnd())
    {
      f1 = pp->getScrX();
      f2 = (pp++)->getScrY();
      f3 = pp->getScrX();
      f4 = (pp++)->getScrY();
  
      if((f1 > f3 - 5) && (f1 < f3 + 5))
      {
        xn = f1;
        yn = y;
      }
      else  
      {
        if((f2 > f4 - 5) && (f2 < f4 + 5))
        {
          xn = x;
          yn = f2;
        }
        else
        { 
          d1 = (f4 - f2) / (f3 - f1);
          b1 = f4 - d1 * f3 ;
          xn = (float(y) - b1) / d1;
          yn = d1 * float(x) + b1;
        }
      }
      if(((f1 >= f3) && (xn >= f3)  && (xn <= f1)) || ((f3 >= f1) && (xn >= f1)  && (xn <= f3)))
        if(((f2 >= f4) && (yn >= f4) && (yn <= f2)) || ((f4 >= f2) && (yn >= f2) && (yn <= f4)))
          if((y >= int(yn) - 5) && (y <= int(yn) + 5) && (x >= int(xn) - 5) && (x <= int(xn) + 5))
            return 1;
    }
    else
      pp++;
  }

  return -1;

}*/

// -----------------------------------------------------------------------
// Creates a new graph and copies all the properties into it.
GraphDeque* GraphDeque::sameNewOne()
{
  // FIXME: implement private copy constructor and use here.
  GraphDeque *pg = new GraphDeque(diagram, Var);

  pg->Color = Color;
  pg->Thick = Thick;
  pg->Style = Style;

  pg->Precision = Precision;
  pg->numMode   = numMode;
  pg->yAxisNo   = yAxisNo;

  qDebug() << "cloning" << Markers.size() << "Markers";
  foreach(Marker *pm, Markers) {
    pg->Markers.append(pm->sameNewOne(pg));
  }

  return pg;
}

// ---------------------------------------------------------------------
/*!
 * lookup a graph closest to some coordinates
 */
#if 0 // later
GraphDeque::iterator GraphDeque::findGraph(double* coordinate)
{
}
#endif

// -----------------------------------------------------------------------
/*!
 * snap position to nearby lattice point.
 * return iterators.
 * BUG: using grossly suboptimal linear search.
 */
GraphDeque::MarkerPos GraphDeque::findSample(std::vector<double>& VarPos) const
{
  DataX const* pD;
  if(end()==begin()){
    qDebug() << "snap: empty GraphDeque";
    return MarkerPos(end(), Graph::const_iterator());
  }

  const_iterator n = begin();
  unsigned m=1;

  for(unsigned ii=1; (pD=axis(ii)); ++ii) {
    double* pp = pD->Points;
    double v = VarPos[ii];
    for(unsigned i=pD->count; i>1; i--) {  // find appropiate marker position
      if(fabs(v-(*pp)) < fabs(v-(*(pp+1)))) break;
      pp++;
      n += m;
    }

    m *= count(ii);
    VarPos[ii] = *pp;
  }
  assert(n<=end());

  auto gPos = n->findSample(VarPos[0]);
  return MarkerPos(n, gPos);
}

/*!
 * write graph coordinates into vector.
 */
void GraphDeque::samplePos(const_iterator here, std::vector<double>& VarPos) const
{
  assert(here>=begin());
  assert(here<=end());
  if(here==end()){ // just assert? later..
    qDebug() << "BUG: invalid iterator in GraphDeque::samplePos";
    return;
  }

  double const*px;
  unsigned nn = here - begin();
  DataX const *pD = axis(0); // BUG: don't use DataX

  assert(VarPos.size()>=numAxes());
  for(unsigned i=1; (pD = axis(i)); ++i) {
    px = coords(i);
    px += (nn % count(i));
    VarPos[i] = *px;
    nn /= count(i);
  }
}

/*!
 * snap position to nearby screen sample.
 * BUG: using grossly suboptimal linear search.
 */
Graph::const_iterator Graph::findSample(double& v) const
{
  qDebug() << "findsample" << end()-begin();
  if(end()-begin()==0){
    qDebug() << "oops. empty graph?";
    return end();
  }
  for(auto i=begin(); i!=end(); ++i) {
    if (i+1 == end()) {
      return i;
    }else if (!i->isPt()) {
      qDebug() << "BUG, not a point in findSample";
      return end();
    } else if (!(i+1)->isPt()) {
      qDebug() << "BUG, not a point in findSample";
      return end();
    }
    if(fabs(v-i->getIndep()) < fabs(v-(i+1)->getIndep())) {
      v = i->getIndep();
      return i;
    }
  }
  qDebug() << "Graph::findSample, empty";
  assert(end()==begin());
  return end();
}

void GraphDeque::invalidateMarkers(){
  for (auto m : Markers){
    m->makeInvalid();
  }
}

/*!
 * push back a new graph.
 * currently done by Diagram (FIXME?)
 */
void GraphDeque::push_back(const Graph& g)
{
  assert(g.begin()>=_begin());
  assert(g.end()<=_end());
  Graphs.push_back(g);
}

// -----------------------------------------------------------------------
// meaning of the values in a graph "Points" list
#define STROKEEND   -2
#define BRANCHEND   -10
#define GRAPHEND    -100
// -----------------------------------------------------------------------
// screen points pseudo iterator implementation.
void Graph::ScrPt::setStrokeEnd()
{
  ScrX = STROKEEND;
}
void Graph::ScrPt::setBranchEnd()
{
  ScrX = BRANCHEND;
}
void Graph::ScrPt::setGraphEnd()
{
  ScrX = GRAPHEND;
}
bool Graph::ScrPt::isPt() const{return ScrX>=0.;}
bool Graph::ScrPt::isStrokeEnd() const{return ScrX<=STROKEEND;}
bool Graph::ScrPt::isBranchEnd() const{return ScrX<=BRANCHEND;}
bool Graph::ScrPt::isGraphEnd() const{return ScrX<=GRAPHEND;}

/*!
 * set screen coordinate for graph sampling point
 * these must be nonnegative, but sometimes aren't,
 * eg. between calcCoordinate and clip.
 * (negative values are reserved for control.)
 */
void Graph::ScrPt::setScrX(float x)
{
  if(x<0){
    std::cerr << "dangerous: negative screen coordinate" << x;
  }
  if(ScrX<0){
    std::cerr << "dangerous: (maybe) overwriting control token" << x;
  }
  ScrX = x;
}
void Graph::ScrPt::setScrY(float x)
{
  if(x<0){ // need to investigate...
    qDebug() << "setting negative screen coordinate" << x << "at" << ScrX;
  }
  ScrY = x;
}
void Graph::ScrPt::setScr(float x, float y)
{
  setScrX(x);
  setScrY(y);
}
void Graph::ScrPt::setIndep(double x)
{
  assert(ScrX>=0);
  indep = x;
}
void Graph::ScrPt::setDep(cplx_t x)
{
  assert(ScrX>=0);
  dep = x;
}
float Graph::ScrPt::getScrX() const
{
  if(ScrX<0){
    std::cerr << "dangerous: returning negative screen coordinate" << ScrX;
  }
  return ScrX;
}
float Graph::ScrPt::getScrY() const
{
  return ScrY;
}
double Graph::ScrPt::getIndep() const
{
  assert(ScrX>=0);
  return indep;
}
cplx_t Graph::ScrPt::getDep() const
{
  assert(ScrX>=0);
  return dep;
}

// vim:ts=8:sw=2:et
