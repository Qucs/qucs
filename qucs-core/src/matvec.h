/*
 * matvec.h - matrix vector class definitions
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
 * $Id: matvec.h,v 1.1 2004-07-03 10:56:40 ela Exp $
 *
 */

#ifndef __MATVEC_H__
#define __MATVEC_H__

class matrix;

class matvec
{
 public:
  matvec ();
  matvec (int, int, int);
  matvec (const matvec &);
  ~matvec ();
  int getSize (void) { return size; }
  int getCols (void) { return cols; }
  int getRows (void) { return rows; }
  void set (vector *, int, int);
  void set (matrix *, int);
  vector * get (int, int);
  matrix * get (int);

 private:
  int size;
  int rows;
  int cols;
  matrix * data;
};

#endif /* __MATVEC_H__ */
