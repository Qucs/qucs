/*
 * complex.h - complex number class definitions
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
 * $Id: complex.h,v 1.14 2004-11-29 19:03:33 raimi Exp $
 *
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

#ifdef log2
#undef log2
#endif

class complex
{
 private:
  nr_double_t r, i;
		
 public:
  complex (nr_double_t real, nr_double_t imag = 0.0);
  complex ();
  complex (const complex&);
		
  // create a complex object given polar coordinates
  friend complex polar (const nr_double_t mag, const nr_double_t ang = 0.0);
			
  // create a complex object given rectangle coordinates
  friend complex rect (const nr_double_t x, const nr_double_t y = 0.0);
  
  // complex manipulations
  friend nr_double_t  real (const complex);  // the real part
  friend nr_double_t  real (nr_double_t);
  friend nr_double_t  imag (const complex);  // the imaginary part
  friend nr_double_t  imag (nr_double_t);
  friend complex      conj (const complex);  // the complex conjugate
  friend nr_double_t  norm (const complex);  // the square of the magnitude
  friend nr_double_t  norm (nr_double_t);
  friend nr_double_t  arg  (const complex);  // the angle in the plane

  // overloaded math functions
  friend nr_double_t   dB (const complex);
  friend complex     sqrt (const complex);
  friend complex      exp (const complex);
  friend complex       ln (const complex);
  friend complex    log10 (const complex);
  friend complex     log2 (const complex);
  friend complex    floor (const complex);
  friend complex      pow (const complex, nr_double_t);
  friend complex      pow (nr_double_t, const complex);
  friend complex      pow (const complex, const complex);
  friend nr_double_t  abs (const complex);
  friend nr_double_t  abs (nr_double_t);
  friend complex      sin (const complex);
  friend complex   arcsin (const complex);
  friend complex      cos (const complex);
  friend complex   arccos (const complex);
  friend complex      tan (const complex);
  friend complex   arctan (const complex);
  friend complex      cot (const complex);
  friend complex   arccot (const complex);
  friend complex     sinh (const complex);
  friend complex   arsinh (const complex);
  friend complex     cosh (const complex);
  friend complex   arcosh (const complex);
  friend complex     tanh (const complex);
  friend complex   artanh (const complex);
  friend complex     coth (const complex);
  friend complex   arcoth (const complex);
  friend complex     ztor (const complex, nr_double_t zref = 50.0);
  friend complex     rtoz (const complex, nr_double_t zref = 50.0);
  friend complex     sign (const complex);
  friend nr_double_t sign (const nr_double_t);
  friend complex     sinc (const complex);
  friend nr_double_t sinc (const nr_double_t);

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
  friend int     operator == (const complex, const complex);
  friend int     operator != (const complex, const complex);
  friend int     operator >= (const complex, const complex);
  friend int     operator <= (const complex, const complex);
		
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
