/*
 * phaseshifter.cpp - phase shifter class implementation
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
 * $Id: phaseshifter.cpp,v 1.6 2004-09-25 21:09:46 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "phaseshifter.h"

phaseshifter::phaseshifter () : circuit (2) {
  type = CIR_PHASESHIFTER;
}

void phaseshifter::calcSP (nr_double_t) {
  nr_double_t p = rad (getPropertyDouble ("phi"));
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z0 - z) / (z0 + z);
  complex d = 1.0 - polar (r * r, 2 * p);
  complex s11 = r * (polar (1, 2 * p) - 1.0) / d;
  complex s21 = (1.0 - r * r) * polar (1, p) / d;
  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}

void phaseshifter::initDC (void) {
  clearY ();
  voltageSource (1, 1, 2);
  setVoltageSources (1);
}

void phaseshifter::initAC (void) {
  nr_double_t p = rad (getPropertyDouble ("phi"));

  if (p == 0.0) { // no phase shift, thus a short
    initDC ();
  }
  else { // compute Y-parameters directly
    setVoltageSources (0);
    nr_double_t z = getPropertyDouble ("Zref");
    nr_double_t y11 = -1 / z / tan (p);
    nr_double_t y21 = -1 / z / sin (p);
    setY (1, 1, rect (0, y11)); setY (2, 2, rect (0, y11));
    setY (1, 2, rect (0, y21)); setY (2, 1, rect (0, y21));
  }
}
