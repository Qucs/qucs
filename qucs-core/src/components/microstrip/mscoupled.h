/*
 * mscoupled.h - parallel coupled microstrip lines class definitions
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
 * $Id: mscoupled.h,v 1.6 2005-03-14 21:59:09 raimi Exp $
 *
 */

#ifndef __MSCOUPLED_H__
#define __MSCOUPLED_H__

class mscoupled : public circuit
{
 public:
  mscoupled ();
  void initDC (void);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void calcPropagation (nr_double_t);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);

  static void analysQuasiStatic (nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t, char *,
				 nr_double_t&, nr_double_t&, nr_double_t&,
				 nr_double_t&);
  static void analyseDispersion (nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t, nr_double_t, char *,
				 nr_double_t&, nr_double_t&, nr_double_t&,
				 nr_double_t&);

 private:
  nr_double_t ae, be, ze, ao, bo, zo;
};

#endif /* __MSCOUPLED_H__ */
