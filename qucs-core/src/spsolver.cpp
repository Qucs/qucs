/*
 * spsolver.cpp - S-parameter solver class implementation
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
 * $Id: spsolver.cpp,v 1.1 2003-12-21 13:25:38 ela Exp $
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
#include "spsolver.h"

// Constructor creates an unnamed instance of the spsolver class.
spsolver::spsolver () : analysis () {
}

// Constructor creates a named instance of the spsolver class.
spsolver::spsolver (char * n) : analysis (n) {
}

// Destructor deletes the spsolver class object.
spsolver::~spsolver () {
}

/* The copy constructor creates a new instance of the spsolver class
   based on the given spsolver object. */
spsolver::spsolver (spsolver & n) : analysis (n) {
}

/* This function joins two nodes of a single circuit (interconnected
   nodes) and returns the resulting circuit. */
circuit * spsolver::interconnectJoin (node * n1, node * n2) {

  circuit * s = n1->getCircuit ();
  circuit * result = new circuit (s->getSize () - 2);
  complex p;

  // interconnected port numbers
  int k = n1->getPort (), l = n2->getPort ();

  int j2; // column index for resulting matrix
  int i2; // row index for resulting matrix
  int j1; // column index for S matrix
  int i1; // row index for S matrix

  // handle single S block only
  i2 = j2 = 1;
  for (j1 = 1; j1 <= s->getSize (); j1++) {

    // skip connected node
    if (j1 == k || j1 == l) continue;

    // assign node name of resulting circuit
    result->setNode (j2, s->getNode(j1)->getName());

    // inside S only
    for (i1 = 1; i1 <= s->getSize (); i1++) {

      // skip connected node
      if (i1 == k || i1 == l) continue;

      // compute S'ij
      p = s->getS (i1, j1);
      p += 
	(s->getS (k, j1) * s->getS (i1, l) * (1.0 - s->getS (l, k)) + 
	 s->getS (l, j1) * s->getS (i1, k) * (1.0 - s->getS (k, l)) + 
	 s->getS (k, j1) * s->getS (l, l) * s->getS (i1, k) +
	 s->getS (l, j1) * s->getS (k, k) * s->getS (i1, l)) / 
	((1.0 - s->getS (k, l)) * (1.0 - s->getS (l, k)) -
	 s->getS (k, k) * s->getS (l, l));
      result->setS (i2++, j2, p);
    }

    // next column
    j2++; i2 = 1;
  }
  return result;
}

/* This function joins two nodes of two different circuits (connected
   nodes) and returns the resulting circuit. */
circuit * spsolver::connectedJoin (node * n1, node * n2) {

  circuit * s = n1->getCircuit ();
  circuit * t = n2->getCircuit ();
  circuit * result = new circuit (s->getSize () + t->getSize () - 2);
  complex p;

  // connected port numbers
  int k = n1->getPort (), l = n2->getPort ();

  int j2; // column index for resulting matrix
  int i2; // row index for resulting matrix
  int j1; // column index for S matrix
  int i1; // row index for S matrix

  // handle S block
  i2 = j2 = 1;
  for (j1 = 1; j1 <= s->getSize (); j1++) {

    // skip connected node
    if (j1 == k) continue;

    // assign node name of resulting circuit
    result->setNode (j2, s->getNode(j1)->getName());

    // inside S
    for (i1 = 1; i1 <= s->getSize (); i1++) {

      // skip connected node
      if (i1 == k) continue;

      // compute S'ij
      p = s->getS (i1, j1);
      p +=
	(s->getS (k, j1) * t->getS (l, l) * s->getS (i1, k)) /
	(1.0 - s->getS (k, k) * t->getS (l, l));
      result->setS (i2++, j2, p);
    }

    // across S and T
    for (i1 = 1; i1 <= t->getSize (); i1++) {

      // skip connected node
      if (i1 == l) continue;

      // compute S'mj
      p =
	(s->getS (k, j1) * t->getS (i1, l)) /
	(1.0 - s->getS (k, k) * t->getS (l, l));
      result->setS (i2++, j2, p);
    }
    // next column
    j2++; i2 = 1;
  }

  // handle T block
  for (j1 = 1; j1 <= t->getSize (); j1++) {

    // skip connected node
    if (j1 == l) continue;

    // assign node name of resulting circuit
    result->setNode (j2, t->getNode(j1)->getName());

    // across T and S
    for (i1 = 1; i1 <= s->getSize (); i1++) {

      // skip connected node
      if (i1 == k) continue;

      // compute S'mj
      p =
	(t->getS (l, j1) * s->getS (i1, k)) /
	(1.0 - t->getS (l, l) * s->getS (k, k));
      result->setS (i2++, j2, p);
    }

    // inside T
    for (i1 = 1; i1 <= t->getSize (); i1++) {

      // skip connected node
      if (i1 == l) continue;

      // compute S'ij
      p = t->getS (i1, j1);
      p +=
	(t->getS (l, j1) * s->getS (k, k) * t->getS (i1, l)) /
	(1.0 - t->getS (l, l) * s->getS (k, k));
      result->setS (i2++, j2, p);
    }

    // next column
    j2++; i2 = 1;
  }

  return result;
}

