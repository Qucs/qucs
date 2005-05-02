/*
 * vpulse.cpp - pulse voltage source class implementation
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
 * $Id: vpulse.cpp,v 1.4 2005-05-02 06:51:01 raimi Exp $
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
#include "consts.h"
#include "vpulse.h"

vpulse::vpulse () : circuit (2) {
  type = CIR_VPULSE;
  setVSource (true);
  setVoltageSources (1);
}

void vpulse::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vpulse::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  setE (VSRC_1, getPropertyDouble ("U1"));
}

void vpulse::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void vpulse::initTR (void) {
  initDC ();
}

void vpulse::calcTR (nr_double_t t) {
  nr_double_t u1 = getPropertyDouble ("U1");
  nr_double_t u2 = getPropertyDouble ("U2");
  nr_double_t t1 = getPropertyDouble ("T1");
  nr_double_t t2 = getPropertyDouble ("T2");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  nr_double_t ut = 0;

  if (t < t1) { // before pulse
    ut = u1;
  }
  else if (t >= t1 && t < t1 + tr) { // rising edge
    ut = u1 + (u2 - u1) / tr * (t - t1);
  }
  else if (t >= t1 + tr && t < t2 - tf) { // during full pulse
    ut = u2;
  }
  else if (t >= t2 - tf && t < t2) { // falling edge
    ut = u2 + (u1 - u2) / tf * (t - (t2 - tf));
  }
  else { // after pulse
    ut = u1;
  }
  setE (VSRC_1, ut);
}
