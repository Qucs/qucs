/***************************************************************************
                               lc_filter.cpp
                              ---------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Toyoyuki Ishikawa
                           (C) 2005 by Vincent
                           (C) 2005 by Michael Margraf
 ***************************************************************************/

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

#include "lc_filter.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <qstring.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
 
LC_Filter::LC_Filter()
{
}

LC_Filter::~LC_Filter()
{
}

// #########################################################################
QString LC_Filter::num2str(double Num)
{
  char c = 0;
  double cal = fabs(Num);
  if(cal > 1e-20) {
    cal = log10(cal) / 3.0;
    if(cal < -0.2)  cal -= 0.98;
    int Expo = int(cal);

    if(Expo >= -5) if(Expo <= 4)
      switch(Expo) {
        case -5: c = 'f'; break;
        case -4: c = 'p'; break;
        case -3: c = 'n'; break;
        case -2: c = 'u'; break;
        case -1: c = 'm'; break;
        case  1: c = 'k'; break;
        case  2: c = 'M'; break;
        case  3: c = 'G'; break;
        case  4: c = 'T'; break;
      }

    if(c)  Num /= pow(10.0, double(3*Expo));
  }

  QString Str = QString::number(Num, 'g', 4);
  if(c)  Str += c;
  
  return Str;
}


// Calculate normalized component value for Butterworth filter
double LC_Filter::ButterworthValue(int No, int Order)
{
  return 2.0 * sin(double(2*No + 1) / double(2*Order) * M_PI);
}


// Calculate normalized component value for Chebichev filter
double LC_Filter::ChebichevValue(int No, int Order, double Ripple)
{
  // We must define static variables since g(k+1) is tied to g(k)
  static double ak, bk, gk ;
  double        a, bk1, bk2 ;

  Ripple = sqrt (Ripple * Ripple - 1.0);
  if (No == 0) {   // first value of new series
    ak = 2.0 * sin (double(2 * No + 1) * M_PI / double(2 * Order));
    gk = ak / (sinh (Ripple / (2 * Order)));
  }
  else {
    a = sin ((2 * No + 1) * M_PI / (2 * Order)) ;
    gk = (2.0 * ak * a) / (bk * gk) ;
    ak = a ;
  }

  bk1 = sinh (Ripple / (2 * Order));
  bk2 = sin ((No + 1) * M_PI / Order);
  bk = bk1 * bk1 + bk2 * bk2;

  return gk ;
}


