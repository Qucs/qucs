/*
 * jfet.cpp - jfet class implementation
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
 * $Id: jfet.cpp,v 1.3 2004-06-21 23:11:42 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

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
#include "jfet.h"

#define NODE_G 1 /* gate node   */
#define NODE_D 2 /* drain node  */
#define NODE_S 3 /* source node */

jfet::jfet () : circuit (3) {
  rs = rd = NULL;
  type = CIR_JFET;
}

void jfet::calcSP (nr_double_t frequency) {

  // fetch computed operating points
  nr_double_t Cgd = getOperatingPoint ("Cgd");
  nr_double_t Cgs = getOperatingPoint ("Cgs");
  nr_double_t ggs = getOperatingPoint ("ggs");
  nr_double_t ggd = getOperatingPoint ("ggd");
  nr_double_t gds = getOperatingPoint ("gds");
  nr_double_t gm  = getOperatingPoint ("gm");

  // compute the models admittances
  complex Ygd = rect (ggd, 2.0 * M_PI * frequency * Cgd);
  complex Ygs = rect (ggs, 2.0 * M_PI * frequency * Cgs);
  complex Yds = gds;

  // build admittance matrix and convert it to S-parameter matrix
  matrix y = matrix (3); 
  y.set (NODE_G, NODE_G, Ygd + Ygs);
  y.set (NODE_G, NODE_D, -Ygd);
  y.set (NODE_G, NODE_S, -Ygs);
  y.set (NODE_D, NODE_G, gm - Ygd);
  y.set (NODE_D, NODE_D, Ygd + Yds);
  y.set (NODE_D, NODE_S, -Yds - gm);
  y.set (NODE_S, NODE_G, -Ygs - gm);
  y.set (NODE_S, NODE_D, -Yds);
  y.set (NODE_S, NODE_S, Ygs + Yds + gm);
  copyMatrixS (ytos (y));
}

void jfet::initDC (dcsolver * solver) {

  // initialize starting values
  setV (NODE_G, 0.8);
  setV (NODE_D, 0.0);
  setV (NODE_S, 0.0);
  UgdPrev = real (getV (NODE_G) - getV (NODE_D));
  UgsPrev = real (getV (NODE_G) - getV (NODE_S));

  // possibly insert series resistance at source
  nr_double_t Rs = getPropertyDouble ("Rs");
  if (Rs != 0) {
    // create additional circuit if necessary and reassign nodes
    rs = device::splitResistance (this, rs, solver->getNet (),
				  "Rs", "source", NODE_S);
    device::applyResistance (rs, Rs);
  }
  // no series resistance at source
  else {
    device::disableResistance (this, rs, solver->getNet (), NODE_S);
  }

  // possibly insert series resistance at drain
  nr_double_t Rd = getPropertyDouble ("Rd");
  if (Rd != 0) {
    // create additional circuit if necessary and reassign nodes
    rd = device::splitResistance (this, rd, solver->getNet (),
				  "Rd", "drain", NODE_D);
    device::applyResistance (rd, Rd);
  }
  // no series resistance at drain
  else {
    device::disableResistance (this, rd, solver->getNet (), NODE_D);
  }
}

