/*
 * vexp.cpp - exponential voltage source class implementation
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2007 Gunther Kraut <gn.kraut@t-online.de>
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
 * $Id: vexp.cpp,v 1.1 2007/04/15 10:17:51 ela Exp $
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
#include "net.h"
#include "component_id.h"
#include "consts.h"
#include "vexp.h"

vexp::vexp () : circuit (2) {
  type = CIR_VEXP;
  setVSource (true);
  setVoltageSources (1);
}

void vexp::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vexp::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  setE (VSRC_1, getPropertyDouble ("U1"));
}

void vexp::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void vexp::initTR (void) {
  initDC ();
}

void vexp::calcTR (nr_double_t t) {
  nr_double_t u1 = getPropertyDouble ("U1");
  nr_double_t u2 = getPropertyDouble ("U2");
  nr_double_t t1 = getPropertyDouble ("T1");
  nr_double_t t2 = getPropertyDouble ("T2");
  nr_double_t tr = getPropertyDouble ("Tr");
  nr_double_t tf = getPropertyDouble ("Tf");
  nr_double_t ut = 0;
  nr_double_t s  = getNet()->getSrcFactor ();

  if (t <= t1) { // before pulse
    ut = u1;
  }
  else if (t > t1 && t <= t2) { // rising edge
    ut = u1 + (u2 - u1) * (1 - exp (-(t - t1) / tr));
  }
  else { // falling edge
    ut += u1;
    ut += (u2 - u1) * (1 - exp (-(t - t1) / tr));
    ut -= (u2 - u1) * (1 - exp (-(t - t2) / tf));
  }
  setE (VSRC_1, ut * s);
}
