/*
 * consts.h - global math constant header file
 *
 * Copyright (C) 2004, 2005, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: consts.h,v 1.6 2007-08-07 20:43:04 ela Exp $
 *
 */

/*\file consts.h 
  Global math constant header file
*/

#ifndef __CONSTS_H__
#define __CONSTS_H__

#include <math.h>

#ifndef M_PI
/*!\brief  Archimedes' constant (\f$\pi\f$) */ 
#define M_PI      3.1415926535897932384626433832795029
#endif
/*!\brief Half of Archimedes' constant (\f$\pi/2\f$) */
#ifndef M_PI_2
#define M_PI_2    1.5707963267948966192313216916397514
#endif
/*!\brief A quarter of Archimedes' constant (\f$\pi/4\f$) */
#ifndef M_PI_4
#define M_PI_4    0.7853981633974483096156608458198757
#endif
/*!\brief Inverse of Archimedes' constant (\f$1/\pi\f$) */
#ifndef M_1_PI
#define M_1_PI    0.3183098861837906715377675267450287
#endif
/*\brief Twice the inverse of Archimedes' constant (\f$2/\pi\f$) */
#ifndef M_2_PI
#define M_2_PI    0.6366197723675813430755350534900574
#endif
/*!\brief Square root of Archimedes' constant (\f$\sqrt{\pi}\f$) */
#ifndef M_SQRTPI
#define M_SQRTPI  1.77245385090551602729816748334
#endif
/*!\brief  Euler's constant (\f$e=\sum_{n=0}^\infty \frac{1}{n!}\f$) */
#ifndef M_E
#define M_E       2.7182818284590452353602874713526625
#endif
/*!\brief Decimal logartihm of Euler's constant (\f$\log_{10} (e)\f$) */
#ifndef M_LOG10E
#define M_LOG10E  0.4342944819032518276511289189166051
#endif
/*!\brief Binary logartihm of Euler's constant (\f$\log_{2} (e)\f$)*/
#ifndef M_LOG2E
#define M_LOG2E   1.4426950408889634073599246810018922
#endif
/*!\brief Natural logarithm of 2 (\f$\ln 2\f$) */
#ifndef M_LN2
#define M_LN2     0.6931471805599453094172321214581766
#endif
/*!\brief Natural logarithm of 10 (\f$\ln 10\f$) */
#ifndef M_LN10
#define M_LN10    2.3025850929940456840179914546843642
#endif
/*!\brief Square root of 2 (\f$\sqrt{2}\f$) */
#ifndef M_SQRT2
#define M_SQRT2   1.4142135623730950488016887242096981
#endif
/*!\brief Inverse of Square root of 2 (\f$1/\sqrt{2}\f$) */
#ifndef M_SQRT1_2
#define M_SQRT1_2 0.7071067811865475244008443621048490
#endif
/*!\brief LIMEXP 
  \todo What is it?
*/
#ifndef M_LIMEXP
#define M_LIMEXP  80.0
#endif

#endif /* __CONSTS_H__ */
