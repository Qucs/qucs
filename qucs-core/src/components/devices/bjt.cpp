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
 * $Id: bjt.cpp,v 1.7 2004-07-26 22:07:29 ela Exp $
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
#include "spsolver.h"
#include "component_id.h"
#include "constants.h"
#include "device.h"
#include "bjt.h"

#define NEWSGP 1

#define NODE_B 1 /* base node       */
#define NODE_C 2 /* collector node  */
#define NODE_E 3 /* emitter node    */
#define NODE_S 4 /* substrate node  */

bjt::bjt () : circuit (4) {
  cbcx = rb = re = rc = NULL;
  type = CIR_BJT;
  enabled = 0;
}

void bjt::calcSP (nr_double_t frequency) {

  // fetch computed operating points
  nr_double_t Cbe  = getOperatingPoint ("Cbe");
  nr_double_t gbe  = getOperatingPoint ("gpi");
  nr_double_t Cbci = getOperatingPoint ("Cbci");
  nr_double_t Cbcx = getOperatingPoint ("Cbcx");
  nr_double_t gbc  = getOperatingPoint ("gmu");
  nr_double_t Ccs  = getOperatingPoint ("Ccs");
  nr_double_t gmf  = getOperatingPoint ("gmf");
  nr_double_t gmr  = getOperatingPoint ("gmr");
  nr_double_t Rbb  = getOperatingPoint ("Rbb");

  // summarize internal and external base-collector capacitance
  if (Rbb == 0) Cbci += Cbcx;

  // compute admittance matrix entries
  complex Ybe = rect (gbe, 2.0 * M_PI * frequency * Cbe);
  complex Ybc = rect (gbc, 2.0 * M_PI * frequency * Cbci);
  complex Ycs = rect (0.0, 2.0 * M_PI * frequency * Ccs);

  // build admittance matrix and convert it to S-parameter matrix
  matrix y = matrix (4);
  y.set (NODE_B, NODE_B, Ybc + Ybe);
  y.set (NODE_B, NODE_C, -Ybc);
  y.set (NODE_B, NODE_E, -Ybe);
  y.set (NODE_B, NODE_S, 0);
  y.set (NODE_C, NODE_B, -Ybc + gmf - gmr);
  y.set (NODE_C, NODE_C, Ybc + gmr + Ycs);
  y.set (NODE_C, NODE_E, -gmf);
  y.set (NODE_C, NODE_S, -Ycs);
  y.set (NODE_E, NODE_B, -Ybe - gmf + gmr);
  y.set (NODE_E, NODE_C, -gmr);
  y.set (NODE_E, NODE_E, Ybe + gmf);
  y.set (NODE_E, NODE_S, 0);
  y.set (NODE_S, NODE_B, 0);
  y.set (NODE_S, NODE_C, -Ycs);
  y.set (NODE_S, NODE_E, 0);
  y.set (NODE_S, NODE_S, Ycs);
  setMatrixS (ytos (y));
}

