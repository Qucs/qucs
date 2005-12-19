/*
 * constants.h - global natural constant header file
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: constants.h,v 1.4 2005/12/19 07:55:13 raimi Exp $
 *
 */

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "consts.h"
#include "precision.h"

#define C0   299792458.0              /* speed of light in vacuum  */
#define kB   1.380650524e-23          /* Boltzmann constant        */
#define Q    1.6021765314e-19         /* elementary charge         */
#define MU0  12.566370614e-7          /* magnetic constant         */
#define E0   8.854187817e-12          /* electric constant         */
#define H    6.626069311e-34          /* Planck constant           */
#define K    -273.15                  /* absolute 0 in centigrade  */
#define Z0   376.73031346958504364963 /* wave resistance in vacuum */
#define T0   290                      /* standard temperature      */

#define kBoverQ    0.86173433260414314916e-4
#define QoverkB    1.16045045690360379713e+4

#define ESiO2      3.9     /* relative permittivity */
#define ESi        11.7
#define EGe        15.8
#define EGaAs      13.1
#define NiSi       1.45e16 /* intrinsic carrier concentration in 1/m^3 */
#define NiGe       2.40e19
#define NiGaAa     9.00e12
#define EgSi       1.11    /* energy gap at 300K in eV */
#define EgSchottky 0.69
#define EgGe       0.67
#define EgGaAs     1.43
#define Eg0Si      1.16    /* energy gap at 0K in eV */

#define GMin       NR_TINY

#define sqr(x)    ((x) * (x))
#define cubic(x)  ((x) * (x) * (x))
#define quadr(x)  (exp (4 * log (fabs (x))))
#define coth(x)   (1 / tanh (x))
#define sech(x)   (1 / cosh (x))
#define cosech(x) (1 / sinh (x))
#define kelvin(x) ((x) - K)
#define degree(x) (K + (x))
#define rad(x)    (M_PI * (x) / 180.0)
#define deg(x)    (180.0 * (x) / M_PI)

#ifndef MAX
# define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
# define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#endif /* __CONSTANTS_H__ */
