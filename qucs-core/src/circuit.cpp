/*
 * circuit.cpp - circuit class implementation
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
 * $Id: circuit.cpp,v 1.25 2004-09-06 06:40:07 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "matrix.h"
#include "node.h"
#include "property.h"
#include "circuit.h"
#include "microstrip/substrate.h"
#include "operatingpoint.h"
#include "component_id.h"

// Constructor creates an unnamed instance of the circuit class.
circuit::circuit () : object () {
  size = 0;
  MatrixN = MatrixS = MatrixY = NULL;
  nodes = NULL;
  pacport = 0;
  original = 1;
  subst = NULL;
  vsource = 0;
  vsources = 0;
  oper = NULL;
  inserted = -1;
  linear = 1;
  enabled = 0;
  subcircuit = NULL;
  type = CIR_UNKNOWN;
}

/* Constructor creates an unnamed instance of the circuit class with a
   certain number of ports. */
circuit::circuit (int s) : object () {
  assert (s >= 0 /* && s <= MAX_CIR_PORTS */);
  size = s;
  if (size > 0) {
    MatrixS = new complex[s * s];
    MatrixN = new complex[s * s];
    MatrixY = new complex[s * s];
    nodes = new node[s];
  }
  pacport = 0;
  original = 1;
  subst = NULL;
  vsource = 0;
  vsources = 0;
  oper = NULL;
  inserted = -1;
  linear = 1;
  subcircuit = NULL;
  enabled = 0;
  type = CIR_UNKNOWN;
}

/* The copy constructor creates a new instance based on the given
   circuit object. */
circuit::circuit (const circuit & c) : object (c) {
  size = c.size;
  pacport = c.pacport;
  original = c.original;
  type = c.type;
  subst = c.subst;
  vsource = c.vsource;
  vsources = c.vsources;
  inserted = c.inserted;
  linear = c.linear;
  enabled = 0;
  subcircuit = c.subcircuit ? strdup (c.subcircuit) : NULL;

  if (size > 0) {
    // copy each node and set its circuit to the current circuit object
    nodes = new node[size];
    for (int i = 0; i < size; i++) {
      node * n = new node (c.nodes[i]);
      n->setCircuit (this);
      nodes[i] = * n;
      delete n;
    }
    // copy each s-parameter
    MatrixS = new complex[size * size];
    memcpy (MatrixS, c.MatrixS, size * size * sizeof (complex));
    // copy each noise-correlation-parameter
    MatrixN = new complex[size * size];
    memcpy (MatrixN, c.MatrixN, size * size * sizeof (complex));
    // copy each G-MNA entry
    MatrixY = new complex[size * size];
    memcpy (MatrixY, c.MatrixY, size * size * sizeof (complex));
  }
  else {
    nodes = NULL;
    MatrixS = MatrixN = MatrixY = NULL;
  }

  // copy operating points
  copyOperatingPoints (c.oper);
}

// Destructor deletes a circuit object.
circuit::~circuit () {
  if (size > 0) {
    delete[] MatrixS;
    delete[] MatrixN;
    delete[] MatrixY;
    delete[] nodes;
  }
  if (subcircuit) free (subcircuit);
  deleteOperatingPoints ();
}

/* With this function the number of ports of the circuit object can be
   changed.  Previously stored node and matrix information gets
   completely lost except the current size equals the given size. */
void circuit::setSize (int s) {
  // nothing to do here
  if (size == s) return;
  assert (s >= 0 /* && s <= MAX_CIR_PORTS */);

  if (size > 0) {
    // destroy any matrix and node information
    delete[] MatrixS;
    delete[] MatrixN;
    delete[] MatrixY;
    delete[] nodes;
    nodes = NULL;
    MatrixS = MatrixN = MatrixY = NULL;
  }

  if ((size = s) > 0) {
    // re-create matrix and node information space
    nodes = new node[size];
    MatrixS = new complex[size * size];
    MatrixN = new complex[size * size];
    MatrixY = new complex[size * size];
  }
}

/* This function sets the name and port number of one of the circuit's
   nodes.  It also tells the appropriate node about the circuit it
   belongs to.  The optional 'intern' argument is used to mark a node
   to be for internal use only. */
void circuit::setNode (int i, char * n, int intern) {
  nodes[i - 1].setName (n);
  nodes[i - 1].setCircuit (this);
  nodes[i - 1].setPort (i);
  nodes[i - 1].setInternal (intern);
}

// Returns one of the circuit's nodes.
node * circuit::getNode (int i) {
  return &nodes[i - 1];
}

