/*
 * eqnsys.h - equations system solver class definitions
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
 * $Id: eqnsys.h,v 1.4 2004/08/01 16:08:02 ela Exp $
 *
 */

#ifndef __EQNSYS_H__
#define __EQNSYS_H__

// Definition of equation system solving algorithms.
enum algo_type {
  ALGO_INVERSE = 0,
  ALGO_GAUSS,
  ALGO_GAUSS_JORDAN,
  ALGO_LU_DECOMPOSITION,
  ALGO_JACOBI
};

class matrix;

class eqnsys
{
 public:
  eqnsys ();
  eqnsys (eqnsys &);
  ~eqnsys ();
  void setAlgo (int a) { algo = a; }
  int getAlgo (void) { return algo; }
  void passEquationSys (matrix *, matrix *, matrix *);
  void solve (void);

 private:
  int algo;
  matrix * A;
  matrix * B;
  matrix * X;
  void solve_inverse (void);
  void solve_gauss (void);
  void solve_gauss_jordan (void);
  void solve_lu (void);
  void solve_jacobi (void);
  nr_double_t convergence_criteria (void);
  void ensure_diagonal (void);
};

#endif /* __EQNSYS_H__ */
