/*
 * msopen.h - microstrip open end class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2004 Michael Margraf <Michael.Margraf@alumni.TU-Berlin.DE>
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
 * $Id: msopen.h,v 1.5 2004-09-26 09:58:52 ela Exp $
 *
 */

#ifndef __MSOPEN_H__
#define __MSOPEN_H__

class msopen : public circuit
{
 public:
  msopen ();
  static nr_double_t calcCend (nr_double_t, nr_double_t, nr_double_t,
			       nr_double_t, nr_double_t, char *, char *,
			       char *);
  void calcSP (nr_double_t);
  void initDC (void);
  void calcAC (nr_double_t);
  complex calcY (nr_double_t);
};

#endif /* __MSOPEN_H__ */
