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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: evaluate.cpp,v 1.31 2005/05/17 09:35:07 raimi Exp $
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
  res->v = new vector (ztor (*v1));
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
  res->v = new vector (rtoz (*v1));
  return res;
}

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

// ***************** matrix conversions *****************
constant * evaluate::stoy_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (stoy (*m));
  return res;
}

constant * evaluate::stoy_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (stoy (*mv));
  return res;
}

constant * evaluate::stoz_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (stoz (*m));
  return res;
}

constant * evaluate::stoz_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (stoz (*mv));
  return res;
}

constant * evaluate::ytos_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (ytos (*m));
  return res;
}

constant * evaluate::ytos_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (ytos (*mv));
  return res;
}

constant * evaluate::ytoz_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (ytoz (*m));
  return res;
}

constant * evaluate::ytoz_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (ytoz (*mv));
  return res;
}

constant * evaluate::ztos_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (ztos (*m));
  return res;
}

constant * evaluate::ztos_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (ztos (*mv));
  return res;
}

constant * evaluate::ztoy_m (constant * args) {
  matrix * m = M (args->getResult (0));
  constant * res = new constant (TAG_MATRIX);
  res->m = new matrix (ztoy (*m));
  return res;
}

constant * evaluate::ztoy_mv (constant * args) {
  matvec * mv = MV (args->getResult (0));
  constant * res = new constant (TAG_MATVEC);
  res->mv = new matvec (ztoy (*mv));
  return res;
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

// Array containing all kinds of applications.
struct application_t eqn::applications[] = {
  { "+", TAG_DOUBLE,  evaluate::plus_d, 1, { TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_c, 1, { TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_v, 1, { TAG_VECTOR  } },
  { "+", TAG_MATRIX,  evaluate::plus_m, 1, { TAG_MATRIX  } },
  { "+", TAG_MATVEC,  evaluate::plus_mv, 1, { TAG_MATVEC } },

  { "+", TAG_DOUBLE,  evaluate::plus_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "+", TAG_COMPLEX, evaluate::plus_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "+", TAG_VECTOR,  evaluate::plus_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "+", TAG_VECTOR,  evaluate::plus_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "+", TAG_VECTOR,  evaluate::plus_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "+", TAG_VECTOR,  evaluate::plus_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },
  { "+", TAG_MATRIX,  evaluate::plus_m_m, 2, { TAG_MATRIX,  TAG_MATRIX  } },
  { "+", TAG_STRING,  evaluate::plus_s_s, 2, { TAG_STRING,  TAG_STRING  } },
  { "+", TAG_STRING,  evaluate::plus_c_s, 2, { TAG_CHAR,    TAG_STRING  } },
  { "+", TAG_STRING,  evaluate::plus_s_c, 2, { TAG_STRING,  TAG_CHAR    } },
  { "+", TAG_MATVEC,  evaluate::plus_mv_mv, 2, { TAG_MATVEC, TAG_MATVEC } },
  { "+", TAG_MATVEC,  evaluate::plus_mv_m, 2, { TAG_MATVEC, TAG_MATRIX  } },
  { "+", TAG_MATVEC,  evaluate::plus_m_mv, 2, { TAG_MATRIX, TAG_MATVEC  } },

  { "-", TAG_DOUBLE,  evaluate::minus_d, 1, { TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_c, 1, { TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_v, 1, { TAG_VECTOR  } },
  { "-", TAG_MATRIX,  evaluate::minus_m, 1, { TAG_MATRIX  } },
  { "-", TAG_MATVEC,  evaluate::minus_mv, 1, { TAG_MATVEC } },

  { "-", TAG_DOUBLE,  evaluate::minus_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "-", TAG_COMPLEX, evaluate::minus_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "-", TAG_COMPLEX, evaluate::minus_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "-", TAG_VECTOR,  evaluate::minus_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "-", TAG_VECTOR,  evaluate::minus_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "-", TAG_VECTOR,  evaluate::minus_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "-", TAG_VECTOR,  evaluate::minus_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },
  { "-", TAG_MATRIX,  evaluate::minus_m_m, 2, { TAG_MATRIX,  TAG_MATRIX  } },
  { "-", TAG_MATVEC,  evaluate::minus_mv_mv, 2, { TAG_MATVEC, TAG_MATVEC } },
  { "-", TAG_MATVEC,  evaluate::minus_mv_m, 2, { TAG_MATVEC, TAG_MATRIX  } },
  { "-", TAG_MATVEC,  evaluate::minus_m_mv, 2, { TAG_MATRIX, TAG_MATVEC  } },

  { "*", TAG_DOUBLE,  evaluate::times_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "*", TAG_COMPLEX, evaluate::times_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "*", TAG_COMPLEX, evaluate::times_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "*", TAG_COMPLEX, evaluate::times_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "*", TAG_VECTOR,  evaluate::times_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "*", TAG_VECTOR,  evaluate::times_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "*", TAG_VECTOR,  evaluate::times_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "*", TAG_VECTOR,  evaluate::times_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "*", TAG_VECTOR,  evaluate::times_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },
  { "*", TAG_MATRIX,  evaluate::times_m_m, 2, { TAG_MATRIX,  TAG_MATRIX  } },
  { "*", TAG_MATRIX,  evaluate::times_m_c, 2, { TAG_MATRIX,  TAG_COMPLEX } },
  { "*", TAG_MATRIX,  evaluate::times_c_m, 2, { TAG_COMPLEX, TAG_MATRIX  } },
  { "*", TAG_MATRIX,  evaluate::times_m_d, 2, { TAG_MATRIX,  TAG_DOUBLE  } },
  { "*", TAG_MATRIX,  evaluate::times_d_m, 2, { TAG_DOUBLE,  TAG_MATRIX  } },
  { "*", TAG_MATVEC,  evaluate::times_mv_mv, 2, { TAG_MATVEC,TAG_MATVEC  } },
  { "*", TAG_MATVEC,  evaluate::times_mv_c, 2, { TAG_MATVEC, TAG_COMPLEX } },
  { "*", TAG_MATVEC,  evaluate::times_c_mv, 2, { TAG_COMPLEX,TAG_MATVEC  } },
  { "*", TAG_MATVEC,  evaluate::times_mv_d, 2, { TAG_MATVEC, TAG_DOUBLE  } },
  { "*", TAG_MATVEC,  evaluate::times_d_mv, 2, { TAG_DOUBLE, TAG_MATVEC  } },
  { "*", TAG_MATVEC,  evaluate::times_mv_v, 2, { TAG_MATVEC, TAG_VECTOR  } },
  { "*", TAG_MATVEC,  evaluate::times_v_mv, 2, { TAG_VECTOR, TAG_MATVEC  } },

  { "/", TAG_DOUBLE,  evaluate::over_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "/", TAG_COMPLEX, evaluate::over_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "/", TAG_COMPLEX, evaluate::over_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "/", TAG_COMPLEX, evaluate::over_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "/", TAG_VECTOR,  evaluate::over_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "/", TAG_VECTOR,  evaluate::over_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "/", TAG_VECTOR,  evaluate::over_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "/", TAG_VECTOR,  evaluate::over_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "/", TAG_VECTOR,  evaluate::over_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },
  { "/", TAG_MATRIX,  evaluate::over_m_c, 2, { TAG_MATRIX,  TAG_COMPLEX } },
  { "/", TAG_MATRIX,  evaluate::over_m_d, 2, { TAG_MATRIX,  TAG_DOUBLE  } },
  { "/", TAG_MATVEC,  evaluate::over_mv_c, 2, { TAG_MATRIX, TAG_COMPLEX } },
  { "/", TAG_MATVEC,  evaluate::over_mv_d, 2, { TAG_MATVEC, TAG_DOUBLE  } },
  { "/", TAG_MATVEC,  evaluate::over_mv_v, 2, { TAG_MATVEC, TAG_VECTOR  } },

  { "%", TAG_DOUBLE,  evaluate::modulo_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "%", TAG_COMPLEX, evaluate::modulo_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "%", TAG_COMPLEX, evaluate::modulo_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "%", TAG_COMPLEX, evaluate::modulo_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "%", TAG_VECTOR,  evaluate::modulo_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "%", TAG_VECTOR,  evaluate::modulo_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "%", TAG_VECTOR,  evaluate::modulo_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "^", TAG_DOUBLE,  evaluate::power_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "^", TAG_COMPLEX, evaluate::power_c_c, 2, { TAG_COMPLEX, TAG_COMPLEX } },
  { "^", TAG_COMPLEX, evaluate::power_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "^", TAG_COMPLEX, evaluate::power_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { "^", TAG_VECTOR,  evaluate::power_v_d, 2, { TAG_VECTOR,  TAG_DOUBLE  } },
  { "^", TAG_VECTOR,  evaluate::power_d_v, 2, { TAG_DOUBLE,  TAG_VECTOR  } },
  { "^", TAG_VECTOR,  evaluate::power_v_c, 2, { TAG_VECTOR,  TAG_COMPLEX } },
  { "^", TAG_VECTOR,  evaluate::power_c_v, 2, { TAG_COMPLEX, TAG_VECTOR  } },
  { "^", TAG_VECTOR,  evaluate::power_v_v, 2, { TAG_VECTOR,  TAG_VECTOR  } },

  { "real", TAG_DOUBLE,  evaluate::real_d, 1, { TAG_DOUBLE  } },
  { "real", TAG_DOUBLE,  evaluate::real_c, 1, { TAG_COMPLEX } },
  { "real", TAG_VECTOR,  evaluate::real_v, 1, { TAG_VECTOR  } },
  { "real", TAG_MATRIX,  evaluate::real_m, 1, { TAG_MATRIX  } },
  { "real", TAG_MATVEC,  evaluate::real_mv, 1, { TAG_MATVEC } },

  { "imag", TAG_DOUBLE,  evaluate::imag_d, 1, { TAG_DOUBLE  } },
  { "imag", TAG_DOUBLE,  evaluate::imag_c, 1, { TAG_COMPLEX } },
  { "imag", TAG_VECTOR,  evaluate::imag_v, 1, { TAG_VECTOR  } },
  { "imag", TAG_MATRIX,  evaluate::imag_m, 1, { TAG_MATRIX  } },
  { "imag", TAG_MATVEC,  evaluate::imag_mv, 1, { TAG_MATVEC } },

  { "abs", TAG_DOUBLE,  evaluate::abs_d, 1, { TAG_DOUBLE  } },
  { "abs", TAG_DOUBLE,  evaluate::abs_c, 1, { TAG_COMPLEX } },
  { "abs", TAG_VECTOR,  evaluate::abs_v, 1, { TAG_VECTOR  } },
  { "abs", TAG_MATRIX,  evaluate::abs_m, 1, { TAG_MATRIX  } },
  { "abs", TAG_MATVEC,  evaluate::abs_mv, 1, { TAG_MATVEC } },

  { "mag", TAG_DOUBLE,  evaluate::abs_d, 1, { TAG_DOUBLE  } },
  { "mag", TAG_DOUBLE,  evaluate::abs_c, 1, { TAG_COMPLEX } },
  { "mag", TAG_VECTOR,  evaluate::abs_v, 1, { TAG_VECTOR  } },
  { "mag", TAG_MATRIX,  evaluate::abs_m, 1, { TAG_MATRIX  } },
  { "mag", TAG_MATVEC,  evaluate::abs_mv, 1, { TAG_MATVEC } },

  { "conj", TAG_DOUBLE,  evaluate::conj_d,  1, { TAG_DOUBLE  } },
  { "conj", TAG_COMPLEX, evaluate::conj_c,  1, { TAG_COMPLEX } },
  { "conj", TAG_VECTOR,  evaluate::conj_v,  1, { TAG_VECTOR  } },
  { "conj", TAG_MATRIX,  evaluate::conj_m,  1, { TAG_MATRIX  } },
  { "conj", TAG_MATVEC,  evaluate::conj_mv, 1, { TAG_MATVEC  } },

  { "norm", TAG_DOUBLE,  evaluate::norm_d, 1, { TAG_DOUBLE  } },
  { "norm", TAG_DOUBLE,  evaluate::norm_c, 1, { TAG_COMPLEX } },
  { "norm", TAG_VECTOR,  evaluate::norm_v, 1, { TAG_VECTOR  } },

  { "phase", TAG_DOUBLE,  evaluate::phase_d, 1, { TAG_DOUBLE  } },
  { "phase", TAG_DOUBLE,  evaluate::phase_c, 1, { TAG_COMPLEX } },
  { "phase", TAG_VECTOR,  evaluate::phase_v, 1, { TAG_VECTOR  } },
  { "phase", TAG_MATRIX,  evaluate::phase_m,  1, { TAG_MATRIX } },
  { "phase", TAG_MATVEC,  evaluate::phase_mv, 1, { TAG_MATVEC } },

  { "angle", TAG_DOUBLE,  evaluate::arg_d, 1, { TAG_DOUBLE  } },
  { "angle", TAG_DOUBLE,  evaluate::arg_c, 1, { TAG_COMPLEX } },
  { "angle", TAG_VECTOR,  evaluate::arg_v, 1, { TAG_VECTOR  } },
  { "angle", TAG_MATRIX,  evaluate::arg_m,  1, { TAG_MATRIX } },
  { "angle", TAG_MATVEC,  evaluate::arg_mv, 1, { TAG_MATVEC } },

  { "arg", TAG_DOUBLE,  evaluate::arg_d, 1, { TAG_DOUBLE  } },
  { "arg", TAG_DOUBLE,  evaluate::arg_c, 1, { TAG_COMPLEX } },
  { "arg", TAG_VECTOR,  evaluate::arg_v, 1, { TAG_VECTOR  } },
  { "arg", TAG_MATRIX,  evaluate::arg_m,  1, { TAG_MATRIX } },
  { "arg", TAG_MATVEC,  evaluate::arg_mv, 1, { TAG_MATVEC } },

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

  { "sec", TAG_DOUBLE,  evaluate::sec_d, 1, { TAG_DOUBLE  } },
  { "sec", TAG_COMPLEX, evaluate::sec_c, 1, { TAG_COMPLEX } },
  { "sec", TAG_VECTOR,  evaluate::sec_v, 1, { TAG_VECTOR  } },

  { "cosec", TAG_DOUBLE,  evaluate::cosec_d, 1, { TAG_DOUBLE  } },
  { "cosec", TAG_COMPLEX, evaluate::cosec_c, 1, { TAG_COMPLEX } },
  { "cosec", TAG_VECTOR,  evaluate::cosec_v, 1, { TAG_VECTOR  } },

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

  { "sech", TAG_DOUBLE,  evaluate::sech_d, 1, { TAG_DOUBLE  } },
  { "sech", TAG_COMPLEX, evaluate::sech_c, 1, { TAG_COMPLEX } },
  { "sech", TAG_VECTOR,  evaluate::sech_v, 1, { TAG_VECTOR  } },

  { "cosech", TAG_DOUBLE,  evaluate::cosech_d, 1, { TAG_DOUBLE  } },
  { "cosech", TAG_COMPLEX, evaluate::cosech_c, 1, { TAG_COMPLEX } },
  { "cosech", TAG_VECTOR,  evaluate::cosech_v, 1, { TAG_VECTOR  } },

  { "arcoth", TAG_DOUBLE,  evaluate::arcoth_d, 1, { TAG_DOUBLE  } },
  { "arcoth", TAG_COMPLEX, evaluate::arcoth_c, 1, { TAG_COMPLEX } },
  { "arcoth", TAG_VECTOR,  evaluate::arcoth_v, 1, { TAG_VECTOR  } },

  { "ztor", TAG_DOUBLE,  evaluate::ztor_d, 1, { TAG_DOUBLE  } },
  { "ztor", TAG_COMPLEX, evaluate::ztor_c, 1, { TAG_COMPLEX } },
  { "ztor", TAG_VECTOR,  evaluate::ztor_v, 1, { TAG_VECTOR  } },

  { "rtoz", TAG_DOUBLE,  evaluate::rtoz_d, 1, { TAG_DOUBLE  } },
  { "rtoz", TAG_COMPLEX, evaluate::rtoz_c, 1, { TAG_COMPLEX } },
  { "rtoz", TAG_VECTOR,  evaluate::rtoz_v, 1, { TAG_VECTOR  } },

  { "rtoswr", TAG_DOUBLE, evaluate::rtoswr_d, 1, { TAG_DOUBLE  } },
  { "rtoswr", TAG_DOUBLE, evaluate::rtoswr_c, 1, { TAG_COMPLEX } },
  { "rtoswr", TAG_VECTOR, evaluate::rtoswr_v, 1, { TAG_VECTOR  } },

  { "diff", TAG_VECTOR, evaluate::diff_v_2, 2, { TAG_VECTOR, TAG_VECTOR } },
  { "diff", TAG_VECTOR, evaluate::diff_v_3, 3, { TAG_VECTOR, TAG_VECTOR,
						 TAG_DOUBLE } },

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

  { "array", TAG_VECTOR, evaluate::index_mv_2, 3,
    { TAG_MATVEC, TAG_DOUBLE, TAG_DOUBLE } },
  { "array", TAG_MATRIX, evaluate::index_mv_1, 2,
    { TAG_MATVEC, TAG_DOUBLE } },
  { "array", TAG_VECTOR,  evaluate::index_v_1, 2,
    { TAG_VECTOR, TAG_DOUBLE | TAG_RANGE } },
  { "array", TAG_VECTOR,  evaluate::index_v_2, 3,
    { TAG_VECTOR, TAG_DOUBLE | TAG_RANGE, TAG_DOUBLE | TAG_RANGE } },
  { "array", TAG_COMPLEX, evaluate::index_m_2, 3,
    { TAG_MATRIX, TAG_DOUBLE, TAG_DOUBLE } },
  { "array", TAG_CHAR, evaluate::index_s_1, 2, { TAG_STRING, TAG_DOUBLE } },

  { "stoy", TAG_MATRIX, evaluate::stoy_m,  1, { TAG_MATRIX } },
  { "stoy", TAG_MATVEC, evaluate::stoy_mv, 1, { TAG_MATVEC } },
  { "stoz", TAG_MATRIX, evaluate::stoz_m,  1, { TAG_MATRIX } },
  { "stoz", TAG_MATVEC, evaluate::stoz_mv, 1, { TAG_MATVEC } },
  { "ytos", TAG_MATRIX, evaluate::ytos_m,  1, { TAG_MATRIX } },
  { "ytos", TAG_MATVEC, evaluate::ytos_mv, 1, { TAG_MATVEC } },
  { "ytoz", TAG_MATRIX, evaluate::ytoz_m,  1, { TAG_MATRIX } },
  { "ytoz", TAG_MATVEC, evaluate::ytoz_mv, 1, { TAG_MATVEC } },
  { "ztos", TAG_MATRIX, evaluate::ztos_m,  1, { TAG_MATRIX } },
  { "ztos", TAG_MATVEC, evaluate::ztos_mv, 1, { TAG_MATVEC } },
  { "ztoy", TAG_MATRIX, evaluate::ztoy_m,  1, { TAG_MATRIX } },
  { "ztoy", TAG_MATVEC, evaluate::ztoy_mv, 1, { TAG_MATVEC } },

  { "twoport", TAG_MATRIX, evaluate::twoport_m,  3,
    { TAG_MATRIX, TAG_CHAR, TAG_CHAR } },
  { "twoport", TAG_MATVEC, evaluate::twoport_mv, 3,
    { TAG_MATVEC, TAG_CHAR, TAG_CHAR } },

  { "inverse",   TAG_MATRIX,  evaluate::inverse_m,    1, { TAG_MATRIX } },
  { "inverse",   TAG_MATVEC,  evaluate::inverse_mv,   1, { TAG_MATVEC } },
  { "transpose", TAG_MATRIX,  evaluate::transpose_m,  1, { TAG_MATRIX } },
  { "transpose", TAG_MATVEC,  evaluate::transpose_mv, 1, { TAG_MATVEC } },
  { "det",       TAG_COMPLEX, evaluate::det_m,        1, { TAG_MATRIX } },
  { "det",       TAG_VECTOR,  evaluate::det_mv,       1, { TAG_MATVEC } },
  { "eye",       TAG_MATRIX,  evaluate::eye_m,        1, { TAG_DOUBLE } },
  { "adjoint",   TAG_MATRIX,  evaluate::adjoint_m,    1, { TAG_MATRIX } },
  { "adjoint",   TAG_MATVEC,  evaluate::adjoint_mv,   1, { TAG_MATVEC } },

  { "sign", TAG_DOUBLE,  evaluate::sign_d, 1, { TAG_DOUBLE  } },
  { "sign", TAG_COMPLEX, evaluate::sign_c, 1, { TAG_COMPLEX } },
  { "sign", TAG_VECTOR,  evaluate::sign_v, 1, { TAG_VECTOR  } },

  { "sinc", TAG_DOUBLE,  evaluate::sinc_d, 1, { TAG_DOUBLE  } },
  { "sinc", TAG_COMPLEX, evaluate::sinc_c, 1, { TAG_COMPLEX } },
  { "sinc", TAG_VECTOR,  evaluate::sinc_v, 1, { TAG_VECTOR  } },

  { "Rollet", TAG_DOUBLE,  evaluate::rollet_m,  1, { TAG_MATRIX } },
  { "Rollet", TAG_VECTOR,  evaluate::rollet_mv, 1, { TAG_MATVEC } },
  { "Mu",     TAG_DOUBLE,  evaluate::mu1_m,     1, { TAG_MATRIX } },
  { "Mu",     TAG_VECTOR,  evaluate::mu1_mv,    1, { TAG_MATVEC } },
  { "Mu2",    TAG_DOUBLE,  evaluate::mu2_m,     1, { TAG_MATRIX } },
  { "Mu2",    TAG_VECTOR,  evaluate::mu2_mv,    1, { TAG_MATVEC } },

  { "linspace", TAG_VECTOR, evaluate::linspace, 3,
    { TAG_DOUBLE, TAG_DOUBLE, TAG_DOUBLE } },
  { "logspace", TAG_VECTOR, evaluate::logspace, 3,
    { TAG_DOUBLE, TAG_DOUBLE, TAG_DOUBLE } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_d_v, 5,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_DOUBLE, TAG_VECTOR } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_d_d, 5,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_DOUBLE, TAG_DOUBLE } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_d, 4,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_DOUBLE } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_v_v, 5,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_VECTOR } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_v_d, 5,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_DOUBLE } },
  { "NoiseCircle", TAG_VECTOR, evaluate::noise_circle_v, 4,
    { TAG_VECTOR, TAG_VECTOR, TAG_VECTOR, TAG_VECTOR } },
  { "StabCircleL", TAG_VECTOR, evaluate::stab_circle_l_v, 2,
    { TAG_MATVEC, TAG_VECTOR } },
  { "StabCircleL", TAG_VECTOR, evaluate::stab_circle_l_d, 2,
    { TAG_MATVEC, TAG_DOUBLE } },
  { "StabCircleL", TAG_VECTOR, evaluate::stab_circle_l, 1, { TAG_MATVEC } },
  { "StabCircleS", TAG_VECTOR, evaluate::stab_circle_s_v, 2,
    { TAG_MATVEC, TAG_VECTOR } },
  { "StabCircleS", TAG_VECTOR, evaluate::stab_circle_s_d, 2,
    { TAG_MATVEC, TAG_DOUBLE } },
  { "StabCircleS", TAG_VECTOR, evaluate::stab_circle_s, 1, { TAG_MATVEC } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_d_v, 3,
    { TAG_MATVEC, TAG_DOUBLE, TAG_VECTOR } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_d_d, 3,
    { TAG_MATVEC, TAG_DOUBLE, TAG_DOUBLE } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_d, 2,
    { TAG_MATVEC, TAG_DOUBLE } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_v_v, 3,
    { TAG_MATVEC, TAG_VECTOR, TAG_VECTOR } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_v_d, 3,
    { TAG_MATVEC, TAG_VECTOR, TAG_DOUBLE } },
  { "GaCircle", TAG_VECTOR, evaluate::ga_circle_v, 2,
    { TAG_MATVEC, TAG_VECTOR } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_d_v, 3,
    { TAG_MATVEC, TAG_DOUBLE, TAG_VECTOR } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_d_d, 3,
    { TAG_MATVEC, TAG_DOUBLE, TAG_DOUBLE } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_d, 2,
    { TAG_MATVEC, TAG_DOUBLE } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_v_v, 3,
    { TAG_MATVEC, TAG_VECTOR, TAG_VECTOR } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_v_d, 3,
    { TAG_MATVEC, TAG_VECTOR, TAG_DOUBLE } },
  { "GpCircle", TAG_VECTOR, evaluate::gp_circle_v, 2,
    { TAG_MATVEC, TAG_VECTOR } },
  { "PlotVs", TAG_VECTOR, evaluate::plot_vs_v, 2,
    { TAG_VECTOR, TAG_VECTOR } },
  { "PlotVs", TAG_MATVEC, evaluate::plot_vs_mv, 2,
    { TAG_MATVEC, TAG_VECTOR } },

  { NULL, 0, NULL, 0, { 0 } /* end of list */ }
};
