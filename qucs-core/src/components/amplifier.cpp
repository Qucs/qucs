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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: amplifier.cpp,v 1.1 2004-11-01 08:36:00 ela Exp $
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
  setS (1, 1, (z1 - z0) / (z1 + z0));
  setS (1, 2, 0);
  setS (2, 2, (z2 - z0) / (z2 + z0));
  setS (2, 1, 4 * z0 * sqrt (z1 * z2) * g / (z1 + z0) / (z2 + z0));
}

void amplifier::initDC (void) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");
  setY (1, 1, 1 / z1);
  setY (1, 2, 0);
  setY (2, 1, -2 * g / sqrt (z1 * z2));
  setY (2, 2, 1 / z2);
}

void amplifier::initAC (void) {
  initDC ();
}

void amplifier::initTR (void) {
  initDC ();
}
