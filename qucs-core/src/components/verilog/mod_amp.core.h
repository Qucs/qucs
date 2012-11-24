/*
 * mod_amp.core.h - device class definitions for mod_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __mod_amp_H__
#define __mod_amp_H__

class mod_amp : public circuit
{
 public:
  CREATOR (mod_amp);

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
  nr_double_t _rhs[14];
  nr_double_t _qhs[14];
  nr_double_t _ghs[14];
  nr_double_t _chs[14];
  nr_double_t _jstat[14][14];
  nr_double_t _jdyna[14][14];
  nr_double_t _white_pwr[14][14];
  nr_double_t _flicker_pwr[14][14];
  nr_double_t _flicker_exp[14][14];
  nr_double_t _charges[14][14];
  nr_double_t _caps[14][14][14][14];

  // device model parameters
  nr_double_t GBP;
  nr_double_t AOLDC;
  nr_double_t FP2;
  nr_double_t RO;
  nr_double_t CD;
  nr_double_t RD;
  nr_double_t IOFF;
  nr_double_t IB;
  nr_double_t VOFF;
  nr_double_t CMRRDC;
  nr_double_t FCM;
  nr_double_t PSRT;
  nr_double_t NSRT;
  nr_double_t VLIMP;
  nr_double_t VLIMN;
  nr_double_t ILMAX;
  nr_double_t CSCALE;

  // global model variables



};

#endif /* __mod_amp_H__ */
