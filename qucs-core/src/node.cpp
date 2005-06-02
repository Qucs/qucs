/*
 * node.cpp - node class implementation
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: node.cpp,v 1.3 2005/06/02 18:17:51 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "complex.h"
#include "object.h"
#include "node.h"

// Constructor creates an unnamed instance of the node class.
node::node () : object () {
  internal = port = nNode = 0;
  _circuit = NULL;
}

// Constructor creates a named instance of the node class.
node::node (char * n) : object (n) {
  internal = port = nNode = 0;
  _circuit = NULL;
}

/* The copy constructor creates a new instance of the node class based
   on the given node object. */
node::node (const node & n) : object (n) {
  internal = n.internal;
  port = n.port;
  nNode = n.nNode;
  _circuit = n._circuit;
}

// Destructor destroys a node object.
node::~node () {
}


// Sets the unique number of this node.
void node::setNode (int n) {
  nNode = n;
}

// Returns the unique number of this node.
int node::getNode (void) {
  return nNode;
}

// Sets the port number of this node.
void node::setPort (int n) {
  port = n;
}

// Returns the port number of this node.
int node::getPort (void) {
  return port;
}

// Sets this node's circuit.
void node::setCircuit (circuit * c) {
  _circuit = c;
}

// Returns this node's circuit.
circuit * node::getCircuit (void) {
  return _circuit;
}
