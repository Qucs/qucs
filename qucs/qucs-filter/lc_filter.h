/***************************************************************************
                                lc_filter.h
                               -------------
    begin                : Wed Mar 02 2005
    copyright            : (C) 2005 by Toyoyuki Ishikawa
                           (C) 2005 by Vincent
                           (C) 2005 by Michael Margraf
                           (C) 2005 by Stefan Jahn
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef LC_FILTER_H
#define LC_FILTER_H

#define CLASS_LOWPASS   0
#define CLASS_HIGHPASS  1
#define CLASS_BANDPASS  2
#define CLASS_BANDSTOP  3

#define TYPE_BESSEL      0
#define TYPE_BUTTERWORTH 1
#define TYPE_CHEBYSHEV   2

#include <math.h>

#ifdef __MINGW32__
#define atanh(x) (0.5 * log((1.0 + (x)) / (1.0 - (x))))
#define asinh(x) log((x) + sqrt((x) * (x) + 1.0))
#define acosh(x) log((x) + sqrt((x) * (x) - 1.0))
#endif

struct tFilter {
   int Type;
   int Class;
   int Order;
   double Ripple;  // in dB
   double Impedance;
   double Frequency;
   double Frequency2;
};


class QString;

class LC_Filter {
public:
  LC_Filter();
 ~LC_Filter();

  static QString* createSchematic(tFilter*);

private:
  static QString num2str(double);
  static double  BesselValue(int, int);
  static double  ButterworthValue(int, int);
  static double  ChebyshevValue(int, int, double);
};

#endif
