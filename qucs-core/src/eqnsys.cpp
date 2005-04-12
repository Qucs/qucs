/*
 * eqnsys.cpp - equation system solver class implementation
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
 * $Id: eqnsys.cpp,v 1.28 2005-04-12 06:50:37 raimi Exp $
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
#include <float.h>

#if HAVE_IEEEFP_H
# include <ieeefp.h>
#endif

#include "logging.h"
#include "complex.h"
#include "tmatrix.h"
#include "eqnsys.h"
#include "exception.h"
#include "exceptionstack.h"

#ifdef __MINGW32__
# define finite(x) _finite(x)
#endif

#define TINY 1e-12

// Little helper macro.
#define Swap(type,a,b) { type t; t = a; a = b; b = t; }

using namespace qucs;

// Constructor creates an unnamed instance of the eqnsys class.
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys () {
  A = NULL;
  B = X = NULL;
  T = R = NULL;
  nPvt = NULL;
  cMap = rMap = NULL;
  update = 1;
  pivoting = PIVOT_PARTIAL;
  N = 0;
}

// Destructor deletes the eqnsys class object.
template <class nr_type_t>
eqnsys<nr_type_t>::~eqnsys () {
  if (R != NULL) delete R;
  if (T != NULL) delete T;
  if (B != NULL) delete B;
  if (rMap != NULL) delete rMap;
  if (cMap != NULL) delete cMap;
  if (nPvt != NULL) delete nPvt;
}

/* The copy constructor creates a new instance of the eqnsys class
   based on the given eqnsys object. */
template <class nr_type_t>
eqnsys<nr_type_t>::eqnsys (eqnsys & e) {
  A = e.A;
  T = R = NULL;
  B = e.B ? new tvector<nr_type_t> (*(e.B)) : NULL;
  cMap = rMap = NULL;
  nPvt = NULL;
  update = 1;
  X = e.X;
  N = 0;
}

/* With this function the describing matrices for the equation system
   is passed to the equation system solver class.  Matrix A is the
   left hand side of the equation system and B the right hand side
   vector.  The reference pointer to the X vector is going to be the
   solution vector.  The B vector will be locally copied. */
template <class nr_type_t>
void eqnsys<nr_type_t>::passEquationSys (tmatrix<nr_type_t> * nA,
					 tvector<nr_type_t> * refX,
					 tvector<nr_type_t> * nB) {
  if (nA != NULL) {
    A = nA;
    update = 1;
    if (N != A->getCols ()) {
      N = A->getCols ();
      if (cMap) delete cMap; cMap = new int[N];
      if (rMap) delete rMap; rMap = new int[N];
      if (nPvt) delete nPvt; nPvt = new nr_double_t[N];
    }
  }
  else {
    update = 0;
  }
  if (B != NULL) delete B;
  B = new tvector<nr_type_t> (*nB);
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
  case ALGO_LU_FACTORIZATION:
    factorize_lu_crout ();
    break;
  case ALGO_LU_SUBSTITUTION_CROUT:
    substitute_lu_crout ();
    break;
  case ALGO_LU_SUBSTITUTION_DOOLITTLE:
    substitute_lu_doolittle ();
    break;
  case ALGO_JACOBI: case ALGO_GAUSS_SEIDEL:
    solve_iterative ();
    break;
  case ALGO_SOR:
    solve_sor ();
    break;
  case ALGO_QR_DECOMPOSITION:
    solve_qr ();
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
  int i, c, r, pivot;
  
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
      B->set (r, B->get (r) - f * B->get (i));
    }
  }

  // backward substitution
  for (i = N; i > 0; i--) {
    f = B->get (i);
    for (c = i + 1; c <= N; c++) {
      f -= A->get (i, c) * X->get (c);
    }
    f /= A->get (i, i);
    X->set (i, f);
  }
}

/* The function solves the equation system applying a modified
   Gaussian elimination with full column pivoting only (no row
   pivoting). */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_gauss_jordan (void) {
  nr_double_t MaxPivot;
  nr_type_t f;
  int i, c, r, pivot;

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
    B->set (i, B->get (i) / f);

    // compute new rows and columns
    for (r = 1; r <= N; r++) {
      if (r != i) {
	f = A->get (r, i);
        for (c = i + 1; c <= N; c++) {
          A->set (r, c, A->get (r, c) - f * A->get (i, c));
        }
        B->set (r, B->get (r) - f * B->get (i));
      }
    }
  }

  // right hand side is now the solution
  *X = *B;
}

