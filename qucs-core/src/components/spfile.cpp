/*
 * spfile.cpp - S-parameter file class implementation
 *
 * Copyright (C) 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: spfile.cpp,v 1.31 2008-10-07 20:15:32 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "component.h"
#include "matvec.h"
#include "dataset.h"
#include "strlist.h"
#include "poly.h"
#include "spline.h"
#include "spfile.h"

// Type of data and interpolators.
#define DATA_RECTANGULAR 1
#define DATA_POLAR       2
#define INTERPOL_LINEAR  1
#define INTERPOL_CUBIC   2

// Constructor creates an empty and unnamed instance of the spfile class.
spfile::spfile () : circuit () {
  data = NULL;
  sfreq = nfreq = NULL;
  spara = FMIN = SOPT = RN = NULL;
  interpolType = dataType = 0;
  type = CIR_SPFILE;
  setVariableSized (true);
}

// Destructor deletes spfile object from memory.
spfile::~spfile () {
  if (spara) {
    for (int i = 0; i < sqr (getSize ()); i++) {
      if (spara[i].v1) delete spara[i].v1;
      if (spara[i].v2) delete spara[i].v2;
    }
    free (spara);
  }
  if (RN) {
    if (RN->v1) delete RN->v1;
    if (RN->v2) delete RN->v2;
    free (RN);
  }
  if (FMIN) {
    if (FMIN->v1) delete FMIN->v1;
    if (FMIN->v2) delete FMIN->v2;
    free (FMIN);
  }
  if (SOPT) {
    if (SOPT->v1) delete SOPT->v1;
    if (SOPT->v2) delete SOPT->v2;
    free (SOPT);
  }
#if DEBUG && 0
  if (data) {
    data->setFile ("spfile.dat");
    data->print ();
  }
#endif
  if (data) delete data;
}

void spfile::calcSP (nr_double_t frequency) {

  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;

  // set interpolated S-parameters
  setMatrixS (expandSParaMatrix (getInterpolMatrixS (frequency)));
}

/* This function returns the S-parameter matrix of the circuit for the
   given frequency.  It uses interpolation for frequency points which
   are not part of the original touchstone file. */
matrix spfile::getInterpolMatrixS (nr_double_t frequency) {

  // first interpolate the matrix values
  matrix s (getSize () - 1);
  for (int r = 0; r < getSize () - 1; r++) {
    for (int c = 0; c < getSize () - 1; c++) {
      int i = r * getSize () + c;
      s.set (r, c, interpolate (&spara[i], frequency));
    }
  }
  
  // then convert them to S-parameters if necessary
  switch (paraType) {
  case 'Y':
    s = ytos (s);
    break;
  case 'Z':
    s = ztos (s);
    break;
  case 'H':
    s = htos (s);
    break;
  case 'G':
    s = gtos (s);
    break;
  }
  return s;
}

void spfile::calcNoiseSP (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || nfreq == NULL) return;
  setMatrixN (calcMatrixCs (frequency));
}

matrix spfile::calcMatrixCs (nr_double_t frequency) {
  // set interpolated noise correlation matrix
  nr_double_t r = real (interpolate (RN, frequency));
  nr_double_t f = real (interpolate (FMIN, frequency));
  nr_complex_t g = interpolate (SOPT, frequency);
  matrix s = getInterpolMatrixS (frequency);
  matrix n = correlationMatrix (f, g, r, s);
  matrix c = expandNoiseMatrix (n, expandSParaMatrix (s));
  return c;
}

/* This function expands the actual S-parameter file data stored
   within the touchstone file to have an additional reference one-port
   whose S-parameter is -1 (i.e. ground). */