/* Goes through the list of circuit objects and runs its frequency
   dependent calc() function. */
void spsolver::calc (nr_double_t freq) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    c->calc (freq);
  }
}

/* Go through each registered circuit object in the list and find the
   connection which results in a new subnetwork with the smallest
   number of s-parameters to calculate. */
void spsolver::reduce (void) {

  node * n1, * n2, * cand;
  circuit * result, * c1, * c2, * cand1, * cand2;
  int ports;
  circuit * root = subnet->getRoot ();

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
      if ((cand = subnet->findConnectedCircuitNode (c->getNode (i))) != NULL) {

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
      result = connectedJoin (n1, n2);
      subnet->reducedCircuit (result);
      subnet->removeCircuit (cand1);
      subnet->removeCircuit (cand2);
      subnet->insertCircuit (result);
      result->setOriginal (0);
    }
    // interconnect
    else {
#if DEBUG
      logprint (LOG_STATUS, "DEBUG: interconnected node (%s): %s\n",
		n1->getName (), cand1->getName ());
#endif
      result = interconnectJoin (n1, n2);
      subnet->reducedCircuit (result);
      subnet->removeCircuit (cand1);
      subnet->insertCircuit (result);
      result->setOriginal (0);
    }
  }
}

/* This is the netlist solver.  It prepares the circuit list for each
   requested frequency and solves it then. */
void spsolver::solve (void) {

  nr_double_t start, stop, step, freq;
  int ports;
  circuit * root;

  // get frequency range
  start = getPropertyDouble ("Start");
  stop  = getPropertyDouble ("Stop");
  step  = getPropertyDouble ("Step");

  insertConnections ();

  for (freq = start; freq <= stop; freq += step) {

    root = subnet->getRoot ();
    ports = root->countNodes ();
    subnet->setReduced (0);
    calc (freq);
    
#if DEBUG
    logprint (LOG_STATUS, "NOTIFY: solving netlist for f = %e\n", 
	      (double) freq);
#endif
    
    while (ports > subnet->getPorts ()) {
      reduce ();
      ports -= 2;
    }

    saveResults (freq);
    subnet->getDroppedCircuits ();
    subnet->deleteUnusedCircuits ();
  }
}

/* The function goes through the list of circuit objects and creates T
   circuits if necessary.  It looks for nodes in the circuit list
   connected to the given node. */
void spsolver::insertTee (node * n) {

  int count = 0;
  node * nodes[4], * _node;
  tee * result;
  char * _name = n->getName ();
  circuit * root = subnet->getRoot ();

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
	    subnet->insertedCircuit (result);
	    result->setNode (1, _name);
	    subnet->insertedNode (result->getNode (2));
	    subnet->insertedNode (result->getNode (3));

	    // rename the nodes connected to the T
	    nodes[1]->setName (result->getNode(2)->getName ());
	    nodes[2]->setName (result->getNode(3)->getName ());

	    // complete the nodes of the T
	    result->getNode(2)->setCircuit (result);
	    result->getNode(3)->setCircuit (result);
	    result->getNode(2)->setPort (2);
	    result->getNode(3)->setPort (3);

	    // put the T in the circuit list
	    subnet->insertCircuit (result);
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
void spsolver::insertOpen (node * n) {

  circuit * result;

  if (subnet->findConnectedNode (n) == NULL) {
    result = new open ();
    subnet->insertedCircuit (result);
    result->setNode (1, n->getName());
    subnet->insertCircuit (result);
  }
}

/* This function prepares the circuit list by adding Ts and opens to
   the circuit list.  With this adjustments the solver is able to
   solve the circuit. */
void spsolver::insertConnections (void) {

  circuit * root = subnet->getRoot ();
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
void spsolver::insertDifferentialPorts (void) {
  circuit * root = subnet->getRoot ();
  for (circuit * c = root; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isPort ()) {

      // create an ideal transformator and assign its node names
      circuit * result = new itrafo ();
      subnet->insertedCircuit (result);
      subnet->insertedNode (result->getNode (1));
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
      subnet->insertCircuit (result);
    }
  }
}

/* This function saves the results of a single solve() functionality
   (for the given frequency) into the output dataset. */
void spsolver::saveResults (nr_double_t freq) {

  vector * f, * s;
  node * sig_i, * sig_j;
  char * n;
  int res_i, res_j;
  circuit * root = subnet->getRoot ();

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
	  sig_i = subnet->findConnectedNode (c->getNode (i));
	  sig_j = subnet->findConnectedNode (c->getNode (j));
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
char * spsolver::createSP (int i, int j) {
  static char text[16];
  sprintf (text, "S%d%d", i, j);
  return text;
}
