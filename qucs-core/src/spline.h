/*
 * spline.h - spline class definitions
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: spline.h,v 1.1 2005/05/23 19:50:51 raimi Exp $
 *
 */

#ifndef __SPLINE_H__
#define __SPLINE_H__

// Types of boundary conditions.
enum spline_boundary_type {
  SPLINE_BC_UNKNOWN = -1,
  SPLINE_BC_NATURAL,
  SPLINE_BC_CLAMPED,
  SPLINE_BC_PERIODIC
};

class vector;
class poly;

class spline
{
 public:
  spline ();
  spline (vector *, vector *);
  ~spline ();

  void vectors (vector *, vector *);
  void construct (void);
  poly evaluate (nr_double_t);
  void setBoundary (int b) { boundary = b; }

 private:
  nr_double_t * upper_bound (nr_double_t *, nr_double_t *, nr_double_t);

 private:
  nr_double_t * x;
  nr_double_t * f0;
  nr_double_t * f1;
  nr_double_t * f2;
  nr_double_t * f3;
  nr_double_t d0, dn;
  int n;
  int boundary;
};

#endif /* __SPLINE_H__ */
