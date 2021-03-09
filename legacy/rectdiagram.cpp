/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
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

// a Cartesian diagram

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "diagram.h"
#include "painting.h"
#include "qucs_app.h"
#include "misc.h"
#include "some_font_stuff.h"
#include "qucs_globals.h"
#include "module.h"
#include <QLineEdit>
#include "obsolete_stuff.h"

namespace {

using namespace qucs;

class RectDiagram : public Diagram, public Painting{
  RectDiagram(RectDiagram const& c) : Diagram(c) {}
public:
  explicit RectDiagram();
 ~RectDiagram();

private:
  Element* clone() const override {untested(); return new RectDiagram(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  void calcCoordinate(const double*, const double*, const double*, float*, float*, Axis const*) const;
  void finishMarkerCoordinates(float&, float&) const;
  bool insideDiagram(float, float) const;

  pos_t center() const override{ untested();
    return Element::center();
  }

private: // Painting
  rect_t bounding_rect() const override { untested();
    QPointF tl(0, -y2); // eek
    QPointF br(x2, 0);
    return rect_t(QRectF(tl, br));
  }

  void paint(ViewPainter* v) const override{
    Diagram const* cd = this;
    Diagram* d=const_cast<Diagram*>(cd);
    d->paintDiagram(v);
    //		Element::paint(p);
  }

public: // legacy cruft.
  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;

protected:
  void clip(Graph::iterator &) const;
}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Rect", &D);
Module::INSTALL pp("diagrams", &D);

RectDiagram::RectDiagram() : Diagram(0, 0)
{ untested();
  x1 = 10;      // position of label text
  y1 = y3 = 33;
  x2 = 240;    // initial size of diagram
  y2 = 160;
  x3 = 247;    // with some distance for right axes text

  Name = "Rect"; // BUG
  setTypeName("Rect");
  calcDiagram();
}

RectDiagram::~RectDiagram()
{ untested();
}

// ------------------------------------------------------------
void RectDiagram::calcCoordinate(const double* xD, const double* yD, const double*,
                                 float *px, float *py, Axis const *pa) const
{ untested();
  double x  = *xD;
  double yr = yD[0];
  double yi = yD[1];
  if(xAxis.log) { untested();
    x /= xAxis.low;
    if(x <= 0.0) {
      *px = -1e5;   // "negative infinity"
    }else{
      *px = float(log10(x)/log10(xAxis.up / xAxis.low) * double(x2));
    }
  }else{
    *px = float((x-xAxis.low)/(xAxis.up-xAxis.low)*double(x2));
  }

  if(pa->log) { untested();
    yr = sqrt(yr*yr + yi*yi);
    if(yr <= 0.0)  *py = -1e5;   // "negative infinity"
    else *py = float(log10(yr/fabs(pa->low)) /
                     log10(pa->up/pa->low) * double(y2));
  }else{ untested();
    if(fabs(yi) > 1e-250) {
      // preserve negative values if not complex number
      yr = sqrt(yr*yr + yi*yi);
    }else{
    }
    *py = float((yr-pa->low)/(pa->up-pa->low)*double(y2));
  }

  if(!std::isfinite(*px)){
    *px = 0.0;
  }else{
  }
  if(!std::isfinite(*py)){
    *py = 0.0;
  }else{
  }
}

// --------------------------------------------------------------
void RectDiagram::finishMarkerCoordinates(float& fCX, float& fCY) const
{ untested();
  if(!insideDiagram(fCX, fCY)) { untested();
	  fCX = fCY = 0.0;
  }
}

// --------------------------------------------------------------
void RectDiagram::calcLimits()
{ untested();
  int i;
  double a, b, c;

  if(xAxis.autoScale) {// check before, to preserve limit exchange (max < min)
    if(xAxis.log) { untested();
      calcAxisLogScale(&xAxis, i, a, b, c, x2);
      xAxis.step = 1.0;
    }else{
      calcAxisScale(&xAxis, a, b, c, xAxis.step, double(x2));
    }
    xAxis.limit_min = xAxis.low;
    xAxis.limit_max = xAxis.up;
  }else{
  }

  if(yAxis.autoScale) {
    // check before, to preserve limit exchange (max < min)
    if(yAxis.log) { untested();
      calcAxisLogScale(&yAxis, i, a, b, c, y2);
      yAxis.step = 1.0;
    }
    else  calcAxisScale(&yAxis, a, b, c, yAxis.step, double(y2));
    yAxis.limit_min = yAxis.low;
    yAxis.limit_max = yAxis.up;
  }else{
  }

  if(zAxis.autoScale) {
    // check before, to preserve limit exchange (max < min)
    if(zAxis.log) { untested();
      calcAxisLogScale(&zAxis, i, a, b, c, y2);
      zAxis.step = 1.0;
    }else{
      calcAxisScale(&zAxis, a, b, c, zAxis.step, double(y2));
    }
    zAxis.limit_min = zAxis.low;
    zAxis.limit_max = zAxis.up;
  }else{
  }
}

// --------------------------------------------------------------
int RectDiagram::calcDiagram()
{ untested();
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  double GridStep, corr, zD, zDstep, GridNum;
  // get size of text using the screen-compatible metric
  FontMetrics metrics;
  y1 = 18; // was: QucsSettings.fontSize() + 6;

  x1 = 10;      // position of label text
  x3 = x2 + 7;
  QString tmp;
  bool back = false;
  int  z, w, valid = 0;

  // =====  give "step" the right sign (if user made it wrong)  ==============
  xAxis.step = fabs(xAxis.step);
  if(xAxis.limit_min > xAxis.limit_max){
    xAxis.step *= -1.0;
  }else{
  }

  yAxis.step = fabs(yAxis.step);
  if(yAxis.limit_min > yAxis.limit_max){
    yAxis.step *= -1.0;
  }else{
  }

  zAxis.step = fabs(zAxis.step);
  if(zAxis.limit_min > zAxis.limit_max){
    zAxis.step *= -1.0;
  }else{
  }


  // ====  x grid  =======================================================
if(xAxis.log) { untested();
  if(xAxis.autoScale) { untested();
    if(xAxis.max*xAxis.min < 1e-200)  goto Frame;  // invalid
  }
  else  if(xAxis.limit_min*xAxis.limit_max < 1e-200)  goto Frame;  // invalid

  back = calcAxisLogScale(&xAxis, z, zD, zDstep, corr, x2);

  if(back) z = x2;
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen));  // x grid

    if((zD < 1.5*zDstep) || (z == 0) || (z == x2)) { untested();
      tmp = misc::StringNiceNum(zD);
      if(xAxis.up < 0.0)  tmp = '-'+tmp;
      w = metrics.width(tmp);  // width of text
      // center text horizontally under the x tick mark
      Texts.append(new Text(z-(w>>1), -y1, tmp));
      Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));  // x tick marks
    }

