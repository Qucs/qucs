/*
 * vcvs.cpp - vcvs class implementation
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
 * $Id: vcvs.cpp,v 1.8 2004-09-25 21:09:46 ela Exp $
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
#include "vcvs.h"

vcvs::vcvs () : circuit (4) {
  type = CIR_VCVS;
  setVoltageSources (1);
}

void vcvs::calcSP (nr_double_t frequency) {

  nr_double_t g = getPropertyDouble ("G");
  nr_double_t t = getPropertyDouble ("T");

  complex z1 = polar (g, M_PI - 2.0 * M_PI * frequency * t);
  complex z2 = polar (g, - 2.0 * M_PI * frequency * t);

  setS (1, 1, 1.0);
  setS (1, 2, 0.0);
  setS (1, 3, 0.0);
  setS (1, 4, 0.0);
  setS (2, 1, z2);
  setS (2, 2, 0.0);
  setS (2, 3, 1.0);
  setS (2, 4, z1);
  setS (3, 1, z1);
  setS (3, 2, 1.0);
  setS (3, 3, 0.0);
  setS (3, 4, z2);
  setS (4, 1, 0.0);
  setS (4, 2, 0.0);
  setS (4, 3, 0.0);
  setS (4, 4, 1.0);
}

void vcvs::initDC (void) {
  nr_double_t g = getPropertyDouble ("G");
  setC (1, 1, +g); setC (1, 2, -1.0); setC (1, 3, +1.0); setC (1, 4, -g);
  setB (1, 1, +0); setB (2, 1, -1.0); setB (3, 1, +1.0); setB (4, 1, +0);
  setD (1, 1, 0.0);
  setE (1, 0.0);
}

void vcvs::calcAC (nr_double_t frequency) {
  initDC ();
  nr_double_t t = getPropertyDouble ("T");
  complex g = polar (getPropertyDouble ("G"), - 2.0 * M_PI * frequency * t);
  setC (1, 1, +g); setC (1, 4, -g);
}
