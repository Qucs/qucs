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
 * $Id: spfile.cpp,v 1.3 2004-07-26 06:30:29 ela Exp $
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
#include "matrix.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "analysis.h"
#include "spsolver.h"
#include "spfile.h"
#include "constants.h"

// Constructor creates an empty and unnamed instance of the spfile class.
spfile::spfile () : circuit () {
  data = NULL;
  ndata = NULL;
  sfreq = nfreq = Fmin = Gopt = Rn = NULL;
  index = NULL;
  type = CIR_SPFILE;
}

// Destructor deletes spfile object from memory.
spfile::~spfile () {
  if (index) free (index);
#if DEBUG
  if (data) {
    data->setFile ("spfile.dat");
    data->print ();
  }
#endif
  if (data) delete data;
}

void spfile::calcSP (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (index == NULL || sfreq == NULL) return;

  // set interpolated S-parameters
#if 0
  for (int r = 1; r <= getSize (); r++) {
    for (int c = 1; c <= getSize (); c++) {
      int i = (r - 1) * getSize () + c - 1;
      setS (r, c, interpolate (sfreq, index[i].v, frequency));
    }
  }
#endif
  matrix s = getInterpolatedMatrixS (frequency);
  fprintf (stderr, "S1=\n");
  s.print ();
  fprintf (stderr, "S2=\n");
  shrinkSParaMatrix (expandSParaMatrix (s)).print ();
  setMatrixS (expandSParaMatrix (s));
}

matrix& spfile::getInterpolatedMatrixS (nr_double_t frequency) {
  matrix * s = new matrix (getSize () - 1);
  for (int r = 1; r <= getSize () - 1; r++) {
    for (int c = 1; c <= getSize () - 1; c++) {
      int i = (r - 1) * getSize () + c - 1;
      s->set (r, c, interpolate (sfreq, index[i].v, frequency));
    }
  }
  return *s;
}

void spfile::calcNoise (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (index == NULL || nfreq == NULL) return;

  // set interpolated S-parameters
#if 0
  for (int r = 1; r <= getSize (); r++) {
    for (int c = 1; c <= getSize (); c++) {
      setN (r, c, interpolate (nfreq, &ndata->get (r, c), frequency));
    }
  }
#endif
  nr_double_t rn = real (interpolate (nfreq, Rn, frequency));
  complex gopt = interpolate (nfreq, Gopt, frequency);
  nr_double_t fmin = real (interpolate (nfreq, Fmin, frequency));
  matrix s = getInterpolatedMatrixS (frequency);
  matrix n = correlationMatrix (fmin, gopt, rn, s);
  fprintf (stderr, "C1=\n");
  n.print();
  fprintf (stderr, "C2=\n");
  shrinkNoiseMatrix (expandNoiseMatrix (n, expandSParaMatrix (s)), expandSParaMatrix (s)).print ();
  setMatrixN (expandNoiseMatrix (n, expandSParaMatrix (s)));

  nr_double_t f, fm, r;
  complex g;
  f = noisePara (s, n, fm, g, r);
  fprintf (stderr, "F: %g Fmin: %g==%g, Gopt: %g,%g==%g,%g, Rn: %g==%g\n",
	   f, fmin, fm, real(gopt),
	   imag (gopt),
	   real (g), imag (g), rn, r);

}

// Returns the specified matrix vector entry.
vector& spfile::fetchS (int r, int c) {
  return *(index[(r - 1) * getSize () + c - 1].v);
}

// Returns the matrix entry for the given frequency index.
complex spfile::fetchS (int r, int c, int idx) {
  vector * var = index[(r - 1) * getSize () + c - 1].v;
  return var->get (idx);
}

// Returns the matrix for the given frequency index.
matrix& spfile::fetchS (int idx) {
  matrix * res = new matrix (getSize () - 1);
  for (int r = 1; r <= getSize () - 1; r++) {
    for (int c = 1; c <= getSize () - 1; c++) {
      vector * v = index[(r - 1) * getSize () + c - 1].v;
      res->set (r, c, v->get (idx));
    }
  }
  return *res;
}

