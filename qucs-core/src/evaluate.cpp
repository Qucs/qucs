/*
 * evaluate.cpp - the Qucs equation evaluator implementations
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
 * $Id: evaluate.cpp,v 1.39 2005-12-05 12:09:36 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "poly.h"
#include "spline.h"
#include "fourier.h"
#include "constants.h"
#include "circuit.h"
#include "equation.h"
#include "evaluate.h"
#include "exception.h"
#include "exceptionstack.h"
#include "strlist.h"

using namespace eqn;
using namespace qucs;

// Short macros in order to obtain the correct constant value.
#define D(con)   ((constant *) (con))->d
#define C(con)   ((constant *) (con))->c
#define V(con)   ((constant *) (con))->v
#define M(con)   ((constant *) (con))->m
#define MV(con)  ((constant *) (con))->mv
#define STR(con) ((constant *) (con))->s
#define CHR(con) ((constant *) (con))->chr
#define INT(con) ((int) D (con))

#define A(a) ((assignment *) (a))
#define R(r) ((reference *) (r))

// Argument macros.
#define _D(var,idx) nr_double_t (var) = D (args->getResult (idx));
#define _C(var,idx) complex * (var) = C (args->getResult (idx));
#define _V(var,idx) vector * (var) = V (args->getResult (idx));
#define _M(var,idx) matrix * (var) = M (args->getResult (idx));
#define _MV(var,idx) matvec * (var) = MV (args->getResult (idx));

#define _ARD0(var) _D (var,0)
#define _ARD1(var) _D (var,1)
#define _ARD2(var) _D (var,2)
#define _ARC0(var) _C (var,0)
#define _ARC1(var) _C (var,1)
#define _ARC2(var) _C (var,2)
#define _ARM0(var) _M (var,0)
#define _ARM1(var) _M (var,1)
#define _ARM2(var) _M (var,2)
#define _ARV0(var) _V (var,0)
#define _ARV1(var) _V (var,1)
#define _ARV2(var) _V (var,2)
#define _ARMV0(var) _MV (var,0)
#define _ARMV1(var) _MV (var,1)
#define _ARMV2(var) _MV (var,2)

// Return value macros.
#define _DEFD() constant * res = new constant (TAG_DOUBLE);
#define _DEFC() constant * res = new constant (TAG_COMPLEX);
#define _DEFV() constant * res = new constant (TAG_VECTOR);
#define _DEFM() constant * res = new constant (TAG_MATRIX);
#define _DEFMV() constant * res = new constant (TAG_MATVEC);
#define _RETD(var) res->d = (var); return res;
#define _RETC(var) res->c = new complex (var); return res;
#define _RETV(var) res->v = new vector (var); return res;
#define _RETM(var) res->m = new matrix (var); return res;
#define _RETMV(var) res->mv = new matvec (var); return res;

// Throws a math exception.
#define THROW_MATH_EXCEPTION(txt) do { \
  qucs::exception * e = new qucs::exception (EXCEPTION_MATH); \
  e->setText (txt); throw_exception (e); } while (0)

/* The QUCS_CONCAT macros create a new concatenated symbol for the
   compiler in a portable way.  It is essential to use these macros
   like QUCS_CONCAT (a,b) and *not* like QUCS_CONCAT (a, b) or its
   variants. */
#if defined (__STDC__) || defined (__cplusplus)
# define QUCS_CONCAT2(a, b) a##b
# define QUCS_CONCAT3(a, b, c) a##b##c
#else
# define QUCS_CONCAT2(a, b) a/* */b
# define QUCS_CONCAT3(a, b, c) a/* */b/* */c
#endif

// The following macro is meant to be used for some simple functions.
#define MAKE_FUNC_DEFINITION_0(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  nr_double_t d = D (args->getResult (0));			  \
  constant * res = new constant (TAG_DOUBLE);			  \
  res->d = cfunc (d); return res;				  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  complex * c = C (args->getResult (0));			  \
  constant * res = new constant (TAG_COMPLEX);			  \
  res->c = new complex (cfunc (*c)); return res;		  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  vector * v = V (args->getResult (0));				  \
  constant * res = new constant (TAG_VECTOR);			  \
  res->v = new vector (cfunc (*v)); return res;			  \
}

#define MAKE_FUNC_DEFINITION_1(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) {  \
  nr_double_t d = D (args->getResult (0));                         \
  constant * res = new constant (TAG_DOUBLE);                      \
  res->d = cfunc (d); return res;                                  \
}								   \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) {  \
  complex * c = C (args->getResult (0));                           \
  constant * res = new constant (TAG_DOUBLE);                      \
  res->d = cfunc (*c); return res;                                 \
}								   \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) {  \
  vector * v = V (args->getResult (0));                            \
  constant * res = new constant (TAG_VECTOR);                      \
  res->v = new vector (cfunc (*v)); return res;                    \
}								   \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m) (constant * args) {  \
  matrix * m = M (args->getResult (0));                            \
  constant * res = new constant (TAG_MATRIX);                      \
  res->m = new matrix (cfunc (*m)); return res;                    \
}								   \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv) (constant * args) { \
  matvec * mv = MV (args->getResult (0));                          \
  constant * res = new constant (TAG_MATVEC);                      \
  res->mv = new matvec (cfunc (*mv)); return res;                  \
}

MAKE_FUNC_DEFINITION_0 (exp);    // exponential function
MAKE_FUNC_DEFINITION_0 (sin);    // sine
MAKE_FUNC_DEFINITION_0 (cos);    // cosine
MAKE_FUNC_DEFINITION_0 (tan);    // tangent
MAKE_FUNC_DEFINITION_0 (sinh);   // sine hyperbolicus
MAKE_FUNC_DEFINITION_0 (cosh);   // cosine hyperbolicus
MAKE_FUNC_DEFINITION_0 (tanh);   // tangent hyperbolicus
MAKE_FUNC_DEFINITION_0 (coth);   // cotangent hyperbolicus
MAKE_FUNC_DEFINITION_0 (sech);   // secans hyperbolicus
MAKE_FUNC_DEFINITION_0 (cosech); // cosecans hyperbolicus
MAKE_FUNC_DEFINITION_0 (sign);   // signum function
MAKE_FUNC_DEFINITION_0 (sinc);   // sin(x)/x aka sinc function

MAKE_FUNC_DEFINITION_1 (real);   // real value
MAKE_FUNC_DEFINITION_1 (imag);   // imaginary value
MAKE_FUNC_DEFINITION_1 (abs);    // absolute value

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

