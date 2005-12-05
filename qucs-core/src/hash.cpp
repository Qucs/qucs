/*
 * hash.cpp - hash table functions
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: hash.cpp,v 1.1 2005/12/05 12:09:36 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "hash.h"

using namespace qucs;

/* Calculate the hash code for a given string. This is the standard
   callback for any newly created hash table.  */
static int hash_code (char * key) {
  int code = 0;
  char * p = key;
  while (*p) { code = (code << 1) ^ *p; p++; }
  return code;
}

/* This is the default callback for any newly created hash for
   determining two keys being equal.  Return zero if both strings are
   equal, otherwise non-zero.  */
static int hash_key_equals (char * key1, char * key2) {
  char * p1, * p2;
  if (key1 == key2) return 0;
  p1 = key1;
  p2 = key2;
  while (*p1 && *p2) {
    if (*p1 != *p2) return -1;
    p1++; p2++;
  }
  if (!*p1 && !*p2) return 0;
  return -1;
}

/* This is the default routine for determining the actual hash table
   key length of the given key.  */
static unsigned hash_key_length (char * key) {
  unsigned len = 0;
  while (*key++) len++;
  len++;
  return len;
}

// Constructor for the hash table.
hash::hash (int size) {
  // set initial hash table size to a binary value
  int n;
  for (n = size, buckets = 1; n != 1; n >>= 1) 
    buckets <<= 1;
  if (buckets < HASH_MIN_SIZE)
    buckets = HASH_MIN_SIZE;

  // initialize default values
  fill = 0;
  keys = 0;
  code = hash_code;
  equals = hash_key_equals;
  keylen = hash_key_length;

  // allocate space for the hash table buckets
  table = (hashbucket **) calloc (buckets, sizeof (hashbucket *));
}

// Destructor for the hash table.
hash::~hash () {
  for (int n = 0; n < buckets; n++) {
    if (table[n]) delete table[n];
  }
  free (table);
}

/* Clears the hash table.  Afterwards it does not contain any key.
   The hash table is also shrunken to a minimal size.  */
void hash::clear (void) {
  for (int n = 0; n < buckets; n++) {
    if (table[n]) delete table[n];
  }
  free (table);

  // reinitialize the hash table
  buckets = HASH_MIN_SIZE;
  fill = 0;
  keys = 0;
  table = (hashbucket **) calloc (buckets, sizeof (hashbucket *));
}

/* Rebuilds a hash table.  Double (type is HASH_EXPAND) its size and
   expand the hash codes or half (type is HASH_SHRINK) its size and
   shrink the hash codes if these would be placed somewhere else.  */
void hash::rehash (int type) {
  int n, e;
  hashbucket * bucket, * next;

  // Expand!
  if (type == HASH_EXPAND) {
    // Reallocate and initialize the hash table itself.
    buckets *= 2;
    table = (hashbucket **) realloc (table, sizeof (hashbucket *) * buckets);
    for (n = buckets / 2; n < buckets; n++) { table[n] = NULL; }

    /* Go through all hash table entries and check if it is necessary
       to relocate them.  */
    for (n = 0; n < buckets / 2; n++){
      bucket = table[n];
      for (e = 0; bucket && e < bucket->size; e++) {
	int loc = HASH_LOCATION (bucket->entry[e]->code);
	if (n != loc) {
	  /* copy this entry to the far entry */
	  if ((next = table[loc]) == NULL) {
	    next = new hashbucket ();
	    table[loc] = next;
	  }
	  next->add (bucket->entry[e]);
	  if (next->size == 1) fill++;
	  /* delete this entry */
	  bucket->del (e);
	  if (bucket->size == 0) fill--;
	  e--;
	}
      }
    }
  }
  // Shrink!
  else if (type == HASH_SHRINK && buckets > HASH_MIN_SIZE) {
    buckets /= 2;
    for (n = buckets; n < buckets * 2; n++) {
      bucket = table[n];
      if (bucket && bucket->size) {
	for (e = 0; e < bucket->size; e++) {
	  int loc = HASH_LOCATION (bucket->entry[e]->code);
	  if ((next = table[loc]) == NULL) {
	    next = new hashbucket ();
	  }
	  next->add (bucket->entry[e]);
	  if (next->size == 1) fill++;
	}
	delete bucket;
      }
      fill--;
    }
    table = (hashbucket **) realloc (table, sizeof (hashbucket *) * buckets);
  }
}

/* This function adds a new element consisting of key and value to an
   existing hash.  If the hash is 75% filled it gets rehashed (size
   will be doubled).  When the key already exists then the value just
   gets replaced dropping and returning the old value.  */
void * hash::put (char * key, void * value) {
  int code = this->code (key);
  hashbucket * bucket = table[HASH_LOCATION (code)];

  /* Check if the key is already stored. If so replace the value. */
  if (bucket) {
    for (int e = 0; e < bucket->size; e++) {
      if (bucket->entry[e]->code == code) {
	if (equals (bucket->entry[e]->key, key) == 0) {
	  void * old = bucket->entry[e]->value;
	  bucket->entry[e]->value = value;
	  return old;
	}
      }
    }
  }
  else {
    bucket = new hashbucket ();
    table[HASH_LOCATION (code)] = bucket;
  }

  /* Fill this entry. */
  hashentry * entry = new hashentry ();
  entry->key = (char *) malloc (keylen (key));
  memcpy (entry->key, key, keylen (key));
  entry->value = value;
  entry->code = code;

  bucket->add (entry);
  keys++;

  /* 75% filled ? */
  if (bucket->size == 1) {
    fill++;
    if (fill > HASH_EXPAND_LIMIT) {
      rehash (HASH_EXPAND);
    }
  }
  return NULL;
}

/* Delete an existing hash entry accessed via a given key form the
   hash table.  Return NULL if the key has not been found within the
   hash, otherwise the previous value.  */
void * hash::del (char * key) {
  void * value;
  int code = this->code (key);
  hashbucket * bucket = table[HASH_LOCATION (code)];
  if (bucket) {
    for (int n = 0; n < bucket->size; n++) {
      if (bucket->entry[n]->code == code) {
	if (equals (bucket->entry[n]->key, key) == 0) {
	  value = bucket->entry[n]->value;
	  bucket->del (n);
	  if (bucket->size == 0) {
	    fill--;
	    if (fill < HASH_SHRINK_LIMIT) {
	      rehash (HASH_SHRINK);
	    }
	  }
	  keys--;
	  return value;
	}
      }
    }
  }
  return NULL;
}

/* Hash table lookup.  Find a value for a given key in the hash table.
   Return NULL if the key has not been found within the hash table.  */
void * hash::get (char * key) {
  int code = this->code (key);
  hashbucket * bucket = table[HASH_LOCATION (code)];
  if (bucket) {
    for (int n = 0; n < bucket->size; n++) {
      if (bucket->entry[n]->code == code)
	if (equals (bucket->entry[n]->key, key) == 0)
	  return bucket->entry[n]->value;
    }
  }
  return NULL;
}