#define LU_FAILURE 0
#define VIRTUAL_RES(txt,i) {					  \
  qucs::exception * e = new qucs::exception (EXCEPTION_SINGULAR); \
  e->setText (txt);						  \
  e->setData (rMap[i - 1]);					  \
  A->set (i, i, TINY); /* virtual resistance to ground */	  \
  throw_exception (e); }

/* The function uses LU decomposition and the appropriate forward and
   backward substitutions in order to solve the linear equation
   system.  It is very useful when dealing with equation systems where
   the left hand side (the A matrix) does not change but the right
   hand side (the B vector) only. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_lu (void) {

  // skip decomposition if requested
  if (update) {
    // perform LU composition
    factorize_lu_crout ();
  }

  // finally solve the equation system
  substitute_lu_crout ();
}

/* This function decomposes the left hand matrix into an upper U and
   lower L matrix.  The algorithm is called LU decomposition (Crout's
   definition).  The function performs the actual LU decomposition of
   the matrix A using (implicit) partial row pivoting. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_lu_crout (void) {
  nr_double_t d, MaxPivot;
  nr_type_t f;
  int k, c, r, pivot;

  // initialize pivot exchange table
  for (r = 1; r <= N; r++) {
    for (MaxPivot = 0, c = 1; c <= N; c++)
      if ((d = abs (A->get (r, c))) > MaxPivot)
	MaxPivot = d;
    if (MaxPivot <= 0) MaxPivot = TINY;
    nPvt[r - 1] = 1 / MaxPivot;
    rMap[r - 1] = r;
  }

  // decompose the matrix into L (lower) and U (upper) matrix
  for (c = 1; c <= N; c++) {
    // upper matrix entries
    for (r = 1; r < c; r++) {
      f = A->get (r, c);
      for (k = 1; k < r; k++) f -= A->get (r, k) * A->get (k, c);
      A->set (r, c, f / A->get (r, r));
    }
    // lower matrix entries
    for (MaxPivot = 0, pivot = r; r <= N; r++) {
      f = A->get (r, c);
      for (k = 1; k < c; k++) f -= A->get (r, k) * A->get (k, c);
      A->set (r, c, f);
      // larger pivot ?
      if ((d = nPvt[r - 1] * abs (f)) > MaxPivot) {
	MaxPivot = d;
	pivot = r;
      }
    }

    // check pivot element and throw appropriate exception
    if (MaxPivot <= 0) {
#if LU_FAILURE
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during LU decomposition");
      e->setData (c);
      throw_exception (e);
      goto fail;
#else /* insert virtual resistance */
      VIRTUAL_RES ("no pivot != 0 found during LU decomposition", c);
#endif
    }

    // swap matrix rows if necessary and remember that step in the
    // exchange table
    if (c != pivot) {
      A->exchangeRows (c, pivot);
      Swap (int, rMap[c - 1], rMap[pivot - 1]);
      Swap (nr_double_t, nPvt[c - 1], nPvt[pivot - 1]);
    }
  }
#if LU_FAILURE
 fail:
#endif
}

/* This function decomposes the left hand matrix into an upper U and
   lower L matrix.  The algorithm is called LU decomposition
   (Doolittle's definition).  The function performs the actual LU
   decomposition of the matrix A using (implicit) partial row pivoting. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_lu_doolittle (void) {
  nr_double_t d, MaxPivot;
  nr_type_t f;
  int k, c, r, pivot;

  // initialize pivot exchange table
  for (r = 1; r <= N; r++) {
    for (MaxPivot = 0, c = 1; c <= N; c++)
      if ((d = abs (A->get (r, c))) > MaxPivot)
	MaxPivot = d;
    if (MaxPivot <= 0) MaxPivot = TINY;
    nPvt[r - 1] = 1 / MaxPivot;
    rMap[r - 1] = r;
  }

  // decompose the matrix into L (lower) and U (upper) matrix
  for (c = 1; c <= N; c++) {
    // upper matrix entries
    for (r = 1; r < c; r++) {
      f = A->get (r, c);
      for (k = 1; k < r; k++) f -= A->get (r, k) * A->get (k, c);
      A->set (r, c, f);
    }
    // lower matrix entries
    for (MaxPivot = 0, pivot = r; r <= N; r++) {
      f = A->get (r, c);
      for (k = 1; k < c; k++) f -= A->get (r, k) * A->get (k, c);
      A->set (r, c, f);
      // larger pivot ?
      if ((d = nPvt[r - 1] * abs (f)) > MaxPivot) {
	MaxPivot = d;
	pivot = r;
      }
    }

    // check pivot element and throw appropriate exception
    if (MaxPivot <= 0) {
#if LU_FAILURE
      qucs::exception * e = new qucs::exception (EXCEPTION_PIVOT);
      e->setText ("no pivot != 0 found during LU decomposition");
      e->setData (c);
      throw_exception (e);
      goto fail;
#else /* insert virtual resistance */
      VIRTUAL_RES ("no pivot != 0 found during LU decomposition", c);
