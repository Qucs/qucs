/*
 * complex.h - complex number class definitions
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: complex.h,v 1.1 2003/12/20 19:03:25 ela Exp $
 *
 */

#ifndef __COMPLEX_H__
#define __COMPLEX_H__

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
  friend nr_double_t  imag (const complex);  // the imaginary part
  friend complex      conj (const complex);  // the complex conjugate
  friend nr_double_t  norm (const complex);  // the square of the magnitude
  friend nr_double_t  arg  (const complex);  // the angle in the plane

  // overloaded math functions
  friend nr_double_t  abs  (const complex);

  // operator functions
  friend complex operator + (const complex, const complex);
  friend complex operator + (const complex, const nr_double_t);
  friend complex operator + (const nr_double_t,  const complex);

  friend complex operator - (const complex, const complex);
  friend complex operator - (const complex, const nr_double_t);
  friend complex operator - (const nr_double_t,  const complex);

  friend complex operator * (const complex, const complex);
  friend complex operator * (const complex, const nr_double_t);
  friend complex operator * (const nr_double_t,  const complex);

  friend complex operator / (const complex, const complex);
  friend complex operator / (const complex, const nr_double_t);
  friend complex operator / (const nr_double_t,  const complex);

  // comparisons
  friend int     operator == (const complex, const complex);
  friend int     operator != (const complex, const complex);
		
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

  complex        operator +  ();
  complex        operator -  ();
};

#endif /* __COMPLEX_H__ */
