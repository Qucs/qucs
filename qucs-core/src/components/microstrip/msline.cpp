/*
 * msline.cpp - microstrip transmission line class implementation
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
 * $Id: msline.cpp,v 1.3 2004-04-25 17:08:59 ela Exp $
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
#include "substrate.h"
#include "constants.h"
#include "msline.h"

msline::msline () : circuit (2) {
  type = CIR_MSLINE;
}

void msline::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. L, W */
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("W"); z = z0;
  nr_double_t r = (z - z0) / (z + z0);
  complex p = polar (1, - 2.0 * M_PI * frequency * l / C);
  complex s11 = r * (1 - p * p) / (1 - p * p - r * r);
  complex s21 = p * (1 - r * r) / (1 - p * p - r * r);

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");

  setS (1, 1, s11);
  setS (2, 2, s11);
  setS (1, 2, s21);
  setS (2, 1, s21);
}
