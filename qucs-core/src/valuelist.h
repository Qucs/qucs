/*
 * valuelist.h - value list template class definitions
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
 * $Id: valuelist.h,v 1.4 2007/09/16 16:49:39 ela Exp $
 *
 */

#ifndef __VALUELIST_H__
#define __VALUELIST_H__

template <class type_t> class valentry;
template <class type_t> class valuelist;
template <class type_t> class valuelistiterator;

/* Value list entry. */
template <class type_t>
class valentry
{
  friend class valuelistiterator<type_t>;
  friend class valuelist<type_t>;

 public:
  ~valentry () { free (key); delete value; }
  char * key;
  type_t * value;
  valentry * next;
  valentry * prev;
};

/* The value list class. */
template <class type_t>
class valuelist
{
  friend class valuelistiterator<type_t>;

 public:
  valuelist ();
  ~valuelist ();
  valuelist (const valuelist &);
  void add (const char *, type_t *);
  void append (char *, type_t *);
  void append (valuelist *);
  void del (char *);
  int  length (void);
  int  contains (char *);
  type_t * get (const char *);
  void clear (void);

 private:
  int size;
  valentry<type_t> * root;
  valentry<type_t> * last;
};

/* Value list iterator. */
template <class type_t>
class valuelistiterator
{
 public:
  valuelistiterator (valuelist<type_t> &);
  ~valuelistiterator ();

  int count (void);
  char * toFirst (void);
  char * toLast (void);
  char * operator++ (void);
  char * operator-- (void);
  char * operator * (void) { return current (); }
  char * current (void);
  char * currentKey (void);
  type_t * currentVal (void);
  char * first (void);
  char * last (void);

 private:
  valuelist<type_t> * _valuelist;
  valentry<type_t> * _first;
  valentry<type_t> * _last;
  valentry<type_t> * _current;
};

#include "valuelist.cpp"

#endif /* __VALUELIST_H__ */
