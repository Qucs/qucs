/*
 * acsolver.h - AC solver class definitions
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
 * $Id: acsolver.h,v 1.2 2004-09-17 11:48:52 ela Exp $
 *
 */

#ifndef __ACSOLVER_H__
#define __ACSOLVER_H__

#include "nasolver.h"

class sweep;

class acsolver : public nasolver<complex>
{
 public:
  acsolver ();
  acsolver (char *);
  acsolver (acsolver &);
  ~acsolver ();
  void solve (void);
  static void calc (acsolver *);
  void init (void);
  void saveAllResults (nr_double_t);

 private:
  sweep * swp;
  nr_double_t freq;
};

#endif /* __ACSOLVER_H__ */

