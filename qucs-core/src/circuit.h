/*
 * circuit.h - circuit class definitions
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
 * $Id: circuit.h,v 1.1 2003/12/20 19:03:25 ela Exp $
 *
 */

#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

class node;
class property;

class circuit : public object
{
 public:
  circuit ();
  circuit (int);
  circuit (const circuit &);
  ~circuit ();
  virtual void calc (nr_double_t) { };
  void setNode (int, char *);
  node * getNode (int);
  complex getS (int x, int y) { return data[x - 1 + (y - 1) * size]; }
  void setS (int x, int y, complex z) { data[x - 1 + (y - 1) * size] = z; }
  int getSize (void) { return size; }
  void print (void);
  int isPort (void) { return port; }
  void setPort (int p) { port = p; }
  int isOriginal (void) { return org; }
  void setOriginal (int o) { org = o; }
  int countCircuits (void);
  int countPorts (void);
  int countNodes (void);
  circuit * interconnectJoin (node *, node *);
  circuit * connectedJoin (node *, node *);

 protected:
  static const nr_double_t z0 = 50.0;

 private:
  int size;
  int port;
  int org;
  complex * data;
  node * nodes;
};

#endif /* __CIRCUIT_H__ */
