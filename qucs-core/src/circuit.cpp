/*
 * circuit.cpp - circuit class implementation
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
 * $Id: circuit.cpp,v 1.3 2003-12-26 14:04:07 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "property.h"
#include "circuit.h"
#include "component_id.h"

// Constructor creates an unnamed instance of the circuit class.
circuit::circuit () : object () {
  size = 0;
  data = NULL;
  nodes = NULL;
  port = 0;
  org = 1;
  type = CIR_UNKNOWN;
}

/* Constructor creates an unnamed instance of the circuit class with a
   certain number of ports. */
circuit::circuit (int s) : object () {
  size = s;
  data = new complex[s * s];
  nodes = new node[s];
  port = 0;
  org = 1;
  type = CIR_UNKNOWN;
}

/* The copy constructor creates a new instance based on the given
   circuit object. */
circuit::circuit (const circuit & c) : object (c) {
  size = c.size;
  port = c.port;
  org = c.org;
  type = c.type;
  nodes = new node[size];
  data = new complex[size * size];

  // copy each node and set its circuit to the current circuit object
  for (int i = 0; i < size; i++) {
    node * n = new node (c.nodes[i]);
    n->setCircuit (this);
    nodes[i] = * n;
    delete n;
  }
  // copy each s parameter
  for (int i = 0; i < size * size; i++) {
    complex * z = new complex (c.data[i]);
    data[i] = * z;
    delete z;
  }
}

// Destructor deletes a circuit object.
circuit::~circuit () {
  delete[] data;
  delete[] nodes;
}

/* This function sets the name and port number of one of the circuit's
   nodes. */
void circuit::setNode (int i, char * n) {
  nodes[i - 1].setName (n);
  nodes[i - 1].setCircuit (this);
  nodes[i - 1].setPort (i);
}

// Returns one of the circuit's nodes.
node * circuit::getNode (int i) {
  return &nodes[i - 1];
}

/* This function counts the number of signals (ports) within the list
   of circuits. */
int circuit::countPorts (void) {
  int count = 0;
  for (circuit * c = this; c != NULL; c = (circuit *) c->getNext ()) {
    if (c->isPort ())
      count++;
  }
  return count;
}

/* This function counts the number of circuits within the list 
   of circuits. */
int circuit::countCircuits (void) {
  int count = 0;
  for (circuit * c = this; c != NULL; c = (circuit *) c->getNext ()) {
    count++;
  }
  return count;
}

/* This function counts the number of circuits within the list 
   of circuits. */
int circuit::countNodes (void) {
  int count = 0;
  for (circuit * c = this; c != NULL; c = (circuit *) c->getNext ()) {
    if (!c->isPort ())
      count += c->getSize ();
  }
  return count;
}

#if DEBUG
// DEBUG function:  Prints the S parameters of this circuit object.
void circuit::print (void) {
  for (int i = 1; i <= getSize (); i++) {
    for (int j = 1; j <= getSize (); j++) {
      logprint (LOG_STATUS, "%s S%d%d(%+.3e,%+.3e) ", getName (), i, j, 
		(double) real (getS (i, j)), (double) imag (getS (i, j)));
    }
    logprint (LOG_STATUS, "\n");
  }
}
#endif /* DEBUG */
