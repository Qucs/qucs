/*
 * log_amp.core.h - device class definitions for log_amp module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __log_amp_H__
#define __log_amp_H__

class log_amp : public circuit
{
 public:
  CREATOR (log_amp);

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
  nr_double_t Kv;
  nr_double_t Dk;
  nr_double_t Ib1;
  nr_double_t Ibr;
  nr_double_t M;
  nr_double_t N;
  nr_double_t Vosout;
  nr_double_t Rinp;
  nr_double_t Fc;
  nr_double_t Ro;
  nr_double_t Ntc;
  nr_double_t Vosouttc;
  nr_double_t Dktc;
  nr_double_t Ib1tc;
  nr_double_t Ibrtc;
  nr_double_t Tnom;

  // global model variables



};

#endif /* __log_amp_H__ */
