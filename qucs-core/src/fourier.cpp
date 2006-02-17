/*
 * fourier.cpp - fourier transformation class implementation
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: fourier.cpp,v 1.3 2006-02-17 07:24:06 raimi Exp $
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
#include "fourier.h"

// Helper macro.
#define Swap(a,b) { nr_double_t t; t = a; a = b; b = t; }

using namespace fourier;

/* The function performs a 1-dimensional fast fourier transformation.
   Each data item is meant to be defined in equidistant steps.  The
   number of data items needs to be of binary size, e.g. 64, 128. */
void fourier::_fft_1d (nr_double_t * data, int len, int isign) {

  // bit reversal method
  int i, j, m, n;
  n = 2 * len;
  j = 0;
  for (i = 0; i < n; i += 2) {
    if (j > i) {                   // was index already swapped ?
      Swap (data[j], data[i]);     // swap real part
      Swap (data[j+1], data[i+1]); // swap imaginary part
    }
    m = n / 2;
    while (m >= 2 && j >= m) {     // calculate next swap index
      j -= m;
      m >>= 1;
    }
    j += m;
  }

  // Danielson-Lanzcos algorithm
  int mmax, istep;
  nr_double_t wtemp, theta, wr, wi, wpr, wpi, tempi, tempr;
  mmax = 2;
  while (n > mmax) {
    istep = mmax << 1;
    theta = isign * (2 * M_PI / mmax);
    wtemp = sin (0.5 * theta);
    wpr = -2.0 * wtemp * wtemp;
    wpi = sin (theta);
    wr = 1.0;
    wi = 0.0;
    for (m = 1; m < mmax; m += 2) {
      for (i = m; i <= n; i += istep) {
	j = i + mmax;
	tempr = wr * data[j-1] - wi * data[j];
	tempi = wr * data[j] + wi * data[j-1];
	data[j-1] = data[i-1] - tempr;
	data[j] = data[i] - tempi;
	data[i-1] += tempr;
	data[i] += tempi;
      }
      wr = (wtemp = wr) * wpr - wi * wpi + wr;
      wi = wi * wpr + wtemp * wpi + wi;
    }
    mmax = istep;
  }
}

/* This function performs a 1-dimensional fast fourier transformation
   on the given vector 'var'.  If 'sign' is -1 the inverse fft is
   computed, if +1 the fft itself is computed.  It returns a vector of
   binary size (as necessary for a fft). */
vector fourier::fft_1d (vector var, int isign) {
  int i, n, len = var.getSize ();

  // compute necessary binary data array size
  int size = 2;
  while (size < len) size <<= 1;

  // put data items (temporary array) in place
  nr_double_t * data;
  data = (nr_double_t *) calloc (2 * size * sizeof (nr_double_t), 1);
  for (n = i = 0; i < len; i++, n += 2) {
    data[n] = real (var (i)); data[n+1] = imag (var (i));
  }

  // run 1-dimensional fft
  _fft_1d (data, size, isign);

  // store transformed data items in result vector
  vector res = vector (size);
  for (n = i = 0; i < size; i++, n += 2) {
    res (i) = rect (data[n], data[n+1]);
    if (isign > 0) res (i) /= size;
  }

  // free temporary data array
  free (data);
  return res;
}

/* The function performs a 1-dimensional discrete fourier
   transformation.  Each data item is meant to be defined in
   equidistant steps. */
void fourier::_dft_1d (nr_double_t * data, int len, int isign) {
  int k, n, size = 2 * len * sizeof (nr_double_t);
  nr_double_t * res = (nr_double_t *) calloc (size, 1);
  nr_double_t theta, c, s;
  for (n = 0; n < 2 * len; n += 2) {
    theta = n * M_PI / 2 / len;
    for (k = 0; k < 2 * len; k += 2) {
      c = cos (k * theta);
      s = isign * sin (k * theta);
      res[n] += data[k] * c + data[k+1] * s;
      res[n+1] += data[k+1] * c - data[k] * s;
    }
  }
  memcpy (data, res, size);
  free (res);
}

/* The function performs a 1-dimensional discrete fourier
   transformation on the given vector 'var'.  If 'sign' is -1 the
   inverse dft is computed, if +1 the dft itself is computed. */
vector fourier::dft_1d (vector var, int isign) {
  int k, n, len = var.getSize ();
  vector res = vector (len);
  for (n = 0; n < len; n++) {
    nr_double_t theta = - isign * 2 * M_PI * n / len;
    complex val = 0;
    for (k = 0; k < len; k++)
      val += var (k) * polar (1.0, theta * k);
    res (n) = isign > 0 ? val / len : val;
  }
  return res;
}

// Helper functions.
vector fourier::ifft_1d (vector var) {
  return fft_1d (var, -1);
}
vector fourier::idft_1d (vector var) {
  return dft_1d (var, -1);
}
void fourier::_ifft_1d (nr_double_t * data, int len) {
  _fft_1d (data, len, -1);
}
void fourier::_idft_1d (nr_double_t * data, int len) {
  _dft_1d (data, len, -1);
}
