/*
 * nodelist.cpp - node list class implementation
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
 * $Id: nodelist.cpp,v 1.9 2004/12/20 18:41:17 raimi Exp $
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
#include "object.h"
#include "node.h"
#include "complex.h"
#include "circuit.h"
#include "net.h"
#include "nodelist.h"

// Constructor creates an instance of the nodelist class.
nodelist::nodelist () {
  root = last = NULL;
  narray = NULL;
  txt = NULL;
  sorting = 0;
}

/* The function creates a nodelist for the given netlist.  The
   nodelist is based on the circuit list and consists of unique nodes
   inside the circuit list only.  Each node in the list has references
   to their actual circuit nodes and thereby to the circuits it is
   connected to. */
nodelist::nodelist (net * subnet) {
  root = last = NULL;
  narray = NULL;
  txt = NULL;
  sorting = 0;

  circuit * c;
  // go through circuit list and find unique nodes
  for (c = subnet->getRoot (); c != NULL; c = (circuit *) c->getNext ()) {
    for (int i = 1; i <= c->getSize (); i++) {
      node * n = c->getNode (i);
      if (contains (n->getName ()) == 0) {
	add (n->getName (), n->getInternal ());
      }
    }
  }
  // add circuit nodes to each unique node in the list
  for (struct nodelist_t * n = getRoot (); n != NULL; n = n->next) {
    for (c = subnet->getRoot (); c != NULL; c = (circuit *) c->getNext ()) {
      for (int i = 1; i <= c->getSize (); i++) {
	if (!strcmp (n->name, c->getNode(i)->getName ())) {
	  addCircuitNode (n, c->getNode (i));
	}
      }
    }
  }
}

/* This copy constructor creates a instance of the nodelist class
   based on the given nodelist. */
nodelist::nodelist (const nodelist & o) {
  struct nodelist_t * n;
  root = last = NULL;
  narray = NULL;
  for (n = o.root; n != NULL; n = n->next) append (copy (n));
  txt = o.txt ? strdup (o.txt) : NULL;
  sorting = o.sorting;
}

// Destructor deletes an instance of the nodelist class.
nodelist::~nodelist () {
  struct nodelist_t * next;
  while (root) {
    next = root->next;
    release (root);
    root = next;
  }
  if (txt) free (txt);
  if (narray) free (narray);
}

// The function copies the given node with all its properties.
struct nodelist_t * nodelist::copy (struct nodelist_t * n) {
  struct nodelist_t * copy = create (n->name, n->internal);
  copy->n = n->n;
  copy->nNodes = n->nNodes;
  copy->nAlloc = n->nAlloc;
  if (copy->nAlloc) {
    copy->nodes = (node **) malloc (sizeof (node *) * n->nAlloc);
    if (copy->nNodes) {
      memcpy (copy->nodes, n->nodes, sizeof (node *) * n->nNodes);
    }
  }
  return copy;
}

// This function adds a node name to the list and saves the internal flag.
void nodelist::add (char * str, int intern) {
  add (create (str, intern));
}

// The function creates a node based upon the given arguments.
struct nodelist_t * nodelist::create (char * str, int intern) {
  struct nodelist_t * n;
  n = (struct nodelist_t *) calloc (sizeof (struct nodelist_t), 1);
  n->internal = intern;
  n->name = str ? strdup (str) : NULL;
  return n;
}

// This function adds a node to the list.
void nodelist::add (struct nodelist_t * n) {
  n->next = root;
  if (root == NULL) last = n;
  root = n;
}

// This function appends a node name to the list.
void nodelist::append (char * str, int intern) {
  append (create (str, intern));
}

// This function appends the given node to the list.
void nodelist::append (struct nodelist_t * n) {
  n->next = NULL;
  if (root)
    last->next = n;
  else
    root = n;
  last = n;
}

// This function removes the node with the given name from the list.
void nodelist::remove (char * name) {
  remove (getNode (name));
}

// The function removes the given node from the list.
void nodelist::remove (struct nodelist_t * del, int keep) {
  struct nodelist_t * prev, * n;
  // go through the list
  for (prev = NULL, n = root; n != NULL; prev = n, n = n->next) {
    if (n == del) {
      // adjust the chain properly
      if (prev == NULL)
	root = n->next;
      else
	prev->next = n->next;
      if (n == last) last = prev;
      // delete node if requested and return
      if (!keep) release (n);
      return;
    }
  }
}

// This function free()'s the given node.
void nodelist::release (struct nodelist_t * n) {
  if (n->name) free (n->name);
  if (n->nodes) free (n->nodes);
  free (n);
}

