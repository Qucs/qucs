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
 * $Id: jfet.cpp,v 1.1 2004/06/05 12:20:38 ela Exp $
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
#include "jfet.h"

jfet::jfet () : circuit (3) {
  rs = rd = NULL;
  type = CIR_JFET;
}

void jfet::calcSP (nr_double_t frequency) {
}

void jfet::initDC (dcsolver * solver) {

  // initialize starting values
  setV (1, 0.8);
  setV (2, 0.0);
  setV (3, 0.0);
  UgdPrev = real (getV (1) - getV (2));
  UgsPrev = real (getV (1) - getV (3));

  // possibly insert series resistance at source
  nr_double_t Rs = getPropertyDouble ("Rs");
  if (Rs != 0) {
    // create additional circuit if necessary and reassign nodes
    rs = diode::splitResistance (this, rs, solver->getNet (),
				 "Rs", "source", 3);
    diode::applyResistance (rs, Rs);
  }
  // no series resistance
  else {
    diode::disableResistance (this, rs, solver->getNet (), 2);
  }

  // possibly insert series resistance at drain
  nr_double_t Rd = getPropertyDouble ("Rd");
  if (Rd != 0) {
    // create additional circuit if necessary and reassign nodes
    rd = diode::splitResistance (this, rd, solver->getNet (),
				 "Rd", "drain", 2);
    diode::applyResistance (rd, Rd);
  }
  // no series resistance
  else {
    diode::disableResistance (this, rd, solver->getNet (), 2);
  }
}

void jfet::calcDC (void) {
  nr_double_t Is  = getPropertyDouble ("Is");
  nr_double_t n   = getPropertyDouble ("N");
  nr_double_t Vt0 = getPropertyDouble ("Vt0");
  nr_double_t l   = getPropertyDouble ("Lambda");
  nr_double_t b   = getPropertyDouble ("Beta");

  nr_double_t Ugs, Ugd, Id, Ut, T, gm, IeqG, IeqD, IeqS, UgsCrit, UgdCrit;
  nr_double_t Uds, ggs, Igs, ggd, Igd, Ig, Ugst, Ids, gds, Ugdt, gtiny;

  T = -K + 26.5;
  Ut = T * kB / Q;
  Ugd = real (getV (1) - getV (2));
  Ugs = real (getV (1) - getV (3));

  // critical voltage necessary for bad start values
  UgsCrit = n * Ut * log (n * Ut / M_SQRT2 / Is);
  UgdCrit = n * Ut * log (n * Ut / M_SQRT2 / Is);
  UgsPrev = Ugs = diode::pnVoltage (Ugs, UgsPrev, Ut * n, UgsCrit);
  UgdPrev = Ugd = diode::pnVoltage (Ugd, UgdPrev, Ut * n, UgdCrit);

  Uds = Ugs - Ugd;

  // gate source diode
  gtiny = Ugs < - 10 * Ut * n ? Is : 0;
  ggs = Is / Ut / n * exp (Ugs / Ut / n) + gtiny;
  Igs = Is * (exp (Ugs / Ut / n) - 1) + gtiny * Ugs;

  // gate drain diode
  gtiny = Ugd < - 10 * Ut * n ? Is : 0;
  ggd = Is / Ut / n * exp (Ugd / Ut / n) + gtiny;
  Igd = Is * (exp (Ugd / Ut / n) - 1) + gtiny * Ugd;

  Ig = Igs + Igd;

  if (Uds >= 0) {
    Ugst = Ugs - Vt0;
    if (Ugst < 0) {
      Ids = 0;
      gm = 0;
      gds = 0;
    }
    else {
      if (Ugst <= Uds) {
	Ids = b * (1 + l * Uds) * pow (Ugst, 2);
	gm = 2 * b * (1 + l * Uds) * Ugst;
	gds = l * b * pow (Ugst, 2);
      }
      else {
	Ids = b * (1 + l * Uds) * Uds * (2 * Ugst - Uds);
	gm = 2 * b * (1 + l * Uds) * Uds;
	gds = 2 * b * (1 + l * Uds) * (Ugst - Uds) +
	  l * b * Uds * (2 * Ugst - Uds);
      }
    }
  }
  else {
    Ugdt = Ugd - Vt0;
    if (Ugdt < 0) {
      Ids = 0;
      gm = 0;
      gds = 0;
    }
    else {
      if (Ugdt <= -Uds) {
	Ids = - b * (1 + l * Uds) * pow (Ugdt, 2);
	gm = - 2 * b * (1 + l * Uds) * Ugdt;
	gds = l * b * pow (Ugdt, 2) - gm;
      }
      else {
	Ids = b * (1 + l * Uds) * Uds * (2 * Ugdt + Uds);
	gm = 2 * b * (1 + l * Uds) * Uds;
	gds = 2 * b * (1 + l * Uds) * Ugdt +
	  l * b * Uds * (2 * Ugdt + Uds);
      }
    }
  }

  Id = Ids - Igd;
  
  IeqG = -Igd - Igs;
  IeqD = Igd + gm * Ugs + gds * Uds - Id;
  IeqS = Igs - gm * Ugs - gds * Uds + Id;

  setI (1, IeqG);
  setI (2, IeqD);
  setI (3, IeqS);

  IeqG = Igd - ggd * Ugd;
  IeqD = (Ig - Igd) - ggs * Ugs;
  IeqS = (Id + Igd) - gds * Uds - gm * Ugs;

  setI (1, -IeqD - IeqG);
  setI (2, -IeqS + IeqG);
  setI (3, +IeqS + IeqD);

  setY (1, 1, ggs + ggd); setY (1, 2, -ggd); setY (1, 3, -ggs);
  setY (2, 1, -ggd + gm); setY (2, 2, +gds + ggd); setY (2, 3, -gm - gds);
  setY (3, 1, -ggs - gm); setY (3, 2, -gds); setY (3, 3, ggs + gds + gm);
}

void jfet::calcOperatingPoints (void) {
}
