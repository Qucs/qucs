/*
 * bjt.cpp - bipolar junction transistor class implementation
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
 * $Id: bjt.cpp,v 1.1 2004-07-07 13:52:34 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "complex.h"
#include "matrix.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "net.h"
#include "analysis.h"
#include "dcsolver.h"
#include "component_id.h"
#include "constants.h"
#include "device.h"
#include "bjt.h"

#define NODE_B 1 /* base node       */
#define NODE_C 2 /* collector node  */
#define NODE_E 3 /* emitter node    */
#define NODE_S 4 /* substrate node  */

bjt::bjt () : circuit (4) {
  rb = re = rc = NULL;
  type = CIR_BJT;
}

void bjt::calcSP (nr_double_t) {
}

void bjt::initDC (dcsolver * solver) {

  // initialize starting values
  setV (NODE_B, 0.8);
  setV (NODE_C, 0.0);
  setV (NODE_E, 0.0);
  setV (NODE_S, 0.0);
  UbePrev = real (getV (NODE_B) - getV (NODE_E));
  UbcPrev = real (getV (NODE_B) - getV (NODE_C));

  // possibly insert series resistance at emitter
  nr_double_t Re = getPropertyDouble ("Re");
  if (Re != 0) {
    // create additional circuit if necessary and reassign nodes
    re = device::splitResistance (this, re, solver->getNet (),
				  "Re", "emitter", NODE_E);
    device::applyResistance (re, Re);
  }
  // no series resistance at emitter
  else {
    device::disableResistance (this, re, solver->getNet (), NODE_E);
  }

  // possibly insert series resistance at collector
  nr_double_t Rc = getPropertyDouble ("Rc");
  if (Rc != 0) {
    // create additional circuit if necessary and reassign nodes
    rc = device::splitResistance (this, rc, solver->getNet (),
				  "Rc", "drain", NODE_C);
    device::applyResistance (rc, Rc);
  }
  // no series resistance at collector
  else {
    device::disableResistance (this, rc, solver->getNet (), NODE_C);
  }

  // insert base series resistance
  nr_double_t Rb = getPropertyDouble ("Rbm");
  // create additional circuit and reassign nodes
  rb = device::splitResistance (this, rb, solver->getNet (),
				"Rbb", "base", NODE_B);
  device::applyResistance (rb, Rb);
}

