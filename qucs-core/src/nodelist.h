/*
 * nodelist.h - node list class definitions
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
 * $Id: nodelist.h,v 1.1 2003-12-26 14:04:07 ela Exp $
 *
 */

#ifndef __NODELIST_H__
#define __NODELIST_H__

class node;

struct nodelist_t {
  int n;
  char * name;
  node ** nodes;
  int nNodes;
  struct nodelist_t * next;
};

class nodelist
{
 public:
  nodelist ();
  nodelist (const nodelist &);
  ~nodelist ();
  void add (char *);
  void append (char *);
  struct nodelist_t * getRoot (void) { return root; }
  int length (void);
  int contains (char *);
  char * get (int);
  void addCircuitNode (struct nodelist_t *, node *);
  void assignNodes (void);
  void print (void);
  struct nodelist_t * getNode (int);
  struct nodelist_t * getLastNode (void);

 private:
  struct nodelist_t * root;
};

#endif /* __NODELIST_H__ */
