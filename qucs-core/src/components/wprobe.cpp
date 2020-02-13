/*
 * wprobe.cpp - AC/DC and transient watt probe class implementation
 *
 * Copyright (C) 2015, Pedro Macedo
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt
 *
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

//Taking iprobe as basis

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "wprobe.h"

using namespace qucs;

wprobe::wprobe () : circuit (4) {
  type = CIR_WPROBE;
  setProbe (true);
  setVSource (true);
  setVoltageSources (1);
}

// voltmeter nodes must be at nodes 1 and 2 as so it's assumed
//   in saveNoiseResults() for noise calculations
//NODE 1 --> V+
//NODE 2 --> V-
//NODE 3 --> I+
//NODE 4 --> I-

void wprobe::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_3, NODE_4);
}

void wprobe::initAC (void) {
  initDC ();
}

void wprobe::saveOperatingPoints (void) {
  nr_double_t Vr = real (getV (NODE_1) - getV (NODE_2));
  nr_double_t Vi = imag (getV (NODE_1) - getV (NODE_2));
  // operating points as for the vprobe
  setOperatingPoint ("Vr", Vr);
  setOperatingPoint ("Vi", Vi);

  // read current and voltage values to calculate power values
  nr_complex_t Vw = getV (NODE_1) - getV (NODE_2);
  nr_complex_t Iw = getJ (VSRC_1);
  nr_complex_t Sw = Vw * conj (Iw);
  // save P and Q instead of just S since operating points cannot hold complex values
  nr_double_t P = real (Sw);
  nr_double_t Q = imag (Sw);
  setOperatingPoint ("P", P);
  setOperatingPoint ("Q", Q);
}

void wprobe::initTR (void) {
  initDC ();
}

//properties
PROP_REQ [] = { PROP_NO_PROP };
PROP_OPT [] = {
  PROP_NO_PROP };
struct define_t wprobe::cirdef =
  { "WProbe", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
