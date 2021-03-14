/***************************************************************************
    copyright            : (C) 2003, 2004, 2005 by Michael Margraf
                               2020, 2021 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Diagram: Visualise data.
//
/*
  \todo Should probably use QFontMetrics::boundingRect(QString).width instead
        of QFontMetrics::width(QString), since only the first gives the actual
	width (see Qt docs)
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <stdlib.h>
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif
#include <locale.h>

#include "diagram.h"
#include "element_list.h"
#include "graph.h"
#include "misc.h"
#include "mnemo.h"
#include "platform.h"
#include "qio.h"
#include "qt_compat.h"
#include "qucs_app.h"
#include "some_font_stuff.h"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QRectF>
#include <QRegExp>
#include <QString>
#include <QTextStream>

namespace qucs {

Diagram::Diagram(Diagram const& p)
  : Element(p),
    x2(p.x2),
    y2(p.y2),
    Bounding_x1(p.Bounding_x1),
    Bounding_x2(p.Bounding_x2),
    Bounding_y1(p.Bounding_y1),
    Bounding_y2(p.Bounding_y2),
    Name(p.Name) // yikes.
{

#if 0
    Arcs.clear();
    for(auto p1 : p.Arcs) {untested();
	    Arcs.append(new Arc(*p1));
    }
    Lines.clear();
    for(auto p1 : p.Lines) {
	    Lines.append(new Line(*p1));
    }
#endif

	 new_subckt();
	 for (auto i : *p._subckt){
		 assert(i);
		 _subckt->push_back(i->clone());
	 }
}

Diagram::Diagram(int cx, int cy)
  : Element(cx, cy)
{
	new_subckt();
  
  // x1, x2, y1, y2 are the selectable boundings of the diagram, but these
  // are the real boundings. They are set in "createAxisLabels()".
  Bounding_x1 = Bounding_x2 = Bounding_y1 = Bounding_y2 = 0;

  xAxis.numGraphs = yAxis.numGraphs = zAxis.numGraphs = 0;
  xAxis.min = xAxis.low =
  yAxis.min = yAxis.low = zAxis.min = zAxis.low = 0.0;
  xAxis.max = xAxis.up =
  yAxis.max = yAxis.up = zAxis.max = zAxis.up = 1.0;
  xAxis.GridOn = yAxis.GridOn = true;
  zAxis.GridOn = false;
  xAxis.log = yAxis.log = zAxis.log = false;

  xAxis.limit_min = yAxis.limit_min = zAxis.limit_min = 0.0;
  xAxis.limit_max = yAxis.limit_max = zAxis.limit_max = 1.0;
  xAxis.step = yAxis.step = zAxis.step = 1.0;
  xAxis.autoScale = yAxis.autoScale = zAxis.autoScale = true;

  /* CODE RELATED TO THE PHASOR DIAGRAM
  //used in phasor diagram, scales for Volts, Ampere, Watts and Ohms
  xAxisI = xAxisV = xAxisP = xAxisZ = xAxis;
  yAxisI = yAxisV = yAxisP = yAxisZ = yAxis;
  zAxisI = zAxisV = zAxisP = zAxisZ = zAxis;*/

  rotX = 315;  // for 3D diagram
  rotY = 0;
  rotZ = 225;
  hideLines = true;  // hide invisible lines

//  Type = isDiagram;
  GridPen = QPen(Qt::lightGray,0);

  set_label("diag");
}

Diagram::~Diagram()
{
  if(freq!=nullptr) delete[] freq;
  freq= nullptr;

  delete _subckt;
}

index_t Diagram::param_count() const
{
  return 8; // yikes. too many.
}

void Diagram::set_param_by_index(index_t i, std::string const& v)
{ untested();

	pos_t p = position();
	int cx = getX(p);
	int cy = getY(p);
	bool ok = true;
	auto n = QString_(v);
	QColor co;
	char c;
	switch(i){
	case 0: untested();
		cx = n.toInt(&ok);
		break;
	case 1: untested();
		cy = n.toInt(&ok);
		break;
	case 2: untested();
		x2 = n.toInt(&ok);
		break;
	case 3: untested();
		y2 = n.toInt(&ok);
		break;
	case 4:
		c = n.at(0).toLatin1() - '0';
		xAxis.GridOn = yAxis.GridOn = (c & 1) != 0;
		hideLines = (c & 2) != 0;
		break;
	case 5:
		co.setNamedColor(n);
		GridPen.setColor(co);
		ok = GridPen.color().isValid();
		break;
	case 6:
		GridPen.setStyle((Qt::PenStyle)n.toInt(&ok));
		break;
	case 7:
		xAxis.log = n.at(0) != '0';
		c = n.at(1).toLatin1();
		yAxis.log = ((c - '0') & 1) == 1;
		zAxis.log = ((c - '0') & 2) == 2;
		break;
	}

	trace4("Diag param", cx, cy, x2, y2);

	if(!ok){
		throw qucs::Exception("bad value");
	}else{
	}

	setPosition(pos_t(cx, cy)); // yikes. setPosition?
}

void Diagram::paintDiagram(ViewPainter *p)
{itested();

#if 1
    // draws some box with corners to resize.
    // need a similar thing for scalable symbols.
    if(1) {itested();
      int x_, y_;
      float fx_, fy_;

      p->setPen(QPen(Qt::darkGray,3));
      p->drawRect(x_-5, y_-5, TO_INT(fx_), TO_INT(fy_));
      p->setPen(QPen(Qt::darkRed,2));
      p->drawResizeRect(0, y2);  // markers for changing the size
      p->drawResizeRect(0, 0);
      p->drawResizeRect(x2, y2);
      p->drawResizeRect(x2, 0 );
    }
#endif
}

void Diagram::new_subckt()
{
	assert(!_subckt);
	_subckt = new ElementList();
}

void Diagram::paintMarkers(ViewPainter *, bool paintAll)
{itested();
  incomplete();
#if 0
    // draw markers last, so they are at the top of painting layers
    foreach(Graph *pg, Graphs)
      foreach(Marker *pm, pg->Markers)
          if ((pm->Type & 1)||paintAll) pm->paint(p, cx(), cy());
#endif
}

// ------------------------------------------------------------

