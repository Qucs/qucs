/*
 * ifile.cpp - file based current source class implementation
 *
 * Copyright (C) 2007, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: ifile.cpp,v 1.2 2008/10/05 17:52:11 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "dataset.h"
#include "poly.h"
#include "spline.h"
#include "ifile.h"

// Type of data and interpolators.
#define INTERPOL_LINEAR  1
#define INTERPOL_CUBIC   2
#define INTERPOL_HOLD    3
#define REPEAT_NO 	 1
#define REPEAT_YES       2

ifile::ifile () : circuit (2) {
  type = CIR_IFILE;
  setISource (true);
  interpolType = dataType = 0;
  ts = NULL;
  is = NULL;
  sp = NULL;
  data = NULL;
}

// Destructor deletes ifile object from memory.
ifile::~ifile () {
  if (data) delete data;
  if (sp) delete sp;
}

void ifile::prepare (void) {

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
		  "independent and the current source samples as dependents\n",
		  file);
	return;
      }
      is = data->getVariables();    // current
      ts = data->getDependencies(); // time
      int tlen = ts->getSize();
      if (dataType == REPEAT_YES) {
	// waveform duration
	duration = real (ts->get (tlen - 1)) - real(ts->get (0));
	// set first current to the end of the current vector
	is->set (is->get (0), tlen - 1);
	// add samples at the end to perform correct interpolation
	if (interpolType == INTERPOL_CUBIC) {
	  // add two additional points
	  data->appendVariable (new vector(1,is->get(1)));
	  data->appendDependency (new vector(1,ts->get(1)+ts->get(tlen-1)));
	  data->appendVariable (new vector(1,is->get(2)));
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
nr_double_t ifile::interpolate (vector * var, vector * dep, nr_double_t x) {
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
nr_double_t ifile::interpolate_lin (vector * dep, vector * var, nr_double_t x,
				    int idx) {
  nr_double_t x1, x2, y1, y2;

  x1 = real (dep->get (idx));
  x2 = real (dep->get (idx + 1));
  y1 = real (var->get (idx));
  y2 = real (var->get (idx + 1));
  return ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
}

void ifile::initSP (void) {
  allocMatrixS ();
  setS (NODE_1, NODE_1, 1.0);
  setS (NODE_1, NODE_2, 0.0);
  setS (NODE_2, NODE_1, 0.0);
  setS (NODE_2, NODE_2, 1.0);
}

void ifile::initDC (void) {
  allocMatrixMNA ();
  prepare ();
}

void ifile::initAC (void) {
  initDC ();
}

void ifile::initTR (void) {
  initDC ();
}

void ifile::calcTR (nr_double_t t) {
  nr_double_t G = getPropertyDouble ("G");
  nr_double_t T = getPropertyDouble ("T");
  nr_double_t i = interpolate (is, ts, t - T);
  setI (NODE_1, +G * i); setI (NODE_2, -G * i);
}

// properties
struct define_t ifile::cirdef =
  { "Ifile", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    { { "File", PROP_STR, { PROP_NO_VAL, "ifile.dat" }, PROP_NO_RANGE },
      PROP_NO_PROP },
    { { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" }, PROP_NO_RANGE },
      { "Repeat", PROP_STR, { PROP_NO_VAL, "no" }, PROP_NO_RANGE },
      { "G", PROP_REAL, { 1, PROP_NO_STR }, PROP_NO_RANGE },
      { "T", PROP_REAL, { 0, PROP_NO_STR }, PROP_POS_RANGE },
      PROP_NO_PROP }
  };