matrix spfile::expandSParaMatrix (matrix s) {
  assert (s.getCols () == s.getRows ());
  int r, c, ports = s.getCols () + 1;
  nr_double_t g = -1;
  nr_complex_t fr, ss, sr, sc, sa;
  matrix res (ports);

  // compute S'mm
  for (sa = 0, r = 0; r < ports - 1; r++)
    for (c = 0; c < ports - 1; c++) sa += s.get (r, c);
  ss = (2 - g - ports + sa) / (1 - ports * g - sa);
  res.set (ports - 1, ports - 1, ss);
  fr = (1.0 - g * ss) / (1.0 - g);
    
  // compute S'im
  for (r = 0; r < ports - 1; r++) {
    for (sc = 0, c = 0; c < ports - 1; c++) sc += s.get (r, c);
    res.set (r, ports - 1, fr * (1.0 - sc));
  }

  // compute S'mj
  for (c = 0; c < ports - 1; c++) {
    for (sr = 0, r = 0; r < ports - 1; r++) sr += s.get (r, c);
    res.set (ports - 1, c, fr * (1.0 - sr));
  }

  // compute S'ij
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      fr = g * res (r, ports - 1) * res (ports - 1, c) / (1.0 - g * ss);
      res.set (r, c, s.get (r, c) - fr);
    }
  }

  return res;
}

/* The function is the counterpart of the above expandSParaMatrix()
   function.  It shrinks the S-parameter matrix by removing the
   reference port. */
matrix spfile::shrinkSParaMatrix (matrix s) {
  assert (s.getCols () == s.getRows () && s.getCols () > 0);
  int r, c, ports = s.getCols ();
  nr_double_t g = -1;
  matrix res (ports - 1);

  // compute S'ij
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      res.set (r, c, s (r, c) + g * s (r, ports - 1)  *
	       s (ports - 1, c) / (1.0 - g * s (ports - 1, ports - 1)));
    }
  }
  return res;
}

/* This function expands the actual noise correlation matrix to have an
   additional reference one-port whose S-parameter is -1
   (i.e. ground).  The given S-parameter matrix is required to perform
   this transformation and is obtained using the expandSParaMatrix()
   function. */
matrix spfile::expandNoiseMatrix (matrix n, matrix s) {
  assert (s.getCols () == s.getRows () && n.getCols () == n.getRows () &&
	  n.getCols () == s.getCols () - 1);
  nr_double_t T = getPropertyDouble ("Temp");
  int r, c, ports = n.getCols () + 1;
  nr_double_t g = -1;

  // create K matrix
  matrix k (ports, ports - 1);
  for (r = 0; r < ports - 1; r++) {
    for (c = 0; c < ports - 1; c++) {
      if (r == c)
	k.set (r, c, 1.0 + g * (s.get (r, ports - 1) - 1.0));
      else
	k.set (r, c, g * s.get (r, ports - 1));
    }
  }
  for (c = 0; c < ports - 1; c++)
    k.set (ports - 1, c, g * s.get (ports - 1, ports - 1) - 1.0);

  // create D vector
  matrix d (ports, 1);
  for (r = 0; r < ports - 1; r++) d.set (r, 0, s.get (r, ports - 1));
  d.set (ports - 1, 0, s.get (ports - 1, ports - 1) - 1.0);

  // expand noise correlation matrix
  matrix res (ports);
  res = (k * n * adjoint (k) - kelvin (T) / T0 * fabs (1 - norm (g)) *
	 d * adjoint (d)) * norm (1 / (1 - g));
  return res;
}

/* The function is the counterpart of the above expandNoiseMatrix()
   function.  It shrinks the noise correlation matrix by removing the
   reference port.  The given S-parameter matrix is required to perform
   this transformation and is obtained using the expandSParaMatrix()
   function. */
matrix spfile::shrinkNoiseMatrix (matrix n, matrix s) {
  assert (s.getCols () == s.getRows () && n.getCols () == n.getRows () &&
	  n.getCols () == s.getCols () && n.getCols () > 0);
  int r, ports = n.getCols ();
  nr_double_t g = -1;
  nr_double_t T = getPropertyDouble ("Temp");

  // create K' matrix
  matrix k (ports - 1, ports);
  for (r = 0; r < ports - 1; r++) k.set (r, r, 1);
  for (r = 0; r < ports - 1; r++)
    k.set (r, ports - 1, g * s.get (r, ports - 1) /
	   (1.0 - g * s.get (ports - 1, ports - 1)));

  // create D' vector
  matrix d (ports - 1, 1);
  for (r = 0; r < ports - 1; r++) d.set (r, 0, s.get (r, ports - 1));
  
  // shrink noise correlation matrix
  matrix res (ports - 1);
  res = k * n * adjoint (k) + kelvin (T) / T0 * fabs (1.0 - norm (g)) /
    norm (1.0 - g * s.get (ports - 1, ports - 1)) * d * adjoint (d);
  return res;
}