/*!
   Put axis labels into the text list.
*/
void Diagram::createAxisLabels()
{itested();
  int   x, y, w, wmax = 0;
  QString Str;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  int LineSpacing = metrics.lineSpacing();

  nfreqa=0;
  x = (x2>>1);
  y = -y1;
  if(xAxis.Label.isEmpty()) {itested();
    // write all x labels ----------------------------------------
#if 0
    foreach(Graph *pg, Graphs) {itested();
	  DataX const *pD = pg->axis(0);
	  if(!pD) continue;
	  y -= LineSpacing;
      if(Name[0] != 'C') {   // locus curve ?
	    w = metrics.width(pD->Var) >> 1;
	    if(w > wmax)  wmax = w;
	    Texts.append(new Text(x-w, y, pD->Var, pg->Color, 12.0));
	  }
	  if(Name[0] == 'C') { untested();
  	    w = metrics.width("real("+pg->Var+")") >> 1;
	    if(w > wmax)  wmax = w;
  	    Texts.append(new Text(x-w, y, "real("+pg->Var+")",
                                pg->Color, 12.0));
	  }else{
	  }
    }
#endif
  } else {itested();
    y -= LineSpacing;
    encode_String(xAxis.Label, Str);
    w = metrics.width(Str) >> 1;
    if(w > wmax)  wmax = w;
    // Texts.append(new Text(x-w, y, Str, Qt::black, 12.0));
  }
  Bounding_y2 = 0;
  Bounding_y1 = y - LineSpacing;
  Bounding_x2 = wmax - (x2 >> 1);
  if(Bounding_x2 < 0) Bounding_x2 = 0;
  Bounding_x1 = Bounding_x2;


  wmax = 0;
  x = -x1;
  y = y2>>1;
  if(yAxis.Label.isEmpty()) {itested();
    // draw left y-label for all graphs ------------------------------
#if 0
    foreach(Graph *pg, Graphs) {itested();
      if(pg->yAxisNo != 0)  continue;
      if(pg->cPointsY) {itested();
	if(Name[0] != 'C') {   // location curve ?
          w = metrics.width(pg->Var) >> 1;
          if(w > wmax)  wmax = w;
          Texts.append(new Text(x, y-w, pg->Var, pg->Color, 12.0, 0.0, 1.0));
	}else{ untested();
          w = metrics.width("imag("+pg->Var+")") >> 1;
          if(w > wmax)  wmax = w;
          Texts.append(new Text(x, y-w, "imag("+pg->Var+")",
                                pg->Color, 12.0, 0.0, 1.0));
	}
      }else{     // if no data => <invalid>
        w = metrics.width(pg->Var+INVALID_STR) >> 1;
        if(w > wmax)  wmax = w;
        Texts.append(new Text(x, y-w, pg->Var+INVALID_STR,
                              pg->Color, 12.0, 0.0, 1.0));
      }
      x -= LineSpacing;
    }
#endif
  }else{ untested();
    encode_String(yAxis.Label, Str);
    w = metrics.width(Str) >> 1;
    if(w > wmax)  wmax = w;
    // Texts.append(new Text(x, y-w, Str, Qt::black, 12.0, 0.0, 1.0));
    x -= LineSpacing;
  }
  if(Bounding_x1 < -x) Bounding_x1 = -x;


  x = x3;
  y = y2>>1;
  if(zAxis.Label.isEmpty()) {itested();
    // draw right y-label for all graphs ------------------------------
#if 0
    foreach(Graph *pg, Graphs) {itested();
      if(pg->yAxisNo != 1)  continue;
      if(pg->cPointsY) {itested();
	if(Name[0] != 'C') {   // location curve ?
          w = metrics.width(pg->Var) >> 1;
          if(w > wmax)  wmax = w;
          Texts.append(new Text(x, y+w, pg->Var,
                                pg->Color, 12.0, 0.0, -1.0));
	}
	else { untested();
          w = metrics.width("imag("+pg->Var+")") >> 1;
          if(w > wmax)  wmax = w;
          Texts.append(new Text(x, y+w, "imag("+pg->Var+")",
                                pg->Color, 12.0, 0.0, -1.0));
	}
      }
      else {     // if no data => <invalid>
        w = metrics.width(pg->Var+INVALID_STR) >> 1;
        if(w > wmax)  wmax = w;
        Texts.append(new Text(x, y+w, pg->Var+INVALID_STR,
                              pg->Color, 12.0, 0.0, -1.0));
      }
      x += LineSpacing;
    }
#endif
  }else{ untested();
    encode_String(zAxis.Label, Str);
    w = metrics.width(Str) >> 1;
    if(w > wmax)  wmax = w;
    // Texts.append(new Text(x, y+w, Str, Qt::black, 12.0, 0.0, -1.0));
  }
  x -= x2;
  if(Bounding_x2 < x){
    Bounding_x2 = x;
  }else{
  }

  wmax -= y2 >> 1;
  if(wmax > 0) { untested();
    Bounding_y2 = wmax;
    wmax *= -1;
    if(wmax < Bounding_y1) Bounding_y1 = wmax;
  }else{
  }
}

// ------------------------------------------------------------
int Diagram::regionCode(float x, float y) const
{ untested();
  int code=0;   // code for clipping
  if(x < 0.0)
    code |= 1;
  else if(x > float(x2))  // compare as float to avoid integer overflow
    code |= 2;

  if(y < 0.0)
    code |= 4;
  else if(y > float(y2))  // compare as float to avoid integer overflow
    code |= 8;

  return code;
}

// ------------------------------------------------------------
// Is virtual. This one is for round diagrams only.
bool Diagram::insideDiagram(float x, float y) const
{ untested();
  float R = x2/2.0;
  x -= R;
  y -= R;
  R += 1.0; // +1 seems better ? (allow graph to go a little outside)
  //qDebug() << "insideDiagram" << x << y << R << (x*x + y*y- R*R);
  return ((x*x + y*y) <= R*R);
}

#if 0
// (try to) set a Marker to a diagram
Marker* Diagram::setMarker(int x, int y)
{ untested();
  if(getSelected(x, y)) { untested();
    // test all graphs of the diagram
    foreach(Graph *pg, Graphs) { untested();
      int n  = pg->getSelected(x-cx(), cy()-y); // sic!
      if(n >= 0) { untested();
	assert(pg->parentDiagram() == this);
	Marker *pm = new Marker(pg, n, x-cx(), y-cy());
	pg->Markers.append(pm);
	return pm;
      }
    }
  }
  return NULL;
}
#endif

