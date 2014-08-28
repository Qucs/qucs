/***************************************************************************
  copyright: (C) 2010 by Michael Margraf
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef TL_FILTER_H
#define TL_FILTER_H

#include "filter.h"

struct tSubstrate {
  double er;
  double height;
  double thickness;
  double tand;
  double resistivity;
  double roughness;
  double minWidth, maxWidth;
};

// transmission line filter
class TL_Filter : public Filter {
public:
  TL_Filter();

protected:
  static void calcMicrostrip(tSubstrate*, double, double, double&, double&l);
  static void getMicrostrip(double, double, tSubstrate*, double&, double&);
  static double getMicrostripOpen(double Wh, double er, double er_eff);

  static double dispersionKirschning(double, double, double, double&, double&);
  static void getCoupledMicrostrip
        (double, double, double, tSubstrate*, double&, double&, double&, double&);
  static void sythesizeCoupledMicrostrip
        (double, double, double, tSubstrate*, double&, double&, double&);
};

#endif
