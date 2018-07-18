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

#include <stdlib.h>
#include <iostream>

#include <QPainter>
#include <QDebug>

class Diagram;

Graph::Graph(Diagram const* d, const QString& _Line) :
  Element(),
  Style(GRAPHSTYLE_SOLID),
  diagram(d)
{
  ElemType = isGraph;

  Var    = _Line;
  countY = 0;    // no points in graph
  Thick  = numMode = 0;
  Color  = 0x0000ff;   // blue
  Precision  = 3;
  ElemSelected = false;
  yAxisNo = 0;   // left y axis

  cPointsY = 0;
  gy=NULL;
}

Graph::~Graph()
{
  if(cPointsY != 0)
    delete[] cPointsY;
}

QRectF Graph::boundingRect() const
{
  return *(new QRectF());
}

void Graph::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{

}

// ---------------------------------------------------------------------
void Graph::createMarkerText() const
{
  for(auto pm : Markers) {
    pm->createText();
  }
}

// ---------------------------------------------------------------------
void Graph::paint(ViewPainter *p, int x0, int y0)
{
  if(!ScrPoints.size())
    return;

  if(ElemSelected) {
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
void Graph::paintLines(ViewPainter *p, int x0, int y0)
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
// ---------------------------------------------------------------------
/*paint function for phasor diagram*/
void Graph::paintvect(ViewPainter *p, int x0, int y0)
{
    if(!ScrPoints.size())
    return;

  if(ElemSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,Thick*p->PrintScale+4));
    drawvect(x0, y0, p);

    p->Painter->setPen(QPen(Qt::white, Thick*p->PrintScale, Qt::SolidLine));
    drawvect(x0, y0, p);
    return;
  }

  // **** not selected ****
  p->Painter->setPen(QPen(QColor(Color), Thick*p->PrintScale, Qt::SolidLine));
  drawvect(x0, y0, p);
}
// ---------------------------------------------------------------------
QString Graph::save()
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
bool Graph::load(const QString& _s)
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
 * Checks if the coordinates x/y point to the graph. returns the number of the
 * branch of the graph, -1 upon a miss.
 *
 * x/y are relative to diagram cx/cy. 5 is the precision the user must point
 * onto the graph.
 *
 * FIXME: should return reference to hit sample point or some context.
 */
int Graph::getSelected(int x, int y)
{
  auto pp = ScrPoints.begin();
  if(pp == ScrPoints.end()) return -1;

  int A, z=0;
  int dx, dx2, x1;
  int dy, dy2, y1;

  int countX = cPointsX.at(0)->count;
  if(pp->isStrokeEnd()) {
    if(pp->isBranchEnd()) z++;
    pp++;
    if(pp->isBranchEnd()) {
      if(pp->isGraphEnd())  return -1;   // not even one point ?
      z++;
      pp++;
      if(pp->isGraphEnd())  return -1;   // not even one point ?
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
        return z*countX;   // points on graph symbol
      }
      else {
        z++;   // next branch
        pp++;
      }
    }
    return -1;
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
        dx2 = int(pp->getScrX());  // go on as graph can also be selected between strokes
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

      if(A <= 0)  return z*countX;  // lies x/y onto the graph line ?
    }
    pp++;
    z++;
  }

  return -1;
}
// -----------------------------------------------------------------------
/*it's a select function for phasordiagram that with the 2 points of the vector 
creates a linear equation and find if the point is in that equation*/

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

}
// -----------------------------------------------------------------------
// Creates a new graph and copies all the properties into it.
Graph* Graph::sameNewOne()
{
  Graph *pg = new Graph(diagram, Var);

  pg->Color = Color;
  pg->Thick = Thick;
  pg->Style = Style;

  pg->Precision = Precision;
  pg->numMode   = numMode;
  pg->yAxisNo   = yAxisNo;

  foreach(Marker *pm, Markers)
    pg->Markers.append(pm->sameNewOne(pg));

  return pg;
}

/*!
 * find a sample point close to VarPos, snap to it, and return data at VarPos
 */
std::pair<double,double> Graph::findSample(std::vector<double>& VarPos) const
{
  DataX const* pD;
  unsigned nVarPos=0;
  unsigned n=0;
  unsigned m=1;

  for(unsigned ii=0; (pD=axis(ii)); ++ii) {
    double* pp = pD->Points;
    double v = VarPos[nVarPos];
    for(unsigned i=pD->count; i>1; i--) {  // find appropiate marker position
      if(fabs(v-(*pp)) < fabs(v-(*(pp+1)))) break;
      pp++;
      n += m;
    }

    m *= pD->count;
    VarPos[nVarPos++] = *pp;
  }

  return std::pair<double,double>(cPointsY[2*n], cPointsY[2*n+1]);
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
void Graph::ScrPt::setDep(double x)
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
double Graph::ScrPt::getDep() const
{
  assert(ScrX>=0);
  return dep;
}

// vim:ts=8:sw=2:et