void spfile::prepare (void) {

  // check type of data
  char * type = getPropertyString ("Data");
  if (!strcmp (type, "rectangular")) {
    // rectangular data
    dataType = DATA_RECTANGULAR;
  }
  else if (!strcmp (type, "polar")) {
    // polar data
    dataType = DATA_POLAR;
  }

  // check type of interpolator
  type = getPropertyString ("Interpolator");
  if (!strcmp (type, "linear")) {
    interpolType = INTERPOL_LINEAR;
  }
  else if (!strcmp (type, "cubic")) {
    interpolType = INTERPOL_CUBIC;
  }

  // load S-parameter file
  char * file = getPropertyString ("File");
  if (data == NULL) data = dataset::load_touchstone (file);
  if (data != NULL) {
    // determine the number of ports defined by that file
    int ports = (int) sqrt ((double) data->countVariables ());
    if (ports == getSize () - 1) {
      if (spara == NULL) {
	// find matrix vector entries in touchstone dataset
	createIndex ();
      }
      if (sfreq == NULL) {
	logprint (LOG_ERROR, "ERROR: file `%s' contains no `frequency' "
		  "vector\n", file);
      }
    }
    else {
      logprint (LOG_ERROR, "ERROR: file `%s' specifies a %d-port, `%s' "
		"requires a %d-port\n", file, ports, getName (),
		getSize () - 1);
    }
  }
}

void spfile::initSP (void) {
  // allocate S-parameter matrix
  allocMatrixS ();
  // initialize data
  prepare ();
}

/* The function creates an additional data vector for the given matrix
   entry and adds it to the dataset. */
void spfile::createVector (int r, int c) {
  int i = r * getSize () + c;
  spara[i].r = r;
  spara[i].c = c;
  vector * v = new vector (matvec::createMatrixString ("S", r, c), 
			   sfreq->getSize ());
  v->setDependencies (new strlist ());
  v->getDependencies()->add (sfreq->getName ());
  data->addVariable (v);
  spara[i].v = v;
}

/* This function goes through the dataset stored within the original
   touchstone file and looks for the S-parameter matrices and
   frequency vector.  It also tries to find the noise parameter
   data. */
void spfile::createIndex (void) {
  vector * v; int s = getSize (); char * n;
  int r, c, i;

  // go through list of dependency vectors and find frequency vectors
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if ((n = v->getName ()) != NULL) {
      if (!strcmp (n, "frequency")) sfreq = v;
      else if (!strcmp (n, "nfreq")) nfreq = v;
    }
  }

  // create vector index
  spara = (struct spfile_index_t *)
    calloc (1, sizeof (struct spfile_index_t) * s * s);

  // go through list of variable vectors and find matrix entries
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    if ((n = matvec::isMatrixVector (v->getName (), r, c)) != NULL) {
      // save matrix vector indices
      i = r * s + c;
      spara[i].r = r;
      spara[i].c = c;
      spara[i].v = v;
      spara[i].f = sfreq;
      paraType = n[0];  // save type of touchstone data
      free (n);
    }
    if ((n = v->getName ()) != NULL) {
      // find noise parameter vectors
      if (!strcmp (n, "Rn")) {
	RN = (struct spfile_index_t *)
	  calloc (1, sizeof (struct spfile_index_t));
	RN->v = v;
	RN->f = nfreq;
      }
      else if (!strcmp (n, "Fmin")) {
	FMIN = (struct spfile_index_t *)
	  calloc (1, sizeof (struct spfile_index_t));
	FMIN->v = v;
	FMIN->f = nfreq;
      }
      else if (!strcmp (n, "Sopt")) {
	SOPT = (struct spfile_index_t *)
	  calloc (1, sizeof (struct spfile_index_t));
	SOPT->v = v;
	SOPT->f = nfreq;
      }
    }
  }
}