// Cohen-Sutherland clipping algorithm
// possibly better to leave this to Qt.
// the real issue is resampling, not clipping.
void Diagram::rectClip(GraphIterator &p) const
{ untested();
  int code, z=0;
  float x=0, y=0, dx, dy;
  float x_1 = (p-2)->getScrX(), y_1 = (p-2)->getScrY();
  float x_2 = (p-1)->getScrX(), y_2 = (p-1)->getScrY();

  int code1 = regionCode(x_1, y_1);
  int code2 = regionCode(x_2, y_2);
  if((code1 | code2) == 0)  return;  // line completly inside ?

  if(code1 == 0){
  }else if((p-3)->isPt()) { untested();
    p++;
    (p-3)->setStrokeEnd();
  }else{
  }
  if(code1 & code2)   // line not visible at all ?
    goto endWithHidden;

  if(code2 != 0) { untested();
    p->setStrokeEnd();
    (p+1)->setScr(x_2, y_2);
    z += 2;
  }else{
  }


  for(;;) { untested();
    if((code1 | code2) == 0) break;  // line completly inside ?

    if(code1)  code = code1;
    else  code = code2;

    dx = x_2 - x_1;  // dx and dy never equals zero !
    dy = y_2 - y_1;
    if(code & 1) { untested();
      y = y_1 - dy * x_1 / dx;
      x = 0.0;
    } else if(code & 2) { untested();
      y = y_1 + dy * (x2-x_1) / dx;
      x = float(x2);
    } else if(code & 4) { untested();
      x = x_1 - dx * y_1 / dy;
      y = 0.0;
    } else if(code & 8) { untested();
      x = x_1 + dx * (y2-y_1) / dy;
      y = float(y2);
    }else{
    }

    if(code == code1) { untested();
      x_1 = x;
      y_1 = y;
      code1 = regionCode(x, y);
    } else { untested();
      x_2 = x;
      y_2 = y;
      code2 = regionCode(x, y);
    }
    if(code1 & code2){
      goto endWithHidden; // line not visible at all ?
    }else{
    }
  }

  (p-2)->setScr(x_1, y_1);
  (p-1)->setScr(x_2, y_2);
  p += z;
  return;

endWithHidden:
    (p-2)->setScr(x_2, y_2);
    p -= 1;
}

/*!
   Clipping for round diagrams (smith, polar, ...)
*/
void Diagram::clip(Graph::iterator &p) const
{ untested();
  float R = float(x2) / 2.0;
  float x_1 = (p-2)->getScrX() - R, y_1 = (p-2)->getScrY() - R;
  float x_2 = (p-1)->getScrX() - R, y_2 = (p-1)->getScrY() - R;

  float dt1 = R*R;   // square of radius
  float dt2 = dt1 - x_2*x_2 - y_2*y_2;
  dt1 -= x_1*x_1 + y_1*y_1;

  if(dt1 >= 0.0) if(dt2 >= 0.0)  return;  // line completly inside ?

  if(dt1 < 0.0) if((p-3)->isPt()) { // is there already a line end flag ?
    p++;
    (p-3)->setStrokeEnd();
  }

  float x = x_1-x_2;
  float y = y_1-y_2;
  float C = x_1*x + y_1*y;
  float D = x*x + y*y;
  float F = C*C + dt1*D;

  x_1 += R;
  y_1 += R;
  x_2 += R;
  y_2 += R;
  if(F <= 0.0) {   // line not visible at all ?
    (p-2)->setScr(x_2, y_2);
    p -= 1;
    return;
  }

  int code = 0;
  R   = sqrt(F);
  dt1 = C - R;
  if((dt1 > 0.0) && (dt1 < D)) { // intersection outside start/end point ?
    (p-2)->setScr(x_1 - x*dt1 / D, y_1 - y*dt1 / D);
    code |= 1;
  }
  else { untested();
    (p-2)->setScr(x_1, y_1);
  }

  dt2 = C + R;
  if((dt2 > 0.0) && (dt2 < D)) { // intersection outside start/end point ?
    (p-1)->setScr(x_1 - x*dt2 / D, y_1 - y*dt2 / D);
    p->setStrokeEnd();
    p += 2;
    code |= 2;
  }
  (p-1)->setScr(x_2, y_2);

  if(code == 0) {   // intersections both lie outside ?
    (p-2)->setScr(x_2, y_2);
    --p;
  }

}


// ------------------------------------------------------------
// g->Points must already be empty!!!
// is this a Graph Member?
void Diagram::calcData(Graph *)
{itested();
#if 0
  double *px;
  double *pz = g->cPointsY;
  if(!pz)  return;
  if(g->numAxes() < 1) return;

  int i, z, Counter=2;
  int Size = ((2*(g->count(0)) + 1) * g->countY) + 10;
  
  if(xAxis.autoScale)  if(yAxis.autoScale)  if(zAxis.autoScale)
    Counter = -50000;

  double Dummy = 0.0;  // not used
  double *py = &Dummy;

  g->resizeScrPoints(Size);
  auto p = g->begin();
  auto p_end = g->begin();
  p_end += Size - 9;   // limit of buffer
  p->setStrokeEnd();
  ++p;
  assert(p!=g->end());

  Axis *pa;
  if(g->yAxisNo == 0)  pa = &yAxis;
  else  pa = &zAxis;

  switch(g->Style) {
    case GRAPHSTYLE_SOLID: // ***** solid line ****************************
    case GRAPHSTYLE_DASH:
    case GRAPHSTYLE_DOT:
    case GRAPHSTYLE_LONGDASH:

      for(i=g->countY; i>0; i--) {  // every branch of curves
	px = g->axis(0)->Points;
	calcCoordinateP(px, pz, py, p, pa);
	++px;
	pz += 2;
	++p;
	for(z=g->axis(0)->count-1; z>0; z--) {  // every point
	  FIT_MEMORY_SIZE;  // need to enlarge memory block ?
	  calcCoordinateP(px, pz, py, p, pa);
	  ++px;
	  pz += 2;
	  ++p;
	  if(Counter >= 2)   // clipping only if an axis is manual
	    clip(p);
	}
	if((p-3)->isStrokeEnd() && !(p-3)->isBranchEnd())
	  p -= 3;  // no single point after "no stroke"
	else if((p-2)->isBranchEnd() && !(p-1)->isGraphEnd()) { untested();
	  if((!(p-1)->isPt()))
	    --p; // erase last hidden point
	}
	(p++)->setBranchEnd();
      }

      p->setGraphEnd();
/*z = p-g->Points+1;
p = g->Points;
qDebug("\n****** p=%p", p);
for(int zz=0; zz<z; zz+=2)
  qDebug("c: %d/%d", *(p+zz), *(p+zz+1));*/
      return;

    default:  // symbol (e.g. star) at each point **********************
      for(i=g->countY; i>0; i--) {  // every branch of curves
        px = g->axis(0)->Points;
        for(z=g->axis(0)->count; z>0; z--) {  // every point
          calcCoordinateP(px, pz, py, p, pa);
          ++px;
          pz += 2;
          if(insideDiagramP(p))    // within diagram ?
            ++p;
        }
	(p++)->setBranchEnd();
	assert(p!=g->end());
      }
      (p++)->setGraphEnd();
/*qDebug("\n******");
for(int zz=0; zz<60; zz+=2)
  qDebug("c: %d/%d", *(g->Points+zz), *(g->Points+zz+1));*/
      return;
  }

  // unreachable
#endif
}


// -------------------------------------------------------
// doesn't seem to be the bounding box
void Diagram::Bounding(int& _x1, int& _y1, int& _x2, int& _y2)
{itested();
  _x1 =  - Bounding_x1;
  _y1 =  - y2 - Bounding_y2;
  _x2 =  + x2 + Bounding_x2;
  _y2 =  - Bounding_y1;
  trace5("bounding diag", label(), _x1, _x2, _y1, _y2);
}

