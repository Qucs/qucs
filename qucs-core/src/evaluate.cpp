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
 * $Id: evaluate.cpp,v 1.1 2004-03-21 18:55:48 ela Exp $
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

// Short macros in order to obtains the correct constant value.
#define D(con) ((constant *) (con))->d
#define C(con) ((constant *) (con))->c
#define V(con) ((constant *) (con))->v
#define M(con) ((constant *) (con))->m

constant * evaluate::plus_d_d (constant * args) {
  nr_double_t d1 = D (args->get (0));
  nr_double_t d2 = D (args->get (1));
  constant * res = new constant (TAG_DOUBLE);
  res->d = d1 + d2;
  return res;
}

constant * evaluate::plus_c_d (constant * args) {
  complex *   c1 = C (args->get (0));
  nr_double_t d2 = D (args->get (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (*c1 + d2);
  return res;
}

constant * evaluate::plus_d_c (constant * args) {
  nr_double_t d1 = D (args->get (0));
  complex *   c2 = C (args->get (1));
  constant * res = new constant (TAG_COMPLEX);
  res->c = new complex (d1 + *c2);
  return res;
}

// Array containing all kinds of applications.
struct application_t eqn::applications[] = {
  { "+", TAG_DOUBLE,  evaluate::plus_d_d, 2, { TAG_DOUBLE,  TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_c_d, 2, { TAG_COMPLEX, TAG_DOUBLE  } },
  { "+", TAG_COMPLEX, evaluate::plus_d_c, 2, { TAG_DOUBLE,  TAG_COMPLEX } },
  { NULL, 0, NULL, 0, { } /* end of list */ }
};