constant * evaluate::plus_m (constant * args) {
  matrix *    m1 = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1);
  return res;
}

constant * evaluate::plus_mv (constant * args) {
  matvec *    v1 = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1);
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
  res->v = new vector (*v1 + d2);
  return res;
}

constant * evaluate::plus_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (d1 + *v2);
  return res;
}

constant * evaluate::plus_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 + *c2);
  return res;
}

constant * evaluate::plus_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v2 + *c1);
  return res;
}

constant * evaluate::plus_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 + *v2);
  return res;
}

constant * evaluate::plus_m_m (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 + *m2);
  return res;
}

constant * evaluate::plus_mv_mv (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 + *v2);
  return res;
}

constant * evaluate::plus_mv_m (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 + *m2);
  return res;
}

constant * evaluate::plus_m_mv (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*m1 + *v2);
  return res;
}

constant * evaluate::plus_s_s (constant * args) {
  char * s1 = STR (args->getResult (0));
  char * s2 = STR (args->getResult (1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + strlen (s2) + 1);
  strcpy (p, s1); strcat (p, s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_c_s (constant * args) {
  char   c1 = CHR (args->getResult (0));
  char * s2 = STR (args->getResult (1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s2) + 2);
  p[0] = c1; strcpy (&p[1], s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_s_c (constant * args) {
  char * s1 = STR (args->getResult (0));
  char   c2 = CHR (args->getResult (1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + 2);
  strcpy (p, s1); p[strlen (s1)] = c2; p[strlen (s1) + 1] = '\0';
  res->s = p;
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

constant * evaluate::minus_m (constant * args) {
  matrix *    m1 = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (-*m1);
  return res;
}

constant * evaluate::minus_mv (constant * args) {
  matvec *    v1 = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (-*v1);
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
  res->v = new vector (*v1 - d2);
  return res;
}

constant * evaluate::minus_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (d1 - *v2);
  return res;
}

constant * evaluate::minus_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 - *c2);
  return res;
}

constant * evaluate::minus_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*c1 - *v2);
  return res;
}

constant * evaluate::minus_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 - *v2);
  return res;
}

constant * evaluate::minus_m_m (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 - *m2);
  return res;
}

constant * evaluate::minus_mv_mv (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 - *v2);
  return res;
}

constant * evaluate::minus_mv_m (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 - *m2);
  return res;
}

constant * evaluate::minus_m_mv (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*m1 - *v2);
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
  res->v = new vector (*v1 * d2);
  return res;
}

constant * evaluate::times_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (d1 * *v2);
  return res;
}

constant * evaluate::times_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 * *c2);
  return res;
}

constant * evaluate::times_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*c1 * *v2);
  return res;
}

constant * evaluate::times_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 * *v2);
  return res;
}

constant * evaluate::times_m_m (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  if (m1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->m = new matrix (m1->getRows (), m2->getCols ());
  } else {
    res->m = new matrix (*m1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_c (constant * args) {
  matrix  * m1 = M (args->getResult (0));
  complex * c2 = C (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 * *c2);
  return res;
}

constant * evaluate::times_c_m (constant * args) {
  complex * c1 = C (args->getResult (0));
  matrix  * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*c1 * *m2);
  return res;
}

constant * evaluate::times_m_d (constant * args) {
  matrix    * m1 = M (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 * d2);
  return res;
}

constant * evaluate::times_d_m (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  matrix    * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (d1 * *m2);
  return res;
}

constant * evaluate::times_mv_mv (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  if (v1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_c (constant * args) {
  matvec  * v1 = MV (args->getResult (0));
  complex * c2 = C (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 * *c2);
  return res;
}

constant * evaluate::times_c_mv (constant * args) {
  complex * c1 = C (args->getResult (0));
  matvec  * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*c1 * *v2);
  return res;
}

constant * evaluate::times_mv_d (constant * args) {
  matvec    * v1 = MV (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 * d2);
  return res;
}

constant * evaluate::times_d_mv (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  matvec    * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (d1 * *v2);
  return res;
}

constant * evaluate::times_mv_m (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  matrix * m2 = M (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  if (v1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), m2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_mv (constant * args) {
  matrix * m1 = M (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  if (m1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v2->getSize (), m1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*m1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_v (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 * *v2);
  return res;
}

constant * evaluate::times_v_mv (constant * args) {
  vector * v1 = V (args->getResult (0));
  matvec * v2 = MV (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 * *v2);
  return res;
}

// ****************** division *************************
constant * evaluate::over_d_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_DOUBLE);
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  res->d = d1 / d2;
  return res;
}

constant * evaluate::over_c_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 / *c2);
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  return res;
}

constant * evaluate::over_c_d (constant * args) {
  complex *   c1 = C (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 / d2);
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  return res;
}

constant * evaluate::over_d_c (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 / *c2);
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  return res;
}

constant * evaluate::over_v_d (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 / d2);
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  return res;
}

constant * evaluate::over_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (d1 / *v2);
  return res;
}

constant * evaluate::over_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 / *c2);
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  return res;
}

constant * evaluate::over_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*c1 / *v2);
  return res;
}

constant * evaluate::over_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 / *v2);
  return res;
}

constant * evaluate::over_m_c (constant * args) {
  matrix  * m1 = M (args->getResult (0));
  complex * c2 = C (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 / *c2);
  return res;
}

constant * evaluate::over_m_d (constant * args) {
  matrix    * m1 = M (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (*m1 / d2);
  return res;
}

constant * evaluate::over_mv_c (constant * args) {
  matvec  * v1 = MV (args->getResult (0));
  complex * c2 = C (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 / *c2);
  return res;
}

constant * evaluate::over_mv_d (constant * args) {
  matvec    * v1 = MV (args->getResult (0));
  nr_double_t d2 = D (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 / d2);
  return res;
}

constant * evaluate::over_mv_v (constant * args) {
  matvec * v1 = MV (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*v1 / *v2);
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
  res->v = new vector (*v1 % d2);
  return res;
}

constant * evaluate::modulo_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (d1 % *v2);
  return res;
}

constant * evaluate::modulo_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 % *c2);
  return res;
}

constant * evaluate::modulo_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*c1 % *v2);
  return res;
}

constant * evaluate::modulo_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v1 % *v2);
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
  res->v = new vector (pow (*v1, d2));
  return res;
}

constant * evaluate::power_d_v (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  vector * v2    = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (pow (d1, *v2));
  return res;
}

