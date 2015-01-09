/*
 * nodelist.h - node list class definitions
 *
 * Copyright (C) 2003, 2004, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#ifndef __NODELIST_H__
#define __NODELIST_H__

#include <vector>

namespace qucs {

class node;
class net;

struct nodelist_t {
public:
nodelist_t() : n(0), name(), nodes(nullptr), nNodes(0), nAlloc(0), internal(0), next(nullptr) {} ;
  int n;
  std::string name;
  node ** nodes;
  int nNodes;
  int nAlloc;
  int internal;
  nodelist_t * next;
};

class nodelist
{
 public:
  nodelist ();
  nodelist (net *);
  nodelist (const nodelist &);
  ~nodelist ();
  void add (const std::string &, int intern = 0);
  void append (const std::string &, int intern = 0);
  struct nodelist_t * getRoot (void) { return root; }
  int length (void);
  int contains (const std::string &);
  int getNodeNr (const std::string &);
  std::string get (int);
  int isInternal (int);
  void addCircuitNode (struct nodelist_t *, node *);
  void assignNodes (void);
  void print (void);
  struct nodelist_t * getNode (int);
  std::string getNodeString (int);
  void sort (void);
  struct nodelist_t * copy (struct nodelist_t *);
  void add (struct nodelist_t *);
  void append (struct nodelist_t *);
  void release (struct nodelist_t *);
  void remove (const std::string &);
  void remove (struct nodelist_t *, int keep = 0);
  void remove (circuit *);
  struct nodelist_t * create (const std::string&, int);
  void insert (struct nodelist_t *);
  void insert (circuit *);
  void delCircuitNode (struct nodelist_t *, node *);
  void sortedNodes (node **, node **);
  struct nodelist_t * getNode (const std::string &);

 private:
  std::vector<nodelist_t *> narray;
  struct nodelist_t * root;
  struct nodelist_t * last;
  int sorting;
};

} // namespace qucs

#endif /* __NODELIST_H__ */