#endif
    }

    // swap matrix rows if necessary and remember that step in the
    // exchange table
    if (c != pivot) {
      A->exchangeRows (c, pivot);
      Swap (int, rMap[c - 1], rMap[pivot - 1]);
      Swap (nr_double_t, nPvt[c - 1], nPvt[pivot - 1]);
    }

    // finally divide by the pivot element
    if (c != N) {
      f = 1 / A->get (c, c);
      for (r = c + 1; r <= N; r++) A->set (r, c, A->get (r, c) * f);
    }
  }
#if LU_FAILURE
 fail:
#endif
}

/* The function is used in order to run the forward and backward
   substitutions using the LU decomposed matrix (Crout's definition -
   Uii are ones).  */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_lu_crout (void) {
  nr_type_t f;
  int i, c;
  tvector<nr_type_t> Y (N);

  // forward substitution in order to solve LY = B
  for (i = 1; i <= N; i++) {
    f = B->get (rMap[i - 1]);
    for (c = 1; c < i; c++) f -= A->get (i, c) * Y.get (c);
    f /= A->get (i, i);
    Y.set (i, f);
  }
   
  // backward substitution in order to solve UX = Y
  for (i = N; i > 0; i--) {
    f = Y.get (i);
    for (c = i + 1; c <= N; c++) f -= A->get (i, c) * X->get (c);
    // remember that the Uii diagonal are ones only in Crout's definition
    X->set (i, f);
  }
}

/* The function is used in order to run the forward and backward
   substitutions using the LU decomposed matrix (Doolittle's
   definition - Lii are ones).  This function is here because of
   transposed LU matrices as used in the AC noise analysis. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_lu_doolittle (void) {
  nr_type_t f;
  int i, c;
  tvector<nr_type_t> Y (N);

  // forward substitution in order to solve LY = B
  for (i = 1; i <= N; i++) {
    f = B->get (rMap[i - 1]);
    for (c = 1; c < i; c++) f -= A->get (i, c) * Y.get (c);
    // remember that the Lii diagonal are ones only in Doolittle's definition
    Y.set (i, f);
  }
   
  // backward substitution in order to solve UX = Y
  for (i = N; i > 0; i--) {
    f = Y.get (i);
    for (c = i + 1; c <= N; c++) f -= A->get (i, c) * X->get (c);
    f /= A->get (i, i);
    X->set (i, f);
  }
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
  int error, conv, i, c, r;
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
#if DEBUG && 0
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
    B->set (r, B->get (r) / f);
  }

  // the current X vector is a good initial guess for the iteration
  tvector<nr_type_t> * Xprev = new tvector<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 1; r <= N; r++) {
      for (f = 0, c = 1; c <= N; c++) {
	if (algo == ALGO_GAUSS_SEIDEL) {
	  // Gauss-Seidel
	  if (c < r)      f += A->get (r, c) * X->get (c);
	  else if (c > r) f += A->get (r, c) * Xprev->get (c);
	}
	else {
	  // Jacobi
	  if (c != r) f += A->get (r, c) * Xprev->get (c);
	}
      }
      X->set (r, B->get (r) - f);
    }
    // check for convergence
    for (conv = 1, r = 1; r <= N; r++) {
      diff = abs (X->get (r) - Xprev->get (r));
      if (diff >= abstol + reltol * abs (X->get (r))) {
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
#if DEBUG && 0
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
  int error, conv, i, c, r;
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
#if DEBUG && 0
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
    B->set (r, B->get (r) / f);
  }

  // the current X vector is a good initial guess for the iteration
  tvector<nr_type_t> * Xprev = new tvector<nr_type_t> (*X);

  // start iterating here
  i = 0; error = 0;
  do {
    // compute new solution vector
    for (r = 1; r <= N; r++) {
      for (f = 0, c = 1; c <= N; c++) {
	if (c < r)      f += A->get (r, c) * X->get (c);
	else if (c > r) f += A->get (r, c) * Xprev->get (c);
      }
      X->set (r, (1 - l) * Xprev->get (r) + l * (B->get (r) - f));
    }
    // check for convergence
    for (s = 0, d = 0, conv = 1, r = 1; r <= N; r++) {
      diff = abs (X->get (r) - Xprev->get (r));
      if (diff >= abstol + reltol * abs (X->get (r))) {
	conv = 0;
	break;
      }
      d += diff; s += abs (X->get (r));
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
#if DEBUG && 0
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
   diagonal elements have pairs of 1 and -1 on the same column and
   row. */
