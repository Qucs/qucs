/*
 * ccvs.cpp - ccvs class implementation
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
 * $Id: ccvs.cpp,v 1.3 2004/01/30 21:40:35 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#define __USE_BSD
#define __USE_XOPEN
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "ccvs.h"

ccvs::ccvs () : circuit (4) {
  type = CIR_CCVS;
  setVoltageSources (2);
}

void ccvs::calcS (nr_double_t frequency) {

  nr_double_t g = getPropertyDouble ("G") / z0;
  nr_double_t t = getPropertyDouble ("T");

  complex z1 = polar (g / 2.0, M_PI - 2.0 * M_PI * frequency * t);
  complex z2 = polar (g / 2.0, - 2.0 * M_PI * frequency * t);

  setS (1, 1, 0.0);
  setS (1, 2, 0.0);
  setS (1, 3, 0.0);
  setS (1, 4, 1.0);
  setS (2, 1, z2);
  setS (2, 2, 0.0);
  setS (2, 3, 1.0);
  setS (2, 4, z1);
  setS (3, 1, z1);
  setS (3, 2, 1.0);
  setS (3, 3, 0.0);
  setS (3, 4, z2);
  setS (4, 1, 1.0);
  setS (4, 2, 0.0);
  setS (4, 3, 0.0);
  setS (4, 4, 0.0);
}

void ccvs::calcY (void) {
  setB (1, 1, -1.0); setB (1, 2, +0.0); setB (1, 3, +0.0); setB (1, 4, +1.0); 
  setB (2, 1, +0.0); setB (2, 2, -1.0); setB (2, 3, +1.0); setB (2, 4, +0.0); 
  setC (1, 1, +0.0); setC (1, 2, +1.0); setC (1, 3, -1.0); setC (1, 4, +0.0); 
  setC (2, 1, +1.0); setC (2, 2, +0.0); setC (2, 3, +0.0); setC (2, 4, -1.0); 
  setD (1, - getPropertyDouble ("G"));
  setD (2, 0.0);
  setE (1, 0.0);
  setE (2, 0.0);
}
