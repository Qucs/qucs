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

/*!
  \class Marker
  \brief The Marker class implements the marker object used for all the
         diagram
*/

#include "marker.h"
#include "diagram.h"
#include "graph.h"
#include "main.h"

#include <QString>
#include <QPainter>
#include <QDebug>

#include <limits.h>
#include <cmath>
#include <stdlib.h>

#include "misc.h"

#define CHECK_MARKER \
  if(pGraph){ \
    assert(SplPosD>=pGraph->begin()); \
    assert(SplPosD<=pGraph->end()); \
    if(SplPosD!=pGraph->end()){ \
      assert(SplPosX>=pGraph->_begin()); \
      assert(SplPosX<=pGraph->_end()); \
      assert(SplPosD->begin()>=pGraph->_begin()); \
      assert(SplPosD->end()<=pGraph->_end()); \
      if(SplPosX!=SplPosD->end()){ \
	assert(SplPosX>=SplPosD->begin()); \
	assert(SplPosX<=SplPosD->end()); \
	assert(SplPosX->isPt()); \
      } \
    } \
  }

static double default_Z0=50;

/*!
 * copy a marker
 */
Marker::Marker(const Marker& m) :
    Element(m),
    pGraph(m.pGraph),
    VarPos(m.VarPos),
//    SplPosD(m.SplPosD),
//    SplPosX(m.SplPosX),
    Text(m.Text),
    transparent(m.transparent),
    Precision(m.Precision),
    numMode(m.numMode)
{
  qDebug() << "cloning Marker";
  SplPosD = pGraph->end();
}

/*!
 * create a marker based on click position and
 * the branch number.
 *
 * the click position is used to compute the marker position. currently, the
 * marker position is the sampling point closest to the click.
 *
 * redundant: why not pass a GraphDeque::MarkerPos?!
 */
Marker::Marker(GraphDeque::const_iterator const& pos,
               GraphDeque const *pg_,
               int cx_, int cy_) :
  Element(),
  pGraph(pg_),
  SplPosD(pos),
  Precision(3),
  numMode(0),
  Z0(default_Z0) // BUG: see declaration.
{
  Type = isMarker;
  isSelected = transparent = false;

  if(pGraph){ // tmp hack
    SplPosX = pGraph->_end();
  }

  cx =  cx_;
  cy = -cy_;
  fCX = float(cx);
  fCY = float(cy);

  //Default setting for displaying extra parameters. The markers will show the impedance data if the chart is type "Smith".
  //In the case of having an admittance chart, admittance will be display. 
  if (diag()->Name == "Smith") {
    optText = Marker::SHOW_Z;
  } else if (diag()->Name == "ySmith") {
    optText = Marker::SHOW_Y;
  } else {
    optText = 0;
  }

  if(!pGraph){
    qDebug() << "no Graph in Marker::Marker";
    makeInvalid();
  }else if(pGraph->begin()==pGraph->end()) {
    qDebug() << "no data in pGraph";
  }else{
    VarPos.resize(pGraph->numAxes());
    initText(pos);
    assert(pGraph->begin()<=pos);
    assert(pos<=pGraph->end());
    createText();
  }

  CHECK_MARKER

  x1 =  cx + 60;
  y1 = -cy - 60;

}

Marker::~Marker()
{
}

// ---------------------------------------------------------------------
/*!
 * compute VarPos from Graph instance and click position (cx, cy)
 * FIXME: (should this) use sample from findSample?
 */
