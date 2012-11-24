/*
 * nigbt.core.h - device class definitions for nigbt module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __nigbt_H__
#define __nigbt_H__

class nigbt : public circuit
{
 public:
  CREATOR (nigbt);

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
  nr_double_t _rhs[9];
  nr_double_t _qhs[9];
  nr_double_t _ghs[9];
  nr_double_t _chs[9];
  nr_double_t _jstat[9][9];
  nr_double_t _jdyna[9][9];
  nr_double_t _white_pwr[9][9];
  nr_double_t _flicker_pwr[9][9];
  nr_double_t _flicker_exp[9][9];
  nr_double_t _charges[9][9];
  nr_double_t _caps[9][9][9][9];

  // device model parameters
  nr_double_t Agd;
  nr_double_t Area;
  nr_double_t Kp;
  nr_double_t Tau;
  nr_double_t Wb;
  nr_double_t BVf;
  nr_double_t BVn;
  nr_double_t Cgs;
  nr_double_t Coxd;
  nr_double_t Jsne;
  nr_double_t Kf;
  nr_double_t Mun;
  nr_double_t Mup;
  nr_double_t Nb;
  nr_double_t Theta;
  nr_double_t Vt;
  nr_double_t Vtd;
  nr_double_t Tnom;

  // global model variables
  double Nl;
  double Wl;
  double Isne;
  double B;
  double Dp;
  double A;
  double Ads;
  double Tj;
  double Ni;
  double Vpsat;
  double Vnsat;
  double Alpha1;
  double Alpha2;
  double KpT;



};

#endif /* __nigbt_H__ */
