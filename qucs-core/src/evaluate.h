/*
 * evaluate.h - definitions for Qucs equation evaluations
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: evaluate.h,v 1.3 2004-04-18 18:37:16 margraf Exp $
 *
 */

#ifndef __EVALUATE_H__
#define __EVALUATE_H__

namespace eqn {

class constant;

/* This class is merely a container for the applications which can be
   applied to equation constants. */
class evaluate
{
public:
  static constant * plus_d (constant *);
  static constant * plus_c (constant *);
  static constant * plus_v (constant *);

  static constant * plus_d_d (constant *);
  static constant * plus_c_d (constant *);
  static constant * plus_d_c (constant *);
  static constant * plus_v_d (constant *);
  static constant * plus_d_v (constant *);
  static constant * plus_v_v (constant *);

  static constant * minus_d (constant *);
  static constant * minus_c (constant *);
  static constant * minus_v (constant *);

  static constant * minus_d_d (constant *);
  static constant * minus_c_d (constant *);
  static constant * minus_d_c (constant *);
  static constant * minus_v_d (constant *);
  static constant * minus_d_v (constant *);
  static constant * minus_v_v (constant *);

  static constant * times_d_d (constant *);
  static constant * times_c_d (constant *);
  static constant * times_d_c (constant *);
  static constant * times_v_d (constant *);
  static constant * times_d_v (constant *);
  static constant * times_v_v (constant *);

  static constant * over_d_d (constant *);
  static constant * over_c_d (constant *);
  static constant * over_d_c (constant *);
  static constant * over_v_d (constant *);
  static constant * over_d_v (constant *);
  static constant * over_v_v (constant *);

  static constant * modulo_d_d (constant *);
  static constant * modulo_c_d (constant *);
  static constant * modulo_d_c (constant *);
  static constant * modulo_v_d (constant *);
  static constant * modulo_d_v (constant *);
  static constant * modulo_v_v (constant *);

  static constant * power_d_d (constant *);
  static constant * power_c_d (constant *);
  static constant * power_d_c (constant *);
  static constant * power_v_d (constant *);
  static constant * power_d_v (constant *);
  static constant * power_v_v (constant *);

  static constant * real_d (constant *);
  static constant * real_c (constant *);
  static constant * real_v (constant *);

  static constant * imag_d (constant *);
  static constant * imag_c (constant *);
  static constant * imag_v (constant *);

  static constant * abs_d (constant *);
  static constant * abs_c (constant *);
  static constant * abs_v (constant *);

  static constant * conj_d (constant *);
  static constant * conj_c (constant *);
  static constant * conj_v (constant *);

  static constant * norm_d (constant *);
  static constant * norm_c (constant *);
  static constant * norm_v (constant *);

  static constant * phase_d (constant *);
  static constant * phase_c (constant *);
  static constant * phase_v (constant *);

  static constant * arg_d (constant *);
  static constant * arg_c (constant *);
  static constant * arg_v (constant *);

  static constant * dB_d (constant *);
  static constant * dB_c (constant *);
  static constant * dB_v (constant *);

  static constant * sqrt_d (constant *);
  static constant * sqrt_c (constant *);
  static constant * sqrt_v (constant *);

  static constant * exp_d (constant *);
  static constant * exp_c (constant *);
  static constant * exp_v (constant *);

  static constant * ln_d (constant *);
  static constant * ln_c (constant *);
  static constant * ln_v (constant *);

  static constant * log10_d (constant *);
  static constant * log10_c (constant *);
  static constant * log10_v (constant *);

};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

// Structure defining an application.
struct application_t
{
  char * application; /* the name of the application     */
  int retval;         /* its return type                 */
  evaluator_t eval;   /* the actual application function */
  int nargs;          /* number of arguments             */
  int args[16];       /* the appropriate argument types  */
};

extern struct application_t applications[];

} /* namespace */

#endif /* __EVALUATE_H__ */
