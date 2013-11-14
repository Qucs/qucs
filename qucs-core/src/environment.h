/*
 * environment.h - variable environment class definitions
 *
 * Copyright (C) 2004, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#include "equation.h"

#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

class variable;
class checker;
class solver;
class dataset;
template <class type_t> class ptrlist;

class environment
{
 public:
  environment ();
  environment (const char *);
  environment (const environment &);
  virtual ~environment ();
  void copy (const environment &);
  void setName (char *);
  char * getName (void);
  void print (bool all = false);
  void setDefinitions (struct definition_t * d) { defs = d; }
  struct definition_t * getDefinitions (void) { return defs; }

  // variable specific functionality
  void copyVariables (variable *);
  void deleteVariables (void);
  void addVariable (variable *, bool pass = true);
  variable * getVariable (char *);

  // equation specific functionality
  void setChecker (eqn::checker * c) { checkee = c; }
  eqn::checker * getChecker (void) { return checkee; }
  void setSolver (eqn::solver * s) { solvee = s; }
  eqn::solver * getSolver (void) { return solvee; }
  int equationChecker (int noundefined = 1);
  int equationSolver (dataset *);
  int runSolver (void);
  void equationSolver (void);

  // subcircuit specific
  ::vector getVector (char *);
  void setDoubleConstant (char *, nr_double_t);
  nr_double_t getDoubleConstant (char *);
  void setDouble (char *, nr_double_t);
  nr_double_t getDouble (char *);
  void setDoubleReference (char *, char *);
  char * getDoubleReference (char *);
  void updateReferences (environment *);
  void passConstants (void);
  void fetchConstants (void);
  variable * findValue (char *);
  void setValue (char *, eqn::constant *);
  void saveResults (void);

  // children functionality
  void delChild (environment *);
  void addChild (environment *);

 private:
  char * name;
  variable * root;
  eqn::checker * checkee;
  eqn::solver * solvee;
  ptrlist<environment> * children;
  bool iscopy;
  struct definition_t * defs;
};

#endif /* __ENVIRONMENT_H__ */
