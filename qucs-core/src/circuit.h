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
 * $Id: circuit.h,v 1.12 2004/05/22 09:17:24 ela Exp $
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

class circuit : public object
{
 public:
  circuit ();
  circuit (int);
  circuit (const circuit &);
  ~circuit ();
  virtual void calcSP (nr_double_t) { }
  virtual void calcDC (void) { }
  virtual void calcAC (nr_double_t) { }
  virtual void initDC (void) { }
  virtual void calcOperatingPoints (void) { }
  void setNode (int, char *);
  node * getNode (int);
  complex getS (int, int);
  void setS (int, int, complex);
  complex getY (int, int);
  void setY (int, int, complex);
  nr_double_t getG (int, int);
  void setG (int, int, nr_double_t);
  int getSize (void) { return size; }
  void print (void);
  int isPort (void) { return port; }
  void setPort (int p) { port = p; }
  int isVoltageSource (void) { return source; }
  void setVoltageSource (int s) { source = s; }
  int getVoltageSources (void);
  void setVoltageSources (int);
  int isOriginal (void) { return org; }
  void setOriginal (int o) { org = o; }
  int countCircuits (void);
  int countPorts (void);
  int countNodes (void);
  void setType (int t) { type = t; }
  int getType (void) { return type; }
  substrate * getSubstrate (void);
  void setSubstrate (substrate *);
  complex getB (int, int);
  complex getC (int, int);
  complex getD (int, int);
  complex getE (int);
  complex getI (int);
  complex getV (int);
  void setB (int, int, complex);
  void setC (int, int, complex);
  void setD (int, int, complex);
  void setE (int, complex);
  void setI (int, complex);
  void setV (int, complex);
  void addOperatingPoint (char *, nr_double_t);
  nr_double_t getOperatingPoint (char *);
  void setOperatingPoint (char *, nr_double_t);
  int hasOperatingPoint (char *);
  void copyOperatingPoints (operatingpoint *);
  void deleteOperatingPoints (void);
  int isNonLinear (void);

 protected:
  static const nr_double_t z0 = 50.0;
  int type;

 private:
  int size;
  int port;
  int source;
  int nSources;
  int org;
  complex * MatrixS;
  complex * MatrixY;
  complex MatrixB[MAX_CIR_VSRCS * MAX_CIR_PORTS];
  complex MatrixC[MAX_CIR_VSRCS * MAX_CIR_PORTS];
  complex MatrixD[MAX_CIR_VSRCS * MAX_CIR_VSRCS];
  complex MatrixE[MAX_CIR_VSRCS];
  complex MatrixI[MAX_CIR_PORTS];
  complex MatrixV[MAX_CIR_PORTS];

  node * nodes;
  substrate * subst;
  operatingpoint * oper;
};

#endif /* __CIRCUIT_H__ */
