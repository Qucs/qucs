/*
 * evaluate.cpp - the Qucs equation evaluator implementations
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
 * $Id: evaluate.cpp,v 1.7 2004-04-30 17:27:41 margraf Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "equation.h"
#include "evaluate.h"

using namespace eqn;

// Short macros in order to obtain the correct constant value.
#define D(con) ((constant *) (con))->d
#define C(con) ((constant *) (con))->c
#define V(con) ((constant *) (con))->v
#define M(con) ((constant *) (con))->m

// ******************** unary plus *************************
constant * evaluate::plus_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::plus_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1);
  return res;
}

constant * evaluate::plus_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1);
  return res;
}

// ****************** addition *****************************
constant * evaluate::plus_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 + d2;
  return res;
}

constant * evaluate::plus_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 + *c2);
  return res;
}

constant * evaluate::plus_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 + d2);
  return res;
}

constant * evaluate::plus_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 + *c2);
  return res;
}

constant * evaluate::plus_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 + d2);
  return res;
}

constant * evaluate::plus_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v2 + d1);
  return res;
}

constant * evaluate::plus_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 + *c2);
  return res;
}

constant * evaluate::plus_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v2 + *c1);
  return res;
}

constant * evaluate::plus_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 + *v2);
  return res;
}

// ******************** unary minus ***************************
constant * evaluate::minus_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = -d1;
  return res;
}

constant * evaluate::minus_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (-*c1);
  return res;
}

constant * evaluate::minus_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (-*v1);
  return res;
}

// ****************** subtraction *****************************
constant * evaluate::minus_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 - d2;
  return res;
}

constant * evaluate::minus_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 - *c2);
  return res;
}

constant * evaluate::minus_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 - d2);
  return res;
}

constant * evaluate::minus_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 - *c2);
  return res;
}

constant * evaluate::minus_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 - d2);
  return res;
}

constant * evaluate::minus_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (d1 - *v2);
  return res;
}

constant * evaluate::minus_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 - *c2);
  return res;
}

constant * evaluate::minus_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*c1 - *v2);
  return res;
}

constant * evaluate::minus_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (*v1 - *v2);
  return res;
}

// ****************** multiplication *************************
constant * evaluate::times_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 * d2;
  return res;
}

constant * evaluate::times_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) * (*c2));
  return res;
}

constant * evaluate::times_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) * d2);
  return res;
}

constant * evaluate::times_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 * (*c2));
  return res;
}

constant * evaluate::times_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) * d2);
  return res;
}

constant * evaluate::times_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v2) * d1);
  return res;
}

constant * evaluate::times_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) * (*c2));
  return res;
}

constant * evaluate::times_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v2) * (*c1));
  return res;
}

constant * evaluate::times_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) * (*v2));
  return res;
}

// ****************** division *************************
constant * evaluate::over_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 / d2;
  return res;
}

constant * evaluate::over_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) / (*c2));
  return res;
}

constant * evaluate::over_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) / d2);
  return res;
}

constant * evaluate::over_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 / (*c2));
  return res;
}

constant * evaluate::over_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) / d2);
  return res;
}

constant * evaluate::over_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = (d1 / (*v2));
  return res;
}

constant * evaluate::over_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) / (*c2));
  return res;
}

constant * evaluate::over_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*c1) / (*v2));
  return res;
}

constant * evaluate::over_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) / (*v2));
  return res;
}

// ****************** modulo *************************
constant * evaluate::modulo_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = fmod (d1, d2);
  return res;
}

constant * evaluate::modulo_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) % (*c2));
  return res;
}

constant * evaluate::modulo_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex ((*c1) % d2);
  return res;
}

constant * evaluate::modulo_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 % (*c2));
  return res;
}

constant * evaluate::modulo_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = *v1 % d2;
  return res;
}

constant * evaluate::modulo_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = d1 % *v2;
  return res;
}

constant * evaluate::modulo_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*v1) % (*c2));
  return res;
}

constant * evaluate::modulo_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = ((*c1) % (*v2));
  return res;
}

constant * evaluate::modulo_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = *v1 % *v2;
  return res;
}

// ****************** power *************************
constant * evaluate::power_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = pow (d1, d2);
  return res;
}

constant * evaluate::power_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (pow (*c1, *c2));
  return res;
}

constant * evaluate::power_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (pow (*c1, d2));
  return res;
}

constant * evaluate::power_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (pow (d1, *c2));
  return res;
}

constant * evaluate::power_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = pow (*v1, d2);
  return res;
}

constant * evaluate::power_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = pow (d1, *v2);
  return res;
}

constant * evaluate::power_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = pow (*v1, *c2);
  return res;
}

constant * evaluate::power_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = pow (*c1, *v2);
  return res;
}

constant * evaluate::power_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = pow (*v1, *v2);
  return res;
}

// ************** real value **********************
constant * evaluate::real_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::real_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = real (*c1);
  return res;
}

constant * evaluate::real_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = real (*v1);
  return res;
}

// ************** imaginary value **********************
constant * evaluate::imag_d (constant * ) {
  constant * res = new constant (TAG_DOUBLE);
  res->d = 0.0;
  return res;
}

constant * evaluate::imag_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = imag (*c1);
  return res;
}

constant * evaluate::imag_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = imag (*v1);
  return res;
}

// ************** absolute value **********************
constant * evaluate::abs_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::abs_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = abs (*c1);
  return res;
}

constant * evaluate::abs_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = abs (*v1);
  return res;
}

// ************** conjugate complex **********************
constant * evaluate::conj_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::conj_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (conj (*c1));
  return res;
}

constant * evaluate::conj_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = conj (*v1);
  return res;
}

// ********** square of absolute value *****************
constant * evaluate::norm_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 * d1;
  return res;
}

constant * evaluate::norm_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = norm (*c1);
  return res;
}

constant * evaluate::norm_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = norm (*v1);
  return res;
}

// ********** phase in degrees *****************
constant * evaluate::phase_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  if(d1 >= 0.0) res->d = 0.0;
  else res->d = 180.0;
  return res;
}

constant * evaluate::phase_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = arg (*c1) * 180.0 / M_PIl;
  return res;
}

constant * evaluate::phase_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arg (*v1);
  *res->v *= 180.0 / M_PIl;
  return res;
}

// ********** phase in radians *****************
constant * evaluate::arg_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  if(d1 >= 0.0) res->d = 0.0;
  else res->d = M_PIl;
  return res;
}

constant * evaluate::arg_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = arg (*c1);
  return res;
}

constant * evaluate::arg_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arg (*v1);
  return res;
}

// ********** voltage decibel *****************
constant * evaluate::dB_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 20.0 * log10 (fabs (d1));
  return res;
}

constant * evaluate::dB_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = dB (*c1);
  return res;
}

constant * evaluate::dB_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = dB (*v1);
  return res;
}

// ********** square root *****************
constant * evaluate::sqrt_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if (d1 < 0.0) {
    res->c = new complex (0.0, sqrt (-d1));
  }
  else {
    res->c = new complex (sqrt (d1));
  }
  return res;
}

constant * evaluate::sqrt_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (sqrt (*c1));
  return res;
}

constant * evaluate::sqrt_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = sqrt (*v1);
  return res;
}

// ********** exponential function *****************
constant * evaluate::exp_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = exp (d1);
  return res;
}

constant * evaluate::exp_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (exp (*c1));
  return res;
}

constant * evaluate::exp_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = exp (*v1);
  return res;
}

// ********** natural logarithm *****************
constant * evaluate::ln_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if(d1 >= 0.0) res->c = new complex (log (d1));
  else res->c = new complex (log(-d1), M_PIl);
  return res;
}

constant * evaluate::ln_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (ln (*c1));
  return res;
}

constant * evaluate::ln_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = ln (*v1);
  return res;
}

// ********** decimal logarithm *****************
constant * evaluate::log10_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if(d1 >= 0.0) res->c = new complex (log10 (d1));
  else res->c = new complex (log10(-d1), M_PIl * M_LOG10El);
  return res;
}

constant * evaluate::log10_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (log10 (*c1));
  return res;
}

constant * evaluate::log10_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = log10 (*v1);
  return res;
}

// ********** binary logarithm *****************
constant * evaluate::log2_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if(d1 >= 0.0) res->c = new complex (log2 (d1));
  else res->c = new complex (log2(-d1), M_PIl * M_LOG2El);
  return res;
}

constant * evaluate::log2_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (log2 (*c1));
  return res;
}

constant * evaluate::log2_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = log2 (*v1);
  return res;
}

// **************** sine ***********************
constant * evaluate::sin_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = sin (d1);
  return res;
}

constant * evaluate::sin_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (sin (*c1));
  return res;
}

constant * evaluate::sin_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = sin (*v1);
  return res;
}

// ************* arcus sine *********************
constant * evaluate::arcsin_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = atan (d1 / sqrt (1.0 - d1*d1));
  return res;
}

constant * evaluate::arcsin_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arcsin (*c1));
  return res;
}

constant * evaluate::arcsin_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arcsin (*v1);
  return res;
}

// **************** cosine *********************
constant * evaluate::cos_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = cos (d1);
  return res;
}

constant * evaluate::cos_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (cos (*c1));
  return res;
}

constant * evaluate::cos_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = cos (*v1);
  return res;
}

// ************* arcus cosine ******************
constant * evaluate::arccos_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = M_PI_2l - atan (d1 / sqrt (1.0 - d1*d1));
  return res;
}

constant * evaluate::arccos_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arccos (*c1));
  return res;
}

constant * evaluate::arccos_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arccos (*v1);
  return res;
}

// **************** tangent *********************
constant * evaluate::tan_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = tan (d1);
  return res;
}

constant * evaluate::tan_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (tan (*c1));
  return res;
}

constant * evaluate::tan_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = tan (*v1);
  return res;
}

// ************** arcus tangent ******************
constant * evaluate::arctan_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = atan (d1);
  return res;
}

constant * evaluate::arctan_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arctan (*c1));
  return res;
}

constant * evaluate::arctan_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arctan (*v1);
  return res;
}

// *************** cotangent ********************
constant * evaluate::cot_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 1.0 / tan (d1);
  return res;
}

constant * evaluate::cot_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (cot (*c1));
  return res;
}

constant * evaluate::cot_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = cot (*v1);
  return res;
}

// ************ arcus cotangent *****************
constant * evaluate::arccot_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = M_PI_2l - atan (d1);
  return res;
}

constant * evaluate::arccot_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arccot (*c1));
  return res;
}

constant * evaluate::arccot_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arccot (*v1);
  return res;
}

// *********** sine hyperbolicus *******************
constant * evaluate::sinh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = sinh (d1);
  return res;
}

constant * evaluate::sinh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (sinh (*c1));
  return res;
}

constant * evaluate::sinh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = sinh (*v1);
  return res;
}

// ********** area sine hyperbolicus **************
constant * evaluate::arsinh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = log (d1 + sqrt (d1*d1 + 1));
  return res;
}

constant * evaluate::arsinh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arsinh (*c1));
  return res;
}

constant * evaluate::arsinh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arsinh (*v1);
  return res;
}

// *********** cosine hyperbolicus ***************
constant * evaluate::cosh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = cosh (d1);
  return res;
}

constant * evaluate::cosh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (cosh (*c1));
  return res;
}

constant * evaluate::cosh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = cosh (*v1);
  return res;
}

// ********* area cosine hyperbolicus ************
constant * evaluate::arcosh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = log (d1 + sqrt (d1*d1 - 1));
  return res;
}

constant * evaluate::arcosh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arcosh (*c1));
  return res;
}

constant * evaluate::arcosh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arcosh (*v1);
  return res;
}

// ********** tangent hyperbolicus **************
constant * evaluate::tanh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = tanh (d1);
  return res;
}

constant * evaluate::tanh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (tanh (*c1));
  return res;
}

constant * evaluate::tanh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = tanh (*v1);
  return res;
}

// ******* area tangent hyperbolicus **********
constant * evaluate::artanh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 0.5 * log ((1.0 + d1) / (1.0 - d1));
  return res;
}

constant * evaluate::artanh_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (artanh (*c1));
  return res;
}

constant * evaluate::artanh_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = artanh (*v1);
  return res;
}

// ********* cotangent hyperbolicus ************
constant * evaluate::coth_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 1.0 / tanh (d1);
  return res;
}

constant * evaluate::coth_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (coth (*c1));
  return res;
}

constant * evaluate::coth_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = coth (*v1);
  return res;
}

// ******* area cotangent hyperbolicus **********
constant * evaluate::arcoth_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 0.5 * log ((d1 + 1.0) / (d1 - 1.0));
  return res;
}

constant * evaluate::arcoth_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (arcoth (*c1));
  return res;
}

constant * evaluate::arcoth_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = arcoth (*v1);
  return res;
}

// ***** convert impedance to reflexion coefficient *****
constant * evaluate::ztor_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = (d1 - 50.0) / (d1 + 50.0);
  return res;
}

constant * evaluate::ztor_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (ztor (*c1));
  return res;
}

constant * evaluate::ztor_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = ztor (*v1);
  return res;
}

// ***** convert reflexion coefficient to impedance *****
constant * evaluate::rtoz_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 50.0 * (1 + d1) / (1 - d1);
  return res;
}

constant * evaluate::rtoz_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (rtoz (*c1));
  return res;
}

constant * evaluate::rtoz_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = rtoz (*v1);
  return res;
}

// ** convert reflexion coefficient to standing wave ratio **
constant * evaluate::rtoswr_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = (1+ fabs (d1)) / (1 - fabs (d1));
  return res;
}

constant * evaluate::rtoswr_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = (1+ abs (*c1)) / (1 - abs (*c1));
  return res;
}

constant * evaluate::rtoswr_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  for (int i = 0; i < v1->getSize (); i++)
    res->v->set ( (1+ abs (v1->get (i))) / (1 - abs (v1->get (i))), i);
  return res;
}

// ***************** maximum *******************
constant * evaluate::max_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::max_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  if(fabs (arg (*c1)) < M_PI_2l) res->c = new complex (abs (*c1));
  else  res->c = new complex (-abs (*c1));
  return res;
}

constant * evaluate::max_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = v1->maximum ();
  return res;
}

// ***************** minimum *******************
constant * evaluate::min_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::min_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  if(fabs (arg (*c1)) < M_PI_2l) res->c = new complex (abs (*c1));
  else  res->c = new complex (-abs (*c1));
  return res;
}

constant * evaluate::min_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = v1->minimum ();
  return res;
}

// ******************** sum **********************
constant * evaluate::sum_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::sum_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1);
  return res;
}

constant * evaluate::sum_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (sum (*v1));
  return res;
}

// ****************** product ********************
constant * evaluate::prod_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::prod_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1);
  return res;
}

constant * evaluate::prod_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (prod (*v1));
  return res;
}

// ******************* average *********************
constant * evaluate::avg_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1;
  return res;
}

constant * evaluate::avg_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1);
  return res;
}

constant * evaluate::avg_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (avg (*v1));
  return res;
}


// Array containing all kinds of applications.
struct application_t eqn::applications[] = {
  { "+", TAG_DOUBLE,  evaluate::plus_d, 1, { TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_c, 1, { TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_v, 1, { TAG_VECTOR  } },

  { "+", TAG_DOUBLE,  evaluate::plus_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "+", TAG_COMPLEX, evaluate::plus_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "+", TAG_VECTOR,  evaluate::plus_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "+", TAG_VECTOR,  evaluate::plus_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "+", TAG_VECTOR,  evaluate::plus_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "-", TAG_DOUBLE,  evaluate::minus_d, 1, { TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_c, 1, { TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_v, 1, { TAG_VECTOR  } },

  { "-", TAG_DOUBLE,  evaluate::minus_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "-", TAG_COMPLEX, evaluate::minus_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "-", TAG_VECTOR,  evaluate::minus_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "-", TAG_VECTOR,  evaluate::minus_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_c_v, 2, { TAG_COMPLEX,  TAG_VECTOR } },
  { "-", TAG_VECTOR,  evaluate::minus_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "*", TAG_DOUBLE,  evaluate::times_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "*", TAG_COMPLEX, evaluate::times_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "*", TAG_COMPLEX, evaluate::times_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "*", TAG_COMPLEX, evaluate::times_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "*", TAG_VECTOR,  evaluate::times_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "*", TAG_VECTOR,  evaluate::times_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "*", TAG_VECTOR,  evaluate::times_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "*", TAG_VECTOR,  evaluate::times_c_v, 2, { TAG_COMPLEX,  TAG_VECTOR } },
  { "*", TAG_VECTOR,  evaluate::times_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "/", TAG_DOUBLE,  evaluate::over_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "/", TAG_COMPLEX, evaluate::over_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "/", TAG_COMPLEX, evaluate::over_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "/", TAG_COMPLEX, evaluate::over_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "/", TAG_VECTOR,  evaluate::over_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "/", TAG_VECTOR,  evaluate::over_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "/", TAG_VECTOR,  evaluate::over_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "/", TAG_VECTOR,  evaluate::over_c_v, 2, { TAG_COMPLEX,  TAG_VECTOR } },
  { "/", TAG_VECTOR,  evaluate::over_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "%", TAG_DOUBLE,  evaluate::modulo_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "%", TAG_COMPLEX, evaluate::modulo_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "%", TAG_COMPLEX, evaluate::modulo_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "%", TAG_COMPLEX, evaluate::modulo_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "%", TAG_VECTOR,  evaluate::modulo_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "%", TAG_VECTOR,  evaluate::modulo_c_v, 2, { TAG_COMPLEX,  TAG_VECTOR } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "^", TAG_DOUBLE,  evaluate::power_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "^", TAG_COMPLEX, evaluate::power_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "^", TAG_COMPLEX, evaluate::power_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "^", TAG_COMPLEX, evaluate::power_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "^", TAG_VECTOR,  evaluate::power_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "^", TAG_VECTOR,  evaluate::power_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "^", TAG_VECTOR,  evaluate::power_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "^", TAG_VECTOR,  evaluate::power_c_v, 2, { TAG_COMPLEX,  TAG_VECTOR } },
  { "^", TAG_VECTOR,  evaluate::power_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "real", TAG_DOUBLE,  evaluate::real_d, 1, { TAG_DOUBLE  } },
  { "real", TAG_DOUBLE,  evaluate::real_c, 1, { TAG_COMPLEX } },
  { "real", TAG_VECTOR,  evaluate::real_v, 1, { TAG_VECTOR  } },

  { "imag", TAG_DOUBLE,  evaluate::imag_d, 1, { TAG_DOUBLE  } },
  { "imag", TAG_DOUBLE,  evaluate::imag_c, 1, { TAG_COMPLEX } },
  { "imag", TAG_VECTOR,  evaluate::imag_v, 1, { TAG_VECTOR  } },

  { "abs", TAG_DOUBLE,  evaluate::abs_d, 1, { TAG_DOUBLE  } },
  { "abs", TAG_DOUBLE,  evaluate::abs_c, 1, { TAG_COMPLEX } },
  { "abs", TAG_VECTOR,  evaluate::abs_v, 1, { TAG_VECTOR  } },

  { "mag", TAG_DOUBLE,  evaluate::abs_d, 1, { TAG_DOUBLE  } },
  { "mag", TAG_DOUBLE,  evaluate::abs_c, 1, { TAG_COMPLEX } },
  { "mag", TAG_VECTOR,  evaluate::abs_v, 1, { TAG_VECTOR  } },

  { "conj", TAG_DOUBLE,  evaluate::conj_d, 1, { TAG_DOUBLE  } },
  { "conj", TAG_COMPLEX, evaluate::conj_c, 1, { TAG_COMPLEX } },
  { "conj", TAG_VECTOR,  evaluate::conj_v, 1, { TAG_VECTOR  } },

  { "norm", TAG_DOUBLE,  evaluate::norm_d, 1, { TAG_DOUBLE  } },
  { "norm", TAG_DOUBLE,  evaluate::norm_c, 1, { TAG_COMPLEX } },
  { "norm", TAG_VECTOR,  evaluate::norm_v, 1, { TAG_VECTOR  } },

  { "phase", TAG_DOUBLE,  evaluate::phase_d, 1, { TAG_DOUBLE  } },
  { "phase", TAG_DOUBLE,  evaluate::phase_c, 1, { TAG_COMPLEX } },
  { "phase", TAG_VECTOR,  evaluate::phase_v, 1, { TAG_VECTOR  } },

  { "angle", TAG_DOUBLE,  evaluate::arg_d, 1, { TAG_DOUBLE  } },
  { "angle", TAG_DOUBLE,  evaluate::arg_c, 1, { TAG_COMPLEX } },
  { "angle", TAG_VECTOR,  evaluate::arg_v, 1, { TAG_VECTOR  } },

  { "arg", TAG_DOUBLE,  evaluate::arg_d, 1, { TAG_DOUBLE  } },
  { "arg", TAG_DOUBLE,  evaluate::arg_c, 1, { TAG_COMPLEX } },
  { "arg", TAG_VECTOR,  evaluate::arg_v, 1, { TAG_VECTOR  } },

  { "dB", TAG_DOUBLE,  evaluate::dB_d, 1, { TAG_DOUBLE  } },
  { "dB", TAG_DOUBLE,  evaluate::dB_c, 1, { TAG_COMPLEX } },
  { "dB", TAG_VECTOR,  evaluate::dB_v, 1, { TAG_VECTOR  } },

  { "sqrt", TAG_COMPLEX, evaluate::sqrt_d, 1, { TAG_DOUBLE } },
  { "sqrt", TAG_COMPLEX, evaluate::sqrt_c, 1, { TAG_COMPLEX } },
  { "sqrt", TAG_VECTOR,  evaluate::sqrt_v, 1, { TAG_VECTOR  } },

  { "exp", TAG_DOUBLE,  evaluate::exp_d, 1, { TAG_DOUBLE  } },
  { "exp", TAG_COMPLEX, evaluate::exp_c, 1, { TAG_COMPLEX } },
  { "exp", TAG_VECTOR,  evaluate::exp_v, 1, { TAG_VECTOR  } },

  { "ln", TAG_COMPLEX, evaluate::ln_d, 1, { TAG_DOUBLE  } },
  { "ln", TAG_COMPLEX, evaluate::ln_c, 1, { TAG_COMPLEX } },
  { "ln", TAG_VECTOR,  evaluate::ln_v, 1, { TAG_VECTOR  } },

  { "log10", TAG_COMPLEX, evaluate::log10_d, 1, { TAG_DOUBLE  } },
  { "log10", TAG_COMPLEX, evaluate::log10_c, 1, { TAG_COMPLEX } },
  { "log10", TAG_VECTOR,  evaluate::log10_v, 1, { TAG_VECTOR  } },

  { "log2", TAG_COMPLEX, evaluate::log2_d, 1, { TAG_DOUBLE  } },
  { "log2", TAG_COMPLEX, evaluate::log2_c, 1, { TAG_COMPLEX } },
  { "log2", TAG_VECTOR,  evaluate::log2_v, 1, { TAG_VECTOR  } },

  { "sin", TAG_DOUBLE,  evaluate::sin_d, 1, { TAG_DOUBLE  } },
  { "sin", TAG_COMPLEX, evaluate::sin_c, 1, { TAG_COMPLEX } },
  { "sin", TAG_VECTOR,  evaluate::sin_v, 1, { TAG_VECTOR  } },

  { "arcsin", TAG_DOUBLE,  evaluate::arcsin_d, 1, { TAG_DOUBLE  } },
  { "arcsin", TAG_COMPLEX, evaluate::arcsin_c, 1, { TAG_COMPLEX } },
  { "arcsin", TAG_VECTOR,  evaluate::arcsin_v, 1, { TAG_VECTOR  } },

  { "cos", TAG_DOUBLE,  evaluate::cos_d, 1, { TAG_DOUBLE  } },
  { "cos", TAG_COMPLEX, evaluate::cos_c, 1, { TAG_COMPLEX } },
  { "cos", TAG_VECTOR,  evaluate::cos_v, 1, { TAG_VECTOR  } },

  { "arccos", TAG_DOUBLE,  evaluate::arccos_d, 1, { TAG_DOUBLE  } },
  { "arccos", TAG_COMPLEX, evaluate::arccos_c, 1, { TAG_COMPLEX } },
  { "arccos", TAG_VECTOR,  evaluate::arccos_v, 1, { TAG_VECTOR  } },

  { "tan", TAG_DOUBLE,  evaluate::tan_d, 1, { TAG_DOUBLE  } },
  { "tan", TAG_COMPLEX, evaluate::tan_c, 1, { TAG_COMPLEX } },
  { "tan", TAG_VECTOR,  evaluate::tan_v, 1, { TAG_VECTOR  } },

  { "arctan", TAG_DOUBLE,  evaluate::arctan_d, 1, { TAG_DOUBLE  } },
  { "arctan", TAG_COMPLEX, evaluate::arctan_c, 1, { TAG_COMPLEX } },
  { "arctan", TAG_VECTOR,  evaluate::arctan_v, 1, { TAG_VECTOR  } },

  { "cot", TAG_DOUBLE,  evaluate::cot_d, 1, { TAG_DOUBLE  } },
  { "cot", TAG_COMPLEX, evaluate::cot_c, 1, { TAG_COMPLEX } },
  { "cot", TAG_VECTOR,  evaluate::cot_v, 1, { TAG_VECTOR  } },

  { "arccot", TAG_DOUBLE,  evaluate::arccot_d, 1, { TAG_DOUBLE  } },
  { "arccot", TAG_COMPLEX, evaluate::arccot_c, 1, { TAG_COMPLEX } },
  { "arccot", TAG_VECTOR,  evaluate::arccot_v, 1, { TAG_VECTOR  } },

  { "sinh", TAG_DOUBLE,  evaluate::sinh_d, 1, { TAG_DOUBLE  } },
  { "sinh", TAG_COMPLEX, evaluate::sinh_c, 1, { TAG_COMPLEX } },
  { "sinh", TAG_VECTOR,  evaluate::sinh_v, 1, { TAG_VECTOR  } },

  { "arsinh", TAG_DOUBLE,  evaluate::arsinh_d, 1, { TAG_DOUBLE  } },
  { "arsinh", TAG_COMPLEX, evaluate::arsinh_c, 1, { TAG_COMPLEX } },
  { "arsinh", TAG_VECTOR,  evaluate::arsinh_v, 1, { TAG_VECTOR  } },

  { "cosh", TAG_DOUBLE,  evaluate::cosh_d, 1, { TAG_DOUBLE  } },
  { "cosh", TAG_COMPLEX, evaluate::cosh_c, 1, { TAG_COMPLEX } },
  { "cosh", TAG_VECTOR,  evaluate::cosh_v, 1, { TAG_VECTOR  } },

  { "arcosh", TAG_DOUBLE,  evaluate::arcosh_d, 1, { TAG_DOUBLE  } },
  { "arcosh", TAG_COMPLEX, evaluate::arcosh_c, 1, { TAG_COMPLEX } },
  { "arcosh", TAG_VECTOR,  evaluate::arcosh_v, 1, { TAG_VECTOR  } },

  { "tanh", TAG_DOUBLE,  evaluate::tanh_d, 1, { TAG_DOUBLE  } },
  { "tanh", TAG_COMPLEX, evaluate::tanh_c, 1, { TAG_COMPLEX } },
  { "tanh", TAG_VECTOR,  evaluate::tanh_v, 1, { TAG_VECTOR  } },

  { "artanh", TAG_DOUBLE,  evaluate::artanh_d, 1, { TAG_DOUBLE  } },
  { "artanh", TAG_COMPLEX, evaluate::artanh_c, 1, { TAG_COMPLEX } },
  { "artanh", TAG_VECTOR,  evaluate::artanh_v, 1, { TAG_VECTOR  } },

  { "coth", TAG_DOUBLE,  evaluate::coth_d, 1, { TAG_DOUBLE  } },
  { "coth", TAG_COMPLEX, evaluate::coth_c, 1, { TAG_COMPLEX } },
  { "coth", TAG_VECTOR,  evaluate::coth_v, 1, { TAG_VECTOR  } },

  { "arcoth", TAG_DOUBLE,  evaluate::arcoth_d, 1, { TAG_DOUBLE  } },
  { "arcoth", TAG_COMPLEX, evaluate::arcoth_c, 1, { TAG_COMPLEX } },
  { "arcoth", TAG_VECTOR,  evaluate::arcoth_v, 1, { TAG_VECTOR  } },

  { "ztor", TAG_DOUBLE,  evaluate::ztor_d, 1, { TAG_DOUBLE  } },
  { "ztor", TAG_COMPLEX, evaluate::ztor_c, 1, { TAG_COMPLEX } },
  { "ztor", TAG_VECTOR,  evaluate::ztor_v, 1, { TAG_VECTOR  } },

  { "rtoz", TAG_DOUBLE,  evaluate::rtoz_d, 1, { TAG_DOUBLE  } },
  { "rtoz", TAG_COMPLEX, evaluate::rtoz_c, 1, { TAG_COMPLEX } },
  { "rtoz", TAG_VECTOR,  evaluate::rtoz_v, 1, { TAG_VECTOR  } },

  { "rtoswr", TAG_DOUBLE,  evaluate::rtoswr_d, 1, { TAG_DOUBLE  } },
  { "rtoswr", TAG_DOUBLE,  evaluate::rtoswr_c, 1, { TAG_COMPLEX } },
  { "rtoswr", TAG_VECTOR,  evaluate::rtoswr_v, 1, { TAG_VECTOR  } },

  { "max", TAG_DOUBLE,  evaluate::max_d, 1, { TAG_DOUBLE  } },
  { "max", TAG_DOUBLE,  evaluate::max_c, 1, { TAG_COMPLEX } },
  { "max", TAG_DOUBLE,  evaluate::max_v, 1, { TAG_VECTOR  } },

  { "min", TAG_DOUBLE,  evaluate::min_d, 1, { TAG_DOUBLE  } },
  { "min", TAG_DOUBLE,  evaluate::min_c, 1, { TAG_COMPLEX } },
  { "min", TAG_DOUBLE,  evaluate::min_v, 1, { TAG_VECTOR  } },

  { "sum", TAG_DOUBLE,  evaluate::sum_d, 1, { TAG_DOUBLE  } },
  { "sum", TAG_COMPLEX, evaluate::sum_c, 1, { TAG_COMPLEX } },
  { "sum", TAG_COMPLEX, evaluate::sum_v, 1, { TAG_VECTOR  } },

  { "prod", TAG_DOUBLE,  evaluate::prod_d, 1, { TAG_DOUBLE  } },
  { "prod", TAG_COMPLEX, evaluate::prod_c, 1, { TAG_COMPLEX } },
  { "prod", TAG_COMPLEX, evaluate::prod_v, 1, { TAG_VECTOR  } },

  { "avg", TAG_DOUBLE,  evaluate::avg_d, 1, { TAG_DOUBLE  } },
  { "avg", TAG_COMPLEX, evaluate::avg_c, 1, { TAG_COMPLEX } },
  { "avg", TAG_COMPLEX, evaluate::avg_v, 1, { TAG_VECTOR  } },

  { NULL, 0, NULL, 0, { } /* end of list */ }
};
