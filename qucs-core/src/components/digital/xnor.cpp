/*
 * xnor.cpp - logical xnor class implementation
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
 * $Id: xnor.cpp,v 1.5 2006/01/02 07:09:44 margraf Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "complex.h"
#include "object.h"
#include "circuit.h"
#include "component_id.h"
#include "constants.h"
#include "digital.h"
#include "xnor.h"

logicxnor::logicxnor () : digital () {
  type = CIR_XNOR;
  setVariableSized (true);
}

void logicxnor::calcOutput (void) {
  nr_double_t v = getPropertyDouble ("V");
  nr_double_t n = getSize () - 1;
  nr_double_t x;
  for (x = 1, i = 0; i < n; i++) {
    x *= -calcTransfer (i);
  }
  Vout = v / 2 * (1 + x);
}

void logicxnor::calcDerivatives (void) {
  nr_double_t n = getSize () - 1;
  nr_double_t x;
  for (int k = 0; k < n; k++) {
    for (x = 1, i = 0; i < n; i++) {
      if (i != k) x *= -calcTransfer (i);
    }
    g[k] = -0.5 * calcDerivative (k) * x;
  }
}
