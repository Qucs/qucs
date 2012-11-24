/*
 * phototransistor.core.h - device class definitions for phototransistor module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __phototransistor_H__
#define __phototransistor_H__

class phototransistor : public circuit
{
 public:
  CREATOR (phototransistor);

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
  nr_double_t Bf;
  nr_double_t Br;
  nr_double_t Is;
  nr_double_t Nf;
  nr_double_t Nr;
  nr_double_t Vaf;
  nr_double_t Var;
  nr_double_t Mje;
  nr_double_t Vje;
  nr_double_t Cje;
  nr_double_t Mjc;
  nr_double_t Vjc;
  nr_double_t Cjc;
  nr_double_t Tr;
  nr_double_t Tf;
  nr_double_t Ikf;
  nr_double_t Ikr;
  nr_double_t Rc;
  nr_double_t Re;
  nr_double_t Rb;
  nr_double_t Kf;
  nr_double_t Ffe;
  nr_double_t Af;
  nr_double_t Responsivity;
  nr_double_t P0;
  nr_double_t P1;
  nr_double_t P2;
  nr_double_t P3;
  nr_double_t P4;

  // global model variables

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
  // 
  double con8;
  // 
  double con9;
  // 
  double con10;
  // 
  double TwoQ;
  // 
  double FourKT;
  // 
  double GMIN;


};

#endif /* __phototransistor_H__ */
