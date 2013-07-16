/*
 * precision.h - global precision header file
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
 * $Id$
 *
 */

#ifndef __PRECISION_H__
#define __PRECISION_H__

#ifndef __FLT_EPSILON__
#define __FLT_EPSILON__  1.1920928955078125e-07
#endif
#ifndef __FLT_MIN__
#define __FLT_MIN__      1.1754943508222875e-38
#endif
#ifndef __FLT_MAX__
#define __FLT_MAX__      3.4028234663852886e+38
#endif

#ifndef __DBL_EPSILON__
#define __DBL_EPSILON__  2.2204460492503131e-16
#endif
#ifndef __DBL_MIN__
#define __DBL_MIN__      2.2250738585072014e-308
#endif
#ifndef __DBL_MAX__
#define __DBL_MAX__	 1.7976931348623157e+308
#endif

#if NR_DOUBLE_SIZE == 12

#ifndef __LDBL_EPSILON__
#define __LDBL_EPSILON__ 1.0842021724855044e-19
#endif
#ifndef __LDBL_MIN__
#define __LDBL_MIN__     3.3621031431120935e-4932
#endif
#ifndef __LDBL_MAX__
#define __LDBL_MAX__     1.1897314953572318e+4932
#endif

#elif NR_DOUBLE_SIZE == 16

#ifndef __LDBL_EPSILON__
#define __LDBL_EPSILON__ 1.9259299443872359e-34
#endif
#ifndef __LDBL_MIN__
#define __LDBL_MIN__     3.3621031431120935e-4932
#endif
#ifndef __LDBL_MAX__
#define __LDBL_MAX__     1.1897314953572318e+4932
#endif

#endif


#if NR_DOUBLE_SIZE == 4  /* S 23bit MAN | S 7bit EXP */
  /* single precision */
#define NR_TYPE float
#define NR_EPSI __FLT_EPSILON__
#define NR_MIN  __FLT_MIN__
#define NR_MAX  __FLT_MAX__
#define NR_TINY 1e-6
#define NR_DECS "6" /* 6 digits */

#elif NR_DOUBLE_SIZE == 8  /* S 52bit MAN | S 10bit EXP */
  /* double precision */
#define NR_TYPE double
#define NR_EPSI __DBL_EPSILON__
#define NR_MIN  __DBL_MIN__
#define NR_MAX  __DBL_MAX__
#define NR_TINY 1e-12
#define NR_DECS "11" /* 15 digits */

#elif NR_DOUBLE_SIZE == 12 /* S 63bit MAN | S 14bit EXP */
  /* IEEE 80-bit floating point */
#define NR_TYPE long double
#define NR_EPSI __LDBL_EPSILON__
#define NR_MIN  __LDBL_MIN__
#define NR_MAX  __LDBL_MAX__
#define NR_TINY 1e-15
#define NR_DECS "15" /* 18 digits */

#elif NR_DOUBLE_SIZE == 16 /* S 112bit MAN | S 14bit EXP */
  /* IEEE 128-bit floating point */
#define NR_TYPE long double
#define NR_EPSI __LDBL_EPSILON__
#define NR_MIN  __LDBL_MIN__
#define NR_MAX  __LDBL_MAX__
#define NR_TINY 1e-15
#define NR_DECS "15" /* 33 digits */

#endif



#endif /* __PRECISION_H__ */
