/*
 * ccvs.cpp - ccvs class implementation
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
 * $Id: ccvs.cpp,v 1.14 2006/02/25 14:42:50 raimi Exp $
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
#include "ccvs.h"

ccvs::ccvs () : circuit (4) {
  type = CIR_CCVS;
  setVoltageSources (2);
}

void ccvs::calcSP (nr_double_t frequency) {

  nr_double_t g = getPropertyDouble ("G") / z0;
  nr_double_t t = getPropertyDouble ("T");

  complex z1 = polar (g / 2.0, M_PI - 2.0 * M_PI * frequency * t);
  complex z2 = polar (g / 2.0, - 2.0 * M_PI * frequency * t);

  setS (NODE_1, NODE_1, 0.0); setS (NODE_1, NODE_2, 0.0);
  setS (NODE_1, NODE_3, 0.0); setS (NODE_1, NODE_4, 1.0);
  setS (NODE_2, NODE_1, z2);  setS (NODE_2, NODE_2, 0.0);
  setS (NODE_2, NODE_3, 1.0); setS (NODE_2, NODE_4, z1);
  setS (NODE_3, NODE_1, z1);  setS (NODE_3, NODE_2, 1.0);
  setS (NODE_3, NODE_3, 0.0); setS (NODE_3, NODE_4, z2);
  setS (NODE_4, NODE_1, 1.0); setS (NODE_4, NODE_2, 0.0);
  setS (NODE_4, NODE_3, 0.0); setS (NODE_4, NODE_4, 0.0);
}

void ccvs::initDC (void) {
  allocMatrixMNA ();
  setB (NODE_1, VSRC_1, +1.0); setB (NODE_2, VSRC_1, +0.0);
  setB (NODE_3, VSRC_1, +0.0); setB (NODE_4, VSRC_1, -1.0);
  setB (NODE_1, VSRC_2, +0.0); setB (NODE_2, VSRC_2, -1.0);
  setB (NODE_3, VSRC_2, +1.0); setB (NODE_4, VSRC_2, +0.0);
  setC (VSRC_1, NODE_1, +0.0); setC (VSRC_1, NODE_2, +1.0);
  setC (VSRC_1, NODE_3, -1.0); setC (VSRC_1, NODE_4, +0.0);
  setC (VSRC_2, NODE_1, +1.0); setC (VSRC_2, NODE_2, +0.0);
  setC (VSRC_2, NODE_3, +0.0); setC (VSRC_2, NODE_4, -1.0);
  setD (VSRC_1, VSRC_1, - getPropertyDouble ("G"));
  setD (VSRC_2, VSRC_2, 0.0);
  setD (VSRC_1, VSRC_2, 0.0); setD (VSRC_2, VSRC_1, 0.0);
  setE (VSRC_1, 0.0);
  setE (VSRC_2, 0.0);
}

void ccvs::initAC (void) {
  initDC ();
}

void ccvs::calcAC (nr_double_t frequency) {
  nr_double_t t = getPropertyDouble ("T");
  complex g = polar (getPropertyDouble ("G"), - 2.0 * M_PI * frequency * t);
  setD (VSRC_1, VSRC_1, -g);
}

void ccvs::initTR (void) {
  nr_double_t t = getPropertyDouble ("T");
  initDC ();
  deleteHistory ();
  if (t > 0.0) {
    setHistory (true);
    initHistory (t);
    setC (VSRC_2, NODE_1, +0.0); setC (VSRC_2, NODE_2, +1.0);
    setC (VSRC_2, NODE_3, -1.0); setC (VSRC_2, NODE_4, +0.0);
    setC (VSRC_1, NODE_1, +1.0); setC (VSRC_1, NODE_2, +0.0);
    setC (VSRC_1, NODE_3, +0.0); setC (VSRC_1, NODE_4, -1.0);
    setD (VSRC_1, VSRC_1, 0.0);
  }
}

void ccvs::calcTR (nr_double_t t) {
  nr_double_t T = getPropertyDouble ("T");
  if (T > 0.0) {
    T = t - T;
    nr_double_t g = getPropertyDouble ("G");
    nr_double_t i = getJ (VSRC_1, T);
    setE (VSRC_2, g * i);
  }
}
