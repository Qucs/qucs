/*
 * vrect.cpp - rectangular pulse voltage source class implementation
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: vrect.cpp,v 1.1 2004/10/03 10:30:51 ela Exp $
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
#include "consts.h"
#include "vrect.h"

vrect::vrect () : circuit (2) {
  setS (1, 1, 0.0);
  setS (1, 2, 1.0);
  setS (2, 1, 1.0);
  setS (2, 2, 0.0);
  type = CIR_VRECT;
  setVSource (true);
  setVoltageSources (1);
}

void vrect::initDC (void) {
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t u  = getPropertyDouble ("U") * th / (th + tl);
  voltageSource (1, 1, 2);
  setE (1, u);
}

void vrect::initAC (void) {
  initDC ();
  setE (1, 0);
}

void vrect::initTR (void) {
  initDC ();
}

void vrect::calcTR (nr_double_t t) {
  nr_double_t u  = getPropertyDouble ("U");
  nr_double_t th = getPropertyDouble ("TH");
  nr_double_t tl = getPropertyDouble ("TL");
  nr_double_t ut = 0;

  t = t - (th + tl) * floor (t / (th + tl));
  if (t < th) { // high pulse
    ut = u;
  }
  setE (1, ut);
}
