/*
 * ptrlist.h - pointer list template class definitions
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
 * $Id: ptrlist.h,v 1.1 2005-03-14 21:59:07 raimi Exp $
 *
 */

#ifndef __PTRLIST_H__
#define __PTRLIST_H__

template <class type_t>
class ptrentry
{
 public:
  type_t * data;
  ptrentry * next;
  ptrentry * prev;
};

template <class type_t>
class ptrlist
{
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

#include "ptrlist.cpp"

#endif /* __PTRLIST_H__ */