// -------------------------------------------------------
QRectF Diagram::boundingRect() const
{itested();
  int x1_, y1_, x2_, y2_;

  Diagram* d=const_cast<Diagram*>(this);
  d->Bounding(x1_, y1_, x2_, y2_);

  QPointF tl(0, -y2);
  QPointF br(x2, 0);
  return QRectF(tl, br);
}

// Check if the resize area was clicked. If so return "true" and sets
// x1/y1 and x2/y2 to the border coordinates to draw a rectangle.
bool Diagram::resizeTouched(float fX, float fY, float len)
{ untested();
  float fCX = float(cx()), fCY = float(cy());
  float fX2 = float(cx()+x2), fY2 = float(cy()-y2);
  if(fX < fCX-len) return false;
  if(fX > fX2+len) return false;
  if(fY < fY2-len) return false;
  if(fY > fCY+len) return false;

  State = 0;
  if(fX < fCX+len) State = 1;
  else  if(fX <= fX2-len) return false;
  if(fY > fCY-len) State |= 2;
  else  if(fY >= fY2+len) return false;

  return true;
}

// --------------------------------------------------------------------------

// --------------------------------------------------------------------------
#if 0
void Diagram::loadGraphData(const QString& defaultDataSet)
{itested();
  int yNum = yAxis.numGraphs;
  int zNum = zAxis.numGraphs;
  yAxis.numGraphs = zAxis.numGraphs = 0;

  double xmin = xAxis.min, ymin = yAxis.min, zmin = zAxis.min;
  double xmax = xAxis.max, ymax = yAxis.max, zmax = zAxis.max;
  yAxis.min = zAxis.min = xAxis.min =  DBL_MAX;
  yAxis.max = zAxis.max = xAxis.max = -DBL_MAX;

  int No=0;
#if 0
  foreach(Graph *pg, Graphs) {itested();
    qDebug() << "load GraphData load" << defaultDataSet << pg->Var;
    if(pg->loadDatFile(defaultDataSet) != 1)   // load data, determine max/min values
      No++;
    getAxisLimits(pg);
  }
#endif

  if(No <= 0) {   // All dataset files unchanged ?
    yAxis.numGraphs = yNum;  // rebuild scrollbar position
    zAxis.numGraphs = zNum;

    xAxis.min = xmin; yAxis.min = ymin; zAxis.min = zmin;
    xAxis.max = xmax; yAxis.max = ymax; zAxis.max = zmax;
    return;    // -> no update neccessary
  }

  if(xAxis.min > xAxis.max)
    xAxis.min = xAxis.max = 0.0;
  if(yAxis.min > yAxis.max)
    yAxis.min = yAxis.max = 0.0;
  if(zAxis.min > zAxis.max) 
    zAxis.min = zAxis.max = 0.0;

/*  if((Name == "Polar") || (Name == "Smith")) {  // one axis only
    if(yAxis.min > zAxis.min)  yAxis.min = zAxis.min;
    if(yAxis.max < zAxis.max)  yAxis.max = zAxis.max;
  }*/
  updateGraphData();
}
#endif


// ------------------------------------------------------------------------

// --------------------------------------------------------------------------
/*!
 * does not (yet) load a dat file. only part of it.
 * this way, it would belong to graph.cpp. but it's too obsolete, lets see..
 *
 * FIXME: must invalidate markers.
 */
