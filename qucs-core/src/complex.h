/*
 * complex.h - complex number class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: complex.h,v 1.23 2006-03-14 07:29:13 raimi Exp $
 *
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#ifdef log2
#undef log2
#endif

class complex;

// create a complex object given rectangle coordinates
complex rect (const nr_double_t x, const nr_double_t y = 0.0);
  
// create a complex object given polar coordinates
complex polar (const nr_double_t mag, const nr_double_t ang = 0.0);
			
// complex manipulations
nr_double_t  real (const nr_double_t);
nr_double_t  imag (const nr_double_t);
nr_double_t  norm (const nr_double_t);
nr_double_t  conj (const nr_double_t);
nr_double_t   fix (const nr_double_t);
nr_double_t  step (const nr_double_t);

// overloaded math functions
nr_double_t xhypot (const nr_double_t, const nr_double_t);
nr_double_t    abs (const nr_double_t);
nr_double_t   sign (const nr_double_t);
nr_double_t   sinc (const nr_double_t);

class complex
{
 private:
  nr_double_t r, i;
		
 public:
  complex (nr_double_t real, nr_double_t imag = 0.0);
  complex ();
  complex (const complex&);
		
  // complex manipulations
  friend nr_double_t arg  (const complex);  // the angle in the plane
  friend nr_double_t real (const complex);  // the real part
  friend nr_double_t imag (const complex);  // the imaginary part
  friend nr_double_t norm (const complex);  // the square of the magnitude
  friend complex     conj (const complex);  // the complex conjugate

  nr_double_t norm (void);  // the square of the magnitude
  nr_double_t real (void);  // the real part
  nr_double_t imag (void);  // the imaginary part
  complex     conj (void);  // the complex conjugate
  nr_double_t abs  (void);  // the magnitude

  // overloaded math functions
  friend nr_double_t     dB (const complex);
  friend complex       sqrt (const complex);
  friend complex        exp (const complex);
  friend complex         ln (const complex);
  friend complex      log10 (const complex);
  friend complex       log2 (const complex);
  friend complex      floor (const complex);
  friend complex        pow (const complex, const nr_double_t);
  friend complex        pow (const nr_double_t, const complex);
  friend complex        pow (const complex, const complex);
  friend complex        sin (const complex);
  friend complex     arcsin (const complex);
  friend complex        cos (const complex);
  friend complex     arccos (const complex);
  friend complex        tan (const complex);
  friend complex     arctan (const complex);
  friend complex        cot (const complex);
  friend complex     arccot (const complex);
  friend complex       sinh (const complex);
  friend complex     arsinh (const complex);
  friend complex       cosh (const complex);
  friend complex     arcosh (const complex);
  friend complex       tanh (const complex);
  friend complex     artanh (const complex);
  friend complex       coth (const complex);
  friend complex     arcoth (const complex);
  friend complex       ztor (const complex, complex zref = 50.0);
  friend complex       rtoz (const complex, complex zref = 50.0);
  friend complex       ytor (const complex, complex zref = 50.0);
  friend complex       rtoy (const complex, complex zref = 50.0);
  friend nr_double_t    abs (const complex);
  friend complex       sign (const complex);
  friend complex       sinc (const complex);
  friend nr_double_t xhypot (const complex, const complex);
  friend complex       ceil (const complex);
  friend complex        fix (const complex);
  friend complex      round (const complex);
  friend complex        sqr (const complex);
  friend complex       step (const complex);
  friend complex         jn (const int, const complex);
  friend complex         yn (const int, const complex);
  friend complex        erf (const complex);
  friend complex       erfc (const complex);
  friend complex     erfinv (const complex);
  friend complex         i0 (const complex);
  friend complex      polar (const complex, const complex);
  friend complex    arctan2 (const complex, const complex);

  // operator functions
  friend complex operator + (const complex, const complex);
  friend complex operator + (const complex, const nr_double_t);
  friend complex operator + (const nr_double_t, const complex);

  friend complex operator - (const complex, const complex);
  friend complex operator - (const complex, const nr_double_t);
  friend complex operator - (const nr_double_t, const complex);

  friend complex operator * (const complex, const complex);
  friend complex operator * (const complex, const nr_double_t);
  friend complex operator * (const nr_double_t, const complex);

  friend complex operator / (const complex, const complex);
  friend complex operator / (const complex, const nr_double_t);
  friend complex operator / (const nr_double_t, const complex);

  friend complex operator % (const complex, const complex);
  friend complex operator % (const complex, const nr_double_t);
  friend complex operator % (const nr_double_t, const complex);

  // comparisons
  friend bool    operator == (const complex, const complex);
  friend bool    operator != (const complex, const complex);
  friend bool    operator >= (const complex, const complex);
  friend bool    operator <= (const complex, const complex);
  friend bool    operator >  (const complex, const complex);
  friend bool    operator <  (const complex, const complex);
		
  // assignment operations
  complex&       operator  = (const complex);
  complex&       operator  = (const nr_double_t);

  complex&       operator += (const complex);
  complex&       operator += (const nr_double_t);

  complex&       operator -= (const complex);
  complex&       operator -= (const nr_double_t);

  complex&       operator *= (const complex);
  complex&       operator *= (const nr_double_t);

  complex&       operator /= (const complex);
  complex&       operator /= (const nr_double_t);

  complex&       operator %= (const complex);
  complex&       operator %= (const nr_double_t);

  complex        operator +  ();
  complex        operator -  ();

  // debugging
  void print (void);
};

#endif /* __COMPLEX_H__ */
