/*
 * inoise.cpp - noise current source class implementation
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: inoise.cpp,v 1.4 2005/01/17 12:19:02 raimi Exp $
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
#include "constants.h"
#include "component_id.h"
#include "inoise.h"

inoise::inoise () : circuit (2) {
  type = CIR_INOISE;
}

void inoise::initSP (void) {
  allocMatrixS ();
  setS (1, 1, 1.0);
  setS (1, 2, 0.0);
  setS (2, 1, 0.0);
  setS (2, 2, 1.0);
}

void inoise::calcNoiseSP (nr_double_t frequency) {
  nr_double_t i = getPropertyDouble ("i");
  nr_double_t e = getPropertyDouble ("e");
  nr_double_t c = getPropertyDouble ("c");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t ipsd = i / (a + c * pow (frequency, e)) / kB / T0 * z0;
  setN (1, 1, +ipsd); setN (2, 2, +ipsd);
  setN (1, 2, -ipsd); setN (2, 1, -ipsd);
}
