/*
 * msstep.cpp - microstrip impedance step class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
 *
 * This is free software; you can redistribute it and/or modify
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
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id: msstep.cpp,v 1.3 2004/08/19 19:44:24 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "msstep.h"
#include "msline.h"

msstep::msstep () : circuit (2) {
  type = CIR_MSSTEP;
  setVoltageSources (1);
}

void msstep::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  char * SModel = getPropertyString ("MSModel");
  char * DModel = getPropertyString ("MSDispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");

  // compute parallel capacitance
  nr_double_t t1 = log10 (er);
  nr_double_t t2 = W1 / W2;
  nr_double_t Cs = sqrt (W1 * W2) *
    (t2 * (10.1 * t1 + 2.33) - 12.6 * t1 - 3.17);

  // compute series inductance
  t1 = log10 (t2);
  t2 = t2 - 1;
  nr_double_t Ls = h * (t2 * (40.5 + 0.2 * t2) - 75 * t1);

  nr_double_t ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;
  msline::analyseQuasiStatic (W1, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W1, h, er, ZlEff, ErEff, frequency, DModel,
			      ZlEffFreq, ErEffFreq);
  nr_double_t L1 = ZlEffFreq * sqrt (ErEffFreq) / C0;

  msline::analyseQuasiStatic (W2, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W2, h, er, ZlEff, ErEff, frequency, DModel,
			      ZlEffFreq, ErEffFreq);
  nr_double_t L2 = ZlEffFreq * sqrt (ErEffFreq) / C0;

  Ls /= (L1 + L2);
  L1 *= Ls;
  L2 *= Ls;

  // build Z-parameter matrix and convert to S-parameters
  complex z21 = rect (0.0, 0.5e12 / (M_PI * frequency * Cs));
  complex z11 = rect (0.0, 2e-9 * M_PI * frequency * L1) + z21;
  complex z22 = rect (0.0, 2e-9 * M_PI * frequency * L2) + z21;
  matrix z (2);
  z.set (1, 1, z11);
  z.set (1, 2, z21);
  z.set (2, 1, z21);
  z.set (2, 2, z22);
  setMatrixS (ztos (z));
}

void msstep::calcDC (void) {
  // a DC short (voltage source V = 0 volts)
  setC (1, 1, +1.0); setC (1, 2, -1.0);
  setB (1, 1, +1.0); setB (2, 1, -1.0);
  setE (1, 0.0);
  setD (1, 1, 0.0);
}
