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
 * $Id: net.cpp,v 1.1 2003/12/20 19:03:25 ela Exp $
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

// Constructor creates an unnamed instance of the net class.
net::net () : object () {
  root = drop = NULL;
  nPorts = nCircuits = 0;
  insertedNodes = inserted = reduced = 0;
  data = new dataset ("SP");
}

// Constructor creates a named instance of the net class.
net::net (char * n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = 0;
  insertedNodes = inserted = reduced = 0;
  data = new dataset ("SP");
}

// Destructor deletes the net class object.
net::~net () {
  delete data;
}

/* The copy constructor creates a new instance of the net class based
   on the given net object. */
net::net (net & n) : object (n) {
  root = drop = NULL;
  nPorts = nCircuits = 0;
  insertedNodes = inserted = reduced = 0;
}

/* This function prepends the given circuit to the list of registered
   circuits. */
void net::insertCircuit (circuit * c) {
  if (root) root->setPrev (c);
  c->setNext (root);
  c->setPrev (NULL);
  root = c;
  nCircuits++;
}

/* This function prepends the given signal circuit to the list of
   registered circuits. */
void net::insertPort (circuit * c) {
  insertCircuit (c);
  nPorts++;
  if (!c->isPort ()) c->setPort (nPorts);
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

/* The function re-shifts all circuits in the drop list to the actual
   list of circuit objects. */
void net::getDroppedCircuits (void) {
  circuit * n;
  for (circuit * c = drop; c != NULL; c = n) {
    n = (circuit *) c->getNext ();
    if (c->isPort ())
      insertPort (c);
    else
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

/* Goes through the list of circuit objects and runs its frequency
   dependent calc() function. */
void net::calc (nr_double_t freq) {
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calc (freq);
  }
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

/* Go through each registered circuit object in the list and find the
   connection which results in a new subnetwork with the smallest
   number of s-parameters to calculate. */
void net::reduce (void) {

  node * n1, * n2, * cand;
  circuit * result, * c1, * c2, * cand1, * cand2;
  int ports;

  // initialize local variables
  result = c1 = c2 = cand1 = cand2 = NULL;
  n1 = n2 = cand = NULL;
  ports = 10000; // huge

  // go through the circuit list
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {

    // skip signal ports
    if (c->isPort ()) continue;

    // and each node in the circuit
    for (int i = 1; i <= c->getSize (); i++) {

      // find duplicate node
      if ((cand = findConnectedCircuitNode (c->getNode (i))) != NULL) {

	// save both candidates
	c1 = c; c2 = cand->getCircuit ();
	// connected
	if (c1 != c2) {
	  if (c1->getSize () + c2->getSize () - 2 < ports) {
	    ports = c1->getSize () + c2->getSize () - 2;
	    cand1 = c1; cand2 = c2; n1 = c1->getNode (i); n2 = cand;
	  }
	}
	// interconnect
	else {
	  if (c1->getSize () - 2 < ports) {
	    ports = c1->getSize () - 2;
	    cand1 = c1; cand2 = c2; n1 = c1->getNode (i); n2 = cand;
	  }
	}
      }
    }
  }

  // found a connection ?
  if (cand1 != NULL && cand2 != NULL) {
    // connected
    if (cand1 != cand2) {
#if DEBUG
      logprint (LOG_STATUS, "DEBUG: connected node (%s): %s - %s\n",
		n1->getName (), cand1->getName (), cand2->getName ());
#endif /* DEBUG */
      result = cand1->connectedJoin (n1, n2);
      reducedCircuit (result);
      removeCircuit (cand1);
      removeCircuit (cand2);
      insertCircuit (result);
      result->setOriginal (0);
    }
    // interconnect
    else {
#if DEBUG
      logprint (LOG_STATUS, "DEBUG: interconnected node (%s): %s\n",
		n1->getName (), cand1->getName ());
#endif
      result = cand1->interconnectJoin (n1, n2);
      reducedCircuit (result);
      removeCircuit (cand1);
      insertCircuit (result);
      result->setOriginal (0);
    }
  }
}

/* This is the netlist solver.  It prepares the circuit list for each
   requested frequency and solves it then. */
void net::solve (void) {

  nr_double_t start, stop, step, freq;
  int ports;

  // get frequency range
  start = getPropertyDouble ("Start");
  stop  = getPropertyDouble ("Stop");
  step  = getPropertyDouble ("Step");

  insertConnections ();

  for (freq = start; freq <= stop; freq += step) {

    ports = root->countNodes ();
    reduced = 0;
    calc (freq);
    
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: solving netlist for f = %e\n", 
	      (double) freq);
#endif
    
    while (ports > nPorts) {
      reduce ();
      ports -= 2;
    }

    saveResults (freq);
    getDroppedCircuits ();
    deleteUnusedCircuits ();
  }
}

#if DEBUG
// DEBUG function: Lists the netlist.
void net::list (void) {
  logprint (LOG_STATUS, "DEBUG: netlist `%s'\n", getName ());
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    logprint (LOG_STATUS, "       %s[", c->getName ());
    for (int i = 1; i <= c->getSize (); i++) {
      logprint (LOG_STATUS, "%s-%s", 
		c->getNode(i)->getName (), c->getNode(i)->getNode ());
      if (i != c->getSize ())
	logprint (LOG_STATUS, ",");
    }
    logprint (LOG_STATUS, "] ");
    c->listProperties ();
  }
}
#endif /* DEBUG */

/* The function goes through the list of circuit objects and creates T
   circuits if necessary.  It looks for nodes in the circuit list
   connected to the given node. */
void net::insertTee (node * n) {

  int count = 0;
  node * nodes[4], * _node;
  tee * result;
  char * _name = n->getName ();

  nodes[0] = n;

  // go through list of circuit objects
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    // and each node in a circuit
    for (int i = 1; i <= c->getSize (); i++) {
      _node = c->getNode (i);
      if (!strcmp (_node->getName (), _name)) {
	if (_node != n) {

	  // found a connected node
	  nodes[++count] = _node;
	  if (count == 2) {

	    // create a T and assign its node names
	    result = new tee ();
	    insertedCircuit (result);
	    result->setNode (1, _name);
	    insertedNode (result->getNode (2));
	    insertedNode (result->getNode (3));

	    // rename the nodes connected to the T
	    nodes[1]->setName (result->getNode(2)->getName ());
	    nodes[2]->setName (result->getNode(3)->getName ());

	    // complete the nodes of the T
	    result->getNode(2)->setCircuit (result);
	    result->getNode(3)->setCircuit (result);
	    result->getNode(2)->setPort (2);
	    result->getNode(3)->setPort (3);

	    // put the T in the circuit list
	    insertCircuit (result);
	    nodes[1] = result->getNode (1);
	    count = 1;
	  }
	}
      }
    }
  }
}

