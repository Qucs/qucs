/*
 * cmplx.cpp - complex number class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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
 * $Id: cmplx.cpp,v 1.3 2008-01-13 10:50:23 ela Exp $
 *
 */

/*!\file cmplx.cpp
   
   Implement complex number class and functions
*/

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <math.h>

#include "real.h"
#include "cmplx.h"

/*!\brief Constructor from a real imaginary pair 

   Construct a complex from a real part and an imaginary part 
   \todo Why not inline
*/
cmplx::cmplx (nr_double_t real, nr_double_t imag) {
  r = real;
  i = imag;
}

/*!\brief Default constructor 

   Initialize the complex number to \f$0 + 0 i\f$
   \todo Why not inline
*/
cmplx::cmplx () {
  r = 0.0;
  i = 0.0;
}

/*!\brief Copy constructor 
   \todo Why not inline
*/
cmplx::cmplx (const cmplx& z) {
  r = z.r;
  i = z.i;
}

/*!\brief Compute complex modulus 

   \return modulus of complex object
   \todo Why not inline
*/
nr_double_t cmplx::abs (void) {
  return xhypot (r, i);
}

/*!\brief Compute complex modulus 

   \param[in] z Complex number
   \return Modulus of z
   \todo Why not inline
*/
nr_double_t abs (const cmplx z) {
  return xhypot (z.r, z.i);
}

/*!\brief Compute euclidian norm of complex number

   Compute \f$(\Re\mathrm{e}\;z )^2+ (\Im\mathrm{m}\;z)^2=|z|^2\f$
   \return Euclidian norm of z
   \todo Why not inline
*/
nr_double_t cmplx::norm (void) {
  return r * r + i * i;
}

/*!\brief Compute argument of complex number

   \return Argument of z
   \todo Why not inline
*/
nr_double_t cmplx::arg (void) {
  return atan2 (i, r);
}

/*!\brief Compute argument of complex number

   \param[in] z Complex number
   \return Argument of z
   \todo lack of nr_double_t cmplx::arg(void)
   \todo Why not inline
*/
nr_double_t arg (const cmplx z) {
  return atan2 (z.i, z.r);
}

/*!\brief Real part of complex number

   \return Real part
   \todo Why not inline?
*/
nr_double_t cmplx::real (void) {
  return r;
}

/*!\brief Real part of complex number

   \param[in] z Complex number
   \return Real part of z
   \todo Why not inline?
*/
nr_double_t real (const cmplx z) {
  return z.r;
}

/*!\brief Imaginary part of complex number

   \return Imaginary part of complex object
   \todo Why not inline?
*/
nr_double_t cmplx::imag (void) {
  return i;
}

/*!\brief Imaginary part of complex number

   \param[in] z Complex number
   \return Imaginary part of z
   \todo Why not inline?
*/
nr_double_t imag (const cmplx z) {
  return z.i;
}

/*!\brief Conjugate of complex number

   \return Conjugate of complex object
   \todo Why not inline?
   \todo Why not operator ~
*/
cmplx cmplx::conj (void) {
  return cmplx (r, -i);
}

/*!\brief Conjugate of complex number

   \param[in] z Complex number
   \return Conjugate of complex z
   \todo Why not inline?
*/
cmplx conj (const cmplx z) {
  return cmplx (z.r, -z.i);
}

/*!\brief Unary +
  \todo Why not inline 
*/
cmplx cmplx::operator+() {
  return cmplx (r, i);
}

/*!\brief Unary -
  \todo Why not inline 
*/
cmplx cmplx::operator-() {
  return cmplx (-r, -i);
}

/*!\brief += operator for complex
  \todo Why not inline 
*/
cmplx& cmplx::operator+=(const cmplx z2) {
  r += z2.r;
  i += z2.i;
  return *this;
}

/*!\brief += operator for real
  \todo Why not inline 
*/
cmplx& cmplx::operator+=(const nr_double_t r2) {
  r += r2;
  return *this;
}

/*!\brief -= operator for complex
  \todo Why not inline 
*/
cmplx& cmplx::operator-=(const cmplx z2) {
  r -= z2.r;
  i -= z2.i;
  return *this;
}

/*!\brief -= operator for real
  \todo Why not inline 
*/
cmplx& cmplx::operator-=(const nr_double_t r2) {
  r -= r2;
  return *this;
}

/*!\brief *= operator for complex
  \todo Why not inline 
  \todo Add for real
*/
cmplx& cmplx::operator*=(const nr_double_t r2) {
  r *= r2;
  i *= r2;
  return *this;
}

/*!\brief /= operator for complex
  \todo Why not inline 
  \todo Add for real
*/
cmplx& cmplx::operator/=(const nr_double_t r2) {
  r /= r2;
  i /= r2;
  return *this;
}

/*!\brief Addition of two complex
  \todo Why not inline
*/
cmplx operator+(const cmplx z1, const cmplx z2) {
  return cmplx (z1.r + z2.r, z1.i + z2.i);
}

/*!\brief Addition of real and complex
  \todo Why not inline
*/
cmplx operator+(const nr_double_t r1, const cmplx z2) {
  return cmplx (r1 + z2.r, z2.i);
}

