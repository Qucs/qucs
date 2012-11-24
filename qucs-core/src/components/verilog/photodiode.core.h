/*
 * photodiode.core.h - device class definitions for photodiode module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __photodiode_H__
#define __photodiode_H__

class photodiode : public circuit
{
 public:
  CREATOR (photodiode);

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
  nr_double_t _rhs[4];
  nr_double_t _qhs[4];
  nr_double_t _ghs[4];
  nr_double_t _chs[4];
  nr_double_t _jstat[4][4];
  nr_double_t _jdyna[4][4];
  nr_double_t _white_pwr[4][4];
  nr_double_t _flicker_pwr[4][4];
  nr_double_t _flicker_exp[4][4];
  nr_double_t _charges[4][4];
  nr_double_t _caps[4][4][4][4];

  // device model parameters
  nr_double_t N;
  nr_double_t Rseries;
  nr_double_t Is;
  nr_double_t Bv;
  nr_double_t Ibv;
  nr_double_t Vj;
  nr_double_t Cj0;
  nr_double_t M;
  nr_double_t Area;
  nr_double_t Tnom;
  nr_double_t Fc;
  nr_double_t Tt;
  nr_double_t Xti;
  nr_double_t Eg;
  nr_double_t Responsivity;
  nr_double_t Rsh;
  nr_double_t QEpercent;
  nr_double_t Lambda;
  int LEVEL;
  nr_double_t Kf;
  nr_double_t Af;
  nr_double_t Ffe;

  // global model variables

  // 
  double F1;
  // 
  double F2;
  // 
  double F3;
  // 
  double Rseries_Area;
  // 
  double Vt_T2;
  // 
  double Vj_T2;
  // 
  double GMIN;
  // 
  double Res;
  // 
  double con1;
  // 
  double con2;
  // 
  double con3;
  // 
  double con4;
  // 
  double con5;
  // 
  double con6;


};

#endif /* __photodiode_H__ */