constant * evaluate::power_v_c (constant * args) {
  vector *    v1 = V (args->getResult (0));
  complex *   c2 = C (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (pow (*v1, *c2));
  return res;
}

constant * evaluate::power_c_v (constant * args) {
  complex *   c1 = C (args->getResult (0));
  vector *    v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (pow (*c1, *v2));
  return res;
}

constant * evaluate::power_v_v (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (pow (*v1, *v2));
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
  res->v = new vector (conj (*v1));
  return res;
}

constant * evaluate::conj_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (conj (*m));
  return res;
}

constant * evaluate::conj_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (conj (*mv));
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
  res->v = new vector (norm (*v1));
  return res;
}

// ********** phase in degrees *****************
constant * evaluate::phase_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 >= 0.0 ? 0.0 : 180.0;
  return res;
}

constant * evaluate::phase_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = deg (arg (*c1));
  return res;
}

constant * evaluate::phase_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (deg (arg (*v1)));
  return res;
}

constant * evaluate::phase_m (constant * args) {
  matrix *    m1 = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (deg (arg (*m1)));
  return res;
}

constant * evaluate::phase_mv (constant * args) {
  matvec *    v1 = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (deg (arg (*v1)));
  return res;
}

// ********** phase in radians *****************
constant * evaluate::arg_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 >= 0.0 ? 0.0 : M_PI;
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
  res->v = new vector (arg (*v1));
  return res;
}

constant * evaluate::arg_m (constant * args) {
  matrix *    m1 = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (arg (*m1));
  return res;
}

constant * evaluate::arg_mv (constant * args) {
  matvec *    v1 = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (arg (*v1));
  return res;
}

// ******* unwrap phase in radians ************
constant * evaluate::unwrap_v_1 (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (unwrap (*v1));
  return res;
}

constant * evaluate::unwrap_v_2 (constant * args) {
  vector *    v1 = V (args->getResult (0));
  nr_double_t d1 = D (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (unwrap (*v1, fabs (d1)));
  return res;
}

// ******** radian/degree conversion **********
constant * evaluate::deg2rad_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = rad (d1);
  return res;
}

constant * evaluate::deg2rad_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->c = new complex (rad (real (*c1)));
  return res;
}

constant * evaluate::deg2rad_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  vector *     v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) v->add (rad (real (v1->get (i))));
  res->v = v;
  return res;
}

constant * evaluate::rad2deg_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = deg (d1);
  return res;
}

constant * evaluate::rad2deg_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->c = new complex (deg (real (*c1)));
  return res;
}

constant * evaluate::rad2deg_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  vector *     v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) v->add (deg (real (v1->get (i))));
  res->v = v;
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
  res->v = new vector (dB (*v1));
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
  res->v = new vector (sqrt (*v1));
  return res;
}

// ********** natural logarithm *****************
constant * evaluate::ln_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if (d1 >= 0.0) res->c = new complex (log (d1));
  else res->c = new complex (log (-d1), M_PI);
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
  res->v = new vector (ln (*v1));
  return res;
}

// ********** decimal logarithm *****************
constant * evaluate::log10_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if (d1 >= 0.0) res->c = new complex (log10 (d1));
  else res->c = new complex (log10 (-d1), M_PI * M_LOG10E);
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
  res->v = new vector (log10 (*v1));
  return res;
}

// ********** binary logarithm *****************
constant * evaluate::log2_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  if (d1 >= 0.0) res->c = new complex (log (d1) * M_LOG2E);
  else res->c = new complex (log (-d1) * M_LOG2E , M_PI * M_LOG2E);
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
  res->v = new vector (log2 (*v1));
  return res;
}

// ************* arcus sine *********************
constant * evaluate::arcsin_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = atan (d1 / sqrt (1.0 - d1 * d1));
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
  res->v = new vector (arcsin (*v1));
  return res;
}

// ************* arcus cosine ******************
constant * evaluate::arccos_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = M_PI_2 - atan (d1 / sqrt (1.0 - d1 * d1));
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
  res->v = new vector (arccos (*v1));
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
  res->v = new vector (arctan (*v1));
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
  res->v = new vector (cot (*v1));
  return res;
}

// ************ arcus cotangent *****************
constant * evaluate::arccot_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = M_PI_2 - atan (d1);
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
  res->v = new vector (arccot (*v1));
  return res;
}

// ***************** secans *********************
constant * evaluate::sec_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 1.0 / cos (d1);
  return res;
}

constant * evaluate::sec_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (1.0 / cos (*c1));
  return res;
}

constant * evaluate::sec_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (1.0 / cos (*v1));
  return res;
}

// ***************** cosecans *********************
constant * evaluate::cosec_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = 1.0 / sin (d1);
  return res;
}

constant * evaluate::cosec_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (1.0 / sin (*c1));
  return res;
}

constant * evaluate::cosec_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (1.0 / sin (*v1));
  return res;
}

// ********** area sine hyperbolicus **************
constant * evaluate::arsinh_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = log (d1 + sqrt (d1 * d1 + 1));
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
  res->v = new vector (arsinh (*v1));
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
  res->v = new vector (arcosh (*v1));
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
  res->v = new vector (artanh (*v1));
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
  res->v = new vector (arcoth (*v1));
  return res;
}

// This is the rtoz, ztor, ytor, rtoy helper macro.
#define MAKE_FUNC_DEFINITION_2(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) {   \
  nr_double_t d = D (args->getResult (0));			    \
  constant * res = new constant (TAG_DOUBLE);			    \
  res->d = real (cfunc (rect (d, 0))); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_d) (constant * args) { \
  nr_double_t d = D (args->getResult (0));			    \
  nr_double_t z = D (args->getResult (1));			    \
  constant * res = new constant (TAG_DOUBLE);			    \
  res->d = real (cfunc (rect (d, 0), z)); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_c) (constant * args) { \
  nr_double_t d = D (args->getResult (0));			    \
  complex * z = C (args->getResult (1));			    \
  constant * res = new constant (TAG_COMPLEX);			    \
  res->c = new complex (cfunc (rect (d, 0), *z)); return res;	    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) {   \
  complex * c = C (args->getResult (0));			    \
  constant * res = new constant (TAG_COMPLEX);			    \
  res->c = new complex (cfunc (*c)); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_d) (constant * args) { \
  complex * c = C (args->getResult (0));			    \
  nr_double_t z = D (args->getResult (1));			    \
  constant * res = new constant (TAG_COMPLEX);			    \
  res->c = new complex (cfunc (*c, z)); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_c) (constant * args) { \
  complex * c = C (args->getResult (0));			    \
  complex * z = C (args->getResult (1));			    \
  constant * res = new constant (TAG_COMPLEX);			    \
  res->c = new complex (cfunc (*c, *z)); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) {   \
  vector * v = V (args->getResult (0));				    \
  constant * res = new constant (TAG_VECTOR);			    \
  res->v = new vector (cfunc (*v)); return res;			    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_d) (constant * args) { \
  vector * v = V (args->getResult (0));				    \
  nr_double_t z = D (args->getResult (1));			    \
  constant * res = new constant (TAG_VECTOR);			    \
  res->v = new vector (cfunc (*v, z)); return res;		    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_c) (constant * args) { \
  vector * v = V (args->getResult (0));				    \
  complex * z = C (args->getResult (1));			    \
  constant * res = new constant (TAG_VECTOR);			    \
  res->v = new vector (cfunc (*v, *z)); return res;		    \
}								    \

