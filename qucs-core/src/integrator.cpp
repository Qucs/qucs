/*
 * integrator.cpp - integrator class implementation
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
 * $Id: integrator.cpp,v 1.1 2004-09-12 14:09:19 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "integrator.h"

// Some definitions for the save-state variables.
#define STATE_SHIFT 3
#define STATE_NUM   8
#define STATE_MASK  7

// Constructor creates an unnamed instance of the integrator class.
integrator::integrator () {
  coefficients = NULL;
  order = 0;
  states = 0;
  currentstate = 0;
  stateval = NULL;
  integrate_func = NULL;
}

/* The copy constructor creates a new instance based on the given
   integrator object. */
integrator::integrator (const integrator & c) {
  coefficients = c.coefficients;
  order = c.order;
  states = c.states;
  currentstate = c.currentstate;
  coefficients = c.coefficients;
  integrate_func = c.integrate_func;

  // copy state variables if necessary
  if (states && c.stateval) {
    int size = states * sizeof (nr_double_t) * STATE_NUM; 
    stateval = (nr_double_t *) malloc (size);
    memcpy (stateval, c.stateval, size);
  }
  else stateval = NULL;
}

// Destructor deletes a integrator object.
integrator::~integrator () {
  if (stateval) free (stateval);
}

/* The function allocates and initializes memory for the save-state
   variables. */
void integrator::initStates (void) {
  if (stateval != NULL) free (stateval);
  if (states) {
    stateval = (nr_double_t *)
      calloc (states, sizeof (nr_double_t) * STATE_NUM);
  }
  currentstate = 0;
}

// Clears the save-state variables.
void integrator::clearStates (void) {
  if (states && stateval)
    memset (stateval, 0, states * sizeof (nr_double_t) * STATE_NUM);
  currentstate = 0;
}

/* The function returns a save-state variable at the given position.
   Higher positions mean earlier states.  By default the function
   returns the current state of the save-state variable. */
nr_double_t integrator::getState (int state, int n) {
  int i = (n + currentstate) & STATE_MASK;
  return stateval[(state << STATE_SHIFT) + i];
}

/* This function applies the given value to a save-state variable.
   Higher positions mean earlier states.  By default the function sets
   the current state of the save-state variable. */
void integrator::setState (int state, nr_double_t val, int n) {
  int i = (n + currentstate) & STATE_MASK;
  stateval[(state << STATE_SHIFT) + i] = val;
}

// Shifts one state forward.
void integrator::nextState (void) {
  if (--currentstate < 0) currentstate = STATE_NUM - 1;
}

// Shifts one state backward.
void integrator::prevState (void) {
  currentstate = (currentstate + 1) & STATE_MASK;
}

/* This function applies the given value to a save-state variable through
   all history values. */
void integrator::fillState (int state, nr_double_t val) {
  nr_double_t * p = &stateval[state << STATE_SHIFT];
  for (int i = 0; i < STATE_NUM; i++) *p++ = val;
}
