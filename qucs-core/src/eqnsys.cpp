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
 * $Id: eqnsys.cpp,v 1.16 2004/09/23 14:09:11 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "logging.h"
#include "complex.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "constants.h"
#include "exception.h"
#include "exceptionstack.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
#endif

using namespace qucs;

// Constructor creates an unnamed instance of the eqnsys class.
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys () {
  A = B = X = NULL;
}

// Destructor deletes the eqnsys class object.
template <class nr_type_t>
eqnsys<nr_type_t>::~eqnsys () {
  if (A != NULL) delete A;
  if (B != NULL) delete B;
}

/* The copy constructor creates a new instance of the eqnsys class
   based on the given eqnsys object. */
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys (eqnsys & e) {
  if (e.A != NULL) A = new tmatrix<nr_type_t> (*(e.A));
  if (e.B != NULL) B = new tmatrix<nr_type_t> (*(e.B));
  X = e.X;
}

/* With this function the describing matrices for the equation system
   is passed to the equation system solver class.  Matrix A is the
   left hand side of the equation system and B the right hand side
   vector.  The reference pointer to the X matrix is going to be the
   solution vector.  Both the matrices A and B will be locally
   copied. */
template <class nr_type_t>
void eqnsys<nr_type_t>::passEquationSys (tmatrix<nr_type_t> * nA,
					 tmatrix<nr_type_t> * refX,
					 tmatrix<nr_type_t> * nB) {
  if (A != NULL) delete A;
  if (B != NULL) delete B;
  A = new tmatrix<nr_type_t> (*nA);
  B = new tmatrix<nr_type_t> (*nB);
  X = refX;
}

/* Depending on the algorithm applied to the equation system solver
   the function stores the solution of the system into the matrix
   pointed to by the X matrix reference. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve (void) {
#if DEBUG && 0
  time_t t = time (NULL);
#endif  
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
  case ALGO_JACOBI: case ALGO_GAUSS_SEIDEL:
    solve_iterative ();
    break;
  case ALGO_SOR:
    solve_sor ();
    break;
  }
#if DEBUG && 0
  logprint (LOG_STATUS, "NOTIFY: %dx%d eqnsys solved in %ld seconds\n",
	    A->getRows (), A->getCols (), time (NULL) - t);
#endif
}

/* Simple matrix inversion is used to solve the equation system. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_inverse (void) {
  *X = inverse (*A) * *B;
}

/* The function solves the equation system applying Gaussian
   elimination with full column pivoting only (no row pivoting). */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_gauss (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
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
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_gauss_jordan (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
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
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_lu (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
  int k, i, c, r, pivot, N = A->getCols ();
  tmatrix<nr_type_t> * Y = new tmatrix<nr_type_t> (N, 1);
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
#if 0
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during LU decomposition");
      e->setData (i);
      throw_exception (e);
      goto fail;
#else /* insert virtual resistance */
      qucs::exception * e = new qucs::exception (EXCEPTION_SINGULAR);
      e->setText ("no pivot != 0 found during LU decomposition");
      e->setData (i);
      A->set (i, i, 1e-12); // virtual resistance to ground
      throw_exception (e);
#endif
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
   method (called Jacobi's method) or a single-step method (called
   Gauss-Seidel) depending on the given algorithm.  If the current X
   vector (the solution vector) is the solution within a
   Newton-Raphson iteration it is a good initial guess and the method
   is very likely to converge.  On divergence the method falls back to
   LU decomposition. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_iterative (void) {
  nr_type_t f;
  int error, conv, i, c, r, N = A->getCols ();
  int MaxIter = N; // -> less than N^3 operations
  nr_double_t reltol = 1e-4;
  nr_double_t abstol = 1e-12;
  nr_double_t diff, crit;

  // ensure that all diagonal values are non-zero
  ensure_diagonal ();

  // try to raise diagonal dominance
  preconditioner ();

  // decide here about possible convergence
  if ((crit = convergence_criteria ()) >= 1) {
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: convergence criteria: %g >= 1 (%dx%d)\n",
	      crit, N, N);
#endif
    //solve_lu ();
    //return;
  }

  // normalize the equation system to have ones on its diagonal
  for (r = 1; r <= N; r++) {
    f = A->get (r, r);
    assert (f != 0); // singular matrix
    for (c = 1; c <= N; c++) A->set (r, c, A->get (r, c) / f);
    B->set (r, 1, B->get (r, 1) / f);
  }

  // the current X vector is a good initial guess for the iteration
  tmatrix<nr_type_t> * Xprev = new tmatrix<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 1; r <= N; r++) {
      for (f = 0, c = 1; c <= N; c++) {
	if (algo == ALGO_GAUSS_SEIDEL) {
	  // Gauss-Seidel
	  if (c < r)      f += A->get (r, c) * X->get (c, 1);
	  else if (c > r) f += A->get (r, c) * Xprev->get (c, 1);
	}
	else {
	  // Jacobi
	  if (c != r) f += A->get (r, c) * Xprev->get (c, 1);
	}
      }
      X->set (r, 1, B->get (r, 1) - f);
    }
    // check for convergence
    for (conv = 1, r = 1; r <= N; r++) {
      diff = abs (X->get (r, 1) - Xprev->get (r, 1));
      if (diff >= abstol + reltol * abs (X->get (r, 1))) {
	conv = 0;
	break;
      }
      if (!finite (diff)) { error++; break; }
    }
    // save last values
    *Xprev = *X;
  }
  while (++i < MaxIter && !conv);

  delete Xprev;

  if (!conv || error) {
    logprint (LOG_ERROR,
	      "WARNING: no convergence after %d %s iterations\n",
	      i, algo == ALGO_JACOBI ? "jacobi" : "gauss-seidel");
    solve_lu ();
  }
#if DEBUG
  else {
    logprint (LOG_STATUS,
	      "NOTIFY: %s convergence after %d iterations\n",
	      algo == ALGO_JACOBI ? "jacobi" : "gauss-seidel", i);
  }
#endif
}

