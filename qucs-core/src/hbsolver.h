/*
 * hbsolver.h - harmonic balance solver class definitions
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: hbsolver.h,v 1.12 2006/04/27 07:02:48 raimi Exp $
 *
 */

#ifndef __HBSOLVER_H__
#define __HBSOLVER_H__

#include "ptrlist.h"
#include "tvector.h"

class vector;
class strlist;
class circuit;

class hbsolver : public analysis
{
 public:
  hbsolver ();
  hbsolver (char *);
  hbsolver (hbsolver &);
  ~hbsolver ();
  void solve (void);
  void initHB (void);
  void initDC (void);
  static void calc (hbsolver *);
  void collectFrequencies (void);
  int  checkBalance (void);

  void splitCircuits (void);
  void expandFrequencies (nr_double_t, int);
  bool isExcitation (circuit *);
  strlist * circuitNodes (ptrlist<circuit>);
  void getNodeLists (void);
  int  assignVoltageSources (ptrlist<circuit>);
  int  assignNodes (ptrlist<circuit>, strlist *, int offset = 0);
  void prepareLinear (void);
  void createMatrixLinearA (void);
  void fillMatrixLinearA (tmatrix<complex> *, int);
  void invertMatrix (tmatrix<complex> *, tmatrix<complex> *);
  void createMatrixLinearY (void);
  void saveResults (void);
  void calcConstantCurrent (void);
  complex excitationZ (tvector<complex> *, circuit *, int);
  void finalSolution (void);
  void fillMatrixNonLinear (tmatrix<complex> *, tmatrix<complex> *, 
			    tvector<complex> *, tvector<complex> *,
			    tvector<complex> *, tvector<complex> *, int);
  void prepareNonLinear (void);
  void solveHB (void);
  void loadMatrices (void);
  void VectorFFT (tvector<complex> *, int isign = 1);
  void VectorIFFT (tvector<complex> *, int isign = 1);
  int  calcOrder (int);
  void MatrixFFT (tmatrix<complex> *);
  void calcJacobian (void);
  void solveVoltages (void);
  tvector<complex> expandVector (tvector<complex>, int);
  tmatrix<complex> expandMatrix (tmatrix<complex>, int);
  void saveNodeVoltages (circuit *, int);

 private:
  tvector<nr_double_t> negfreqs;    // full frequency set
  tvector<nr_double_t> posfreqs;    // full frequency set but positive
  tvector<nr_double_t> rfreqs;      // real positive frequency set
  int * ndfreqs;                    // number of frequencies for each dimension
  tvector<nr_double_t> dfreqs;      // base frequencies for each dimension
  nr_double_t frequency;
  strlist * nlnodes, * lnnodes, * banodes, * nanodes, * exnodes;
  ptrlist<circuit> excitations;
  ptrlist<circuit> nolcircuits;
  ptrlist<circuit> lincircuits;

  tmatrix<complex> * Y;  // transadmittance matrix of linear network
  tmatrix<complex> * A;  // MNA-matrix of linear network
  tmatrix<complex> * Z;  // transimpedance matrix of linear network

  tmatrix<complex> * YV; // linear transadmittance matrix
  tmatrix<complex> * NA; // MNA-matrix of complete network

  tmatrix<complex> * JQ; // C-Jacobian in t and f
  tmatrix<complex> * JG; // G-Jacobian in t and f
  tmatrix<complex> * JF; // full Jacobian for non-linear balancing
  tvector<complex> * IG; // currents in t and f
  tvector<complex> * FQ; // charges in t and f
  tvector<complex> * VS;
  tvector<complex> * VP;
  tvector<complex> * FV; // error vector F(V) of HB equation
  tvector<complex> * IL; // currents into linear network
  tvector<complex> * IN; // currents into non-linear network

  tvector<complex> * IR;
  tvector<complex> * QR;
  tvector<complex> * RH;
  tvector<complex> * OM;

  tvector<complex> * IC; // source currents into balanced nodes
  tvector<complex> * IS; // currents through sources themselves
  tvector<complex> * x;
  tvector<complex> * vs;

  int runs;
  int lnfreqs;
  int nlfreqs;
  int nnlvsrcs;
  int nlnvsrcs;
  int nlnnodes;
  int nnanodes;
  int nexnodes;
  int nbanodes;
};

#endif /* __HBSOLVER_H__ */
