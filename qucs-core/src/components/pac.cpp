/*
 * pac.cpp - AC power source class implementation
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
 * $Id: pac.cpp,v 1.7 2004/10/03 10:30:51 ela Exp $
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
#include "consts.h"
#include "pac.h"

pac::pac () : circuit (2) {
  type = CIR_PAC;
  setISource (true);
}

void pac::calcSP (nr_double_t) {
  nr_double_t z = getPropertyDouble ("Z") / z0;
  setS (1, 1, z / (z + 2));
  setS (2, 2, z / (z + 2));
  setS (1, 2, 2 / (z + 2));
  setS (2, 1, 2 / (z + 2));
}

void pac::calcDC (void) {
  nr_double_t g = 1.0 / getPropertyDouble ("Z");
  clearI ();
  setY (1, 1, +g); setY (2, 2, +g);
  setY (1, 2, -g); setY (2, 1, -g);
}

void pac::calcAC (nr_double_t) {
  nr_double_t p = getPropertyDouble ("P");
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t i = sqrt (p / r);
  calcDC ();
  setI (1, +i); setI (2, -i);
}

void pac::calcTR (nr_double_t t) {
  nr_double_t p = getPropertyDouble ("P");
  nr_double_t r = getPropertyDouble ("Z");
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t i = sqrt (p / r) * sin (2 * M_PI * f * t);
  calcDC ();
  setI (1, +i); setI (2, -i);
}