void bjt::calcDC (void) {

  // fetch device model parameters
  nr_double_t Is   = getPropertyDouble ("Is");
  nr_double_t Nf   = getPropertyDouble ("Nf");
  nr_double_t Nr   = getPropertyDouble ("Nr");
  nr_double_t Vaf  = getPropertyDouble ("Vaf");
  nr_double_t Var  = getPropertyDouble ("Var");
  nr_double_t Ikf  = getPropertyDouble ("Ikf");
  nr_double_t Ikr  = getPropertyDouble ("Ikr");
  nr_double_t Bf   = getPropertyDouble ("Bf");
  nr_double_t Br   = getPropertyDouble ("Br");
  nr_double_t Ise  = getPropertyDouble ("Ise");
  nr_double_t Isc  = getPropertyDouble ("Isc");
  nr_double_t Ne   = getPropertyDouble ("Ne");
  nr_double_t Nc   = getPropertyDouble ("Nc");
  nr_double_t Rb   = getPropertyDouble ("Rb");
  nr_double_t Rbm  = getPropertyDouble ("Rbm");
  nr_double_t Irb  = getPropertyDouble ("Irb");

  nr_double_t T, Ut, Ube, Ubc, If, Ir, It, Qb, Q1, Q2, Ibe, Ibc, Rbb;
  nr_double_t Iben, Ibcn, Ibei, Ibci, gbe, gbc, gif, gir, gitf, gitr, git;
  nr_double_t Uce, IeqB, IeqC, IeqE, IeqS, dQbdUbe, dQbdUbc, UbeCrit, UbcCrit;
  nr_double_t gtiny, gbei, gben, gbci, gbcn;

  T = -K + 26.5;
  Ut = T * kB / Q;
  Ube = real (getV (NODE_B) - getV (NODE_E));
  Ubc = real (getV (NODE_B) - getV (NODE_C));

  // critical voltage necessary for bad start values
  UbeCrit = Nf * Ut * log (Nf * Ut / M_SQRT2 / Is);
  UbcCrit = Nr * Ut * log (Nr * Ut / M_SQRT2 / Is);
  UbePrev = Ube = device::pnVoltage (Ube, UbePrev, Ut * Nf, UbeCrit);
  UbcPrev = Ubc = device::pnVoltage (Ubc, UbcPrev, Ut * Nr, UbcCrit);

  Uce = Ube - Ubc;

  // base-emitter diodes
  gtiny = Ube < - 10 * Ut * Nf ? (Is + Ise) : 0;
  If = Is * (exp (Ube / Nf / Ut) - 1);
  Ibei = If / Bf;
  gbei = Is / Nf / Ut * exp (Ube / Nf / Ut) / Bf;
  Iben = Ise * (exp (Ube / Ne / Ut) - 1);
  gben = Ise / Ne / Ut * exp (Ube / Ne / Ut);
  Ibe = Ibei + Iben + gtiny * Ube;
  gbe = gbei + gben + gtiny;

  // base-collector diodes
  gtiny = Ubc < - 10 * Ut * Nr ? (Is + Isc) : 0;
  Ir = Is * (exp (Ubc / Nr / Ut) - 1);
  Ibci = Ir / Br;
  gbci = Is / Nr / Ut * exp (Ubc / Nr / Ut) / Br;
  Ibcn = Isc * (exp (Ubc / Nc / Ut) - 1);
  gbcn = Isc / Nc / Ut * exp (Ubc / Nc / Ut);
  Ibc = Ibci + Ibcn + gtiny * Ubc;
  gbc = gbci + gbcn + gtiny;

  gif = gbei * Bf;
  gir = gbci * Br;

  // compute base charge terms
  Q1 = 1 / (1 - Ubc / Vaf - Ube / Var);
  Q2 = If / Ikf + Ir / Ikr;
  nr_double_t SArg = 1 + 4 * Q2;
  nr_double_t Sqrt = SArg > 0 ? sqrt (SArg) : 1;
  Qb = Q1 * (1 + Sqrt) / 2;
  dQbdUbe = Q1 * (Qb / Var + gif / Ikf / Sqrt);
  dQbdUbc = Q1 * (Qb / Vaf + gir / Ikr / Sqrt);

  It = (If - Ir) / Qb;

  // compute forward and backward transconductance
  gitf = (gif - If / Qb * dQbdUbe) / Qb;
  gitr = (gir - Ir / Qb * dQbdUbc) / Qb;
  git = gitf - gitr;

  if (/*Irb != */0) {
    nr_double_t a1 = (Ibe + Ibc) / Irb;
    nr_double_t a2 = (sqrt (1 + 144 / sqr (M_PI) * a1) - 1) / 
      sqrt (24 / sqr (M_PI) * a1);
    nr_double_t a3 = tan (a2);
    Rbb = Rbm + 3 * (Rb - Rbm) * a3 / a2 / sqr (a3); // check that!!!
  }
  else {
    Rbb = Rbm + (Rb - Rbm) / Qb;
  }
  device::applyResistance (rb, Rbb);

  // compute autonomic current sources
  IeqB = Ibe - gbe * Ube;
  IeqC = Ibc - gbc * Ubc;
  IeqE = It - git * Ube - gitr * Uce;
  IeqS = 0;
  setI (NODE_B, -IeqB - IeqC);
  setI (NODE_C, +IeqC - IeqE - IeqS);
  setI (NODE_E, +IeqB + IeqE);
  setI (NODE_S, +IeqS);

  // apply admittance matrix elements
  setY (NODE_B, NODE_B, gbc + gbe);
  setY (NODE_B, NODE_C, -gbc);
  setY (NODE_B, NODE_E, -gbe);
  setY (NODE_B, NODE_S, 0);
  setY (NODE_C, NODE_B, -gbc + git);
  setY (NODE_C, NODE_C, gitr + gbc);
  setY (NODE_C, NODE_E, -gitr - git);
  setY (NODE_C, NODE_S, 0);
  setY (NODE_E, NODE_B, -gbe - git);
  setY (NODE_E, NODE_C, -gitr);
  setY (NODE_E, NODE_E, gbe + gitr + git);
  setY (NODE_E, NODE_S, 0);
  setY (NODE_S, NODE_B, 0);
  setY (NODE_S, NODE_C, 0);
  setY (NODE_S, NODE_E, 0);
  setY (NODE_S, NODE_S, 0);
}

void bjt::calcOperatingPoints (void) {

}
