/*
 * bsim3v34pMOS.core.h - device class definitions for bsim3v34pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 */

#ifndef __bsim3v34pMOS_H__
#define __bsim3v34pMOS_H__

class bsim3v34pMOS : public qucs::circuit
{
 public:
  CREATOR (bsim3v34pMOS);

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
  qucs::matrix calcMatrixY (nr_double_t);
  qucs::matrix calcMatrixCy (nr_double_t);

 private:
  int doHB;
  int doAC;
  int doTR;

  nr_double_t _freq;
  nr_double_t _rhs[6];
  nr_double_t _qhs[6];
  nr_double_t _ghs[6];
  nr_double_t _chs[6];
  nr_double_t _jstat[6][6];
  nr_double_t _jdyna[6][6];
  nr_double_t _white_pwr[6][6];
  nr_double_t _flicker_pwr[6][6];
  nr_double_t _flicker_exp[6][6];
  nr_double_t _charges[6][6];
  nr_double_t _caps[6][6][6][6];

  // device model parameters
  nr_double_t L;
  nr_double_t W;
  nr_double_t PS;
  nr_double_t PD;
  nr_double_t AS;
  nr_double_t AD;
  nr_double_t NRS;
  nr_double_t NRD;
  nr_double_t NQSMOD;
  nr_double_t GMIN;
  nr_double_t VERSION;
  nr_double_t PARAMCHK;
  nr_double_t MOBMOD;
  nr_double_t CAPMOD;
  nr_double_t NOIMOD;
  nr_double_t BINUNIT;
  nr_double_t TOX;
  nr_double_t TOXM;
  nr_double_t CDSC;
  nr_double_t CDSCB;
  nr_double_t CDSCD;
  nr_double_t CIT;
  nr_double_t NFACTOR;
  nr_double_t XJ;
  nr_double_t VSAT;
  nr_double_t AT;
  nr_double_t A0;
  nr_double_t AGS;
  nr_double_t A1;
  nr_double_t A2;
  nr_double_t KETA;
  nr_double_t NSUB;
  nr_double_t NCH;
  nr_double_t NGATE;
  nr_double_t GAMMA1;
  nr_double_t GAMMA2;
  nr_double_t VBX;
  nr_double_t VBM;
  nr_double_t XT;
  nr_double_t K1;
  nr_double_t KT1;
  nr_double_t KT1L;
  nr_double_t KT2;
  nr_double_t K2;
  nr_double_t K3;
  nr_double_t K3B;
  nr_double_t W0;
  nr_double_t NLX;
  nr_double_t DVT0;
  nr_double_t DVT1;
  nr_double_t DVT2;
  nr_double_t DVT0W;
  nr_double_t DVT1W;
  nr_double_t DVT2W;
  nr_double_t DROUT;
  nr_double_t DSUB;
  nr_double_t VTHO;
  nr_double_t VTH0;
  nr_double_t UA;
  nr_double_t UA1;
  nr_double_t UB;
  nr_double_t UB1;
  nr_double_t UC;
  nr_double_t UC1;
  nr_double_t U0;
  nr_double_t UTE;
  nr_double_t VOFF;
  nr_double_t TNOM;
  nr_double_t CGSO;
  nr_double_t CGDO;
  nr_double_t CGBO;
  nr_double_t XPART;
  nr_double_t ELM;
  nr_double_t DELTA;
  nr_double_t RSH;
  nr_double_t RDSW;
  nr_double_t PRWG;
  nr_double_t PRWB;
  nr_double_t PRT;
  nr_double_t ETA0;
  nr_double_t ETAB;
  nr_double_t PCLM;
  nr_double_t PDIBLC1;
  nr_double_t PDIBLC2;
  nr_double_t PDIBLCB;
  nr_double_t PSCBE1;
  nr_double_t PSCBE2;
  nr_double_t PVAG;
  nr_double_t JS;
  nr_double_t JSW;
  nr_double_t PB;
  nr_double_t NJ;
  nr_double_t XTI;
  nr_double_t MJ;
  nr_double_t PBSW;
  nr_double_t MJSW;
  nr_double_t PBSWG;
  nr_double_t MJSWG;
  nr_double_t CJ;
  nr_double_t VFBCV;
  nr_double_t VFB;
  nr_double_t CJSW;
  nr_double_t CJSWG;
  nr_double_t TPB;
  nr_double_t TCJ;
  nr_double_t TPBSW;
  nr_double_t TCJSW;
  nr_double_t TPBSWG;
  nr_double_t TCJSWG;
  nr_double_t ACDE;
  nr_double_t MOIN;
  nr_double_t NOFF;
  nr_double_t VOFFCV;
  nr_double_t LINT;
  nr_double_t LL;
  nr_double_t LLC;
  nr_double_t LLN;
  nr_double_t LW;
  nr_double_t LWC;
  nr_double_t LWN;
  nr_double_t LWL;
  nr_double_t LWLC;
  nr_double_t LMIN;
  nr_double_t LMAX;
  nr_double_t WR;
  nr_double_t WINT;
  nr_double_t DWG;
  nr_double_t DWB;
  nr_double_t WL;
  nr_double_t WLC;
  nr_double_t WLN;
  nr_double_t WW;
  nr_double_t WWC;
  nr_double_t WWN;
  nr_double_t WWL;
  nr_double_t WWLC;
  nr_double_t WMIN;
  nr_double_t WMAX;
  nr_double_t B0;
  nr_double_t B1;
  nr_double_t CGSL;
  nr_double_t CGDL;
  nr_double_t CKAPPA;
  nr_double_t CF;
  nr_double_t CLC;
  nr_double_t CLE;
  nr_double_t DWC;
  nr_double_t DLC;
  nr_double_t ALPHA0;
  nr_double_t ALPHA1;
  nr_double_t BETA0;
  nr_double_t IJTH;
  nr_double_t LCDSC;
  nr_double_t LCDSCB;
  nr_double_t LCDSCD;
  nr_double_t LCIT;
  nr_double_t LNFACTOR;
  nr_double_t LXJ;
  nr_double_t LVSAT;
  nr_double_t LAT;
  nr_double_t LA0;
  nr_double_t LAGS;
  nr_double_t LA1;
  nr_double_t LA2;
  nr_double_t LKETA;
  nr_double_t LNSUB;
  nr_double_t LNCH;
  nr_double_t LNGATE;
  nr_double_t LGAMMA1;
  nr_double_t LGAMMA2;
  nr_double_t LVBX;
  nr_double_t LVBM;
  nr_double_t LXT;
  nr_double_t LK1;
  nr_double_t LKT1;
  nr_double_t LKT1L;
  nr_double_t LKT2;
  nr_double_t LK2;
  nr_double_t LK3;
  nr_double_t LK3B;
  nr_double_t LW0;
  nr_double_t LNLX;
  nr_double_t LDVT0;
  nr_double_t LDVT1;
  nr_double_t LDVT2;
  nr_double_t LDVT0W;
  nr_double_t LDVT1W;
  nr_double_t LDVT2W;
  nr_double_t LDROUT;
  nr_double_t LDSUB;
  nr_double_t LVTH0;
  nr_double_t LVTHO;
  nr_double_t LUA;
  nr_double_t LUA1;
  nr_double_t LUB;
  nr_double_t LUB1;
  nr_double_t LUC;
  nr_double_t LUC1;
  nr_double_t LU0;
  nr_double_t LUTE;
  nr_double_t LVOFF;
  nr_double_t LELM;
  nr_double_t LDELTA;
  nr_double_t LRDSW;
  nr_double_t LPRWG;
  nr_double_t LPRWB;
  nr_double_t LPRT;
  nr_double_t LETA0;
  nr_double_t LETAB;
  nr_double_t LPCLM;
  nr_double_t LPDIBLC1;
  nr_double_t LPDIBLC2;
  nr_double_t LPDIBLCB;
  nr_double_t LPSCBE1;
  nr_double_t LPSCBE2;
  nr_double_t LPVAG;
  nr_double_t LWR;
  nr_double_t LDWG;
  nr_double_t LDWB;
  nr_double_t LB0;
  nr_double_t LB1;
  nr_double_t LCGSL;
  nr_double_t LCGDL;
  nr_double_t LCKAPPA;
  nr_double_t LCF;
  nr_double_t LCLC;
  nr_double_t LCLE;
  nr_double_t LALPHA0;
  nr_double_t LALPHA1;
  nr_double_t LBETA0;
  nr_double_t LVFBCV;
  nr_double_t LVFB;
  nr_double_t LACDE;
  nr_double_t LMOIN;
  nr_double_t LNOFF;
  nr_double_t LVOFFCV;
  nr_double_t WCDSC;
  nr_double_t WCDSCB;
  nr_double_t WCDSCD;
  nr_double_t WCIT;
  nr_double_t WNFACTOR;
  nr_double_t WXJ;
  nr_double_t WVSAT;
  nr_double_t WAT;
  nr_double_t WA0;
  nr_double_t WAGS;
  nr_double_t WA1;
  nr_double_t WA2;
  nr_double_t WKETA;
  nr_double_t WNSUB;
  nr_double_t WNCH;
  nr_double_t WNGATE;
  nr_double_t WGAMMA1;
  nr_double_t WGAMMA2;
  nr_double_t WVBX;
  nr_double_t WVBM;
  nr_double_t WXT;
  nr_double_t WK1;
  nr_double_t WKT1;
  nr_double_t WKT1L;
  nr_double_t WKT2;
  nr_double_t WK2;
  nr_double_t WK3;
  nr_double_t WK3B;
  nr_double_t WW0;
  nr_double_t WNLX;
  nr_double_t WDVT0;
  nr_double_t WDVT1;
  nr_double_t WDVT2;
  nr_double_t WDVT0W;
  nr_double_t WDVT1W;
  nr_double_t WDVT2W;
  nr_double_t WDROUT;
  nr_double_t WDSUB;
  nr_double_t WVTH0;
  nr_double_t WVTHO;
  nr_double_t WUA;
  nr_double_t WUA1;
  nr_double_t WUB;
  nr_double_t WUB1;
  nr_double_t WUC;
  nr_double_t WUC1;
  nr_double_t WU0;
  nr_double_t WUTE;
  nr_double_t WVOFF;
  nr_double_t WELM;
  nr_double_t WDELTA;
  nr_double_t WRDSW;
  nr_double_t WPRWG;
  nr_double_t WPRWB;
  nr_double_t WPRT;
  nr_double_t WETA0;
  nr_double_t WETAB;
  nr_double_t WPCLM;
  nr_double_t WPDIBLC1;
  nr_double_t WPDIBLC2;
  nr_double_t WPDIBLCB;
  nr_double_t WPSCBE1;
  nr_double_t WPSCBE2;
  nr_double_t WPVAG;
  nr_double_t WWR;
  nr_double_t WDWG;
  nr_double_t WDWB;
  nr_double_t WB0;
  nr_double_t WB1;
  nr_double_t WCGSL;
  nr_double_t WCGDL;
  nr_double_t WCKAPPA;
  nr_double_t WCF;
  nr_double_t WCLC;
  nr_double_t WCLE;
  nr_double_t WALPHA0;
  nr_double_t WALPHA1;
  nr_double_t WBETA0;
  nr_double_t WVFBCV;
  nr_double_t WVFB;
  nr_double_t WACDE;
  nr_double_t WMOIN;
  nr_double_t WNOFF;
  nr_double_t WVOFFCV;
  nr_double_t PCDSC;
  nr_double_t PCDSCB;
  nr_double_t PCDSCD;
  nr_double_t PCIT;
  nr_double_t PNFACTOR;
  nr_double_t PXJ;
  nr_double_t PVSAT;
  nr_double_t PAT;
  nr_double_t PA0;
  nr_double_t PAGS;
  nr_double_t PA1;
  nr_double_t PA2;
  nr_double_t PKETA;
  nr_double_t PNSUB;
  nr_double_t PNCH;
  nr_double_t PNGATE;
  nr_double_t PGAMMA1;
  nr_double_t PGAMMA2;
  nr_double_t PVBX;
  nr_double_t PVBM;
  nr_double_t PXT;
  nr_double_t PK1;
  nr_double_t PKT1;
  nr_double_t PKT1L;
  nr_double_t PKT2;
  nr_double_t PK2;
  nr_double_t PK3;
  nr_double_t PK3B;
  nr_double_t PW0;
  nr_double_t PNLX;
  nr_double_t PDVT0;
  nr_double_t PDVT1;
  nr_double_t PDVT2;
  nr_double_t PDVT0W;
  nr_double_t PDVT1W;
  nr_double_t PDVT2W;
  nr_double_t PDROUT;
  nr_double_t PDSUB;
  nr_double_t PVTH0;
  nr_double_t PVTHO;
  nr_double_t PUA;
  nr_double_t PUA1;
  nr_double_t PUB;
  nr_double_t PUB1;
  nr_double_t PUC;
  nr_double_t PUC1;
  nr_double_t PU0;
  nr_double_t PUTE;
  nr_double_t PVOFF;
  nr_double_t PELM;
  nr_double_t PDELTA;
  nr_double_t PRDSW;
  nr_double_t PPRWG;
  nr_double_t PPRWB;
  nr_double_t PPRT;
  nr_double_t PETA0;
  nr_double_t PETAB;
  nr_double_t PPCLM;
  nr_double_t PPDIBLC1;
  nr_double_t PPDIBLC2;
  nr_double_t PPDIBLCB;
  nr_double_t PPSCBE1;
  nr_double_t PPSCBE2;
  nr_double_t PPVAG;
  nr_double_t PWR;
  nr_double_t PDWG;
  nr_double_t PDWB;
  nr_double_t PB0;
  nr_double_t PB1;
  nr_double_t PCGSL;
  nr_double_t PCGDL;
  nr_double_t PCKAPPA;
  nr_double_t PCF;
  nr_double_t PCLC;
  nr_double_t PCLE;
  nr_double_t PALPHA0;
  nr_double_t PALPHA1;
  nr_double_t PBETA0;
  nr_double_t PVFBCV;
  nr_double_t PVFB;
  nr_double_t PACDE;
  nr_double_t PMOIN;
  nr_double_t PNOFF;
  nr_double_t PVOFFCV;
  nr_double_t KF;
  nr_double_t AF;
  nr_double_t EF;