/* The function solves the linear equation system using a single-step
   iterative algorithm.  It is a modification of the Gauss-Seidel
   method and is called successive over relaxation.  The function uses
   an adaptive scheme to adjust the relaxation parameter. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_sor (void) {
  nr_type_t f;
  int error, conv, i, c, r, N = A->getCols ();
  int MaxIter = N; // -> less than N^3 operations
  nr_double_t reltol = 1e-4;
  nr_double_t abstol = 1e-12;
  nr_double_t diff, crit, l = 1, d, s;

  // ensure that all diagonal values are non-zero
  ensure_diagonal ();

  // try to raise diagonal dominance
  preconditioner ();

  // decide here about possible convergence
  if ((crit = convergence_criteria ()) >= 1) {
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: convergence criteria: %g >= 1 (%dx%d)\n",
	      crit, N, N);
#endif
    //solve_lu ();
    //return;
  }

  // normalize the equation system to have ones on its diagonal
  for (r = 1; r <= N; r++) {
    f = A->get (r, r);
    assert (f != 0); // singular matrix
    for (c = 1; c <= N; c++) A->set (r, c, A->get (r, c) / f);
    B->set (r, 1, B->get (r, 1) / f);
  }

  // the current X vector is a good initial guess for the iteration
  tmatrix<nr_type_t> * Xprev = new tmatrix<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 1; r <= N; r++) {
      for (f = 0, c = 1; c <= N; c++) {
	if (c < r)      f += A->get (r, c) * X->get (c, 1);
	else if (c > r) f += A->get (r, c) * Xprev->get (c, 1);
      }
      X->set (r, 1, (1 - l) * Xprev->get (r, 1) + l * (B->get (r, 1) - f));
    }
    // check for convergence
    for (s = 0, d = 0, conv = 1, r = 1; r <= N; r++) {
      diff = abs (X->get (r, 1) - Xprev->get (r, 1));
      if (diff >= abstol + reltol * abs (X->get (r, 1))) {
	conv = 0;
	break;
      }
      d += diff; s += abs (X->get (r, 1));
      if (!finite (diff)) { error++; break; }
    }
    if (!error) {
      // adjust relaxation based on average errors
      if ((s == 0 && d == 0) || d >= abstol * N + reltol * s) {
	// values <= 1 -> non-convergence to convergence
	if (l >= 0.6) l -= 0.1;
	if (l >= 1.0) l = 1.0;
      }
      else {
	// values >= 1 -> faster convergence
	if (l < 1.5) l += 0.01;
	if (l < 1.0) l = 1.0;
      }
    }
    // save last values
    *Xprev = *X;
  }
  while (++i < MaxIter && !conv);

  delete Xprev;

  if (!conv || error) {
    logprint (LOG_ERROR,
	      "WARNING: no convergence after %d sor iterations (l = %g)\n",
	      i, l);
    solve_lu ();
  }
#if DEBUG
  else {
    logprint (LOG_STATUS,
	      "NOTIFY: sor convergence after %d iterations\n", i);
  }
#endif
}

/* The function computes the convergence criteria for iterative
   methods like Jacobi or Gauss-Seidel as defined by Schmidt and
   v.Mises. */
