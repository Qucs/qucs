/*
 * cpwline.cpp - coplanar waveguide line class implementation
 *
 * Copyright (C) 2004 Vincent Habchi, F5RCS <10.50@free.fr>
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
 * $Id: cpwline.cpp,v 1.1 2004/11/29 19:04:03 raimi Exp $
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
#include "cpwline.h"

cpwline::cpwline () : circuit (2) {
  type = CIR_CPWLINE;
}

void cpwline::initSP (void) {

  // allocate S-parameter matrix
  allocMatrixS ();

  // get properties of substrate and corner
  nr_double_t W = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h = subst->getPropertyDouble ("h");
}

void cpwline::calcSP (nr_double_t frequency) {
}

void cpwline::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (1, 1, 2);
}

void cpwline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void cpwline::calcAC (nr_double_t frequency) {
}