/*!\brief Addition of complex and real
  \todo Why not inline
*/
cmplx operator+(const cmplx z1, const nr_double_t r2) {
  return cmplx (z1.r + r2, z1.i);
}

/*!\brief Substraction of two complex
  \todo Why not inline
*/
cmplx operator-(const cmplx z1, const cmplx z2) {
  return cmplx (z1.r - z2.r, z1.i - z2.i);
}

/*!\brief Substraction of real and complex
  \todo Why not inline
*/
cmplx operator-(const nr_double_t r1, const cmplx z2) {
  return cmplx (r1 - z2.r, -z2.i);
}

/*!\brief Substraction of complex and real
  \todo Why not inline
*/
cmplx operator-(const cmplx z1, const nr_double_t r2) {
  return cmplx (z1.r - r2, z1.i);
}

/*!\brief Multiplication of complex and real
  \todo Why not inline
*/
cmplx operator*(const cmplx z1, const nr_double_t r2) {
  return cmplx (z1.r * r2, z1.i * r2);
}

/*!\brief Multiplication of real and complex
  \todo Why not inline
*/
cmplx operator*(const nr_double_t r1, const cmplx z2) {
  return cmplx (z2.r * r1, z2.i * r1);
}

/*!\brief Multiplication of two complex
  \todo Why not inline
*/
cmplx operator*(const cmplx z1, const cmplx z2) {
  return cmplx (z1.r * z2.r - z1.i * z2.i, z1.i * z2.r + z1.r * z2.i);
}

/*!\brief Divide a complex by a double
   \todo why not inline
   \todo Put near *
*/
cmplx operator/(const cmplx z1, const nr_double_t r2) {
  return cmplx (z1.r / r2, z1.i / r2);
}

/*!\brief Divide a complex by a complex
   \todo Put near *
*/
cmplx operator/(const cmplx z1, const cmplx z2) {
#if 0
  nr_double_t n = norm (z2);
  return cmplx ((z1.r * z2.r + z1.i * z2.i) / n,
		(z1.i * z2.r - z1.r * z2.i) / n);
#else /* avoid numerical overflow and underrun */
  nr_double_t r, i, n, d;
  if (fabs (z2.r) > fabs (z2.i)) {
    n = z2.i / z2.r;
    d = z2.r + z2.i * n;
    r = (z1.r + z1.i * n) / d;
    i = (z1.i - z1.r * n) / d;
  }
  else {
    n = z2.r / z2.i;
    d = z2.r * n + z2.i;
    r = (z1.r * n + z1.i) / d;
    i = (z1.i * n - z1.r) / d;
  }
  return cmplx (r, i);
#endif
}

/*!\brief Divide and assign
   \todo Put near *=
*/
cmplx& cmplx::operator/=(const cmplx z) {
#if 0
  nr_double_t n1, n2;
  n1 = norm (z);
  n2 = (r * z.r + i * z.i) / n1;
  i  = (i * z.r - r * z.i) / n1;
  r  = n2;
#else /* avoid numerical overflow and underrun */
  nr_double_t n, d, t;
  if (fabs (z.r) > fabs (z.i)) {
    n = z.i / z.r;
    d = z.r + z.i * n;
    t = (r + i * n) / d;
    i = (i - r * n) / d;
    r = t;
  }
  else {
    n = z.r / z.i;
    d = z.r * n + z.i;
    t = (r * n + i) / d;
    i = (i * n - r) / d;
    r = t;
  }
#endif
  return *this;
}

/*!\brief Divide a real by a complex
   \todo Put near *
*/
cmplx operator/(const nr_double_t r1, const cmplx z2) {
#if 0
  nr_double_t n = norm (z2);
  return cmplx (r1 * z2.r / n, -r1 * z2.i / n);
#else /* avoid numerical overflow and underrun */
  nr_double_t r, i, n, d;
  if (fabs (z2.r) > fabs (z2.i)) {
    n = z2.i / z2.r;
    d = z2.r + z2.i * n;
    r = r1 / d;
    i = -n * r1 / d;
  }
  else {
    n = z2.r / z2.i;
    d = z2.r * n + z2.i;
    r = r1 * n / d;
    i = -r1 / d;
  }
  return cmplx (r, i);
#endif
}

/*!\brief Assign
   \todo Why not inline 
*/
cmplx& cmplx::operator=(const cmplx z) {
  r = z.r;
  i = z.i;
  return *this;
}

/*!\brief Assign with real 
   \todo Why not inline 
*/
cmplx& cmplx::operator=(const nr_double_t x) {
  r = x;
  i = 0.0;
  return *this;
}

/*!\brief Multiply and assign
   \todo Put near other *=
*/
cmplx& cmplx::operator*=(const cmplx z) {
  nr_double_t n;
  n = r * z.r - i * z.i;
  i = i * z.r + r * z.i;
  r = n;
  return *this;
}

#ifdef DEBUG
#include <stdio.h>
/*!\brief Print a complex number
   \todo Transform in inline
   \todo Add a debug method that is void if non debug
*/
void cmplx::print (void) {
  fprintf (stderr, "%+.2e,%+.2e ", (double) r, (double) i);
}
#endif /* DEBUG */
