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
 * $Id: complex.h,v 1.7 2008-12-19 19:46:21 ela Exp $
 *
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#include "real.h"

#ifndef HAVE_COMPLEX
#include "cmplx.h"
typedef cmplx nr_complex_t;
#elif defined HAVE_TR1_COMPLEX
#include <tr1/complex>
using namespace std;
using namespace std::tr1;
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

// create a complex object given rectangle coordinates
nr_complex_t rect (const nr_double_t x, const nr_double_t y = 0.0);

// overloaded math functions
#ifndef HAVE_CXX_COMPLEX_ACOS
nr_complex_t    acos (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_ACOSH
nr_complex_t   acosh (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_ASIN
nr_complex_t    asin (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_ASINH
nr_complex_t   asinh (const nr_complex_t);
#endif 

#ifndef HAVE_CXX_COMPLEX_ATAN
nr_complex_t    atan (const nr_complex_t);
#endif 

#ifndef HAVE_CXX_COMPLEX_ATANH
nr_complex_t   atanh (const nr_complex_t);
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

#ifndef HAVE_CXX_COMPLEX_FMOD
nr_complex_t    fmod (const nr_complex_t x, const nr_complex_t y);
nr_complex_t    fmod (const nr_complex_t x, const nr_double_t y);
nr_complex_t    fmod (const nr_double_t x, const nr_complex_t y);
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
nr_double_t     norm (const nr_complex_t);
#endif

#ifndef HAVE_CXX_COMPLEX_POLAR
nr_complex_t   polar (const nr_double_t mag, const nr_double_t ang = 0.0);
#endif

#ifndef HAVE_CXX_COMPLEX_POLAR_COMPLEX
nr_complex_t   polar (const nr_complex_t a, const nr_complex_t p);
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
nr_double_t       dB (const nr_complex_t);
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
nr_complex_t    sinc (const nr_complex_t);
nr_double_t   xhypot (const nr_complex_t, const nr_complex_t);
nr_double_t   xhypot (const nr_double_t, const nr_complex_t);
nr_double_t   xhypot (const nr_complex_t, const nr_double_t);
nr_complex_t   floor (const nr_complex_t);
nr_complex_t    ceil (const nr_complex_t);
nr_complex_t     fix (const nr_complex_t);
nr_complex_t   trunc (const nr_complex_t);
nr_complex_t   round (const nr_complex_t);
nr_complex_t     sqr (const nr_complex_t);
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
