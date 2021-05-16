/***************************************************************************
                          smithdiagram.cpp  -  description
                             -------------------
    begin                : Sat Oct 18 2003
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
  \class SmithDiagram
  \brief The SmithDiagram class implements the Impedance and Admittance Smith diagram
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <cmath>
#include <float.h>
#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "misc.h"
#include "diagram.h"
#include "obsolete_stuff.h"
#include "qucs_globals.h"
#include "module.h"


#define x2 _width
#define y2 _height

namespace{

using namespace qucs;

class SmithDiagram : public Diagram  {
public: 
  SmithDiagram(int _cx=0, int _cy=0, bool ImpMode=true);
 ~SmithDiagram();

  Element* clone() const {return new SmithDiagram(*this);}
  static Element* info(QString&, char* &, bool getNewOne=false);
  static Element* info_y(QString&, char* &, bool getNewOne=false);
  int  calcDiagram();
  void calcLimits();
  virtual diag_coordinate_t calcCoordinate(double const&, double const&) const override{
    incomplete();
    return diag_coordinate_t();
  }
  QString extraMarkerText(Marker const*) const;

	std::string dev_type() const override{
		return "Smith";
	}
public: // legacy cruft.
  QList<Graph *>  Graphs;
  QList<Arc *>    Arcs;
  QList<Line *>   Lines;
  QList<Text *>   Texts;
  void calcSmithAxisScale(Axis*, int&, int&){incomplete();}
  void createSmithChart(Axis*, int /*Mode=7*/){incomplete();}
  void clip(Graph::iterator &p) const;

}D;
Dispatcher<Diagram>::INSTALL p(&diagram_dispatcher, "Smith", &D);
Module::INSTALL pp("diagrams", &D);

const double pi = 3.1415926535897932384626433832795029;  /* pi   */

#if 0
void p2c(double &Real, double &Imag) {
  double Real_ = Real;
  Real = Real_ * cos(Imag * pi / 180.0); // real part
  Imag = Real_ * sin(Imag * pi / 180.0); // imaginary part
}
void r2z(double &Real, double &Imag, double Z0) {
  double tmp = Z0 / ((1.0 - Real) * (1.0 - Real) + Imag * Imag);
  Real = (1.0 - Real * Real - Imag * Imag) * tmp;
  Imag *= 2.0 * tmp;
}
void c2p(double &Real, double &Imag) {
  double Real_ = Real;
  Real = sqrt(Real * Real + Imag * Imag); // magnitude
  Imag = 180.0 / pi * atan2(Imag, Real_); // phase in degree
}

/*!
   Clipping for round diagrams (smith, polar, ...)
*/
void SmithDiagram::clip(Graph::iterator &p) const
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
#endif


} // namespace

SmithDiagram::SmithDiagram(int, int, bool ImpMode) : Diagram()
{
  x1 = 10;     // position of label text
  y1 = 2;
  x2 = 200;    // initial size of diagram
  y2 = 200;
  y3 = 0;
  x3 = 207;    // with some distance for right axes text
  if(ImpMode)  Name = "Smith";  // with impedance circles
  else  Name = "ySmith";        // with admittance circles

  Arcs.append(new struct Arc(0, y2, x2, y2, 0, 16*360, QPen(Qt::black,0)));
//  calcDiagram();    // calculate circles for smith chart with |r|=1
}

SmithDiagram::~SmithDiagram()
{
}

// ------------------------------------------------------------
// calculate the screen coordinates for the graph data
#if 0
void SmithDiagram::calcCoordinate(const double*, const double* yD, const double*,
                                  float *px, float *py, Axis const*) const
{
  double yr = yD[0];
  double yi = yD[1];
  *px = float((yr/yAxis.up + 1.0)*double(x2)/2.0);
  *py = float((yi/yAxis.up + 1.0)*double(y2)/2.0);

  if(std::isfinite(*px))
    if(std::isfinite(*py))
      return;

  *px = *py = float(cx()) / 2.0;
}
#endif

// ------------------------------------------------------------
void SmithDiagram::calcLimits()
{
  int a;
  calcSmithAxisScale(&yAxis, a, a);
  yAxis.limit_min = 0.0;
  yAxis.step = double(a);
  yAxis.limit_max = yAxis.up;
}

// ------------------------------------------------------------
// calculate the circles and arcs of the smith chart
int SmithDiagram::calcDiagram()
{
  Lines.clear();
  Texts.clear();
  Arcs.clear();

  x3 = x2 + 7;
  if(Name.at(0) == 'y') {
    createSmithChart(&yAxis, 6);
  }else{
    createSmithChart(&yAxis, 7);
  }

  // outer most circle
  Arcs.append(new Arc(0, x2, x2, x2, 0, 16*360, QPen(Qt::black,0)));

  // horizontal line Im(r)=0
  Lines.append(new Line(0, x2>>1, x2, x2>>1, GridPen));

  return 3;
}

