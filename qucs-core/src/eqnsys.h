/*
 * eqnsys.h - equations system solver class definitions
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
 * $Id: eqnsys.h,v 1.13 2005-04-01 06:52:47 raimi Exp $
 *
 */

#ifndef __EQNSYS_H__
#define __EQNSYS_H__

// Definition of equation system solving algorithms.
enum algo_type {
  ALGO_INVERSE                   = 0x0001,
  ALGO_GAUSS                     = 0x0002,
  ALGO_GAUSS_JORDAN              = 0x0004,
  ALGO_LU_FACTORIZATION          = 0x0008,
  ALGO_LU_SUBSTITUTION_CROUT     = 0x0010,
  ALGO_LU_SUBSTITUTION_DOOLITTLE = 0x0020,
  ALGO_LU_DECOMPOSITION          = 0x0018,
  ALGO_JACOBI                    = 0x0040,
  ALGO_GAUSS_SEIDEL              = 0x0080,
  ALGO_SOR                       = 0x0100
};

// Definition of pivoting strategies.
enum pivot_type {
  PIVOT_NONE    = 0x01,
  PIVOT_PARTIAL = 0x02,
  PIVOT_FULL    = 0x04
};

#include "tvector.h"
#include "tmatrix.h"

template <class nr_type_t>
class eqnsys
{
 public:
  eqnsys ();
  eqnsys (eqnsys &);
  ~eqnsys ();
  void setAlgo (int a) { algo = a; }
  int  getAlgo (void) { return algo; }
  void passEquationSys (tmatrix<nr_type_t> *, tvector<nr_type_t> *,
			tvector<nr_type_t> *);
  void solve (void);

 private:
  int update;
  int algo;
  int pivoting;
  int * rMap;
  int * cMap;
  tmatrix<nr_type_t> * A;
  tvector<nr_type_t> * B;
  tvector<nr_type_t> * X;
  void solve_inverse (void);
  void solve_gauss (void);
  void solve_gauss_jordan (void);
  void solve_lu (void);
  void solve_lu_factorization (void);
  void solve_lu_subst_crout (void);
  void solve_lu_subst_doolittle (void);
  void solve_iterative (void);
  void solve_sor (void);
  nr_double_t convergence_criteria (void);
  void ensure_diagonal (void);
  void ensure_diagonal_MNA (void);
  int countPairs (int, int&, int&);
  void preconditioner (void);
};

#include "eqnsys.cpp"

#endif /* __EQNSYS_H__ */