// obsolete.
#if 0
int Graph::loadDatFile(const QString&)
{itested();
  Graph* g = this;
  QFile file;
  QString Variable;
  QFileInfo Info(fileName);

  int pos = g->Var.indexOf(':');
//  if(g->Var.right(3) == "].X")  // e.g. stdl[8:0].X
//    if(pos > g->Var.indexOf('['))
//      pos = -1;

  /* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
     to change the locale to the default. */
  setlocale (LC_NUMERIC, "C");

  if(pos <= 0) {itested();
    file.setFileName(fileName);
    Variable = g->Var;
  }
  else { untested();
    file.setFileName(Info.path()+QDir::separator() + g->Var.left(pos)+".dat");
    Variable = g->Var.mid(pos+1);
  }

  Info.setFile(file);
  if(g->lastLoaded.isValid())
    if(g->lastLoaded > Info.lastModified())
      return 1;    // dataset unchanged -> no update neccessary

  g->countY = 0;
  g->mutable_axes().clear(); // HACK
  if(g->cPointsY) { delete[] g->cPointsY;  g->cPointsY = 0; }
  if(Variable.isEmpty()) return 0;

#if 0 // FIXME encapsulation. implement digital waves later.
  if(Variable.right(2) == ".X")
    if(Name.at(0) != 'T')
      return 0;  // digital variables only for tabulars and ziming diagram
#endif


  if(!file.open(QIODevice::ReadOnly))  return 0;

  // *****************************************************************
  // To strongly speed up the file read operation the whole file is
  // read into the memory in one piece.
  QByteArray FileContent;
  FileContent = file.readAll();
  file.close();
  char *FileString = FileContent.data();
  if(!FileString)  return 0;
  char *pPos = FileString+FileContent.size()-1;
  if(*pPos > ' ')  if(*pPos != '>')  return 0;
  *pPos = 0;


  // *****************************************************************
  // look for variable name in data file  ****************************
  bool isIndep = false;
  Variable = "dep "+Variable+" ";
  // "pFile" is used through-out the whole function and must NOT used
  // for other purposes!
  char *pFile = strstr(FileString, Variable.toLatin1());
  while(pFile) {itested();
    if(*(pFile-1) == '<')     // is dependent variable ?
      break;
    else if(strncmp(pFile-3, "<in", 3) == 0) {  // is independent variable ?
      isIndep = true;
      break;
    }
    pFile = strstr(pFile+4, Variable.toLatin1());
  }

  if(!pFile)  return 0;   // data not found

  QString Line, tmp;
  pFile += Variable.length();
  pPos = strchr(pFile, '>');
  if(!pPos)  return 0;   // file corrupt
  *pPos = 0;
  Line = QString(pFile);
  *pPos = '>';
  pFile = pPos+1;
  if(!isIndep) {itested();
    pos = 0;
    tmp = Line.section(' ', pos, pos);
    while(!tmp.isEmpty()) {itested();
      g->mutable_axes().push_back(new DataX(tmp));  // name of independet variable
      pos++;
      tmp = Line.section(' ', pos, pos);
    }
  }

  // *****************************************************************
  // get independent variable ****************************************
  bool ok=true;
  double *p;
  int counting = 0;
  if(isIndep) {    // create independent variable by myself ?
    counting = Line.toInt(&ok);  // get number of values
    g->mutable_axes().push_back(new DataX("number", 0, counting));
    if(!ok)  return 0;

    p = new double[counting];  // memory of new independent variable
    g->countY = 1;
    g->mutable_axes().back()->Points = p;
    for(int z=1; z<=counting; z++)  *(p++) = double(z);
    auto Axis = g->mutable_axes().back();
    Axis->min(1.);
    Axis->max(double(counting));
  }
  else {  // ...................................
    // get independent variables from data file
    g->countY = 1;
#if 0 // FIXME: we do not have a Name.
    DataX *bLast = 0;
    if(Name == "Rect3D")  bLast = g->axis(1);  // y axis for Rect3D
#endif

#if 0 // FIXME: this is about diagram. do after load.
    double min_tmp = xAxis.min, max_tmp = xAxis.max;
#endif
    DataX const *pD;
    for(int ii= g->numAxes(); (pD = g->axis(--ii)); ) {itested();
#if 0 // FIXME: this is about diagram. do after load.
      pa = &xAxis;
      if(pD == g->axis(0)) { untested();
        xAxis.min = min_tmp;    // only count first independent variable
        xAxis.max = max_tmp;
      }
      else if(pD == bLast)  pa = &yAxis;   // y axis for Rect3D
#endif
      counting = loadIndepVarData(pD->Var, FileString, mutable_axis(ii));
      if(counting <= 0)  return 0;

      g->countY *= counting;
    }
    g->countY /= counting;
  }


  // *****************************************************************
  // get dependent variables *****************************************
  counting  *= g->countY;
  p = new double[2*counting]; // memory for dependent variables
  g->cPointsY = p;
#if 0 // FIXME: what does this do?!
  if(g->yAxisNo == 0)  pa = &yAxis;   // for which axis
  else  pa = &zAxis;
  (pa->numGraphs)++;    // count graphs
#endif

  char *pEnd;
  double x, y;
  pPos = pFile;

if(Variable.right(3) != ".X ") { // not "digital"

  for(int z=counting; z>0; z--) {itested();
    pEnd = 0;
    while((*pPos) && (*pPos <= ' '))  pPos++; // find start of next number
    x = strtod(pPos, &pEnd);  // real part
    pPos = pEnd + 1;
    if(*pEnd < ' ')   // is there an imaginary part ?
      y = 0.0;
    else { untested();
      if(((*pEnd != '+') && (*pEnd != '-')) || (*pPos != 'j')) { untested();
        delete[] g->cPointsY;  g->cPointsY = 0;
        return 0;
      }
      *pPos = *pEnd;  // overwrite 'j' with sign
      pEnd = 0;
      y = strtod(pPos, &pEnd); // imaginary part
      *pPos = 'j';   // write back old character
      pPos = pEnd;
    }
    *(p++) = x;
    *(p++) = y;
#if 0 // FIXME there is no Name here.
    if(Name[0] != 'C')
#endif
	 {itested();
      if(fabs(y) >= 1e-250) x = sqrt(x*x+y*y);
      if(std::isfinite(x)) {itested();
			auto Axis = g->mutable_axes().back();
			Axis->min(x);
			Axis->max(x);
      }
    }

#if 0 // this is not location curce code.
    else {   // location curve needs different treatment
      if(std::isfinite(x)) { untested();
        if(x > xAxis.max) xAxis.max = x;
        if(x < xAxis.min) xAxis.min = x;
      }
      if(std::isfinite(y)) { untested();
        if(y > pa->max) pa->max = y;
        if(y < pa->min) pa->min = y;
      }
    }
#endif
  }

} else {  // of "if not digital"

  char *pc = (char*)p;
  pEnd = pc + 2*(counting-1)*sizeof(double);
  // for digital variables (e.g. 100ZX0):
  for(int z=counting; z>0; z--) { untested();

    while((*pPos) && (*pPos <= ' '))  pPos++; // find start of next bit vector
    if(*pPos == 0) { untested();
      delete[] g->cPointsY;  g->cPointsY = 0;
      return 0;
    }

    while(*pPos > ' ') {    // copy bit vector
      *(pc++) = *(pPos++);
      if(pEnd <= pc) { untested();
        counting = pc - (char*)g->cPointsY;
        pc = (char*)realloc(g->cPointsY, counting+1024);
        pEnd = pc;
        g->cPointsY = (double*)pEnd;
        pc += counting;
        pEnd += counting+1020;
      }
    }
    *(pc++) = 0;   // terminate each vector with NULL
  }

}  // of "if not digital"

  lastLoaded = QDateTime::currentDateTime();
  return 2;
  return 0;
}
#endif

#if 0 // obsolete
int Graph::loadIndepVarData(const QString& Variable,
			      char *FileString, DataX* pD)
{itested();
  bool isIndep = false;
  QString Line, tmp;

  /* WORK-AROUND: A bug in SCIM (libscim) which Qt is linked to causes
     to change the locale to the default. */
  setlocale (LC_NUMERIC, "C");

  Line = "dep "+Variable+" ";
  // "pFile" is used through-out the whole function and must NOT used
  // for other purposes!
  char *pFile = strstr(FileString, Line.toLatin1());
  while(pFile) {itested();
    if(*(pFile-1) == '<')     // is dependent variable ?
      break;
    else if(strncmp(pFile-3, "<in", 3) == 0) {  // is independent variable ?
      isIndep = true;
      break;
    }
    pFile = strstr(pFile+4, Line.toLatin1());
  }

  if(!pFile)  return -1;   // data not found

  pFile += Line.length();
  char *pPos = strchr(pFile, '>');
  if(!pPos)  return -1;   // file corrupt
  *pPos = 0;
  Line = QString(pFile);
  *pPos = '>';
  pFile = pPos+1;
  char *pEnd;
  if(!isIndep) {           // dependent variable can also be used...
    if(Line.indexOf(' ') >= 0)  return -1; // ...if only one dependency
    Line = "<indep "+Line+" ";
    pPos = strstr(FileString, Line.toLatin1());
    if(!pPos)  return -1;
    pPos += Line.length();
    pEnd = strchr(pPos, '>');
    if(!pEnd)  return -1;   // file corrupt
    *pEnd = 0;
    Line = QString(pPos);
    *pEnd = '>';
  }


  bool ok;
  int n = Line.toInt(&ok);  // number of values
  if(!ok)  return -1;

  double *p = new double[n];     // memory for new independent variable
//  DataX *pD = pg->mutable_axes().back();
  pD->Points = p;
  pD->count  = n;


  double x;
  pPos = pFile;
  // find first position containing no whitespace
  while((*pPos) && (*pPos <= ' '))  pPos++;

  for(int z=0; z<n; z++) {itested();
    pEnd = 0;
    x = strtod(pPos, &pEnd);  // real part
    if(pPos == pEnd) { untested();
      delete[] pD->Points;  pD->Points = 0;
      return -1;
    }
    
    *(p++) = x;
#if 0 // this is not location curve code
    if(Name[0] != 'C')   // not for location curves
      if(std::isfinite(x)) { untested();
        if(x > pa->max) pa->max = x;
        if(x < pa->min) pa->min = x;
      }
#endif
    
    pPos = pEnd;
    while((*pPos) && (*pPos <= ' '))  pPos++;  // find start of next number
  }

  return n;   // return number of independent data
}
#endif