template <class nr_type_t>
nr_double_t eqnsys<nr_type_t>::convergence_criteria (void) {
  nr_double_t f = 0;
  for (int r = 1; r <= A->getCols (); r++) {
    for (int c = 1; c <= A->getCols (); c++) {
      if (r != c) f += norm (A->get (r, c) / A->get (r, r));
    }
  }
  return sqrt (f);
}

/* The function tries to ensure that there are non-zero diagonal
   elements in the equation system matrix A.  This is required for
   iterative solution methods. */
template <class nr_type_t>
void eqnsys<nr_type_t>::ensure_diagonal (void) {
  ensure_diagonal_MNA ();
}

/* The function ensures that there are non-zero diagonal elements in
   the equation system matrix A.  It achieves this condition for
   non-singular matrices which have been produced by the modified
   nodal analysis.  It takes advantage of the fact that the zero
   diagonal elements have pairs of 1 und -1 on the same column and
   row. */
template <class nr_type_t>
void eqnsys<nr_type_t>::ensure_diagonal_MNA (void) {
  int restart, exchanged, begin = 1, pairs;
  int pivot1, pivot2, i, N = A->getCols ();
  do {
    restart = exchanged = 0;
    /* search for zero diagonals with lone pairs */
    for (i = begin; i <= N; i++) {
      if (A->get (i, i) == 0) {
	pairs = countPairs (i, pivot1, pivot2);
	if (pairs == 1) { /* lone pair found, substitute rows */
	  A->exchangeRows (i, pivot1);
	  B->exchangeRows (i, pivot1);
	  exchanged = 1;
	}
	else if ((pairs > 1) && !restart) {
	  restart = 1;
	  begin = i;
	}
      }
    }

    /* all lone pairs are gone, look for zero diagonals with multiple pairs */
    if (restart) {
      for (i = begin; !exchanged && i <= N; i++) {
	if (A->get (i, i) == 0) {
	  pairs = countPairs (i, pivot1, pivot2);
	  A->exchangeRows (i, pivot1);
	  B->exchangeRows (i, pivot1);
	  exchanged = 1;
	}
      }
    }
  }
  while (restart);
}

/* Helper function for the above ensure_diagonal_MNA() function.  It
   looks for the pairs of 1 and -1 on the given row and column index. */
template <class nr_type_t>
int eqnsys<nr_type_t>::countPairs (int i, int& r1, int& r2) {
  int pairs = 0, N = A->getCols ();
  for (int r = 1; r <= N; r++) {
    if (fabs (real (A->get (r, i))) == 1.0) {
      r1 = r;
      pairs++;
      for (r++; r <= N; r++) {
	if (fabs (real (A->get (r, i))) == 1.0) {
	  r2 = r;
	  if (++pairs >= 2) return pairs;
	}
      }
    }
  }
  return pairs;
}

/* The function tries to raise the absulute value of diagonal elements
   by swapping rows and thereby make the A matrix diagonally
   dominant. */
template <class nr_type_t>
void eqnsys<nr_type_t>::preconditioner (void) {
  int pivot, r, N = A->getCols ();
  nr_double_t MaxPivot;
  for (int i = 1; i <= N; i++) {
    // find maximum column value for pivoting
    for (MaxPivot = 0, pivot = i, r = 1; r <= N; r++) {
      if (abs (A->get (r, i)) > MaxPivot && 
	  abs (A->get (i, r)) >= abs (A->get (r, r))) {
        MaxPivot = abs (A->get (r, i));
        pivot = r;
      }
    }
    // swap matrix rows if possible
    if (i != pivot) {
      A->exchangeRows (i, pivot);
      B->exchangeRows (i, pivot);
    }
  }
}
