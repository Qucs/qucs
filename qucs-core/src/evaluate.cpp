/*
 * evaluate.cpp - the Qucs equation evaluator implementations
 *
 * Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2006 Gunther Kraut <gn.kraut@t-online.de>
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
#include "receiver.h"
#include "constants.h"
#include "fspecial.h"
#include "circuit.h"
#include "range.h"
#include "equation.h"
#include "evaluate.h"
#include "exception.h"
#include "exceptionstack.h"
#include "strlist.h"

using namespace eqn;
using namespace qucs;
using namespace fourier;
using namespace fspecial;

// Short macros in order to obtain the correct constant value.
#define D(con)   ((constant *) (con))->d
#define C(con)   ((constant *) (con))->c
#define V(con)   ((constant *) (con))->v
#define M(con)   ((constant *) (con))->m
#define MV(con)  ((constant *) (con))->mv
#define STR(con) ((constant *) (con))->s
#define CHR(con) ((constant *) (con))->chr
#define INT(con) ((int) D (con))
#define RNG(con) ((constant *) (con))->r
#define B(con)   ((constant *) (con))->b

#define A(a) ((assignment *) (a))
#define R(r) ((reference *) (r))

// Argument macros.
#define _ARES(idx) args->getResult(idx)
#define _ARG(idx) args->get(idx)

#define _D(var,idx) nr_double_t (var) = D (_ARES (idx));
#define _BO(var,idx) bool (var) = B (_ARES (idx));
#define _CX(var,idx) nr_complex_t * (var) = C (_ARES (idx));
#define _V(var,idx) vector * (var) = V (_ARES (idx));
#define _M(var,idx) matrix * (var) = M (_ARES (idx));
#define _MV(var,idx) matvec * (var) = MV (_ARES (idx));
#define _I(var,idx) int (var) = INT (_ARES (idx));
#define _R(var,idx) range * (var) = RNG (_ARES (idx));

#define _ARR0(var) _R (var,0)
#define _ARR1(var) _R (var,1)
#define _ARR2(var) _R (var,2)
#define _ARI0(var) _I (var,0)
#define _ARI1(var) _I (var,1)
#define _ARI2(var) _I (var,2)
#define _ARD0(var) _D (var,0)
#define _ARD1(var) _D (var,1)
#define _ARD2(var) _D (var,2)
#define _ARB0(var) _BO (var,0)
#define _ARB1(var) _BO (var,1)
#define _ARB2(var) _BO (var,2)
#define _ARC0(var) _CX (var,0)
#define _ARC1(var) _CX (var,1)
#define _ARC2(var) _CX (var,2)
#define _ARM0(var) _M (var,0)
#define _ARM1(var) _M (var,1)
#define _ARM2(var) _M (var,2)
#define _ARV0(var) _V (var,0)
#define _ARV1(var) _V (var,1)
#define _ARV2(var) _V (var,2)
#define _ARMV0(var) _MV (var,0)
#define _ARMV1(var) _MV (var,1)
#define _ARMV2(var) _MV (var,2)

// Return value definition macros.
#define _DEFD() constant * res = new constant (TAG_DOUBLE);
#define _DEFB() constant * res = new constant (TAG_BOOLEAN);
#define _DEFC() constant * res = new constant (TAG_COMPLEX);
#define _DEFV() constant * res = new constant (TAG_VECTOR);
#define _DEFM() constant * res = new constant (TAG_MATRIX);
#define _DEFMV() constant * res = new constant (TAG_MATVEC);
#define _DEFR() constant * res = new constant (TAG_RANGE);

// Return value macros.
#define _RETD(var) res->d = (var); return res;
#define _RETB(var) res->b = (var); return res;
#define _RETC(var) res->c = new nr_complex_t (var); return res;
#define _RETV(var) res->v = new vector (var); return res;
#define _RETM(var) res->m = new matrix (var); return res;
#define _RETMV(var) res->mv = new matvec (var); return res;
#define _RETR(var) res->r = (var); return res;

// Return value macros without arguments.
#define __RETC() res->c = new nr_complex_t (); return res;
#define __RETV() res->v = new vector (); return res;
#define __RETM() res->m = new matrix (); return res;
#define __RETMV() res->mv = new matvec (); return res;

#define SOLVEE(idx) args->get(idx)->solvee

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
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFC ();							  \
  _RETC (cfunc (*c));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}

#define MAKE_FUNC_DEFINITION_1(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) { \
  _ARD0 (d);							  \
  _DEFD ();							  \
  _RETD (cfunc (d));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) { \
  _ARC0 (c);							  \
  _DEFD ();							  \
  res->d = cfunc (*c); return res;				  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) { \
  _ARV0 (v);							  \
  _DEFV ();							  \
  _RETV (cfunc (*v));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_m) (constant * args) { \
  _ARM0 (m);							  \
  _DEFM ();							  \
  _RETM (cfunc (*m));						  \
}								  \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_mv) (constant * args) {\
  _ARMV0 (mv);							  \
  _DEFMV ();							  \
  _RETMV (cfunc (*mv));						  \
}

MAKE_FUNC_DEFINITION_0 (exp);    // exponential function
MAKE_FUNC_DEFINITION_0 (limexp); // limited exponential function
MAKE_FUNC_DEFINITION_0 (sin);    // sine
MAKE_FUNC_DEFINITION_0 (cos);    // cosine
MAKE_FUNC_DEFINITION_0 (tan);    // tangent
MAKE_FUNC_DEFINITION_0 (sinh);   // sine hyperbolicus
MAKE_FUNC_DEFINITION_0 (cosh);   // cosine hyperbolicus
MAKE_FUNC_DEFINITION_0 (tanh);   // tangent hyperbolicus
MAKE_FUNC_DEFINITION_0 (coth);   // cotangent hyperbolicus
MAKE_FUNC_DEFINITION_0 (sech);   // secans hyperbolicus
MAKE_FUNC_DEFINITION_0 (cosech); // cosecans hyperbolicus
MAKE_FUNC_DEFINITION_0 (signum); // signum function
MAKE_FUNC_DEFINITION_0 (sign);   // sign function
MAKE_FUNC_DEFINITION_0 (sinc);   // sin(x)/x aka sinc function
MAKE_FUNC_DEFINITION_0 (sqr);    // square value

MAKE_FUNC_DEFINITION_1 (real);   // real value
MAKE_FUNC_DEFINITION_1 (imag);   // imaginary value
MAKE_FUNC_DEFINITION_1 (abs);    // absolute value

// ******************** unary plus *************************
constant * evaluate::plus_d (constant * args) {
  _ARD0 (d1); _DEFD (); _RETD (d1);
}

constant * evaluate::plus_c (constant * args) {
  _ARC0 (c1); _DEFC (); _RETC (*c1);
}

constant * evaluate::plus_v (constant * args) {
  _ARV0 (v1); _DEFV (); _RETV (*v1);
}

constant * evaluate::plus_m (constant * args) {
  _ARM0 (m1); _DEFM (); _RETM (*m1);
}

constant * evaluate::plus_mv (constant * args) {
  _ARMV0 (v1); _DEFMV (); _RETMV (*v1);
}

// ****************** addition *****************************
constant * evaluate::plus_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 + d2);
}

constant * evaluate::plus_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (*c1 + *c2);
}

constant * evaluate::plus_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (*c1 + d2);
}

constant * evaluate::plus_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 + *c2);
}

constant * evaluate::plus_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 + d2);
}

constant * evaluate::plus_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 + *v2);
}

constant * evaluate::plus_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 + *c2);
}

constant * evaluate::plus_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v2 + *c1);
}

constant * evaluate::plus_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 + *v2);
}

constant * evaluate::plus_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*m1 + *m2);
}

constant * evaluate::plus_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 + *v2);
}

constant * evaluate::plus_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  _RETMV (*v1 + *m2);
}

constant * evaluate::plus_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*m1 + *v2);
}

constant * evaluate::plus_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 + d2);
}

constant * evaluate::plus_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 + *m2);
}

constant * evaluate::plus_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 + *c2);
}

constant * evaluate::plus_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 + *m2);
}

constant * evaluate::plus_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARD1  (d2);
  _DEFMV ();
  _RETMV (*m1 + d2);
}

constant * evaluate::plus_d_mv (constant * args) {
  _ARD0  (d1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (d1 + *m2);
}

constant * evaluate::plus_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (*m1 + *c2);
}

constant * evaluate::plus_c_mv (constant * args) {
  _ARC0  (c1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*c1 + *m2);
}

constant * evaluate::plus_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (*m1 + *v2);
}

constant * evaluate::plus_v_mv (constant * args) {
  _ARV0  (v1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*v1 + *m2);
}

constant * evaluate::plus_s_s (constant * args) {
  char * s1 = STR (_ARES(0));
  char * s2 = STR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + strlen (s2) + 1);
  strcpy (p, s1); strcat (p, s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_c_s (constant * args) {
  char   c1 = CHR (_ARES(0));
  char * s2 = STR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s2) + 2);
  p[0] = c1; strcpy (&p[1], s2);
  res->s = p;
  return res;
}

constant * evaluate::plus_s_c (constant * args) {
  char * s1 = STR (_ARES(0));
  char   c2 = CHR (_ARES(1));
  constant * res = new constant (TAG_STRING);
  char * p = (char *) malloc (strlen (s1) + 2);
  strcpy (p, s1); p[strlen (s1)] = c2; p[strlen (s1) + 1] = '\0';
  res->s = p;
  return res;
}

// ******************** unary minus ***************************
constant * evaluate::minus_d (constant * args) {
  _ARD0 (d1); _DEFD (); _RETD (-d1);
}

constant * evaluate::minus_c (constant * args) {
  _ARC0 (c1); _DEFC (); _RETC (-*c1);
}

constant * evaluate::minus_v (constant * args) {
  _ARV0 (v1); _DEFV (); _RETV (-*v1);
}

constant * evaluate::minus_m (constant * args) {
  _ARM0 (m1); _DEFM (); _RETM (-*m1);
}

constant * evaluate::minus_mv (constant * args) {
  _ARMV0 (v1); _DEFMV (); _RETMV (-*v1);
}

// ****************** subtraction *****************************
constant * evaluate::minus_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 - d2);
}

constant * evaluate::minus_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (*c1 - *c2);
}

constant * evaluate::minus_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (*c1 - d2);
}

constant * evaluate::minus_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 - *c2);
}

constant * evaluate::minus_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 - d2);
}

constant * evaluate::minus_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 - *v2);
}

constant * evaluate::minus_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 - *c2);
}

constant * evaluate::minus_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 - *v2);
}

constant * evaluate::minus_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 - *v2);
}

constant * evaluate::minus_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*m1 - *m2);
}

constant * evaluate::minus_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 - *v2);
}

constant * evaluate::minus_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  _RETMV (*v1 - *m2);
}

constant * evaluate::minus_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*m1 - *v2);
}

constant * evaluate::minus_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 - d2);
}

constant * evaluate::minus_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 - *m2);
}

constant * evaluate::minus_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 - *c2);
}

constant * evaluate::minus_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 - *m2);
}

constant * evaluate::minus_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARD1  (d2);
  _DEFMV ();
  _RETMV (*m1 - d2);
}

constant * evaluate::minus_d_mv (constant * args) {
  _ARD0  (d1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (d1 - *m2);
}

constant * evaluate::minus_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (*m1 - *c2);
}

constant * evaluate::minus_c_mv (constant * args) {
  _ARC0  (c1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*c1 - *m2);
}

constant * evaluate::minus_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (*m1 - *v2);
}

constant * evaluate::minus_v_mv (constant * args) {
  _ARV0  (v1);
  _ARMV1 (m2);
  _DEFMV ();
  _RETMV (*v1 - *m2);
}

// ****************** multiplication *************************
constant * evaluate::times_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (d1 * d2);
}

constant * evaluate::times_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC ((*c1) * (*c2));
}

constant * evaluate::times_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC ((*c1) * d2);
}

constant * evaluate::times_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 * (*c2));
}

constant * evaluate::times_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 * d2);
  return res;
}

constant * evaluate::times_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 * *v2);
}

constant * evaluate::times_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 * *c2);
}

constant * evaluate::times_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 * *v2);
}

constant * evaluate::times_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 * *v2);
}

constant * evaluate::times_m_m (constant * args) {
  _ARM0 (m1);
  _ARM1 (m2);
  _DEFM ();
  if (m1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->m = new matrix (m1->getRows (), m2->getCols ());
  } else {
    res->m = new matrix (*m1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 * *c2);
}

constant * evaluate::times_c_m (constant * args) {
  _ARC0 (c1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (*c1 * *m2);
}

constant * evaluate::times_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 * d2);
}

constant * evaluate::times_d_m (constant * args) {
  _ARD0 (d1);
  _ARM1 (m2);
  _DEFM ();
  _RETM (d1 * *m2);
}

constant * evaluate::times_mv_mv (constant * args) {
  _ARMV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  if (v1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_c (constant * args) {
  _ARMV0 (v1);
  _ARC1 (c2);
  _DEFMV ();
  _RETMV (*v1 * *c2);
}

constant * evaluate::times_c_mv (constant * args) {
  _ARC0 (c1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*c1 * *v2);
}

constant * evaluate::times_mv_d (constant * args) {
  _ARMV0 (v1);
  _ARD1 (d2);
  _DEFMV ();
  _RETMV (*v1 * d2);
}

constant * evaluate::times_d_mv (constant * args) {
  _ARD0 (d1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (d1 * *v2);
}

constant * evaluate::times_mv_m (constant * args) {
  _ARMV0 (v1);
  _ARM1 (m2);
  _DEFMV ();
  if (v1->getCols () != m2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v1->getSize (), v1->getRows (), m2->getCols ());
  } else {
    res->mv = new matvec (*v1 * *m2);
  }
  return res;
}

constant * evaluate::times_m_mv (constant * args) {
  _ARM0 (m1);
  _ARMV1 (v2);
  _DEFMV ();
  if (m1->getCols () != v2->getRows ()) {
    THROW_MATH_EXCEPTION ("nonconformant arguments in matrix multiplication");
    res->mv = new matvec (v2->getSize (), m1->getRows (), v2->getCols ());
  } else {
    res->mv = new matvec (*m1 * *v2);
  }
  return res;
}

constant * evaluate::times_mv_v (constant * args) {
  _ARMV0 (v1);
  _ARV1 (v2);
  _DEFMV ();
  _RETMV (*v1 * *v2);
}

constant * evaluate::times_v_mv (constant * args) {
  _ARV0 (v1);
  _ARMV1 (v2);
  _DEFMV ();
  _RETMV (*v1 * *v2);
}

// ****************** division *************************
constant * evaluate::over_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETD (d1 / d2);
}

constant * evaluate::over_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (*c1 / *c2);
}

constant * evaluate::over_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (*c1 / d2);
}

constant * evaluate::over_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETC (d1 / *c2);
}

constant * evaluate::over_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  if (d2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETV (*v1 / d2);
}

constant * evaluate::over_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 / *v2);
}

constant * evaluate::over_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  if (*c2 == 0.0) THROW_MATH_EXCEPTION ("division by zero");
  _RETV (*v1 / *c2);
}

constant * evaluate::over_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 / *v2);
}

constant * evaluate::over_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 / *v2);
}

constant * evaluate::over_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (*m1 / *c2);
}

constant * evaluate::over_m_d (constant * args) {
  _ARM0 (m1);
  _ARD1 (d2);
  _DEFM ();
  _RETM (*m1 / d2);
}

constant * evaluate::over_mv_c (constant * args) {
  _ARMV0 (v1);
  _ARC1 (c2);
  _DEFMV ();
  _RETMV (*v1 / *c2);
}

constant * evaluate::over_mv_d (constant * args) {
  _ARMV0 (v1);
  _ARD1 (d2);
  _DEFMV ();
  _RETMV (*v1 / d2);
}

constant * evaluate::over_mv_v (constant * args) {
  _ARMV0 (v1);
  _ARV1 (v2);
  _DEFMV ();
  _RETMV (*v1 / *v2);
}

// ****************** modulo *************************
constant * evaluate::modulo_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (fmod (d1, d2));
}

constant * evaluate::modulo_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC ((*c1) % (*c2));
}

constant * evaluate::modulo_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC ((*c1) % d2);
}

constant * evaluate::modulo_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (d1 % (*c2));
}

constant * evaluate::modulo_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (*v1 % d2);
}

constant * evaluate::modulo_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (d1 % *v2);
}

constant * evaluate::modulo_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (*v1 % *c2);
}

constant * evaluate::modulo_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*c1 % *v2);
}

constant * evaluate::modulo_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (*v1 % *v2);
}

// ****************** power *************************
constant * evaluate::power_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (pow (d1, d2));
}

constant * evaluate::power_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (pow (*c1, *c2));
}

constant * evaluate::power_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  _RETC (pow (*c1, d2));
}

constant * evaluate::power_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  _RETC (pow (d1, *c2));
}

constant * evaluate::power_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (pow (*v1, d2));
}

constant * evaluate::power_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (d1, *v2));
}

constant * evaluate::power_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (pow (*v1, *c2));
}

constant * evaluate::power_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (*c1, *v2));
}

constant * evaluate::power_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (pow (*v1, *v2));
}

constant * evaluate::power_m_d (constant * args) {
  _ARM0 (m1);
  _ARI1 (i2);
  _DEFM ();
  _RETM (pow (*m1, i2));
}

constant * evaluate::power_m_c (constant * args) {
  _ARM0 (m1);
  _ARC1 (c2);
  _DEFM ();
  _RETM (pow (*m1, (int) real (*c2)));
}

constant * evaluate::power_mv_d (constant * args) {
  _ARMV0 (m1);
  _ARI1  (i2);
  _DEFMV ();
  _RETMV (pow (*m1, i2));
}

constant * evaluate::power_mv_c (constant * args) {
  _ARMV0 (m1);
  _ARC1  (c2);
  _DEFMV ();
  _RETMV (pow (*m1, (int) real (*c2)));
}

constant * evaluate::power_mv_v (constant * args) {
  _ARMV0 (m1);
  _ARV1  (v2);
  _DEFMV ();
  _RETMV (pow (*m1, *v2));
}

// ****************** hypotenuse *************************
constant * evaluate::xhypot_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (xhypot (d1, d2));
}

constant * evaluate::xhypot_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFD ();
  _RETD (xhypot (*c1, *c2));
}

constant * evaluate::xhypot_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (xhypot (*c1, d2));
}

constant * evaluate::xhypot_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFD ();
  _RETD (xhypot (d1, *c2));
}

constant * evaluate::xhypot_v_d (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (xhypot (*v1, d2));
}

constant * evaluate::xhypot_d_v (constant * args) {
  _ARD0 (d1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (d1, *v2));
}

constant * evaluate::xhypot_v_c (constant * args) {
  _ARV0 (v1);
  _ARC1 (c2);
  _DEFV ();
  _RETV (xhypot (*v1, *c2));
}

constant * evaluate::xhypot_c_v (constant * args) {
  _ARC0 (c1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (*c1, *v2));
}

constant * evaluate::xhypot_v_v (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (xhypot (*v1, *v2));
}

// ************** conjugate complex **********************
constant * evaluate::conj_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::conj_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (conj (*c1));
}

constant * evaluate::conj_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (conj (*v1));
}

constant * evaluate::conj_m (constant * args) {
  _ARM0 (m);
  _DEFM ();
  _RETM (conj (*m));
}

constant * evaluate::conj_mv (constant * args) {
  _ARMV0 (mv);
  _DEFMV ();
  _RETMV (conj (*mv));
}

// ********** square of absolute value *****************
constant * evaluate::norm_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 * d1);
}

constant * evaluate::norm_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (norm (*c1));
}

constant * evaluate::norm_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (norm (*v1));
}

// ********** phase in degrees *****************
constant * evaluate::phase_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 < 0.0 ? 180.0 : 0.0);
}

constant * evaluate::phase_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (deg (arg (*c1)));
}

constant * evaluate::phase_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (deg (arg (*v1)));
}

constant * evaluate::phase_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (deg (arg (*m1)));
}

constant * evaluate::phase_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (deg (arg (*v1)));
}

// ********** phase in radians *****************
constant * evaluate::arg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1 < 0.0 ? M_PI : 0.0);
}

constant * evaluate::arg_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (arg (*c1));
}

constant * evaluate::arg_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (arg (*v1));
}

constant * evaluate::arg_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (arg (*m1));
}

constant * evaluate::arg_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (arg (*v1));
}

// ******* unwrap phase in radians ************
constant * evaluate::unwrap_v_1 (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (unwrap (*v1));
}

constant * evaluate::unwrap_v_2 (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _DEFV ();
  _RETV (unwrap (*v1, fabs (d2)));
}

constant * evaluate::unwrap_v_3 (constant * args) {
  _ARV0 (v1);
  _ARD1 (d2);
  _ARD2 (d3);
  _DEFV ();
  _RETV (unwrap (*v1, fabs (d2), fabs (d3)));
}

// ******** radian/degree conversion **********
constant * evaluate::deg2rad_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (rad (d1));
}

constant * evaluate::deg2rad_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (rad (real (*c1)));
}

constant * evaluate::deg2rad_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) v->add (rad (real (v1->get (i))));
  res->v = v;
  return res;
}

constant * evaluate::rad2deg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (deg (d1));
}

constant * evaluate::rad2deg_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (deg (real (*c1)));
}

constant * evaluate::rad2deg_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) v->add (deg (real (v1->get (i))));
  res->v = v;
  return res;
}

// ********** voltage decibel *****************
constant * evaluate::dB_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (10.0 * log10 (fabs (d1)));
}

constant * evaluate::dB_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD (dB (*c1));
}

constant * evaluate::dB_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (dB (*v1));
}

constant * evaluate::dB_m (constant * args) {
  _ARM0 (m1);
  _DEFM ();
  _RETM (dB (*m1));
}

constant * evaluate::dB_mv (constant * args) {
  _ARMV0 (v1);
  _DEFMV ();
  _RETMV (dB (*v1));
}

// ********** square root *****************
constant * evaluate::sqrt_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (0.0, sqrt (-d1));
  else
    res->c = new nr_complex_t (sqrt (d1));
  return res;
}

constant * evaluate::sqrt_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (sqrt (*c1));
}

constant * evaluate::sqrt_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (sqrt (*v1));
}

// ********** natural logarithm *****************
constant * evaluate::ln_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (log (-d1), M_PI);
  else
    res->c = new nr_complex_t (log (d1));
  return res;
}

constant * evaluate::ln_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (log (*c1));
}

constant * evaluate::ln_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log (*v1));
}

// ********** decimal logarithm *****************
constant * evaluate::log10_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (log10 (-d1), M_PI * M_LOG10E);
  else
    res->c = new nr_complex_t (log10 (d1));
  return res;
}

constant * evaluate::log10_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (log10 (*c1));
}

constant * evaluate::log10_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log10 (*v1));
}

// ********** binary logarithm *****************
constant * evaluate::log2_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  if (d1 < 0.0)
    res->c = new nr_complex_t (log (-d1) * M_LOG2E, M_PI * M_LOG2E);
  else
    res->c = new nr_complex_t (log (d1) * M_LOG2E);
  return res;
}

constant * evaluate::log2_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (log2 (*c1));
}

constant * evaluate::log2_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (log2 (*v1));
}

// ************* arcus sine *********************
constant * evaluate::arcsin_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (asin (d1));
}

constant * evaluate::arcsin_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asin (*c1));
}

constant * evaluate::arcsin_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asin (*v1));
}

// ************* arcus cosine ******************
constant * evaluate::arccos_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (acos (d1));
}

constant * evaluate::arccos_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acos (*c1));
}

constant * evaluate::arccos_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acos (*v1));
}

// ************** arcus tangent ******************
constant * evaluate::arctan_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (atan (d1));
}

constant * evaluate::arctan_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (atan (*c1));
}

constant * evaluate::arctan_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (atan (*v1));
}

// *************** cotangent ********************
constant * evaluate::cot_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / tan (d1));
}

constant * evaluate::cot_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (cot (*c1));
}

constant * evaluate::cot_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (cot (*v1));
}

// ************ arcus cotangent *****************
constant * evaluate::arccot_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (M_PI_2 - atan (d1));
}

constant * evaluate::arccot_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acot (*c1));
}

constant * evaluate::arccot_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acot (*v1));
}

// ***************** secans *********************
constant * evaluate::sec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / cos (d1));
}

constant * evaluate::sec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (1.0 / cos (*c1));
}

constant * evaluate::sec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (1.0 / cos (*v1));
}

// *************** arcus secans *******************
constant * evaluate::arcsec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (acos (1.0 / d1));
}

constant * evaluate::arcsec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acos (1.0 / *c1));
}

constant * evaluate::arcsec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acos (1.0 / *v1));
}

// ***************** cosecans *********************
constant * evaluate::cosec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (1.0 / sin (d1));
}

constant * evaluate::cosec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (1.0 / sin (*c1));
}

constant * evaluate::cosec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (1.0 / sin (*v1));
}

// ************* arcus cosecans *******************
constant * evaluate::arccosec_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (asin (1.0 / d1));
}

constant * evaluate::arccosec_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asin (1.0 / *c1));
}

constant * evaluate::arccosec_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asin (1.0 / *v1));
}

// ********** area sine hyperbolicus **************
constant * evaluate::arsinh_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (log (d1 + sqrt (d1 * d1 + 1)));
}

constant * evaluate::arsinh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asinh (*c1));
}

constant * evaluate::arsinh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asinh (*v1));
}

// ********** area cosecans hyperbolicus **************
constant * evaluate::arcosech_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  d1 = 1 / d1;
  _RETD (log (d1 + sqrt (d1 * d1 + 1)));
}

constant * evaluate::arcosech_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asinh (1.0 / *c1));
}

constant * evaluate::arcosech_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asinh (1 / *v1));
}

// ********* area cosine hyperbolicus ************
constant * evaluate::arcosh_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  _RETC (acosh (nr_complex_t (d1)));
}

constant * evaluate::arcosh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acosh (*c1));
}

constant * evaluate::arcosh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acosh (*v1));
}

// ********* area secans hyperbolicus ***********
constant * evaluate::arsech_d (constant * args) {
  _ARD0 (d1);
  _DEFC ();
  _RETC (asech (nr_complex_t (d1)));
}

constant * evaluate::arsech_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (asech (*c1));
}

constant * evaluate::arsech_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (asech (*v1));
}

// ******* area tangent hyperbolicus **********
constant * evaluate::artanh_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (0.5 * log ((1.0 + d1) / (1.0 - d1)));
}

constant * evaluate::artanh_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (atanh (*c1));
}

constant * evaluate::artanh_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (atanh (*v1));
}

// ******* area cotangent hyperbolicus **********
constant * evaluate::arcoth_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (0.5 * log ((d1 + 1.0) / (d1 - 1.0)));
}

constant * evaluate::arcoth_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (acoth (*c1));
}

constant * evaluate::arcoth_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  _RETV (acoth (*v1));
}

// This is the rtoz, ztor, ytor, rtoy helper macro.
#define MAKE_FUNC_DEFINITION_2(cfunc) \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d) (constant * args) {   \
  _ARD0 (d);							    \
  _DEFD ();							    \
  _RETD (real (cfunc (rect (d, 0))));				    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_d) (constant * args) { \
  _ARD0 (d);							    \
  _ARD1 (z);							    \
  _DEFD ();							    \
  _RETD (real (cfunc (rect (d, 0), z)));			    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_d_c) (constant * args) { \
  _ARD0 (d);							    \
  _ARC1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (rect (d, 0), *z));				    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c) (constant * args) {   \
  _ARC0 (c);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c));						    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_d) (constant * args) { \
  _ARC0 (c);							    \
  _ARD1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c, z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_c_c) (constant * args) { \
  _ARC0 (c);							    \
  _ARC1 (z);							    \
  _DEFC ();							    \
  _RETC (cfunc (*c, *z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v) (constant * args) {   \
  _ARV0 (v);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v));						    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_d) (constant * args) { \
  _ARV0 (v);							    \
  _ARD1 (z);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v, z));					    \
}								    \
constant * evaluate:: QUCS_CONCAT2 (cfunc,_v_c) (constant * args) { \
  _ARV0 (v);							    \
  _ARC1 (z);							    \
  _DEFV ();							    \
  _RETV (cfunc (*v, *z));					    \
}								    \

MAKE_FUNC_DEFINITION_2 (ztor);
MAKE_FUNC_DEFINITION_2 (rtoz);
MAKE_FUNC_DEFINITION_2 (ytor);
MAKE_FUNC_DEFINITION_2 (rtoy);

// ** convert reflexion coefficient to standing wave ratio **
constant * evaluate::rtoswr_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD ((1 + fabs (d1)) / (1 - fabs (d1)));
}

constant * evaluate::rtoswr_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  _RETD ((1 + abs (*c1)) / (1 - abs (*c1)));
}

constant * evaluate::rtoswr_v (constant * args) {
  _ARV0 (v1);
  _DEFV ();
  res->v = new vector (v1->getSize ());
  for (int i = 0; i < v1->getSize (); i++)
    res->v->set ((1 + abs (v1->get (i))) / (1 - abs (v1->get (i))), i);
  return res;
}

// ** differentiate vector with respect to another vector **
constant * evaluate::diff_v_2 (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _DEFV ();
  _RETV (diff (*v1, *v2));
}

constant * evaluate::diff_v_3 (constant * args) {
  _ARV0 (v1);
  _ARV1 (v2);
  _ARI2 (i3);
  _DEFV ();
  _RETV (diff (*v1, *v2, i3));
}

// ***************** maximum *******************
constant * evaluate::max_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::max_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  if (fabs (arg (*c1)) < M_PI_2)
    res->d = abs (*c1);
  else
    res->d = -abs (*c1);
  return res;
}

constant * evaluate::max_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->maximum ());
}

constant * evaluate::max_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (MAX (d1, d2));
}

constant * evaluate::max_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = d1;
  nr_double_t b = fabs (arg (*c2)) < M_PI_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a > b ? d1 : *c2;
  _RETC (r);
}

constant * evaluate::max_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < M_PI_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = fabs (arg (*c2)) < M_PI_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a > b ? *c1 : *c2;
  _RETC (r);
}

constant * evaluate::max_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < M_PI_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = d2;
  nr_complex_t r = a > b ? *c1 : d2;
  _RETC (r);
}

// ***************** minimum *******************
constant * evaluate::min_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::min_c (constant * args) {
  _ARC0 (c1);
  _DEFD ();
  if (fabs (arg (*c1)) < M_PI_2)
    res->d = abs (*c1);
  else
    res->d = -abs (*c1);
  return res;
}

constant * evaluate::min_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->minimum ());
}

constant * evaluate::min_d_d (constant * args) {
  _ARD0 (d1);
  _ARD1 (d2);
  _DEFD ();
  _RETD (MIN (d1, d2));
}

constant * evaluate::min_d_c (constant * args) {
  _ARD0 (d1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = d1;
  nr_double_t b = fabs (arg (*c2)) < M_PI_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a < b ? d1 : *c2;
  _RETC (r);
}

constant * evaluate::min_c_c (constant * args) {
  _ARC0 (c1);
  _ARC1 (c2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < M_PI_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = fabs (arg (*c2)) < M_PI_2 ? abs (*c2) : -abs (*c2);
  nr_complex_t r = a < b ? *c1 : *c2;
  _RETC (r);
}

constant * evaluate::min_c_d (constant * args) {
  _ARC0 (c1);
  _ARD1 (d2);
  _DEFC ();
  nr_double_t a = fabs (arg (*c1)) < M_PI_2 ? abs (*c1) : -abs (*c1);
  nr_double_t b = d2;
  nr_complex_t r = a < b ? *c1 : d2;
  _RETC (r);
}

// ******************** sum **********************
constant * evaluate::sum_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::sum_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::sum_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (sum (*v1));
}

// ****************** product ********************
constant * evaluate::prod_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::prod_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::prod_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (prod (*v1));
}

// ******************* average *********************
constant * evaluate::avg_d (constant * args) {
  _ARD0 (d1);
  _DEFD ();
  _RETD (d1);
}

constant * evaluate::avg_c (constant * args) {
  _ARC0 (c1);
  _DEFC ();
  _RETC (*c1);
}

constant * evaluate::avg_v (constant * args) {
  _ARV0 (v1);
  _DEFC ();
  _RETC (avg (*v1));
}

// ******************* lengths *********************
constant * evaluate::length_d (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_c (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_v (constant * args) {
  _ARV0 (v1);
  _DEFD ();
  _RETD (v1->getSize ());
}

constant * evaluate::length_m (constant *) {
  _DEFD ();
  _RETD (1);
}

constant * evaluate::length_mv (constant * args) {
  _ARV0 (mv);
  _DEFD ();
  _RETD (mv->getSize ());
}

// ***************** array indices *****************
constant * evaluate::index_mv_2 (constant * args) {
  _ARMV0 (mv);
  _ARI1 (r);
  _ARI2 (c);
  _DEFV ();
  if (r < 1 || r > mv->getRows () || c < 1 || c > mv->getCols ()) {
    char txt[256];
    sprintf (txt, "matvec indices [%d,%d] out of bounds [1-%d,1-%d]",
	     r, c, mv->getRows (), mv->getCols ());
    THROW_MATH_EXCEPTION (txt);
    res->v = new vector (mv->getSize ());
  } else {
    res->v = new vector (mv->get (r - 1, c - 1));
  }
  return res;
}

constant * evaluate::index_mv_1 (constant * args) {
  _ARMV0 (mv);
  _ARI1 (i);
  _DEFM ();
  if (i < 1 || i > mv->getSize ()) {
    char txt[256];
    sprintf (txt, "matvec index [%d] out of bounds [1-%d]", i, mv->getSize ());
    THROW_MATH_EXCEPTION (txt);
    res->m = new matrix (mv->getRows (), mv->getCols ());
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
  _ARV0 (v);
  int i = INT (_ARES (idx));
  int type = _ARG(idx)->getType ();
  vector * vres;
  strlist * deps = _ARES(0)->getDataDependencies ();
  int didx = (deps ? deps->length () : 0) - idx;
  int dsize = SOLVEE(0)->getDependencySize (deps, idx);

  // all of the data vector
  if (type == TAG_RANGE) {
    if (dsize > 1) {
      // dependent vectors: only ':' possible
      vres = new vector (*(res->v));
      skip *= deps ? SOLVEE(0)->getDataSize (deps->get (didx - 1)) : 1;
      size *= deps ? SOLVEE(0)->getDataSize (deps->get (didx)) : 1;
    }
    else {
      // independent vectors
      range * r = RNG (_ARES (idx));
      int i, n, k;

constant * evaluate::notequal_d_c (constant * args) {
  _ARD0 (d0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (d0 != *c1);
}

constant * evaluate::notequal_d_v (constant * args) {
  _ARD0 (d0);
  _ARV1 (v1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (d0 != real (v1->get (i)) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_c_d (constant * args) {
  _ARC0 (c0);
  _ARD1 (d1);
  _DEFB ();
  _RETB (*c0 != d1);
}

constant * evaluate::notequal_c_c (constant * args) {
  _ARC0 (c0);
  _ARC1 (c1);
  _DEFB ();
  _RETB (*c0 != *c1);
}

constant * evaluate::notequal_c_v (constant * args) {
  _ARC0 (c0);
  _ARV1 (v1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v1->getSize (); i++) {
    v->add (*c0 != v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_d (constant * args) {
  _ARV0 (v0);
  _ARD1 (d1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (real (v0->get (i)) != d1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_c (constant * args) {
  _ARV0 (v0);
  _ARC1 (c1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) != *c1 ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}

constant * evaluate::notequal_v_v (constant * args) {
  _ARV0 (v0);
  _ARV1 (v1);
  _DEFV ();
  vector * v = new vector ();
  for (int i = 0; i < v0->getSize (); i++) {
    v->add (v0->get (i) != v1->get (i) ? 1.0 : 0.0);
  }
  res->v = v;
  return res;
}


constant * evaluate::notequal_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 != b1);
}

// *************************** not *************************
constant * evaluate::not_b (constant * args) {
  _ARB0 (b0);
  _DEFB ();
  _RETB (!b0);
}

// *************************** or **************************
constant * evaluate::or_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 || b1);
}

// ************************** and **************************
constant * evaluate::and_b_b (constant * args) {
  _ARB0 (b0);
  _ARB1 (b1);
  _DEFB ();
  _RETB (b0 && b1);
}

// ******************* random numbers **********************
constant * evaluate::rand (constant *) {
  _DEFD ();
  _RETD (((nr_double_t) ::rand ()) / (nr_double_t) RAND_MAX);
}

constant * evaluate::srand_d (constant * args) {
  static int done = 0;
  _ARD0 (d0);
  _DEFD ();
  if (!done) {
    unsigned int i0 = (unsigned int) d0;
    ::srand (i0);
    done = 1;
    _RETD (1.0);
  } else {
    _RETD (0.0);
  }
}

// ******************* immediate vectors *******************
constant * evaluate::vector_x (constant * args) {
  _DEFV ();
  vector * v = new vector ();
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    constant * c = arg->getResult ();
    switch (arg->getType ()) {
    case TAG_COMPLEX:
      v->add (*(c->c)); break;
    case TAG_DOUBLE:
      v->add (c->d); break;
    case TAG_BOOLEAN:
      v->add (c->b ? 1.0 : 0.0); break;
    case TAG_VECTOR:
      v->add (c->v); break;
    default:
      v->add (0.0); break;
    }
  }
  res->v = v;
  return res;
}

// ******************* immediate matrices ******************
constant * evaluate::matrix_x (constant * args) {
  _DEFM ();
  /* create temporary list of vectors */
  vector * va = NULL;
  vector * v = new vector ();
  va = v;
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    constant * c = arg->getResult ();
    switch (arg->getType ()) {
    case TAG_COMPLEX:
      v->add (*(c->c)); break;
    case TAG_DOUBLE:
      v->add (c->d); break;
    case TAG_BOOLEAN:
      v->add (c->b ? 1.0 : 0.0); break;
    case TAG_VECTOR:
      v->add (c->v); break;
    case TAG_CHAR:
      if (c->chr == ';') {
	/* append new vector, i.e. a new matrix row */
	vector * vn = new vector ();
	v->setNext (vn);
	v = vn;
      }
      else v->add (0.0);
      break;
    default:
      v->add (0.0); break;
    }
  }
  /* determine matrix dimensions and create it */
  int r, c;
  for (r = 0, c = 0, v = va; v != NULL; v = (vector *) v->getNext (), r++) {
    if (c < v->getSize ()) c = v->getSize ();
  }
  matrix * m = new matrix (r, c);
  /* fill in matrix entries and delete temporary vector list */
  vector * vn = NULL;
  for (r = 0, v = va; v != NULL; v = vn, r++) {
    for (c = 0; c < v->getSize (); c++) {
      m->set (r, c, v->get (c));
    }
    vn = (vector *) v->getNext ();
    delete v;
  }
  /* return result matrix */
  res->m = m;
  return res;
}

// ********************** EMI receiver *********************
constant * evaluate::receiver_v_v (constant * args) {
  _ARV0 (da);
  _ARV1 (dt);
  _DEFV ();

  // run receiver functionality
  vector * ed;
  if (_ARG(2)) {
    _ARI2 (len);
    ed = emi::receiver (da, dt, len);
  }
  else {
    ed = emi::receiver (da, dt);
  }

  // create two vectors for spectrum and frequency
  int rlen = ed->getSize ();
  vector * rvec = new vector (rlen);
  vector * rfeq = new vector (rlen);
  for (int i = 0; i < rlen; i++) {
    (*rvec)(i) = real (ed->get (i));
    (*rfeq)(i) = imag (ed->get (i));
  }
  delete ed;

  // put results back into equation solver
  node * gen = SOLVEE(0)->addGeneratedEquation (rfeq, "Frequency");
  res->addPrepDependencies (A(gen)->result);
  res->dropdeps = 1;
  res->v = rvec;
  return res;
}

// Include the application array.
#include "applications.h"
