/*
 * diode.cpp - diode class implementation
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: diode.cpp,v 1.1 2004-02-01 22:36:03 ela Exp $
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
#include "constants.h"
#include "diode.h"

diode::diode () : circuit (2) {
  type = CIR_DIODE;
}

void diode::calcS (nr_double_t frequency) {
}

void diode::calcY (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t Ud, Id, Ut, T, gd;
  static nr_double_t Idprev = 0.0;

  Is = 1e-15;
  T = 290.0;
  Ut = kB * T / Q;
  Ud = real (getV (1) - getV (2));
  printf ("Ud = %e\n", Ud);
  gd = Is / Ut * exp (Ud / Ut);
  printf ("gd = %e\n", gd);
  Id = Idprev - Ud * gd;
  Idprev = Id;

  setI (1, +Id);
  setI (2, -Id);
  setY (gd);
}
