/*
 * cpline.h - coplanar line class definitions
 *
 * Copyright (C) 2004 Vincent Habchi, F5RCS <10.50@free.fr>
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
 * $Id: cpline.h,v 1.1 2004/11/29 19:04:03 raimi Exp $
 *
 */

#ifndef __CPLINE_H__
#define __CPLINE_H__

class cpline : public circuit
{
 public:
  cpline ();
  void calcSP (nr_double_t);
  void initSP (void);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);

  static void ellipke (nr_double_t, nr_double_t &, nr_double_t &);
};

#endif /* __CPLINE_H__ */