/* The function adds an open to the circuit list if the given node is
   unconnected. */
void net::insertOpen (node * n) {

  circuit * result;

  if (findConnectedNode (n) == NULL) {
    result = new open ();
    insertedCircuit (result);
    result->setNode (1, n->getName());
    insertCircuit (result);
  }
}

/* This function prepares the circuit list by adding Ts and opens to
   the circuit list.  With this adjustments the solver is able to
   solve the circuit. */
void net::insertConnections (void) {

#if DEBUG
  logprint (LOG_STATUS, "NOTIFY: preparing circuit for analysis\n");
#endif /* DEBUG */
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    for (int i = 1; i <= c->getSize (); i++) {
      insertTee (c->getNode (i));
      insertOpen (c->getNode (i));
    }
  }
  insertDifferentialPorts ();
}

/* This function inserts an ideal transformator before an AC power
   source in order to allow differential S parameter ports.  */
void net::insertDifferentialPorts (void) {
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isPort ()) {

      // create an ideal transformator and assign its node names
      circuit * result = new itrafo ();
      insertedCircuit (result);
      insertedNode (result->getNode (1));
      result->setNode (2, c->getNode(1)->getName ());
      result->setNode (3, c->getNode(2)->getName ());

      // rename the nodes connected to the trafo
      c->getNode(1)->setName (result->getNode(1)->getName ());
      c->getNode(2)->setName ("PacGround");

      // complete the nodes of the trafo
      result->getNode(1)->setCircuit (result);
      result->getNode(1)->setPort (1);

      // pass the port impedance to the ideal trafo
      result->addProperty ("Z", c->getPropertyDouble ("Z"));

      // put the trafo in the circuit list
      insertCircuit (result);
    }
  }
}

/* This function saves the results of a single solve() functionality
   (for the given frequency) into the output dataset. */
void net::saveResults (nr_double_t freq) {

  vector * f, * s;
  node * sig_i, * sig_j;
  char * n;
  int res_i, res_j;

  // add current frequency to the dependency of the output dataset
  if ((f = data->findDependency ("frequency")) == NULL) {
    f = new vector ("frequency");
    data->addDependency (f);
  }
  f->add (freq);

  // go through the list of remaining circuits
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    // skip signals
    if (!c->isPort ()) {
      // handle each s-parameter
      for (int i = 1; i <= c->getSize (); i++) {
	for (int j = 1; j <= c->getSize (); j++) {

	  // generate the appropriate variable name
	  sig_i = findConnectedNode (c->getNode (i));
	  sig_j = findConnectedNode (c->getNode (j));
	  res_i = sig_i->getCircuit()->getPropertyInteger ("Num");
	  res_j = sig_j->getCircuit()->getPropertyInteger ("Num");
	  n = createSP (res_i, res_j);

	  // add variable data item to dataset
	  if ((s = data->findVariable (n)) == NULL) {
	    s = new vector (n);
	    s->setDependencies (new strlist ());
	    s->getDependencies()->add (f->getName ());
	    data->addVariable (s);
	  }
	  s->add (c->getS (i, j));
	}
      }
    }
  }
}

// Create an appropriate variable name.
char * net::createSP (int i, int j) {
  static char text[16];
  sprintf (text, "S%d%d", i, j);
  return text;
}

// Returns the output dataset.
dataset * net::getData (void) {
  return data;
}
