/*
 * attenuator.cpp - attenuator class implementation
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
 * $Id: attenuator.cpp,v 1.6 2004-04-04 09:11:06 ela Exp $
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
#include "attenuator.h"

attenuator::attenuator () : circuit (2) {
  type = CIR_ATTENUATOR;
  setVoltageSources (2);
}

void attenuator::calcSP (nr_double_t) {
  nr_double_t a = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t r = (z0 - z) / (z0 + z);
  nr_double_t s11 = r * (1 - a * a) / (a * a - r * r);
  nr_double_t s21 = a * (1 - r * r) / (a * a - r * r);
  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}

void attenuator::calcDC (void) {
  nr_double_t a = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Zref");
  nr_double_t z11 = z * (a * a + 1) / (a * a - 1);
  nr_double_t z21 = z * (a * 2) / (a * a - 1);
  setB (1, 1, +1.0); setB (1, 2, +0.0); setB (2, 1, +0.0); setB (2, 2, +1.0);
  setC (1, 1, -1.0); setC (1, 2, +0.0); setC (2, 1, +0.0); setC (2, 2, -1.0); 
  setD (1, 1, +z11); setD (2, 2, +z11); setD (1, 2, +z21); setD (2, 1, +z21);
  setE (1, +0.0); setE (2, +0.0);
}
