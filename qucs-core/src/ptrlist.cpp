/*
 * ptrlist.cpp - pointer list template class implementation
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: ptrlist.cpp,v 1.1 2005/03/14 21:59:07 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ptrlist.h"

// Constructor creates an unnamed instance of the ptrlist class.
template <class type_t>
ptrlist<type_t>::ptrlist () {
  size = 0;
  root = NULL;
}

/* This copy constructor creates a instance of the ptrlist class based
   on the given ptrlist. */
template <class type_t>
ptrlist<type_t>::ptrlist (const ptrlist<type_t> & p) {
  ptrentry<type_t> * ptr;
  size = 0;
  root = NULL;
  for (ptr = p.root; ptr != NULL; ptr = ptr->next) append (ptr->data);
}

// Destructor deletes a ptrlist object.
template <class type_t>
ptrlist<type_t>::~ptrlist () {
  ptrentry<type_t> * next;
  while (root) {
    next = root->next;
    delete root;
    size--;
    root = next;
  }
}

// Puts a new entry at the beginning of the pointer list.
template <class type_t>
void ptrlist<type_t>::add (type_t * ptr) {
  ptrentry<type_t> * entry = new ptrentry<type_t> ();
  if (root) root->prev = entry;
  entry->data = ptr;
  entry->next = root;
  entry->prev = NULL;
  root = entry;
  size++;
}

// Appends a new entry at the end of the pointer list.
template <class type_t>
void ptrlist<type_t>::append (type_t * ptr) {
  ptrentry<type_t> * entry = new ptrentry<type_t> ();
  entry->data = ptr;
  entry->next = NULL;
  if (root) {
    ptrentry<type_t> * p;
    for (p = root; p->next != NULL; p = p->next);
    p->next = entry;
    entry->prev = p;
  }
  else {
    root = entry;
    entry->prev = NULL;
  }
  size++;
}

// Returns the size of the pointer list.
template <class type_t>
int ptrlist<type_t>::length (void) {
  return size;
}

// Removes any occurrence of the given pointer from the pointer list.
template <class type_t>
void ptrlist<type_t>::del (type_t * ptr) {
  ptrentry<type_t> * next = NULL;
  for (ptrentry<type_t> * p = root; p != NULL; p = next) {
    next = p->next;
    if (p->data == ptr) {
      if (p == root) {
	root = p->next;
	if (root) root->prev = NULL;
      }
      else {
	p->prev->next = p->next;
	if (p->next) p->next->prev = p->prev;
      }
      delete p;
      size--;
    }
  }
}

// Returns the number of occurrences of the given pointer in the list.
template <class type_t>
int ptrlist<type_t>::contains (type_t * ptr) {
  int count = 0;
  for (ptrentry<type_t> * p = root; p != NULL; p = p->next) {
    if (p->data == ptr) count++;
  }
  return count;
}

// Returns the first position of the given pointer in the list.
template <class type_t>
int ptrlist<type_t>::index (type_t * ptr) {
  int idx = -1;
  for (ptrentry<type_t> * p = root; p != NULL; p = p->next, idx++) {
    if (p->data == ptr) break;
  }
  return idx;
}

// Returns the pointer at the given position.
template <class type_t>
type_t * ptrlist<type_t>::get (int idx) {
  ptrentry<type_t> * ptr = root;
  for (int i = 0 ; i < idx && ptr != NULL; ptr = ptr->next, i++);
  return ptr ? ptr->data : NULL;
}