void bjt::initDC (dcsolver * solver) {

  // apply polarity of BJT
  char * type = getPropertyString ("Type");
  nr_double_t pol = !strcmp (type, "pnp") ? -1 : 1;
  nr_double_t T = getPropertyDouble ("Temp");

  // initialize starting values
  setV (NODE_B, 0.8);
  setV (NODE_C, 0.0);
  setV (NODE_E, 0.0);
  setV (NODE_S, 0.0);
  UbePrev = real (getV (NODE_B) - getV (NODE_E)) * pol;
  UbcPrev = real (getV (NODE_B) - getV (NODE_C)) * pol;
  subnet = solver->getNet ();

  // disable additional base-collector capacitance
  if (enabled) {
    disableCapacitance (this, cbcx, subnet);
    enabled = 0;
  }

  // possibly insert series resistance at emitter
  nr_double_t Re = getPropertyDouble ("Re");
  if (Re != 0) {
    // create additional circuit if necessary and reassign nodes
    re = splitResistance (this, re, subnet, "Re", "emitter", NODE_E);
    re->setProperty ("R", Re);
    re->setProperty ("Temp", T);
  }
  // no series resistance at emitter
  else {
    disableResistance (this, re, subnet, NODE_E);
  }

  // possibly insert series resistance at collector
  nr_double_t Rc = getPropertyDouble ("Rc");
  if (Rc != 0) {
    // create additional circuit if necessary and reassign nodes
    rc = splitResistance (this, rc, subnet, "Rc", "collector", NODE_C);
    rc->setProperty ("R", Rc);
    rc->setProperty ("Temp", T);
  }
  // no series resistance at collector
  else {
    disableResistance (this, rc, subnet, NODE_C);
  }

  // possibly insert base series resistance
  nr_double_t Rb  = getPropertyDouble ("Rb");
  nr_double_t Rbm = getPropertyDouble ("Rbm");
  if (Rbm == 0) Rbm = Rb; // Rbm defaults to Rb if zero
  if (Rb < Rbm) Rbm = Rb; // Rbm must be less or equal Rb
  setProperty ("Rbm", Rbm);
  if (Rbm != 0) {
    // create additional circuit and reassign nodes
    rb = splitResistance (this, rb, subnet, "Rbb", "base", NODE_B);
    rb->setProperty ("R", Rb);
    rb->setProperty ("Temp", T);
  }
  // no series resistance at base
  else {
    disableResistance (this, rb, subnet, NODE_B);
    Rbb = 0; // set this operating point
  }
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
  nr_double_t T    = getPropertyDouble ("Temp");

  nr_double_t Ut, Ube, Ubc, Ir, It, Q1, Q2, Ibe, Ibc;
  nr_double_t Iben, Ibcn, Ibei, Ibci, gbe, gbc, gtiny;
  nr_double_t Uce, IeqB, IeqC, IeqE, IeqS, UbeCrit, UbcCrit;
  nr_double_t gm, go;

  // apply polarity of BJT
  char * type = getPropertyString ("Type");
  nr_double_t pol = !strcmp (type, "pnp") ? -1 : 1;

  // interpret zero as infinity for these model parameters
  Ikf = Ikf > 0 ? 1.0 / Ikf : 0;
  Ikr = Ikr > 0 ? 1.0 / Ikr : 0;
  Vaf = Vaf > 0 ? 1.0 / Vaf : 0;
  Var = Var > 0 ? 1.0 / Var : 0;

  T = kelvin (T);
  Ut = T * kB / Q;
  Ube = real (getV (NODE_B) - getV (NODE_E)) * pol;
  Ubc = real (getV (NODE_B) - getV (NODE_C)) * pol;

  // critical voltage necessary for bad start values
  UbeCrit = pnCriticalVoltage (Is, Nf * Ut);
  UbcCrit = pnCriticalVoltage (Is, Nr * Ut);
  UbePrev = Ube = pnVoltage (Ube, UbePrev, Ut * Nf, UbeCrit);
  UbcPrev = Ubc = pnVoltage (Ubc, UbcPrev, Ut * Nr, UbcCrit);

  Uce = Ube - Ubc;

  // base-emitter diodes
  gtiny = Ube < - 10 * Ut * Nf ? (Is + Ise) : 0;
  If = pnCurrent (Ube, Is, Ut * Nf);
  Ibei = If / Bf;
  gif = pnConductance (Ube, Is, Ut * Nf);
  gbei = gif / Bf;
  Iben = pnCurrent (Ube, Ise, Ut * Ne);
  gben = pnConductance (Ube, Ise, Ut * Ne);
  Ibe = Ibei + Iben + gtiny * Ube;
  gbe = gbei + gben + gtiny;

  // base-collector diodes
  gtiny = Ubc < - 10 * Ut * Nr ? (Is + Isc) : 0;
  Ir = pnCurrent (Ubc, Is, Ut * Nr);
  Ibci = Ir / Br;
  gir = pnConductance (Ubc, Is, Ut * Nr);
  gbci = gir / Br;
  Ibcn = pnCurrent (Ubc, Isc, Ut * Nc);
  gbcn = pnConductance (Ubc, Isc, Ut * Nc);
  Ibc = Ibci + Ibcn + gtiny * Ubc;
  gbc = gbci + gbcn + gtiny;

  // compute base charge quantities
  Q1 = 1 / (1 - Ubc * Vaf - Ube * Var);
  Q2 = If * Ikf + Ir * Ikr;
  nr_double_t SArg = 1.0 + 4.0 * Q2;
  nr_double_t Sqrt = SArg > 0 ? sqrt (SArg) : 1;
  Qb = Q1 * (1 + Sqrt) / 2;
  dQbdUbe = Q1 * (Qb * Var + gif * Ikf / Sqrt);
  dQbdUbc = Q1 * (Qb * Vaf + gir * Ikr / Sqrt);

  // compute transfer current
  It = (If - Ir) / Qb;

  // compute forward and backward transconductance
  gitf = (gif - If * dQbdUbe / Qb) / Qb;
  gitr = (gir - Ir * dQbdUbc / Qb) / Qb;

  // compute old SPICE values
  go = (gir + It * dQbdUbc) / Qb;
  gm = (gif - It * dQbdUbe) / Qb - go;
  setOperatingPoint ("gm", gm);
  setOperatingPoint ("go", go);

  // calculate current-dependent base resistance
  if (Rbm != 0) {
    if (Irb != 0) {
      nr_double_t a, b, z;
      a = (Ibci + Ibcn + Ibei + Iben) / Irb;
      z = (sqrt (1 + 144 / sqr (M_PI) * a) - 1) / 24 * sqr (M_PI) / sqrt (a);
      b = tan (z);
      Rbb = Rbm + 3 * (Rb - Rbm) * (b - z) / z / sqr (b);
    }
    else {
      Rbb = Rbm + (Rb - Rbm) / Qb;
    }
    rb->setProperty ("R", Rbb);
    rb->calcDC ();
  }

  // compute autonomic current sources
  IeqB = Ibe - gbe * Ube;
  IeqC = Ibc - gbc * Ubc;
#if NEWSGP
  IeqE = It - gitf * Ube + gitr * Ubc;
#else
  IeqE = It - gm * Ube - go * Uce;
#endif
  IeqS = 0;
  setI (NODE_B, (-IeqB - IeqC) * pol);
  setI (NODE_C, (+IeqC - IeqE - IeqS) * pol);
  setI (NODE_E, (+IeqB + IeqE) * pol);
  setI (NODE_S, (+IeqS) * pol);

  // apply admittance matrix elements
#if NEWSGP
  setY (NODE_B, NODE_B, gbc + gbe);
  setY (NODE_B, NODE_C, -gbc);
  setY (NODE_B, NODE_E, -gbe);
  setY (NODE_B, NODE_S, 0);
  setY (NODE_C, NODE_B, -gbc + gitf - gitr);
  setY (NODE_C, NODE_C, gbc + gitr);
  setY (NODE_C, NODE_E, -gitf);
  setY (NODE_C, NODE_S, 0);
  setY (NODE_E, NODE_B, -gbe - gitf + gitr);
  setY (NODE_E, NODE_C, -gitr);
  setY (NODE_E, NODE_E, gbe + gitf);
  setY (NODE_E, NODE_S, 0);
  setY (NODE_S, NODE_B, 0);
  setY (NODE_S, NODE_C, 0);
  setY (NODE_S, NODE_E, 0);
  setY (NODE_S, NODE_S, 0);
#else
  setY (NODE_B, NODE_B, gbc + gbe);
  setY (NODE_B, NODE_C, -gbc);
  setY (NODE_B, NODE_E, -gbe);
  setY (NODE_B, NODE_S, 0);
  setY (NODE_C, NODE_B, -gbc + gm);
  setY (NODE_C, NODE_C, go + gbc);
  setY (NODE_C, NODE_E, -go - gm);
  setY (NODE_C, NODE_S, 0);
  setY (NODE_E, NODE_B, -gbe - gm);
  setY (NODE_E, NODE_C, -go);
  setY (NODE_E, NODE_E, gbe + go + gm);
  setY (NODE_E, NODE_S, 0);
  setY (NODE_S, NODE_B, 0);
  setY (NODE_S, NODE_C, 0);
  setY (NODE_S, NODE_E, 0);
  setY (NODE_S, NODE_S, 0);
#endif
}

