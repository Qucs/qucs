/*
 * cpwgap.cpp - coplanar waveguide gap class implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $Id: cpwgap.cpp,v 1.2 2005-06-02 18:17:55 raimi Exp $
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
#include "cpwgap.h"

cpwgap::cpwgap () : circuit (2) {
  type = CIR_CPWGAP;
}

void cpwgap::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

matrix cpwgap::calcMatrixY (nr_double_t frequency) {

  nr_double_t W = getPropertyDouble ("W");
  nr_double_t g = getPropertyDouble ("G");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");

  // calculate series capacitance
  er = (er + 1) / 2;
  nr_double_t p = g / 4 / W;
  nr_double_t C = 2 * E0 * er * W / M_PI *
    (p - sqrt (1 + p * p) + log ((1 + sqrt (1 + p * p)) / p));

  // build Y-parameter matrix
  complex y11 = rect (0.0, 2.0 * M_PI * frequency * C);
  matrix y (2);
  y.set (0, 0, +y11);
  y.set (0, 1, -y11);
  y.set (1, 0, -y11);
  y.set (1, 1, +y11);
  return y;
}

void cpwgap::initDC (void) {
  allocMatrixMNA ();
  clearY ();
}

void cpwgap::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}
