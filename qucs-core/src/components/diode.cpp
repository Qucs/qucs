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
 * $Id: diode.cpp,v 1.12 2004/06/04 16:01:47 ela Exp $
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
#include "net.h"
#include "analysis.h"
#include "dcsolver.h"
#include "component_id.h"
#include "constants.h"
#include "diode.h"

diode::diode () : circuit (2) {
  rs = NULL;
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

void diode::initDC (dcsolver * solver) {

  // initialize starting values
  setV (1, 0.0);
  setV (2, 0.9);
  Uprev = real (getV (2) - getV (1));

  // possibly insert series resistance
  nr_double_t Rs = getPropertyDouble ("Rs");
  if (Rs != 0) {
    // create additional circuit if necessary and reassign nodes
    rs = splitResistance (this, rs, solver->getNet (), "Rs", "anode", 2);
    applyResistance (rs, Rs);
  }
  // no series resistance
  else {
    disableResistance (this, rs, solver->getNet (), 2);
  }
}

/* The function fills in the necessary values for all types of analyses
   into the given resistor circuit. */
void diode::applyResistance (circuit * res, nr_double_t Rs) {
  // apply constant MNA entries
  nr_double_t g = 1.0 / Rs;
  res->setY (1, 1, +g); res->setY (2, 2, +g);
  res->setY (1, 2, -g); res->setY (2, 1, -g);

  // apply constant S-Matrix
  nr_double_t r = Rs / z0;
  res->setS (1, 1, r / (r + 2.0)); res->setS (1, 2, 2.0 / (r + 2.0));
  res->setS (2, 2, r / (r + 2.0)); res->setS (2, 1, 2.0 / (r + 2.0));
}

/* This function can be used to create an extra resistor circuit.  If
   the 'res' argument is NULL then the new circuit is created, the
   nodes get re-arranged and it is inserted into the given
   netlist. The given arguments can be explained as follows.
   base:     calling circuit (this)
   res:      additional resistor circuit (can be NULL)
   subnet:   the netlist object
   c:        name of the additional circuit
   n:        name of the inserted (internal) node
   internal: number of new internal node (the orignal external node) */
circuit * diode::splitResistance (circuit * base, circuit * res, net * subnet,
				  char * c, char * n, int internal) {
  int external = (internal == 1) ? 2 : 1;
  if (res == NULL) {
    res = new circuit (2);
    res->setName (createInternal (c, base->getName ()));
    res->setNode (external, base->getNode(internal)->getName ());
    res->setNode (internal, createInternal (n, base->getName ()), 1);
    subnet->insertCircuit (res);
  }
  base->setNode (internal, res->getNode(internal)->getName (), 1);
  return res;
}

/* This function is the counterpart of the above routine.  It removes
   the resistance circuit from the netlist and re-assigns the original
   node. */
void diode::disableResistance (circuit * base, circuit * res, net * subnet,
			       int internal) {
  int external = (internal == 1) ? 2 : 1;
  if (res != NULL) {
    subnet->removeCircuit (res);
    base->setNode (internal, res->getNode(external)->getName (), 0);
  }
}

void diode::calcDC (void) {
  nr_double_t Is = getPropertyDouble ("Is");
  nr_double_t n = getPropertyDouble ("n");
  nr_double_t Ud, Id, Ut, T, gd, Ieq, Ucrit, gtiny;

  T = -K + 26.5;
  Ut = T * kB / Q;
  Ud = real (getV (2) - getV (1));

  // critical voltage necessary for bad start values
  Ucrit = n * Ut * log (n * Ut / M_SQRT2 / Is);
  Uprev = Ud = pnVoltage (Ud, Uprev, Ut * n, Ucrit);

  // tiny derivative for little junction voltage
  gtiny = Ud < - 10 * Ut * n ? Is : 0;

  gd = Is / Ut / n * exp (Ud / Ut / n) + gtiny;
  //fprintf (stderr, "%s: gd=%g, Ud=%g\n", getName(), gd, Ud);
  Id = Is * (exp (Ud / Ut / n) - 1) + gtiny * Ud;
  Ieq = Id - Ud * gd;

  setI (1, +Ieq);
  setI (2, -Ieq);

  setY (1, 1, +gd); setY (2, 2, +gd);
  setY (1, 2, -gd); setY (2, 1, -gd);
}

/* The function limits the forward pn-voltage for each DC iteration in
   order to avoid numerical overflows and thereby improve the
   convergence. */
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
