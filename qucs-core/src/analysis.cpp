/*
 * analysis.cpp - analysis class implementation
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
 * $Id: analysis.cpp,v 1.6 2004-12-07 22:33:31 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
#include "complex.h"
#include "sweep.h"
#include "vector.h"
#include "analysis.h"

// Constructor creates an unnamed instance of the analysis class.
analysis::analysis () : object () {
  data = NULL;
  subnet = NULL;
  env = NULL;
  actions = NULL;
  type = ANALYSIS_UNKNOWN;
  runs = 0;
}

// Constructor creates a named instance of the analysis class.
analysis::analysis (char * n) : object (n) {
  data = NULL;
  subnet = NULL;
  env = NULL;
  actions = NULL;
  type = ANALYSIS_UNKNOWN;
  runs = 0;
}

// Destructor deletes the analysis class object.
analysis::~analysis () {
}

/* The copy constructor creates a new instance of the analysis class
   based on the given analysis object. */
analysis::analysis (analysis & a) : object (a) {
  data = a.data;
  subnet = a.subnet;
  env = a.env;
  actions = a.actions;
  type = a.type;
  runs = a.runs;
}

/* This function adds the given analysis to the actions being
   associated with the current analysis object. */
void analysis::addAnalysis (analysis * a) {
  a->setPrev (NULL);
  a->setNext (actions);
  actions = a;
}

/* This function deletes the given analysis from the actions being
   associated with the current analysis object. */
void analysis::delAnalysis (analysis * a) {
  if (a == actions) {
    actions = (analysis *) a->getNext ();
    if (actions) actions->setPrev (NULL);
  }
  else {
    if (a->getNext ()) a->getNext()->setPrev (a->getPrev ());
    a->getPrev()->setNext (a->getNext ());
  }
}

/* The following function creates a sweep object depending on the
   analysis's properties.  Supported sweep types are: linear,
   logarithmic, lists and constants. */
sweep * analysis::createSweep (char * n) {
  sweep * swp = NULL;
  // get type of sweep
  char * type = getPropertyString ("Type");

  // linearly or logarithmically stepped sweeps
  if (!strcmp (type, "lin") || !strcmp (type, "log")) {
    nr_double_t start = getPropertyDouble ("Start");
    nr_double_t stop = getPropertyDouble ("Stop");
    int points = getPropertyInteger ("Points");
    if (!strcmp (type, "lin")) {
      swp = new linsweep (n);
      ((linsweep *) swp)->create (start, stop, points);
    }
    else if (!strcmp (type, "log")) {
      swp = new logsweep (n);
      ((logsweep *) swp)->create (start, stop, points);
    }
  }

  // lists of values and constant values
  else if (!strcmp (type, "list") || !strcmp (type, "const")) {
    vector * values = getPropertyVector ("Values");
    int points = values->getSize ();
    if (!strcmp (type, "list")) {
      swp = new lstsweep (n);
      ((lstsweep *) swp)->create (points);
    }
    else if (!strcmp (type, "const")) {
      swp = new consweep (n);
      ((consweep *) swp)->create (points);
    }
    for (int i = 0; i < values->getSize (); i++)
      swp->set (i, real (values->get (i)));
  }
  return swp;
}
