/*
 * net.cpp - net class implementation
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
 * $Id: net.cpp,v 1.13 2004-07-08 06:38:43 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "circuit.h"
#include "strlist.h"
#include "vector.h"
#include "dataset.h"
#include "net.h"
#include "tee.h"
#include "open.h"
#include "itrafo.h"
#include "analysis.h"
#include "environment.h"
#include "component_id.h"

// Constructor creates an unnamed instance of the net class.
net::net () : object () {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
  env = NULL;
}

// Constructor creates a named instance of the net class.
net::net (char * n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
  env = NULL;
}

// Destructor deletes the net class object.
net::~net () {
  circuit * n;
  for (circuit * c = root; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    delete c;
  }
  root = NULL;
}

/* The copy constructor creates a new instance of the net class based
   on the given net object. */
net::net (net & n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
  env = n.env;
}

/* This function prepends the given circuit to the list of registered
   circuits. */
void net::insertCircuit (circuit * c) {
  if (root) root->setPrev (c);
  c->setNext (root);
  c->setPrev (NULL);
  root = c;
  nCircuits++;

  // handle AC power sources as s-parameter ports
  if (c->getType () == CIR_PAC) {
    nPorts++;
    if (!c->isPort ()) c->setPort (nPorts);
  }
  // handle DC voltage sources
  if (c->getVoltageSources () > 0) {
    if (!c->isVoltageSource ()) c->setVoltageSource (nSources + 1);
    nSources += c->getVoltageSources ();
  }
}

/* The function removes the given circuit from the list of registered
   circuits. */
void net::removeCircuit (circuit * c) {

  // adjust the circuit chain appropriately
  if (c == root) {
    root = (circuit *) c->getNext ();
    if (root) root->setPrev (NULL);
  }
  else {
    if (c->getNext ()) c->getNext()->setPrev (c->getPrev ());
    c->getPrev()->setNext (c->getNext ());
  }
  nCircuits--;
  if (c->isPort ()) nPorts--;
  if (c->isVoltageSource ()) nSources -= c->getVoltageSources ();

  // shift the circuit object to the drop list
  if (c->isOriginal ()) {
    if (drop) drop->setPrev (c);
    c->setNext (drop);
    c->setPrev (NULL);
    drop = c;
  }
  // really destroy the circuit object
  else delete c;
}

/* This function prepends the given analysis to the list of registered
   analyses. */
void net::insertAnalysis (analysis * a) {
  if (actions) actions->setPrev (a);
  a->setNext (actions);
  a->setPrev (NULL);
  actions = a;
}

/* The function removes the given analysis from the list of registered
   analyses. */
void net::removeAnalysis (analysis * a) {

  // adjust the analysis chain appropriately
  if (a == actions) {
    actions = (analysis *) a->getNext ();
    if (actions) actions->setPrev (NULL);
  }
  else {
    if (a->getNext ()) a->getNext()->setPrev (a->getPrev ());
    a->getPrev()->setNext (a->getNext ());
  }
}

/* The function returns the analysis associated with the netlist
   object specified by the given instance name and returns NULL if
   there is no such analysis. */
analysis * net::findAnalysis (char * n) {
  if (n == NULL) return NULL;
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    if (!strcmp (a->getName (), n))
      return a;
  }
  return NULL;
}

analysis * net::findAnalysis (int type) {
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    if (a->getType () == type)
      return a;
  }
  return NULL;
}

/* This function runs all registered analyses applied to the current
   netlist. */
dataset * net::runAnalysis (void) {
  dataset * out = new dataset ();

  // apply some data to all analyses
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    a->setEnv (env);
    a->setNet (this);
    a->setData (out);
  }

  orderAnalysis ();
  // solve the analyses
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    a->solve ();
  }
  return out;
}

/* The function returns the analysis with the second lowest order.  If
   there is no recursive sweep it returns NULL. */
analysis * net::findSecondOrder (void) {
  analysis * parent = NULL;
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    // parameter sweeps are potential parent sweeps
    if (a->getType () == ANALYSIS_SWEEP) {
      // find the appropriate sub analysis
      analysis * child = getChildAnalysis (a);
      if (child != NULL) {
	// check if child is not another variable sweep
	if (child->getType () != ANALYSIS_SWEEP) {
	  parent = a;
	  break;
	}
	// check if the child's child is still in the analysis list
	else if (getChildAnalysis (child) == NULL) {
	  parent = a;
	  break;
	}
      }
    }
  }
  return parent;
}

/* The function reorders (prioritizes) the registered analysis to the
   netlist object.  In fact it chains the analyses to be executed in
   a certain order. */
