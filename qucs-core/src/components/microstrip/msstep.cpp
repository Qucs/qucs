/*
 * msstep.cpp - microstrip impedance step class implementation
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: msstep.cpp,v 1.1 2004-08-07 10:48:47 margraf Exp $
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
}

void msstep::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t w1 = getPropertyDouble ("W1");
  nr_double_t w2 = getPropertyDouble ("W2");
  char * SModel = getPropertyString ("Model");
  char * DModel = getPropertyString ("DispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");

  /* local variables */
  nr_double_t tmp1  = log (er);
  nr_double_t tmp2  = w1/w2;
  nr_double_t Cs = sqrt (w1*w2) *
		   (tmp2 * (10.1 * tmp1 + 2.33) - 12.6 * tmp1 - 3.17);

  tmp1 = log(tmp2);
  tmp2 = tmp2 - 1;
  nr_double_t Ls = tmp2 * (40.5 + 0.2 * tmp2) - 75 * tmp1;
  Ls *= h;


  nr_double_t ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;
  msline::analyseQuasiStatic (w1, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (w1, h, er, ZlEff, ErEff, frequency, DModel,
				ZlEffFreq, ErEffFreq);
  nr_double_t L1 = ZlEffFreq * sqrt (ErEffFreq) / C0;

  msline::analyseQuasiStatic (w2, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (w2, h, er, ZlEff, ErEff, frequency, DModel,
				ZlEffFreq, ErEffFreq);
  nr_double_t L2 = ZlEffFreq * sqrt (ErEffFreq) / C0;


  Ls /= L1 + L2;
  L1 *= Ls;
  L2 *= Ls;


  complex z21 = rect (0.0, 0.5e12 / (M_PI*frequency*Cs));
  complex z11 = rect (0.0, 2e-9*M_PI*frequency*L1) + z21;
  complex z22 = rect (0.0, 2e-9*M_PI*frequency*L2) + z21;

  matrix z (2);
  z.set (1, 1, z11);
  z.set (1, 2, z21);
  z.set (2, 1, z21);
  z.set (2, 2, z22);
  setMatrixS (ztos (z));
}

void msstep::calcDC (void) {
  setC (1, 1, +1.0); setC (1, 2, -1.0);
  setB (1, 1, +1.0); setB (2, 1, -1.0);
  setE (1, 0.0);
  setD (1, 1, 0.0);
}
