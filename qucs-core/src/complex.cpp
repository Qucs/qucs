/*
 * complex.cpp - complex number class implementation
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
 * $Id: complex.cpp,v 1.7 2004-04-26 18:36:37 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <math.h>

#include "complex.h"

complex::complex (nr_double_t real, nr_double_t imag) {
  r = real;
  i = imag;
}

complex::complex () {
  r = 0.0;
  i = 0.0;
}

complex::complex (const complex& z) {
  r = z.r;
  i = z.i;
}

nr_double_t abs (const complex z) {
  return sqrt (z.r * z.r + z.i * z.i);
}

nr_double_t norm (const complex z) {
  return z.r * z.r + z.i * z.i;
}

nr_double_t arg (const complex z) {
  return atan2 (z.i, z.r);
}

nr_double_t real (const complex z) {
  return z.r;
}

nr_double_t imag (const complex z) {
  return z.i;
}

complex conj (const complex z) {
  return complex (z.r, -z.i);
}

nr_double_t dB (const complex z) {
  return 10.0 * log10 (z.r * z.r + z.i * z.i);
}

// returns the first result of square root z
complex sqrt (const complex z) {
  nr_double_t phi = arg (z);
  if (phi < 0.0) phi += 2.0 * M_PI;
  return polar (sqrt (abs (z)), phi / 2.0);
}

complex exp (const complex z) {
  nr_double_t mag = exp (real (z));
  return complex (mag * cos (imag (z)), mag * sin (imag (z)));
}

// returns the first result of natural logarithm z
complex ln (const complex z) {
  nr_double_t phi = arg (z);
  if (phi < 0.0) phi += 2.0 * M_PI;
  return complex (log (abs (z)), phi);
}

// returns the first result of decimal logarithm z
complex log10 (const complex z) {
  nr_double_t phi = arg (z);
  if (phi < 0.0) phi += 2.0 * M_PI;
  return complex (log10 (abs (z)), phi * log10 (exp (1)));
}

complex pow (const complex z, nr_double_t d) {
  return polar (pow (abs (z), d), arg (z) * d);
}

complex pow (nr_double_t d, const complex z) {
  return exp (z * log (d));
}

complex pow (const complex z1, const complex z2) {
  return exp (z2 * ln (z1));
}

complex sin (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (-i), r) + polar (exp (i), -r)) / 2.0;
}

complex cos (const complex z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return (polar (exp (-i), r) - polar (exp (i), -r)) / 2.0;
}

complex tan (const complex z) {
  nr_double_t r = 2.0 * real (z);
  nr_double_t i = 2.0 * imag (z);
  return 1.0 + 2.0 / (polar (exp (-i), r) + 1.0);
}

// converts impedance to reflexion coefficient
complex ztor (const complex z, nr_double_t zref) {
  return (z - zref) / (z + zref);
}

// converts reflexion coefficient to impedance
complex rtoz (const complex r, nr_double_t zref) {
  return zref * (1 + r) / (1 - r);
}

complex floor (const complex z) {
  return complex (floor (real (z)), floor (imag (z)));
}

complex polar (const nr_double_t mag, const nr_double_t ang) {
  return complex (mag * cos (ang), mag * sin (ang));
}

complex rect (const nr_double_t x, const nr_double_t y) {
  return complex (x, y);
}

complex complex::operator+() {
  return complex (r, i);
}

complex complex::operator-() {
  return complex (-r, -i);
}

complex& complex::operator+=(const complex z2) {
  r += z2.r;
  i += z2.i;
  return *this;
}

complex& complex::operator+=(const nr_double_t r2) {
  r += r2;
  return *this;
}

complex& complex::operator-=(const complex z2) {
  r -= z2.r;
  i -= z2.i;
  return *this;
}

complex& complex::operator-=(const nr_double_t r2) {
  r -= r2;
  return *this;
}

complex& complex::operator*=(const nr_double_t r2) {
  r *= r2;
  i *= r2;
  return *this;
}

complex& complex::operator/=(const nr_double_t r2) {
  r /= r2;
  i /= r2;
  return *this;
}

complex operator+(const complex z1, const complex z2) {
  return complex (z1.r + z2.r, z1.i + z2.i);
}

complex operator+(const nr_double_t r1, const complex z2) {
  return complex (r1 + z2.r, z2.i);
}

complex operator+(const complex z1, const nr_double_t r2) {
  return complex (z1.r + r2, z1.i);
}

complex operator-(const complex z1, const complex z2) {
  return complex (z1.r - z2.r, z1.i - z2.i);
}

complex operator-(const nr_double_t r1, const complex z2) {
  return complex (r1 - z2.r, -z2.i);
}

complex operator-(const complex z1, const nr_double_t r2) {
  return complex (z1.r - r2, z1.i);
}

complex operator*(const complex z1, const nr_double_t r2) {
  return complex (z1.r * r2, z1.i * r2);
}

complex operator*(const nr_double_t r1, const complex z2) {
  return complex (z2.r * r1, z2.i * r1);
}

complex operator*(const complex z1, const complex z2) { 
  return complex (z1.r * z2.r - z1.i * z2.i, z1.i * z2.r + z1.r * z2.i);
}

int operator==(const complex z1, const complex z2) {
  return (z1.r == z2.r) && (z1.i == z2.i);
}

int operator!=(const complex z1, const complex z2) {
  return (z1.r != z2.r) || (z1.i != z2.i);
}

complex operator/(const complex z1, const nr_double_t r2) {
  return complex (z1.r / r2, z1.i / r2);
}

complex operator/(const complex z1, const complex z2) {
  nr_double_t n = norm (z2);
  return complex ((z1.r * z2.r + z1.i * z2.i) / n,
		  (z1.i * z2.r - z1.r * z2.i) / n);
}

complex& complex::operator/=(const complex z) {
  nr_double_t n1, n2;
  n1 = norm (z);                 
  n2 = (r * z.r + i * z.i) / n1;
  i  = (i * z.r - r * z.i) / n1;
  r  = n2;
  return *this;
}

complex operator/(const nr_double_t r1, const complex z2) {
  nr_double_t n = norm (z2);
  return complex (r1 * z2.r / n, -r1 * z2.i / n);
}

complex operator%(const complex z1, const complex z2) {
  return z1 - z2 * floor (z1 / z2);
}

complex operator%(const complex z1, const nr_double_t r2) {
  return z1 - r2 * floor (z1 / r2);
}

complex operator%(const nr_double_t r1, const complex z2) {
  return r1 - z2 * floor (r1 / z2);
}

complex& complex::operator%=(const complex z) { 
  *this = *this % z;
  return *this;
}

complex& complex::operator%=(const nr_double_t r) { 
  *this = *this % r;
  return *this;
}

complex& complex::operator=(const complex z) {
  r = z.r;
  i = z.i;
  return *this;
}

complex& complex::operator=(const nr_double_t x) {
  r = x;
  i = 0.0;
  return *this;
}

complex& complex::operator*=(const complex z) { 
  nr_double_t n;
  n = r * z.r - i * z.i;
  i = i * z.r + r * z.i;
  r = n;
  return *this;
}
