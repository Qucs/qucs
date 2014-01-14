/*
 * complex.h - complex number class definitions
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "real.h"

#if defined HAVE_TR1_COMPLEX
#include <tr1/complex>
using namespace std;
//using namespace std::tr1;
typedef std::complex<nr_double_t> nr_complex_t;
#else
#include <complex>
using namespace std;
typedef std::complex<nr_double_t> nr_complex_t;
#endif

// undefine this macro if it is defined already
#ifdef log2
#undef log2
#endif

// overloaded math functions
#ifndef HAVE_CXX_COMPLEX_ACOS
nr_complex_t    acos (const nr_complex_t);
#else
using std::tr1::acos;
#endif

#ifndef HAVE_CXX_COMPLEX_ACOSH
nr_complex_t   acosh (const nr_complex_t);
#else
using std::tr1::acosh;
#endif

#ifndef HAVE_CXX_COMPLEX_ASIN
nr_complex_t    asin (const nr_complex_t);
#else
using std::tr1::asin;
#endif

#ifndef HAVE_CXX_COMPLEX_ASINH
nr_complex_t   asinh (const nr_complex_t);
#else
using std::tr1::asinh;
#endif 

#ifndef HAVE_CXX_COMPLEX_ATAN
nr_complex_t    atan (const nr_complex_t);
#else
using std::tr1::atan;
#endif 

#ifndef HAVE_CXX_COMPLEX_ATANH
nr_complex_t   atanh (const nr_complex_t);
#else
using std::tr1::atanh;
#endif 

#ifndef HAVE_CXX_COMPLEX_ATAN2
nr_complex_t   atan2 (const nr_complex_t, const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_COS 
nr_complex_t     cos (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_COSH
nr_complex_t    cosh (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_EXP
nr_complex_t     exp (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_LOG 
nr_complex_t     log (const nr_complex_t);
#endif 

#ifndef HAVE_CXX_COMPLEX_LOG10 
nr_complex_t   log10 (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_LOG2
nr_complex_t    log2 (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_NORM
/*!\brief Compute euclidian norm of complex number

   Compute \f$(\Re\mathrm{e}\;z )^2+ (\Im\mathrm{m}\;z)^2=|z|^2\f$
   \param[in] z Complex number
   \return Euclidian norm of z
*/
inline nr_double_t norm (const nr_complex_t z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return r * r + i * i;
}
#endif

#ifndef HAVE_CXX_COMPLEX_POLAR
/*!\brief Construct a complex number using polar notation
   \param[in] mag Magnitude
   \param[in] ang Angle
   \return complex number in rectangular form
*/
inline nr_complex_t polar (const nr_double_t mag, const nr_double_t ang = 0.0) {
  return nr_complex_t (mag * cos (ang), mag * sin (ang));
}
#endif

#ifndef HAVE_CXX_COMPLEX_POLAR_COMPLEX
/*!\brief Construct a complex number using polar notation
   \param[in] mag Magnitude
   \param[in] ang Angle
   \return complex number in rectangular form
   \todo Why not inline
*/
nr_complex_t   polar (const nr_complex_t a, const nr_complex_t p = 0.0);
nr_complex_t   polar (const nr_double_t a, const nr_complex_t p);
nr_complex_t   polar (const nr_complex_t a, const nr_double_t p = 0.0);
#endif

