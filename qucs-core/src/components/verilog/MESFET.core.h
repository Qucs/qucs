/*
 * MESFET.core.h - device class definitions for MESFET module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __MESFET_H__
#define __MESFET_H__

class MESFET : public circuit
{
 public:
  CREATOR (MESFET);

  void initDC (void);
  void restartDC (void);
  void calcDC (void);
  void saveOperatingPoints (void);
  void loadOperatingPoints (void);
  void calcOperatingPoints (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initSP (void);
  void calcSP (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);
  void initHB (int);
  void calcHB (int);
  
 private:
  void initVerilog (void);
  void calcVerilog (void);
  void initModel (void);
  void loadVariables (void);
  void initializeModel (void);
  void initializeInstance (void);
  void initialStep (void);
  void finalStep (void);
  matrix calcMatrixY (nr_double_t);
  matrix calcMatrixCy (nr_double_t);

 private:
  int doHB;
  int doAC;
  int doTR;

  nr_double_t _freq;
  nr_double_t _rhs[7];
  nr_double_t _qhs[7];
  nr_double_t _ghs[7];
  nr_double_t _chs[7];
  nr_double_t _jstat[7][7];
  nr_double_t _jdyna[7][7];
  nr_double_t _white_pwr[7][7];
  nr_double_t _flicker_pwr[7][7];
  nr_double_t _flicker_exp[7][7];
  nr_double_t _charges[7][7];
  nr_double_t _caps[7][7][7][7];

  // device model parameters
  int LEVEL;
  nr_double_t Vto;
  nr_double_t Beta;
  nr_double_t Alpha;
  nr_double_t Lambda;
  nr_double_t B;
  nr_double_t Qp;
  nr_double_t Delta;
  nr_double_t Vmax;
  nr_double_t Vdelta1;
  nr_double_t Vdelta2;
  nr_double_t Gamma;
  nr_double_t Nsc;
  nr_double_t Is;
  nr_double_t N;
  nr_double_t Vbi;
  nr_double_t Bv;
  nr_double_t Xti;
  nr_double_t Fc;
  nr_double_t Tau;
  nr_double_t Rin;
  nr_double_t Area;
  nr_double_t Eg;
  nr_double_t M;
  nr_double_t Cgd;
  nr_double_t Cgs;
  nr_double_t Cds;
  nr_double_t Betatc;
  nr_double_t Alphatc;
  nr_double_t Gammatc;
  nr_double_t Ng;
  nr_double_t Nd;
  int ILEVELS;
  int ILEVELD;
  int QLEVELS;
  int QLEVELD;
  int QLEVELDS;
  nr_double_t Vtotc;
  nr_double_t Rg;
  nr_double_t Rd;
  nr_double_t Rs;
  nr_double_t Rgtc;
  nr_double_t Rdtc;
  nr_double_t Rstc;
  nr_double_t Ibv;
  nr_double_t Rf;
  nr_double_t R1;
  nr_double_t Af;
  nr_double_t Kf;
  nr_double_t Gdsnoi;
  nr_double_t Tnom;

  // global model variables



};

#endif /* __MESFET_H__ */