/* This function computes the noise correlation matrix of a twoport
   based upon the noise parameters and the given S-parameter
   matrix. */
matrix spfile::correlationMatrix (nr_double_t Fmin, nr_complex_t Sopt,
				  nr_double_t Rn, matrix s) {
  assert (s.getCols () == s.getRows () && s.getCols () == 2);
  matrix c (2);
  nr_complex_t Kx = 4 * Rn / z0 / norm (1.0 + Sopt);
  c.set (0, 0, (Fmin - 1) * (norm (s.get (0, 0)) - 1) +
	 Kx * norm (1.0 - s.get (0, 0) * Sopt));
  c.set (1, 1, norm (s.get (1, 0)) * ((Fmin - 1) + Kx * norm (Sopt)));
  c.set (0, 1, s.get (0, 0) / s.get (1, 0) * c.get (1, 1) -
	 conj (s.get (1, 0)) * conj (Sopt) * Kx);
  c.set (1, 0, conj (c.get (0, 1)));
  return c;
}

/* The function computes the noise figure and noise parameters for the
   given S-parameter and noise correlation matrices of a twoport. */
nr_double_t spfile::noiseFigure (matrix s, matrix c, nr_double_t& Fmin,
				 nr_complex_t& Sopt, nr_double_t& Rn) {
  assert (s.getCols () == s.getRows () && c.getCols () == c.getRows () &&
	  s.getCols () == 2 && c.getCols () == 2);
  nr_complex_t n1, n2;
  n1 = c.get (0, 0) * norm (s.get (1, 0)) -
    2 * real (c.get (0, 1) * s.get (1, 0) * conj (s.get (0, 0))) +
    c.get (1, 1) * norm (s.get (0, 0));
  n2 = 2.0 * (c.get (1, 1) * s.get (0, 0) -
	      c.get (0, 1) * s.get (1, 0)) / (c.get (1, 1) + n1);

  // optimal source reflection coefficient
  Sopt = 1 - norm (n2);
  if (real (Sopt) < 0.0)
    Sopt = (1.0 + sqrt (Sopt)) / n2;  // avoid a negative radicant
  else
    Sopt = (1.0 - sqrt (Sopt)) / n2;

  // minimum noise figure
  Fmin = real (1.0 + (c.get (1, 1) - n1 * norm (Sopt)) /
	       norm (s.get (1, 0)) / (1.0 + norm (Sopt)));

  // equivalent noise resistance
  Rn = real ((c (0, 0) - 2.0 *
	      real (c (0, 1) * conj ((1.0 + s (0, 0)) / s (1, 0))) +
	      c (1, 1) * norm ((1.0 + s (0, 0)) / s (1, 0))) / 4.0);
  Rn = Rn * z0;

  // noise figure itself
  return real (1.0 + c.get (1, 1) / norm (s.get (1, 0)));
}

/* This function returns an interpolated value for f(x).  The
   piecewise function f is given by 'var' and the appropriate
   dependent data by 'dep'.  The piecewise dependency data must be
   real and sorted in ascendant order. */