void jfet::calcDC (void) {

  // fetch device model parameters
  nr_double_t Is   = getPropertyDouble ("Is");
  nr_double_t n    = getPropertyDouble ("N");
  nr_double_t Isr  = getPropertyDouble ("Isr");
  nr_double_t nr   = getPropertyDouble ("Nr");
  nr_double_t Vt0  = getPropertyDouble ("Vt0");
  nr_double_t l    = getPropertyDouble ("Lambda");
  nr_double_t beta = getPropertyDouble ("Beta");

  nr_double_t Ugs, Ugd, Ut, T, gm, IeqG, IeqD, IeqS, UgsCrit, UgdCrit;
  nr_double_t Uds, ggs, Igs, ggd, Igd, Ids, gds, gtiny;

  // apply polarity of JFET
  char * type = getPropertyString ("Type");
  nr_double_t pol = (type && !strcmp (type, "pfet")) ? -1 : 1;

  T = -K + 26.5;
  Ut = T * kB / Q;
  Ugd = real (getV (NODE_G) - getV (NODE_D)) * pol;
  Ugs = real (getV (NODE_G) - getV (NODE_S)) * pol;

  // critical voltage necessary for bad start values
  UgsCrit = n * Ut * log (n * Ut / M_SQRT2 / Is);
  UgdCrit = n * Ut * log (n * Ut / M_SQRT2 / Is);
  UgsPrev = Ugs = device::pnVoltage (Ugs, UgsPrev, Ut * n, UgsCrit);
  UgdPrev = Ugd = device::pnVoltage (Ugd, UgdPrev, Ut * n, UgdCrit);

  Uds = Ugs - Ugd;

  // gate-source diode
  gtiny = Ugs < - 10 * Ut * n ? (Is + Isr) : 0;
  ggs = Is / Ut / n * exp (Ugs / Ut / n) + 
    Isr / Ut / n * exp (Ugs / Ut / nr) + gtiny;
  Igs = Is * (exp (Ugs / Ut / n) - 1) + 
    Isr * (exp (Ugs / Ut / nr) - 1) + gtiny * Ugs;

  // gate-drain diode
  gtiny = Ugd < - 10 * Ut * n ? (Is + Isr) : 0;
  ggd = Is / Ut / n * exp (Ugd / Ut / n) + 
    Isr / Ut / n * exp (Ugd / Ut / nr) + gtiny;
  Igd = Is * (exp (Ugd / Ut / n) - 1) + 
    Isr * (exp (Ugd / Ut / nr) - 1) + gtiny * Ugd;

  // normal (forward) mode of operation
  if (Uds >= 0) {
    nr_double_t Ugst = Ugs - Vt0;
    // normal mode, cutoff region
    if (Ugst < 0) {
      Ids = 0;
      gm = 0;
      gds = 0;
    }
    else {
      nr_double_t b = beta * (1 + l * Uds);
      // normal mode, saturation region
      if (Ugst <= Uds) {
	Ids = b * Ugst * Ugst;
	gm  = b * 2 * Ugst;
	gds = l * b * Ugst * Ugst;
      }
      // normal mode, linear region
      else {
	Ids = b * Uds * (2 * Ugst - Uds);
	gm  = b * 2 * Uds;
	gds = b * 2 * (Ugst - Uds) + l * beta * Uds * (2 * Ugst - Uds);
      }
    }
  }
  // inverse (backward) mode of operation
  else {
    nr_double_t Ugdt = Ugd - Vt0;
    // inverse mode, cutoff region
    if (Ugdt < 0) {
      Ids = 0;
      gm  = 0;
      gds = 0;
    }
    else {
      nr_double_t b = beta * (1 - l * Uds);
      // inverse mode, saturation region
      if (Ugdt <= -Uds) {
	Ids = - b * Ugdt * Ugdt;
	gm  = - b * 2 * Ugdt;
	gds = beta * l * Ugdt * Ugdt + b * 2 * Ugdt;
      }
      // inverse mode, linear region
      else {
	Ids = b * Uds * (2 * Ugdt + Uds);
	gm  = b * 2 * Uds;
	gds = 2 * b * Ugdt - beta * l * Uds * (2 * Ugdt + Uds);
      }
    }
  }

  // save these operating points
  setOperatingPoint ("gm", gm);
  setOperatingPoint ("gds", gds);
  setOperatingPoint ("Id", Ids);

  // compute autonomic current sources
  IeqG = Igs - ggs * Ugs;
  IeqD = Igd - ggd * Ugd;
  IeqS = Ids - gm * Ugs - gds * Uds;
  setI (NODE_G, (-IeqG - IeqD) * pol);
  setI (NODE_D, (+IeqD - IeqS) * pol);
  setI (NODE_S, (+IeqG + IeqS) * pol);

  // apply admittance matrix elements
  setY (NODE_G, NODE_G, ggs + ggd);
  setY (NODE_G, NODE_D, -ggd);
  setY (NODE_G, NODE_S, -ggs);
  setY (NODE_D, NODE_G, -ggd + gm);
  setY (NODE_D, NODE_D, gds + ggd);
  setY (NODE_D, NODE_S, -gm - gds);
  setY (NODE_S, NODE_G, -ggs - gm);
  setY (NODE_S, NODE_D, -gds);
  setY (NODE_S, NODE_S, ggs + gds + gm);
}

void jfet::calcOperatingPoints (void) {

  // fetch device model parameters
  nr_double_t Is   = getPropertyDouble ("Is");
  nr_double_t n    = getPropertyDouble ("N");
  nr_double_t Isr  = getPropertyDouble ("Isr");
  nr_double_t nr   = getPropertyDouble ("Nr");
  nr_double_t z    = getPropertyDouble ("M");
  nr_double_t Cgd0 = getPropertyDouble ("Cgd");
  nr_double_t Cgs0 = getPropertyDouble ("Cgs");
  nr_double_t Pb   = getPropertyDouble ("Pb");
  nr_double_t Fc   = getPropertyDouble ("Fc");
  
  nr_double_t Ugs, Ugd, Ut, T, ggd, ggs, Cgs, Cgd, Igs, Igd;

  // apply polarity of JFET
  char * type = getPropertyString ("Type");
  nr_double_t pol = (type && !strcmp (type, "pfet")) ? -1 : 1;

  T = -K + 26.5;
  Ut = kB * T / Q;
  Ugd = real (getV (NODE_G) - getV (NODE_D)) * pol;
  Ugs = real (getV (NODE_G) - getV (NODE_S)) * pol;

  // calculate diode currents and its derivatives
  ggs = Is / Ut / n * exp (Ugs / Ut / n) + Isr / Ut / n * exp (Ugs / Ut / nr);
  Igs = Is * (exp (Ugs / Ut / n) - 1) + Isr * (exp (Ugs / Ut / nr) - 1);
  ggd = Is / Ut / n * exp (Ugd / Ut / n) + Isr / Ut / n * exp (Ugd / Ut / nr);
  Igd = Is * (exp (Ugd / Ut / n) - 1) + Isr * (exp (Ugd / Ut / nr) - 1);

  // capacitance of gate-drain diode
  if (Ugd <= Fc * Pb)
    Cgd = Cgd0 * pow (1 - Ugd / Pb, -z);
  else
    Cgd = Cgd0 / pow (1 - Fc, z) * (1 + z * (Ugd - Fc * Pb) / Pb / (1 - Fc));

  // capacitance of gate-source diode
  if (Ugs <= Fc * Pb)
    Cgs = Cgs0 * pow (1 - Ugs / Pb, -z);
  else
    Cgs = Cgs0 / pow (1 - Fc, z) * (1 + z * (Ugs - Fc * Pb) / Pb / (1 - Fc));

  // save operating points
  setOperatingPoint ("ggs", ggs);
  setOperatingPoint ("ggd", ggd);
  setOperatingPoint ("Vgs", Ugs);
  setOperatingPoint ("Vgd", Ugd);
  setOperatingPoint ("Vds", Ugs - Ugd);
  setOperatingPoint ("Cgd", Cgd);
  setOperatingPoint ("Cgs", Cgs);
}
