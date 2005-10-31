/*
 * precision.h - global precision header file
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: precision.h,v 1.1 2005-10-31 16:15:31 ela Exp $
 *
 */

#ifndef __PRECISION_H__
#define __PRECISION_H__

#if NR_DOUBLE_SIZE == 4  /* S 23bit MAN | S 7bit EXP */

#define NR_TYPE float
#define NR_EPSI 1.1920928955078125e-07 /* IEEE float  precision 2^{-23} */
#define NR_MIN  1.1754943508222875e-38
#define NR_MAX  3.4028234663852886e+38
#define NR_TINY 1e-6
#define NR_DECS "6"

#elif NR_DOUBLE_SIZE == 8  /* S 52bit MAN | S 10bit EXP */

#define NR_TYPE double
#define NR_EPSI 2.2204460492503131e-16 /* IEEE double precision 2^{-52} */
#define NR_MIN  2.2250738585072014e-308
#define NR_MAX  1.7976931348623157e+308
#define NR_TINY 1e-12
#define NR_DECS "11"

#elif NR_DOUBLE_SIZE == 12 /* S 63bit MAN | S 14bit EXP */

#define NR_TYPE long double
#define NR_EPSI 1.0842021724855044e-19 /* IEEE long double precision 2^{-63} */
#define NR_MIN  3.3621031431120935e-4932
#define NR_MAX  1.1897314953572318e+4932
#define NR_TINY 1e-15
#define NR_DECS "15"

#endif

#endif /* __PRECISION_H__ */
