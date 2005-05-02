/*
 * itrafo.cpp - ideal trafo class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: itrafo.cpp,v 1.5 2005-05-02 06:51:01 raimi Exp $
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
#include "component_id.h"
#include "itrafo.h"

itrafo::itrafo () : circuit (3) {
  type = CIR_ITRAFO;
}

void itrafo::initSP (void) {
  allocMatrixS ();
  nr_double_t z = getPropertyDouble ("Z");
  nr_double_t n = 2 * z0 + z;
  setS (NODE_1, NODE_1,  (2.0 * z0 - z) / n);
  setS (NODE_1, NODE_2,  (2.0 * sqrt (z0 * z)) / n);
  setS (NODE_1, NODE_3, -(2.0 * sqrt (z0 * z)) / n);
  setS (NODE_2, NODE_1,  (2.0 * sqrt (z0 * z)) / n);
  setS (NODE_2, NODE_2,  (z) / n);
  setS (NODE_2, NODE_3,  (2.0 * z0) / n);
  setS (NODE_3, NODE_1, -(2.0 * sqrt (z0 * z)) / n);
  setS (NODE_3, NODE_2,  (2.0 * z0) / n);
  setS (NODE_3, NODE_3,  (z) / n);
}
