/*
 * EKV26MOS.core.h - device class definitions for EKV26MOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __EKV26MOS_H__
#define __EKV26MOS_H__

class EKV26MOS : public circuit
{
 public:
  CREATOR (EKV26MOS);

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
  nr_double_t LEVEL;
  nr_double_t L;
  nr_double_t W;
  nr_double_t Np;
  nr_double_t Ns;
  nr_double_t Cox;
  nr_double_t Xj;
  nr_double_t Dw;
  nr_double_t Dl;
  nr_double_t Vto;
  nr_double_t Gamma;
  nr_double_t Phi;
  nr_double_t Kp;
  nr_double_t Theta;
  nr_double_t EO;
  nr_double_t Ucrit;
  nr_double_t Lambda;
  nr_double_t Weta;
  nr_double_t Leta;
  nr_double_t Q0;
  nr_double_t Lk;
  nr_double_t Tcv;
  nr_double_t Bex;
  nr_double_t Ucex;
  nr_double_t Ibbt;
  nr_double_t Hdif;
  nr_double_t Rsh;
  nr_double_t Rsc;
  nr_double_t Rdc;
  nr_double_t Cgso;
  nr_double_t Cgdo;
  nr_double_t Cgbo;
  nr_double_t Iba;
  nr_double_t Ibb;
  nr_double_t Ibn;
  nr_double_t Kf;
  nr_double_t Af;
  nr_double_t Avto;
  nr_double_t Akp;
  nr_double_t Agamma;
  nr_double_t N;
  nr_double_t Is;
  nr_double_t Bv;
  nr_double_t Ibv;
  nr_double_t Vj;
  nr_double_t Cj0;
  nr_double_t M;
  nr_double_t Area;
  nr_double_t Fc;
  nr_double_t Tt;
  nr_double_t Xti;
  nr_double_t Xpart;
  nr_double_t Tnom;
  int nmos;
  int pmos;

  // global model variables
  double Ucrit_T;
  double Phi_T;
  double Weff;
  double RDeff;
  double RSeff;
  double Kpa_T;
  double Gammaa;
  double Vc;
  double Lc;
  double Lmin;
  double eta;
  double Ibb_T;
  double Vj_T2;
  double F3;
  double GMIN;
  double fourkt;
  double p_n_MOS;
  double Area1;
  double Area2;
  double Area3;
  double DP1;
  double DP2;
  double DP3;
  double DP4;
  double con3;
  double con4;
  double con5;
  double con6;
  double con7;
  double con8;
  double con9;
  double con10;
  double con11;
  double con12;
  double con13;
  double con14;
  double con15;
  double con16;
  double con17;
  double con18;
  double con19;
  double con20;
  double con21;
  double con22;
  double con23;
  double con24;
  double Spart;
  double con25;
  double con26;
  double con27;
  double con28;
  double con29;
  double con30;
  double con31;
  double con32;
  double con33;



};

#endif /* __EKV26MOS_H__ */