MAKE_FUNC_DEFINITION_2 (ztor);
MAKE_FUNC_DEFINITION_2 (rtoz);
MAKE_FUNC_DEFINITION_2 (ytor);
MAKE_FUNC_DEFINITION_2 (rtoy);

// ** convert reflexion coefficient to standing wave ratio **
constant * evaluate::rtoswr_d (constant * args) {
  nr_double_t d1 = D (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = (1 + fabs (d1)) / (1 - fabs (d1));
  return res;
}

constant * evaluate::rtoswr_c (constant * args) {
  complex *   c1 = C (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = (1 + abs (*c1)) / (1 - abs (*c1));
  return res;
}

constant * evaluate::rtoswr_v (constant * args) {
  vector *    v1 = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (v1->getSize ());
  for (int i = 0; i < v1->getSize (); i++)
    res->v->set ((1 + abs (v1->get (i))) / (1 - abs (v1->get (i))), i);
  return res;
}

// ** differentiate vector with respect to another vector **
constant * evaluate::diff_v_2 (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (diff (*v1, *v2));
  return res;
}

constant * evaluate::diff_v_3 (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  int      i3 = INT (args->getResult (2));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (diff (*v1, *v2, i3));
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
  if (fabs (arg (*c1)) < M_PI_2) res->c = new complex (abs (*c1));
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
  if (fabs (arg (*c1)) < M_PI_2) res->c = new complex (abs (*c1));
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

// ***************** array indices *****************
constant * evaluate::index_mv_2 (constant * args) {
  matvec * mv = MV (args->getResult (0));
  int r = INT (args->getResult (1));
  int c = INT (args->getResult (2));
  constant * res = new constant (TAG_VECTOR);
  if (r < 1 || r > mv->getRows () || c < 1 || c > mv->getCols ()) {
    char txt[256];
    sprintf (txt, "matvec indices [%d,%d] out of bounds [1-%d,1-%d]",
	     r, c, mv->getRows (), mv->getCols ());
    THROW_MATH_EXCEPTION (txt);
  } else {
    res->v = new vector (mv->get (r - 1, c - 1));
  }
  return res;
}

constant * evaluate::index_mv_1 (constant * args) {
  matvec * mv = MV (args->getResult (0));
  int i = INT (args->getResult (1));
  constant * res = new constant (TAG_MATRIX);
  if (i < 1 || i > mv->getSize ()) {
    char txt[256];
    sprintf (txt, "matvec index [%d] out of bounds [1-%d]", i, mv->getSize ());
    THROW_MATH_EXCEPTION (txt);
  } else {
    res->m = new matrix (mv->get (i - 1));
  }
  return res;
}

/* Little helper macros for the following functionality. */
#define EQUATION_HAS_DEPS(v,n) \
  ((v)->getDataDependencies() != NULL && \
   (v)->getDataDependencies()->length() >= n)
#define EQUATION_DEPS(v) \
  ((v)->getDataDependencies() ? (v)->getDataDependencies()->length() : 1)

/* This following function is used to get a subset of data entries
   from a data vector with certain data dependencies. */
void evaluate::extract_vector (constant * args, int idx, int &skip, int &size,
			       constant * res) {
  vector * v = V (args->getResult (0));
  int i = INT (args->getResult (idx));
  int type = args->get(idx)->getType ();
  vector * vres;
  strlist * deps = args->getResult(0)->getDataDependencies ();
  int didx = (deps ? deps->length () : 0) - idx;
  int dsize = solver::getDependencySize (deps, idx);

  // all of the data vector
  if (type == TAG_RANGE) {
    vres = new vector (*(res->v));
    skip *= deps ? solver::getDataSize (deps->get (didx - 1)) : 1;
    size *= deps ? solver::getDataSize (deps->get (didx)) : 1;
  }
  // a subset
  else {
    vres = new vector (dsize * size);
    int len = deps ? solver::getDataSize (deps->get (didx)) : v->getSize ();
    if (i < 0 || i >= len) {
      char txt[256];
      sprintf (txt, "vector index %d (%d) out of bounds [%d,%d]",
	       idx, i, 0, len - 1);
      THROW_MATH_EXCEPTION (txt);
    } else {
      int k, n;
      for (n = k = 0; k < dsize * size; n += skip, k++) {
	vres->set (res->v->get (dsize * i + n), k);
      }
    }
    if (deps && didx >= 0) {
      res->addDropDependencies (deps->get (didx));
    }
  }
  if (res->v != NULL) delete res->v;
  res->v = vres;
}

constant * evaluate::index_v_1 (constant * args) {
  vector * v = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  int skip = 1, size = 1;
  res->v = new vector (*v);
  extract_vector (args, 1, skip, size, res);
  return res;
}

constant * evaluate::index_v_2 (constant * args) {
  vector * v = V (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  int skip = 1, size = 1;
  res->v = new vector (*v);
  if (!EQUATION_HAS_DEPS (args->getResult (0), 2)) {
    char txt[256];
    sprintf (txt, "invalid number of vector indices (%d > %d)", 2,
	     EQUATION_DEPS (args->getResult (0)));
    THROW_MATH_EXCEPTION (txt);
    return res;
  }
  extract_vector (args, 1, skip, size, res);
  extract_vector (args, 2, skip, size, res);
  return res;
}

constant * evaluate::index_m_2 (constant * args) {
  matrix * m = M (args->getResult (0));
  int r = INT (args->getResult (1));
  int c = INT (args->getResult (2));
  constant * res = new constant (TAG_COMPLEX);
  if (r < 1 || r > m->getRows () || c < 1 || c > m->getCols ()) {
    char txt[256];
    sprintf (txt, "matrix indices [%d,%d] out of bounds [1-%d,1-%d]",
	     r, c, m->getRows (), m->getCols ());
    THROW_MATH_EXCEPTION (txt);
  } else {
    res->c = new complex (m->get (r - 1, c - 1));
  }
  return res;
}

constant * evaluate::index_s_1 (constant * args) {
  char * s = STR (args->getResult (0));
  int i = INT (args->getResult (1));
  constant * res = new constant (TAG_CHAR);
  res->chr = (i >= 0 && i < (int) strlen (s)) ? s[i] : ' ';
  return res;
}

// Interpolator helper macro without number of points given.
#define INTERPOL_HELPER()                     \
  constant * arg = new constant (TAG_DOUBLE); \
  arg->d = 64;                                \
  arg->solvee = args->getResult(0)->solvee;   \
  arg->evaluate ();                           \
  args->append (arg);

// ***************** interpolation *****************
constant * evaluate::interpolate_v_v (constant * args) {
  INTERPOL_HELPER();
  return interpolate_v_v_d (args);
}

constant * evaluate::interpolate_v_v_d (constant * args) {
  vector * v1 = V (args->getResult (0));
  vector * v2 = V (args->getResult (1));
  int n  = INT (args->getResult (2));
  constant * res = new constant (TAG_VECTOR);
  if (v1->getSize () < 3) {
    THROW_MATH_EXCEPTION ("interpolate: number of datapoints must be greater "
			  "than 2");
    res->v = new vector ();
    return res;
  }
  nr_double_t last  = real (v2->get (v2->getSize () - 1));
  nr_double_t first = real (v2->get (0));
  constant * arg = new constant (TAG_VECTOR);
  arg->v = new vector (::linspace (first, last, n));
  arg->solvee = args->getResult(0)->solvee;
  arg->evaluate ();
  vector * val = new vector (n);
  spline spl (SPLINE_BC_NATURAL);
  spl.vectors (*v1, *v2);
  spl.construct ();
  for (int k = 0; k < arg->v->getSize (); k++) {
    val->set (spl.evaluate (real (arg->v->get (k))).f0, k);
  }
  res->v = val;
  node * gen =
    args->get(0)->solvee->addGeneratedEquation (arg->v, "Interpolate");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  return res;
}

// ***************** fourier transformations *****************
#define FOURIER_HELPER(cfunc,dep) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_v) (constant * args) {	 \
  vector * v = V (args->getResult (0));					 \
  vector * t = V (args->getResult (1));					 \
  constant * res = new constant (TAG_VECTOR);				 \
  vector * val = new vector (QUCS_CONCAT2 (cfunc,_1d) (*v));		 \
  res->v = val;								 \
  int n = t->getSize ();						 \
  nr_double_t last  = real (t->get (n - 1));				 \
  nr_double_t first = real (t->get (0));				 \
  nr_double_t delta = (last - first) / (n - 1);				 \
  n = val->getSize ();							 \
  constant * arg = new constant (TAG_VECTOR);				 \
  arg->v = new vector (::linspace (0, 1.0 / delta, n));			 \
  arg->solvee = args->getResult(0)->solvee;				 \
  arg->evaluate ();							 \
  node * gen = args->get(0)->solvee->addGeneratedEquation (arg->v, dep); \
  res->addPrepDependencies (A(gen)->result);				 \
  res->dropdeps = 1;							 \
  args->append (arg);							 \
  return res;								 \
}

FOURIER_HELPER (fft, "Frequency");
FOURIER_HELPER (ifft,"Time");
FOURIER_HELPER (dft, "Frequency");
FOURIER_HELPER (idft,"Time");

// This is the stoz, ztos, ytos, stoy helper macro.
#define MAKE_FUNC_DEFINITION_3(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m) (constant * args) {    \
  matrix * m = M (args->getResult (0));				     \
  constant * res = new constant (TAG_MATRIX);			     \
  res->m = new matrix (cfunc (*m)); return res;			     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_d) (constant * args) {  \
  matrix * m = M (args->getResult (0));				     \
  nr_double_t z = D (args->getResult (1));			     \
  constant * res = new constant (TAG_MATRIX);			     \
  res->m = new matrix (cfunc (*m, rect (z, 0))); return res;	     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_c) (constant * args) {  \
  matrix * m = M (args->getResult (0));				     \
  complex * z = C (args->getResult (1));			     \
  constant * res = new constant (TAG_MATRIX);			     \
  res->m = new matrix (cfunc (*m, *z)); return res;		     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m_v) (constant * args) {  \
  matrix * m = M (args->getResult (0));				     \
  vector * z = V (args->getResult (1));				     \
  constant * res = new constant (TAG_MATRIX);			     \
  res->m = new matrix (cfunc (*m, *z)); return res;		     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv) (constant * args) {   \
  matvec * m = MV (args->getResult (0));			     \
  constant * res = new constant (TAG_MATVEC);			     \
  res->mv = new matvec (cfunc (*m)); return res;		     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_d) (constant * args) { \
  matvec * m = MV (args->getResult (0));			     \
  nr_double_t z = D (args->getResult (1));			     \
  constant * res = new constant (TAG_MATVEC);			     \
  res->mv = new matvec (cfunc (*m, rect (z, 0))); return res;	     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_c) (constant * args) { \
  matvec * m = MV (args->getResult (0));			     \
  complex * z = C (args->getResult (1));			     \
  constant * res = new constant (TAG_MATVEC);			     \
  res->mv = new matvec (cfunc (*m, *z)); return res;		     \
}								     \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv_v) (constant * args) { \
  matvec * m = MV (args->getResult (0));			     \
  vector * z = V (args->getResult (1));				     \
  constant * res = new constant (TAG_MATVEC);			     \
  res->mv = new matvec (cfunc (*m, *z)); return res;		     \
}								     \

MAKE_FUNC_DEFINITION_3 (stoy);
MAKE_FUNC_DEFINITION_3 (ytos);
MAKE_FUNC_DEFINITION_3 (stoz);
MAKE_FUNC_DEFINITION_3 (ztos);

// ***************** matrix conversions *****************
constant * evaluate::ytoz_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (ytoz (*m));
}

