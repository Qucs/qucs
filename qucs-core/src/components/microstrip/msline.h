/*
 * msline.h - microstrip transmission line class definitions
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
 * $Id: msline.h,v 1.14 2004/09/12 14:09:20 ela Exp $
 *
 */

#ifndef __MSLINE_H__
#define __MSLINE_H__

class msline : public circuit
{
 public:
  msline ();
  void calcDC (void) { }
  void initDC (void);
  void calcNoise (nr_double_t);
  void calcSP (nr_double_t);
  static void analyseQuasiStatic (nr_double_t, nr_double_t, nr_double_t,
				  nr_double_t, char *,
				  nr_double_t&, nr_double_t&, nr_double_t&);
  static void analyseDispersion (nr_double_t, nr_double_t, nr_double_t,
				 nr_double_t, nr_double_t, nr_double_t, char *,
				 nr_double_t&, nr_double_t&);
  static void Hammerstad_ab (nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  static void Hammerstad_er (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t&);
  static void Hammerstad_zl (nr_double_t, nr_double_t&);
  static void Getsinger_disp (nr_double_t, nr_double_t, nr_double_t,
			      nr_double_t, nr_double_t,
			      nr_double_t&, nr_double_t&);
  static void Kirschning_er (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t&);
  static void Kirschning_zl (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  static void analyseLoss (nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			   nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			   nr_double_t, nr_double_t, char *,
			   nr_double_t&, nr_double_t&);

 private:
  nr_double_t alpha, zref;
};

#endif /* __MSLINE_H__ */
