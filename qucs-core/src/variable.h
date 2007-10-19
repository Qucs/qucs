/*
 * variable.h - generic variable class definitions
 *
 * Copyright (C) 2004, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: variable.h,v 1.7 2007-10-19 17:54:17 ela Exp $
 *
 */

#ifndef __VARIABLE_H__
#define __VARIABLE_H__

#include "components/microstrip/substrate.h"
#include "analysis.h"
#include "equation.h"

using namespace eqn;

// Enumerate possible types of variables.
enum variably_type {
  VAR_UNKNOWN = -1, // not yet defined
  VAR_CONSTANT,     // equation constant
  VAR_REFERENCE,    // equation reference
  VAR_SUBSTRATE,    // substrate definition
  VAR_VALUE,        // equation result
  VAR_ANALYSIS      // analysis
};

class substrate;
class equation;
class analysis;

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
  void setConstant (constant * c) { type = VAR_CONSTANT; value.c = c; }
  constant * getConstant (void) { return value.c; }
  void setReference (reference * r) { type = VAR_REFERENCE; value.r = r; }
  reference * getReference (void) { return value.r; }
  void setSubstrate (substrate * s) { type = VAR_SUBSTRATE; value.s = s; }
  substrate * getSubstrate (void) { return value.s; }
  void setValue (constant * v) { type = VAR_VALUE; value.v = v; }
  constant * getValue (void) { return value.v; }
  void setAnalysis (analysis * a) { type = VAR_ANALYSIS; value.a = a; }
  analysis * getAnalysis (void) { return value.a; }
  char * toString (void);
  void setPassing (bool p) { pass = p; }
  bool getPassing (void) { return pass; }

 private:
  char * name;
  char * text;
  bool pass;
  int type;
  union value_t {
    constant * c;  // equation constant
    reference * r; // equation reference
    substrate * s; // substrate definition
    constant * v;  // equation result
    analysis * a;  // analysis
  } value;
  variable * next;
};

#endif /* __VARIABLE_H__ */
