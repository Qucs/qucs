/*
 * device.cpp - device class implementation
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
 * $Id: device.cpp,v 1.3 2004/07/10 14:45:27 ela Exp $
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
#include "constants.h"
#include "device.h"

/* The function fills in the necessary values for all types of
   analyses into the given arbitrary resistor circuit. */
void applyResistance (circuit * res, nr_double_t Rs) {
  // apply constant MNA entries
  nr_double_t g = 1.0 / Rs;
  res->setY (1, 1, +g); res->setY (2, 2, +g);
  res->setY (1, 2, -g); res->setY (2, 1, -g);

  // apply constant S-Matrix
  nr_double_t r = Rs / circuit::z0;
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
circuit * splitResistance (circuit * base, circuit * res, net * subnet,
			   char * c, char * n, int internal) {
  if (res == NULL) {
    res = new circuit (2);
    res->setName (circuit::createInternal (c, base->getName ()));
    res->setNode (1, base->getNode(internal)->getName ());
    res->setNode (2, circuit::createInternal (n, base->getName ()), 1);
    subnet->insertCircuit (res);
  }
  base->setNode (internal, res->getNode(2)->getName (), 1);
  return res;
}

/* This function is the counterpart of the above routine.  It removes
   the resistance circuit from the netlist and re-assigns the original
   node. */
void disableResistance (circuit * base, circuit * res, net * subnet,
			int internal) {
  if (res != NULL) {
    subnet->removeCircuit (res);
    base->setNode (internal, res->getNode(1)->getName (), 0);
  }
}

/* The function limits the forward pn-voltage for each DC iteration in
   order to avoid numerical overflows and thereby improve the
   convergence. */
nr_double_t pnVoltage (nr_double_t Ud, nr_double_t Uold,
		       nr_double_t Ut, nr_double_t Ucrit) {
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

// The function computes the exponential pn-junction current.
nr_double_t pnCurrent (nr_double_t Upn, nr_double_t Iss, nr_double_t Ute) {
  return Iss * (exp (Upn / Ute) - 1);
}

// The function computes the exponential pn-junction current's derivative.
nr_double_t pnConductance (nr_double_t Upn, nr_double_t Iss, nr_double_t Ute) {
  return Iss * exp (Upn / Ute) / Ute;
}

// Computes pn-junction depletion capacitance.
nr_double_t pnCapacitance (nr_double_t Uj, nr_double_t Cj, nr_double_t Vj,
			   nr_double_t Mj, nr_double_t Fc) {
  nr_double_t C;
  if (Uj <= Fc * Vj)
    C = Cj * exp (-Mj * log (1 - Uj / Vj));
  else
    C = Cj * exp (-Mj * log (1 - Fc)) * 
      (1 + Mj * (Uj - Fc * Vj) / Vj / (1 - Fc));
  return C;
}

/* This function computes the pn-junction depletion capacitance with
   no linearization factor given. */
nr_double_t pnCapacitance (nr_double_t Uj, nr_double_t Cj, nr_double_t Vj,
			   nr_double_t Mj) {
  nr_double_t C;
  if (Uj <= 0)
    C = Cj * exp (-Mj * log (1 - Uj / Vj));
  else
    C = Cj * (1 + Mj * Uj / Vj);
  return C;
}
