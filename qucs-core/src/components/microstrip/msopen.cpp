/*
 * msopen.cpp - microstrip open end class implementation
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
 * $Id: msopen.cpp,v 1.1 2004/08/07 10:48:47 margraf Exp $
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
#include "matrix.h"
#include "msopen.h"
#include "msline.h"

msopen::msopen () : circuit (1) {
  type = CIR_MSOPEN;
}

// returns the microstrip open end capacitance
nr_double_t msopen::calcCend (nr_double_t frequency, nr_double_t w,
		nr_double_t h, nr_double_t t, nr_double_t er,
		char * SModel, char * DModel) {

  nr_double_t ZlEff, ErEff, WEff, ZlEffFreq, ErEffFreq;
  msline::analyseQuasiStatic (w, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (w, h, er, ZlEff, ErEff, frequency, DModel,
				ZlEffFreq, ErEffFreq);


  w /= h;
  /* local variables */
  nr_double_t Q1 = 0.434907 *
	(pow (ErEffFreq, 0.81) + 0.26) / (pow (ErEffFreq, 0.81) + 0.189) *
	(pow (w, 0.8544) + 0.236) / (pow (w, 0.8544) + 0.87);
  nr_double_t Q2 = pow (w, 0.371) / (2.358 * er + 1.0) + 1.0;
  nr_double_t Q3 = atan (0.084 * pow (w, 1.9413 / Q2)) *
			0.5274 / pow (ErEffFreq, 0.9236) + 1.0;
  nr_double_t Q4 = 0.0377 * (6.0 - 5.0 * exp (0.036 * (1.0 - er)) *
		atan (0.067 * pow (w, 1.456))) + 1.0;
  nr_double_t Q5 = 1.0 - 0.218 * exp (-7.5 * w);

  return Q1 * Q3 * Q5 / Q4 * h * sqrt (ErEffFreq) / C0 * ZlEffFreq;
}

void msopen::calcSP (nr_double_t frequency) {

  /* how to get properties of this component, e.g. W */
  nr_double_t w = getPropertyDouble ("W");
  char * SModel = getPropertyString ("Model");
  char * DModel = getPropertyString ("DispModel");

  /* how to get properties of the substrate, e.g. Er, H */
  substrate * subst = getSubstrate ();
  nr_double_t er    = subst->getPropertyDouble ("er");
  nr_double_t h     = subst->getPropertyDouble ("h");
  nr_double_t t     = subst->getPropertyDouble ("t");

  nr_double_t C = -0.5 / M_PI / frequency /
		  calcCend (frequency, w, h, t, er, SModel, DModel);

  setS (1, 1, complex (-Z0, C) / complex (Z0, C));
}

void msopen::calcDC (void) {
  setC (1, 1,  0.0);
  setB (1, 1, +1.0);
  setE (1, 0.0);
  setD (1, 1, +1.0);
}