constant * evaluate::ytoz_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (ytoz (*mv));
}

constant * evaluate::ztoy_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (ztoy (*m));
}

constant * evaluate::ztoy_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (ztoy (*mv));
}

// These are stos() helpers.
#define _CHKM(m) /* check square matrix */              \
  if (m->getCols () != m->getRows ()) {                 \
    THROW_MATH_EXCEPTION ("stos: not a square matrix"); \
    res->m = new matrix (m->getRows (), m->getCols ()); \
    return res; }
#define _CHKMV(mv) /* check square matrix vector */                        \
  if (mv->getCols () != mv->getRows ()) {                                  \
    THROW_MATH_EXCEPTION ("stos: not a square matrix");                    \
    res->mv = new matvec (mv->getSize (), mv->getRows (), mv->getCols ()); \
    return res; }
#define _CHKMA(m,cond) \
  if (!(cond)) {                                            \
    THROW_MATH_EXCEPTION ("stos: nonconformant arguments"); \
    res->m = new matrix (m->getRows (), m->getCols ());     \
    return res; }
#define _CHKMVA(mv,cond) \
  if (!(cond)) {                                                           \
    THROW_MATH_EXCEPTION ("stos: nonconformant arguments");                \
    res->mv = new matvec (mv->getSize (), mv->getRows (), mv->getCols ()); \
    return res; }

