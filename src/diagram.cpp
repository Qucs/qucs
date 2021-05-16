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
	 xAxis(p.xAxis),
	 yAxis(p.yAxis),
	 zAxis(p.zAxis),
    _width(p._width),
    _height(p._height),
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
		 auto cl = i->clone();
		 _subckt->push_back(cl);
		 if(i->owner() == &p){
			 cl->set_owner(this);
		 }else{
		 }
	 }
}

#if 1
Diagram::Diagram()
  : Element()
{
	new_subckt();
  
  // x1, x2, y1, y2 are the selectable boundings of the diagram, but these
  // are the real boundings. They are set in "createAxisLabels()".
  // Bounding_x1 = Bounding_x2 = Bounding_y1 = Bounding_y2 = 0;

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


//  Type = isDiagram;
  GridPen = QPen(Qt::lightGray,0);

  set_label("diag");
}
#endif

Diagram::~Diagram()
{
  delete _subckt;
}

index_t Diagram::param_count() const
{
  return 8; // yikes. too many.
}

// not sure if this is needed.
std::string Diagram::param_by_name(std::string const& n)const
{
	if(n == "$$xaxislog"){ untested();
		return std::to_string(xAxis.log);
	}else if(n == "$$yaxislog"){ untested();
		return std::to_string(yAxis.log);
	}else{ untested();
		incomplete();
		return ("incomplete_unnamed");
	}
}

std::string Diagram::param_value(index_t i)const
{
	if(i==7){
		incomplete(); // some mangling. move to legacy.
		return std::to_string(xAxis.log);
	}else{
		throw qucs::ExceptionCantFind( short_label(), std::to_string(i), "params");
	}
}

std::string Diagram::param_name(index_t n) const
{
	throw qucs::ExceptionCantFind( short_label(), std::to_string(n), "params");
}

void Diagram::set_param_by_index(index_t i, std::string const& v)
{itested();

	pos_t p = position();
	int cx = getX(p);
	int cy = getY(p);
	bool ok = true;
	auto n = QString_(v);
	QColor co;
	char c;
	switch(i){
	case 0:itested();
		cx = n.toInt(&ok);
		break;
	case 1:itested();
		cy = n.toInt(&ok);
		break;
	case 2:itested();
		_width = n.toInt(&ok);
		break;
	case 3:itested();
		_height = n.toInt(&ok);
		break;
	case 4:
		c = n.at(0).toLatin1() - '0';
		xAxis.GridOn = yAxis.GridOn = (c & 1) != 0;
		// hideLines = (c & 2) != 0; // 3d diagram?
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

	trace4("Diag param", cx, cy, _width, _height);

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
      // int x_, y_;
      // float fx_, fy_;
      // p->drawRect(x_-5, y_-5, TO_INT(fx_), TO_INT(fy_));

      p->setPen(QPen(Qt::darkGray,3));
      p->setPen(QPen(Qt::darkRed,2));
      p->drawResizeRect(0, _height);  // markers for changing the size
      p->drawResizeRect(0, 0);
      p->drawResizeRect(_width, _height);
      p->drawResizeRect(_width, 0 );
    }
#endif
}

void Diagram::new_subckt()
{
	assert(!_subckt);
	_subckt = new ElementList();
}

void Diagram::paintMarkers(ViewPainter *, bool)
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
  x = _width / 2;
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
  // Bounding_y2 = 0;
  // Bounding_y1 = y - LineSpacing;
  // Bounding_x2 = wmax - (x2 >> 1);
  // if(Bounding_x2 < 0) Bounding_x2 = 0;
  // Bounding_x1 = Bounding_x2;


  wmax = 0;
  x = -x1;
  y = _height / 2;
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
//   if(Bounding_x1 < -x) Bounding_x1 = -x;


  x = x3;
  y = _height / 2;
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
  x -= _width;
//  if(Bounding_x2 < x){
//    Bounding_x2 = x;
//  }else{
//  }

  wmax -= _height / 2;
  if(wmax > 0) { untested();
//    Bounding_y2 = wmax;
    wmax *= -1;
//    if(wmax < Bounding_y1) Bounding_y1 = wmax;
  }else{
  }
} // createAxisLabels

