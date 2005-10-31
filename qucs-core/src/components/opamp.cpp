/*
 * opamp.cpp - operational amplifier class implementation
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
 * $Id: opamp.cpp,v 1.8 2005-10-31 16:15:31 ela Exp $
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
#include "opamp.h"

#define NODE_INM 0
#define NODE_INP 1
#define NODE_OUT 2

opamp::opamp () : circuit (3) {
  type = CIR_OPAMP;
  setVoltageSources (1);
}

void opamp::initSP (void) {
  allocMatrixS ();
  setS (NODE_INP, NODE_INP, 1);
  setS (NODE_INP, NODE_OUT, 0);
  setS (NODE_INP, NODE_INM, 0);
  setS (NODE_INM, NODE_INP, 0);
  setS (NODE_INM, NODE_OUT, 0);
  setS (NODE_INM, NODE_INM, 1);
  setS (NODE_OUT, NODE_INP, +4 * gv);
  setS (NODE_OUT, NODE_OUT, -1);
  setS (NODE_OUT, NODE_INM, -4 * gv);
}

void opamp::initDC (void) {
  allocMatrixMNA ();
  setB (NODE_INP, VSRC_1, 0);
  setB (NODE_OUT, VSRC_1, 1);
  setB (NODE_INM, VSRC_1, 0);
  setC (VSRC_1, NODE_OUT, -1); setD (VSRC_1, VSRC_1, 0); setE (VSRC_1, 0);
}

void opamp::calcDC (void) {
  nr_double_t g    = getPropertyDouble ("G");
  nr_double_t uMax = getPropertyDouble ("Umax");
  nr_double_t Uin  = real (getV (NODE_INP) - getV (NODE_INM));
  nr_double_t Uout = uMax * M_2_PI * atan (Uin * g * M_PI_2 / uMax);
  gv = g / (1 + sqr (M_PI_2 / uMax * g * Uin)) + GMin;
  setC (VSRC_1, NODE_INP, +gv);
  setC (VSRC_1, NODE_INM, -gv);
  setE (VSRC_1, Uin * gv - Uout);
}

void opamp::calcOperatingPoints (void) {
  setOperatingPoint ("g", gv);
}

void opamp::initAC (void) {
  initDC ();
  setC (VSRC_1, NODE_INP, +gv);
  setC (VSRC_1, NODE_INM, -gv);
}

void opamp::initTR (void) {
  initDC ();
}

void opamp::calcTR (nr_double_t) {
  calcDC ();
}