// Function -- MATRIX stos (MATRIX, DOUBLE)
constant * evaluate::stos_m_d (constant * args) {
  _ARM0 (m); _ARD1 (zref); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, zref));
}

// Function -- MATRIX stos (MATRIX, COMPLEX)
constant * evaluate::stos_m_c (constant * args) {
  _ARM0 (m); _ARC1 (zref); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, DOUBLE)
constant * evaluate::stos_m_d_d (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, zref, z0));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, COMPLEX)
constant * evaluate::stos_m_d_c (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, rect (zref, 0), *z0));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, DOUBLE)
constant * evaluate::stos_m_c_d (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref, rect (z0, 0)));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, COMPLEX)
constant * evaluate::stos_m_c_c (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m);
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, VECTOR)
constant * evaluate::stos_m_v (constant * args) {
  _ARM0 (m); _ARV1 (zref); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref));
}

// Function -- MATRIX stos (MATRIX, VECTOR, DOUBLE)
constant * evaluate::stos_m_v_d (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARD2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, rect (z0, 0)));
}

// Function -- MATRIX stos (MATRIX, VECTOR, COMPLEX)
constant * evaluate::stos_m_v_c (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARC2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, DOUBLE, VECTOR)
constant * evaluate::stos_m_d_v (constant * args) {
  _ARM0 (m); _ARD1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize ());
  _RETM (stos (*m, rect (zref, 0), *z0));
}

// Function -- MATRIX stos (MATRIX, COMPLEX, VECTOR)
constant * evaluate::stos_m_c_v (constant * args) {
  _ARM0 (m); _ARC1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATRIX stos (MATRIX, VECTOR, VECTOR)
constant * evaluate::stos_m_v_v (constant * args) {
  _ARM0 (m); _ARV1 (zref); _ARV2 (z0); _DEFM ();
  _CHKM (m); _CHKMA (m, m->getRows () == z0->getSize () &&
		     m->getRows () == zref->getSize ());
  _RETM (stos (*m, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE)
constant * evaluate::stos_mv_d (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, zref));
}

// Function -- MATVEC stos (MATVEC, COMPLEX)
constant * evaluate::stos_mv_c (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, DOUBLE)
constant * evaluate::stos_mv_d_d (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, zref, z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, COMPLEX)
constant * evaluate::stos_mv_d_c (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, rect (zref, 0), *z0));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, DOUBLE)
constant * evaluate::stos_mv_c_d (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref, rect (z0, 0)));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, COMPLEX)
constant * evaluate::stos_mv_c_c (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv);
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, VECTOR)
constant * evaluate::stos_mv_v (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref));
}

// Function -- MATVEC stos (MATVEC, VECTOR, DOUBLE)
constant * evaluate::stos_mv_v_d (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARD2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, rect (z0, 0)));
}

// Function -- MATVEC stos (MATVEC, VECTOR, COMPLEX)
constant * evaluate::stos_mv_v_c (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARC2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, DOUBLE, VECTOR)
constant * evaluate::stos_mv_d_v (constant * args) {
  _ARMV0 (mv); _ARD1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize ());
  _RETMV (stos (*mv, rect (zref, 0), *z0));
}

// Function -- MATVEC stos (MATVEC, COMPLEX, VECTOR)
constant * evaluate::stos_mv_c_v (constant * args) {
  _ARMV0 (mv); _ARC1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

// Function -- MATVEC stos (MATVEC, VECTOR, VECTOR)
constant * evaluate::stos_mv_v_v (constant * args) {
  _ARMV0 (mv); _ARV1 (zref); _ARV2 (z0); _DEFMV ();
  _CHKMV (mv); _CHKMVA (mv, mv->getRows () == z0->getSize () &&
			mv->getRows () == zref->getSize ());
  _RETMV (stos (*mv, *zref, *z0));
}

constant * evaluate::twoport_m (constant * args) {
  matrix * m = M (args->getResult (0));
  char f = CHR (args->getResult (1));
  char t = CHR (args->getResult (2));
  constant * res = new constant (TAG_MATRIX);
  if (m->getRows () < 2 || m->getCols () < 2) {
    THROW_MATH_EXCEPTION ("invalid matrix dimensions for twoport "
			  "transformation");
    res->m = new matrix (*m);
  }
  else
    res->m = new matrix (twoport (*m, toupper (f), toupper (t)));
  return res;
}

constant * evaluate::twoport_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  char f = CHR (args->getResult (1));
  char t = CHR (args->getResult (2));
  constant * res = new constant (TAG_MATVEC);
  if (mv->getRows () < 2 || mv->getCols () < 2) {
    THROW_MATH_EXCEPTION ("invalid matrix dimensions for twoport "
			  "transformation");
    res->mv = new matvec (*mv);
  }
  else
    res->mv = new matvec (twoport (*mv, toupper (f), toupper (t)));
  return res;
}

constant * evaluate::inverse_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (inverse (*m));
  return res;
}

constant * evaluate::inverse_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (inverse (*mv));
  return res;
}

constant * evaluate::transpose_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (transpose (*m));
  return res;
}

constant * evaluate::transpose_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (transpose (*mv));
  return res;
}

constant * evaluate::det_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (det (*m));
  return res;
}

constant * evaluate::det_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (det (*mv));
  return res;
}

constant * evaluate::eye_m (constant * args) {
  int i = INT (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (eye (i));
  return res;
}

constant * evaluate::adjoint_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (adjoint (*m));
  return res;
}