// ------------------------------------------------------------
#if 0
Element* SmithDiagram::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Smith Chart");
  BitmapFile = (char *) "smith";

  if(getNewOne)  return new SmithDiagram();
  return 0;
}

// ------------------------------------------------------------
Element* SmithDiagram::info_y(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Admittance Smith");
  BitmapFile = (char *) "ysmith";

  if(getNewOne)  return new SmithDiagram(0, 0, false);
  return 0;
}
#endif

QString SmithDiagram::extraMarkerText(Marker const*) const
{
#if 0
  assert(m);
  Graph const* pGraph = m->graph();
  assert(pGraph);
  std::vector<double> const& Pos = m->varPos();
  unsigned nVarPos = pGraph->numAxes();
  assert(nVarPos == Pos.size());
  double Zr, Zi, Yr, Yi, Ds;
  double Z0 = m->Z0;
  double Precision = m->precision(); // hmmm
  QString ExtraParamsText;//Variable used for displaying extra marker data (impedance and/or admittance)

  Zr = m->powReal();
  Zi = m->powImag();

  r2z(Zr, Zi, Z0);
  // convert impedance to admittance
  Yr = Zr; Yi = Zi;
  c2p(Yr, Yi);
  Yr = 1.0 / Yr; // magnitude
  Yi = -Yi; // angle
  p2c(Yr, Yi);
    
  QString Var = pGraph->Var;
  QString varName;

  QString valMarkerZ = misc::complexRect(Zr, Zi, Precision);
  QString valMarkerY = misc::complexRect(Yr, Yi, Precision);

  double omega = 2.0 * pi * m->powFreq();
  QString extText, unitSymbol;

  if (m->optText & Marker::SHOW_Z) {
      if(Var.startsWith("S")) { // uuh, ooh hack.
        varName = 'Z' + Var.mid(1);
      } else {
        varName = Var;
      }
      ExtraParamsText = "\n" + varName +": " + valMarkerZ + ' ' + QChar(0x2126);
  }
  
  if (m->optText & Marker::SHOW_Y) {
      if(Var.startsWith("S")) { // uuh, ooh hack.
        varName = 'Y' + Var.mid(1);
      } else {
        varName = Var;
      }
      ExtraParamsText += "\n" + varName +": " + valMarkerY  + " S";
  }
  
  if (m->optText & Marker::SHOW_ZS) {
    if (Zi < 0) { // capacitive reactance
      Ds = -1.0/(omega*Zi); // equivalent series capacitance
      unitSymbol = "F";
    } else { // inductive reactance
      Ds = Zi / omega; // equivalent series inductance
      unitSymbol = "H";
    }
    ExtraParamsText += "\n" + misc::num2str(Zr, Precision) + QChar(0x2126)
      + "+" + misc::num2str(Ds, Precision) + unitSymbol;
  }

  if (m->optText & Marker::SHOW_ZP) {
    if (Yi > 0) { // capacitive susceptance
      Ds = Yi/omega; // equivalent parallel capacitance
      unitSymbol = "F";
    } else { // inductive susceptance
      Ds = -1.0 / (omega * Yi); // equivalent parallel inductance
      unitSymbol = "H";
    }
    ExtraParamsText += "\n" + misc::num2str(1/Yr, Precision) + QChar(0x2126)
      + "||" + misc::num2str(Ds, Precision) + unitSymbol;
  }
  return ExtraParamsText;
#endif
  return "incomplete";
}

