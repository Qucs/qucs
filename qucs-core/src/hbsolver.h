/*
 * hbsolver.h - harmonic balance solver class definitions
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: hbsolver.h,v 1.3 2006/03/22 09:39:19 raimi Exp $
 *
 */

#ifndef __HBSOLVER_H__
#define __HBSOLVER_H__

#include "ptrlist.h"
#include "tvector.h"

class vector;
class strlist;
class circuit;

class hbsolver : public analysis
{
 public:
  hbsolver ();
  hbsolver (char *);
  hbsolver (hbsolver &);
  ~hbsolver ();
  void solve (void);
  void initHB (void);
  void initDC (void);
  static void calc (hbsolver *);
  void collectFrequencies (void);
  void solveLinear (void);
  int  checkBalance (void);

  void splitCircuits (void);
  void expandFrequencies (nr_double_t, int);
  bool isExcitation (circuit *);
  strlist * circuitNodes (ptrlist<circuit>);
  void getNodeLists (void);

 private:
  tvector<nr_double_t> frequencies;
  nr_double_t frequency;
  strlist * nlnodes, * lnnodes, * banodes;
  ptrlist<circuit> excitations;
  ptrlist<circuit> nlcircuits;
  ptrlist<circuit> lncircuits;
};

#endif /* __HBSOLVER_H__ */
