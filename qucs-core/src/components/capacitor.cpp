/*
 * capacitor.cpp - capacitor class implementation
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
 * $Id: capacitor.cpp,v 1.10 2004-09-14 19:33:09 ela Exp $
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
}

void capacitor::calcSP (nr_double_t frequency) {
  nr_double_t c = getPropertyDouble ("C") * z0;
  complex y = 2 * rect (0, 2.0 * M_PI * frequency * c);
  setS (1, 1, 1 / (1 + y));
  setS (2, 2, 1 / (1 + y));
  setS (1, 2, y / (1 + y));
  setS (2, 1, y / (1 + y));
}

void capacitor::initDC (void) {
  clearI ();
  clearY ();
}

void capacitor::calcAC (nr_double_t frequency) {
  nr_double_t c = getPropertyDouble ("C");
  complex y = rect (0, 2.0 * M_PI * frequency * c);
  setY (1, 1, +y); setY (2, 2, +y);
  setY (1, 2, -y); setY (2, 1, -y);
}

void capacitor::initAC (void) {
  clearI ();
}

#define qState 0 // charge state
#define cState 1 // current state

void capacitor::initTR (void) {
  setStates (2);
}

void capacitor::calcTR (nr_double_t t) {
  nr_double_t c = getPropertyDouble ("C");
  nr_double_t g, i;
  nr_double_t v = real (getV (1) - getV (2));

  if (t == 0.0) { // initial transient guess
    fillState (qState, v * c);
    integrate (qState, c, g, i);
    fillState (cState, getState (cState));
  }

  //fprintf (stderr, "vcap = %g\n", v);
  setState (qState, v * c);
  integrate (qState, c, g, i);
  setY (1, 1, +g); setY (2, 2, +g);
  setY (1, 2, -g); setY (2, 1, -g);
  setI (1 , -i);
  setI (2 , +i);
}
