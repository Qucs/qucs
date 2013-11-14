/*
 * circuit.h - circuit class definitions
 *
 * Copyright (C) 2003-2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

enum circuit_flag {
  CIRCUIT_ENABLED     = 1,
  CIRCUIT_LINEAR      = 2,
  CIRCUIT_ORIGINAL    = 4,
  CIRCUIT_VSOURCE     = 8,
  CIRCUIT_ISOURCE     = 16,
  CIRCUIT_INTVSOURCE  = 32,
  CIRCUIT_VARSIZE     = 64,
  CIRCUIT_PROBE       = 128,
  CIRCUIT_HISTORY     = 256,
};

#define NODE_1 0
#define NODE_2 1
#define NODE_3 2
#define NODE_4 3
#define NODE_5 4
#define NODE_6 5
#define VSRC_1 0
#define VSRC_2 1
#define VSRC_3 2
#define VSRC_4 3
#define VSRC_5 4

#define MODFLAG(val,bit) if (val) flag |= (bit); else flag &= ~(bit);
#define RETFLAG(bit)     ((flag & (bit)) != 0)

#define CREATOR(val) \
  val (); \
  static circuit * create (void) { return new val (); } \
  static struct define_t cirdef; \
  static struct define_t * definition (void) { return &cirdef; }

class node;
class property;
class substrate;
class operatingpoint;
class characteristic;
class matrix;
class net;
class environment;
class history;

#include "integrator.h"
#include "valuelist.h"

class circuit : public object, public integrator
{
 public:
  // constructor and destructor set
  circuit ();
  circuit (int);
  circuit (const circuit &);
  ~circuit ();

  // functionality to be overloaded by real circuit implementations
  virtual void initSP (void) { allocMatrixS (); }
  virtual void calcSP (nr_double_t) { }
  virtual void initDC (void) { allocMatrixMNA (); }
  virtual void calcDC (void) { }
  virtual void restartDC (void) { }
  virtual void initNoiseSP (void) { allocMatrixN (); }
  virtual void calcNoiseSP (nr_double_t) { }
  virtual void initNoiseAC (void) { allocMatrixN (vsources); }
  virtual void calcNoiseAC (nr_double_t) { }
  virtual void initAC (void) { allocMatrixMNA (); }
  virtual void calcAC (nr_double_t) { }
  virtual void initTR (void) { allocMatrixMNA (); }
  virtual void calcTR (nr_double_t) { }
  virtual void initHB (void) { allocMatrixMNA (); }
  virtual void calcHB (nr_double_t) { }
  virtual void initHB (int) { allocMatrixMNA (); }
  virtual void calcHB (int) { }
  virtual void calcOperatingPoints (void) { }
  virtual void saveOperatingPoints (void) { }
  virtual void calcCharacteristics (nr_double_t) { }
  virtual void saveCharacteristics (nr_double_t) { }

  // real basics
  void   setNode (int, const char *, int intern = 0);
  node * getNode (int);
  void   setType (int t) { type = t; }
  int    getType (void) { return type; }
  int    getSize (void) { return size; }
  void   setSize (int);
  bool   isEnabled (void) { return RETFLAG (CIRCUIT_ENABLED); }
  void   setEnabled (bool e) { MODFLAG (e, CIRCUIT_ENABLED); }
  bool   isVariableSized (void) { return RETFLAG (CIRCUIT_VARSIZE); }
  void   setVariableSized (bool v) { MODFLAG (v, CIRCUIT_VARSIZE); }
  bool   isProbe (void) { return RETFLAG (CIRCUIT_PROBE); }
  void   setProbe (bool p) { MODFLAG (p, CIRCUIT_PROBE); }
  void   setNet (net * n) { subnet = n; }
  net *  getNet (void) { return subnet; }

  // subcircuitry
  char * getSubcircuit (void) { return subcircuit; }
  void   setSubcircuit (char *);

  // environment specific
  environment * getEnv (void) { return env; }
  void setEnv (environment * e) { env = e; }

  // nodal analyses helpers
  void setInternalVoltageSource (bool i) { MODFLAG (i, CIRCUIT_INTVSOURCE); }
  bool isInternalVoltageSource (void) { return RETFLAG (CIRCUIT_INTVSOURCE); }
  void setVoltageSource (int s) { vsource = s; }
  int  getVoltageSource (void) { return vsource; }
  int  getVoltageSources (void);
  void setVoltageSources (int);
  void voltageSource (int, int, int, nr_double_t value = 0.0);
  bool isVSource (void) { return RETFLAG (CIRCUIT_VSOURCE); }
  void setVSource (bool v) { MODFLAG (v, CIRCUIT_VSOURCE); }
  bool isISource (void) { return RETFLAG (CIRCUIT_ISOURCE); }
  void setISource (bool i) { MODFLAG (i, CIRCUIT_ISOURCE); }
  int  getNoiseSources (void);
  void setNoiseSources (int);

  // transient analyses helpers
  void transientCapacitance (int, int, int, nr_double_t, nr_double_t,
			     nr_double_t);
  void transientCapacitance (int, int, nr_double_t, nr_double_t, nr_double_t);
  void transientCapacitanceQ (int, int, int, nr_double_t);
  void transientCapacitanceQ (int, int, nr_double_t);
  void transientCapacitanceC (int, int, int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC (int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC2V (int, int, int, nr_double_t, nr_double_t);
  void transientCapacitanceC2Q (int, int, int, nr_double_t, nr_double_t);
  void setDelta (nr_double_t * d) { deltas = d; }
  nr_double_t * getDelta (void) { return deltas; }

  // history specific functionality
  bool hasHistory (void) { return RETFLAG (CIRCUIT_HISTORY); }
  void setHistory (bool h) { MODFLAG (h, CIRCUIT_HISTORY); }
  void initHistory (nr_double_t);
  void deleteHistory (void);
  void appendHistory (int, nr_double_t);
  void applyHistory (history *);
  nr_double_t getV (int, nr_double_t);
  nr_double_t getJ (int, nr_double_t);
  nr_double_t getHistoryAge (void);

  // s-parameter helpers
  int  getPort (void) { return pacport; }
  void setPort (int p) { pacport = p; }
  int  getInserted (void) { return inserted; }
  void setInserted (int i) { inserted = i; }
  bool isOriginal (void) { return RETFLAG (CIRCUIT_ORIGINAL); }
  void setOriginal (bool o) { MODFLAG (o, CIRCUIT_ORIGINAL); }

  // microstrip helpers
  substrate * getSubstrate (void);
  void setSubstrate (substrate *);

  // matrix entry modificators
  nr_complex_t getS (int, int);
  nr_complex_t getN (int, int);
  nr_complex_t getY (int, int);
  nr_complex_t getB (int, int);
  nr_complex_t getC (int, int);
  nr_complex_t getD (int, int);
  nr_complex_t getQV (int, int);
  nr_complex_t getGV (int);
  nr_complex_t getCV (int);
  nr_complex_t getE (int);
  nr_complex_t getI (int);
  nr_complex_t getJ (int);
  nr_complex_t getV (int);
  nr_complex_t getQ (int);
  nr_double_t getG (int, int);
  void setS (int, int, nr_complex_t);
  void setN (int, int, nr_complex_t);
  void setY (int, int, nr_complex_t);
  void setB (int, int, nr_complex_t);
  void setC (int, int, nr_complex_t);
  void setD (int, int, nr_complex_t);
  void setQV (int, int, nr_complex_t);
  void setGV (int, nr_complex_t);
  void setCV (int, nr_complex_t);
  void setE (int, nr_complex_t);
  void setI (int, nr_complex_t);
  void setJ (int, nr_complex_t);
  void setV (int, nr_complex_t);
  void setQ (int, nr_complex_t);
  void setG (int, int, nr_double_t);
  void clearB (void);
  void clearC (void);
  void clearD (void);
  void clearE (void);
  void clearI (void);
  void clearJ (void);
  void clearV (void);
  void clearY (void);
  void addY (int, int, nr_complex_t);
  void addY (int, int, nr_double_t);
  void addI (int, nr_complex_t);
  void addI (int, nr_double_t);

  // operating point functionality
  void        addOperatingPoint (const char *, nr_double_t);
  nr_double_t getOperatingPoint (const char *);
  void        setOperatingPoint (const char *, nr_double_t);
  int         hasOperatingPoint (char *);
  valuelist<operatingpoint> & getOperatingPoints (void) { return oper; }

  // characteristics functionality
  void        addCharacteristic (const char *, nr_double_t);
  nr_double_t getCharacteristic (char *);
  void        setCharacteristic (const char *, nr_double_t);
  int         hasCharacteristic (char *);
  valuelist<characteristic> & getCharacteristics (void) { return charac; }

  // differentiate between linear and non-linear circuits
  void setNonLinear (bool l) { MODFLAG (!l, CIRCUIT_LINEAR); }
  bool isNonLinear (void) { return !RETFLAG (CIRCUIT_LINEAR); }

  // miscellaneous functionality
  void print (void);
  static char * createInternal (const char *, const char *);
  void setInternalNode (int, const char *);

  //  operations
  void   allocMatrixS (void);
  void   allocMatrixN (int sources = 0);
  void   allocMatrixMNA (void);
  void   freeMatrixMNA (void);
  void   allocMatrixHB (void);
  void   freeMatrixHB (void);
  void   setMatrixS (matrix);
  matrix getMatrixS (void);
  void   setMatrixN (matrix);
  matrix getMatrixN (void);
  void   setMatrixY (matrix);
  matrix getMatrixY (void);

  static const nr_double_t z0;

 protected:
  int type;
  int pol;

 private:
  int size;
  int pacport;
  int vsource;
  int vsources;
  int nsources;
  int inserted;
  int flag;
  nr_complex_t * MatrixS;
  nr_complex_t * MatrixN;
  nr_complex_t * MatrixY;
  nr_complex_t * MatrixB;
  nr_complex_t * MatrixC;
  nr_complex_t * MatrixD;
  nr_complex_t * VectorE;
  nr_complex_t * VectorI;
  nr_complex_t * VectorV;
  nr_complex_t * VectorJ;
  nr_complex_t * VectorQ;
  nr_complex_t * MatrixQV;
  nr_complex_t * VectorGV;
  nr_complex_t * VectorCV;
  char * subcircuit;
  node * nodes;
  substrate * subst;
  valuelist<operatingpoint> oper;
  valuelist<characteristic> charac;
  net * subnet;
  environment * env;
  nr_double_t * deltas;
  int nHistories;
  history * histories;
};

#endif /* __CIRCUIT_H__ */
