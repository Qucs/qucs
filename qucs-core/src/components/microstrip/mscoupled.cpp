/*
 * mscoupled.cpp - parallel coupled microstrip lines class implementation
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
 * $Id: mscoupled.cpp,v 1.1 2004-08-19 19:44:24 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "substrate.h"
#include "constants.h"
#include "msline.h"
#include "mscoupled.h"

mscoupled::mscoupled () : circuit (4) {
  type = CIR_MSCOUPLED;
}

void mscoupled::calcSP (nr_double_t frequency) {

  // fetch line properties
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t W = getPropertyDouble ("W");
  nr_double_t s = getPropertyDouble ("S");
  char * SModel = getPropertyString ("Model");
  char * DModel = getPropertyString ("DispModel");

  // fetch substrate properties
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");
  nr_double_t tand  = subst->getPropertyDouble ("tand");
  nr_double_t rho   = subst->getPropertyDouble ("rho");
  nr_double_t D     = subst->getPropertyDouble ("D");

  // initialize local variables
  nr_double_t u, g;
  nr_double_t Zle, ErEffe, Zlo, ErEffo;
  ErEffe = ErEffo = er;
  Zle = Zlo = z0;

  // normalized width and gap
  u = W / h;
  g = s / h;

  // HAMMERSTAD and JENSEN
  if (!strcmp (SModel, "Hammerstad")) {
    nr_double_t Zl1, Fe, Fo, t1, a, b, fo, Mu, Alpha, Beta;
    nr_double_t Pe, Po, r, fo1, q, p, n, Psi, Phi, m, Theta;

    // modifying equations for even mode
    m = 0.2175 + pow (4.113 + pow (20.36 / g, 6), -0.251) +
      log (pow (g, 10) / (1 + pow (g / 13.8, 10))) / 123;
    Alpha = 0.5 * exp (-g);
    Psi = 1 + g / 1.45 + pow (g, 2.09) / 3.95;
    Phi = 0.8645 * pow (u, 0.172);
    Pe = Phi / (Psi * (Alpha * pow (u, m) + (1 - Alpha) * pow (u, -m)));

    // modifying equations for odd mode
    n = (1 / 17.7 + exp (-6.424 - 0.76 * log (g) - pow (g / 0.23, 5))) *
      log ((10 + 68.3 * sqr (g)) / (1 + 32.5 * pow (g, 3.093)));
    Beta = 0.2306 + log (pow (g, 10) / (1 + pow (g / 3.73, 10))) / 301.8 +
      log (1 + 0.646 * pow (g, 1.175)) / 5.3;
    Theta = 1.729 + 1.175 * log (1 + 0.627 / (g + 0.327 * pow (g, 2.17)));
    Po = Pe - Theta / Psi * exp (Beta * pow (u, -n) * log (u));

    // further modifying equations
    r = 1 + 0.15 * (1 - exp (1 + sqr (er - 1) / 8.2) / (1 + pow (g, -6)));
    fo1 = 1 - exp (-0.179 * pow (g, 0.15) -
		   log (0.328 * pow (g, r) / log (M_E + pow (g / 7, 2.8))));
    q = exp (-1.366 - g);
    p = exp (-0.745 * pow (g, 0.295)) / cosh (pow (g, 0.68));
    fo = fo1 * exp (p * log (u) + q * sin (M_PI * log (u) / M_LN10));
    Mu = g * exp (-g) + u * (20 + sqr (g)) / (10 + sqr (g));
    t1 = (1 + 10 / Mu);
    msline::HandJ_ab (u, er, a, b);
    Fe = pow (t1, -a * b);
    Fo = fo * Fe;

    // finally compute effective dielectric constants and impedances
    ErEffe = (er + 1) / 2 + (er - 1) / 2 * Fe;
    ErEffo = (er + 1) / 2 + (er - 1) / 2 * Fo;
    Zl1 = Z0 / (u + 1.98 * pow (u, 0.172));
    Zle = Zl1 / (1 - Zl1 * Pe / Z0);
    Zlo = Zl1 / (1 - Zl1 * Po / Z0);
  }

  nr_double_t ZleFreq, ErEffeFreq, ZloFreq, ErEffoFreq;
  ZleFreq = Zle;
  ErEffeFreq = ErEffe;
  ZloFreq = Zlo;
  ErEffoFreq = ErEffo;

  // GETSINGER
  if (!strcmp (DModel, "Getsinger")) {
    nr_double_t g, f, d, e, z;

    // even mode dispersion
    g = 0.6 + 0.009 * Zle / 2;
    f = 2 * MU0 * h * frequency / Zle * 2;
    e = er - (er - ErEffe) / (1 + g * sqr (f));
    d = (er - e) * (e - ErEffe) / e / (er - ErEffe);
    z = Zle * sqrt (e / ErEffe) / (1 + d);
    ErEffeFreq = e; ZleFreq = z;

    // odd mode dispersion
    g = 0.6 + 0.009 * Zlo * 2;
    f = 2 * MU0 * h * frequency / Zlo / 2;
    e = er - (er - ErEffo) / (1 + g * sqr (f));
    d = (er - e) * (e - ErEffo) / e / (er - ErEffo);
    z = Zlo * sqrt (e / ErEffo) / (1 + d);
    ErEffoFreq = e; ZloFreq = z;
  }

  // analyse losses of line
  nr_double_t ace, aco, ade, ado;
  msline::analyseLoss (W, t, er, rho, D, tand, ZleFreq, ZloFreq, ErEffeFreq,
		       frequency, "Hammerstad", ace, ade);
  msline::analyseLoss (W, t, er, rho, D, tand, ZloFreq, ZleFreq, ErEffoFreq,
		       frequency, "Hammerstad", aco, ado);

  // compute propagation constants for even and odd mode
  complex ge, go;
  nr_double_t ae, ao, be, bo, k0;
  k0 = 2 * M_PI * frequency / C0;
  ae = ace + ade;
  ao = aco + ado;
  be = sqrt (ErEffeFreq) * k0;
  bo = sqrt (ErEffoFreq) * k0;
  ge = rect (ae, be);
  go = rect (ao, bo);

  // compute abbreviations
  complex Ee, Eo, De, Do, Xe, Xo, Ye, Yo;
  Ee = (sqr (ZleFreq) + sqr (z0)) * sinh (ge * l);
  Eo = (sqr (ZloFreq) + sqr (z0)) * sinh (go * l);
  De = 2 * ZleFreq * z0 * cosh (ge * l) + Ee;
  Do = 2 * ZloFreq * z0 * cosh (go * l) + Eo;
  Xe = Ee / 2 / De;
  Xo = Eo / 2 / Do;
  Ye = Zle * z0 / De;
  Yo = Zlo * z0 / Do;

  // reflexion coefficients
  setS (1, 1, Xe + Xo); setS (2, 2, Xe + Xo);
  setS (3, 3, Xe + Xo); setS (4, 4, Xe + Xo);
  // through paths
  setS (1, 2, Ye + Yo); setS (2, 1, Ye + Yo);
  setS (3, 4, Ye + Yo); setS (4, 3, Ye + Yo);
  // coupled paths
  setS (1, 4, Xe - Xo); setS (4, 1, Xe - Xo);
  setS (2, 3, Xe - Xo); setS (3, 2, Xe - Xo);
  // isolated paths
  setS (1, 3, Ye - Yo); setS (3, 1, Ye - Yo);
  setS (2, 4, Ye - Yo); setS (4, 2, Ye - Yo);
}
