/*
 * inductor.cpp - inductor class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: inductor.cpp,v 1.8 2004/09/12 14:09:19 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#define __USE_BSD
#define __USE_XOPEN
#include <stdio.h>
#include <stdlib.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "inductor.h"

inductor::inductor () : circuit (2) {
  type = CIR_INDUCTOR;
}

void inductor::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L") / z0;
  complex z = rect (0, 2.0 * M_PI * frequency * l);
  setS (1, 1, z / (z + 2)); setS (2, 2, z / (z + 2));
  setS (1, 2, 2 / (z + 2)); setS (2, 1, 2 / (z + 2));
}

void inductor::initDC (void) {
  voltageSource (1, 1, 2);
  setVoltageSources (1);  
}

void inductor::calcDC (void) {
  clearY ();
}

void inductor::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");

  // for non-zero inductance usual MNA entries
  if (l != 0.0) {
    setVoltageSources (0);
  }
  // for zero inductance create a zero voltage source
  else {
    initDC ();
    calcDC ();
  }
}

void inductor::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");

  // for non-zero inductance usual MNA entries
  if (l != 0.0) {
    complex y = rect (0, -1 / (2.0 * M_PI * frequency * l));
    setY (1, 1, +y); setY (2, 2, +y);
    setY (1, 2, -y); setY (2, 1, -y);
  }
}

void inductor::initTR (void) {
  initDC ();
  clearY ();
  setStates (2);
}

#define fState 0 // flux state
#define vState 1 // voltage state

void inductor::calcTR (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t r, v;

  integrate (fState, l, r, v);
  setD (1, 1, -r);
  setE (1, v);
}
