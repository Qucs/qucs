/*
 * coupler.cpp - ideal coupler class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: coupler.cpp,v 1.1 2006/01/04 10:40:33 raimi Exp $
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
#include "coupler.h"

coupler::coupler () : circuit (4) {
  type = CIR_COUPLER;
}

void coupler::initSP (void) {
  allocMatrixS ();
  nr_double_t k = getPropertyDouble ("k");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t p = rad (getPropertyDouble ("phi"));
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t k2 = k * k;
  nr_double_t r2 = r * r;
  complex a = k2 * (polar (1, 2 * p) + 1);
  complex b = r2 * (1 - a);
  complex c = k2 * (polar (1, 2 * p) - 1);
  complex d = 1 - 2 * r2 * (1 + c) + b * b;
  complex s = r * (a * b + c + polar (2 * r2 * k2, 2 * p)) / d;
  setS (NODE_1, NODE_1, s); setS (NODE_2, NODE_2, s);
  setS (NODE_3, NODE_3, s); setS (NODE_4, NODE_4, s);
  s = sqrt (1 - k2) * (1 - r2) * (1 - b) / d;
  setS (NODE_1, NODE_2, s); setS (NODE_2, NODE_1, s);
  setS (NODE_3, NODE_4, s); setS (NODE_4, NODE_3, s);
  s = polar (k, p) * (1 - r2) * (1 - b) / d;
  setS (NODE_1, NODE_3, s); setS (NODE_3, NODE_1, s);
  setS (NODE_2, NODE_4, s); setS (NODE_4, NODE_2, s);
  s = 2 * sqrt (1 - k2) * polar (k, p) * r * (1 - r2) / d;
  setS (NODE_1, NODE_4, s); setS (NODE_4, NODE_1, s);
  setS (NODE_2, NODE_3, s); setS (NODE_3, NODE_2, s);
}

void coupler::initDC (void) {
  setVoltageSources (2);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_4);
  voltageSource (VSRC_2, NODE_2, NODE_3);
}

void coupler::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  nr_double_t k = getPropertyDouble ("k");
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t p = rad (getPropertyDouble ("phi"));
  nr_double_t b = 2 * sqrt (1 - k * k);
  complex a = k * k * (polar (1, 2 * p) + 1);
  complex c = polar (2 * k, p);
  complex d = z * (a * a - c * c);
  complex y;
  y = a * (2 - a) / d;
  setY (NODE_1, NODE_1, y); setY (NODE_2, NODE_2, y);
  setY (NODE_3, NODE_3, y); setY (NODE_4, NODE_4, y);
  y = -a * b / d;
  setY (NODE_1, NODE_2, y); setY (NODE_2, NODE_1, y);
  setY (NODE_3, NODE_4, y); setY (NODE_4, NODE_3, y);
  y = c * (a - 2) / d;
  setY (NODE_1, NODE_3, y); setY (NODE_3, NODE_1, y);
  setY (NODE_2, NODE_4, y); setY (NODE_4, NODE_2, y);
  y = b * c / d;
  setY (NODE_1, NODE_4, y); setY (NODE_4, NODE_1, y);
  setY (NODE_2, NODE_3, y); setY (NODE_3, NODE_2, y);
}

void coupler::initTR (void) {
  initDC ();
}
