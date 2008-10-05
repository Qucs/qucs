/*
 * analysis.h - analysis class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: analysis.h,v 1.17 2008/10/05 17:52:11 ela Exp $
 *
 */

#ifndef __ANALYSIS_H__
#define __ANALYSIS_H__

#define SAVE_OPS 1 // save operating points
#define SAVE_ALL 2 // also save subcircuit nodes and operating points
#define SAVE_CVS 4 // save characteristic values

#define ACREATOR(val) \
  val (); \
  static analysis * create (void) { return new val (); } \
  static struct define_t anadef; \
  static struct define_t * definition (void) { return &anadef; }

class dataset;
class net;
class object;
class environment;
class sweep;
template <class type_t> class ptrlist;

enum analysis_type {
  ANALYSIS_UNKNOWN = -1,
  ANALYSIS_SWEEP,
  ANALYSIS_DC,
  ANALYSIS_AC,
  ANALYSIS_HBALANCE,
  ANALYSIS_TRANSIENT,
  ANALYSIS_SPARAMETER
};

class analysis : public object
{
 public:
  analysis ();
  analysis (char *);
  analysis (analysis &);
  ~analysis ();
  virtual int solve (void) { return 0; }
  virtual int initialize (void) { return 0; }
  virtual int cleanup (void) { return 0; }
  dataset * getData (void) { return data; }
  void setData (dataset * d) { data = d; }
  net * getNet (void) { return subnet; }
  void setNet (net * netlist) { subnet = netlist; }
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }
  ptrlist<analysis> * getAnalysis (void) { return actions; }
  void setAnalysis (ptrlist<analysis> * a) { actions = a; }
  void addAnalysis (analysis *);
  void delAnalysis (analysis *);
  int  getType (void) { return type; }
  void setType (int t) { type = t; }
  sweep * createSweep (const char *);
  void saveVariable (const char *, nr_complex_t, vector *);
  bool getProgress (void) { return progress; }
  void setProgress (bool p) { progress = p; }

 protected:
  int runs;
  int type;
  net * subnet;
  dataset * data;
  environment * env;
  ptrlist<analysis> * actions;
  bool progress;
};

#endif /* __ANALYSIS_H__ */