// This function counts the node names in the list.
int nodelist::length (void) {
  int res = 0;
  for (struct nodelist_t * n = root; n != NULL; n = n->next) res++;
  return res;
}

// This function finds the specified node name in the list.
int nodelist::contains (char * str) {
  int res = 0;
  for (struct nodelist_t * n = root; n != NULL; n = n->next) {
    if (n->name != NULL && str != NULL && !strcmp (n->name, str)) 
      res++;
  }
  return res;
}

/* This function returns the node name positioned at the specified
   location in the node name list. */
char * nodelist::get (int nr) {
  return narray[nr]->name;
}

/* This function returns non-zero if the node positioned at the
   specified location in the node name list is marked internal and
   zero otherwise. */
int nodelist::isInternal (int nr) {
  return narray[nr]->internal;
}

/* The function returns the nodelist structure at the specified
   location in the node name list. */
struct nodelist_t * nodelist::getNode (int nr) {
  return narray[nr];
}

/* The function returns the nodelist structure with the given name in
   the node name list.  It returns NULL if there is no such node. */
struct nodelist_t * nodelist::getNode (char * name) {
  for (struct nodelist_t * n = root; n != NULL; n = n->next)
    if (!strcmp (name, n->name)) return n;
  return NULL;
}

/* Returns a comma separated list of the circuits connected to the
   node specified by the given number. */
char * nodelist::getNodeString (int nr) {
  if (txt) free (txt); txt = NULL;
  // find the specified node
  struct nodelist_t * n = getNode (nr);
  // append circuit names connected to the node
  int len = (n->nNodes - 1) + 1;
  txt = (char *) malloc (len); txt[0] = '\0';
  for (int i = 0; i < n->nNodes; i++) {
    char * str = n->nodes[i]->getCircuit()->getName ();
    len += strlen (str);
    txt = (char *) realloc (txt, len);
    strcat (txt, str);
    if (i != n->nNodes - 1) strcat (txt, ",");
  }
  return txt;
}

/* The function returns the nodelist structure at the end of the node
   name list. */
struct nodelist_t * nodelist::getLastNode (void) {
  return last;
}

// This function enumerates the nodes in the node name list.
void nodelist::assignNodes (void) {
  int i = 1;

  // create fast array access possibility
  if (narray) free (narray);
  narray = (struct nodelist_t **)
    malloc (sizeof (struct nodelist_t *) * length ());

  for (struct nodelist_t * n = root; n != NULL; n = n->next) {
    // ground node gets a zero counter
    if (!strcmp (n->name, "gnd")) {
      n->n = 0;
      narray[0] = n;
    }
    // others get a unique number greater than zero
    else {
      narray[i] = n;
      n->n = i++;
    }
  }
}

/* The function appends a node pointer to the given nodelist
   structure. */
void nodelist::addCircuitNode (struct nodelist_t * nl, node * n) {
  if (nl->nNodes >= nl->nAlloc) { // ensure capacity
    if (nl->nAlloc == 0) {
      nl->nAlloc = 2;
      nl->nodes = (node **) malloc (sizeof (node *) * nl->nAlloc);
    }
    else {
      nl->nAlloc *= 2;
      nl->nodes = (node **) realloc (nl->nodes, sizeof (node *) * nl->nAlloc);
    }
  }
  nl->nodes[nl->nNodes++] = n;
  if (n->getInternal ()) nl->internal = n->getInternal ();
}

/* This function deletes the given node from the nodelist
   structure. */
void nodelist::delCircuitNode (struct nodelist_t * nl, node * n) {
  assert (nl->nNodes > 0);
  if (nl->nNodes > 1) {
    int i, j;
    // copy nodelist except the given one
    for (j = i = 0; j < nl->nNodes - 1; i++, j++) {
      if (nl->nodes[i] == n) i++;
      nl->nodes[j] = nl->nodes[i];
    }
  }
  else {
    // no more nodes in the list
    free (nl->nodes);
    nl->nodes = NULL;
    nl->nAlloc = 0;
  }
  nl->nNodes--;
}

/* This function is used as sorting criteria for the S-parameter
   analysis.  It returns the number of nodes a join of the two
   circuits connected to the given node would yield. */
static int sortfunc (struct nodelist_t * n) {
  int p;
  circuit * c1 = n->nodes[0]->getCircuit ();
  circuit * c2 = n->nNodes > 1 ? n->nodes[1]->getCircuit () : NULL;
  if (c1->getPort () || (c2 && c2->getPort ())) return -1;
  if (c1 == c2) { // interconnect
    p = c1->getSize () - 2;
  } else {        // connect
    p = c1->getSize () + (c2 ? c2->getSize () - 2 : 0);
  }
  return p;
}