void Marker::initText(GraphDeque::const_iterator const& pos)
{
  assert(pGraph);
  assert(pos>=pGraph->begin());
  assert(pos<=pGraph->end());
  Axis const *pa;
  assert(diag());
  if(pGraph->yAxisNo == 0)  pa = &(diag()->yAxis);
  else  pa = &(diag()->zAxis);
  SplPosD = pos;

  int x, y, d, dmin = INT_MAX;

  Graph::const_iterator here = pos->begin();

  assert(here>=pGraph->_begin());
  for(auto spl=pos->begin(); spl!=pos->end(); ++spl) {
    if(!spl->isPt()){
      qDebug() << "BUG in calcData? bogus data in spl";
      continue;
    }
    // diag()->calcCoordinate(px, pz, py, &fCX, &fCY, pa);
    fCX = spl->getScrX();
    fCY = spl->getScrY();

    x = int(fCX+0.5) - cx;
    y = int(fCY+0.5) - cy;
    d = x*x + y*y;
    if(d < dmin) {
      dmin = d;
      here = spl;
    }
  }

  SplPosX = here;

  assert(here<pos->end());
  assert(VarPos.size());
  VarPos[0] = here->getIndep();

  pGraph->samplePos(pos, VarPos); // FIXME: don't touch VarPos[0]

  if(VarPos[0] != here->getIndep()){
    qDebug() << "BUG" << VarPos[0] << here->getIndep();
  }
}

// ---------------------------------------------------------------------
/*!
 * (should)
 * create marker label Text the screen position cx and cy from VarPos.
 * does a lot of fancy stuff to be sorted out.
 */
void Marker::createText()
{
  assert(pGraph);
  if(!(pGraph->cPointsY)) {
    makeInvalid();
    return;
  }else if(SplPosD == pGraph->end()){ // here?!
    qDebug() << "invalid marker" << pGraph->numAxes();
    for(auto x : VarPos){
      qDebug() << x;
    }
    GraphDeque::MarkerPos P = pGraph->findSample(VarPos);
    SplPosD = P.first;
    SplPosX = P.second;
    VarDep[0] = SplPosX->getDep().real();
    VarDep[1] = SplPosX->getDep().imag();
  }


  if(splPos().second<pGraph->_begin()){
  }else if(splPos().second>=pGraph->_end()){
    qDebug() << "bogus marker end";
  }else{
  }

#if 0 // not yet. marker pos can be invalid here.
  assert(pGraph->begin()<=pGraph->end());
  assert(pGraph->begin()<=SplPosD);
  assert(SplPosD<=pGraph->end());
#endif

  unsigned nVarPos = VarPos.size();

  if(nVarPos > pGraph->numAxes()){
    qDebug() << "huh, VarPos too big?!";
  }
  if(nVarPos != pGraph->numAxes()){
    qDebug() << "padding" << VarPos.size() << pGraph->numAxes();
    VarPos.resize(pGraph->numAxes());
    while((unsigned int)nVarPos < pGraph->numAxes()){
      VarPos[nVarPos++] = 0.; // pad
    }
  }

  // independent variables
  nVarPos = pGraph->numAxes();

  // BUG? not necessarily needed here.
  auto p = pGraph->findSample(VarPos);
  SplPosD = p.first;
  SplPosX = p.second;

  CHECK_MARKER

  if(SplPosX->isPt()){
    VarDep[0] = p.second->getDep().real();
    VarDep[1] = p.second->getDep().imag();
  }

  assignText();
}

/*!
 * turn SplPosX, VarPos[1+] into Text
 * (recheck: use SplPos{X,Y} only?)
 */
