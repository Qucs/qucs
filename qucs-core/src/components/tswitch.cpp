/*
 * tswitch.cpp - time controlled switch class implementation
 *
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "tswitch.h"

tswitch::tswitch () : circuit (2) {
  type = CIR_TSWITCH;
  setVoltageSources (1);
}

nr_double_t tswitch::initState (void) {
  char * init = getPropertyString ("init");
  bool on = !strcmp (init, "on");
  return on ? getPropertyDouble ("Ron") : getPropertyDouble ("Roff");
}

void tswitch::initSP (void) {
  nr_double_t r = initState ();
  allocMatrixS ();
  setS (NODE_1, NODE_1, r / (r + 2));
  setS (NODE_2, NODE_2, r / (r + 2));
  setS (NODE_1, NODE_2, 2 / (r + 2));
  setS (NODE_2, NODE_1, 2 / (r + 2));
}

void tswitch::calcNoiseSP (nr_double_t) {
  nr_double_t T = getPropertyDouble ("Temp");
  nr_double_t r = initState ();
  nr_double_t f = kelvin (T) * 4.0 * r * z0 / sqr (2.0 * z0 + r) / T0;
  setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
  setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
}

void tswitch::calcNoiseAC (nr_double_t) {
  nr_double_t r = initState ();
  if (r > 0.0 || r < 0.0) {
    nr_double_t T = getPropertyDouble ("Temp");
    nr_double_t f = kelvin (T) / T0 * 4.0 / r;
    setN (NODE_1, NODE_1, +f); setN (NODE_2, NODE_2, +f);
    setN (NODE_1, NODE_2, -f); setN (NODE_2, NODE_1, -f);
  }
}

void tswitch::initDC (void) {
  nr_double_t r = initState ();
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  setD (VSRC_1, VSRC_1, -r);
}

void tswitch::initAC (void) {
  initDC ();
}

void tswitch::initTR (void) {
  ::vector * values = getPropertyVector ("time");
  T = real (sum (*values));
  repeat = (values->getSize () % 2) == 0 ? true : false;
  initDC ();
}

void tswitch::calcTR (nr_double_t t) {
  char * init = getPropertyString ("init");
  nr_double_t ron = getPropertyDouble ("Ron");
  nr_double_t roff = getPropertyDouble ("Roff");
  ::vector * values = getPropertyVector ("time");
  bool on = !strcmp (init, "on");
  nr_double_t ti = 0;

  if (repeat) t = t - T * floor (t / T);
  for (int i = 0; i < values->getSize (); i++) {
    ti += real (values->get (i));
    if (t >= ti) on = !on; else break;
  }

  setD (VSRC_1, VSRC_1, on ? -ron : -roff);
}

// properties
PROP_REQ [] = {
  { "init", PROP_STR, { PROP_NO_VAL, "off" }, PROP_RNG_STR2 ("on", "off") },
  { "time", PROP_LIST, { 1e-9, PROP_NO_STR }, PROP_POS_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Ron", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
  { "Roff", PROP_REAL, { 1e12, PROP_NO_STR }, PROP_POS_RANGE },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };
struct define_t tswitch::cirdef =
  { "Switch", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
