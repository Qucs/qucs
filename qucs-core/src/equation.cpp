/*
 * equation.cpp - checker for the Qucs equations
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
 * $Id: equation.cpp,v 1.1 2004/03/07 08:33:01 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "matrix.h"
#include "dataset.h"
#include "strlist.h"
#include "equation.h"

using namespace eqn;

/* The global list of equations and expression lists. */
node * eqn::equations = NULL;
node * eqn::expressions = NULL;

constant::constant () {
  type = TAG_UNKNOWN;
}

constant::constant (int tag) {
  type = tag;
}

constant::~constant () {
}

void constant::print () {
  switch (type) {
  case TAG_DOUBLE:
    printf ("%g", d);
    break;
  case TAG_COMPLEX:
    printf ("%g%cj%g", real (*c), imag (*c) >= 0 ? '+' : '-', imag (*c));
    break;
  }
}

reference::reference () {
  n = NULL;
}

void reference::print (void) {
  printf ("%s", n);
}

reference::~reference () {
}

assignment::assignment () {
  body = NULL;
  result = NULL;
}

assignment::~assignment () {
}

void assignment::print (void) {
  printf ("%s = ", result);
  body->print ();
}

application::application () {
  n = NULL;
  nargs = 0;
  args = NULL;
}

void application::print (void) {
  printf ("%s(", n);
  for (node * arg = args; arg != NULL; arg = arg->getNext ()) {
    arg->print ();
    if (arg->getNext ()) printf (",");
  }
  printf (")");
}

application::~application () {
}

node::node () {
  tag = UNKNOWN;
  next = NULL;
}

node::node (int type) {
  tag = type;
  next = NULL;
}

void node::print (void) {
  switch (tag) {
  case CONSTANT:
    con->print ();
    break;
  case APPLICATION:
    app->print ();
    break;
  case REFERENCE:
    ref->print ();
    break;
  case ASSIGNMENT:
    assign->print ();
    break;
  }
}

int node::count (void) {
  int c = 0;
  for (node * n = this; n != NULL; n = n->getNext ())
    c++;
  return c;
}

void node::append (node * last) {
  node * n;
  for (n = this; n->getNext () != NULL; n = n->getNext ());
  last->setNext (NULL);
  n->setNext (last);
}

node::~node () {
}

void equation_list (void) {
  for (node * eqn = equations; eqn != NULL; eqn = eqn->getNext ()) {
    eqn->print ();
    printf ("\n");
  }
}
