/*
 * tline.cpp - ideal transmission line class implementation
 *
 * Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: tline.cpp,v 1.11 2006-02-22 11:43:57 raimi Exp $
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
#include "tline.h"

tline::tline () : circuit (2) {
  type = CIR_TLINE;
}

void tline::calcSP (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t b = 2 * M_PI * frequency / C0;
  complex p = exp (-l * rect (a, b));
  complex s11 = r * (1 - p * p) / (1 - p * p * r * r);
  complex s21 = p * (1 - r * r) / (1 - p * p * r * r);
  setS (NODE_1, NODE_1, s11); setS (NODE_2, NODE_2, s11);
  setS (NODE_1, NODE_2, s21); setS (NODE_2, NODE_1, s21);
}

void tline::initDC (void) {
  setVoltageSources (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void tline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void tline::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t a = getPropertyDouble ("Alpha");
  nr_double_t b = 2 * M_PI * frequency / C0;
  complex y11 = +1 / z / tanh (rect (a, b) * l);
  complex y21 = -1 / z / sinh (rect (a, b) * l);
  setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
  setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
}
