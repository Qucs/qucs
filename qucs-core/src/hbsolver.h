/*
 * hbsolver.h - harmonic balance solver class definitions
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: hbsolver.h,v 1.2 2005/06/02 18:17:50 raimi Exp $
 *
 */

#ifndef __HBSOLVER_H__
#define __HBSOLVER_H__

#include "nasolver.h"

class vector;
class strlist;

class hbsolver : public nasolver<complex>
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
  void collectHarmonics (void);
  void solveLinear (void);
  void getNonLinearNodes (void);
  int  checkBalance (void);

 private:
  vector * frequencies;
  nr_double_t frequency;
  strlist * nlnodes;
};

#endif /* __HBSOLVER_H__ */
