/*
 * eqnsys.cpp - equation system solver class implementation
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
 * $Id: eqnsys.cpp,v 1.7 2004-07-31 16:59:14 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <assert.h>

#include "logging.h"
#include "complex.h"
#include "matrix.h"
#include "eqnsys.h"
#include "constants.h"
#include "exception.h"
#include "exceptionstack.h"

using namespace qucs;

// Constructor creates an unnamed instance of the eqnsys class.
eqnsys::eqnsys () {
  A = B = X = NULL;
}

// Destructor deletes the eqnsys class object.
eqnsys::~eqnsys () {
  if (A != NULL) delete A;
  if (B != NULL) delete B;
}

/* The copy constructor creates a new instance of the eqnsys class
   based on the given eqnsys object. */
eqnsys::eqnsys (eqnsys & e) {
  if (e.A != NULL) A = new matrix (*(e.A));
  if (e.B != NULL) B = new matrix (*(e.B));
  X = e.X;
}

/* With this function the describing matrices for the equation system
   is passed to the equation system solver class.  Matrix A is the
   left hand side of the equation system and B the right hand side
   vector.  The reference pointer to the X matrix is going to be the
   solution vector.  Both the matrices A and B will be locally
   copied. */
void eqnsys::passEquationSys (matrix * nA, matrix * refX, matrix * nB) {
  if (A != NULL) delete A;
  if (B != NULL) delete B;
  A = new matrix (*nA);
  B = new matrix (*nB);
  X = refX;
}

/* Depending on the algorithm applied to the equation system solver
   the function stores the solution of the system into the matrix
   pointed to by the X matrix reference. */
void eqnsys::solve (void) {
  switch (algo) {
  case ALGO_INVERSE:
    solve_inverse ();
    break;
  case ALGO_GAUSS:
    solve_gauss ();
    break;
  case ALGO_GAUSS_JORDAN:
    solve_gauss_jordan ();
    break;
  case ALGO_LU_DECOMPOSITION:
    solve_lu ();
    break;
  case ALGO_JACOBI:
    solve_jacobi ();
    break;
  }
}

/* Simple matrix inversion is used to solve the equation system. */
void eqnsys::solve_inverse (void) {
  *X = inverse (*A) * *B;
}

/* The function solves the equation system applying Gaussian
   elimination with full column pivoting only (no row pivoting). */
void eqnsys::solve_gauss (void) {
  nr_double_t MaxPivot;
  complex f;
  int i, c, r, pivot, N = A->getCols ();
  
  // triangulate the matrix
  for (i = 1; i < N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r <= N; r++) {
      if (abs (A->get (r, i)) > MaxPivot) {
	MaxPivot = abs (A->get (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }
    // compute new rows and columns
    for (r = i + 1; r <= N; r++) {
      f = A->get (r, i) / A->get (i, i);
      for (c = i + 1; c <= N; c++) {
	A->set (r, c, A->get (r, c) - f * A->get (i, c));
      }
      B->set (r, 1, B->get (r, 1) - f * B->get (i, 1));
    }
  }

  // backward substitution
  for (i = N; i > 0; i--) {
    f = B->get (i, 1);
    for (c = i + 1; c <= N; c++) {
      f -= A->get (i, c) * X->get (c, 1);
    }
    f /= A->get (i, i);
    X->set (i, 1, f);
  }
}

/* The function solves the equation system applying a modified
   Gaussian elimination with full column pivoting only (no row
   pivoting). */
void eqnsys::solve_gauss_jordan (void) {
  nr_double_t MaxPivot;
  complex f;
  int i, c, r, pivot, N = A->getCols ();

  // create the eye matrix
  for (i = 1; i <= N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r <= N; r++) {
      if (abs (A->get (r, i)) > MaxPivot) {
	MaxPivot = abs (A->get (r, i));
	pivot = r;
      }
    }
    // exchange rows if necessary
    assert (MaxPivot != 0);
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }

    // compute current row
    f = A->get (i, i);
    for (c = i + 1; c <= N; c++) {
      A->set (i, c, A->get (i, c) / f);
    }
    B->set (i, 1, B->get (i, 1) / f);

    // compute new rows and columns
    for (r = 1; r <= N; r++) {
      if (r != i) {
	f = A->get (r, i);
        for (c = i + 1; c <= N; c++) {
          A->set (r, c, A->get (r, c) - f * A->get (i, c));
        }
        B->set (r, 1, B->get (r, 1) - f * B->get (i, 1));
      }
    }
  }

  // right hand side is now the solution
  *X = *B;
}

/* This function decomposites the left hand matrix into an upper U and
   lower L matrix.  The algorithm is called LU decomposition.  It is
   very useful when dealing with equation systems where the left hand
   side (the A matrix) does not change but the right hand side (the B
   vector) only. */