/*!
   Checks if the two graphs have the same independent variables.
*/
bool Diagram::sameDependencies(Graph const*, Graph const*) const
{ untested();
  incomplete();
#if 0
  // FIXME
  // return g1->same(*g2);
  if(g1 == g2)  return true;
  if(g1->numAxes()!=g2->numAxes()) return false;

  for(unsigned i=0; i<g1->numAxes(); ++i) { untested();
    if(g1->axisName(i) != g2->axisName(i))  return false;
  }

  return true;
#endif
  return false;
}

// ------------------------------------------------------------

// ------------------------------------------------------------
//void Diagram::setCenter(int x, int y, bool relative)
//{
//  if(relative) { untested();
//    _cx += x;  _cy += y;
//  } else {
//    _cx = x;  _cy = y;
//  }
//}

// -------------------------------------------------------
// override Element::center... not needed?
//pos_t Diagram::center() const
//{ untested();
//  return pos_t(_cx, _cy);
//}

// -------------------------------------------------------
//void Diagram::getCenter(int& x, int& y)
//{ untested();
//
//  trace3("diag getcenter", label(), _cx, _cy);
//  assert(false);
//  //????
//  x = _cx;
//  y = _cy;
//}

// ------------------------------------------------------------
// Object* Diagram::newOne() const
// { untested();
//   unreachable();
//   return new Diagram();
// }

// ------------------------------------------------------------
void Diagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{ untested();
  if(!insideDiagram(fCX, fCY)) { untested();
      fCX = float(x2 >> 1);
      fCY = float(y2 >> 1);
  }
}

// ------------------------------------------------------------
// BUG
QString Diagram::save() const
{
  QString s = "<"+Name+" "+QString::number(cx())+" "+QString::number(cy())+" ";
  s += QString::number(x2)+" "+QString::number(y2)+" ";
  char c = '0';
  if(xAxis.GridOn){
	  c |= 1;
  }else{
  }
  if(hideLines) {
	  c |= 2;
  }else{
  }
  s += c;
  s += " " + GridPen.color().name() + " " + QString::number(GridPen.style());

  if(xAxis.log){
	  s += " 1";
  }else{
	  s += " 0";
  }

  c = '0';
  if(yAxis.log){
	  c |= 1;
  }else{
  }
  if(zAxis.log) {
	  c |= 2;
  }
  s += c;

  if(xAxis.autoScale){
	  s += " 1 ";
  }else{
	  s += " 0 ";
  }

  s += QString::number(xAxis.limit_min) + " ";
  s += QString::number(xAxis.step) + " ";
  s += QString::number(xAxis.limit_max);
  if(yAxis.autoScale){
	  s+= " 1 ";
  }else{
	  s+= " 0 ";
  }
  s += QString::number(yAxis.limit_min) + " ";
  s += QString::number(yAxis.step) + " ";
  s += QString::number(yAxis.limit_max);
  if(zAxis.autoScale){
	  s+= " 1 ";
  }else{
	  s+= " 0 ";
  }
  s += QString::number(zAxis.limit_min) + " ";
  s += QString::number(zAxis.step) + " ";
  s += QString::number(zAxis.limit_max) + " ";

  s += QString::number(rotX)+" "+QString::number(rotY)+" "+
       QString::number(rotZ);

  // labels can contain spaces -> must be last items in the line
  s += " \""+xAxis.Label+"\" \""+yAxis.Label+"\" \""+zAxis.Label+"\" \""+sfreq+"\">\n";

  return s;

#if 0
  foreach(Graph *pg, Graphs)
    s += pg->save()+"\n";
#else
    s += "legacy. does not work\n";
#endif

  s += "  </"+Name+">";
  return s;
}

// ------------------------------------------------------------
// BUG: lang_sch. MOVE to legacy
bool Diagram::load(const QString& Line, istream_t& stream)
{
	incomplete();
	bool ok;
	QString s = Line;

	if(s.at(0) != '<') return false;
	if(s.at(s.length()-1) != '>'){ untested();
		return false;
	}else{
	}
	s = s.mid(1, s.length()-2);   // cut off start and end character

	QString n;
	n  = s.section(' ',1,1);    // cx
	int cx = n.toInt(&ok);
	if(!ok) return false;

	n  = s.section(' ',2,2);    // cy
	int cy = n.toInt(&ok);
	if(!ok) return false;

	setCenter(pos_t(cx, cy));

	n  = s.section(' ',3,3);    // x2
	x2 = n.toInt(&ok);
	if(!ok){ untested();
		return false;
	}else{
	}

	n  = s.section(' ',4,4);    // y2
	y2 = n.toInt(&ok);
	if(!ok) return false;

	char c;
	n = s.section(' ',5,5);    // GridOn
	c = n.at(0).toLatin1() - '0';
	xAxis.GridOn = yAxis.GridOn = (c & 1) != 0;
	hideLines = (c & 2) != 0;

	n = s.section(' ',6,6);    // color for GridPen
	QColor co;
	co.setNamedColor(n);
	GridPen.setColor(co);
	if(!GridPen.color().isValid()){ untested();
		return false;
	}else{
	}

	n = s.section(' ',7,7);    // line style
	GridPen.setStyle((Qt::PenStyle)n.toInt(&ok));
	if(!ok) return false;

	n = s.section(' ',8,8);    // xlog, ylog
	xAxis.log = n.at(0) != '0';
	c = n.at(1).toLatin1();
	yAxis.log = ((c - '0') & 1) == 1;
	zAxis.log = ((c - '0') & 2) == 2;

	n = s.section(' ',9,9);   // xAxis.autoScale
	if(n.at(0) != '"') {
		// backward compatible
		if(n == "1")  xAxis.autoScale = true;
		else  xAxis.autoScale = false;

		n = s.section(' ',10,10);    // xAxis.limit_min
		xAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',11,11);  // xAxis.step
		xAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',12,12);  // xAxis.limit_max
		xAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',13,13);    // yAxis.autoScale
		if(n == "1")  yAxis.autoScale = true;
		else  yAxis.autoScale = false;

		n = s.section(' ',14,14);    // yAxis.limit_min
		yAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',15,15);    // yAxis.step
		yAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',16,16);    // yAxis.limit_max
		yAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',17,17);    // zAxis.autoScale
		if(n == "1")  zAxis.autoScale = true;
		else  zAxis.autoScale = false;

		n = s.section(' ',18,18);    // zAxis.limit_min
		zAxis.limit_min = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',19,19);    // zAxis.step
		zAxis.step = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',20,20);    // zAxis.limit_max
		zAxis.limit_max = n.toDouble(&ok);
		if(!ok) return false;

		n = s.section(' ',21,21); // rotX
		if(n.at(0) != '"') {
			// backward compatible
			rotX = n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',22,22); // rotY
			rotY = n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',23,23); // rotZ
			rotZ = n.toInt(&ok);
			if(!ok) return false;
		}else{ untested();
		}
	}

	trace3("load diag", label(), Element::cx(), Element::cy());


	xAxis.Label = s.section('"',1,1);   // xLabel
	yAxis.Label = s.section('"',3,3);   // yLabel left
	zAxis.Label = s.section('"',5,5);   // yLabel right

	Graph *pg;
	return true;
	// rest is done in lang_sch now.
	// .......................................................
	// load graphs of the diagram
	while(!stream.atEnd()) {
		s = QString::fromStdString(stream.read_line());
		s = s.trimmed();
		if(s.isEmpty()) continue;

		trace2("diagram::load", Name, label());
		if(s == ("</"+Name+">")){
			return true;  // found end tag ?
		}else if(s.section(' ', 0,0) == "<Mkr") { untested();
			incomplete(); // load like variables??

			// .......................................................
			// load markers of the diagram
			// pg = Graphs.last();
			// if(!pg){ untested();
			// 	return false;
			// }else{ untested();
			// }
			// assert(pg->parentDiagram() == this);
			/// 		 Marker *pm = new Marker(nullptr);
			/// 		 if(!pm->load(s)) { untested();
			/// 			 delete pm;
			/// 			 return false;
			/// 		 }
			//      pg->Markers.append(pm);
			continue;
		}

		pg = new Graph(this);
		trace2("graph load", s, Name);
		if(!pg->load(s)) { untested();
			delete pg;
			return false;
		}
		// Graphs.append(pg);
	}
	return false;   // end tag missing
}

