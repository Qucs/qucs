/*
 * dcsolver.h - DC solver class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: dcsolver.h,v 1.6 2004-06-27 15:11:48 ela Exp $
 *
 */

#ifndef __DCSOLVER_H__
#define __DCSOLVER_H__

class analysis;
class circuit;
class nodelist;
class matrix;
class complex;

class dcsolver : public analysis
{
 public:
  dcsolver ();
  dcsolver (char *);
  dcsolver (dcsolver &);
  ~dcsolver ();
  void solve (void);
  void createNodeList (void);
  void createMatrix (void);
  void assignVoltageSources (void);
  circuit * findVoltageSource (int);
  int countNodes (void);
  void createGMatrix (void);
  void calc (void);
  void createBMatrix (void);
  void createCMatrix (void);
  void createDMatrix (void);
  void createIMatrix (void);
  void createEMatrix (void);
  void runMNA (void);
  void init (void);
  void savePreviousIteration (void);
  void saveResults (void);
  char * createV (int);
  char * createI (int);
  char * createOP (char *, char *);
  void saveNodeVoltages (void);
  int checkConvergence (void);
  void saveVariable (char *, complex);

 private:
  nodelist * nlist;
  matrix * A;
  matrix * z;
  matrix * x;
  matrix * xprev;
  matrix * zprev;
  nr_double_t reltol;
  nr_double_t abstol;
  nr_double_t vntol;
  int saveOPs;
};

#endif /* __DCSOLVER_H__ */