// ------------------------------------------------------------
#if 0
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
#endif



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
// Check if the resize area was clicked. If so return "true" and sets
// x1/y1 and x2/y2 to the border coordinates to draw a rectangle.
#if 0
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
#endif

// --------------------------------------------------------------------------

#if 0
bool Diagram::sameDependencies(Graph const*, Graph const*) const
{ untested();
  incomplete();
  // FIXME
  // return g1->same(*g2);
  if(g1 == g2)  return true;
  if(g1->numAxes()!=g2->numAxes()) return false;

  for(unsigned i=0; i<g1->numAxes(); ++i) { untested();
    if(g1->axisName(i) != g2->axisName(i))  return false;
  }

  return true;
  return false;
}
#endif

// ------------------------------------------------------------
void Diagram::finishMarkerCoordinates(float&, float&) const
{ untested();
#if 0
  if(!insideDiagram(fCX, fCY)) { untested();
      fCX = float(x2 >> 1);
      fCY = float(y2 >> 1);
  }
#endif
}

// ------------------------------------------------------------
// BUG
QString Diagram::save() const
{
  QString s = "<"+Name+" "+QString::number(cx())+" "+QString::number(cy())+" ";
  s += QString::number(_width)+" "+QString::number(_height)+" ";
  char c = '0';
  if(xAxis.GridOn){
	  c |= 1;
  }else{
  }
// 3d?
//  if(hideLines) {
//	  c |= 2;
//  }else{
//  }
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

  // 3d
  int rotX = 0;
  int rotY = 0;
  int rotZ = 0;
  s += QString::number(rotX)+" "+QString::number(rotY)+" "+
       QString::number(rotZ);

  // labels can contain spaces -> must be last items in the line
  QString sfreq="0"; // ?
  s += " \""+xAxis.Label+"\" \""+yAxis.Label+"\" \""+zAxis.Label+"\" \""+sfreq+"\">\n";

  return s;
}

// ------------------------------------------------------------
// BUG: lang_sch. MOVE to legacy
bool Diagram::load(const QString& Line, istream_t&)
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

	setPosition(pos_t(cx, cy));

	n  = s.section(' ',3,3);    // x2
	_width = n.toInt(&ok);
	if(!ok){ untested();
		return false;
	}else{
	}

	n  = s.section(' ',4,4);    // y2
	_height = n.toInt(&ok);
	if(!ok) return false;

	char c;
	n = s.section(' ',5,5);    // GridOn
	c = n.at(0).toLatin1() - '0';
	xAxis.GridOn = yAxis.GridOn = (c & 1) != 0;
	// hideLines = (c & 2) != 0; // 3d?

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
			/* rotX = */ n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',22,22); // rotY
			/* rotY = */ n.toInt(&ok);
			if(!ok) return false;

			n = s.section(' ',23,23); // rotZ
			/* rotZ = */ n.toInt(&ok);
			if(!ok) return false;
		}else{ untested();
		}
	}

	trace3("load diag", label(), Element::cx(), Element::cy());


	xAxis.Label = s.section('"',1,1);   // xLabel
	yAxis.Label = s.section('"',3,3);   // yLabel left
	zAxis.Label = s.section('"',5,5);   // yLabel right

	return true;
	// rest is done in lang_sch now.
}
// --------------------------------------------------------------

// convenience wrappers
// bool Diagram::insideDiagramP(Graph::iterator const& p) const
// { untested();
//   float f1 = p->getScrX();
//   float f2 = p->getScrY();
//   return insideDiagram(f1,f2);
// }
/*--------------------------------------------------------------------------*/
void Diagram::prepare()
{
	for(auto i : *scope()){
		i->prepare();
	}
}
/*--------------------------------------------------------------------------*/
Widget* Diagram::schematicWidget(Doc*) const
{itested();
  trace0("Component::editElement");
  incomplete();
  return nullptr;
//  return new DiagramDialog(Doc); // memory leak?
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
