/*
 * hicumL0V1p2.core.h - device class definitions for hicumL0V1p2 module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __hicumL0V1p2_H__
#define __hicumL0V1p2_H__

class hicumL0V1p2 : public circuit
{
 public:
  CREATOR (hicumL0V1p2);

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
  nr_double_t _rhs[8];
  nr_double_t _qhs[8];
  nr_double_t _ghs[8];
  nr_double_t _chs[8];
  nr_double_t _jstat[8][8];
  nr_double_t _jdyna[8][8];
  nr_double_t _white_pwr[8][8];
  nr_double_t _flicker_pwr[8][8];
  nr_double_t _flicker_exp[8][8];
  nr_double_t _charges[8][8];
  nr_double_t _caps[8][8][8][8];

  // device model parameters
  nr_double_t is;
  nr_double_t mcf;
  nr_double_t mcr;
  nr_double_t vef;
  nr_double_t ver;
  nr_double_t iqf;
  nr_double_t fiqf;
  nr_double_t iqr;
  nr_double_t iqfh;
  nr_double_t tfh;
  nr_double_t ahq;
  nr_double_t ibes;
  nr_double_t mbe;
  nr_double_t ires;
  nr_double_t mre;
  nr_double_t ibcs;
  nr_double_t mbc;
  nr_double_t cje0;
  nr_double_t vde;
  nr_double_t ze;
  nr_double_t aje;
  nr_double_t vdedc;
  nr_double_t zedc;
  nr_double_t ajedc;
  nr_double_t t0;
  nr_double_t dt0h;
  nr_double_t tbvl;
  nr_double_t tef0;
  nr_double_t gte;
  nr_double_t thcs;
  nr_double_t ahc;
  nr_double_t tr;
  nr_double_t rci0;
  nr_double_t vlim;
  nr_double_t vpt;
  nr_double_t vces;
  nr_double_t cjci0;
  nr_double_t vdci;
  nr_double_t zci;
  nr_double_t vptci;
  nr_double_t cjcx0;
  nr_double_t vdcx;
  nr_double_t zcx;
  nr_double_t vptcx;
  nr_double_t fbc;
  nr_double_t rbi0;
  nr_double_t vr0e;
  nr_double_t vr0c;
  nr_double_t fgeo;
  nr_double_t rbx;
  nr_double_t rcx;
  nr_double_t re;
  nr_double_t itss;
  nr_double_t msf;
  nr_double_t iscs;
  nr_double_t msc;
  nr_double_t cjs0;
  nr_double_t vds;
  nr_double_t zs;
  nr_double_t vpts;
  nr_double_t cbcpar;
  nr_double_t cbepar;
  nr_double_t eavl;
  nr_double_t kavl;
  nr_double_t kf;
  nr_double_t af;
  nr_double_t vgb;
  nr_double_t vge;
  nr_double_t vgc;
  nr_double_t vgs;
  nr_double_t f1vg;
  nr_double_t f2vg;
  nr_double_t alt0;
  nr_double_t kt0;
  nr_double_t zetact;
  nr_double_t zetabet;
  nr_double_t zetaci;
  nr_double_t alvs;
  nr_double_t alces;
  nr_double_t zetarbi;
  nr_double_t zetarbx;
  nr_double_t zetarcx;
  nr_double_t zetare;
  nr_double_t zetaiqf;
  nr_double_t alkav;
  nr_double_t aleav;
  nr_double_t zetarth;
  int flsh;
  nr_double_t rth;
  nr_double_t cth;
  int npn;
  int pnp;
  nr_double_t tnom;
  nr_double_t dt;

  // global model variables
  double HICUMtype; // Device type from npn or pnp flags (no)

  // B-C internal junction charge (C)
  double qjci;
  // B-E internal junction charge (C)
  double qjei;
  // B-E internal junction capacitance (F)
  double cjei;
  // Transfer Current (A)
  double it;
  // Base-collector diode current (A)
  double ijbc;
  // Avalanche current (A)
  double iavl;
  // Substrate-collector diode current (A)
  double ijsc;
  // Current through external to internal emitter node (A)
  double Ieei;
  // Current through external to internal collector node (A)
  double Icci;
  // Current through external to internal base node (A)
  double Ibbi;
  // Base-collector diode current minus the avalanche current (A)
  double Ibici;
  // Base-emitter diode current (A)
  double ijbe;


};

#endif /* __hicumL0V1p2_H__ */
