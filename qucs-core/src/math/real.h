/*
 * real.h - some real valued function definitions
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
 * $Id: real.h,v 1.4 2008-01-15 19:14:03 ela Exp $
 *
 */

#ifndef __REAL_H__
#define __REAL_H__

// complex manipulations
nr_double_t   real (const nr_double_t);
nr_double_t   imag (const nr_double_t);
nr_double_t   norm (const nr_double_t);
nr_double_t   conj (const nr_double_t);
#ifndef HAVE_COMPLEX
nr_double_t    abs (const nr_double_t);
#endif

// extra math functions
nr_double_t limexp (const nr_double_t);
nr_double_t signum (const nr_double_t);
nr_double_t   sign (const nr_double_t);
nr_double_t xhypot (const nr_double_t, const nr_double_t);
nr_double_t   sinc (const nr_double_t);
nr_double_t    fix (const nr_double_t);
nr_double_t   step (const nr_double_t);

#ifndef HAVE_ROUND
nr_double_t  round (const nr_double_t);
#endif
#ifndef HAVE_TRUNC
nr_double_t  trunc (const nr_double_t);
#endif

// more extra math functions
unsigned int factorial (unsigned int);

#endif /* __REAL_H__ */
