/*
 * HBT_X.core.h - device class definitions for HBT_X module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __HBT_X_H__
#define __HBT_X_H__

class HBT_X : public circuit
{
 public:
  CREATOR (HBT_X);

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
  nr_double_t _rhs[19];
  nr_double_t _qhs[19];
  nr_double_t _ghs[19];
  nr_double_t _chs[19];
  nr_double_t _jstat[19][19];
  nr_double_t _jdyna[19][19];
  nr_double_t _white_pwr[19][19];
  nr_double_t _flicker_pwr[19][19];
  nr_double_t _flicker_exp[19][19];
  nr_double_t _charges[19][19];
  nr_double_t _caps[19][19][19][19];

  // device model parameters
  int Mode;
  int Noise;
  int Debug;
  int DebugPlus;
  nr_double_t Temp;
  nr_double_t Rth;
  nr_double_t Cth;
  int N;
  nr_double_t L;
  nr_double_t W;
  nr_double_t Jsf;
  nr_double_t nf;
  nr_double_t Vg;
  nr_double_t Jse;
  nr_double_t ne;
  nr_double_t Rbxx;
  nr_double_t Vgb;
  nr_double_t Jsee;
  nr_double_t nee;
  nr_double_t Rbbxx;
  nr_double_t Vgbb;
  nr_double_t Jsr;
  nr_double_t nr;
  nr_double_t Vgr;
  nr_double_t XCjc;
  nr_double_t Jsc;
  nr_double_t nc;
  nr_double_t Rcxx;
  nr_double_t Vgc;
  nr_double_t Bf;
  nr_double_t kBeta;
  nr_double_t Br;
  nr_double_t VAF;
  nr_double_t VAR;
  nr_double_t IKF;
  nr_double_t IKR;
  nr_double_t Mc;
  nr_double_t BVceo;
  nr_double_t kc;
  nr_double_t BVebo;
  nr_double_t Tr;
  nr_double_t Trx;
  nr_double_t Tf;
  nr_double_t Tft;
  nr_double_t Thcs;
  nr_double_t Ahc;
  nr_double_t Cje;
  nr_double_t mje;
  nr_double_t Vje;
  nr_double_t Cjc;
  nr_double_t mjc;
  nr_double_t Vjc;
  nr_double_t kjc;
  nr_double_t Cmin;
  nr_double_t J0;
  nr_double_t XJ0;
  nr_double_t Rci0;
  nr_double_t Jk;
  nr_double_t RJk;
  nr_double_t Vces;
  nr_double_t Rc;
  nr_double_t Re;
  nr_double_t Rb;
  nr_double_t Rb2;
  nr_double_t Lc;
  nr_double_t Le;
  nr_double_t Lb;
  nr_double_t Cq;
  nr_double_t Cpb;
  nr_double_t Cpc;
  nr_double_t Kfb;
  nr_double_t Afb;
  nr_double_t Ffeb;
  nr_double_t Kb;
  nr_double_t Ab;
  nr_double_t Fb;
  nr_double_t Kfe;
  nr_double_t Afe;
  nr_double_t Ffee;
  nr_double_t Tnom;

  // global model variables



};

#endif /* __HBT_X_H__ */
