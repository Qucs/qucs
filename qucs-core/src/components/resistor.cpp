/*
 * resistor.cpp - resistor class implementation
 *
 * Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: resistor.cpp,v 1.19 2005/01/17 12:19:02 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "resistor.h"
#include "constants.h"

resistor::resistor () : circuit (2) {
  type = CIR_RESISTOR;
}

void resistor::calcSP (nr_double_t) {
  // calculate s-parameters
  nr_double_t z = getPropertyDouble ("R") / z0;
  setS (1, 1, z / (z + 2));
  setS (2, 2, z / (z + 2));
  setS (1, 2, 2 / (z + 2));
  setS (2, 1, 2 / (z + 2));
}

void resistor::calcNoiseSP (nr_double_t) {
  // calculate noise correlation matrix
  nr_double_t r = getPropertyDouble ("R");
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t f = kelvin (T) * 4.0 * r * z0 / sqr (2.0 * z0 + r) / T0;
  setN (1, 1, +f); setN (2, 2, +f);
  setN (1, 2, -f); setN (2, 1, -f);
}

void resistor::initDC (void) {
  nr_double_t r = getPropertyDouble ("R");

  // for non-zero resistances usual MNA entries
  if (r != 0.0) {
    nr_double_t g = 1.0 / r;
    setVoltageSources (0);
    allocMatrixMNA ();
    setY (1, 1, +g); setY (2, 2, +g);
    setY (1, 2, -g); setY (2, 1, -g);
  }
  // for zero resistances create a zero voltage source
  else {
    setVoltageSources (1);
    setInternalVoltageSource (1);
    allocMatrixMNA ();
    clearY ();
    voltageSource (1, 1, 2);
  }
}

/* The calcDC() function is here partly implemented again because the
   circuit can be used to simulate controlled non-zero resistances. */
void resistor::calcDC (void) {
  nr_double_t r = getPropertyDouble ("R");

  // for non-zero resistances usual MNA entries
  if (r != 0.0) {
    nr_double_t g = 1.0 / r;
    setY (1, 1, +g); setY (2, 2, +g);
    setY (1, 2, -g); setY (2, 1, -g);
  }
}

void resistor::initAC (void) {
  initDC ();
}

void resistor::calcAC (nr_double_t) {
  calcDC ();
}

void resistor::initTR (void) {
  initDC ();
}

void resistor::calcTR (nr_double_t) {
  calcDC ();
}