template <class nr_type_t>
void eqnsys<nr_type_t>::ensure_diagonal_MNA (void) {
  int restart, exchanged, begin = 1, pairs;
  int pivot1, pivot2, i;
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
  int pairs = 0;
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

/* The function tries to raise the absolute value of diagonal elements
   by swapping rows and thereby make the A matrix diagonally
   dominant. */
template <class nr_type_t>
void eqnsys<nr_type_t>::preconditioner (void) {
  int pivot, r;
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

#define A_ (*A)
#define X_ (*X)
#define B_ (*B)
#define R_ (*R)
#define T_ (*T)

/* This function uses QR decomposition using householder
   transformations in order to solve the given equation system. */
template <class nr_type_t>
void eqnsys<nr_type_t>::solve_qr (void) {
  factorize_qr_householder ();
  substitute_qr_householder ();
  //factorize_qrh ();
  //substitute_qrh ();
}

/* The following function computes the euclidian norm of the given
   column vector of the matrix A starting from the given row. */
template <class nr_type_t>
nr_double_t eqnsys<nr_type_t>::euclidianCol (int c, int r) {
  nr_double_t scale = 0, n = 1, x, ax;
  for (int i = r; i <= N; i++) {
    if ((x = real (A_(i, c))) != 0) {
      ax = fabs (x);
      if (scale < ax) {
	x = scale / ax;
	n = 1 + n * x * x;
	scale = ax;
      }
      else {
	x = ax / scale;
	n += x * x;
      }
    }
    if ((x = imag (A_(i, c))) != 0) {
      ax = fabs (x);
      if (scale < ax) {
	x = scale / ax;
	n = 1 + n * x * x;
	scale = ax;
      }
      else {
	x = ax / scale;
	n += x * x;
      }
    }
  }
  return scale * sqrt (n);
}

/* The function decomposes the matrix A into two matrices, the
   orthonormal matrix Q and the upper triangular matrix R.  The
   original matrix is replaced by the householder vectors in the lower
   triangular (including the diagonal) and the upper triangular R
   matrix with its diagonal elements stored in the R vector. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_qrh (void) {
  int c, r, k, pivot;
  nr_type_t f, t;
  nr_double_t s, MaxPivot;

  if (R) delete R; R = new tvector<nr_type_t> (N);

  for (c = 1; c <= N; c++) {
    // compute column norms and save in work array
    nPvt[c - 1] = euclidianCol (c);
    cMap[c - 1] = c; // initialize permutation vector
  }

  for (c = 1; c <= N; c++) {

    // put column with largest norm into pivot position
    MaxPivot = nPvt[c - 1]; pivot = c;
    for (r = c + 1; r <= N; r++) {
      if ((s = nPvt[r - 1]) > MaxPivot) {
	pivot = r;
	MaxPivot = s;
      }
    }
    if (pivot != c) {
      A->exchangeCols (pivot, c);
      Swap (int, cMap[pivot - 1], cMap[c - 1]);
      Swap (nr_double_t, nPvt[pivot - 1], nPvt[c - 1]);
    }

    // compute householder vector
    if (c < N) {
      nr_type_t a, b, l;
      s = euclidianCol (c, c + 1);
      a = A_(c, c);
      l = a != 0 ? sign (a) : 1;
      b = - l * xhypot (a, s); // Wj
      t = xhypot (s, a - b);   // || Vi - Wi ||
      R_(c) = b;
      // householder vector entries Ui
      A_(c, c) = (a - b) / t;
      for (r = c + 1; r <= N; r++) A_(r, c) /= t;
    }
    else {
      R_(c) = A_(c, c);
    }

    // apply householder transformation to remaining columns
    for (r = c + 1; r <= N; r++) {
      for (f = 0, k = c; k <= N; k++) f += conj (A_(k, c)) * A_(k, r);
      for (k = c; k <= N; k++) A_(k, r) -= 2 * f * A_(k, c);
    }

    // update norms of remaining columns too
    for (r = c + 1; r <= N; r++) {
      nPvt[r - 1] = euclidianCol (r, c + 1);
    }
  }
}

/* The function decomposes the matrix A into two matrices, the
   orthonormal matrix Q and the upper triangular matrix R.  The
   original matrix is replaced by the householder vectors in the lower
   triangular and the upper triangular R matrix (including the
   diagonal).  The householder vectors are normalized to have in its
   first position. */
template <class nr_type_t>
void eqnsys<nr_type_t>::factorize_qr_householder (void) {
  int c, r, k, pivot;
  nr_type_t f, t;
  nr_double_t s, MaxPivot;

  if (T) delete T; T = new tvector<nr_type_t> (N);

  for (c = 1; c <= N; c++) {
    // compute column norms and save in work array
    nPvt[c - 1] = euclidianCol (c);
    cMap[c - 1] = c; // initialize permutation vector
  }

  for (c = 1; c <= N; c++) {

    // put column with largest norm into pivot position
    MaxPivot = nPvt[c - 1]; pivot = c;
    for (r = c + 1; r <= N; r++) {
      if ((s = nPvt[r - 1]) > MaxPivot) {
	pivot = r;
	MaxPivot = s;
      }
    }
    if (pivot != c) {
      A->exchangeCols (pivot, c);
      Swap (int, cMap[pivot - 1], cMap[c - 1]);
      Swap (nr_double_t, nPvt[pivot - 1], nPvt[c - 1]);
    }

    // compute householder vector
    {
      nr_type_t a, b, l;
      s = euclidianCol (c, c + 1);
      a = A_(c, c);
      if (s == 0) {
	t = T_(c) = 0;
      } else {
	l = a != 0 ? sign (a) : 1;
	b = - l * xhypot (a, s); // Wj
	t = T_(c) = (b - a) / b;
	// householder vector entries Ui
	for (f = a - b, r = c + 1; r <= N; r++) A_(r, c) /= f;
	A_(c, c) = b;
      }
    }

    // apply householder transformation to remaining columns
    if (t != 0) for (r = c + 1; r <= N; r++) {
      for (f = A_(c, r), k = c + 1; k <= N; k++)
	f += conj (A_(k, c)) * A_(k, r);
      A_(c, r) -= t * f;
      for (k = c + 1; k <= N; k++) A_(k, r) -= t * f * A_(k, c);
    }

    // update norms of remaining columns too
    for (r = c + 1; r <= N; r++) {
      if (nPvt[r - 1] > 0)
	nPvt[r - 1] = euclidianCol (r, c + 1);
    }
  }
}

/* The function uses the householder vectors in order to compute Q'B,
   then the equation system RX = B is solved by backward substitution. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_qrh (void) {
  int c, r;
  nr_type_t f;

  // form the new right hand side Q'B
  for (c = 1; c < N; c++) {
    // scalar product u_k^T * B
    for (f = 0, r = c; r <= N; r++) f += conj (A_(r, c)) * B_(r);
    // z - 2 * f * u_k
    for (r = c; r <= N; r++) B_(r) -= 2 * f * A_(r, c);
  }

  // backward substitution in order to solve RX = Q'B
  for (r = N; r > 0; r--) {
    f = B_(r);
    for (c = r + 1; c <= N; c++) f -= A_(r, c) * X_(cMap[c - 1]);
    if (abs (R_(r)) > 0)
      X_(cMap[r - 1]) = f / R_(r);
    else
      X_(cMap[r - 1]) = 0;
  }
}

/* The function uses the householder vectors in order to compute Q'B,
   then the equation system RX = B is solved by backward substitution. */
template <class nr_type_t>
void eqnsys<nr_type_t>::substitute_qr_householder (void) {
  int c, r;
  nr_type_t f;

  // form the new right hand side Q'B
  for (c = 1; c < N; c++) {
    if (T_(c) != 0) {
      // scalar product u_k^T * B
      for (f = B_(c), r = c + 1; r <= N; r++) f += conj (A_(r, c)) * B_(r);
      B_(c) -= T_(c) * f;
      // z - 2 * f * u_k
      for (r = c + 1; r <= N; r++) B_(r) -= T_(c) * f * A_(r, c);
    }
  }

  // backward substitution in order to solve RX = Q'B
  for (r = N; r > 0; r--) {
    f = B_(r);
    for (c = r + 1; c <= N; c++) f -= A_(r, c) * X_(cMap[c - 1]);
    if (abs (A_(r, r)) > TINY)
      X_(cMap[r - 1]) = f / A_(r, r);
    else
      X_(cMap[r - 1]) = 0;
  }

  // TODO: compute least square solution for singular matrices
}
