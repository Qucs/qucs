/*
 * ptrlist.h - pointer list template class definitions
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: ptrlist.h,v 1.4 2006/03/29 08:02:03 raimi Exp $
 *
 */

#ifndef __PTRLIST_H__
#define __PTRLIST_H__

template <class type_t> class ptrentry;
template <class type_t> class ptrlist;
template <class type_t> class ptrlistiterator;

/* Pointer entry class. */
template <class type_t>
class ptrentry
{
  friend class ptrlist<type_t>;
  friend class ptrlistiterator<type_t>;

 public:
  type_t * data;

 private:
  ptrentry * next;
  ptrentry * prev;
};

/* Pointer list class. */
template <class type_t>
class ptrlist
{
  friend class ptrlistiterator<type_t>;

 public:
  ptrlist ();
  ~ptrlist ();
  ptrlist (const ptrlist &);
  void add (type_t *);
  void append (type_t *);
  void del (type_t *);
  int  length (void);
  int  contains (type_t *);
  int  index (type_t *);
  type_t * get (int);

 private:
  int size;
  ptrentry<type_t> * root;
};

/* Pointer list iterator. */
template <class type_t>
class ptrlistiterator
{
 public:
  ptrlistiterator ();
  ptrlistiterator (ptrlist<type_t> &);
  ~ptrlistiterator ();

  int count (void);
  type_t * toFirst (void);
  type_t * toLast (void);
  type_t * operator++ (void);
  type_t * operator-- (void);
  type_t * operator * (void) { return current (); }
  type_t * current (void);
  type_t * first (void);
  type_t * last (void);

 private:
  ptrlist<type_t> * _ptrlist;
  ptrentry<type_t> * _first;
  ptrentry<type_t> * _last;
  ptrentry<type_t> * _current;
};

#include "ptrlist.cpp"

#endif /* __PTRLIST_H__ */
