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
 * $Id: circuit.h,v 1.3 2003-12-26 14:04:07 ela Exp $
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
  virtual void calcS (nr_double_t) { };
  virtual void calcG (void) { };
  void setNode (int, char *);
  node * getNode (int);
  complex getS (int x, int y) { return data[x - 1 + (y - 1) * size]; }
  void setS (int x, int y, complex z) { data[x - 1 + (y - 1) * size] = z; }
  complex getG () { return G; }
  void setG (complex g) { G = g; }
  int getSize (void) { return size; }
  void print (void);
  int isPort (void) { return port; }
  void setPort (int p) { port = p; }
  int isVoltageSource (void) { return source; }
  void setVoltageSource (int s) { source = s; }
  int isOriginal (void) { return org; }
  void setOriginal (int o) { org = o; }
  int countCircuits (void);
  int countPorts (void);
  int countNodes (void);
  void setType (int t) { type = t; }
  int getType (void) { return type; }

 protected:
  static const nr_double_t z0 = 50.0;
  int type;

 private:
  int size;
  int port;
  int source;
  int org;
  complex * data;
  complex G;
  node * nodes;
};

#endif /* __CIRCUIT_H__ */