void Marker::assignText()
{
  double v=0.;   // needed for 2D graph in 3D diagram
  double *py=&v;
  if(pGraph->axis(1)) {
    *py = VarPos[1];
  }else{
    qDebug() << *py << "is not" << VarPos[1]; // does it really matter?!
  }

  Text = "";
  assert(pGraph);
  if(pGraph->isEmpty()) {
    qDebug() << "empty";
    makeInvalid();
    return;
  }
  assert(pGraph->begin()<=SplPosD);
  assert(SplPosD<=pGraph->end());

  // why check over and over again?! do in the right place and just assert otherwise.
  if(VarPos.size() != pGraph->numAxes()){
    qDebug() << "huh, wrong size" << VarPos.size() << pGraph->numAxes();
    VarPos.resize(pGraph->numAxes());
  }

  for(unsigned i=0; i<pGraph->numAxes(); ++i) {
    Text += pGraph->axisName(i) + ": " + QString::number(VarPos[i],'g',Precision) + "\n";
  }


  double pz[2];
  if (SplPosX->isPt()){
    pz[0] = SplPosX->getDep().real();
    pz[1] = SplPosX->getDep().imag();

    Text += pGraph->Var + ": ";
    switch(numMode) {
      case nM_Rect: Text += misc::complexRect(*pz, *(pz+1), Precision);
		    break;
      case nM_Deg: Text += misc::complexDeg(*pz, *(pz+1), Precision);
		   break;
      case nM_Rad: Text += misc::complexRad(*pz, *(pz+1), Precision);
		   break;
    }
    assert(diag());
    Text += diag()->extraMarkerText(this);
  }else{
    Text+="NA"; // BUG?
    pz[0] = 1e-3;
    pz[1] = 1e-3;
  }

  Axis const *pa;
  if(pGraph->yAxisNo == 0)  pa = &(diag()->yAxis);
  else  pa = &(diag()->zAxis);
  double *pp = &(VarPos[0]);

  diag()->calcCoordinate(pp, pz, py, &fCX, &fCY, pa);
  diag()->finishMarkerCoordinates(fCX, fCY);

  cx = int(fCX+0.5);
  cy = int(fCY+0.5);
  getTextSize();
}

// ---------------------------------------------------------------------
/*!
 * retrieve sample marker position in terms of sampling iterator from marker
 */
GraphDeque::MarkerPos Marker::splPos() const
{
  return GraphDeque::MarkerPos(SplPosD, SplPosX);
}

// ---------------------------------------------------------------------
void Marker::makeInvalid()
{
  if(pGraph){
    qDebug() << "resetting SplPos";
    SplPosD = pGraph->end();
    assert(SplPosD>=pGraph->begin());
  }else{
    qDebug() << "theres no graph to invalidate marker, BUG?";
  }
  fCX = fCY = -1e3; // invalid coordinates
  assert(diag());
  diag()->finishMarkerCoordinates(fCX, fCY); // leave to diagram
  cx = int(fCX+0.5);
  cy = int(fCY+0.5);

  Text = QObject::tr("invalid");
  getTextSize();

  qDebug() << "marker is now invalid" << VarPos.size();
  if(pGraph){
    assert(SplPosD>=pGraph->begin());
    assert(SplPosD<=pGraph->end());
  }
}

// ---------------------------------------------------------------------
void Marker::getTextSize()
{
  // get size of text using the screen-compatible metric
  QFontMetrics metrics(QucsSettings.font, 0);
  QSize r = metrics.size(0, Text);
  x2 = r.width()+5;
  y2 = r.height()+5;
}

// ---------------------------------------------------------------------
bool Marker::moveLeftRight(bool left)
{
  assert(pGraph);
  assert(SplPosD>=pGraph->begin());
  assert(SplPosD<=pGraph->end());

  if(SplPosD==pGraph->end()) {
    // invalid marker
    return false;
  }else if(SplPosX==SplPosD->end()) {
    // oops?
    return false;
  }

  CHECK_MARKER

  if(left) {
    if(SplPosX == SplPosD->begin()) return false;
    if((SplPosX-1)->isStrokeEnd()) return false;
    --SplPosX;
    assert(!SplPosX->isStrokeEnd());
  } else {
    if(SplPosX == SplPosD->end()) return false; // reachable?
    if(SplPosX+1 == SplPosD->end()) return false;
    if(SplPosX->isStrokeEnd()) return false;
    ++SplPosX;
    assert(SplPosX != SplPosD->end());
  }
  VarPos[0] = SplPosX->getIndep();
  // other VarPos unchanged
  assignText();

  return true;
}

