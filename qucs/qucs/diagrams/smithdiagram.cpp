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

#include "smithdiagram.h"
#include "misc.h"
// #include "../dialogs/matchdialog.h" // For r2z function
//

namespace{
const double pi = 3.1415926535897932384626433832795029;  /* pi   */
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

}

SmithDiagram::SmithDiagram(int _cx, int _cy, bool ImpMode) : Diagram(_cx, _cy)
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

  *px = *py = float(cx) / 2.0;
}

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
  if(Name.at(0) == 'y')  createSmithChart(&yAxis, 6);
  else  createSmithChart(&yAxis);

  // outer most circle
  Arcs.append(new Arc(0, x2, x2, x2, 0, 16*360, QPen(Qt::black,0)));

  // horizontal line Im(r)=0
  Lines.append(new Line(0, x2>>1, x2, x2>>1, GridPen));

  return 3;
}

// ------------------------------------------------------------
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

QString SmithDiagram::extraMarkerText(Marker const* m) const
{
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
}

// vim:ts=8:sw=2:noet
 
