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
 * $Id: dcsolver.cpp,v 1.33 2004-11-29 19:03:33 raimi Exp $
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
  saveOPs |= !strcmp (getPropertyString ("saveOPs"), "yes") ? SAVE_OPS : 0;
  saveOPs |= !strcmp (getPropertyString ("saveAll"), "yes") ? SAVE_ALL : 0;

  // initialize node voltages, first guess for non-linear circuits and
  // generate extra circuits if necessary
  init ();
  setCalculation ((calculate_func_t) &calc);

  // start the iterative solver
  solve_pre ();

  // local variables for the fallback thingies
  int retry = -1, error, fallback = 0;
  int helpers[] = {
    CONV_GMinStepping,
    CONV_SteepestDescent,
    CONV_LineSearch,
    CONV_Attenuation,
    -1 };

  do {
    // Run the DC solver once.
    try_running () {
      applyNodeset ();
      error = solve_nonlinear ();
#if DEBUG
      if (!error) {
	logprint (LOG_STATUS,
		  "NOTIFY: %s: convergence reached after %d iterations\n",
		  getName (), iterations);
      }
#endif /* DEBUG */
      if (!error) retry = -1;
    }
    // Appropriate exception handling.
    catch_exception () {
    case EXCEPTION_NO_CONVERGENCE:
      pop_exception ();
      convHelper = helpers[fallback++];
      if (convHelper != -1) {
	logprint (LOG_ERROR, "WARNING: %s: %s analysis failed, using fallback "
		  "#%d\n", getName (), getDescription (), fallback);
	retry++;
      }
      else {
	retry = -1;
      }
      break;
    default:
      // Otherwise return.
      estack.print ();
      error++;
      break;
    }
  } while (retry != -1);

  // save results and cleanup the solver
  saveResults ("V", "I", saveOPs);

  solve_post ();
}

/* Goes through the list of circuit objects and runs its calcDC()
   function. */
void dcsolver::calc (dcsolver * self) {
  circuit * root = self->getNet()->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calcDC ();
  }
}

/* Goes through the list of circuit objects and runs its initDC()
   function. */
void dcsolver::init (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->initDC ();
  }
}