#ifndef HAVE_CXX_COMPLEX_POW
nr_complex_t     pow (const nr_complex_t, const nr_double_t);
nr_complex_t     pow (const nr_double_t, const nr_complex_t);
nr_complex_t     pow (const nr_complex_t, const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_SIN 
nr_complex_t     sin (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_SINH
nr_complex_t    sinh (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_SQRT
nr_complex_t    sqrt (const nr_complex_t);
#endif 

#ifndef HAVE_CXX_COMPLEX_TAN
nr_complex_t     tan (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_TANH
nr_complex_t     tanh (const nr_complex_t);
#endif

// extra math functions
/*!\brief Magnitude in dB

   Compute \f$10\log_{10} |z|^2=20\log_{10} |z|\f$
   \param[in] z complex number
   \return Magnitude in dB
*/
inline nr_double_t dB (const nr_complex_t z) {
  return 10.0 * log10 (norm (z));
}

nr_complex_t  limexp (const nr_complex_t);
nr_complex_t     cot (const nr_complex_t);
nr_complex_t    acot (const nr_complex_t);
nr_complex_t   asech (const nr_complex_t);
nr_complex_t    coth (const nr_complex_t);
nr_complex_t   acoth (const nr_complex_t);
nr_complex_t    ztor (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    rtoz (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    ytor (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t    rtoy (const nr_complex_t, const nr_complex_t zref = 50.0);
nr_complex_t  signum (const nr_complex_t);
nr_complex_t    sign (const nr_complex_t);
/*!\brief Cardinal sinus 
   
   Compute \f$\mathrm{sinc}\;z=\frac{\sin z}{z}\f$
   \param[in] z complex number
   \return cardianal sinus of z
*/
inline nr_complex_t    sinc (const nr_complex_t z) {
  if (real(z) == 0 && imag(z)) return 1;
  return sin (z) / z;
}
nr_double_t   xhypot (const nr_complex_t, const nr_complex_t);
nr_double_t   xhypot (const nr_double_t, const nr_complex_t);
nr_double_t   xhypot (const nr_complex_t, const nr_double_t);

#ifndef HAVE_CXX_COMPLEX_FLOOR 
/*!\brief Complex floor 

    floor is the largest integral value not greater than argument
    Apply floor to real and imaginary part 
    \param[in] z complex number
    \return floored complex number
*/
inline nr_complex_t   floor (const nr_complex_t z) {
  return nr_complex_t (floor (real (z)), floor (imag (z)));
}
#endif

/*!\brief Complex ceil
    Ceil is the smallest integral value not less than argument
    Apply ceil to real and imaginary part 
    \param[in] z complex number
    \return ceilled complex number
*/
inline nr_complex_t ceil (const nr_complex_t z) {
  return nr_complex_t (ceil (real (z)), ceil (imag (z)));
}
/*!\brief Complex fix
    
    Apply fix to real and imaginary part 
    \param[in] z complex number
    \return fixed complex number
    \todo why not using real fix
*/
inline nr_complex_t fix (const nr_complex_t z) {
  nr_double_t x = real (z);
  nr_double_t y = imag (z);
  x = (x > 0) ? floor (x) : ceil (x);
  y = (y > 0) ? floor (y) : ceil (y);
  return nr_complex_t (x, y);
}
/*!\brief Complex round
    round is the nearest integral value 
    Apply round to real and imaginary part 
    \param[in] z complex number
    \return rounded complex number
*/
inline nr_complex_t round (const nr_complex_t z) {
  return nr_complex_t (round (real (z)), round (imag (z)));
}

/*!\brief Complex trunc
    
    Apply round to integer, towards zero to real and imaginary part 
    \param[in] z complex number
    \return rounded complex number
*/
inline nr_complex_t trunc (const nr_complex_t z) {
  return nr_complex_t (trunc (real (z)), trunc (imag (z)));
}

#ifndef HAVE_CXX_COMPLEX_FMOD
/*!\brief Complex fmod
    Apply fmod to the complex z
    \param[in] x complex number (dividant)
    \param[in] y complex number (divisor)
    \return return \f$x - n * y\f$ where n is the quotient of \f$x / y\f$, 
    rounded towards zero to an integer.
*/
inline nr_complex_t    fmod (const nr_complex_t x, const nr_complex_t y) {
  nr_complex_t n = floor (x / y);
  return x - n * y;
}

/*!\brief Complex fmod (double version)
    Apply fmod to the complex z
    \param[in] x complex number (dividant)
    \param[in] y double number (divisor)
    \return return \f$x - n * y\f$ where n is the quotient of \f$x / y\f$, 
    rounded towards zero to an integer.
*/
inline nr_complex_t    fmod (const nr_complex_t x, const nr_double_t y) {
  nr_complex_t n = floor (x / y);
  return x - n * y;
}

/*!\brief Complex fmod (double version)
    Apply fmod to the complex z
    \param[in] x double number (dividant)
    \param[in] y complex number (divisor)
    \return return \f$x - n * y\f$ where n is the quotient of \f$x / y\f$, 
    rounded towards zero to an integer.
*/
inline nr_complex_t  fmod (const nr_double_t x, const nr_complex_t y) {
  nr_complex_t n = floor (x / y);
  return x - n * y;
}
#endif

/*!\brief Square of complex number
    
    \param[in] z complex number
    \return squared complex number
*/
inline nr_complex_t sqr (const nr_complex_t z) {
  nr_double_t r = real (z);
  nr_double_t i = imag (z);
  return nr_complex_t (r * r - i * i, 2 * r * i);
}

nr_complex_t    step (const nr_complex_t);
nr_complex_t      jn (const int, const nr_complex_t);
nr_complex_t      yn (const int, const nr_complex_t);
nr_complex_t      i0 (const nr_complex_t);
nr_complex_t     erf (const nr_complex_t);
nr_complex_t    erfc (const nr_complex_t);
nr_complex_t  erfinv (const nr_complex_t);
nr_complex_t erfcinv (const nr_complex_t);

// modulo operators
nr_complex_t operator % (const nr_complex_t, const nr_complex_t);
nr_complex_t operator % (const nr_complex_t, const nr_double_t);
nr_complex_t operator % (const nr_double_t, const nr_complex_t);

// comparisons
bool operator == (const nr_complex_t, const nr_complex_t);
bool operator != (const nr_complex_t, const nr_complex_t);
bool operator >= (const nr_complex_t, const nr_complex_t);
bool operator <= (const nr_complex_t, const nr_complex_t);
bool operator >  (const nr_complex_t, const nr_complex_t);
bool operator <  (const nr_complex_t, const nr_complex_t);

#endif /* __COMPLEX_H__ */
