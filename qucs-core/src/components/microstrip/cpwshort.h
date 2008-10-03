/*
 * cpwshort.h - coplanar waveguide short class definitions
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: cpwshort.h,v 1.4 2008-10-03 14:49:49 ela Exp $
 *
 */

#ifndef __CPWSHORT_H__
#define __CPWSHORT_H__

class cpwshort : public circuit
{
 public:
  cpwshort ();
  void initSP (void);
  void calcSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);

  CREATOR (cpwshort);

  void checkProperties (void);
  nr_double_t calcLend (nr_double_t);
  nr_complex_t calcZ (nr_double_t);
};

#endif /* __CPWSHORT_H__ */
