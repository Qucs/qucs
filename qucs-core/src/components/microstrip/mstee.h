/*
 * mstee.h - microstrip t-junction class definitions
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
 * $Id: mstee.h,v 1.2 2005/02/01 22:56:55 raimi Exp $
 *
 */

#ifndef __MSTEE_H__
#define __MSTEE_H__

class circuit;
class net;

class mstee : public circuit
{
 public:
  mstee ();
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initSP (void);
  void calcSP (nr_double_t);
  void initTR (void);

  friend circuit * splitMicrostrip (circuit *, circuit *, net *,
				    char *, char *, int);
  friend void disableMicrostrip (circuit *, circuit *, net *, int);

 private:
  void calcPropagation (nr_double_t);
  void initLines (void);

 private:
  nr_double_t Bt, La, Lb, L2, Ta2, Tb2;
  circuit * lineA;
  circuit * lineB;
  circuit * line2;
};

#endif /* __MSTEE_H__ */
