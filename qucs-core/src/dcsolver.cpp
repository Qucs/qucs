/*
 * dcsolver.cpp - DC solver class implementation
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
 * $Id: dcsolver.cpp,v 1.27 2004-09-06 06:40:07 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>

#include "object.h"
#include "complex.h"
#include "circuit.h"
#include "net.h"
#include "analysis.h"
#include "nasolver.h"
#include "dcsolver.h"

// Constructor creates an unnamed instance of the dcsolver class.
dcsolver::dcsolver () : nasolver<nr_double_t> () {
  saveOPs = 0;
  type = ANALYSIS_DC;
  setDescription ("DC");
}

// Constructor creates a named instance of the dcsolver class.
dcsolver::dcsolver (char * n) : nasolver<nr_double_t> (n) {
  saveOPs = 0;
  type = ANALYSIS_DC;
  setDescription ("DC");
}

// Destructor deletes the dcsolver class object.
dcsolver::~dcsolver () {
}

/* The copy constructor creates a new instance of the dcsolver class
   based on the given dcsolver object. */
dcsolver::dcsolver (dcsolver & o) : nasolver<nr_double_t> (o) {
  saveOPs = o.saveOPs;
}

/* This is the DC netlist solver.  It prepares the circuit list and
   solves it then. */
void dcsolver::solve (void) {
  // fetch simulation properties
  saveOPs = !strcmp (getPropertyString ("saveOPs"), "yes") ? 1 : 0;

  // initialize node voltages, first guess for non-linear circuits and
  // generate extra circuits if necessary
  init ();

  // start the iterative solver
  solve_pre ();
  solve_nonlinear ();

  // save results and cleanup the solver
  saveResults ("V", "I", saveOPs);
  solve_post ();
}

/* Goes through the list of circuit objects and runs its calcDC()
   function. */
void dcsolver::calc (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcDC ();
  }
}

/* Goes through the list of circuit objects and runs its initDC()
   function. */
void dcsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initDC (this);
  }
}
