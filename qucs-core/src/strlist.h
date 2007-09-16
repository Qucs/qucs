/*
 * strlist.h - string list class definitions
 *
 * Copyright (C) 2003, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: strlist.h,v 1.11 2007/09/16 16:49:39 ela Exp $
 *
 */

#ifndef __STRLIST_H__
#define __STRLIST_H__

/* String list entry. */
struct strlist_t {
  char * str;
  struct strlist_t * next;
};

/* String list class. */
class strlist
{
  friend class strlistiterator;

 public:
  strlist ();
  strlist (const strlist &);
  ~strlist ();
  void add (char *);
  void add (strlist *);
  void append (char *);
  void append (strlist *);
  int length (void);
  int contains (char *);
  char * get (int);
  char * first (void);
  char * last (void);
  int index (char *);
  static strlist * join (strlist *, strlist *);
  void del (strlist *);
  char * toString (const char * concat = " ");

 private:
  struct strlist_t * root;
  char * txt;
};

/* String list iterator. */
class strlistiterator
{
 public:
  strlistiterator ();
  strlistiterator (strlist &);
  strlistiterator (strlist *);
  ~strlistiterator ();

  int count (void);
  char * toFirst (void);
  char * toLast (void);
  char * operator++ (void);
  char * operator * (void) { return current (); }
  char * current (void);
  char * first (void);
  char * last (void);

 private:
  strlist * _strlist;
  struct strlist_t * _first;
  struct strlist_t * _last;
  struct strlist_t * _current;
};

#endif /* __STRLIST_H__ */