// Stores the matrix entry for the given frequency index.
void spfile::storeS (int r, int c, complex z, int idx) {
  vector * var = index[(r - 1) * getSize () + c - 1].v;
  return var->set (z, idx);
}

/* This function expands the actual S-parameter file data stored with
   the touchstone file to have an additional reference one-port whose
   S-parameter is -1 (i.e. ground). */
void spfile::expandPort (void) {
  int r, c, ports = getSize ();
  complex f, s, sr, sc, sa;
  nr_double_t g = -1; // reference port is ground

  // go through each S-parameter frequency point
  for (int n = 0; n < sfreq->getSize (); n++) {
    
    // compute S'mm
    for (sa = 0, r = 1; r <= ports - 1; r++)
      for (c = 1; c <= ports - 1; c++) sa += fetchS (r, c, n);
    s = (2 - g - ports + sa) / (1 - ports * g - sa);
    storeS (ports, ports, s, n);
    f = (1 - g * s) / (1 - g);
    
    // compute S'im
    for (r = 1; r <= ports - 1; r++) {
      for (sc = 0, c = 1; c <= ports - 1; c++) sc += fetchS (r, c, n);
      storeS (r, ports, f * (1 - sc), n);
    }

    // compute S'mj
    for (c = 1; c <= ports - 1; c++) {
      for (sr = 0, r = 1; r <= ports - 1; r++) sr += fetchS (r, c, n);
      storeS (ports, c, f * (1 - sr), n);
    }

    // compute S'ij
    for (r = 1; r <= ports - 1; r++) {
      for (c = 1; c <= ports - 1; c++) {
	f = g * fetchS (r, ports, n) * fetchS (ports, c, n) / (1 - g * s);
	storeS (r, c, fetchS (r, c, n) - f, n);
      }
    }
  }

  // also expand noise correlation matrix if necessary
  if (nfreq != NULL) {
    nr_double_t T = getPropertyDouble ("Temp");

    // create K matrix
    matvec k (nfreq->getSize (), ports, ports - 1);
    for (int r = 1; r <= ports - 1; r++) {
      for (int c = 1; c <= ports - 1; c++) {
	if (r == c)
	  k.set (1 + g * (fetchS (r, ports) - 1), r, c);
	else
	  k.set (g * fetchS (r, ports), r, c);
      }
    }
    for (int c = 1; c <= ports - 1; c++) {
      k.set (1 - g * fetchS (ports, ports), ports, c);
    }
    for (int r = 0; r < nfreq->getSize(); r++) {
      fprintf (stderr, "K[%d]=\n", r);
      k.get (r).print();
      fprintf (stderr, "K+[%d]=\n", r);
      transpose (conj (k.get(r))).print();
    }

    // create D vector
    matvec d (nfreq->getSize (), ports, 1);
    for (int r = 1; r <= ports - 1; r++) d.set (fetchS (r, ports), r, 1);
    d.set (fetchS (ports, ports) - 1, ports, 1);
    for (int r = 0; r < nfreq->getSize(); r++) {
      fprintf (stderr, "D[%d]=\n", r);
      d.get (r).print();
    }

    // expand noise correlation matrix
    matvec C;
    fprintf (stderr, "T=%g\n", kelvin (T));
    C = norm (1 / (1 - g)) * (k * (*ndata) * transpose (conj (k)) -
			      kelvin (T) / T0 * abs (1 - norm (g)) * d *
			      transpose (conj (d)));
    delete ndata;
    ndata = new matvec (C);
  }
}