void net::orderAnalysis (void) {
  analysis * parent, * child, * a;
  analysis * dc = findAnalysis (ANALYSIS_DC);
  int dcApplied = 0;
  do {
    // get second order sweep
    if ((parent = findSecondOrder ()) != NULL) {
      child = getChildAnalysis (parent);
      removeAnalysis (child);
      // apply sub-analysis to each parent analysis if any
      for (a = actions; a != NULL; a = (analysis *) a->getNext ()) {
	char * cn = getChild (a);
	if (cn != NULL && !strcmp (cn, child->getName ())) {
	  a->addAnalysis (child);
	  // apply DC analysis if necessary
	  if (child->getType () != ANALYSIS_DC && 
	      child->getType () != ANALYSIS_SWEEP && dc != NULL) {
	    removeAnalysis (dc);
	    a->addAnalysis (dc);
	    dcApplied++;
	  }
	}
      }
      // sort the sub-analysis of each parent
      for (a = actions; a != NULL; a = (analysis *) a->getNext ()) {
	sortChildAnalyses (a);
      }
    }
  } while (parent != NULL);

  // sort the parent analyses
  parent = new analysis ();
  parent->setAnalysis (actions);
  sortChildAnalyses (parent);
  actions = parent->getAnalysis ();
  delete parent;
}

// This function sorts the analyses of the given parent analysis.
void net::sortChildAnalyses (analysis * parent) {
  analysis * next, * alist = parent->getAnalysis ();
  for (analysis * a = alist; a != NULL; a = next) {
    next = (analysis *) a->getNext ();
    if (a->getType () == ANALYSIS_DC) {
      parent->delAnalysis (a);
      parent->addAnalysis (a);
    }
  }
}

// Returns the instance name of the given parents child analysis.
char * net::getChild (analysis * parent) {
  char * child = NULL;
  if (parent != NULL && parent->getType () == ANALYSIS_SWEEP)
    child = parent->getPropertyString ("Sim");
  return child;
}

// Returns the child analysis of the given parent if possible.
analysis * net::getChildAnalysis (analysis * parent) {
  return findAnalysis (getChild (parent));;
}

// Returns the last order sweep being not an parameter sweep.
analysis * net::findLastOrder (analysis * a) {
  analysis * child = a->getAnalysis ();
  if (child != NULL && child->getType () == ANALYSIS_SWEEP) {
    return findLastOrder (child);
  }
  return child ? child : a;
}

/* The function re-shifts all circuits in the drop list to the actual
   list of circuit objects. */
void net::getDroppedCircuits (void) {
  circuit * n;
  for (circuit * c = drop; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    insertCircuit (c);
  }
  drop = NULL;
}

/* This function deletes all unnecessary circuits in the list of
   registered circuit objects. */
void net::deleteUnusedCircuits (void) {
  circuit * n;
  for (circuit * c = root; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    if (!c->isOriginal ()) removeCircuit (c);
  }
}

/* Returns the first node in the list of real circuit objects
   connected to the given node.  If there is no such node (unconnected
   node) the function returns NULL. */
node * net::findConnectedCircuitNode (node * n) {

  char * _name = n->getName ();
  node * _node;

  // through the list of circuit objects
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    // skip signal circuits
    if (c->isPort ()) continue;
    // through the list of nodes in a circuit
    for (int i = 1; i <= c->getSize (); i++) {
      _node = c->getNode (i);
      if (!strcmp (_node->getName (), _name)) {
	if (_node != n) {
	  return _node;
	}
      }
    }
  }
  return NULL;
}

/* Returns the first node in the list of circuit objects (including
   signals) connected to the given node.  If there is no such node
   (unconnected node) the function returns NULL. */
node * net::findConnectedNode (node * n) {

  char * _name = n->getName ();
  node * _node;

  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    for (int i = 1; i <= c->getSize (); i++) {
      _node = c->getNode (i);
      if (!strcmp (_node->getName (), _name)) {
	if (_node != n) {
	  return _node;
	}
      }
    }
  }
  return NULL;
}

// Rename the given circuit and mark it as being a reduced one.
void net::reducedCircuit (circuit * c) {
  char n[32];
  sprintf (n, "reduced%d", reduced++);
  c->setName (n);
}

/* Rename the given circuit and mark it as being a inserted one and
   remember when it was inserted. */
void net::insertedCircuit (circuit * c) {
  char n[32];
  sprintf (n, "inserted%d", inserted);
  c->setName (n);
  c->setInserted (inserted);
  inserted++;
}

// Rename the given node and mark it as being a inserted one.
void net::insertedNode (node * c) {
  char n[32];
  sprintf (n, "inode%d", insertedNodes++);
  c->setName (n);
}

#if DEBUG
// DEBUG function: Lists the netlist.
void net::list (void) {
  logprint (LOG_STATUS, "DEBUG: netlist `%s'\n", getName ());
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    logprint (LOG_STATUS, "       %s[", c->getName ());
    for (int i = 1; i <= c->getSize (); i++) {
      logprint (LOG_STATUS, "%s-%d", 
		c->getNode(i)->getName (), c->getNode(i)->getNode ());
      if (i != c->getSize ())
	logprint (LOG_STATUS, ",");
    }
    logprint (LOG_STATUS, "] { %s }\n", c->propertyList ());
  }
}
#endif /* DEBUG */
