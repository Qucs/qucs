/*
 * cccs.cpp - cccs class implementation
 *
 * Copyright (C) 2003, 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include "cccs.h"

using namespace qucs;

cccs::cccs () : circuit (4) {
  type = CIR_CCCS;
  setVoltageSources (1);
}

void cccs::calcSP (nr_double_t frequency) {

  nr_double_t g = getPropertyDouble ("G");
  nr_double_t t = getPropertyDouble ("T");

  nr_complex_t z1 = qucs::polar (g, M_PI - 2.0 * M_PI * frequency * t);
  nr_complex_t z2 = qucs::polar (g, - 2.0 * M_PI * frequency * t);

  setS (NODE_1, NODE_1, 0.0); setS (NODE_1, NODE_2, 0.0);
  setS (NODE_1, NODE_3, 0.0); setS (NODE_1, NODE_4, 1.0);
  setS (NODE_2, NODE_1, z1);  setS (NODE_2, NODE_2, 1.0);
  setS (NODE_2, NODE_3, 0.0); setS (NODE_2, NODE_4, z2);
  setS (NODE_3, NODE_1, z2);  setS (NODE_3, NODE_2, 0.0);
  setS (NODE_3, NODE_3, 1.0); setS (NODE_3, NODE_4, z1);
  setS (NODE_4, NODE_1, 1.0); setS (NODE_4, NODE_2, 0.0);
  setS (NODE_4, NODE_3, 0.0); setS (NODE_4, NODE_4, 0.0);
}

void cccs::initDC (void) {
  setISource (false);
  allocMatrixMNA ();
  nr_double_t g = getPropertyDouble ("G");
  setC (VSRC_1, NODE_1, +1.0); setC (VSRC_1, NODE_2, +0.0);
  setC (VSRC_1, NODE_3, +0.0); setC (VSRC_1, NODE_4, -1.0);
  setB (NODE_1, VSRC_1, +1/g); setB (NODE_2, VSRC_1, +1.0);
  setB (NODE_3, VSRC_1, -1.0); setB (NODE_4, VSRC_1, -1/g);
  setD (VSRC_1, VSRC_1, 0.0);
  setE (VSRC_1, 0.0);
}

void cccs::initAC (void) {
  initDC ();
}

void cccs::calcAC (nr_double_t frequency) {
  nr_double_t g = getPropertyDouble ("G");
  nr_double_t t = getPropertyDouble ("T");
  nr_complex_t r = qucs::polar (1.0 / g, - 2.0 * M_PI * frequency * t);
  setB (NODE_1, VSRC_1, +r); setB (NODE_4, VSRC_1, -r);
}

void cccs::initTR (void) {
  nr_double_t t = getPropertyDouble ("T");
  initDC ();
  deleteHistory ();
  if (t > 0.0) {
    setISource (true);
    setHistory (true);
    initHistory (t);
    setB (NODE_1, VSRC_1, +1.0); setB (NODE_2, VSRC_1, +0.0);
    setB (NODE_3, VSRC_1, -0.0); setB (NODE_4, VSRC_1, -1.0);
  }
}

void cccs::calcTR (nr_double_t t) {
  nr_double_t T = getPropertyDouble ("T");
  if (T > 0.0) {
    T = t - T;
    nr_double_t g = getPropertyDouble ("G");
    nr_double_t i = getJ (VSRC_1, T);
    setI (NODE_2, -g * i);
    setI (NODE_3, +g * i);
  }
}

// properties
PROP_REQ [] = {
  { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP };
PROP_OPT [] = {
  { "T", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE }, PROP_NO_PROP };
struct define_t cccs::cirdef =
  { "CCCS", 4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
