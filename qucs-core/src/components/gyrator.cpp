/*
 * gyrator.cpp - gyrator class implementation
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
 * $Id: gyrator.cpp,v 1.10 2004-11-24 19:15:48 raimi Exp $
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
#include "gyrator.h"

gyrator::gyrator () : circuit (4) {
  type = CIR_GYRATOR;
  setVoltageSources (2);
}

void gyrator::initSP (void) {
  nr_double_t R = getPropertyDouble ("R");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = R / z;
  nr_double_t s1 = r * r / (r * r + 4.0);
  nr_double_t s2 = 2.0 * r / (r * r + 4.0);
  allocMatrixS ();
  setS (1, 1, s1); setS (2, 2, s1); setS (3, 3, s1); setS (4, 4, s1);
  setS (1, 4, 1.0 - s1);
  setS (2, 3, 1.0 - s1);
  setS (3, 2, 1.0 - s1);
  setS (4, 1, 1.0 - s1);
  setS (1, 2, +s2); setS (2, 4, +s2); setS (3, 1, +s2); setS (4, 3, +s2);
  setS (1, 3, -s2); setS (2, 1, -s2); setS (3, 4, -s2); setS (4, 2, -s2);
}

void gyrator::initDC (void) {
  nr_double_t r = getPropertyDouble ("R");
  allocMatrixMNA ();
  setB (1, 1, +1.0); setB (2, 1, +0.0); setB (3, 1, +0.0); setB (4, 1, -1.0);
  setB (2, 1, +0.0); setB (2, 2, +1.0); setB (3, 2, -1.0); setB (4, 2, +0.0);
  setC (1, 1, +0.0); setC (1, 2, +1/r); setC (1, 3, -1/r); setC (1, 4, +0.0);
  setC (2, 1, -1/r); setC (2, 2, +0.0); setC (2, 3, +0.0); setC (2, 4, +1/r);
  setD (1, 1, -1.0); setD (2, 2, -1.0); setD (1, 2, +0.0); setD (2, 1, +0.0);
  setE (1, +0.0);
  setE (2, +0.0);
}

void gyrator::initAC (void) {
  initDC ();
}

void gyrator::initTR (void) {
  initDC ();
}