#if 0 // stuff moved here, for later.
void Diagram::calcSmithAxisScale(Axis *Axis, int& GridX, int& GridY)
{ untested();
  xAxis.low = xAxis.min;
  xAxis.up  = xAxis.max;

  Axis->low = 0.0;
  if(fabs(Axis->min) > Axis->max)
    Axis->max = fabs(Axis->min);  // also fit negative values
  if(Axis->autoScale) { untested();
    if(Axis->max > 1.01)  Axis->up = 1.05*Axis->max;
    else  Axis->up = 1.0;
    GridX = GridY = 4;
  }
  else { untested();
    Axis->up = Axis->limit_max = fabs(Axis->limit_max);
    GridX = GridY = int(Axis->step);
  }
}
void Diagram::createSmithChart(Axis *Axis, int Mode)
{ untested();
  int GridX;    // number of arcs with re(z)=const
  int GridY;    // number of arcs with im(z)=const
  calcSmithAxisScale(Axis, GridX, GridY);


  if(!xAxis.GridOn)  return;

  bool Zplane = ((Mode & 1) == 1);   // impedance or admittance chart ?
  bool Above  = ((Mode & 2) == 2);   // paint upper half ?
  bool Below  = ((Mode & 4) == 4);   // paint lower half ?

  int dx2 = x2>>1;

  double im, n_cos, n_sin, real, real1, real2, root;
  double rMAXq = Axis->up*Axis->up;
  int    theta, beta, phi, len, m, x, y;

  int R1 = int(x2/Axis->up + 0.5);
  // ....................................................
  // draw arcs with im(z)=const
  for(m=1; m<GridY; m++) { untested();
    n_sin = pi*double(m)/double(GridY);
    n_cos = cos(n_sin);
    n_sin = sin(n_sin);
    im = (1.0-n_cos)/n_sin * pow(Axis->up,0.7); // up^0.7 is beauty correction
    y  = int(im/Axis->up*x2 + 0.5);  // diameter

    if(Axis->up <= 1.0) {       // Smith chart with |r|=1
      beta  = int(16.0*180.0*atan2(n_sin-im,n_cos-1.0)/pi - 0.5);
      if(beta<0) beta += 16*360;
      theta = 16*270-beta;
    }
    else {         // Smith chart with |r|>1
      im = 1.0/im;
      real = (rMAXq+1.0)/(rMAXq-1.0);
      root =  real*real - im*im - 1.0;
      if(root < 0.0) {  // circle lies completely within the Smith chart ?
        beta = 0;       // yes, ...
        theta = 16*360; // ... draw whole circle
      }
      else { untested();
	// calculate both intersections with most outer circle
	real1 =  sqrt(root)-real;
	real2 = -sqrt(root)-real;

	root  = (real1+1.0)*(real1+1.0) + im*im;
	n_cos = (real1*real1 + im*im - 1.0) / root;
	n_sin = 2.0*im / root;
	beta  = int(16.0*180.0*atan2(n_sin-1.0/im,n_cos-1.0)/pi);
	if(beta<0) beta += 16*360;

	root  = (real2+1.0)*(real2+1.0) + im*im;
	n_cos = (real2*real2 + im*im - 1.0) / root;
	n_sin = 2.0*im / root;
	theta  = int(16.0*180.0*atan2(n_sin-1/im,n_cos-1)/pi);
	if(theta<0) theta += 16*360;
	theta = theta - beta;   // arc length
	if(theta < 0) theta = 16*360+theta;
      }
    }

    if(Zplane)
      x = (x2 + R1 - y) >> 1;
    else { untested();
      x = (x2 - R1 - y) >> 1;
      beta = 16*180 - beta - theta;  // mirror
      if(beta < 0) beta += 16*360;   // angle has to be > 0
    }

    if(Above)
      Arcs.append(new struct Arc(x, dx2+y, y, y, beta, theta, GridPen));
    if(Below)
      Arcs.append(new struct Arc(x, dx2, y, y, 16*360-beta-theta, theta, GridPen));
  }

  // ....................................................
  // draw  arcs with Re(z)=const
  theta = 0;       // arc length
  beta  = 16*180;  // start angle
  if(Above)  { beta = 0;  theta = 16*180; }
  if(Below)  theta += 16*180;

  for(m=1; m<GridX; m++) { untested();
    im = m*(Axis->up+1.0)/GridX - Axis->up;
    y  = int((1.0-im)/Axis->up*double(dx2) + 0.5);  // diameter

    if(Zplane)
      x = ((x2+R1)>>1) - y;
    else
      x = (x2-R1)>>1;
    if(fabs(fabs(im)-1.0) > 0.2)   // if too near to |r|=1, it looks ugly
      Arcs.append(new struct Arc(x, (x2+y)>>1, y, y, beta, theta, GridPen));

    if(Axis->up > 1.0) {  // draw arcs on the rigth-handed side ?
      im = 1.0-im;
      im = (rMAXq-1.0)/(im*(im/2.0+1.0)) - 1.0;
      if(Zplane)  x += y;
      else  x -= y;
      if(im >= 1.0)
        Arcs.append(new struct Arc(x, (x2+y)>>1, y, y, beta, theta, GridPen));
      else { untested();
        phi = int(16.0*180.0/pi*acos(im));
        len = 16*180-phi;
        if(Above && Below)  len += len;
        else if(Below)  phi = 16*180;
        if(!Zplane)  phi += 16*180;
        Arcs.append(new struct Arc(x, (x2+y)>>1, y, y, phi, len, GridPen));
      }
    }
  }


  // ....................................................
  if(Axis->up > 1.0) {  // draw circle with |r|=1 ?
    x = (x2-R1) >> 1;
    y = (x2+R1) >> 1;
    Arcs.append(new struct Arc(x, y, R1, R1, beta, theta, QPen(Qt::black,0)));

    // vertical line Re(r)=1 (visible only if |r|>1)
    if(Zplane)  x = y;
    y = int(sqrt(rMAXq-1)/Axis->up*dx2 + 0.5);
    if(Above)  m = y;
    else  m = 0;
    if(!Below)  y = 0;
    Lines.append(new Line(x, dx2+m, x, dx2-y, GridPen));

//    if(Below)  y = 4;
//    else  y = y2-4-QucsSettings.font.pointSize();
    Texts.append(new Text(0, y, misc::StringNum(Axis->up)));
  }

}
#endif

// vim:ts=8:sw=2:noet
 
