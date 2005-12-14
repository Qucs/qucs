/*
 * digisource.cpp - digital source class implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: digisource.cpp,v 1.2 2005-12-14 08:57:27 raimi Exp $
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
#include "vector.h"
#include "component_id.h"
#include "consts.h"
#include "digisource.h"

digisource::digisource () : circuit (1) {
  type = CIR_DIGISOURCE;
  setVSource (true);
  setVoltageSources (1);
}

void digisource::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, -1.0);
}

void digisource::initDC (void) {
  char * init = getPropertyString ("init");
  nr_double_t v = getPropertyDouble ("V");
  bool lo = !strcmp (init, "low");
  allocMatrixMNA ();
  setC (VSRC_1, NODE_1, 1.0);
  setB (NODE_1, VSRC_1, 1.0);
  setD (VSRC_1, VSRC_1, 0.0);
  setE (VSRC_1, lo ? 0 : v);
}

void digisource::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void digisource::initTR (void) {
  vector * values = getPropertyVector ("times");
  T = real (sum (*values));
  initDC ();
}

void digisource::calcTR (nr_double_t t) {
  char * init = getPropertyString ("init");
  nr_double_t v = getPropertyDouble ("V");
  vector * values = getPropertyVector ("times");
  bool lo = !strcmp (init, "low");
  nr_double_t ti = 0;

  t = t - T * floor (t / T);
  for (int i = 0; i < values->getSize (); i++) {
    ti += real (values->get (i));
    if (t >= ti) lo = !lo; else break;
  }

  setE (VSRC_1, lo ? 0 : v);
}
