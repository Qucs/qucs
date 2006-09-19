/*
 * equation.h - checker definitions for Qucs equations
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: equation.h,v 1.29 2006-09-19 08:22:20 raimi Exp $
 *
 */

#ifndef __EQUATION_H__
#define __EQUATION_H__

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "matrix.h"
#include "matvec.h"

class strlist;
class dataset;
class range;

namespace eqn {

class solver;
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
  void setDataDependencies (strlist *);
  strlist * getDataDependencies (void) { return dataDependencies; }
  void setDropDependencies (strlist * deps) { dropDependencies = deps; }
  void addDropDependencies (char *);
  strlist * getDropDependencies (void) { return dropDependencies; }
  void setPrepDependencies (strlist * deps) { prepDependencies = deps; }
  void addPrepDependencies (char *);
  strlist * getPrepDependencies (void) { return prepDependencies; }
  strlist * recurseDependencies (checker *, strlist *);
  node * get (int);
  constant * getResult (int);
  int getType (void) { return type; }
  int getTag (void) { return tag; }
  void setType (int tag) { type = tag; }
  constant * getResult (void) { return res; } 
  void setResult (constant * r) { res = r; }
  char * getInstance (void);
  void setInstance (char *);
  void applyInstance (void);
  constant * calculate (void);    
  strlist * collectDependencies (void);
  strlist * collectDataDependencies (void);

  /* These functions should be overloaded by derivative classes. */
  virtual void print (void) { }
  virtual void addDependencies (strlist *) { }
  virtual int evalType (void) { return type; }
  virtual char * toString (void) { return txt; }
  virtual constant * evaluate (void) { return res; }
  
public:
  int duplicate;
  int cycle;
  int evalPossible;
  char * txt;
  int evaluated;
  char * instance;
  int output;
  int dropdeps;
  solver * solvee;
  checker * checkee;

private:
  int type;
  int tag;
  node * next;
  strlist * dependencies;
  constant * res;
  strlist * dataDependencies;
  strlist * dropDependencies;
  strlist * prepDependencies;
};

enum ConstantTag {
  TAG_UNKNOWN =   0,
  TAG_DOUBLE  =   1, /* double constant          */
  TAG_COMPLEX =   2, /* complex value            */
  TAG_VECTOR  =   4, /* list of complex values   */
  TAG_MATRIX  =   8, /* complex matrix           */
  TAG_MATVEC  =  16, /* list of complex matrices */
  TAG_CHAR    =  32, /* single character         */
  TAG_STRING  =  64, /* character string         */
  TAG_RANGE   = 128, /* interval specification   */
};

/* This class represents any type of constant expression. */
class constant : public node
{
public:
  constant ();
  constant (int);
  ~constant ();
  void print (void);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);

public:
  bool dataref;
  int type;
  union {
    nr_double_t d;
    complex * c;
    vector * v;
    matrix * m;
    matvec * mv;
    char chr;
    char * s;
    range * r;
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
  void findVariable (void);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);

public:
  char * n;
  node * ref;
};

/* This class represents assignments with a left hand and right hand
   side. */
class assignment : public node
{
public:
  assignment ();
  ~assignment ();
  void print (void);
  void addDependencies (strlist *);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);
  
public:
  char * result;
  node * body;
};

// Type of application function.
typedef constant * (* evaluator_t) (constant *);

/* The application class represents any kind of operation (unary,
   binary and n-ary ones) containing the appropriate argument list. */
class application : public node
{
public:
  application ();
  ~application ();
  void print (void);
  void addDependencies (strlist *);
  int evalType (void);
  char * toString (void);
  constant * evaluate (void);

public:
  char * n;
  int nargs;
  node * args;
  evaluator_t eval;

private:
  void evalTypeArgs (void);
  char * createKey (void);
  int evalTypeFast (void);
};

/* This class implements the actual functionality regarding a set of
   equations stored within a netlist. */
class checker
{
public:
  checker ();
  ~checker ();
  void collectDependencies (void);
  void setEquations (node *);
  node * getEquations (void) { return equations; }
  void list (void);
  int findUndefined (int);
  static char * tag2key (int);
  static int isGenerated (char *);
  strlist * getVariables (void);
  int findDuplicate (void);
  static node * findEquation (node *, char *);
  int detectCycles (void);
  static strlist * foldDependencies (strlist *);
  node * appendEquation (node *, node *);
  void dropEquation (node *);
  void reorderEquations (void);
  static node * lastEquation (node *);
  int applyTypes (void);
  int checkExport (void);
  void constants (void);
  int check (int noundefined = 1);
  strlist * variables (void);

public:
  node * equations;

 private:
  bool consts;
};

/* The solver class is finally used to solve the list of equations. */
class solver
{
public:
  solver (checker *);
  ~solver ();
  void setEquations (node * eqn) { equations = eqn; }
  node * getEquations (void) { return equations; }
  void setData (dataset * d) { data = d; }
  dataset * getDataset (void) { return data; }
  void evaluate (void);
  node * addEquationData (vector *, bool ref = false);
  node * addEquationData (matvec *);
  node * addGeneratedEquation (vector *, char *);
  vector * dataVector (node *);
  void checkinDataset (void);
  void checkoutDataset (void);
  static int dataSize (constant *);
  int getDependencySize (strlist *, int);
  int getDataSize (char *);
  strlist * collectDataDependencies (node *);
  int dataSize (strlist *);
  vector * getDataVector (char *);
  void findMatrixVectors (vector *);
  char * isMatrixVector (char *, int&, int&);
  int findEquationResult (node *);
  int solve (dataset *);

public:
  node * equations;

private:
  dataset * data;
  int generated;
  checker * checkee;
};

} /* namespace */

#endif /* __EQUATION_H__ */
