/*
 * strlist.h - string list class definitions
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: strlist.h,v 1.5 2004/11/29 19:03:39 raimi Exp $
 *
 */

#ifndef __STRLIST_H__
#define __STRLIST_H__

struct strlist_t {
  char * str;
  struct strlist_t * next;
};

class strlist
{
 public:
  strlist ();
  strlist (const strlist &);
  ~strlist ();
  void add (char *);
  void append (char *);
  struct strlist_t * getRoot (void) { return root; }
  int length (void);
  int contains (char *);
  char * get (int);
  int index (char *);
  static strlist * join (strlist *, strlist *);
  void del (strlist *);
  char * toString (char * concat = " ");

 private:
  struct strlist_t * root;
  char * txt;
};

#endif /* __STRLIST_H__ */
