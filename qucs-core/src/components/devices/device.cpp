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
 * $Id: device.cpp,v 1.12 2004-10-18 16:21:21 ela Exp $
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
#include "resistor.h"
#include "capacitor.h"

/* This function can be used to create an extra resistor circuit.  If
   the 'res' argument is NULL then the new circuit is created, the
   nodes get re-arranged and it is inserted into the given
   netlist. The given arguments can be explained as follows.
   base:     calling circuit (this)
   res:      additional resistor circuit (can be NULL)
   subnet:   the netlist object
   c:        name of the additional circuit
   n:        name of the inserted (internal) node
   internal: number of new internal node (the original external node) */
circuit * splitResistance (circuit * base, circuit * res, net * subnet,
			   char * c, char * n, int internal) {
  if (res == NULL) {
    res = new resistor ();
    char * name = circuit::createInternal (c, base->getName ());
    char * node = circuit::createInternal (n, base->getName ());
    res->setName (name);
    res->setNode (1, base->getNode(internal)->getName ());
    res->setNode (2, node, 1);
    subnet->insertCircuit (res);
    free (name);
    free (node);
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
    subnet->removeCircuit (res, 0);
    base->setNode (internal, res->getNode(1)->getName (), 0);
  }
}

/* This function creates a new capacitance circuit if the given one is
   not NULL.  The new circuit is connected between the given nodes and
   a name is applied based upon the parents (base) name and the given
   name 'c'.  The circuit is then put into the netlist. */
circuit * splitCapacitance (circuit * base, circuit * cap, net * subnet,
			    char * c, node * n1, node * n2) {
  if (cap == NULL) {
    cap = new capacitor ();
    char * name = circuit::createInternal (c, base->getName ());
    cap->setName (name);
    cap->setNode (1, n1->getName ());
    cap->setNode (2, n2->getName ());
    free (name);
  }
  subnet->insertCircuit (cap);
  return cap;
}

// The function removes the given capacitance circuit from the netlist.
void disableCapacitance (circuit *, circuit * cap, net * subnet) {
  if (cap != NULL) {
    subnet->removeCircuit (cap, 0);
  }
}

/* This function checks whether the given circuit object exists and is
   chained within the current netlist.  It returns non-zero if so and
   zero otherwise. */
int deviceEnabled (circuit * c) {
  if (c != NULL && c->isEnabled ())
    return 1;
  return 0;
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
  nr_double_t c;
  if (Uj <= Fc * Vj)
    c = Cj * exp (-Mj * log (1 - Uj / Vj));
  else
    c = Cj * exp (-Mj * log (1 - Fc)) * 
      (1 + Mj * (Uj - Fc * Vj) / Vj / (1 - Fc));
  return c;
}

// Computes pn-junction depletion charge.
nr_double_t pnCharge (nr_double_t Uj, nr_double_t Cj, nr_double_t Vj,
		      nr_double_t Mj, nr_double_t Fc) {
  nr_double_t q, a, b;
  if (Uj <= Fc * Vj) {
    a = 1 - Uj / Vj;
    b = exp ((1 - Mj) * log (a));
    q = Cj * Vj / (1 - Mj) * (1 - b);
  }
  else {
    a = 1 - Fc;
    b = exp ((1 - Mj) * log (a));
    a = exp ((1 + Mj) * log (a));
    nr_double_t c = 1 - Fc * (1 + Mj);
    nr_double_t d = Fc * Vj;
    nr_double_t e = Vj * (1 - b) / (1 - Mj);
    q = Cj * (e + (c * (Uj - d) + Mj / 2 / Vj * (sqr (Uj) - sqr (d))) / a);
  }
  return q;
}

/* This function computes the pn-junction depletion capacitance with
   no linearization factor given. */
nr_double_t pnCapacitance (nr_double_t Uj, nr_double_t Cj, nr_double_t Vj,
			   nr_double_t Mj) {
  nr_double_t c;
  if (Uj <= 0)
    c = Cj * exp (-Mj * log (1 - Uj / Vj));
  else
    c = Cj * (1 + Mj * Uj / Vj);
  return c;
}

/* This function computes the pn-junction depletion charge with no
   linearization factor given. */
nr_double_t pnCharge (nr_double_t Uj, nr_double_t Cj, nr_double_t Vj,
		      nr_double_t Mj) {
  nr_double_t q;
  if (Uj <= 0)
    q = Cj * Vj / (1 - Mj) * (1 - exp ((1 - Mj) * log (1 - Uj / Vj)));
  else
    q = Cj * Uj * (1 + Mj * Uj / 2 / Vj);
  return q;
}

