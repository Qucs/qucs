/*
 * mscorner.cpp - microstrip corner class implementation
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
 * $Id: mscorner.cpp,v 1.2 2004/07/31 16:59:15 ela Exp $
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
#include "mscorner.h"

mscorner::mscorner () : circuit (2) {
  type = CIR_MSCORNER;
}

void mscorner::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t W = getPropertyDouble ("W");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");

  /* local variables */
  complex z11, z21;
  nr_double_t L, C, Wh = W/h;

  // check validity
  if ((Wh < 0.2) || (Wh > 6.0)) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for 0.2 <= W/h <= 6.0\n");
  }
  if ((er < 2.36) || (er > 10.4)) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for 2.36 <= er <= 10.4\n");
  }
  if (frequency/h < 1e6) {
    logprint (LOG_STATUS,
	"Model for microstrip corner defined for freq/h[mm] <= 1GHz\n");
  }

  // capacitance in pF
  C = W * ( (10.35 * er + 2.5) * Wh + (2.6 * er + 5.64) );
  // inductance in nH
  L = 220.0 * h * ( 1.0 - 1.35 * exp( -0.18 * pow(Wh, 1.39) ) );

  z21 = rect (0.0, 1e12 / (2*M_PI*frequency*C));
  z11 = rect (0.0, 2e-9*M_PI*frequency*L) + z21;

  matrix z (2);
  z.set (1, 1, z11);
  z.set (1, 2, z21);
  z.set (2, 1, z21);
  z.set (2, 2, z11);
  setMatrixS (ztos (z));
}

void mscorner::calcDC (void) {
  setC (1, 1, +1.0); setC (1, 2, -1.0);
  setB (1, 1, +1.0); setB (2, 1, -1.0);
  setE (1, 0.0);
  setD (1, 1, 0.0);
}