constant * evaluate::adjoint_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (adjoint (*mv));
  return res;
}

// ***************** s-parameter stability factors *****************
constant * evaluate::rollet_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  res->d = rollet (*m);
  return res;
}

constant * evaluate::rollet_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (rollet (*mv));
  return res;
}

constant * evaluate::mu1_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  nr_double_t k;
  k = (1 - norm (m->get (0, 0))) /
    (abs (m->get (1, 1) - conj (m->get (0, 0)) * det (*m)) +
     abs (m->get (0, 1) * m->get (1, 0)));
  res->d = k;
  return res;
}

constant * evaluate::mu1_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  vector k;
  k = (1 - norm (mv->get (0, 0))) /
    (abs (mv->get (1, 1) - conj (mv->get (0, 0)) * det (*mv)) +
     abs (mv->get (0, 1) * mv->get (1, 0)));
  res->v = new vector (k);
  return res;
}

constant * evaluate::mu2_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_DOUBLE);
  nr_double_t k;
  k = (1 - norm (m->get (1, 1))) /
    (abs (m->get (0, 0) - conj (m->get (1, 1)) * det (*m)) +
     abs (m->get (0, 1) * m->get (1, 0)));
  res->d = k;
  return res;
}

constant * evaluate::mu2_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_VECTOR);
  vector k;
  k = (1 - norm (mv->get (1, 1))) /
    (abs (mv->get (0, 0) - conj (mv->get (1, 1)) * det (*mv)) +
     abs (mv->get (0, 1) * mv->get (1, 0)));
  res->v = new vector (k);
  return res;
}


// ***************** vector creation *****************
constant * evaluate::linspace (constant * args) {
  nr_double_t start = D (args->getResult (0));
  nr_double_t stop = D (args->getResult (1));
  int points = INT (args->getResult (2));
  constant * res = new constant (TAG_VECTOR);
  if (points < 2) {
    THROW_MATH_EXCEPTION ("linspace: number of points must be greater than 1");
    res->v = new vector ();
    return res;
  }
  res->v = new vector (::linspace (start, stop, points));
  return res;
}

constant * evaluate::logspace (constant * args) {
  nr_double_t start = D (args->getResult (0));
  nr_double_t stop = D (args->getResult (1));
  int points = INT (args->getResult (2));
  constant * res = new constant (TAG_VECTOR);
  if (points < 2) {
    THROW_MATH_EXCEPTION ("logspace: number of points must be greater than 1");
    res->v = new vector ();
    return res;
  }
  if (start * stop <= 0) {
    THROW_MATH_EXCEPTION ("logspace: invalid start/stop values");
    res->v = new vector (points);
    return res;
  }
  res->v = new vector (::logspace (start, stop, points));
  return res;
}

// Circle helper macro with a number of points given.
#define CIRCLE_HELPER_D(argi)                   \
  int n = INT (args->getResult (argi));         \
  if (n < 2) {                                  \
    THROW_MATH_EXCEPTION ("Circle: number of points must be greater than 1"); \
    constant * res = new constant (TAG_VECTOR); \
    res->v = new vector ();                     \
    return res;                                 \
  }                                             \
  constant * arg = new constant (TAG_VECTOR);   \
  arg->v = new vector (::linspace (0, 360, n)); \
  arg->solvee = args->getResult(0)->solvee;     \
  arg->evaluate ();                             \
  delete args->get(argi);                       \
  args->get((argi)-1)->setNext (NULL);          \
  args->append (arg);

// Circle helper macro with no additional argument given.
#define CIRCLE_HELPER_A()                        \
  constant * arg = new constant (TAG_VECTOR);    \
  arg->v = new vector (::linspace (0, 360, 64)); \
  arg->solvee = args->getResult(0)->solvee;      \
  arg->evaluate ();                              \
  args->append (arg);

// ***************** s-parameter noise circles *****************
constant * evaluate::noise_circle_d_v (constant * args) {
  vector * Sopt = V (args->getResult (0));
  vector * Fmin = V (args->getResult (1));
  vector * Rn   = V (args->getResult (2));
  nr_double_t F = D (args->getResult (3));
  vector * arc  = V (args->getResult (4));

  constant * res = new constant (TAG_VECTOR);
  vector N = circuit::z0 / 4 / *Rn * (F - *Fmin) * norm (1 + *Sopt);
  vector R = sqrt (N * N + N * (1 - norm (*Sopt))) / (1 + N);
  vector C = *Sopt / (1 + N);
  vector * circle = new vector (C.getSize () * arc->getSize ());
  int i, a, j; complex v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = args->get(4)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::noise_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (4);
  return noise_circle_d_v (args);
}

constant * evaluate::noise_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return noise_circle_d_v (args);
}

constant * evaluate::noise_circle_v_v (constant * args) {
  vector * Sopt = V (args->getResult (0));
  vector * Fmin = V (args->getResult (1));
  vector * Rn   = V (args->getResult (2));
  vector * F    = V (args->getResult (3));
  vector * arc  = V (args->getResult (4));

  constant * res = new constant (TAG_VECTOR);
  vector * circle =
    new vector (Sopt->getSize () * arc->getSize () * F->getSize ());
  int i, a, j, f; complex v; vector N, R, C;
  for (f = 0; f < F->getSize (); f++) {
    N = circuit::z0 / 4 / *Rn * (F->get (f) - *Fmin) * norm (1 + *Sopt);
    R = sqrt (N * N + N * (1 - norm (*Sopt))) / (1 + N);
    C = *Sopt / (1 + N);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
	v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
	j = i * F->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = args->get(3)->solvee->addGeneratedEquation (F, "NF");
  res->addPrepDependencies (A(gen)->result);
  gen = args->get(4)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);

  res->v = circle;
  return res;
}

constant * evaluate::noise_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (4);
  return noise_circle_v_v (args);
}

constant * evaluate::noise_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return noise_circle_v_v (args);
}

// ***************** s-parameter stability circles *****************
constant * evaluate::stab_circle_l_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  vector * arc = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  vector D = norm (S->get (1, 1)) - norm (det (*S));
  vector C = (conj (S->get (1, 1)) - S->get (0, 0) * conj (det (*S))) / D;
  vector R = abs (S->get (0, 1)) * abs (S->get (1, 0)) / D;
  vector * circle = new vector (S->getSize () * arc->getSize ());
  int a, d, i; complex v;
  for (i = 0, d = 0; i < S->getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, d++) {
      v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
      circle->set (v, d);
    }
  }
  node * gen = args->get(1)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::stab_circle_l_d (constant * args) {
  CIRCLE_HELPER_D (1);
  return stab_circle_l_v (args);
}

