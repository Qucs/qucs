/*
 * dcfeed.cpp - DC feed class implementation
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
 * $Id: dcfeed.cpp,v 1.5 2004-05-23 15:27:26 ela Exp $
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
#include "dcfeed.h"

dcfeed::dcfeed () : circuit (2) {
  setS (1, 1, 1.0);
  setS (2, 2, 1.0);
  setS (1, 2, 0.0);
  setS (2, 1, 0.0);
  type = CIR_DCFEED;
  setVoltageSources (1);
}

void dcfeed::calcDC (void) {
  setC (1, 1, +1.0); setC (1, 2, -1.0);
  setB (1, 1, +1.0); setB (2, 1, -1.0);
  setE (1, 0.0);
  setD (1, 1, 0.0);
}
