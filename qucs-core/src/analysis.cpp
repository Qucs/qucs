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
 * $Id: analysis.cpp,v 1.5 2004/05/17 19:50:51 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "object.h"
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