// Sets the subcircuit reference for the circuit object.
void circuit::setSubcircuit (char * n) {
  if (subcircuit) free (subcircuit);
  subcircuit = n ? strdup (n) : NULL;
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

/* Returns the current substrate of the circuit object.  Used for
   microstrip components only. */
substrate * circuit::getSubstrate (void) { 
  return subst;
}

// Sets the substrate of the circuit object.
void circuit::setSubstrate (substrate * s) {
  subst = s;
}

/* Returns the circuits B-MNA matrix value of the given voltage source
   built in the circuit depending on the port number. */
complex circuit::getB (int port, int nr) {
  return MatrixB[(nr - vsource) * size + port - 1];
}

/* Sets the circuits B-MNA matrix value of the given voltage source
   built in the circuit depending on the port number. */
void circuit::setB (int port, int nr, complex z) {
  MatrixB[(nr - 1) * size + port - 1] = z;
}

/* Returns the circuits C-MNA matrix value of the given voltage source
   built in the circuit depending on the port number. */
complex circuit::getC (int nr, int port) {
  return MatrixC[(nr - vsource) * size + port - 1];
}

/* Sets the circuits C-MNA matrix value of the given voltage source
   built in the circuit depending on the port number. */
void circuit::setC (int nr, int port, complex z) {
  MatrixC[(nr - 1) * size + port - 1] = z;
}

/* Returns the circuits D-MNA matrix value of the given voltage source
   built in the circuit. */
complex circuit::getD (int r, int c) {
  return MatrixD[(r - vsource) * vsources + c - vsource];
}

/* Sets the circuits D-MNA matrix value of the given voltage source
   built in the circuit. */
void circuit::setD (int r, int c, complex z) {
  MatrixD[(r - 1) * vsources + c - 1] = z;
}

/* Returns the circuits E-MNA matrix value of the given voltage source
   built in the circuit. */
complex circuit::getE (int nr) {
  return MatrixE[nr - vsource];
}

/* Sets the circuits E-MNA matrix value of the given voltage source
   built in the circuit. */
void circuit::setE (int nr, complex z) {
  MatrixE[nr - 1] = z;
}

/* Returns the circuits I-MNA matrix value of the current source built
   in the circuit. */
complex circuit::getI (int port) {
  return MatrixI[port - 1];
}

/* Sets the circuits I-MNA matrix value of the current source built in
   the circuit depending on the port number. */
void circuit::setI (int port, complex z) {
  MatrixI[port - 1] = z;
}

// Returns the circuits voltage value at the given port.
complex circuit::getV (int port) {
  return MatrixV[port - 1];
}

// Sets the circuits voltage value at the given port.
void circuit::setV (int port, complex z) {
  MatrixV[port - 1] = z;
}

/* Returns the circuits G-MNA matrix value depending on the port
   numbers. */
complex circuit::getY (int r, int c) {
  return MatrixY[(r - 1) * size + c - 1];
}

/* Sets the circuits G-MNA matrix value depending on the port
   numbers. */
void circuit::setY (int r, int c, complex y) {
  MatrixY[(r - 1) * size + c - 1] = y;
}

/* Returns the circuits G-MNA matrix value depending on the port
   numbers. */
nr_double_t circuit::getG (int r, int c) {
  return real (MatrixY[(r - 1) * size + c - 1]);
}

/* Sets the circuits G-MNA matrix value depending on the port
   numbers. */
void circuit::setG (int r, int c, nr_double_t y) {
  MatrixY[(r - 1) * size + c - 1] = y;
}

/* This function adds a operating point consisting of a key and a
   value to the circuit. */
void circuit::addOperatingPoint (char * n, nr_double_t val) {
  operatingpoint * p = new operatingpoint (n, val);
  p->setNext (oper);
  oper = p;
}

/* Returns the requested operating point value which has been
   previously added as its double representation.  If there is no such
   operating point the function returns zero. */
nr_double_t circuit::getOperatingPoint (char * n) {
  operatingpoint * p = oper->findOperatingPoint (n);
  if (p != NULL) return p->getValue ();
  return 0.0;
}

/* This function sets the operating point specified by the given name
   to the value passed to the function. */
void circuit::setOperatingPoint (char * n, nr_double_t val) {
  operatingpoint * p = oper->findOperatingPoint (n);
  if (p != NULL)
    p->setValue (val);
  else
    addOperatingPoint (n, val);
}

/* The function checks whether the circuit has got a certain operating
   point value.  If so it returns non-zero, otherwise it returns
   zero. */
int circuit::hasOperatingPoint (char * n) {
  return (oper && oper->findOperatingPoint (n)) ? 1 : 0;
}

/* This function copies all properties in the given operating point
   list into a circuit object. */
void circuit::copyOperatingPoints (operatingpoint * org) {
  operatingpoint * p;
  oper = NULL;
  while (org != NULL) {
    p = new operatingpoint (*org);
    p->setNext (oper);
    oper = p;
    org = org->getNext ();
  }
}

// Deletes all operating points of a circuit object.
void circuit::deleteOperatingPoints (void) {
  operatingpoint * n;
  for (operatingpoint * p = oper; p != NULL; p = n) {
    n = p->getNext ();
    delete p;
  }
  oper = NULL;
}

// Returns the S-parameter at the given matrix position.
complex circuit::getS (int x, int y) { 
  return MatrixS[y - 1 + (x - 1) * size];
}

// Sets the S-parameter at the given matrix position.
void circuit::setS (int x, int y, complex z) {
  MatrixS[y - 1 + (x - 1) * size] = z;
}

// Returns the noise-correlation-parameter at the given matrix position.
complex circuit::getN (int r, int c) { 
  return MatrixN[c - 1 + (r - 1) * size];
}

// Sets the noise-correlation-parameter at the given matrix position.
void circuit::setN (int r, int c, complex z) {
  MatrixN[c - 1 + (r - 1) * size] = z;
}

// Returns the number of internal voltage sources for DC analysis.
int circuit::getVoltageSources (void) {
  return vsources;
}

// Sets the number of internal voltage sources for DC analysis.
void circuit::setVoltageSources (int s) {
  assert (s >= 0 && s <= MAX_CIR_VSRCS);
  vsources = s;
}

/* The function returns an internal node or circuit name with the
   given prefix and based on the given circuits name.  The caller is
   responsible to free() the returned string. */
char * circuit::createInternal (char * prefix, char * obj) {
  char * n = (char *) malloc (strlen (prefix) + strlen (obj) + 3);
  sprintf (n, "_%s#%s", prefix, obj);
  return n;
}

/* This function copies the matrix elements inside the given matrix to
   the internal S-parameter matrix of the circuit. */
void circuit::setMatrixS (matrix & s) {
  int r = s.getRows ();
  int c = s.getCols ();
  // copy matrix elements
  if (r > 0 && c > 0 && r * c == size * size) {
    memcpy (MatrixS, s.getData (), sizeof (complex) * r * c);
  }
}

/* The function return a matrix containing the S-parameters of the
   circuit. */
matrix& circuit::getMatrixS (void) {
  matrix * res = new matrix (size);
  memcpy (res->getData (), MatrixS, sizeof (complex) * size * size);
  return *res;
}

/* This function copies the matrix elements inside the given matrix to
   the internal noise correlation matrix of the circuit. */
void circuit::setMatrixN (matrix & n) {
  int r = n.getRows ();
  int c = n.getCols ();
  // copy matrix elements
  if (r > 0 && c > 0 && r * c == size * size) {
    memcpy (MatrixN, n.getData (), sizeof (complex) * r * c);
  }
}

/* The function return a matrix containing the noise correlation
   matrix of the circuit. */
matrix& circuit::getMatrixN (void) {
  matrix * res = new matrix (size);
  memcpy (res->getData (), MatrixN, sizeof (complex) * size * size);
  return *res;
}

/* This function copies the matrix elements inside the given matrix to
   the internal G-MNA matrix of the circuit. */
void circuit::setMatrixY (matrix & y) {
  int r = y.getRows ();
  int c = y.getCols ();
  // copy matrix elements
  if (r > 0 && c > 0 && r * c == size * size) {
    memcpy (MatrixY, y.getData (), sizeof (complex) * r * c);
  }
}

/* The function return a matrix containing the G-MNA matrix of the
   circuit. */
matrix& circuit::getMatrixY (void) {
  matrix * res = new matrix (size);
  memcpy (res->getData (), MatrixY, sizeof (complex) * size * size);
  return *res;
}

// The function cleans up the B-MNA matrix entries.
void circuit::clearB (void) {
  memset (MatrixB, 0, sizeof (complex) * size * vsources);
}

// The function cleans up the C-MNA matrix entries.
void circuit::clearC (void) {
  memset (MatrixC, 0, sizeof (complex) * size * vsources);
}

// The function cleans up the D-MNA matrix entries.
void circuit::clearD (void) {
  memset (MatrixD, 0, sizeof (complex) * vsources * vsources);
}

// The function cleans up the E-MNA matrix entries.
void circuit::clearE (void) {
  memset (MatrixE, 0, sizeof (complex) * vsources);
}

// The function cleans up the I-MNA matrix entries.
void circuit::clearI (void) {
  memset (MatrixI, 0, sizeof (complex) * size);
}

// The function cleans up the V-MNA matrix entries.
void circuit::clearV (void) {
  memset (MatrixV, 0, sizeof (complex) * size);
}

// The function cleans up the G-MNA matrix entries.
void circuit::clearY (void) {
  memset (MatrixY, 0, sizeof (complex) * size * size);
}
