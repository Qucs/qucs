/*
 * hash.h - hash function interface
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
 * $Id: hash.h,v 1.1 2005/12/05 12:09:36 raimi Exp $
 *
 */

#ifndef __HASH_H__
#define __HASH_H__

namespace qucs {

/* Useful defines. */
#define HASH_SHRINK         4
#define HASH_EXPAND         8
#define HASH_MIN_SIZE       4
#define HASH_SHRINK_LIMIT   (buckets >> 2)
#define HASH_EXPAND_LIMIT   ((buckets >> 1) + (buckets >> 2))
#define HASH_LOCATION(code) ((code) & (buckets - 1))


/* This is the basic structure of a hash entry consisting of its key,
   the actual value stored in the hash table and the hash code of the
   key. */
class hashentry
{
 public:
  hashentry () {  // Constructor.
    code = 0; key = NULL; value = NULL;
  }
  ~hashentry () { // Destructor.
    if (key) free (key);
  }

 public:
  int code;      // Hash code.
  char * key;    // Hash key.
  void * value;  // Hash value.
};

/* The hash table consists of different hash buckets.  This contains
   the bucket's size and the entry array.  */
class hashbucket
{
 public:
  hashbucket () {  // Constructor.
    capacity = size = 0;
    entry = NULL;
  }
  ~hashbucket () { // Destructor.
    if (entry) {
      for (int n = 0; n < size; n++) delete entry[n];
      free (entry);
    }
  }

 public:
  // Adds an entry to the bucket.
  void add (hashentry * e) {
    if (size == 0) {
      capacity = HASH_MIN_SIZE;
      entry = (hashentry **) malloc (sizeof (hashentry *) * capacity);
    }
    else if (size >= capacity) {
      capacity *= 2;
      entry = (hashentry **) realloc (entry, sizeof (hashentry *) * capacity);
    }
    entry[size++] = e;
  }
  // Removes an entry from the bucket.
  void del (int idx) {
    delete entry[idx];
    size--;
    if (idx != size) entry[idx] = entry[size];
  }

 public:
  int capacity;       // The capacity of the bucket.
  int size;           // The current size.
  hashentry ** entry; // Hash entry table.
};


/* This structure keeps information of a specific hash table.  */
class hash
{
 public:
  hash ();
  hash (int);
  ~hash ();

  void clear (void);
  void rehash (int);
  void * put (char *, void *);
  void * get (char *);
  void * del (char *);

 public:
  int buckets;
  int fill;
  int keys;
  int (* equals) (char *, char *);
  int (* code) (char *);
  unsigned (* keylen) (char *);
  hashbucket ** table;
};

} /* namespace */

#endif /* __HASH_H__ */
