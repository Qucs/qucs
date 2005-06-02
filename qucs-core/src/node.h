/*
 * node.h - node class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: node.h,v 1.4 2005/06/02 18:17:51 raimi Exp $
 *
 */

#ifndef __NODE_H__
#define __NODE_H__

class circuit;

class node : public object
{
 public:
  node ();
  node (char *);
  node (const node &);
  ~node ();
  void setNode (int);
  int  getNode (void);
  void setPort (int);
  int  getPort (void);
  void setCircuit (circuit *);
  circuit * getCircuit (void);
  void setInternal (int i) { internal = i; }
  int  getInternal (void) { return internal; }

 private:
  int nNode;
  int port;
  int internal;
  circuit * _circuit;
};

#endif /* __NODE_H__ */
