/*
 * sweep.h - variable sweep class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: sweep.h,v 1.1 2004/06/22 19:32:56 ela Exp $
 *
 */

#ifndef __SWEEP_H__
#define __SWEEP_H__

enum sweep_type {
  SWEEP_UNKNOWN = -1, // not yet defined
  SWEEP_CONSTANT,     // constant value
  SWEEP_LINEAR,       // linear
  SWEEP_LOGARITHMIC,  // logarithmic
  SWEEP_LIST          // list of values
};

class object;

class sweep : public object
{
 public:
  sweep ();
  sweep (char *);
  sweep (sweep &);
  ~sweep ();
  int getSize (void) { return size; }
  int getType (void) { return type; }
  nr_double_t get (int);
  nr_double_t next (void);
  nr_double_t prev (void);
  void set (int, nr_double_t);
  void setSize (int);
  char * toString (void);
  void reverse (void);
  void reset (void) { counter = 0; };

 protected:
  int type;

 private:
  nr_double_t * data;
  int size;
  char * txt;
  int counter;
};

class linsweep : public sweep
{
 public:
  linsweep ();
  linsweep (char *);
  ~linsweep ();
  void create (nr_double_t, nr_double_t, int);
};

class logsweep : public sweep
{
 public:
  logsweep ();
  logsweep (char *);
  ~logsweep ();
  void create (nr_double_t, nr_double_t, int);
};

class consweep : public sweep
{
 public:
  consweep ();
  consweep (char *);
  ~consweep ();
  void create (nr_double_t);
};

class lstsweep : public sweep
{
 public:
  lstsweep ();
  lstsweep (char *);
  ~lstsweep ();
  void create (int);
};

#endif /* __SWEEP_H__ */