// Compute critical voltage of pn-junction.
nr_double_t pnCriticalVoltage (nr_double_t Iss, nr_double_t Ute) {
  return Ute * log (Ute / M_SQRT2 / Iss);
}

/* The function limits the forward fet-voltage for each DC iteration
   in order to avoid numerical overflows and thereby improve the
   convergence. */
nr_double_t fetVoltage (nr_double_t Ufet, nr_double_t Uold, nr_double_t Uth) {
  nr_double_t Utsthi = fabs (2 * (Uold - Uth)) + 2.0;
  nr_double_t Utstlo = Utsthi / 2 + 2.0;
  nr_double_t Utox   = Uth + 3.5;
  nr_double_t DeltaU = Ufet - Uold;

  if (Uold >= Uth) { /* FET is on */
    if (Uold >= Utox) {
      if (DeltaU <= 0) { /* going off */
	if (Ufet >= Utox) {
	  if (-DeltaU > Utstlo) {
	    Ufet = Uold - Utstlo;
	  }
	} else {
	  Ufet = MAX (Ufet, Uth + 2);
	}
      } else { /* staying on */
	if (DeltaU >= Utsthi) {
	  Ufet = Uold + Utsthi;
	}
      }
    } else { /* middle region */
      if (DeltaU <= 0) { /* decreasing */
	Ufet = MAX (Ufet, Uth - 0.5);
      } else { /* increasing */
	Ufet = MIN (Ufet, Uth + 4);
      }
    }
  } else { /* FET is off */
    if (DeltaU <= 0) { /* staying off */
      if (-DeltaU > Utsthi) {
	Ufet = Uold - Utsthi;
      } 
    } else { /* going on */
      if (Ufet <= Uth + 0.5) {
	if (DeltaU > Utstlo) {
	  Ufet = Uold + Utstlo;
	}
      } else {
	Ufet = Uth + 0.5;
      }
    }
  }
  return Ufet;
}

/* The function limits the drain-source voltage for each DC iteration
   in order to avoid numerical overflows and thereby improve the
   convergence. */
nr_double_t fetVoltageDS (nr_double_t Ufet, nr_double_t Uold) {
  if (Uold >= 3.5) {
    if (Ufet > Uold) {
      Ufet = MIN (Ufet, 3 * Uold + 2);
    } else if (Ufet < 3.5) {
      Ufet = MAX (Ufet, 2);
    }
  } else {
    if (Ufet > Uold) {
      Ufet = MIN (Ufet, 4);
    } else {
      Ufet = MAX (Ufet, -0.5);
    }
  }
  return Ufet;
}

/* This function calculates the overlap capacitance for MOS based upon
   the given voltages, surface potential and the zero-bias oxide
   capacitance. */
void fetCapacitanceMeyer (nr_double_t Ugs, nr_double_t Ugd, nr_double_t Uth,
			  nr_double_t Udsat, nr_double_t Phi, nr_double_t Cox,
			  nr_double_t& Cgs, nr_double_t& Cgd,
			  nr_double_t& Cgb) {

  nr_double_t Utst = Ugs - Uth;
  if (Utst <= -Phi) { // cutoff regions
    Cgb = Cox;
    Cgs = 0;
    Cgd = 0;
  } else if (Utst <= -Phi / 2) {
    Cgb = -Utst * Cox / Phi;
    Cgs = 0;
    Cgd = 0;
  } else if (Utst <= 0) { // depletion region
    Cgb = -Utst * Cox / Phi;
    Cgs = Utst * Cox * 4 / 3 / Phi + 2 * Cox / 3;
    Cgd = 0;
  } else {
    Cgb = 0;
    nr_double_t Uds = Ugs - Ugd;
    if (Udsat <= Uds) { // saturation region
      Cgs = 2 * Cox / 3;
      Cgd = 0;
    } else { // linear region
      nr_double_t Sqr1 = sqr (Udsat - Uds);
      nr_double_t Sqr2 = sqr (2 * Udsat - Uds);
      Cgs = Cox * (1 - Sqr1 / Sqr2) * 2 / 3;
      Cgd = Cox * (1 - Udsat * Udsat / Sqr2) * 2 / 3;
    }
  }
}
