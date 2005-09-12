/***************************************************************************
                               lc_filter.cpp
                              ---------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Toyoyuki Ishikawa
                           (C) 2005 by Vincent Habchi, F5RCS
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


double BesselCoef [9] [10] = {
{0.5755, 2.1478, 0, 0, 0, 0, 0, 0, 0, 0 },
{0.3374, 0.9705, 2.2034, 0, 0, 0, 0, 0, 0, 0},
{0.2334, 0.6725, 1.0815, 2.2404, 0, 0, 0, 0, 0, 0},
{0.1743, 0.5072, 0.8040, 1.1110, 2.2582, 0, 0, 0, 0, 0},
{0.1365, 0.4002, 0.6392, 0.8538, 1.1126, 2.2645, 0, 0, 0, 0},
{0.1106, 0.3259, 0.5249, 0.7020, 0.8690, 1.1052, 2.2659, 0, 0, 0},
{0.0919, 0.2719, 0.4409, 0.5936, 0.7303, 0.8695, 1.0956, 2.2656, 0, 0},
{0.0780, 0.2313, 0.3370, 0.5108, 0.6306, 0.7407, 0.8639, 1.0863, 2.2649, 0},
{0.0672, 0.1998, 0.3270, 0.4454, 0.5528, 0.6493, 0.7420, 0.8561, 1.0781, 2.2641}};

// Calculate normalized component value for Bessel filter
double LC_Filter::BesselValue(int No, int Order)
{
  return BesselCoef [Order - 2] [No];
}


// Calculate normalized component value for Butterworth filter
double LC_Filter::ButterworthValue(int No, int Order)
{
  return 2.0 * sin(double(2*No + 1) / double(2*Order) * M_PI);
}


// Calculate normalized component value for Tschebyscheff filter
// "Ripple" is in dB !
double LC_Filter::TschebyscheffValue(int No, int Order, double Ripple)
{
  // We must define static variables since g(k+1) is tied to g(k)
  static double ak, gk ;
  double        a, b;

  Ripple = sqrt(pow(10.0,  (Ripple / 10.0)) - 1.0);
  Ripple = sinh(asinh(1.0 / Ripple) / double(Order));

  a = sin(double(2 * No + 1) / double(2 * Order) * M_PI);
  if (No == 0)
    gk = a / Ripple;
  else {
    b   = sin(double(No) * M_PI / double(Order));
    gk *= Ripple * Ripple + b * b;
    gk  = ak * a / gk;
  }
  ak = a;

  return 2.0 * gk;
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
QString* LC_Filter::createSchematic(tFilter *Filter)
{
  double Value, Value2, Omega, Bandwidth;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Omega = (Filter->Frequency2 + Filter->Frequency) / 2.0;
  else {
    Filter->Frequency2 = 0.0;
    Omega = Filter->Frequency;
  }

  Bandwidth = fabs(Filter->Frequency2 - Filter->Frequency) / Omega;
  Omega *= 2.0*M_PI;   // angular frequency

  // create the Qucs schematic
  QString *s = new QString("<Qucs Schematic ");
  *s += PACKAGE_VERSION;
  *s += ">\n";

  int i, x, yc=320, yl;
  x = 20;
  if(Filter->Class != CLASS_BANDPASS)  x += 40;
  *s += "<Components>\n";
  *s += QString("<Pac P1 1 %1 320 18 -26 0 1 \"1\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  for(i = 0; i < Filter->Order; i++) {
    x = 100 +((i+1) * 70);
    yc = 320;
    yl = 240;

    switch(Filter->Type) {
      case TYPE_BESSEL:
        Value = BesselValue(i, Filter->Order);
        break;
      case TYPE_BUTTERWORTH:
        Value = ButterworthValue(i, Filter->Order);
        break;
      case TYPE_TSCHEBYSCHEFF:
        Value = TschebyscheffValue(i, Filter->Order, Filter->Ripple);
        break;
      default:
        Value = 0.0;
        *s = QString("");
        return s;
    }

    // de-normalize
    if(i & 1)  Value *= Filter->Impedance / Omega;
    else  Value /= Filter->Impedance * Omega;


    switch(Filter->Class) {

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
        Value2 = 0.25 / Filter->Frequency / Filter->Frequency2 / M_PI / M_PI / Value;
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
        Value *= 0.5 * fabs(Filter->Frequency2/Filter->Frequency - Filter->Frequency/Filter->Frequency2);
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


  if(Filter->Order & 1)  x += 110;
  else  x += 70;
  *s += QString("<Pac P2 1 %1 320 18 -26 0 1 \"2\" 1 \"%2 Ohm\" 1 \"0 dBm\" 0 \"1 GHz\" 0>\n").arg(x).arg(Filter->Impedance);
  *s += QString("<GND * 1 %1 350 0 0 0 0>\n").arg(x);

  yc += 100;
  Value = Filter->Frequency / 10.0;
  if((Filter->Class == CLASS_BANDPASS) || (Filter->Class == CLASS_BANDSTOP))
    Value2 = 10.0 * Filter->Frequency2;
  else
    Value2 = 10.0 * Filter->Frequency;
  *s += QString("<.SP SP1 1 70 %1 0 50 0 0 \"log\" 1 \"%2Hz\" 1 \"%3Hz\" 1 \"200\" 1 \"no\" 0 \"1\" 0 \"2\" 0>\n").arg(yc).arg(num2str(Value)).arg(num2str(Value2));
  *s += QString("<Eqn Eqn1 1 260 %1 -28 15 0 0 \"dBS21=dB(S[2,1])\" 1 \"dBS11=dB(S[1,1])\" 1 \"yes\" 0>\n").arg(yc+10);
  *s += "</Components>\n";

  *s += "<Wires>\n";

  // connect left source
  x = 20;
  if(Filter->Class != CLASS_BANDPASS)  x += 40;
  *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  *s += QString("<%1 240 170 240 \"\" 0 0 0>\n").arg(x);

  // wires down to shunt components
  for(i = 0; i < (Filter->Order / 2) + 1; i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
  }

  // horizontal wires for series components
  if(Filter->Class == CLASS_BANDPASS) {
    for(i = 0; i < (Filter->Order / 2); i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+20);
      *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x-30).arg(x);
      *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x-30).arg(x);
    }
    if(Filter->Order & 1) {
      *s += QString("<%1 290 %2 290 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
      *s += QString("<%1 350 %2 350 \"\" 0 0 0>\n").arg(x+110).arg(x+140);
    }
  }
  else
    for(i = 0; i < (Filter->Order / 2); i++) {
      x = 170 + (i * 140);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x).arg(x+40);
      *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x+100).arg(x+140);
      if(Filter->Class == CLASS_BANDSTOP) {
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+40).arg(x+40);
        *s += QString("<%1 240 %2 205 \"\" 0 0 0>\n").arg(x+100).arg(x+100);
      }
    }

  // connect right source
  if(Filter->Order & 1) {
    x  += 140 + 110;
    *s += QString("<%1 240 %2 290 \"\" 0 0 0>\n").arg(x).arg(x);
    *s += QString("<%1 240 %2 240 \"\" 0 0 0>\n").arg(x-110).arg(x);
  }
  *s += "</Wires>\n";

  *s += "<Diagrams>\n";
  *s += "</Diagrams>\n";

  *s += "<Paintings>\n";

  *s += QString("<Text 400 %1 12 #000000 0 \"").arg(yc+10);
  switch(Filter->Type) {
    case TYPE_BESSEL:        *s += QString("Bessel "); break;
    case TYPE_BUTTERWORTH:   *s += QString("Butterworth "); break;
    case TYPE_TSCHEBYSCHEFF: *s += QString("Tschebyscheff "); break;
  }

  switch(Filter->Class) {
    case CLASS_LOWPASS:
      *s += QString("low-pass filter\\n%1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_HIGHPASS:
      *s += QString("high-pass filter\\n%1Hz cutoff").arg(num2str(Filter->Frequency)); break;
    case CLASS_BANDPASS:
      *s += QString("band-pass filter\\n%1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
    case CLASS_BANDSTOP:
      *s += QString("band-reject filter\\n%1Hz...%2Hz").arg(num2str(Filter->Frequency)).arg(num2str(Filter->Frequency2)); break;
  }
  *s += QString(", PI-type,\\nimpedance matching %3 Ohm\">\n").arg(Filter->Impedance);
  *s += "</Paintings>\n";

  return s;
}
