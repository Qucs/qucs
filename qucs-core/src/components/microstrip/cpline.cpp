/*
 * cpline.cpp - coplanar line class implementation
 *
 * Copyright (C) 2004 Vincent Habchi, F5RCS <10.50@free.fr>
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
 * $Id: cpline.cpp,v 1.2 2004-12-01 20:23:47 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "matrix.h"
#include "cpline.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
# define isnan(x)  _isnan(x)
# define isinf(x)  (!_finite(x) && !_isnan(x))
#endif

cpline::cpline () : circuit (2) {
  type = CIR_CPLINE;
}

void cpline::initSP (void) {

  // allocate S-parameter matrix
  allocMatrixS ();

  // get properties of substrate and corner
  nr_double_t W = getPropertyDouble ("W");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h = subst->getPropertyDouble ("h");
}

void cpline::calcSP (nr_double_t frequency) {
}

void cpline::initDC (void) {
  // a DC short (voltage source V = 0 volts)
  setVoltageSources (1);
  setInternalVoltageSource (1);
  allocMatrixMNA ();
  clearY ();
  voltageSource (1, 1, 2);
}

void cpline::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
}

void cpline::calcAC (nr_double_t frequency) {
}

/* The function computes the complete elliptic integral of first kind
   K() and the second kind E() using the arithmetic-geometric mean
   algorithm (AGM) by Abramowitz and Stegun. */
void cpline::ellipke (nr_double_t arg, nr_double_t &k, nr_double_t &e) {
  int iMax = 16;
  if (arg == 1.0) {
    k = DBL_MAX / DBL_MIN; // infinite
    e = 0;
  }
  else if (isinf (arg) && arg < 0) {
    k = 0;
    e = DBL_MAX / DBL_MIN; // infinite
  }
  else {
    nr_double_t a, b, c, f, s, fk = 1, fe = 1, t, da = arg;
    int i;
    if (arg < 0) {
      fk = 1 / sqrt (1 - arg);
      fe = sqrt (1 - arg);
      da = -arg / (1 - arg);
    }
    a = 1;
    b = sqrt (1 - da);
    c = sqrt (da);
    f = 0.5;
    s = f * c * c;
    for (i = 0; i < iMax; i++) {
      t = (a + b) / 2;
      c = (a - b) / 2;
      b = sqrt (a * b);
      a = t;
      f *= 2;
      s += f * c * c;
      if (c / a < 1e-16) break;
    }
    if (i >= iMax) {
      k = 0; e = 0;
    }
    else {
      k = M_PI_2 / a;
      e = M_PI_2 * (1 - s) / a;
      if (arg < 0) {
	k *= fk;
	e *= fe;
      }
    }
  }
}
