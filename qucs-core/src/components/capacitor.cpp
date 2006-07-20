/*
 * capacitor.cpp - capacitor class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: capacitor.cpp,v 1.19 2006/07/20 10:47:36 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#define __USE_BSD
#define __USE_XOPEN
#include <stdio.h>
#include <stdlib.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "capacitor.h"

capacitor::capacitor () : circuit (2) {
  type = CIR_CAPACITOR;
  setISource (true);
}

void capacitor::calcSP (nr_double_t frequency) {
  nr_double_t c = getPropertyDouble ("C") * z0;
  complex y = 2 * rect (0, 2.0 * M_PI * frequency * c);
  setS (NODE_1, NODE_1, 1 / (1 + y));
  setS (NODE_2, NODE_2, 1 / (1 + y));
  setS (NODE_1, NODE_2, y / (1 + y));
  setS (NODE_2, NODE_1, y / (1 + y));
}

void capacitor::initDC (void) {
  allocMatrixMNA ();
}

void capacitor::calcAC (nr_double_t frequency) {
  nr_double_t c = getPropertyDouble ("C");
  complex y = rect (0, 2.0 * M_PI * frequency * c);
  setY (NODE_1, NODE_1, +y); setY (NODE_2, NODE_2, +y);
  setY (NODE_1, NODE_2, -y); setY (NODE_2, NODE_1, -y);
}

void capacitor::initAC (void) {
  allocMatrixMNA ();
}

#define qState 0 // charge state
#define cState 1 // current state

void capacitor::initTR (void) {
  setStates (2);
  initDC ();
}

void capacitor::calcTR (nr_double_t) {

  /* if this is a controlled capacitance then do nothing here */
  if (hasProperty ("Controlled")) return;

  nr_double_t c = getPropertyDouble ("C");
  nr_double_t g, i;
  nr_double_t v = real (getV (NODE_1) - getV (NODE_2));

  /* apply initial condition if requested */
  if (getMode () == MODE_INIT) {
    nr_double_t ic = getPropertyDouble ("V");
    if (ic != 0) v = ic;
  }

  setState (qState, c * v);
  integrate (qState, c, g, i);
  setY (NODE_1, NODE_1, +g); setY (NODE_2, NODE_2, +g);
  setY (NODE_1, NODE_2, -g); setY (NODE_2, NODE_1, -g);
  setI (NODE_1 , -i);
  setI (NODE_2 , +i);
}

void capacitor::initHB (void) {
  initAC ();
}

void capacitor::calcHB (nr_double_t frequency) {
  calcAC (frequency);
}
