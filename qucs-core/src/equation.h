/*
 * equation.h - checker definitions for Qucs equations
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
 * $Id: equation.h,v 1.1 2004/03/07 08:33:01 ela Exp $
 *
 */

#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"

namespace eqn {

class node;
class constant;
class reference;
class assignment;
class application;

enum ConstantTag {
  TAG_UNKNOWN = -1,
  TAG_DOUBLE = 0,
  TAG_COMPLEX,
  TAG_VECTOR,
  TAG_MATRIX
};

class constant {
 public:
  constant ();
  constant (int);
  ~constant ();
  void print (void);

 public:
  int type;
  union {
    nr_double_t d;
    complex * c;
    vector * v;
    matrix * m;
  };
};

class reference {
 public:
  reference ();
  ~reference ();
  void print (void);

 public:
  char * n;
};

class assignment {
 public:
  assignment ();
  ~assignment ();
  void print (void);
  
 public:
  char * result;
  node * body;
};

class application {
 public:
  application ();
  ~application ();
  void print (void);

 public:
  char * n;
  int nargs;
  node * args;
  /* todo: function pointer */
};

enum NodeTag {
  UNKNOWN = -1,
  CONSTANT = 0, /* source code constant */
  REFERENCE,    /* variable reference   */
  APPLICATION,  /* call to function     */
  ASSIGNMENT    /* root of equation     */
};

class node {
 public:
  node ();
  node (int);
  ~node ();
  node * getNext (void) { return next; }
  void setNext (node * n) { next = n; }
  int count (void);
  void append (node *);
  void print (void);

 public:
  /* todo: semantic type */
  union {
    assignment * assign;
    application * app;
    reference * ref;
    constant * con;
  };

 private:
  int tag;
  node * next;
};

/* The global list of equations and expression lists. */
extern node * equations;
extern node * expressions;

} /* namespace */

__BEGIN_DECLS

void equation_list (void);

__END_DECLS

#endif /* __EQUATION_H__ */