matrix& spfile::expandNoiseMatrix (matrix& n, matrix& s) {
  nr_double_t T = getPropertyDouble ("Temp");
  int r, c, ports = n.getCols () + 1;
  nr_double_t g = -1;

  // create K matrix
  matrix k (ports, ports - 1);
  for (r = 1; r <= ports - 1; r++) {
    for (c = 1; c <= ports - 1; c++) {
      if (r == c)
	k.set (r, c, 1 + g * (s.get (r, ports) - 1));
      else
	k.set (r, c, g * s.get (r, ports));
    }
  }
  for (c = 1; c <= ports - 1; c++) {
    k.set (ports, c, 1 - g * s.get (ports, ports));
  }

  // create D vector
  matrix d (ports, 1);
  for (r = 1; r <= ports - 1; r++) d.set (r, 1, s.get (r, ports));
  d.set (ports, 1, s.get (ports, ports) - 1);

  // expand noise correlation matrix
  matrix * e = new matrix (ports);
  *e = (k * n * transpose (conj (k)) - kelvin (T) / T0 * abs (1 - norm (g)) *
	d * transpose (conj (d))) * norm (1 / (1 - g));
  return *e;
}

matrix& spfile::shrinkNoiseMatrix (matrix& n, matrix& s) {
  int r, ports = n.getCols ();
  nr_double_t g = -1;
  nr_double_t T = getPropertyDouble ("Temp");

  // create K' matrix
  matrix k (ports - 1, ports);
  for (r = 1; r <= ports - 1; r++) k.set (r, r, 1);
  for (r = 1; r <= ports - 1; r++)
    k.set (r, ports, g * s.get (r, ports) / (1 - g * s.get (ports, ports)));
  s.print();
  k.print();

  // create D' vector
  matrix d (ports - 1, 1);
  for (r = 1; r <= ports - 1; r++) d.set (r, 1, s.get (r, ports));
  d.print();
  
  // shrink noise correlation matrix
  matrix * e = new matrix (ports - 1);
  *e = k * n * transpose (conj (k)) + kelvin (T) / T0 * abs (1 - norm (g)) /
    norm (1 - g * s.get (ports, ports)) * d * transpose (conj (d));

  return *e;
}

matrix& spfile::expandSParaMatrix (matrix& s) {
  int r, c, ports = s.getCols () + 1;
  nr_double_t g = -1;
  complex fr, ss, sr, sc, sa;
  matrix * e = new matrix (ports);

  // compute S'mm
  for (sa = 0, r = 1; r <= ports - 1; r++)
    for (c = 1; c <= ports - 1; c++) sa += s.get (r, c);
  ss = (2 - g - ports + sa) / (1 - ports * g - sa);
  e->set (ports, ports, ss);
  fr = (1 - g * ss) / (1 - g);
    
  // compute S'im
  for (r = 1; r <= ports - 1; r++) {
    for (sc = 0, c = 1; c <= ports - 1; c++) sc += s.get (r, c);
    e->set (r, ports, fr * (1 - sc));
  }

  // compute S'mj
  for (c = 1; c <= ports - 1; c++) {
    for (sr = 0, r = 1; r <= ports - 1; r++) sr += s.get (r, c);
    e->set (ports, c, fr * (1 - sr));
  }

  // compute S'ij
  for (r = 1; r <= ports - 1; r++) {
    for (c = 1; c <= ports - 1; c++) {
      fr = g * e->get (r, ports) * e->get (ports, c) / (1 - g * ss);
      e->set (r, c, s.get (r, c) - fr);
    }
  }

  return *e;
}

matrix& spfile::shrinkSParaMatrix (matrix& s) {
  int r, c, ports = s.getCols ();
  nr_double_t g = -1;
  matrix * e = new matrix (ports - 1);

  // compute S'ij
  for (r = 1; r <= ports - 1; r++) {
    for (c = 1; c <= ports - 1; c++) {
      e->set (r, c, s.get (r, c) + g * s.get (r, ports)  *
	      s.get (ports, c) / (1 - g * s.get (ports, ports)));
    }
  }
  return *e;
}