void bjt::calcOperatingPoints (void) {

  // fetch device model parameters
  nr_double_t Cje0 = getPropertyDouble ("Cje");
  nr_double_t Vje  = getPropertyDouble ("Vje");
  nr_double_t Mje  = getPropertyDouble ("Mje");
  nr_double_t Cjc0 = getPropertyDouble ("Cjc");
  nr_double_t Vjc  = getPropertyDouble ("Vjc");
  nr_double_t Mjc  = getPropertyDouble ("Mjc");
  nr_double_t Xcjc = getPropertyDouble ("Xcjc");
  nr_double_t Cjs0 = getPropertyDouble ("Cjs");
  nr_double_t Vjs  = getPropertyDouble ("Vjs");
  nr_double_t Mjs  = getPropertyDouble ("Mjs");
  nr_double_t Fc   = getPropertyDouble ("Fc");
  nr_double_t Vtf  = getPropertyDouble ("Vtf");
  nr_double_t Tf   = getPropertyDouble ("Tf");
  nr_double_t Xtf  = getPropertyDouble ("Xtf");
  nr_double_t Itf  = getPropertyDouble ("Itf");
  nr_double_t Tr   = getPropertyDouble ("Tr");

  nr_double_t Cbe, Ube, Ubc, Cbci, Cbcx, Usc, Cbc, Ccs;

  // apply polarity of BJT
  char * type = getPropertyString ("Type");
  nr_double_t pol = !strcmp (type, "pnp") ? -1 : 1;

  // interpret zero as infinity for that model parameter
  Vtf = Vtf > 0 ? 1.0 / Vtf : 0;

  Ube = real (getV (NODE_B) - getV (NODE_E)) * pol;
  Ubc = real (getV (NODE_B) - getV (NODE_C)) * pol;
  Usc = real (getV (NODE_S) - getV (NODE_C)) * pol;

  // depletion capacitance of base-emitter diode
  Cbe = pnCapacitance (Ube, Cje0, Vje, Mje, Fc);

  // diffusion capacitance of base-emitter diode
  nr_double_t e, Tff, dTffdUbe;
  e = exp (Ubc / 1.44 * Vtf);
  Tff = Tf * (1 + Xtf * sqr (If / (If + Itf)) * e);
  dTffdUbe = Tf * Xtf * 2 * gif * If * Itf / cubic (If + Itf) * e;
  Cbe += (If * dTffdUbe + Tff * (gif - If / Qb * dQbdUbe)) / Qb;

  // depletion and diffusion capacitance of base-collector diode
  Cbc = pnCapacitance (Ubc, Cjc0, Vjc, Mjc, Fc);
  Cbci = Cbc * Xcjc + Tr * gir;
  Cbcx = Cbc * (1 - Xcjc);

  // depletion capacitance of collector-substrate diode
  Ccs = pnCapacitance (Usc, Cjs0, Vjs, Mjs);

  // finally save the operating points
  setOperatingPoint ("Cbe", Cbe);
  setOperatingPoint ("Cbci", Cbci);
  setOperatingPoint ("Cbcx", Cbcx);
  setOperatingPoint ("Ccs", Ccs);
  setOperatingPoint ("gmf", gitf);
  setOperatingPoint ("gmr", gitr);
  setOperatingPoint ("gmu", gbci + gbcn);
  setOperatingPoint ("gpi", gbei + gben);
  setOperatingPoint ("Vbe", Ube);
  setOperatingPoint ("Vbc", Ubc);
  setOperatingPoint ("Vce", Ube - Ubc);
  setOperatingPoint ("Rbb", Rbb);
}

void bjt::initSP (spsolver * solver) {
  nr_double_t Cbcx = getOperatingPoint ("Cbcx");
  nr_double_t Rbb  = getOperatingPoint ("Rbb");
  subnet = solver->getNet ();

  /* if necessary then insert external capacitance between internal
     collector node and external base node */
  if (Rbb != 0 && Cbcx != 0) {
    if (!enabled) {
      cbcx = splitCapacitance (this, cbcx, subnet, "Cbcx", rb->getNode (1),
			       getNode (NODE_C));
      enabled = 1;
    }
    cbcx->setProperty ("C", Cbcx);  // S-parameters will be computed
  }
  else {
    disableCapacitance (this, cbcx, subnet);
    enabled = 0;
  }
}
