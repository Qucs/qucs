/*
 * nasolver.h - nodal analysis solver class definitions
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
 * $Id: nasolver.h,v 1.3 2004/09/12 14:09:19 ela Exp $
 *
 */

#ifndef __NASOLVER_H__
#define __NASOLVER_H__

#include "tmatrix.h"

#define SAVE_OPS 1 // save operating points
#define SAVE_ALL 2 // also save subcircuit nodes and operating points

class analysis;
class circuit;
class nodelist;
class complex;
class vector;

template <class nr_type_t>
class nasolver : public analysis
{
 public:
  nasolver ();
  nasolver (char *);
  nasolver (nasolver &);
  ~nasolver ();
  virtual void calc (void) { }
  int solve_once (void);
  void solve_nonlinear (void);
  void solve_linear (void);
  void solve_pre (void);
  void solve_post (void);
  void setDescription (char * n) { desc = n; }
  char * getDescription (void) { return desc; }
  void saveResults (char *, char *, int, vector * f = NULL);

 protected:
  void savePreviousIteration (void);

 private:
  void createMatrix (void);
  void assignVoltageSources (void);
  circuit * findVoltageSource (int);
  int countNodes (void);
  void createGMatrix (void);
  void createBMatrix (void);
  void createCMatrix (void);
  void createDMatrix (void);
  void createIMatrix (void);
  void createEMatrix (void);
  void runMNA (void);
  char * createV (int, char *, int);
  char * createI (int, char *, int);
  char * createOP (char *, char *);
  void saveNodeVoltages (void);
  int checkConvergence (void);
  void saveVariable (char *, nr_type_t, vector * f = NULL);
  nr_type_t MatValX (complex, complex *);
  nr_type_t MatValX (complex, nr_double_t *);

 protected:
  tmatrix<nr_type_t> * z;
  tmatrix<nr_type_t> * x;
  tmatrix<nr_type_t> * xprev;
  tmatrix<nr_type_t> * zprev;

 private:
  nodelist * nlist;
  tmatrix<nr_type_t> * A;
  nr_double_t reltol;
  nr_double_t abstol;
  nr_double_t vntol;

 private:
  char * desc;
};

#include "nasolver.cpp"

#endif /* __NASOLVER_H__ */