    zD += zDstep;
    if(zD > 9.5*zDstep)  zDstep *= 10.0;
    if(back) { untested();
      z = int(corr*log10(zD / fabs(xAxis.up)) + 0.5); // int() implies floor()
      z = x2 - z;
    }
    else
      z = int(corr*log10(zD / fabs(xAxis.low)) + 0.5);// int() implies floor()
  }
}
else {  // not logarithmical
  calcAxisScale(&xAxis, GridNum, zD, zDstep, GridStep, double(x2));

  double Expo;
  if(xAxis.up == 0.0)  Expo = log10(fabs(xAxis.up-xAxis.low));
  else  Expo = log10(fabs(xAxis.up));

  zD += 0.5;     // perform rounding
  z = int(zD);   //  "int(...)" implies "floor(...)"
  while((z <= x2) && (z >= 0)) {    // create all grid lines
    if(fabs(GridNum) < 0.01*pow(10.0, Expo)) GridNum = 0.0;// make 0 really 0
    tmp = misc::StringNiceNum(GridNum);
    w = metrics.width(tmp);  // width of text
    // center text horizontally under the x tick mark
    Texts.append(new Text(z-(w>>1), -y1, tmp)); // Text(x, y, str, ...)
    GridNum += GridStep;

    if(xAxis.GridOn)  if(z < x2)  if(z > 0)
      Lines.prepend(new Line(z, y2, z, 0, GridPen)); // x grid
    Lines.append(new Line(z, 5, z, -5, QPen(Qt::black,0)));   // x tick marks
    zD += zDstep;
    z = int(zD);
  }
} // of "if(xlog) ... else ..."


  // ====  y grid  =======================================================
  if(zAxis.numGraphs > 0) if(calcYAxis(&zAxis, x2)) valid |= 2;
  if(yAxis.numGraphs > 0) if(calcYAxis(&yAxis, 0))  valid |= 1;


Frame:
  // outer frame
  Lines.append(new Line(0,  y2, x2, y2, QPen(Qt::black,0)));
  Lines.append(new Line(x2, y2, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,   0, x2,  0, QPen(Qt::black,0)));
  Lines.append(new Line(0,  y2,  0,  0, QPen(Qt::black,0)));
  return valid;
}

// ------------------------------------------------------------
bool RectDiagram::insideDiagram(float x, float y) const
{ untested();
  return (regionCode(x, y) == 0);
}

// ------------------------------------------------------------
void RectDiagram::clip(Graph::iterator &p) const
{ untested();
  rectClip(p);
}

// ------------------------------------------------------------
#if 0
Element* RectDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{ untested();
  unreachable();
  Name = QObject::tr("Cartesian");
  BitmapFile = (char *) "rect";

  if(getNewOne)  return new RectDiagram();
  return 0;
}
#endif

}
// vim:ts=8:sw=2:noet
