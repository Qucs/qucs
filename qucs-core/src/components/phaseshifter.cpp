/*
 * phaseshifter.cpp - phase shifter class implementation
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
 * $Id: phaseshifter.cpp,v 1.1 2004/01/13 23:23:01 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

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
#include "phaseshifter.h"

phaseshifter::phaseshifter () : circuit (2) {
  type = CIR_PHASESHIFTER;
}

void phaseshifter::calcS (nr_double_t frequency) {
  nr_double_t p = getPropertyDouble ("phi");
  p = p * M_PI / 180.0;
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z0 - z) / (z0 + z);
  complex d = 1.0 - polar (r * r, 2 * p);
  complex s11 = r * (polar (1, 2 * p) - 1.0) / d;
  complex s21 = (1.0 - r * r) * polar (1, p) / d;
  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}
