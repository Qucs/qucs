/*
 * net.cpp - net class implementation
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: net.cpp,v 1.3 2003-12-26 14:04:07 ela Exp $
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
#include "component_id.h"

// Constructor creates an unnamed instance of the net class.
net::net () : object () {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
}

// Constructor creates a named instance of the net class.
net::net (char * n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
}

// Destructor deletes the net class object.
net::~net () {
}

/* The copy constructor creates a new instance of the net class based
   on the given net object. */
net::net (net & n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = nSources = 0;
  insertedNodes = inserted = reduced = 0;
  actions = NULL;
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
  else if (c->getType () == CIR_VDC) {
    nSources++;
    if (!c->isVoltageSource ()) c->setVoltageSource (nSources);
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
  if (c->isVoltageSource ()) nSources--;

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
  delete a;
}

/* This function runs all registered analyses applied to the current
   netlist. */
dataset * net::runAnalysis (void) {
  dataset * out = new dataset ();
  for (analysis * a = actions; a != NULL; a = (analysis *) a->getNext ()) {
    // solve the analysis
    a->setNet (this);
    a->setData (out);
    a->solve ();
  }
  return out;
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

// Rename the given circuit and mark it as being a inserted one.
void net::insertedCircuit (circuit * c) {
  char n[32];
  sprintf (n, "inserted%d", inserted++);
  c->setName (n);
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
    logprint (LOG_STATUS, "]\n");
    c->listProperties ();
  }
}
#endif /* DEBUG */
