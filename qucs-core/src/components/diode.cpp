/*
 * diode.cpp - diode class implementation
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
 * $Id: diode.cpp,v 1.9 2004/05/20 18:06:33 ela Exp $
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

void diode::calcSP (nr_double_t frequency) {
  complex z = getOperatingPoint ("gd");
  z += rect (0, getOperatingPoint ("Cd") * 2.0 * M_PI * frequency);
  z = 1.0 / z / z0; 
  setS (1, 1,   z / (z + 2.0));
  setS (2, 2,   z / (z + 2.0));
  setS (1, 2, 2.0 / (z + 2.0));
  setS (2, 1, 2.0 / (z + 2.0));
}

void diode::initDC (void) {
  setV (1, 0.0);
  setV (2, 0.9);
  lastU = 0.9;
}

void diode::calcDC (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t n = getPropertyDouble ("n");
  nr_double_t Ud, Id, Ut, T, gd, Ieq, Ucrit, gtiny;

  T = -K + 26.5;
  Ut = kB * T / Q;
  Ud = real (getV (2) - getV (1));

  // critical voltage necessary for bad start values
  Ucrit = n * Ut * log (Ut / M_SQRT2 / Is);
  lastU = Ud = pnVoltage (Ud, lastU, Ut / n, Ucrit);

  // tiny derivative for little junction voltage
  gtiny = Ud < - 10 * Ut * n ? Is : 0;

  gd = Is / Ut / n * exp (Ud / Ut / n) + gtiny;
  fprintf (stderr, "%s: gd=%g, Ud=%g\n", getName(), gd, Ud);
  Id = Is * (exp (Ud / Ut / n) - 1) + gtiny * Ud;
  Ieq = Id - Ud * gd;

  setI (1, +Ieq);
  setI (2, -Ieq);

  setY (1, 1, +gd); setY (2, 2, +gd);
  setY (1, 2, -gd); setY (2, 1, -gd);
}

nr_double_t diode::pnVoltage (nr_double_t Ud, nr_double_t Uold, nr_double_t Ut,
			      nr_double_t Ucrit) {
  nr_double_t arg;
  if (Ud > Ucrit && fabs (Ud - Uold) > 2 * Ut) {
    if (Uold > 0) {
      arg = 1 + (Ud - Uold) / Ut;
      Ud = arg > 0 ? Uold + Ut * log (arg) : Ucrit;
    }
    else Ud = Ut * log (Ud / Ut);
  }
  return Ud;
}

void diode::calcOperatingPoints (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t n = getPropertyDouble ("n");
  nr_double_t z = getPropertyDouble ("z");
  nr_double_t cj0 = getPropertyDouble ("Cj0");
  nr_double_t vd = getPropertyDouble ("Vd");
  
  nr_double_t Ud, Id, Ut, T, gd, Cd;

  T = -K + 26.5;
  Ut = kB * T / Q;
  Ud = real (getV (2) - getV (1));
  gd = Is / Ut / n * exp (Ud / Ut / n);
  Id = Is * (exp (Ud / Ut / n) - 1);

  if (Ud < 0)
    Cd = cj0 * pow (1 - Ud / vd, -z);
  else
    Cd = cj0 * (1 + z * Ud / vd);

  //fprintf (stderr, "gd=%g, Ud=%g, Id=%g, Cd=%g\n", gd, Ud, Id, Cd);
  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Ud", Ud);
  setOperatingPoint ("Cd", Cd);
}
