/*
 * input.h - input netlist class definitions
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
 * $Id: input.h,v 1.6 2004-11-24 19:15:46 raimi Exp $
 *
 */

#ifndef __INPUT_H__
#define __INPUT_H__

class net;
class circuit;
class analysis;
class environment;

class input : public object
{
 public:
  input ();
  input (char *);
  ~input ();
  int netlist (net *);
  FILE * getFile (void) { return fd; }
  void setFile (FILE * f) { fd = f; }
  void factory (void);
  circuit * createCircuit (char *);
  analysis * createAnalysis (char *);
  substrate * createSubstrate (char *);
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }
  static void assignDefaultProperties (object *, struct define_t *);

 private:
  FILE * fd;
  net * subnet;
  environment * env;
};

// externalize global variable
extern int netlist_check;

#endif /* __INPUT_H__ */
