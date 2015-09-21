/*
 * expo_taperedline.cpp - ideal tapered transmission line class implementation
 *
 * Copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "expo_taperedline.h"
#include <matrix.h>

using namespace qucs;

expo_taperedline::expo_taperedline () : circuit (2) {
  type = CIR_expo_taperedline;
}
//**************************************************************
// This function calculates the S-parameter matrix of an exponential tapered
// line by discretizing it into differential slots.
void expo_taperedline::calcSparams(nr_double_t frequency)
{
  nr_double_t l = getPropertyDouble ("L");//Length
  nr_double_t z = getPropertyDouble ("Z1");//Source impedance
  nr_double_t a = getPropertyDouble ("a");//Exponential coefficient
  double lambda = C0/frequency;
  double step = 0.001*lambda;
  double Zi, Zaux=z;
  nr_complex_t T11, T12, T21, T22;
  matrix T = eye(2);
  matrix Taux = eye(2);
  for (int i = step; i < l; i+=step)
  {
    Zi = z*std::exp(a*i);
    S11 = (Zi-Zaux)/(Zi+Zaux);
    S21 = 2*std::sqrt(Zaux*Zi)/(Zaux+Zi);
    Zaux = Zi;
    T11 = (S11*S21 + S11*S11)/S21;
    T12 = S11/S21;
    T21 = S11/S21;
    T22 = 1./S21;
    Taux.set(1,1, T11);
    Taux.set(1,2, T12);
    Taux.set(2,1, T21);
    Taux.set(2,2, T22);
    T=T*Taux;
  }
  //Converting T matrix to S-parameters
  S11 = T.get(1,2);///T.get(2,2);
  S12 = (T.get(1,1)*T.get(2,2)-T.get(2,1)*T.get(1,2))/T.get(2,2);
  S21 = 1./T.get(2,2);
  S22 = -T.get(2,1)/T.get(2,2);

}

void expo_taperedline::calcSP (nr_double_t frequency) {
  calcSparams(frequency);
  setS (NODE_1, NODE_1, S11); setS (NODE_2, NODE_2, S22);
  setS (NODE_1, NODE_2, S12); setS (NODE_2, NODE_1, S21);
}

void expo_taperedline::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z0");
  nr_double_t a = getPropertyDouble ("a");
  a = std::log (a) / 2;
  a = std::exp (a * l);
  nr_double_t r = (z - z0) / (z + z0);
  nr_double_t f = (a - 1) * (r * r - 1) / sqr (a - r * r) * celsius2kelvin (T) / T0;
  nr_double_t n11 = -f * (r * r + a);
  nr_double_t n21 = +f * 2 * r * std::sqrt (a);
  setN (NODE_1, NODE_1, n11); setN (NODE_2, NODE_2, n11);
  setN (NODE_1, NODE_2, n21); setN (NODE_2, NODE_1, n21);
}

void expo_taperedline::calcNoiseAC (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z0");
  nr_double_t a = getPropertyDouble ("a");
  a = std::log (a) / 2;
  if (a * l != 0.0) {
    a = std::exp (a * l);
    nr_double_t f = 4.0 * celsius2kelvin (T) / T0 / z / (a - 1);
    nr_double_t n11 = +f * (a + 1);
    nr_double_t n21 = -f * 2 * std::sqrt (a);
    setN (NODE_1, NODE_1, n11); setN (NODE_2, NODE_2, n11);
    setN (NODE_1, NODE_2, n21); setN (NODE_2, NODE_1, n21);
  }
}

void expo_taperedline::initDC (void) {
  nr_double_t z = getPropertyDouble ("Z0");//Source impedance
  nr_double_t a = getPropertyDouble ("a");//Exponential coefficient
  nr_double_t l = getPropertyDouble ("L");//Length
  a = std::log (a) / 2;
  if (a * l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
    a = std::exp (a * l);
    nr_double_t f = 1 / z / (a - 1);
    nr_double_t y11 = +f * (a + 1);
    nr_double_t y21 = -f * 2 * std::sqrt (a);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  } else {
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void expo_taperedline::initAC (void) {
  nr_double_t l = getPropertyDouble ("L");
  if (l != 0.0) {
    setVoltageSources (0);
    allocMatrixMNA ();
  } else {
    setVoltageSources (1);
    allocMatrixMNA ();
    voltageSource (VSRC_1, NODE_1, NODE_2);
  }
}

void expo_taperedline::calcAC (nr_double_t frequency) {
  nr_double_t l = getPropertyDouble ("L");
  nr_double_t z = getPropertyDouble ("Z0");
  nr_double_t a = getPropertyDouble ("a");
  nr_double_t b = 2 * pi * frequency / C0;
  a = std::log (a) / 2;
  if (l != 0.0) {
    nr_complex_t y11 = +1 / z / tanh (nr_complex_t (a, b) * l);
    nr_complex_t y21 = -1 / z / sinh (nr_complex_t (a, b) * l);
    setY (NODE_1, NODE_1, y11); setY (NODE_2, NODE_2, y11);
    setY (NODE_1, NODE_2, y21); setY (NODE_2, NODE_1, y21);
  }
}


// properties
PROP_REQ [] = {
  { "Z0", PROP_REAL, { 50, PROP_NO_STR }, PROP_POS_RANGE },
  { "L", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "a", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Alpha", PROP_REAL, { 1, PROP_NO_STR }, PROP_POS_RANGEX },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t expo_taperedline::cirdef =
  { "EXPTAPTLIN", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
