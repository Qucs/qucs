/*
 * acsolver.h - AC solver class definitions
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
 * $Id: acsolver.h,v 1.3 2005/01/24 19:36:59 raimi Exp $
 *
 */

#ifndef __ACSOLVER_H__
#define __ACSOLVER_H__

#include "nasolver.h"

class sweep;
class vector;

class acsolver : public nasolver<complex>
{
 public:
  acsolver ();
  acsolver (char *);
  acsolver (acsolver &);
  ~acsolver ();
  void solve (void);
  void solve_noise (void);
  static void calc (acsolver *);
  void init (void);
  void saveAllResults (nr_double_t);
  void saveNoiseResults (vector *);

 private:
  sweep * swp;
  nr_double_t freq;
  int noise;
  tvector<complex> * vn;
};

#endif /* __ACSOLVER_H__ */

