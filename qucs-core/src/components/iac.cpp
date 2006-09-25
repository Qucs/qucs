/*
 * iac.cpp - AC current source class implementation
 *
 * Copyright (C) 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: iac.cpp,v 1.13 2006-09-25 07:45:01 raimi Exp $
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
#include "component_id.h"
#include "consts.h"
#include "constants.h"
#include "iac.h"

iac::iac () : circuit (2) {
  type = CIR_IAC;
  setISource (true);
}

void iac::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void iac::initDC (void) {
  allocMatrixMNA ();
  clearI ();
}

void iac::initAC (void) {
  nr_double_t a = getPropertyDouble ("I");
  nr_double_t p = getPropertyDouble ("Phase");
  complex i = polar (a, rad (p));
  allocMatrixMNA ();
  setI (NODE_1, +i); setI (NODE_2, -i);
}

void iac::calcTR (nr_double_t t) {
  nr_double_t f = getPropertyDouble ("f");
  nr_double_t p = getPropertyDouble ("Phase");
  nr_double_t d = getPropertyDouble ("Theta");
  nr_double_t a = getPropertyDouble ("I");
  nr_double_t s = getNet()->getSrcFactor ();
  nr_double_t i = s * a * exp (-d * t * f) * sin (2 * M_PI * f * t + rad (p));
  setI (NODE_1, +i); setI (NODE_2, -i);
}
