/*
 * trafo.cpp - trafo class implementation
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: trafo.cpp,v 1.11 2005/04/25 18:46:32 raimi Exp $
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
#include "trafo.h"

trafo::trafo () : circuit (4) {
  type = CIR_TRAFO;
  setVoltageSources (1);
}

void trafo::initSP (void) {
  nr_double_t  t = getPropertyDouble ("T");
  nr_double_t  d = t * t + 1.0;
  nr_double_t z1 = t * t / d;
  nr_double_t z2 = t / d;
  nr_double_t z3 = 1 / d;
  allocMatrixS ();
  setS (1, 1,  z1); setS (1, 2,  z2); setS (1, 3, -z2); setS (1, 4,  z3);
  setS (2, 1,  z2); setS (2, 2,  z3); setS (2, 3,  z1); setS (2, 4, -z2);
  setS (3, 1, -z2); setS (3, 2,  z1); setS (3, 3,  z3); setS (3, 4,  z2);
  setS (4, 1,  z3); setS (4, 2, -z2); setS (4, 3,  z2); setS (4, 4,  z1);
}

void trafo::initDC (void) {
  nr_double_t t = getPropertyDouble ("T");
  allocMatrixMNA ();
  setB (1, 1, -1); setB (2, 1, +t); setB (3, 1, -t); setB (4, 1, +1);
  setC (1, 1, +1); setC (1, 2, -t); setC (1, 3, +t); setC (1, 4, -1);
  setD (1, 1, +0);
  setE (1, 0);
}

void trafo::initAC (void) {
  initDC ();
}

void trafo::initTR (void) {
  initDC ();
}
