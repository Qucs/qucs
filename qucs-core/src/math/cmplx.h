/*
 * cmplx.h - complex number class definitions
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
 * $Id: cmplx.h,v 1.2 2008-01-12 19:33:04 ela Exp $
 *
 */

#ifndef __CMPLX_H__
#define __CMPLX_H__

#ifdef log2
#undef log2
#endif

/*\brief Complex class
  Implement a generic complex class
*/
class cmplx
{
 private:
  nr_double_t r; //!< Real part
  nr_double_t i; //!< Imaginary part
		
 public:
  cmplx (nr_double_t real, nr_double_t imag = 0.0);
  cmplx ();
  cmplx (const cmplx&);
		
  // complex manipulations
  friend nr_double_t arg  (const cmplx);  // the angle in the plane
  friend nr_double_t real (const cmplx);  // the real part
  friend nr_double_t imag (const cmplx);  // the imaginary part
  friend cmplx       conj (const cmplx);  // the complex conjugate
  friend nr_double_t fabs (const cmplx);  // the magnitude

  nr_double_t arg  (void);  // the angle in the plane
  nr_double_t real (void);  // the real part
  nr_double_t imag (void);  // the imaginary part
  cmplx       conj (void);  // the complex conjugate
  nr_double_t fabs (void);  // the magnitude
  nr_double_t norm (void);  // the square of the magnitude

  // operator functions
  friend cmplx operator + (const cmplx, const cmplx);
  friend cmplx operator + (const cmplx, const nr_double_t);
  friend cmplx operator + (const nr_double_t, const cmplx);

  friend cmplx operator - (const cmplx, const cmplx);
  friend cmplx operator - (const cmplx, const nr_double_t);
  friend cmplx operator - (const nr_double_t, const cmplx);

  friend cmplx operator * (const cmplx, const cmplx);
  friend cmplx operator * (const cmplx, const nr_double_t);
  friend cmplx operator * (const nr_double_t, const cmplx);

  friend cmplx operator / (const cmplx, const cmplx);
  friend cmplx operator / (const cmplx, const nr_double_t);
  friend cmplx operator / (const nr_double_t, const cmplx);

  // assignment operations
  cmplx&      operator  = (const cmplx);
  cmplx&      operator  = (const nr_double_t);

  cmplx&      operator += (const cmplx);
  cmplx&      operator += (const nr_double_t);

  cmplx&      operator -= (const cmplx);
  cmplx&      operator -= (const nr_double_t);

  cmplx&      operator *= (const cmplx);
  cmplx&      operator *= (const nr_double_t);

  cmplx&      operator /= (const cmplx);
  cmplx&      operator /= (const nr_double_t);

  cmplx       operator +  ();
  cmplx       operator -  ();

  // debugging
  void print (void);
};

#endif /* __CMPLX_H__ */
