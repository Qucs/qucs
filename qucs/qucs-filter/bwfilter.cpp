//==================================================
// bwfilter class (Butterworth)
// Programmed by Toyoyuki Ishikawa, March 2005
//==================================================

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "bwfilter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <qstring.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
 
bwfilter::bwfilter()
{
}

bwfilter::~bwfilter()
{
}

double* bwfilter::solveFilter(int Class, double impedance, int order, double freq)
{
  double Normalize;
  freq *= 2.0*M_PI;   // angular frequency
  impedance /= IMPSTANDARD;

  int j = 0;
  if(Class == CLASS_HIGHPASS)  j++;

  // get memory for filter coefficients
  double *BwPi = (double*) malloc(order * sizeof(double));
  if(!BwPi) return 0;


  // calculate the filter coefficients
  for(int i = 0; i < order; i++) {

      Normalize = 2.0 * sin(double(2*i + 1) / double(2*order) * M_PI);


/*
//  double hhh;
  double a0;
  double s1;
  double sc1, sc2, sc3;
  double sns, si;
double ripple = 1.0;

  a0 = sqrt(pow(10,  (ripple / 10)) - 1);
  a0 = sinh(1 / (double)order * asinh(1 / a0));
  s1 = 2 * (sin(M_PI / (2 * (double)order)) / a0);

  if(i == 0)
    Normalize = s1;
//  for(int i = 1; i <= (numberofparts - 1); i++)
  else {
      sc1 = sin(((2 * i + 1) / ( 2 * (double)order)) * M_PI);
      sc2 = sin(((2 * i - 1) / ( 2 * (double)order)) * M_PI);
      sc3 = pow((sin((i * M_PI) / (double)order)) , 2);
      sns = 4 * ((sc1 * sc2) / (a0 * a0 + sc3));
      si = sns / s1;
      s1 = si;
      Normalize = si;
//      LpfNormalize[i] = si;
    }

*/



      if(Class == CLASS_HIGHPASS)
        Normalize = 1.0 / Normalize;
      Normalize /= freq;

      if((i+j) & 1) {
//	  BwTee[i] = Normalize / impedance;
	  BwPi[i]  = Normalize * impedance;
      }
      else {
//	  BwTee[i] = Normalize * impedance;
	  BwPi[i]  = Normalize / impedance;
      }
  }
  return BwPi;
}

void bwfilter::solveBwBPF(double, int, double)
{
};

void bwfilter::solveBwBRF(double, int, double)
{
};

QString* bwfilter::createSchematic(int Class, double impedance, int order, double frequency)
{
  // calculate the filter coefficients
  double *BwPi = solveFilter(Class, impedance, order, frequency);
  if(!BwPi)  return 0;

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";

  *s += "<Components>\n";
  *s += QString("<Pac P1 1 60 310 18 -26 0 1 \"1\" 1 \"%1 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(impedance);
  *s += "<GND * 1 60 340 0 0 0 0>\n";

  int i, x = 0;
  for(i = 0; i < order; i++) {
    x = 100 +((i+1) * 70);
    int yc = 310;
    int yl = 240;
    if(i & 1) {
      if(Class == CLASS_LOWPASS)
	*s += QString("<L L%1 1 %2 %3 -26 10 0 0 \"%4 nH\" 1>\n").arg(i+1).arg(x).arg(yl).arg(BwPi[i] * 1e9);
      else
	*s += QString("<C C%1 1 %2 %3 -27 10 0 0 \"%4 pF\" 1>\n").arg(i+1).arg(x).arg(yl).arg(BwPi[i] * 1e12);
    }
    else {
      if(Class == CLASS_LOWPASS)
	*s += QString("<C C%1 1 %2 %3 17 -26 0 1 \"%4 pF\" 1>\n").arg(i+1).arg(x).arg(yc).arg(BwPi[i] * 1e12);
      else
	*s += QString("<L L%1 1 %2 %3 17 -26 0 1 \"%4 nH\" 1>\n").arg(i+1).arg(x).arg(yc).arg(BwPi[i] * 1e9);
      *s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x).arg(yc + 30);
    }
  }

  if(order & 1)  x += 110;
  else  x += 70;
  *s += QString("<Pac P2 1 %1 310 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(impedance);
  *s += QString("<GND * 1 %1 340 0 0 0 0>\n").arg(x);

  *s += QString("<.SP SP1 1 150 400 0 60 0 0 \"log\" 1 \"%1 MHz\" 1 \"%2 MHz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(frequency / 1e7).arg(frequency / 1e5);
  *s += "<Eqn Eqn1 1 360 410 -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n";
  *s += "</Components>\n";

  *s += "<Wires>\n";
  for(i = 0; i < (order / 2) + 1; i++)
    {
      x = 170 +(i * 140);
      *s += QString("<%1 240 %2 280 \"\" 0 0 0>\n").arg(x).arg(x);
    }
  *s += "<60 240 60 280 \"\" 0 0 0>\n";
  if(order & 1)
    *s += QString("<%1 240 %2 280 \"\" 0 0 0>\n").arg(x + 110).arg(x + 110);

  for(i = 0; i < (order / 2); i++)
    {
      x = 170 +(i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x + 40);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x + 100).arg(x + 140);
    }
  *s += "<60 240 170 240 \"\" 0 0 0>\n";

  if(order & 1)
    *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x + 140).arg(x + 140 + 110);
  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";
  char *pClass = "HPF";
  if(Class == CLASS_LOWPASS)  pClass = "LPF";
  *s += QString("<Text 160 150 12 #000000 0 \"Butterworth %1 PI-type\\nImpedance matching %2Ohm\\n%3MHz cutoff\">\n").arg(pClass).arg(impedance).arg(frequency / 1e6);
  *s += "</Paintings>\n";

  return s;
}