void spfile::initSP (spsolver *) {

  // load S-parameter file
  char * file = getPropertyString ("File");
  if (data == NULL) data = dataset::load_touchstone (file);
  if (data != NULL) {
    // determine the number of ports defined by that file
    int ports = (int) sqrt (data->countVariables ());
    if (ports == getSize () - 1) {
      if (index == NULL) {
	createIndex ();
      }
      if (sfreq == NULL) {
	logprint (LOG_ERROR, "ERROR: file `%s' contains no `frequency' "
		  "vector\n", file);
      }
      else {
	//createNoiseMatrix ();
	//expandPort ();
      }
    }
    else {
      logprint (LOG_ERROR, "ERROR: file `%s' specifies a %d-port, `%s' "
		"requires a %d-port\n", file, ports, getName (),
		getSize () - 1);
    }
  }
}

/* The function creates an additional data vector for the given matrix
   entry and adds it to the dataset. */
void spfile::createVector (int r, int c) {
  int i = (r - 1) * getSize () + c - 1;
  index[i].r = r;
  index[i].c = c;
  vector * v = new vector (matvec::createMatrixString ("S", r, c), 
			   sfreq->getSize ());
  v->setDependencies (new strlist ());
  v->getDependencies()->add (sfreq->getName ());
  data->addVariable (v);
  index[i].v = v;
}

/* This function goes through the dataset stored within the original
   touchstone file and looks for the S-parameter matrices and
   frequency vector. */
void spfile::createIndex (void) {
  vector * v; int s = getSize (); char * n;
  int r, c, i;

  // create vector index
  index = (struct spfile_index_t *)
    calloc (1, sizeof (struct spfile_index_t) * s * s);

  // go through list of variable vectors and find matrix entries
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    if ((n = matvec::isMatrixVector (v->getName (), r, c)) != NULL) {
      // save matrix vector indices
      i = (r - 1) * s + c - 1;
      index[i].r = r;
      index[i].c = c;
      index[i].v = v;
      free (n);
    }
    if ((n = v->getName ()) != NULL) {
      // find noise parameter vectors
      if (!strcmp (n, "Rn")) Rn = v;
      else if (!strcmp (n, "Fmin")) Fmin = v;
      else if (!strcmp (n, "Gopt")) Gopt = v;
    }
  }

  // go through list of dependency vectors and find frequency vectors
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if ((n = v->getName ()) != NULL) {
      if (!strcmp (n, "frequency")) sfreq = v;
      else if (!strcmp (n, "nfreq")) nfreq = v;
    }
  }

  // expand definition by an additional reference port
  if (sfreq != NULL) {
    for (r = 1; r <= s - 1; r++) createVector (r, s);
    for (c = 1; c <= s - 1; c++) createVector (s, c);
    createVector (s, s);
  }
}

void spfile::createNoiseMatrix (void) {
  if (nfreq == NULL) return;
  vector * c11 = new vector (matvec::createMatrixString ("C", 1, 1));
  vector * c12 = new vector (matvec::createMatrixString ("C", 1, 2));
  vector * c21 = new vector (matvec::createMatrixString ("C", 2, 1));
  vector * c22 = new vector (matvec::createMatrixString ("C", 2, 2));
  vector s11 = fetchS (1, 1);
  vector s21 = fetchS (2, 1);

  vector Kx = 4 * (*Rn) / z0 / norm (1 + (*Gopt));
  *c11 = ((*Fmin) - 1) * (norm (s11) - 1) + Kx * norm (1 - s11 * (*Gopt));
  *c22 = norm (s21) * (((*Fmin) - 1) + Kx * norm (*Gopt));
  *c12 = s11 / s21 * (*c22) - conj (s21) * conj (*Gopt) * Kx;
  *c21 = conj (*c12);

  for (int n = 0; n < nfreq->getSize (); n++) {
    matrix c;
    complex g;
    nr_double_t f, fm, r;
    matrix s (2);
    s.set (1, 1, fetchS (1, 1, n));
    s.set (1, 2, fetchS (1, 2, n));
    s.set (2, 1, fetchS (2, 1, n));
    s.set (1, 2, fetchS (2, 2, n));
    c = correlationMatrix (real (Fmin->get (n)), Gopt->get (n),
			   real (Rn->get (n)), s);
    c.print ();
    f = noisePara (s, c, fm, g, r);
    fprintf (stderr, "F: %g Fmin: %g==%g, Gopt: %g,%g==%g,%g, Rn: %g==%g\n",
	     f, real (Fmin->get (n)), fm, real (Gopt->get (n)),
	     imag (Gopt->get (n)),
	     real (g), imag (g), real (Rn->get (n)), r);
  }

  c11->setDependencies (new strlist ());
  c11->getDependencies()->add (nfreq->getName ());
  data->addVariable (c11);
  c12->setDependencies (new strlist ());
  c12->getDependencies()->add (nfreq->getName ());
  data->addVariable (c12);
  c21->setDependencies (new strlist ());
  c21->getDependencies()->add (nfreq->getName ());
  data->addVariable (c21);
  c22->setDependencies (new strlist ());
  c22->getDependencies()->add (nfreq->getName ());
  data->addVariable (c22);

  ndata = new matvec (nfreq->getSize (), 2, 2);
  ndata->set (*c11, 1, 1);
  ndata->set (*c12, 1, 2);
  ndata->set (*c21, 2, 1);
  ndata->set (*c22, 2, 2);
}

