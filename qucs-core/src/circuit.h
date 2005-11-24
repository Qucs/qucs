/*
 * circuit.h - circuit class definitions
 *
 * Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: circuit.h,v 1.41 2005/11/24 10:10:21 raimi Exp $
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

#define MODFLAG(val,bit) if (val) flag |= (bit); else flag &= ~(bit);
#define RETFLAG(bit)     ((flag & (bit)) != 0)

class node;
class property;
class substrate;
class operatingpoint;
class matrix;
class transient;
class net;

#include "integrator.h"

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
  virtual void calcOperatingPoints (void) { }
  virtual void saveOperatingPoints (void) { }

  // real basics
  void   setNode (int, char *, int intern = 0);
  node * getNode (int);
  void   setType (int t) { type = t; }
  int    getType (void) { return type; }
  int    getSize (void) { return size; }
  void   setSize (int);
  bool   isEnabled (void) { return RETFLAG (CIRCUIT_ENABLED); }
  void   setEnabled (bool e) { MODFLAG (e, CIRCUIT_ENABLED); }
  bool   isVariableSized (void) { return RETFLAG (CIRCUIT_VARSIZE); }
  void   setVariableSized (bool v) { MODFLAG (v, CIRCUIT_VARSIZE); }
  void   setNet (net * n) { subnet = n; }
  net *  getNet (void) { return subnet; }

  // subcircuitry
  char * getSubcircuit (void) { return subcircuit; }
  void   setSubcircuit (char *);

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
  void setDelta (nr_double_t * d) { deltas = d; }
  nr_double_t * getDelta (void) { return deltas; }

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
  complex getS (int, int);
  complex getN (int, int);
  complex getY (int, int);
  complex getB (int, int);
  complex getC (int, int);
  complex getD (int, int);
  complex getE (int);
  complex getI (int);
  complex getJ (int);
  complex getV (int);
  nr_double_t getG (int, int);
  void setS (int, int, complex);
  void setN (int, int, complex);
  void setY (int, int, complex);
  void setB (int, int, complex);
  void setC (int, int, complex);
  void setD (int, int, complex);
  void setE (int, complex);
  void setI (int, complex);
  void setJ (int, complex);
  void setV (int, complex);
  void setG (int, int, nr_double_t);
  void clearB (void);
  void clearC (void);
  void clearD (void);
  void clearE (void);
  void clearI (void);
  void clearJ (void);
  void clearV (void);
  void clearY (void);
  void addY (int, int, complex);
  void addI (int, complex);

  // operating point functionality
  void        addOperatingPoint (char *, nr_double_t);
  nr_double_t getOperatingPoint (char *);
  void        setOperatingPoint (char *, nr_double_t);
  int         hasOperatingPoint (char *);
  void        copyOperatingPoints (operatingpoint *);
  void        deleteOperatingPoints (void);
  operatingpoint * getOperatingPoints (void) { return oper; }

  // differentiate between linear and non-linear circuits
  void setNonLinear (bool l) { MODFLAG (!l, CIRCUIT_LINEAR); }
  bool isNonLinear (void) { return !RETFLAG (CIRCUIT_LINEAR); }

  // miscellaneous functionality
  void print (void);
  static char * createInternal (char *, char *);

  // matrix operations
  void   allocMatrixS (void);
  void   allocMatrixN (int sources = 0);
  void   allocMatrixMNA (void);
  void   freeMatrixMNA (void);
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
  complex * MatrixS;
  complex * MatrixN;
  complex * MatrixY;
  complex * MatrixB;
  complex * MatrixC;
  complex * MatrixD;
  complex * MatrixE;
  complex * MatrixI;
  complex * MatrixV;
  complex * MatrixJ;
  char * subcircuit;
  node * nodes;
  substrate * subst;
  operatingpoint * oper;
  net * subnet;
  nr_double_t * deltas;
};

#endif /* __CIRCUIT_H__ */
