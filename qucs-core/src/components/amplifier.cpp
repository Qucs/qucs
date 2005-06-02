/*
 * amplifier.cpp - amplifier class implementation
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: amplifier.cpp,v 1.4 2005/06/02 18:17:51 raimi Exp $
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
#include "constants.h"
#include "amplifier.h"

amplifier::amplifier () : circuit (2) {
  type = CIR_AMPLIFIER;
}

void amplifier::initSP (void) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");
  allocMatrixS ();
  setS (NODE_1, NODE_1, (z1 - z0) / (z1 + z0));
  setS (NODE_1, NODE_2, 0);
  setS (NODE_2, NODE_2, (z2 - z0) / (z2 + z0));
  setS (NODE_2, NODE_1, 4 * z0 * sqrt (z1 * z2) * g / (z1 + z0) / (z2 + z0));
}

void amplifier::initDC (void) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");
  allocMatrixMNA ();
  setY (NODE_1, NODE_1, 1 / z1);
  setY (NODE_1, NODE_2, 0);
  setY (NODE_2, NODE_1, -2 * g / sqrt (z1 * z2));
  setY (NODE_2, NODE_2, 1 / z2);
}

void amplifier::initAC (void) {
  initDC ();
}

void amplifier::initTR (void) {
  initDC ();
}