// ====================================================================
// This is the main function. It creates an LC filter.
// Input parameters:
//       Class      - CLASS_LOWPASS
//                    CLASS_HIGHPASS
//                    CLASS_BANDPASS
//                    CLASS_BANDSTOP
//       Impedance  - input and output reference impedance in ohms
//       Order      - order of the filter
//       Frequency  - corner frequency (lowpass and highpass) or
//                    band start frequency (bandpass and bandstop)
//       Frequency2 - band stop frequency (only for bandpass and bandstop)
QString* LC_Filter::createSchematic(int Class, double Impedance, int Order,
                                    double Frequency, double Frequency2)
{
  double Value, Value2, Omega, Bandwidth;
  if((Class == CLASS_BANDPASS) || (Class == CLASS_BANDSTOP))
    Omega = (Frequency2 + Frequency) / 2.0;
  else
    Omega = Frequency;

  Bandwidth = fabs(Frequency2 - Frequency) / Omega;
  Omega *= 2.0*M_PI;   // angular frequency

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";

  int i, x, yc=320, yl;
  x = 20;
  if(Class != CLASS_BANDPASS)  x += 40;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 320 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  for(i = 0; i < Order; i++) {
    x = 100 +((i+1) * 70);
    yc = 320;
    yl = 240;
    Value = ButterworthValue(i, Order);
//Value = ChebichevValue(i, Order, 0.2);

    // de-normalize
    if(i & 1)  Value *= Impedance / Omega;
    else  Value /= Impedance * Omega;


    switch(Class) {

      case CLASS_LOWPASS:
        if(i & 1)
          *s += QString("<L L%1 1 %2 %3 -26 10 0 0 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<C C%1 1 %2 %3 17 -26 0 1 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_HIGHPASS:
        Value = 1.0 / Omega / Omega / Value;  // transform to highpass
        if(i & 1)
          *s += QString("<C C%1 1 %2 %3 -27 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value));
        else
          *s += QString("<L L%1 1 %2 %3 17 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value));
        break;


      case CLASS_BANDPASS:
        Value /= Bandwidth;    // transform to bandpass
        Value2 = 0.25 / Frequency / Frequency2 / M_PI / M_PI / Value;
        if(i & 1) {
          *s += QString("<L L%1 1 %2 %3 -26 -44 0 0 \"%4H\" 1>\n").arg(i+1).arg(x+40).arg(yl).arg(num2str(Value));
          *s += QString("<C C%1 1 %2 %3 -26 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x-20).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L%1 1 %2 %3 8 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C%1 1 %2 %3 -8 46 0 1 \"%4F\" 1>\n").arg(i+1).arg(x-30).arg(yc).arg(num2str(Value));
        }
        break;


      case CLASS_BANDSTOP:
        Value2 = 1.0 / Omega / Omega / Bandwidth / Value; // transform to bandstop
        Value *= 0.5 * fabs(Frequency2/Frequency - Frequency/Frequency2);
        if(i & 1) {
          *s += QString("<L L%1 1 %2 %3 -26 -44 0 0 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yl-35).arg(num2str(Value));
          *s += QString("<C C%1 1 %2 %3 -26 10 0 0 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yl).arg(num2str(Value2));
        }
        else {
          *s += QString("<L L%1 1 %2 %3 17 -26 0 1 \"%4H\" 1>\n").arg(i+1).arg(x).arg(yc).arg(num2str(Value2));
          *s += QString("<C C%1 1 %2 %3 17 -26 0 1 \"%4F\" 1>\n").arg(i+1).arg(x).arg(yc+60).arg(num2str(Value));
        }
        yc += 60;
        break;

    }

    if((i & 1) == 0)
      *s += QString("<GND * 1 %1 %2 0 0 0 0>\n").arg(x).arg(yc + 30);

  }


  if(Order & 1)  x += 110;
  else  x += 70;
  *s += QString("<Pac P2 1 %1 320 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  yc += 100;
  Value = Frequency / 10.0;
  if((Class == CLASS_BANDPASS) || (Class == CLASS_BANDSTOP))
    Value2 = 10.0 * Frequency2;
  else
    Value2 = 10.0 * Frequency;
  *s += QString("<.SP SP1 1 70 %1 0 50 0 0 \"log\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(yc).arg(num2str(Value)).arg(num2str(Value2));
  *s += QString("<Eqn Eqn1 1 260 %1 -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n").arg(yc+10);
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  x = 20;
  if(Class != CLASS_BANDPASS)  x += 40;
  *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  *s += QString("<%1 240 170 240 \"\" 0 0 0>\n").arg(x);

  // wires down to shunt components
  for(i = 0; i < (Order / 2) + 1; i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  }

  // horizontal wires for series components
  if(Class == CLASS_BANDPASS) {
    for(i = 0; i < (Order / 2); i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+20);
      *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x-30).arg(x);
      *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x-30).arg(x);
    }
    if(Order & 1) {
      *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
      *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
    }
  }
  else
    for(i = 0; i < (Order / 2); i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+40);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+100).arg(x+140);
      if(Class == CLASS_BANDSTOP) {
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+40).arg(x+40);
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+100).arg(x+100);
      }
    }

  // connect right source
  if(Order & 1) {
    x  += 140 + 110;
    *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
    *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x-110).arg(x);
  }
  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 400 %1 12 #000000 0 \"Butterworth ").arg(yc+10);
  switch(Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter\\n%1Hz cutoff").arg(num2str(Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter\\n%1Hz cutoff").arg(num2str(Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter\\n%1Hz...%2Hz").arg(num2str(Frequency)).arg(num2str(Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-reject filter\\n%1Hz...%2Hz").arg(num2str(Frequency)).arg(num2str(Frequency2)); break;
  }
  *s += QString(", PI-type,\\nimpedance matching %3 Ohm\">\n").arg(Impedance);
  *s += "</Paintings>\n";

  return s;
}
