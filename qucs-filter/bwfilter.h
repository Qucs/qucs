//=============================================
// bwfilter class header
// Programmed by Toyoyuki Ishikawa, March 2005
//=============================================

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _QUCSBWFILTER_
#define _QUCSBWFILTER_

#define IMPSTANDARD 1.0

#define CLASS_LOWPASS   0
#define CLASS_HIGHPASS  1
#define CLASS_BANDPASS  2
#define CLASS_BANDSTOP  3


class QString;


class bwfilter
{
 public:
  bwfilter();
  ~bwfilter();
  static double* solveFilter(int, double, int, double);

  void solveBwBPF(double, int, double);
  void solveBwBRF(double, int, double);

  static QString* createSchematic(int, double, int, double);
};

#endif