void eqnsys::solve_lu (void) {
  nr_double_t MaxPivot;
  complex f;
  int k, i, c, r, pivot, N = A->getCols ();
  matrix * Y = new matrix (N, 1);
  int * change = new int[N];

  // initialize pivot exchange table
  for (i = 1; i <= N; i++) change[i - 1] = i;

  // decomposite the matrix into L (lower) and U (upper) matrix
  for (i = 1, c = 2; c <= N; c++, i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = r = i; r <= N; r++) {
      if (abs (A->get (r, i)) > MaxPivot) {
        MaxPivot = abs (A->get (r, i));
        pivot = r;
      }
    }
    // check pivot element and throw appropriate exception
    if (MaxPivot <= 0) {
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during LU decomposition");
      e->setData (c);
      throw_exception (e);
      goto fail;
    }

    // swap matrix rows if necessary and remember that step in the
    // exchange table
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      k = change[i - 1];
      change[i - 1] = change[pivot - 1];
      change[pivot - 1] = k;
    }
    // compute new column
    for (r = 1; r <= N; r++) {
      if (r < c) {
        // upper matrix entries
        for (f = 0, k = 1; k <= r - 1; k++)
          f += A->get (r, k) * A->get (k, c);
        A->set (r, c, (A->get (r, c) - f) / A->get (r, r));
      }
      else {
        // lower matrix entries
        for (f = 0, k = 1; k <= c - 1; k++)
          f += A->get (r, k) * A->get (k, c);
        A->set (r, c, A->get (r, c) - f);
      }
    }
  }

  // forward substitution in order to solve LY = B
  for (i = 1; i <= N; i++) {
    f = B->get (change[i - 1], 1);
    for (c = 1; c <= i - 1; c++)
      f -= A->get (i, c) * Y->get (c, 1);
    f /= A->get (i, i);

    // check for possible division by zero
    if (A->get (i, i) == 0) {
      qucs::exception * e = new qucs::exception (EXCEPTION_WRONG_VOLTAGE);
      e->setText ("forward substitution failed in LU decomposition");
      e->setData (i);
      throw_exception (e);
      goto fail;
    }
    Y->set (i, 1, f);
  }
   
  // backward substitution in order to solve UX = Y
  for (i = N; i > 0; i--) {
    f = Y->get (i, 1);
    for (c = i + 1; c <= N; c++)
      f -= A->get (i, c) * X->get (c, 1);
    // remember that the Uii diagonal are ones only
    X->set (i, 1, f);
  }

 fail:
  delete Y;
  delete change;
}

/* The function solves the equation system using a full-step iterative
   method (called Jacobi's method). */
void eqnsys::solve_jacobi (void) {
  nr_double_t MaxPivot;
  complex f;
  int conv, k, i, c, r, rpivot, cpivot, N = A->getCols ();
  int MaxIter = 350;
  nr_double_t reltol = 1e-3;
  nr_double_t abstol = 1e-12;
  int * change = new int[N];

  // initialize pivot exchange table
  for (i = 1; i <= N; i++) change[i - 1] = i;

  // TODO: Find algorithm to ensure that!!!
  // ensure that all diagonal values are non-zero
  for (i = 1; i <= N; i++) {
    //fprintf (stderr, "not pivoted:\n");
    //A->print ();
    for (MaxPivot = 0, rpivot = cpivot = i, r = i; r <= N; r++) {
      for (c = 1; c <= N; c++) {
	if (abs (A->get (r, c)) > MaxPivot && abs (A->get (c, r)) > 0) {
	  MaxPivot = abs (A->get (r, c));
	  rpivot = r;
	  cpivot = c;
	}
      }
    }
    if (MaxPivot <= 0) {
      //A->print ();
    }
    assert (MaxPivot > 0);
    if (cpivot != i) {
      A->exchangeCols (i, cpivot);
      k = change[i - 1];
      change[i - 1] = change[cpivot - 1];
      change[cpivot - 1] = k;
      //fprintf (stderr, "exchanged c: %d-%d\n", i, cpivot);
      //A->print ();
    }
    if (rpivot != i) {
      A->exchangeRows (i, rpivot);
      //fprintf (stderr, "exchanged r: %d-%d\n", i, rpivot);
      //A->print ();
    }
  }

  for (f = 1, i = 1; i <= N; i++) f *= A->get (i, i);
  //fprintf (stderr, "pivoted:\n");
  //A->print ();
  assert (f != 0);
  if (criteria_schmidt_mises () >= 1) {
    solve_lu ();
    return;
  }

  matrix * Xprev = new matrix (*X);
  for (r = 1; r <= N; r++) Xprev->set (r, 1, 0);
  i = 0;
  do {
    for (r = 1; r <= N; r++) {
      for (f = 0, c = 1; c <= N; c++) {
	if (c != r) {
	  f += A->get (r, c) * Xprev->get (c, 1);
	}
      }
      X->set (r, 1, (B->get (change[r - 1], 1) - f) / A->get (r, r));
    }
    //fprintf (stderr, "NOTE: step %d\n", i);
    //X->print ();
    for (conv = 1, r = 1; r <= N; r++) {
      if (abs (X->get (r, 1) - Xprev->get (r, 1)) >= 
	  abstol + reltol * abs (X->get (r, 1))) {
	conv = 0;
	break;
      }
    }
    *Xprev = *X;
  }
  while (i++ < MaxIter && !conv);
#if DEBUG
  if (!conv) {
    fprintf (stderr, "ERROR: no convergence after %d iterations\n", i);
  }
  else {
    fprintf (stderr, "NOTE: convergence after %d iterations\n", i);
  }
  //X->print ();
#endif
  delete change;
  delete Xprev;
}

nr_double_t eqnsys::criteria_schmidt_mises (void) {
  complex f = 0;
  for (int r = 1; r <= A->getCols (); r++) {
    for (int c = 1; c <= A->getCols (); c++) {
      if (r != c) f += sqr (A->get (r, c) / A->get (r, r));
    }
  }
  return abs (f);
}
