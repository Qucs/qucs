/***************************************************************************
                          marker.cpp  -  description
                             -------------------
    begin                : Sat Apr 10 2004
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

#include "marker.h"
#include "diagram.h"
#include "graph.h"
#include "main.h"

#include <qstring.h>
#include <qwidget.h>
#include <qpainter.h>

#include <limits.h>
#include <math.h>


Marker::Marker(Diagram *Diag_, Graph *pg_, int _nn, int cx_, int cy_)
{
  Type = isMarker;
  isSelected = false;

  Diag   = Diag_;
  pGraph = pg_;
  Precision = 2;   // before createText()
  lookNfeel = 1;
  numMode = nVarPos = 0;
  cx = cx_;  cy = cy_;

  if(!pGraph)  makeInvalid();
  else initText(_nn);   // finally create marker

  x1 = cx + 100;
  y1 = cy + 100;
}

Marker::~Marker()
{
}

// ---------------------------------------------------------------------
void Marker::initText(int n)
{
  if(pGraph->cPointsX.isEmpty()) {
      makeInvalid();
      return;
  }

  double num, *py = (pGraph->cPointsY) + 2*n;
  Text = "";
  nVarPos = 0;
  DataX *pD = pGraph->cPointsX.first();

  // find exact marker position
  int nn, x, y, d, dmin = INT_MAX;
  for(nn=n % pD->count; nn<pD->count; nn++) {
    num = *(pD->Points + nn);
    Diag->calcCoordinate(num, *py, *(py+1), &x, &y);
    x -= cx;
    y -= cy;
    d = x*x + y*y;
    if(d < dmin) {
      dmin = d;
      n = (n - n % pD->count) + nn;
    }
    py += 2;
  }
  if(dmin == INT_MAX)
    n = (n - n % pD->count) + nn - 1; // take last position

  // independent variables
  nn = n;
  for(; pD!=0; pD = pGraph->cPointsX.next()) {
    num = *(pD->Points + (nn % pD->count));
    VarPos[nVarPos++] = num;
    Text += pD->Var + ": " + QString::number(num,'g',Precision) + "\n";
    nn /= pD->count;
  }

  // dependent variable
  double yr = *((pGraph->cPointsY) + 2*n);
  double yi = *((pGraph->cPointsY) + 2*n+1);
  Text += pGraph->Var + ": ";
  switch(numMode) {
    case 0: Text += complexRect(yr, yi, Precision);
	    break;
    case 1: Text += complexDeg(yr, yi, Precision);
	    break;
    case 2: Text += complexRad(yr, yi, Precision);
	    break;
  }

  Diag->calcCoordinate(VarPos[0], yr, yi, &cx, &cy);

  QFontMetrics  metrics(QucsSettings.font);
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
void Marker::createText()
{
  if(!(pGraph->cPointsY)) {
    makeInvalid();
    return;
  }

  Text = "";
  double *pp, v;
  while((unsigned int)nVarPos < pGraph->cPointsX.count())
    VarPos[nVarPos++] = 0.0;   // fill up VarPos


  // independent variables
  int n = 0, m = 1, i;
  DataX *pD;
  nVarPos = 0;
  for(pD = pGraph->cPointsX.first(); pD!=0; pD = pGraph->cPointsX.next()) {
    pp = pD->Points;
    v  = VarPos[nVarPos];
    for(i=pD->count; i>1; i--) {  // find appropiate marker position
      if(fabs(v-(*pp)) < fabs(v-(*(pp+1)))) break;
      pp++;
      n += m;
    }

    m *= pD->count;
    VarPos[nVarPos++] = *pp;
    Text += pD->Var + ": " + QString::number(*pp,'g',Precision) + "\n";
  }


  double yr = *((pGraph->cPointsY) + 2*n);
  double yi = *((pGraph->cPointsY) + 2*n+1);
  Text += pGraph->Var + ": ";
  switch(numMode) {
    case 0: Text += complexRect(yr, yi, Precision);
	    break;
    case 1: Text += complexDeg(yr, yi, Precision);
	    break;
    case 2: Text += complexRad(yr, yi, Precision);
	    break;
  }

  Diag->calcCoordinate(VarPos[0], yr, yi, &cx, &cy);

  QFontMetrics  metrics(QucsSettings.font);
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
void Marker::makeInvalid()
{
  cx = 0;
  cy = 0;
  if(Diag) if(Diag->Name != "Rect") {
    cx = Diag->x2 >> 1;
    cy = Diag->y2 >> 1;
  }
  Text = QObject::tr("invalid");

  QFontMetrics  metrics(QucsSettings.font);
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
bool Marker::moveLeftRight(bool left)
{
  int n;
  double *px;

  DataX *pD = pGraph->cPointsX.getFirst();
  px = pD->Points;
  if(!px) return false;
  for(n=0; n<pD->count; n++) {
    if(VarPos[0] <= *px) break;
    px++;
  }
  if(n == pD->count) px--;

  if(left) {
    if(px <= pD->Points) return false;
    px--;  // one position to the left
  }
  else {
    if(px >= (pD->Points + pD->count - 1)) return false;
    px++;  // one position to the right
  }
  VarPos[0] = *px;
  createText();

  return true;
}

// ---------------------------------------------------------------------
bool Marker::moveUpDown(bool up)
{
  int n, i=0;
  double *px;

  DataX *pD = pGraph->cPointsX.first();
  if(!pD) return false;

  if(up) {  // move upwards ? **********************
    do {
      i++;
      pD = pGraph->cPointsX.next();
      if(!pD) return false;
      px = pD->Points;
      if(!px) return false;
      for(n=1; n<pD->count; n++) {  // go through all data points
        if(fabs(VarPos[i]-(*px)) < fabs(VarPos[i]-(*(px+1)))) break;
        px++;
      }

    } while(px >= (pD->Points + pD->count - 1));  // go to next dimension ?

    px++;  // one position up
    VarPos[i] = *px;
    while(i > 1) {
      pD = pGraph->cPointsX.prev();
      i--;
      VarPos[i] = *(pD->Points);
    }
  }
  else {  // move downwards **********************
    do {
      i++;
      pD = pGraph->cPointsX.next();
      if(!pD) return false;
      px = pD->Points;
      if(!px) return false;
      for(n=0; n<pD->count; n++) {
        if(fabs(VarPos[i]-(*px)) < fabs(VarPos[i]-(*(px+1)))) break;
        px++;
      }

    } while(px <= pD->Points);  // go to next dimension ?

    px--;  // one position up
    VarPos[i] = *px;
    while(i > 1) {
      pD = pGraph->cPointsX.prev();
      i--;
      VarPos[i] = *(pD->Points + pD->count - 1);
    }
  }
  createText();

  return true;
}

// ---------------------------------------------------------------------
void Marker::paint(QPainter *p, int x0, int y0)
{
  p->setPen(QPen(QPen::darkMagenta,0));
  p->drawRect(x0+x1, y0-y1, x2, -y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1-y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1-y2);
  }

  p->setPen(QPen(QPen::black,1));
  p->drawText(x0+x1+3, y0-y1-y2+3, x2, y2, Qt::AlignAuto, Text);

  if(isSelected) {
    p->setPen(QPen(QPen::darkGray,3));
    p->drawRoundRect(x0+x1-2, y0-y1+2, x2+4, -y2-4);
  }
}

// ---------------------------------------------------------------------
void Marker::paintScheme(QPainter *p)
{
  int x0 = Diag->cx;
  int y0 = Diag->cy;
  p->drawRect(x0+x1, y0-y1, x2, -y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0-y1-y2);
  }
  else {
    if(cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2, y0-y1-y2);
  }
}

// ------------------------------------------------------------
void Marker::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  y1 -= y;
  }
  else {
    x1 = x;  y1 = y;
  }
}

// -------------------------------------------------------
void Marker::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(Diag) {
    _x1 = Diag->cx + x1;
    _y1 = Diag->cy - y1-y2;
    _x2 = Diag->cx + x1+x2;
    _y2 = Diag->cy - y1;
  }
  else {
    _x1 = x1;
    _y1 = y1+y2;
    _x2 = x1+x2;
    _y2 = y1;
  }
}

// ---------------------------------------------------------------------
QString Marker::save()
{
  QString s  = "<Mkr ";

  for(int i=0; i<nVarPos; i++)
    s += QString::number(VarPos[i])+"/";
  s.at(s.length()-1) = ' ';

  s += QString::number(x1) +" "+ QString::number(y1) +" "
      +QString::number(Precision) +" "+ QString::number(numMode) +">";
  return s;
}

// ---------------------------------------------------------------------
// All graphs must have been loaded before this function !
bool Marker::load(const QString& _s)
{
  bool ok;
  QString s = _s;

  if(s.at(0) != '<') return false;
  if(s.at(s.length()-1) != '>') return false;
  s = s.mid(1, s.length()-2);   // cut off start and end character

  if(s.section(' ',0,0) != "Mkr") return false;

  QString n;
  n  = s.section(' ',1,1);    // VarPos
  nVarPos = 0;
  int i=0, j;
  do {
    j = n.find('/', i);
    VarPos[nVarPos++] = n.mid(i,j-i).toDouble(&ok);
    if(!ok) return false;
    if(nVarPos > 255) return false;
    i = j+1;
  } while(j >= 0);

  n  = s.section(' ',2,2);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);      // Precision
  if(n.isEmpty()) return true;  //  is optional
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);      // numMode
  if(n.isEmpty()) return true;  //  is optional
  numMode = n.toInt(&ok);
  if(!ok) return false;

  return true;
}

// ------------------------------------------------------------------------
// Checks if the coordinates x/y point to the marker text. x/y are relative
// to diagram cx/cy.
int Marker::getSelected(int x_, int y_)
{
  if(x_ >= x1) if(x_ <= x1+x2) if(y_ >= y1) if(y_ <= y1+y2)
    return 1;

  return -1;
}

// ------------------------------------------------------------------------
Marker* Marker::sameNewOne(Graph *pGraph_)
{
  Marker *pm = new Marker(Diag, pGraph_, 0, cx ,cy);

  pm->x1 = x1;  pm->y1 = y1;
  pm->x2 = x2;  pm->y2 = y2;

  pm->nVarPos = nVarPos;
  for(int z=0; z<nVarPos; z++)
    pm->VarPos[z] = VarPos[z];

  pm->Text      = Text;
  pm->lookNfeel = lookNfeel;

  pm->Precision = Precision;
  pm->numMode   = numMode;

  return pm;
}
