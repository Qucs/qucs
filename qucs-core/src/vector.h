/*
 * vector.h - vector class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: vector.h,v 1.2 2004/02/13 20:31:45 ela Exp $
 *
 */

#ifndef __VECTOR_H__
#define __VECTOR_H__

class complex;
class strlist;

class vector : public object
{
 public:
  vector ();
  vector (char *);
  vector (const vector &);
  ~vector ();
  void add (complex);
  void add (vector *);
  complex get (int);
  int getSize (void);
  int checkSizes (vector &, vector &);
  int getRequested (void) { return requested; }
  void setRequested (int n) { requested = n; }
  void reverse (void);
  strlist * getDependencies (void) { return dependencies; }
  void setDependencies (strlist * s) { dependencies = s; }
  void setOrigin (char *);
  char * getOrigin (void);

  // operator functions
  friend vector * operator + (vector &, vector &);
  friend vector * operator + (vector &, const complex);
  friend vector * operator - (vector &, vector &);
  friend vector * operator - (vector &, const complex);
  friend vector * operator * (vector &, vector &);
  friend vector * operator * (vector &, const complex);
  friend vector * operator / (vector &, vector &);
  friend vector * operator / (vector &, const complex);

  // comparisons
  //  friend int      operator == (const vector *, const vector *);
  //  friend int      operator != (const vector *, const vector *);

  // assignment operations
  vector& operator += (vector &);
  vector& operator += (const complex);
  vector& operator -= (vector &);
  vector& operator -= (const complex);
  vector& operator *= (vector &);
  vector& operator *= (const complex);
  vector& operator /= (vector &);
  vector& operator /= (const complex);

 private:
  int requested;
  int size;
  int capacity;
  strlist * dependencies;
  complex * data;
  char * origin;
};

#endif /* __VECTOR_H__ */
