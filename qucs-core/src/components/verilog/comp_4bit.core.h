/*
 * comp_4bit.core.h - device class definitions for comp_4bit module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __comp_4bit_H__
#define __comp_4bit_H__

class comp_4bit : public circuit
{
 public:
  CREATOR (comp_4bit);

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
  nr_double_t _rhs[17];
  nr_double_t _qhs[17];
  nr_double_t _ghs[17];
  nr_double_t _chs[17];
  nr_double_t _jstat[17][17];
  nr_double_t _jdyna[17][17];
  nr_double_t _white_pwr[17][17];
  nr_double_t _flicker_pwr[17][17];
  nr_double_t _flicker_exp[17][17];
  nr_double_t _charges[17][17];
  nr_double_t _caps[17][17][17][17];

  // device model parameters
  nr_double_t TR;
  nr_double_t Delay;

  // global model variables
  double Rd;
  double Cd;



};

#endif /* __comp_4bit_H__ */