nr_complex_t spfile::interpolate (struct spfile_index_t * data,
				  nr_double_t x) {
  vector * dep = data->f;
  vector * var = data->v;
  int idx = -1, len = dep->getSize ();
  nr_complex_t res = 0;

  // no chance to interpolate
  if (len <= 0) {
    return res;
  }
  // no interpolation necessary
  else if (len == 1) {
    res = var->get (0);
  }
  // linear interpolation
  else if (interpolType == INTERPOL_LINEAR || len < 3) {
    // find appropriate dependency index
    for (int i = 0; i < len; i++) {
      if (x >= real (dep->get (i))) idx = i;
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
  // cubic spline interpolation
  else if (interpolType == INTERPOL_CUBIC) {
    // create splines if necessary
    if (data->v1 == NULL && data->v2 == NULL) {
      data->v1 = new spline (SPLINE_BC_NATURAL);
      data->v2 = new spline (SPLINE_BC_NATURAL);
      if (dataType == DATA_RECTANGULAR) {
	data->v1->vectors (real (*var), *dep);
	data->v2->vectors (imag (*var), *dep);
      }
      else if (dataType == DATA_POLAR) {
	data->v1->vectors (abs (*var), *dep);
	data->v2->vectors (unwrap (arg (*var)), *dep);
      }
      data->v1->construct ();
      data->v2->construct ();
    }
    // evaluate spline functions
    res = interpolate_spl (data->v1, data->v2, x);
  }
  return res;
}

/* The function is called by the overall interpolator to perform
   cubic spline interpolation. */
nr_complex_t spfile::interpolate_spl (spline * v1, spline * v2,
				      nr_double_t x) {
  nr_double_t f1 = v1->evaluate (x).f0;
  nr_double_t f2 = v2->evaluate (x).f0;
  // rectangular data
  if (dataType == DATA_RECTANGULAR) {
    return rect (f1, f2);
  }
  // polar data
  else if (dataType == DATA_POLAR) {
    return polar (f1, f2);
  }
  return 0;
}

/* The function is called by the overall interpolator to perform
   linear interpolation. */
nr_complex_t spfile::interpolate_lin (vector * dep, vector * var,
				      nr_double_t x, int idx) {
  nr_double_t x1, x2, y1, y2, f1, f2;

  x1 = real (dep->get (idx));
  x2 = real (dep->get (idx + 1));

  // rectangular data
  if (dataType == DATA_RECTANGULAR) {
    y1 = real (var->get (idx));
    y2 = real (var->get (idx + 1));
    f1 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    y1 = imag (var->get (idx));
    y2 = imag (var->get (idx + 1));
    f2 = ((x2 - x) * y1 + (x - x1) * y2) / (x2 - x1);
    return rect (f1, f2);
  }
  // polar data
  else if (dataType == DATA_POLAR) {
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

void spfile::initDC (void) {
  // get appropriate property value
  char * dc = getPropertyString ("duringDC");

  // a short during DC including the reference node
  if (!strcmp (dc, "shortall")) {
    int v, n, lastnode = getSize () - 1;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // a short during DC excluding the reference node
  if (!strcmp (dc, "short")) {
    int v, n, lastnode = getSize () - 2;
    setVoltageSources (lastnode);
    allocMatrixMNA ();
    // place zero voltage sources
    for (v = VSRC_1, n = NODE_1; n < lastnode; n++, v++) {
      voltageSource (v, n, lastnode);
    }
    return;
  }
  // an open during DC
  else if (!strcmp (dc, "open")) {
    setVoltageSources (0);
    allocMatrixMNA ();
    return;
  }
  // none specified, DC value of IDFT ?
  else {
    setVoltageSources (0);
    allocMatrixMNA ();
  }
}

void spfile::initAC (void) {
  setVoltageSources (0);
  allocMatrixMNA ();
  initSP ();
}

void spfile::calcAC (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || sfreq == NULL) return;
  // calculate interpolated S-parameters
  calcSP (frequency);
  // convert S-parameters to Y-parameters
  setMatrixY (stoy (getMatrixS ()));
}

void spfile::calcNoiseAC (nr_double_t frequency) {
  // nothing to do if the given file type had errors
  if (spara == NULL || nfreq == NULL) return;
  setMatrixN (cstocy (calcMatrixCs (frequency), getMatrixY () * z0) / z0);
}

void spfile::initTR (void) {
  initDC ();
}

// properties
PROP_REQ [] = {
  { "File", PROP_STR, { PROP_NO_VAL, "spfile.snp" }, PROP_NO_RANGE },
  PROP_NO_PROP };
PROP_OPT [] = {
  { "Data", PROP_STR, { PROP_NO_VAL, "polar" },
    PROP_RNG_STR2 ("rectangular", "polar") },
  { "Interpolator", PROP_STR, { PROP_NO_VAL, "linear" },
    PROP_RNG_STR2 ("linear", "cubic") },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  { "duringDC", PROP_STR, { PROP_NO_VAL, "open" },
    PROP_RNG_STR4 ("open", "short", "shortall", "unspecified") },
  PROP_NO_PROP };
struct define_t spfile::cirdef =
  { "SPfile",
    PROP_NODES, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, PROP_DEF };
