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
 * $Id: evaluate.h,v 1.5 2004/04/30 17:27:41 margraf Exp $
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
  static constant * plus_c_c (constant *);
  static constant * plus_c_d (constant *);
  static constant * plus_d_c (constant *);
  static constant * plus_v_d (constant *);
  static constant * plus_d_v (constant *);
  static constant * plus_v_c (constant *);
  static constant * plus_c_v (constant *);
  static constant * plus_v_v (constant *);

  static constant * minus_d (constant *);
  static constant * minus_c (constant *);
  static constant * minus_v (constant *);

  static constant * minus_d_d (constant *);
  static constant * minus_c_c (constant *);
  static constant * minus_c_d (constant *);
  static constant * minus_d_c (constant *);
  static constant * minus_v_d (constant *);
  static constant * minus_d_v (constant *);
  static constant * minus_v_c (constant *);
  static constant * minus_c_v (constant *);
  static constant * minus_v_v (constant *);

  static constant * times_d_d (constant *);
  static constant * times_c_c (constant *);
  static constant * times_c_d (constant *);
  static constant * times_d_c (constant *);
  static constant * times_v_d (constant *);
  static constant * times_d_v (constant *);
  static constant * times_v_c (constant *);
  static constant * times_c_v (constant *);
  static constant * times_v_v (constant *);

  static constant * over_d_d (constant *);
  static constant * over_c_c (constant *);
  static constant * over_c_d (constant *);
  static constant * over_d_c (constant *);
  static constant * over_v_d (constant *);
  static constant * over_d_v (constant *);
  static constant * over_v_c (constant *);
  static constant * over_c_v (constant *);
  static constant * over_v_v (constant *);

  static constant * modulo_d_d (constant *);
  static constant * modulo_c_c (constant *);
  static constant * modulo_c_d (constant *);
  static constant * modulo_d_c (constant *);
  static constant * modulo_v_d (constant *);
  static constant * modulo_d_v (constant *);
  static constant * modulo_v_c (constant *);
  static constant * modulo_c_v (constant *);
  static constant * modulo_v_v (constant *);

  static constant * power_d_d (constant *);
  static constant * power_c_c (constant *);
  static constant * power_c_d (constant *);
  static constant * power_d_c (constant *);
  static constant * power_v_d (constant *);
  static constant * power_d_v (constant *);
  static constant * power_v_c (constant *);
  static constant * power_c_v (constant *);
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

  static constant * log2_d (constant *);
  static constant * log2_c (constant *);
  static constant * log2_v (constant *);

  static constant * sin_d (constant *);
  static constant * sin_c (constant *);
  static constant * sin_v (constant *);

  static constant * arcsin_d (constant *);
  static constant * arcsin_c (constant *);
  static constant * arcsin_v (constant *);

  static constant * cos_d (constant *);
  static constant * cos_c (constant *);
  static constant * cos_v (constant *);

  static constant * arccos_d (constant *);
  static constant * arccos_c (constant *);
  static constant * arccos_v (constant *);

  static constant * tan_d (constant *);
  static constant * tan_c (constant *);
  static constant * tan_v (constant *);

  static constant * arctan_d (constant *);
  static constant * arctan_c (constant *);
  static constant * arctan_v (constant *);

  static constant * cot_d (constant *);
  static constant * cot_c (constant *);
  static constant * cot_v (constant *);

  static constant * arccot_d (constant *);
  static constant * arccot_c (constant *);
  static constant * arccot_v (constant *);

  static constant * sinh_d (constant *);
  static constant * sinh_c (constant *);
  static constant * sinh_v (constant *);

  static constant * arsinh_d (constant *);
  static constant * arsinh_c (constant *);
  static constant * arsinh_v (constant *);

  static constant * cosh_d (constant *);
  static constant * cosh_c (constant *);
  static constant * cosh_v (constant *);

  static constant * arcosh_d (constant *);
  static constant * arcosh_c (constant *);
  static constant * arcosh_v (constant *);

  static constant * tanh_d (constant *);
  static constant * tanh_c (constant *);
  static constant * tanh_v (constant *);

  static constant * artanh_d (constant *);
  static constant * artanh_c (constant *);
  static constant * artanh_v (constant *);

  static constant * arcoth_d (constant *);
  static constant * arcoth_c (constant *);
  static constant * arcoth_v (constant *);

  static constant * coth_d (constant *);
  static constant * coth_c (constant *);
  static constant * coth_v (constant *);

  static constant * ztor_d (constant *);
  static constant * ztor_c (constant *);
  static constant * ztor_v (constant *);

  static constant * rtoz_d (constant *);
  static constant * rtoz_c (constant *);
  static constant * rtoz_v (constant *);

  static constant * rtoswr_d (constant *);
  static constant * rtoswr_c (constant *);
  static constant * rtoswr_v (constant *);

  static constant * max_d (constant *);
  static constant * max_c (constant *);
  static constant * max_v (constant *);

  static constant * min_d (constant *);
  static constant * min_c (constant *);
  static constant * min_v (constant *);

  static constant * sum_d (constant *);
  static constant * sum_c (constant *);
  static constant * sum_v (constant *);

  static constant * prod_d (constant *);
  static constant * prod_c (constant *);
  static constant * prod_v (constant *);

  static constant * avg_d (constant *);
  static constant * avg_c (constant *);
  static constant * avg_v (constant *);

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
