/*
 * variable.h - generic variable class definitions
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
 * $Id: variable.h,v 1.2 2004/02/13 20:31:45 ela Exp $
 *
 */

#ifndef __VARIABLE_H__
#define __VARIABLE_H__

// Enumerate possible types of variables.
enum variably_type {
  VAR_UNKNOWN = -1,
  VAR_VECTOR,
  VAR_MATRIX,
  VAR_DOUBLE,
  VAR_INTEGER,
  VAR_COMPLEX,
  VAR_STRING
};

class vector;
class matrix;
class complex;

class variable
{
 public:
  variable ();
  variable (char *);
  variable (const variable &);
  virtual ~variable ();
  void setName (char *);
  char * getName (void);
  void setNext (variable * v) { next = v; }
  variable * getNext (void) { return next; }

  void setType (int t) { type = t; }
  int getType (void) { return type; }
  void setVector (vector * vec) { type = VAR_VECTOR; value.v = vec; }
  vector * getVector (void) { return value.v; }
  void setMatrix (matrix * mat) { type = VAR_MATRIX; value.m = mat; }
  matrix * getMatrix (void) { return value.m; }
  void setComplex (complex * com) { type = VAR_COMPLEX; value.c = com; }
  complex * getComplex (void) { return value.c; }
  void setDouble (nr_double_t dou) { type = VAR_DOUBLE; value.d = dou; }
  nr_double_t getDouble (void) { return value.d; }
  void setInteger (int n) { type = VAR_INTEGER; value.i = n; }
  int getInteger (void) { return value.i; }

  // assignment operations regarding double values
  variable& operator  = (const nr_double_t);
  variable& operator += (const nr_double_t);
  variable& operator -= (const nr_double_t);
  variable& operator *= (const nr_double_t);
  variable& operator /= (const nr_double_t);
  variable  operator +  ();
  variable  operator -  ();

  // operator functions regarding double values
  friend variable operator + (const variable, const nr_double_t);
  friend variable operator + (const nr_double_t,  const variable);
  friend variable operator - (const variable, const nr_double_t);
  friend variable operator - (const nr_double_t,  const variable);
  friend variable operator * (const variable, const nr_double_t);
  friend variable operator * (const nr_double_t,  const variable);
  friend variable operator / (const variable, const nr_double_t);
  friend variable operator / (const nr_double_t,  const variable);

  // comparisons
  friend int operator == (const variable, const nr_double_t);
  friend int operator == (const nr_double_t, const variable);
  friend int operator != (const variable, const nr_double_t);
  friend int operator != (const nr_double_t, const variable);
  friend int operator >= (const variable, const nr_double_t);
  friend int operator >= (const nr_double_t, const variable);
  friend int operator <= (const variable, const nr_double_t);
  friend int operator <= (const nr_double_t, const variable);
  friend int operator >  (const variable, const nr_double_t);
  friend int operator >  (const nr_double_t, const variable);
  friend int operator <  (const variable, const nr_double_t);
  friend int operator <  (const nr_double_t, const variable);

 private:
  char * name;
  int type;
  union value_t {
    vector * v;     // complex vector
    matrix * m;     // matrix
    nr_double_t d;  // real number
    int i;          // integer number
    complex * c;    // complex number
  } value;
  variable * next;
};

#endif /* __VARIABLE_H__ */