// --------------------------------------------------------------

// ------------------------------------------------------------


// --------------------------------------------------------------

// ------------------------------------------------------------


/*!
  Calculations for logarithmical Cartesian diagrams
  (RectDiagram and  Rect3DDiagram).

 \param       Axis   - pointer to the axis to scale
 \param       len    - length of axis in pixel on the screen
 \return value: "true" if axis runs from largest to smallest value

 \param[out]  z      - screen coordinate where the first grid is placed
 \param[out]  zD     - number where the first grid is placed
 \param[out]  zDstep - number increment from one grid to the next
 \param[out]  coor   - scale factor for calculate screen coordinate

 \todo use this as example to document other methods
*/
bool Diagram::calcAxisLogScale(Axis *Axis, int& z, double& zD,
				double& zDstep, double& corr, int len)
{itested();
  if(fabs(Axis->max-Axis->min) < 1e-200) { // if max = min, double difference
    Axis->max *= 10.0;
    Axis->min /= 10.0;
  }
  Axis->low = Axis->min; Axis->up = Axis->max;

  if(!Axis->autoScale) { untested();
    Axis->low = Axis->limit_min;
    Axis->up  = Axis->limit_max;
  }


  bool mirror=false, mirror2=false;
  double tmp;
  if(Axis->up < 0.0) {   // for negative values
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
    mirror = true;
  }

  double Base, Expo;
  if(Axis->autoScale) {itested();
    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }

    Expo = floor(log10(Axis->max));
    Base = Axis->max/pow(10.0,Expo);
    if(Base > 3.0001) Axis->up = pow(10.0,Expo+1.0);
    else  if(Base < 1.0001) Axis->up = pow(10.0,Expo);
	  else Axis->up = 3.0 * pow(10.0,Expo);

    Expo = floor(log10(Axis->min));
    Base = Axis->min/pow(10.0,Expo);
    if(Base < 2.999) Axis->low = pow(10.0,Expo);
    else  if(Base > 9.999) Axis->low = pow(10.0,Expo+1.0);
	  else Axis->low = 3.0 * pow(10.0,Expo);

    corr = double(len) / log10(Axis->up / Axis->low);

    z = 0;
    zD = Axis->low;
    zDstep = pow(10.0,Expo);

    if(mirror) {   // set back values ?
      tmp = Axis->min;
      Axis->min = -Axis->max;
      Axis->max = -tmp;
    }
  }
  else {   // user defined limits
    if(Axis->up < Axis->low) { untested();
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
      mirror2 = true;
    }

    Expo = floor(log10(Axis->low));
    Base = ceil(Axis->low/pow(10.0,Expo));
    zD = Base * pow(10.0, Expo);
    zDstep = pow(10.0,Expo);
    if(zD > 9.5*zDstep)  zDstep *= 10.0;

    corr = double(len) / log10(Axis->up / Axis->low);
    z = int(corr*log10(zD / Axis->low) + 0.5); // int(..) implies floor(..)

    if(mirror2) {   // set back values ?
      tmp = Axis->low;
      Axis->low = Axis->up;
      Axis->up  = tmp;
    }
  }

  if(mirror) {   // set back values ?
    tmp = Axis->low;
    Axis->low = -Axis->up;
    Axis->up  = -tmp;
  }

  if(mirror == mirror2)  return false;
  else  return true;
}

// --------------------------------------------------------------

// convenience wrappers
bool Diagram::insideDiagramP(Graph::iterator const& p) const
{ untested();
  float f1 = p->getScrX();
  float f2 = p->getScrY();
  return insideDiagram(f1,f2);
}
// void Diagram::calcCoordinateP (const double*x, const double*y, const double*z, Graph::iterator& p, Axis const* A) const
// {itested();
//   float f1, f2;
//   calcCoordinate(x, y, z, &f1, &f2, A);
//   p->setScr(f1, f2);
// };


