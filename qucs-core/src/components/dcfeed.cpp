/*
 * dcfeed.cpp - DC feed class implementation
 *
 * Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: dcfeed.cpp,v 1.10 2005-06-02 18:17:51 raimi Exp $
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
#include "dcfeed.h"

dcfeed::dcfeed () : circuit (2) {
  type = CIR_DCFEED;
}

void dcfeed::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
}

void dcfeed::initDC (void) {
  setVoltageSources (1);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
}

void dcfeed::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

#define fState 0 // flux state
#define vState 1 // voltage state

void dcfeed::initTR (void) {
  initDC ();
  setStates (2);
}

void dcfeed::calcTR (nr_double_t) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t r, v;
  nr_double_t i = real (getJ (VSRC_1));

  setState (fState, i * l);
  integrate (fState, l, r, v);
  setD (VSRC_1, VSRC_1, -r);
  setE (VSRC_1, v);
}
