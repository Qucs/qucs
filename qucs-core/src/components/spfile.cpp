/*
 * spfile.cpp - S-parameter file class implementation
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
 * $Id: spfile.cpp,v 1.1 2004-07-21 16:25:09 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "component_id.h"
#include "vector.h"
#include "matvec.h"
#include "dataset.h"
#include "spfile.h"
#include "constants.h"

spfile::spfile () : circuit () {
  data = NULL;
  dep = NULL;
  index = NULL;
  type = CIR_SPFILE;
}

spfile::~spfile () {
  if (index) free (index);
  if (data) delete data;
}

void spfile::calcSP (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (index == NULL || dep == NULL) return;

  // set interpolated S-parameters
  for (int r = 1; r <= getSize (); r++) {
    for (int c = 1; c <= getSize (); c++) {
      int i = (r - 1) * getSize () + c - 1;
      vector * var = index[i].v;
      setS (r, c, interpolate (dep, var, frequency));
    }
  }
}

void spfile::initSP (void) {

  // load S-parameter file
  char * file = getPropertyString ("File");
  if (data == NULL) data = dataset::load_touchstone (file);
  if (data != NULL) {
    // determine the number of ports defined by that file
    int ports = (int) sqrt (data->countVariables ());
    if (ports == getSize ()) {
      if (index == NULL) createIndex ();
      if (dep == NULL) {
	logprint (LOG_ERROR, "ERROR: file `%s' contains no `frequency' "
		  "vector\n", file);
      }
    }
    else {
      logprint (LOG_ERROR, "ERROR: file `%s' specifies a %d-port, `%s' "
		"requires a %d-port\n", file, ports, getName (), getSize ());
    }
  }
}

void spfile::createIndex (void) {
  vector * v; int s = getSize (); char * n;

  // create vector index
  index = (struct spfile_index_t *)
    calloc (1, sizeof (struct spfile_index_t) * s * s);

  // go through list of variable vectors and find matrix entries
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    int r, c, i;
    if ((n = matvec::isMatrixVector (v->getName (), r, c)) != NULL) {
      // save matrix vector indices
      i = (r - 1) * s + c - 1;
      index[i].r = r;
      index[i].c = c;
      index[i].v = v;
      free (n);
    }
  }

  // go through list of dependency vectors and find frequency vector
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if ((n = v->getName ()) != NULL) {
      if (!strcmp (n, "frequency")) dep = v;
    }
  }
}

complex spfile::interpolate (vector * dep, vector * var, nr_double_t x) {
  int idx = -1, len = dep->getSize ();
  complex res = 0;

  if (len != 0) {   // no chance to interpolate
    if (len == 1) { // no interpolation necessary
      res = var->get (0);
    }
    else {
      // find appropriate dependency index
      for (int i = 0; i < len; i++) {
	if (x >= real (dep->get (i))) {
	  idx = i;
	}
      }
      // dependency variable in scope or beyond
      if (idx != -1) {
	if (x == real (dep->get (idx))) {
	  // found direct value
	  res = var->get (idx);
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
  }
  return res;
}

complex spfile::interpolate_lin (vector * dep, vector * var,
				 nr_double_t x, int idx) {
  nr_double_t x1, x2, y1, y2, f1, f2;
  char * type = getPropertyString ("Data");

  x1 = real (dep->get (idx));
  x2 = real (dep->get (idx + 1));

  // rectangular data
  if (!strcmp (type, "rect")) {
    y1 = real (var->get (idx));
    y2 = real (var->get (idx + 1));
    f1 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    y1 = imag (var->get (idx));
    y2 = imag (var->get (idx + 1));
    f2 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    return rect (f1, f2);
  }
  // polar data
  else if (!strcmp (type, "polar")) {
    y1 = abs (var->get (idx));
    y2 = abs (var->get (idx + 1));
    f1 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    y1 = arg (var->get (idx));
    y2 = arg (var->get (idx + 1));
    if (y1 < 0 && y2 > 0 && fabs (y2 - y1) >= M_PI) {
      // fix clockwise phase steps in left half-plane
      y1 += 2 * M_PI;
    }
    f2 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    return polar (f1, f2);
  }
  return 0;
}
