/*
 * transient.cpp - transient helper class implementation
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
 * $Id: transient.cpp,v 1.5 2004/09/17 17:28:24 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "net.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "transient.h"

// Defines where the equivalent admittance coefficient is going to be stored.
#define COEFF_G 0

/* The function calculates the integration coefficient for numerical
   integration methods.  Supported methods are: Gear (order 1-6),
   Trapezoidal and Euler. */
void calcCoefficients (char * IMethod, int order, nr_double_t * coefficients,
		       nr_double_t delta) {

  // GEAR order 1 to 6
  if (!strcmp (IMethod, "Gear")) {
    int i, r, c;
    tmatrix<nr_double_t> A (order + 1);
    tmatrix<nr_double_t> x (order + 1, 1);
    tmatrix<nr_double_t> b (order + 1, 1);

    eqnsys<nr_double_t> e;
    e.setAlgo (ALGO_LU_DECOMPOSITION);

    // right hand side vector
    for (i = 1; i <= order + 1; i++) b.set (i, 1, 1);
    for (i = 2; i <= order + 1; i++) {
      A.set (i, 1, i - 1); // first column
      A.set (1, i, 1);     // first row
    }
    for (c = 1; c <= order - 1; c++) {
      nr_double_t entry = -c;
      for (r = 1; r <= order; r++) {
	A.set (r + 1, c + 2, entry);
	entry *= -c;
      }
    }
    e.passEquationSys (&A, &x, &b);
    e.solve ();

    // vector x consists of b, a0, a1 ... a_n right here
    nr_double_t k = x.get (1, 1);
    coefficients[COEFF_G] = 1 / delta / k;
    for (i = 1; i <= order; i++) {
      coefficients[i] = - 1 / delta / k * x.get (i + 1, 1);
    }
  }
  // EULER
  else if (!strcmp (IMethod, "Euler")) {
    coefficients[COEFF_G] = 1 / delta;
    coefficients[1] = - 1 / delta;
  }
  // TRAPEZOIDAL (bilinear)
  else if (!strcmp (IMethod, "Trapezoidal")) {
    coefficients[COEFF_G] = 2 / delta;
    coefficients[1] = - 2 / delta;
  }
}

// This is the implicit Euler integrator.
void integrateEuler (integrator * c, int qstate, nr_double_t cap,
		     nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1];
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

// Trapezoidal integrator.
void integrateBilinear (integrator * c, int qstate, nr_double_t cap,
			nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1] - c->getState (cstate, 1);
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

// Integrator using the Gear coefficients.
void integrateGear (integrator * c, int qstate, nr_double_t cap,
		    nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int i, cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  for (ceq = 0, i = 1; i <= c->getOrder (); i++) {
    ceq += c->getState (qstate, i) * coeff[i];
  }
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

/* The function applies the appropriate integration function to the
   given circuit object. */
void setIntegrationMethod (circuit * c, char * IMethod) {
  if (!strcmp (IMethod, "Gear"))
    c->setIntegration (integrateGear);
  else if (!strcmp (IMethod, "Trapezoidal"))
    c->setIntegration (integrateBilinear);
  else if (!strcmp (IMethod, "Euler"))
    c->setIntegration (integrateEuler);
}
