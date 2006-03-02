/*
 * relais.cpp - time controlled switch class implementation
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
 * $Id: relais.cpp,v 1.1 2006/03/02 08:06:03 raimi Exp $
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
#include "vector.h"
#include "component_id.h"
#include "consts.h"
#include "relais.h"

relais::relais () : circuit (4) {
  type = CIR_RELAIS;
  setVoltageSources (1);
}

void relais::initSP (void) {
  allocMatrixS ();
}

void relais::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_2, NODE_3);
  state = 0;
}

#define REAL_OFF 0
#define REAL_ON  1
#define HYST_OFF 2
#define HYST_ON  3

void relais::calcDC (void) {
  nr_double_t von  = getPropertyDouble ("Von");
  nr_double_t voff = getPropertyDouble ("Voff");
  nr_double_t ron  = getPropertyDouble ("Ron");
  nr_double_t roff = getPropertyDouble ("Roff");
  nr_double_t v = real (getV (NODE_1) - getV (NODE_4));
  nr_double_t r = 0;
  if (state == REAL_OFF) {
    if (v >= von) {
      state = REAL_ON;
    }
  } else if (state == REAL_ON) {
    if (v <= voff) {
      state = REAL_OFF;
    }
  }
  if (state == REAL_ON) {
    r = ron;
  } else if (state == REAL_OFF) {
    r = roff;
  }
  setD (VSRC_1, VSRC_1, r);
}

void relais::initAC (void) {
  initDC ();
}

void relais::initTR (void) {
  initDC ();
}

void relais::calcTR (nr_double_t t) {
  calcDC ();
}
