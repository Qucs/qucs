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
 * $Id: equation.h,v 1.4 2004/03/21 09:57:10 ela Exp $
 *
 */

#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"

class strlist;

namespace eqn {

class checker;
class constant;
class reference;
class assignment;
class application;

enum NodeTag {
  UNKNOWN = -1,
  CONSTANT = 0, /* source code constant */
  REFERENCE,    /* variable reference   */
  APPLICATION,  /* call to function     */
  ASSIGNMENT    /* root of equation     */
};

/* The equation node base class defines and implements the basic
   functionality of an equation node.  Possible types of nodes are
   listed in 'NodeTag'. */
class node
{
public:
  node ();
  node (int);
  virtual ~node ();
  node * getNext (void) { return next; }
  void setNext (node * n) { next = n; }
  int count (void);
  void append (node *);
  void setDependencies (strlist *);
  strlist * getDependencies (void);
  strlist * recurseDependencies (checker *, strlist *);

  /* These functions should be overloaded by derivative classes. */
  virtual void print (void) { }
  virtual void addDependencies (strlist *) { }
  
public:
  int duplicate;
  int cycle;
  int evalPossible;

private:
  int tag;
  node * next;
  strlist * dependencies;
};

enum ConstantTag {
  TAG_UNKNOWN = -1,
  TAG_DOUBLE = 0,   /* double constant        */
  TAG_COMPLEX,      /* complex value          */
  TAG_VECTOR,       /* list of complex values */
  TAG_MATRIX        /* complex matrix         */
};

/* This class represents any type of constant expression. */
class constant : public node
{
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

/* The class represents variable references. */
class reference : public node
{
public:
  reference ();
  ~reference ();
  void print (void);
  void addDependencies (strlist *);

public:
  char * n;
};

/* This class represents assigments with a left hand and right hand
   side. */
class assignment : public node
{
public:
  assignment ();
  ~assignment ();
  void print (void);
  void addDependencies (strlist *);
  
public:
  char * result;
  node * body;
};

/* The application class represents any kind of operation (unary,
   binary and n-ary ones) containing the appropriate argument list. */
class application : public node
{
public:
  application ();
  ~application ();
  void print (void);
  void addDependencies (strlist *);

public:
  char * n;
  int nargs;
  node * args;
  /* todo: function pointer */
};

/* This class implements the actual functionality regarding a set of
   equations stored within a netlist. */
class checker
{
public:
  checker ();
  ~checker ();
  void collectDependencies (void);
  void setEquations (node * eqn) { equations = eqn; }
  node * getEquations (void) { return equations; }
  void list (void);
  int findUndefined (void);
  strlist * getVariables (void);
  int findDuplicate (void);
  node * findEquation (node *, char *);
  int detectCycles (void);
  static strlist * foldDependencies (strlist *);
  node * appendEquation (node *, node *);
  void dropEquation (node *);
  void reorderEquations (void);
  node * lastEquation (node *);

public:
  node * equations;
};

/* The global list of equations and expression lists. */
extern node * equations;
extern node * expressions;

} /* namespace */

#endif /* __EQUATION_H__ */
