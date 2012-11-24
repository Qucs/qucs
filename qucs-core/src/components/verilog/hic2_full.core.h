/*
 * hic2_full.core.h - device class definitions for hic2_full module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __hic2_full_H__
#define __hic2_full_H__

class hic2_full : public circuit
{
 public:
  CREATOR (hic2_full);

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
  nr_double_t _rhs[10];
  nr_double_t _qhs[10];
  nr_double_t _ghs[10];
  nr_double_t _chs[10];
  nr_double_t _jstat[10][10];
  nr_double_t _jdyna[10][10];
  nr_double_t _white_pwr[10][10];
  nr_double_t _flicker_pwr[10][10];
  nr_double_t _flicker_exp[10][10];
  nr_double_t _charges[10][10];
  nr_double_t _caps[10][10][10][10];

  // device model parameters
  nr_double_t c10;
  nr_double_t qp0;
  nr_double_t ich;
  nr_double_t hfe;
  nr_double_t hfc;
  nr_double_t hjei;
  nr_double_t hjci;
  nr_double_t ibeis;
  nr_double_t mbei;
  nr_double_t ireis;
  nr_double_t mrei;
  nr_double_t ibeps;
  nr_double_t mbep;
  nr_double_t ireps;
  nr_double_t mrep;
  nr_double_t mcf;
  nr_double_t tbhrec;
  nr_double_t ibcis;
  nr_double_t mbci;
  nr_double_t ibcxs;
  nr_double_t mbcx;
  nr_double_t ibets;
  nr_double_t abet;
  int tunode;
  nr_double_t favl;
  nr_double_t qavl;
  nr_double_t alfav;
  nr_double_t alqav;
  nr_double_t rbi0;
  nr_double_t rbx;
  nr_double_t fgeo;
  nr_double_t fdqr0;
  nr_double_t fcrbi;
  nr_double_t fqi;
  nr_double_t re;
  nr_double_t rcx;
  nr_double_t itss;
  nr_double_t msf;
  nr_double_t iscs;
  nr_double_t msc;
  nr_double_t tsf;
  nr_double_t rsu;
  nr_double_t csu;
  nr_double_t cjei0;
  nr_double_t vdei;
  nr_double_t zei;
  nr_double_t ajei;
  nr_double_t cjep0;
  nr_double_t vdep;
  nr_double_t zep;
  nr_double_t ajep;
  nr_double_t cjci0;
  nr_double_t vdci;
  nr_double_t zci;
  nr_double_t vptci;
  nr_double_t cjcx0;
  nr_double_t vdcx;
  nr_double_t zcx;
  nr_double_t vptcx;
  nr_double_t fbcpar;
  nr_double_t fbepar;
  nr_double_t cjs0;
  nr_double_t vds;
  nr_double_t zs;
  nr_double_t vpts;
  nr_double_t t0;
  nr_double_t dt0h;
  nr_double_t tbvl;
  nr_double_t tef0;
  nr_double_t gtfe;
  nr_double_t thcs;
  nr_double_t ahc;
  nr_double_t fthc;
  nr_double_t rci0;
  nr_double_t vlim;
  nr_double_t vces;
  nr_double_t vpt;
  nr_double_t tr;
  nr_double_t cbepar;
  nr_double_t cbcpar;
  nr_double_t alqf;
  nr_double_t alit;
  int flnqs;
  nr_double_t kf;
  nr_double_t af;
  int cfbe;
  nr_double_t latb;
  nr_double_t latl;
  nr_double_t vgb;
  nr_double_t alt0;
  nr_double_t kt0;
  nr_double_t zetaci;
  nr_double_t alvs;
  nr_double_t alces;
  nr_double_t zetarbi;
  nr_double_t zetarbx;
  nr_double_t zetarcx;
  nr_double_t zetare;
  nr_double_t zetacx;
  nr_double_t vge;
  nr_double_t vgc;
  nr_double_t vgs;
  nr_double_t f1vg;
  nr_double_t f2vg;
  nr_double_t zetact;
  nr_double_t zetabet;
  nr_double_t alb;
  int flsh;
  nr_double_t rth;
  nr_double_t cth;
  nr_double_t flcomp;
  nr_double_t tnom;
  nr_double_t dt;

  // global model variables



};

#endif /* __hic2_full_H__ */
