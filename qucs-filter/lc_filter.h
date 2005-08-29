/***************************************************************************
                                lc_filter.h
                               -------------
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

#ifndef LC_FILTER_H
#define LC_FILTER_H

#define CLASS_LOWPASS   0
#define CLASS_HIGHPASS  1
#define CLASS_BANDPASS  2
#define CLASS_BANDSTOP  3


class QString;

class LC_Filter {
public:
  LC_Filter();
 ~LC_Filter();

  static QString* createSchematic(int, double, int, double, double Frequency2=0.0);

private:
  static QString num2str(double);
  static double  ButterworthValue(int, int);
  static double  ChebichevValue(int, int, double);
};

#endif
