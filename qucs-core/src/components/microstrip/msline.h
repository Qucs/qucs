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
 * $Id: msline.h,v 1.6 2004-05-16 13:48:40 ela Exp $
 *
 */

#ifndef __MSLINE_H__
#define __MSLINE_H__

class msline : public circuit
{
 public:
  msline ();
  void calcSP (nr_double_t);
  void analyseQuasiStatic (nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			   char *);
  void analyseDispersion (nr_double_t, nr_double_t, nr_double_t, nr_double_t,
			  char *);

 private:
  nr_double_t ErEff;
  nr_double_t ZlEff;
  nr_double_t ErEffFreq;
  nr_double_t ZlEffFreq;
};

#endif /* __MSLINE_H__ */
