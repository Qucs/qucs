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
 * $Id: nodelist.cpp,v 1.2 2004/06/04 16:01:47 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "object.h"
#include "node.h"
#include "complex.h"
#include "circuit.h"
#include "nodelist.h"

// Constructor creates an instance of the nodelist class.
nodelist::nodelist () {
  root = NULL;
}

/* This copy constructor creates a instance of the nodelist class
   based on the given nodelist. */
nodelist::nodelist (const nodelist & o) {
  struct nodelist_t * n, * last;
  root = NULL;
  for (n = o.root; n != NULL; n = n->next) {
    append (n->name);
    last = getLastNode ();
    last->n = n->n;
    last->nNodes = n->nNodes;
    last->nodes = (node **) malloc (sizeof (node *) * last->nNodes);
    memcpy (last->nodes, n->nodes, sizeof (node *) * last->nNodes);
  }
}

// Destructor deletes an instance of the nodelist class.
nodelist::~nodelist () {
  struct nodelist_t * next;
  while (root) {
    next = root->next;
    if (root->name) free (root->name);
    if (root->nodes) free (root->nodes);
    free (root);
    root = next;
  }
}

// This function adds a node name to the list and saves the internal flag.
void nodelist::add (char * str, int intern) {
  struct nodelist_t * n;
  n = (struct nodelist_t *) calloc (sizeof (struct nodelist_t), 1);
  n->next = root;
  n->internal = intern;
  n->name = str ? strdup (str) : NULL;
  root = n;
}

// This function append a node name to the list.
void nodelist::append (char * str, int intern) {
  struct nodelist_t * n;
  n = (struct nodelist_t *) calloc (sizeof (struct nodelist_t), 1);
  n->next = NULL;
  n->internal = intern;
  n->name = str ? strdup (str) : NULL;
  if (root) {
    struct nodelist_t * e;
    for (e = root; e->next != NULL; e = e->next);
    e->next = n;
  }
  else {
    root = n;
  }
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
  for (struct nodelist_t * n = root; n != NULL; n = n->next)
    if (n->n == nr) return n->name;
  return NULL;
}

/* This function returns non-zero if the node positioned at the
   specified location in the node name list is marked internal and
   zero otherwise. */
int nodelist::isInternal (int nr) {
  for (struct nodelist_t * n = root; n != NULL; n = n->next)
    if (n->n == nr) return n->internal;
  return 0;
}

/* The function returns the nodelist structure at the specified
   location in the node name list. */
struct nodelist_t * nodelist::getNode (int nr) {
  for (struct nodelist_t * n = root; n != NULL; n = n->next)
    if (n->n == nr) return n;
  return NULL;
}

/* The function returns the nodelist structure at the end of the node
   name list. */
struct nodelist_t * nodelist::getLastNode (void) {
  for (struct nodelist_t * n = root; n != NULL; n = n->next)
    if (n->next == NULL) return n;
  return NULL;
}

// This function enumerates the nodes in the node name list.
void nodelist::assignNodes (void) {
  int i = 1;
  for (struct nodelist_t * n = root; n != NULL; n = n->next) {
    // ground node gets a zero counter
    if (!strcmp (n->name, "gnd"))
      n->n = 0;
    // others get a unique number greater than zero
    else
      n->n = i++;
  }
}

/* The function appends a node pointer to the given nodelist
   structure. */
void nodelist::addCircuitNode (struct nodelist_t * nl, node * n) {
  if (nl->nNodes == 0) {
    nl->nodes = (node **) malloc (sizeof (node *));
  }
  else {
    nl->nodes = (node **) 
      realloc (nl->nodes, sizeof (node *) * (1 + nl->nNodes));
  }
  nl->nodes[nl->nNodes++] = n;
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
