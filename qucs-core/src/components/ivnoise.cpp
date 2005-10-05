/*
 * ivnoise.cpp - correlated noise sources class implementation
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
 * $Id: ivnoise.cpp,v 1.1 2005-10-05 11:22:42 raimi Exp $
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
#include "constants.h"
#include "matrix.h"
#include "component_id.h"
#include "ivnoise.h"

#define NODE_I1P 0
#define NODE_I1N 3
#define NODE_V2P 1
#define NODE_V2N 2

ivnoise::ivnoise () : circuit (4) {
  type = CIR_IVNOISE;
  setVoltageSources (1);
}

void ivnoise::initSP (void) {
  allocMatrixS ();
  setS (NODE_I1P, NODE_I1P, 1.0);
  setS (NODE_I1N, NODE_I1N, 1.0);
  setS (NODE_V2P, NODE_V2N, 1.0);
  setS (NODE_V2N, NODE_V2P, 1.0);
}

void ivnoise::calcNoiseSP (nr_double_t frequency) {
  nr_double_t C = getPropertyDouble ("C");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t k = a + c * pow (frequency, e);
  nr_double_t i1 = getPropertyDouble ("i1") / k / kB / T0;
  nr_double_t u2 = getPropertyDouble ("v2") / k / kB / T0;
  nr_double_t ci = C * sqrt (i1 * u2) / 2;

  // entries of source 1
  setN (NODE_I1P, NODE_I1P, +i1 * z0); setN (NODE_I1N, NODE_I1N, +i1 * z0);
  setN (NODE_I1P, NODE_I1N, -i1 * z0); setN (NODE_I1N, NODE_I1P, -i1 * z0);
  // entries of source 2
  setN (NODE_V2P, NODE_V2P, +u2 /4/z0); setN (NODE_V2N, NODE_V2N, +u2 /4/z0);
  setN (NODE_V2P, NODE_V2N, -u2 /4/z0); setN (NODE_V2N, NODE_V2P, -u2 /4/z0);
  // correlation entries
  setN (NODE_I1P, NODE_V2P, +ci); setN (NODE_I1N, NODE_V2N, +ci);
  setN (NODE_I1P, NODE_V2N, -ci); setN (NODE_I1N, NODE_V2P, -ci);
  setN (NODE_V2P, NODE_I1P, +ci); setN (NODE_V2N, NODE_I1N, +ci);
  setN (NODE_V2P, NODE_I1N, -ci); setN (NODE_V2N, NODE_I1P, -ci);
}

void ivnoise::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_V2P, NODE_V2N);
}

void ivnoise::initAC (void) {
  initDC ();
}

void ivnoise::initTR (void) {
  initDC ();
}

void ivnoise::calcNoiseAC (nr_double_t frequency) {
  nr_double_t C = getPropertyDouble ("C");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t k = a + c * pow (frequency, e);
  nr_double_t i1 = getPropertyDouble ("i1") / k / kB / T0;
  nr_double_t u2 = getPropertyDouble ("v2") / k / kB / T0;
  nr_double_t ci = C * sqrt (i1 * u2);
  setN (NODE_I1P, NODE_I1P, +i1); setN (NODE_I1N, NODE_I1N, +i1);
  setN (NODE_I1P, NODE_I1N, -i1); setN (NODE_I1N, NODE_I1P, -i1);
  setN (NODE_5, NODE_I1P, ci);
  setN (NODE_I1P, NODE_5, ci);
  setN (NODE_5, NODE_5, u2);
}
