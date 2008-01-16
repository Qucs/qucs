/*
 * diac.cpp - diac class implementation
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2008 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: diac.cpp,v 1.4 2008/01/16 18:14:16 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "net.h"
#include "matrix.h"
#include "component_id.h"
#include "constants.h"
#include "device.h"
#include "devstates.h"
#include "diac.h"

#define NODE_A1 0 /* anode 1 */
#define NODE_A2 1 /* anode 2 (cathode) */
#define NODE_IN 2 /* internal node */

using namespace device;

// Constructor for the diac.
diac::diac () : circuit (3) {
  type = CIR_DIAC;
}

// Callback for initializing the DC analysis.
void diac::initDC (void) {
  // allocate MNA matrices
  allocMatrixMNA ();
  // create internal node
  setNode (NODE_IN, createInternal (getName (), "int"));
}

// Callback for DC analysis.
void diac::calcDC (void) {
  // get device properties
  nr_double_t Ubo = getPropertyDouble ("Vbo");
  nr_double_t Ibo = getPropertyDouble ("Ibo");
  nr_double_t Is  = getPropertyDouble ("Is");
  nr_double_t N   = getPropertyDouble ("N");
  nr_double_t Ri  = getPropertyDouble ("Ri");
  nr_double_t T   = getPropertyDouble ("Temp");

  nr_double_t Ut, Gi_bo, Ud_bo, Ieq, Vd;

  T = kelvin (T);
  Ut = N * T * kBoverQ;
  Gi_bo = Ibo / Ubo;
  Ud_bo = Ut * log (Ibo / Is + 1.0);

  Vd = Ud = real (getV (NODE_IN) - getV (NODE_A2));
  Id = sign (Ud) * Is;
  Ud = fabs (Ud);

  if (Ud > Ud_bo)
    gi = 1 / Ri;
  else
    gi = Gi_bo;

  Ud /= Ut;
  if (Ud >= 80.0) {
    Id *= exp (80.0) * (1.0 + Ud - 80.0) - 1.0;
    Ud  = 80.0;
  }
  else
    Id *= exp (Ud) - 1.0;

  gd  = Is / Ut * exp (Ud);
  Ieq = Id - Vd * gd;

  // fill in I-Vector
  setI (NODE_A2, +Ieq);
  setI (NODE_IN, -Ieq);
  setI (NODE_A1, 0);

  // fill in G-Matrix
  setY (NODE_A2, NODE_A2, +gd); setY (NODE_IN, NODE_IN, +gd);
  setY (NODE_A2, NODE_IN, -gd); setY (NODE_IN, NODE_A2, -gd);
  setY (NODE_A1, NODE_A1, +gi); addY (NODE_IN, NODE_IN, +gi);
  setY (NODE_A1, NODE_IN, -gi); setY (NODE_IN, NODE_A1, -gi);
}

// Saves operating points (voltages).
void diac::saveOperatingPoints (void) {
  nr_double_t Vd = real (getV (NODE_IN) - getV (NODE_A2));
  nr_double_t Vi = real (getV (NODE_A1) - getV (NODE_IN));
  setOperatingPoint ("Vd", Vd);
  setOperatingPoint ("Vi", Vi);
}

// Loads operating points (voltages).
void diac::loadOperatingPoints (void) {
  Ud = getOperatingPoint ("Vd");
  Ui = getOperatingPoint ("Vi");
}

// Calculates and saves operating points.
void diac::calcOperatingPoints (void) {
  nr_double_t Cj0 = getPropertyDouble ("Cj0");
  // calculate capacitances and charges
  nr_double_t Ci;
  Ci = Cj0;
  Qi = Cj0 * Ui;
  // save operating points
  setOperatingPoint ("gi", gi);
  setOperatingPoint ("gd", gd);
  setOperatingPoint ("Id", Id);
  setOperatingPoint ("Ci", Ci);
}

// Callback for initializing the AC analysis.
void diac::initAC (void) {
  initDC ();
}

// Build admittance matrix for AC and SP analysis.
matrix diac::calcMatrixY (nr_double_t frequency) {
  nr_double_t gd = getOperatingPoint ("gd");
  nr_double_t gi = getOperatingPoint ("gi");
  nr_double_t Ci = getOperatingPoint ("Ci");
  nr_complex_t yi = rect (gi, Ci * 2.0 * M_PI * frequency);
  matrix y (3);
  y.set (NODE_A2, NODE_A2, +gd);
  y.set (NODE_IN, NODE_IN, +gd +yi);
  y.set (NODE_A2, NODE_IN, -gd);
  y.set (NODE_IN, NODE_A2, -gd);
  y.set (NODE_A1, NODE_A1, +yi);
  y.set (NODE_A1, NODE_IN, -yi);
  y.set (NODE_IN, NODE_A1, -yi);
  return y;
}

// Callback for the AC analysis.
void diac::calcAC (nr_double_t frequency) {
  setMatrixY (calcMatrixY (frequency));
}

// Callback for S-parameter analysis.
void diac::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

#define qState 0 // charge state
#define cState 1 // current state

// Callback for initializing the TR analysis.
void diac::initTR (void) {
  setStates (2);
  initDC ();
}

// Callback for the TR analysis.
void diac::calcTR (nr_double_t) {
  calcDC ();
  saveOperatingPoints ();
  loadOperatingPoints ();
  calcOperatingPoints ();

  nr_double_t Ci = getOperatingPoint ("Ci");
  transientCapacitance (qState, NODE_A1, NODE_IN, Ci, Ui, Qi);
}
