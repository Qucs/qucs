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
 * $Id: transient.cpp,v 1.8 2004/09/22 16:47:57 ela Exp $
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

#define COEFFDEBUG 0

// Defines where the equivalent admittance coefficient is going to be stored.
#define COEFF_G 0

/* The function calculates the integration coefficient for numerical
   integration methods.  Supported methods are: Gear (order 1-6),
   Trapezoidal and Euler. */
void calcCorrectorCoeff (int IMethod, int order, nr_double_t * coefficients,
			 nr_double_t * delta, int& charges) {

  tmatrix<nr_double_t> A (order + 1);
  tmatrix<nr_double_t> x (order + 1, 1);
  tmatrix<nr_double_t> b (order + 1, 1);
  eqnsys<nr_double_t> e;
  e.setAlgo (ALGO_LU_DECOMPOSITION);

  switch (IMethod) {
  case INTEGRATOR_GEAR: // GEAR order 1 to 6
    {
      int i, r, c;
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

      // vector x consists of b_{-1}, a_{0}, a_{1} ... a_{k-1} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Gear order %d:", order);
      for (i = 1; i <= x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i, 1));
      }
      logprint (LOG_STATUS, "\n");
#endif
      nr_double_t k = x.get (1, 1);
      coefficients[COEFF_G] = 1 / delta[0] / k;
      for (i = 1; i <= order; i++) {
	coefficients[i] = - 1 / delta[0] / k * x.get (i + 1, 1);
      }
      charges = order + 1;
    }
    break;
  case INTEGRATOR_EULER: // FORWARD EULER
    coefficients[COEFF_G] = 1 / delta[0];
    coefficients[1] = - 1 / delta[0];
    charges = 2;
    break;
  case INTEGRATOR_TRAPEZOIDAL: // TRAPEZOIDAL (bilinear)
    coefficients[COEFF_G] = 2 / delta[0];
    coefficients[1] = - 2 / delta[0];
    charges = 2;
    break;
  case INTEGRATOR_ADAMSMOULTON: // ADAMS-MOULTON order 1 to 6
    {
      int i, r, c;
      // right hand side vector
      for (i = 1; i <= order + 1; i++) b.set (i, 1, 1);
      for (i = 2; i <= order + 1; i++) {
	A.set (i, 2, i - 1); // second column
	A.set (2, i, 1);     // second row
      }
      A.set (1, 1, 1);
      for (c = 1; c <= order - 2; c++) {
	nr_double_t entry = -c;
	for (r = 2; r <= order; r++) {
	  A.set (r + 1, c + 3, r * entry);
	  entry *= -c;
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();

      // vector x consists of a_{0}, b_{-1}, b_{0} ... b_{k-2} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Moulton order %d:", order);
      for (i = 1; i <= x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i, 1));
      }
      logprint (LOG_STATUS, "\n");
#endif
      nr_double_t k = x.get (2, 1);
      coefficients[COEFF_G] = 1 / delta[0] / k;
      coefficients[1] = -x.get (1, 1) / delta[0] / k;
      for (i = 2; i <= order; i++) {
	coefficients[i] = -x.get (i + 1, 1) / k;
      }
      charges = 2;
    }
    break;
  }
}

/* The function calculates the integration coefficient for numerical
   integration methods.  Supported methods are: Adams-Bashford (order
   1-6) and Euler. */
