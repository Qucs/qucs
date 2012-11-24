/*
 * DLS_1ton.core.h - device class definitions for DLS_1ton module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __DLS_1ton_H__
#define __DLS_1ton_H__

class DLS_1ton : public circuit
{
 public:
  CREATOR (DLS_1ton);

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
  nr_double_t _rhs[5];
  nr_double_t _qhs[5];
  nr_double_t _ghs[5];
  nr_double_t _chs[5];
  nr_double_t _jstat[5][5];
  nr_double_t _jdyna[5][5];
  nr_double_t _white_pwr[5][5];
  nr_double_t _flicker_pwr[5][5];
  nr_double_t _flicker_exp[5][5];
  nr_double_t _charges[5][5];
  nr_double_t _caps[5][5][5][5];

  // device model parameters
  nr_double_t LEVEL;
  nr_double_t Delay;

  // global model variables
  double Rd;
  double Cd;



};

#endif /* __DLS_1ton_H__ */