  // global model variables

  // 
  double TYPE;
  // 
  double tox;
  // 
  double cox;
  // 
  double vth0;
  // 
  double ngate;
  // 
  double uc;
  // 
  double tnom;
  // 
  double pdiblb;
  // 
  double bulkJctBotGradingCoeff;
  // 
  double bulkJctSideGradingCoeff;
  // 
  double bulkJctGateSideGradingCoeff;
  // 
  double unitAreaJctCap;
  // 
  double unitAreaTempJctCap;
  // 
  double unitLengthSidewallJctCap;
  // 
  double unitLengthSidewallTempJctCap;
  // 
  double unitLengthGateSidewallJctCap;
  // 
  double unitLengthGateSidewallTempJctCap;
  // 
  double Temp;
  // 
  double factor1;
  // 
  double vtm;
  // 
  double T_0;
  // 
  double T1;
  // 
  double T2;
  // 
  double T3;
  // 
  double T4;
  // 
  double T5;
  // 
  double PhiB;
  // 
  double PhiBSW;
  // 
  double PhiBSWG;
  // 
  double tmp;
  // 
  double tmp1;
  // 
  double tmp2;
  // 
  double tmp3;
  // 
  double leff;
  // 
  double weff;
  // 
  double leffCV;
  // 
  double weffCV;
  // 
  double cdsc;
  // 
  double cdscb;
  // 
  double cdscd;
  // 
  double cit;
  // 
  double nfactor;
  // 
  double xj;
  // 
  double a0;
  // 
  double ags;
  // 
  double a1;
  // 
  double a2;
  // 
  double keta;
  // 
  double vfb;
  // 
  double k1;
  // 
  double kt1;
  // 
  double kt1l;
  // 
  double kt2;
  // 
  double k3;
  // 
  double k3b;
  // 
  double w0;
  // 
  double nlx;
  // 
  double dvt0;
  // 
  double dvt1;
  // 
  double dvt2;
  // 
  double dvt0w;
  // 
  double dvt1w;
  // 
  double dvt2w;
  // 
  double ua;
  // 
  double ub;
  // 
  double voff;
  // 
  double delta;
  // 
  double prwg;
  // 
  double prwb;
  // 
  double eta0;
  // 
  double etab;
  // 
  double pclm;
  // 
  double pscbe1;
  // 
  double pscbe2;
  // 
  double pvag;
  // 
  double dwg;
  // 
  double dwb;
  // 
  double b0;
  // 
  double b1;
  // 
  double alpha0;
  // 
  double alpha1;
  // 
  double beta0;
  // 
  double cgsl;
  // 
  double cgdl;
  // 
  double ckappa;
  // 
  double vfbcv;
  // 
  double acde;
  // 
  double moin;
  // 
  double noff_param;
  // 
  double voffcv_param;
  // 
  double abulkCVfactor;
  // 
  double u0temp;
  // 
  double vsattemp;
  // 
  double rds0;
  // 
  double phi;
  // 
  double sqrtPhi;
  // 
  double phis3;
  // 
  double Xdep0;
  // 
  double litl;
  // 
  double vbi;
  // 
  double cdep0;
  // 
  double ldeb;
  // 
  double vbsc;
  // 
  double k1ox;
  // 
  double k2ox;
  // 
  double theta0vb0;
  // 
  double thetaRout;
  // 
  double vfbzb;
  // 
  double sourceArea;
  // 
  double drainArea;
  // 
  double sourcePerimeter;
  // 
  double drainPerimeter;
  // 
  double SourceSatCurrent;
  // 
  double DrainSatCurrent;
  // 
  double sourceConductance;
  // 
  double drainConductance;
  // 
  double Nvtm;
  // 
  double vjsm;
  // 
  double vjdm;
  // 
  double IsEvjsm;
  // 
  double IsEvjdm;
  // 
  double cgso_param;
  // 
  double cgdo_param;
  // 
  double cgbo_param;


};

#endif /* __bsim3v34pMOS_H__ */
