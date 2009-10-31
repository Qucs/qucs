/*
 * receiver.cpp - receiver transformation class implementation
 *
 * Copyright (C) 2009 Dirk Schaefer <schad@5pm.de>
 * Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: receiver.cpp,v 1.1 2009-10-31 17:18:27 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "consts.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "spline.h"
#include "interpolator.h"
#include "fourier.h"
#include "receiver.h"

/* The function returns a power-of-two value which is equal or larger
   than the given value.  The maximum returned value is 2^30. */
nr_int32_t emi::nearestbin32 (int x) {
  nr_int32_t boundary = 1 << 30;
  nr_int32_t current = 1;
  if (x >= boundary) return boundary;
  while (current < x) current <<= 1;
  return current;
}

/* The function computes a EMI receiver spectrum based on the given
   waveform in the time domain.  The number of points in the waveform
   is required to be a power of two. */
vector * emi::receiver (nr_double_t * ida, nr_double_t * idt, int ilength) {

  int i, n, points, istart, istop;
  nr_double_t tstart, tstop, fres;
  vector * ed = new vector ();
  
  points = ilength;
  istart = 0;
  istop = ilength - 1;
  
  /* ilength must be a power of 2 - write wrapper later on */
  fourier::_fft_1d (ida, ilength, 1); /* 1 = forward fft */

  /* start at first AC point (0 as DC point remains untouched)
     additionally only half of the FFT result required */
  for (i = 2; i < points; i++) {
    ida[i] /= points / 2;
  }

  /* calculate frequency step */
  tstart = idt[istart];
  tstop = idt[istop];
  fres = 1.0 / (tstop - tstart);

  /* generate data vector; inplace calculation of magnitudes */
  nr_double_t * d = ida;
  for (n = 0, i = 0; i < points / 2; i++, n += 2){
    /* abs value of complex number */
    d[i] = xhypot (ida[n], ida[n + 1]);
    /* vector contains complex values; thus every second value */
  }

  points /= 2;

  /* define EMI settings */
  struct settings settings[] = {
    {   200, 150e3,   200,   200 },
    { 150e3,  30e6,   9e3,   9e3 },
    {  30e6,   1e9, 120e3, 120e3 },
    {     0,     0,     0,      0}
  };
  
  /* define EMI noise floor */
  nr_double_t noise = pow (10.0, (-100.0 / 40.0)) * 1e-6; 
  
  /* generate resulting data & frequency vector */
  nr_double_t fcur, dcur;
  int ei = 0;

  /* go through each EMI setting */
  for (i = 0; settings[i].bandwidth != 0; i++ ) {

    nr_double_t bw = settings[i].bandwidth;
    nr_double_t fstart = settings[i].start;
    nr_double_t fstop = settings[i].stop;
    nr_double_t fstep = settings[i].stepsize;

    fcur = fstart;
    while (fcur <= fstop) {
      /* calculate indices covering current bandwidth */
      int il = floor ((fcur - bw / 2) / fres);
      int ir = ceil  ((fcur + bw / 2) / fres);

      /* right index (ri) greater 0 and left index less than points ->
	 at least part of data is within bandwidth indices */
      if (ir >= 0 && il < points - 1) {
	/* adjust indices to reside in the data array */
	if (il < 0) il = 0;
	if (ir > points - 1) ir = points - 1;
	 
	/* bandpass calculation */
	nr_double_t q = fcur / bw;
	nr_double_t a = 1.0;

	/* sum-up the values within the bandwidth */
	dcur = 0;
	for (int j = 0; j < ir - il; j++){
	  nr_complex_t p = rect (0, fres * (il + j) / fcur);
	  nr_complex_t w = a / q * p / (1.0 + p / q + p * p);
	  dcur += norm (w) * d[il + j]; 
	}

	/* add noise to the result */
	dcur += noise * sqrt (bw);
      
	/* save result */
	ed->add (rect (dcur, fcur));
	ei++;
      }

      /* goto next frequency */
      fcur += fstep;
    }
  }

  /* returning values of function */
  return ed;
}

/* This is a wrapper for the basic EMI rceiver functionality.  It
   takes an arbitraty waveform in the time domain and interpolates it
   such, that its length results in a power of two elements. */
vector * emi::receiver (vector * da, vector * dt, int len) {

  int i, nlen, olen =  da->getSize ();

  // don't allow less points than the actual length
  if (len < da->getSize ()) len = da->getSize ();

  // find a power-of-two length
  nlen = emi::nearestbin32 (len);

  nr_double_t * ida = new nr_double_t[2 * nlen];
  nr_double_t * idt = new nr_double_t[nlen];

  // no interpolation necessary
  if (olen == nlen) {
    // just copy values
    for (i = 0; i < olen; i++) {
      ida[2 * i + 0] = real ((*da)(i));
      ida[2 * i + 1] = 0;
      idt[i] = real ((*dt)(i));
    }
  }
  // interpolation necessary
  else {
    // copy values for interpolator object
    for (i = 0; i < olen; i++) {
      ida[i] = real ((*da)(i));
      idt[i] = real ((*dt)(i));
    }
    // create interpolator (use cubic splines)
    interpolator * inter = new interpolator ();
    inter->vectors (ida, idt, olen);
    inter->prepare (INTERPOL_CUBIC, REPEAT_NO, DATA_RECTANGULAR);
    // adjust the time domain vector using interpolation
    nr_double_t tstep = real (dt->get (olen - 1) - dt->get (0)) / nlen;
    for (i = 0; i < nlen; i++) {
      nr_double_t t = i * tstep;
      ida[2 * i + 0] = inter->rinterpolate (t);
      ida[2 * i + 1] = 0;
      idt[i] = t;
    }
    // destroy interpolator
    delete inter;
  }

  // run actual EMI receiver computations
  vector * res = receiver (ida, idt, nlen);

  // delete intermediate data
  delete[] ida;
  delete[] idt;

  return res;
}
