/*
 * circuit.h - circuit class definitions
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: circuit.h,v 1.22 2004/09/06 06:40:07 ela Exp $
 *
 */

#ifndef __CIRCUIT_H__
#define __CIRCUIT_H__

#define MAX_CIR_PORTS 6
#define MAX_CIR_VSRCS 3

class node;
class property;
class substrate;
class operatingpoint;
class dcsolver;
class spsolver;
class acsolver;
class matrix;

class circuit : public object
{
 public:
  // constructor and destructor set
  circuit ();
  circuit (int);
  circuit (const circuit &);
  ~circuit ();

  // functionality to be overloaded by real circuit implementations
  virtual void initSP (spsolver *) { }
  virtual void calcSP (nr_double_t) { }
  virtual void initDC (dcsolver *) { }
  virtual void calcDC (void) { }
  virtual void calcNoise (nr_double_t) { }
  virtual void initAC (acsolver *) { }
  virtual void calcAC (nr_double_t) { }
  virtual void calcOperatingPoints (void) { }

  // real basics
  void   setNode (int, char *, int intern = 0);
  node * getNode (int);
  void   setType (int t) { type = t; }
  int    getType (void) { return type; }
  int    getSize (void) { return size; }
  void   setSize (int);
  int    isEnabled (void) { return enabled; }
  void   setEnabled (int e) { enabled = e; }

  // subcircuitry
  char * getSubcircuit (void) { return subcircuit; }
  void   setSubcircuit (char *);

  // nodal analyses helpers
  void setInternalVoltageSource (int i) { internal = i; }
  int  isInternalVoltageSource (void) { return internal; }
  void setVoltageSource (int s) { vsource = s; }
  int  isVoltageSource (void) { return vsource; }
  int  getVoltageSources (void);
  void setVoltageSources (int);

  // s-parameter helpers
  int  isPort (void) { return pacport; }
  void setPort (int p) { pacport = p; }
  int  isOriginal (void) { return original; }
  void setOriginal (int o) { original = o; }
  int  getInserted (void) { return inserted; }
  void setInserted (int i) { inserted = i; }

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
  void setV (int, complex);
  void setG (int, int, nr_double_t);
  void clearB (void);
  void clearC (void);
  void clearD (void);
  void clearE (void);
  void clearI (void);
  void clearV (void);
  void clearY (void);

  // operating point functionality
  void        addOperatingPoint (char *, nr_double_t);
  nr_double_t getOperatingPoint (char *);
  void        setOperatingPoint (char *, nr_double_t);
  int         hasOperatingPoint (char *);
  void        copyOperatingPoints (operatingpoint *);
  void        deleteOperatingPoints (void);
  operatingpoint * getOperatingPoints (void) { return oper; }

  // differentiate between linear and non-linear circuits
  void setNonLinear (int n) { linear = !n; }
  int  isNonLinear (void) { return !linear; }

  // miscellaneous functionality
  void print (void);
  static char * createInternal (char *, char *);

  // matrix operations
  void    setMatrixS (matrix &);
  matrix& getMatrixS (void);
  void    setMatrixN (matrix &);
  matrix& getMatrixN (void);
  void    setMatrixY (matrix &);
  matrix& getMatrixY (void);

  static const nr_double_t z0 = 50.0;

 protected:
  int type;

 private:
  int size;
  int pacport;
  int vsource;
  int vsources;
  int original;
  int internal;
  int inserted;
  int linear;
  int enabled;
  complex * MatrixS;
  complex * MatrixN;
  complex * MatrixY;
  complex MatrixB[MAX_CIR_VSRCS * MAX_CIR_PORTS];
  complex MatrixC[MAX_CIR_VSRCS * MAX_CIR_PORTS];
  complex MatrixD[MAX_CIR_VSRCS * MAX_CIR_VSRCS];
  complex MatrixE[MAX_CIR_VSRCS];
  complex MatrixI[MAX_CIR_PORTS];
  complex MatrixV[MAX_CIR_PORTS];
  char * subcircuit;
  node * nodes;
  substrate * subst;
  operatingpoint * oper;
};

#endif /* __CIRCUIT_H__ */
