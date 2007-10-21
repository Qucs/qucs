/*
 * vfile.cpp - file based voltage source class implementation
 *
 * Copyright (C) 2007 Gunther Kraut <gn.kraut@t-online.de>
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: vfile.cpp,v 1.4 2007/10/21 20:08:02 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __MINGW32__
#define strcasecmp stricmp
#endif

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "dataset.h"
#include "net.h"
#include "component_id.h"
#include "vector.h"
#include "consts.h"
#include "poly.h"
#include "spline.h"
#include "vfile.h"

// Type of data and interpolators.
#define INTERPOL_LINEAR  1
#define INTERPOL_CUBIC   2
#define INTERPOL_HOLD    3
#define REPEAT_NO 	 1
#define REPEAT_YES       2

vfile::vfile () : circuit (2) {
  type = CIR_VFILE;
  setVSource (true);
  setVoltageSources (1);
  interpolType = dataType = 0;
  ts = NULL;
  vs = NULL;
  sp = NULL;
  data = NULL;
}

// Destructor deletes vfile object from memory.
vfile::~vfile () {
  if (data) delete data;
  if (sp) delete sp;
}

void vfile::prepare (void) {

  // check type of interpolator
  char * type = getPropertyString ("Interpolator");
  if (!strcmp (type, "linear")) {
    interpolType = INTERPOL_LINEAR;
  } else if (!strcmp (type, "cubic")) {
    interpolType = INTERPOL_CUBIC;
  } else if (!strcmp (type, "hold")) {
    interpolType = INTERPOL_HOLD;
  }

  // check type of repetition
  type = getPropertyString ("Repeat");
  if (!strcmp (type, "no")) {
    // rectangular data
    dataType = REPEAT_NO;
  } else if (!strcmp (type, "yes")) {
    // polar data
    dataType = REPEAT_YES;
  }

  // load file with samples
  char * file = getPropertyString ("File");
  if (data == NULL) {
    if (strlen (file) > 4 && !strcasecmp (&file[strlen (file) - 4], ".dat"))
      data = dataset::load (file);
    else
      data = dataset::load_csv (file);
    if (data != NULL) {
      // check number of variables / dependencies defined by that file
      if (data->countVariables () != 1 || data->countDependencies () != 1) {
	logprint (LOG_ERROR, "ERROR: file `%s' must have time as an "
		  "independent and the voltage source samples as dependents\n",
		  file);
	return;
      }
      vs = data->getVariables();    // voltage
      ts = data->getDependencies(); // time
      int tlen = ts->getSize();
      if (dataType == REPEAT_YES) {
	// waveform duration
	duration = real (ts->get (tlen - 1)) - real(ts->get (0));
	// set first voltage to the end of the voltage vector
	vs->set (vs->get (0), tlen - 1);
	// add samples at the end to perform correct interpolation
	if (interpolType == INTERPOL_CUBIC) {
	  // add two additional points
	  data->appendVariable (new vector(1,vs->get(1)));
	  data->appendDependency (new vector(1,ts->get(1)+ts->get(tlen-1)));
	  data->appendVariable (new vector(1,vs->get(2)));
	  data->appendDependency (new vector(1,ts->get(2)+ts->get(tlen-1)));
	}
      }
    }
  }
}

/* This function returns an interpolated value for f(x).  The
   piecewise function f is given by 'var' and the appropriate
   dependent data by 'dep'.  The piecewise dependency data must be
   real and sorted in ascendant order. */
nr_double_t vfile::interpolate (vector * var, vector * dep, nr_double_t x) {
  int idx = -1, len = dep->getSize ();
  nr_double_t res = 0;

  // no chance to interpolate
  if (len <= 0) {
    return res;
  }
  // no interpolation necessary
  else if (len == 1) {
    res = real (var->get (0));
    return res;
  }
  else if (dataType == REPEAT_YES)
    x = x - floor (x / duration) * duration;

  // linear interpolation
  if (interpolType == INTERPOL_LINEAR) {
    // find appropriate dependency index
    for (int i = 0; i < len; i++) {
      if (x >= real (dep->get (i))) idx = i;
    }
    // dependency variable in scope or beyond
    if (idx != -1) {
      if (x == real (dep->get (idx))) {
	// found direct value
        res = real (var->get (idx));
      }
      else {
	// dependency variable is beyond scope; use last tangent
	if (idx == len - 1) idx--;
	res = interpolate_lin (dep, var, x, idx);
      }
    }
    else {
      // dependency variable is before scope; use first tangent
      idx = 0;
      res = interpolate_lin (dep, var, x, idx);
    }
  }
  // cubic spline interpolation
  else if (interpolType == INTERPOL_CUBIC) {
    // create splines if necessary
    if (sp == NULL) {
      sp = new spline (SPLINE_BC_NATURAL);
      if (dataType == REPEAT_YES) sp->setBoundary (SPLINE_BC_PERIODIC);
      sp->vectors (*var, *dep);
      sp->construct ();
    }
    // evaluate spline functions
    res = sp->evaluate (x).f0;
  }
  else if (interpolType == INTERPOL_HOLD) {
    // find appropriate dependency index
    for (int i = 0; i < len; i++) {
      if (x >= real (dep->get (i))) idx = i;
    }
    if (idx != -1) // dependency variable in scope or beyond
      res = real (var->get (idx));
    else // dependency variable is before scope
      res = real (var->get (0));
  }
  return res;
}
 

/* The function is called by the overall interpolator to perform
   linear interpolation. */
nr_double_t vfile::interpolate_lin (vector * dep, vector * var, nr_double_t x,
				    int idx) {
  nr_double_t x1, x2, y1, y2;

  x1 = real (dep->get (idx));
  x2 = real (dep->get (idx + 1));
  y1 = real (var->get (idx));
  y2 = real (var->get (idx + 1));
  return ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
}

void vfile::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 0.0);
  setS (NODE_1, NODE_2, 1.0);
  setS (NODE_2, NODE_1, 1.0);
  setS (NODE_2, NODE_2, 0.0);
}

void vfile::initDC (void) {
  allocMatrixMNA ();
  voltageSource (VSRC_1, NODE_1, NODE_2);
  prepare ();
  setE (VSRC_1, 0);
}

void vfile::initAC (void) {
  initDC ();
  setE (VSRC_1, 0);
}

void vfile::initTR (void) {
  initDC ();
}

void vfile::calcTR (nr_double_t t) {
  nr_double_t G = getPropertyDouble ("G");
  nr_double_t T = getPropertyDouble ("T");
  nr_double_t u = interpolate (vs, ts, t - T);
  setE (VSRC_1, G * u);
}