// ---------------------------------------------------------------------
bool Marker::moveUpDown(bool up)
{
  DataX const *pD = pGraph->axis(0);
  if(!pD) return false;
  assert(SplPosD>=pGraph->begin());
  assert(SplPosD<=pGraph->end());
  if(SplPosD==pGraph->end()){
    // is this reachable?
    return false;
  }

  unsigned X = SplPosX - SplPosD->begin();
  if(up) {
    if(SplPosD+1==pGraph->end()) return false;
    ++SplPosD;
  } else {  // down
    if(SplPosD==pGraph->begin()) return false;
    --SplPosD;
  }

  SplPosX = SplPosD->begin() + X; // BUG: needs rectangular grid.
  VarPos[0] = SplPosX->getIndep();
  pGraph->samplePos(SplPosD, VarPos);
  assignText();
  return true;
}

// ---------------------------------------------------------------------
void Marker::paint(ViewPainter *p, int x0, int y0)
{
  // keep track of painter state
  p->Painter->save();

  // Workaround for bug in Qt: If WorldMatrix is turned off, \n in the
  // text creates a terrible mess.
  p->Painter->setWorldMatrixEnabled(true);
  QMatrix wm = p->Painter->worldMatrix();
  p->Painter->setWorldMatrix(QMatrix());

  int x2_, y2_;
  p->Painter->setPen(QPen(Qt::black,1));
  x2_ = p->drawText(Text, x0+x1+3, y0+y1+3, &y2_);
  x2_ += int(6.0*p->Scale);
  y2_ += int(6.0*p->Scale);
  if(!transparent) {
    p->eraseRect(x0+x1, y0+y1, x2_, y2_);
    p->drawText(Text, x0+x1+3, y0+y1+3);
  }
  p->Painter->setWorldMatrix(wm);
  p->Painter->setWorldMatrixEnabled(false);

  // restore painter state
  p->Painter->restore();

  p->Painter->setPen(QPen(Qt::darkMagenta,0));
  p->drawRectD(x0+x1, y0+y1, x2_, y2_);

  x2 = int(float(x2_) / p->Scale);
  y2 = int(float(y2_) / p->Scale);

  int x1_, y1_;
  p->map(x0+x1, y0+y1, x1_, y1_);
  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(-cy >= y1+(y2>>1))
      y1_ += y2_ - 1;
  }
  else {
    x1_ += x2_ - 1;
    if(-cy >= y1+(y2>>1))
      y1_ += y2_ - 1;
  }
  float fx2, fy2;
  fx2 = (float(x0)+fCX)*p->Scale + p->DX;
  fy2 = (float(y0)-fCY)*p->Scale + p->DY;
  p->Painter->drawLine(x1_, y1_, TO_INT(fx2), TO_INT(fy2));

  if(isSelected) {
    p->Painter->setPen(QPen(Qt::darkGray,3));
    p->drawRoundRect(x0+x1-3, y0+y1-3, x2+6, y2+6);
  }
}

// ---------------------------------------------------------------------
void Marker::paintScheme(QPainter *p)
{
  assert(diag());
  int x0 = diag()->cx;
  int y0 = diag()->cy;
  p->drawRect(x0+x1, y0+y1, x2, y2);

  // which corner of rectangle should be connected to line ?
  if(cx < x1+(x2>>1)) {
    if(-cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1, y0+y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1, y0+y1+y2-1);
  }
  else {
    if(-cy < y1+(y2>>1))
      p->drawLine(x0+cx, y0-cy, x0+x1+x2-1, y0+y1);
    else
      p->drawLine(x0+cx, y0-cy, x0+x1+x2-1, y0+y1+y2-1);
  }
}

// ------------------------------------------------------------
void Marker::setCenter(int x, int y, bool relative)
{
  if(relative) {
    x1 += x;  y1 += y;
  }
  else {
    x1 = x;  y1 = y;
  }
}