/* The function evaluates the sorting criteria of the given two nodes.
   It returns non-zero if 'n1' should be inserted before 'n2'. */
static int insfunc (struct nodelist_t * n1, struct nodelist_t * n2) {
  int p1 = sortfunc (n1);
  int p2 = sortfunc (n2);
  return p1 >= 0 && (p1 <= p2 || p2 < 0);
}

/* The function inserts the given node structure into the node list.
   If the nodelist is sorted then the node gets inserted at a certain
   position. */
void nodelist::insert (struct nodelist_t * n) {
  // first node at all
  if (root == NULL) {
    last = root = n;
    return;
  }

  // sorted node list
  if (sorting) {
    int added = 0;
    struct nodelist_t * nl, * prev;
    for (prev = NULL, nl = root; nl != NULL; prev = nl, nl = nl->next) {
      if (insfunc (n, nl)) {
	if (prev == NULL) {
	  n->next = root;
	  root = n;
	} else {
	  n->next = nl;
	  prev->next = n;
	}
	added++;
	break;
      }
    }
    if (!added) append (n);
    return;
  }

  // unsorted node list
  add (n);
}

/* This function removes the nodes associated with the given circuit
   from the node list.  If the node list is sorted then the order gets
   rearranged properly. */
void nodelist::remove (circuit * c) {
  // go through each node of the circuit
  for (int i = 1; i <= c->getSize (); i++) {
    node * n = c->getNode (i);
    struct nodelist_t * nl;
    if ((nl = getNode (n->getName ())) != NULL) {
      // remove node from node structure
      delCircuitNode (nl, n);
      if (nl->nNodes <= 0) {
	// completely remove the node structure
	remove (nl);
      }
      else if (sorting && sortfunc (nl) > 0) {
	// rearrange sorting
	remove (nl, 1);
	insert (nl);
      }
    }
  }
}

/* The following function can be used to insert a new circuit to the
   node list.  It goes through each node of the circuit and rearranges
   the node list appropriately. */
void nodelist::insert (circuit * c) {
  // go through each node of the circuit
  for (int i = 1; i <= c->getSize (); i++) {
    struct nodelist_t * nl;
    node * n = c->getNode (i);
    // is this node already in the nodelist?
    if (contains (n->getName ()) == 0) {
      // no, create new node and put it into the list
      nl = create (n->getName (), n->getInternal ());
      addCircuitNode (nl, n);
      if (sorting) {
	if (c->getPort ())
	  append (nl);
	else
	  insert (nl);
      }
      else add (nl);
    }
    else {
      // yes, put additional node into nodelist structure
      if ((nl = getNode (n->getName ())) != NULL) {
	addCircuitNode (nl, n);
	if (sorting && sortfunc (nl) > 0) {
	  // rearrange sorting
	  remove (nl, 1);
	  insert (nl);
	}
      }
    }
  }
}

/* The function completely rebuilds the nodelist.  Once sort()'ed it
   keeps being sorted when removing or inserting new circuits. */
void nodelist::sort (void) {
  nodelist * nodes = new nodelist ();
  struct nodelist_t * nl, * cand;
  int p, i, ports, MaxPorts, len = length ();

  // go through the list until there is no node left
  for (i = 0; i < len; i++) {
    // find last order node
    cand = NULL;
    for (MaxPorts = -1, p = 0, nl = root; nl != NULL; nl = nl->next) {
      ports = sortfunc (nl);
      if (ports > MaxPorts || MaxPorts < 0 || ports == -1) {
	cand = nl;
	MaxPorts = ports;
      }
      if (ports == -1) break;
    }
    // add last order node
    remove (cand, 1);
    nodes->add (cand);
  }

  // store sorted node list in current object
  root = nodes->getRoot ();
  last = nodes->getLastNode ();
  nodes->root = NULL;
  sorting = 1;

  // delete temporary node list
  delete nodes;
}

// The function returns the first two nodes of the sorted list.
void nodelist::sortedNodes (node ** node1, node ** node2) {
  assert (root->nNodes == 2);
  *node1 = root->nodes[0];
  *node2 = root->nodes[1];
}

#if DEBUG
// Debug function: Prints the entire nodelist.
void nodelist::print (void) {
  for (struct nodelist_t * n = root; n != NULL; n = n->next) {
    logprint (LOG_STATUS, "DEBUG: node %s-%d [", n->name, n->n);
    for (int i = 0; i < n->nNodes; i++) {
      logprint (LOG_STATUS, "%s", n->nodes[i]->getCircuit()->getName ());
      if (i != n->nNodes - 1) logprint (LOG_STATUS, ",");
    }
    logprint (LOG_STATUS, "]\n");
  }
}
#endif /* DEBUG */