constant * evaluate::stab_circle_l (constant * args) {
  CIRCLE_HELPER_A ();
  return stab_circle_l_v (args);
}

constant * evaluate::stab_circle_s_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  vector * arc = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  vector D = norm (S->get (0, 0)) - norm (det (*S));
  vector C = (conj (S->get (0, 0)) - S->get (1, 1) * conj (det (*S))) / D;
  vector R = abs (S->get (0, 1)) * abs (S->get (1, 0)) / D;
  vector * circle = new vector (S->getSize () * arc->getSize ());
  int a, d, i; complex v;
  for (i = 0, d = 0; i < S->getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, d++) {
      v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
      circle->set (v, d);
    }
  }
  node * gen = args->get(1)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::stab_circle_s_d (constant * args) {
  CIRCLE_HELPER_D (1);
  return stab_circle_s_v (args);
}

constant * evaluate::stab_circle_s (constant * args) {
  CIRCLE_HELPER_A ();
  return stab_circle_s_v (args);
}

// ***************** s-parameter gain circles *****************
constant * evaluate::ga_circle_d_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  nr_double_t G = D (args->getResult (1));
  vector * arc = V (args->getResult (2));

  constant * res = new constant (TAG_VECTOR);
  vector g, D, c, s, k, C, R, d;
  D = det (*S);
  c = S->get (0, 0) - conj (S->get (1, 1)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  g = G / norm (S->get (1, 0));
  d = 1 + g * (norm (S->get (0, 0)) - norm (D));
  C = g * conj (c) / d;
  R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);

  vector * circle = new vector (S->getSize () * arc->getSize ());
  int i, a, j; complex v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = args->get(2)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::ga_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return ga_circle_d_v (args);
}

constant * evaluate::ga_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return ga_circle_d_v (args);
}

constant * evaluate::ga_circle_v_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  vector * G = V (args->getResult (1));
  vector * arc = V (args->getResult (2));

  constant * res = new constant (TAG_VECTOR);
  vector * circle =
    new vector (S->getSize () * arc->getSize () * G->getSize ());
  int i, a, j, f; complex v; vector g, D, c, s, k, R, C, d;
  D = det (*S);
  c = S->get (0, 0) - conj (S->get (1, 1)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  for (f = 0; f < G->getSize (); f++) {
    g = G->get (f) / norm (S->get (1, 0));
    d = 1 + g * (norm (S->get (0, 0)) - norm (D));
    C = g * conj (c) / d;
    R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
	v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
	j = i * G->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = args->get(1)->solvee->addGeneratedEquation (G, "Ga");
  res->addPrepDependencies (A(gen)->result);
  gen = args->get(2)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::ga_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return ga_circle_v_v (args);
}

constant * evaluate::ga_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return ga_circle_v_v (args);
}

constant * evaluate::gp_circle_d_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  nr_double_t G = D (args->getResult (1));
  vector * arc = V (args->getResult (2));

  constant * res = new constant (TAG_VECTOR);
  vector g, D, c, s, k, C, R, d;
  D = det (*S);
  c = S->get (1, 1) - conj (S->get (0, 0)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  g = G / norm (S->get (1, 0));
  d = 1 + g * (norm (S->get (1, 1)) - norm (D));
  C = g * conj (c) / d;
  R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);

  vector * circle = new vector (S->getSize () * arc->getSize ());
  int i, a, j; complex v;
  for (i = 0, j = 0; i < C.getSize (); i++) {
    for (a = 0; a < arc->getSize (); a++, j++) {
      v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
      circle->set (v, j);
    }
  }

  node * gen = args->get(2)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::gp_circle_d_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return gp_circle_d_v (args);
}

constant * evaluate::gp_circle_d (constant * args) {
  CIRCLE_HELPER_A ();
  return gp_circle_d_v (args);
}

constant * evaluate::gp_circle_v_v (constant * args) {
  matvec * S = MV (args->getResult (0));
  vector * G = V (args->getResult (1));
  vector * arc = V (args->getResult (2));

  constant * res = new constant (TAG_VECTOR);
  vector * circle =
    new vector (S->getSize () * arc->getSize () * G->getSize ());
  int i, a, j, f; complex v; vector g, D, c, s, k, R, C, d;
  D = det (*S);
  c = S->get (1, 1) - conj (S->get (0, 0)) * D;
  k = rollet (*S);
  s = S->get (0, 1) * S->get (1, 0);
  for (f = 0; f < G->getSize (); f++) {
    g = G->get (f) / norm (S->get (1, 0));
    d = 1 + g * (norm (S->get (1, 1)) - norm (D));
    C = g * conj (c) / d;
    R = sqrt (1 - 2 * k * g * abs (s) + g * g * norm (s)) / abs (d);
    for (i = 0; i < C.getSize (); i++) {
      for (a = 0; a < arc->getSize (); a++) {
	v = C.get (i) + R.get (i) * exp (rect (0, 1) * rad (arc->get (a)));
	j = i * G->getSize () * arc->getSize () + f * arc->getSize () + a;
	circle->set (v, j);
      }
    }
  }

  node * gen;
  gen = args->get(1)->solvee->addGeneratedEquation (G, "Gp");
  res->addPrepDependencies (A(gen)->result);
  gen = args->get(2)->solvee->addGeneratedEquation (arc, "Arcs");
  res->addPrepDependencies (A(gen)->result);
  res->v = circle;
  return res;
}

constant * evaluate::gp_circle_v_d (constant * args) {
  CIRCLE_HELPER_D (2);
  return gp_circle_v_v (args);
}

constant * evaluate::gp_circle_v (constant * args) {
  CIRCLE_HELPER_A ();
  return gp_circle_v_v (args);
}

constant * evaluate::plot_vs_v (constant * args) {
  vector * v = V (args->getResult (0));
  vector * a = V (args->getResult (1));
  constant * res = new constant (TAG_VECTOR);
  res->v = new vector (*v);
  node * gen = args->get(1)->solvee->addGeneratedEquation (a, "Versus");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  return res;
}

constant * evaluate::plot_vs_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  vector * a = V (args->getResult (1));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (*mv);
  node * gen = args->get(1)->solvee->addGeneratedEquation (a, "Versus");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  return res;
}

// Include the application array.
#include "applications.h"
