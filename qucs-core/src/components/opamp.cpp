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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: opamp.cpp,v 1.2 2004/11/10 20:26:37 ela Exp $
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

opamp::opamp () : circuit (3) {
  type = CIR_OPAMP;
  setVoltageSources (1);
}

void opamp::calcSP (nr_double_t) {
  setS (1, 1, 1); setS (1, 2, 0); setS (1, 3, 0);
  setS (3, 1, 0); setS (3, 2, 0); setS (3, 3, 1);
  setS (2, 1, +4 * gv);
  setS (2, 2, -1);
  setS (2, 3, -4 * gv);
}

void opamp::initDC (void) {
  setB (1, 1, 0);  setB (2, 1, 1); setB (3, 1, 0);
  setC (1, 2, -1); setD (1, 1, 0); setE (1, 0);
}

void opamp::calcDC (void) {
  nr_double_t g    = getPropertyDouble ("G");
  nr_double_t uMax = getPropertyDouble ("Umax");
  nr_double_t Uin  = real (getV (1) - getV (3));
  nr_double_t Uout, Gv, Geq;
  gv = g / (1 + sqr (M_PI_2 / uMax * g * Uin));
  Uout = uMax / M_2_PI * atan (M_PI_2 / uMax * g * Uin);
  setC (1, 1, +gv); setC (1, 3, -gv); setC (1, 2, -1 + 1 / gv);
  setY (2, 2,  1 / gv);
  setI (2, getV (2) - Uout / gv);
  //fprintf (stderr, "Uout=%g, gv=%g, Uin=%g, Umax=%g\n", Uout, gv, Uin, uMax);
  setE (1, Uin - Uout / gv);
  //setD (1, 1, getV (2) - Uin * gv);
  //setI (2, Uout - Uin * gv);
}

void opamp::initAC (void) {
  initDC ();
  setC (1, 1, +gv); setC (3, 1, -gv);
}

void opamp::initTR (void) {
  initDC ();
}

void opamp::calcTR (nr_double_t) {
  calcDC ();
}
