/*
 * spsolver.h - S-parameter solver class definitions
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
 * $Id: spsolver.h,v 1.2 2004-04-13 20:41:17 ela Exp $
 *
 */

#ifndef __SPSOLVER_H__
#define __SPSOLVER_H__

class analysis;
class circuit;
class node;

class spsolver : public analysis
{
 public:
  spsolver ();
  spsolver (char *);
  spsolver (spsolver &);
  ~spsolver ();
  void calc (nr_double_t);
  void reduce (void);
  void solve (void);
  void insertConnections (void);
  void insertDifferentialPorts (void);
  void insertTee (node *);
  void insertOpen (node *);
  circuit * interconnectJoin (node *, node *);
  circuit * connectedJoin (node *, node *);
  void saveResults (nr_double_t);
  char * createSP (int, int);

 private:
  int runs;
};

#endif /* __SPSOLVER_H__ */