void calcPredictorCoeff (int IMethod, int order, nr_double_t * coefficients,
			 nr_double_t * delta) {

  tmatrix<nr_double_t> A (order + 1);
  tmatrix<nr_double_t> x (order + 1, 1);
  tmatrix<nr_double_t> b (order + 1, 1);
  eqnsys<nr_double_t> e;
  e.setAlgo (ALGO_LU_DECOMPOSITION);

  switch (IMethod) {
  case INTEGRATOR_ADAMSBASHFORD: // ADAMS-BASHFORD order 1 to 6
    {
      int i, r, c;
      // right hand side vector
      for (i = 1; i <= order + 1; i++) b.set (i, 1, 1);
      for (i = 2; i <= order + 1; i++) A.set (2, i, 1); // second row
      A.set (1, 1, 1);
      for (c = 1; c <= order - 1; c++) {
	nr_double_t entry = -c;
	for (r = 2; r <= order; r++) {
	  A.set (r + 1, c + 2, r * entry);
	  entry *= -c;
	}
      }
      e.passEquationSys (&A, &x, &b);
      e.solve ();

      // vector x consists of a_{0}, b_{0}, b_{1} ... b_{k-1} right here
#if COEFFDEBUG
      logprint (LOG_STATUS, "DEBUG: Bashford order %d:", order);
      for (i = 1; i <= x.getRows (); i++) {
	logprint (LOG_STATUS, " %g", x.get (i, 1));
      }
      logprint (LOG_STATUS, "\n");
#endif
      coefficients[COEFF_G] = x.get (1, 1);
      for (i = 1; i <= order; i++) {
	coefficients[i] = x.get (i + 1, 1) * delta[0];
      }
    }
    break;
  case INTEGRATOR_EULER: // BACKWARD EULER
    coefficients[COEFF_G] = 1;
    coefficients[1] = delta[0];
    break;
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

// Integrator using the Adams-Moulton coefficients.
void integrateMoulton (integrator * c, int qstate, nr_double_t cap,
		       nr_double_t& geq, nr_double_t& ceq) {
  nr_double_t * coeff = c->getCoefficients ();
  int i, cstate = qstate + 1;
  nr_double_t cur;
  geq = cap * coeff[COEFF_G];
  ceq = c->getState (qstate, 1) * coeff[1];
  for (i = 2; i <= c->getOrder (); i++) {
    ceq += c->getState (cstate, i - 1) * coeff[i];
  }
  cur = c->getState (qstate) * coeff[COEFF_G] + ceq;
  c->setState (cstate, cur);
}

/* The function applies the appropriate integration function to the
   given circuit object. */
void setIntegrationMethod (circuit * c, int IMethod) {
  switch (IMethod) {
  case INTEGRATOR_GEAR:
    c->setIntegration (integrateGear);
    break;
  case INTEGRATOR_TRAPEZOIDAL:
    c->setIntegration (integrateBilinear);
    break;
  case INTEGRATOR_EULER:
    c->setIntegration (integrateEuler);
    break;
  case INTEGRATOR_ADAMSMOULTON:
    c->setIntegration (integrateMoulton);
    break;
  default:
    c->setIntegration (NULL);
    break;
  }
}

/* Returns an appropriate integrator type identifier and the maximum
   order depending on the given string argument. */
int integratorType (char * IMethod, int& MaxOrder) {
  if (!strcmp (IMethod, "Gear")) {
    if (MaxOrder > 6) MaxOrder = 6;
    return INTEGRATOR_GEAR;
  }
  else if (!strcmp (IMethod, "Trapezoidal")) {
    if (MaxOrder > 2) MaxOrder = 2;
    return INTEGRATOR_TRAPEZOIDAL;
  }
  else if (!strcmp (IMethod, "Euler")) {
    if (MaxOrder > 1) MaxOrder = 1;
    return INTEGRATOR_EULER;
  }
  else if (!strcmp (IMethod, "AdamsMoulton")) {
    if (MaxOrder > 6) MaxOrder = 6;
    return INTEGRATOR_ADAMSMOULTON;
  }
  else if (!strcmp (IMethod, "AdamsBashford")) {
    if (MaxOrder > 6) MaxOrder = 6;
    return INTEGRATOR_ADAMSBASHFORD;
  }
  return INTEGRATOR_UNKNOWN;
}

// Structure defining integration algorithm for each possible order.
struct integration_types_t {
  int IMethod;
  int correctorType[6];
};

static struct integration_types_t integration_types[] = {
  { INTEGRATOR_EULER,
    { INTEGRATOR_EULER }
  },
  { INTEGRATOR_TRAPEZOIDAL,
    { INTEGRATOR_EULER, INTEGRATOR_TRAPEZOIDAL }
  },
  { INTEGRATOR_GEAR,
    { INTEGRATOR_GEAR, INTEGRATOR_GEAR, INTEGRATOR_GEAR,
      INTEGRATOR_GEAR, INTEGRATOR_GEAR, INTEGRATOR_GEAR }
  },
  { INTEGRATOR_ADAMSMOULTON,
    { INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON,
      INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON,
      INTEGRATOR_ADAMSMOULTON, INTEGRATOR_ADAMSMOULTON }
  },
  { INTEGRATOR_ADAMSBASHFORD,
    { INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD,
      INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD,
      INTEGRATOR_ADAMSBASHFORD, INTEGRATOR_ADAMSBASHFORD }
  }
};

/* The function returns the appropriate integration type for the given
   integration type and order. */
int integratorType (int IMethod, int order) {
  return integration_types[IMethod].correctorType[order - 1];
}
