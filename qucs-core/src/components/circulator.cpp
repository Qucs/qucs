/*
 * circulator.cpp - circulator class implementation
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
 * $Id: circulator.cpp,v 1.4 2004/02/17 15:30:58 ela Exp $
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
#include "circulator.h"

circulator::circulator () : circuit (3) {
  type = CIR_CIRCULATOR;
}

void circulator::calcSP (nr_double_t) {
  nr_double_t z1 = getPropertyDouble ("Z1");
  nr_double_t z2 = getPropertyDouble ("Z2");
  nr_double_t z3 = getPropertyDouble ("Z3");
  nr_double_t r1 = (z0 - z1) / (z0 + z1);
  nr_double_t r2 = (z0 - z2) / (z0 + z2);
  nr_double_t r3 = (z0 - z3) / (z0 + z3);
  nr_double_t d  = 1 - r1 * r2 * r3;
  setS (1, 1, (r2 * r3 - r1) / d);
  setS (2, 2, (r1 * r3 - r2) / d);
  setS (3, 3, (r1 * r2 - r3) / d);
  setS (1, 2, sqrt (z2 / z1) * (z1 + z0) / (z2 + z0) * r3 * (1 - r1 * r1) / d);
  setS (2, 3, sqrt (z3 / z2) * (z2 + z0) / (z3 + z0) * r1 * (1 - r2 * r2) / d);
  setS (3, 1, sqrt (z1 / z3) * (z3 + z0) / (z1 + z0) * r2 * (1 - r3 * r3) / d);
  setS (2, 1, sqrt (z1 / z2) * (z2 + z0) / (z1 + z0) * (1 - r2 * r2) / d);
  setS (1, 3, sqrt (z3 / z1) * (z1 + z0) / (z3 + z0) * (1 - r1 * r1) / d);
  setS (3, 2, sqrt (z2 / z3) * (z3 + z0) / (z2 + z0) * (1 - r3 * r3) / d);
}
