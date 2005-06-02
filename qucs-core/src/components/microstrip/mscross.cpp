/*
 * mscross.cpp - microstrip cross-junction class implementation
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: mscross.cpp,v 1.4 2005-06-02 18:17:55 raimi Exp $
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
#include "mscross.h"

mscross::mscross () : circuit (4) {
  type = CIR_MSCROSS;
}

void mscross::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, -1.0 / 2.0);
  setS (NODE_1, NODE_2,  1.0 / 2.0);
  setS (NODE_1, NODE_3,  1.0 / 2.0);
  setS (NODE_1, NODE_4,  1.0 / 2.0);
  setS (NODE_2, NODE_1,  1.0 / 2.0);
  setS (NODE_2, NODE_2, -1.0 / 2.0);
  setS (NODE_2, NODE_3,  1.0 / 2.0);
  setS (NODE_2, NODE_4,  1.0 / 2.0);
  setS (NODE_3, NODE_1,  1.0 / 2.0);
  setS (NODE_3, NODE_2,  1.0 / 2.0);
  setS (NODE_3, NODE_3, -1.0 / 2.0);
  setS (NODE_3, NODE_4,  1.0 / 2.0);
  setS (NODE_4, NODE_1,  1.0 / 2.0);
  setS (NODE_4, NODE_2,  1.0 / 2.0);
  setS (NODE_4, NODE_3,  1.0 / 2.0);
  setS (NODE_4, NODE_4, -1.0 / 2.0);
}

void mscross::initDC (void) {
  setVoltageSources (3);
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  voltageSource (VSRC_2, NODE_1, NODE_3);
  voltageSource (VSRC_3, NODE_1, NODE_4);
}

void mscross::initAC (void) {
  initDC ();
}