// -------------------------------------------------------
void Marker::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{
  if(diag()) {
    _x1 = diag()->cx + x1;
    _y1 = diag()->cy + y1;
    _x2 = diag()->cx + x1+x2;
    _y2 = diag()->cy + y1+y2;
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

  for(auto i : VarPos){
    s += QString::number(i)+"/";
  }
  s.replace(s.length()-1,1,' ');
  //s.at(s.length()-1) = (const QChar&)' ';

  s += QString::number(x1) +" "+ QString::number(y1) +" "
      +QString::number(Precision) +" "+ QString::number(numMode);
  if(transparent)  s += " 1";
  else  s += " 0";

  if (diag()->Name.count("Smith"))//Impedance/admittance smith charts
  {
    s += " " + QString::number(optText);
  }

  s+=">"; 

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

  int i=0, j;
  QString n = s.section(' ',1,1);    // VarPos

  unsigned nVarPos = 0;
  j = (n.count('/') + 3);
  VarPos.resize(j);

  do {
    j = n.indexOf('/', i);
    VarPos[nVarPos++] = n.mid(i,j-i).toDouble(&ok);
    if(!ok) return false;
    i = j+1;
  } while(j >= 0);

  VarDep[1] = VarPos.back(); VarPos.pop_back();
  VarDep[0] = VarPos.back(); VarPos.pop_back();

  n  = s.section(' ',2,2);    // x1
  x1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',3,3);    // y1
  y1 = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',4,4);      // Precision
  Precision = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',5,5);      // numMode
  numMode = n.toInt(&ok);
  if(!ok) return false;

  n  = s.section(' ',6,6);      // transparent
  if(n.isEmpty()) return true;  // is optional
  if(n == "0")  transparent = false;
  else  transparent = true;

  // for Smith charts; optional parameters to be displayed
  n  = s.section(' ',7,7);
  if(n.isEmpty()) return true;  // backward compatibility
  int numOpt = n.toInt(&ok);
  if(!ok) return false;
  
  optText = static_cast<extraText>(numOpt);

  return true;
}

// ------------------------------------------------------------------------
// Checks if the coordinates x/y point to the marker text. x/y are relative
// to diagram cx/cy.
bool Marker::getSelected(int x_, int y_)
{
  if(x_ >= x1) if(x_ <= x1+x2) if(y_ >= y1) if(y_ <= y1+y2)
    return true;

  return false;
}

/* WAVE AC CODE
// ------------------------------------------------------------------------
// will find the y value of a point in time for waveac
double Marker::wavevalY(double xn,std::vector<double>& VarPos)  
{
  double n;
  double af=0.0; //angles
  double A = 0.0;
  double yp[2];

  n=VarPos[0];
  VarPos[0]= diag()->freq[0];
  auto p = pGraph->findSample(VarPos);
  yp[0]=p.first;
  yp[1]=p.second;

  af = atan2 (yp[1],yp[0]);
  A = sqrt(yp[1]*yp[1] +yp[0]*yp[0]);
  VarPos[0]=n;
  return A*sin(2*pi*(diag()->freq[0])*xn + af);
}*/


// ------------------------------------------------------------------------
/*
 * the diagram this belongs to
 */
const Diagram* Marker::diag() const
{
  if(!pGraph) return NULL;
  return pGraph->parentDiagram();
}

// ------------------------------------------------------------------------
Marker* Marker::sameNewOne(GraphDeque const *pGraph_)
{
  assert(pGraph_);
  CHECK_MARKER
  Marker *pm = new Marker(*this);
  pm->setGraph(pGraph_);
  return pm;
}
// ------------------------------------------------------------------------
QString Marker::unit(double n)
{
  QString value="";
  if(n < 1e-9)
  {
    n/= 1e-9;
    value.setNum(n);
    value+= " p";
  }
  else if(n < 1e-6)
  {
    n/= 1e-9;
    value.setNum(n);
    value+= " n";
  }
  else if(n < 1e-3)
  {
    n/= 1e-6;
    value.setNum(n);
    value+= " u";
  }
  else if(n < 1)
  {
    n/= 1e-3;
    value.setNum(n);
    value+= " m";
  }
  else
  {
    value.setNum(n);
  }
  return value;
}

// ------------------------------------------------------------------------
void Marker::setGraph(GraphDeque const *d)
{
  pGraph = d;
  assert(d);
  SplPosD = d->end();
//  SplPosX = invalid
}

// vim:ts=8:sw=2:noet
