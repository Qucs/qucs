/*
 * net.h - net class definitions
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
 * $Id: net.h,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
 *
 */

#ifndef __NET_H__
#define __NET_H__

class circuit;
class node;
class dataset;

class net : public object
{
 public:
  net ();
  net (char *);
  net (net &);
  ~net ();
  circuit * getRoot (void) { return root; }
  void setRoot (circuit * c) { root = c; }
  void insertCircuit (circuit *);
  void insertPort (circuit *);
  void removeCircuit (circuit *);
  void calc (nr_double_t);
  void reduce (void);
  void solve (void);
  void list (void);
  void reducedCircuit (circuit *);
  node * findConnectedNode (node *);
  node * findConnectedCircuitNode (node *);
  void insertConnections (void);
  void insertDifferentialPorts (void);
  void insertTee (node *);
  void insertOpen (node *);
  void insertedCircuit (circuit *);
  circuit * findCircuits (node *, int);
  void insertedNode (node *);
  void getDroppedCircuits (void);
  void deleteUnusedCircuits (void);
  void saveResults (nr_double_t);
  char * createSP (int, int);
  dataset * getData (void);

private:
  circuit * root;
  circuit * drop;
  int nCircuits;
  int nPorts;
  int reduced;
  int inserted;
  int insertedNodes;
  dataset * data;
};

#endif /* __NET_H__ */
