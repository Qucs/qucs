/*
 * mscross.cpp - microstrip cross-junction class implementation
 *
 * Copyright (C) 2004, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: mscross.cpp,v 1.5 2007-02-04 18:08:52 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "substrate.h"
#include "msline.h"
#include "matrix.h"
#include "mscross.h"

mscross::mscross () : circuit (6) {
  type = CIR_MSCROSS;
}

void mscross::initModel (void) {
  setNode (NODE_5, createInternal (getName (), "i13"));
  setNode (NODE_6, createInternal (getName (), "i24"));
}

void mscross::initSP (void) {
  initModel ();
  allocMatrixS ();
}

void mscross::calcSP (nr_double_t frequency) {
  setMatrixS (ytos (calcMatrixY (frequency)));
}

void mscross::initDC (void) {
  initModel ();
  setVoltageSources (5);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_5);
  voltageSource (VSRC_2, NODE_3, NODE_5);
  voltageSource (VSRC_3, NODE_2, NODE_6);
  voltageSource (VSRC_4, NODE_4, NODE_6);
  voltageSource (VSRC_5, NODE_5, NODE_6);
}

void mscross::initAC (void) {
  initModel ();
  setVoltageSources (0);
  allocMatrixMNA ();
}

void mscross::calcAC (nr_double_t f) {
  setMatrixY (calcMatrixY (f));
}

matrix mscross::calcMatrixY (nr_double_t f) {
  nr_double_t W1 = getPropertyDouble ("W1");
  nr_double_t W2 = getPropertyDouble ("W2");
  nr_double_t W3 = getPropertyDouble ("W3");
  nr_double_t W4 = getPropertyDouble ("W4");
  substrate * subst = getSubstrate ();
  nr_double_t er = subst->getPropertyDouble ("er");
  nr_double_t h  = subst->getPropertyDouble ("h");
  nr_double_t t  = subst->getPropertyDouble ("t");
  nr_double_t W1h = W1 / h;
  nr_double_t W2h = W2 / h;
  nr_double_t X, Y;
  nr_double_t C1, C2, C3, C4, L1, L2, L3;

  // original model
  X = log10 (W1h) * (86.6 * W2h - 30.9 * sqrt (W2h) + 367) + 
    cubic (W2h) + 74 * W2h + 130;
  C1 = C2 = C3 = C4 = 1e-9 * W1 * (0.25 * X * pow (1 / W1h, 1.0 / 3.0) - 60 +
				   1 / W2h / 2 - 0.375 * W1h * (1 - W2h));
  Y = 165.6 * W2h + 31.2 * sqrt (W2h) - 11.8 * sqr (W2h);
  L1 = 1e-9 * h * (Y * W1h - 32 * W2h + 3) * pow (1 / W1h, 1.5);
  L2 = 1e-9 * h * (Y * W2h - 32 * W1h + 3) * pow (1 / W2h, 1.5);
  L3 = 1e-9 * h * (5 * W2h * cos (M_PI / 2 * (1.5 - W1h)) -
		   (1 + 7 / W1h ) / W2h - 337.5);

  // L3 correction
  L3 = L3 * 0.8;

  // capacitance corrections
  char * SModel = getPropertyString ("MSModel");
  char * DModel = getPropertyString ("MSDispModel");
  nr_double_t Zl1, Er1, Zl2, Er2;
  nr_double_t ZlEff, ErEff, WEff;
  msline::analyseQuasiStatic (W1, h, t, 9.9, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W1, h, 9.9, ZlEff, ErEff, f, DModel,
                              Zl1, Er1);
  msline::analyseQuasiStatic (W1, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W1, h, er, ZlEff, ErEff, f, DModel,
                              Zl2, Er2);
  C1 = C1 * Zl1 / Zl2 * sqrt (Er2 / Er1);
  msline::analyseQuasiStatic (W2, h, t, 9.9, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W2, h, 9.9, ZlEff, ErEff, f, DModel,
                              Zl1, Er1);
  msline::analyseQuasiStatic (W2, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W2, h, er, ZlEff, ErEff, f, DModel,
                              Zl2, Er2);
  C2 = C2 * Zl1 / Zl2 * sqrt (Er2 / Er1);
  msline::analyseQuasiStatic (W3, h, t, 9.9, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W3, h, 9.9, ZlEff, ErEff, f, DModel,
                              Zl1, Er1);
  msline::analyseQuasiStatic (W3, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W3, h, er, ZlEff, ErEff, f, DModel,
                              Zl2, Er2);
  C3 = C3 * Zl1 / Zl2 * sqrt (Er2 / Er1);
  msline::analyseQuasiStatic (W4, h, t, 9.9, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W4, h, 9.9, ZlEff, ErEff, f, DModel,
                              Zl1, Er1);
  msline::analyseQuasiStatic (W4, h, t, er, SModel, ZlEff, ErEff, WEff);
  msline::analyseDispersion  (W4, h, er, ZlEff, ErEff, f, DModel,
                              Zl2, Er2);
  C4 = C4 * Zl1 / Zl2 * sqrt (Er2 / Er1);

  complex yc1 = rect (0, 2 * M_PI * f * C1);
  complex yc2 = rect (0, 2 * M_PI * f * C2);
  complex yc3 = rect (0, 2 * M_PI * f * C3);
  complex yc4 = rect (0, 2 * M_PI * f * C4);
  complex yl1 = 1 / rect (0, 2 * M_PI * f * L1);
  complex yl2 = 1 / rect (0, 2 * M_PI * f * L2);
  complex yl3 = 1 / rect (0, 2 * M_PI * f * L3);
  matrix M (6);
  M.set (NODE_1, NODE_1, yl1 + yc1);
  M.set (NODE_2, NODE_2, yl2 + yc2);
  M.set (NODE_3, NODE_3, yl1 + yc3);
  M.set (NODE_4, NODE_4, yl2 + yc4);
  M.set (NODE_1, NODE_5, -yl1); M.set (NODE_5, NODE_1, -yl1);
  M.set (NODE_3, NODE_5, -yl1); M.set (NODE_5, NODE_3, -yl1);
  M.set (NODE_2, NODE_6, -yl2); M.set (NODE_6, NODE_2, -yl2);
  M.set (NODE_4, NODE_6, -yl2); M.set (NODE_6, NODE_4, -yl2);
  M.set (NODE_5, NODE_6, -yl3); M.set (NODE_6, NODE_5, -yl2);
  M.set (NODE_5, NODE_5, yl1 + yl1 + yl3);
  M.set (NODE_6, NODE_6, yl2 + yl2 + yl3);
  return M;
}

void mscross::initTR (void) {
  initDC ();
}