/* PHASOR AND WAVEAC RELATED CODE
//only for phasor diagram detect if the points are in the diagram,
//  if not tell with are the limits that the point has passed
bool Diagram::insideDiagramPh(Graph::iterator const& p ,float* xn, float* yn) const 
{ untested();
  float f1 = p->getScrX();
  float f2 = p->getScrY();
  float xa,ya;

  xa = f1;
  ya = f2;

  if(f1 < 0.0)
    xa = 0.0;
  if(f1 > float(x2))
    xa = float(x2);
  if(f2 < 0.0)
    ya = 0.0;
  if(f2 > float(y2))
    ya = float(y2);
  
  *xn = xa;
  *yn = ya;

  return ((xa == f1)&&(ya == f2));
}
//for phasor if the original point isn't in diagram with the limits calculated in insideDiagramPh
//  will create a point inside the diagram if possible
bool Diagram::newcoordinate(Graph::iterator const& p,float* xn, float* yn) const
{ untested();
  float f1 = (p-1)->getScrX();
  float f2 = (p-1)->getScrY();
  float f3 = p->getScrX();
  float f4 = p->getScrY();
  float xc = *xn;
  float yc = *yn;
  float xt,yt;
  float d;
  float b;
  
  if(((f1 > f3 - 3) && (f1 < f3 + 3)) || ((f2 > f4 - 3) && (f2 < f4 + 3)))
  { untested();
    d = 0.0;
    b = 0.0;
  }
  else
  { untested();
    d = (f4 - f2) / (f3 - f1);
    b = f2 - d * f1;
  }


  if((f1 > f3 - 3) && (f1 < f3 + 3) && (f2 != f4))
  { untested();
    xt = f1;
    yt = yc;
  }
  else
  { untested();
    if((f2 > f4 - 3) && (f2 < f4 + 3) && (f1 != f3))
    { untested();
      xt = xc;
      yt = f2;
    }
    else
    { untested();
      yt = d*xc + b;
      xt = (yc - b) / d;
    }
  }
  if((yt >= 0.0) && (yt <= float(y2)))
  { untested();
      *yn = yt;
      return true;
  }
  else
  { untested();
    if((xt >= 0.0) && (xt <= float(x2)))
    { untested();
	*xn = xt;
	return true;
    }
    else
	return false;
	
  }  
}

//  will read the values receive and find if is one the values determined by AC and remove repeated number.
//  if there isn't any value that match will find the closest number and replace
void Diagram::findfreq(Graph *g)
{ untested();
  if(freq!=nullptr) delete[] freq;
  freq= nullptr;
  int z = QString::compare(g->axis(0)->Var,"acfrequency",Qt::CaseInsensitive);//meaning that only work in AC 
  if(z != 0)
  { untested();
    nfreqt=1;
    freq = new double;
    freq[0] = 0;
    sfreq = "0 Hz;";
    return;
  }
  double scale = 1.0;
  QString num;
  bool ok;
  double freqnum;
  int n=0; 
  int m=0;
  int a;
  QString value;
  int s;
  n=sfreq.count(';')+1;
  freq= new double[n];

  do{ untested();
    n = sfreq.indexOf(";",m,Qt::CaseInsensitive);
    if(n==-1 || Name == "Waveac") n = sfreq.size()-1;
    value=sfreq.mid(m,n+1-m);
    a=value.size();

    if(value.indexOf("ghz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e9;
      a = value.indexOf("ghz",0,Qt::CaseInsensitive);    
    }
    else if(value.indexOf("mhz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e6;
      a = value.indexOf("mhz",0,Qt::CaseInsensitive);     
    }
    else if(value.indexOf("khz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1e3;
      a = value.indexOf("khz",0,Qt::CaseInsensitive);
    }
    else if(value.indexOf("hz",0,Qt::CaseInsensitive) != -1)
    { untested();
      scale = 1.0;
      a = value.indexOf("hz",0,Qt::CaseInsensitive);
    }

    double *px,f=0;
    int i,z;
    double d,dmin=DBL_MAX;
    num = value.mid(0,a);
    freqnum = num.toDouble(&ok) * scale;
    if(!ok)
    { untested();
      scale = 1.0;
      for(s=a;s>0;s--)
      { untested();
	num = value.mid(0,s);
	freqnum = num.toDouble(&ok) * scale;
	if(ok)
	{ untested();
	  value.resize(s);
	  break;
	}
      }
      if(s==0)
	goto end;
    }

    for(i=g->countY; i>0; i--) {  // every branch of curves
      px = g->axis(0)->Points;
      for(z=g->axis(0)->count; z>0; z--) {  // every point
	if(*px > 0)
	{ untested();
	  d=fabs(freqnum - *px);
	  if(d<dmin) 
	  { untested();
	    dmin=d;
	    f= *px; 
	  }
	}
	++px;
      }
    }
    freqnum = f;
    for(s=0;s<nfreqt;s++)
    { untested();
      if(freq[s]==freqnum)
      { untested();
	freqnum = 0;
	break;
      }
      if(freq[s]>freqnum)
      { untested();
	f=freq[s];
	freq[s]=freqnum;
	freqnum=f;
      }
    }  
    if(freqnum == 0) 
    { untested();
      value.clear();
      goto end;
    }
    nfreqt++;
    freq[nfreqt-1]=freqnum;
end:
    m=n+1;
    
  }while(n!=sfreq.size()-1);

  if(freqnum==0 &&nfreqt==0)
  { untested();
    nfreqt=1;
    freq[0] = 0;
    sfreq = "0 Hz;";
    return;   
  }
  nfreqa=0;
  sfreq.clear();
  while(nfreqa<nfreqt)
  { untested();
    freqnum=freq[nfreqa];

    if(freqnum >= 1e9)
    { untested();
      freqnum/= 1e9;
      value.setNum(freqnum);
      value+= " GHz;";
    }
    else if(freqnum >= 1e6)
    { untested();
      freqnum/= 1e6;
      value.setNum(freqnum);
      value+= " MHz;";
    }
    else if(freqnum >= 1e3)
    { untested();
      freqnum/= 1e3;
      value.setNum(freqnum);
      value+= " KHz;";
    }
    else
    { untested();
      value.setNum(freqnum);
      value+= " Hz;";
    }

    sfreq.append(value);
    nfreqa++;
  }

  

  
}

// for phasor will find the biggest absolute value of all max limits and replace the others
void Diagram::setlimitsphasor(Axis *x ,Axis *y)
{ untested();
  double yrx,yrn,yix,yin;

    yrn = x->min;
    yrx = x->max;
    yin = y->min;
    yix = y->max;

    if(fabs(yrn) > yrx)
      yrx = fabs(yrn);
    else
      yrn = (-1.0) * yrx;

    if(fabs(yin) > yix)
      yix = fabs(yin);
    else
      yin = (-1.0) * yix;
    
    if(yrx < yix)
      yrx = yix;

    x->max = y->max = yrx ;
    x->min = y->min = (-1.0) * yrx;

}

//for marker of waveac to find the value of x
double Diagram::wavevalX(int i) const
{ untested();
    return i*xAxis.up/(sc*50); 
}
*/
/*--------------------------------------------------------------------------*/
void Diagram::prepare()
{
	for(auto i : *scope()){
		i->prepare();
	}
}
/*--------------------------------------------------------------------------*/
Widget* Diagram::schematicWidget(Doc* Doc) const
{ untested();
  trace0("Component::editElement");
  incomplete();
  return nullptr;
//  return new DiagramDialog(Doc); // memory leak?
}
/*--------------------------------------------------------------------------*/
rect_t Diagram::bounding_rect() const
{ untested();
	QPointF tl(0, -y2); // eek
	QPointF br(x2, 0);
	return rect_t(QRectF(tl, br));
}
/*--------------------------------------------------------------------------*/
void Axis::fit_in_data(double lo, double hi)
{
	if(min > lo){itested();
		min = lo;
	}else{itested();
	}
	if(max < hi){itested();
		max = hi;
	}else{itested();
	}
}
/*--------------------------------------------------------------------------*/
} // qucs
/*--------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------*/
