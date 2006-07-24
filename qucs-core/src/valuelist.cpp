/*
 * valuelist.cpp - value list template class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: valuelist.cpp,v 1.2 2006-07-24 08:07:42 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "valuelist.h"

// Constructor creates an instance of the valuelist class.
template <class type_t>
valuelist<type_t>::valuelist () {
  size = 0;
  last = root = NULL;
}

/* This copy constructor creates a instance of the valuelist class based
   on the given valuelist. */
template <class type_t>
valuelist<type_t>::valuelist (const valuelist<type_t> & p) {
  valentry<type_t> * ptr;
  size = 0;
  last = root = NULL;
  for (ptr = p.root; ptr != NULL; ptr = ptr->next)
    append (ptr->key, new type_t (*(ptr->value)));
}

// Destructor deletes a valuelist object.
template <class type_t>
valuelist<type_t>::~valuelist () {
  clear ();
}

// Resets the value list.
template <class type_t>
void valuelist<type_t>::clear (void) {
  valentry<type_t> * next;
  while (root) {
    next = root->next;
    delete root;
    root = next;
  }
  size = 0;
  last = NULL;
}

// Puts a new entry at the beginning of the value list.
template <class type_t>
void valuelist<type_t>::add (char * n, type_t * ptr) {
  valentry<type_t> * entry = new valentry<type_t> ();
  if (root) root->prev = entry;
  entry->key = strdup (n);
  entry->value = ptr;
  entry->next = root;
  entry->prev = NULL;
  root = entry;
  if (!last) last = root;
  size++;
}

// Appends a new entry at the end of the value list.
template <class type_t>
void valuelist<type_t>::append (char * n, type_t * ptr) {
  valentry<type_t> * entry = new valentry<type_t> ();
  entry->key = strdup (n);
  entry->value = ptr;
  entry->next = NULL;
  if (root) {
    valentry<type_t> * p;
    for (p = root; p->next != NULL; p = p->next);
    p->next = entry;
    entry->prev = p;
  }
  else {
    root = entry;
    entry->prev = NULL;
  }
  last = entry;
  size++;
}

// Returns the size of the value list.
template <class type_t>
int valuelist<type_t>::length (void) {
  return size;
}

// Removes any occurrence of the given key from the value list.
template <class type_t>
void valuelist<type_t>::del (char * n) {
  valentry<type_t> * next = NULL;
  for (valentry<type_t> * p = root; p != NULL; p = next) {
    next = p->next;
    if (!strcmp (p->key, n)) {
      if (p == root) {
	root = p->next;
	if (root) root->prev = NULL;
      }
      else {
	p->prev->next = p->next;
	if (p->next) p->next->prev = p->prev;
      }
      if (p == last) last = p->prev;
      delete p;
      size--;
    }
  }
}

// Returns the number of occurrences of the given key in the list.
template <class type_t>
int valuelist<type_t>::contains (char * n) {
  int count = 0;
  for (valentry<type_t> * p = root; p != NULL; p = p->next) {
    if (!strcmp (p->key, n)) count++;
  }
  return count;
}

// Returns the value associated with the given key.
template <class type_t>
type_t * valuelist<type_t>::get (char * n) {
  for (valentry<type_t> * p = root; p != NULL; p = p->next) {
    if (!strcmp (p->key, n)) return p->value;
  }
  return NULL;
}

// Constructor for value list iterator.
template <class type_t>
valuelistiterator<type_t>::valuelistiterator (valuelist<type_t> & v) {
  _valuelist = &v;
  toLast ();
  toFirst ();
}

// Destructor for value list iterator.
template <class type_t>
valuelistiterator<type_t>::~valuelistiterator () {
}

// Returns number of items this iterator operates on.
template <class type_t>
int valuelistiterator<type_t>::count (void) {
  return _valuelist->size;
}

// Sets the current to the first item in the iterator list.
template <class type_t>
char * valuelistiterator<type_t>::toFirst (void) {
  _current = _first = _valuelist->root;
  return _current ? _current->key : NULL;
}

// Sets the current to the last item in the iterator list.
template <class type_t>
char * valuelistiterator<type_t>::toLast (void) {
  _current = _last = _valuelist->last;
  return _current ? _current->key : NULL;
}

// Makes the succeeding item current and returns the new current item.
template <class type_t>
char * valuelistiterator<type_t>::operator++ (void) {
  _current = _current->next;
  return _current ? _current->key : NULL;
}

// Makes the preceding item current and returns the new current item.
template <class type_t>
char * valuelistiterator<type_t>::operator-- (void) {
  _current = _current->prev;
  return _current ? _current->key : NULL;
}

// Returns the current iterator item.
template <class type_t>
char * valuelistiterator<type_t>::current (void) {
  return _current ? _current->key : NULL;
}

// Returns the current iterator items key.
template <class type_t>
char * valuelistiterator<type_t>::currentKey (void) {
  return current ();
}

// Returns the current iterator items value.
template <class type_t>
type_t * valuelistiterator<type_t>::currentVal (void) {
  return _current ? _current->value : NULL;
}

// Returns the first iterator item.
template <class type_t>
char * valuelistiterator<type_t>::first (void) {
  return _first ? _first->key : NULL;
}

// Returns the last iterator item.
template <class type_t>
char * valuelistiterator<type_t>::last (void) {
  return _last ? _last->key : NULL;
}
