/*
 * bsim3v34pMOS.core.cpp - device implementations for bsim3v34pMOS module
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "bsim3v34pMOS.analogfunction.h"
#include "component.h"
#include "device.h"
#include "bsim3v34pMOS.core.h"

#ifndef CIR_bsim3v34pMOS
#define CIR_bsim3v34pMOS -1
#endif

// external nodes
#define drain 0
#define gate 1
#define source 2
#define bulk 3
// internal nodes
#define drainp 4
#define sourcep 5

// useful macro definitions
#define NP(node) real (getV (node))
#define BP(pnode,nnode) (NP(pnode) - NP(nnode))
#define _load_static_residual2(pnode,nnode,current)\
	_rhs[pnode] -= current;\
	_rhs[nnode] += current;
#define _load_static_augmented_residual2(pnode,nnode,current)\
	_rhs[pnode] -= current;\
	_rhs[nnode] += current;
#define _load_static_residual1(node,current)\
	_rhs[node] -= current;
#define _load_static_augmented_residual1(node,current)\
	_rhs[node] -= current;
#define _load_static_jacobian4(pnode,nnode,vpnode,vnnode,conductance)\
	_jstat[pnode][vpnode] += conductance;\
	_jstat[nnode][vnnode] += conductance;\
	_jstat[pnode][vnnode] -= conductance;\
	_jstat[nnode][vpnode] -= conductance;\
	if (doHB) {\
	_ghs[pnode] += conductance * BP(vpnode,vnnode);\
	_ghs[nnode] -= conductance * BP(vpnode,vnnode);\
        } else {\
	_rhs[pnode] += conductance * BP(vpnode,vnnode);\
	_rhs[nnode] -= conductance * BP(vpnode,vnnode);\
	}
#define _load_static_jacobian2p(node,vpnode,vnnode,conductance)\
	_jstat[node][vpnode] += conductance;\
	_jstat[node][vnnode] -= conductance;\
	if (doHB) {\
        _ghs[node] += conductance * BP(vpnode,vnnode);\
        } else {\
        _rhs[node] += conductance * BP(vpnode,vnnode);\
	}
#define _load_static_jacobian2s(pnode,nnode,node,conductance)\
	_jstat[pnode][node] += conductance;\
	_jstat[nnode][node] -= conductance;\
	if (doHB) {\
	_ghs[pnode] += conductance * NP(node);\
	_ghs[nnode] -= conductance * NP(node);\
        } else {\
	_rhs[pnode] += conductance * NP(node);\
	_rhs[nnode] -= conductance * NP(node);\
	}
#define _load_static_jacobian1(node,vnode,conductance)\
	_jstat[node][vnode] += conductance;\
	if (doHB) {\
	_ghs[node] += conductance * NP(vnode);\
        } else {\
	_rhs[node] += conductance * NP(vnode);\
	}
#define _load_dynamic_residual2(pnode,nnode,charge)\
	if (doTR) _charges[pnode][nnode] += charge;\
	if (doHB) {\
	_qhs[pnode] -= charge;\
	_qhs[nnode] += charge;\
	}
#define _load_dynamic_residual1(node,charge)\
	if (doTR) _charges[node][node] += charge;\
	if (doHB) {\
	_qhs[node] -= charge;\
	}
#define _load_dynamic_jacobian4(pnode,nnode,vpnode,vnnode,capacitance)\
	if (doAC) {\
	_jdyna[pnode][vpnode] += capacitance;\
	_jdyna[nnode][vnnode] += capacitance;\
	_jdyna[pnode][vnnode] -= capacitance;\
	_jdyna[nnode][vpnode] -= capacitance;\
	}\
        if (doTR) {\
        _caps[pnode][nnode][vpnode][vnnode] += capacitance;\
  	}\
	if (doHB) {\
	_chs[pnode] += capacitance * BP(vpnode,vnnode);\
	_chs[nnode] -= capacitance * BP(vpnode,vnnode);\
	}
#define _load_dynamic_jacobian2s(pnode,nnode,vnode,capacitance)\
	if (doAC) {\
	_jdyna[pnode][vnode] += capacitance;\
	_jdyna[nnode][vnode] -= capacitance;\
	}\
	if (doTR) {\
	_caps[pnode][nnode][vnode][vnode] += capacitance;\
	}\
	if (doHB) {\
	_chs[pnode] += capacitance * NP(vnode);\
	_chs[nnode] -= capacitance * NP(vnode);\
	}
#define _load_dynamic_jacobian2p(node,vpnode,vnnode,capacitance)\
	if (doAC) {\
	_jdyna[node][vpnode] += capacitance;\
        _jdyna[node][vnnode] -= capacitance;\
        }\
	if (doTR) {\
        _caps[node][node][vpnode][vnnode] += capacitance;\
	}\
	if (doHB) {\
	_chs[node] += capacitance * BP(vpnode,vnnode);\
	}
#define _load_dynamic_jacobian1(node,vnode,capacitance)\
	if (doAC) {\
	_jdyna[node][vnode] += capacitance;\
	}\
	if (doTR) {\
	_caps[node][node][vnode][vnode] += capacitance;\
	}\
	if (doHB) {\
	_chs[node] += capacitance * NP(vnode);\
	}

#define _save_whitenoise1(n1,pwr,type)\
	_white_pwr[n1][n1] += pwr;
#define _save_whitenoise2(n1,n2,pwr,type)\
	_white_pwr[n1][n2] += pwr;
#define _save_flickernoise1(n1,pwr,exp,type)\
	_flicker_pwr[n1][n1] += pwr;\
	_flicker_exp[n1][n1] += exp;
#define _save_flickernoise2(n1,n2,pwr,exp,type)\
	_flicker_pwr[n1][n2] += pwr;\
	_flicker_exp[n1][n2] += exp;
#define _load_whitenoise2(n1,n2,pwr)\
	cy (n1,n2) -= pwr/kB/T0; cy (n2,n1) -= pwr/kB/T0;\
	cy (n1,n1) += pwr/kB/T0; cy (n2,n2) += pwr/kB/T0;
#define _load_whitenoise1(n1,pwr)\
	cy (n1,n1) += pwr/kB/T0;
#define _load_flickernoise2(n1,n2,pwr,exp)\
	cy (n1,n2) -= pwr*pow(_freq,-exp)/kB/T0;\
	cy (n2,n1) -= pwr*pow(_freq,-exp)/kB/T0;\
	cy (n1,n1) += pwr*pow(_freq,-exp)/kB/T0;\
	cy (n2,n2) += pwr*pow(_freq,-exp)/kB/T0;
#define _load_flickernoise1(n1,pwr,exp)\
	cy (n1,n1) += pwr*pow(_freq,-exp)/kB/T0;

// derivative helper macros
#define m00_hypot(v00,x,y)      v00 = xhypot(x,y);
#define m10_hypot(v10,v00,x,y)  v10 = (x)/(v00);
#define m11_hypot(v11,v00,x,y)  v11 = (y)/(v00);
#define m00_max(v00,x,y)        v00 = ((x)>(y))?(x):(y);
#define m10_max(v10,v00,x,y)    v10 = ((x)>(y))?1.0:0.0;
#define m11_max(v11,v00,x,y)    v11 = ((x)>(y))?0.0:1.0;
#define m00_min(v00,x,y)        v00 = ((x)<(y))?(x):(y);
#define m10_min(v10,v00,x,y)    v10 = ((x)<(y))?1.0:0.0;
#define m11_min(v11,v00,x,y)    v11 = ((x)<(y))?0.0:1.0;
#define m00_pow(v00,x,y)        v00 = pow(x,y);
#define m10_pow(v10,v00,x,y)    v10 = (x==0.0)?0.0:(v00)*(y)/(x);
#define m11_pow(v11,v00,x,y)    v11 = (x==0.0)?0.0:(log(x)*(v00));

#define m00_div(v00,v10,x,y)    double v10=1/(y); double v00=(x)*v10;
#define m10_div(v10,v00,vv,x,y)
#define m11_div(v11,v00,vv,x,y) double v11 = -v00*vv;

#define m00_mult(v00,v10,v11,x,y) double v10=(x); double v11=(y); double v00=v10*v11;
#define m00_add(v00,x,y)        double v00=(x)+(y);

#define m00_cos(v00,x)          v00 = cos(x);
#define m10_cos(v10,v00,x)      v10 = (-sin(x));
#define m00_sin(v00,x)          v00 = sin(x);
#define m10_sin(v10,v00,x)      v10 = (cos(x));
#define m00_tan(v00,x)          v00 = tan(x);
#define m10_tan(v10,v00,x)      v10 = (1.0/cos(x)/cos(x));
#define m00_cosh(v00,x)         v00 = cosh(x);
#define m10_cosh(v10,v00,x)     v10 = (sinh(x));
#define m00_sinh(v00,x)         v00 = sinh(x);
#define m10_sinh(v10,v00,x)     v10 = (cosh(x));
#define m00_tanh(v00,x)         v00 = tanh(x);
#define m10_tanh(v10,v00,x)     v10 = (1.0/cosh(x)/cosh(x));
#define m00_acos(v00,x)         v00 = acos(x);
#define m10_acos(v10,v00,x)     v10 = (-1.0/sqrt(1-x*x));
#define m00_asin(v00,x)         v00 = asin(x);
#define m10_asin(v10,v00,x)     v10 = (+1.0/sqrt(1-x*x));
#define m00_atan(v00,x)         v00 = atan(x);
#define m10_atan(v10,v00,x)     v10 = (+1.0/(1+x*x));
#define m00_atanh(v00,x)        v00 = atanh(x);
#define m10_atanh(v10,v00,x)    v10 = (+1.0/(1-x*x));
#define m00_logE(v00,x)         v00 = log(x);
#define m10_logE(v10,v00,x)     v10 = (1.0/x);
#define m00_log10(v00,x)        v00 = log10(x);
#define m10_log10(v10,v00,x)    v10 = (1.0/x/M_LN10);
#define m00_sqrt(v00,x)         v00 = sqrt(x);
#define m10_sqrt(v10,v00,x)     v10 = (0.5/v00);
#define m00_fabs(v00,x)         v00 = fabs(x);
#define m10_fabs(v10,v00,x)     v10 = (((x)>=0)?(+1.0):(-1.0));

#define m00_exp(v00,x)          v00 = exp(x);
#define m10_exp(v10,v00,x)      v10 = v00;

#define m00_abs(v00)            ((v00)<(0)?(-(v00)):(v00))
#define m00_floor(v00,x)        v00 = floor(x);
#define m00_limexp(v00,x)       v00 = ((x)<80.0?exp(x):exp(80.0)*(x-79.0));
#define m10_limexp(v10,v00,x)   v10 = ((x)<80.0?(v00):exp(80.0));

#define m20_logE(v00)           (-1.0/v00/v00)
#define m20_exp(v00)            exp(v00)
#define m20_limexp(v00)         ((v00)<80.0?exp(v00):0.0)
#define m20_sqrt(v00)           (-0.25/(v00)/sqrt(v00))
#define m20_fabs(v00)           0.0
#define m20_pow(x,y)            ((y)*((y)-1.0)*pow(x,y)/(x)/(x))
#define m00_vt(x)               (kBoverQ*(x))
#define m10_vt(x)               (kBoverQ)

// simulator specific definitions
#define _modelname              "bsim3v34pMOS"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
bsim3v34pMOS::bsim3v34pMOS() : circuit (6)
{
  type = CIR_bsim3v34pMOS;
}

/* Initialization of model. */
void bsim3v34pMOS::initModel (void)
{
  // create internal nodes
  setInternalNode (drainp, "drainp");
  setInternalNode (sourcep, "sourcep");

  // get device model parameters
  loadVariables ();
  // evaluate global model equations
  initializeModel ();
  // evaluate initial step equations
  initialStep ();
  // evaluate global instance equations
  initializeInstance ();
}

/* Initialization of DC analysis. */
void bsim3v34pMOS::initDC (void)
{
  allocMatrixMNA ();
  initModel ();
  pol = 1;
  restartDC ();
  doAC = 1;
  doTR = 0;
  doHB = 0;
}

/* Run when DC is restarted (fallback algorithms). */
void bsim3v34pMOS::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void bsim3v34pMOS::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[sourcep][source] = 0.0;
  _white_pwr[drain][drainp] = 0.0;
  _white_pwr[drainp][sourcep] = 0.0;
  _flicker_pwr[drainp][sourcep] = 0.0;
  _flicker_exp[drainp][sourcep] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 6; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 6; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void bsim3v34pMOS::loadVariables (void)
{
  L = getPropertyDouble ("L");
  W = getPropertyDouble ("W");
  PS = getPropertyDouble ("PS");
  PD = getPropertyDouble ("PD");
  AS = getPropertyDouble ("AS");
  AD = getPropertyDouble ("AD");
  NRS = getPropertyDouble ("NRS");
  NRD = getPropertyDouble ("NRD");
  NQSMOD = getPropertyDouble ("NQSMOD");
  GMIN = getPropertyDouble ("GMIN");
  VERSION = getPropertyDouble ("VERSION");
  PARAMCHK = getPropertyDouble ("PARAMCHK");
  MOBMOD = getPropertyDouble ("MOBMOD");
  CAPMOD = getPropertyDouble ("CAPMOD");
  NOIMOD = getPropertyDouble ("NOIMOD");
  BINUNIT = getPropertyDouble ("BINUNIT");
  TOX = getPropertyDouble ("TOX");
  TOXM = getPropertyDouble ("TOXM");
  CDSC = getPropertyDouble ("CDSC");
  CDSCB = getPropertyDouble ("CDSCB");
  CDSCD = getPropertyDouble ("CDSCD");
  CIT = getPropertyDouble ("CIT");
  NFACTOR = getPropertyDouble ("NFACTOR");
  XJ = getPropertyDouble ("XJ");
  VSAT = getPropertyDouble ("VSAT");
  AT = getPropertyDouble ("AT");
  A0 = getPropertyDouble ("A0");
  AGS = getPropertyDouble ("AGS");
  A1 = getPropertyDouble ("A1");
  A2 = getPropertyDouble ("A2");
  KETA = getPropertyDouble ("KETA");
  NSUB = getPropertyDouble ("NSUB");
  NCH = getPropertyDouble ("NCH");
  NGATE = getPropertyDouble ("NGATE");
  GAMMA1 = getPropertyDouble ("GAMMA1");
  GAMMA2 = getPropertyDouble ("GAMMA2");
  VBX = getPropertyDouble ("VBX");
  VBM = getPropertyDouble ("VBM");
  XT = getPropertyDouble ("XT");
  K1 = getPropertyDouble ("K1");
  KT1 = getPropertyDouble ("KT1");
  KT1L = getPropertyDouble ("KT1L");
  KT2 = getPropertyDouble ("KT2");
  K2 = getPropertyDouble ("K2");
  K3 = getPropertyDouble ("K3");
  K3B = getPropertyDouble ("K3B");
  W0 = getPropertyDouble ("W0");
  NLX = getPropertyDouble ("NLX");
  DVT0 = getPropertyDouble ("DVT0");
  DVT1 = getPropertyDouble ("DVT1");
  DVT2 = getPropertyDouble ("DVT2");
  DVT0W = getPropertyDouble ("DVT0W");
  DVT1W = getPropertyDouble ("DVT1W");
  DVT2W = getPropertyDouble ("DVT2W");
  DROUT = getPropertyDouble ("DROUT");
  DSUB = getPropertyDouble ("DSUB");
  VTHO = getPropertyDouble ("VTHO");
  VTH0 = getPropertyDouble ("VTH0");
  UA = getPropertyDouble ("UA");
  UA1 = getPropertyDouble ("UA1");
  UB = getPropertyDouble ("UB");
  UB1 = getPropertyDouble ("UB1");
  UC = getPropertyDouble ("UC");
  UC1 = getPropertyDouble ("UC1");
  U0 = getPropertyDouble ("U0");
  UTE = getPropertyDouble ("UTE");
  VOFF = getPropertyDouble ("VOFF");
  TNOM = getPropertyDouble ("TNOM");
  CGSO = getPropertyDouble ("CGSO");
  CGDO = getPropertyDouble ("CGDO");
  CGBO = getPropertyDouble ("CGBO");
  XPART = getPropertyDouble ("XPART");
  ELM = getPropertyDouble ("ELM");
  DELTA = getPropertyDouble ("DELTA");
  RSH = getPropertyDouble ("RSH");
  RDSW = getPropertyDouble ("RDSW");
  PRWG = getPropertyDouble ("PRWG");
  PRWB = getPropertyDouble ("PRWB");
  PRT = getPropertyDouble ("PRT");
  ETA0 = getPropertyDouble ("ETA0");
  ETAB = getPropertyDouble ("ETAB");
  PCLM = getPropertyDouble ("PCLM");
  PDIBLC1 = getPropertyDouble ("PDIBLC1");
  PDIBLC2 = getPropertyDouble ("PDIBLC2");
  PDIBLCB = getPropertyDouble ("PDIBLCB");
  PSCBE1 = getPropertyDouble ("PSCBE1");
  PSCBE2 = getPropertyDouble ("PSCBE2");
  PVAG = getPropertyDouble ("PVAG");
  JS = getPropertyDouble ("JS");
  JSW = getPropertyDouble ("JSW");
  PB = getPropertyDouble ("PB");
  NJ = getPropertyDouble ("NJ");
  XTI = getPropertyDouble ("XTI");
  MJ = getPropertyDouble ("MJ");
  PBSW = getPropertyDouble ("PBSW");
  MJSW = getPropertyDouble ("MJSW");
  PBSWG = getPropertyDouble ("PBSWG");
  MJSWG = getPropertyDouble ("MJSWG");
  CJ = getPropertyDouble ("CJ");
  VFBCV = getPropertyDouble ("VFBCV");
  VFB = getPropertyDouble ("VFB");
  CJSW = getPropertyDouble ("CJSW");
  CJSWG = getPropertyDouble ("CJSWG");
  TPB = getPropertyDouble ("TPB");
  TCJ = getPropertyDouble ("TCJ");
  TPBSW = getPropertyDouble ("TPBSW");
  TCJSW = getPropertyDouble ("TCJSW");
  TPBSWG = getPropertyDouble ("TPBSWG");
  TCJSWG = getPropertyDouble ("TCJSWG");
  ACDE = getPropertyDouble ("ACDE");
  MOIN = getPropertyDouble ("MOIN");
  NOFF = getPropertyDouble ("NOFF");
  VOFFCV = getPropertyDouble ("VOFFCV");
  LINT = getPropertyDouble ("LINT");
  LL = getPropertyDouble ("LL");
  LLC = getPropertyDouble ("LLC");
  LLN = getPropertyDouble ("LLN");
  LW = getPropertyDouble ("LW");
  LWC = getPropertyDouble ("LWC");
  LWN = getPropertyDouble ("LWN");
  LWL = getPropertyDouble ("LWL");
  LWLC = getPropertyDouble ("LWLC");
  LMIN = getPropertyDouble ("LMIN");
  LMAX = getPropertyDouble ("LMAX");
  WR = getPropertyDouble ("WR");
  WINT = getPropertyDouble ("WINT");
  DWG = getPropertyDouble ("DWG");
  DWB = getPropertyDouble ("DWB");
  WL = getPropertyDouble ("WL");
  WLC = getPropertyDouble ("WLC");
  WLN = getPropertyDouble ("WLN");
  WW = getPropertyDouble ("WW");
  WWC = getPropertyDouble ("WWC");
  WWN = getPropertyDouble ("WWN");
  WWL = getPropertyDouble ("WWL");
  WWLC = getPropertyDouble ("WWLC");
  WMIN = getPropertyDouble ("WMIN");
  WMAX = getPropertyDouble ("WMAX");
  B0 = getPropertyDouble ("B0");
  B1 = getPropertyDouble ("B1");
  CGSL = getPropertyDouble ("CGSL");
  CGDL = getPropertyDouble ("CGDL");
  CKAPPA = getPropertyDouble ("CKAPPA");
  CF = getPropertyDouble ("CF");
  CLC = getPropertyDouble ("CLC");
  CLE = getPropertyDouble ("CLE");
  DWC = getPropertyDouble ("DWC");
  DLC = getPropertyDouble ("DLC");
  ALPHA0 = getPropertyDouble ("ALPHA0");
  ALPHA1 = getPropertyDouble ("ALPHA1");
  BETA0 = getPropertyDouble ("BETA0");
  IJTH = getPropertyDouble ("IJTH");
  LCDSC = getPropertyDouble ("LCDSC");
  LCDSCB = getPropertyDouble ("LCDSCB");
  LCDSCD = getPropertyDouble ("LCDSCD");
  LCIT = getPropertyDouble ("LCIT");
  LNFACTOR = getPropertyDouble ("LNFACTOR");
  LXJ = getPropertyDouble ("LXJ");
  LVSAT = getPropertyDouble ("LVSAT");
  LAT = getPropertyDouble ("LAT");
  LA0 = getPropertyDouble ("LA0");
  LAGS = getPropertyDouble ("LAGS");
  LA1 = getPropertyDouble ("LA1");
  LA2 = getPropertyDouble ("LA2");
  LKETA = getPropertyDouble ("LKETA");
  LNSUB = getPropertyDouble ("LNSUB");
  LNCH = getPropertyDouble ("LNCH");
  LNGATE = getPropertyDouble ("LNGATE");
  LGAMMA1 = getPropertyDouble ("LGAMMA1");
  LGAMMA2 = getPropertyDouble ("LGAMMA2");
  LVBX = getPropertyDouble ("LVBX");
  LVBM = getPropertyDouble ("LVBM");
  LXT = getPropertyDouble ("LXT");
  LK1 = getPropertyDouble ("LK1");
  LKT1 = getPropertyDouble ("LKT1");
  LKT1L = getPropertyDouble ("LKT1L");
  LKT2 = getPropertyDouble ("LKT2");
  LK2 = getPropertyDouble ("LK2");
  LK3 = getPropertyDouble ("LK3");
  LK3B = getPropertyDouble ("LK3B");
  LW0 = getPropertyDouble ("LW0");
  LNLX = getPropertyDouble ("LNLX");
  LDVT0 = getPropertyDouble ("LDVT0");
  LDVT1 = getPropertyDouble ("LDVT1");
  LDVT2 = getPropertyDouble ("LDVT2");
  LDVT0W = getPropertyDouble ("LDVT0W");
  LDVT1W = getPropertyDouble ("LDVT1W");
  LDVT2W = getPropertyDouble ("LDVT2W");
  LDROUT = getPropertyDouble ("LDROUT");
  LDSUB = getPropertyDouble ("LDSUB");
  LVTH0 = getPropertyDouble ("LVTH0");
  LVTHO = getPropertyDouble ("LVTHO");
  LUA = getPropertyDouble ("LUA");
  LUA1 = getPropertyDouble ("LUA1");
  LUB = getPropertyDouble ("LUB");
  LUB1 = getPropertyDouble ("LUB1");
  LUC = getPropertyDouble ("LUC");
  LUC1 = getPropertyDouble ("LUC1");
  LU0 = getPropertyDouble ("LU0");
  LUTE = getPropertyDouble ("LUTE");
  LVOFF = getPropertyDouble ("LVOFF");
  LELM = getPropertyDouble ("LELM");
  LDELTA = getPropertyDouble ("LDELTA");
  LRDSW = getPropertyDouble ("LRDSW");
  LPRWG = getPropertyDouble ("LPRWG");
  LPRWB = getPropertyDouble ("LPRWB");
  LPRT = getPropertyDouble ("LPRT");
  LETA0 = getPropertyDouble ("LETA0");
  LETAB = getPropertyDouble ("LETAB");
  LPCLM = getPropertyDouble ("LPCLM");
  LPDIBLC1 = getPropertyDouble ("LPDIBLC1");
  LPDIBLC2 = getPropertyDouble ("LPDIBLC2");
  LPDIBLCB = getPropertyDouble ("LPDIBLCB");
  LPSCBE1 = getPropertyDouble ("LPSCBE1");
  LPSCBE2 = getPropertyDouble ("LPSCBE2");
  LPVAG = getPropertyDouble ("LPVAG");
  LWR = getPropertyDouble ("LWR");
  LDWG = getPropertyDouble ("LDWG");
  LDWB = getPropertyDouble ("LDWB");
  LB0 = getPropertyDouble ("LB0");
  LB1 = getPropertyDouble ("LB1");
  LCGSL = getPropertyDouble ("LCGSL");
  LCGDL = getPropertyDouble ("LCGDL");
  LCKAPPA = getPropertyDouble ("LCKAPPA");
  LCF = getPropertyDouble ("LCF");
  LCLC = getPropertyDouble ("LCLC");
  LCLE = getPropertyDouble ("LCLE");
  LALPHA0 = getPropertyDouble ("LALPHA0");
  LALPHA1 = getPropertyDouble ("LALPHA1");
  LBETA0 = getPropertyDouble ("LBETA0");
  LVFBCV = getPropertyDouble ("LVFBCV");
  LVFB = getPropertyDouble ("LVFB");
  LACDE = getPropertyDouble ("LACDE");
  LMOIN = getPropertyDouble ("LMOIN");
  LNOFF = getPropertyDouble ("LNOFF");
  LVOFFCV = getPropertyDouble ("LVOFFCV");
  WCDSC = getPropertyDouble ("WCDSC");
  WCDSCB = getPropertyDouble ("WCDSCB");
  WCDSCD = getPropertyDouble ("WCDSCD");
  WCIT = getPropertyDouble ("WCIT");
  WNFACTOR = getPropertyDouble ("WNFACTOR");
  WXJ = getPropertyDouble ("WXJ");
  WVSAT = getPropertyDouble ("WVSAT");
  WAT = getPropertyDouble ("WAT");
  WA0 = getPropertyDouble ("WA0");
  WAGS = getPropertyDouble ("WAGS");
  WA1 = getPropertyDouble ("WA1");
  WA2 = getPropertyDouble ("WA2");
  WKETA = getPropertyDouble ("WKETA");
  WNSUB = getPropertyDouble ("WNSUB");
  WNCH = getPropertyDouble ("WNCH");
  WNGATE = getPropertyDouble ("WNGATE");
  WGAMMA1 = getPropertyDouble ("WGAMMA1");
  WGAMMA2 = getPropertyDouble ("WGAMMA2");
  WVBX = getPropertyDouble ("WVBX");
  WVBM = getPropertyDouble ("WVBM");
  WXT = getPropertyDouble ("WXT");
  WK1 = getPropertyDouble ("WK1");
  WKT1 = getPropertyDouble ("WKT1");
  WKT1L = getPropertyDouble ("WKT1L");
  WKT2 = getPropertyDouble ("WKT2");
  WK2 = getPropertyDouble ("WK2");
  WK3 = getPropertyDouble ("WK3");
  WK3B = getPropertyDouble ("WK3B");
  WW0 = getPropertyDouble ("WW0");
  WNLX = getPropertyDouble ("WNLX");
  WDVT0 = getPropertyDouble ("WDVT0");
  WDVT1 = getPropertyDouble ("WDVT1");
  WDVT2 = getPropertyDouble ("WDVT2");
  WDVT0W = getPropertyDouble ("WDVT0W");
  WDVT1W = getPropertyDouble ("WDVT1W");
  WDVT2W = getPropertyDouble ("WDVT2W");
  WDROUT = getPropertyDouble ("WDROUT");
  WDSUB = getPropertyDouble ("WDSUB");
  WVTH0 = getPropertyDouble ("WVTH0");
  WVTHO = getPropertyDouble ("WVTHO");
  WUA = getPropertyDouble ("WUA");
  WUA1 = getPropertyDouble ("WUA1");
  WUB = getPropertyDouble ("WUB");
  WUB1 = getPropertyDouble ("WUB1");
  WUC = getPropertyDouble ("WUC");
  WUC1 = getPropertyDouble ("WUC1");
  WU0 = getPropertyDouble ("WU0");
  WUTE = getPropertyDouble ("WUTE");
  WVOFF = getPropertyDouble ("WVOFF");
  WELM = getPropertyDouble ("WELM");
  WDELTA = getPropertyDouble ("WDELTA");
  WRDSW = getPropertyDouble ("WRDSW");
  WPRWG = getPropertyDouble ("WPRWG");
  WPRWB = getPropertyDouble ("WPRWB");
  WPRT = getPropertyDouble ("WPRT");
  WETA0 = getPropertyDouble ("WETA0");
  WETAB = getPropertyDouble ("WETAB");
  WPCLM = getPropertyDouble ("WPCLM");
  WPDIBLC1 = getPropertyDouble ("WPDIBLC1");
  WPDIBLC2 = getPropertyDouble ("WPDIBLC2");
  WPDIBLCB = getPropertyDouble ("WPDIBLCB");
  WPSCBE1 = getPropertyDouble ("WPSCBE1");
  WPSCBE2 = getPropertyDouble ("WPSCBE2");
  WPVAG = getPropertyDouble ("WPVAG");
  WWR = getPropertyDouble ("WWR");
  WDWG = getPropertyDouble ("WDWG");
  WDWB = getPropertyDouble ("WDWB");
  WB0 = getPropertyDouble ("WB0");
  WB1 = getPropertyDouble ("WB1");
  WCGSL = getPropertyDouble ("WCGSL");
  WCGDL = getPropertyDouble ("WCGDL");
  WCKAPPA = getPropertyDouble ("WCKAPPA");
  WCF = getPropertyDouble ("WCF");
  WCLC = getPropertyDouble ("WCLC");
  WCLE = getPropertyDouble ("WCLE");
  WALPHA0 = getPropertyDouble ("WALPHA0");
  WALPHA1 = getPropertyDouble ("WALPHA1");
  WBETA0 = getPropertyDouble ("WBETA0");
  WVFBCV = getPropertyDouble ("WVFBCV");
  WVFB = getPropertyDouble ("WVFB");
  WACDE = getPropertyDouble ("WACDE");
  WMOIN = getPropertyDouble ("WMOIN");
  WNOFF = getPropertyDouble ("WNOFF");
  WVOFFCV = getPropertyDouble ("WVOFFCV");
  PCDSC = getPropertyDouble ("PCDSC");
  PCDSCB = getPropertyDouble ("PCDSCB");
  PCDSCD = getPropertyDouble ("PCDSCD");
  PCIT = getPropertyDouble ("PCIT");
  PNFACTOR = getPropertyDouble ("PNFACTOR");
  PXJ = getPropertyDouble ("PXJ");
  PVSAT = getPropertyDouble ("PVSAT");
  PAT = getPropertyDouble ("PAT");
  PA0 = getPropertyDouble ("PA0");
  PAGS = getPropertyDouble ("PAGS");
  PA1 = getPropertyDouble ("PA1");
  PA2 = getPropertyDouble ("PA2");
  PKETA = getPropertyDouble ("PKETA");
  PNSUB = getPropertyDouble ("PNSUB");
  PNCH = getPropertyDouble ("PNCH");
  PNGATE = getPropertyDouble ("PNGATE");
  PGAMMA1 = getPropertyDouble ("PGAMMA1");
  PGAMMA2 = getPropertyDouble ("PGAMMA2");
  PVBX = getPropertyDouble ("PVBX");
  PVBM = getPropertyDouble ("PVBM");
  PXT = getPropertyDouble ("PXT");
  PK1 = getPropertyDouble ("PK1");
  PKT1 = getPropertyDouble ("PKT1");
  PKT1L = getPropertyDouble ("PKT1L");
  PKT2 = getPropertyDouble ("PKT2");
  PK2 = getPropertyDouble ("PK2");
  PK3 = getPropertyDouble ("PK3");
  PK3B = getPropertyDouble ("PK3B");
  PW0 = getPropertyDouble ("PW0");
  PNLX = getPropertyDouble ("PNLX");
  PDVT0 = getPropertyDouble ("PDVT0");
  PDVT1 = getPropertyDouble ("PDVT1");
  PDVT2 = getPropertyDouble ("PDVT2");
  PDVT0W = getPropertyDouble ("PDVT0W");
  PDVT1W = getPropertyDouble ("PDVT1W");
  PDVT2W = getPropertyDouble ("PDVT2W");
  PDROUT = getPropertyDouble ("PDROUT");
  PDSUB = getPropertyDouble ("PDSUB");
  PVTH0 = getPropertyDouble ("PVTH0");
  PVTHO = getPropertyDouble ("PVTHO");
  PUA = getPropertyDouble ("PUA");
  PUA1 = getPropertyDouble ("PUA1");
  PUB = getPropertyDouble ("PUB");
  PUB1 = getPropertyDouble ("PUB1");
  PUC = getPropertyDouble ("PUC");
  PUC1 = getPropertyDouble ("PUC1");
  PU0 = getPropertyDouble ("PU0");
  PUTE = getPropertyDouble ("PUTE");
  PVOFF = getPropertyDouble ("PVOFF");
  PELM = getPropertyDouble ("PELM");
  PDELTA = getPropertyDouble ("PDELTA");
  PRDSW = getPropertyDouble ("PRDSW");
  PPRWG = getPropertyDouble ("PPRWG");
  PPRWB = getPropertyDouble ("PPRWB");
  PPRT = getPropertyDouble ("PPRT");
  PETA0 = getPropertyDouble ("PETA0");
  PETAB = getPropertyDouble ("PETAB");
  PPCLM = getPropertyDouble ("PPCLM");
  PPDIBLC1 = getPropertyDouble ("PPDIBLC1");
  PPDIBLC2 = getPropertyDouble ("PPDIBLC2");
  PPDIBLCB = getPropertyDouble ("PPDIBLCB");
  PPSCBE1 = getPropertyDouble ("PPSCBE1");
  PPSCBE2 = getPropertyDouble ("PPSCBE2");
  PPVAG = getPropertyDouble ("PPVAG");
  PWR = getPropertyDouble ("PWR");
  PDWG = getPropertyDouble ("PDWG");
  PDWB = getPropertyDouble ("PDWB");
  PB0 = getPropertyDouble ("PB0");
  PB1 = getPropertyDouble ("PB1");
  PCGSL = getPropertyDouble ("PCGSL");
  PCGDL = getPropertyDouble ("PCGDL");
  PCKAPPA = getPropertyDouble ("PCKAPPA");
  PCF = getPropertyDouble ("PCF");
  PCLC = getPropertyDouble ("PCLC");
  PCLE = getPropertyDouble ("PCLE");
  PALPHA0 = getPropertyDouble ("PALPHA0");
  PALPHA1 = getPropertyDouble ("PALPHA1");
  PBETA0 = getPropertyDouble ("PBETA0");
  PVFBCV = getPropertyDouble ("PVFBCV");
  PVFB = getPropertyDouble ("PVFB");
  PACDE = getPropertyDouble ("PACDE");
  PMOIN = getPropertyDouble ("PMOIN");
  PNOFF = getPropertyDouble ("PNOFF");
  PVOFFCV = getPropertyDouble ("PVOFFCV");
  KF = getPropertyDouble ("KF");
  AF = getPropertyDouble ("AF");
  EF = getPropertyDouble ("EF");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void bsim3v34pMOS::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void bsim3v34pMOS::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void bsim3v34pMOS::initialStep (void)
{
#if defined(_DERIVATE)
double tmp3_Vdrainp_sourcep;
double tmp3_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T5_Vgate_sourcep;
double T5_Vdrainp_sourcep;
double T5_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T4_Vgate_sourcep;
double T4_Vdrainp_sourcep;
double T4_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp_Vgate_sourcep;
double tmp_Vdrainp_sourcep;
double tmp_Vbulk_sourcep;
#endif
double tconst;
int Fatal_Flag;
double cle;
double clc;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
double elm;
double wr;
double prt;
double rdsw;
double ute;
double ub1;
double ua1;
double dsub;
double drout;
double vbm;
double at;
double vsat;
double Inv_LW;
double Inv_W;
double Inv_L;
double dwc;
double dw;
#if defined(_DERIVATE)
double T3_Vgate_sourcep;
double T3_Vdrainp_sourcep;
double T3_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T2_Vgate_sourcep;
double T2_Vdrainp_sourcep;
double T2_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp2_Vgate_sourcep;
double tmp2_Vdrainp_sourcep;
double tmp2_Vbulk_sourcep;
#endif
double dl;
#if defined(_DERIVATE)
double tmp1_Vdrainp_sourcep;
double tmp1_Vbulk_sourcep;
#endif
double Wdrn;
double Ldrn;
double delTemp;
double jctSidewallTempSatCurDensity;
double jctTempSatCurDensity;
#if defined(_DERIVATE)
double T1_Vgate_sourcep;
double T1_Vdrainp_sourcep;
double T1_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T_0_Vgate_sourcep;
double T_0_Vdrainp_sourcep;
double T_0_Vbulk_sourcep;
#endif
double Eg;
double ni;
double Eg0;
double Vtm0;
double TRatio;
double Tnom;
double ppdiblb;
double ppdibl2;
double ppdibl1;
double pvth0;
double pngate;
double pnpeak;
double wpdiblb;
double wpdibl2;
double wpdibl1;
double wvth0;
double wngate;
double wnpeak;
double lpdiblb;
double lpdibl2;
double lpdibl1;
double lvth0;
double lngate;
double lnpeak;
#if defined(_DYNAMIC)
double cf;
#endif
double sourceSquares;
double drainSquares;
double sheetResistance;
double GatesidewallJctPotential;
double sidewallJctPotential;
double jctTempExponent;
double jctEmissionCoeff;
double bulkJctPotential;
double jctSidewallSatCurDensity;
double jctSatCurDensity;
double pdibl2;
double pdibl1;
double dlc;
double dlcGiven;
double u0;
double uc1;
double vth0Given;
double pk2;
double wk2;
double lk2;
double k2Given;
double k2;
double pk1;
double wk1;
double lk1;
double k1Given;
double vfbGiven;
double pvbx;
double wvbx;
double lvbx;
double vbxGiven;
double vbx;
double pgamma2;
double wgamma2;
double lgamma2;
double gamma2Given;
double gamma2;
double pgamma1;
double wgamma1;
double lgamma1;
double gamma1Given;
double gamma1;
double npeakGiven;
double npeak;
double xtGiven;
double xt;
double nsubGiven;
double nsub;
{
tox=TOX;
cox=(3.453133e-11/tox);
TYPE=(-1.0);
if
((NSUB==(-99.0)))
{
nsub=6.0e16;
nsubGiven=0;
}
else
{
nsub=NSUB;
nsubGiven=1;
}
if
((XT==(-99.0)))
{
xt=1.55e-7;
xtGiven=0;
}
else
{
xt=XT;
xtGiven=1;
}
if
((NCH==(-99.0)))
{
npeak=1.7e17;
npeakGiven=0;
}
else
{
npeak=NCH;
npeakGiven=1;
}
if
((GAMMA1==(-99.0)))
{
gamma1=0.0;
gamma1Given=0;
}
else
{
gamma1=GAMMA1;
gamma1Given=1;
}
if
((LGAMMA1==(-99.0)))
{
lgamma1=0.0;
}
else
{
lgamma1=LGAMMA1;
}
if
((WGAMMA1==(-99.0)))
{
wgamma1=0.0;
}
else
{
wgamma1=WGAMMA1;
}
if
((PGAMMA1==(-99.0)))
{
pgamma1=0.0;
}
else
{
pgamma1=PGAMMA1;
}
if
((GAMMA2==(-99.0)))
{
gamma2=0.0;
gamma2Given=0;
}
else
{
gamma2=GAMMA2;
gamma2Given=1;
}
if
((LGAMMA2==(-99.0)))
{
lgamma2=0.0;
}
else
{
lgamma2=LGAMMA2;
}
if
((WGAMMA2==(-99.0)))
{
wgamma2=0.0;
}
else
{
wgamma2=WGAMMA2;
}
if
((PGAMMA2==(-99.0)))
{
pgamma2=0.0;
}
else
{
pgamma2=PGAMMA2;
}
if
((VBX==(-99.0)))
{
vbx=0.0;
vbxGiven=0.0;
}
else
{
vbx=VBX;
vbxGiven=1;
}
if
((LVBX==(-99.0)))
{
lvbx=0.0;
}
else
{
lvbx=LVBX;
}
if
((WVBX==(-99.0)))
{
wvbx=0.0;
}
else
{
wvbx=WVBX;
}
if
((PVBX==(-99.0)))
{
pvbx=0.0;
}
else
{
pvbx=PVBX;
}
if
((VFB==(-99.0)))
{
vfb=0.0;
vfbGiven=0.0;
}
else
{
vfb=VFB;
vfbGiven=1.0;
}
if
((K1==(-99.0)))
{
k1=0.0;
k1Given=0.0;
}
else
{
k1=K1;
k1Given=1.0;
}
if
((LK1==(-99.0)))
{
lk1=0.0;
}
else
{
lk1=LK1;
}
if
((WK1==(-99.0)))
{
wk1=0.0;
}
else
{
wk1=WK1;
}
if
((PK1==(-99.0)))
{
pk1=0.0;
}
else
{
pk1=PK1;
}
if
((K2==(-99.0)))
{
k2=0.0;
k2Given=0.0;
}
else
{
k2=K2;
k2Given=1.0;
}
if
((LK2==(-99.0)))
{
lk2=0.0;
}
else
{
lk2=LK2;
}
if
((WK2==(-99.0)))
{
wk2=0.0;
}
else
{
wk2=WK2;
}
if
((PK2==(-99.0)))
{
pk2=0.0;
}
else
{
pk2=PK2;
}
if
((NCH>1.0e20))
{
npeak=(npeak*1.0e-6);
}
else
{
npeak=npeak;
}
if
((NGATE>1.0e23))
{
ngate=(NGATE*1.0e-6);
}
else
{
ngate=NGATE;
}
if
(((VTHO==(-99.0))&&(VTH0==(-99.0))))
{
vth0=((TYPE>0.0)?0.7:(-0.7));
vth0Given=0;
}
else
{
vth0Given=1;
if
((VTH0!=(-99)))
{
vth0=VTHO;
}
if
((VTHO!=(-99)))
{
vth0=VTH0;
}
}
if
((UC==(-99.0)))
{
uc=((MOBMOD==3)?(-0.0465):(-0.0465e-9));
}
else
{
uc=UC;
}
if
((UC1==(-99.0)))
{
uc1=((MOBMOD==3)?(-0.056):(-0.056e-9));
}
else
{
uc1=UC1;
}
if
((U0==(-99.0)))
{
u0=((TYPE==1)?0.067:0.025);
}
else
{
u0=U0;
}
if
((TNOM==(-99.0)))
{
tnom=300.15;
}
else
{
tnom=(TNOM+273.15);
}
if
((DLC==(-99.0)))
{
dlcGiven=0;
dlc=LINT;
}
else
{
dlcGiven=1;
dlc=DLC;
}
if
((CGSO==(-99.0)))
{
if
((dlcGiven&&(dlc>0.0)))
{
cgso_param=((dlc*cox)-CGSL);
}
else
{
cgso_param=((0.6*XJ)*cox);
}
}
else
{
cgso_param=CGSO;
}
if
((CGDO==(-99.0)))
{
if
((dlcGiven&&(dlc>0.0)))
{
cgdo_param=((dlc*cox)-CGDL);
}
else
{
cgdo_param=((0.6*XJ)*cox);
}
}
else
{
cgdo_param=CGDO;
}
if
((CGBO==(-99.0)))
{
cgbo_param=((2.0*DWC)*cox);
}
else
{
cgbo_param=CGBO;
}
pdibl1=PDIBLC1;
pdibl2=PDIBLC2;
pdiblb=PDIBLCB;
jctSatCurDensity=JS;
jctSidewallSatCurDensity=JSW;
bulkJctPotential=PB;
jctEmissionCoeff=NJ;
jctTempExponent=XTI;
bulkJctBotGradingCoeff=MJ;
sidewallJctPotential=PBSW;
bulkJctSideGradingCoeff=MJSW;
GatesidewallJctPotential=PBSWG;
bulkJctGateSideGradingCoeff=MJSWG;
unitAreaJctCap=CJ;
unitLengthSidewallJctCap=CJSW;
unitLengthGateSidewallJctCap=CJSWG;
sheetResistance=RSH;
drainSquares=NRD;
sourceSquares=NRS;
drainArea=AD;
sourceArea=AS;
drainPerimeter=PD;
sourcePerimeter=PS;
#ifdef _DYNAMIC
if
((CF==(-99.0)))
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+(0.4e-6/tox)))
cf=(((2.0*3.453133e-11)/3.141592654)*d00_logE0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
cf=CF;
#endif
}
#endif /* if (...) */
if
((LNCH>1.0e20))
{
lnpeak=(LNCH*1.0e-6);
}
else
{
lnpeak=LNCH;
}
if
((LNGATE>1.0e23))
{
lngate=(LNGATE*1.0e-6);
}
else
{
lngate=LNGATE;
}
if
((LVTH0!=0.0))
{
lvth0=LVTH0;
}
if
((LVTHO!=0.0))
{
lvth0=LVTHO;
}
lpdibl1=LPDIBLC1;
lpdibl2=LPDIBLC2;
lpdiblb=LPDIBLCB;
if
((WNCH>1.0e20))
{
wnpeak=(WNCH*1.0e-6);
}
else
{
wnpeak=WNCH;
}
if
((WNGATE>1.0e23))
{
wngate=(WNGATE*1.0e-6);
}
else
{
wngate=WNGATE;
}
if
((WVTH0!=0.0))
{
wvth0=WVTH0;
}
if
((WVTHO!=0.0))
{
wvth0=WVTHO;
}
wpdibl1=WPDIBLC1;
wpdibl2=WPDIBLC2;
wpdiblb=WPDIBLCB;
if
((PNCH>1.0e20))
{
pnpeak=(PNCH*1.0e-6);
}
else
{
pnpeak=PNCH;
}
if
((PNGATE>1.0e20))
{
pngate=(PNGATE*1.0e-6);
}
else
{
pngate=PNGATE;
}
if
((PVTH0!=0.0))
{
pvth0=PVTH0;
}
if
((PVTHO!=0.0))
{
pvth0=PVTHO;
}
ppdibl1=PPDIBLC1;
ppdibl2=PPDIBLC2;
ppdiblb=PPDIBLCB;
if
((bulkJctPotential<0.1))
{
bulkJctPotential=0.1;
logprint(LOG_ERROR,"Given pb is less than 0.1. Pb is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((sidewallJctPotential<0.1))
{
sidewallJctPotential=0.1;
logprint(LOG_ERROR,"Given pbsw is less than 0.1. Pbsw is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((GatesidewallJctPotential<0.1))
{
GatesidewallJctPotential=0.1;
logprint(LOG_ERROR,"Given pbswg is less than 0.1. Pbswg is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
Temp=_circuit_temp;
Tnom=tnom;
TRatio=(Temp/Tnom);
{
double m00_sqrt(d00_sqrt0,((1.03594e-10/3.453133e-11)*tox))
factor1=d00_sqrt0;
}
Vtm0=(8.617087e-5*Tnom);
Eg0=(1.16-(((7.02e-4*Tnom)*Tnom)/(Tnom+1108.0)));
{
double m00_sqrt(d00_sqrt0,(Tnom/300.15))
double m00_exp(d00_exp1,(21.5565981-(Eg0/(2.0*Vtm0))))
ni=(((1.45e10*(Tnom/300.15))*d00_sqrt0)*d00_exp1);
}
vtm=(8.617087e-5*Temp);
Eg=(1.16-(((7.02e-4*Temp)*Temp)/(Temp+1108.0)));
if
((Temp!=Tnom))
{
{
double m00_logE(d00_logE0,(Temp/Tnom))
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(((Eg0/Vtm0)-(Eg/vtm))+(jctTempExponent*d00_logE0));
}
{
double m00_exp(d00_exp0,(T_0/jctEmissionCoeff))
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_exp0;
}
jctTempSatCurDensity=(jctSatCurDensity*T1);
jctSidewallTempSatCurDensity=(jctSidewallSatCurDensity*T1);
}
else
{
jctTempSatCurDensity=jctSatCurDensity;
jctSidewallTempSatCurDensity=jctSidewallSatCurDensity;
}
if
((jctTempSatCurDensity<0.0))
{
jctTempSatCurDensity=0.0;
}
if
((jctSidewallTempSatCurDensity<0.0))
{
jctSidewallTempSatCurDensity=0.0;
}
delTemp=(_circuit_temp-tnom);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(TCJ*delTemp);
if
((T_0>=(-1.0)))
{
if
((VERSION==3.24))
{
unitAreaTempJctCap=(unitAreaJctCap*(1.0+T_0));
}
else
{
unitAreaJctCap=(unitAreaJctCap*(1.0+T_0));
}
}
else
{
if
((unitAreaJctCap>0.0))
{
if
((VERSION==3.24))
{
unitAreaTempJctCap=0.0;
}
else
{
unitAreaJctCap=0.0;
}
logprint(LOG_ERROR,"Temperature effect has caused cj to be negative. Cj is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(TCJSW*delTemp);
if
((T_0>=(-1.0)))
{
if
((VERSION==3.24))
{
unitLengthSidewallTempJctCap=(unitLengthSidewallJctCap*(1.0+T_0));
}
else
{
unitLengthSidewallJctCap=(unitLengthSidewallJctCap*(1.0+T_0));
}
}
else
{
if
((unitLengthSidewallJctCap>0.0))
{
if
((VERSION==3.24))
{
unitLengthSidewallTempJctCap=0.0;
}
else
{
unitLengthSidewallJctCap=0.0;
}
logprint(LOG_ERROR,"Temperature effect has caused cjsw to be negative. Cjsw is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(TCJSWG*delTemp);
if
((T_0>=(-1.0)))
{
if
((VERSION==3.24))
{
unitLengthGateSidewallTempJctCap=(unitLengthGateSidewallJctCap*(1.0+T_0));
}
else
{
unitLengthGateSidewallJctCap=(unitLengthGateSidewallJctCap*(1.0+T_0));
}
}
else
{
if
((unitLengthGateSidewallJctCap>0.0))
{
if
((VERSION==3.24))
{
unitLengthGateSidewallTempJctCap=0.0;
}
else
{
unitLengthGateSidewallJctCap=0.0;
}
logprint(LOG_ERROR,"Temperature effect has caused cjswg to be negative. Cjswg is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
}
PhiB=(bulkJctPotential-(TPB*delTemp));
if
((PhiB<0.01))
{
PhiB=0.01;
logprint(LOG_ERROR,"Temperature effect has caused pb to be less than 0.01. Pb is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
PhiBSW=(sidewallJctPotential-(TPBSW*delTemp));
if
((PhiBSW<=0.01))
{
PhiBSW=0.01;
logprint(LOG_ERROR,"Temperature effect has caused pbsw to be less than 0.01. Pbsw is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
PhiBSWG=(GatesidewallJctPotential-(TPBSWG*delTemp));
if
((PhiBSWG<=0.01))
{
PhiBSWG=0.01;
logprint(LOG_ERROR,"Temperature effect has caused pbswg to be less than 0.01. Pbswg is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
Ldrn=L;
Wdrn=W;
{
double m00_pow(d00_pow0,Ldrn,LLN)
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=d00_pow0;
}
{
double m00_pow(d00_pow0,Wdrn,LWN)
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vbulk_sourcep=0.0;
#endif
tmp1=(((LL/T_0)+(LW/T1))+(LWL/(T_0*T1)));
dl=(LINT+tmp1);
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vbulk_sourcep=0.0;
#endif
tmp2=(((LLC/T_0)+(LWC/T1))+(LWLC/(T_0*T1)));
dlc=(dlc+tmp2);
{
double m00_pow(d00_pow0,Ldrn,WLN)
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=d00_pow0;
}
{
double m00_pow(d00_pow0,Wdrn,WWN)
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vbulk_sourcep=0.0;
#endif
tmp1=(((WL/T2)+(WW/T3))+(WWL/(T2*T3)));
dw=(WINT+tmp1);
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vbulk_sourcep=0.0;
#endif
tmp2=(((WLC/T2)+(WWC/T3))+(WWLC/(T2*T3)));
dwc=(DWC+tmp2);
leff=(L-(2.0*dl));
if
((leff<=0.0))
{
logprint(LOG_ERROR,"BSIM3: device %m: Effective channel length <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
weff=(W-(2.0*dw));
if
((leff<=0.0))
{
logprint(LOG_ERROR,"BSIM3: device %m: Effective channel width <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
leffCV=(L-(2.0*dlc));
if
((leffCV<=0.0))
{
logprint(LOG_ERROR,"BSIM3: device %m: Effective channel length for C-V <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
weffCV=(W-(2.0*dwc));
if
((weffCV<=0.0))
{
logprint(LOG_ERROR,"BSIM3: device %m: Effective channel width for C-V <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((BINUNIT==1))
{
Inv_L=(1.0e-6/leff);
Inv_W=(1.0e-6/weff);
Inv_LW=(1.0e-12/(leff*weff));
}
else
{
Inv_L=(1.0/leff);
Inv_W=(1.0/weff);
Inv_LW=(1.0/(leff*weff));
}
cdsc=(((CDSC+(LCDSC*Inv_L))+(WCDSC*Inv_W))+(PCDSC*Inv_LW));
cdscb=(((CDSCB+(LCDSCB*Inv_L))+(WCDSCB*Inv_W))+(PCDSCB*Inv_LW));
cdscd=(((CDSCD+(LCDSCD*Inv_L))+(WCDSCD*Inv_W))+(PCDSCD*Inv_LW));
cit=(((CIT+(LCIT*Inv_L))+(WCIT*Inv_W))+(PCIT*Inv_LW));
nfactor=(((NFACTOR+(LNFACTOR*Inv_L))+(WNFACTOR*Inv_W))+(PNFACTOR*Inv_LW));
xj=(((XJ+(LXJ*Inv_L))+(WXJ*Inv_W))+(PXJ*Inv_LW));
vsat=(((VSAT+(LVSAT*Inv_L))+(WVSAT*Inv_W))+(PVSAT*Inv_LW));
at=(((AT+(LAT*Inv_L))+(WAT*Inv_W))+(PAT*Inv_LW));
a0=(((A0+(LA0*Inv_L))+(WA0*Inv_W))+(PA0*Inv_LW));
ags=(((AGS+(LAGS*Inv_L))+(WAGS*Inv_W))+(PAGS*Inv_LW));
a1=(((A1+(LA1*Inv_L))+(WA1*Inv_W))+(PA1*Inv_LW));
a2=(((A2+(LA2*Inv_L))+(WA2*Inv_W))+(PA2*Inv_LW));
keta=(((KETA+(LKETA*Inv_L))+(WKETA*Inv_W))+(PKETA*Inv_LW));
nsub=(((nsub+(LNSUB*Inv_L))+(WNSUB*Inv_W))+(PNSUB*Inv_LW));
npeak=(((npeak+(lnpeak*Inv_L))+(wnpeak*Inv_W))+(pnpeak*Inv_LW));
ngate=(((ngate+(lngate*Inv_L))+(wngate*Inv_W))+(pngate*Inv_LW));
gamma1=(((gamma1+(lgamma1*Inv_L))+(wgamma1*Inv_W))+(pgamma1*Inv_LW));
gamma2=(((gamma2+(lgamma2*Inv_L))+(wgamma2*Inv_W))+(pgamma2*Inv_LW));
vbx=(((vbx+(lvbx*Inv_L))+(wvbx*Inv_W))+(pvbx*Inv_LW));
vbm=(((VBM+(LVBM*Inv_L))+(WVBM*Inv_W))+(PVBM*Inv_LW));
xt=(((xt+(LXT*Inv_L))+(WXT*Inv_W))+(PXT*Inv_LW));
vfb=(((vfb+(LVFB*Inv_L))+(WVFB*Inv_W))+(PVFB*Inv_LW));
k1=(((k1+(lk1*Inv_L))+(wk1*Inv_W))+(pk1*Inv_LW));
kt1=(((KT1+(LKT1*Inv_L))+(WKT1*Inv_W))+(PKT1*Inv_LW));
kt1l=(((KT1L+(LKT1L*Inv_L))+(WKT1L*Inv_W))+(PKT1L*Inv_LW));
k2=(((k2+(lk2*Inv_L))+(wk2*Inv_W))+(pk2*Inv_LW));
kt2=(((KT2+(LKT2*Inv_L))+(WKT2*Inv_W))+(PKT2*Inv_LW));
k3=(((K3+(LK3*Inv_L))+(WK3*Inv_W))+(PK3*Inv_LW));
k3b=(((K3B+(LK3B*Inv_L))+(WK3B*Inv_W))+(PK3B*Inv_LW));
w0=(((W0+(LW0*Inv_L))+(WW0*Inv_W))+(PW0*Inv_LW));
nlx=(((NLX+(LNLX*Inv_L))+(WNLX*Inv_W))+(PNLX*Inv_LW));
dvt0=(((DVT0+(LDVT0*Inv_L))+(WDVT0*Inv_W))+(PDVT0*Inv_LW));
dvt1=(((DVT1+(LDVT1*Inv_L))+(WDVT1*Inv_W))+(PDVT1*Inv_LW));
dvt2=(((DVT2+(LDVT2*Inv_L))+(WDVT2*Inv_W))+(PDVT2*Inv_LW));
dvt0w=(((DVT0W+(LDVT0W*Inv_L))+(WDVT0W*Inv_W))+(PDVT0W*Inv_LW));
dvt1w=(((DVT1W+(LDVT1W*Inv_L))+(WDVT1W*Inv_W))+(PDVT1W*Inv_LW));
dvt2w=(((DVT2W+(LDVT2W*Inv_L))+(WDVT2W*Inv_W))+(PDVT2W*Inv_LW));
drout=(((DROUT+(LDROUT*Inv_L))+(WDROUT*Inv_W))+(PDROUT*Inv_LW));
dsub=(((DSUB+(LDSUB*Inv_L))+(WDSUB*Inv_W))+(PDSUB*Inv_LW));
vth0=(((vth0+(lvth0*Inv_L))+(wvth0*Inv_W))+(pvth0*Inv_LW));
ua=(((UA+(LUA*Inv_L))+(WUA*Inv_W))+(PUA*Inv_LW));
ua1=(((UA1+(LUA1*Inv_L))+(WUA1*Inv_W))+(PUA1*Inv_LW));
ub=(((UB+(LUB*Inv_L))+(WUB*Inv_W))+(PUB*Inv_LW));
ub1=(((UB1+(LUB1*Inv_L))+(WUB1*Inv_W))+(PUB1*Inv_LW));
uc=(((uc+(LUC*Inv_L))+(WUC*Inv_W))+(PUC*Inv_LW));
uc1=(((uc1+(LUC1*Inv_L))+(WUC1*Inv_W))+(PUC1*Inv_LW));
u0=(((u0+(LU0*Inv_L))+(WU0*Inv_W))+(PU0*Inv_LW));
ute=(((UTE+(LUTE*Inv_L))+(WUTE*Inv_W))+(PUTE*Inv_LW));
voff=(((VOFF+(LVOFF*Inv_L))+(WVOFF*Inv_W))+(PVOFF*Inv_LW));
delta=(((DELTA+(LDELTA*Inv_L))+(WDELTA*Inv_W))+(PDELTA*Inv_LW));
rdsw=(((RDSW+(LRDSW*Inv_L))+(WRDSW*Inv_W))+(PRDSW*Inv_LW));
prwg=(((PRWG+(LPRWG*Inv_L))+(WPRWG*Inv_W))+(PPRWG*Inv_LW));
prwb=(((PRWB+(LPRWB*Inv_L))+(WPRWB*Inv_W))+(PPRWB*Inv_LW));
prt=(((PRT+(LPRT*Inv_L))+(WPRT*Inv_W))+(PPRT*Inv_LW));
eta0=(((ETA0+(LETA0*Inv_L))+(WETA0*Inv_W))+(PETA0*Inv_LW));
etab=(((ETAB+(LETAB*Inv_L))+(WETAB*Inv_W))+(PETAB*Inv_LW));
pclm=(((PCLM+(LPCLM*Inv_L))+(WPCLM*Inv_W))+(PPCLM*Inv_LW));
pdibl1=(((pdibl1+(lpdibl1*Inv_L))+(wpdibl1*Inv_W))+(ppdibl1*Inv_LW));
pdibl2=(((pdibl2+(lpdibl2*Inv_L))+(wpdibl2*Inv_W))+(ppdibl2*Inv_LW));
pdiblb=(((pdiblb+(lpdiblb*Inv_L))+(wpdiblb*Inv_W))+(ppdiblb*Inv_LW));
pscbe1=(((PSCBE1+(LPSCBE1*Inv_L))+(WPSCBE1*Inv_W))+(PPSCBE1*Inv_LW));
pscbe2=(((PSCBE2+(LPSCBE2*Inv_L))+(WPSCBE2*Inv_W))+(PPSCBE2*Inv_LW));
pvag=(((PVAG+(LPVAG*Inv_L))+(WPVAG*Inv_W))+(PPVAG*Inv_LW));
wr=(((WR+(LWR*Inv_L))+(WWR*Inv_W))+(PWR*Inv_LW));
dwg=(((DWG+(LDWG*Inv_L))+(WDWG*Inv_W))+(PDWG*Inv_LW));
dwb=(((DWB+(LDWB*Inv_L))+(WDWB*Inv_W))+(PDWB*Inv_LW));
b0=(((B0+(LB0*Inv_L))+(WB0*Inv_W))+(PB0*Inv_LW));
b1=(((B1+(LB1*Inv_L))+(WB1*Inv_W))+(PB1*Inv_LW));
alpha0=(((ALPHA0+(LALPHA0*Inv_L))+(WALPHA0*Inv_W))+(PALPHA0*Inv_LW));
alpha1=(((ALPHA1+(LALPHA1*Inv_L))+(WALPHA1*Inv_W))+(PALPHA1*Inv_LW));
beta0=(((BETA0+(LBETA0*Inv_L))+(WBETA0*Inv_W))+(PBETA0*Inv_LW));
#if defined(_DYNAMIC)
elm=(((ELM+(LELM*Inv_L))+(WELM*Inv_W))+(PELM*Inv_LW));
#endif
#if defined(_DYNAMIC)
cgsl=(((CGSL+(LCGSL*Inv_L))+(WCGSL*Inv_W))+(PCGSL*Inv_LW));
#endif
#if defined(_DYNAMIC)
cgdl=(((CGDL+(LCGDL*Inv_L))+(WCGDL*Inv_W))+(PCGDL*Inv_LW));
#endif
#if defined(_DYNAMIC)
ckappa=(((CKAPPA+(LCKAPPA*Inv_L))+(WCKAPPA*Inv_W))+(PCKAPPA*Inv_LW));
#endif
#if defined(_DYNAMIC)
cf=(((cf+(LCF*Inv_L))+(WCF*Inv_W))+(PCF*Inv_LW));
#endif
clc=(((CLC+(LCLC*Inv_L))+(WCLC*Inv_W))+(PCLC*Inv_LW));
cle=(((CLE+(LCLE*Inv_L))+(WCLE*Inv_W))+(PCLE*Inv_LW));
vfbcv=(((VFBCV+(LVFBCV*Inv_L))+(WVFBCV*Inv_W))+(PVFBCV*Inv_LW));
acde=(((ACDE+(LACDE*Inv_L))+(WACDE*Inv_W))+(PACDE*Inv_LW));
moin=(((MOIN+(LMOIN*Inv_L))+(WMOIN*Inv_W))+(PMOIN*Inv_LW));
noff_param=(((NOFF+(LNOFF*Inv_L))+(WNOFF*Inv_W))+(PNOFF*Inv_LW));
voffcv_param=(((VOFFCV+(LVOFFCV*Inv_L))+(WVOFFCV*Inv_W))+(PVOFFCV*Inv_LW));
{
double m00_pow(d00_pow0,(clc/leffCV),cle)
abulkCVfactor=(1.0+d00_pow0);
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(TRatio-1.0);
ua=(ua+(ua1*T_0));
ub=(ub+(ub1*T_0));
uc=(uc+(uc1*T_0));
if
((u0>1.0))
{
u0=(u0/1.0e4);
}
{
double m00_pow(d00_pow0,TRatio,ute)
u0temp=(u0*d00_pow0);
}
vsattemp=(vsat-(at*T_0));
{
double m00_pow(d00_pow0,(weff*1E6),wr)
rds0=((rdsw+(prt*T_0))/d00_pow0);
}
Fatal_Flag=0;
if
((nlx<(-leff)))
{
logprint(LOG_ERROR,"Fatal: Nlx = %g is less than -Leff.",nlx);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((tox<=0.0))
{
logprint(LOG_ERROR,"Fatal: Tox = %g is not positive.",tox);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((TOXM<=0.0))
{
logprint(LOG_ERROR,"Fatal: Toxm = %g is not positive.",TOXM);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((npeak<=0.0))
{
logprint(LOG_ERROR,"Fatal: Nch = %g is not positive.",npeak);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((nsub<=0.0))
{
logprint(LOG_ERROR,"Fatal: Nsub = %g is not positive.",nsub);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((ngate<0.0))
{
logprint(LOG_ERROR,"Fatal: Ngate = %g Ngate is not positive.",ngate);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((ngate>1.0e25))
{
logprint(LOG_ERROR,"Fatal: Ngate = %g Ngate is too high",ngate);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((xj<=0.0))
{
logprint(LOG_ERROR,"Fatal: Xj = %g is not positive.",xj);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((dvt1<0.0))
{
logprint(LOG_ERROR,"Fatal: Dvt1 = %g is negative.",dvt1);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((dvt1w<0.0))
{
logprint(LOG_ERROR,"Fatal: Dvt1w = %g is negative.",dvt1w);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((w0==(-weff)))
{
logprint(LOG_ERROR,"Fatal: (W0 + Weff) = 0 causing divided-by-zero.");
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((dsub<0.0))
{
logprint(LOG_ERROR,"Fatal: Dsub = %g is negative.",dsub);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((b1==(-weff)))
{
logprint(LOG_ERROR,"Fatal: (B1 + Weff) = 0 causing divided-by-zero.");
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((u0temp<=0.0))
{
logprint(LOG_ERROR,"Fatal: u0 at current temperature = %g is not positive.",u0temp);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((delta<0.0))
{
logprint(LOG_ERROR,"Fatal: Delta = %g is less than zero.",delta);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((vsattemp<=0.0))
{
logprint(LOG_ERROR,"Fatal: Vsat at current temperature = %g is not positive.",vsattemp);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((pclm<=0.0))
{
logprint(LOG_ERROR,"Fatal: Pclm = %g is not positive.",pclm);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((drout<0.0))
{
logprint(LOG_ERROR,"Fatal: Drout = %g is negative.",drout);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((pscbe2<=0.0))
{
logprint(LOG_ERROR,"Warning: Pscbe2 = %g is not positive.",pscbe2);
logprint(LOG_ERROR,"\n");
}
if
(((unitLengthSidewallJctCap>0.0)||(unitLengthGateSidewallJctCap>0.0)))
{
if
((drainPerimeter<weff))
{
logprint(LOG_ERROR,"Warning: Pd = %g is less than W.",drainPerimeter);
logprint(LOG_ERROR,"\n");
}
if
((sourcePerimeter<weff))
{
logprint(LOG_ERROR,"Warning: Ps = %g is less than W.",sourcePerimeter);
logprint(LOG_ERROR,"\n");
}
}
if
((noff_param<0.1))
{
logprint(LOG_ERROR,"Warning: Noff = %g is too small.",noff_param);
logprint(LOG_ERROR,"\n");
}
if
((noff_param>4.0))
{
logprint(LOG_ERROR,"Warning: Noff = %g is too large.",noff_param);
logprint(LOG_ERROR,"\n");
}
if
((voffcv_param<(-0.5)))
{
logprint(LOG_ERROR,"Warning: Voffcv = %g is too small.",voffcv_param);
logprint(LOG_ERROR,"\n");
}
if
((voffcv_param>0.5))
{
logprint(LOG_ERROR,"Warning: Voffcv = %g is too large.",voffcv_param);
logprint(LOG_ERROR,"\n");
}
if
((IJTH<0.0))
{
logprint(LOG_ERROR,"Fatal: Ijth = %g cannot be negative.",IJTH);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((clc<0.0))
{
logprint(LOG_ERROR,"Fatal: Clc = %g is negative.",clc);
logprint(LOG_ERROR,"\n");
Fatal_Flag=1;
}
if
((moin<5.0))
{
logprint(LOG_ERROR,"Warning: Moin = %g is too small.",moin);
logprint(LOG_ERROR,"\n");
}
if
((moin>25.0))
{
logprint(LOG_ERROR,"Warning: Moin = %g is too large.",moin);
logprint(LOG_ERROR,"\n");
}
if
((((acde<0.4)&&(!(VERSION==3.24)))||(((acde<0.4)&&(VERSION==3.24))&&(CAPMOD==3.0))))
{
logprint(LOG_ERROR,"Warning: Acde = %g is too small.",acde);
logprint(LOG_ERROR,"\n");
}
if
((((acde>1.6)&&(!(VERSION==3.24)))||(((acde>1.6)&&(VERSION==3.24))&&(CAPMOD==3.0))))
{
logprint(LOG_ERROR,"Warning: Acde = %g is too large.",acde);
logprint(LOG_ERROR,"\n");
}
if
((PARAMCHK==1))
{
if
((leff<=5.0e-8))
{
logprint(LOG_ERROR,"Warning: Leff = %g may be too small.",leff);
logprint(LOG_ERROR,"\n");
}
if
((leffCV<=5.0e-8))
{
logprint(LOG_ERROR,"Warning: Leff for CV = %g may be too small.",leffCV);
logprint(LOG_ERROR,"\n");
}
if
((weff<=1.0e-7))
{
logprint(LOG_ERROR,"Warning: Weff = %g may be too small.",weff);
logprint(LOG_ERROR,"\n");
}
if
((weffCV<=1.0e-7))
{
logprint(LOG_ERROR,"Warning: Weff for CV = %g may be too small.",weffCV);
logprint(LOG_ERROR,"\n");
}
if
((nlx<0.0))
{
logprint(LOG_ERROR,"Warning: Nlx = %g is negative.",nlx);
logprint(LOG_ERROR,"\n");
}
if
((tox<1.0e-9))
{
logprint(LOG_ERROR,"Warning: Tox = %g is less than 10A.",tox);
logprint(LOG_ERROR,"\n");
}
if
((npeak<=1.0e15))
{
logprint(LOG_ERROR,"Warning: Nch = %g may be too small.",npeak);
logprint(LOG_ERROR,"\n");
}
else
{
if
((npeak>=1.0e21))
{
logprint(LOG_ERROR,"Warning: Nch = %g may be too large.",npeak);
logprint(LOG_ERROR,"\n");
}
}
if
((nsub<=1.0e14))
{
logprint(LOG_ERROR,"Warning: Nsub = %g may be too small.",nsub);
logprint(LOG_ERROR,"\n");
}
else
{
if
((nsub>=1.0e21))
{
logprint(LOG_ERROR,"Warning: Nsub = %g may be too large.",nsub);
logprint(LOG_ERROR,"\n");
}
}
if
(((ngate>0.0)&&(ngate<=1.0e18)))
{
logprint(LOG_ERROR,"Warning: Ngate = %g is less than 1.E18cm^-3.",ngate);
logprint(LOG_ERROR,"\n");
}
if
((dvt0<0.0))
{
logprint(LOG_ERROR,"Warning: Dvt0 = %g is negative.",dvt0);
logprint(LOG_ERROR,"\n");
}
{
double m00_fabs(d00_fabs0,(1.0e-6/(w0+weff)))
if
((d00_fabs0>10.0))
{
logprint(LOG_ERROR,"Warning: (W0 + Weff) may be too small.");
logprint(LOG_ERROR,"\n");
}
}
if
((nfactor<0.0))
{
logprint(LOG_ERROR,"Warning: Nfactor = %g is negative.",nfactor);
logprint(LOG_ERROR,"\n");
}
if
((cdsc<0.0))
{
logprint(LOG_ERROR,"Warning: Cdsc = %g is negative.",cdsc);
logprint(LOG_ERROR,"\n");
}
if
((cdscd<0.0))
{
logprint(LOG_ERROR,"Warning: Cdscd = %g is negative.",cdscd);
logprint(LOG_ERROR,"\n");
}
if
((eta0<0.0))
{
logprint(LOG_ERROR,"Warning: Eta0 = %g is negative.",eta0);
logprint(LOG_ERROR,"\n");
}
{
double m00_fabs(d00_fabs0,(1.0e-6/(b1+weff)))
if
((d00_fabs0>10.0))
{
logprint(LOG_ERROR,"Warning: (B1 + Weff) may be too small.");
logprint(LOG_ERROR,"\n");
}
}
if
((a2<0.01))
{
logprint(LOG_ERROR,"Warning: A2 = %g is too small. Set to 0.01.",a2);
logprint(LOG_ERROR,"\n");
a2=0.01;
}
else
{
if
((a2>1.0))
{
logprint(LOG_ERROR,"Warning: A2 = %g is larger than 1. A2 is set to 1 and A1 is set to 0.",a2);
logprint(LOG_ERROR,"\n");
a2=1.0;
a1=0.0;
}
}
if
((rdsw<0.0))
{
logprint(LOG_ERROR,"Warning: Rdsw = %g is negative. Set to zero.",rdsw);
logprint(LOG_ERROR,"\n");
#if defined(_DYNAMIC)
rdsw=0.0;
#endif
rds0=0.0;
}
else
{
if
(((rds0>0.0)&&(rds0<0.001)))
{
logprint(LOG_ERROR,"Warning: Rds at current temperature = %g is less than 0.001 ohm. Set to zero.",rds0);
logprint(LOG_ERROR,"\n");
rds0=0.0;
}
}
if
((vsattemp<1.0e3))
{
logprint(LOG_ERROR,"Warning: Vsat at current temperature = %g may be too small.",vsattemp);
logprint(LOG_ERROR,"\n");
}
if
((pdibl1<0.0))
{
logprint(LOG_ERROR,"Warning: Pdibl1 = %g is negative.",pdibl1);
logprint(LOG_ERROR,"\n");
}
if
((pdibl2<0.0))
{
logprint(LOG_ERROR,"Warning: Pdibl2 = %g is negative.",pdibl2);
logprint(LOG_ERROR,"\n");
}
if
((cgdo_param<0.0))
{
logprint(LOG_ERROR,"Warning: cgdo = %g is negative. Set to zero.",cgdo_param);
logprint(LOG_ERROR,"\n");
#if defined(_DYNAMIC)
cgdo_param=0.0;
#endif
}
if
((cgso_param<0.0))
{
logprint(LOG_ERROR,"Warning: cgso = %g is negative. Set to zero.",cgso_param);
logprint(LOG_ERROR,"\n");
#if defined(_DYNAMIC)
cgso_param=0.0;
#endif
}
if
((cgbo_param<0.0))
{
logprint(LOG_ERROR,"Warning: cgbo = %g is negative. Set to zero.",cgbo_param);
logprint(LOG_ERROR,"\n");
#if defined(_DYNAMIC)
cgbo_param=0.0;
#endif
}
}
if
(Fatal_Flag)
{
exit(1);
}
#if defined(_DYNAMIC)
cgdo_param=((cgdo_param+cf)*weffCV);
#endif
#if defined(_DYNAMIC)
cgso_param=((cgso_param+cf)*weffCV);
#endif
#if defined(_DYNAMIC)
cgbo_param=(cgbo_param*leffCV);
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(leffCV*leffCV);
#if defined(_DYNAMIC)
tconst=((u0temp*elm)/(((cox*weffCV)*leffCV)*T_0));
#endif
if
(((!npeakGiven)&&gamma1Given))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(gamma1*cox);
npeak=((3.021E22*T_0)*T_0);
}
{
double m00_logE(d00_logE0,(npeak/ni))
phi=((2.0*Vtm0)*d00_logE0);
}
{
double m00_sqrt(d00_sqrt0,phi)
sqrtPhi=d00_sqrt0;
}
phis3=(sqrtPhi*phi);
{
double m00_sqrt(d00_sqrt0,((2.0*1.03594e-10)/((1.60219e-19*npeak)*1.0e6)))
Xdep0=(d00_sqrt0*sqrtPhi);
}
{
double m00_sqrt(d00_sqrt0,((3.0*xj)*tox))
litl=d00_sqrt0;
}
{
double m00_logE(d00_logE0,((1.0e20*npeak)/(ni*ni)))
vbi=(Vtm0*d00_logE0);
}
{
double m00_sqrt(d00_sqrt0,(((((1.60219e-19*1.03594e-10)*npeak)*1.0e6)/2.0)/phi))
cdep0=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,((1.03594e-10*Vtm0)/((1.60219e-19*npeak)*1.0e6)))
ldeb=(d00_sqrt0/3.0);
}
{
double m00_pow(d00_pow0,(npeak/2.0e16),(-0.25))
acde=(acde*d00_pow0);
}
if
((k1Given||k2Given))
{
if
((!k1Given))
{
logprint(LOG_ERROR,"Warning: k1 should be specified with k2.");
logprint(LOG_ERROR,"\n");
k1=0.53;
}
if
((!k2Given))
{
logprint(LOG_ERROR,"Warning: k2 should be specified with k1.");
logprint(LOG_ERROR,"\n");
k2=(-0.0186);
}
if
(nsubGiven)
{
logprint(LOG_ERROR,"Warning: nsub is ignored because k1 or k2 is given.");
logprint(LOG_ERROR,"\n");
}
if
(xtGiven)
{
logprint(LOG_ERROR,"Warning: xt is ignored because k1 or k2 is given.");
logprint(LOG_ERROR,"\n");
}
if
(vbxGiven)
{
logprint(LOG_ERROR,"Warning: vbx is ignored because k1 or k2 is given.");
logprint(LOG_ERROR,"\n");
}
if
(gamma1Given)
{
logprint(LOG_ERROR,"Warning: gamma1 is ignored because k1 or k2 is given.");
logprint(LOG_ERROR,"\n");
}
if
(gamma2Given)
{
logprint(LOG_ERROR,"Warning: gamma2 is ignored because k1 or k2 is given.");
logprint(LOG_ERROR,"\n");
}
}
else
{
if
((!vbxGiven))
{
vbx=(phi-(((7.7348e-4*npeak)*xt)*xt));
}
if
((vbx>0.0))
{
vbx=(-vbx);
}
if
((vbm>0.0))
{
vbm=(-vbm);
}
if
((!gamma1Given))
{
{
double m00_sqrt(d00_sqrt0,npeak)
gamma1=((5.753e-12*d00_sqrt0)/cox);
}
}
if
((!gamma2Given))
{
{
double m00_sqrt(d00_sqrt0,nsub)
gamma2=((5.753e-12*d00_sqrt0)/cox);
}
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(gamma1-gamma2);
{
double m00_sqrt(d00_sqrt0,(phi-vbx))
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=(d00_sqrt0-sqrtPhi);
}
{
double m00_sqrt(d00_sqrt0,(phi*(phi-vbm)))
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(d00_sqrt0-phi);
}
k2=((T_0*T1)/((2.0*T2)+vbm));
{
double m00_sqrt(d00_sqrt0,(phi-vbm))
k1=(gamma2-((2.0*k2)*d00_sqrt0));
}
}
if
((k2<0.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((0.5*k1)/k2);
vbsc=(0.9*(phi-(T_0*T_0)));
if
((vbsc>(-3.0)))
{
vbsc=(-3.0);
}
else
{
if
((vbsc<(-30.0)))
{
vbsc=(-30.0);
}
}
}
else
{
vbsc=(-30.0);
}
if
((vbsc>vbm))
{
vbsc=vbm;
}
if
((!vfbGiven))
{
if
(vth0Given)
{
vfb=(((TYPE*vth0)-phi)-(k1*sqrtPhi));
}
else
{
vfb=(-1.0);
}
}
if
((!vth0Given))
{
vth0=(TYPE*((vfb+phi)+(k1*sqrtPhi)));
}
k1ox=((k1*tox)/TOXM);
k2ox=((k2*tox)/TOXM);
{
double m00_sqrt(d00_sqrt0,(((1.03594e-10/3.453133e-11)*tox)*Xdep0))
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_sqrt0;
}
{
double m00_exp(d00_exp0,((((-0.5)*dsub)*leff)/T1))
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=d00_exp0;
}
theta0vb0=(T_0+((2.0*T_0)*T_0));
{
double m00_exp(d00_exp0,((((-0.5)*drout)*leff)/T1))
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=d00_exp0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(T_0+((2.0*T_0)*T_0));
thetaRout=((pdibl1*T2)+pdibl2);
{
double m00_sqrt(d00_sqrt0,Xdep0)
#if defined(_DERIVATE)
tmp_Vgate_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vbulk_sourcep=0.0;
#endif
tmp=d00_sqrt0;
}
#if defined(_DERIVATE)
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vbulk_sourcep=0.0;
#endif
tmp1=(vbi-phi);
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vbulk_sourcep=0.0;
#endif
tmp2=(factor1*tmp);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(((((-0.5)*dvt1w)*weff)*leff)/tmp2);
if
((T_0>(-34.0)))
{
{
double m00_exp(d00_exp0,T_0)
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_exp0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(T1*(1.0+(2.0*T1)));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=1.713908431e-15;
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(T1*(1.0+(2.0*T1)));
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(dvt0w*T2);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(T_0*tmp1);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((((-0.5)*dvt1)*leff)/tmp2);
if
((T_0>(-34.0)))
{
{
double m00_exp(d00_exp0,T_0)
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_exp0;
}
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=(T1*(1.0+(2.0*T1)));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=1.713908431e-15;
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=(T1*(1.0+(2.0*T1)));
}
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=((dvt0*T3)*tmp1);
#if defined(_DERIVATE)
T4_Vgate_sourcep=0.0;
T4_Vdrainp_sourcep=0.0;
T4_Vbulk_sourcep=0.0;
#endif
T4=((tox*phi)/(weff+w0));
{
double m00_sqrt(d00_sqrt0,(1.0+(nlx/leff)))
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T5_Vgate_sourcep=0.0;
T5_Vdrainp_sourcep=0.0;
T5_Vbulk_sourcep=0.0;
#endif
T5=(((k1ox*(T_0-1.0))*sqrtPhi)+((kt1+(kt1l/leff))*(TRatio-1.0)));
#if defined(_DERIVATE)
tmp3_Vdrainp_sourcep=0.0;
tmp3_Vbulk_sourcep=0.0;
#endif
tmp3=(((((TYPE*vth0)-T2)-T3)+(k3*T4))+T5);
vfbzb=((tmp3-phi)-(k1*sqrtPhi));
drainConductance=(sheetResistance*drainSquares);
if
((drainConductance>0.0))
{
drainConductance=(1.0/drainConductance);
}
else
{
drainConductance=0.0;
}
sourceConductance=(sheetResistance*sourceSquares);
if
((sourceConductance>0.0))
{
sourceConductance=(1.0/sourceConductance);
}
else
{
sourceConductance=0.0;
}
Nvtm=(vtm*jctEmissionCoeff);
if
(((sourceArea<=0.0)&&(sourcePerimeter<=0.0)))
{
SourceSatCurrent=1.0e-14;
}
else
{
SourceSatCurrent=((sourceArea*jctTempSatCurDensity)+(sourcePerimeter*jctSidewallTempSatCurDensity));
}
if
(((SourceSatCurrent>0.0)&&(IJTH>0.0)))
{
{
double m00_logE(d00_logE0,((IJTH/SourceSatCurrent)+1.0))
vjsm=(Nvtm*d00_logE0);
}
if
((VERSION==3.24))
{
{
double m00_exp(d00_exp0,(vjsm/Nvtm))
IsEvjsm=(SourceSatCurrent*d00_exp0);
}
}
}
if
(((drainArea<=0.0)&&(drainPerimeter<=0.0)))
{
DrainSatCurrent=1.0e-14;
}
else
{
DrainSatCurrent=((drainArea*jctTempSatCurDensity)+(drainPerimeter*jctSidewallTempSatCurDensity));
}
if
(((DrainSatCurrent>0.0)&&(IJTH>0.0)))
{
{
double m00_logE(d00_logE0,((IJTH/DrainSatCurrent)+1.0))
vjdm=(Nvtm*d00_logE0);
}
if
((VERSION==3.24))
{
{
double m00_exp(d00_exp0,(vjdm/Nvtm))
IsEvjdm=(DrainSatCurrent*d00_exp0);
}
}
}
}
}

/* Evaluate Verilog-AMS equations in final step. */
void bsim3v34pMOS::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void bsim3v34pMOS::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double leffx2;
double fourkt;
double cqbulk;
double cqdrn;
double cqgate;
#if defined(_DYNAMIC)
double Qgate;
#if defined(_DERIVATE)
double Qgate_Vdrainp_sourcep;
double Qgate_Vgate_sourcep;
double Qgate_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double Qbulk;
#if defined(_DERIVATE)
double Qbulk_Vdrainp_sourcep;
double Qbulk_Vgate_sourcep;
double Qbulk_Vbulk_sourcep;
#endif
#endif
double Qsrc;
#if defined(_DYNAMIC)
double Qdrn;
#if defined(_DERIVATE)
double Qdrn_Vgate_sourcep;
double Qdrn_Vdrainp_sourcep;
double Qdrn_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qgb;
#if defined(_DERIVATE)
double qgb_Vgate_sourcep;
double qgb_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qgs;
#if defined(_DERIVATE)
double qgs_Vdrainp_sourcep;
double qgs_Vbulk_sourcep;
double qgs_Vgate_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qgd;
#if defined(_DERIVATE)
double qgd_Vbulk_sourcep;
double qgd_Vgate_sourcep;
double qgd_Vdrainp_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qgso;
#if defined(_DERIVATE)
double qgso_Vdrainp_sourcep;
double qgso_Vbulk_sourcep;
double qgso_Vgate_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qgdo;
#if defined(_DERIVATE)
double qgdo_Vbulk_sourcep;
double qgdo_Vgate_sourcep;
double qgdo_Vdrainp_sourcep;
#endif
#endif
double Tox2;
double DeltaPhi;
#if defined(_DERIVATE)
double DeltaPhi_Vgate_sourcep;
double DeltaPhi_Vdrainp_sourcep;
double DeltaPhi_Vbulk_sourcep;
#endif
double CoxWLcen;
#if defined(_DERIVATE)
double CoxWLcen_Vgate_sourcep;
double CoxWLcen_Vdrainp_sourcep;
double CoxWLcen_Vbulk_sourcep;
#endif
double Coxeff;
#if defined(_DERIVATE)
double Coxeff_Vgate_sourcep;
double Coxeff_Vdrainp_sourcep;
double Coxeff_Vbulk_sourcep;
#endif
double Ccen;
#if defined(_DERIVATE)
double Ccen_Vgate_sourcep;
double Ccen_Vdrainp_sourcep;
double Ccen_Vbulk_sourcep;
#endif
double LINK;
double Tcen;
#if defined(_DERIVATE)
double Tcen_Vgate_sourcep;
double Tcen_Vdrainp_sourcep;
double Tcen_Vbulk_sourcep;
#endif
double Tox;
double Cox;
double qinoi;
#if defined(_DERIVATE)
double qinoi_Vdrainp_sourcep;
double qinoi_Vgate_sourcep;
double qinoi_Vbulk_sourcep;
#endif
double VdseffCV;
#if defined(_DERIVATE)
double VdseffCV_Vdrainp_sourcep;
double VdseffCV_Vgate_sourcep;
double VdseffCV_Vbulk_sourcep;
#endif
double V4;
#if defined(_DERIVATE)
double V4_Vdrainp_sourcep;
double V4_Vgate_sourcep;
double V4_Vbulk_sourcep;
#endif
#if defined(_DYNAMIC)
double Qsub0;
#if defined(_DERIVATE)
double Qsub0_Vgate_sourcep;
double Qsub0_Vdrainp_sourcep;
double Qsub0_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double Qac0;
#if defined(_DERIVATE)
double Qac0_Vdrainp_sourcep;
double Qac0_Vbulk_sourcep;
double Qac0_Vgate_sourcep;
#endif
#endif
double Vfbeff;
#if defined(_DERIVATE)
double Vfbeff_Vdrainp_sourcep;
double Vfbeff_Vbulk_sourcep;
double Vfbeff_Vgate_sourcep;
#endif
double V3;
#if defined(_DERIVATE)
double V3_Vdrainp_sourcep;
double V3_Vbulk_sourcep;
double V3_Vgate_sourcep;
#endif
double VdsatCV;
#if defined(_DERIVATE)
double VdsatCV_Vdrainp_sourcep;
double VdsatCV_Vgate_sourcep;
double VdsatCV_Vbulk_sourcep;
#endif
double Noff2;
#if defined(_DERIVATE)
double Noff2_Vdrainp_sourcep;
double Noff2_Vbulk_sourcep;
#endif
double VbseffCV;
#if defined(_DERIVATE)
double VbseffCV_Vdrainp_sourcep;
double VbseffCV_Vbulk_sourcep;
#endif
double Alphaz;
#if defined(_DERIVATE)
double Alphaz_Vdrainp_sourcep;
double Alphaz_Vgate_sourcep;
double Alphaz_Vbulk_sourcep;
#endif
double AbulkCV;
#if defined(_DERIVATE)
double AbulkCV_Vgate_sourcep;
double AbulkCV_Vdrainp_sourcep;
double AbulkCV_Vbulk_sourcep;
#endif
double Two_Third_CoxWL;
double One_Third_CoxWL;
double qinv;
#if defined(_DERIVATE)
double qinv_Vdrainp_sourcep;
double qinv_Vgate_sourcep;
double qinv_Vbulk_sourcep;
#endif
double Arg1;
#if defined(_DERIVATE)
double Arg1_Vdrainp_sourcep;
double Arg1_Vgate_sourcep;
double Arg1_Vbulk_sourcep;
#endif
double CoxWL;
double Vfb;
#if defined(_DERIVATE)
double Vfb_Vdrainp_sourcep;
double Vfb_Vbulk_sourcep;
#endif
double qbulk;
#if defined(_DERIVATE)
double qbulk_Vdrainp_sourcep;
double qbulk_Vgate_sourcep;
double qbulk_Vbulk_sourcep;
#endif
#if defined(_DYNAMIC)
double qsrc;
#if defined(_DERIVATE)
double qsrc_Vgate_sourcep;
double qsrc_Vdrainp_sourcep;
double qsrc_Vbulk_sourcep;
#endif
#endif
double qdrn;
#if defined(_DERIVATE)
double qdrn_Vgate_sourcep;
double qdrn_Vdrainp_sourcep;
double qdrn_Vbulk_sourcep;
#endif
double qgate;
#if defined(_DERIVATE)
double qgate_Vdrainp_sourcep;
double qgate_Vgate_sourcep;
double qgate_Vbulk_sourcep;
#endif
double capbd;
#if defined(_DYNAMIC)
double sarg;
#if defined(_DERIVATE)
double sarg_Vdrainp_sourcep;
double sarg_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double arg;
#if defined(_DERIVATE)
double arg_Vdrainp_sourcep;
double arg_Vbulk_sourcep;
#endif
#endif
double capbs;
#if defined(_DYNAMIC)
double qbd;
#if defined(_DERIVATE)
double qbd_Vgate_sourcep;
double qbd_Vdrainp_sourcep;
double qbd_Vbulk_sourcep;
#endif
#endif
#if defined(_DYNAMIC)
double qbs;
#if defined(_DERIVATE)
double qbs_Vgate_sourcep;
double qbs_Vdrainp_sourcep;
double qbs_Vbulk_sourcep;
#endif
#endif
double mjswg;
double mjsw;
double mj;
double czbsswg;
double czbssw;
double czbdsw;
double czbdswg;
double czbs;
double czbd;
double csub;
#if defined(_DERIVATE)
double csub_Vgate_sourcep;
double csub_Vdrainp_sourcep;
double csub_Vbulk_sourcep;
#endif
double cdrain;
#if defined(_DERIVATE)
double cdrain_Vgate_sourcep;
double cdrain_Vdrainp_sourcep;
double cdrain_Vbulk_sourcep;
#endif
double Isub;
#if defined(_DERIVATE)
double Isub_Vgate_sourcep;
double Isub_Vdrainp_sourcep;
double Isub_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp_Vgate_sourcep;
double tmp_Vdrainp_sourcep;
double tmp_Vbulk_sourcep;
#endif
double Ids;
#if defined(_DERIVATE)
double Ids_Vgate_sourcep;
double Ids_Vdrainp_sourcep;
double Ids_Vbulk_sourcep;
#endif
double Idsa;
#if defined(_DERIVATE)
double Idsa_Vgate_sourcep;
double Idsa_Vdrainp_sourcep;
double Idsa_Vbulk_sourcep;
#endif
double Idl;
#if defined(_DERIVATE)
double Idl_Vgate_sourcep;
double Idl_Vdrainp_sourcep;
double Idl_Vbulk_sourcep;
#endif
double gche;
#if defined(_DERIVATE)
double gche_Vgate_sourcep;
double gche_Vdrainp_sourcep;
double gche_Vbulk_sourcep;
#endif
double fgche2;
#if defined(_DERIVATE)
double fgche2_Vgate_sourcep;
double fgche2_Vdrainp_sourcep;
double fgche2_Vbulk_sourcep;
#endif
double fgche1;
#if defined(_DERIVATE)
double fgche1_Vdrainp_sourcep;
double fgche1_Vgate_sourcep;
double fgche1_Vbulk_sourcep;
#endif
double beta;
#if defined(_DERIVATE)
double beta_Vgate_sourcep;
double beta_Vdrainp_sourcep;
double beta_Vbulk_sourcep;
#endif
double CoxWovL;
#if defined(_DERIVATE)
double CoxWovL_Vdrainp_sourcep;
double CoxWovL_Vgate_sourcep;
double CoxWovL_Vbulk_sourcep;
#endif
double VASCBE;
#if defined(_DERIVATE)
double VASCBE_Vdrainp_sourcep;
double VASCBE_Vgate_sourcep;
double VASCBE_Vbulk_sourcep;
#endif
double Va;
#if defined(_DERIVATE)
double Va_Vgate_sourcep;
double Va_Vdrainp_sourcep;
double Va_Vbulk_sourcep;
#endif
double VADIBL;
#if defined(_DERIVATE)
double VADIBL_Vdrainp_sourcep;
double VADIBL_Vgate_sourcep;
double VADIBL_Vbulk_sourcep;
#endif
double VACLM;
#if defined(_DERIVATE)
double VACLM_Vgate_sourcep;
double VACLM_Vdrainp_sourcep;
double VACLM_Vbulk_sourcep;
#endif
double diffVds;
#if defined(_DERIVATE)
double diffVds_Vdrainp_sourcep;
double diffVds_Vgate_sourcep;
double diffVds_Vbulk_sourcep;
#endif
double Vasat;
#if defined(_DERIVATE)
double Vasat_Vgate_sourcep;
double Vasat_Vdrainp_sourcep;
double Vasat_Vbulk_sourcep;
#endif
double Vdseff;
#if defined(_DERIVATE)
double Vdseff_Vgate_sourcep;
double Vdseff_Vdrainp_sourcep;
double Vdseff_Vbulk_sourcep;
#endif
double vdsat;
double Vdsat;
#if defined(_DERIVATE)
double Vdsat_Vgate_sourcep;
double Vdsat_Vdrainp_sourcep;
double Vdsat_Vbulk_sourcep;
#endif
double Vgst2Vtm;
#if defined(_DERIVATE)
double Vgst2Vtm_Vdrainp_sourcep;
double Vgst2Vtm_Vgate_sourcep;
double Vgst2Vtm_Vbulk_sourcep;
#endif
double Lambda;
#if defined(_DERIVATE)
double Lambda_Vgate_sourcep;
double Lambda_Vdrainp_sourcep;
double Lambda_Vbulk_sourcep;
#endif
double EsatL;
#if defined(_DERIVATE)
double EsatL_Vgate_sourcep;
double EsatL_Vdrainp_sourcep;
double EsatL_Vbulk_sourcep;
#endif
double Esat;
#if defined(_DERIVATE)
double Esat_Vgate_sourcep;
double Esat_Vdrainp_sourcep;
double Esat_Vbulk_sourcep;
#endif
double WVCoxRds;
#if defined(_DERIVATE)
double WVCoxRds_Vdrainp_sourcep;
double WVCoxRds_Vgate_sourcep;
double WVCoxRds_Vbulk_sourcep;
#endif
double WVCox;
#if defined(_DERIVATE)
double WVCox_Vdrainp_sourcep;
double WVCox_Vgate_sourcep;
double WVCox_Vbulk_sourcep;
#endif
double ueff;
#if defined(_DERIVATE)
double ueff_Vgate_sourcep;
double ueff_Vdrainp_sourcep;
double ueff_Vbulk_sourcep;
#endif
double Denomi;
#if defined(_DERIVATE)
double Denomi_Vgate_sourcep;
double Denomi_Vdrainp_sourcep;
double Denomi_Vbulk_sourcep;
#endif
double Abulk;
#if defined(_DERIVATE)
double Abulk_Vgate_sourcep;
double Abulk_Vdrainp_sourcep;
double Abulk_Vbulk_sourcep;
#endif
double T8;
#if defined(_DERIVATE)
double T8_Vgate_sourcep;
double T8_Vdrainp_sourcep;
double T8_Vbulk_sourcep;
#endif
double Abulk0;
#if defined(_DERIVATE)
double Abulk0_Vgate_sourcep;
double Abulk0_Vdrainp_sourcep;
double Abulk0_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp1_Vdrainp_sourcep;
double tmp1_Vbulk_sourcep;
#endif
double Rds;
#if defined(_DERIVATE)
double Rds_Vgate_sourcep;
double Rds_Vdrainp_sourcep;
double Rds_Vbulk_sourcep;
#endif
double Weff;
#if defined(_DERIVATE)
double Weff_Vdrainp_sourcep;
double Weff_Vgate_sourcep;
double Weff_Vbulk_sourcep;
#endif
double ExpVgst;
#if defined(_DERIVATE)
double ExpVgst_Vgate_sourcep;
double ExpVgst_Vdrainp_sourcep;
double ExpVgst_Vbulk_sourcep;
#endif
double Vgsteff;
#if defined(_DERIVATE)
double Vgsteff_Vdrainp_sourcep;
double Vgsteff_Vgate_sourcep;
double Vgsteff_Vbulk_sourcep;
#endif
double ExpArg;
#if defined(_DERIVATE)
double ExpArg_Vdrainp_sourcep;
double ExpArg_Vgate_sourcep;
double ExpArg_Vbulk_sourcep;
#endif
double VgstNVt;
#if defined(_DERIVATE)
double VgstNVt_Vdrainp_sourcep;
double VgstNVt_Vgate_sourcep;
double VgstNVt_Vbulk_sourcep;
#endif
double T10;
#if defined(_DERIVATE)
double T10_Vgate_sourcep;
double T10_Vdrainp_sourcep;
double T10_Vbulk_sourcep;
#endif
double Vgst;
#if defined(_DERIVATE)
double Vgst_Vdrainp_sourcep;
double Vgst_Vgate_sourcep;
double Vgst_Vbulk_sourcep;
#endif
double Vgs_eff;
#if defined(_DERIVATE)
double Vgs_eff_Vdrainp_sourcep;
double Vgs_eff_Vgate_sourcep;
double Vgs_eff_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T5_Vgate_sourcep;
double T5_Vdrainp_sourcep;
double T5_Vbulk_sourcep;
#endif
double T6;
#if defined(_DERIVATE)
double T6_Vgate_sourcep;
double T6_Vdrainp_sourcep;
double T6_Vbulk_sourcep;
#endif
double T7;
#if defined(_DERIVATE)
double T7_Vgate_sourcep;
double T7_Vdrainp_sourcep;
double T7_Vbulk_sourcep;
#endif
double n;
#if defined(_DERIVATE)
double n_Vdrainp_sourcep;
double n_Vbulk_sourcep;
#endif
double tmp4;
#if defined(_DERIVATE)
double tmp4_Vgate_sourcep;
double tmp4_Vdrainp_sourcep;
double tmp4_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp3_Vdrainp_sourcep;
double tmp3_Vbulk_sourcep;
#endif
double Vth;
#if defined(_DERIVATE)
double Vth_Vdrainp_sourcep;
double Vth_Vbulk_sourcep;
#endif
double DIBL_Sft;
#if defined(_DERIVATE)
double DIBL_Sft_Vdrainp_sourcep;
double DIBL_Sft_Vbulk_sourcep;
#endif
double dDIBL_Sft_dVd;
#if defined(_DERIVATE)
double dDIBL_Sft_dVd_Vdrainp_sourcep;
double dDIBL_Sft_dVd_Vbulk_sourcep;
#endif
double T9;
#if defined(_DERIVATE)
double T9_Vgate_sourcep;
double T9_Vdrainp_sourcep;
double T9_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double tmp2_Vgate_sourcep;
double tmp2_Vdrainp_sourcep;
double tmp2_Vbulk_sourcep;
#endif
double TempRatio;
double Delt_vth;
#if defined(_DERIVATE)
double Delt_vth_Vdrainp_sourcep;
double Delt_vth_Vbulk_sourcep;
#endif
double thetavth;
#if defined(_DERIVATE)
double thetavth_Vdrainp_sourcep;
double thetavth_Vbulk_sourcep;
#endif
double Theta0;
#if defined(_DERIVATE)
double Theta0_Vdrainp_sourcep;
double Theta0_Vbulk_sourcep;
#endif
double ltw;
#if defined(_DERIVATE)
double ltw_Vdrainp_sourcep;
double ltw_Vbulk_sourcep;
#endif
double lt1;
#if defined(_DERIVATE)
double lt1_Vdrainp_sourcep;
double lt1_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T4_Vgate_sourcep;
double T4_Vdrainp_sourcep;
double T4_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T2_Vgate_sourcep;
double T2_Vdrainp_sourcep;
double T2_Vbulk_sourcep;
#endif
double V0;
#if defined(_DERIVATE)
double T3_Vgate_sourcep;
double T3_Vdrainp_sourcep;
double T3_Vbulk_sourcep;
#endif
double Vtm;
double Leff;
double Xdep;
#if defined(_DERIVATE)
double Xdep_Vdrainp_sourcep;
double Xdep_Vbulk_sourcep;
#endif
double sqrtPhis;
#if defined(_DERIVATE)
double sqrtPhis_Vdrainp_sourcep;
double sqrtPhis_Vbulk_sourcep;
#endif
double Phis;
#if defined(_DERIVATE)
double Phis_Vdrainp_sourcep;
double Phis_Vbulk_sourcep;
#endif
double Vbseff;
#if defined(_DERIVATE)
double Vbseff_Vdrainp_sourcep;
double Vbseff_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
double T1_Vgate_sourcep;
double T1_Vdrainp_sourcep;
double T1_Vbulk_sourcep;
#endif
double Vbs;
#if defined(_DERIVATE)
double Vbs_Vdrainp_sourcep;
double Vbs_Vbulk_sourcep;
#endif
double Vgs;
#if defined(_DERIVATE)
double Vgs_Vdrainp_sourcep;
double Vgs_Vgate_sourcep;
#endif
double Vds;
#if defined(_DERIVATE)
double Vds_Vdrainp_sourcep;
#endif
double mode;
double evbd;
#if defined(_DERIVATE)
double evbd_Vbulk_sourcep;
double evbd_Vdrainp_sourcep;
#endif
double cbd;
#if defined(_DERIVATE)
double cbd_Vbulk_sourcep;
double cbd_Vdrainp_sourcep;
#endif
double gbd;
#if defined(_DERIVATE)
double gbd_Vbulk_sourcep;
double gbd_Vdrainp_sourcep;
#endif
#if defined(_DERIVATE)
double T_0_Vgate_sourcep;
double T_0_Vdrainp_sourcep;
double T_0_Vbulk_sourcep;
#endif
double evbs;
#if defined(_DERIVATE)
double evbs_Vbulk_sourcep;
#endif
double cbs;
#if defined(_DERIVATE)
double cbs_Vbulk_sourcep;
#endif
double gbs;
#if defined(_DERIVATE)
double gbs_Vbulk_sourcep;
#endif
double temp;
#if defined(_DYNAMIC)
double vgb;
#if defined(_DERIVATE)
double vgb_Vgate_sourcep;
double vgb_Vbulk_sourcep;
#endif
#endif
double vgd;
#if defined(_DERIVATE)
double vgd_Vgate_sourcep;
double vgd_Vdrainp_sourcep;
#endif
double vbd;
#if defined(_DERIVATE)
double vbd_Vbulk_sourcep;
double vbd_Vdrainp_sourcep;
#endif
double vds;
#if defined(_DERIVATE)
double vds_Vdrainp_sourcep;
#endif
double vgs;
#if defined(_DERIVATE)
double vgs_Vgate_sourcep;
#endif
double vbs;
#if defined(_DERIVATE)
double vbs_Vbulk_sourcep;
#endif
#if defined(_DERIVATE)
vbs_Vbulk_sourcep=(TYPE);
#endif
vbs=(TYPE*BP(bulk,sourcep));
#if defined(_DERIVATE)
vgs_Vgate_sourcep=(TYPE);
#endif
vgs=(TYPE*BP(gate,sourcep));
#if defined(_DERIVATE)
vds_Vdrainp_sourcep=(TYPE);
#endif
vds=(TYPE*BP(drainp,sourcep));
#if defined(_DERIVATE)
vbd_Vbulk_sourcep=vbs_Vbulk_sourcep;
vbd_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vbd=(vbs-vds);
#if defined(_DERIVATE)
vgd_Vgate_sourcep=vgs_Vgate_sourcep;
vgd_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vgd=(vgs-vds);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vgb_Vgate_sourcep=vgs_Vgate_sourcep;
vgb_Vbulk_sourcep=(-vbs_Vbulk_sourcep);
#endif
vgb=(vgs-vbs);
#endif
temp=_circuit_temp;
if
((SourceSatCurrent<=0.0))
{
#if defined(_DERIVATE)
gbs_Vbulk_sourcep=0.0;
#endif
gbs=GMIN;
#if defined(_DERIVATE)
cbs_Vbulk_sourcep=((gbs_Vbulk_sourcep*vbs)+(gbs*vbs_Vbulk_sourcep));
#endif
cbs=(gbs*vbs);
}
else
{
if
((IJTH==0.0))
{
{
double m00_exp(d00_exp0,(vbs/Nvtm))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulk_sourcep=(vbs_Vbulk_sourcep/Nvtm)*d10_exp0;
#endif
evbs=d00_exp0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbs_Vbulk_sourcep=((SourceSatCurrent*evbs_Vbulk_sourcep)/Nvtm);
#endif
gbs=(((SourceSatCurrent*evbs)/Nvtm)+GMIN);
#endif
#if defined(_DERIVATE)
cbs_Vbulk_sourcep=((SourceSatCurrent*evbs_Vbulk_sourcep)+(GMIN*vbs_Vbulk_sourcep));
#endif
cbs=((SourceSatCurrent*(evbs-1.0))+(GMIN*vbs));
}
else
{
if
((vbs<vjsm))
{
{
double m00_exp(d00_exp0,(vbs/Nvtm))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulk_sourcep=(vbs_Vbulk_sourcep/Nvtm)*d10_exp0;
#endif
evbs=d00_exp0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbs_Vbulk_sourcep=((SourceSatCurrent*evbs_Vbulk_sourcep)/Nvtm);
#endif
gbs=(((SourceSatCurrent*evbs)/Nvtm)+GMIN);
#endif
#if defined(_DERIVATE)
cbs_Vbulk_sourcep=((SourceSatCurrent*evbs_Vbulk_sourcep)+(GMIN*vbs_Vbulk_sourcep));
#endif
cbs=((SourceSatCurrent*(evbs-1.0))+(GMIN*vbs));
}
else
{
if
((VERSION==3.24))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(IsEvjsm/Nvtm);
#if defined(_DERIVATE)
cbs_Vbulk_sourcep=((GMIN*vbs_Vbulk_sourcep)+((T_0_Vbulk_sourcep*(vbs-vjsm))+(T_0*vbs_Vbulk_sourcep)));
#endif
cbs=(((IsEvjsm-SourceSatCurrent)+(GMIN*vbs))+(T_0*(vbs-vjsm)));
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((SourceSatCurrent+IJTH)/Nvtm);
#if defined(_DERIVATE)
cbs_Vbulk_sourcep=((GMIN*vbs_Vbulk_sourcep)+((T_0_Vbulk_sourcep*(vbs-vjsm))+(T_0*vbs_Vbulk_sourcep)));
#endif
cbs=((IJTH+(GMIN*vbs))+(T_0*(vbs-vjsm)));
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbs_Vbulk_sourcep=0.0;
#endif
gbs=(T_0+GMIN);
#endif
}
}
}
if
((DrainSatCurrent<=0.0))
{
#if defined(_DERIVATE)
gbd_Vbulk_sourcep=0.0;
gbd_Vdrainp_sourcep=0.0;
#endif
gbd=GMIN;
#if defined(_DERIVATE)
cbd_Vbulk_sourcep=((gbd_Vbulk_sourcep*vbd)+(gbd*vbd_Vbulk_sourcep));
cbd_Vdrainp_sourcep=((gbd_Vdrainp_sourcep*vbd)+(gbd*vbd_Vdrainp_sourcep));
#endif
cbd=(gbd*vbd);
}
else
{
if
((IJTH==0.0))
{
{
double m00_exp(d00_exp0,(vbd/Nvtm))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vbulk_sourcep=(vbd_Vbulk_sourcep/Nvtm)*d10_exp0;
evbd_Vdrainp_sourcep=(vbd_Vdrainp_sourcep/Nvtm)*d10_exp0;
#endif
evbd=d00_exp0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbd_Vbulk_sourcep=((DrainSatCurrent*evbd_Vbulk_sourcep)/Nvtm);
gbd_Vdrainp_sourcep=((DrainSatCurrent*evbd_Vdrainp_sourcep)/Nvtm);
#endif
gbd=(((DrainSatCurrent*evbd)/Nvtm)+GMIN);
#endif
#if defined(_DERIVATE)
cbd_Vbulk_sourcep=((DrainSatCurrent*evbd_Vbulk_sourcep)+(GMIN*vbd_Vbulk_sourcep));
cbd_Vdrainp_sourcep=((DrainSatCurrent*evbd_Vdrainp_sourcep)+(GMIN*vbd_Vdrainp_sourcep));
#endif
cbd=((DrainSatCurrent*(evbd-1.0))+(GMIN*vbd));
}
else
{
if
((vbd<vjdm))
{
{
double m00_exp(d00_exp0,(vbd/Nvtm))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vbulk_sourcep=(vbd_Vbulk_sourcep/Nvtm)*d10_exp0;
evbd_Vdrainp_sourcep=(vbd_Vdrainp_sourcep/Nvtm)*d10_exp0;
#endif
evbd=d00_exp0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbd_Vbulk_sourcep=((DrainSatCurrent*evbd_Vbulk_sourcep)/Nvtm);
gbd_Vdrainp_sourcep=((DrainSatCurrent*evbd_Vdrainp_sourcep)/Nvtm);
#endif
gbd=(((DrainSatCurrent*evbd)/Nvtm)+GMIN);
#endif
#if defined(_DERIVATE)
cbd_Vbulk_sourcep=((DrainSatCurrent*evbd_Vbulk_sourcep)+(GMIN*vbd_Vbulk_sourcep));
cbd_Vdrainp_sourcep=((DrainSatCurrent*evbd_Vdrainp_sourcep)+(GMIN*vbd_Vdrainp_sourcep));
#endif
cbd=((DrainSatCurrent*(evbd-1.0))+(GMIN*vbd));
}
else
{
if
((VERSION==3.24))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(IsEvjdm/Nvtm);
#if defined(_DERIVATE)
cbd_Vbulk_sourcep=((GMIN*vbd_Vbulk_sourcep)+((T_0_Vbulk_sourcep*(vbd-vjdm))+(T_0*vbd_Vbulk_sourcep)));
cbd_Vdrainp_sourcep=((GMIN*vbd_Vdrainp_sourcep)+((T_0_Vdrainp_sourcep*(vbd-vjdm))+(T_0*vbd_Vdrainp_sourcep)));
#endif
cbd=(((IsEvjdm-DrainSatCurrent)+(GMIN*vbd))+(T_0*(vbd-vjdm)));
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((DrainSatCurrent+IJTH)/Nvtm);
#if defined(_DERIVATE)
cbd_Vbulk_sourcep=((GMIN*vbd_Vbulk_sourcep)+((T_0_Vbulk_sourcep*(vbd-vjdm))+(T_0*vbd_Vbulk_sourcep)));
cbd_Vdrainp_sourcep=((GMIN*vbd_Vdrainp_sourcep)+((T_0_Vdrainp_sourcep*(vbd-vjdm))+(T_0*vbd_Vdrainp_sourcep)));
#endif
cbd=((IJTH+(GMIN*vbd))+(T_0*(vbd-vjdm)));
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
gbd_Vbulk_sourcep=0.0;
gbd_Vdrainp_sourcep=0.0;
#endif
gbd=(T_0+GMIN);
#endif
}
}
}
if
((vds>=0.0))
{
mode=1;
#if defined(_DERIVATE)
Vds_Vdrainp_sourcep=vds_Vdrainp_sourcep;
#endif
Vds=vds;
#if defined(_DERIVATE)
Vgs_Vdrainp_sourcep=0.0;
Vgs_Vgate_sourcep=vgs_Vgate_sourcep;
#endif
Vgs=vgs;
#if defined(_DERIVATE)
Vbs_Vdrainp_sourcep=0.0;
Vbs_Vbulk_sourcep=vbs_Vbulk_sourcep;
#endif
Vbs=vbs;
}
else
{
mode=(-1);
#if defined(_DERIVATE)
Vds_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
Vds=(-vds);
#if defined(_DERIVATE)
Vgs_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
Vgs_Vgate_sourcep=vgd_Vgate_sourcep;
#endif
Vgs=vgd;
#if defined(_DERIVATE)
Vbs_Vdrainp_sourcep=vbd_Vdrainp_sourcep;
Vbs_Vbulk_sourcep=vbd_Vbulk_sourcep;
#endif
Vbs=vbd;
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=Vbs_Vdrainp_sourcep;
T_0_Vbulk_sourcep=Vbs_Vbulk_sourcep;
#endif
T_0=((Vbs-vbsc)-0.001);
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)-(0.004*vbsc)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)-(0.004*vbsc)))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))*d10_sqrt0;
T1_Vbulk_sourcep=((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#if defined(_DERIVATE)
Vbseff_Vdrainp_sourcep=(0.5*(T_0_Vdrainp_sourcep+T1_Vdrainp_sourcep));
Vbseff_Vbulk_sourcep=(0.5*(T_0_Vbulk_sourcep+T1_Vbulk_sourcep));
#endif
Vbseff=(vbsc+(0.5*(T_0+T1)));
if
((Vbseff<Vbs))
{
#if defined(_DERIVATE)
Vbseff_Vdrainp_sourcep=Vbs_Vdrainp_sourcep;
Vbseff_Vbulk_sourcep=Vbs_Vbulk_sourcep;
#endif
Vbseff=Vbs;
}
if
((Vbseff>0.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(-phi*Vbseff_Vdrainp_sourcep/(phi+Vbseff)/(phi+Vbseff));
T_0_Vbulk_sourcep=(-phi*Vbseff_Vbulk_sourcep/(phi+Vbseff)/(phi+Vbseff));
#endif
T_0=(phi/(phi+Vbseff));
#if defined(_DERIVATE)
Phis_Vdrainp_sourcep=(phi*T_0_Vdrainp_sourcep);
Phis_Vbulk_sourcep=(phi*T_0_Vbulk_sourcep);
#endif
Phis=(phi*T_0);
#if defined(_DERIVATE)
sqrtPhis_Vdrainp_sourcep=(-phis3*(0.5*Vbseff_Vdrainp_sourcep)/(phi+(0.5*Vbseff))/(phi+(0.5*Vbseff)));
sqrtPhis_Vbulk_sourcep=(-phis3*(0.5*Vbseff_Vbulk_sourcep)/(phi+(0.5*Vbseff))/(phi+(0.5*Vbseff)));
#endif
sqrtPhis=(phis3/(phi+(0.5*Vbseff)));
}
else
{
#if defined(_DERIVATE)
Phis_Vdrainp_sourcep=(-Vbseff_Vdrainp_sourcep);
Phis_Vbulk_sourcep=(-Vbseff_Vbulk_sourcep);
#endif
Phis=(phi-Vbseff);
{
double m00_sqrt(d00_sqrt0,Phis)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,Phis)
#endif
#if defined(_DERIVATE)
sqrtPhis_Vdrainp_sourcep=Phis_Vdrainp_sourcep*d10_sqrt0;
sqrtPhis_Vbulk_sourcep=Phis_Vbulk_sourcep*d10_sqrt0;
#endif
sqrtPhis=d00_sqrt0;
}
}
#if defined(_DERIVATE)
Xdep_Vdrainp_sourcep=((Xdep0*sqrtPhis_Vdrainp_sourcep)/sqrtPhi);
Xdep_Vbulk_sourcep=((Xdep0*sqrtPhis_Vbulk_sourcep)/sqrtPhi);
#endif
Xdep=((Xdep0*sqrtPhis)/sqrtPhi);
Leff=leff;
Vtm=vtm;
{
double m00_sqrt(d00_sqrt0,Xdep)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,Xdep)
#endif
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=Xdep_Vdrainp_sourcep*d10_sqrt0;
T3_Vbulk_sourcep=Xdep_Vbulk_sourcep*d10_sqrt0;
#endif
T3=d00_sqrt0;
}
V0=(vbi-phi);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(dvt2*Vbseff_Vdrainp_sourcep);
T_0_Vbulk_sourcep=(dvt2*Vbseff_Vbulk_sourcep);
#endif
T_0=(dvt2*Vbseff);
if
((T_0>=(-0.5)))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=T_0_Vdrainp_sourcep;
T1_Vbulk_sourcep=T_0_Vbulk_sourcep;
#endif
T1=(1.0+T_0);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=dvt2;
}
else
{
#if defined(_DERIVATE)
T4_Vgate_sourcep=0.0;
T4_Vdrainp_sourcep=(-(8.0*T_0_Vdrainp_sourcep)/(3.0+(8.0*T_0))/(3.0+(8.0*T_0)));
T4_Vbulk_sourcep=(-(8.0*T_0_Vbulk_sourcep)/(3.0+(8.0*T_0))/(3.0+(8.0*T_0)));
#endif
T4=(1.0/(3.0+(8.0*T_0)));
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=(((3.0*T_0_Vdrainp_sourcep)*T4)+((1.0+(3.0*T_0))*T4_Vdrainp_sourcep));
T1_Vbulk_sourcep=(((3.0*T_0_Vbulk_sourcep)*T4)+((1.0+(3.0*T_0))*T4_Vbulk_sourcep));
#endif
T1=((1.0+(3.0*T_0))*T4);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=(((dvt2*T4_Vdrainp_sourcep)*T4)+((dvt2*T4)*T4_Vdrainp_sourcep));
T2_Vbulk_sourcep=(((dvt2*T4_Vbulk_sourcep)*T4)+((dvt2*T4)*T4_Vbulk_sourcep));
#endif
T2=((dvt2*T4)*T4);
}
#if defined(_DERIVATE)
lt1_Vdrainp_sourcep=(((factor1*T3_Vdrainp_sourcep)*T1)+((factor1*T3)*T1_Vdrainp_sourcep));
lt1_Vbulk_sourcep=(((factor1*T3_Vbulk_sourcep)*T1)+((factor1*T3)*T1_Vbulk_sourcep));
#endif
lt1=((factor1*T3)*T1);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(dvt2w*Vbseff_Vdrainp_sourcep);
T_0_Vbulk_sourcep=(dvt2w*Vbseff_Vbulk_sourcep);
#endif
T_0=(dvt2w*Vbseff);
if
((T_0>=(-0.5)))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=T_0_Vdrainp_sourcep;
T1_Vbulk_sourcep=T_0_Vbulk_sourcep;
#endif
T1=(1.0+T_0);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=dvt2w;
}
else
{
#if defined(_DERIVATE)
T4_Vgate_sourcep=0.0;
T4_Vdrainp_sourcep=(-(8.0*T_0_Vdrainp_sourcep)/(3.0+(8.0*T_0))/(3.0+(8.0*T_0)));
T4_Vbulk_sourcep=(-(8.0*T_0_Vbulk_sourcep)/(3.0+(8.0*T_0))/(3.0+(8.0*T_0)));
#endif
T4=(1.0/(3.0+(8.0*T_0)));
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=(((3.0*T_0_Vdrainp_sourcep)*T4)+((1.0+(3.0*T_0))*T4_Vdrainp_sourcep));
T1_Vbulk_sourcep=(((3.0*T_0_Vbulk_sourcep)*T4)+((1.0+(3.0*T_0))*T4_Vbulk_sourcep));
#endif
T1=((1.0+(3.0*T_0))*T4);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=(((dvt2w*T4_Vdrainp_sourcep)*T4)+((dvt2w*T4)*T4_Vdrainp_sourcep));
T2_Vbulk_sourcep=(((dvt2w*T4_Vbulk_sourcep)*T4)+((dvt2w*T4)*T4_Vbulk_sourcep));
#endif
T2=((dvt2w*T4)*T4);
}
#if defined(_DERIVATE)
ltw_Vdrainp_sourcep=(((factor1*T3_Vdrainp_sourcep)*T1)+((factor1*T3)*T1_Vdrainp_sourcep));
ltw_Vbulk_sourcep=(((factor1*T3_Vbulk_sourcep)*T1)+((factor1*T3)*T1_Vbulk_sourcep));
#endif
ltw=((factor1*T3)*T1);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(-(((-0.5)*dvt1)*Leff)*lt1_Vdrainp_sourcep/lt1/lt1);
T_0_Vbulk_sourcep=(-(((-0.5)*dvt1)*Leff)*lt1_Vbulk_sourcep/lt1/lt1);
#endif
T_0=((((-0.5)*dvt1)*Leff)/lt1);
if
((T_0>(-34.0)))
{
{
double m00_exp(d00_exp0,T_0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=T_0_Vdrainp_sourcep*d10_exp0;
T1_Vbulk_sourcep=T_0_Vbulk_sourcep*d10_exp0;
#endif
T1=d00_exp0;
}
#if defined(_DERIVATE)
Theta0_Vdrainp_sourcep=((T1_Vdrainp_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vdrainp_sourcep)));
Theta0_Vbulk_sourcep=((T1_Vbulk_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vbulk_sourcep)));
#endif
Theta0=(T1*(1.0+(2.0*T1)));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=1.713908431e-15;
#if defined(_DERIVATE)
Theta0_Vdrainp_sourcep=((T1_Vdrainp_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vdrainp_sourcep)));
Theta0_Vbulk_sourcep=((T1_Vbulk_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vbulk_sourcep)));
#endif
Theta0=(T1*(1.0+(2.0*T1)));
}
#if defined(_DERIVATE)
thetavth_Vdrainp_sourcep=(dvt0*Theta0_Vdrainp_sourcep);
thetavth_Vbulk_sourcep=(dvt0*Theta0_Vbulk_sourcep);
#endif
thetavth=(dvt0*Theta0);
#if defined(_DERIVATE)
Delt_vth_Vdrainp_sourcep=thetavth_Vdrainp_sourcep*V0;
Delt_vth_Vbulk_sourcep=thetavth_Vbulk_sourcep*V0;
#endif
Delt_vth=(thetavth*V0);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(-((((-0.5)*dvt1w)*weff)*Leff)*ltw_Vdrainp_sourcep/ltw/ltw);
T_0_Vbulk_sourcep=(-((((-0.5)*dvt1w)*weff)*Leff)*ltw_Vbulk_sourcep/ltw/ltw);
#endif
T_0=(((((-0.5)*dvt1w)*weff)*Leff)/ltw);
if
((T_0>(-34.0)))
{
{
double m00_exp(d00_exp0,T_0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=T_0_Vdrainp_sourcep*d10_exp0;
T1_Vbulk_sourcep=T_0_Vbulk_sourcep*d10_exp0;
#endif
T1=d00_exp0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=((T1_Vdrainp_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vdrainp_sourcep)));
T2_Vbulk_sourcep=((T1_Vbulk_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vbulk_sourcep)));
#endif
T2=(T1*(1.0+(2.0*T1)));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=1.713908431e-15;
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=((T1_Vdrainp_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vdrainp_sourcep)));
T2_Vbulk_sourcep=((T1_Vbulk_sourcep*(1.0+(2.0*T1)))+(T1*(2.0*T1_Vbulk_sourcep)));
#endif
T2=(T1*(1.0+(2.0*T1)));
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(dvt0w*T2_Vdrainp_sourcep);
T_0_Vbulk_sourcep=(dvt0w*T2_Vbulk_sourcep);
#endif
T_0=(dvt0w*T2);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=T_0_Vdrainp_sourcep*V0;
T2_Vbulk_sourcep=T_0_Vbulk_sourcep*V0;
#endif
T2=(T_0*V0);
TempRatio=((temp/tnom)-1.0);
{
double m00_sqrt(d00_sqrt0,(1.0+(nlx/Leff)))
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=((k1ox*T_0_Vdrainp_sourcep)*sqrtPhi+(kt2*Vbseff_Vdrainp_sourcep)*TempRatio);
T1_Vbulk_sourcep=((k1ox*T_0_Vbulk_sourcep)*sqrtPhi+(kt2*Vbseff_Vbulk_sourcep)*TempRatio);
#endif
T1=(((k1ox*(T_0-1.0))*sqrtPhi)+(((kt1+(kt1l/Leff))+(kt2*Vbseff))*TempRatio));
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vbulk_sourcep=0.0;
#endif
tmp2=((tox*phi)/(weff+w0));
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=(etab*Vbseff_Vdrainp_sourcep);
T3_Vbulk_sourcep=(etab*Vbseff_Vbulk_sourcep);
#endif
T3=(eta0+(etab*Vbseff));
if
((T3<1.0e-4))
{
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=(-(-(2.0e4*T3_Vdrainp_sourcep))/(3.0-(2.0e4*T3))/(3.0-(2.0e4*T3)));
T9_Vbulk_sourcep=(-(-(2.0e4*T3_Vbulk_sourcep))/(3.0-(2.0e4*T3))/(3.0-(2.0e4*T3)));
#endif
T9=(1.0/(3.0-(2.0e4*T3)));
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=(((-T3_Vdrainp_sourcep)*T9)+((2.0e-4-T3)*T9_Vdrainp_sourcep));
T3_Vbulk_sourcep=(((-T3_Vbulk_sourcep)*T9)+((2.0e-4-T3)*T9_Vbulk_sourcep));
#endif
T3=((2.0e-4-T3)*T9);
#if defined(_DERIVATE)
T4_Vgate_sourcep=0.0;
T4_Vdrainp_sourcep=((T9_Vdrainp_sourcep*T9)+(T9*T9_Vdrainp_sourcep));
T4_Vbulk_sourcep=((T9_Vbulk_sourcep*T9)+(T9*T9_Vbulk_sourcep));
#endif
T4=(T9*T9);
}
else
{
#if defined(_DERIVATE)
T4_Vgate_sourcep=0.0;
T4_Vdrainp_sourcep=0.0;
T4_Vbulk_sourcep=0.0;
#endif
T4=1.0;
}
#if defined(_DERIVATE)
dDIBL_Sft_dVd_Vdrainp_sourcep=T3_Vdrainp_sourcep*theta0vb0;
dDIBL_Sft_dVd_Vbulk_sourcep=T3_Vbulk_sourcep*theta0vb0;
#endif
dDIBL_Sft_dVd=(T3*theta0vb0);
#if defined(_DERIVATE)
DIBL_Sft_Vdrainp_sourcep=((dDIBL_Sft_dVd_Vdrainp_sourcep*Vds)+(dDIBL_Sft_dVd*Vds_Vdrainp_sourcep));
DIBL_Sft_Vbulk_sourcep=dDIBL_Sft_dVd_Vbulk_sourcep*Vds;
#endif
DIBL_Sft=(dDIBL_Sft_dVd*Vds);
#if defined(_DERIVATE)
Vth_Vdrainp_sourcep=(((((((k1ox*sqrtPhis_Vdrainp_sourcep)-(k2ox*Vbseff_Vdrainp_sourcep))-Delt_vth_Vdrainp_sourcep)-T2_Vdrainp_sourcep)+(((k3b*Vbseff_Vdrainp_sourcep)*tmp2)+((k3+(k3b*Vbseff))*tmp2_Vdrainp_sourcep)))+T1_Vdrainp_sourcep)-DIBL_Sft_Vdrainp_sourcep);
Vth_Vbulk_sourcep=(((((((k1ox*sqrtPhis_Vbulk_sourcep)-(k2ox*Vbseff_Vbulk_sourcep))-Delt_vth_Vbulk_sourcep)-T2_Vbulk_sourcep)+(((k3b*Vbseff_Vbulk_sourcep)*tmp2)+((k3+(k3b*Vbseff))*tmp2_Vbulk_sourcep)))+T1_Vbulk_sourcep)-DIBL_Sft_Vbulk_sourcep);
#endif
Vth=(((((((((TYPE*vth0)-(k1*sqrtPhi))+(k1ox*sqrtPhis))-(k2ox*Vbseff))-Delt_vth)-T2)+((k3+(k3b*Vbseff))*tmp2))+T1)-DIBL_Sft);
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=0.0;
tmp2_Vdrainp_sourcep=(-(nfactor*1.03594e-10)*Xdep_Vdrainp_sourcep/Xdep/Xdep);
tmp2_Vbulk_sourcep=(-(nfactor*1.03594e-10)*Xdep_Vbulk_sourcep/Xdep/Xdep);
#endif
tmp2=((nfactor*1.03594e-10)/Xdep);
#if defined(_DERIVATE)
tmp3_Vdrainp_sourcep=((cdscb*Vbseff_Vdrainp_sourcep)+(cdscd*Vds_Vdrainp_sourcep));
tmp3_Vbulk_sourcep=(cdscb*Vbseff_Vbulk_sourcep);
#endif
tmp3=((cdsc+(cdscb*Vbseff))+(cdscd*Vds));
#if defined(_DERIVATE)
tmp4_Vgate_sourcep=0.0;
tmp4_Vdrainp_sourcep=((tmp2_Vdrainp_sourcep+((tmp3_Vdrainp_sourcep*Theta0)+(tmp3*Theta0_Vdrainp_sourcep)))/cox);
tmp4_Vbulk_sourcep=((tmp2_Vbulk_sourcep+((tmp3_Vbulk_sourcep*Theta0)+(tmp3*Theta0_Vbulk_sourcep)))/cox);
#endif
tmp4=(((tmp2+(tmp3*Theta0))+cit)/cox);
if
((tmp4>=(-0.5)))
{
#if defined(_DERIVATE)
n_Vdrainp_sourcep=tmp4_Vdrainp_sourcep;
n_Vbulk_sourcep=tmp4_Vbulk_sourcep;
#endif
n=(1.0+tmp4);
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(-(8.0*tmp4_Vdrainp_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
T_0_Vbulk_sourcep=(-(8.0*tmp4_Vbulk_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
#endif
T_0=(1.0/(3.0+(8.0*tmp4)));
#if defined(_DERIVATE)
n_Vdrainp_sourcep=(((3.0*tmp4_Vdrainp_sourcep)*T_0)+((1.0+(3.0*tmp4))*T_0_Vdrainp_sourcep));
n_Vbulk_sourcep=(((3.0*tmp4_Vbulk_sourcep)*T_0)+((1.0+(3.0*tmp4))*T_0_Vbulk_sourcep));
#endif
n=((1.0+(3.0*tmp4))*T_0);
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(vfb+phi);
if
((((ngate>1.0e18)&&(ngate<1.0e25))&&(Vgs>T_0)))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=((((1.0e6*1.60219e-19)*1.03594e-10)*ngate)/(cox*cox));
{
double m00_sqrt(d00_sqrt0,(1.0+((2.0*(Vgs-T_0))/T1)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+((2.0*(Vgs-T_0))/T1)))
#endif
#if defined(_DERIVATE)
T4_Vgate_sourcep=(((2.0*(Vgs_Vgate_sourcep-T_0_Vgate_sourcep))*T1-(2.0*(Vgs-T_0))*T1_Vgate_sourcep)/T1/T1)*d10_sqrt0;
T4_Vdrainp_sourcep=(((2.0*(Vgs_Vdrainp_sourcep-T_0_Vdrainp_sourcep))*T1-(2.0*(Vgs-T_0))*T1_Vdrainp_sourcep)/T1/T1)*d10_sqrt0;
T4_Vbulk_sourcep=(((2.0*(-T_0_Vbulk_sourcep))*T1-(2.0*(Vgs-T_0))*T1_Vbulk_sourcep)/T1/T1)*d10_sqrt0;
#endif
T4=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=((T1_Vgate_sourcep*(T4-1.0))+(T1*T4_Vgate_sourcep));
T2_Vdrainp_sourcep=((T1_Vdrainp_sourcep*(T4-1.0))+(T1*T4_Vdrainp_sourcep));
T2_Vbulk_sourcep=((T1_Vbulk_sourcep*(T4-1.0))+(T1*T4_Vbulk_sourcep));
#endif
T2=(T1*(T4-1.0));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((((0.5*T2_Vgate_sourcep)*T2)+((0.5*T2)*T2_Vgate_sourcep))*T1-((0.5*T2)*T2)*T1_Vgate_sourcep)/T1/T1);
T3_Vdrainp_sourcep=(((((0.5*T2_Vdrainp_sourcep)*T2)+((0.5*T2)*T2_Vdrainp_sourcep))*T1-((0.5*T2)*T2)*T1_Vdrainp_sourcep)/T1/T1);
T3_Vbulk_sourcep=(((((0.5*T2_Vbulk_sourcep)*T2)+((0.5*T2)*T2_Vbulk_sourcep))*T1-((0.5*T2)*T2)*T1_Vbulk_sourcep)/T1/T1);
#endif
T3=(((0.5*T2)*T2)/T1);
#if defined(_DERIVATE)
T7_Vgate_sourcep=(-T3_Vgate_sourcep);
T7_Vdrainp_sourcep=(-T3_Vdrainp_sourcep);
T7_Vbulk_sourcep=(-T3_Vbulk_sourcep);
#endif
T7=((1.12-T3)-0.05);
{
double m00_sqrt(d00_sqrt0,((T7*T7)+0.224))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T7*T7)+0.224))
#endif
#if defined(_DERIVATE)
T6_Vgate_sourcep=((T7_Vgate_sourcep*T7)+(T7*T7_Vgate_sourcep))*d10_sqrt0;
T6_Vdrainp_sourcep=((T7_Vdrainp_sourcep*T7)+(T7*T7_Vdrainp_sourcep))*d10_sqrt0;
T6_Vbulk_sourcep=((T7_Vbulk_sourcep*T7)+(T7*T7_Vbulk_sourcep))*d10_sqrt0;
#endif
T6=d00_sqrt0;
}
#if defined(_DERIVATE)
T5_Vgate_sourcep=(-(0.5*(T7_Vgate_sourcep+T6_Vgate_sourcep)));
T5_Vdrainp_sourcep=(-(0.5*(T7_Vdrainp_sourcep+T6_Vdrainp_sourcep)));
T5_Vbulk_sourcep=(-(0.5*(T7_Vbulk_sourcep+T6_Vbulk_sourcep)));
#endif
T5=(1.12-(0.5*(T7+T6)));
#if defined(_DERIVATE)
Vgs_eff_Vdrainp_sourcep=(Vgs_Vdrainp_sourcep-T5_Vdrainp_sourcep);
Vgs_eff_Vgate_sourcep=(Vgs_Vgate_sourcep-T5_Vgate_sourcep);
Vgs_eff_Vbulk_sourcep=(-T5_Vbulk_sourcep);
#endif
Vgs_eff=(Vgs-T5);
}
else
{
#if defined(_DERIVATE)
Vgs_eff_Vdrainp_sourcep=Vgs_Vdrainp_sourcep;
Vgs_eff_Vgate_sourcep=Vgs_Vgate_sourcep;
Vgs_eff_Vbulk_sourcep=0.0;
#endif
Vgs_eff=Vgs;
}
#if defined(_DERIVATE)
Vgst_Vdrainp_sourcep=(Vgs_eff_Vdrainp_sourcep-Vth_Vdrainp_sourcep);
Vgst_Vgate_sourcep=Vgs_eff_Vgate_sourcep;
Vgst_Vbulk_sourcep=(Vgs_eff_Vbulk_sourcep-Vth_Vbulk_sourcep);
#endif
Vgst=(Vgs_eff-Vth);
#if defined(_DERIVATE)
T10_Vgate_sourcep=0.0;
T10_Vdrainp_sourcep=(2.0*n_Vdrainp_sourcep)*Vtm;
T10_Vbulk_sourcep=(2.0*n_Vbulk_sourcep)*Vtm;
#endif
T10=((2.0*n)*Vtm);
#if defined(_DERIVATE)
VgstNVt_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*T10-Vgst*T10_Vdrainp_sourcep)/T10/T10);
VgstNVt_Vgate_sourcep=((Vgst_Vgate_sourcep*T10-Vgst*T10_Vgate_sourcep)/T10/T10);
VgstNVt_Vbulk_sourcep=((Vgst_Vbulk_sourcep*T10-Vgst*T10_Vbulk_sourcep)/T10/T10);
#endif
VgstNVt=(Vgst/T10);
#if defined(_DERIVATE)
ExpArg_Vdrainp_sourcep=(((-Vgst_Vdrainp_sourcep)*T10-((2.0*voff)-Vgst)*T10_Vdrainp_sourcep)/T10/T10);
ExpArg_Vgate_sourcep=(((-Vgst_Vgate_sourcep)*T10-((2.0*voff)-Vgst)*T10_Vgate_sourcep)/T10/T10);
ExpArg_Vbulk_sourcep=(((-Vgst_Vbulk_sourcep)*T10-((2.0*voff)-Vgst)*T10_Vbulk_sourcep)/T10/T10);
#endif
ExpArg=(((2.0*voff)-Vgst)/T10);
if
((VgstNVt>34.0))
{
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=Vgst_Vdrainp_sourcep;
Vgsteff_Vgate_sourcep=Vgst_Vgate_sourcep;
Vgsteff_Vbulk_sourcep=Vgst_Vbulk_sourcep;
#endif
Vgsteff=Vgst;
}
else
{
if
((ExpArg>34.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(Vgst_Vgate_sourcep/(n*Vtm));
T_0_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*(n*Vtm)-(Vgst-voff)*n_Vdrainp_sourcep*Vtm)/(n*Vtm)/(n*Vtm));
T_0_Vbulk_sourcep=((Vgst_Vbulk_sourcep*(n*Vtm)-(Vgst-voff)*n_Vbulk_sourcep*Vtm)/(n*Vtm)/(n*Vtm));
#endif
T_0=((Vgst-voff)/(n*Vtm));
{
double m00_exp(d00_exp0,T_0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ExpVgst_Vgate_sourcep=T_0_Vgate_sourcep*d10_exp0;
ExpVgst_Vdrainp_sourcep=T_0_Vdrainp_sourcep*d10_exp0;
ExpVgst_Vbulk_sourcep=T_0_Vbulk_sourcep*d10_exp0;
#endif
ExpVgst=d00_exp0;
}
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=(((Vtm*cdep0)/cox)*ExpVgst_Vdrainp_sourcep);
Vgsteff_Vgate_sourcep=(((Vtm*cdep0)/cox)*ExpVgst_Vgate_sourcep);
Vgsteff_Vbulk_sourcep=(((Vtm*cdep0)/cox)*ExpVgst_Vbulk_sourcep);
#endif
Vgsteff=(((Vtm*cdep0)/cox)*ExpVgst);
}
else
{
{
double m00_exp(d00_exp0,VgstNVt)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ExpVgst_Vgate_sourcep=VgstNVt_Vgate_sourcep*d10_exp0;
ExpVgst_Vdrainp_sourcep=VgstNVt_Vdrainp_sourcep*d10_exp0;
ExpVgst_Vbulk_sourcep=VgstNVt_Vbulk_sourcep*d10_exp0;
#endif
ExpVgst=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+ExpVgst))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+ExpVgst))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=((T10_Vgate_sourcep*d00_logE0)+(T10*ExpVgst_Vgate_sourcep*d10_logE0));
T1_Vdrainp_sourcep=((T10_Vdrainp_sourcep*d00_logE0)+(T10*ExpVgst_Vdrainp_sourcep*d10_logE0));
T1_Vbulk_sourcep=((T10_Vbulk_sourcep*d00_logE0)+(T10*ExpVgst_Vbulk_sourcep*d10_logE0));
#endif
T1=(T10*d00_logE0);
}
{
double m00_exp(d00_exp0,ExpArg)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-((T10_Vgate_sourcep*(((-cox)/(Vtm*cdep0))*d00_exp0))+(T10*(((-cox)/(Vtm*cdep0))*ExpArg_Vgate_sourcep*d10_exp0))));
T2_Vdrainp_sourcep=(-((T10_Vdrainp_sourcep*(((-cox)/(Vtm*cdep0))*d00_exp0))+(T10*(((-cox)/(Vtm*cdep0))*ExpArg_Vdrainp_sourcep*d10_exp0))));
T2_Vbulk_sourcep=(-((T10_Vbulk_sourcep*(((-cox)/(Vtm*cdep0))*d00_exp0))+(T10*(((-cox)/(Vtm*cdep0))*ExpArg_Vbulk_sourcep*d10_exp0))));
#endif
T2=(1.0-(T10*(((-cox)/(Vtm*cdep0))*d00_exp0)));
}
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=((T1_Vdrainp_sourcep*T2-T1*T2_Vdrainp_sourcep)/T2/T2);
Vgsteff_Vgate_sourcep=((T1_Vgate_sourcep*T2-T1*T2_Vgate_sourcep)/T2/T2);
Vgsteff_Vbulk_sourcep=((T1_Vbulk_sourcep*T2-T1*T2_Vbulk_sourcep)/T2/T2);
#endif
Vgsteff=(T1/T2);
}
}
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=sqrtPhis_Vdrainp_sourcep;
T9_Vbulk_sourcep=sqrtPhis_Vbulk_sourcep;
#endif
T9=(sqrtPhis-sqrtPhi);
#if defined(_DERIVATE)
Weff_Vdrainp_sourcep=(-(2.0*((dwg*Vgsteff_Vdrainp_sourcep)+(dwb*T9_Vdrainp_sourcep))));
Weff_Vgate_sourcep=(-(2.0*((dwg*Vgsteff_Vgate_sourcep)+(dwb*T9_Vgate_sourcep))));
Weff_Vbulk_sourcep=(-(2.0*((dwg*Vgsteff_Vbulk_sourcep)+(dwb*T9_Vbulk_sourcep))));
#endif
Weff=(weff-(2.0*((dwg*Vgsteff)+(dwb*T9))));
if
((Weff<2.0e-8))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(-(-(2.0*Weff_Vgate_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
T_0_Vdrainp_sourcep=(-(-(2.0*Weff_Vdrainp_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
T_0_Vbulk_sourcep=(-(-(2.0*Weff_Vbulk_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
#endif
T_0=(1.0/(6.0e-8-(2.0*Weff)));
#if defined(_DERIVATE)
Weff_Vdrainp_sourcep=(((2.0e-8*(-Weff_Vdrainp_sourcep))*T_0)+((2.0e-8*(4.0e-8-Weff))*T_0_Vdrainp_sourcep));
Weff_Vgate_sourcep=(((2.0e-8*(-Weff_Vgate_sourcep))*T_0)+((2.0e-8*(4.0e-8-Weff))*T_0_Vgate_sourcep));
Weff_Vbulk_sourcep=(((2.0e-8*(-Weff_Vbulk_sourcep))*T_0)+((2.0e-8*(4.0e-8-Weff))*T_0_Vbulk_sourcep));
#endif
Weff=((2.0e-8*(4.0e-8-Weff))*T_0);
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((prwg*Vgsteff_Vgate_sourcep)+(prwb*T9_Vgate_sourcep));
T_0_Vdrainp_sourcep=((prwg*Vgsteff_Vdrainp_sourcep)+(prwb*T9_Vdrainp_sourcep));
T_0_Vbulk_sourcep=((prwg*Vgsteff_Vbulk_sourcep)+(prwb*T9_Vbulk_sourcep));
#endif
T_0=((prwg*Vgsteff)+(prwb*T9));
if
((T_0>=(-0.9)))
{
#if defined(_DERIVATE)
Rds_Vgate_sourcep=(rds0*T_0_Vgate_sourcep);
Rds_Vdrainp_sourcep=(rds0*T_0_Vdrainp_sourcep);
Rds_Vbulk_sourcep=(rds0*T_0_Vbulk_sourcep);
#endif
Rds=(rds0*(1.0+T_0));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(-(20.0*T_0_Vgate_sourcep)/(17.0+(20.0*T_0))/(17.0+(20.0*T_0)));
T1_Vdrainp_sourcep=(-(20.0*T_0_Vdrainp_sourcep)/(17.0+(20.0*T_0))/(17.0+(20.0*T_0)));
T1_Vbulk_sourcep=(-(20.0*T_0_Vbulk_sourcep)/(17.0+(20.0*T_0))/(17.0+(20.0*T_0)));
#endif
T1=(1.0/(17.0+(20.0*T_0)));
#if defined(_DERIVATE)
Rds_Vgate_sourcep=(((rds0*T_0_Vgate_sourcep)*T1)+((rds0*(0.8+T_0))*T1_Vgate_sourcep));
Rds_Vdrainp_sourcep=(((rds0*T_0_Vdrainp_sourcep)*T1)+((rds0*(0.8+T_0))*T1_Vdrainp_sourcep));
Rds_Vbulk_sourcep=(((rds0*T_0_Vbulk_sourcep)*T1)+((rds0*(0.8+T_0))*T1_Vbulk_sourcep));
#endif
Rds=((rds0*(0.8+T_0))*T1);
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=(-(0.5*k1ox)*sqrtPhis_Vdrainp_sourcep/sqrtPhis/sqrtPhis);
T1_Vbulk_sourcep=(-(0.5*k1ox)*sqrtPhis_Vbulk_sourcep/sqrtPhis/sqrtPhis);
#endif
T1=((0.5*k1ox)/sqrtPhis);
{
double m00_sqrt(d00_sqrt0,(xj*Xdep))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(xj*Xdep))
#endif
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=(xj*Xdep_Vdrainp_sourcep)*d10_sqrt0;
T9_Vbulk_sourcep=(xj*Xdep_Vbulk_sourcep)*d10_sqrt0;
#endif
T9=d00_sqrt0;
}
#if defined(_DERIVATE)
tmp1_Vdrainp_sourcep=(2.0*T9_Vdrainp_sourcep);
tmp1_Vbulk_sourcep=(2.0*T9_Vbulk_sourcep);
#endif
tmp1=(Leff+(2.0*T9));
#if defined(_DERIVATE)
T5_Vgate_sourcep=0.0;
T5_Vdrainp_sourcep=(-Leff*tmp1_Vdrainp_sourcep/tmp1/tmp1);
T5_Vbulk_sourcep=(-Leff*tmp1_Vbulk_sourcep/tmp1/tmp1);
#endif
T5=(Leff/tmp1);
#if defined(_DERIVATE)
tmp2_Vgate_sourcep=(a0*T5_Vgate_sourcep);
tmp2_Vdrainp_sourcep=(a0*T5_Vdrainp_sourcep);
tmp2_Vbulk_sourcep=(a0*T5_Vbulk_sourcep);
#endif
tmp2=(a0*T5);
#if defined(_DERIVATE)
tmp3_Vdrainp_sourcep=0.0;
tmp3_Vbulk_sourcep=0.0;
#endif
tmp3=(weff+b1);
#if defined(_DERIVATE)
tmp4_Vgate_sourcep=0.0;
tmp4_Vdrainp_sourcep=(-b0*tmp3_Vdrainp_sourcep/tmp3/tmp3);
tmp4_Vbulk_sourcep=(-b0*tmp3_Vbulk_sourcep/tmp3/tmp3);
#endif
tmp4=(b0/tmp3);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(tmp2_Vgate_sourcep+tmp4_Vgate_sourcep);
T2_Vdrainp_sourcep=(tmp2_Vdrainp_sourcep+tmp4_Vdrainp_sourcep);
T2_Vbulk_sourcep=(tmp2_Vbulk_sourcep+tmp4_Vbulk_sourcep);
#endif
T2=(tmp2+tmp4);
#if defined(_DERIVATE)
T6_Vgate_sourcep=((T5_Vgate_sourcep*T5)+(T5*T5_Vgate_sourcep));
T6_Vdrainp_sourcep=((T5_Vdrainp_sourcep*T5)+(T5*T5_Vdrainp_sourcep));
T6_Vbulk_sourcep=((T5_Vbulk_sourcep*T5)+(T5*T5_Vbulk_sourcep));
#endif
T6=(T5*T5);
#if defined(_DERIVATE)
T7_Vgate_sourcep=((T5_Vgate_sourcep*T6)+(T5*T6_Vgate_sourcep));
T7_Vdrainp_sourcep=((T5_Vdrainp_sourcep*T6)+(T5*T6_Vdrainp_sourcep));
T7_Vbulk_sourcep=((T5_Vbulk_sourcep*T6)+(T5*T6_Vbulk_sourcep));
#endif
T7=(T5*T6);
#if defined(_DERIVATE)
Abulk0_Vgate_sourcep=((T1_Vgate_sourcep*T2)+(T1*T2_Vgate_sourcep));
Abulk0_Vdrainp_sourcep=((T1_Vdrainp_sourcep*T2)+(T1*T2_Vdrainp_sourcep));
Abulk0_Vbulk_sourcep=((T1_Vbulk_sourcep*T2)+(T1*T2_Vbulk_sourcep));
#endif
Abulk0=(1.0+(T1*T2));
#if defined(_DERIVATE)
T8_Vgate_sourcep=((ags*a0)*T7_Vgate_sourcep);
T8_Vdrainp_sourcep=((ags*a0)*T7_Vdrainp_sourcep);
T8_Vbulk_sourcep=((ags*a0)*T7_Vbulk_sourcep);
#endif
T8=((ags*a0)*T7);
#if defined(_DERIVATE)
Abulk_Vgate_sourcep=(Abulk0_Vgate_sourcep+(((((-T1_Vgate_sourcep)*T8)+((-T1)*T8_Vgate_sourcep))*Vgsteff)+(((-T1)*T8)*Vgsteff_Vgate_sourcep)));
Abulk_Vdrainp_sourcep=(Abulk0_Vdrainp_sourcep+(((((-T1_Vdrainp_sourcep)*T8)+((-T1)*T8_Vdrainp_sourcep))*Vgsteff)+(((-T1)*T8)*Vgsteff_Vdrainp_sourcep)));
Abulk_Vbulk_sourcep=(Abulk0_Vbulk_sourcep+(((((-T1_Vbulk_sourcep)*T8)+((-T1)*T8_Vbulk_sourcep))*Vgsteff)+(((-T1)*T8)*Vgsteff_Vbulk_sourcep)));
#endif
Abulk=(Abulk0+(((-T1)*T8)*Vgsteff));
if
((Abulk0<0.1))
{
#if defined(_DERIVATE)
T9_Vgate_sourcep=(-(-(20.0*Abulk0_Vgate_sourcep))/(3.0-(20.0*Abulk0))/(3.0-(20.0*Abulk0)));
T9_Vdrainp_sourcep=(-(-(20.0*Abulk0_Vdrainp_sourcep))/(3.0-(20.0*Abulk0))/(3.0-(20.0*Abulk0)));
T9_Vbulk_sourcep=(-(-(20.0*Abulk0_Vbulk_sourcep))/(3.0-(20.0*Abulk0))/(3.0-(20.0*Abulk0)));
#endif
T9=(1.0/(3.0-(20.0*Abulk0)));
#if defined(_DERIVATE)
Abulk0_Vgate_sourcep=(((-Abulk0_Vgate_sourcep)*T9)+((0.2-Abulk0)*T9_Vgate_sourcep));
Abulk0_Vdrainp_sourcep=(((-Abulk0_Vdrainp_sourcep)*T9)+((0.2-Abulk0)*T9_Vdrainp_sourcep));
Abulk0_Vbulk_sourcep=(((-Abulk0_Vbulk_sourcep)*T9)+((0.2-Abulk0)*T9_Vbulk_sourcep));
#endif
Abulk0=((0.2-Abulk0)*T9);
}
if
((Abulk<0.1))
{
#if defined(_DERIVATE)
T9_Vgate_sourcep=(-(-(20.0*Abulk_Vgate_sourcep))/(3.0-(20.0*Abulk))/(3.0-(20.0*Abulk)));
T9_Vdrainp_sourcep=(-(-(20.0*Abulk_Vdrainp_sourcep))/(3.0-(20.0*Abulk))/(3.0-(20.0*Abulk)));
T9_Vbulk_sourcep=(-(-(20.0*Abulk_Vbulk_sourcep))/(3.0-(20.0*Abulk))/(3.0-(20.0*Abulk)));
#endif
T9=(1.0/(3.0-(20.0*Abulk)));
#if defined(_DERIVATE)
Abulk_Vgate_sourcep=(((-Abulk_Vgate_sourcep)*T9)+((0.2-Abulk)*T9_Vgate_sourcep));
Abulk_Vdrainp_sourcep=(((-Abulk_Vdrainp_sourcep)*T9)+((0.2-Abulk)*T9_Vdrainp_sourcep));
Abulk_Vbulk_sourcep=(((-Abulk_Vbulk_sourcep)*T9)+((0.2-Abulk)*T9_Vbulk_sourcep));
#endif
Abulk=((0.2-Abulk)*T9);
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=(keta*Vbseff_Vdrainp_sourcep);
T2_Vbulk_sourcep=(keta*Vbseff_Vbulk_sourcep);
#endif
T2=(keta*Vbseff);
if
((T2>=(-0.9)))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(-T2_Vgate_sourcep/(1.0+T2)/(1.0+T2));
T_0_Vdrainp_sourcep=(-T2_Vdrainp_sourcep/(1.0+T2)/(1.0+T2));
T_0_Vbulk_sourcep=(-T2_Vbulk_sourcep/(1.0+T2)/(1.0+T2));
#endif
T_0=(1.0/(1.0+T2));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(-T2_Vgate_sourcep/(0.8+T2)/(0.8+T2));
T1_Vdrainp_sourcep=(-T2_Vdrainp_sourcep/(0.8+T2)/(0.8+T2));
T1_Vbulk_sourcep=(-T2_Vbulk_sourcep/(0.8+T2)/(0.8+T2));
#endif
T1=(1.0/(0.8+T2));
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(((20.0*T2_Vgate_sourcep)*T1)+((17.0+(20.0*T2))*T1_Vgate_sourcep));
T_0_Vdrainp_sourcep=(((20.0*T2_Vdrainp_sourcep)*T1)+((17.0+(20.0*T2))*T1_Vdrainp_sourcep));
T_0_Vbulk_sourcep=(((20.0*T2_Vbulk_sourcep)*T1)+((17.0+(20.0*T2))*T1_Vbulk_sourcep));
#endif
T_0=((17.0+(20.0*T2))*T1);
}
#if defined(_DERIVATE)
Abulk_Vgate_sourcep=((T_0_Vgate_sourcep*Abulk)+(T_0*Abulk_Vgate_sourcep));
Abulk_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*Abulk)+(T_0*Abulk_Vdrainp_sourcep));
Abulk_Vbulk_sourcep=((T_0_Vbulk_sourcep*Abulk)+(T_0*Abulk_Vbulk_sourcep));
#endif
Abulk=(T_0*Abulk);
#if defined(_DERIVATE)
Abulk0_Vgate_sourcep=((T_0_Vgate_sourcep*Abulk0)+(T_0*Abulk0_Vgate_sourcep));
Abulk0_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*Abulk0)+(T_0*Abulk0_Vdrainp_sourcep));
Abulk0_Vbulk_sourcep=((T_0_Vbulk_sourcep*Abulk0)+(T_0*Abulk0_Vbulk_sourcep));
#endif
Abulk0=(T_0*Abulk0);
if
((MOBMOD==1))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=Vgsteff_Vgate_sourcep;
T_0_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep+Vth_Vdrainp_sourcep)+Vth_Vdrainp_sourcep);
T_0_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep+Vth_Vbulk_sourcep)+Vth_Vbulk_sourcep);
#endif
T_0=((Vgsteff+Vth)+Vth);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=(uc*Vbseff_Vdrainp_sourcep);
T2_Vbulk_sourcep=(uc*Vbseff_Vbulk_sourcep);
#endif
T2=(ua+(uc*Vbseff));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(T_0_Vgate_sourcep/tox);
T3_Vdrainp_sourcep=(T_0_Vdrainp_sourcep/tox);
T3_Vbulk_sourcep=(T_0_Vbulk_sourcep/tox);
#endif
T3=(T_0/tox);
#if defined(_DERIVATE)
T5_Vgate_sourcep=((T3_Vgate_sourcep*(T2+(ub*T3)))+(T3*(T2_Vgate_sourcep+(ub*T3_Vgate_sourcep))));
T5_Vdrainp_sourcep=((T3_Vdrainp_sourcep*(T2+(ub*T3)))+(T3*(T2_Vdrainp_sourcep+(ub*T3_Vdrainp_sourcep))));
T5_Vbulk_sourcep=((T3_Vbulk_sourcep*(T2+(ub*T3)))+(T3*(T2_Vbulk_sourcep+(ub*T3_Vbulk_sourcep))));
#endif
T5=(T3*(T2+(ub*T3)));
}
else
{
if
((MOBMOD==2))
{
#if defined(_DERIVATE)
T5_Vgate_sourcep=(((Vgsteff_Vgate_sourcep/tox)*((ua+(uc*Vbseff))+((ub*Vgsteff)/tox)))+((Vgsteff/tox)*((ub*Vgsteff_Vgate_sourcep)/tox)));
T5_Vdrainp_sourcep=(((Vgsteff_Vdrainp_sourcep/tox)*((ua+(uc*Vbseff))+((ub*Vgsteff)/tox)))+((Vgsteff/tox)*((uc*Vbseff_Vdrainp_sourcep)+((ub*Vgsteff_Vdrainp_sourcep)/tox))));
T5_Vbulk_sourcep=(((Vgsteff_Vbulk_sourcep/tox)*((ua+(uc*Vbseff))+((ub*Vgsteff)/tox)))+((Vgsteff/tox)*((uc*Vbseff_Vbulk_sourcep)+((ub*Vgsteff_Vbulk_sourcep)/tox))));
#endif
T5=((Vgsteff/tox)*((ua+(uc*Vbseff))+((ub*Vgsteff)/tox)));
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=Vgsteff_Vgate_sourcep;
T_0_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep+Vth_Vdrainp_sourcep)+Vth_Vdrainp_sourcep);
T_0_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep+Vth_Vbulk_sourcep)+Vth_Vbulk_sourcep);
#endif
T_0=((Vgsteff+Vth)+Vth);
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=(uc*Vbseff_Vdrainp_sourcep);
T2_Vbulk_sourcep=(uc*Vbseff_Vbulk_sourcep);
#endif
T2=(1.0+(uc*Vbseff));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(T_0_Vgate_sourcep/tox);
T3_Vdrainp_sourcep=(T_0_Vdrainp_sourcep/tox);
T3_Vbulk_sourcep=(T_0_Vbulk_sourcep/tox);
#endif
T3=(T_0/tox);
#if defined(_DERIVATE)
T4_Vgate_sourcep=((T3_Vgate_sourcep*(ua+(ub*T3)))+(T3*(ub*T3_Vgate_sourcep)));
T4_Vdrainp_sourcep=((T3_Vdrainp_sourcep*(ua+(ub*T3)))+(T3*(ub*T3_Vdrainp_sourcep)));
T4_Vbulk_sourcep=((T3_Vbulk_sourcep*(ua+(ub*T3)))+(T3*(ub*T3_Vbulk_sourcep)));
#endif
T4=(T3*(ua+(ub*T3)));
#if defined(_DERIVATE)
T5_Vgate_sourcep=((T4_Vgate_sourcep*T2)+(T4*T2_Vgate_sourcep));
T5_Vdrainp_sourcep=((T4_Vdrainp_sourcep*T2)+(T4*T2_Vdrainp_sourcep));
T5_Vbulk_sourcep=((T4_Vbulk_sourcep*T2)+(T4*T2_Vbulk_sourcep));
#endif
T5=(T4*T2);
}
}
if
((T5>=(-0.8)))
{
#if defined(_DERIVATE)
Denomi_Vgate_sourcep=T5_Vgate_sourcep;
Denomi_Vdrainp_sourcep=T5_Vdrainp_sourcep;
Denomi_Vbulk_sourcep=T5_Vbulk_sourcep;
#endif
Denomi=(1.0+T5);
}
else
{
#if defined(_DERIVATE)
Denomi_Vgate_sourcep=((T5_Vgate_sourcep*(1.0/(7.0+(10.0*T5))))+((0.6+T5)*(-(10.0*T5_Vgate_sourcep)/(7.0+(10.0*T5))/(7.0+(10.0*T5)))));
Denomi_Vdrainp_sourcep=((T5_Vdrainp_sourcep*(1.0/(7.0+(10.0*T5))))+((0.6+T5)*(-(10.0*T5_Vdrainp_sourcep)/(7.0+(10.0*T5))/(7.0+(10.0*T5)))));
Denomi_Vbulk_sourcep=((T5_Vbulk_sourcep*(1.0/(7.0+(10.0*T5))))+((0.6+T5)*(-(10.0*T5_Vbulk_sourcep)/(7.0+(10.0*T5))/(7.0+(10.0*T5)))));
#endif
Denomi=((0.6+T5)*(1.0/(7.0+(10.0*T5))));
}
#if defined(_DERIVATE)
ueff_Vgate_sourcep=(-u0temp*Denomi_Vgate_sourcep/Denomi/Denomi);
ueff_Vdrainp_sourcep=(-u0temp*Denomi_Vdrainp_sourcep/Denomi/Denomi);
ueff_Vbulk_sourcep=(-u0temp*Denomi_Vbulk_sourcep/Denomi/Denomi);
#endif
ueff=(u0temp/Denomi);
#if defined(_DERIVATE)
WVCox_Vdrainp_sourcep=Weff_Vdrainp_sourcep*vsattemp*cox;
WVCox_Vgate_sourcep=Weff_Vgate_sourcep*vsattemp*cox;
WVCox_Vbulk_sourcep=Weff_Vbulk_sourcep*vsattemp*cox;
#endif
WVCox=((Weff*vsattemp)*cox);
#if defined(_DERIVATE)
WVCoxRds_Vdrainp_sourcep=((WVCox_Vdrainp_sourcep*Rds)+(WVCox*Rds_Vdrainp_sourcep));
WVCoxRds_Vgate_sourcep=((WVCox_Vgate_sourcep*Rds)+(WVCox*Rds_Vgate_sourcep));
WVCoxRds_Vbulk_sourcep=((WVCox_Vbulk_sourcep*Rds)+(WVCox*Rds_Vbulk_sourcep));
#endif
WVCoxRds=(WVCox*Rds);
#if defined(_DERIVATE)
Esat_Vgate_sourcep=(-(2.0*vsattemp)*ueff_Vgate_sourcep/ueff/ueff);
Esat_Vdrainp_sourcep=(-(2.0*vsattemp)*ueff_Vdrainp_sourcep/ueff/ueff);
Esat_Vbulk_sourcep=(-(2.0*vsattemp)*ueff_Vbulk_sourcep/ueff/ueff);
#endif
Esat=((2.0*vsattemp)/ueff);
#if defined(_DERIVATE)
EsatL_Vgate_sourcep=Esat_Vgate_sourcep*Leff;
EsatL_Vdrainp_sourcep=Esat_Vdrainp_sourcep*Leff;
EsatL_Vbulk_sourcep=Esat_Vbulk_sourcep*Leff;
#endif
EsatL=(Esat*Leff);
if
((a1==0.0))
{
#if defined(_DERIVATE)
Lambda_Vgate_sourcep=0.0;
Lambda_Vdrainp_sourcep=0.0;
Lambda_Vbulk_sourcep=0.0;
#endif
Lambda=a2;
}
else
{
if
((a1>0.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(1.0-a2);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(T_0_Vgate_sourcep-(a1*Vgsteff_Vgate_sourcep));
T1_Vdrainp_sourcep=(T_0_Vdrainp_sourcep-(a1*Vgsteff_Vdrainp_sourcep));
T1_Vbulk_sourcep=(T_0_Vbulk_sourcep-(a1*Vgsteff_Vbulk_sourcep));
#endif
T1=((T_0-(a1*Vgsteff))-0.0001);
{
double m00_sqrt(d00_sqrt0,((T1*T1)+(0.0004*T_0)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T1*T1)+(0.0004*T_0)))
#endif
#if defined(_DERIVATE)
T2_Vgate_sourcep=(((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep))+(0.0004*T_0_Vgate_sourcep))*d10_sqrt0;
T2_Vdrainp_sourcep=(((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep))+(0.0004*T_0_Vdrainp_sourcep))*d10_sqrt0;
T2_Vbulk_sourcep=(((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep))+(0.0004*T_0_Vbulk_sourcep))*d10_sqrt0;
#endif
T2=d00_sqrt0;
}
#if defined(_DERIVATE)
Lambda_Vgate_sourcep=(T_0_Vgate_sourcep-(0.5*(T1_Vgate_sourcep+T2_Vgate_sourcep)));
Lambda_Vdrainp_sourcep=(T_0_Vdrainp_sourcep-(0.5*(T1_Vdrainp_sourcep+T2_Vdrainp_sourcep)));
Lambda_Vbulk_sourcep=(T_0_Vbulk_sourcep-(0.5*(T1_Vbulk_sourcep+T2_Vbulk_sourcep)));
#endif
Lambda=((a2+T_0)-(0.5*(T1+T2)));
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(a1*Vgsteff_Vgate_sourcep);
T1_Vdrainp_sourcep=(a1*Vgsteff_Vdrainp_sourcep);
T1_Vbulk_sourcep=(a1*Vgsteff_Vbulk_sourcep);
#endif
T1=((a2+(a1*Vgsteff))-0.0001);
{
double m00_sqrt(d00_sqrt0,((T1*T1)+(0.0004*a2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T1*T1)+(0.0004*a2)))
#endif
#if defined(_DERIVATE)
T2_Vgate_sourcep=((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep))*d10_sqrt0;
T2_Vdrainp_sourcep=((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep))*d10_sqrt0;
T2_Vbulk_sourcep=((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep))*d10_sqrt0;
#endif
T2=d00_sqrt0;
}
#if defined(_DERIVATE)
Lambda_Vgate_sourcep=(0.5*(T1_Vgate_sourcep+T2_Vgate_sourcep));
Lambda_Vdrainp_sourcep=(0.5*(T1_Vdrainp_sourcep+T2_Vdrainp_sourcep));
Lambda_Vbulk_sourcep=(0.5*(T1_Vbulk_sourcep+T2_Vbulk_sourcep));
#endif
Lambda=(0.5*(T1+T2));
}
}
#if defined(_DERIVATE)
Vgst2Vtm_Vdrainp_sourcep=Vgsteff_Vdrainp_sourcep;
Vgst2Vtm_Vgate_sourcep=Vgsteff_Vgate_sourcep;
Vgst2Vtm_Vbulk_sourcep=Vgsteff_Vbulk_sourcep;
#endif
Vgst2Vtm=(Vgsteff+(2.0*Vtm));
if
(((Rds==0.0)&&(Lambda==1.0)))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(-(((Abulk_Vgate_sourcep*EsatL)+(Abulk*EsatL_Vgate_sourcep))+Vgst2Vtm_Vgate_sourcep)/((Abulk*EsatL)+Vgst2Vtm)/((Abulk*EsatL)+Vgst2Vtm));
T_0_Vdrainp_sourcep=(-(((Abulk_Vdrainp_sourcep*EsatL)+(Abulk*EsatL_Vdrainp_sourcep))+Vgst2Vtm_Vdrainp_sourcep)/((Abulk*EsatL)+Vgst2Vtm)/((Abulk*EsatL)+Vgst2Vtm));
T_0_Vbulk_sourcep=(-(((Abulk_Vbulk_sourcep*EsatL)+(Abulk*EsatL_Vbulk_sourcep))+Vgst2Vtm_Vbulk_sourcep)/((Abulk*EsatL)+Vgst2Vtm)/((Abulk*EsatL)+Vgst2Vtm));
#endif
T_0=(1.0/((Abulk*EsatL)+Vgst2Vtm));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vbulk_sourcep=0.0;
#endif
tmp1=0.0;
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep));
T1_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep));
T1_Vbulk_sourcep=((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep));
#endif
T1=(T_0*T_0);
#if defined(_DERIVATE)
T2_Vgate_sourcep=((Vgst2Vtm_Vgate_sourcep*T_0)+(Vgst2Vtm*T_0_Vgate_sourcep));
T2_Vdrainp_sourcep=((Vgst2Vtm_Vdrainp_sourcep*T_0)+(Vgst2Vtm*T_0_Vdrainp_sourcep));
T2_Vbulk_sourcep=((Vgst2Vtm_Vbulk_sourcep*T_0)+(Vgst2Vtm*T_0_Vbulk_sourcep));
#endif
T2=(Vgst2Vtm*T_0);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((EsatL_Vgate_sourcep*Vgst2Vtm)+(EsatL*Vgst2Vtm_Vgate_sourcep));
T3_Vdrainp_sourcep=((EsatL_Vdrainp_sourcep*Vgst2Vtm)+(EsatL*Vgst2Vtm_Vdrainp_sourcep));
T3_Vbulk_sourcep=((EsatL_Vbulk_sourcep*Vgst2Vtm)+(EsatL*Vgst2Vtm_Vbulk_sourcep));
#endif
T3=(EsatL*Vgst2Vtm);
#if defined(_DERIVATE)
Vdsat_Vgate_sourcep=((T3_Vgate_sourcep*T_0)+(T3*T_0_Vgate_sourcep));
Vdsat_Vdrainp_sourcep=((T3_Vdrainp_sourcep*T_0)+(T3*T_0_Vdrainp_sourcep));
Vdsat_Vbulk_sourcep=((T3_Vbulk_sourcep*T_0)+(T3*T_0_Vbulk_sourcep));
#endif
Vdsat=(T3*T_0);
}
else
{
#if defined(_DERIVATE)
T9_Vgate_sourcep=((Abulk_Vgate_sourcep*WVCoxRds)+(Abulk*WVCoxRds_Vgate_sourcep));
T9_Vdrainp_sourcep=((Abulk_Vdrainp_sourcep*WVCoxRds)+(Abulk*WVCoxRds_Vdrainp_sourcep));
T9_Vbulk_sourcep=((Abulk_Vbulk_sourcep*WVCoxRds)+(Abulk*WVCoxRds_Vbulk_sourcep));
#endif
T9=(Abulk*WVCoxRds);
#if defined(_DERIVATE)
T7_Vgate_sourcep=((Vgst2Vtm_Vgate_sourcep*T9)+(Vgst2Vtm*T9_Vgate_sourcep));
T7_Vdrainp_sourcep=((Vgst2Vtm_Vdrainp_sourcep*T9)+(Vgst2Vtm*T9_Vdrainp_sourcep));
T7_Vbulk_sourcep=((Vgst2Vtm_Vbulk_sourcep*T9)+(Vgst2Vtm*T9_Vbulk_sourcep));
#endif
T7=(Vgst2Vtm*T9);
#if defined(_DERIVATE)
T6_Vgate_sourcep=((Vgst2Vtm_Vgate_sourcep*WVCoxRds)+(Vgst2Vtm*WVCoxRds_Vgate_sourcep));
T6_Vdrainp_sourcep=((Vgst2Vtm_Vdrainp_sourcep*WVCoxRds)+(Vgst2Vtm*WVCoxRds_Vdrainp_sourcep));
T6_Vbulk_sourcep=((Vgst2Vtm_Vbulk_sourcep*WVCoxRds)+(Vgst2Vtm*WVCoxRds_Vbulk_sourcep));
#endif
T6=(Vgst2Vtm*WVCoxRds);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(((2.0*Abulk_Vgate_sourcep)*((T9-1.0)+(1.0/Lambda)))+((2.0*Abulk)*(T9_Vgate_sourcep+(-Lambda_Vgate_sourcep/Lambda/Lambda))));
T_0_Vdrainp_sourcep=(((2.0*Abulk_Vdrainp_sourcep)*((T9-1.0)+(1.0/Lambda)))+((2.0*Abulk)*(T9_Vdrainp_sourcep+(-Lambda_Vdrainp_sourcep/Lambda/Lambda))));
T_0_Vbulk_sourcep=(((2.0*Abulk_Vbulk_sourcep)*((T9-1.0)+(1.0/Lambda)))+((2.0*Abulk)*(T9_Vbulk_sourcep+(-Lambda_Vbulk_sourcep/Lambda/Lambda))));
#endif
T_0=((2.0*Abulk)*((T9-1.0)+(1.0/Lambda)));
#if defined(_DERIVATE)
T1_Vgate_sourcep=((((Vgst2Vtm_Vgate_sourcep*((2.0/Lambda)-1.0))+(Vgst2Vtm*(-2.0*Lambda_Vgate_sourcep/Lambda/Lambda)))+((Abulk_Vgate_sourcep*EsatL)+(Abulk*EsatL_Vgate_sourcep)))+(3.0*T7_Vgate_sourcep));
T1_Vdrainp_sourcep=((((Vgst2Vtm_Vdrainp_sourcep*((2.0/Lambda)-1.0))+(Vgst2Vtm*(-2.0*Lambda_Vdrainp_sourcep/Lambda/Lambda)))+((Abulk_Vdrainp_sourcep*EsatL)+(Abulk*EsatL_Vdrainp_sourcep)))+(3.0*T7_Vdrainp_sourcep));
T1_Vbulk_sourcep=((((Vgst2Vtm_Vbulk_sourcep*((2.0/Lambda)-1.0))+(Vgst2Vtm*(-2.0*Lambda_Vbulk_sourcep/Lambda/Lambda)))+((Abulk_Vbulk_sourcep*EsatL)+(Abulk*EsatL_Vbulk_sourcep)))+(3.0*T7_Vbulk_sourcep));
#endif
T1=(((Vgst2Vtm*((2.0/Lambda)-1.0))+(Abulk*EsatL))+(3.0*T7));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((Vgst2Vtm_Vgate_sourcep*(EsatL+(2.0*T6)))+(Vgst2Vtm*(EsatL_Vgate_sourcep+(2.0*T6_Vgate_sourcep))));
T2_Vdrainp_sourcep=((Vgst2Vtm_Vdrainp_sourcep*(EsatL+(2.0*T6)))+(Vgst2Vtm*(EsatL_Vdrainp_sourcep+(2.0*T6_Vdrainp_sourcep))));
T2_Vbulk_sourcep=((Vgst2Vtm_Vbulk_sourcep*(EsatL+(2.0*T6)))+(Vgst2Vtm*(EsatL_Vbulk_sourcep+(2.0*T6_Vbulk_sourcep))));
#endif
T2=(Vgst2Vtm*(EsatL+(2.0*T6)));
{
double m00_sqrt(d00_sqrt0,((T1*T1)-((2.0*T_0)*T2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T1*T1)-((2.0*T_0)*T2)))
#endif
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep))-(((2.0*T_0_Vgate_sourcep)*T2)+((2.0*T_0)*T2_Vgate_sourcep)))*d10_sqrt0;
T3_Vdrainp_sourcep=(((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep))-(((2.0*T_0_Vdrainp_sourcep)*T2)+((2.0*T_0)*T2_Vdrainp_sourcep)))*d10_sqrt0;
T3_Vbulk_sourcep=(((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep))-(((2.0*T_0_Vbulk_sourcep)*T2)+((2.0*T_0)*T2_Vbulk_sourcep)))*d10_sqrt0;
#endif
T3=d00_sqrt0;
}
#if defined(_DERIVATE)
Vdsat_Vgate_sourcep=(((T1_Vgate_sourcep-T3_Vgate_sourcep)*T_0-(T1-T3)*T_0_Vgate_sourcep)/T_0/T_0);
Vdsat_Vdrainp_sourcep=(((T1_Vdrainp_sourcep-T3_Vdrainp_sourcep)*T_0-(T1-T3)*T_0_Vdrainp_sourcep)/T_0/T_0);
Vdsat_Vbulk_sourcep=(((T1_Vbulk_sourcep-T3_Vbulk_sourcep)*T_0-(T1-T3)*T_0_Vbulk_sourcep)/T_0/T_0);
#endif
Vdsat=((T1-T3)/T_0);
}
#if defined(_DYNAMIC)
vdsat=Vdsat;
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=Vdsat_Vgate_sourcep;
T1_Vdrainp_sourcep=(Vdsat_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
T1_Vbulk_sourcep=Vdsat_Vbulk_sourcep;
#endif
T1=((Vdsat-Vds)-delta);
{
double m00_sqrt(d00_sqrt0,((T1*T1)+((4.0*delta)*Vdsat)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T1*T1)+((4.0*delta)*Vdsat)))
#endif
#if defined(_DERIVATE)
T2_Vgate_sourcep=(((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep))+((4.0*delta)*Vdsat_Vgate_sourcep))*d10_sqrt0;
T2_Vdrainp_sourcep=(((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep))+((4.0*delta)*Vdsat_Vdrainp_sourcep))*d10_sqrt0;
T2_Vbulk_sourcep=(((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep))+((4.0*delta)*Vdsat_Vbulk_sourcep))*d10_sqrt0;
#endif
T2=d00_sqrt0;
}
#if defined(_DERIVATE)
Vdseff_Vgate_sourcep=(Vdsat_Vgate_sourcep-(0.5*(T1_Vgate_sourcep+T2_Vgate_sourcep)));
Vdseff_Vdrainp_sourcep=(Vdsat_Vdrainp_sourcep-(0.5*(T1_Vdrainp_sourcep+T2_Vdrainp_sourcep)));
Vdseff_Vbulk_sourcep=(Vdsat_Vbulk_sourcep-(0.5*(T1_Vbulk_sourcep+T2_Vbulk_sourcep)));
#endif
Vdseff=(Vdsat-(0.5*(T1+T2)));
if
(((Vds==0.0)&&(VERSION==3.24)))
{
#if defined(_DERIVATE)
Vdseff_Vgate_sourcep=0.0;
Vdseff_Vdrainp_sourcep=0.0;
Vdseff_Vbulk_sourcep=0.0;
#endif
Vdseff=0.0;
}
#if defined(_DERIVATE)
tmp4_Vgate_sourcep=(-(((((0.5*Abulk_Vgate_sourcep)*Vdsat)+((0.5*Abulk)*Vdsat_Vgate_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdsat)*Vgst2Vtm_Vgate_sourcep)/Vgst2Vtm/Vgst2Vtm));
tmp4_Vdrainp_sourcep=(-(((((0.5*Abulk_Vdrainp_sourcep)*Vdsat)+((0.5*Abulk)*Vdsat_Vdrainp_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdsat)*Vgst2Vtm_Vdrainp_sourcep)/Vgst2Vtm/Vgst2Vtm));
tmp4_Vbulk_sourcep=(-(((((0.5*Abulk_Vbulk_sourcep)*Vdsat)+((0.5*Abulk)*Vdsat_Vbulk_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdsat)*Vgst2Vtm_Vbulk_sourcep)/Vgst2Vtm/Vgst2Vtm));
#endif
tmp4=(1.0-(((0.5*Abulk)*Vdsat)/Vgst2Vtm));
#if defined(_DERIVATE)
T9_Vgate_sourcep=((WVCoxRds_Vgate_sourcep*Vgsteff)+(WVCoxRds*Vgsteff_Vgate_sourcep));
T9_Vdrainp_sourcep=((WVCoxRds_Vdrainp_sourcep*Vgsteff)+(WVCoxRds*Vgsteff_Vdrainp_sourcep));
T9_Vbulk_sourcep=((WVCoxRds_Vbulk_sourcep*Vgsteff)+(WVCoxRds*Vgsteff_Vbulk_sourcep));
#endif
T9=(WVCoxRds*Vgsteff);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((EsatL_Vgate_sourcep+Vdsat_Vgate_sourcep)+(((2.0*T9_Vgate_sourcep)*tmp4)+((2.0*T9)*tmp4_Vgate_sourcep)));
T_0_Vdrainp_sourcep=((EsatL_Vdrainp_sourcep+Vdsat_Vdrainp_sourcep)+(((2.0*T9_Vdrainp_sourcep)*tmp4)+((2.0*T9)*tmp4_Vdrainp_sourcep)));
T_0_Vbulk_sourcep=((EsatL_Vbulk_sourcep+Vdsat_Vbulk_sourcep)+(((2.0*T9_Vbulk_sourcep)*tmp4)+((2.0*T9)*tmp4_Vbulk_sourcep)));
#endif
T_0=((EsatL+Vdsat)+((2.0*T9)*tmp4));
#if defined(_DERIVATE)
T9_Vgate_sourcep=((WVCoxRds_Vgate_sourcep*Abulk)+(WVCoxRds*Abulk_Vgate_sourcep));
T9_Vdrainp_sourcep=((WVCoxRds_Vdrainp_sourcep*Abulk)+(WVCoxRds*Abulk_Vdrainp_sourcep));
T9_Vbulk_sourcep=((WVCoxRds_Vbulk_sourcep*Abulk)+(WVCoxRds*Abulk_Vbulk_sourcep));
#endif
T9=(WVCoxRds*Abulk);
#if defined(_DERIVATE)
T1_Vgate_sourcep=((-2.0*Lambda_Vgate_sourcep/Lambda/Lambda)+T9_Vgate_sourcep);
T1_Vdrainp_sourcep=((-2.0*Lambda_Vdrainp_sourcep/Lambda/Lambda)+T9_Vdrainp_sourcep);
T1_Vbulk_sourcep=((-2.0*Lambda_Vbulk_sourcep/Lambda/Lambda)+T9_Vbulk_sourcep);
#endif
T1=(((2.0/Lambda)-1.0)+T9);
#if defined(_DERIVATE)
Vasat_Vgate_sourcep=((T_0_Vgate_sourcep*T1-T_0*T1_Vgate_sourcep)/T1/T1);
Vasat_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T1-T_0*T1_Vdrainp_sourcep)/T1/T1);
Vasat_Vbulk_sourcep=((T_0_Vbulk_sourcep*T1-T_0*T1_Vbulk_sourcep)/T1/T1);
#endif
Vasat=(T_0/T1);
if
((Vdseff>Vds))
{
#if defined(_DERIVATE)
Vdseff_Vgate_sourcep=0.0;
Vdseff_Vdrainp_sourcep=Vds_Vdrainp_sourcep;
Vdseff_Vbulk_sourcep=0.0;
#endif
Vdseff=Vds;
}
#if defined(_DERIVATE)
diffVds_Vdrainp_sourcep=(Vds_Vdrainp_sourcep-Vdseff_Vdrainp_sourcep);
diffVds_Vgate_sourcep=(-Vdseff_Vgate_sourcep);
diffVds_Vbulk_sourcep=(-Vdseff_Vbulk_sourcep);
#endif
diffVds=(Vds-Vdseff);
if
(((pclm>0.0)&&(diffVds>1.0e-10)))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(-(pclm*Abulk_Vgate_sourcep)*litl/((pclm*Abulk)*litl)/((pclm*Abulk)*litl));
T_0_Vdrainp_sourcep=(-(pclm*Abulk_Vdrainp_sourcep)*litl/((pclm*Abulk)*litl)/((pclm*Abulk)*litl));
T_0_Vbulk_sourcep=(-(pclm*Abulk_Vbulk_sourcep)*litl/((pclm*Abulk)*litl)/((pclm*Abulk)*litl));
#endif
T_0=(1.0/((pclm*Abulk)*litl));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((Vgsteff_Vgate_sourcep*EsatL-Vgsteff*EsatL_Vgate_sourcep)/EsatL/EsatL);
T2_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep*EsatL-Vgsteff*EsatL_Vdrainp_sourcep)/EsatL/EsatL);
T2_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep*EsatL-Vgsteff*EsatL_Vbulk_sourcep)/EsatL/EsatL);
#endif
T2=(Vgsteff/EsatL);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Leff*(Abulk_Vgate_sourcep+T2_Vgate_sourcep));
T1_Vdrainp_sourcep=(Leff*(Abulk_Vdrainp_sourcep+T2_Vdrainp_sourcep));
T1_Vbulk_sourcep=(Leff*(Abulk_Vbulk_sourcep+T2_Vbulk_sourcep));
#endif
T1=(Leff*(Abulk+T2));
#if defined(_DERIVATE)
T9_Vgate_sourcep=((T_0_Vgate_sourcep*T1)+(T_0*T1_Vgate_sourcep));
T9_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T1)+(T_0*T1_Vdrainp_sourcep));
T9_Vbulk_sourcep=((T_0_Vbulk_sourcep*T1)+(T_0*T1_Vbulk_sourcep));
#endif
T9=(T_0*T1);
#if defined(_DERIVATE)
VACLM_Vgate_sourcep=((T9_Vgate_sourcep*diffVds)+(T9*diffVds_Vgate_sourcep));
VACLM_Vdrainp_sourcep=((T9_Vdrainp_sourcep*diffVds)+(T9*diffVds_Vdrainp_sourcep));
VACLM_Vbulk_sourcep=((T9_Vbulk_sourcep*diffVds)+(T9*diffVds_Vbulk_sourcep));
#endif
VACLM=(T9*diffVds);
}
else
{
#if defined(_DERIVATE)
VACLM_Vgate_sourcep=0.0;
VACLM_Vdrainp_sourcep=0.0;
VACLM_Vbulk_sourcep=0.0;
#endif
VACLM=5.834617425e14;
}
if
((thetaRout>0.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((((Vgst2Vtm_Vgate_sourcep*Abulk)+(Vgst2Vtm*Abulk_Vgate_sourcep))*Vdsat)+((Vgst2Vtm*Abulk)*Vdsat_Vgate_sourcep));
T_0_Vdrainp_sourcep=((((Vgst2Vtm_Vdrainp_sourcep*Abulk)+(Vgst2Vtm*Abulk_Vdrainp_sourcep))*Vdsat)+((Vgst2Vtm*Abulk)*Vdsat_Vdrainp_sourcep));
T_0_Vbulk_sourcep=((((Vgst2Vtm_Vbulk_sourcep*Abulk)+(Vgst2Vtm*Abulk_Vbulk_sourcep))*Vdsat)+((Vgst2Vtm*Abulk)*Vdsat_Vbulk_sourcep));
#endif
T_0=((Vgst2Vtm*Abulk)*Vdsat);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Vgst2Vtm_Vgate_sourcep+((Abulk_Vgate_sourcep*Vdsat)+(Abulk*Vdsat_Vgate_sourcep)));
T1_Vdrainp_sourcep=(Vgst2Vtm_Vdrainp_sourcep+((Abulk_Vdrainp_sourcep*Vdsat)+(Abulk*Vdsat_Vdrainp_sourcep)));
T1_Vbulk_sourcep=(Vgst2Vtm_Vbulk_sourcep+((Abulk_Vbulk_sourcep*Vdsat)+(Abulk*Vdsat_Vbulk_sourcep)));
#endif
T1=(Vgst2Vtm+(Abulk*Vdsat));
#if defined(_DERIVATE)
VADIBL_Vdrainp_sourcep=((Vgst2Vtm_Vdrainp_sourcep-((T_0_Vdrainp_sourcep*T1-T_0*T1_Vdrainp_sourcep)/T1/T1))/thetaRout);
VADIBL_Vgate_sourcep=((Vgst2Vtm_Vgate_sourcep-((T_0_Vgate_sourcep*T1-T_0*T1_Vgate_sourcep)/T1/T1))/thetaRout);
VADIBL_Vbulk_sourcep=((Vgst2Vtm_Vbulk_sourcep-((T_0_Vbulk_sourcep*T1-T_0*T1_Vbulk_sourcep)/T1/T1))/thetaRout);
#endif
VADIBL=((Vgst2Vtm-(T_0/T1))/thetaRout);
#if defined(_DERIVATE)
T7_Vgate_sourcep=0.0;
T7_Vdrainp_sourcep=(pdiblb*Vbseff_Vdrainp_sourcep);
T7_Vbulk_sourcep=(pdiblb*Vbseff_Vbulk_sourcep);
#endif
T7=(pdiblb*Vbseff);
if
((T7>=(-0.9)))
{
#if defined(_DERIVATE)
T3_Vgate_sourcep=(-T7_Vgate_sourcep/(1.0+T7)/(1.0+T7));
T3_Vdrainp_sourcep=(-T7_Vdrainp_sourcep/(1.0+T7)/(1.0+T7));
T3_Vbulk_sourcep=(-T7_Vbulk_sourcep/(1.0+T7)/(1.0+T7));
#endif
T3=(1.0/(1.0+T7));
#if defined(_DERIVATE)
VADIBL_Vdrainp_sourcep=((T3_Vdrainp_sourcep*VADIBL)+(T3*VADIBL_Vdrainp_sourcep));
VADIBL_Vgate_sourcep=((T3_Vgate_sourcep*VADIBL)+(T3*VADIBL_Vgate_sourcep));
VADIBL_Vbulk_sourcep=((T3_Vbulk_sourcep*VADIBL)+(T3*VADIBL_Vbulk_sourcep));
#endif
VADIBL=(T3*VADIBL);
}
else
{
#if defined(_DERIVATE)
T4_Vgate_sourcep=(-T7_Vgate_sourcep/(0.8+T7)/(0.8+T7));
T4_Vdrainp_sourcep=(-T7_Vdrainp_sourcep/(0.8+T7)/(0.8+T7));
T4_Vbulk_sourcep=(-T7_Vbulk_sourcep/(0.8+T7)/(0.8+T7));
#endif
T4=(1.0/(0.8+T7));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((20.0*T7_Vgate_sourcep)*T4)+((17.0+(20.0*T7))*T4_Vgate_sourcep));
T3_Vdrainp_sourcep=(((20.0*T7_Vdrainp_sourcep)*T4)+((17.0+(20.0*T7))*T4_Vdrainp_sourcep));
T3_Vbulk_sourcep=(((20.0*T7_Vbulk_sourcep)*T4)+((17.0+(20.0*T7))*T4_Vbulk_sourcep));
#endif
T3=((17.0+(20.0*T7))*T4);
#if defined(_DERIVATE)
VADIBL_Vdrainp_sourcep=((T3_Vdrainp_sourcep*VADIBL)+(T3*VADIBL_Vdrainp_sourcep));
VADIBL_Vgate_sourcep=((T3_Vgate_sourcep*VADIBL)+(T3*VADIBL_Vgate_sourcep));
VADIBL_Vbulk_sourcep=((T3_Vbulk_sourcep*VADIBL)+(T3*VADIBL_Vbulk_sourcep));
#endif
VADIBL=(T3*VADIBL);
}
}
else
{
#if defined(_DERIVATE)
VADIBL_Vdrainp_sourcep=0.0;
VADIBL_Vgate_sourcep=0.0;
VADIBL_Vbulk_sourcep=0.0;
#endif
VADIBL=5.834617425e14;
}
#if defined(_DERIVATE)
T9_Vgate_sourcep=(((-pvag*EsatL_Vgate_sourcep/EsatL/EsatL)*Vgsteff)+((pvag/EsatL)*Vgsteff_Vgate_sourcep));
T9_Vdrainp_sourcep=(((-pvag*EsatL_Vdrainp_sourcep/EsatL/EsatL)*Vgsteff)+((pvag/EsatL)*Vgsteff_Vdrainp_sourcep));
T9_Vbulk_sourcep=(((-pvag*EsatL_Vbulk_sourcep/EsatL/EsatL)*Vgsteff)+((pvag/EsatL)*Vgsteff_Vbulk_sourcep));
#endif
T9=((pvag/EsatL)*Vgsteff);
if
((T9>(-0.9)))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=T9_Vgate_sourcep;
T_0_Vdrainp_sourcep=T9_Vdrainp_sourcep;
T_0_Vbulk_sourcep=T9_Vbulk_sourcep;
#endif
T_0=(1.0+T9);
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((T9_Vgate_sourcep*1.0*(17.0+(20.0*T9))-((0.8+T9)*1.0)*(20.0*T9_Vgate_sourcep))/(17.0+(20.0*T9))/(17.0+(20.0*T9)));
T_0_Vdrainp_sourcep=((T9_Vdrainp_sourcep*1.0*(17.0+(20.0*T9))-((0.8+T9)*1.0)*(20.0*T9_Vdrainp_sourcep))/(17.0+(20.0*T9))/(17.0+(20.0*T9)));
T_0_Vbulk_sourcep=((T9_Vbulk_sourcep*1.0*(17.0+(20.0*T9))-((0.8+T9)*1.0)*(20.0*T9_Vbulk_sourcep))/(17.0+(20.0*T9))/(17.0+(20.0*T9)));
#endif
T_0=(((0.8+T9)*1.0)/(17.0+(20.0*T9)));
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=((((VACLM_Vgate_sourcep*VADIBL)+(VACLM*VADIBL_Vgate_sourcep))*(VACLM+VADIBL)-(VACLM*VADIBL)*(VACLM_Vgate_sourcep+VADIBL_Vgate_sourcep))/(VACLM+VADIBL)/(VACLM+VADIBL));
T1_Vdrainp_sourcep=((((VACLM_Vdrainp_sourcep*VADIBL)+(VACLM*VADIBL_Vdrainp_sourcep))*(VACLM+VADIBL)-(VACLM*VADIBL)*(VACLM_Vdrainp_sourcep+VADIBL_Vdrainp_sourcep))/(VACLM+VADIBL)/(VACLM+VADIBL));
T1_Vbulk_sourcep=((((VACLM_Vbulk_sourcep*VADIBL)+(VACLM*VADIBL_Vbulk_sourcep))*(VACLM+VADIBL)-(VACLM*VADIBL)*(VACLM_Vbulk_sourcep+VADIBL_Vbulk_sourcep))/(VACLM+VADIBL)/(VACLM+VADIBL));
#endif
T1=((VACLM*VADIBL)/(VACLM+VADIBL));
#if defined(_DERIVATE)
Va_Vgate_sourcep=(Vasat_Vgate_sourcep+((T_0_Vgate_sourcep*T1)+(T_0*T1_Vgate_sourcep)));
Va_Vdrainp_sourcep=(Vasat_Vdrainp_sourcep+((T_0_Vdrainp_sourcep*T1)+(T_0*T1_Vdrainp_sourcep)));
Va_Vbulk_sourcep=(Vasat_Vbulk_sourcep+((T_0_Vbulk_sourcep*T1)+(T_0*T1_Vbulk_sourcep)));
#endif
Va=(Vasat+(T_0*T1));
if
((pscbe2>0.0))
{
if
((diffVds>((pscbe1*litl)/34.0)))
{
{
double m00_exp(d00_exp0,((pscbe1*litl)/diffVds))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
VASCBE_Vdrainp_sourcep=((Leff*(-(pscbe1*litl)*diffVds_Vdrainp_sourcep/diffVds/diffVds)*d10_exp0)/pscbe2);
VASCBE_Vgate_sourcep=((Leff*(-(pscbe1*litl)*diffVds_Vgate_sourcep/diffVds/diffVds)*d10_exp0)/pscbe2);
VASCBE_Vbulk_sourcep=((Leff*(-(pscbe1*litl)*diffVds_Vbulk_sourcep/diffVds/diffVds)*d10_exp0)/pscbe2);
#endif
VASCBE=((Leff*d00_exp0)/pscbe2);
}
}
else
{
#if defined(_DERIVATE)
VASCBE_Vdrainp_sourcep=0.0;
VASCBE_Vgate_sourcep=0.0;
VASCBE_Vbulk_sourcep=0.0;
#endif
VASCBE=((5.834617425e14*Leff)/pscbe2);
}
}
else
{
#if defined(_DERIVATE)
VASCBE_Vdrainp_sourcep=0.0;
VASCBE_Vgate_sourcep=0.0;
VASCBE_Vbulk_sourcep=0.0;
#endif
VASCBE=5.834617425e14;
}
#if defined(_DERIVATE)
CoxWovL_Vdrainp_sourcep=((cox*Weff_Vdrainp_sourcep)/Leff);
CoxWovL_Vgate_sourcep=((cox*Weff_Vgate_sourcep)/Leff);
CoxWovL_Vbulk_sourcep=((cox*Weff_Vbulk_sourcep)/Leff);
#endif
CoxWovL=((cox*Weff)/Leff);
#if defined(_DERIVATE)
beta_Vgate_sourcep=((ueff_Vgate_sourcep*CoxWovL)+(ueff*CoxWovL_Vgate_sourcep));
beta_Vdrainp_sourcep=((ueff_Vdrainp_sourcep*CoxWovL)+(ueff*CoxWovL_Vdrainp_sourcep));
beta_Vbulk_sourcep=((ueff_Vbulk_sourcep*CoxWovL)+(ueff*CoxWovL_Vbulk_sourcep));
#endif
beta=(ueff*CoxWovL);
#if defined(_DERIVATE)
fgche1_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep*(1.0-(((0.5*Abulk)*Vdseff)/Vgst2Vtm)))+(Vgsteff*(-(((((0.5*Abulk_Vdrainp_sourcep)*Vdseff)+((0.5*Abulk)*Vdseff_Vdrainp_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdseff)*Vgst2Vtm_Vdrainp_sourcep)/Vgst2Vtm/Vgst2Vtm))));
fgche1_Vgate_sourcep=((Vgsteff_Vgate_sourcep*(1.0-(((0.5*Abulk)*Vdseff)/Vgst2Vtm)))+(Vgsteff*(-(((((0.5*Abulk_Vgate_sourcep)*Vdseff)+((0.5*Abulk)*Vdseff_Vgate_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdseff)*Vgst2Vtm_Vgate_sourcep)/Vgst2Vtm/Vgst2Vtm))));
fgche1_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep*(1.0-(((0.5*Abulk)*Vdseff)/Vgst2Vtm)))+(Vgsteff*(-(((((0.5*Abulk_Vbulk_sourcep)*Vdseff)+((0.5*Abulk)*Vdseff_Vbulk_sourcep))*Vgst2Vtm-((0.5*Abulk)*Vdseff)*Vgst2Vtm_Vbulk_sourcep)/Vgst2Vtm/Vgst2Vtm))));
#endif
fgche1=(Vgsteff*(1.0-(((0.5*Abulk)*Vdseff)/Vgst2Vtm)));
#if defined(_DERIVATE)
fgche2_Vgate_sourcep=((Vdseff_Vgate_sourcep*EsatL-Vdseff*EsatL_Vgate_sourcep)/EsatL/EsatL);
fgche2_Vdrainp_sourcep=((Vdseff_Vdrainp_sourcep*EsatL-Vdseff*EsatL_Vdrainp_sourcep)/EsatL/EsatL);
fgche2_Vbulk_sourcep=((Vdseff_Vbulk_sourcep*EsatL-Vdseff*EsatL_Vbulk_sourcep)/EsatL/EsatL);
#endif
fgche2=(1.0+(Vdseff/EsatL));
#if defined(_DERIVATE)
gche_Vgate_sourcep=((((beta_Vgate_sourcep*fgche1)+(beta*fgche1_Vgate_sourcep))*fgche2-(beta*fgche1)*fgche2_Vgate_sourcep)/fgche2/fgche2);
gche_Vdrainp_sourcep=((((beta_Vdrainp_sourcep*fgche1)+(beta*fgche1_Vdrainp_sourcep))*fgche2-(beta*fgche1)*fgche2_Vdrainp_sourcep)/fgche2/fgche2);
gche_Vbulk_sourcep=((((beta_Vbulk_sourcep*fgche1)+(beta*fgche1_Vbulk_sourcep))*fgche2-(beta*fgche1)*fgche2_Vbulk_sourcep)/fgche2/fgche2);
#endif
gche=((beta*fgche1)/fgche2);
#if defined(_DERIVATE)
Idl_Vgate_sourcep=((((gche_Vgate_sourcep*Vdseff)+(gche*Vdseff_Vgate_sourcep))*(1.0+(gche*Rds))-(gche*Vdseff)*((gche_Vgate_sourcep*Rds)+(gche*Rds_Vgate_sourcep)))/(1.0+(gche*Rds))/(1.0+(gche*Rds)));
Idl_Vdrainp_sourcep=((((gche_Vdrainp_sourcep*Vdseff)+(gche*Vdseff_Vdrainp_sourcep))*(1.0+(gche*Rds))-(gche*Vdseff)*((gche_Vdrainp_sourcep*Rds)+(gche*Rds_Vdrainp_sourcep)))/(1.0+(gche*Rds))/(1.0+(gche*Rds)));
Idl_Vbulk_sourcep=((((gche_Vbulk_sourcep*Vdseff)+(gche*Vdseff_Vbulk_sourcep))*(1.0+(gche*Rds))-(gche*Vdseff)*((gche_Vbulk_sourcep*Rds)+(gche*Rds_Vbulk_sourcep)))/(1.0+(gche*Rds))/(1.0+(gche*Rds)));
#endif
Idl=((gche*Vdseff)/(1.0+(gche*Rds)));
#if defined(_DERIVATE)
Idsa_Vgate_sourcep=((Idl_Vgate_sourcep*(1.0+(diffVds/Va)))+(Idl*((diffVds_Vgate_sourcep*Va-diffVds*Va_Vgate_sourcep)/Va/Va)));
Idsa_Vdrainp_sourcep=((Idl_Vdrainp_sourcep*(1.0+(diffVds/Va)))+(Idl*((diffVds_Vdrainp_sourcep*Va-diffVds*Va_Vdrainp_sourcep)/Va/Va)));
Idsa_Vbulk_sourcep=((Idl_Vbulk_sourcep*(1.0+(diffVds/Va)))+(Idl*((diffVds_Vbulk_sourcep*Va-diffVds*Va_Vbulk_sourcep)/Va/Va)));
#endif
Idsa=(Idl*(1.0+(diffVds/Va)));
#if defined(_DERIVATE)
Ids_Vgate_sourcep=((Idsa_Vgate_sourcep*(1.0+(diffVds/VASCBE)))+(Idsa*((diffVds_Vgate_sourcep*VASCBE-diffVds*VASCBE_Vgate_sourcep)/VASCBE/VASCBE)));
Ids_Vdrainp_sourcep=((Idsa_Vdrainp_sourcep*(1.0+(diffVds/VASCBE)))+(Idsa*((diffVds_Vdrainp_sourcep*VASCBE-diffVds*VASCBE_Vdrainp_sourcep)/VASCBE/VASCBE)));
Ids_Vbulk_sourcep=((Idsa_Vbulk_sourcep*(1.0+(diffVds/VASCBE)))+(Idsa*((diffVds_Vbulk_sourcep*VASCBE-diffVds*VASCBE_Vbulk_sourcep)/VASCBE/VASCBE)));
#endif
Ids=(Idsa*(1.0+(diffVds/VASCBE)));
#if defined(_DERIVATE)
tmp_Vgate_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vbulk_sourcep=0.0;
#endif
tmp=(alpha0+(alpha1*Leff));
if
(((tmp<=0.0)||(beta0<=0.0)))
{
#if defined(_DERIVATE)
Isub_Vgate_sourcep=0.0;
Isub_Vdrainp_sourcep=0.0;
Isub_Vbulk_sourcep=0.0;
#endif
Isub=0.0;
}
else
{
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=(tmp/Leff);
if
((diffVds>(beta0/34.0)))
{
{
double m00_exp(d00_exp0,((-beta0)/diffVds))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=((((T2_Vgate_sourcep*diffVds)+(T2*diffVds_Vgate_sourcep))*d00_exp0)+((T2*diffVds)*(-(-beta0)*diffVds_Vgate_sourcep/diffVds/diffVds)*d10_exp0));
T1_Vdrainp_sourcep=((((T2_Vdrainp_sourcep*diffVds)+(T2*diffVds_Vdrainp_sourcep))*d00_exp0)+((T2*diffVds)*(-(-beta0)*diffVds_Vdrainp_sourcep/diffVds/diffVds)*d10_exp0));
T1_Vbulk_sourcep=((((T2_Vbulk_sourcep*diffVds)+(T2*diffVds_Vbulk_sourcep))*d00_exp0)+((T2*diffVds)*(-(-beta0)*diffVds_Vbulk_sourcep/diffVds/diffVds)*d10_exp0));
#endif
T1=((T2*diffVds)*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=((T2_Vgate_sourcep*1.713908431e-15*diffVds)+((T2*1.713908431e-15)*diffVds_Vgate_sourcep));
T1_Vdrainp_sourcep=((T2_Vdrainp_sourcep*1.713908431e-15*diffVds)+((T2*1.713908431e-15)*diffVds_Vdrainp_sourcep));
T1_Vbulk_sourcep=((T2_Vbulk_sourcep*1.713908431e-15*diffVds)+((T2*1.713908431e-15)*diffVds_Vbulk_sourcep));
#endif
T1=((T2*1.713908431e-15)*diffVds);
}
#if defined(_DERIVATE)
Isub_Vgate_sourcep=((T1_Vgate_sourcep*Idsa)+(T1*Idsa_Vgate_sourcep));
Isub_Vdrainp_sourcep=((T1_Vdrainp_sourcep*Idsa)+(T1*Idsa_Vdrainp_sourcep));
Isub_Vbulk_sourcep=((T1_Vbulk_sourcep*Idsa)+(T1*Idsa_Vbulk_sourcep));
#endif
Isub=(T1*Idsa);
}
#if defined(_DERIVATE)
cdrain_Vgate_sourcep=Ids_Vgate_sourcep;
cdrain_Vdrainp_sourcep=Ids_Vdrainp_sourcep;
cdrain_Vbulk_sourcep=Ids_Vbulk_sourcep;
#endif
cdrain=Ids;
#if defined(_DERIVATE)
csub_Vgate_sourcep=Isub_Vgate_sourcep;
csub_Vdrainp_sourcep=Isub_Vdrainp_sourcep;
csub_Vbulk_sourcep=Isub_Vbulk_sourcep;
#endif
csub=Isub;
if
((VERSION==3.24))
{
czbd=(unitAreaTempJctCap*drainArea);
czbs=(unitAreaTempJctCap*sourceArea);
}
else
{
czbd=(unitAreaJctCap*drainArea);
czbs=(unitAreaJctCap*sourceArea);
}
if
((drainPerimeter<weff))
{
if
((VERSION==3.24))
{
czbdswg=(unitLengthGateSidewallTempJctCap*drainPerimeter);
}
else
{
czbdswg=(unitLengthGateSidewallJctCap*drainPerimeter);
}
czbdsw=0.0;
}
else
{
if
((VERSION==3.24))
{
czbdsw=(unitLengthSidewallTempJctCap*(drainPerimeter-weff));
czbdswg=(unitLengthGateSidewallTempJctCap*weff);
}
else
{
czbdsw=(unitLengthSidewallJctCap*(drainPerimeter-weff));
czbdswg=(unitLengthGateSidewallJctCap*weff);
}
}
if
((sourcePerimeter<weff))
{
czbssw=0.0;
if
((VERSION==3.24))
{
czbsswg=(unitLengthGateSidewallTempJctCap*sourcePerimeter);
}
else
{
czbsswg=(unitLengthGateSidewallJctCap*sourcePerimeter);
}
}
else
{
if
((VERSION==3.24))
{
czbssw=(unitLengthSidewallTempJctCap*(sourcePerimeter-weff));
czbsswg=(unitLengthGateSidewallTempJctCap*weff);
}
else
{
czbssw=(unitLengthSidewallJctCap*(sourcePerimeter-weff));
czbsswg=(unitLengthGateSidewallJctCap*weff);
}
}
mj=bulkJctBotGradingCoeff;
mjsw=bulkJctSideGradingCoeff;
mjswg=bulkJctGateSideGradingCoeff;
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=0.0;
#endif
qbs=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=0.0;
qbd_Vbulk_sourcep=0.0;
#endif
qbd=0.0;
#endif
if
((vbs==0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=0.0;
#endif
qbs=0.0;
#endif
#if defined(_DYNAMIC)
capbs=((czbs+czbssw)+czbsswg);
#endif
}
else
{
if
((vbs<0.0))
{
#ifdef _DYNAMIC
if
((czbs>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vbulk_sourcep=(-(vbs_Vbulk_sourcep/PhiB));
#endif
arg=(1.0-(vbs/PhiB));
#endif
#ifdef _DYNAMIC
if
((mj==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mj)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=((-mj)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=(((PhiB*czbs)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mj));
#endif
qbs=(((PhiB*czbs)*(1.0-(arg*sarg)))/(1.0-mj));
#endif
#if defined(_DYNAMIC)
capbs=(czbs*sarg);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=0.0;
#endif
qbs=0.0;
#endif
#if defined(_DYNAMIC)
capbs=0.0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((czbssw>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vbulk_sourcep=(-(vbs_Vbulk_sourcep/PhiBSW));
#endif
arg=(1.0-(vbs/PhiBSW));
#endif
#ifdef _DYNAMIC
if
((mjsw==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjsw)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=((-mjsw)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=(qbs_Vbulk_sourcep+(((PhiBSW*czbssw)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mjsw)));
#endif
qbs=(qbs+(((PhiBSW*czbssw)*(1.0-(arg*sarg)))/(1.0-mjsw)));
#endif
#if defined(_DYNAMIC)
capbs=(capbs+(czbssw*sarg));
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((czbsswg>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vbulk_sourcep=(-(vbs_Vbulk_sourcep/PhiBSWG));
#endif
arg=(1.0-(vbs/PhiBSWG));
#endif
#ifdef _DYNAMIC
if
((mjswg==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjswg)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vbulk_sourcep=((-mjswg)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vbulk_sourcep=(qbs_Vbulk_sourcep+(((PhiBSWG*czbsswg)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mjswg)));
#endif
qbs=(qbs+(((PhiBSWG*czbsswg)*(1.0-(arg*sarg)))/(1.0-mjswg)));
#endif
#if defined(_DYNAMIC)
capbs=(capbs+(czbsswg*sarg));
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((czbs+czbssw)+czbsswg);
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=vbs_Vbulk_sourcep*((((czbs*mj)/PhiB)+((czbssw*mjsw)/PhiBSW))+((czbsswg*mjswg)/PhiBSWG));
#endif
T1=(vbs*((((czbs*mj)/PhiB)+((czbssw*mjsw)/PhiBSW))+((czbsswg*mjswg)/PhiBSWG)));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbs_Vgate_sourcep=(vbs*(T_0_Vgate_sourcep+(0.5*T1_Vgate_sourcep)));
qbs_Vdrainp_sourcep=(vbs*(T_0_Vdrainp_sourcep+(0.5*T1_Vdrainp_sourcep)));
qbs_Vbulk_sourcep=((vbs_Vbulk_sourcep*(T_0+(0.5*T1)))+(vbs*(T_0_Vbulk_sourcep+(0.5*T1_Vbulk_sourcep))));
#endif
qbs=(vbs*(T_0+(0.5*T1)));
#endif
#if defined(_DYNAMIC)
capbs=(T_0+T1);
#endif
}
}
if
((vbd==0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=0.0;
qbd_Vbulk_sourcep=0.0;
#endif
qbd=0.0;
#endif
#if defined(_DYNAMIC)
capbd=((czbd+czbdsw)+czbdswg);
#endif
}
else
{
if
((vbd<0.0))
{
#ifdef _DYNAMIC
if
((czbd>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_Vdrainp_sourcep/PhiB));
arg_Vbulk_sourcep=(-(vbd_Vbulk_sourcep/PhiB));
#endif
arg=(1.0-(vbd/PhiB));
#endif
#ifdef _DYNAMIC
if
((mj==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mj)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mj)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulk_sourcep=((-mj)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=(((PhiB*czbd)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mj));
qbd_Vbulk_sourcep=(((PhiB*czbd)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mj));
#endif
qbd=(((PhiB*czbd)*(1.0-(arg*sarg)))/(1.0-mj));
#endif
#if defined(_DYNAMIC)
capbd=(czbd*sarg);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=0.0;
qbd_Vbulk_sourcep=0.0;
#endif
qbd=0.0;
#endif
#if defined(_DYNAMIC)
capbd=0.0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((czbdsw>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_Vdrainp_sourcep/PhiBSW));
arg_Vbulk_sourcep=(-(vbd_Vbulk_sourcep/PhiBSW));
#endif
arg=(1.0-(vbd/PhiBSW));
#endif
#ifdef _DYNAMIC
if
((mjsw==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjsw)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mjsw)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulk_sourcep=((-mjsw)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=(qbd_Vdrainp_sourcep+(((PhiBSW*czbdsw)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mjsw)));
qbd_Vbulk_sourcep=(qbd_Vbulk_sourcep+(((PhiBSW*czbdsw)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mjsw)));
#endif
qbd=(qbd+(((PhiBSW*czbdsw)*(1.0-(arg*sarg)))/(1.0-mjsw)));
#endif
#if defined(_DYNAMIC)
capbd=(capbd+(czbdsw*sarg));
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((czbdswg>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_Vdrainp_sourcep/PhiBSWG));
arg_Vbulk_sourcep=(-(vbd_Vbulk_sourcep/PhiBSWG));
#endif
arg=(1.0-(vbd/PhiBSWG));
#endif
#ifdef _DYNAMIC
if
((mjswg==0.5))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulk_sourcep=(-arg_Vbulk_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjswg)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mjswg)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulk_sourcep=((-mjswg)*arg_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=0.0;
qbd_Vdrainp_sourcep=(qbd_Vdrainp_sourcep+(((PhiBSWG*czbdswg)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mjswg)));
qbd_Vbulk_sourcep=(qbd_Vbulk_sourcep+(((PhiBSWG*czbdswg)*(-((arg_Vbulk_sourcep*sarg)+(arg*sarg_Vbulk_sourcep))))/(1.0-mjswg)));
#endif
qbd=(qbd+(((PhiBSWG*czbdswg)*(1.0-(arg*sarg)))/(1.0-mjswg)));
#endif
#if defined(_DYNAMIC)
capbd=(capbd+(czbdswg*sarg));
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((czbd+czbdsw)+czbdswg);
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=vbd_Vdrainp_sourcep*((((czbd*mj)/PhiB)+((czbdsw*mjsw)/PhiBSW))+((czbdswg*mjswg)/PhiBSWG));
T1_Vbulk_sourcep=vbd_Vbulk_sourcep*((((czbd*mj)/PhiB)+((czbdsw*mjsw)/PhiBSW))+((czbdswg*mjswg)/PhiBSWG));
#endif
T1=(vbd*((((czbd*mj)/PhiB)+((czbdsw*mjsw)/PhiBSW))+((czbdswg*mjswg)/PhiBSWG)));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbd_Vgate_sourcep=(vbd*(T_0_Vgate_sourcep+(0.5*T1_Vgate_sourcep)));
qbd_Vdrainp_sourcep=((vbd_Vdrainp_sourcep*(T_0+(0.5*T1)))+(vbd*(T_0_Vdrainp_sourcep+(0.5*T1_Vdrainp_sourcep))));
qbd_Vbulk_sourcep=((vbd_Vbulk_sourcep*(T_0+(0.5*T1)))+(vbd*(T_0_Vbulk_sourcep+(0.5*T1_Vbulk_sourcep))));
#endif
qbd=(vbd*(T_0+(0.5*T1)));
#endif
#if defined(_DYNAMIC)
capbd=(T_0+T1);
#endif
}
}
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=0.0;
qgate_Vgate_sourcep=0.0;
qgate_Vbulk_sourcep=0.0;
#endif
qgate=0.0;
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vbulk_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=0.0;
qsrc_Vdrainp_sourcep=0.0;
qsrc_Vbulk_sourcep=0.0;
#endif
qsrc=0.0;
#endif
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=0.0;
qbulk_Vgate_sourcep=0.0;
qbulk_Vbulk_sourcep=0.0;
#endif
qbulk=0.0;
if
((XPART<0))
{
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=0.0;
qgate_Vgate_sourcep=0.0;
qgate_Vbulk_sourcep=0.0;
#endif
qgate=0.0;
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vbulk_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=0.0;
qsrc_Vdrainp_sourcep=0.0;
qsrc_Vbulk_sourcep=0.0;
#endif
qsrc=0.0;
#endif
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=0.0;
qbulk_Vgate_sourcep=0.0;
qbulk_Vbulk_sourcep=0.0;
#endif
qbulk=0.0;
}
else
{
if
((CAPMOD==0))
{
if
((Vbseff<0.0))
{
#if defined(_DERIVATE)
Vbseff_Vdrainp_sourcep=Vbs_Vdrainp_sourcep;
Vbseff_Vbulk_sourcep=Vbs_Vbulk_sourcep;
#endif
Vbseff=Vbs;
}
else
{
#if defined(_DERIVATE)
Vbseff_Vdrainp_sourcep=(-Phis_Vdrainp_sourcep);
Vbseff_Vbulk_sourcep=(-Phis_Vbulk_sourcep);
#endif
Vbseff=(phi-Phis);
}
#if defined(_DERIVATE)
Vfb_Vdrainp_sourcep=0.0;
Vfb_Vbulk_sourcep=0.0;
#endif
Vfb=vfbcv;
#if defined(_DERIVATE)
Vth_Vdrainp_sourcep=(Vfb_Vdrainp_sourcep+(k1ox*sqrtPhis_Vdrainp_sourcep));
Vth_Vbulk_sourcep=(Vfb_Vbulk_sourcep+(k1ox*sqrtPhis_Vbulk_sourcep));
#endif
Vth=((Vfb+phi)+(k1ox*sqrtPhis));
#if defined(_DERIVATE)
Vgst_Vdrainp_sourcep=(Vgs_eff_Vdrainp_sourcep-Vth_Vdrainp_sourcep);
Vgst_Vgate_sourcep=Vgs_eff_Vgate_sourcep;
Vgst_Vbulk_sourcep=(Vgs_eff_Vbulk_sourcep-Vth_Vbulk_sourcep);
#endif
Vgst=(Vgs_eff-Vth);
CoxWL=((cox*weffCV)*leffCV);
#if defined(_DERIVATE)
Arg1_Vdrainp_sourcep=((Vgs_eff_Vdrainp_sourcep-Vbseff_Vdrainp_sourcep)-Vfb_Vdrainp_sourcep);
Arg1_Vgate_sourcep=Vgs_eff_Vgate_sourcep;
Arg1_Vbulk_sourcep=((Vgs_eff_Vbulk_sourcep-Vbseff_Vbulk_sourcep)-Vfb_Vbulk_sourcep);
#endif
Arg1=((Vgs_eff-Vbseff)-Vfb);
if
((Arg1<=0.0))
{
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*Arg1_Vdrainp_sourcep);
qgate_Vgate_sourcep=(CoxWL*Arg1_Vgate_sourcep);
qgate_Vbulk_sourcep=(CoxWL*Arg1_Vbulk_sourcep);
#endif
qgate=(CoxWL*Arg1);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-qgate_Vdrainp_sourcep);
qbulk_Vgate_sourcep=(-qgate_Vgate_sourcep);
qbulk_Vbulk_sourcep=(-qgate_Vbulk_sourcep);
#endif
qbulk=(-qgate);
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vbulk_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=0.0;
qinv_Vgate_sourcep=0.0;
qinv_Vbulk_sourcep=0.0;
#endif
qinv=0.0;
}
else
{
if
((Vgst<=0.0))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=(0.5*k1ox);
{
double m00_sqrt(d00_sqrt0,((T1*T1)+Arg1))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T1*T1)+Arg1))
#endif
#if defined(_DERIVATE)
T2_Vgate_sourcep=(((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep))+Arg1_Vgate_sourcep)*d10_sqrt0;
T2_Vdrainp_sourcep=(((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep))+Arg1_Vdrainp_sourcep)*d10_sqrt0;
T2_Vbulk_sourcep=(((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep))+Arg1_Vbulk_sourcep)*d10_sqrt0;
#endif
T2=d00_sqrt0;
}
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=((CoxWL*k1ox)*(T2_Vdrainp_sourcep-T1_Vdrainp_sourcep));
qgate_Vgate_sourcep=((CoxWL*k1ox)*(T2_Vgate_sourcep-T1_Vgate_sourcep));
qgate_Vbulk_sourcep=((CoxWL*k1ox)*(T2_Vbulk_sourcep-T1_Vbulk_sourcep));
#endif
qgate=((CoxWL*k1ox)*(T2-T1));
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-qgate_Vdrainp_sourcep);
qbulk_Vgate_sourcep=(-qgate_Vgate_sourcep);
qbulk_Vbulk_sourcep=(-qgate_Vbulk_sourcep);
#endif
qbulk=(-qgate);
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vbulk_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=0.0;
qinv_Vgate_sourcep=0.0;
qinv_Vbulk_sourcep=0.0;
#endif
qinv=0.0;
}
else
{
One_Third_CoxWL=(CoxWL/3.0);
Two_Third_CoxWL=(2.0*One_Third_CoxWL);
#if defined(_DERIVATE)
AbulkCV_Vgate_sourcep=Abulk0_Vgate_sourcep*abulkCVfactor;
AbulkCV_Vdrainp_sourcep=Abulk0_Vdrainp_sourcep*abulkCVfactor;
AbulkCV_Vbulk_sourcep=Abulk0_Vbulk_sourcep*abulkCVfactor;
#endif
AbulkCV=(Abulk0*abulkCVfactor);
#if defined(_DERIVATE)
Vdsat_Vgate_sourcep=((Vgst_Vgate_sourcep*AbulkCV-Vgst*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
Vdsat_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*AbulkCV-Vgst*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
Vdsat_Vbulk_sourcep=((Vgst_Vbulk_sourcep*AbulkCV-Vgst*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
Vdsat=(Vgst/AbulkCV);
if
((XPART>0.5))
{
if
((Vdsat<=Vds))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Vdsat_Vgate_sourcep/3.0);
T1_Vdrainp_sourcep=(Vdsat_Vdrainp_sourcep/3.0);
T1_Vbulk_sourcep=(Vdsat_Vbulk_sourcep/3.0);
#endif
T1=(Vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-T1_Vdrainp_sourcep));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-T1_Vgate_sourcep));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-T1_Vbulk_sourcep));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-T1));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((-Two_Third_CoxWL)*Vgst_Vgate_sourcep);
T2_Vdrainp_sourcep=((-Two_Third_CoxWL)*Vgst_Vdrainp_sourcep);
T2_Vbulk_sourcep=((-Two_Third_CoxWL)*Vgst_Vbulk_sourcep);
#endif
T2=((-Two_Third_CoxWL)*Vgst);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+T2_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(-(qgate_Vgate_sourcep+T2_Vgate_sourcep));
qbulk_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+T2_Vbulk_sourcep));
#endif
qbulk=(-(qgate+T2));
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vbulk_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
else
{
#if defined(_DERIVATE)
Alphaz_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*Vdsat-Vgst*Vdsat_Vdrainp_sourcep)/Vdsat/Vdsat);
Alphaz_Vgate_sourcep=((Vgst_Vgate_sourcep*Vdsat-Vgst*Vdsat_Vgate_sourcep)/Vdsat/Vdsat);
Alphaz_Vbulk_sourcep=((Vgst_Vbulk_sourcep*Vdsat-Vgst*Vdsat_Vbulk_sourcep)/Vdsat/Vdsat);
#endif
Alphaz=(Vgst/Vdsat);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(2.0*Vdsat_Vgate_sourcep);
T1_Vdrainp_sourcep=((2.0*Vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
T1_Vbulk_sourcep=(2.0*Vdsat_Vbulk_sourcep);
#endif
T1=((2.0*Vdsat)-Vds);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-Vds*(3.0*T1_Vgate_sourcep)/(3.0*T1)/(3.0*T1));
T2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*T1)-Vds*(3.0*T1_Vdrainp_sourcep))/(3.0*T1)/(3.0*T1));
T2_Vbulk_sourcep=(-Vds*(3.0*T1_Vbulk_sourcep)/(3.0*T1)/(3.0*T1));
#endif
T2=(Vds/(3.0*T1));
#if defined(_DERIVATE)
T3_Vgate_sourcep=T2_Vgate_sourcep*Vds;
T3_Vdrainp_sourcep=((T2_Vdrainp_sourcep*Vds)+(T2*Vds_Vdrainp_sourcep));
T3_Vbulk_sourcep=T2_Vbulk_sourcep*Vds;
#endif
T3=(T2*Vds);
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=0.0;
T9_Vbulk_sourcep=0.0;
#endif
T9=(0.25*CoxWL);
#if defined(_DERIVATE)
T4_Vgate_sourcep=((T9_Vgate_sourcep*Alphaz)+(T9*Alphaz_Vgate_sourcep));
T4_Vdrainp_sourcep=((T9_Vdrainp_sourcep*Alphaz)+(T9*Alphaz_Vdrainp_sourcep));
T4_Vbulk_sourcep=((T9_Vbulk_sourcep*Alphaz)+(T9*Alphaz_Vbulk_sourcep));
#endif
T4=(T9*Alphaz);
#if defined(_DERIVATE)
T7_Vgate_sourcep=((-T1_Vgate_sourcep)-(3.0*T3_Vgate_sourcep));
T7_Vdrainp_sourcep=(((2.0*Vds_Vdrainp_sourcep)-T1_Vdrainp_sourcep)-(3.0*T3_Vdrainp_sourcep));
T7_Vbulk_sourcep=((-T1_Vbulk_sourcep)-(3.0*T3_Vbulk_sourcep));
#endif
T7=(((2.0*Vds)-T1)-(3.0*T3));
#if defined(_DERIVATE)
T8_Vgate_sourcep=(T3_Vgate_sourcep-T1_Vgate_sourcep);
T8_Vdrainp_sourcep=((T3_Vdrainp_sourcep-T1_Vdrainp_sourcep)-(2.0*Vds_Vdrainp_sourcep));
T8_Vbulk_sourcep=(T3_Vbulk_sourcep-T1_Vbulk_sourcep);
#endif
T8=((T3-T1)-(2.0*Vds));
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-(0.5*(Vds_Vdrainp_sourcep-T3_Vdrainp_sourcep))));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-(0.5*(-T3_Vgate_sourcep))));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-(0.5*(-T3_Vbulk_sourcep))));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-(0.5*(Vds-T3))));
#if defined(_DERIVATE)
T10_Vgate_sourcep=((T4_Vgate_sourcep*T8)+(T4*T8_Vgate_sourcep));
T10_Vdrainp_sourcep=((T4_Vdrainp_sourcep*T8)+(T4*T8_Vdrainp_sourcep));
T10_Vbulk_sourcep=((T4_Vbulk_sourcep*T8)+(T4*T8_Vbulk_sourcep));
#endif
T10=(T4*T8);
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=((T4_Vgate_sourcep*T7)+(T4*T7_Vgate_sourcep));
qdrn_Vdrainp_sourcep=((T4_Vdrainp_sourcep*T7)+(T4*T7_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=((T4_Vbulk_sourcep*T7)+(T4*T7_Vbulk_sourcep));
#endif
qdrn=(T4*T7);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qdrn_Vdrainp_sourcep)+T10_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(-((qgate_Vgate_sourcep+qdrn_Vgate_sourcep)+T10_Vgate_sourcep));
qbulk_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qdrn_Vbulk_sourcep)+T10_Vbulk_sourcep));
#endif
qbulk=(-((qgate+qdrn)+T10));
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
}
else
{
if
((XPART<0.5))
{
if
((Vds>=Vdsat))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Vdsat_Vgate_sourcep/3.0);
T1_Vdrainp_sourcep=(Vdsat_Vdrainp_sourcep/3.0);
T1_Vbulk_sourcep=(Vdsat_Vbulk_sourcep/3.0);
#endif
T1=(Vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-T1_Vdrainp_sourcep));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-T1_Vgate_sourcep));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-T1_Vbulk_sourcep));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-T1));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((-Two_Third_CoxWL)*Vgst_Vgate_sourcep);
T2_Vdrainp_sourcep=((-Two_Third_CoxWL)*Vgst_Vdrainp_sourcep);
T2_Vbulk_sourcep=((-Two_Third_CoxWL)*Vgst_Vbulk_sourcep);
#endif
T2=((-Two_Third_CoxWL)*Vgst);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+T2_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(-(qgate_Vgate_sourcep+T2_Vgate_sourcep));
qbulk_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+T2_Vbulk_sourcep));
#endif
qbulk=(-(qgate+T2));
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(0.4*T2_Vgate_sourcep);
qdrn_Vdrainp_sourcep=(0.4*T2_Vdrainp_sourcep);
qdrn_Vbulk_sourcep=(0.4*T2_Vbulk_sourcep);
#endif
qdrn=(0.4*T2);
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
else
{
#if defined(_DERIVATE)
Alphaz_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*Vdsat-Vgst*Vdsat_Vdrainp_sourcep)/Vdsat/Vdsat);
Alphaz_Vgate_sourcep=((Vgst_Vgate_sourcep*Vdsat-Vgst*Vdsat_Vgate_sourcep)/Vdsat/Vdsat);
Alphaz_Vbulk_sourcep=((Vgst_Vbulk_sourcep*Vdsat-Vgst*Vdsat_Vbulk_sourcep)/Vdsat/Vdsat);
#endif
Alphaz=(Vgst/Vdsat);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(2.0*Vdsat_Vgate_sourcep);
T1_Vdrainp_sourcep=((2.0*Vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
T1_Vbulk_sourcep=(2.0*Vdsat_Vbulk_sourcep);
#endif
T1=((2.0*Vdsat)-Vds);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-Vds*(3.0*T1_Vgate_sourcep)/(3.0*T1)/(3.0*T1));
T2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*T1)-Vds*(3.0*T1_Vdrainp_sourcep))/(3.0*T1)/(3.0*T1));
T2_Vbulk_sourcep=(-Vds*(3.0*T1_Vbulk_sourcep)/(3.0*T1)/(3.0*T1));
#endif
T2=(Vds/(3.0*T1));
#if defined(_DERIVATE)
T3_Vgate_sourcep=T2_Vgate_sourcep*Vds;
T3_Vdrainp_sourcep=((T2_Vdrainp_sourcep*Vds)+(T2*Vds_Vdrainp_sourcep));
T3_Vbulk_sourcep=T2_Vbulk_sourcep*Vds;
#endif
T3=(T2*Vds);
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=0.0;
T9_Vbulk_sourcep=0.0;
#endif
T9=(0.25*CoxWL);
#if defined(_DERIVATE)
T4_Vgate_sourcep=((T9_Vgate_sourcep*Alphaz)+(T9*Alphaz_Vgate_sourcep));
T4_Vdrainp_sourcep=((T9_Vdrainp_sourcep*Alphaz)+(T9*Alphaz_Vdrainp_sourcep));
T4_Vbulk_sourcep=((T9_Vbulk_sourcep*Alphaz)+(T9*Alphaz_Vbulk_sourcep));
#endif
T4=(T9*Alphaz);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-(0.5*(Vds_Vdrainp_sourcep-T3_Vdrainp_sourcep))));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-(0.5*(-T3_Vgate_sourcep))));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-(0.5*(-T3_Vbulk_sourcep))));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-(0.5*(Vds-T3))));
#if defined(_DERIVATE)
T6_Vgate_sourcep=((((8.0*Vdsat_Vgate_sourcep)*Vdsat)+((8.0*Vdsat)*Vdsat_Vgate_sourcep))-(6.0*Vdsat_Vgate_sourcep)*Vds);
T6_Vdrainp_sourcep=(((((8.0*Vdsat_Vdrainp_sourcep)*Vdsat)+((8.0*Vdsat)*Vdsat_Vdrainp_sourcep))-(((6.0*Vdsat_Vdrainp_sourcep)*Vds)+((6.0*Vdsat)*Vds_Vdrainp_sourcep)))+(((1.2*Vds_Vdrainp_sourcep)*Vds)+((1.2*Vds)*Vds_Vdrainp_sourcep)));
T6_Vbulk_sourcep=((((8.0*Vdsat_Vbulk_sourcep)*Vdsat)+((8.0*Vdsat)*Vdsat_Vbulk_sourcep))-(6.0*Vdsat_Vbulk_sourcep)*Vds);
#endif
T6=((((8.0*Vdsat)*Vdsat)-((6.0*Vdsat)*Vds))+((1.2*Vds)*Vds));
#if defined(_DERIVATE)
T8_Vgate_sourcep=((T2_Vgate_sourcep*T1-T2*T1_Vgate_sourcep)/T1/T1);
T8_Vdrainp_sourcep=((T2_Vdrainp_sourcep*T1-T2*T1_Vdrainp_sourcep)/T1/T1);
T8_Vbulk_sourcep=((T2_Vbulk_sourcep*T1-T2*T1_Vbulk_sourcep)/T1/T1);
#endif
T8=(T2/T1);
#if defined(_DERIVATE)
T7_Vgate_sourcep=((-T1_Vgate_sourcep)-((T8_Vgate_sourcep*T6)+(T8*T6_Vgate_sourcep)));
T7_Vdrainp_sourcep=((Vds_Vdrainp_sourcep-T1_Vdrainp_sourcep)-((T8_Vdrainp_sourcep*T6)+(T8*T6_Vdrainp_sourcep)));
T7_Vbulk_sourcep=((-T1_Vbulk_sourcep)-((T8_Vbulk_sourcep*T6)+(T8*T6_Vbulk_sourcep)));
#endif
T7=((Vds-T1)-(T8*T6));
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=((T4_Vgate_sourcep*T7)+(T4*T7_Vgate_sourcep));
qdrn_Vdrainp_sourcep=((T4_Vdrainp_sourcep*T7)+(T4*T7_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=((T4_Vbulk_sourcep*T7)+(T4*T7_Vbulk_sourcep));
#endif
qdrn=(T4*T7);
#if defined(_DERIVATE)
T7_Vgate_sourcep=(2.0*(T1_Vgate_sourcep+T3_Vgate_sourcep));
T7_Vdrainp_sourcep=(2.0*(T1_Vdrainp_sourcep+T3_Vdrainp_sourcep));
T7_Vbulk_sourcep=(2.0*(T1_Vbulk_sourcep+T3_Vbulk_sourcep));
#endif
T7=(2.0*(T1+T3));
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep-((T4_Vdrainp_sourcep*T7)+(T4*T7_Vdrainp_sourcep))));
qbulk_Vgate_sourcep=(-(qgate_Vgate_sourcep-((T4_Vgate_sourcep*T7)+(T4*T7_Vgate_sourcep))));
qbulk_Vbulk_sourcep=(-(qgate_Vbulk_sourcep-((T4_Vbulk_sourcep*T7)+(T4*T7_Vbulk_sourcep))));
#endif
qbulk=(-(qgate-(T4*T7)));
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
}
else
{
if
((Vds>=Vdsat))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Vdsat_Vgate_sourcep/3.0);
T1_Vdrainp_sourcep=(Vdsat_Vdrainp_sourcep/3.0);
T1_Vbulk_sourcep=(Vdsat_Vbulk_sourcep/3.0);
#endif
T1=(Vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-T1_Vdrainp_sourcep));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-T1_Vgate_sourcep));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-T1_Vbulk_sourcep));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-T1));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((-Two_Third_CoxWL)*Vgst_Vgate_sourcep);
T2_Vdrainp_sourcep=((-Two_Third_CoxWL)*Vgst_Vdrainp_sourcep);
T2_Vbulk_sourcep=((-Two_Third_CoxWL)*Vgst_Vbulk_sourcep);
#endif
T2=((-Two_Third_CoxWL)*Vgst);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+T2_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(-(qgate_Vgate_sourcep+T2_Vgate_sourcep));
qbulk_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+T2_Vbulk_sourcep));
#endif
qbulk=(-(qgate+T2));
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(0.5*T2_Vgate_sourcep);
qdrn_Vdrainp_sourcep=(0.5*T2_Vdrainp_sourcep);
qdrn_Vbulk_sourcep=(0.5*T2_Vbulk_sourcep);
#endif
qdrn=(0.5*T2);
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
else
{
#if defined(_DERIVATE)
Alphaz_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*Vdsat-Vgst*Vdsat_Vdrainp_sourcep)/Vdsat/Vdsat);
Alphaz_Vgate_sourcep=((Vgst_Vgate_sourcep*Vdsat-Vgst*Vdsat_Vgate_sourcep)/Vdsat/Vdsat);
Alphaz_Vbulk_sourcep=((Vgst_Vbulk_sourcep*Vdsat-Vgst*Vdsat_Vbulk_sourcep)/Vdsat/Vdsat);
#endif
Alphaz=(Vgst/Vdsat);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(2.0*Vdsat_Vgate_sourcep);
T1_Vdrainp_sourcep=((2.0*Vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
T1_Vbulk_sourcep=(2.0*Vdsat_Vbulk_sourcep);
#endif
T1=((2.0*Vdsat)-Vds);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-Vds*(3.0*T1_Vgate_sourcep)/(3.0*T1)/(3.0*T1));
T2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*T1)-Vds*(3.0*T1_Vdrainp_sourcep))/(3.0*T1)/(3.0*T1));
T2_Vbulk_sourcep=(-Vds*(3.0*T1_Vbulk_sourcep)/(3.0*T1)/(3.0*T1));
#endif
T2=(Vds/(3.0*T1));
#if defined(_DERIVATE)
T3_Vgate_sourcep=T2_Vgate_sourcep*Vds;
T3_Vdrainp_sourcep=((T2_Vdrainp_sourcep*Vds)+(T2*Vds_Vdrainp_sourcep));
T3_Vbulk_sourcep=T2_Vbulk_sourcep*Vds;
#endif
T3=(T2*Vds);
#if defined(_DERIVATE)
T9_Vgate_sourcep=0.0;
T9_Vdrainp_sourcep=0.0;
T9_Vbulk_sourcep=0.0;
#endif
T9=(0.25*CoxWL);
#if defined(_DERIVATE)
T4_Vgate_sourcep=((T9_Vgate_sourcep*Alphaz)+(T9*Alphaz_Vgate_sourcep));
T4_Vdrainp_sourcep=((T9_Vdrainp_sourcep*Alphaz)+(T9*Alphaz_Vdrainp_sourcep));
T4_Vbulk_sourcep=((T9_Vbulk_sourcep*Alphaz)+(T9*Alphaz_Vbulk_sourcep));
#endif
T4=(T9*Alphaz);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgs_eff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep)-(0.5*(Vds_Vdrainp_sourcep-T3_Vdrainp_sourcep))));
qgate_Vgate_sourcep=(CoxWL*(Vgs_eff_Vgate_sourcep-(0.5*(-T3_Vgate_sourcep))));
qgate_Vbulk_sourcep=(CoxWL*((Vgs_eff_Vbulk_sourcep-Vfb_Vbulk_sourcep)-(0.5*(-T3_Vbulk_sourcep))));
#endif
qgate=(CoxWL*(((Vgs_eff-Vfb)-phi)-(0.5*(Vds-T3))));
#if defined(_DERIVATE)
T7_Vgate_sourcep=(T1_Vgate_sourcep+T3_Vgate_sourcep);
T7_Vdrainp_sourcep=(T1_Vdrainp_sourcep+T3_Vdrainp_sourcep);
T7_Vbulk_sourcep=(T1_Vbulk_sourcep+T3_Vbulk_sourcep);
#endif
T7=(T1+T3);
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(((-T4_Vgate_sourcep)*T7)+((-T4)*T7_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(((-T4_Vdrainp_sourcep)*T7)+((-T4)*T7_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(((-T4_Vbulk_sourcep)*T7)+((-T4)*T7_Vbulk_sourcep));
#endif
qdrn=((-T4)*T7);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qdrn_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(-((qgate_Vgate_sourcep+qdrn_Vgate_sourcep)+qdrn_Vgate_sourcep));
qbulk_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qdrn_Vbulk_sourcep)+qdrn_Vbulk_sourcep));
#endif
qbulk=(-((qgate+qdrn)+qdrn));
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
}
}
}
}
}
else
{
if
((Vbseff<0.0))
{
#if defined(_DERIVATE)
VbseffCV_Vdrainp_sourcep=Vbseff_Vdrainp_sourcep;
VbseffCV_Vbulk_sourcep=Vbseff_Vbulk_sourcep;
#endif
VbseffCV=Vbseff;
}
else
{
#if defined(_DERIVATE)
VbseffCV_Vdrainp_sourcep=(-Phis_Vdrainp_sourcep);
VbseffCV_Vbulk_sourcep=(-Phis_Vbulk_sourcep);
#endif
VbseffCV=(phi-Phis);
}
CoxWL=((cox*weffCV)*leffCV);
#if defined(_DERIVATE)
Noff2_Vdrainp_sourcep=n_Vdrainp_sourcep*noff_param;
Noff2_Vbulk_sourcep=n_Vbulk_sourcep*noff_param;
#endif
Noff2=(n*noff_param);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=(Vtm*Noff2_Vdrainp_sourcep);
T_0_Vbulk_sourcep=(Vtm*Noff2_Vbulk_sourcep);
#endif
T_0=(Vtm*Noff2);
#if defined(_DERIVATE)
VgstNVt_Vdrainp_sourcep=((Vgst_Vdrainp_sourcep*T_0-(Vgst-voffcv_param)*T_0_Vdrainp_sourcep)/T_0/T_0);
VgstNVt_Vgate_sourcep=((Vgst_Vgate_sourcep*T_0-(Vgst-voffcv_param)*T_0_Vgate_sourcep)/T_0/T_0);
VgstNVt_Vbulk_sourcep=((Vgst_Vbulk_sourcep*T_0-(Vgst-voffcv_param)*T_0_Vbulk_sourcep)/T_0/T_0);
#endif
VgstNVt=((Vgst-voffcv_param)/T_0);
if
((VgstNVt>34.0))
{
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=Vgst_Vdrainp_sourcep;
Vgsteff_Vgate_sourcep=Vgst_Vgate_sourcep;
Vgsteff_Vbulk_sourcep=Vgst_Vbulk_sourcep;
#endif
Vgsteff=(Vgst-voffcv_param);
}
else
{
if
((VgstNVt<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=T_0_Vdrainp_sourcep*d00_logE0;
Vgsteff_Vgate_sourcep=T_0_Vgate_sourcep*d00_logE0;
Vgsteff_Vbulk_sourcep=T_0_Vbulk_sourcep*d00_logE0;
#endif
Vgsteff=(T_0*d00_logE0);
}
}
else
{
{
double m00_exp(d00_exp0,VgstNVt)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ExpVgst_Vgate_sourcep=VgstNVt_Vgate_sourcep*d10_exp0;
ExpVgst_Vdrainp_sourcep=VgstNVt_Vdrainp_sourcep*d10_exp0;
ExpVgst_Vbulk_sourcep=VgstNVt_Vbulk_sourcep*d10_exp0;
#endif
ExpVgst=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+ExpVgst))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+ExpVgst))
#endif
#if defined(_DERIVATE)
Vgsteff_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*d00_logE0)+(T_0*ExpVgst_Vdrainp_sourcep*d10_logE0));
Vgsteff_Vgate_sourcep=((T_0_Vgate_sourcep*d00_logE0)+(T_0*ExpVgst_Vgate_sourcep*d10_logE0));
Vgsteff_Vbulk_sourcep=((T_0_Vbulk_sourcep*d00_logE0)+(T_0*ExpVgst_Vbulk_sourcep*d10_logE0));
#endif
Vgsteff=(T_0*d00_logE0);
}
}
}
if
((CAPMOD==1))
{
if
((VERSION<3.2))
{
#if defined(_DERIVATE)
Vfb_Vdrainp_sourcep=(Vth_Vdrainp_sourcep-(k1ox*sqrtPhis_Vdrainp_sourcep));
Vfb_Vbulk_sourcep=(Vth_Vbulk_sourcep-(k1ox*sqrtPhis_Vbulk_sourcep));
#endif
Vfb=((Vth-phi)-(k1ox*sqrtPhis));
}
else
{
#if defined(_DERIVATE)
Vfb_Vdrainp_sourcep=0.0;
Vfb_Vbulk_sourcep=0.0;
#endif
Vfb=vfbzb;
}
#if defined(_DERIVATE)
Arg1_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-VbseffCV_Vdrainp_sourcep)-Vfb_Vdrainp_sourcep)-Vgsteff_Vdrainp_sourcep);
Arg1_Vgate_sourcep=(Vgs_eff_Vgate_sourcep-Vgsteff_Vgate_sourcep);
Arg1_Vbulk_sourcep=(((Vgs_eff_Vbulk_sourcep-VbseffCV_Vbulk_sourcep)-Vfb_Vbulk_sourcep)-Vgsteff_Vbulk_sourcep);
#endif
Arg1=(((Vgs_eff-VbseffCV)-Vfb)-Vgsteff);
if
((Arg1<=0.0))
{
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*Arg1_Vdrainp_sourcep);
qgate_Vgate_sourcep=(CoxWL*Arg1_Vgate_sourcep);
qgate_Vbulk_sourcep=(CoxWL*Arg1_Vbulk_sourcep);
#endif
qgate=(CoxWL*Arg1);
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(0.5*k1ox);
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)+Arg1))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)+Arg1))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=(((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))+Arg1_Vgate_sourcep)*d10_sqrt0;
T1_Vdrainp_sourcep=(((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))+Arg1_Vdrainp_sourcep)*d10_sqrt0;
T1_Vbulk_sourcep=(((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))+Arg1_Vbulk_sourcep)*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=((CoxWL*k1ox)*(T1_Vdrainp_sourcep-T_0_Vdrainp_sourcep));
qgate_Vgate_sourcep=((CoxWL*k1ox)*(T1_Vgate_sourcep-T_0_Vgate_sourcep));
qgate_Vbulk_sourcep=((CoxWL*k1ox)*(T1_Vbulk_sourcep-T_0_Vbulk_sourcep));
#endif
qgate=((CoxWL*k1ox)*(T1-T_0));
}
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(-qgate_Vdrainp_sourcep);
qbulk_Vgate_sourcep=(-qgate_Vgate_sourcep);
qbulk_Vbulk_sourcep=(-qgate_Vbulk_sourcep);
#endif
qbulk=(-qgate);
One_Third_CoxWL=(CoxWL/3.0);
Two_Third_CoxWL=(2.0*One_Third_CoxWL);
#if defined(_DERIVATE)
AbulkCV_Vgate_sourcep=Abulk0_Vgate_sourcep*abulkCVfactor;
AbulkCV_Vdrainp_sourcep=Abulk0_Vdrainp_sourcep*abulkCVfactor;
AbulkCV_Vbulk_sourcep=Abulk0_Vbulk_sourcep*abulkCVfactor;
#endif
AbulkCV=(Abulk0*abulkCVfactor);
#if defined(_DERIVATE)
VdsatCV_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep*AbulkCV-Vgsteff*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vgate_sourcep=((Vgsteff_Vgate_sourcep*AbulkCV-Vgsteff*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep*AbulkCV-Vgsteff*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
VdsatCV=(Vgsteff/AbulkCV);
if
((VdsatCV<Vds))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(Vgsteff_Vgate_sourcep-(VdsatCV_Vgate_sourcep/3.0));
T_0_Vdrainp_sourcep=(Vgsteff_Vdrainp_sourcep-(VdsatCV_Vdrainp_sourcep/3.0));
T_0_Vbulk_sourcep=(Vgsteff_Vbulk_sourcep-(VdsatCV_Vbulk_sourcep/3.0));
#endif
T_0=(Vgsteff-(VdsatCV/3.0));
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(qgate_Vdrainp_sourcep+(CoxWL*T_0_Vdrainp_sourcep));
qgate_Vgate_sourcep=(qgate_Vgate_sourcep+(CoxWL*T_0_Vgate_sourcep));
qgate_Vbulk_sourcep=(qgate_Vbulk_sourcep+(CoxWL*T_0_Vbulk_sourcep));
#endif
qgate=(qgate+(CoxWL*T_0));
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(VdsatCV_Vgate_sourcep-Vgsteff_Vgate_sourcep);
T_0_Vdrainp_sourcep=(VdsatCV_Vdrainp_sourcep-Vgsteff_Vdrainp_sourcep);
T_0_Vbulk_sourcep=(VdsatCV_Vbulk_sourcep-Vgsteff_Vbulk_sourcep);
#endif
T_0=(VdsatCV-Vgsteff);
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep+(One_Third_CoxWL*T_0_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep+(One_Third_CoxWL*T_0_Vgate_sourcep));
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep+(One_Third_CoxWL*T_0_Vbulk_sourcep));
#endif
qbulk=(qbulk+(One_Third_CoxWL*T_0));
if
((XPART>0.5))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(-Two_Third_CoxWL);
}
else
{
if
((XPART<0.5))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=((-0.4)*CoxWL);
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(-One_Third_CoxWL);
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((T_0_Vgate_sourcep*Vgsteff)+(T_0*Vgsteff_Vgate_sourcep));
qsrc_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*Vgsteff)+(T_0*Vgsteff_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=((T_0_Vbulk_sourcep*Vgsteff)+(T_0*Vgsteff_Vbulk_sourcep));
#endif
qsrc=(T_0*Vgsteff);
#endif
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=AbulkCV_Vgate_sourcep*Vds;
T_0_Vdrainp_sourcep=((AbulkCV_Vdrainp_sourcep*Vds)+(AbulkCV*Vds_Vdrainp_sourcep));
T_0_Vbulk_sourcep=AbulkCV_Vbulk_sourcep*Vds;
#endif
T_0=(AbulkCV*Vds);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(12.0*(Vgsteff_Vgate_sourcep-(0.5*T_0_Vgate_sourcep)));
T1_Vdrainp_sourcep=(12.0*(Vgsteff_Vdrainp_sourcep-(0.5*T_0_Vdrainp_sourcep)));
T1_Vbulk_sourcep=(12.0*(Vgsteff_Vbulk_sourcep-(0.5*T_0_Vbulk_sourcep)));
#endif
T1=(12.0*((Vgsteff-(0.5*T_0))+1.0e-20));
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-Vds*T1_Vgate_sourcep/T1/T1);
T2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*T1-Vds*T1_Vdrainp_sourcep)/T1/T1);
T2_Vbulk_sourcep=(-Vds*T1_Vbulk_sourcep/T1/T1);
#endif
T2=(Vds/T1);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((T_0_Vgate_sourcep*T2)+(T_0*T2_Vgate_sourcep));
T3_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T2)+(T_0*T2_Vdrainp_sourcep));
T3_Vbulk_sourcep=((T_0_Vbulk_sourcep*T2)+(T_0*T2_Vbulk_sourcep));
#endif
T3=(T_0*T2);
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(qgate_Vdrainp_sourcep+(CoxWL*((Vgsteff_Vdrainp_sourcep-(0.5*Vds_Vdrainp_sourcep))+T3_Vdrainp_sourcep)));
qgate_Vgate_sourcep=(qgate_Vgate_sourcep+(CoxWL*(Vgsteff_Vgate_sourcep+T3_Vgate_sourcep)));
qgate_Vbulk_sourcep=(qgate_Vbulk_sourcep+(CoxWL*(Vgsteff_Vbulk_sourcep+T3_Vbulk_sourcep)));
#endif
qgate=(qgate+(CoxWL*((Vgsteff-(0.5*Vds))+T3)));
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep+(((CoxWL*(-AbulkCV_Vdrainp_sourcep))*((0.5*Vds)-T3))+((CoxWL*(1.0-AbulkCV))*((0.5*Vds_Vdrainp_sourcep)-T3_Vdrainp_sourcep))));
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep+(((CoxWL*(-AbulkCV_Vgate_sourcep))*((0.5*Vds)-T3))+((CoxWL*(1.0-AbulkCV))*(-T3_Vgate_sourcep))));
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep+(((CoxWL*(-AbulkCV_Vbulk_sourcep))*((0.5*Vds)-T3))+((CoxWL*(1.0-AbulkCV))*(-T3_Vbulk_sourcep))));
#endif
qbulk=(qbulk+((CoxWL*(1.0-AbulkCV))*((0.5*Vds)-T3)));
if
((XPART>0.5))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vgate_sourcep)+(0.25*T_0_Vgate_sourcep))-((((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vgate_sourcep+T1_Vgate_sourcep))/(T1+T1)/(T1+T1))));
qsrc_Vdrainp_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vdrainp_sourcep)+(0.25*T_0_Vdrainp_sourcep))-((((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vdrainp_sourcep+T1_Vdrainp_sourcep))/(T1+T1)/(T1+T1))));
qsrc_Vbulk_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vbulk_sourcep)+(0.25*T_0_Vbulk_sourcep))-((((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vbulk_sourcep+T1_Vbulk_sourcep))/(T1+T1)/(T1+T1))));
#endif
qsrc=((-CoxWL)*(((0.5*Vgsteff)+(0.25*T_0))-((T_0*T_0)/(T1+T1))));
#endif
}
else
{
if
((XPART<0.5))
{
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-(0.5*CoxWL)*((((T1_Vgate_sourcep/12.0)*T1)+((T1/12.0)*T1_Vgate_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
T2_Vdrainp_sourcep=(-(0.5*CoxWL)*((((T1_Vdrainp_sourcep/12.0)*T1)+((T1/12.0)*T1_Vdrainp_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
T2_Vbulk_sourcep=(-(0.5*CoxWL)*((((T1_Vbulk_sourcep/12.0)*T1)+((T1/12.0)*T1_Vbulk_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
#endif
T2=((0.5*CoxWL)/(((T1/12.0)*T1)/12.0));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((Vgsteff_Vgate_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))/3.0)+((Vgsteff_Vgate_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vgate_sourcep-((4.0*T_0_Vgate_sourcep)/3.0)))))))-((((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vgate_sourcep))/15.0));
T3_Vdrainp_sourcep=(((Vgsteff_Vdrainp_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))/3.0)+((Vgsteff_Vdrainp_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vdrainp_sourcep-((4.0*T_0_Vdrainp_sourcep)/3.0)))))))-((((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vdrainp_sourcep))/15.0));
T3_Vbulk_sourcep=(((Vgsteff_Vbulk_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))/3.0)+((Vgsteff_Vbulk_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vbulk_sourcep-((4.0*T_0_Vbulk_sourcep)/3.0)))))))-((((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vbulk_sourcep))/15.0));
#endif
T3=((Vgsteff*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))-((((2.0*T_0)*T_0)*T_0)/15.0));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(((-T2_Vgate_sourcep)*T3)+((-T2)*T3_Vgate_sourcep));
qsrc_Vdrainp_sourcep=(((-T2_Vdrainp_sourcep)*T3)+((-T2)*T3_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=(((-T2_Vbulk_sourcep)*T3)+((-T2)*T3_Vbulk_sourcep));
#endif
qsrc=((-T2)*T3);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((-0.5)*(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qsrc_Vdrainp_sourcep=((-0.5)*(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=((-0.5)*(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qsrc=((-0.5)*(qgate+qbulk));
#endif
}
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qsrc_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qsrc_Vbulk_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
#endif
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qinv_Vgate_sourcep=(-(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qinv_Vbulk_sourcep=(-(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qinv=(-(qgate+qbulk));
}
else
{
if
((CAPMOD==2))
{
if
((VERSION<3.2))
{
#if defined(_DERIVATE)
Vfb_Vdrainp_sourcep=(Vth_Vdrainp_sourcep-(k1ox*sqrtPhis_Vdrainp_sourcep));
Vfb_Vbulk_sourcep=(Vth_Vbulk_sourcep-(k1ox*sqrtPhis_Vbulk_sourcep));
#endif
Vfb=((Vth-phi)-(k1ox*sqrtPhis));
}
else
{
#if defined(_DERIVATE)
Vfb_Vdrainp_sourcep=0.0;
Vfb_Vbulk_sourcep=0.0;
#endif
Vfb=vfbzb;
}
#if defined(_DERIVATE)
V3_Vdrainp_sourcep=((Vfb_Vdrainp_sourcep-Vgs_eff_Vdrainp_sourcep)+VbseffCV_Vdrainp_sourcep);
V3_Vbulk_sourcep=((Vfb_Vbulk_sourcep-Vgs_eff_Vbulk_sourcep)+VbseffCV_Vbulk_sourcep);
V3_Vgate_sourcep=(-Vgs_eff_Vgate_sourcep);
#endif
V3=(((Vfb-Vgs_eff)+VbseffCV)-0.02);
if
((Vfb<=0.0))
{
{
double m00_sqrt(d00_sqrt0,((V3*V3)-((4.0*0.02)*Vfb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V3*V3)-((4.0*0.02)*Vfb)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((V3_Vgate_sourcep*V3)+(V3*V3_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=(((V3_Vdrainp_sourcep*V3)+(V3*V3_Vdrainp_sourcep))-((4.0*0.02)*Vfb_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=(((V3_Vbulk_sourcep*V3)+(V3*V3_Vbulk_sourcep))-((4.0*0.02)*Vfb_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-(-0.02)*T_0_Vgate_sourcep/T_0/T_0);
T2_Vdrainp_sourcep=(-(-0.02)*T_0_Vdrainp_sourcep/T_0/T_0);
T2_Vbulk_sourcep=(-(-0.02)*T_0_Vbulk_sourcep/T_0/T_0);
#endif
T2=((-0.02)/T_0);
}
else
{
{
double m00_sqrt(d00_sqrt0,((V3*V3)+((4.0*0.02)*Vfb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V3*V3)+((4.0*0.02)*Vfb)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((V3_Vgate_sourcep*V3)+(V3*V3_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=(((V3_Vdrainp_sourcep*V3)+(V3*V3_Vdrainp_sourcep))+((4.0*0.02)*Vfb_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=(((V3_Vbulk_sourcep*V3)+(V3*V3_Vbulk_sourcep))+((4.0*0.02)*Vfb_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-0.02*T_0_Vgate_sourcep/T_0/T_0);
T2_Vdrainp_sourcep=(-0.02*T_0_Vdrainp_sourcep/T_0/T_0);
T2_Vbulk_sourcep=(-0.02*T_0_Vbulk_sourcep/T_0/T_0);
#endif
T2=(0.02/T_0);
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=(0.5*((V3_Vgate_sourcep*T_0-V3*T_0_Vgate_sourcep)/T_0/T_0));
T1_Vdrainp_sourcep=(0.5*((V3_Vdrainp_sourcep*T_0-V3*T_0_Vdrainp_sourcep)/T_0/T_0));
T1_Vbulk_sourcep=(0.5*((V3_Vbulk_sourcep*T_0-V3*T_0_Vbulk_sourcep)/T_0/T_0));
#endif
T1=(0.5*(1.0+(V3/T_0)));
#if defined(_DERIVATE)
Vfbeff_Vdrainp_sourcep=(Vfb_Vdrainp_sourcep-(0.5*(V3_Vdrainp_sourcep+T_0_Vdrainp_sourcep)));
Vfbeff_Vbulk_sourcep=(Vfb_Vbulk_sourcep-(0.5*(V3_Vbulk_sourcep+T_0_Vbulk_sourcep)));
Vfbeff_Vgate_sourcep=(-(0.5*(V3_Vgate_sourcep+T_0_Vgate_sourcep)));
#endif
Vfbeff=(Vfb-(0.5*(V3+T_0)));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qac0_Vdrainp_sourcep=(CoxWL*(Vfbeff_Vdrainp_sourcep-Vfb_Vdrainp_sourcep));
Qac0_Vbulk_sourcep=(CoxWL*(Vfbeff_Vbulk_sourcep-Vfb_Vbulk_sourcep));
Qac0_Vgate_sourcep=(CoxWL*Vfbeff_Vgate_sourcep);
#endif
Qac0=(CoxWL*(Vfbeff-Vfb));
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(0.5*k1ox);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((Vgs_eff_Vgate_sourcep-Vfbeff_Vgate_sourcep)-Vgsteff_Vgate_sourcep);
T3_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-Vfbeff_Vdrainp_sourcep)-VbseffCV_Vdrainp_sourcep)-Vgsteff_Vdrainp_sourcep);
T3_Vbulk_sourcep=(((Vgs_eff_Vbulk_sourcep-Vfbeff_Vbulk_sourcep)-VbseffCV_Vbulk_sourcep)-Vgsteff_Vbulk_sourcep);
#endif
T3=(((Vgs_eff-Vfbeff)-VbseffCV)-Vgsteff);
if
((k1ox==0.0))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=0.0;
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=0.0;
}
else
{
if
((T3<0.0))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(T_0_Vgate_sourcep+(T3_Vgate_sourcep/k1ox));
T1_Vdrainp_sourcep=(T_0_Vdrainp_sourcep+(T3_Vdrainp_sourcep/k1ox));
T1_Vbulk_sourcep=(T_0_Vbulk_sourcep+(T3_Vbulk_sourcep/k1ox));
#endif
T1=(T_0+(T3/k1ox));
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=CoxWL;
}
else
{
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)+T3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)+T3))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=(((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))+T3_Vgate_sourcep)*d10_sqrt0;
T1_Vdrainp_sourcep=(((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))+T3_Vdrainp_sourcep)*d10_sqrt0;
T1_Vbulk_sourcep=(((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))+T3_Vbulk_sourcep)*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=(((CoxWL*T_0_Vgate_sourcep)*T1-(CoxWL*T_0)*T1_Vgate_sourcep)/T1/T1);
T2_Vdrainp_sourcep=(((CoxWL*T_0_Vdrainp_sourcep)*T1-(CoxWL*T_0)*T1_Vdrainp_sourcep)/T1/T1);
T2_Vbulk_sourcep=(((CoxWL*T_0_Vbulk_sourcep)*T1-(CoxWL*T_0)*T1_Vbulk_sourcep)/T1/T1);
#endif
T2=((CoxWL*T_0)/T1);
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qsub0_Vgate_sourcep=((CoxWL*k1ox)*(T1_Vgate_sourcep-T_0_Vgate_sourcep));
Qsub0_Vdrainp_sourcep=((CoxWL*k1ox)*(T1_Vdrainp_sourcep-T_0_Vdrainp_sourcep));
Qsub0_Vbulk_sourcep=((CoxWL*k1ox)*(T1_Vbulk_sourcep-T_0_Vbulk_sourcep));
#endif
Qsub0=((CoxWL*k1ox)*(T1-T_0));
#endif
#if defined(_DERIVATE)
AbulkCV_Vgate_sourcep=Abulk0_Vgate_sourcep*abulkCVfactor;
AbulkCV_Vdrainp_sourcep=Abulk0_Vdrainp_sourcep*abulkCVfactor;
AbulkCV_Vbulk_sourcep=Abulk0_Vbulk_sourcep*abulkCVfactor;
#endif
AbulkCV=(Abulk0*abulkCVfactor);
#if defined(_DERIVATE)
VdsatCV_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep*AbulkCV-Vgsteff*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vgate_sourcep=((Vgsteff_Vgate_sourcep*AbulkCV-Vgsteff*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep*AbulkCV-Vgsteff*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
VdsatCV=(Vgsteff/AbulkCV);
#if defined(_DERIVATE)
V4_Vdrainp_sourcep=(VdsatCV_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
V4_Vgate_sourcep=VdsatCV_Vgate_sourcep;
V4_Vbulk_sourcep=VdsatCV_Vbulk_sourcep;
#endif
V4=((VdsatCV-Vds)-0.02);
{
double m00_sqrt(d00_sqrt0,((V4*V4)+((4.0*0.02)*VdsatCV)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V4*V4)+((4.0*0.02)*VdsatCV)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(((V4_Vgate_sourcep*V4)+(V4*V4_Vgate_sourcep))+((4.0*0.02)*VdsatCV_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=(((V4_Vdrainp_sourcep*V4)+(V4*V4_Vdrainp_sourcep))+((4.0*0.02)*VdsatCV_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=(((V4_Vbulk_sourcep*V4)+(V4*V4_Vbulk_sourcep))+((4.0*0.02)*VdsatCV_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
VdseffCV_Vdrainp_sourcep=(VdsatCV_Vdrainp_sourcep-(0.5*(V4_Vdrainp_sourcep+T_0_Vdrainp_sourcep)));
VdseffCV_Vgate_sourcep=(VdsatCV_Vgate_sourcep-(0.5*(V4_Vgate_sourcep+T_0_Vgate_sourcep)));
VdseffCV_Vbulk_sourcep=(VdsatCV_Vbulk_sourcep-(0.5*(V4_Vbulk_sourcep+T_0_Vbulk_sourcep)));
#endif
VdseffCV=(VdsatCV-(0.5*(V4+T_0)));
if
(((Vds==0.0)&&(VERSION==3.24)))
{
#if defined(_DERIVATE)
VdseffCV_Vdrainp_sourcep=0.0;
VdseffCV_Vgate_sourcep=0.0;
VdseffCV_Vbulk_sourcep=0.0;
#endif
VdseffCV=0.0;
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((AbulkCV_Vgate_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vgate_sourcep));
T_0_Vdrainp_sourcep=((AbulkCV_Vdrainp_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vdrainp_sourcep));
T_0_Vbulk_sourcep=((AbulkCV_Vbulk_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vbulk_sourcep));
#endif
T_0=(AbulkCV*VdseffCV);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(12.0*(Vgsteff_Vgate_sourcep-(0.5*T_0_Vgate_sourcep)));
T1_Vdrainp_sourcep=(12.0*(Vgsteff_Vdrainp_sourcep-(0.5*T_0_Vdrainp_sourcep)));
T1_Vbulk_sourcep=(12.0*(Vgsteff_Vbulk_sourcep-(0.5*T_0_Vbulk_sourcep)));
#endif
T1=(12.0*((Vgsteff-(0.5*T_0))+1e-20));
#if defined(_DERIVATE)
T2_Vgate_sourcep=((VdseffCV_Vgate_sourcep*T1-VdseffCV*T1_Vgate_sourcep)/T1/T1);
T2_Vdrainp_sourcep=((VdseffCV_Vdrainp_sourcep*T1-VdseffCV*T1_Vdrainp_sourcep)/T1/T1);
T2_Vbulk_sourcep=((VdseffCV_Vbulk_sourcep*T1-VdseffCV*T1_Vbulk_sourcep)/T1/T1);
#endif
T2=(VdseffCV/T1);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((T_0_Vgate_sourcep*T2)+(T_0*T2_Vgate_sourcep));
T3_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T2)+(T_0*T2_Vdrainp_sourcep));
T3_Vbulk_sourcep=((T_0_Vbulk_sourcep*T2)+(T_0*T2_Vbulk_sourcep));
#endif
T3=(T_0*T2);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T4_Vgate_sourcep=(-(((((12.0*T2_Vgate_sourcep)*T2)+((12.0*T2)*T2_Vgate_sourcep))*AbulkCV)+(((12.0*T2)*T2)*AbulkCV_Vgate_sourcep)));
T4_Vdrainp_sourcep=(-(((((12.0*T2_Vdrainp_sourcep)*T2)+((12.0*T2)*T2_Vdrainp_sourcep))*AbulkCV)+(((12.0*T2)*T2)*AbulkCV_Vdrainp_sourcep)));
T4_Vbulk_sourcep=(-(((((12.0*T2_Vbulk_sourcep)*T2)+((12.0*T2)*T2_Vbulk_sourcep))*AbulkCV)+(((12.0*T2)*T2)*AbulkCV_Vbulk_sourcep)));
#endif
T4=(1.0-(((12.0*T2)*T2)*AbulkCV));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T5_Vgate_sourcep=(((((6.0*T_0_Vgate_sourcep)*((4.0*Vgsteff)-T_0))+((6.0*T_0)*((4.0*Vgsteff_Vgate_sourcep)-T_0_Vgate_sourcep)))*(T1*T1)-((6.0*T_0)*((4.0*Vgsteff)-T_0))*((T1_Vgate_sourcep*T1)+(T1*T1_Vgate_sourcep)))/(T1*T1)/(T1*T1));
T5_Vdrainp_sourcep=(((((6.0*T_0_Vdrainp_sourcep)*((4.0*Vgsteff)-T_0))+((6.0*T_0)*((4.0*Vgsteff_Vdrainp_sourcep)-T_0_Vdrainp_sourcep)))*(T1*T1)-((6.0*T_0)*((4.0*Vgsteff)-T_0))*((T1_Vdrainp_sourcep*T1)+(T1*T1_Vdrainp_sourcep)))/(T1*T1)/(T1*T1));
T5_Vbulk_sourcep=(((((6.0*T_0_Vbulk_sourcep)*((4.0*Vgsteff)-T_0))+((6.0*T_0)*((4.0*Vgsteff_Vbulk_sourcep)-T_0_Vbulk_sourcep)))*(T1*T1)-((6.0*T_0)*((4.0*Vgsteff)-T_0))*((T1_Vbulk_sourcep*T1)+(T1*T1_Vbulk_sourcep)))/(T1*T1)/(T1*T1));
#endif
T5=((((6.0*T_0)*((4.0*Vgsteff)-T_0))/(T1*T1))-0.5);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T6_Vgate_sourcep=(((((12.0*T2_Vgate_sourcep)*T2)+((12.0*T2)*T2_Vgate_sourcep))*Vgsteff)+(((12.0*T2)*T2)*Vgsteff_Vgate_sourcep));
T6_Vdrainp_sourcep=(((((12.0*T2_Vdrainp_sourcep)*T2)+((12.0*T2)*T2_Vdrainp_sourcep))*Vgsteff)+(((12.0*T2)*T2)*Vgsteff_Vdrainp_sourcep));
T6_Vbulk_sourcep=(((((12.0*T2_Vbulk_sourcep)*T2)+((12.0*T2)*T2_Vbulk_sourcep))*Vgsteff)+(((12.0*T2)*T2)*Vgsteff_Vbulk_sourcep));
#endif
T6=(((12.0*T2)*T2)*Vgsteff);
#endif
#if defined(_DERIVATE)
qinoi_Vdrainp_sourcep=((-CoxWL)*((Vgsteff_Vdrainp_sourcep-(0.5*T_0_Vdrainp_sourcep))+((AbulkCV_Vdrainp_sourcep*T3)+(AbulkCV*T3_Vdrainp_sourcep))));
qinoi_Vgate_sourcep=((-CoxWL)*((Vgsteff_Vgate_sourcep-(0.5*T_0_Vgate_sourcep))+((AbulkCV_Vgate_sourcep*T3)+(AbulkCV*T3_Vgate_sourcep))));
qinoi_Vbulk_sourcep=((-CoxWL)*((Vgsteff_Vbulk_sourcep-(0.5*T_0_Vbulk_sourcep))+((AbulkCV_Vbulk_sourcep*T3)+(AbulkCV*T3_Vbulk_sourcep))));
#endif
qinoi=((-CoxWL)*((Vgsteff-(0.5*T_0))+(AbulkCV*T3)));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(CoxWL*((Vgsteff_Vdrainp_sourcep-(0.5*VdseffCV_Vdrainp_sourcep))+T3_Vdrainp_sourcep));
qgate_Vgate_sourcep=(CoxWL*((Vgsteff_Vgate_sourcep-(0.5*VdseffCV_Vgate_sourcep))+T3_Vgate_sourcep));
qgate_Vbulk_sourcep=(CoxWL*((Vgsteff_Vbulk_sourcep-(0.5*VdseffCV_Vbulk_sourcep))+T3_Vbulk_sourcep));
#endif
qgate=(CoxWL*((Vgsteff-(0.5*VdseffCV))+T3));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T7_Vgate_sourcep=(-AbulkCV_Vgate_sourcep);
T7_Vdrainp_sourcep=(-AbulkCV_Vdrainp_sourcep);
T7_Vbulk_sourcep=(-AbulkCV_Vbulk_sourcep);
#endif
T7=(1.0-AbulkCV);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(((CoxWL*T7_Vdrainp_sourcep)*((0.5*VdseffCV)-T3))+((CoxWL*T7)*((0.5*VdseffCV_Vdrainp_sourcep)-T3_Vdrainp_sourcep)));
qbulk_Vgate_sourcep=(((CoxWL*T7_Vgate_sourcep)*((0.5*VdseffCV)-T3))+((CoxWL*T7)*((0.5*VdseffCV_Vgate_sourcep)-T3_Vgate_sourcep)));
qbulk_Vbulk_sourcep=(((CoxWL*T7_Vbulk_sourcep)*((0.5*VdseffCV)-T3))+((CoxWL*T7)*((0.5*VdseffCV_Vbulk_sourcep)-T3_Vbulk_sourcep)));
#endif
qbulk=((CoxWL*T7)*((0.5*VdseffCV)-T3));
#endif
if
((XPART>0.5))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vgate_sourcep)+(0.25*T_0_Vgate_sourcep))-((((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vgate_sourcep+T1_Vgate_sourcep))/(T1+T1)/(T1+T1))));
qsrc_Vdrainp_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vdrainp_sourcep)+(0.25*T_0_Vdrainp_sourcep))-((((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vdrainp_sourcep+T1_Vdrainp_sourcep))/(T1+T1)/(T1+T1))));
qsrc_Vbulk_sourcep=((-CoxWL)*(((0.5*Vgsteff_Vbulk_sourcep)+(0.25*T_0_Vbulk_sourcep))-((((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))*(T1+T1)-(T_0*T_0)*(T1_Vbulk_sourcep+T1_Vbulk_sourcep))/(T1+T1)/(T1+T1))));
#endif
qsrc=((-CoxWL)*(((0.5*Vgsteff)+(0.25*T_0))-((T_0*T_0)/(T1+T1))));
#endif
}
else
{
if
((XPART<0.5))
{
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-(0.5*CoxWL)*((((T1_Vgate_sourcep/12.0)*T1)+((T1/12.0)*T1_Vgate_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
T2_Vdrainp_sourcep=(-(0.5*CoxWL)*((((T1_Vdrainp_sourcep/12.0)*T1)+((T1/12.0)*T1_Vdrainp_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
T2_Vbulk_sourcep=(-(0.5*CoxWL)*((((T1_Vbulk_sourcep/12.0)*T1)+((T1/12.0)*T1_Vbulk_sourcep))/12.0)/(((T1/12.0)*T1)/12.0)/(((T1/12.0)*T1)/12.0));
#endif
T2=((0.5*CoxWL)/(((T1/12.0)*T1)/12.0));
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((Vgsteff_Vgate_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))/3.0)+((Vgsteff_Vgate_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vgate_sourcep-((4.0*T_0_Vgate_sourcep)/3.0)))))))-((((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vgate_sourcep))/15.0));
T3_Vdrainp_sourcep=(((Vgsteff_Vdrainp_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))/3.0)+((Vgsteff_Vdrainp_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vdrainp_sourcep-((4.0*T_0_Vdrainp_sourcep)/3.0)))))))-((((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vdrainp_sourcep))/15.0));
T3_Vbulk_sourcep=(((Vgsteff_Vbulk_sourcep*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))+(Vgsteff*(((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))/3.0)+((Vgsteff_Vbulk_sourcep*(Vgsteff-((4.0*T_0)/3.0)))+(Vgsteff*(Vgsteff_Vbulk_sourcep-((4.0*T_0_Vbulk_sourcep)/3.0)))))))-((((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vbulk_sourcep))/15.0));
#endif
T3=((Vgsteff*((((2.0*T_0)*T_0)/3.0)+(Vgsteff*(Vgsteff-((4.0*T_0)/3.0)))))-((((2.0*T_0)*T_0)*T_0)/15.0));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(((-T2_Vgate_sourcep)*T3)+((-T2)*T3_Vgate_sourcep));
qsrc_Vdrainp_sourcep=(((-T2_Vdrainp_sourcep)*T3)+((-T2)*T3_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=(((-T2_Vbulk_sourcep)*T3)+((-T2)*T3_Vbulk_sourcep));
#endif
qsrc=((-T2)*T3);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((-0.5)*(qgate_Vgate_sourcep+qbulk_Vgate_sourcep));
qsrc_Vdrainp_sourcep=((-0.5)*(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=((-0.5)*(qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep));
#endif
qsrc=((-0.5)*(qgate+qbulk));
#endif
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+Qac0_Vdrainp_sourcep)+Qsub0_Vdrainp_sourcep);
qgate_Vgate_sourcep=((qgate_Vgate_sourcep+Qac0_Vgate_sourcep)+Qsub0_Vgate_sourcep);
qgate_Vbulk_sourcep=((qgate_Vbulk_sourcep+Qac0_Vbulk_sourcep)+Qsub0_Vbulk_sourcep);
#endif
qgate=((qgate+Qac0)+Qsub0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep-(Qac0_Vdrainp_sourcep+Qsub0_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep-(Qac0_Vgate_sourcep+Qsub0_Vgate_sourcep));
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep-(Qac0_Vbulk_sourcep+Qsub0_Vbulk_sourcep));
#endif
qbulk=(qbulk-(Qac0+Qsub0));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qsrc_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qsrc_Vbulk_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
#endif
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=qinoi_Vdrainp_sourcep;
qinv_Vgate_sourcep=qinoi_Vgate_sourcep;
qinv_Vbulk_sourcep=qinoi_Vbulk_sourcep;
#endif
qinv=qinoi;
}
else
{
if
((CAPMOD==3))
{
#if defined(_DERIVATE)
V3_Vdrainp_sourcep=((-Vgs_eff_Vdrainp_sourcep)+VbseffCV_Vdrainp_sourcep);
V3_Vbulk_sourcep=((-Vgs_eff_Vbulk_sourcep)+VbseffCV_Vbulk_sourcep);
V3_Vgate_sourcep=(-Vgs_eff_Vgate_sourcep);
#endif
V3=(((vfbzb-Vgs_eff)+VbseffCV)-0.02);
if
((vfbzb<=0.0))
{
{
double m00_sqrt(d00_sqrt0,((V3*V3)-((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V3*V3)-((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((V3_Vgate_sourcep*V3)+(V3*V3_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=((V3_Vdrainp_sourcep*V3)+(V3*V3_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=((V3_Vbulk_sourcep*V3)+(V3*V3_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-(-0.02)*T_0_Vgate_sourcep/T_0/T_0);
T2_Vdrainp_sourcep=(-(-0.02)*T_0_Vdrainp_sourcep/T_0/T_0);
T2_Vbulk_sourcep=(-(-0.02)*T_0_Vbulk_sourcep/T_0/T_0);
#endif
T2=((-0.02)/T_0);
}
else
{
{
double m00_sqrt(d00_sqrt0,((V3*V3)+((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V3*V3)+((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((V3_Vgate_sourcep*V3)+(V3*V3_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=((V3_Vdrainp_sourcep*V3)+(V3*V3_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=((V3_Vbulk_sourcep*V3)+(V3*V3_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-0.02*T_0_Vgate_sourcep/T_0/T_0);
T2_Vdrainp_sourcep=(-0.02*T_0_Vdrainp_sourcep/T_0/T_0);
T2_Vbulk_sourcep=(-0.02*T_0_Vbulk_sourcep/T_0/T_0);
#endif
T2=(0.02/T_0);
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=(0.5*((V3_Vgate_sourcep*T_0-V3*T_0_Vgate_sourcep)/T_0/T_0));
T1_Vdrainp_sourcep=(0.5*((V3_Vdrainp_sourcep*T_0-V3*T_0_Vdrainp_sourcep)/T_0/T_0));
T1_Vbulk_sourcep=(0.5*((V3_Vbulk_sourcep*T_0-V3*T_0_Vbulk_sourcep)/T_0/T_0));
#endif
T1=(0.5*(1.0+(V3/T_0)));
#if defined(_DERIVATE)
Vfbeff_Vdrainp_sourcep=(-(0.5*(V3_Vdrainp_sourcep+T_0_Vdrainp_sourcep)));
Vfbeff_Vbulk_sourcep=(-(0.5*(V3_Vbulk_sourcep+T_0_Vbulk_sourcep)));
Vfbeff_Vgate_sourcep=(-(0.5*(V3_Vgate_sourcep+T_0_Vgate_sourcep)));
#endif
Vfbeff=(vfbzb-(0.5*(V3+T_0)));
Cox=cox;
Tox=(1.0e8*tox);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(Vgs_eff_Vgate_sourcep/Tox);
T_0_Vdrainp_sourcep=((Vgs_eff_Vdrainp_sourcep-VbseffCV_Vdrainp_sourcep)/Tox);
T_0_Vbulk_sourcep=((Vgs_eff_Vbulk_sourcep-VbseffCV_Vbulk_sourcep)/Tox);
#endif
T_0=(((Vgs_eff-VbseffCV)-vfbzb)/Tox);
#if defined(_DERIVATE)
tmp_Vgate_sourcep=T_0_Vgate_sourcep*acde;
tmp_Vdrainp_sourcep=T_0_Vdrainp_sourcep*acde;
tmp_Vbulk_sourcep=T_0_Vbulk_sourcep*acde;
#endif
tmp=(T_0*acde);
if
((((-34.0)<tmp)&&(tmp<34.0)))
{
{
double m00_exp(d00_exp0,tmp)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
Tcen_Vgate_sourcep=(ldeb*tmp_Vgate_sourcep*d10_exp0);
Tcen_Vdrainp_sourcep=(ldeb*tmp_Vdrainp_sourcep*d10_exp0);
Tcen_Vbulk_sourcep=(ldeb*tmp_Vbulk_sourcep*d10_exp0);
#endif
Tcen=(ldeb*d00_exp0);
}
}
else
{
if
((tmp<=(-34.0)))
{
#if defined(_DERIVATE)
Tcen_Vgate_sourcep=0.0;
Tcen_Vdrainp_sourcep=0.0;
Tcen_Vbulk_sourcep=0.0;
#endif
Tcen=(ldeb*1.713908431e-15);
}
else
{
#if defined(_DERIVATE)
Tcen_Vgate_sourcep=0.0;
Tcen_Vdrainp_sourcep=0.0;
Tcen_Vbulk_sourcep=0.0;
#endif
Tcen=(ldeb*5.834617425e14);
}
}
LINK=(1.0e-3*tox);
#if defined(_DERIVATE)
V3_Vdrainp_sourcep=(-Tcen_Vdrainp_sourcep);
V3_Vbulk_sourcep=(-Tcen_Vbulk_sourcep);
V3_Vgate_sourcep=(-Tcen_Vgate_sourcep);
#endif
V3=((ldeb-Tcen)-LINK);
{
double m00_sqrt(d00_sqrt0,((V3*V3)+((4.0*LINK)*ldeb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V3*V3)+((4.0*LINK)*ldeb)))
#endif
#if defined(_DERIVATE)
V4_Vdrainp_sourcep=((V3_Vdrainp_sourcep*V3)+(V3*V3_Vdrainp_sourcep))*d10_sqrt0;
V4_Vgate_sourcep=((V3_Vgate_sourcep*V3)+(V3*V3_Vgate_sourcep))*d10_sqrt0;
V4_Vbulk_sourcep=((V3_Vbulk_sourcep*V3)+(V3*V3_Vbulk_sourcep))*d10_sqrt0;
#endif
V4=d00_sqrt0;
}
#if defined(_DERIVATE)
Tcen_Vgate_sourcep=(-(0.5*(V3_Vgate_sourcep+V4_Vgate_sourcep)));
Tcen_Vdrainp_sourcep=(-(0.5*(V3_Vdrainp_sourcep+V4_Vdrainp_sourcep)));
Tcen_Vbulk_sourcep=(-(0.5*(V3_Vbulk_sourcep+V4_Vbulk_sourcep)));
#endif
Tcen=(ldeb-(0.5*(V3+V4)));
#if defined(_DERIVATE)
T1_Vgate_sourcep=(0.5*((V3_Vgate_sourcep*V4-V3*V4_Vgate_sourcep)/V4/V4));
T1_Vdrainp_sourcep=(0.5*((V3_Vdrainp_sourcep*V4-V3*V4_Vdrainp_sourcep)/V4/V4));
T1_Vbulk_sourcep=(0.5*((V3_Vbulk_sourcep*V4-V3*V4_Vbulk_sourcep)/V4/V4));
#endif
T1=(0.5*(1.0+(V3/V4)));
#if defined(_DERIVATE)
Ccen_Vgate_sourcep=(-1.03594e-10*Tcen_Vgate_sourcep/Tcen/Tcen);
Ccen_Vdrainp_sourcep=(-1.03594e-10*Tcen_Vdrainp_sourcep/Tcen/Tcen);
Ccen_Vbulk_sourcep=(-1.03594e-10*Tcen_Vbulk_sourcep/Tcen/Tcen);
#endif
Ccen=(1.03594e-10/Tcen);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-Cox*Ccen_Vgate_sourcep/(Cox+Ccen)/(Cox+Ccen));
T2_Vdrainp_sourcep=(-Cox*Ccen_Vdrainp_sourcep/(Cox+Ccen)/(Cox+Ccen));
T2_Vbulk_sourcep=(-Cox*Ccen_Vbulk_sourcep/(Cox+Ccen)/(Cox+Ccen));
#endif
T2=(Cox/(Cox+Ccen));
#if defined(_DERIVATE)
Coxeff_Vgate_sourcep=((T2_Vgate_sourcep*Ccen)+(T2*Ccen_Vgate_sourcep));
Coxeff_Vdrainp_sourcep=((T2_Vdrainp_sourcep*Ccen)+(T2*Ccen_Vdrainp_sourcep));
Coxeff_Vbulk_sourcep=((T2_Vbulk_sourcep*Ccen)+(T2*Ccen_Vbulk_sourcep));
#endif
Coxeff=(T2*Ccen);
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((-Ccen_Vgate_sourcep)*Tcen-(-Ccen)*Tcen_Vgate_sourcep)/Tcen/Tcen);
T3_Vdrainp_sourcep=(((-Ccen_Vdrainp_sourcep)*Tcen-(-Ccen)*Tcen_Vdrainp_sourcep)/Tcen/Tcen);
T3_Vbulk_sourcep=(((-Ccen_Vbulk_sourcep)*Tcen-(-Ccen)*Tcen_Vbulk_sourcep)/Tcen/Tcen);
#endif
T3=((-Ccen)/Tcen);
#if defined(_DERIVATE)
CoxWLcen_Vgate_sourcep=((CoxWL*Coxeff_Vgate_sourcep)/Cox);
CoxWLcen_Vdrainp_sourcep=((CoxWL*Coxeff_Vdrainp_sourcep)/Cox);
CoxWLcen_Vbulk_sourcep=((CoxWL*Coxeff_Vbulk_sourcep)/Cox);
#endif
CoxWLcen=((CoxWL*Coxeff)/Cox);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qac0_Vdrainp_sourcep=((CoxWLcen_Vdrainp_sourcep*(Vfbeff-vfbzb))+(CoxWLcen*Vfbeff_Vdrainp_sourcep));
Qac0_Vbulk_sourcep=((CoxWLcen_Vbulk_sourcep*(Vfbeff-vfbzb))+(CoxWLcen*Vfbeff_Vbulk_sourcep));
Qac0_Vgate_sourcep=((CoxWLcen_Vgate_sourcep*(Vfbeff-vfbzb))+(CoxWLcen*Vfbeff_Vgate_sourcep));
#endif
Qac0=(CoxWLcen*(Vfbeff-vfbzb));
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(0.5*k1ox);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((Vgs_eff_Vgate_sourcep-Vfbeff_Vgate_sourcep)-Vgsteff_Vgate_sourcep);
T3_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-Vfbeff_Vdrainp_sourcep)-VbseffCV_Vdrainp_sourcep)-Vgsteff_Vdrainp_sourcep);
T3_Vbulk_sourcep=(((Vgs_eff_Vbulk_sourcep-Vfbeff_Vbulk_sourcep)-VbseffCV_Vbulk_sourcep)-Vgsteff_Vbulk_sourcep);
#endif
T3=(((Vgs_eff-Vfbeff)-VbseffCV)-Vgsteff);
if
((k1ox==0.0))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=0.0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=0.0;
#if defined(_DERIVATE)
T2_Vgate_sourcep=0.0;
T2_Vdrainp_sourcep=0.0;
T2_Vbulk_sourcep=0.0;
#endif
T2=0.0;
}
else
{
if
((T3<0.0))
{
#if defined(_DERIVATE)
T1_Vgate_sourcep=(T_0_Vgate_sourcep+(T3_Vgate_sourcep/k1ox));
T1_Vdrainp_sourcep=(T_0_Vdrainp_sourcep+(T3_Vdrainp_sourcep/k1ox));
T1_Vbulk_sourcep=(T_0_Vbulk_sourcep+(T3_Vbulk_sourcep/k1ox));
#endif
T1=(T_0+(T3/k1ox));
#if defined(_DERIVATE)
T2_Vgate_sourcep=CoxWLcen_Vgate_sourcep;
T2_Vdrainp_sourcep=CoxWLcen_Vdrainp_sourcep;
T2_Vbulk_sourcep=CoxWLcen_Vbulk_sourcep;
#endif
T2=CoxWLcen;
}
else
{
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)+T3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)+T3))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=(((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))+T3_Vgate_sourcep)*d10_sqrt0;
T1_Vdrainp_sourcep=(((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))+T3_Vdrainp_sourcep)*d10_sqrt0;
T1_Vbulk_sourcep=(((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))+T3_Vbulk_sourcep)*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#if defined(_DERIVATE)
T2_Vgate_sourcep=((((CoxWLcen_Vgate_sourcep*T_0)+(CoxWLcen*T_0_Vgate_sourcep))*T1-(CoxWLcen*T_0)*T1_Vgate_sourcep)/T1/T1);
T2_Vdrainp_sourcep=((((CoxWLcen_Vdrainp_sourcep*T_0)+(CoxWLcen*T_0_Vdrainp_sourcep))*T1-(CoxWLcen*T_0)*T1_Vdrainp_sourcep)/T1/T1);
T2_Vbulk_sourcep=((((CoxWLcen_Vbulk_sourcep*T_0)+(CoxWLcen*T_0_Vbulk_sourcep))*T1-(CoxWLcen*T_0)*T1_Vbulk_sourcep)/T1/T1);
#endif
T2=((CoxWLcen*T_0)/T1);
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qsub0_Vgate_sourcep=((CoxWLcen_Vgate_sourcep*k1ox*(T1-T_0))+((CoxWLcen*k1ox)*(T1_Vgate_sourcep-T_0_Vgate_sourcep)));
Qsub0_Vdrainp_sourcep=((CoxWLcen_Vdrainp_sourcep*k1ox*(T1-T_0))+((CoxWLcen*k1ox)*(T1_Vdrainp_sourcep-T_0_Vdrainp_sourcep)));
Qsub0_Vbulk_sourcep=((CoxWLcen_Vbulk_sourcep*k1ox*(T1-T_0))+((CoxWLcen*k1ox)*(T1_Vbulk_sourcep-T_0_Vbulk_sourcep)));
#endif
Qsub0=((CoxWLcen*k1ox)*(T1-T_0));
#endif
if
((k1ox<=0.0))
{
#if defined(_DERIVATE)
Denomi_Vgate_sourcep=0.0;
Denomi_Vdrainp_sourcep=0.0;
Denomi_Vbulk_sourcep=0.0;
#endif
Denomi=((0.25*moin)*Vtm);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(0.5*sqrtPhi);
}
else
{
#if defined(_DERIVATE)
Denomi_Vgate_sourcep=0.0;
Denomi_Vdrainp_sourcep=0.0;
Denomi_Vbulk_sourcep=0.0;
#endif
Denomi=(((moin*Vtm)*k1ox)*k1ox);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=0.0;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(k1ox*sqrtPhi);
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=((2.0*T_0_Vgate_sourcep)+Vgsteff_Vgate_sourcep);
T1_Vdrainp_sourcep=((2.0*T_0_Vdrainp_sourcep)+Vgsteff_Vdrainp_sourcep);
T1_Vbulk_sourcep=((2.0*T_0_Vbulk_sourcep)+Vgsteff_Vbulk_sourcep);
#endif
T1=((2.0*T_0)+Vgsteff);
{
double m00_logE(d00_logE0,(1.0+((T1*Vgsteff)/Denomi)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+((T1*Vgsteff)/Denomi)))
#endif
#if defined(_DERIVATE)
DeltaPhi_Vgate_sourcep=(Vtm*((((T1_Vgate_sourcep*Vgsteff)+(T1*Vgsteff_Vgate_sourcep))*Denomi-(T1*Vgsteff)*Denomi_Vgate_sourcep)/Denomi/Denomi)*d10_logE0);
DeltaPhi_Vdrainp_sourcep=(Vtm*((((T1_Vdrainp_sourcep*Vgsteff)+(T1*Vgsteff_Vdrainp_sourcep))*Denomi-(T1*Vgsteff)*Denomi_Vdrainp_sourcep)/Denomi/Denomi)*d10_logE0);
DeltaPhi_Vbulk_sourcep=(Vtm*((((T1_Vbulk_sourcep*Vgsteff)+(T1*Vgsteff_Vbulk_sourcep))*Denomi-(T1*Vgsteff)*Denomi_Vbulk_sourcep)/Denomi/Denomi)*d10_logE0);
#endif
DeltaPhi=(Vtm*d00_logE0);
}
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=(4.0*Vth_Vdrainp_sourcep);
T3_Vbulk_sourcep=(4.0*Vth_Vbulk_sourcep);
#endif
T3=(4.0*((Vth-vfbzb)-phi));
Tox2=(Tox+Tox);
if
((T3>=0.0))
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((Vgsteff_Vgate_sourcep+T3_Vgate_sourcep)/Tox2);
T_0_Vdrainp_sourcep=((Vgsteff_Vdrainp_sourcep+T3_Vdrainp_sourcep)/Tox2);
T_0_Vbulk_sourcep=((Vgsteff_Vbulk_sourcep+T3_Vbulk_sourcep)/Tox2);
#endif
T_0=((Vgsteff+T3)/Tox2);
}
else
{
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(Vgsteff_Vgate_sourcep/Tox2);
T_0_Vdrainp_sourcep=(Vgsteff_Vdrainp_sourcep/Tox2);
T_0_Vbulk_sourcep=(Vgsteff_Vbulk_sourcep/Tox2);
#endif
T_0=((Vgsteff+1.0e-20)/Tox2);
}
{
double m00_logE(d00_logE0,T_0)
double m00_exp(d00_exp1,(0.7*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,T_0)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
tmp_Vgate_sourcep=(0.7*T_0_Vgate_sourcep*d10_logE0)*d10_exp1;
tmp_Vdrainp_sourcep=(0.7*T_0_Vdrainp_sourcep*d10_logE0)*d10_exp1;
tmp_Vbulk_sourcep=(0.7*T_0_Vbulk_sourcep*d10_logE0)*d10_exp1;
#endif
tmp=d00_exp1;
}
#if defined(_DERIVATE)
T1_Vgate_sourcep=tmp_Vgate_sourcep;
T1_Vdrainp_sourcep=tmp_Vdrainp_sourcep;
T1_Vbulk_sourcep=tmp_Vbulk_sourcep;
#endif
T1=(1.0+tmp);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(((0.7*tmp_Vgate_sourcep)*(T_0*Tox2)-(0.7*tmp)*T_0_Vgate_sourcep*Tox2)/(T_0*Tox2)/(T_0*Tox2));
T2_Vdrainp_sourcep=(((0.7*tmp_Vdrainp_sourcep)*(T_0*Tox2)-(0.7*tmp)*T_0_Vdrainp_sourcep*Tox2)/(T_0*Tox2)/(T_0*Tox2));
T2_Vbulk_sourcep=(((0.7*tmp_Vbulk_sourcep)*(T_0*Tox2)-(0.7*tmp)*T_0_Vbulk_sourcep*Tox2)/(T_0*Tox2)/(T_0*Tox2));
#endif
T2=((0.7*tmp)/(T_0*Tox2));
#if defined(_DERIVATE)
Tcen_Vgate_sourcep=(-1.9e-9*T1_Vgate_sourcep/T1/T1);
Tcen_Vdrainp_sourcep=(-1.9e-9*T1_Vdrainp_sourcep/T1/T1);
Tcen_Vbulk_sourcep=(-1.9e-9*T1_Vbulk_sourcep/T1/T1);
#endif
Tcen=(1.9e-9/T1);
#if defined(_DERIVATE)
Ccen_Vgate_sourcep=(-1.03594e-10*Tcen_Vgate_sourcep/Tcen/Tcen);
Ccen_Vdrainp_sourcep=(-1.03594e-10*Tcen_Vdrainp_sourcep/Tcen/Tcen);
Ccen_Vbulk_sourcep=(-1.03594e-10*Tcen_Vbulk_sourcep/Tcen/Tcen);
#endif
Ccen=(1.03594e-10/Tcen);
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(-Cox*Ccen_Vgate_sourcep/(Cox+Ccen)/(Cox+Ccen));
T_0_Vdrainp_sourcep=(-Cox*Ccen_Vdrainp_sourcep/(Cox+Ccen)/(Cox+Ccen));
T_0_Vbulk_sourcep=(-Cox*Ccen_Vbulk_sourcep/(Cox+Ccen)/(Cox+Ccen));
#endif
T_0=(Cox/(Cox+Ccen));
#if defined(_DERIVATE)
Coxeff_Vgate_sourcep=((T_0_Vgate_sourcep*Ccen)+(T_0*Ccen_Vgate_sourcep));
Coxeff_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*Ccen)+(T_0*Ccen_Vdrainp_sourcep));
Coxeff_Vbulk_sourcep=((T_0_Vbulk_sourcep*Ccen)+(T_0*Ccen_Vbulk_sourcep));
#endif
Coxeff=(T_0*Ccen);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(((-Ccen_Vgate_sourcep)*Tcen-(-Ccen)*Tcen_Vgate_sourcep)/Tcen/Tcen);
T1_Vdrainp_sourcep=(((-Ccen_Vdrainp_sourcep)*Tcen-(-Ccen)*Tcen_Vdrainp_sourcep)/Tcen/Tcen);
T1_Vbulk_sourcep=(((-Ccen_Vbulk_sourcep)*Tcen-(-Ccen)*Tcen_Vbulk_sourcep)/Tcen/Tcen);
#endif
T1=((-Ccen)/Tcen);
#if defined(_DERIVATE)
CoxWLcen_Vgate_sourcep=((CoxWL*Coxeff_Vgate_sourcep)/Cox);
CoxWLcen_Vdrainp_sourcep=((CoxWL*Coxeff_Vdrainp_sourcep)/Cox);
CoxWLcen_Vbulk_sourcep=((CoxWL*Coxeff_Vbulk_sourcep)/Cox);
#endif
CoxWLcen=((CoxWL*Coxeff)/Cox);
#if defined(_DERIVATE)
AbulkCV_Vgate_sourcep=Abulk0_Vgate_sourcep*abulkCVfactor;
AbulkCV_Vdrainp_sourcep=Abulk0_Vdrainp_sourcep*abulkCVfactor;
AbulkCV_Vbulk_sourcep=Abulk0_Vbulk_sourcep*abulkCVfactor;
#endif
AbulkCV=(Abulk0*abulkCVfactor);
#if defined(_DERIVATE)
VdsatCV_Vdrainp_sourcep=(((Vgsteff_Vdrainp_sourcep-DeltaPhi_Vdrainp_sourcep)*AbulkCV-(Vgsteff-DeltaPhi)*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vgate_sourcep=(((Vgsteff_Vgate_sourcep-DeltaPhi_Vgate_sourcep)*AbulkCV-(Vgsteff-DeltaPhi)*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
VdsatCV_Vbulk_sourcep=(((Vgsteff_Vbulk_sourcep-DeltaPhi_Vbulk_sourcep)*AbulkCV-(Vgsteff-DeltaPhi)*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
VdsatCV=((Vgsteff-DeltaPhi)/AbulkCV);
#if defined(_DERIVATE)
V4_Vdrainp_sourcep=(VdsatCV_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
V4_Vgate_sourcep=VdsatCV_Vgate_sourcep;
V4_Vbulk_sourcep=VdsatCV_Vbulk_sourcep;
#endif
V4=((VdsatCV-Vds)-0.02);
{
double m00_sqrt(d00_sqrt0,((V4*V4)+((4.0*0.02)*VdsatCV)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((V4*V4)+((4.0*0.02)*VdsatCV)))
#endif
#if defined(_DERIVATE)
T_0_Vgate_sourcep=(((V4_Vgate_sourcep*V4)+(V4*V4_Vgate_sourcep))+((4.0*0.02)*VdsatCV_Vgate_sourcep))*d10_sqrt0;
T_0_Vdrainp_sourcep=(((V4_Vdrainp_sourcep*V4)+(V4*V4_Vdrainp_sourcep))+((4.0*0.02)*VdsatCV_Vdrainp_sourcep))*d10_sqrt0;
T_0_Vbulk_sourcep=(((V4_Vbulk_sourcep*V4)+(V4*V4_Vbulk_sourcep))+((4.0*0.02)*VdsatCV_Vbulk_sourcep))*d10_sqrt0;
#endif
T_0=d00_sqrt0;
}
#if defined(_DERIVATE)
VdseffCV_Vdrainp_sourcep=(VdsatCV_Vdrainp_sourcep-(0.5*(V4_Vdrainp_sourcep+T_0_Vdrainp_sourcep)));
VdseffCV_Vgate_sourcep=(VdsatCV_Vgate_sourcep-(0.5*(V4_Vgate_sourcep+T_0_Vgate_sourcep)));
VdseffCV_Vbulk_sourcep=(VdsatCV_Vbulk_sourcep-(0.5*(V4_Vbulk_sourcep+T_0_Vbulk_sourcep)));
#endif
VdseffCV=(VdsatCV-(0.5*(V4+T_0)));
#if defined(_DERIVATE)
T1_Vgate_sourcep=(0.5*((V4_Vgate_sourcep*T_0-V4*T_0_Vgate_sourcep)/T_0/T_0));
T1_Vdrainp_sourcep=(0.5*((V4_Vdrainp_sourcep*T_0-V4*T_0_Vdrainp_sourcep)/T_0/T_0));
T1_Vbulk_sourcep=(0.5*((V4_Vbulk_sourcep*T_0-V4*T_0_Vbulk_sourcep)/T_0/T_0));
#endif
T1=(0.5*(1.0+(V4/T_0)));
#if defined(_DERIVATE)
T2_Vgate_sourcep=(-0.02*T_0_Vgate_sourcep/T_0/T_0);
T2_Vdrainp_sourcep=(-0.02*T_0_Vdrainp_sourcep/T_0/T_0);
T2_Vbulk_sourcep=(-0.02*T_0_Vbulk_sourcep/T_0/T_0);
#endif
T2=(0.02/T_0);
#if defined(_DERIVATE)
T3_Vgate_sourcep=((((-T1_Vgate_sourcep)-T2_Vgate_sourcep)*AbulkCV-((1.0-T1)-T2)*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
T3_Vdrainp_sourcep=((((-T1_Vdrainp_sourcep)-T2_Vdrainp_sourcep)*AbulkCV-((1.0-T1)-T2)*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
T3_Vbulk_sourcep=((((-T1_Vbulk_sourcep)-T2_Vbulk_sourcep)*AbulkCV-((1.0-T1)-T2)*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
T3=(((1.0-T1)-T2)/AbulkCV);
if
(((Vds==0.0)&&(VERSION==3.24)))
{
#if defined(_DERIVATE)
VdseffCV_Vdrainp_sourcep=0.0;
VdseffCV_Vgate_sourcep=0.0;
VdseffCV_Vbulk_sourcep=0.0;
#endif
VdseffCV=0.0;
}
#if defined(_DERIVATE)
T_0_Vgate_sourcep=((AbulkCV_Vgate_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vgate_sourcep));
T_0_Vdrainp_sourcep=((AbulkCV_Vdrainp_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vdrainp_sourcep));
T_0_Vbulk_sourcep=((AbulkCV_Vbulk_sourcep*VdseffCV)+(AbulkCV*VdseffCV_Vbulk_sourcep));
#endif
T_0=(AbulkCV*VdseffCV);
#if defined(_DERIVATE)
T1_Vgate_sourcep=(Vgsteff_Vgate_sourcep-DeltaPhi_Vgate_sourcep);
T1_Vdrainp_sourcep=(Vgsteff_Vdrainp_sourcep-DeltaPhi_Vdrainp_sourcep);
T1_Vbulk_sourcep=(Vgsteff_Vbulk_sourcep-DeltaPhi_Vbulk_sourcep);
#endif
T1=(Vgsteff-DeltaPhi);
#if defined(_DERIVATE)
T2_Vgate_sourcep=(12.0*(T1_Vgate_sourcep-(0.5*T_0_Vgate_sourcep)));
T2_Vdrainp_sourcep=(12.0*(T1_Vdrainp_sourcep-(0.5*T_0_Vdrainp_sourcep)));
T2_Vbulk_sourcep=(12.0*(T1_Vbulk_sourcep-(0.5*T_0_Vbulk_sourcep)));
#endif
T2=(12.0*((T1-(0.5*T_0))+1.0e-20));
#if defined(_DERIVATE)
T3_Vgate_sourcep=((T_0_Vgate_sourcep*T2-T_0*T2_Vgate_sourcep)/T2/T2);
T3_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T2-T_0*T2_Vdrainp_sourcep)/T2/T2);
T3_Vbulk_sourcep=((T_0_Vbulk_sourcep*T2-T_0*T2_Vbulk_sourcep)/T2/T2);
#endif
T3=(T_0/T2);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T4_Vgate_sourcep=(-(((12.0*T3_Vgate_sourcep)*T3)+((12.0*T3)*T3_Vgate_sourcep)));
T4_Vdrainp_sourcep=(-(((12.0*T3_Vdrainp_sourcep)*T3)+((12.0*T3)*T3_Vdrainp_sourcep)));
T4_Vbulk_sourcep=(-(((12.0*T3_Vbulk_sourcep)*T3)+((12.0*T3)*T3_Vbulk_sourcep)));
#endif
T4=(1.0-((12.0*T3)*T3));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T5_Vgate_sourcep=((AbulkCV_Vgate_sourcep*((((6.0*T_0)*((4.0*T1)-T_0))/(T2*T2))-0.5))+(AbulkCV*(((((6.0*T_0_Vgate_sourcep)*((4.0*T1)-T_0))+((6.0*T_0)*((4.0*T1_Vgate_sourcep)-T_0_Vgate_sourcep)))*(T2*T2)-((6.0*T_0)*((4.0*T1)-T_0))*((T2_Vgate_sourcep*T2)+(T2*T2_Vgate_sourcep)))/(T2*T2)/(T2*T2))));
T5_Vdrainp_sourcep=((AbulkCV_Vdrainp_sourcep*((((6.0*T_0)*((4.0*T1)-T_0))/(T2*T2))-0.5))+(AbulkCV*(((((6.0*T_0_Vdrainp_sourcep)*((4.0*T1)-T_0))+((6.0*T_0)*((4.0*T1_Vdrainp_sourcep)-T_0_Vdrainp_sourcep)))*(T2*T2)-((6.0*T_0)*((4.0*T1)-T_0))*((T2_Vdrainp_sourcep*T2)+(T2*T2_Vdrainp_sourcep)))/(T2*T2)/(T2*T2))));
T5_Vbulk_sourcep=((AbulkCV_Vbulk_sourcep*((((6.0*T_0)*((4.0*T1)-T_0))/(T2*T2))-0.5))+(AbulkCV*(((((6.0*T_0_Vbulk_sourcep)*((4.0*T1)-T_0))+((6.0*T_0)*((4.0*T1_Vbulk_sourcep)-T_0_Vbulk_sourcep)))*(T2*T2)-((6.0*T_0)*((4.0*T1)-T_0))*((T2_Vbulk_sourcep*T2)+(T2*T2_Vbulk_sourcep)))/(T2*T2)/(T2*T2))));
#endif
T5=(AbulkCV*((((6.0*T_0)*((4.0*T1)-T_0))/(T2*T2))-0.5));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T6_Vgate_sourcep=((((T5_Vgate_sourcep*VdseffCV)+(T5*VdseffCV_Vgate_sourcep))*AbulkCV-(T5*VdseffCV)*AbulkCV_Vgate_sourcep)/AbulkCV/AbulkCV);
T6_Vdrainp_sourcep=((((T5_Vdrainp_sourcep*VdseffCV)+(T5*VdseffCV_Vdrainp_sourcep))*AbulkCV-(T5*VdseffCV)*AbulkCV_Vdrainp_sourcep)/AbulkCV/AbulkCV);
T6_Vbulk_sourcep=((((T5_Vbulk_sourcep*VdseffCV)+(T5*VdseffCV_Vbulk_sourcep))*AbulkCV-(T5*VdseffCV)*AbulkCV_Vbulk_sourcep)/AbulkCV/AbulkCV);
#endif
T6=((T5*VdseffCV)/AbulkCV);
#endif
#if defined(_DERIVATE)
qinoi_Vdrainp_sourcep=((CoxWLcen_Vdrainp_sourcep*(T1-(T_0*(0.5-T3))))+(CoxWLcen*(T1_Vdrainp_sourcep-((T_0_Vdrainp_sourcep*(0.5-T3))+(T_0*(-T3_Vdrainp_sourcep))))));
qinoi_Vgate_sourcep=((CoxWLcen_Vgate_sourcep*(T1-(T_0*(0.5-T3))))+(CoxWLcen*(T1_Vgate_sourcep-((T_0_Vgate_sourcep*(0.5-T3))+(T_0*(-T3_Vgate_sourcep))))));
qinoi_Vbulk_sourcep=((CoxWLcen_Vbulk_sourcep*(T1-(T_0*(0.5-T3))))+(CoxWLcen*(T1_Vbulk_sourcep-((T_0_Vbulk_sourcep*(0.5-T3))+(T_0*(-T3_Vbulk_sourcep))))));
#endif
qinoi=(CoxWLcen*(T1-(T_0*(0.5-T3))));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=qinoi_Vdrainp_sourcep;
qgate_Vgate_sourcep=qinoi_Vgate_sourcep;
qgate_Vbulk_sourcep=qinoi_Vbulk_sourcep;
#endif
qgate=qinoi;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T7_Vgate_sourcep=(-AbulkCV_Vgate_sourcep);
T7_Vdrainp_sourcep=(-AbulkCV_Vdrainp_sourcep);
T7_Vbulk_sourcep=(-AbulkCV_Vbulk_sourcep);
#endif
T7=(1.0-AbulkCV);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=((((CoxWLcen_Vdrainp_sourcep*T7)+(CoxWLcen*T7_Vdrainp_sourcep))*((0.5*VdseffCV)-((T_0*VdseffCV)/T2)))+((CoxWLcen*T7)*((0.5*VdseffCV_Vdrainp_sourcep)-((((T_0_Vdrainp_sourcep*VdseffCV)+(T_0*VdseffCV_Vdrainp_sourcep))*T2-(T_0*VdseffCV)*T2_Vdrainp_sourcep)/T2/T2))));
qbulk_Vgate_sourcep=((((CoxWLcen_Vgate_sourcep*T7)+(CoxWLcen*T7_Vgate_sourcep))*((0.5*VdseffCV)-((T_0*VdseffCV)/T2)))+((CoxWLcen*T7)*((0.5*VdseffCV_Vgate_sourcep)-((((T_0_Vgate_sourcep*VdseffCV)+(T_0*VdseffCV_Vgate_sourcep))*T2-(T_0*VdseffCV)*T2_Vgate_sourcep)/T2/T2))));
qbulk_Vbulk_sourcep=((((CoxWLcen_Vbulk_sourcep*T7)+(CoxWLcen*T7_Vbulk_sourcep))*((0.5*VdseffCV)-((T_0*VdseffCV)/T2)))+((CoxWLcen*T7)*((0.5*VdseffCV_Vbulk_sourcep)-((((T_0_Vbulk_sourcep*VdseffCV)+(T_0*VdseffCV_Vbulk_sourcep))*T2-(T_0*VdseffCV)*T2_Vbulk_sourcep)/T2/T2))));
#endif
qbulk=((CoxWLcen*T7)*((0.5*VdseffCV)-((T_0*VdseffCV)/T2)));
#endif
#ifdef _DYNAMIC
if
((XPART>0.5))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(((-CoxWLcen_Vgate_sourcep)*(((T1/2.0)+(T_0/4.0))-(((0.5*T_0)*T_0)/T2)))+((-CoxWLcen)*(((T1_Vgate_sourcep/2.0)+(T_0_Vgate_sourcep/4.0))-(((((0.5*T_0_Vgate_sourcep)*T_0)+((0.5*T_0)*T_0_Vgate_sourcep))*T2-((0.5*T_0)*T_0)*T2_Vgate_sourcep)/T2/T2))));
qsrc_Vdrainp_sourcep=(((-CoxWLcen_Vdrainp_sourcep)*(((T1/2.0)+(T_0/4.0))-(((0.5*T_0)*T_0)/T2)))+((-CoxWLcen)*(((T1_Vdrainp_sourcep/2.0)+(T_0_Vdrainp_sourcep/4.0))-(((((0.5*T_0_Vdrainp_sourcep)*T_0)+((0.5*T_0)*T_0_Vdrainp_sourcep))*T2-((0.5*T_0)*T_0)*T2_Vdrainp_sourcep)/T2/T2))));
qsrc_Vbulk_sourcep=(((-CoxWLcen_Vbulk_sourcep)*(((T1/2.0)+(T_0/4.0))-(((0.5*T_0)*T_0)/T2)))+((-CoxWLcen)*(((T1_Vbulk_sourcep/2.0)+(T_0_Vbulk_sourcep/4.0))-(((((0.5*T_0_Vbulk_sourcep)*T_0)+((0.5*T_0)*T_0_Vbulk_sourcep))*T2-((0.5*T_0)*T_0)*T2_Vbulk_sourcep)/T2/T2))));
#endif
qsrc=((-CoxWLcen)*(((T1/2.0)+(T_0/4.0))-(((0.5*T_0)*T_0)/T2)));
#endif
}
else
{
#ifdef _DYNAMIC
if
((XPART<0.5))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T2_Vgate_sourcep=(T2_Vgate_sourcep/12.0);
T2_Vdrainp_sourcep=(T2_Vdrainp_sourcep/12.0);
T2_Vbulk_sourcep=(T2_Vbulk_sourcep/12.0);
#endif
T2=(T2/12.0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T3_Vgate_sourcep=(((0.5*CoxWLcen_Vgate_sourcep)*(T2*T2)-(0.5*CoxWLcen)*((T2_Vgate_sourcep*T2)+(T2*T2_Vgate_sourcep)))/(T2*T2)/(T2*T2));
T3_Vdrainp_sourcep=(((0.5*CoxWLcen_Vdrainp_sourcep)*(T2*T2)-(0.5*CoxWLcen)*((T2_Vdrainp_sourcep*T2)+(T2*T2_Vdrainp_sourcep)))/(T2*T2)/(T2*T2));
T3_Vbulk_sourcep=(((0.5*CoxWLcen_Vbulk_sourcep)*(T2*T2)-(0.5*CoxWLcen)*((T2_Vbulk_sourcep*T2)+(T2*T2_Vbulk_sourcep)))/(T2*T2)/(T2*T2));
#endif
T3=((0.5*CoxWLcen)/(T2*T2));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T4_Vgate_sourcep=(((T1_Vgate_sourcep*((((2.0*T_0)*T_0)/3.0)+(T1*(T1-((4.0*T_0)/3.0)))))+(T1*(((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))/3.0)+((T1_Vgate_sourcep*(T1-((4.0*T_0)/3.0)))+(T1*(T1_Vgate_sourcep-((4.0*T_0_Vgate_sourcep)/3.0)))))))-((((((2.0*T_0_Vgate_sourcep)*T_0)+((2.0*T_0)*T_0_Vgate_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vgate_sourcep))/15.0));
T4_Vdrainp_sourcep=(((T1_Vdrainp_sourcep*((((2.0*T_0)*T_0)/3.0)+(T1*(T1-((4.0*T_0)/3.0)))))+(T1*(((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))/3.0)+((T1_Vdrainp_sourcep*(T1-((4.0*T_0)/3.0)))+(T1*(T1_Vdrainp_sourcep-((4.0*T_0_Vdrainp_sourcep)/3.0)))))))-((((((2.0*T_0_Vdrainp_sourcep)*T_0)+((2.0*T_0)*T_0_Vdrainp_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vdrainp_sourcep))/15.0));
T4_Vbulk_sourcep=(((T1_Vbulk_sourcep*((((2.0*T_0)*T_0)/3.0)+(T1*(T1-((4.0*T_0)/3.0)))))+(T1*(((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))/3.0)+((T1_Vbulk_sourcep*(T1-((4.0*T_0)/3.0)))+(T1*(T1_Vbulk_sourcep-((4.0*T_0_Vbulk_sourcep)/3.0)))))))-((((((2.0*T_0_Vbulk_sourcep)*T_0)+((2.0*T_0)*T_0_Vbulk_sourcep))*T_0)+(((2.0*T_0)*T_0)*T_0_Vbulk_sourcep))/15.0));
#endif
T4=((T1*((((2.0*T_0)*T_0)/3.0)+(T1*(T1-((4.0*T_0)/3.0)))))-((((2.0*T_0)*T_0)*T_0)/15.0));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(((-T3_Vgate_sourcep)*T4)+((-T3)*T4_Vgate_sourcep));
qsrc_Vdrainp_sourcep=(((-T3_Vdrainp_sourcep)*T4)+((-T3)*T4_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=(((-T3_Vbulk_sourcep)*T4)+((-T3)*T4_Vbulk_sourcep));
#endif
qsrc=((-T3)*T4);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=((-0.5)*qgate_Vgate_sourcep);
qsrc_Vdrainp_sourcep=((-0.5)*qgate_Vdrainp_sourcep);
qsrc_Vbulk_sourcep=((-0.5)*qgate_Vbulk_sourcep);
#endif
qsrc=((-0.5)*qgate);
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(((qgate_Vdrainp_sourcep+Qac0_Vdrainp_sourcep)+Qsub0_Vdrainp_sourcep)-qbulk_Vdrainp_sourcep);
qgate_Vgate_sourcep=(((qgate_Vgate_sourcep+Qac0_Vgate_sourcep)+Qsub0_Vgate_sourcep)-qbulk_Vgate_sourcep);
qgate_Vbulk_sourcep=(((qgate_Vbulk_sourcep+Qac0_Vbulk_sourcep)+Qsub0_Vbulk_sourcep)-qbulk_Vbulk_sourcep);
#endif
qgate=(((qgate+Qac0)+Qsub0)-qbulk);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep-(Qac0_Vdrainp_sourcep+Qsub0_Vdrainp_sourcep));
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep-(Qac0_Vgate_sourcep+Qsub0_Vgate_sourcep));
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep-(Qac0_Vbulk_sourcep+Qsub0_Vbulk_sourcep));
#endif
qbulk=(qbulk-(Qac0+Qsub0));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qsrc_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qsrc_Vbulk_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
#endif
#if defined(_DERIVATE)
qinv_Vdrainp_sourcep=(-qinoi_Vdrainp_sourcep);
qinv_Vgate_sourcep=(-qinoi_Vgate_sourcep);
qinv_Vbulk_sourcep=(-qinoi_Vbulk_sourcep);
#endif
qinv=(-qinoi);
}
}
}
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_Vbulk_sourcep=0.0;
qgdo_Vgate_sourcep=0.0;
qgdo_Vdrainp_sourcep=0.0;
#endif
qgdo=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=0.0;
qgso_Vbulk_sourcep=0.0;
qgso_Vgate_sourcep=0.0;
#endif
qgso=0.0;
#endif
#ifdef _DYNAMIC
if
((CAPMOD==0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_Vbulk_sourcep=0.0;
qgdo_Vgate_sourcep=(cgdo_param*vgd_Vgate_sourcep);
qgdo_Vdrainp_sourcep=(cgdo_param*vgd_Vdrainp_sourcep);
#endif
qgdo=(cgdo_param*vgd);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=0.0;
qgso_Vbulk_sourcep=0.0;
qgso_Vgate_sourcep=(cgso_param*vgs_Vgate_sourcep);
#endif
qgso=(cgso_param*vgs);
#endif
}
else
{
#ifdef _DYNAMIC
if
((CAPMOD==1.0))
{
#ifdef _DYNAMIC
if
((vgd<0.0))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*vgd)/ckappa)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*vgd)/ckappa)))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=(-((4.0*vgd_Vgate_sourcep)/ckappa))*d10_sqrt0;
T1_Vdrainp_sourcep=(-((4.0*vgd_Vdrainp_sourcep)/ckappa))*d10_sqrt0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_Vbulk_sourcep=(-((((weffCV*0.5)*cgdl)*ckappa)*T1_Vbulk_sourcep));
qgdo_Vgate_sourcep=((cgdo_param*vgd_Vgate_sourcep)-((((weffCV*0.5)*cgdl)*ckappa)*T1_Vgate_sourcep));
qgdo_Vdrainp_sourcep=((cgdo_param*vgd_Vdrainp_sourcep)-((((weffCV*0.5)*cgdl)*ckappa)*T1_Vdrainp_sourcep));
#endif
qgdo=((cgdo_param*vgd)-((((weffCV*0.5)*cgdl)*ckappa)*(T1-1.0)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_Vbulk_sourcep=0.0;
qgdo_Vgate_sourcep=(((weffCV*cgdl)+cgdo_param)*vgd_Vgate_sourcep);
qgdo_Vdrainp_sourcep=(((weffCV*cgdl)+cgdo_param)*vgd_Vdrainp_sourcep);
#endif
qgdo=(((weffCV*cgdl)+cgdo_param)*vgd);
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((vgs<0.0))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*vgs)/ckappa)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*vgs)/ckappa)))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=(-((4.0*vgs_Vgate_sourcep)/ckappa))*d10_sqrt0;
T1_Vdrainp_sourcep=0.0;
T1_Vbulk_sourcep=0.0;
#endif
T1=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=(-((((weffCV*0.5)*cgsl)*ckappa)*T1_Vdrainp_sourcep));
qgso_Vbulk_sourcep=(-((((weffCV*0.5)*cgsl)*ckappa)*T1_Vbulk_sourcep));
qgso_Vgate_sourcep=((cgso_param*vgs_Vgate_sourcep)-((((weffCV*0.5)*cgsl)*ckappa)*T1_Vgate_sourcep));
#endif
qgso=((cgso_param*vgs)-((((weffCV*0.5)*cgsl)*ckappa)*(T1-1.0)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=0.0;
qgso_Vbulk_sourcep=0.0;
qgso_Vgate_sourcep=(((weffCV*cgsl)+cgso_param)*vgs_Vgate_sourcep);
#endif
qgso=(((weffCV*cgsl)+cgso_param)*vgs);
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T_0_Vgate_sourcep=vgd_Vgate_sourcep;
T_0_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(vgd+0.02);
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)+(4.0*0.02)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)+(4.0*0.02)))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))*d10_sqrt0;
T1_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))*d10_sqrt0;
T1_Vbulk_sourcep=((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T2_Vgate_sourcep=(0.5*(T_0_Vgate_sourcep-T1_Vgate_sourcep));
T2_Vdrainp_sourcep=(0.5*(T_0_Vdrainp_sourcep-T1_Vdrainp_sourcep));
T2_Vbulk_sourcep=(0.5*(T_0_Vbulk_sourcep-T1_Vbulk_sourcep));
#endif
T2=(0.5*(T_0-T1));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=(weffCV*cgdl);
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*T2)/ckappa)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*T2)/ckappa)))
#endif
#if defined(_DERIVATE)
T4_Vgate_sourcep=(-((4.0*T2_Vgate_sourcep)/ckappa))*d10_sqrt0;
T4_Vdrainp_sourcep=(-((4.0*T2_Vdrainp_sourcep)/ckappa))*d10_sqrt0;
T4_Vbulk_sourcep=(-((4.0*T2_Vbulk_sourcep)/ckappa))*d10_sqrt0;
#endif
T4=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_Vbulk_sourcep=(T3_Vbulk_sourcep*vgd-((T3_Vbulk_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vbulk_sourcep+((0.5*ckappa)*T4_Vbulk_sourcep)))));
qgdo_Vgate_sourcep=(((T3_Vgate_sourcep*vgd)+((cgdo_param+T3)*vgd_Vgate_sourcep))-((T3_Vgate_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vgate_sourcep+((0.5*ckappa)*T4_Vgate_sourcep)))));
qgdo_Vdrainp_sourcep=(((T3_Vdrainp_sourcep*vgd)+((cgdo_param+T3)*vgd_Vdrainp_sourcep))-((T3_Vdrainp_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vdrainp_sourcep+((0.5*ckappa)*T4_Vdrainp_sourcep)))));
#endif
qgdo=(((cgdo_param+T3)*vgd)-(T3*(T2+((0.5*ckappa)*(T4-1.0)))));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T_0_Vgate_sourcep=vgs_Vgate_sourcep;
T_0_Vdrainp_sourcep=0.0;
T_0_Vbulk_sourcep=0.0;
#endif
T_0=(vgs+0.02);
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((T_0*T_0)+(4.0*0.02)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_0*T_0)+(4.0*0.02)))
#endif
#if defined(_DERIVATE)
T1_Vgate_sourcep=((T_0_Vgate_sourcep*T_0)+(T_0*T_0_Vgate_sourcep))*d10_sqrt0;
T1_Vdrainp_sourcep=((T_0_Vdrainp_sourcep*T_0)+(T_0*T_0_Vdrainp_sourcep))*d10_sqrt0;
T1_Vbulk_sourcep=((T_0_Vbulk_sourcep*T_0)+(T_0*T_0_Vbulk_sourcep))*d10_sqrt0;
#endif
T1=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T2_Vgate_sourcep=(0.5*(T_0_Vgate_sourcep-T1_Vgate_sourcep));
T2_Vdrainp_sourcep=(0.5*(T_0_Vdrainp_sourcep-T1_Vdrainp_sourcep));
T2_Vbulk_sourcep=(0.5*(T_0_Vbulk_sourcep-T1_Vbulk_sourcep));
#endif
T2=(0.5*(T_0-T1));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T3_Vgate_sourcep=0.0;
T3_Vdrainp_sourcep=0.0;
T3_Vbulk_sourcep=0.0;
#endif
T3=(weffCV*cgsl);
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*T2)/ckappa)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*T2)/ckappa)))
#endif
#if defined(_DERIVATE)
T4_Vgate_sourcep=(-((4.0*T2_Vgate_sourcep)/ckappa))*d10_sqrt0;
T4_Vdrainp_sourcep=(-((4.0*T2_Vdrainp_sourcep)/ckappa))*d10_sqrt0;
T4_Vbulk_sourcep=(-((4.0*T2_Vbulk_sourcep)/ckappa))*d10_sqrt0;
#endif
T4=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=(T3_Vdrainp_sourcep*vgs-((T3_Vdrainp_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vdrainp_sourcep+((0.5*ckappa)*T4_Vdrainp_sourcep)))));
qgso_Vbulk_sourcep=(T3_Vbulk_sourcep*vgs-((T3_Vbulk_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vbulk_sourcep+((0.5*ckappa)*T4_Vbulk_sourcep)))));
qgso_Vgate_sourcep=(((T3_Vgate_sourcep*vgs)+((cgso_param+T3)*vgs_Vgate_sourcep))-((T3_Vgate_sourcep*(T2+((0.5*ckappa)*(T4-1.0))))+(T3*(T2_Vgate_sourcep+((0.5*ckappa)*T4_Vgate_sourcep)))));
#endif
qgso=(((cgso_param+T3)*vgs)-(T3*(T2+((0.5*ckappa)*(T4-1.0)))));
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((mode>0))
{
#ifdef _DYNAMIC
if
((NQSMOD==0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgd_Vbulk_sourcep=qgdo_Vbulk_sourcep;
qgd_Vgate_sourcep=qgdo_Vgate_sourcep;
qgd_Vdrainp_sourcep=qgdo_Vdrainp_sourcep;
#endif
qgd=qgdo;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgs_Vdrainp_sourcep=qgso_Vdrainp_sourcep;
qgs_Vbulk_sourcep=qgso_Vbulk_sourcep;
qgs_Vgate_sourcep=qgso_Vgate_sourcep;
#endif
qgs=qgso;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgb_Vgate_sourcep=(cgbo_param*vgb_Vgate_sourcep);
qgb_Vbulk_sourcep=(cgbo_param*vgb_Vbulk_sourcep);
#endif
qgb=(cgbo_param*vgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+qgd_Vdrainp_sourcep)+qgs_Vdrainp_sourcep);
qgate_Vgate_sourcep=(((qgate_Vgate_sourcep+qgd_Vgate_sourcep)+qgs_Vgate_sourcep)+qgb_Vgate_sourcep);
qgate_Vbulk_sourcep=(((qgate_Vbulk_sourcep+qgd_Vbulk_sourcep)+qgs_Vbulk_sourcep)+qgb_Vbulk_sourcep);
#endif
qgate=(((qgate+qgd)+qgs)+qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep-qgb_Vgate_sourcep);
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep-qgb_Vbulk_sourcep);
#endif
qbulk=(qbulk-qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(qdrn_Vgate_sourcep-qgd_Vgate_sourcep);
qdrn_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qgd_Vdrainp_sourcep);
qdrn_Vbulk_sourcep=(qdrn_Vbulk_sourcep-qgd_Vbulk_sourcep);
#endif
qdrn=(qdrn-qgd);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qdrn_Vgate_sourcep));
qsrc_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qdrn_Vbulk_sourcep));
#endif
qsrc=(-((qgate+qbulk)+qdrn));
#endif
}
else
{
#if defined(_DYNAMIC)
CoxWL=((cox*weffCV)*leffCV);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgd_Vbulk_sourcep=qgdo_Vbulk_sourcep;
qgd_Vgate_sourcep=qgdo_Vgate_sourcep;
qgd_Vdrainp_sourcep=qgdo_Vdrainp_sourcep;
#endif
qgd=qgdo;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgs_Vdrainp_sourcep=qgso_Vdrainp_sourcep;
qgs_Vbulk_sourcep=qgso_Vbulk_sourcep;
qgs_Vgate_sourcep=qgso_Vgate_sourcep;
#endif
qgs=qgso;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgb_Vgate_sourcep=(cgbo_param*vgb_Vgate_sourcep);
qgb_Vbulk_sourcep=(cgbo_param*vgb_Vbulk_sourcep);
#endif
qgb=(cgbo_param*vgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(qgd_Vdrainp_sourcep+qgs_Vdrainp_sourcep);
qgate_Vgate_sourcep=((qgd_Vgate_sourcep+qgs_Vgate_sourcep)+qgb_Vgate_sourcep);
qgate_Vbulk_sourcep=((qgd_Vbulk_sourcep+qgs_Vbulk_sourcep)+qgb_Vbulk_sourcep);
#endif
qgate=((qgd+qgs)+qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=0.0;
qbulk_Vgate_sourcep=(-qgb_Vgate_sourcep);
qbulk_Vbulk_sourcep=(-qgb_Vbulk_sourcep);
#endif
qbulk=(-qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-qgd_Vgate_sourcep);
qdrn_Vdrainp_sourcep=(-qgd_Vdrainp_sourcep);
qdrn_Vbulk_sourcep=(-qgd_Vbulk_sourcep);
#endif
qdrn=(-qgd);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qdrn_Vgate_sourcep));
qsrc_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qsrc_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qdrn_Vbulk_sourcep));
#endif
qsrc=(-((qgate+qbulk)+qdrn));
#endif
}
#endif /* if (...) */
}
else
{
#ifdef _DYNAMIC
if
((NQSMOD==0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgd_Vbulk_sourcep=qgdo_Vbulk_sourcep;
qgd_Vgate_sourcep=qgdo_Vgate_sourcep;
qgd_Vdrainp_sourcep=qgdo_Vdrainp_sourcep;
#endif
qgd=qgdo;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgs_Vdrainp_sourcep=qgso_Vdrainp_sourcep;
qgs_Vbulk_sourcep=qgso_Vbulk_sourcep;
qgs_Vgate_sourcep=qgso_Vgate_sourcep;
#endif
qgs=qgso;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgb_Vgate_sourcep=(cgbo_param*vgb_Vgate_sourcep);
qgb_Vbulk_sourcep=(cgbo_param*vgb_Vbulk_sourcep);
#endif
qgb=(cgbo_param*vgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+qgd_Vdrainp_sourcep)+qgs_Vdrainp_sourcep);
qgate_Vgate_sourcep=(((qgate_Vgate_sourcep+qgd_Vgate_sourcep)+qgs_Vgate_sourcep)+qgb_Vgate_sourcep);
qgate_Vbulk_sourcep=(((qgate_Vbulk_sourcep+qgd_Vbulk_sourcep)+qgs_Vbulk_sourcep)+qgb_Vbulk_sourcep);
#endif
qgate=(((qgate+qgd)+qgs)+qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vgate_sourcep=(qbulk_Vgate_sourcep-qgb_Vgate_sourcep);
qbulk_Vbulk_sourcep=(qbulk_Vbulk_sourcep-qgb_Vbulk_sourcep);
#endif
qbulk=(qbulk-qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(qdrn_Vgate_sourcep-qgs_Vgate_sourcep);
qsrc_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qgs_Vdrainp_sourcep);
qsrc_Vbulk_sourcep=(qdrn_Vbulk_sourcep-qgs_Vbulk_sourcep);
#endif
qsrc=(qdrn-qgs);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qsrc_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qsrc_Vbulk_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
#endif
}
else
{
#if defined(_DYNAMIC)
CoxWL=((cox*weffCV)*leffCV);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgd_Vbulk_sourcep=qgdo_Vbulk_sourcep;
qgd_Vgate_sourcep=qgdo_Vgate_sourcep;
qgd_Vdrainp_sourcep=qgdo_Vdrainp_sourcep;
#endif
qgd=qgdo;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgs_Vdrainp_sourcep=qgso_Vdrainp_sourcep;
qgs_Vbulk_sourcep=qgso_Vbulk_sourcep;
qgs_Vgate_sourcep=qgso_Vgate_sourcep;
#endif
qgs=qgso;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgb_Vgate_sourcep=(cgbo_param*vgb_Vgate_sourcep);
qgb_Vbulk_sourcep=(cgbo_param*vgb_Vbulk_sourcep);
#endif
qgb=(cgbo_param*vgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgate_Vdrainp_sourcep=(qgd_Vdrainp_sourcep+qgs_Vdrainp_sourcep);
qgate_Vgate_sourcep=((qgd_Vgate_sourcep+qgs_Vgate_sourcep)+qgb_Vgate_sourcep);
qgate_Vbulk_sourcep=((qgd_Vbulk_sourcep+qgs_Vbulk_sourcep)+qgb_Vbulk_sourcep);
#endif
qgate=((qgd+qgs)+qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbulk_Vdrainp_sourcep=0.0;
qbulk_Vgate_sourcep=(-qgb_Vgate_sourcep);
qbulk_Vbulk_sourcep=(-qgb_Vbulk_sourcep);
#endif
qbulk=(-qgb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qsrc_Vgate_sourcep=(-qgs_Vgate_sourcep);
qsrc_Vdrainp_sourcep=(-qgs_Vdrainp_sourcep);
qsrc_Vbulk_sourcep=(-qgs_Vbulk_sourcep);
#endif
qsrc=(-qgs);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qdrn_Vgate_sourcep=(-((qgate_Vgate_sourcep+qbulk_Vgate_sourcep)+qsrc_Vgate_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vbulk_sourcep=(-((qgate_Vbulk_sourcep+qbulk_Vbulk_sourcep)+qsrc_Vbulk_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdrn_Vgate_sourcep=(qdrn_Vgate_sourcep-qbd_Vgate_sourcep);
Qdrn_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qbd_Vdrainp_sourcep);
Qdrn_Vbulk_sourcep=(qdrn_Vbulk_sourcep-qbd_Vbulk_sourcep);
#endif
Qdrn=(qdrn-qbd);
#endif
#if defined(_DYNAMIC)
Qsrc=(qsrc-qbs);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbulk_Vdrainp_sourcep=((qbulk_Vdrainp_sourcep+qbd_Vdrainp_sourcep)+qbs_Vdrainp_sourcep);
Qbulk_Vgate_sourcep=((qbulk_Vgate_sourcep+qbd_Vgate_sourcep)+qbs_Vgate_sourcep);
Qbulk_Vbulk_sourcep=((qbulk_Vbulk_sourcep+qbd_Vbulk_sourcep)+qbs_Vbulk_sourcep);
#endif
Qbulk=((qbulk+qbd)+qbs);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgate_Vdrainp_sourcep=qgate_Vdrainp_sourcep;
Qgate_Vgate_sourcep=qgate_Vgate_sourcep;
Qgate_Vbulk_sourcep=qgate_Vbulk_sourcep;
#endif
Qgate=qgate;
#endif
if
((mode>0))
{
_load_static_residual2(drainp,sourcep,(TYPE*cdrain));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,sourcep,bulk,sourcep,(TYPE*cdrain_Vbulk_sourcep));
_load_static_jacobian4(drainp,sourcep,drainp,sourcep,(TYPE*cdrain_Vdrainp_sourcep));
_load_static_jacobian4(drainp,sourcep,gate,sourcep,(TYPE*cdrain_Vgate_sourcep));
#endif
_load_static_residual2(bulk,drainp,(TYPE*(cbd-csub)));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,drainp,gate,sourcep,(TYPE*(-csub_Vgate_sourcep)));
_load_static_jacobian4(bulk,drainp,drainp,sourcep,(TYPE*(cbd_Vdrainp_sourcep-csub_Vdrainp_sourcep)));
_load_static_jacobian4(bulk,drainp,bulk,sourcep,(TYPE*(cbd_Vbulk_sourcep-csub_Vbulk_sourcep)));
#endif
_load_static_residual2(bulk,sourcep,(TYPE*cbs));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,sourcep,bulk,sourcep,(TYPE*cbs_Vbulk_sourcep));
#endif
}
else
{
_load_static_residual2(drainp,sourcep,(TYPE*(-cdrain)));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,sourcep,bulk,sourcep,(TYPE*(-cdrain_Vbulk_sourcep)));
_load_static_jacobian4(drainp,sourcep,drainp,sourcep,(TYPE*(-cdrain_Vdrainp_sourcep)));
_load_static_jacobian4(drainp,sourcep,gate,sourcep,(TYPE*(-cdrain_Vgate_sourcep)));
#endif
_load_static_residual2(bulk,drainp,(TYPE*cbd));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,drainp,drainp,sourcep,(TYPE*cbd_Vdrainp_sourcep));
_load_static_jacobian4(bulk,drainp,bulk,sourcep,(TYPE*cbd_Vbulk_sourcep));
#endif
_load_static_residual2(bulk,sourcep,(TYPE*(cbs-csub)));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,sourcep,drainp,sourcep,(TYPE*(-csub_Vdrainp_sourcep)));
_load_static_jacobian4(bulk,sourcep,gate,sourcep,(TYPE*(-csub_Vgate_sourcep)));
_load_static_jacobian4(bulk,sourcep,bulk,sourcep,(TYPE*(cbs_Vbulk_sourcep-csub_Vbulk_sourcep)));
#endif
}
if
((drainConductance>0.0))
{
_load_static_residual2(drain,drainp,(drainConductance*BP(drain,drainp)));
#if defined(_DERIVATE)
_load_static_jacobian4(drain,drainp,drain,drainp,(drainConductance));
#endif
}
else
{
_load_static_residual2(drain,drainp,(BP(drain,drainp)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(drain,drainp,drain,drainp,(1e3));
#endif
}
if
((sourceConductance>0.0))
{
_load_static_residual2(source,sourcep,(sourceConductance*BP(source,sourcep)));
#if defined(_DERIVATE)
_load_static_jacobian4(source,sourcep,source,sourcep,(sourceConductance));
#endif
}
else
{
_load_static_residual2(source,sourcep,(BP(source,sourcep)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(source,sourcep,source,sourcep,(1e3));
#endif
}
#if defined(_DYNAMIC)
cqgate=(TYPE*_DDT(Qgate));
#endif
#if defined(_DYNAMIC)
cqdrn=(TYPE*_DDT(Qdrn));
#endif
#if defined(_DYNAMIC)
cqbulk=(TYPE*_DDT(Qbulk));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(gate,(TYPE*_DDT(Qgate)));
#if defined(_DERIVATE)
_load_dynamic_jacobian2p(gate,bulk,sourcep,(TYPE*Qgate_Vbulk_sourcep));
_load_dynamic_jacobian2p(gate,gate,sourcep,(TYPE*Qgate_Vgate_sourcep));
_load_dynamic_jacobian2p(gate,drainp,sourcep,(TYPE*Qgate_Vdrainp_sourcep));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(drainp,(TYPE*_DDT(Qdrn)));
#if defined(_DERIVATE)
_load_dynamic_jacobian2p(drainp,bulk,sourcep,(TYPE*Qdrn_Vbulk_sourcep));
_load_dynamic_jacobian2p(drainp,drainp,sourcep,(TYPE*Qdrn_Vdrainp_sourcep));
_load_dynamic_jacobian2p(drainp,gate,sourcep,(TYPE*Qdrn_Vgate_sourcep));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(bulk,(TYPE*_DDT(Qbulk)));
#if defined(_DERIVATE)
_load_dynamic_jacobian2p(bulk,bulk,sourcep,(TYPE*Qbulk_Vbulk_sourcep));
_load_dynamic_jacobian2p(bulk,gate,sourcep,(TYPE*Qbulk_Vgate_sourcep));
_load_dynamic_jacobian2p(bulk,drainp,sourcep,(TYPE*Qbulk_Vdrainp_sourcep));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(sourcep,(-(((TYPE*_DDT(Qgate))+(TYPE*_DDT(Qdrn)))+(TYPE*_DDT(Qbulk)))));
#if defined(_DERIVATE)
_load_dynamic_jacobian2p(sourcep,bulk,sourcep,(-(((TYPE*Qgate_Vbulk_sourcep)+(TYPE*Qdrn_Vbulk_sourcep))+(TYPE*Qbulk_Vbulk_sourcep))));
_load_dynamic_jacobian2p(sourcep,gate,sourcep,(-(((TYPE*Qgate_Vgate_sourcep)+(TYPE*Qdrn_Vgate_sourcep))+(TYPE*Qbulk_Vgate_sourcep))));
_load_dynamic_jacobian2p(sourcep,drainp,sourcep,(-(((TYPE*Qgate_Vdrainp_sourcep)+(TYPE*Qdrn_Vdrainp_sourcep))+(TYPE*Qbulk_Vdrainp_sourcep))));
#endif
#endif
if
((NOIMOD==4))
{
fourkt=(5.5226012e-23*Temp);
leffx2=(leff*leff);
_save_flickernoise2(drainp,sourcep,((KF*pow(cdrain,AF))/(cox*leffx2)),EF,"flicker");
_save_whitenoise2(drainp,sourcep,(((fourkt*ueff)*fabs(qinv))/leffx2),"channel");
_save_whitenoise2(drain,drainp,fabs((fourkt*drainConductance)),"thermal");
_save_whitenoise2(sourcep,source,fabs((fourkt*sourceConductance)),"thermal");
}

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void bsim3v34pMOS::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 6; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 6; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void bsim3v34pMOS::saveOperatingPoints (void)
{
  // save global instance operating points
  setOperatingPoint ("TYPE", TYPE);
  setOperatingPoint ("tox", tox);
  setOperatingPoint ("cox", cox);
  setOperatingPoint ("vth0", vth0);
  setOperatingPoint ("ngate", ngate);
  setOperatingPoint ("uc", uc);
  setOperatingPoint ("tnom", tnom);
  setOperatingPoint ("pdiblb", pdiblb);
  setOperatingPoint ("bulkJctBotGradingCoeff", bulkJctBotGradingCoeff);
  setOperatingPoint ("bulkJctSideGradingCoeff", bulkJctSideGradingCoeff);
  setOperatingPoint ("bulkJctGateSideGradingCoeff", bulkJctGateSideGradingCoeff);
  setOperatingPoint ("unitAreaJctCap", unitAreaJctCap);
  setOperatingPoint ("unitAreaTempJctCap", unitAreaTempJctCap);
  setOperatingPoint ("unitLengthSidewallJctCap", unitLengthSidewallJctCap);
  setOperatingPoint ("unitLengthSidewallTempJctCap", unitLengthSidewallTempJctCap);
  setOperatingPoint ("unitLengthGateSidewallJctCap", unitLengthGateSidewallJctCap);
  setOperatingPoint ("unitLengthGateSidewallTempJctCap", unitLengthGateSidewallTempJctCap);
  setOperatingPoint ("Temp", Temp);
  setOperatingPoint ("factor1", factor1);
  setOperatingPoint ("vtm", vtm);
  setOperatingPoint ("T_0", T_0);
  setOperatingPoint ("T1", T1);
  setOperatingPoint ("T2", T2);
  setOperatingPoint ("T3", T3);
  setOperatingPoint ("T4", T4);
  setOperatingPoint ("T5", T5);
  setOperatingPoint ("PhiB", PhiB);
  setOperatingPoint ("PhiBSW", PhiBSW);
  setOperatingPoint ("PhiBSWG", PhiBSWG);
  setOperatingPoint ("tmp", tmp);
  setOperatingPoint ("tmp1", tmp1);
  setOperatingPoint ("tmp2", tmp2);
  setOperatingPoint ("tmp3", tmp3);
  setOperatingPoint ("leff", leff);
  setOperatingPoint ("weff", weff);
  setOperatingPoint ("leffCV", leffCV);
  setOperatingPoint ("weffCV", weffCV);
  setOperatingPoint ("cdsc", cdsc);
  setOperatingPoint ("cdscb", cdscb);
  setOperatingPoint ("cdscd", cdscd);
  setOperatingPoint ("cit", cit);
  setOperatingPoint ("nfactor", nfactor);
  setOperatingPoint ("xj", xj);
  setOperatingPoint ("a0", a0);
  setOperatingPoint ("ags", ags);
  setOperatingPoint ("a1", a1);
  setOperatingPoint ("a2", a2);
  setOperatingPoint ("keta", keta);
  setOperatingPoint ("vfb", vfb);
  setOperatingPoint ("k1", k1);
  setOperatingPoint ("kt1", kt1);
  setOperatingPoint ("kt1l", kt1l);
  setOperatingPoint ("kt2", kt2);
  setOperatingPoint ("k3", k3);
  setOperatingPoint ("k3b", k3b);
  setOperatingPoint ("w0", w0);
  setOperatingPoint ("nlx", nlx);
  setOperatingPoint ("dvt0", dvt0);
  setOperatingPoint ("dvt1", dvt1);
  setOperatingPoint ("dvt2", dvt2);
  setOperatingPoint ("dvt0w", dvt0w);
  setOperatingPoint ("dvt1w", dvt1w);
  setOperatingPoint ("dvt2w", dvt2w);
  setOperatingPoint ("ua", ua);
  setOperatingPoint ("ub", ub);
  setOperatingPoint ("voff", voff);
  setOperatingPoint ("delta", delta);
  setOperatingPoint ("prwg", prwg);
  setOperatingPoint ("prwb", prwb);
  setOperatingPoint ("eta0", eta0);
  setOperatingPoint ("etab", etab);
  setOperatingPoint ("pclm", pclm);
  setOperatingPoint ("pscbe1", pscbe1);
  setOperatingPoint ("pscbe2", pscbe2);
  setOperatingPoint ("pvag", pvag);
  setOperatingPoint ("dwg", dwg);
  setOperatingPoint ("dwb", dwb);
  setOperatingPoint ("b0", b0);
  setOperatingPoint ("b1", b1);
  setOperatingPoint ("alpha0", alpha0);
  setOperatingPoint ("alpha1", alpha1);
  setOperatingPoint ("beta0", beta0);
  setOperatingPoint ("cgsl", cgsl);
  setOperatingPoint ("cgdl", cgdl);
  setOperatingPoint ("ckappa", ckappa);
  setOperatingPoint ("vfbcv", vfbcv);
  setOperatingPoint ("acde", acde);
  setOperatingPoint ("moin", moin);
  setOperatingPoint ("noff_param", noff_param);
  setOperatingPoint ("voffcv_param", voffcv_param);
  setOperatingPoint ("abulkCVfactor", abulkCVfactor);
  setOperatingPoint ("u0temp", u0temp);
  setOperatingPoint ("vsattemp", vsattemp);
  setOperatingPoint ("rds0", rds0);
  setOperatingPoint ("phi", phi);
  setOperatingPoint ("sqrtPhi", sqrtPhi);
  setOperatingPoint ("phis3", phis3);
  setOperatingPoint ("Xdep0", Xdep0);
  setOperatingPoint ("litl", litl);
  setOperatingPoint ("vbi", vbi);
  setOperatingPoint ("cdep0", cdep0);
  setOperatingPoint ("ldeb", ldeb);
  setOperatingPoint ("vbsc", vbsc);
  setOperatingPoint ("k1ox", k1ox);
  setOperatingPoint ("k2ox", k2ox);
  setOperatingPoint ("theta0vb0", theta0vb0);
  setOperatingPoint ("thetaRout", thetaRout);
  setOperatingPoint ("vfbzb", vfbzb);
  setOperatingPoint ("sourceArea", sourceArea);
  setOperatingPoint ("drainArea", drainArea);
  setOperatingPoint ("sourcePerimeter", sourcePerimeter);
  setOperatingPoint ("drainPerimeter", drainPerimeter);
  setOperatingPoint ("SourceSatCurrent", SourceSatCurrent);
  setOperatingPoint ("DrainSatCurrent", DrainSatCurrent);
  setOperatingPoint ("sourceConductance", sourceConductance);
  setOperatingPoint ("drainConductance", drainConductance);
  setOperatingPoint ("Nvtm", Nvtm);
  setOperatingPoint ("vjsm", vjsm);
  setOperatingPoint ("vjdm", vjdm);
  setOperatingPoint ("IsEvjsm", IsEvjsm);
  setOperatingPoint ("IsEvjdm", IsEvjdm);
  setOperatingPoint ("cgso_param", cgso_param);
  setOperatingPoint ("cgdo_param", cgdo_param);
  setOperatingPoint ("cgbo_param", cgbo_param);
}

/* Load operating points. */
void bsim3v34pMOS::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void bsim3v34pMOS::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void bsim3v34pMOS::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void bsim3v34pMOS::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix bsim3v34pMOS::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (6);

  for (int i1 = 0; i1 < 6; i1++) {
    for (int i2 = 0; i2 < 6; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void bsim3v34pMOS::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void bsim3v34pMOS::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void bsim3v34pMOS::initTR (void)
{
  setStates (2 * 6 * 6);
  initDC ();
}

/* Perform transient analysis iteration step. */
void bsim3v34pMOS::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
    state = 2 * (i2 + 6 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 6; i1++) {
    state = 2 * (i1 + 6 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 6; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i3 = 0; i3 < 6; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix bsim3v34pMOS::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (6);

  _load_flickernoise2 (drainp,sourcep,_flicker_pwr[drainp][sourcep],_flicker_exp[drainp][sourcep]);
  _load_whitenoise2 (sourcep,source,_white_pwr[sourcep][source]);
  _load_whitenoise2 (drain,drainp,_white_pwr[drain][drainp]);
  _load_whitenoise2 (drainp,sourcep,_white_pwr[drainp][sourcep]);

  return cy;
}

/* Perform AC noise computations. */
void bsim3v34pMOS::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void bsim3v34pMOS::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void bsim3v34pMOS::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void bsim3v34pMOS::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 6; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 6; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "bsim3v34pMOS.defs.h"
