/***************************************************************************
                        wprobe.cpp  -  description
                             -------------------
    begin                : Sat Oct 30 2004
    copyright            : (C) 2015 by Pedro Macedo
    email                : macedo.pp@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "wprobe.h"

using namespace qucs;

wprobe::wprobe () : circuit (4) {
  type = CIR_WPROBE;
  setProbe(true);
  setVSource (true);
  setVoltageSources (1);
}

// NODE_1 --> I+
// NODE_2 --> I-
// NODE_3 --> V+
// NODE_4 --> V-

void wprobe::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void wprobe::initAC (void) {
  initDC ();
}

void wprobe::saveOperatingPoints (void) {
  nr_double_t Vr = real (getV (NODE_3) - getV (NODE_4));
  nr_double_t Vi = imag (getV (NODE_3) - getV (NODE_4));
  setOperatingPoint ("Vr", Vr);
  setOperatingPoint ("Vi", Vi);
}

void wprobe::calcOperatingPoints (void) {
  nr_double_t VAr = real (getV (NODE_3) * getJ (NODE_1));
  nr_double_t VAi = -imag (getV (NODE_3) * getJ (NODE_1));
  setOperatingPoint ("VAr", VAr);
  setOperatingPoint ("VAi", VAi);

  nr_double_t Wr = VAr;
  setOperatingPoint ("W", Wr);

  nr_double_t var = VAi;
  setOperatingPoint ("var", var);

  nr_double_t alpha = std::atan(VAi/VAr);
  nr_double_t S1 = VAr/std::cos(alpha); 
//  setOperatingPoint ("FP", std::acos(VAr/S1));
  setOperatingPoint ("FP", VAr/S1);
}

void wprobe::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = { 
  { "Ri", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = { PROP_NO_PROP };
struct define_t wprobe::cirdef =
  { "WProbe", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