matrix& spfile::correlationMatrix (nr_double_t Fmin, complex Gopt,
				   nr_double_t Rn, matrix& s) {
  matrix * c = new matrix (2);
  complex Kx = 4 * Rn / z0 / norm (1 + Gopt);
  c->set (1, 1, (Fmin - 1) * (norm (s.get (1, 1)) - 1) +
	  Kx * norm (1 - s.get (1, 1) * Gopt));
  c->set (2, 2, norm (s.get (2, 1)) * ((Fmin - 1) + Kx * norm (Gopt)));
  c->set (1, 2, s.get (1, 1) / s.get (2, 1) * c->get (2, 2) -
	  conj (s.get (2, 1)) * conj (Gopt) * Kx);
  c->set (2, 1, conj (c->get (1, 2)));
  return *c;
}

nr_double_t spfile::noisePara (matrix& s, matrix& c, nr_double_t& Fmin,
			       complex& Gopt, nr_double_t& Rn) {
  complex n1, n2;
  n1 = c.get (1, 1) * norm (s.get (2, 1)) -
    2 * real (c.get (1, 2) * s.get (2, 1) * conj (s.get (1, 1))) +
    c.get (2, 2) * norm (s.get (1, 1));
  n2 = 2 * (c.get (2, 2) * s.get (1, 1) -
	    c.get (1, 2) * s.get (2, 1)) / (c.get (2, 2) + n1);

  // optimal source reflection coefficient
  Gopt = 1 - norm (n2);
  if (real (Gopt) < 0.0)
    Gopt = 1;  // avoid a negative radicant
  else
    Gopt = (1 - sqrt (Gopt)) / n2;

  // minimum noise figure
  Fmin = real (1 + (c.get (2, 2) - n1 * norm (Gopt)) /
	       norm (s.get (2, 1)) / (1 + norm (Gopt)));

  // equivalent noise resistance
  Rn   = real (z0 * (c.get (1, 1) -
		     2 * real (c.get (1, 2) * conj ((1 + s.get (1, 1)) /
						    s.get (2, 1))) +
		     c.get (2, 2) * norm ((1 + s.get (1, 1)) /
					  s.get (2, 1))) / 4);
  return real (1 + c.get (2, 2) / norm (s.get (2, 1)));
}

/* This function returns an interpolated value for f(x).  The
   piecewise function f is given by 'var' and the appropriate
   dependent data by 'dep'.  The piecewise dependency data must be
   real and sorted in ascendant order. */
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

/* The function is called by the overall interpolator to perform
   linear interpolation. */
complex spfile::interpolate_lin (vector * dep, vector * var, nr_double_t x,
				 int idx) {
  nr_double_t x1, x2, y1, y2, f1, f2;
  char * type = getPropertyString ("Data");

  x1 = real (dep->get (idx));
  x2 = real (dep->get (idx + 1));

  // rectangular data
  if (!strcmp (type, "rectangular")) {
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
