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
 * $Id: constants.h,v 1.6 2007-08-15 11:02:44 ela Exp $
 *
 */

/*!\file constants.h 
   \brief global natural constant header file
   \todo Create a material header
*/ 
#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include "consts.h"
#include "precision.h"

/*!\brief speed of light in vacuum (\f$c_0\f$) */
#define C0   299792458.0
/*!\brief magnetic constant of vacuum (\f$\mu_0=4\pi\times10^{-7}\f$) */
#define MU0  12.566370614e-7
/*!\brief Electric constant of vacuum \f$\varepsilon_0\f$*/
#define E0   8.854187817e-12
/*!\brief Wave impedance in vacuum (\f$Z_0=\sqrt{\frac{\mu_0}{\varepsilon_0}}\f$)*/
#define Z0   376.73031346958504364963
/*!\brief Planck constant (\f$h\f$)
   \todo Add h bar (\f$h/(2\pi)\f$)
*/
#define H    6.626069311e-34
/*!\brief Absolute 0 in centigrade  */
#define K    -273.15  
/*!\brief standard temperature      */
#define T0   290

/*!\brief Boltzmann constant (\f$k_B\f$) */
#define kB   1.380650524e-23
/*!\brief Elementary charge (\f$q_e\f$)  */
#define Q    1.6021765314e-19
/*!\brief Boltzmann constant over Elementary charge (\f$k_B/q_e\f$)*/
#define kBoverQ    0.86173433260414314916e-4
/*!\brief Elementary charge over Boltzmann constant (\f$q_e/k_B\f$)*/
#define QoverkB    1.16045045690360379713e+4

/*!\brief Relative permittivity of Silicon dioxide (Silica) */
#define ESiO2      3.9  
/*!\brief Relative permittivity of Silicon */
#define ESi        11.7
/*!\brief relative permittivity of Germanium */
#define EGe        15.8
/*!\brief Relative permittivity of Gallium(III) arsenide */
#define EGaAs      13.1
/*!\brief Intrinsic carrier concentration in 1/m^3 of Silicon */
#define NiSi       1.45e16 
/*!\brief Intrinsic carrier concentration in 1/m^3 of Germanium */
#define NiGe       2.40e19
/*!\brief Intrinsic carrier concentration in 1/m^3 of Gallium(III) arsenide */
#define NiGaAs     9.00e12
/*!\brief Energy gap at 300K in eV of Silicon */
#define EgSi       1.11    
/*!\brief Energy gap at 300K in eV of Schottky 
  \todo What kind of metal (ideal, real, gold)?
*/
#define EgSchottky 0.69
/*!\brief Energy gap at 300K in eV of Germanium */
#define EgGe       0.67
/*!\brief Energy gap at 300K in eV of Gallium(III) arsenide */
#define EgGaAs     1.43
/*!\brief Energy gap at 0K in eV of Silicon */
#define Eg0Si      1.16

/*!\brief Gmin 
   \todo Define and document
*/
#define GMin       NR_TINY

/*!\brief Square a value
   \param[in] x value to square
   \return squarred expression
   \todo static inline
*/
#define sqr(x)    ((x) * (x))
/*!\brief cube function
   \param[in] x value to put at cube
   \return Cubed expression
   \todo static inline
*/
#define cubic(x)  ((x) * (x) * (x))
/*!\brief quad function
   \param[in] x function parameter
   \return \f$x^4\f$
   \todo will be better to implement as static inline
   because we could do something like
   tmp = sqr(x);
   return sqr(tmp);
*/
#define quadr(x)  (exp (4 * log (fabs (x))))
/*!\brief hyperbolic cotangent 
   \todo Better as static inline 
*/
#define coth(x)   (1 / tanh (x))
/*!\brief hyperbolic secant
   \todo Better as static inline 
*/
#define sech(x)   (1 / cosh (x))
/*!\brief hyperbolic cosecant
   \todo Better as static inline 
*/
#define cosech(x) (1 / sinh (x))
/*!\brief Convert kelvin to celcius
   \todo Better as static inline 
   \todo Rename as kelvin2celcius
*/
#define kelvin(x) ((x) - K)
/*!\brief Convert celcius to kelvin
   \todo Better as static inline 
   \todo Rename as celcius2kelvin
*/
#define degree(x) (K + (x))
/*!\brief Convert degree to radian
   \todo Better as static inline 
   \todo Rename as deg2rad
*/
#define rad(x)    (M_PI * (x) / 180.0)
/*!\brief Convert radian to degree
   \todo Better as static inline 
   \todo Rename as rad2deg
*/
#define deg(x)    (180.0 * (x) / M_PI)

#ifndef MAX
/*!\brief Maximum of x and y */
# define MAX(x,y) (((x) > (y)) ? (x) : (y))
#endif

#ifndef MIN
/*!\brief Minimum of x and y */
# define MIN(x,y) (((x) < (y)) ? (x) : (y))
#endif

#endif /* __CONSTANTS_H__ */
