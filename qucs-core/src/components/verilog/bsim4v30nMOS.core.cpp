/*
 * bsim4v30nMOS.core.cpp - device implementations for bsim4v30nMOS module
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

#include "bsim4v30nMOS.analogfunction.h"
#include "component.h"
#include "device.h"
#include "bsim4v30nMOS.core.h"

#ifndef CIR_bsim4v30nMOS
#define CIR_bsim4v30nMOS -1
#endif

// external nodes
#define drain 0
#define gate 1
#define source 2
#define bulk 3
// internal nodes
#define drainp 4
#define sourcep 5
#define gatep 6
#define gatem 7
#define drainb 8
#define sourceb 9
#define bulkp 10

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
#define _modelname              "bsim4v30nMOS"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
bsim4v30nMOS::bsim4v30nMOS() : circuit (11)
{
  type = CIR_bsim4v30nMOS;
}

/* Initialization of model. */
void bsim4v30nMOS::initModel (void)
{
  // create internal nodes
  setInternalNode (drainp, "drainp");
  setInternalNode (sourcep, "sourcep");
  setInternalNode (gatep, "gatep");
  setInternalNode (gatem, "gatem");
  setInternalNode (drainb, "drainb");
  setInternalNode (sourceb, "sourceb");
  setInternalNode (bulkp, "bulkp");

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
void bsim4v30nMOS::initDC (void)
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
void bsim4v30nMOS::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void bsim4v30nMOS::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[sourcep][source] = 0.0;
  _white_pwr[drain][drainp] = 0.0;
  _white_pwr[drainp][sourcep] = 0.0;
  _flicker_pwr[drainp][sourcep] = 0.0;
  _flicker_exp[drainp][sourcep] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 11; i1++) {
  for (i2 = 0; i2 < 11; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 11; i1++) {
  for (i2 = 0; i2 < 11; i2++) {
  for (i3 = 0; i3 < 11; i3++) {
  for (i4 = 0; i4 < 11; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 11; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 11; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void bsim4v30nMOS::loadVariables (void)
{
  GMIN = getPropertyDouble ("GMIN");
  PS = getPropertyDouble ("PS");
  PD = getPropertyDouble ("PD");
  AS = getPropertyDouble ("AS");
  AD = getPropertyDouble ("AD");
  CGBO = getPropertyDouble ("CGBO");
  CGDO = getPropertyDouble ("CGDO");
  CGSO = getPropertyDouble ("CGSO");
  L = getPropertyDouble ("L");
  W = getPropertyDouble ("W");
  MOBMOD = getPropertyDouble ("MOBMOD");
  RDSMOD = getPropertyDouble ("RDSMOD");
  IGCMOD = getPropertyDouble ("IGCMOD");
  IGBMOD = getPropertyDouble ("IGBMOD");
  CAPMOD = getPropertyDouble ("CAPMOD");
  RGATEMOD = getPropertyDouble ("RGATEMOD");
  RBODYMOD = getPropertyDouble ("RBODYMOD");
  DIOMOD = getPropertyDouble ("DIOMOD");
  TEMPMOD = getPropertyDouble ("TEMPMOD");
  GEOMOD = getPropertyDouble ("GEOMOD");
  RGEOMOD = getPropertyDouble ("RGEOMOD");
  PERMOD = getPropertyDouble ("PERMOD");
  TNOIMOD = getPropertyDouble ("TNOIMOD");
  FNOIMOD = getPropertyDouble ("FNOIMOD");
  EPSROX = getPropertyDouble ("EPSROX");
  TOXE = getPropertyDouble ("TOXE");
  TOXP = getPropertyDouble ("TOXP");
  TOXM = getPropertyDouble ("TOXM");
  DTOX = getPropertyDouble ("DTOX");
  XJ = getPropertyDouble ("XJ");
  GAMMA1 = getPropertyDouble ("GAMMA1");
  GAMMA2 = getPropertyDouble ("GAMMA2");
  NDEP = getPropertyDouble ("NDEP");
  NSUB = getPropertyDouble ("NSUB");
  NGATE = getPropertyDouble ("NGATE");
  NSD = getPropertyDouble ("NSD");
  VBX = getPropertyDouble ("VBX");
  XT = getPropertyDouble ("XT");
  RSH = getPropertyDouble ("RSH");
  RSHG = getPropertyDouble ("RSHG");
  VTH0 = getPropertyDouble ("VTH0");
  VFB = getPropertyDouble ("VFB");
  PHIN = getPropertyDouble ("PHIN");
  K1 = getPropertyDouble ("K1");
  K2 = getPropertyDouble ("K2");
  K3 = getPropertyDouble ("K3");
  K3B = getPropertyDouble ("K3B");
  W0 = getPropertyDouble ("W0");
  LPE0 = getPropertyDouble ("LPE0");
  LPEB = getPropertyDouble ("LPEB");
  VBM = getPropertyDouble ("VBM");
  DVT0 = getPropertyDouble ("DVT0");
  DVT1 = getPropertyDouble ("DVT1");
  DVT2 = getPropertyDouble ("DVT2");
  DVTP0 = getPropertyDouble ("DVTP0");
  DVTP1 = getPropertyDouble ("DVTP1");
  DVT0W = getPropertyDouble ("DVT0W");
  DVT1W = getPropertyDouble ("DVT1W");
  DVT2W = getPropertyDouble ("DVT2W");
  U0 = getPropertyDouble ("U0");
  UA = getPropertyDouble ("UA");
  UB = getPropertyDouble ("UB");
  UC = getPropertyDouble ("UC");
  EU = getPropertyDouble ("EU");
  VSAT = getPropertyDouble ("VSAT");
  A0 = getPropertyDouble ("A0");
  AGS = getPropertyDouble ("AGS");
  B0 = getPropertyDouble ("B0");
  B1 = getPropertyDouble ("B1");
  KETA = getPropertyDouble ("KETA");
  A1 = getPropertyDouble ("A1");
  A2 = getPropertyDouble ("A2");
  WINT = getPropertyDouble ("WINT");
  LINT = getPropertyDouble ("LINT");
  DWG = getPropertyDouble ("DWG");
  DWB = getPropertyDouble ("DWB");
  VOFF = getPropertyDouble ("VOFF");
  VOFFL = getPropertyDouble ("VOFFL");
  MINV = getPropertyDouble ("MINV");
  NFACTOR = getPropertyDouble ("NFACTOR");
  ETA0 = getPropertyDouble ("ETA0");
  ETAB = getPropertyDouble ("ETAB");
  DROUT = getPropertyDouble ("DROUT");
  DSUB = getPropertyDouble ("DSUB");
  CIT = getPropertyDouble ("CIT");
  CDSC = getPropertyDouble ("CDSC");
  CDSCB = getPropertyDouble ("CDSCB");
  CDSCD = getPropertyDouble ("CDSCD");
  PCLM = getPropertyDouble ("PCLM");
  PDIBL1 = getPropertyDouble ("PDIBL1");
  PDIBL2 = getPropertyDouble ("PDIBL2");
  PDIBLB = getPropertyDouble ("PDIBLB");
  PSCBE1 = getPropertyDouble ("PSCBE1");
  PSCBE2 = getPropertyDouble ("PSCBE2");
  PVAG = getPropertyDouble ("PVAG");
  DELTA = getPropertyDouble ("DELTA");
  FPROUT = getPropertyDouble ("FPROUT");
  PDITS = getPropertyDouble ("PDITS");
  PDITSD = getPropertyDouble ("PDITSD");
  PDITSL = getPropertyDouble ("PDITSL");
  LAMBDA = getPropertyDouble ("LAMBDA");
  VTL = getPropertyDouble ("VTL");
  LC = getPropertyDouble ("LC");
  XN = getPropertyDouble ("XN");
  RDSW = getPropertyDouble ("RDSW");
  RDSWMIN = getPropertyDouble ("RDSWMIN");
  RDW = getPropertyDouble ("RDW");
  RDWMIN = getPropertyDouble ("RDWMIN");
  RSW = getPropertyDouble ("RSW");
  RSWMIN = getPropertyDouble ("RSWMIN");
  PRWG = getPropertyDouble ("PRWG");
  PRWB = getPropertyDouble ("PRWB");
  WR = getPropertyDouble ("WR");
  NRS = getPropertyDouble ("NRS");
  NRD = getPropertyDouble ("NRD");
  ALPHA0 = getPropertyDouble ("ALPHA0");
  ALPHA1 = getPropertyDouble ("ALPHA1");
  BETA0 = getPropertyDouble ("BETA0");
  AGIDL = getPropertyDouble ("AGIDL");
  BGIDL = getPropertyDouble ("BGIDL");
  CGIDL = getPropertyDouble ("CGIDL");
  EGIDL = getPropertyDouble ("EGIDL");
  AIGBACC = getPropertyDouble ("AIGBACC");
  BIGBACC = getPropertyDouble ("BIGBACC");
  CIGBACC = getPropertyDouble ("CIGBACC");
  NIGBACC = getPropertyDouble ("NIGBACC");
  AIGBINV = getPropertyDouble ("AIGBINV");
  BIGBINV = getPropertyDouble ("BIGBINV");
  CIGBINV = getPropertyDouble ("CIGBINV");
  EIGBINV = getPropertyDouble ("EIGBINV");
  NIGBINV = getPropertyDouble ("NIGBINV");
  AIGC = getPropertyDouble ("AIGC");
  BIGC = getPropertyDouble ("BIGC");
  CIGC = getPropertyDouble ("CIGC");
  AIGSD = getPropertyDouble ("AIGSD");
  BIGSD = getPropertyDouble ("BIGSD");
  CIGSD = getPropertyDouble ("CIGSD");
  DLCIG = getPropertyDouble ("DLCIG");
  NIGC = getPropertyDouble ("NIGC");
  POXEDGE = getPropertyDouble ("POXEDGE");
  PIGCD = getPropertyDouble ("PIGCD");
  NTOX = getPropertyDouble ("NTOX");
  TOXREF = getPropertyDouble ("TOXREF");
  XPART = getPropertyDouble ("XPART");
  CGS0 = getPropertyDouble ("CGS0");
  CGD0 = getPropertyDouble ("CGD0");
  CGB0 = getPropertyDouble ("CGB0");
  CGSL = getPropertyDouble ("CGSL");
  CGDL = getPropertyDouble ("CGDL");
  CKAPPAS = getPropertyDouble ("CKAPPAS");
  CKAPPAD = getPropertyDouble ("CKAPPAD");
  CF = getPropertyDouble ("CF");
  CLC = getPropertyDouble ("CLC");
  CLE = getPropertyDouble ("CLE");
  DLC = getPropertyDouble ("DLC");
  DWC = getPropertyDouble ("DWC");
  VFBCV = getPropertyDouble ("VFBCV");
  NOFF = getPropertyDouble ("NOFF");
  VOFFCV = getPropertyDouble ("VOFFCV");
  ACDE = getPropertyDouble ("ACDE");
  MOIN = getPropertyDouble ("MOIN");
  XRCRG1 = getPropertyDouble ("XRCRG1");
  XRCRG2 = getPropertyDouble ("XRCRG2");
  RBPB = getPropertyDouble ("RBPB");
  RBPD = getPropertyDouble ("RBPD");
  RBPS = getPropertyDouble ("RBPS");
  RBDB = getPropertyDouble ("RBDB");
  RBSB = getPropertyDouble ("RBSB");
  GBMIN = getPropertyDouble ("GBMIN");
  DMCG = getPropertyDouble ("DMCG");
  DMCI = getPropertyDouble ("DMCI");
  DMDG = getPropertyDouble ("DMDG");
  DMCGT = getPropertyDouble ("DMCGT");
  NF = getPropertyDouble ("NF");
  DWJ = getPropertyDouble ("DWJ");
  MIN = getPropertyDouble ("MIN");
  XGW = getPropertyDouble ("XGW");
  XGL = getPropertyDouble ("XGL");
  XL = getPropertyDouble ("XL");
  XW = getPropertyDouble ("XW");
  NGCON = getPropertyDouble ("NGCON");
  IJTHSREV = getPropertyDouble ("IJTHSREV");
  IJTHDREV = getPropertyDouble ("IJTHDREV");
  IJTHSFWD = getPropertyDouble ("IJTHSFWD");
  IJTHDFWD = getPropertyDouble ("IJTHDFWD");
  XJBVS = getPropertyDouble ("XJBVS");
  XJBVD = getPropertyDouble ("XJBVD");
  BVS = getPropertyDouble ("BVS");
  BVD = getPropertyDouble ("BVD");
  JSS = getPropertyDouble ("JSS");
  JSD = getPropertyDouble ("JSD");
  JSWS = getPropertyDouble ("JSWS");
  JSWD = getPropertyDouble ("JSWD");
  JSWGS = getPropertyDouble ("JSWGS");
  JSWGD = getPropertyDouble ("JSWGD");
  CJS = getPropertyDouble ("CJS");
  CJD = getPropertyDouble ("CJD");
  MJS = getPropertyDouble ("MJS");
  MJD = getPropertyDouble ("MJD");
  MJSWS = getPropertyDouble ("MJSWS");
  MJSWD = getPropertyDouble ("MJSWD");
  CJSWS = getPropertyDouble ("CJSWS");
  CJSWD = getPropertyDouble ("CJSWD");
  CJSWGS = getPropertyDouble ("CJSWGS");
  CJSWGD = getPropertyDouble ("CJSWGD");
  MJSWGS = getPropertyDouble ("MJSWGS");
  MJSWGD = getPropertyDouble ("MJSWGD");
  PBS = getPropertyDouble ("PBS");
  PBD = getPropertyDouble ("PBD");
  PBSWS = getPropertyDouble ("PBSWS");
  PBSWD = getPropertyDouble ("PBSWD");
  PBSWGS = getPropertyDouble ("PBSWGS");
  PBSWGD = getPropertyDouble ("PBSWGD");
  TNOM = getPropertyDouble ("TNOM");
  UTE = getPropertyDouble ("UTE");
  KT1 = getPropertyDouble ("KT1");
  KT1L = getPropertyDouble ("KT1L");
  KT2 = getPropertyDouble ("KT2");
  UA1 = getPropertyDouble ("UA1");
  UB1 = getPropertyDouble ("UB1");
  UC1 = getPropertyDouble ("UC1");
  AT = getPropertyDouble ("AT");
  PRT = getPropertyDouble ("PRT");
  NJS = getPropertyDouble ("NJS");
  NJD = getPropertyDouble ("NJD");
  XTIS = getPropertyDouble ("XTIS");
  XTID = getPropertyDouble ("XTID");
  TPB = getPropertyDouble ("TPB");
  TPBSW = getPropertyDouble ("TPBSW");
  TPBSWG = getPropertyDouble ("TPBSWG");
  TCJ = getPropertyDouble ("TCJ");
  TCJSW = getPropertyDouble ("TCJSW");
  TCJSWG = getPropertyDouble ("TCJSWG");
  SA = getPropertyDouble ("SA");
  SB = getPropertyDouble ("SB");
  SD = getPropertyDouble ("SD");
  SAREF = getPropertyDouble ("SAREF");
  SBREF = getPropertyDouble ("SBREF");
  WLOD = getPropertyDouble ("WLOD");
  KU0 = getPropertyDouble ("KU0");
  KVSAT = getPropertyDouble ("KVSAT");
  TKU0 = getPropertyDouble ("TKU0");
  LKU0 = getPropertyDouble ("LKU0");
  WKU0 = getPropertyDouble ("WKU0");
  PKU0 = getPropertyDouble ("PKU0");
  LLODKU0 = getPropertyDouble ("LLODKU0");
  WLODKU0 = getPropertyDouble ("WLODKU0");
  KVTH0 = getPropertyDouble ("KVTH0");
  LKVTH0 = getPropertyDouble ("LKVTH0");
  WKVTH0 = getPropertyDouble ("WKVTH0");
  PKVTH0 = getPropertyDouble ("PKVTH0");
  LLODVTH = getPropertyDouble ("LLODVTH");
  WLODVTH = getPropertyDouble ("WLODVTH");
  STK2 = getPropertyDouble ("STK2");
  LODK2 = getPropertyDouble ("LODK2");
  STETA0 = getPropertyDouble ("STETA0");
  LODETA0 = getPropertyDouble ("LODETA0");
  WL = getPropertyDouble ("WL");
  WLN = getPropertyDouble ("WLN");
  WW = getPropertyDouble ("WW");
  WWN = getPropertyDouble ("WWN");
  WWL = getPropertyDouble ("WWL");
  LL = getPropertyDouble ("LL");
  LLN = getPropertyDouble ("LLN");
  LW = getPropertyDouble ("LW");
  LWN = getPropertyDouble ("LWN");
  LWL = getPropertyDouble ("LWL");
  LLC = getPropertyDouble ("LLC");
  LWC = getPropertyDouble ("LWC");
  LWLC = getPropertyDouble ("LWLC");
  WLC = getPropertyDouble ("WLC");
  WWC = getPropertyDouble ("WWC");
  WWLC = getPropertyDouble ("WWLC");
  NTNOI = getPropertyDouble ("NTNOI");
  KF = getPropertyDouble ("KF");
  AF = getPropertyDouble ("AF");
  EF = getPropertyDouble ("EF");
  TEMP = getPropertyDouble ("TEMP");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void bsim4v30nMOS::initializeModel (void)
{
double dmdgeff;
double dmcieff;
double dmcgeff;
double deta0_lod;
double dk2_lod;
double dvth0_lod;
double od_offset;
double rho;
double inv_odeff;
int i;
double inv_sb;
double inv_sa;
double rho_ref;
double inv_od_ref;
double inv_sbref;
double inv_saref;
double ku0temp;
double w_tmp;
double ldrn;
#if defined(_DERIVATE)
double tmp3_Vgatep_sourcep;
double tmp3_Vdrainp_sourcep;
double tmp3_Vdrainb_sourcep;
double tmp3_Vbulkp_sourcep;
double tmp3_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t9_Vgatep_sourcep;
double t9_Vdrainp_sourcep;
double t9_Vdrainb_sourcep;
double t9_Vbulkp_sourcep;
double t9_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t8_Vgatep_sourcep;
double t8_Vdrainp_sourcep;
double t8_Vdrainb_sourcep;
double t8_Vbulkp_sourcep;
double t8_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double tmp_Vgatep_sourcep;
double tmp_Vdrainp_sourcep;
double tmp_Vdrainb_sourcep;
double tmp_Vbulkp_sourcep;
double tmp_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t5_Vgatep_sourcep;
double t5_Vdrainp_sourcep;
double t5_Vdrainb_sourcep;
double t5_Vbulkp_sourcep;
double t5_Vsourceb_sourcep;
#endif
double toxratioedge;
#if defined(_DERIVATE)
double t10_Vgatep_sourcep;
double t10_Vdrainp_sourcep;
double t10_Vdrainb_sourcep;
double t10_Vbulkp_sourcep;
double t10_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t4_Vgatep_sourcep;
double t4_Vdrainp_sourcep;
double t4_Vdrainb_sourcep;
double t4_Vbulkp_sourcep;
double t4_Vsourceb_sourcep;
#endif
double powweffwr;
double dw;
#if defined(_DERIVATE)
double tmp2_Vgatep_sourcep;
double tmp2_Vdrainp_sourcep;
double tmp2_Vdrainb_sourcep;
double tmp2_Vbulkp_sourcep;
double tmp2_Vsourceb_sourcep;
#endif
double dl;
#if defined(_DERIVATE)
double tmp1_Vgatep_sourcep;
double tmp1_Vdrainp_sourcep;
double tmp1_Vdrainb_sourcep;
double tmp1_Vbulkp_sourcep;
double tmp1_Vsourceb_sourcep;
#endif
double wnew;
double lnew;
double deltemp;
#if defined(_DERIVATE)
double t3_Vgatep_sourcep;
double t3_Vdrainp_sourcep;
double t3_Vdrainb_sourcep;
double t3_Vbulkp_sourcep;
double t3_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t2_Vgatep_sourcep;
double t2_Vdrainp_sourcep;
double t2_Vdrainb_sourcep;
double t2_Vbulkp_sourcep;
double t2_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t1_Vgatep_sourcep;
double t1_Vdrainp_sourcep;
double t1_Vdrainb_sourcep;
double t1_Vbulkp_sourcep;
double t1_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t0_Vgatep_sourcep;
double t0_Vdrainp_sourcep;
double t0_Vdrainb_sourcep;
double t0_Vbulkp_sourcep;
double t0_Vsourceb_sourcep;
#endif
double eg;
double ni;
double eg0;
double vtm0;
double lwlc;
double lwc;
double llc;
double xw;
double xtis;
double xtid;
double xt;
double xn;
double xl;
double xgw;
double xgl;
double wwn;
double wwlc;
double wwl;
double wwc;
double ww;
double wr;
double wlodvth;
double wlodku0;
double wlod;
double wln;
double wlc;
double wl;
double wkvth0;
double wku0;
double wint;
double w;
double vsat;
double voffl;
double voff;
double vbx;
double vbm;
double ute;
double uc1;
double ub1;
double ua1;
double u0;
double tpbswg;
double tpbsw;
double tpb;
double toxref;
double toxm;
double tku0;
double tempmod;
double tcjswg;
double tcjsw;
double tcj;
double stk2;
double steta0;
double sd;
double sbref;
double sb;
double saref;
double sa;
double rsw;
double rshg;
double rsh;
double rgeomod;
double rdw;
double rdsw;
double rbsb;
double rbps;
double rbpd;
double rbpb;
double rbdb;
double ps;
double prt;
double poxedge;
double pkvth0;
double pku0;
double phin;
double permod;
double pdibl2;
double pdibl1;
double pd;
double pbsws;
double pbswgs;
double pbswgd;
double pbswd;
double pbs;
double pbd;
double ntox;
double nsub;
double nsd;
double nrs;
double nrd;
double ngcon;
double ndep;
double minv;
double lwn;
double lwl;
double lw;
double lodk2;
double lodeta0;
double llodvth;
double llodku0;
double lln;
double ll;
double lkvth0;
double lku0;
double lint;
double lc;
double l;
double kvth0;
double kvsat;
double ku0;
double k2;
double jsws;
double jswgs;
double jswgd;
double jswd;
double jss;
double jsd;
double imin;
double ijthsrev;
double ijthsfwd;
double ijthdrev;
double ijthdfwd;
double geomod;
double gbmin;
double gamma2;
double gamma1;
double epsrox;
double dwj;
double dwc;
double dsub;
double drout;
double dmdg;
double dmci;
double dmcgt;
double dmcg;
double dlcig;
double dlc;
double cle;
double clc;
double cjsws;
double cjswgs;
double cjswgd;
double cjswd;
double cjs;
double cjd;
double cf;
double at;
double as;
double ad;
{
a0=A0;
a1=A1;
a2=A2;
acde=ACDE;
ad=AD;
agidl=AGIDL;
ags=AGS;
aigbacc=AIGBACC;
aigbinv=AIGBINV;
aigc=AIGC;
aigsd=AIGSD;
alpha0=ALPHA0;
alpha1=ALPHA1;
as=AS;
at=AT;
b0=B0;
b1=B1;
beta0=BETA0;
bgidl=BGIDL;
bigbacc=BIGBACC;
bigbinv=BIGBINV;
bigc=BIGC;
bigsd=BIGSD;
bvd=BVD;
bvs=BVS;
capmod=CAPMOD;
cdsc=CDSC;
cdscb=CDSCB;
cdscd=CDSCD;
cf=CF;
cgbo=CGBO;
cgdl=CGDL;
cgidl=CGIDL;
cgsl=CGSL;
cigbacc=CIGBACC;
cigbinv=CIGBINV;
cigc=CIGC;
cigsd=CIGSD;
cit=CIT;
cjd=CJD;
cjs=CJS;
cjswd=CJSWD;
cjswgd=CJSWGD;
cjswgs=CJSWGS;
cjsws=CJSWS;
ckappad=CKAPPAD;
ckappas=CKAPPAS;
clc=CLC;
cle=CLE;
delta=DELTA;
diomod=DIOMOD;
dlc=DLC;
dlcig=DLCIG;
dmcg=DMCG;
dmcgt=DMCGT;
dmci=DMCI;
dmdg=DMDG;
drout=DROUT;
dsub=DSUB;
dvt0=DVT0;
dvt0w=DVT0W;
dvt1=DVT1;
dvt1w=DVT1W;
dvt2=DVT2;
dvt2w=DVT2W;
dvtp0=DVTP0;
dvtp1=DVTP1;
dwb=DWB;
dwc=DWC;
dwg=DWG;
dwj=DWJ;
egidl=EGIDL;
eigbinv=EIGBINV;
epsrox=EPSROX;
eta0=ETA0;
etab=ETAB;
eu=EU;
fprout=FPROUT;
gamma1=GAMMA1;
gamma2=GAMMA2;
gbmin=GBMIN;
geomod=GEOMOD;
gmin=GMIN;
igbmod=IGBMOD;
igcmod=IGCMOD;
ijthdfwd=IJTHDFWD;
ijthdrev=IJTHDREV;
ijthsfwd=IJTHSFWD;
ijthsrev=IJTHSREV;
imin=MIN;
jsd=JSD;
jss=JSS;
jswd=JSWD;
jswgd=JSWGD;
jswgs=JSWGS;
jsws=JSWS;
k1=K1;
k2=K2;
k3=K3;
k3b=K3B;
keta=KETA;
kt1=KT1;
kt1l=KT1L;
kt2=KT2;
ku0=KU0;
kvsat=KVSAT;
kvth0=KVTH0;
l=L;
lambda=LAMBDA;
lc=LC;
lint=LINT;
lku0=LKU0;
lkvth0=LKVTH0;
ll=LL;
lln=LLN;
llodku0=LLODKU0;
llodvth=LLODVTH;
lodeta0=LODETA0;
lodk2=LODK2;
lpe0=LPE0;
lpeb=LPEB;
lw=LW;
lwl=LWL;
lwn=LWN;
minv=MINV;
mjd=MJD;
mjs=MJS;
mjswd=MJSWD;
mjswgd=MJSWGD;
mjswgs=MJSWGS;
mjsws=MJSWS;
mobmod=MOBMOD;
moin=MOIN;
ndep=NDEP;
nf=NF;
nfactor=NFACTOR;
ngate=NGATE;
ngcon=NGCON;
nigbacc=NIGBACC;
nigbinv=NIGBINV;
nigc=NIGC;
njd=NJD;
njs=NJS;
noff=NOFF;
nrd=NRD;
nrs=NRS;
nsd=NSD;
nsub=NSUB;
ntox=NTOX;
param_cgdo=CGDO;
param_cgso=CGSO;
pbd=PBD;
pbs=PBS;
pbswd=PBSWD;
pbswgd=PBSWGD;
pbswgs=PBSWGS;
pbsws=PBSWS;
pclm=PCLM;
pd=PD;
pdibl1=PDIBL1;
pdibl2=PDIBL2;
pdiblb=PDIBLB;
pdits=PDITS;
pditsd=PDITSD;
pditsl=PDITSL;
permod=PERMOD;
phin=PHIN;
pigcd=PIGCD;
pku0=PKU0;
pkvth0=PKVTH0;
poxedge=POXEDGE;
prt=PRT;
prwb=PRWB;
prwg=PRWG;
ps=PS;
pscbe1=PSCBE1;
pscbe2=PSCBE2;
pvag=PVAG;
rbdb=RBDB;
rbodymod=RBODYMOD;
rbpb=RBPB;
rbpd=RBPD;
rbps=RBPS;
rbsb=RBSB;
rdsmod=RDSMOD;
rdsw=RDSW;
rdswmin=RDSWMIN;
rdw=RDW;
rdwmin=RDWMIN;
rgatemod=RGATEMOD;
rgeomod=RGEOMOD;
rsh=RSH;
rshg=RSHG;
rsw=RSW;
rswmin=RSWMIN;
sa=SA;
saref=SAREF;
sb=SB;
sbref=SBREF;
sd=SD;
steta0=STETA0;
stk2=STK2;
tcj=TCJ;
tcjsw=TCJSW;
tcjswg=TCJSWG;
tempmod=TEMPMOD;
tku0=TKU0;
toxe=TOXE;
toxm=TOXM;
toxp=TOXP;
toxref=TOXREF;
tpb=TPB;
tpbsw=TPBSW;
tpbswg=TPBSWG;
type=1.0;
u0=U0;
ua=UA;
ua1=UA1;
ub=UB;
ub1=UB1;
uc=UC;
uc1=UC1;
ute=UTE;
vbm=VBM;
vbx=VBX;
vfb=VFB;
vfbcv=VFBCV;
voff=VOFF;
voffcv=VOFFCV;
voffl=VOFFL;
vsat=VSAT;
vth0=VTH0;
vtl=VTL;
w=W;
w0=W0;
wint=WINT;
wku0=WKU0;
wkvth0=WKVTH0;
wl=WL;
wlc=WLC;
wln=WLN;
wlod=WLOD;
wlodku0=WLODKU0;
wlodvth=WLODVTH;
wr=WR;
ww=WW;
wwc=WWC;
wwl=WWL;
wwlc=WWLC;
wwn=WWN;
xgl=XGL;
xgw=XGW;
xj=XJ;
xjbvd=XJBVD;
xjbvs=XJBVS;
xl=XL;
xn=XN;
xrcrg1=XRCRG1;
xrcrg2=XRCRG2;
xt=XT;
xtid=XTID;
xtis=XTIS;
xw=XW;
xpart=XPART;
llc=LLC;
lwc=LWC;
lwlc=LWLC;
Temp=(TEMP+273.15);
if
((mobmod==(-99.0)))
{
mobmod=0;
}
else
{
if
((((mobmod!=0)&&(mobmod!=1))&&(mobmod!=2)))
{
mobmod=0;
logprint(LOG_ERROR,"Warning: MOBMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
}
if
((rdsmod==(-99.0)))
{
rdsmod=0;
}
else
{
if
(((rdsmod!=0)&&(rdsmod!=1)))
{
rdsmod=0;
logprint(LOG_ERROR,"Warning: RDSMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
}
if
((tempmod==(-99.0)))
{
tempmod=0;
}
else
{
if
(((tempmod!=0)&&(tempmod!=1)))
{
tempmod=0;
logprint(LOG_ERROR,"Warning: TEMPMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
}
if
((((diomod!=0)&&(diomod!=1))&&(diomod!=2)))
{
diomod=1;
logprint(LOG_ERROR,"Warning: DIOMOD has been set to its default value: 1.");
logprint(LOG_ERROR,"\n");
}
if
((((capmod!=0)&&(capmod!=1))&&(capmod!=2)))
{
capmod=2;
logprint(LOG_ERROR,"Warning: CAPMOD has been set to its default value: 2.");
logprint(LOG_ERROR,"\n");
}
if
(((permod!=0)&&(permod!=1)))
{
permod=1;
logprint(LOG_ERROR,"Warning: PERMOD has been set to its default value: 1.");
logprint(LOG_ERROR,"\n");
}
if
(((igcmod!=0)&&(igcmod!=1)))
{
igcmod=0;
logprint(LOG_ERROR,"Warning: IGCMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
if
(((igbmod!=0)&&(igbmod!=1)))
{
igbmod=0;
logprint(LOG_ERROR,"Warning: IGBMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
if
(((rbodymod!=0)&&(rbodymod!=1)))
{
rbodymod=0;
logprint(LOG_ERROR,"Warning: RBODYMOD has been set to its default value: 0.");
logprint(LOG_ERROR,"\n");
}
if
((toxref<=0.0))
{
logprint(LOG_ERROR,"Fatal: TOXREF = %e is not positive.",TOXREF);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
(((((toxe!=(-99.0))&&(toxp!=(-99.0)))&&(DTOX!=0.0))&&(toxe!=(toxp+DTOX))))
{
logprint(LOG_ERROR,"Warning: TOXE, TOXP and DTOX all given and TOXE != TOXP + DTOX. DTOX ignored.");
logprint(LOG_ERROR,"\n");
}
else
{
if
(((toxe!=(-99.0))&&(toxp==(-99.0))))
{
toxp=(toxe-DTOX);
}
else
{
if
(((toxe==(-99.0))&&(toxp!=(-99.0))))
{
toxe=(toxp+DTOX);
}
else
{
if
(((toxp==(-99.0))&&(toxe==(-99.0))))
{
toxe=3.0e-9;
toxp=toxe;
}
}
}
}
cox=(3.45311e-11/toxe);
if
((toxe<1.0e-10))
{
logprint(LOG_ERROR,"Warning: TOXE = %e is less than 1A. Recommended TOXE >= 5A",toxe);
logprint(LOG_ERROR,"\n");
}
if
((toxp<1.0e-10))
{
logprint(LOG_ERROR,"Warning: TOXP = %e is less than 1A. Recommended TOXP >= 5A",toxp);
logprint(LOG_ERROR,"\n");
}
if
((toxm==(-99.0)))
{
toxm=toxe;
}
if
((toxm<=0.0))
{
logprint(LOG_ERROR,"Fatal: TOXM = %e is not positive.",toxm);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((toxm<1.0e-10))
{
logprint(LOG_ERROR,"Warning: TOXM = %e is less than 1A. Recommended TOXM >= 5A",toxm);
logprint(LOG_ERROR,"\n");
}
if
((epsrox<=0.0))
{
logprint(LOG_ERROR,"Warning: EPSROX is not positive. Default value taken.");
logprint(LOG_ERROR,"\n");
epsrox=3.9;
}
if
((TNOM<(-273.15)))
{
logprint(LOG_ERROR,"Warning: TNOM is not physically possible. Default value taken.");
logprint(LOG_ERROR,"\n");
tnom=300.15;
}
else
{
tnom=(TNOM+273.15);
}
if
((l<=0.0))
{
logprint(LOG_ERROR,"FATAL : L is not positive.");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((w<=0.0))
{
logprint(LOG_ERROR,"FATAL : W is not positive.");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((nf<1.0))
{
logprint(LOG_ERROR,"Warning : NF must be at least equal to 1.Default value taken");
logprint(LOG_ERROR,"\n");
nf=1.0;
}
if
((phin<(-0.4)))
{
logprint(LOG_ERROR,"Fatal: phin = %e is less than -0.4.",PHIN);
logprint(LOG_ERROR,"\n");
exit(1);
}
else
{
if
((nsub<=0.0))
{
logprint(LOG_ERROR,"Fatal: NSUB = %e is not positive.",NSUB);
logprint(LOG_ERROR,"\n");
exit(1);
}
else
{
if
((NSUB<=1.0e14))
{
logprint(LOG_ERROR,"Warning: NSUB = %e may be too small.",NSUB);
logprint(LOG_ERROR,"\n");
}
else
{
if
((NSUB>=1.0e21))
{
logprint(LOG_ERROR,"Warning: NSUB = %e may be too large.",NSUB);
logprint(LOG_ERROR,"\n");
}
}
}
}
if
((xj<=0.0))
{
logprint(LOG_ERROR,"Fatal: XJ = %e is not positive.",XJ);
logprint(LOG_ERROR,"\n");
}
if
((ngate<0.0))
{
logprint(LOG_ERROR,"Fatal: NGATE = %e is not positive.",NGATE);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((ngate>1.0e25))
{
logprint(LOG_ERROR,"Fatal: NGATE = %e is too high.",NGATE);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
(((ngate>0.0)&&(ngate<=1.0e18)))
{
logprint(LOG_ERROR,"Warning: NGATE = %e is less than 1.E18cm^-3.",NGATE);
logprint(LOG_ERROR,"\n");
}
if
((poxedge<=0.0))
{
logprint(LOG_ERROR,"Fatal: POXEDGE = %e is non-positive.",POXEDGE);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((dsub<0.0))
{
logprint(LOG_ERROR,"Fatal: DSUB = %e is negative.",DSUB);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((drout<0.0))
{
logprint(LOG_ERROR,"Fatal: DROUT = %e is negative.",DROUT);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pdibl1<0.0))
{
logprint(LOG_ERROR,"Warning: PDIBL1 = %e is negative.",PDIBL1);
logprint(LOG_ERROR,"\n");
}
if
((pdibl2<0.0))
{
logprint(LOG_ERROR,"Warning: PDIBL2 = %e is negative.",PDIBL2);
logprint(LOG_ERROR,"\n");
}
if
((dvt1w<0.0))
{
logprint(LOG_ERROR,"Fatal: DVT1W = %e is negative.",DVT1W);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((dvt1<0.0))
{
logprint(LOG_ERROR,"Fatal: DVT1 = %e is negative.",DVT1);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((dvt0<0.0))
{
logprint(LOG_ERROR,"Warning: DVT0 = %e is negative.",DVT0);
logprint(LOG_ERROR,"\n");
}
if
((lpe0<(-leff)))
{
logprint(LOG_ERROR,"Fatal: LPE0 = %e is less than -leff.",LPE0);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((w0==(-weff)))
{
logprint(LOG_ERROR,"Fatal: (W0 + Weff) = 0 causing divided-by-zero.");
logprint(LOG_ERROR,"\n");
exit(1);
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
((eta0<0.0))
{
logprint(LOG_ERROR,"Warning: ETA0 = %e is negative.",ETA0);
logprint(LOG_ERROR,"\n");
}
if
((lpeb<(-leff)))
{
logprint(LOG_ERROR,"Fatal: LPEB = %e is less than -leff.",LPEB);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((nfactor<0.0))
{
logprint(LOG_ERROR,"Warning: NFACTOR = %e is negative.",NFACTOR);
logprint(LOG_ERROR,"\n");
}
if
((cdsc<0.0))
{
logprint(LOG_ERROR,"Warning: CDSC = %e is negative.",CDSC);
logprint(LOG_ERROR,"\n");
}
if
((cdscd<0.0))
{
logprint(LOG_ERROR,"Warning: CDSCD = %e is negative.",CDSCD);
logprint(LOG_ERROR,"\n");
}
if
((u0==(-99.0)))
{
u0=((type==1)?0.067:0.025);
}
if
((ua==(-99.0)))
{
ua=((mobmod==2)?1.0e-15:1.0e-9);
}
if
((uc==(-99.0)))
{
uc=((mobmod==1)?(-0.0465):(-0.0465e-9));
}
if
((uc1==(-99.0)))
{
uc1=((mobmod==1)?(-0.056):(-0.056e-9));
}
if
((eu==(-99.0)))
{
eu=((type==1)?1.67:1.0);
}
if
((prwg<0.0))
{
logprint(LOG_ERROR,"Warning: PRWG = %e is negative. Set to zero.",PRWG);
logprint(LOG_ERROR,"\n");
prwg=0.0;
}
if
((a2<0.01))
{
logprint(LOG_ERROR,"Warning: A2 = %e is too small. Set to 0.01.",a2);
logprint(LOG_ERROR,"\n");
a2=0.01;
}
else
{
if
((a2>1.0))
{
logprint(LOG_ERROR,"Warning: A2 = %e is larger than 1. A2 is set to 1 and A1 is set to 0.",a2);
logprint(LOG_ERROR,"\n");
a2=1.0;
a1=0.0;
}
}
if
((delta<0.0))
{
logprint(LOG_ERROR,"Fatal: DELTA = %e is less than zero.",delta);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
(((lambda!=(-99.0))&&(lambda>0.0)))
{
if
((lambda>1.0e-9))
{
logprint(LOG_ERROR,"Warning: LAMBDA = %e may be too large.",LAMBDA);
logprint(LOG_ERROR,"\n");
}
}
if
((fprout<0.0))
{
logprint(LOG_ERROR,"Fatal: FPROUT = %e is negative.",FPROUT);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pclm<=0.0))
{
logprint(LOG_ERROR,"Fatal: PCLM = %e is not positive.",PCLM);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pdits<0.0))
{
logprint(LOG_ERROR,"Fatal: PDITS = %e is negative.",pdits);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pditsl<0.0))
{
logprint(LOG_ERROR,"Fatal: PDITSL = %e is negative.",pditsl);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pscbe2<=0.0))
{
logprint(LOG_ERROR,"Warning: PSCBE2 = %e is not positive.",PSCBE2);
logprint(LOG_ERROR,"\n");
}
if
(((vtl!=(-99.0))&&(vtl>0.0)))
{
if
((vtl<6.0e4))
{
logprint(LOG_ERROR,"Warning: Thermal velocity VTL = %e may be too small.",vtl);
logprint(LOG_ERROR,"\n");
}
}
if
((xn<3.0))
{
logprint(LOG_ERROR,"Warning: back scattering coeff XN = %e is too small. Reset to 3.0",xn);
logprint(LOG_ERROR,"\n");
xn=3.0;
}
if
((rgatemod==1))
{
if
((rshg<=0.0))
{
logprint(LOG_ERROR,"Warning: RSHG should be positive for RGATEMOD = 1.");
logprint(LOG_ERROR,"\n");
}
}
else
{
if
((rgatemod==2))
{
if
((rshg<=0.0))
{
logprint(LOG_ERROR,"Warning: RSHG <= 0.0 for rgateMod = 2.");
logprint(LOG_ERROR,"\n");
}
else
{
if
((xrcrg1<=0.0))
{
logprint(LOG_ERROR,"Warning: XRCRG1 <= 0.0 for rgateMod = 2.");
logprint(LOG_ERROR,"\n");
}
}
}
}
if
((rgatemod==3))
{
if
((rshg<=0.0))
{
logprint(LOG_ERROR,"Warning: RSHG should be positive for RGATEMOD = 3.");
logprint(LOG_ERROR,"\n");
}
else
{
if
((xrcrg1<=0.0))
{
logprint(LOG_ERROR,"Warning: XRCRG1 should be positive for RGATEMOD = 3.");
logprint(LOG_ERROR,"\n");
}
}
}
if
((ngcon<1.0))
{
logprint(LOG_ERROR,"Fatal: The parameter NGCON cannot be smaller than one.");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
(((l+xl)<=xgl))
{
logprint(LOG_ERROR,"Fatal: The parameter XGL must be smaller than Ldrawn+XL.");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((((sa>0.0)&&(sb>0.0))&&((nf==1.0)||((nf>1.0)&&(sd>0.0)))))
{
if
((saref<=0.0))
{
logprint(LOG_ERROR,"Fatal: SAREF = %e is not positive.",saref);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((sbref<=0.0))
{
logprint(LOG_ERROR,"Fatal: SBREF = %e is not positive.",sbref);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((wlod<0.0))
{
logprint(LOG_ERROR,"Warning: WLOD = %e is less than 0.",wlod);
logprint(LOG_ERROR,"\n");
wlod=0.0;
}
if
((kvsat<(-1.0)))
{
logprint(LOG_ERROR,"Warning: KVSAT = %e is is too small; Reset to -1.0.",kvsat);
logprint(LOG_ERROR,"\n");
kvsat=(-1.0);
}
if
((kvsat>1.0))
{
logprint(LOG_ERROR,"Warning: KVSAT = %e is too big; Reset to 1.0.",kvsat);
logprint(LOG_ERROR,"\n");
kvsat=1.0;
}
if
((lodk2<=0.0))
{
logprint(LOG_ERROR,"Warning: LODK2 = %e is not positive.",lodk2);
logprint(LOG_ERROR,"\n");
}
if
((lodeta0<=0.0))
{
logprint(LOG_ERROR,"Warning: LODETA0 = %e ih not positive.",lodeta0);
logprint(LOG_ERROR,"\n");
}
}
if
((aigc==(-99.0)))
{
aigc=((type==1)?0.43:0.31);
}
if
((bigc==(-99.0)))
{
bigc=((type==1)?0.054:0.024);
}
if
((cigc==(-99.0)))
{
cigc=((type==1)?0.075:0.03);
}
if
((aigsd==(-99.0)))
{
aigsd=((type==1)?0.43:0.31);
}
if
((bigsd==(-99.0)))
{
bigsd=((type==1)?0.054:0.024);
}
if
((cigsd==(-99.0)))
{
cigsd=((type==1)?0.075:0.03);
}
if
((nigbinv<=0.0))
{
logprint(LOG_ERROR,"Fatal: NIGBINV = %e is non-positive.",nigbinv);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((nigbacc<=0.0))
{
logprint(LOG_ERROR,"Fatal: NIGBACC = %e is non-positive.",nigbacc);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((nigc<=0.0))
{
logprint(LOG_ERROR,"Fatal: NIGC = %e is non-positive.",nigc);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pigcd<=0.0))
{
logprint(LOG_ERROR,"Fatal: PIGCD = %e is non-positive.",pigcd);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((pbs<0.1))
{
pbs=0.1;
logprint(LOG_ERROR,"Given PBS is less than 0.1. PBS is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((pbsws<0.1))
{
pbsws=0.1;
logprint(LOG_ERROR,"Given PBSWS is less than 0.1. PBSWS is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((pbswgs<0.1))
{
pbswgs=0.1;
logprint(LOG_ERROR,"Given PBSWGS is less than 0.1. PBSWGS is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((pbd<0.1))
{
pbd=0.1;
logprint(LOG_ERROR,"Given PBD is less than 0.1. PBD is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((pbswd<0.1))
{
pbswd=0.1;
logprint(LOG_ERROR,"Given PBSWD is less than 0.1. PBSWD is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((pbswgd<0.1))
{
pbswgd=0.1;
logprint(LOG_ERROR,"Given PBSWGD is less than 0.1. PBSWGD is set to 0.1.");
logprint(LOG_ERROR,"\n");
}
if
((ijthdfwd<=0.0))
{
ijthdfwd=0.1;
}
if
((ijthsfwd<=0.0))
{
ijthsfwd=0.1;
}
if
((ijthdrev<=0.0))
{
ijthdrev=0.1;
logprint(LOG_ERROR,"IJTHDREV reset to %e.",ijthdrev);
logprint(LOG_ERROR,"\n");
}
if
((ijthsrev<=0.0))
{
ijthsrev=0.1;
logprint(LOG_ERROR,"IJTHSREV reset to %e.",ijthsrev);
logprint(LOG_ERROR,"\n");
}
if
(((xjbvd<=0.0)&&(diomod==2)))
{
xjbvd=1.0;
logprint(LOG_ERROR,"XJBVD reset to %e.",xjbvd);
logprint(LOG_ERROR,"\n");
}
else
{
if
(((xjbvd<0.0)&&(diomod==0)))
{
xjbvd=1.0;
logprint(LOG_ERROR,"XJBVD reset to %e.",xjbvd);
logprint(LOG_ERROR,"\n");
}
}
if
((bvd<=0.0))
{
bvd=10.0;
logprint(LOG_ERROR,"BVD reset to %e.\n",bvd);
logprint(LOG_ERROR,"\n");
}
if
(((xjbvs<=0.0)&&(diomod==2)))
{
xjbvs=1.0;
logprint(LOG_ERROR,"XJBVS reset to %e.\n",xjbvs);
logprint(LOG_ERROR,"\n");
}
else
{
if
(((xjbvs<0.0)&&(diomod==0)))
{
xjbvs=1.0;
logprint(LOG_ERROR,"XJBVS reset to %e.\n",xjbvs);
logprint(LOG_ERROR,"\n");
}
}
if
((bvs<=0.0))
{
bvs=10.0;
logprint(LOG_ERROR,"BVS reset to %g.\n",bvs);
logprint(LOG_ERROR,"\n");
}
if
((gbmin<1.0e-20))
{
logprint(LOG_ERROR,"Warning: GBMIN = %e is too small.",gbmin);
logprint(LOG_ERROR,"\n");
}
if
((clc<0.0))
{
logprint(LOG_ERROR,"Fatal: CLC = %e is negative.",CLC);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((noff<0.1))
{
logprint(LOG_ERROR,"Warning: NOFF = %e is too small.",noff);
logprint(LOG_ERROR,"\n");
}
if
((voffcv<(-0.5)))
{
logprint(LOG_ERROR,"Warning: VOFFCV = %e is too small.",voffcv);
logprint(LOG_ERROR,"\n");
}
if
((moin<5.0))
{
logprint(LOG_ERROR,"Warning: MOIN = %e is too small.",moin);
logprint(LOG_ERROR,"\n");
}
if
((moin>25.0))
{
logprint(LOG_ERROR,"Warning: MOIN = %e is too large.",moin);
logprint(LOG_ERROR,"\n");
}
if
((ckappas<0.02))
{
logprint(LOG_ERROR,"Warning: CKAPPAS = %e is too small.",ckappas);
logprint(LOG_ERROR,"\n");
ckappas=0.02;
}
if
((ckappad<0.02))
{
logprint(LOG_ERROR,"Warning: CKAPPAD = %e is too small.",ckappad);
logprint(LOG_ERROR,"\n");
ckappad=0.02;
}
coxe=((epsrox*8.85418e-12)/toxe);
coxp=((epsrox*8.85418e-12)/toxp);
if
((param_cgdo==(-99.0)))
{
if
((dlc>0.0))
{
param_cgdo=((dlc*coxe)-cgdl);
}
else
{
param_cgdo=((0.6*xj)*coxe);
}
}
if
((param_cgso==(-99.0)))
{
if
((dlc>0.0))
{
param_cgso=((dlc*coxe)-cgsl);
}
else
{
param_cgso=((0.6*xj)*coxe);
}
}
if
((cgbo==(-99.0)))
{
cgbo=((2.0*dwc)*coxe);
}
tratio=(Temp/tnom);
{
double m00_sqrt(d00_sqrt0,((1.03594e-10/(epsrox*8.85418e-12))*toxe))
factor1=d00_sqrt0;
}
vtm0=(8.617087e-5*tnom);
eg0=(1.16-(((7.02e-4*tnom)*tnom)/(tnom+1108.0)));
{
double m00_sqrt(d00_sqrt0,(tnom/300.15))
double m00_exp(d00_exp1,(21.5565981-(eg0/(2.0*vtm0))))
ni=(((1.45e10*(tnom/300.15))*d00_sqrt0)*d00_exp1);
}
vtm=(8.617087e-5*_circuit_temp);
eg=(1.16-(((7.02e-4*_circuit_temp)*_circuit_temp)/(_circuit_temp+1108.0)));
if
((_circuit_temp!=tnom))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((eg0/vtm0)-(eg/vtm));
{
double m00_logE(d00_logE0,(_circuit_temp/tnom))
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_logE0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t0+(xtis*t1));
{
double m00_exp(d00_exp0,(t2/njs))
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=d00_exp0;
}
jss_temp=(jss*t3);
jsws_temp=(jsws*t3);
jswgs_temp=(jswgs*t3);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t0+(xtid*t1));
{
double m00_exp(d00_exp0,(t2/njd))
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=d00_exp0;
}
jsd_temp=(jsd*t3);
jswd_temp=(jswd*t3);
jswgd_temp=(jswgd*t3);
}
else
{
jss_temp=jss;
jsws_temp=jsws;
jswgs_temp=jswgs;
jsd_temp=jsd;
jswd_temp=jswd;
jswgd_temp=jswgd;
}
if
((jss_temp<0.0))
{
jss_temp=0.0;
}
if
((jsws_temp<0.0))
{
jsws_temp=0.0;
}
if
((jswgs_temp<0.0))
{
jswgs_temp=0.0;
}
if
((jsd_temp<0.0))
{
jsd_temp=0.0;
}
if
((jswd_temp<0.0))
{
jswd_temp=0.0;
}
if
((jswgd_temp<0.0))
{
jswgd_temp=0.0;
}
deltemp=(_circuit_temp-tnom);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(tcj*deltemp);
if
((t0>=(-1.0)))
{
cjs_temp=(cjs*(1.0+t0));
cjd_temp=(cjd*(1.0+t0));
}
else
{
if
((cjs>0.0))
{
cjs_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJS to be negative. CJS is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
if
((cjd>0.0))
{
cjd_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJD to be negative. CJD is clamped to zero.\n");
logprint(LOG_ERROR,"\n");
}
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(tcjsw*deltemp);
if
((t0>=(-1.0)))
{
cjsws_temp=(cjsws*(1.0+t0));
cjswd_temp=(cjswd*(1.0+t0));
}
else
{
if
((cjsws>0.0))
{
cjsws_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJSWS to be negative. CJSWS is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
if
((cjswd>0.0))
{
cjswd_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJSWD to be negative. CJSWD is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(tcjswg*deltemp);
if
((t0>=(-1.0)))
{
cjswgs_temp=(cjswgs*(1.0+t0));
cjswgd_temp=(cjswgd*(1.0+t0));
}
else
{
if
((cjswgs>0.0))
{
cjswgs_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJSWGS to be negative. CJSWGS is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
if
((cjswgd>0.0))
{
cjswgd_temp=0.0;
logprint(LOG_ERROR,"Temperature effect has caused CJSWGD to be negative. CJSWGD is clamped to zero.");
logprint(LOG_ERROR,"\n");
}
}
phibs=(pbs-(tpb*deltemp));
if
((phibs<0.01))
{
phibs=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBS to be less than 0.01. PBS is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
phibd=(pbd-(tpb*deltemp));
if
((phibd<0.01))
{
phibd=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBD to be less than 0.01. PBD is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
phibsws=(pbsws-(tpbsw*deltemp));
if
((phibsws<=0.01))
{
phibsws=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBSWS to be less than 0.01. PBSWS is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
phibswd=(pbswd-(tpbsw*deltemp));
if
((phibswd<=0.01))
{
phibswd=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBSWD to be less than 0.01. PBSWD is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
phibswgs=(pbswgs-(tpbswg*deltemp));
if
((phibswgs<=0.01))
{
phibswgs=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBSWGS to be less than 0.01. PBSWGS is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
phibswgd=(pbswgd-(tpbswg*deltemp));
if
((phibswgd<=0.01))
{
phibswgd=0.01;
logprint(LOG_ERROR,"Temperature effect has caused PBSWGD to be less than 0.01. PBSWGD is clamped to 0.01.");
logprint(LOG_ERROR,"\n");
}
lnew=(l+xl);
wnew=((w/nf)+xw);
{
double m00_pow(d00_pow0,lnew,lln)
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_pow0;
}
{
double m00_pow(d00_pow0,wnew,lwn)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=(((ll/t0)+(lw/t1))+(lwl/(t0*t1)));
dl=(lint+tmp1);
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vdrainb_sourcep=0.0;
tmp2_Vbulkp_sourcep=0.0;
tmp2_Vsourceb_sourcep=0.0;
#endif
tmp2=(((llc/t0)+(lwc/t1))+(lwlc/(t0*t1)));
dlc=(dlc+tmp2);
dlcig=(dlcig+tmp2);
{
double m00_pow(d00_pow0,lnew,wln)
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=d00_pow0;
}
{
double m00_pow(d00_pow0,wnew,wwn)
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=(((wl/t2)+(ww/t3))+(wwl/(t2*t3)));
dw=(wint+tmp1);
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vdrainb_sourcep=0.0;
tmp2_Vbulkp_sourcep=0.0;
tmp2_Vsourceb_sourcep=0.0;
#endif
tmp2=(((wlc/t2)+(wwc/t3))+(wwlc/(t2*t3)));
dwj=(dwj+tmp2);
leff=(lnew-(2.0*dl));
weff=(wnew-(2.0*dw));
leffcv=(lnew-(2.0*dlc));
if
((leffcv<=0.0))
{
logprint(LOG_ERROR,"Fatal: Effective channel length for C-V <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
weffcv=(wnew-(2.0*dwc));
if
((weffcv<=0.0))
{
logprint(LOG_ERROR,"Fatal: Effective channel width for C-V <= 0");
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((leff<=1.0e-9))
{
logprint(LOG_ERROR,"Warning: leff = %e <= 1.0e-9. Recommended leff >= 1e-8.",leff);
logprint(LOG_ERROR,"\n");
}
if
((weff<=1.0e-9))
{
logprint(LOG_ERROR,"Warning: weff = %e <= 1.0e-9. Recommended weff >= 1e-7.",weff);
logprint(LOG_ERROR,"\n");
}
if
((leffcv<=1.0e-9))
{
logprint(LOG_ERROR,"warning: leff for CV = %e <= 1.0e-9. recommended leffcv >=1e-8 ",leffcv);
logprint(LOG_ERROR,"\n");
}
if
((weffcv<=1.0e-9))
{
logprint(LOG_ERROR,"warning: weff for CV = %e <= 1.0e-9. recommended weffcv >= 1e-7 ",weffcv);
logprint(LOG_ERROR,"\n");
}
weffcj=(wnew-(2.0*dwj));
if
((weffcj<=0.0))
{
logprint(LOG_ERROR,"Fatal: Effective channel width for S/D junctions <= 0.");
logprint(LOG_ERROR,"\n");
exit(1);
}
{
double m00_pow(d00_pow0,(clc/leffcv),cle)
abulkcvfactor=(1.0+d00_pow0);
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(tratio-1.0);
{
double m00_pow(d00_pow0,(weffcj*1.0e6),wr)
powweffwr=(d00_pow0*nf);
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=0.0;
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=0.0;
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=0.0;
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=0.0;
if
((tempmod==0))
{
ua=(ua+(ua1*t0));
ub=(ub+(ub1*t0));
uc=(uc+(uc1*t0));
vsattemp=(vsat-(at*t0));
#if defined(_DERIVATE)
t10_Vgatep_sourcep=0.0;
t10_Vdrainp_sourcep=0.0;
t10_Vdrainb_sourcep=0.0;
t10_Vbulkp_sourcep=0.0;
t10_Vsourceb_sourcep=0.0;
#endif
t10=(prt*t0);
if
((rdsmod==1))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(rdw+t10);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(rdwmin+t10);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(rsw+t10);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(rswmin+t10);
}
rds0=(((rdsw+t10)*nf)/powweffwr);
rdswmin=(((rdswmin+t10)*nf)/powweffwr);
}
else
{
ua=(ua*(1.0+(ua1*deltemp)));
ub=(ub*(1.0+(ub1*deltemp)));
uc=(uc*(1.0+(uc1*deltemp)));
vsattemp=(vsat*(1.0-(at*deltemp)));
#if defined(_DERIVATE)
t10_Vgatep_sourcep=0.0;
t10_Vdrainp_sourcep=0.0;
t10_Vdrainb_sourcep=0.0;
t10_Vbulkp_sourcep=0.0;
t10_Vsourceb_sourcep=0.0;
#endif
t10=(1.0+(prt*deltemp));
if
((rdsmod==1))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(rdw*t10);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(rdwmin*t10);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(rsw*t10);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(rswmin*t10);
}
rds0=(((rdsw*t10)*nf)/powweffwr);
rdswmin=(((rdswmin*t10)*nf)/powweffwr);
}
if
((t1<0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=0.0;
logprint(LOG_ERROR,"Warning: rdw at current temperature is negative; set to 0.");
logprint(LOG_ERROR,"\n");
}
if
((t2<0.0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=0.0;
logprint(LOG_ERROR,"Warning: rdwmin at current temperature is negative; set to 0.");
logprint(LOG_ERROR,"\n");
}
rd0=(t1/powweffwr);
rdwmin=(t2/powweffwr);
if
((t3<0.0))
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=0.0;
logprint(LOG_ERROR,"Warning: rsw at current temperature is negative; set to 0.");
logprint(LOG_ERROR,"\n");
}
if
((t4<0.0))
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=0.0;
logprint(LOG_ERROR,"Warning: rswmin at current temperature is negative; set to 0.");
logprint(LOG_ERROR,"\n");
}
rs0=(t3/powweffwr);
rswmin=(t4/powweffwr);
if
((u0>1.0))
{
u0=(u0/1.0e4);
}
{
double m00_pow(d00_pow0,tratio,ute)
u0temp=(u0*d00_pow0);
}
if
((u0temp<=0.0))
{
logprint(LOG_ERROR,"Fatal: U0 at current temperature = %e is not positive.",u0temp);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((eu<0.0))
{
eu=0.0;
logprint(LOG_ERROR,"Warning: EU has been negative; reset to 0.0.");
logprint(LOG_ERROR,"\n");
}
if
((vsattemp<=0.0))
{
logprint(LOG_ERROR,"Fatal: VSAT at current temperature = %e is not positive.",vsattemp);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((vsattemp<1.0e3))
{
logprint(LOG_ERROR,"Warning: VSAT at current temperature = %e may be too small.",vsattemp);
logprint(LOG_ERROR,"\n");
}
if
((rds0<0.0))
{
logprint(LOG_ERROR,"Warning: Rds at current temperature = %e is negative. Set to zero.",rds0);
logprint(LOG_ERROR,"\n");
rds0=0.0;
}
if
((rdsw<0.0))
{
logprint(LOG_ERROR,"Warning: rdsw = %e is negative. Set to zero.",rdsw);
logprint(LOG_ERROR,"\n");
#if defined(_DYNAMIC)
rdsw=0.0;
#endif
rds0=0.0;
}
if
((rdswmin<0.0))
{
logprint(LOG_ERROR,"Warning: rdswmin at current temperature = %e is negative. Set to zero.",rdswmin);
logprint(LOG_ERROR,"\n");
rdswmin=0.0;
}
if
((b1==(-weff)))
{
logprint(LOG_ERROR,"Fatal: (B1 + weff) = 0 causing divided-by-zero.");
logprint(LOG_ERROR,"\n");
exit(1);
}
{
double m00_fabs(d00_fabs0,(1.0e-6/(b1+weff)))
if
((d00_fabs0>10.0))
{
logprint(LOG_ERROR,"Warning: (B1 + weff) may be too small.");
logprint(LOG_ERROR,"\n");
}
}
if
(((vtl!=(-99.0))&&(vtl>0.0)))
{
if
((lc<0.0))
{
lc=0.0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(leff/((xn*leff)+lc));
tfactor=((1.0-t0)/(1.0+t0));
}
if
((cf==(-99.0)))
{
{
double m00_logE(d00_logE0,(1.0+(0.4e-6/toxe)))
cf=((((2.0*epsrox)*8.85418e-12)/3.141592654)*d00_logE0);
}
}
param_cgdo=((param_cgdo+cf)*weffcv);
param_cgso=((param_cgso+cf)*weffcv);
cgbo=((cgbo*leffcv)*nf);
if
(((ndep==(-99.0))&&(gamma1!=(-99.0))))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(gamma1*coxe);
ndep=((3.01248e22*t0)*t0);
}
else
{
if
(((ndep!=(-99.0))&&(gamma1==(-99.0))))
{
{
double m00_sqrt(d00_sqrt0,ndep)
gamma1=((5.753e-12*d00_sqrt0)/coxe);
}
}
else
{
if
(((ndep==(-99.0))&&(gamma1==(-99.0))))
{
ndep=1.0e17;
{
double m00_sqrt(d00_sqrt0,ndep)
gamma1=((5.753e-12*d00_sqrt0)/coxe);
}
}
}
}
if
((ndep<=0.0))
{
logprint(LOG_ERROR,"Fatal: NDEP = %e is not positive.",ndep);
logprint(LOG_ERROR,"\n");
exit(1);
}
if
((ndep<=1.0e12))
{
logprint(LOG_ERROR,"Warning: NDEP = %e may be too small.",ndep);
logprint(LOG_ERROR,"\n");
}
else
{
if
((ndep>=1.0e21))
{
logprint(LOG_ERROR,"Warning: NDEP = %e may be too large.",ndep);
logprint(LOG_ERROR,"\n");
}
}
if
((gamma2==(-99.0)))
{
{
double m00_sqrt(d00_sqrt0,nsub)
gamma2=((5.753e-12*d00_sqrt0)/coxe);
}
}
{
double m00_logE(d00_logE0,(ndep/ni))
phi=(((vtm0*d00_logE0)+phin)+0.4);
}
{
double m00_sqrt(d00_sqrt0,phi)
sqrtphi=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,((2.0*1.03594e-10)/((1.60219e-19*ndep)*1.0e6)))
xdep0=(d00_sqrt0*sqrtphi);
}
{
double m00_sqrt(d00_sqrt0,((3.0*xj)*toxe))
litl=d00_sqrt0;
}
{
double m00_logE(d00_logE0,((nsd*ndep)/(ni*ni)))
vbi=(vtm0*d00_logE0);
}
if
((ngate>0.0))
{
{
double m00_logE(d00_logE0,(ngate/nsd))
vfbsd=(vtm0*d00_logE0);
}
}
else
{
vfbsd=0.0;
}
{
double m00_sqrt(d00_sqrt0,(((((1.60219e-19*1.03594e-10)*ndep)*1.0e6)/2.0)/phi))
cdep0=d00_sqrt0;
}
{
double m00_logE(d00_logE0,(toxref/toxe))
double m00_exp(d00_exp1,(ntox*d00_logE0))
toxratio=((d00_exp1/toxe)/toxe);
}
{
double m00_logE(d00_logE0,(toxref/(toxe*poxedge)))
double m00_exp(d00_exp1,(ntox*d00_logE0))
toxratioedge=((((d00_exp1/toxe)/toxe)/poxedge)/poxedge);
}
{
double m00_atan(d00_atan0,minv)
mstar=(0.5+(d00_atan0/3.141592654));
}
voffcbn=(voff+(voffl/leff));
{
double m00_sqrt(d00_sqrt0,((1.03594e-10*vtm0)/((1.60219e-19*ndep)*1.0e6)))
ldeb=(d00_sqrt0/3.0);
}
{
double m00_pow(d00_pow0,(ndep/2.0e16),(-0.25))
acde=(acde*d00_pow0);
}
if
((capmod==2))
{
if
((acde<0.1))
{
logprint(LOG_ERROR,"Warning: ACDE = %e is too small.",acde);
logprint(LOG_ERROR,"\n");
}
if
((acde>1.6))
{
logprint(LOG_ERROR,"Warning: ACDE = %e is too large.",acde);
logprint(LOG_ERROR,"\n");
}
}
if
(((k1!=(-99.0))||(k2!=(-99.0))))
{
if
((k1==(-99.0)))
{
logprint(LOG_ERROR,"Warning: K1 should be specified with K2.");
logprint(LOG_ERROR,"\n");
k1=0.53;
}
if
((k2==(-99.0)))
{
logprint(LOG_ERROR,"Warning: K2 should be specified with K1.");
logprint(LOG_ERROR,"\n");
k2=(-0.0186);
}
}
else
{
if
((vbx==(-99.0)))
{
vbx=(phi-(((7.7348e-4*ndep)*xt)*xt));
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
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(gamma1-gamma2);
{
double m00_sqrt(d00_sqrt0,(phi-vbx))
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(d00_sqrt0-sqrtphi);
}
{
double m00_sqrt(d00_sqrt0,(phi*(phi-vbm)))
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(d00_sqrt0-phi);
}
k2=((t0*t1)/((2.0*t2)+vbm));
{
double m00_sqrt(d00_sqrt0,(phi-vbm))
k1=(gamma2-((2.0*k2)*d00_sqrt0));
}
}
if
((k2<0.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((0.5*k1)/k2);
vbsc=(0.9*(phi-(t0*t0)));
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
((vfb==(-99.0)))
{
if
((vth0!=(-99.0)))
{
vfb=(((type*vth0)-phi)-(k1*sqrtphi));
}
else
{
vfb=(-1.0);
}
}
if
((vth0==(-99.0)))
{
vth0=(type*((vfb+phi)+(k1*sqrtphi)));
}
k1ox=((k1*toxe)/toxm);
k2ox=((k2*toxe)/toxm);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(((type*vth0)-vfb)-phi);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+t3);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(2.5*t3);
vtfbphi1=((type==1)?t4:t5);
if
((vtfbphi1<0.0))
{
vtfbphi1=0.0;
}
vtfbphi2=(4.0*t3);
if
((vtfbphi2<0.0))
{
vtfbphi2=0.0;
}
{
double m00_sqrt(d00_sqrt0,(((1.03594e-10/(epsrox*8.85418e-12))*toxe)*xdep0))
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=d00_sqrt0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((dsub*leff)/tmp);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
theta0vb0=(t1/t4);
}
else
{
theta0vb0=(1.0/(5.834617425e14-2.0));
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((drout*leff)/tmp);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(t1/t4);
}
else
{
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(1.0/(5.834617425e14-2.0));
}
thetarout=((pdibl1*t5)+pdibl2);
{
double m00_sqrt(d00_sqrt0,xdep0)
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=d00_sqrt0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=(vbi-phi);
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vdrainb_sourcep=0.0;
tmp2_Vbulkp_sourcep=0.0;
tmp2_Vsourceb_sourcep=0.0;
#endif
tmp2=(factor1*tmp);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(((dvt1w*weff)*leff)/tmp2);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=0.0;
t8_Vdrainp_sourcep=0.0;
t8_Vdrainb_sourcep=0.0;
t8_Vbulkp_sourcep=0.0;
t8_Vsourceb_sourcep=0.0;
#endif
t8=(t1/t4);
}
else
{
#if defined(_DERIVATE)
t8_Vgatep_sourcep=0.0;
t8_Vdrainp_sourcep=0.0;
t8_Vdrainb_sourcep=0.0;
t8_Vbulkp_sourcep=0.0;
t8_Vsourceb_sourcep=0.0;
#endif
t8=(1.0/(5.834617425e14-2.0));
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(dvt0w*t8);
#if defined(_DERIVATE)
t8_Vgatep_sourcep=0.0;
t8_Vdrainp_sourcep=0.0;
t8_Vdrainb_sourcep=0.0;
t8_Vbulkp_sourcep=0.0;
t8_Vsourceb_sourcep=0.0;
#endif
t8=(t0*tmp1);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((dvt1*leff)/tmp2);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(t1/t4);
}
else
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(1.0/(5.834617425e14-2.0));
}
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=((dvt0*t9)*tmp1);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=((toxe*phi)/(weff+w0));
{
double m00_sqrt(d00_sqrt0,(1.0+(lpe0/leff)))
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_sqrt0;
}
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(((k1ox*(t0-1.0))*sqrtphi)+((kt1+(kt1l/leff))*(tratio-1.0)));
#if defined(_DERIVATE)
tmp3_Vgatep_sourcep=0.0;
tmp3_Vdrainp_sourcep=0.0;
tmp3_Vdrainb_sourcep=0.0;
tmp3_Vbulkp_sourcep=0.0;
tmp3_Vsourceb_sourcep=0.0;
#endif
tmp3=(((((type*vth0)-t8)-t9)+(k3*t4))+t5);
vfbzb=((tmp3-phi)-(k1*sqrtphi));
ldrn=l;
{
double m00_pow(d00_pow0,lnew,llodku0)
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_pow0;
}
w_tmp=(wnew+wlod);
{
double m00_pow(d00_pow0,w_tmp,wlodku0)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=(((lku0/t0)+(wku0/t1))+(pku0/(t0*t1)));
ku0=(1.0+tmp1);
{
double m00_pow(d00_pow0,lnew,llodvth)
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_pow0;
}
{
double m00_pow(d00_pow0,w_tmp,wlodvth)
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_pow0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=(((lkvth0/t0)+(wkvth0/t1))+(pkvth0/(t0*t1)));
kvth0=(1.0+tmp1);
{
double m00_sqrt(d00_sqrt0,((kvth0*kvth0)+1.0e-9))
kvth0=d00_sqrt0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(tratio-1.0);
ku0temp=((ku0*(1.0+(tku0*t0)))+1.0e-9);
inv_saref=(1.0/(saref+(0.5*ldrn)));
inv_sbref=(1.0/(sbref+(0.5*ldrn)));
inv_od_ref=(inv_saref+inv_sbref);
rho_ref=((KU0/ku0temp)*inv_od_ref);
if
((((sa>0.0)&&(sb>0.0))&&((nf==1.0)||((nf>1.0)&&(sd>0.0)))))
{
inv_sa=0;
inv_sb=0;
i=0;
while
((i<nf))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((1.0/nf)/((sa+(0.5*ldrn))+(i*(sd+ldrn))));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=((1.0/nf)/((sb+(0.5*ldrn))+(i*(sd+ldrn))));
inv_sa=(inv_sa+t0);
inv_sb=(inv_sb+t1);
i=(i+1);
}
inv_odeff=(inv_sa+inv_sb);
rho=((KU0/ku0temp)*inv_odeff);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((1.0+rho)/(1.0+rho_ref));
u0temp=(u0temp*t0);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=((1.0+(kvsat*rho))/(1.0+(kvsat*rho_ref)));
vsattemp=(vsattemp*t1);
od_offset=(inv_odeff-inv_od_ref);
dvth0_lod=((KVTH0/kvth0)*od_offset);
{
double m00_pow(d00_pow0,kvth0,lodk2)
dk2_lod=((stk2/d00_pow0)*od_offset);
}
{
double m00_pow(d00_pow0,kvth0,lodeta0)
deta0_lod=((steta0/d00_pow0)*od_offset);
}
vth0=(vth0+dvth0_lod);
if
(((VFB==(-99.0))&&(VTH0==(-99.0))))
{
vfb=(-1.0);
}
else
{
vfb=(vfb+(type*dvth0_lod));
}
vfbzb=(vfbzb+(type*dvth0_lod));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(((type*vth0)-vfb)-phi);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(t3+t3);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(2.5*t3);
vtfbphi1=((type==1)?t4:t5);
if
((vtfbphi1<0.0))
{
vtfbphi1=0.0;
}
vtfbphi2=(4.0*t3);
if
((vtfbphi2<0.0))
{
vtfbphi2=0.0;
}
k2=(k2+dk2_lod);
if
((k2<0.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((0.5*k1)/k2);
vbsc=(0.9*(phi-(t0*t0)));
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
k2ox=((k2*toxe)/toxm);
eta0=(eta0+deta0_lod);
}
if
((rbodymod==1.0))
{
if
((rbdb<1.0e-3))
{
grbdb=1.0e3;
}
else
{
grbdb=(gbmin+(1.0/rbdb));
}
if
((rbpb<1.0e-3))
{
grbpb=1.0e3;
}
else
{
grbpb=(gbmin+(1.0/rbpb));
}
if
((rbps<1.0e-3))
{
grbps=1.0e3;
}
else
{
grbps=(gbmin+(1.0/rbps));
}
if
((rbsb<1.0e-3))
{
grbsb=1.0e3;
}
else
{
grbsb=(gbmin+(1.0/rbsb));
}
if
((rbpd<1.0e-3))
{
grbpd=1.0e3;
}
else
{
grbpd=(gbmin+(1.0/rbpd));
}
}
grgeltd=((rshg*(xgw+((weffcj/3.0)/ngcon)))/((ngcon*nf)*(lnew-xgl)));
if
((grgeltd>0.0))
{
grgeltd=(1.0/grgeltd);
}
else
{
grgeltd=1.0e3;
if
((rgatemod!=0))
{
logprint(LOG_ERROR,"Warning: The gate conductance reset to 1.0e3 ohms.");
logprint(LOG_ERROR,"\n");
}
}
dmcgeff=(dmcg-dmcgt);
dmcieff=dmci;
dmdgeff=(dmdg-dmcgt);
if
((ps>0.0))
{
if
((permod==0))
{
pseff=ps;
}
else
{
pseff=(ps-(weffcj*nf));
}
}
else
{
pseff=bsim4v30nMOS_get_ps(nf,geomod,imin,weffcj,dmcgeff,dmcieff,dmdgeff);
}
if
((pd>0.0))
{
if
((permod==0))
{
pdeff=pd;
}
else
{
pdeff=(pd-(weffcj*nf));
}
}
else
{
pdeff=bsim4v30nMOS_get_pd(nf,geomod,imin,weffcj,dmcgeff,dmcieff,dmdgeff);
}
if
((as>0.0))
{
aseff=as;
}
else
{
aseff=bsim4v30nMOS_get_as(nf,geomod,imin,weffcj,dmcgeff,dmcieff,dmdgeff);
}
if
((ad>0.0))
{
adeff=ad;
}
else
{
adeff=bsim4v30nMOS_get_ad(nf,geomod,imin,weffcj,dmcgeff,dmcieff,dmdgeff);
}
if
((nrs!=(-99.0)))
{
gsdiff=(rsh*nrs);
}
else
{
if
((rgeomod>0))
{
gsdiff=bsim4v30nMOS_get_rtot(nf,geomod,rgeomod,imin,weffcj,rsh,dmcgeff,dmcieff,dmdgeff,1);
}
else
{
gsdiff=0.0;
}
}
if
((gsdiff>0.0))
{
gsdiff=(1.0/gsdiff);
}
else
{
gsdiff=1.0e3;
logprint(LOG_ERROR,"Warning: source conductance reset to 1.0e3 mho.");
logprint(LOG_ERROR,"\n");
}
if
((nrd!=(-99.0)))
{
gddiff=(rsh*nrd);
}
else
{
if
((rgeomod>0))
{
gddiff=bsim4v30nMOS_get_rtot(nf,geomod,rgeomod,imin,weffcj,rsh,dmcgeff,dmcieff,dmdgeff,0);
}
else
{
gddiff=0.0;
}
}
if
((gddiff>0.0))
{
gddiff=(1.0/gddiff);
}
else
{
gddiff=1.0e3;
logprint(LOG_ERROR,"Warning: drain conductance reset to 1.0e3 mho.");
logprint(LOG_ERROR,"\n");
}
aechvb=((type==1)?4.97232e-7:3.42537e-7);
bechvb=((type==1)?7.45669e11:1.16645e12);
aechvbedge=(((aechvb*weff)*dlcig)*toxratioedge);
bechvbedge=(((-bechvb)*toxe)*poxedge);
aechvb=(((aechvb*weff)*leff)*toxratio);
bechvb=(bechvb*(-toxe));
nvtms=(vtm*njs);
if
(((aseff<=0.0)&&(pseff<=0.0)))
{
isbs=1.0e-14;
}
else
{
isbs=(((aseff*jss_temp)+(pseff*jsws_temp))+((weffcj*nf)*jswgs_temp));
}
if
((isbs>0.0))
{
if
((diomod)==(0))
{
if
(((bvs/nvtms)>34.0))
{
xexpbvs=(xjbvs*1.713908431e-15);
}
else
{
{
double m00_exp(d00_exp0,((-bvs)/nvtms))
xexpbvs=(xjbvs*d00_exp0);
}
}
}
else
if
((diomod)==(1))
{
vjsmfwd=bsim4v30nMOS_get_vjm(nvtms,ijthsfwd,isbs,0.0);
{
double m00_exp(d00_exp0,(vjsmfwd/nvtms))
ivjsmfwd=(isbs*d00_exp0);
}
}
else
if
((diomod)==(2))
{
if
(((bvs/nvtms)>34.0))
{
xexpbvs=(xjbvs*1.713908431e-15);
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,((-bvs)/nvtms))
xexpbvs=d00_exp0;
}
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=xexpbvs;
xexpbvs=(xexpbvs*xjbvs);
}
vjsmfwd=bsim4v30nMOS_get_vjm(nvtms,ijthsfwd,isbs,xexpbvs);
{
double m00_exp(d00_exp0,(vjsmfwd/nvtms))
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_exp0;
}
ivjsmfwd=(isbs*(((t0-(xexpbvs/t0))+xexpbvs)-1.0));
sslpfwd=((isbs*(t0+(xexpbvs/t0)))/nvtms);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(ijthsrev/isbs);
if
((t2<1.0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=10.0;
logprint(LOG_ERROR,"Warning: ijthsrev too small and set to 10 times isbsat.\n");
logprint(LOG_ERROR,"\n");
}
{
double m00_logE(d00_logE0,((t2-1.0)/xjbvs))
vjsmrev=((-bvs)-(nvtms*d00_logE0));
}
{
double m00_exp(d00_exp0,((-(bvs+vjsmrev))/nvtms))
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(xjbvs*d00_exp0);
}
ivjsmrev=(isbs*(1.0+t1));
sslprev=(((-isbs)*t1)/nvtms);
}
else
logprint(LOG_ERROR,"Specified diomod = %d not matched",diomod);
logprint(LOG_ERROR,"\n");
}
nvtmd=(vtm*njd);
if
(((adeff<=0.0)&&(pdeff<=0.0)))
{
isbd=1.0e-14;
}
else
{
isbd=(((adeff*jsd_temp)+(pdeff*jswd_temp))+((weffcj*nf)*jswgd_temp));
}
if
((isbd>0.0))
{
if
((diomod)==(0))
{
if
(((bvd/nvtmd)>34.0))
{
xexpbvd=(xjbvd*1.713908431e-15);
}
else
{
{
double m00_exp(d00_exp0,((-bvd)/nvtmd))
xexpbvd=(xjbvd*d00_exp0);
}
}
}
else
if
((diomod)==(1))
{
vjdmfwd=bsim4v30nMOS_get_vjm(nvtmd,ijthdfwd,isbd,0.0);
{
double m00_exp(d00_exp0,(vjdmfwd/nvtmd))
ivjdmfwd=(isbd*d00_exp0);
}
}
else
if
((diomod)==(2))
{
if
(((bvd/nvtmd)>34.0))
{
xexpbvd=(xjbvd*1.713908431e-15);
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,((-bvd)/nvtmd))
xexpbvd=d00_exp0;
}
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=xexpbvd;
xexpbvd=(xexpbvd*xjbvd);
}
vjsmfwd=bsim4v30nMOS_get_vjm(nvtmd,ijthdfwd,isbd,xexpbvd);
{
double m00_exp(d00_exp0,(vjdmfwd/nvtmd))
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_exp0;
}
ivjdmfwd=(isbd*(((t0-(xexpbvd/t0))+xexpbvd)-1.0));
dslpfwd=((isbd*(t0+(xexpbvd/t0)))/nvtmd);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(ijthdrev/isbd);
if
((t2<1.0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=10.0;
logprint(LOG_ERROR,"Warning: ijthdrev too small and set to 10 times idbsat.\n");
logprint(LOG_ERROR,"\n");
}
{
double m00_logE(d00_logE0,((t2-1.0)/xjbvd))
vjdmrev=((-bvd)-(nvtmd*d00_logE0));
}
{
double m00_exp(d00_exp0,((-(bvd+vjdmrev))/nvtmd))
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(xjbvd*d00_exp0);
}
ivjdmrev=(isbd*(1.0+t1));
dslprev=(((-isbd)*t1)/nvtmd);
}
else
logprint(LOG_ERROR,"Specified diomod = %d not matched",diomod);
logprint(LOG_ERROR,"\n");
}
}
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void bsim4v30nMOS::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void bsim4v30nMOS::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void bsim4v30nMOS::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void bsim4v30nMOS::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double leffx2;
double fourkt;
#if defined(_DYNAMIC)
double qb;
#if defined(_DERIVATE)
double qb_Vgatem_sourcep;
double qb_Vgatep_sourcep;
double qb_Vdrainp_sourcep;
double qb_Vdrainb_sourcep;
double qb_Vbulkp_sourcep;
double qb_Vsourceb_sourcep;
#endif
#endif
double qs;
#if defined(_DERIVATE)
double qs_Vgatem_sourcep;
double qs_Vgatep_sourcep;
double qs_Vdrainp_sourcep;
double qs_Vdrainb_sourcep;
double qs_Vbulkp_sourcep;
double qs_Vsourceb_sourcep;
#endif
double qd;
#if defined(_DERIVATE)
double qd_Vgatem_sourcep;
double qd_Vgatep_sourcep;
double qd_Vdrainp_sourcep;
double qd_Vdrainb_sourcep;
double qd_Vbulkp_sourcep;
double qd_Vsourceb_sourcep;
#endif
double qgb;
#if defined(_DERIVATE)
double qgb_Vgatep_sourcep;
double qgb_Vbulkp_sourcep;
#endif
double qgmb;
#if defined(_DERIVATE)
double qgmb_Vgatem_sourcep;
double qgmb_Vbulkp_sourcep;
#endif
double qgso;
#if defined(_DERIVATE)
double qgso_Vdrainp_sourcep;
double qgso_Vdrainb_sourcep;
double qgso_Vbulkp_sourcep;
double qgso_Vsourceb_sourcep;
double qgso_Vgatep_sourcep;
#endif
double qgdo;
#if defined(_DERIVATE)
double qgdo_Vdrainb_sourcep;
double qgdo_Vbulkp_sourcep;
double qgdo_Vsourceb_sourcep;
double qgdo_Vgatep_sourcep;
double qgdo_Vdrainp_sourcep;
#endif
double cgso;
double cgdo;
double qbd;
#if defined(_DERIVATE)
double qbd_Vgatep_sourcep;
double qbd_Vdrainp_sourcep;
double qbd_Vdrainb_sourcep;
double qbd_Vbulkp_sourcep;
double qbd_Vsourceb_sourcep;
#endif
double sarg;
#if defined(_DERIVATE)
double sarg_Vdrainp_sourcep;
double sarg_Vdrainb_sourcep;
double sarg_Vbulkp_sourcep;
double sarg_Vsourceb_sourcep;
#endif
double arg;
#if defined(_DERIVATE)
double arg_Vdrainp_sourcep;
double arg_Vdrainb_sourcep;
double arg_Vbulkp_sourcep;
double arg_Vsourceb_sourcep;
#endif
double qbs;
#if defined(_DERIVATE)
double qbs_Vgatep_sourcep;
double qbs_Vdrainp_sourcep;
double qbs_Vdrainb_sourcep;
double qbs_Vbulkp_sourcep;
double qbs_Vsourceb_sourcep;
#endif
double czbsswg;
double czbssw;
double czbdswg;
double czbdsw;
double czbs;
double czbd;
double deltaphi;
#if defined(_DERIVATE)
double deltaphi_Vgatep_sourcep;
double deltaphi_Vdrainp_sourcep;
double deltaphi_Vdrainb_sourcep;
double deltaphi_Vbulkp_sourcep;
double deltaphi_Vsourceb_sourcep;
#endif
double coxwlcen;
#if defined(_DERIVATE)
double coxwlcen_Vgatep_sourcep;
double coxwlcen_Vdrainp_sourcep;
double coxwlcen_Vdrainb_sourcep;
double coxwlcen_Vbulkp_sourcep;
double coxwlcen_Vsourceb_sourcep;
#endif
double ccen;
#if defined(_DERIVATE)
double ccen_Vgatep_sourcep;
double ccen_Vdrainp_sourcep;
double ccen_Vdrainb_sourcep;
double ccen_Vbulkp_sourcep;
double ccen_Vsourceb_sourcep;
#endif
double v4;
#if defined(_DERIVATE)
double v4_Vgatep_sourcep;
double v4_Vdrainp_sourcep;
double v4_Vdrainb_sourcep;
double v4_Vbulkp_sourcep;
double v4_Vsourceb_sourcep;
#endif
double link;
double tox;
double vdseffcv;
#if defined(_DERIVATE)
double vdseffcv_Vgatep_sourcep;
double vdseffcv_Vdrainp_sourcep;
double vdseffcv_Vdrainb_sourcep;
double vdseffcv_Vbulkp_sourcep;
double vdseffcv_Vsourceb_sourcep;
#endif
double vdsatcv;
#if defined(_DERIVATE)
double vdsatcv_Vgatep_sourcep;
double vdsatcv_Vdrainp_sourcep;
double vdsatcv_Vdrainb_sourcep;
double vdsatcv_Vbulkp_sourcep;
double vdsatcv_Vsourceb_sourcep;
#endif
double qsub0;
#if defined(_DERIVATE)
double qsub0_Vgatep_sourcep;
double qsub0_Vdrainp_sourcep;
double qsub0_Vdrainb_sourcep;
double qsub0_Vbulkp_sourcep;
double qsub0_Vsourceb_sourcep;
#endif
double qac0;
#if defined(_DERIVATE)
double qac0_Vgatep_sourcep;
double qac0_Vdrainp_sourcep;
double qac0_Vdrainb_sourcep;
double qac0_Vbulkp_sourcep;
double qac0_Vsourceb_sourcep;
#endif
double vgstnvt;
#if defined(_DERIVATE)
double vgstnvt_Vgatep_sourcep;
double vgstnvt_Vdrainp_sourcep;
double vgstnvt_Vdrainb_sourcep;
double vgstnvt_Vbulkp_sourcep;
double vgstnvt_Vsourceb_sourcep;
#endif
double vbseffcv;
#if defined(_DERIVATE)
double vbseffcv_Vdrainp_sourcep;
double vbseffcv_Vdrainb_sourcep;
double vbseffcv_Vbulkp_sourcep;
double vbseffcv_Vsourceb_sourcep;
#endif
double alphaz;
#if defined(_DERIVATE)
double alphaz_Vgatep_sourcep;
double alphaz_Vdrainp_sourcep;
double alphaz_Vdrainb_sourcep;
double alphaz_Vbulkp_sourcep;
double alphaz_Vsourceb_sourcep;
#endif
double abulkcv;
#if defined(_DERIVATE)
double abulkcv_Vgatep_sourcep;
double abulkcv_Vdrainp_sourcep;
double abulkcv_Vdrainb_sourcep;
double abulkcv_Vbulkp_sourcep;
double abulkcv_Vsourceb_sourcep;
#endif
double two_third_coxwl;
double arg1;
#if defined(_DERIVATE)
double arg1_Vgatep_sourcep;
double arg1_Vdrainp_sourcep;
double arg1_Vdrainb_sourcep;
double arg1_Vbulkp_sourcep;
double arg1_Vsourceb_sourcep;
#endif
double coxwl;
double qgmid;
#if defined(_DERIVATE)
double qgmid_Vdrainb_sourcep;
double qgmid_Vbulkp_sourcep;
double qgmid_Vsourceb_sourcep;
double qgmid_Vgatep_sourcep;
double qgmid_Vdrainp_sourcep;
double qgmid_Vgatem_sourcep;
#endif
double qbulk;
#if defined(_DERIVATE)
double qbulk_Vgatem_sourcep;
double qbulk_Vgatep_sourcep;
double qbulk_Vdrainp_sourcep;
double qbulk_Vdrainb_sourcep;
double qbulk_Vbulkp_sourcep;
double qbulk_Vsourceb_sourcep;
#endif
double qsrc;
#if defined(_DERIVATE)
double qsrc_Vgatem_sourcep;
double qsrc_Vgatep_sourcep;
double qsrc_Vdrainp_sourcep;
double qsrc_Vdrainb_sourcep;
double qsrc_Vbulkp_sourcep;
double qsrc_Vsourceb_sourcep;
#endif
double qdrn;
#if defined(_DERIVATE)
double qdrn_Vgatem_sourcep;
double qdrn_Vgatep_sourcep;
double qdrn_Vdrainp_sourcep;
double qdrn_Vdrainb_sourcep;
double qdrn_Vbulkp_sourcep;
double qdrn_Vsourceb_sourcep;
#endif
double qgate;
#if defined(_DERIVATE)
double qgate_Vgatep_sourcep;
double qgate_Vdrainp_sourcep;
double qgate_Vdrainb_sourcep;
double qgate_Vbulkp_sourcep;
double qgate_Vsourceb_sourcep;
#endif
double ccn;
double igb;
#if defined(_DERIVATE)
double igb_Vgatep_sourcep;
double igb_Vdrainp_sourcep;
double igb_Vdrainb_sourcep;
double igb_Vbulkp_sourcep;
double igb_Vsourceb_sourcep;
#endif
double igbinv;
#if defined(_DERIVATE)
double igbinv_Vgatep_sourcep;
double igbinv_Vdrainp_sourcep;
double igbinv_Vdrainb_sourcep;
double igbinv_Vbulkp_sourcep;
double igbinv_Vsourceb_sourcep;
#endif
double igbacc;
#if defined(_DERIVATE)
double igbacc_Vgatep_sourcep;
double igbacc_Vdrainp_sourcep;
double igbacc_Vdrainb_sourcep;
double igbacc_Vbulkp_sourcep;
double igbacc_Vsourceb_sourcep;
#endif
double igd;
#if defined(_DERIVATE)
double igd_Vgatep_sourcep;
double igd_Vdrainp_sourcep;
double igd_Vdrainb_sourcep;
double igd_Vbulkp_sourcep;
double igd_Vsourceb_sourcep;
#endif
double igs;
#if defined(_DERIVATE)
double igs_Vgatep_sourcep;
double igs_Vdrainp_sourcep;
double igs_Vdrainb_sourcep;
double igs_Vbulkp_sourcep;
double igs_Vsourceb_sourcep;
#endif
double igcd;
#if defined(_DERIVATE)
double igcd_Vgatep_sourcep;
double igcd_Vdrainp_sourcep;
double igcd_Vdrainb_sourcep;
double igcd_Vbulkp_sourcep;
double igcd_Vsourceb_sourcep;
#endif
double igcs;
#if defined(_DERIVATE)
double igcs_Vgatep_sourcep;
double igcs_Vdrainp_sourcep;
double igcs_Vdrainb_sourcep;
double igcs_Vbulkp_sourcep;
double igcs_Vsourceb_sourcep;
#endif
double t13;
#if defined(_DERIVATE)
double t13_Vgatep_sourcep;
double t13_Vdrainp_sourcep;
double t13_Vdrainb_sourcep;
double t13_Vbulkp_sourcep;
double t13_Vsourceb_sourcep;
#endif
double modif_pigcd;
#if defined(_DERIVATE)
double modif_pigcd_Vgatep_sourcep;
double modif_pigcd_Vdrainp_sourcep;
double modif_pigcd_Vdrainb_sourcep;
double modif_pigcd_Vbulkp_sourcep;
double modif_pigcd_Vsourceb_sourcep;
#endif
double igc;
#if defined(_DERIVATE)
double igc_Vgatep_sourcep;
double igc_Vdrainp_sourcep;
double igc_Vdrainb_sourcep;
double igc_Vbulkp_sourcep;
double igc_Vsourceb_sourcep;
#endif
double expvxnvt;
#if defined(_DERIVATE)
double expvxnvt_Vgatep_sourcep;
double expvxnvt_Vdrainp_sourcep;
double expvxnvt_Vdrainb_sourcep;
double expvxnvt_Vbulkp_sourcep;
double expvxnvt_Vsourceb_sourcep;
#endif
double vaux;
#if defined(_DERIVATE)
double vaux_Vgatep_sourcep;
double vaux_Vdrainp_sourcep;
double vaux_Vdrainb_sourcep;
double vaux_Vbulkp_sourcep;
double vaux_Vsourceb_sourcep;
#endif
double vxnvt;
#if defined(_DERIVATE)
double vxnvt_Vgatep_sourcep;
double vxnvt_Vdrainp_sourcep;
double vxnvt_Vdrainb_sourcep;
double vxnvt_Vbulkp_sourcep;
double vxnvt_Vsourceb_sourcep;
#endif
double voxdepinv;
#if defined(_DERIVATE)
double voxdepinv_Vgatep_sourcep;
double voxdepinv_Vdrainp_sourcep;
double voxdepinv_Vdrainb_sourcep;
double voxdepinv_Vbulkp_sourcep;
double voxdepinv_Vsourceb_sourcep;
#endif
double voxacc;
#if defined(_DERIVATE)
double voxacc_Vgatep_sourcep;
double voxacc_Vdrainp_sourcep;
double voxacc_Vdrainb_sourcep;
double voxacc_Vbulkp_sourcep;
double voxacc_Vsourceb_sourcep;
#endif
double vfbeff;
#if defined(_DERIVATE)
double vfbeff_Vgatep_sourcep;
double vfbeff_Vdrainp_sourcep;
double vfbeff_Vdrainb_sourcep;
double vfbeff_Vbulkp_sourcep;
double vfbeff_Vsourceb_sourcep;
#endif
double v3;
#if defined(_DERIVATE)
double v3_Vgatep_sourcep;
double v3_Vdrainp_sourcep;
double v3_Vdrainb_sourcep;
double v3_Vbulkp_sourcep;
double v3_Vsourceb_sourcep;
#endif
double igisl;
#if defined(_DERIVATE)
double igisl_Vgatep_sourcep;
double igisl_Vdrainp_sourcep;
double igisl_Vdrainb_sourcep;
double igisl_Vbulkp_sourcep;
double igisl_Vsourceb_sourcep;
#endif
double igidl;
#if defined(_DERIVATE)
double igidl_Vgatep_sourcep;
double igidl_Vdrainp_sourcep;
double igidl_Vdrainb_sourcep;
double igidl_Vbulkp_sourcep;
double igidl_Vsourceb_sourcep;
#endif
double gdtot;
#if defined(_DERIVATE)
double gdtot_Vgatep_sourcep;
double gdtot_Vdrainp_sourcep;
double gdtot_Vdrainb_sourcep;
double gdtot_Vbulkp_sourcep;
double gdtot_Vsourceb_sourcep;
#endif
double rd;
#if defined(_DERIVATE)
double rd_Vgatep_sourcep;
double rd_Vdrainp_sourcep;
double rd_Vdrainb_sourcep;
double rd_Vbulkp_sourcep;
double rd_Vsourceb_sourcep;
#endif
double gstot;
#if defined(_DERIVATE)
double gstot_Vgatep_sourcep;
double gstot_Vdrainp_sourcep;
double gstot_Vdrainb_sourcep;
double gstot_Vbulkp_sourcep;
double gstot_Vsourceb_sourcep;
#endif
double rs;
#if defined(_DERIVATE)
double rs_Vgatep_sourcep;
double rs_Vdrainp_sourcep;
double rs_Vdrainb_sourcep;
double rs_Vbulkp_sourcep;
double rs_Vsourceb_sourcep;
#endif
double gcrg;
#if defined(_DERIVATE)
double gcrg_Vgatep_sourcep;
double gcrg_Vdrainp_sourcep;
double gcrg_Vdrainb_sourcep;
double gcrg_Vbulkp_sourcep;
double gcrg_Vsourceb_sourcep;
#endif
double fsevl;
#if defined(_DERIVATE)
double fsevl_Vgatep_sourcep;
double fsevl_Vdrainp_sourcep;
double fsevl_Vdrainb_sourcep;
double fsevl_Vbulkp_sourcep;
double fsevl_Vsourceb_sourcep;
#endif
double vs;
#if defined(_DERIVATE)
double vs_Vgatep_sourcep;
double vs_Vdrainp_sourcep;
double vs_Vdrainb_sourcep;
double vs_Vbulkp_sourcep;
double vs_Vsourceb_sourcep;
#endif
double t11;
#if defined(_DERIVATE)
double t11_Vgatep_sourcep;
double t11_Vdrainp_sourcep;
double t11_Vdrainb_sourcep;
double t11_Vbulkp_sourcep;
double t11_Vsourceb_sourcep;
#endif
double t12;
#if defined(_DERIVATE)
double t12_Vgatep_sourcep;
double t12_Vdrainp_sourcep;
double t12_Vdrainb_sourcep;
double t12_Vbulkp_sourcep;
double t12_Vsourceb_sourcep;
#endif
double cdrain;
#if defined(_DERIVATE)
double cdrain_Vgatep_sourcep;
double cdrain_Vdrainp_sourcep;
double cdrain_Vdrainb_sourcep;
double cdrain_Vbulkp_sourcep;
double cdrain_Vsourceb_sourcep;
#endif
double ids;
#if defined(_DERIVATE)
double ids_Vgatep_sourcep;
double ids_Vdrainp_sourcep;
double ids_Vdrainb_sourcep;
double ids_Vbulkp_sourcep;
double ids_Vsourceb_sourcep;
#endif
double csub;
#if defined(_DERIVATE)
double csub_Vgatep_sourcep;
double csub_Vdrainp_sourcep;
double csub_Vdrainb_sourcep;
double csub_Vbulkp_sourcep;
double csub_Vsourceb_sourcep;
#endif
double isub;
#if defined(_DERIVATE)
double isub_Vgatep_sourcep;
double isub_Vdrainp_sourcep;
double isub_Vdrainb_sourcep;
double isub_Vbulkp_sourcep;
double isub_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double tmp_Vgatep_sourcep;
double tmp_Vdrainp_sourcep;
double tmp_Vdrainb_sourcep;
double tmp_Vbulkp_sourcep;
double tmp_Vsourceb_sourcep;
#endif
double idsa;
#if defined(_DERIVATE)
double idsa_Vgatep_sourcep;
double idsa_Vdrainp_sourcep;
double idsa_Vdrainb_sourcep;
double idsa_Vbulkp_sourcep;
double idsa_Vsourceb_sourcep;
#endif
double vascbe;
#if defined(_DERIVATE)
double vascbe_Vgatep_sourcep;
double vascbe_Vdrainp_sourcep;
double vascbe_Vdrainb_sourcep;
double vascbe_Vbulkp_sourcep;
double vascbe_Vsourceb_sourcep;
#endif
double vadits;
#if defined(_DERIVATE)
double vadits_Vgatep_sourcep;
double vadits_Vdrainp_sourcep;
double vadits_Vdrainb_sourcep;
double vadits_Vbulkp_sourcep;
double vadits_Vsourceb_sourcep;
#endif
double vadibl;
#if defined(_DERIVATE)
double vadibl_Vgatep_sourcep;
double vadibl_Vdrainp_sourcep;
double vadibl_Vdrainb_sourcep;
double vadibl_Vbulkp_sourcep;
double vadibl_Vsourceb_sourcep;
#endif
double vaclm;
#if defined(_DERIVATE)
double vaclm_Vgatep_sourcep;
double vaclm_Vdrainp_sourcep;
double vaclm_Vdrainb_sourcep;
double vaclm_Vbulkp_sourcep;
double vaclm_Vsourceb_sourcep;
#endif
double cclm;
#if defined(_DERIVATE)
double cclm_Vgatep_sourcep;
double cclm_Vdrainp_sourcep;
double cclm_Vdrainb_sourcep;
double cclm_Vbulkp_sourcep;
double cclm_Vsourceb_sourcep;
#endif
double pvagterm;
#if defined(_DERIVATE)
double pvagterm_Vgatep_sourcep;
double pvagterm_Vdrainp_sourcep;
double pvagterm_Vdrainb_sourcep;
double pvagterm_Vbulkp_sourcep;
double pvagterm_Vsourceb_sourcep;
#endif
double fp;
#if defined(_DERIVATE)
double fp_Vgatep_sourcep;
double fp_Vdrainp_sourcep;
double fp_Vdrainb_sourcep;
double fp_Vbulkp_sourcep;
double fp_Vsourceb_sourcep;
#endif
double idl;
#if defined(_DERIVATE)
double idl_Vgatep_sourcep;
double idl_Vdrainp_sourcep;
double idl_Vdrainb_sourcep;
double idl_Vbulkp_sourcep;
double idl_Vsourceb_sourcep;
#endif
double gche;
#if defined(_DERIVATE)
double gche_Vgatep_sourcep;
double gche_Vdrainp_sourcep;
double gche_Vdrainb_sourcep;
double gche_Vbulkp_sourcep;
double gche_Vsourceb_sourcep;
#endif
double fgche2;
#if defined(_DERIVATE)
double fgche2_Vgatep_sourcep;
double fgche2_Vdrainp_sourcep;
double fgche2_Vdrainb_sourcep;
double fgche2_Vbulkp_sourcep;
double fgche2_Vsourceb_sourcep;
#endif
double fgche1;
#if defined(_DERIVATE)
double fgche1_Vgatep_sourcep;
double fgche1_Vdrainp_sourcep;
double fgche1_Vdrainb_sourcep;
double fgche1_Vbulkp_sourcep;
double fgche1_Vsourceb_sourcep;
#endif
double abovvgst2vtm;
#if defined(_DERIVATE)
double abovvgst2vtm_Vgatep_sourcep;
double abovvgst2vtm_Vdrainp_sourcep;
double abovvgst2vtm_Vdrainb_sourcep;
double abovvgst2vtm_Vbulkp_sourcep;
double abovvgst2vtm_Vsourceb_sourcep;
#endif
double beta;
#if defined(_DERIVATE)
double beta_Vgatep_sourcep;
double beta_Vdrainp_sourcep;
double beta_Vdrainb_sourcep;
double beta_Vbulkp_sourcep;
double beta_Vsourceb_sourcep;
#endif
double coxeffwovl;
#if defined(_DERIVATE)
double coxeffwovl_Vgatep_sourcep;
double coxeffwovl_Vdrainp_sourcep;
double coxeffwovl_Vdrainb_sourcep;
double coxeffwovl_Vbulkp_sourcep;
double coxeffwovl_Vsourceb_sourcep;
#endif
double coxeff;
#if defined(_DERIVATE)
double coxeff_Vgatep_sourcep;
double coxeff_Vdrainp_sourcep;
double coxeff_Vdrainb_sourcep;
double coxeff_Vbulkp_sourcep;
double coxeff_Vsourceb_sourcep;
#endif
double tcen;
#if defined(_DERIVATE)
double tcen_Vgatep_sourcep;
double tcen_Vdrainp_sourcep;
double tcen_Vdrainb_sourcep;
double tcen_Vbulkp_sourcep;
double tcen_Vsourceb_sourcep;
#endif
double dt0_dvg;
#if defined(_DERIVATE)
double dt0_dvg_Vgatep_sourcep;
double dt0_dvg_Vdrainp_sourcep;
double dt0_dvg_Vdrainb_sourcep;
double dt0_dvg_Vbulkp_sourcep;
double dt0_dvg_Vsourceb_sourcep;
#endif
double vasat;
#if defined(_DERIVATE)
double vasat_Vgatep_sourcep;
double vasat_Vdrainp_sourcep;
double vasat_Vdrainb_sourcep;
double vasat_Vbulkp_sourcep;
double vasat_Vsourceb_sourcep;
#endif
double diffvds;
#if defined(_DERIVATE)
double diffvds_Vdrainp_sourcep;
double diffvds_Vgatep_sourcep;
double diffvds_Vdrainb_sourcep;
double diffvds_Vbulkp_sourcep;
double diffvds_Vsourceb_sourcep;
#endif
double vdseff;
#if defined(_DERIVATE)
double vdseff_Vgatep_sourcep;
double vdseff_Vdrainp_sourcep;
double vdseff_Vdrainb_sourcep;
double vdseff_Vbulkp_sourcep;
double vdseff_Vsourceb_sourcep;
#endif
double vdsat;
#if defined(_DERIVATE)
double vdsat_Vgatep_sourcep;
double vdsat_Vdrainp_sourcep;
double vdsat_Vdrainb_sourcep;
double vdsat_Vbulkp_sourcep;
double vdsat_Vsourceb_sourcep;
#endif
double vgst2vtm;
#if defined(_DERIVATE)
double vgst2vtm_Vgatep_sourcep;
double vgst2vtm_Vdrainp_sourcep;
double vgst2vtm_Vdrainb_sourcep;
double vgst2vtm_Vbulkp_sourcep;
double vgst2vtm_Vsourceb_sourcep;
#endif
double Lambda;
#if defined(_DERIVATE)
double Lambda_Vgatep_sourcep;
double Lambda_Vdrainp_sourcep;
double Lambda_Vdrainb_sourcep;
double Lambda_Vbulkp_sourcep;
double Lambda_Vsourceb_sourcep;
#endif
double esatl;
#if defined(_DERIVATE)
double esatl_Vgatep_sourcep;
double esatl_Vdrainp_sourcep;
double esatl_Vdrainb_sourcep;
double esatl_Vbulkp_sourcep;
double esatl_Vsourceb_sourcep;
#endif
double esat;
#if defined(_DERIVATE)
double esat_Vgatep_sourcep;
double esat_Vdrainp_sourcep;
double esat_Vdrainb_sourcep;
double esat_Vbulkp_sourcep;
double esat_Vsourceb_sourcep;
#endif
double wvcoxrds;
#if defined(_DERIVATE)
double wvcoxrds_Vgatep_sourcep;
double wvcoxrds_Vdrainp_sourcep;
double wvcoxrds_Vdrainb_sourcep;
double wvcoxrds_Vbulkp_sourcep;
double wvcoxrds_Vsourceb_sourcep;
#endif
double wvcox;
#if defined(_DERIVATE)
double wvcox_Vgatep_sourcep;
double wvcox_Vdrainp_sourcep;
double wvcox_Vdrainb_sourcep;
double wvcox_Vbulkp_sourcep;
double wvcox_Vsourceb_sourcep;
#endif
double ueff;
#if defined(_DERIVATE)
double ueff_Vgatep_sourcep;
double ueff_Vdrainp_sourcep;
double ueff_Vdrainb_sourcep;
double ueff_Vbulkp_sourcep;
double ueff_Vsourceb_sourcep;
#endif
double denomi;
#if defined(_DERIVATE)
double denomi_Vgatep_sourcep;
double denomi_Vdrainp_sourcep;
double denomi_Vdrainb_sourcep;
double denomi_Vbulkp_sourcep;
double denomi_Vsourceb_sourcep;
#endif
double abulk;
#if defined(_DERIVATE)
double abulk_Vgatep_sourcep;
double abulk_Vdrainp_sourcep;
double abulk_Vdrainb_sourcep;
double abulk_Vbulkp_sourcep;
double abulk_Vsourceb_sourcep;
#endif
double dabulk_dvg;
#if defined(_DERIVATE)
double dabulk_dvg_Vgatep_sourcep;
double dabulk_dvg_Vdrainp_sourcep;
double dabulk_dvg_Vdrainb_sourcep;
double dabulk_dvg_Vbulkp_sourcep;
double dabulk_dvg_Vsourceb_sourcep;
#endif
double abulk0;
#if defined(_DERIVATE)
double abulk0_Vgatep_sourcep;
double abulk0_Vdrainp_sourcep;
double abulk0_Vdrainb_sourcep;
double abulk0_Vbulkp_sourcep;
double abulk0_Vsourceb_sourcep;
#endif
double rds;
#if defined(_DERIVATE)
double rds_Vgatep_sourcep;
double rds_Vdrainp_sourcep;
double rds_Vdrainb_sourcep;
double rds_Vbulkp_sourcep;
double rds_Vsourceb_sourcep;
#endif
double Weff;
#if defined(_DERIVATE)
double Weff_Vgatep_sourcep;
double Weff_Vdrainp_sourcep;
double Weff_Vdrainb_sourcep;
double Weff_Vbulkp_sourcep;
double Weff_Vsourceb_sourcep;
#endif
double vgsteff;
#if defined(_DERIVATE)
double vgsteff_Vgatep_sourcep;
double vgsteff_Vdrainp_sourcep;
double vgsteff_Vdrainb_sourcep;
double vgsteff_Vbulkp_sourcep;
double vgsteff_Vsourceb_sourcep;
#endif
double expvgst;
#if defined(_DERIVATE)
double expvgst_Vgatep_sourcep;
double expvgst_Vdrainp_sourcep;
double expvgst_Vdrainb_sourcep;
double expvgst_Vbulkp_sourcep;
double expvgst_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t10_Vgatep_sourcep;
double t10_Vdrainp_sourcep;
double t10_Vdrainb_sourcep;
double t10_Vbulkp_sourcep;
double t10_Vsourceb_sourcep;
#endif
double vgst;
#if defined(_DERIVATE)
double vgst_Vgatep_sourcep;
double vgst_Vdrainp_sourcep;
double vgst_Vdrainb_sourcep;
double vgst_Vbulkp_sourcep;
double vgst_Vsourceb_sourcep;
#endif
double Vgs_eff;
#if defined(_DERIVATE)
double Vgs_eff_Vgatep_sourcep;
double Vgs_eff_Vdrainp_sourcep;
double Vgs_eff_Vdrainb_sourcep;
double Vgs_eff_Vbulkp_sourcep;
double Vgs_eff_Vsourceb_sourcep;
#endif
double vgd_eff;
#if defined(_DERIVATE)
double vgd_eff_Vgatep_sourcep;
double vgd_eff_Vdrainp_sourcep;
double vgd_eff_Vdrainb_sourcep;
double vgd_eff_Vbulkp_sourcep;
double vgd_eff_Vsourceb_sourcep;
#endif
double vgs_eff;
#if defined(_DERIVATE)
double vgs_eff_Vgatep_sourcep;
double vgs_eff_Vdrainp_sourcep;
double vgs_eff_Vdrainb_sourcep;
double vgs_eff_Vbulkp_sourcep;
double vgs_eff_Vsourceb_sourcep;
#endif
double t6;
#if defined(_DERIVATE)
double t6_Vgatep_sourcep;
double t6_Vdrainp_sourcep;
double t6_Vdrainb_sourcep;
double t6_Vbulkp_sourcep;
double t6_Vsourceb_sourcep;
#endif
double t7;
#if defined(_DERIVATE)
double t7_Vgatep_sourcep;
double t7_Vdrainp_sourcep;
double t7_Vdrainb_sourcep;
double t7_Vbulkp_sourcep;
double t7_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t8_Vgatep_sourcep;
double t8_Vdrainp_sourcep;
double t8_Vdrainb_sourcep;
double t8_Vbulkp_sourcep;
double t8_Vsourceb_sourcep;
#endif
double n;
#if defined(_DERIVATE)
double n_Vdrainp_sourcep;
double n_Vdrainb_sourcep;
double n_Vbulkp_sourcep;
double n_Vsourceb_sourcep;
#endif
double tmp4;
#if defined(_DERIVATE)
double tmp4_Vgatep_sourcep;
double tmp4_Vdrainp_sourcep;
double tmp4_Vdrainb_sourcep;
double tmp4_Vbulkp_sourcep;
double tmp4_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double tmp3_Vgatep_sourcep;
double tmp3_Vdrainp_sourcep;
double tmp3_Vdrainb_sourcep;
double tmp3_Vbulkp_sourcep;
double tmp3_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double tmp2_Vgatep_sourcep;
double tmp2_Vdrainp_sourcep;
double tmp2_Vdrainb_sourcep;
double tmp2_Vbulkp_sourcep;
double tmp2_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double tmp1_Vgatep_sourcep;
double tmp1_Vdrainp_sourcep;
double tmp1_Vdrainb_sourcep;
double tmp1_Vbulkp_sourcep;
double tmp1_Vsourceb_sourcep;
#endif
double vth;
#if defined(_DERIVATE)
double vth_Vdrainp_sourcep;
double vth_Vdrainb_sourcep;
double vth_Vbulkp_sourcep;
double vth_Vsourceb_sourcep;
#endif
double lpe_vb;
double dibl_sft;
#if defined(_DERIVATE)
double dibl_sft_Vdrainp_sourcep;
double dibl_sft_Vdrainb_sourcep;
double dibl_sft_Vbulkp_sourcep;
double dibl_sft_Vsourceb_sourcep;
#endif
double ddibl_sft_dvd;
#if defined(_DERIVATE)
double ddibl_sft_dvd_Vdrainp_sourcep;
double ddibl_sft_dvd_Vdrainb_sourcep;
double ddibl_sft_dvd_Vbulkp_sourcep;
double ddibl_sft_dvd_Vsourceb_sourcep;
#endif
double vth_narroww;
#if defined(_DERIVATE)
double t5_Vgatep_sourcep;
double t5_Vdrainp_sourcep;
double t5_Vdrainb_sourcep;
double t5_Vbulkp_sourcep;
double t5_Vsourceb_sourcep;
#endif
double delt_vth;
#if defined(_DERIVATE)
double delt_vth_Vdrainp_sourcep;
double delt_vth_Vdrainb_sourcep;
double delt_vth_Vbulkp_sourcep;
double delt_vth_Vsourceb_sourcep;
#endif
double thetavth;
#if defined(_DERIVATE)
double thetavth_Vdrainp_sourcep;
double thetavth_Vdrainb_sourcep;
double thetavth_Vbulkp_sourcep;
double thetavth_Vsourceb_sourcep;
#endif
double theta0;
#if defined(_DERIVATE)
double theta0_Vdrainp_sourcep;
double theta0_Vdrainb_sourcep;
double theta0_Vbulkp_sourcep;
double theta0_Vsourceb_sourcep;
#endif
double ltw;
#if defined(_DERIVATE)
double ltw_Vdrainp_sourcep;
double ltw_Vdrainb_sourcep;
double ltw_Vbulkp_sourcep;
double ltw_Vsourceb_sourcep;
#endif
double lt1;
#if defined(_DERIVATE)
double lt1_Vdrainp_sourcep;
double lt1_Vdrainb_sourcep;
double lt1_Vbulkp_sourcep;
double lt1_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t4_Vgatep_sourcep;
double t4_Vdrainp_sourcep;
double t4_Vdrainb_sourcep;
double t4_Vbulkp_sourcep;
double t4_Vsourceb_sourcep;
#endif
double v0;
double xdep;
#if defined(_DERIVATE)
double xdep_Vdrainp_sourcep;
double xdep_Vdrainb_sourcep;
double xdep_Vbulkp_sourcep;
double xdep_Vsourceb_sourcep;
#endif
double sqrtphis;
#if defined(_DERIVATE)
double sqrtphis_Vdrainp_sourcep;
double sqrtphis_Vdrainb_sourcep;
double sqrtphis_Vbulkp_sourcep;
double sqrtphis_Vsourceb_sourcep;
#endif
double phis;
#if defined(_DERIVATE)
double phis_Vdrainp_sourcep;
double phis_Vdrainb_sourcep;
double phis_Vbulkp_sourcep;
double phis_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t9_Vgatep_sourcep;
double t9_Vdrainp_sourcep;
double t9_Vdrainb_sourcep;
double t9_Vbulkp_sourcep;
double t9_Vsourceb_sourcep;
#endif
double vbseff;
#if defined(_DERIVATE)
double vbseff_Vdrainp_sourcep;
double vbseff_Vdrainb_sourcep;
double vbseff_Vbulkp_sourcep;
double vbseff_Vsourceb_sourcep;
#endif
double Vbs;
#if defined(_DERIVATE)
double Vbs_Vdrainp_sourcep;
double Vbs_Vbulkp_sourcep;
#endif
double Vds;
#if defined(_DERIVATE)
double Vds_Vdrainp_sourcep;
#endif
double mode;
double evbd;
#if defined(_DERIVATE)
double evbd_Vsourceb_sourcep;
double evbd_Vbulkp_sourcep;
double evbd_Vdrainp_sourcep;
double evbd_Vdrainb_sourcep;
#endif
double cbd;
#if defined(_DERIVATE)
double cbd_Vsourceb_sourcep;
double cbd_Vbulkp_sourcep;
double cbd_Vdrainp_sourcep;
double cbd_Vdrainb_sourcep;
#endif
#if defined(_DERIVATE)
double t3_Vgatep_sourcep;
double t3_Vdrainp_sourcep;
double t3_Vdrainb_sourcep;
double t3_Vbulkp_sourcep;
double t3_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t0_Vgatep_sourcep;
double t0_Vdrainp_sourcep;
double t0_Vdrainb_sourcep;
double t0_Vbulkp_sourcep;
double t0_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t2_Vgatep_sourcep;
double t2_Vdrainp_sourcep;
double t2_Vdrainb_sourcep;
double t2_Vbulkp_sourcep;
double t2_Vsourceb_sourcep;
#endif
#if defined(_DERIVATE)
double t1_Vgatep_sourcep;
double t1_Vdrainp_sourcep;
double t1_Vdrainb_sourcep;
double t1_Vbulkp_sourcep;
double t1_Vsourceb_sourcep;
#endif
double evbs;
#if defined(_DERIVATE)
double evbs_Vbulkp_sourcep;
double evbs_Vsourceb_sourcep;
#endif
double cbs;
#if defined(_DERIVATE)
double cbs_Vbulkp_sourcep;
double cbs_Vsourceb_sourcep;
#endif
double vbd_jct;
#if defined(_DERIVATE)
double vbd_jct_Vbulkp_sourcep;
double vbd_jct_Vdrainp_sourcep;
double vbd_jct_Vdrainb_sourcep;
#endif
double vbs_jct;
#if defined(_DERIVATE)
double vbs_jct_Vbulkp_sourcep;
double vbs_jct_Vsourceb_sourcep;
#endif
double vbdb;
#if defined(_DERIVATE)
double vbdb_Vbulkp_sourcep;
double vbdb_Vdrainb_sourcep;
#endif
double vbsb;
#if defined(_DERIVATE)
double vbsb_Vbulkp_sourcep;
double vbsb_Vsourceb_sourcep;
#endif
double vbedb;
#if defined(_DERIVATE)
double vbedb_Vbulk_sourcep;
double vbedb_Vdrainb_sourcep;
#endif
double vbesb;
#if defined(_DERIVATE)
double vbesb_Vbulk_sourcep;
double vbesb_Vsourceb_sourcep;
#endif
double vdbd;
#if defined(_DERIVATE)
double vdbd_Vdrainb_sourcep;
double vdbd_Vdrainp_sourcep;
#endif
double vbeb;
#if defined(_DERIVATE)
double vbeb_Vbulk_sourcep;
double vbeb_Vbulkp_sourcep;
#endif
double vgegm;
#if defined(_DERIVATE)
double vgegm_Vgate_sourcep;
double vgegm_Vgatep_sourcep;
double vgegm_Vgatem_sourcep;
#endif
double vgmb;
#if defined(_DERIVATE)
double vgmb_Vgatem_sourcep;
double vgmb_Vbulkp_sourcep;
#endif
double vgmg;
#if defined(_DERIVATE)
double vgmg_Vgatem_sourcep;
double vgmg_Vgatep_sourcep;
#endif
double vgeg;
#if defined(_DERIVATE)
double vgeg_Vgate_sourcep;
double vgeg_Vgatep_sourcep;
#endif
double vded;
#if defined(_DERIVATE)
double vded_Vdrain_sourcep;
double vded_Vdrainp_sourcep;
#endif
double vgb;
#if defined(_DERIVATE)
double vgb_Vgatep_sourcep;
double vgb_Vbulkp_sourcep;
#endif
double vbd;
#if defined(_DERIVATE)
double vbd_Vbulkp_sourcep;
double vbd_Vdrainp_sourcep;
#endif
double vgd;
#if defined(_DERIVATE)
double vgd_Vgatep_sourcep;
double vgd_Vdrainp_sourcep;
#endif
double vbes;
#if defined(_DERIVATE)
double vbes_Vbulk_sourcep;
#endif
double vdes;
#if defined(_DERIVATE)
double vdes_Vdrain_sourcep;
#endif
double vses;
#if defined(_DERIVATE)
double vses_Vsource_sourcep;
#endif
double vdbs;
#if defined(_DERIVATE)
double vdbs_Vdrainb_sourcep;
#endif
double vsbs;
#if defined(_DERIVATE)
double vsbs_Vsourceb_sourcep;
#endif
double vgms;
#if defined(_DERIVATE)
double vgms_Vgatem_sourcep;
#endif
double vges;
#if defined(_DERIVATE)
double vges_Vgate_sourcep;
#endif
double vbs;
#if defined(_DERIVATE)
double vbs_Vbulkp_sourcep;
#endif
double vgs;
#if defined(_DERIVATE)
double vgs_Vgatep_sourcep;
#endif
double vds;
#if defined(_DERIVATE)
double vds_Vdrainp_sourcep;
#endif
#if defined(_DERIVATE)
vds_Vdrainp_sourcep=(type);
#endif
vds=(type*BP(drainp,sourcep));
#if defined(_DERIVATE)
vgs_Vgatep_sourcep=(type);
#endif
vgs=(type*BP(gatep,sourcep));
#if defined(_DERIVATE)
vbs_Vbulkp_sourcep=(type);
#endif
vbs=(type*BP(bulkp,sourcep));
#if defined(_DERIVATE)
vges_Vgate_sourcep=(type);
#endif
vges=(type*BP(gate,sourcep));
#if defined(_DERIVATE)
vgms_Vgatem_sourcep=(type);
#endif
vgms=(type*BP(gatem,sourcep));
#if defined(_DERIVATE)
vsbs_Vsourceb_sourcep=(type);
#endif
vsbs=(type*BP(sourceb,sourcep));
#if defined(_DERIVATE)
vdbs_Vdrainb_sourcep=(type);
#endif
vdbs=(type*BP(drainb,sourcep));
#if defined(_DERIVATE)
vses_Vsource_sourcep=(type);
#endif
vses=(type*BP(source,sourcep));
#if defined(_DERIVATE)
vdes_Vdrain_sourcep=(type);
#endif
vdes=(type*BP(drain,sourcep));
#if defined(_DERIVATE)
vbes_Vbulk_sourcep=(type);
#endif
vbes=(type*BP(bulk,sourcep));
#if defined(_DERIVATE)
vgd_Vgatep_sourcep=vgs_Vgatep_sourcep;
vgd_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vgd=(vgs-vds);
#if defined(_DERIVATE)
vbd_Vbulkp_sourcep=vbs_Vbulkp_sourcep;
vbd_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vbd=(vbs-vds);
#if defined(_DERIVATE)
vgb_Vgatep_sourcep=vgs_Vgatep_sourcep;
vgb_Vbulkp_sourcep=(-vbs_Vbulkp_sourcep);
#endif
vgb=(vgs-vbs);
#if defined(_DERIVATE)
vded_Vdrain_sourcep=vdes_Vdrain_sourcep;
vded_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vded=(vdes-vds);
#if defined(_DERIVATE)
vgeg_Vgate_sourcep=vges_Vgate_sourcep;
vgeg_Vgatep_sourcep=(-vgs_Vgatep_sourcep);
#endif
vgeg=(vges-vgs);
#if defined(_DERIVATE)
vgmg_Vgatem_sourcep=vgms_Vgatem_sourcep;
vgmg_Vgatep_sourcep=(-vgs_Vgatep_sourcep);
#endif
vgmg=(vgms-vgs);
#if defined(_DERIVATE)
vgmb_Vgatem_sourcep=vgms_Vgatem_sourcep;
vgmb_Vbulkp_sourcep=(-vbs_Vbulkp_sourcep);
#endif
vgmb=(vgms-vbs);
#if defined(_DERIVATE)
vgegm_Vgate_sourcep=vgeg_Vgate_sourcep;
vgegm_Vgatep_sourcep=(vgeg_Vgatep_sourcep-vgmg_Vgatep_sourcep);
vgegm_Vgatem_sourcep=(-vgmg_Vgatem_sourcep);
#endif
vgegm=(vgeg-vgmg);
#if defined(_DERIVATE)
vbeb_Vbulk_sourcep=vbes_Vbulk_sourcep;
vbeb_Vbulkp_sourcep=(-vbs_Vbulkp_sourcep);
#endif
vbeb=(vbes-vbs);
#if defined(_DERIVATE)
vdbd_Vdrainb_sourcep=vdbs_Vdrainb_sourcep;
vdbd_Vdrainp_sourcep=(-vds_Vdrainp_sourcep);
#endif
vdbd=(vdbs-vds);
#if defined(_DERIVATE)
vbesb_Vbulk_sourcep=vbes_Vbulk_sourcep;
vbesb_Vsourceb_sourcep=(-vsbs_Vsourceb_sourcep);
#endif
vbesb=(vbes-vsbs);
#if defined(_DERIVATE)
vbedb_Vbulk_sourcep=vbes_Vbulk_sourcep;
vbedb_Vdrainb_sourcep=(-vdbs_Vdrainb_sourcep);
#endif
vbedb=(vbes-vdbs);
#if defined(_DERIVATE)
vbsb_Vbulkp_sourcep=vbs_Vbulkp_sourcep;
vbsb_Vsourceb_sourcep=(-vsbs_Vsourceb_sourcep);
#endif
vbsb=(vbs-vsbs);
#if defined(_DERIVATE)
vbdb_Vbulkp_sourcep=vbs_Vbulkp_sourcep;
vbdb_Vdrainb_sourcep=(-vdbs_Vdrainb_sourcep);
#endif
vbdb=(vbs-vdbs);
#if defined(_DERIVATE)
vbs_jct_Vbulkp_sourcep=((rbodymod==0)?vbs_Vbulkp_sourcep:0.0);
vbs_jct_Vsourceb_sourcep=((rbodymod==0)?0.0:vsbs_Vsourceb_sourcep);
#endif
vbs_jct=((rbodymod==0)?vbs:vsbs);
#if defined(_DERIVATE)
vbd_jct_Vbulkp_sourcep=((rbodymod==0)?vbd_Vbulkp_sourcep:0.0);
vbd_jct_Vdrainp_sourcep=((rbodymod==0)?vbd_Vdrainp_sourcep:vdbd_Vdrainp_sourcep);
vbd_jct_Vdrainb_sourcep=((rbodymod==0)?0.0:vdbd_Vdrainb_sourcep);
#endif
vbd_jct=((rbodymod==0)?vbd:vdbd);
nvtms=(vtm*njs);
if
(((aseff<=0.0)&&(pseff<=0.0)))
{
isbs=1.0e-14;
}
else
{
isbs=(((aseff*jss_temp)+(pseff*jsws_temp))+((weffcj*nf)*jswgs_temp));
}
if
((isbs<=0.0))
{
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=(gmin*vbs_jct_Vbulkp_sourcep);
cbs_Vsourceb_sourcep=(gmin*vbs_jct_Vsourceb_sourcep);
#endif
cbs=(gmin*vbs_jct);
}
else
{
if
((diomod)==(0))
{
{
double m00_exp(d00_exp0,(vbs_jct/nvtms))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=(vbs_jct_Vbulkp_sourcep/nvtms)*d10_exp0;
evbs_Vsourceb_sourcep=(vbs_jct_Vsourceb_sourcep/nvtms)*d10_exp0;
#endif
evbs=d00_exp0;
}
{
double m00_exp(d00_exp0,((-(bvs+vbs_jct))/nvtms))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=(xjbvs*((-vbs_jct_Vbulkp_sourcep)/nvtms)*d10_exp0);
t1_Vsourceb_sourcep=(xjbvs*((-vbs_jct_Vsourceb_sourcep)/nvtms)*d10_exp0);
#endif
t1=(xjbvs*d00_exp0);
}
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=((isbs*(evbs_Vbulkp_sourcep-t1_Vbulkp_sourcep))+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=((isbs*(evbs_Vsourceb_sourcep-t1_Vsourceb_sourcep))+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=((isbs*(((evbs+xexpbvs)-t1)-1.0))+(gmin*vbs_jct));
}
else
if
((diomod)==(1))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=(vbs_jct_Vbulkp_sourcep/nvtms);
t2_Vsourceb_sourcep=(vbs_jct_Vsourceb_sourcep/nvtms);
#endif
t2=(vbs_jct/nvtms);
if
((t2<(-34.0)))
{
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=(gmin*vbs_jct_Vbulkp_sourcep);
cbs_Vsourceb_sourcep=(gmin*vbs_jct_Vsourceb_sourcep);
#endif
cbs=((isbs*(1.713908431e-15-1.0))+(gmin*vbs_jct));
}
else
{
if
((vbs_jct<=vjsmfwd))
{
{
double m00_exp(d00_exp0,t2)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=t2_Vbulkp_sourcep*d10_exp0;
evbs_Vsourceb_sourcep=t2_Vsourceb_sourcep*d10_exp0;
#endif
evbs=d00_exp0;
}
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=((isbs*evbs_Vbulkp_sourcep)+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=((isbs*evbs_Vsourceb_sourcep)+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=((isbs*(evbs-1.0))+(gmin*vbs_jct));
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(ivjsmfwd/nvtms);
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*(vbs_jct-vjsmfwd))+(t0*vbs_jct_Vbulkp_sourcep))+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*(vbs_jct-vjsmfwd))+(t0*vbs_jct_Vsourceb_sourcep))+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=(((ivjsmfwd-isbs)+(t0*(vbs_jct-vjsmfwd)))+(gmin*vbs_jct));
}
}
}
else
if
((diomod)==(2))
{
if
((vbs_jct<vjsmrev))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=(vbs_jct_Vbulkp_sourcep/nvtms);
t0_Vsourceb_sourcep=(vbs_jct_Vsourceb_sourcep/nvtms);
#endif
t0=(vbs_jct/nvtms);
if
((t0<(-34.0)))
{
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=0.0;
evbs_Vsourceb_sourcep=0.0;
#endif
evbs=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
evbs_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
#endif
evbs=d00_exp0;
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=evbs_Vbulkp_sourcep;
t1_Vsourceb_sourcep=evbs_Vsourceb_sourcep;
#endif
t1=(evbs-1.0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=(sslprev*vbs_jct_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(sslprev*vbs_jct_Vsourceb_sourcep);
#endif
t2=(ivjsmrev+(sslprev*(vbs_jct-vjsmrev)));
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t2)+(t1*t2_Vbulkp_sourcep))+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*t2)+(t1*t2_Vsourceb_sourcep))+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=((t1*t2)+(gmin*vbs_jct));
}
else
{
if
((vbs_jct<=vjsmfwd))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=(vbs_jct_Vbulkp_sourcep/nvtms);
t0_Vsourceb_sourcep=(vbs_jct_Vsourceb_sourcep/nvtms);
#endif
t0=(vbs_jct/nvtms);
if
((t0<(-34.0)))
{
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=0.0;
evbs_Vsourceb_sourcep=0.0;
#endif
evbs=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbs_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
evbs_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
#endif
evbs=d00_exp0;
}
}
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=(vbs_jct_Vbulkp_sourcep/nvtms);
t3_Vsourceb_sourcep=(vbs_jct_Vsourceb_sourcep/nvtms);
#endif
t3=((bvs+vbs_jct)/nvtms);
if
((t1>34.0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,(-t1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=(-t1_Vbulkp_sourcep)*d10_exp0;
t2_Vsourceb_sourcep=(-t1_Vsourceb_sourcep)*d10_exp0;
#endif
t2=d00_exp0;
}
}
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=((isbs*(evbs_Vbulkp_sourcep-(xjbvs*t2_Vbulkp_sourcep)))+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=((isbs*(evbs_Vsourceb_sourcep-(xjbvs*t2_Vsourceb_sourcep)))+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=((isbs*(((evbs+xexpbvs)-1.0)-(xjbvs*t2)))+(gmin*vbs_jct));
}
else
{
#if defined(_DERIVATE)
cbs_Vbulkp_sourcep=((sslpfwd*vbs_jct_Vbulkp_sourcep)+(gmin*vbs_jct_Vbulkp_sourcep));
cbs_Vsourceb_sourcep=((sslpfwd*vbs_jct_Vsourceb_sourcep)+(gmin*vbs_jct_Vsourceb_sourcep));
#endif
cbs=((ivjsmfwd+(sslpfwd*(vbs_jct-vjsmfwd)))+(gmin*vbs_jct));
}
}
}
else
{ /* no default */ }
}
nvtmd=(vtm*njd);
if
(((adeff<=0.0)&&(pdeff<=0.0)))
{
isbd=1.0e-14;
}
else
{
isbd=(((adeff*jsd_temp)+(pdeff*jswd_temp))+((weffcj*nf)*jswgd_temp));
}
if
((isbd<=0.0))
{
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=0.0;
cbd_Vbulkp_sourcep=(gmin*vbd_jct_Vbulkp_sourcep);
cbd_Vdrainp_sourcep=(gmin*vbd_jct_Vdrainp_sourcep);
cbd_Vdrainb_sourcep=(gmin*vbd_jct_Vdrainb_sourcep);
#endif
cbd=(gmin*vbd_jct);
}
else
{
if
((diomod)==(0))
{
{
double m00_exp(d00_exp0,(vbd_jct/nvtmd))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=0.0;
evbd_Vbulkp_sourcep=(vbd_jct_Vbulkp_sourcep/nvtmd)*d10_exp0;
evbd_Vdrainp_sourcep=(vbd_jct_Vdrainp_sourcep/nvtmd)*d10_exp0;
evbd_Vdrainb_sourcep=(vbd_jct_Vdrainb_sourcep/nvtmd)*d10_exp0;
#endif
evbd=d00_exp0;
}
{
double m00_exp(d00_exp0,((-(bvd+vbd_jct))/nvtmd))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(xjbvd*((-vbd_jct_Vdrainp_sourcep)/nvtmd)*d10_exp0);
t1_Vdrainb_sourcep=(xjbvd*((-vbd_jct_Vdrainb_sourcep)/nvtmd)*d10_exp0);
t1_Vbulkp_sourcep=(xjbvd*((-vbd_jct_Vbulkp_sourcep)/nvtmd)*d10_exp0);
t1_Vsourceb_sourcep=0.0;
#endif
t1=(xjbvd*d00_exp0);
}
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=(isbd*(evbd_Vsourceb_sourcep-t1_Vsourceb_sourcep));
cbd_Vbulkp_sourcep=((isbd*(evbd_Vbulkp_sourcep-t1_Vbulkp_sourcep))+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=((isbd*(evbd_Vdrainp_sourcep-t1_Vdrainp_sourcep))+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=((isbd*(evbd_Vdrainb_sourcep-t1_Vdrainb_sourcep))+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=((isbd*(((evbd+xexpbvd)-t1)-1.0))+(gmin*vbd_jct));
}
else
if
((diomod)==(1))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(vbd_jct_Vdrainp_sourcep/nvtmd);
t2_Vdrainb_sourcep=(vbd_jct_Vdrainb_sourcep/nvtmd);
t2_Vbulkp_sourcep=(vbd_jct_Vbulkp_sourcep/nvtmd);
t2_Vsourceb_sourcep=0.0;
#endif
t2=(vbd_jct/nvtmd);
if
((t2<(-34.0)))
{
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=0.0;
cbd_Vbulkp_sourcep=(gmin*vbd_jct_Vbulkp_sourcep);
cbd_Vdrainp_sourcep=(gmin*vbd_jct_Vdrainp_sourcep);
cbd_Vdrainb_sourcep=(gmin*vbd_jct_Vdrainb_sourcep);
#endif
cbd=((isbd*(1.713908431e-15-1.0))+(gmin*vbd_jct));
}
else
{
if
((vbd_jct<=vjdmfwd))
{
{
double m00_exp(d00_exp0,t2)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=t2_Vsourceb_sourcep*d10_exp0;
evbd_Vbulkp_sourcep=t2_Vbulkp_sourcep*d10_exp0;
evbd_Vdrainp_sourcep=t2_Vdrainp_sourcep*d10_exp0;
evbd_Vdrainb_sourcep=t2_Vdrainb_sourcep*d10_exp0;
#endif
evbd=d00_exp0;
}
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=(isbd*evbd_Vsourceb_sourcep);
cbd_Vbulkp_sourcep=((isbd*evbd_Vbulkp_sourcep)+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=((isbd*evbd_Vdrainp_sourcep)+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=((isbd*evbd_Vdrainb_sourcep)+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=((isbd*(evbd-1.0))+(gmin*vbd_jct));
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(ivjdmfwd/nvtmd);
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=t0_Vsourceb_sourcep*(vbd_jct-vjdmfwd);
cbd_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*(vbd_jct-vjdmfwd))+(t0*vbd_jct_Vbulkp_sourcep))+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*(vbd_jct-vjdmfwd))+(t0*vbd_jct_Vdrainp_sourcep))+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*(vbd_jct-vjdmfwd))+(t0*vbd_jct_Vdrainb_sourcep))+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=(((ivjdmfwd-isbd)+(t0*(vbd_jct-vjdmfwd)))+(gmin*vbd_jct));
}
}
}
else
if
((diomod)==(2))
{
if
((vbd_jct<vjdmrev))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(vbd_jct_Vdrainp_sourcep/nvtmd);
t0_Vdrainb_sourcep=(vbd_jct_Vdrainb_sourcep/nvtmd);
t0_Vbulkp_sourcep=(vbd_jct_Vbulkp_sourcep/nvtmd);
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vbd_jct/nvtmd);
if
((t0<(-34.0)))
{
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=0.0;
evbd_Vbulkp_sourcep=0.0;
evbd_Vdrainp_sourcep=0.0;
evbd_Vdrainb_sourcep=0.0;
#endif
evbd=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
evbd_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
evbd_Vdrainp_sourcep=t0_Vdrainp_sourcep*d10_exp0;
evbd_Vdrainb_sourcep=t0_Vdrainb_sourcep*d10_exp0;
#endif
evbd=d00_exp0;
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=evbd_Vdrainp_sourcep;
t1_Vdrainb_sourcep=evbd_Vdrainb_sourcep;
t1_Vbulkp_sourcep=evbd_Vbulkp_sourcep;
t1_Vsourceb_sourcep=evbd_Vsourceb_sourcep;
#endif
t1=(evbd-1.0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(dslprev*vbd_jct_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(dslprev*vbd_jct_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(dslprev*vbd_jct_Vbulkp_sourcep);
t2_Vsourceb_sourcep=0.0;
#endif
t2=(ivjdmrev+(dslprev*(vbd_jct-vjdmrev)));
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t2)+(t1*t2_Vsourceb_sourcep));
cbd_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t2)+(t1*t2_Vbulkp_sourcep))+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*t2)+(t1*t2_Vdrainp_sourcep))+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*t2)+(t1*t2_Vdrainb_sourcep))+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=((t1*t2)+(gmin*vbd_jct));
}
else
{
if
((vbd_jct<=vjdmfwd))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(vbd_jct_Vdrainp_sourcep/nvtmd);
t0_Vdrainb_sourcep=(vbd_jct_Vdrainb_sourcep/nvtmd);
t0_Vbulkp_sourcep=(vbd_jct_Vbulkp_sourcep/nvtmd);
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vbd_jct/nvtmd);
if
((t0<(-34.0)))
{
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=0.0;
evbd_Vbulkp_sourcep=0.0;
evbd_Vdrainp_sourcep=0.0;
evbd_Vdrainb_sourcep=0.0;
#endif
evbd=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
evbd_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
evbd_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
evbd_Vdrainp_sourcep=t0_Vdrainp_sourcep*d10_exp0;
evbd_Vdrainb_sourcep=t0_Vdrainb_sourcep*d10_exp0;
#endif
evbd=d00_exp0;
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(vbd_jct_Vdrainp_sourcep/nvtmd);
t1_Vdrainb_sourcep=(vbd_jct_Vdrainb_sourcep/nvtmd);
t1_Vbulkp_sourcep=(vbd_jct_Vbulkp_sourcep/nvtmd);
t1_Vsourceb_sourcep=0.0;
#endif
t1=((bvd+vbd_jct)/nvtmd);
if
((t1>34.0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,(-t1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(-t1_Vdrainp_sourcep)*d10_exp0;
t2_Vdrainb_sourcep=(-t1_Vdrainb_sourcep)*d10_exp0;
t2_Vbulkp_sourcep=(-t1_Vbulkp_sourcep)*d10_exp0;
t2_Vsourceb_sourcep=(-t1_Vsourceb_sourcep)*d10_exp0;
#endif
t2=d00_exp0;
}
}
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=(isbd*(evbd_Vsourceb_sourcep-(xjbvd*t2_Vsourceb_sourcep)));
cbd_Vbulkp_sourcep=((isbd*(evbd_Vbulkp_sourcep-(xjbvd*t2_Vbulkp_sourcep)))+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=((isbd*(evbd_Vdrainp_sourcep-(xjbvd*t2_Vdrainp_sourcep)))+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=((isbd*(evbd_Vdrainb_sourcep-(xjbvd*t2_Vdrainb_sourcep)))+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=((isbd*(((evbd+xexpbvd)-1.0)-(xjbvd*t2)))+(gmin*vbd_jct));
}
else
{
#if defined(_DERIVATE)
cbd_Vsourceb_sourcep=0.0;
cbd_Vbulkp_sourcep=((dslpfwd*vbd_jct_Vbulkp_sourcep)+(gmin*vbd_jct_Vbulkp_sourcep));
cbd_Vdrainp_sourcep=((dslpfwd*vbd_jct_Vdrainp_sourcep)+(gmin*vbd_jct_Vdrainp_sourcep));
cbd_Vdrainb_sourcep=((dslpfwd*vbd_jct_Vdrainb_sourcep)+(gmin*vbd_jct_Vdrainb_sourcep));
#endif
cbd=((ivjdmfwd+(dslpfwd*(vbd_jct-vjdmfwd)))+(gmin*vbd_jct));
}
}
}
else
{ /* no default */ }
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
Vbs_Vdrainp_sourcep=0.0;
Vbs_Vbulkp_sourcep=vbs_Vbulkp_sourcep;
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
Vbs_Vdrainp_sourcep=vbd_Vdrainp_sourcep;
Vbs_Vbulkp_sourcep=vbd_Vbulkp_sourcep;
#endif
Vbs=vbd;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=Vbs_Vdrainp_sourcep;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=Vbs_Vbulkp_sourcep;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((Vbs-vbsc)-0.001);
{
double m00_sqrt(d00_sqrt0,((t0*t0)-(0.004*vbsc)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)-(0.004*vbsc)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
if
((t0>=0.0))
{
#if defined(_DERIVATE)
vbseff_Vdrainp_sourcep=(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep));
vbseff_Vdrainb_sourcep=(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep));
vbseff_Vbulkp_sourcep=(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep));
vbseff_Vsourceb_sourcep=(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep));
#endif
vbseff=(vbsc+(0.5*(t0+t1)));
}
else
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(-(-0.002)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep)/(t1-t0)/(t1-t0));
t2_Vdrainb_sourcep=(-(-0.002)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep)/(t1-t0)/(t1-t0));
t2_Vbulkp_sourcep=(-(-0.002)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep)/(t1-t0)/(t1-t0));
t2_Vsourceb_sourcep=(-(-0.002)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep)/(t1-t0)/(t1-t0));
#endif
t2=((-0.002)/(t1-t0));
#if defined(_DERIVATE)
vbseff_Vdrainp_sourcep=(vbsc*t2_Vdrainp_sourcep);
vbseff_Vdrainb_sourcep=(vbsc*t2_Vdrainb_sourcep);
vbseff_Vbulkp_sourcep=(vbsc*t2_Vbulkp_sourcep);
vbseff_Vsourceb_sourcep=(vbsc*t2_Vsourceb_sourcep);
#endif
vbseff=(vbsc*(1.0+t2));
}
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(0.95*phi);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(t9_Vdrainp_sourcep-vbseff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(t9_Vdrainb_sourcep-vbseff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(t9_Vbulkp_sourcep-vbseff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(t9_Vsourceb_sourcep-vbseff_Vsourceb_sourcep);
#endif
t0=((t9-vbseff)-0.001);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+(0.004*t9)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+(0.004*t9)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+(0.004*t9_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+(0.004*t9_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+(0.004*t9_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+(0.004*t9_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
vbseff_Vdrainp_sourcep=(t9_Vdrainp_sourcep-(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep)));
vbseff_Vdrainb_sourcep=(t9_Vdrainb_sourcep-(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep)));
vbseff_Vbulkp_sourcep=(t9_Vbulkp_sourcep-(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep)));
vbseff_Vsourceb_sourcep=(t9_Vsourceb_sourcep-(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep)));
#endif
vbseff=(t9-(0.5*(t0+t1)));
#if defined(_DERIVATE)
phis_Vdrainp_sourcep=(-vbseff_Vdrainp_sourcep);
phis_Vdrainb_sourcep=(-vbseff_Vdrainb_sourcep);
phis_Vbulkp_sourcep=(-vbseff_Vbulkp_sourcep);
phis_Vsourceb_sourcep=(-vbseff_Vsourceb_sourcep);
#endif
phis=(phi-vbseff);
{
double m00_sqrt(d00_sqrt0,phis)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,phis)
#endif
#if defined(_DERIVATE)
sqrtphis_Vdrainp_sourcep=phis_Vdrainp_sourcep*d10_sqrt0;
sqrtphis_Vdrainb_sourcep=phis_Vdrainb_sourcep*d10_sqrt0;
sqrtphis_Vbulkp_sourcep=phis_Vbulkp_sourcep*d10_sqrt0;
sqrtphis_Vsourceb_sourcep=phis_Vsourceb_sourcep*d10_sqrt0;
#endif
sqrtphis=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,phi)
#if defined(_DERIVATE)
xdep_Vdrainp_sourcep=((xdep0*sqrtphis_Vdrainp_sourcep)/d00_sqrt0);
xdep_Vdrainb_sourcep=((xdep0*sqrtphis_Vdrainb_sourcep)/d00_sqrt0);
xdep_Vbulkp_sourcep=((xdep0*sqrtphis_Vbulkp_sourcep)/d00_sqrt0);
xdep_Vsourceb_sourcep=((xdep0*sqrtphis_Vsourceb_sourcep)/d00_sqrt0);
#endif
xdep=((xdep0*sqrtphis)/d00_sqrt0);
}
{
double m00_sqrt(d00_sqrt0,xdep)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,xdep)
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=xdep_Vdrainp_sourcep*d10_sqrt0;
t3_Vdrainb_sourcep=xdep_Vdrainb_sourcep*d10_sqrt0;
t3_Vbulkp_sourcep=xdep_Vbulkp_sourcep*d10_sqrt0;
t3_Vsourceb_sourcep=xdep_Vsourceb_sourcep*d10_sqrt0;
#endif
t3=d00_sqrt0;
}
v0=(vbi-phi);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(dvt2*vbseff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(dvt2*vbseff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(dvt2*vbseff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(dvt2*vbseff_Vsourceb_sourcep);
#endif
t0=(dvt2*vbseff);
if
((t0>=(-0.5)))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=t0_Vdrainp_sourcep;
t1_Vdrainb_sourcep=t0_Vdrainb_sourcep;
t1_Vbulkp_sourcep=t0_Vbulkp_sourcep;
t1_Vsourceb_sourcep=t0_Vsourceb_sourcep;
#endif
t1=(1.0+t0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=dvt2;
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=(-(8.0*t0_Vdrainp_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vdrainb_sourcep=(-(8.0*t0_Vdrainb_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vbulkp_sourcep=(-(8.0*t0_Vbulkp_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vsourceb_sourcep=(-(8.0*t0_Vsourceb_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
#endif
t4=(1.0/(3.0+(8.0*t0)));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(((3.0*t0_Vdrainp_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vdrainp_sourcep));
t1_Vdrainb_sourcep=(((3.0*t0_Vdrainb_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vdrainb_sourcep));
t1_Vbulkp_sourcep=(((3.0*t0_Vbulkp_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vbulkp_sourcep));
t1_Vsourceb_sourcep=(((3.0*t0_Vsourceb_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vsourceb_sourcep));
#endif
t1=((1.0+(3.0*t0))*t4);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(((dvt2*t4_Vdrainp_sourcep)*t4)+((dvt2*t4)*t4_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(((dvt2*t4_Vdrainb_sourcep)*t4)+((dvt2*t4)*t4_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(((dvt2*t4_Vbulkp_sourcep)*t4)+((dvt2*t4)*t4_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(((dvt2*t4_Vsourceb_sourcep)*t4)+((dvt2*t4)*t4_Vsourceb_sourcep));
#endif
t2=((dvt2*t4)*t4);
}
#if defined(_DERIVATE)
lt1_Vdrainp_sourcep=(((factor1*t3_Vdrainp_sourcep)*t1)+((factor1*t3)*t1_Vdrainp_sourcep));
lt1_Vdrainb_sourcep=(((factor1*t3_Vdrainb_sourcep)*t1)+((factor1*t3)*t1_Vdrainb_sourcep));
lt1_Vbulkp_sourcep=(((factor1*t3_Vbulkp_sourcep)*t1)+((factor1*t3)*t1_Vbulkp_sourcep));
lt1_Vsourceb_sourcep=(((factor1*t3_Vsourceb_sourcep)*t1)+((factor1*t3)*t1_Vsourceb_sourcep));
#endif
lt1=((factor1*t3)*t1);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(dvt2w*vbseff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(dvt2w*vbseff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(dvt2w*vbseff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(dvt2w*vbseff_Vsourceb_sourcep);
#endif
t0=(dvt2w*vbseff);
if
((t0>=(-0.5)))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=t0_Vdrainp_sourcep;
t1_Vdrainb_sourcep=t0_Vdrainb_sourcep;
t1_Vbulkp_sourcep=t0_Vbulkp_sourcep;
t1_Vsourceb_sourcep=t0_Vsourceb_sourcep;
#endif
t1=(1.0+t0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=dvt2w;
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=(-(8.0*t0_Vdrainp_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vdrainb_sourcep=(-(8.0*t0_Vdrainb_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vbulkp_sourcep=(-(8.0*t0_Vbulkp_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
t4_Vsourceb_sourcep=(-(8.0*t0_Vsourceb_sourcep)/(3.0+(8.0*t0))/(3.0+(8.0*t0)));
#endif
t4=(1.0/(3.0+(8.0*t0)));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(((3.0*t0_Vdrainp_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vdrainp_sourcep));
t1_Vdrainb_sourcep=(((3.0*t0_Vdrainb_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vdrainb_sourcep));
t1_Vbulkp_sourcep=(((3.0*t0_Vbulkp_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vbulkp_sourcep));
t1_Vsourceb_sourcep=(((3.0*t0_Vsourceb_sourcep)*t4)+((1.0+(3.0*t0))*t4_Vsourceb_sourcep));
#endif
t1=((1.0+(3.0*t0))*t4);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(((dvt2w*t4_Vdrainp_sourcep)*t4)+((dvt2w*t4)*t4_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(((dvt2w*t4_Vdrainb_sourcep)*t4)+((dvt2w*t4)*t4_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(((dvt2w*t4_Vbulkp_sourcep)*t4)+((dvt2w*t4)*t4_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(((dvt2w*t4_Vsourceb_sourcep)*t4)+((dvt2w*t4)*t4_Vsourceb_sourcep));
#endif
t2=((dvt2w*t4)*t4);
}
#if defined(_DERIVATE)
ltw_Vdrainp_sourcep=(((factor1*t3_Vdrainp_sourcep)*t1)+((factor1*t3)*t1_Vdrainp_sourcep));
ltw_Vdrainb_sourcep=(((factor1*t3_Vdrainb_sourcep)*t1)+((factor1*t3)*t1_Vdrainb_sourcep));
ltw_Vbulkp_sourcep=(((factor1*t3_Vbulkp_sourcep)*t1)+((factor1*t3)*t1_Vbulkp_sourcep));
ltw_Vsourceb_sourcep=(((factor1*t3_Vsourceb_sourcep)*t1)+((factor1*t3)*t1_Vsourceb_sourcep));
#endif
ltw=((factor1*t3)*t1);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(-(dvt1*leff)*lt1_Vdrainp_sourcep/lt1/lt1);
t0_Vdrainb_sourcep=(-(dvt1*leff)*lt1_Vdrainb_sourcep/lt1/lt1);
t0_Vbulkp_sourcep=(-(dvt1*leff)*lt1_Vbulkp_sourcep/lt1/lt1);
t0_Vsourceb_sourcep=(-(dvt1*leff)*lt1_Vsourceb_sourcep/lt1/lt1);
#endif
t0=((dvt1*leff)/lt1);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=t0_Vdrainp_sourcep*d10_exp0;
t1_Vdrainb_sourcep=t0_Vdrainb_sourcep*d10_exp0;
t1_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
t1_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=t1_Vdrainp_sourcep;
t2_Vdrainb_sourcep=t1_Vdrainb_sourcep;
t2_Vbulkp_sourcep=t1_Vbulkp_sourcep;
t2_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep));
t3_Vdrainb_sourcep=((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep));
t3_Vbulkp_sourcep=((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep));
t3_Vsourceb_sourcep=((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep));
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=(t3_Vdrainp_sourcep+(2.0*t1_Vdrainp_sourcep)*1.713908431e-15);
t4_Vdrainb_sourcep=(t3_Vdrainb_sourcep+(2.0*t1_Vdrainb_sourcep)*1.713908431e-15);
t4_Vbulkp_sourcep=(t3_Vbulkp_sourcep+(2.0*t1_Vbulkp_sourcep)*1.713908431e-15);
t4_Vsourceb_sourcep=(t3_Vsourceb_sourcep+(2.0*t1_Vsourceb_sourcep)*1.713908431e-15);
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
#if defined(_DERIVATE)
theta0_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t4-t1*t4_Vdrainp_sourcep)/t4/t4);
theta0_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t4-t1*t4_Vdrainb_sourcep)/t4/t4);
theta0_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t4-t1*t4_Vbulkp_sourcep)/t4/t4);
theta0_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t4-t1*t4_Vsourceb_sourcep)/t4/t4);
#endif
theta0=(t1/t4);
}
else
{
#if defined(_DERIVATE)
theta0_Vdrainp_sourcep=0.0;
theta0_Vdrainb_sourcep=0.0;
theta0_Vbulkp_sourcep=0.0;
theta0_Vsourceb_sourcep=0.0;
#endif
theta0=(1.0/(5.834617425e14-2.0));
}
#if defined(_DERIVATE)
thetavth_Vdrainp_sourcep=(dvt0*theta0_Vdrainp_sourcep);
thetavth_Vdrainb_sourcep=(dvt0*theta0_Vdrainb_sourcep);
thetavth_Vbulkp_sourcep=(dvt0*theta0_Vbulkp_sourcep);
thetavth_Vsourceb_sourcep=(dvt0*theta0_Vsourceb_sourcep);
#endif
thetavth=(dvt0*theta0);
#if defined(_DERIVATE)
delt_vth_Vdrainp_sourcep=thetavth_Vdrainp_sourcep*v0;
delt_vth_Vdrainb_sourcep=thetavth_Vdrainb_sourcep*v0;
delt_vth_Vbulkp_sourcep=thetavth_Vbulkp_sourcep*v0;
delt_vth_Vsourceb_sourcep=thetavth_Vsourceb_sourcep*v0;
#endif
delt_vth=(thetavth*v0);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(-((dvt1w*weff)*leff)*ltw_Vdrainp_sourcep/ltw/ltw);
t0_Vdrainb_sourcep=(-((dvt1w*weff)*leff)*ltw_Vdrainb_sourcep/ltw/ltw);
t0_Vbulkp_sourcep=(-((dvt1w*weff)*leff)*ltw_Vbulkp_sourcep/ltw/ltw);
t0_Vsourceb_sourcep=(-((dvt1w*weff)*leff)*ltw_Vsourceb_sourcep/ltw/ltw);
#endif
t0=(((dvt1w*weff)*leff)/ltw);
if
((t0<34.0))
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=t0_Vdrainp_sourcep*d10_exp0;
t1_Vdrainb_sourcep=t0_Vdrainb_sourcep*d10_exp0;
t1_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
t1_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
#endif
t1=d00_exp0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=t1_Vdrainp_sourcep;
t2_Vdrainb_sourcep=t1_Vdrainb_sourcep;
t2_Vbulkp_sourcep=t1_Vbulkp_sourcep;
t2_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
t2=(t1-1.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep));
t3_Vdrainb_sourcep=((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep));
t3_Vbulkp_sourcep=((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep));
t3_Vsourceb_sourcep=((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep));
#endif
t3=(t2*t2);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=(t3_Vdrainp_sourcep+(2.0*t1_Vdrainp_sourcep)*1.713908431e-15);
t4_Vdrainb_sourcep=(t3_Vdrainb_sourcep+(2.0*t1_Vdrainb_sourcep)*1.713908431e-15);
t4_Vbulkp_sourcep=(t3_Vbulkp_sourcep+(2.0*t1_Vbulkp_sourcep)*1.713908431e-15);
t4_Vsourceb_sourcep=(t3_Vsourceb_sourcep+(2.0*t1_Vsourceb_sourcep)*1.713908431e-15);
#endif
t4=(t3+((2.0*t1)*1.713908431e-15));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t4-t1*t4_Vdrainp_sourcep)/t4/t4);
t5_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t4-t1*t4_Vdrainb_sourcep)/t4/t4);
t5_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t4-t1*t4_Vbulkp_sourcep)/t4/t4);
t5_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t4-t1*t4_Vsourceb_sourcep)/t4/t4);
#endif
t5=(t1/t4);
}
else
{
#if defined(_DERIVATE)
t5_Vgatep_sourcep=0.0;
t5_Vdrainp_sourcep=0.0;
t5_Vdrainb_sourcep=0.0;
t5_Vbulkp_sourcep=0.0;
t5_Vsourceb_sourcep=0.0;
#endif
t5=(1.0/(5.834617425e14-2.0));
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(dvt0w*t5_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(dvt0w*t5_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(dvt0w*t5_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(dvt0w*t5_Vsourceb_sourcep);
#endif
t0=(dvt0w*t5);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=t0_Vdrainp_sourcep*v0;
t2_Vdrainb_sourcep=t0_Vdrainb_sourcep*v0;
t2_Vbulkp_sourcep=t0_Vbulkp_sourcep*v0;
t2_Vsourceb_sourcep=t0_Vsourceb_sourcep*v0;
#endif
t2=(t0*v0);
tratio=((Temp/tnom)-1.0);
{
double m00_sqrt(d00_sqrt0,(1.0+(lpe0/leff)))
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=d00_sqrt0;
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=((k1ox*t0_Vdrainp_sourcep)*sqrtphi+(kt2*vbseff_Vdrainp_sourcep)*tratio);
t1_Vdrainb_sourcep=((k1ox*t0_Vdrainb_sourcep)*sqrtphi+(kt2*vbseff_Vdrainb_sourcep)*tratio);
t1_Vbulkp_sourcep=((k1ox*t0_Vbulkp_sourcep)*sqrtphi+(kt2*vbseff_Vbulkp_sourcep)*tratio);
t1_Vsourceb_sourcep=((k1ox*t0_Vsourceb_sourcep)*sqrtphi+(kt2*vbseff_Vsourceb_sourcep)*tratio);
#endif
t1=(((k1ox*(t0-1.0))*sqrtphi)+(((kt1+(kt1l/leff))+(kt2*vbseff))*tratio));
vth_narroww=((toxe*phi)/(weff+w0));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=(etab*vbseff_Vdrainp_sourcep);
t3_Vdrainb_sourcep=(etab*vbseff_Vdrainb_sourcep);
t3_Vbulkp_sourcep=(etab*vbseff_Vbulkp_sourcep);
t3_Vsourceb_sourcep=(etab*vbseff_Vsourceb_sourcep);
#endif
t3=(eta0+(etab*vbseff));
if
((t3<1.0e-4))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=(-(-(2.0e4*t3_Vdrainp_sourcep))/(3.0-(2.0e4*t3))/(3.0-(2.0e4*t3)));
t9_Vdrainb_sourcep=(-(-(2.0e4*t3_Vdrainb_sourcep))/(3.0-(2.0e4*t3))/(3.0-(2.0e4*t3)));
t9_Vbulkp_sourcep=(-(-(2.0e4*t3_Vbulkp_sourcep))/(3.0-(2.0e4*t3))/(3.0-(2.0e4*t3)));
t9_Vsourceb_sourcep=(-(-(2.0e4*t3_Vsourceb_sourcep))/(3.0-(2.0e4*t3))/(3.0-(2.0e4*t3)));
#endif
t9=(1.0/(3.0-(2.0e4*t3)));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=(((-t3_Vdrainp_sourcep)*t9)+((2.0e-4-t3)*t9_Vdrainp_sourcep));
t3_Vdrainb_sourcep=(((-t3_Vdrainb_sourcep)*t9)+((2.0e-4-t3)*t9_Vdrainb_sourcep));
t3_Vbulkp_sourcep=(((-t3_Vbulkp_sourcep)*t9)+((2.0e-4-t3)*t9_Vbulkp_sourcep));
t3_Vsourceb_sourcep=(((-t3_Vsourceb_sourcep)*t9)+((2.0e-4-t3)*t9_Vsourceb_sourcep));
#endif
t3=((2.0e-4-t3)*t9);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=((t9_Vdrainp_sourcep*t9)+(t9*t9_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((t9_Vdrainb_sourcep*t9)+(t9*t9_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((t9_Vbulkp_sourcep*t9)+(t9*t9_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((t9_Vsourceb_sourcep*t9)+(t9*t9_Vsourceb_sourcep));
#endif
t4=(t9*t9);
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=1.0;
}
#if defined(_DERIVATE)
ddibl_sft_dvd_Vdrainp_sourcep=t3_Vdrainp_sourcep*theta0vb0;
ddibl_sft_dvd_Vdrainb_sourcep=t3_Vdrainb_sourcep*theta0vb0;
ddibl_sft_dvd_Vbulkp_sourcep=t3_Vbulkp_sourcep*theta0vb0;
ddibl_sft_dvd_Vsourceb_sourcep=t3_Vsourceb_sourcep*theta0vb0;
#endif
ddibl_sft_dvd=(t3*theta0vb0);
#if defined(_DERIVATE)
dibl_sft_Vdrainp_sourcep=((ddibl_sft_dvd_Vdrainp_sourcep*Vds)+(ddibl_sft_dvd*Vds_Vdrainp_sourcep));
dibl_sft_Vdrainb_sourcep=ddibl_sft_dvd_Vdrainb_sourcep*Vds;
dibl_sft_Vbulkp_sourcep=ddibl_sft_dvd_Vbulkp_sourcep*Vds;
dibl_sft_Vsourceb_sourcep=ddibl_sft_dvd_Vsourceb_sourcep*Vds;
#endif
dibl_sft=(ddibl_sft_dvd*Vds);
{
double m00_sqrt(d00_sqrt0,(1.0+(lpeb/leff)))
lpe_vb=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,phi)
#if defined(_DERIVATE)
vth_Vdrainp_sourcep=(((((((k1ox*sqrtphis_Vdrainp_sourcep)*lpe_vb-(k2ox*vbseff_Vdrainp_sourcep))-delt_vth_Vdrainp_sourcep)-t2_Vdrainp_sourcep)+(k3b*vbseff_Vdrainp_sourcep)*vth_narroww)+t1_Vdrainp_sourcep)-dibl_sft_Vdrainp_sourcep);
vth_Vdrainb_sourcep=(((((((k1ox*sqrtphis_Vdrainb_sourcep)*lpe_vb-(k2ox*vbseff_Vdrainb_sourcep))-delt_vth_Vdrainb_sourcep)-t2_Vdrainb_sourcep)+(k3b*vbseff_Vdrainb_sourcep)*vth_narroww)+t1_Vdrainb_sourcep)-dibl_sft_Vdrainb_sourcep);
vth_Vbulkp_sourcep=(((((((k1ox*sqrtphis_Vbulkp_sourcep)*lpe_vb-(k2ox*vbseff_Vbulkp_sourcep))-delt_vth_Vbulkp_sourcep)-t2_Vbulkp_sourcep)+(k3b*vbseff_Vbulkp_sourcep)*vth_narroww)+t1_Vbulkp_sourcep)-dibl_sft_Vbulkp_sourcep);
vth_Vsourceb_sourcep=(((((((k1ox*sqrtphis_Vsourceb_sourcep)*lpe_vb-(k2ox*vbseff_Vsourceb_sourcep))-delt_vth_Vsourceb_sourcep)-t2_Vsourceb_sourcep)+(k3b*vbseff_Vsourceb_sourcep)*vth_narroww)+t1_Vsourceb_sourcep)-dibl_sft_Vsourceb_sourcep);
#endif
vth=((((((((type*vth0)+(((k1ox*sqrtphis)-(k1*d00_sqrt0))*lpe_vb))-(k2ox*vbseff))-delt_vth)-t2)+((k3+(k3b*vbseff))*vth_narroww))+t1)-dibl_sft);
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=(-1.03594e-10*xdep_Vdrainp_sourcep/xdep/xdep);
tmp1_Vdrainb_sourcep=(-1.03594e-10*xdep_Vdrainb_sourcep/xdep/xdep);
tmp1_Vbulkp_sourcep=(-1.03594e-10*xdep_Vbulkp_sourcep/xdep/xdep);
tmp1_Vsourceb_sourcep=(-1.03594e-10*xdep_Vsourceb_sourcep/xdep/xdep);
#endif
tmp1=(1.03594e-10/xdep);
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=0.0;
tmp2_Vdrainp_sourcep=(nfactor*tmp1_Vdrainp_sourcep);
tmp2_Vdrainb_sourcep=(nfactor*tmp1_Vdrainb_sourcep);
tmp2_Vbulkp_sourcep=(nfactor*tmp1_Vbulkp_sourcep);
tmp2_Vsourceb_sourcep=(nfactor*tmp1_Vsourceb_sourcep);
#endif
tmp2=(nfactor*tmp1);
#if defined(_DERIVATE)
tmp3_Vgatep_sourcep=0.0;
tmp3_Vdrainp_sourcep=((cdscb*vbseff_Vdrainp_sourcep)+(cdscd*Vds_Vdrainp_sourcep));
tmp3_Vdrainb_sourcep=(cdscb*vbseff_Vdrainb_sourcep);
tmp3_Vbulkp_sourcep=(cdscb*vbseff_Vbulkp_sourcep);
tmp3_Vsourceb_sourcep=(cdscb*vbseff_Vsourceb_sourcep);
#endif
tmp3=((cdsc+(cdscb*vbseff))+(cdscd*Vds));
#if defined(_DERIVATE)
tmp4_Vgatep_sourcep=0.0;
tmp4_Vdrainp_sourcep=((tmp2_Vdrainp_sourcep+((tmp3_Vdrainp_sourcep*theta0)+(tmp3*theta0_Vdrainp_sourcep)))/coxe);
tmp4_Vdrainb_sourcep=((tmp2_Vdrainb_sourcep+((tmp3_Vdrainb_sourcep*theta0)+(tmp3*theta0_Vdrainb_sourcep)))/coxe);
tmp4_Vbulkp_sourcep=((tmp2_Vbulkp_sourcep+((tmp3_Vbulkp_sourcep*theta0)+(tmp3*theta0_Vbulkp_sourcep)))/coxe);
tmp4_Vsourceb_sourcep=((tmp2_Vsourceb_sourcep+((tmp3_Vsourceb_sourcep*theta0)+(tmp3*theta0_Vsourceb_sourcep)))/coxe);
#endif
tmp4=(((tmp2+(tmp3*theta0))+cit)/coxe);
if
((tmp4>=(-0.5)))
{
#if defined(_DERIVATE)
n_Vdrainp_sourcep=tmp4_Vdrainp_sourcep;
n_Vdrainb_sourcep=tmp4_Vdrainb_sourcep;
n_Vbulkp_sourcep=tmp4_Vbulkp_sourcep;
n_Vsourceb_sourcep=tmp4_Vsourceb_sourcep;
#endif
n=(1.0+tmp4);
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(-(8.0*tmp4_Vdrainp_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
t0_Vdrainb_sourcep=(-(8.0*tmp4_Vdrainb_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
t0_Vbulkp_sourcep=(-(8.0*tmp4_Vbulkp_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
t0_Vsourceb_sourcep=(-(8.0*tmp4_Vsourceb_sourcep)/(3.0+(8.0*tmp4))/(3.0+(8.0*tmp4)));
#endif
t0=(1.0/(3.0+(8.0*tmp4)));
#if defined(_DERIVATE)
n_Vdrainp_sourcep=(((3.0*tmp4_Vdrainp_sourcep)*t0)+((1.0+(3.0*tmp4))*t0_Vdrainp_sourcep));
n_Vdrainb_sourcep=(((3.0*tmp4_Vdrainb_sourcep)*t0)+((1.0+(3.0*tmp4))*t0_Vdrainb_sourcep));
n_Vbulkp_sourcep=(((3.0*tmp4_Vbulkp_sourcep)*t0)+((1.0+(3.0*tmp4))*t0_Vbulkp_sourcep));
n_Vsourceb_sourcep=(((3.0*tmp4_Vsourceb_sourcep)*t0)+((1.0+(3.0*tmp4))*t0_Vsourceb_sourcep));
#endif
n=((1.0+(3.0*tmp4))*t0);
}
if
((dvtp0>0.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=((-dvtp1)*Vds_Vdrainp_sourcep);
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((-dvtp1)*Vds);
if
((t0<(-34.0)))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=t0_Vdrainp_sourcep*d10_exp0;
t2_Vdrainb_sourcep=t0_Vdrainb_sourcep*d10_exp0;
t2_Vbulkp_sourcep=t0_Vbulkp_sourcep*d10_exp0;
t2_Vsourceb_sourcep=t0_Vsourceb_sourcep*d10_exp0;
#endif
t2=d00_exp0;
}
}
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=(dvtp0*t2_Vdrainp_sourcep);
t3_Vdrainb_sourcep=(dvtp0*t2_Vdrainb_sourcep);
t3_Vbulkp_sourcep=(dvtp0*t2_Vbulkp_sourcep);
t3_Vsourceb_sourcep=(dvtp0*t2_Vsourceb_sourcep);
#endif
t3=(leff+(dvtp0*(1.0+t2)));
{
double m00_logE(d00_logE0,(leff/t3))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(leff/t3))
#endif
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=(vtm*(-leff*t3_Vdrainp_sourcep/t3/t3)*d10_logE0);
t4_Vdrainb_sourcep=(vtm*(-leff*t3_Vdrainb_sourcep/t3/t3)*d10_logE0);
t4_Vbulkp_sourcep=(vtm*(-leff*t3_Vbulkp_sourcep/t3/t3)*d10_logE0);
t4_Vsourceb_sourcep=(vtm*(-leff*t3_Vsourceb_sourcep/t3/t3)*d10_logE0);
#endif
t4=(vtm*d00_logE0);
}
#if defined(_DERIVATE)
vth_Vdrainp_sourcep=(vth_Vdrainp_sourcep-((n_Vdrainp_sourcep*t4)+(n*t4_Vdrainp_sourcep)));
vth_Vdrainb_sourcep=(vth_Vdrainb_sourcep-((n_Vdrainb_sourcep*t4)+(n*t4_Vdrainb_sourcep)));
vth_Vbulkp_sourcep=(vth_Vbulkp_sourcep-((n_Vbulkp_sourcep*t4)+(n*t4_Vbulkp_sourcep)));
vth_Vsourceb_sourcep=(vth_Vsourceb_sourcep-((n_Vsourceb_sourcep*t4)+(n*t4_Vsourceb_sourcep)));
#endif
vth=(vth-(n*t4));
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vfb+phi);
if
((((ngate>1.0e18)&&(ngate<1.0e25))&&(vgs>t0)))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=((((1.0e6*1.6021918e-19)*1.03594e-10)*ngate)/(coxe*coxe));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=(vgs_Vgatep_sourcep-t0_Vgatep_sourcep);
t8_Vdrainp_sourcep=(-t0_Vdrainp_sourcep);
t8_Vdrainb_sourcep=(-t0_Vdrainb_sourcep);
t8_Vbulkp_sourcep=(-t0_Vbulkp_sourcep);
t8_Vsourceb_sourcep=(-t0_Vsourceb_sourcep);
#endif
t8=(vgs-t0);
{
double m00_sqrt(d00_sqrt0,(1.0+((2.0*t8)/t1)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+((2.0*t8)/t1)))
#endif
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(((2.0*t8_Vgatep_sourcep)*t1-(2.0*t8)*t1_Vgatep_sourcep)/t1/t1)*d10_sqrt0;
t4_Vdrainp_sourcep=(((2.0*t8_Vdrainp_sourcep)*t1-(2.0*t8)*t1_Vdrainp_sourcep)/t1/t1)*d10_sqrt0;
t4_Vdrainb_sourcep=(((2.0*t8_Vdrainb_sourcep)*t1-(2.0*t8)*t1_Vdrainb_sourcep)/t1/t1)*d10_sqrt0;
t4_Vbulkp_sourcep=(((2.0*t8_Vbulkp_sourcep)*t1-(2.0*t8)*t1_Vbulkp_sourcep)/t1/t1)*d10_sqrt0;
t4_Vsourceb_sourcep=(((2.0*t8_Vsourceb_sourcep)*t1-(2.0*t8)*t1_Vsourceb_sourcep)/t1/t1)*d10_sqrt0;
#endif
t4=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((2.0*t8_Vgatep_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vgatep_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vdrainp_sourcep=(((2.0*t8_Vdrainp_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vdrainp_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vdrainb_sourcep=(((2.0*t8_Vdrainb_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vdrainb_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vbulkp_sourcep=(((2.0*t8_Vbulkp_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vbulkp_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vsourceb_sourcep=(((2.0*t8_Vsourceb_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vsourceb_sourcep)/(t4+1.0)/(t4+1.0));
#endif
t2=((2.0*t8)/(t4+1.0));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((((0.5*t2_Vgatep_sourcep)*t2)+((0.5*t2)*t2_Vgatep_sourcep))*t1-((0.5*t2)*t2)*t1_Vgatep_sourcep)/t1/t1);
t3_Vdrainp_sourcep=(((((0.5*t2_Vdrainp_sourcep)*t2)+((0.5*t2)*t2_Vdrainp_sourcep))*t1-((0.5*t2)*t2)*t1_Vdrainp_sourcep)/t1/t1);
t3_Vdrainb_sourcep=(((((0.5*t2_Vdrainb_sourcep)*t2)+((0.5*t2)*t2_Vdrainb_sourcep))*t1-((0.5*t2)*t2)*t1_Vdrainb_sourcep)/t1/t1);
t3_Vbulkp_sourcep=(((((0.5*t2_Vbulkp_sourcep)*t2)+((0.5*t2)*t2_Vbulkp_sourcep))*t1-((0.5*t2)*t2)*t1_Vbulkp_sourcep)/t1/t1);
t3_Vsourceb_sourcep=(((((0.5*t2_Vsourceb_sourcep)*t2)+((0.5*t2)*t2_Vsourceb_sourcep))*t1-((0.5*t2)*t2)*t1_Vsourceb_sourcep)/t1/t1);
#endif
t3=(((0.5*t2)*t2)/t1);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(-t3_Vgatep_sourcep);
t7_Vdrainp_sourcep=(-t3_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(-t3_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(-t3_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(-t3_Vsourceb_sourcep);
#endif
t7=((1.12-t3)-0.05);
{
double m00_sqrt(d00_sqrt0,((t7*t7)+0.224))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t7*t7)+0.224))
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((t7_Vgatep_sourcep*t7)+(t7*t7_Vgatep_sourcep))*d10_sqrt0;
t6_Vdrainp_sourcep=((t7_Vdrainp_sourcep*t7)+(t7*t7_Vdrainp_sourcep))*d10_sqrt0;
t6_Vdrainb_sourcep=((t7_Vdrainb_sourcep*t7)+(t7*t7_Vdrainb_sourcep))*d10_sqrt0;
t6_Vbulkp_sourcep=((t7_Vbulkp_sourcep*t7)+(t7*t7_Vbulkp_sourcep))*d10_sqrt0;
t6_Vsourceb_sourcep=((t7_Vsourceb_sourcep*t7)+(t7*t7_Vsourceb_sourcep))*d10_sqrt0;
#endif
t6=d00_sqrt0;
}
#if defined(_DERIVATE)
t5_Vgatep_sourcep=(-(0.5*(t7_Vgatep_sourcep+t6_Vgatep_sourcep)));
t5_Vdrainp_sourcep=(-(0.5*(t7_Vdrainp_sourcep+t6_Vdrainp_sourcep)));
t5_Vdrainb_sourcep=(-(0.5*(t7_Vdrainb_sourcep+t6_Vdrainb_sourcep)));
t5_Vbulkp_sourcep=(-(0.5*(t7_Vbulkp_sourcep+t6_Vbulkp_sourcep)));
t5_Vsourceb_sourcep=(-(0.5*(t7_Vsourceb_sourcep+t6_Vsourceb_sourcep)));
#endif
t5=(1.12-(0.5*(t7+t6)));
#if defined(_DERIVATE)
vgs_eff_Vgatep_sourcep=(vgs_Vgatep_sourcep-t5_Vgatep_sourcep);
vgs_eff_Vdrainp_sourcep=(-t5_Vdrainp_sourcep);
vgs_eff_Vdrainb_sourcep=(-t5_Vdrainb_sourcep);
vgs_eff_Vbulkp_sourcep=(-t5_Vbulkp_sourcep);
vgs_eff_Vsourceb_sourcep=(-t5_Vsourceb_sourcep);
#endif
vgs_eff=(vgs-t5);
}
else
{
#if defined(_DERIVATE)
vgs_eff_Vgatep_sourcep=vgs_Vgatep_sourcep;
vgs_eff_Vdrainp_sourcep=0.0;
vgs_eff_Vdrainb_sourcep=0.0;
vgs_eff_Vbulkp_sourcep=0.0;
vgs_eff_Vsourceb_sourcep=0.0;
#endif
vgs_eff=vgs;
}
if
((((ngate>1.0e18)&&(ngate<1.0e25))&&(vgd>t0)))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=((((1.0e6*1.6021918e-19)*1.03594e-10)*ngate)/(coxe*coxe));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=(vgd_Vgatep_sourcep-t0_Vgatep_sourcep);
t8_Vdrainp_sourcep=(vgd_Vdrainp_sourcep-t0_Vdrainp_sourcep);
t8_Vdrainb_sourcep=(-t0_Vdrainb_sourcep);
t8_Vbulkp_sourcep=(-t0_Vbulkp_sourcep);
t8_Vsourceb_sourcep=(-t0_Vsourceb_sourcep);
#endif
t8=(vgd-t0);
{
double m00_sqrt(d00_sqrt0,(1.0+((2.0*t8)/t1)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+((2.0*t8)/t1)))
#endif
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(((2.0*t8_Vgatep_sourcep)*t1-(2.0*t8)*t1_Vgatep_sourcep)/t1/t1)*d10_sqrt0;
t4_Vdrainp_sourcep=(((2.0*t8_Vdrainp_sourcep)*t1-(2.0*t8)*t1_Vdrainp_sourcep)/t1/t1)*d10_sqrt0;
t4_Vdrainb_sourcep=(((2.0*t8_Vdrainb_sourcep)*t1-(2.0*t8)*t1_Vdrainb_sourcep)/t1/t1)*d10_sqrt0;
t4_Vbulkp_sourcep=(((2.0*t8_Vbulkp_sourcep)*t1-(2.0*t8)*t1_Vbulkp_sourcep)/t1/t1)*d10_sqrt0;
t4_Vsourceb_sourcep=(((2.0*t8_Vsourceb_sourcep)*t1-(2.0*t8)*t1_Vsourceb_sourcep)/t1/t1)*d10_sqrt0;
#endif
t4=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((2.0*t8_Vgatep_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vgatep_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vdrainp_sourcep=(((2.0*t8_Vdrainp_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vdrainp_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vdrainb_sourcep=(((2.0*t8_Vdrainb_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vdrainb_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vbulkp_sourcep=(((2.0*t8_Vbulkp_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vbulkp_sourcep)/(t4+1.0)/(t4+1.0));
t2_Vsourceb_sourcep=(((2.0*t8_Vsourceb_sourcep)*(t4+1.0)-(2.0*t8)*t4_Vsourceb_sourcep)/(t4+1.0)/(t4+1.0));
#endif
t2=((2.0*t8)/(t4+1.0));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((((0.5*t2_Vgatep_sourcep)*t2)+((0.5*t2)*t2_Vgatep_sourcep))*t1-((0.5*t2)*t2)*t1_Vgatep_sourcep)/t1/t1);
t3_Vdrainp_sourcep=(((((0.5*t2_Vdrainp_sourcep)*t2)+((0.5*t2)*t2_Vdrainp_sourcep))*t1-((0.5*t2)*t2)*t1_Vdrainp_sourcep)/t1/t1);
t3_Vdrainb_sourcep=(((((0.5*t2_Vdrainb_sourcep)*t2)+((0.5*t2)*t2_Vdrainb_sourcep))*t1-((0.5*t2)*t2)*t1_Vdrainb_sourcep)/t1/t1);
t3_Vbulkp_sourcep=(((((0.5*t2_Vbulkp_sourcep)*t2)+((0.5*t2)*t2_Vbulkp_sourcep))*t1-((0.5*t2)*t2)*t1_Vbulkp_sourcep)/t1/t1);
t3_Vsourceb_sourcep=(((((0.5*t2_Vsourceb_sourcep)*t2)+((0.5*t2)*t2_Vsourceb_sourcep))*t1-((0.5*t2)*t2)*t1_Vsourceb_sourcep)/t1/t1);
#endif
t3=(((0.5*t2)*t2)/t1);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(-t3_Vgatep_sourcep);
t7_Vdrainp_sourcep=(-t3_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(-t3_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(-t3_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(-t3_Vsourceb_sourcep);
#endif
t7=((1.12-t3)-0.05);
{
double m00_sqrt(d00_sqrt0,((t7*t7)+0.224))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t7*t7)+0.224))
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((t7_Vgatep_sourcep*t7)+(t7*t7_Vgatep_sourcep))*d10_sqrt0;
t6_Vdrainp_sourcep=((t7_Vdrainp_sourcep*t7)+(t7*t7_Vdrainp_sourcep))*d10_sqrt0;
t6_Vdrainb_sourcep=((t7_Vdrainb_sourcep*t7)+(t7*t7_Vdrainb_sourcep))*d10_sqrt0;
t6_Vbulkp_sourcep=((t7_Vbulkp_sourcep*t7)+(t7*t7_Vbulkp_sourcep))*d10_sqrt0;
t6_Vsourceb_sourcep=((t7_Vsourceb_sourcep*t7)+(t7*t7_Vsourceb_sourcep))*d10_sqrt0;
#endif
t6=d00_sqrt0;
}
#if defined(_DERIVATE)
t5_Vgatep_sourcep=(-(0.5*(t7_Vgatep_sourcep+t6_Vgatep_sourcep)));
t5_Vdrainp_sourcep=(-(0.5*(t7_Vdrainp_sourcep+t6_Vdrainp_sourcep)));
t5_Vdrainb_sourcep=(-(0.5*(t7_Vdrainb_sourcep+t6_Vdrainb_sourcep)));
t5_Vbulkp_sourcep=(-(0.5*(t7_Vbulkp_sourcep+t6_Vbulkp_sourcep)));
t5_Vsourceb_sourcep=(-(0.5*(t7_Vsourceb_sourcep+t6_Vsourceb_sourcep)));
#endif
t5=(1.12-(0.5*(t7+t6)));
#if defined(_DERIVATE)
vgd_eff_Vgatep_sourcep=(vgd_Vgatep_sourcep-t5_Vgatep_sourcep);
vgd_eff_Vdrainp_sourcep=(vgd_Vdrainp_sourcep-t5_Vdrainp_sourcep);
vgd_eff_Vdrainb_sourcep=(-t5_Vdrainb_sourcep);
vgd_eff_Vbulkp_sourcep=(-t5_Vbulkp_sourcep);
vgd_eff_Vsourceb_sourcep=(-t5_Vsourceb_sourcep);
#endif
vgd_eff=(vgd-t5);
}
else
{
#if defined(_DERIVATE)
vgd_eff_Vgatep_sourcep=vgd_Vgatep_sourcep;
vgd_eff_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
vgd_eff_Vdrainb_sourcep=0.0;
vgd_eff_Vbulkp_sourcep=0.0;
vgd_eff_Vsourceb_sourcep=0.0;
#endif
vgd_eff=vgd;
}
if
((mode>0))
{
#if defined(_DERIVATE)
Vgs_eff_Vgatep_sourcep=vgs_eff_Vgatep_sourcep;
Vgs_eff_Vdrainp_sourcep=vgs_eff_Vdrainp_sourcep;
Vgs_eff_Vdrainb_sourcep=vgs_eff_Vdrainb_sourcep;
Vgs_eff_Vbulkp_sourcep=vgs_eff_Vbulkp_sourcep;
Vgs_eff_Vsourceb_sourcep=vgs_eff_Vsourceb_sourcep;
#endif
Vgs_eff=vgs_eff;
}
else
{
#if defined(_DERIVATE)
Vgs_eff_Vgatep_sourcep=vgd_eff_Vgatep_sourcep;
Vgs_eff_Vdrainp_sourcep=vgd_eff_Vdrainp_sourcep;
Vgs_eff_Vdrainb_sourcep=vgd_eff_Vdrainb_sourcep;
Vgs_eff_Vbulkp_sourcep=vgd_eff_Vbulkp_sourcep;
Vgs_eff_Vsourceb_sourcep=vgd_eff_Vsourceb_sourcep;
#endif
Vgs_eff=vgd_eff;
}
#if defined(_DERIVATE)
vgst_Vgatep_sourcep=Vgs_eff_Vgatep_sourcep;
vgst_Vdrainp_sourcep=(Vgs_eff_Vdrainp_sourcep-vth_Vdrainp_sourcep);
vgst_Vdrainb_sourcep=(Vgs_eff_Vdrainb_sourcep-vth_Vdrainb_sourcep);
vgst_Vbulkp_sourcep=(Vgs_eff_Vbulkp_sourcep-vth_Vbulkp_sourcep);
vgst_Vsourceb_sourcep=(Vgs_eff_Vsourceb_sourcep-vth_Vsourceb_sourcep);
#endif
vgst=(Vgs_eff-vth);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=n_Vdrainp_sourcep*vtm;
t0_Vdrainb_sourcep=n_Vdrainb_sourcep*vtm;
t0_Vbulkp_sourcep=n_Vbulkp_sourcep*vtm;
t0_Vsourceb_sourcep=n_Vsourceb_sourcep*vtm;
#endif
t0=(n*vtm);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(mstar*vgst_Vgatep_sourcep);
t1_Vdrainp_sourcep=(mstar*vgst_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(mstar*vgst_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(mstar*vgst_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(mstar*vgst_Vsourceb_sourcep);
#endif
t1=(mstar*vgst);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t1_Vgatep_sourcep*t0-t1*t0_Vgatep_sourcep)/t0/t0);
t2_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t0-t1*t0_Vdrainp_sourcep)/t0/t0);
t2_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t0-t1*t0_Vdrainb_sourcep)/t0/t0);
t2_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t0-t1*t0_Vbulkp_sourcep)/t0/t0);
t2_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t0-t1*t0_Vsourceb_sourcep)/t0/t0);
#endif
t2=(t1/t0);
if
((t2>34.0))
{
#if defined(_DERIVATE)
t10_Vgatep_sourcep=t1_Vgatep_sourcep;
t10_Vdrainp_sourcep=t1_Vdrainp_sourcep;
t10_Vdrainb_sourcep=t1_Vdrainb_sourcep;
t10_Vbulkp_sourcep=t1_Vbulkp_sourcep;
t10_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
t10=t1;
}
else
{
if
((t2<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
t10_Vgatep_sourcep=0.0;
t10_Vdrainp_sourcep=0.0;
t10_Vdrainb_sourcep=0.0;
t10_Vbulkp_sourcep=0.0;
t10_Vsourceb_sourcep=0.0;
#endif
t10=(vtm*d00_logE0);
}
#if defined(_DERIVATE)
t10_Vgatep_sourcep=t10_Vgatep_sourcep*n;
t10_Vdrainp_sourcep=((t10_Vdrainp_sourcep*n)+(t10*n_Vdrainp_sourcep));
t10_Vdrainb_sourcep=((t10_Vdrainb_sourcep*n)+(t10*n_Vdrainb_sourcep));
t10_Vbulkp_sourcep=((t10_Vbulkp_sourcep*n)+(t10*n_Vbulkp_sourcep));
t10_Vsourceb_sourcep=((t10_Vsourceb_sourcep*n)+(t10*n_Vsourceb_sourcep));
#endif
t10=(t10*n);
}
else
{
{
double m00_exp(d00_exp0,t2)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
expvgst_Vgatep_sourcep=t2_Vgatep_sourcep*d10_exp0;
expvgst_Vdrainp_sourcep=t2_Vdrainp_sourcep*d10_exp0;
expvgst_Vdrainb_sourcep=t2_Vdrainb_sourcep*d10_exp0;
expvgst_Vbulkp_sourcep=t2_Vbulkp_sourcep*d10_exp0;
expvgst_Vsourceb_sourcep=t2_Vsourceb_sourcep*d10_exp0;
#endif
expvgst=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+expvgst))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+expvgst))
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(vtm*expvgst_Vgatep_sourcep*d10_logE0);
t3_Vdrainp_sourcep=(vtm*expvgst_Vdrainp_sourcep*d10_logE0);
t3_Vdrainb_sourcep=(vtm*expvgst_Vdrainb_sourcep*d10_logE0);
t3_Vbulkp_sourcep=(vtm*expvgst_Vbulkp_sourcep*d10_logE0);
t3_Vsourceb_sourcep=(vtm*expvgst_Vsourceb_sourcep*d10_logE0);
#endif
t3=(vtm*d00_logE0);
}
#if defined(_DERIVATE)
t10_Vgatep_sourcep=(n*t3_Vgatep_sourcep);
t10_Vdrainp_sourcep=((n_Vdrainp_sourcep*t3)+(n*t3_Vdrainp_sourcep));
t10_Vdrainb_sourcep=((n_Vdrainb_sourcep*t3)+(n*t3_Vdrainb_sourcep));
t10_Vbulkp_sourcep=((n_Vbulkp_sourcep*t3)+(n*t3_Vbulkp_sourcep));
t10_Vsourceb_sourcep=((n_Vsourceb_sourcep*t3)+(n*t3_Vsourceb_sourcep));
#endif
t10=(n*t3);
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(-((1.0-mstar)*vgst_Vgatep_sourcep));
t1_Vdrainp_sourcep=(-((1.0-mstar)*vgst_Vdrainp_sourcep));
t1_Vdrainb_sourcep=(-((1.0-mstar)*vgst_Vdrainb_sourcep));
t1_Vbulkp_sourcep=(-((1.0-mstar)*vgst_Vbulkp_sourcep));
t1_Vsourceb_sourcep=(-((1.0-mstar)*vgst_Vsourceb_sourcep));
#endif
t1=(voffcbn-((1.0-mstar)*vgst));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t1_Vgatep_sourcep*t0-t1*t0_Vgatep_sourcep)/t0/t0);
t2_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t0-t1*t0_Vdrainp_sourcep)/t0/t0);
t2_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t0-t1*t0_Vdrainb_sourcep)/t0/t0);
t2_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t0-t1*t0_Vbulkp_sourcep)/t0/t0);
t2_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t0-t1*t0_Vsourceb_sourcep)/t0/t0);
#endif
t2=(t1/t0);
if
((t2<(-34.0)))
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((coxe*1.713908431e-15)/cdep0);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=t3_Vgatep_sourcep*n;
t9_Vdrainp_sourcep=((t3_Vdrainp_sourcep*n)+(t3*n_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((t3_Vdrainb_sourcep*n)+(t3*n_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((t3_Vbulkp_sourcep*n)+(t3*n_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((t3_Vsourceb_sourcep*n)+(t3*n_Vsourceb_sourcep));
#endif
t9=(mstar+(t3*n));
}
else
{
if
((t2>34.0))
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((coxe*5.834617425e14)/cdep0);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=t3_Vgatep_sourcep*n;
t9_Vdrainp_sourcep=((t3_Vdrainp_sourcep*n)+(t3*n_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((t3_Vdrainb_sourcep*n)+(t3*n_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((t3_Vbulkp_sourcep*n)+(t3*n_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((t3_Vsourceb_sourcep*n)+(t3*n_Vsourceb_sourcep));
#endif
t9=(mstar+(t3*n));
}
else
{
{
double m00_exp(d00_exp0,t2)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
expvgst_Vgatep_sourcep=t2_Vgatep_sourcep*d10_exp0;
expvgst_Vdrainp_sourcep=t2_Vdrainp_sourcep*d10_exp0;
expvgst_Vdrainb_sourcep=t2_Vdrainb_sourcep*d10_exp0;
expvgst_Vbulkp_sourcep=t2_Vbulkp_sourcep*d10_exp0;
expvgst_Vsourceb_sourcep=t2_Vsourceb_sourcep*d10_exp0;
#endif
expvgst=d00_exp0;
}
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(coxe/cdep0);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t3_Vgatep_sourcep*expvgst)+(t3*expvgst_Vgatep_sourcep));
t4_Vdrainp_sourcep=((t3_Vdrainp_sourcep*expvgst)+(t3*expvgst_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((t3_Vdrainb_sourcep*expvgst)+(t3*expvgst_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((t3_Vbulkp_sourcep*expvgst)+(t3*expvgst_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((t3_Vsourceb_sourcep*expvgst)+(t3*expvgst_Vsourceb_sourcep));
#endif
t4=(t3*expvgst);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((((t1_Vgatep_sourcep*t4)+(t1*t4_Vgatep_sourcep))*t0-(t1*t4)*t0_Vgatep_sourcep)/t0/t0);
t5_Vdrainp_sourcep=((((t1_Vdrainp_sourcep*t4)+(t1*t4_Vdrainp_sourcep))*t0-(t1*t4)*t0_Vdrainp_sourcep)/t0/t0);
t5_Vdrainb_sourcep=((((t1_Vdrainb_sourcep*t4)+(t1*t4_Vdrainb_sourcep))*t0-(t1*t4)*t0_Vdrainb_sourcep)/t0/t0);
t5_Vbulkp_sourcep=((((t1_Vbulkp_sourcep*t4)+(t1*t4_Vbulkp_sourcep))*t0-(t1*t4)*t0_Vbulkp_sourcep)/t0/t0);
t5_Vsourceb_sourcep=((((t1_Vsourceb_sourcep*t4)+(t1*t4_Vsourceb_sourcep))*t0-(t1*t4)*t0_Vsourceb_sourcep)/t0/t0);
#endif
t5=((t1*t4)/t0);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=(n*t4_Vgatep_sourcep);
t9_Vdrainp_sourcep=((n_Vdrainp_sourcep*t4)+(n*t4_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((n_Vdrainb_sourcep*t4)+(n*t4_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((n_Vbulkp_sourcep*t4)+(n*t4_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((n_Vsourceb_sourcep*t4)+(n*t4_Vsourceb_sourcep));
#endif
t9=(mstar+(n*t4));
}
}
#if defined(_DERIVATE)
vgsteff_Vgatep_sourcep=((t10_Vgatep_sourcep*t9-t10*t9_Vgatep_sourcep)/t9/t9);
vgsteff_Vdrainp_sourcep=((t10_Vdrainp_sourcep*t9-t10*t9_Vdrainp_sourcep)/t9/t9);
vgsteff_Vdrainb_sourcep=((t10_Vdrainb_sourcep*t9-t10*t9_Vdrainb_sourcep)/t9/t9);
vgsteff_Vbulkp_sourcep=((t10_Vbulkp_sourcep*t9-t10*t9_Vbulkp_sourcep)/t9/t9);
vgsteff_Vsourceb_sourcep=((t10_Vsourceb_sourcep*t9-t10*t9_Vsourceb_sourcep)/t9/t9);
#endif
vgsteff=(t10/t9);
{
double m00_sqrt(d00_sqrt0,phi)
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=sqrtphis_Vdrainp_sourcep;
t9_Vdrainb_sourcep=sqrtphis_Vdrainb_sourcep;
t9_Vbulkp_sourcep=sqrtphis_Vbulkp_sourcep;
t9_Vsourceb_sourcep=sqrtphis_Vsourceb_sourcep;
#endif
t9=(sqrtphis-d00_sqrt0);
}
#if defined(_DERIVATE)
Weff_Vgatep_sourcep=(-(2.0*((dwg*vgsteff_Vgatep_sourcep)+(dwb*t9_Vgatep_sourcep))));
Weff_Vdrainp_sourcep=(-(2.0*((dwg*vgsteff_Vdrainp_sourcep)+(dwb*t9_Vdrainp_sourcep))));
Weff_Vdrainb_sourcep=(-(2.0*((dwg*vgsteff_Vdrainb_sourcep)+(dwb*t9_Vdrainb_sourcep))));
Weff_Vbulkp_sourcep=(-(2.0*((dwg*vgsteff_Vbulkp_sourcep)+(dwb*t9_Vbulkp_sourcep))));
Weff_Vsourceb_sourcep=(-(2.0*((dwg*vgsteff_Vsourceb_sourcep)+(dwb*t9_Vsourceb_sourcep))));
#endif
Weff=(weff-(2.0*((dwg*vgsteff)+(dwb*t9))));
if
((Weff<2.0e-8))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-(-(2.0*Weff_Vgatep_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
t0_Vdrainp_sourcep=(-(-(2.0*Weff_Vdrainp_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
t0_Vdrainb_sourcep=(-(-(2.0*Weff_Vdrainb_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
t0_Vbulkp_sourcep=(-(-(2.0*Weff_Vbulkp_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
t0_Vsourceb_sourcep=(-(-(2.0*Weff_Vsourceb_sourcep))/(6.0e-8-(2.0*Weff))/(6.0e-8-(2.0*Weff)));
#endif
t0=(1.0/(6.0e-8-(2.0*Weff)));
#if defined(_DERIVATE)
Weff_Vgatep_sourcep=(((2.0e-8*(-Weff_Vgatep_sourcep))*t0)+((2.0e-8*(4.0e-8-Weff))*t0_Vgatep_sourcep));
Weff_Vdrainp_sourcep=(((2.0e-8*(-Weff_Vdrainp_sourcep))*t0)+((2.0e-8*(4.0e-8-Weff))*t0_Vdrainp_sourcep));
Weff_Vdrainb_sourcep=(((2.0e-8*(-Weff_Vdrainb_sourcep))*t0)+((2.0e-8*(4.0e-8-Weff))*t0_Vdrainb_sourcep));
Weff_Vbulkp_sourcep=(((2.0e-8*(-Weff_Vbulkp_sourcep))*t0)+((2.0e-8*(4.0e-8-Weff))*t0_Vbulkp_sourcep));
Weff_Vsourceb_sourcep=(((2.0e-8*(-Weff_Vsourceb_sourcep))*t0)+((2.0e-8*(4.0e-8-Weff))*t0_Vsourceb_sourcep));
#endif
Weff=((2.0e-8*(4.0e-8-Weff))*t0);
}
if
((rdsmod==1))
{
#if defined(_DERIVATE)
rds_Vgatep_sourcep=0.0;
rds_Vdrainp_sourcep=0.0;
rds_Vdrainb_sourcep=0.0;
rds_Vbulkp_sourcep=0.0;
rds_Vsourceb_sourcep=0.0;
#endif
rds=0.0;
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(prwg*vgsteff_Vgatep_sourcep);
t0_Vdrainp_sourcep=(prwg*vgsteff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(prwg*vgsteff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(prwg*vgsteff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(prwg*vgsteff_Vsourceb_sourcep);
#endif
t0=(1.0+(prwg*vgsteff));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(prwb*t9_Vgatep_sourcep);
t1_Vdrainp_sourcep=(prwb*t9_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(prwb*t9_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(prwb*t9_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(prwb*t9_Vsourceb_sourcep);
#endif
t1=(prwb*t9);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-t0_Vgatep_sourcep/t0/t0)+t1_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-t0_Vdrainp_sourcep/t0/t0)+t1_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-t0_Vdrainb_sourcep/t0/t0)+t1_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-t0_Vbulkp_sourcep/t0/t0)+t1_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-t0_Vsourceb_sourcep/t0/t0)+t1_Vsourceb_sourcep);
#endif
t2=((1.0/t0)+t1);
{
double m00_sqrt(d00_sqrt0,((t2*t2)+0.01))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t2*t2)+0.01))
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(t2_Vgatep_sourcep+((t2_Vgatep_sourcep*t2)+(t2*t2_Vgatep_sourcep))*d10_sqrt0);
t3_Vdrainp_sourcep=(t2_Vdrainp_sourcep+((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep))*d10_sqrt0);
t3_Vdrainb_sourcep=(t2_Vdrainb_sourcep+((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep))*d10_sqrt0);
t3_Vbulkp_sourcep=(t2_Vbulkp_sourcep+((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep))*d10_sqrt0);
t3_Vsourceb_sourcep=(t2_Vsourceb_sourcep+((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep))*d10_sqrt0);
#endif
t3=(t2+d00_sqrt0);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(rds0*0.5);
#if defined(_DERIVATE)
rds_Vgatep_sourcep=((t3_Vgatep_sourcep*t4)+(t3*t4_Vgatep_sourcep));
rds_Vdrainp_sourcep=((t3_Vdrainp_sourcep*t4)+(t3*t4_Vdrainp_sourcep));
rds_Vdrainb_sourcep=((t3_Vdrainb_sourcep*t4)+(t3*t4_Vdrainb_sourcep));
rds_Vbulkp_sourcep=((t3_Vbulkp_sourcep*t4)+(t3*t4_Vbulkp_sourcep));
rds_Vsourceb_sourcep=((t3_Vsourceb_sourcep*t4)+(t3*t4_Vsourceb_sourcep));
#endif
rds=(rdswmin+(t3*t4));
}
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=(-((0.5*k1ox)*lpe_vb)*sqrtphis_Vdrainp_sourcep/sqrtphis/sqrtphis);
t9_Vdrainb_sourcep=(-((0.5*k1ox)*lpe_vb)*sqrtphis_Vdrainb_sourcep/sqrtphis/sqrtphis);
t9_Vbulkp_sourcep=(-((0.5*k1ox)*lpe_vb)*sqrtphis_Vbulkp_sourcep/sqrtphis/sqrtphis);
t9_Vsourceb_sourcep=(-((0.5*k1ox)*lpe_vb)*sqrtphis_Vsourceb_sourcep/sqrtphis/sqrtphis);
#endif
t9=(((0.5*k1ox)*lpe_vb)/sqrtphis);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=t9_Vgatep_sourcep;
t1_Vdrainp_sourcep=t9_Vdrainp_sourcep;
t1_Vdrainb_sourcep=t9_Vdrainb_sourcep;
t1_Vbulkp_sourcep=t9_Vbulkp_sourcep;
t1_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
t1=((t9+k2ox)-(k3b*vth_narroww));
{
double m00_sqrt(d00_sqrt0,(xj*xdep))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(xj*xdep))
#endif
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=(xj*xdep_Vdrainp_sourcep)*d10_sqrt0;
t9_Vdrainb_sourcep=(xj*xdep_Vdrainb_sourcep)*d10_sqrt0;
t9_Vbulkp_sourcep=(xj*xdep_Vbulkp_sourcep)*d10_sqrt0;
t9_Vsourceb_sourcep=(xj*xdep_Vsourceb_sourcep)*d10_sqrt0;
#endif
t9=d00_sqrt0;
}
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=(2.0*t9_Vgatep_sourcep);
tmp1_Vdrainp_sourcep=(2.0*t9_Vdrainp_sourcep);
tmp1_Vdrainb_sourcep=(2.0*t9_Vdrainb_sourcep);
tmp1_Vbulkp_sourcep=(2.0*t9_Vbulkp_sourcep);
tmp1_Vsourceb_sourcep=(2.0*t9_Vsourceb_sourcep);
#endif
tmp1=(leff+(2.0*t9));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=(-leff*tmp1_Vgatep_sourcep/tmp1/tmp1);
t5_Vdrainp_sourcep=(-leff*tmp1_Vdrainp_sourcep/tmp1/tmp1);
t5_Vdrainb_sourcep=(-leff*tmp1_Vdrainb_sourcep/tmp1/tmp1);
t5_Vbulkp_sourcep=(-leff*tmp1_Vbulkp_sourcep/tmp1/tmp1);
t5_Vsourceb_sourcep=(-leff*tmp1_Vsourceb_sourcep/tmp1/tmp1);
#endif
t5=(leff/tmp1);
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=(a0*t5_Vgatep_sourcep);
tmp2_Vdrainp_sourcep=(a0*t5_Vdrainp_sourcep);
tmp2_Vdrainb_sourcep=(a0*t5_Vdrainb_sourcep);
tmp2_Vbulkp_sourcep=(a0*t5_Vbulkp_sourcep);
tmp2_Vsourceb_sourcep=(a0*t5_Vsourceb_sourcep);
#endif
tmp2=(a0*t5);
#if defined(_DERIVATE)
tmp3_Vgatep_sourcep=0.0;
tmp3_Vdrainp_sourcep=0.0;
tmp3_Vdrainb_sourcep=0.0;
tmp3_Vbulkp_sourcep=0.0;
tmp3_Vsourceb_sourcep=0.0;
#endif
tmp3=(weff+b1);
#if defined(_DERIVATE)
tmp4_Vgatep_sourcep=0.0;
tmp4_Vdrainp_sourcep=(-b0*tmp3_Vdrainp_sourcep/tmp3/tmp3);
tmp4_Vdrainb_sourcep=(-b0*tmp3_Vdrainb_sourcep/tmp3/tmp3);
tmp4_Vbulkp_sourcep=(-b0*tmp3_Vbulkp_sourcep/tmp3/tmp3);
tmp4_Vsourceb_sourcep=(-b0*tmp3_Vsourceb_sourcep/tmp3/tmp3);
#endif
tmp4=(b0/tmp3);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(tmp2_Vgatep_sourcep+tmp4_Vgatep_sourcep);
t2_Vdrainp_sourcep=(tmp2_Vdrainp_sourcep+tmp4_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(tmp2_Vdrainb_sourcep+tmp4_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(tmp2_Vbulkp_sourcep+tmp4_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(tmp2_Vsourceb_sourcep+tmp4_Vsourceb_sourcep);
#endif
t2=(tmp2+tmp4);
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((t5_Vgatep_sourcep*t5)+(t5*t5_Vgatep_sourcep));
t6_Vdrainp_sourcep=((t5_Vdrainp_sourcep*t5)+(t5*t5_Vdrainp_sourcep));
t6_Vdrainb_sourcep=((t5_Vdrainb_sourcep*t5)+(t5*t5_Vdrainb_sourcep));
t6_Vbulkp_sourcep=((t5_Vbulkp_sourcep*t5)+(t5*t5_Vbulkp_sourcep));
t6_Vsourceb_sourcep=((t5_Vsourceb_sourcep*t5)+(t5*t5_Vsourceb_sourcep));
#endif
t6=(t5*t5);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((t5_Vgatep_sourcep*t6)+(t5*t6_Vgatep_sourcep));
t7_Vdrainp_sourcep=((t5_Vdrainp_sourcep*t6)+(t5*t6_Vdrainp_sourcep));
t7_Vdrainb_sourcep=((t5_Vdrainb_sourcep*t6)+(t5*t6_Vdrainb_sourcep));
t7_Vbulkp_sourcep=((t5_Vbulkp_sourcep*t6)+(t5*t6_Vbulkp_sourcep));
t7_Vsourceb_sourcep=((t5_Vsourceb_sourcep*t6)+(t5*t6_Vsourceb_sourcep));
#endif
t7=(t5*t6);
#if defined(_DERIVATE)
abulk0_Vgatep_sourcep=((t1_Vgatep_sourcep*t2)+(t1*t2_Vgatep_sourcep));
abulk0_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t2)+(t1*t2_Vdrainp_sourcep));
abulk0_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t2)+(t1*t2_Vdrainb_sourcep));
abulk0_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t2)+(t1*t2_Vbulkp_sourcep));
abulk0_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t2)+(t1*t2_Vsourceb_sourcep));
#endif
abulk0=(1.0+(t1*t2));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((ags*a0)*t7_Vgatep_sourcep);
t8_Vdrainp_sourcep=((ags*a0)*t7_Vdrainp_sourcep);
t8_Vdrainb_sourcep=((ags*a0)*t7_Vdrainb_sourcep);
t8_Vbulkp_sourcep=((ags*a0)*t7_Vbulkp_sourcep);
t8_Vsourceb_sourcep=((ags*a0)*t7_Vsourceb_sourcep);
#endif
t8=((ags*a0)*t7);
#if defined(_DERIVATE)
dabulk_dvg_Vgatep_sourcep=(((-t1_Vgatep_sourcep)*t8)+((-t1)*t8_Vgatep_sourcep));
dabulk_dvg_Vdrainp_sourcep=(((-t1_Vdrainp_sourcep)*t8)+((-t1)*t8_Vdrainp_sourcep));
dabulk_dvg_Vdrainb_sourcep=(((-t1_Vdrainb_sourcep)*t8)+((-t1)*t8_Vdrainb_sourcep));
dabulk_dvg_Vbulkp_sourcep=(((-t1_Vbulkp_sourcep)*t8)+((-t1)*t8_Vbulkp_sourcep));
dabulk_dvg_Vsourceb_sourcep=(((-t1_Vsourceb_sourcep)*t8)+((-t1)*t8_Vsourceb_sourcep));
#endif
dabulk_dvg=((-t1)*t8);
#if defined(_DERIVATE)
abulk_Vgatep_sourcep=(abulk0_Vgatep_sourcep+((dabulk_dvg_Vgatep_sourcep*vgsteff)+(dabulk_dvg*vgsteff_Vgatep_sourcep)));
abulk_Vdrainp_sourcep=(abulk0_Vdrainp_sourcep+((dabulk_dvg_Vdrainp_sourcep*vgsteff)+(dabulk_dvg*vgsteff_Vdrainp_sourcep)));
abulk_Vdrainb_sourcep=(abulk0_Vdrainb_sourcep+((dabulk_dvg_Vdrainb_sourcep*vgsteff)+(dabulk_dvg*vgsteff_Vdrainb_sourcep)));
abulk_Vbulkp_sourcep=(abulk0_Vbulkp_sourcep+((dabulk_dvg_Vbulkp_sourcep*vgsteff)+(dabulk_dvg*vgsteff_Vbulkp_sourcep)));
abulk_Vsourceb_sourcep=(abulk0_Vsourceb_sourcep+((dabulk_dvg_Vsourceb_sourcep*vgsteff)+(dabulk_dvg*vgsteff_Vsourceb_sourcep)));
#endif
abulk=(abulk0+(dabulk_dvg*vgsteff));
if
((abulk0<0.1))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=(-(-(20.0*abulk0_Vgatep_sourcep))/(3.0-(20.0*abulk0))/(3.0-(20.0*abulk0)));
t9_Vdrainp_sourcep=(-(-(20.0*abulk0_Vdrainp_sourcep))/(3.0-(20.0*abulk0))/(3.0-(20.0*abulk0)));
t9_Vdrainb_sourcep=(-(-(20.0*abulk0_Vdrainb_sourcep))/(3.0-(20.0*abulk0))/(3.0-(20.0*abulk0)));
t9_Vbulkp_sourcep=(-(-(20.0*abulk0_Vbulkp_sourcep))/(3.0-(20.0*abulk0))/(3.0-(20.0*abulk0)));
t9_Vsourceb_sourcep=(-(-(20.0*abulk0_Vsourceb_sourcep))/(3.0-(20.0*abulk0))/(3.0-(20.0*abulk0)));
#endif
t9=(1.0/(3.0-(20.0*abulk0)));
#if defined(_DERIVATE)
abulk0_Vgatep_sourcep=(((-abulk0_Vgatep_sourcep)*t9)+((0.2-abulk0)*t9_Vgatep_sourcep));
abulk0_Vdrainp_sourcep=(((-abulk0_Vdrainp_sourcep)*t9)+((0.2-abulk0)*t9_Vdrainp_sourcep));
abulk0_Vdrainb_sourcep=(((-abulk0_Vdrainb_sourcep)*t9)+((0.2-abulk0)*t9_Vdrainb_sourcep));
abulk0_Vbulkp_sourcep=(((-abulk0_Vbulkp_sourcep)*t9)+((0.2-abulk0)*t9_Vbulkp_sourcep));
abulk0_Vsourceb_sourcep=(((-abulk0_Vsourceb_sourcep)*t9)+((0.2-abulk0)*t9_Vsourceb_sourcep));
#endif
abulk0=((0.2-abulk0)*t9);
}
if
((abulk<0.1))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=(-(-(20.0*abulk_Vgatep_sourcep))/(3.0-(20.0*abulk))/(3.0-(20.0*abulk)));
t9_Vdrainp_sourcep=(-(-(20.0*abulk_Vdrainp_sourcep))/(3.0-(20.0*abulk))/(3.0-(20.0*abulk)));
t9_Vdrainb_sourcep=(-(-(20.0*abulk_Vdrainb_sourcep))/(3.0-(20.0*abulk))/(3.0-(20.0*abulk)));
t9_Vbulkp_sourcep=(-(-(20.0*abulk_Vbulkp_sourcep))/(3.0-(20.0*abulk))/(3.0-(20.0*abulk)));
t9_Vsourceb_sourcep=(-(-(20.0*abulk_Vsourceb_sourcep))/(3.0-(20.0*abulk))/(3.0-(20.0*abulk)));
#endif
t9=(1.0/(3.0-(20.0*abulk)));
#if defined(_DERIVATE)
abulk_Vgatep_sourcep=(((-abulk_Vgatep_sourcep)*t9)+((0.2-abulk)*t9_Vgatep_sourcep));
abulk_Vdrainp_sourcep=(((-abulk_Vdrainp_sourcep)*t9)+((0.2-abulk)*t9_Vdrainp_sourcep));
abulk_Vdrainb_sourcep=(((-abulk_Vdrainb_sourcep)*t9)+((0.2-abulk)*t9_Vdrainb_sourcep));
abulk_Vbulkp_sourcep=(((-abulk_Vbulkp_sourcep)*t9)+((0.2-abulk)*t9_Vbulkp_sourcep));
abulk_Vsourceb_sourcep=(((-abulk_Vsourceb_sourcep)*t9)+((0.2-abulk)*t9_Vsourceb_sourcep));
#endif
abulk=((0.2-abulk)*t9);
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(keta*vbseff_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(keta*vbseff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(keta*vbseff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(keta*vbseff_Vsourceb_sourcep);
#endif
t2=(keta*vbseff);
if
((t2>=(-0.9)))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-t2_Vgatep_sourcep/(1.0+t2)/(1.0+t2));
t0_Vdrainp_sourcep=(-t2_Vdrainp_sourcep/(1.0+t2)/(1.0+t2));
t0_Vdrainb_sourcep=(-t2_Vdrainb_sourcep/(1.0+t2)/(1.0+t2));
t0_Vbulkp_sourcep=(-t2_Vbulkp_sourcep/(1.0+t2)/(1.0+t2));
t0_Vsourceb_sourcep=(-t2_Vsourceb_sourcep/(1.0+t2)/(1.0+t2));
#endif
t0=(1.0/(1.0+t2));
}
else
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(-t2_Vgatep_sourcep/(0.8+t2)/(0.8+t2));
t1_Vdrainp_sourcep=(-t2_Vdrainp_sourcep/(0.8+t2)/(0.8+t2));
t1_Vdrainb_sourcep=(-t2_Vdrainb_sourcep/(0.8+t2)/(0.8+t2));
t1_Vbulkp_sourcep=(-t2_Vbulkp_sourcep/(0.8+t2)/(0.8+t2));
t1_Vsourceb_sourcep=(-t2_Vsourceb_sourcep/(0.8+t2)/(0.8+t2));
#endif
t1=(1.0/(0.8+t2));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(((20.0*t2_Vgatep_sourcep)*t1)+((17.0+(20.0*t2))*t1_Vgatep_sourcep));
t0_Vdrainp_sourcep=(((20.0*t2_Vdrainp_sourcep)*t1)+((17.0+(20.0*t2))*t1_Vdrainp_sourcep));
t0_Vdrainb_sourcep=(((20.0*t2_Vdrainb_sourcep)*t1)+((17.0+(20.0*t2))*t1_Vdrainb_sourcep));
t0_Vbulkp_sourcep=(((20.0*t2_Vbulkp_sourcep)*t1)+((17.0+(20.0*t2))*t1_Vbulkp_sourcep));
t0_Vsourceb_sourcep=(((20.0*t2_Vsourceb_sourcep)*t1)+((17.0+(20.0*t2))*t1_Vsourceb_sourcep));
#endif
t0=((17.0+(20.0*t2))*t1);
}
#if defined(_DERIVATE)
abulk_Vgatep_sourcep=((abulk_Vgatep_sourcep*t0)+(abulk*t0_Vgatep_sourcep));
abulk_Vdrainp_sourcep=((abulk_Vdrainp_sourcep*t0)+(abulk*t0_Vdrainp_sourcep));
abulk_Vdrainb_sourcep=((abulk_Vdrainb_sourcep*t0)+(abulk*t0_Vdrainb_sourcep));
abulk_Vbulkp_sourcep=((abulk_Vbulkp_sourcep*t0)+(abulk*t0_Vbulkp_sourcep));
abulk_Vsourceb_sourcep=((abulk_Vsourceb_sourcep*t0)+(abulk*t0_Vsourceb_sourcep));
#endif
abulk=(abulk*t0);
#if defined(_DERIVATE)
abulk0_Vgatep_sourcep=((abulk0_Vgatep_sourcep*t0)+(abulk0*t0_Vgatep_sourcep));
abulk0_Vdrainp_sourcep=((abulk0_Vdrainp_sourcep*t0)+(abulk0*t0_Vdrainp_sourcep));
abulk0_Vdrainb_sourcep=((abulk0_Vdrainb_sourcep*t0)+(abulk0*t0_Vdrainb_sourcep));
abulk0_Vbulkp_sourcep=((abulk0_Vbulkp_sourcep*t0)+(abulk0*t0_Vbulkp_sourcep));
abulk0_Vsourceb_sourcep=((abulk0_Vsourceb_sourcep*t0)+(abulk0*t0_Vsourceb_sourcep));
#endif
abulk0=(abulk0*t0);
if
((mobmod==0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgsteff_Vgatep_sourcep;
t0_Vdrainp_sourcep=((vgsteff_Vdrainp_sourcep+vth_Vdrainp_sourcep)+vth_Vdrainp_sourcep);
t0_Vdrainb_sourcep=((vgsteff_Vdrainb_sourcep+vth_Vdrainb_sourcep)+vth_Vdrainb_sourcep);
t0_Vbulkp_sourcep=((vgsteff_Vbulkp_sourcep+vth_Vbulkp_sourcep)+vth_Vbulkp_sourcep);
t0_Vsourceb_sourcep=((vgsteff_Vsourceb_sourcep+vth_Vsourceb_sourcep)+vth_Vsourceb_sourcep);
#endif
t0=((vgsteff+vth)+vth);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(uc*vbseff_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(uc*vbseff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(uc*vbseff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(uc*vbseff_Vsourceb_sourcep);
#endif
t2=(ua+(uc*vbseff));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(t0_Vgatep_sourcep/toxe);
t3_Vdrainp_sourcep=(t0_Vdrainp_sourcep/toxe);
t3_Vdrainb_sourcep=(t0_Vdrainb_sourcep/toxe);
t3_Vbulkp_sourcep=(t0_Vbulkp_sourcep/toxe);
t3_Vsourceb_sourcep=(t0_Vsourceb_sourcep/toxe);
#endif
t3=(t0/toxe);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t3_Vgatep_sourcep*(t2+(ub*t3)))+(t3*(t2_Vgatep_sourcep+(ub*t3_Vgatep_sourcep))));
t5_Vdrainp_sourcep=((t3_Vdrainp_sourcep*(t2+(ub*t3)))+(t3*(t2_Vdrainp_sourcep+(ub*t3_Vdrainp_sourcep))));
t5_Vdrainb_sourcep=((t3_Vdrainb_sourcep*(t2+(ub*t3)))+(t3*(t2_Vdrainb_sourcep+(ub*t3_Vdrainb_sourcep))));
t5_Vbulkp_sourcep=((t3_Vbulkp_sourcep*(t2+(ub*t3)))+(t3*(t2_Vbulkp_sourcep+(ub*t3_Vbulkp_sourcep))));
t5_Vsourceb_sourcep=((t3_Vsourceb_sourcep*(t2+(ub*t3)))+(t3*(t2_Vsourceb_sourcep+(ub*t3_Vsourceb_sourcep))));
#endif
t5=(t3*(t2+(ub*t3)));
}
else
{
if
((mobmod==1))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgsteff_Vgatep_sourcep;
t0_Vdrainp_sourcep=((vgsteff_Vdrainp_sourcep+vth_Vdrainp_sourcep)+vth_Vdrainp_sourcep);
t0_Vdrainb_sourcep=((vgsteff_Vdrainb_sourcep+vth_Vdrainb_sourcep)+vth_Vdrainb_sourcep);
t0_Vbulkp_sourcep=((vgsteff_Vbulkp_sourcep+vth_Vbulkp_sourcep)+vth_Vbulkp_sourcep);
t0_Vsourceb_sourcep=((vgsteff_Vsourceb_sourcep+vth_Vsourceb_sourcep)+vth_Vsourceb_sourcep);
#endif
t0=((vgsteff+vth)+vth);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(uc*vbseff_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(uc*vbseff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(uc*vbseff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(uc*vbseff_Vsourceb_sourcep);
#endif
t2=(1.0+(uc*vbseff));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(t0_Vgatep_sourcep/toxe);
t3_Vdrainp_sourcep=(t0_Vdrainp_sourcep/toxe);
t3_Vdrainb_sourcep=(t0_Vdrainb_sourcep/toxe);
t3_Vbulkp_sourcep=(t0_Vbulkp_sourcep/toxe);
t3_Vsourceb_sourcep=(t0_Vsourceb_sourcep/toxe);
#endif
t3=(t0/toxe);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t3_Vgatep_sourcep*(ua+(ub*t3)))+(t3*(ub*t3_Vgatep_sourcep)));
t4_Vdrainp_sourcep=((t3_Vdrainp_sourcep*(ua+(ub*t3)))+(t3*(ub*t3_Vdrainp_sourcep)));
t4_Vdrainb_sourcep=((t3_Vdrainb_sourcep*(ua+(ub*t3)))+(t3*(ub*t3_Vdrainb_sourcep)));
t4_Vbulkp_sourcep=((t3_Vbulkp_sourcep*(ua+(ub*t3)))+(t3*(ub*t3_Vbulkp_sourcep)));
t4_Vsourceb_sourcep=((t3_Vsourceb_sourcep*(ua+(ub*t3)))+(t3*(ub*t3_Vsourceb_sourcep)));
#endif
t4=(t3*(ua+(ub*t3)));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t4_Vgatep_sourcep*t2)+(t4*t2_Vgatep_sourcep));
t5_Vdrainp_sourcep=((t4_Vdrainp_sourcep*t2)+(t4*t2_Vdrainp_sourcep));
t5_Vdrainb_sourcep=((t4_Vdrainb_sourcep*t2)+(t4*t2_Vdrainb_sourcep));
t5_Vbulkp_sourcep=((t4_Vbulkp_sourcep*t2)+(t4*t2_Vbulkp_sourcep));
t5_Vsourceb_sourcep=((t4_Vsourceb_sourcep*t2)+(t4*t2_Vsourceb_sourcep));
#endif
t5=(t4*t2);
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(vgsteff_Vgatep_sourcep/toxe);
t0_Vdrainp_sourcep=(vgsteff_Vdrainp_sourcep/toxe);
t0_Vdrainb_sourcep=(vgsteff_Vdrainb_sourcep/toxe);
t0_Vbulkp_sourcep=(vgsteff_Vbulkp_sourcep/toxe);
t0_Vsourceb_sourcep=(vgsteff_Vsourceb_sourcep/toxe);
#endif
t0=((vgsteff+vtfbphi1)/toxe);
{
double m00_logE(d00_logE0,t0)
double m00_exp(d00_exp1,(eu*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,t0)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(eu*t0_Vgatep_sourcep*d10_logE0)*d10_exp1;
t1_Vdrainp_sourcep=(eu*t0_Vdrainp_sourcep*d10_logE0)*d10_exp1;
t1_Vdrainb_sourcep=(eu*t0_Vdrainb_sourcep*d10_logE0)*d10_exp1;
t1_Vbulkp_sourcep=(eu*t0_Vbulkp_sourcep*d10_logE0)*d10_exp1;
t1_Vsourceb_sourcep=(eu*t0_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
t1=d00_exp1;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=(uc*vbseff_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(uc*vbseff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(uc*vbseff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(uc*vbseff_Vsourceb_sourcep);
#endif
t2=(ua+(uc*vbseff));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t1_Vgatep_sourcep*t2)+(t1*t2_Vgatep_sourcep));
t5_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t2)+(t1*t2_Vdrainp_sourcep));
t5_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t2)+(t1*t2_Vdrainb_sourcep));
t5_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t2)+(t1*t2_Vbulkp_sourcep));
t5_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t2)+(t1*t2_Vsourceb_sourcep));
#endif
t5=(t1*t2);
}
}
if
((t5>=(-0.8)))
{
#if defined(_DERIVATE)
denomi_Vgatep_sourcep=t5_Vgatep_sourcep;
denomi_Vdrainp_sourcep=t5_Vdrainp_sourcep;
denomi_Vdrainb_sourcep=t5_Vdrainb_sourcep;
denomi_Vbulkp_sourcep=t5_Vbulkp_sourcep;
denomi_Vsourceb_sourcep=t5_Vsourceb_sourcep;
#endif
denomi=(1.0+t5);
}
else
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=(-(10.0*t5_Vgatep_sourcep)/(7.0+(10.0*t5))/(7.0+(10.0*t5)));
t9_Vdrainp_sourcep=(-(10.0*t5_Vdrainp_sourcep)/(7.0+(10.0*t5))/(7.0+(10.0*t5)));
t9_Vdrainb_sourcep=(-(10.0*t5_Vdrainb_sourcep)/(7.0+(10.0*t5))/(7.0+(10.0*t5)));
t9_Vbulkp_sourcep=(-(10.0*t5_Vbulkp_sourcep)/(7.0+(10.0*t5))/(7.0+(10.0*t5)));
t9_Vsourceb_sourcep=(-(10.0*t5_Vsourceb_sourcep)/(7.0+(10.0*t5))/(7.0+(10.0*t5)));
#endif
t9=(1.0/(7.0+(10.0*t5)));
#if defined(_DERIVATE)
denomi_Vgatep_sourcep=((t5_Vgatep_sourcep*t9)+((0.6+t5)*t9_Vgatep_sourcep));
denomi_Vdrainp_sourcep=((t5_Vdrainp_sourcep*t9)+((0.6+t5)*t9_Vdrainp_sourcep));
denomi_Vdrainb_sourcep=((t5_Vdrainb_sourcep*t9)+((0.6+t5)*t9_Vdrainb_sourcep));
denomi_Vbulkp_sourcep=((t5_Vbulkp_sourcep*t9)+((0.6+t5)*t9_Vbulkp_sourcep));
denomi_Vsourceb_sourcep=((t5_Vsourceb_sourcep*t9)+((0.6+t5)*t9_Vsourceb_sourcep));
#endif
denomi=((0.6+t5)*t9);
}
#if defined(_DERIVATE)
ueff_Vgatep_sourcep=(-u0temp*denomi_Vgatep_sourcep/denomi/denomi);
ueff_Vdrainp_sourcep=(-u0temp*denomi_Vdrainp_sourcep/denomi/denomi);
ueff_Vdrainb_sourcep=(-u0temp*denomi_Vdrainb_sourcep/denomi/denomi);
ueff_Vbulkp_sourcep=(-u0temp*denomi_Vbulkp_sourcep/denomi/denomi);
ueff_Vsourceb_sourcep=(-u0temp*denomi_Vsourceb_sourcep/denomi/denomi);
#endif
ueff=(u0temp/denomi);
#if defined(_DERIVATE)
wvcox_Vgatep_sourcep=Weff_Vgatep_sourcep*vsattemp*coxe;
wvcox_Vdrainp_sourcep=Weff_Vdrainp_sourcep*vsattemp*coxe;
wvcox_Vdrainb_sourcep=Weff_Vdrainb_sourcep*vsattemp*coxe;
wvcox_Vbulkp_sourcep=Weff_Vbulkp_sourcep*vsattemp*coxe;
wvcox_Vsourceb_sourcep=Weff_Vsourceb_sourcep*vsattemp*coxe;
#endif
wvcox=((Weff*vsattemp)*coxe);
#if defined(_DERIVATE)
wvcoxrds_Vgatep_sourcep=((wvcox_Vgatep_sourcep*rds)+(wvcox*rds_Vgatep_sourcep));
wvcoxrds_Vdrainp_sourcep=((wvcox_Vdrainp_sourcep*rds)+(wvcox*rds_Vdrainp_sourcep));
wvcoxrds_Vdrainb_sourcep=((wvcox_Vdrainb_sourcep*rds)+(wvcox*rds_Vdrainb_sourcep));
wvcoxrds_Vbulkp_sourcep=((wvcox_Vbulkp_sourcep*rds)+(wvcox*rds_Vbulkp_sourcep));
wvcoxrds_Vsourceb_sourcep=((wvcox_Vsourceb_sourcep*rds)+(wvcox*rds_Vsourceb_sourcep));
#endif
wvcoxrds=(wvcox*rds);
#if defined(_DERIVATE)
esat_Vgatep_sourcep=(-(2.0*vsattemp)*ueff_Vgatep_sourcep/ueff/ueff);
esat_Vdrainp_sourcep=(-(2.0*vsattemp)*ueff_Vdrainp_sourcep/ueff/ueff);
esat_Vdrainb_sourcep=(-(2.0*vsattemp)*ueff_Vdrainb_sourcep/ueff/ueff);
esat_Vbulkp_sourcep=(-(2.0*vsattemp)*ueff_Vbulkp_sourcep/ueff/ueff);
esat_Vsourceb_sourcep=(-(2.0*vsattemp)*ueff_Vsourceb_sourcep/ueff/ueff);
#endif
esat=((2.0*vsattemp)/ueff);
#if defined(_DERIVATE)
esatl_Vgatep_sourcep=esat_Vgatep_sourcep*leff;
esatl_Vdrainp_sourcep=esat_Vdrainp_sourcep*leff;
esatl_Vdrainb_sourcep=esat_Vdrainb_sourcep*leff;
esatl_Vbulkp_sourcep=esat_Vbulkp_sourcep*leff;
esatl_Vsourceb_sourcep=esat_Vsourceb_sourcep*leff;
#endif
esatl=(esat*leff);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(((-esatl_Vgatep_sourcep)*ueff-(-esatl)*ueff_Vgatep_sourcep)/ueff/ueff);
t0_Vdrainp_sourcep=(((-esatl_Vdrainp_sourcep)*ueff-(-esatl)*ueff_Vdrainp_sourcep)/ueff/ueff);
t0_Vdrainb_sourcep=(((-esatl_Vdrainb_sourcep)*ueff-(-esatl)*ueff_Vdrainb_sourcep)/ueff/ueff);
t0_Vbulkp_sourcep=(((-esatl_Vbulkp_sourcep)*ueff-(-esatl)*ueff_Vbulkp_sourcep)/ueff/ueff);
t0_Vsourceb_sourcep=(((-esatl_Vsourceb_sourcep)*ueff-(-esatl)*ueff_Vsourceb_sourcep)/ueff/ueff);
#endif
t0=((-esatl)/ueff);
if
((a1==0.0))
{
#if defined(_DERIVATE)
Lambda_Vgatep_sourcep=0.0;
Lambda_Vdrainp_sourcep=0.0;
Lambda_Vdrainb_sourcep=0.0;
Lambda_Vbulkp_sourcep=0.0;
Lambda_Vsourceb_sourcep=0.0;
#endif
Lambda=a2;
}
else
{
if
((a1>0.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(1.0-a2);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(t0_Vgatep_sourcep-(a1*vgsteff_Vgatep_sourcep));
t1_Vdrainp_sourcep=(t0_Vdrainp_sourcep-(a1*vgsteff_Vdrainp_sourcep));
t1_Vdrainb_sourcep=(t0_Vdrainb_sourcep-(a1*vgsteff_Vdrainb_sourcep));
t1_Vbulkp_sourcep=(t0_Vbulkp_sourcep-(a1*vgsteff_Vbulkp_sourcep));
t1_Vsourceb_sourcep=(t0_Vsourceb_sourcep-(a1*vgsteff_Vsourceb_sourcep));
#endif
t1=((t0-(a1*vgsteff))-0.0001);
{
double m00_sqrt(d00_sqrt0,((t1*t1)+(0.0004*t0)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t1*t1)+(0.0004*t0)))
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))+(0.0004*t0_Vgatep_sourcep))*d10_sqrt0;
t2_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))+(0.0004*t0_Vdrainp_sourcep))*d10_sqrt0;
t2_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))+(0.0004*t0_Vdrainb_sourcep))*d10_sqrt0;
t2_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))+(0.0004*t0_Vbulkp_sourcep))*d10_sqrt0;
t2_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))+(0.0004*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t2=d00_sqrt0;
}
#if defined(_DERIVATE)
Lambda_Vgatep_sourcep=(t0_Vgatep_sourcep-(0.5*(t1_Vgatep_sourcep+t2_Vgatep_sourcep)));
Lambda_Vdrainp_sourcep=(t0_Vdrainp_sourcep-(0.5*(t1_Vdrainp_sourcep+t2_Vdrainp_sourcep)));
Lambda_Vdrainb_sourcep=(t0_Vdrainb_sourcep-(0.5*(t1_Vdrainb_sourcep+t2_Vdrainb_sourcep)));
Lambda_Vbulkp_sourcep=(t0_Vbulkp_sourcep-(0.5*(t1_Vbulkp_sourcep+t2_Vbulkp_sourcep)));
Lambda_Vsourceb_sourcep=(t0_Vsourceb_sourcep-(0.5*(t1_Vsourceb_sourcep+t2_Vsourceb_sourcep)));
#endif
Lambda=((a2+t0)-(0.5*(t1+t2)));
}
else
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(a1*vgsteff_Vgatep_sourcep);
t1_Vdrainp_sourcep=(a1*vgsteff_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(a1*vgsteff_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(a1*vgsteff_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(a1*vgsteff_Vsourceb_sourcep);
#endif
t1=((a2+(a1*vgsteff))-0.0001);
{
double m00_sqrt(d00_sqrt0,((t1*t1)+(0.0004*a2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t1*t1)+(0.0004*a2)))
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))*d10_sqrt0;
t2_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))*d10_sqrt0;
t2_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))*d10_sqrt0;
t2_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))*d10_sqrt0;
t2_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))*d10_sqrt0;
#endif
t2=d00_sqrt0;
}
#if defined(_DERIVATE)
Lambda_Vgatep_sourcep=(0.5*(t1_Vgatep_sourcep+t2_Vgatep_sourcep));
Lambda_Vdrainp_sourcep=(0.5*(t1_Vdrainp_sourcep+t2_Vdrainp_sourcep));
Lambda_Vdrainb_sourcep=(0.5*(t1_Vdrainb_sourcep+t2_Vdrainb_sourcep));
Lambda_Vbulkp_sourcep=(0.5*(t1_Vbulkp_sourcep+t2_Vbulkp_sourcep));
Lambda_Vsourceb_sourcep=(0.5*(t1_Vsourceb_sourcep+t2_Vsourceb_sourcep));
#endif
Lambda=(0.5*(t1+t2));
}
}
#if defined(_DERIVATE)
vgst2vtm_Vgatep_sourcep=vgsteff_Vgatep_sourcep;
vgst2vtm_Vdrainp_sourcep=vgsteff_Vdrainp_sourcep;
vgst2vtm_Vdrainb_sourcep=vgsteff_Vdrainb_sourcep;
vgst2vtm_Vbulkp_sourcep=vgsteff_Vbulkp_sourcep;
vgst2vtm_Vsourceb_sourcep=vgsteff_Vsourceb_sourcep;
#endif
vgst2vtm=(vgsteff+(2.0*vtm));
if
(((rds==0.0)&&(Lambda==1.0)))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-(((abulk_Vgatep_sourcep*esatl)+(abulk*esatl_Vgatep_sourcep))+vgst2vtm_Vgatep_sourcep)/((abulk*esatl)+vgst2vtm)/((abulk*esatl)+vgst2vtm));
t0_Vdrainp_sourcep=(-(((abulk_Vdrainp_sourcep*esatl)+(abulk*esatl_Vdrainp_sourcep))+vgst2vtm_Vdrainp_sourcep)/((abulk*esatl)+vgst2vtm)/((abulk*esatl)+vgst2vtm));
t0_Vdrainb_sourcep=(-(((abulk_Vdrainb_sourcep*esatl)+(abulk*esatl_Vdrainb_sourcep))+vgst2vtm_Vdrainb_sourcep)/((abulk*esatl)+vgst2vtm)/((abulk*esatl)+vgst2vtm));
t0_Vbulkp_sourcep=(-(((abulk_Vbulkp_sourcep*esatl)+(abulk*esatl_Vbulkp_sourcep))+vgst2vtm_Vbulkp_sourcep)/((abulk*esatl)+vgst2vtm)/((abulk*esatl)+vgst2vtm));
t0_Vsourceb_sourcep=(-(((abulk_Vsourceb_sourcep*esatl)+(abulk*esatl_Vsourceb_sourcep))+vgst2vtm_Vsourceb_sourcep)/((abulk*esatl)+vgst2vtm)/((abulk*esatl)+vgst2vtm));
#endif
t0=(1.0/((abulk*esatl)+vgst2vtm));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep));
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep));
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep));
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep));
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep));
#endif
t1=(t0*t0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((vgst2vtm_Vgatep_sourcep*t0)+(vgst2vtm*t0_Vgatep_sourcep));
t2_Vdrainp_sourcep=((vgst2vtm_Vdrainp_sourcep*t0)+(vgst2vtm*t0_Vdrainp_sourcep));
t2_Vdrainb_sourcep=((vgst2vtm_Vdrainb_sourcep*t0)+(vgst2vtm*t0_Vdrainb_sourcep));
t2_Vbulkp_sourcep=((vgst2vtm_Vbulkp_sourcep*t0)+(vgst2vtm*t0_Vbulkp_sourcep));
t2_Vsourceb_sourcep=((vgst2vtm_Vsourceb_sourcep*t0)+(vgst2vtm*t0_Vsourceb_sourcep));
#endif
t2=(vgst2vtm*t0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((esatl_Vgatep_sourcep*vgst2vtm)+(esatl*vgst2vtm_Vgatep_sourcep));
t3_Vdrainp_sourcep=((esatl_Vdrainp_sourcep*vgst2vtm)+(esatl*vgst2vtm_Vdrainp_sourcep));
t3_Vdrainb_sourcep=((esatl_Vdrainb_sourcep*vgst2vtm)+(esatl*vgst2vtm_Vdrainb_sourcep));
t3_Vbulkp_sourcep=((esatl_Vbulkp_sourcep*vgst2vtm)+(esatl*vgst2vtm_Vbulkp_sourcep));
t3_Vsourceb_sourcep=((esatl_Vsourceb_sourcep*vgst2vtm)+(esatl*vgst2vtm_Vsourceb_sourcep));
#endif
t3=(esatl*vgst2vtm);
#if defined(_DERIVATE)
vdsat_Vgatep_sourcep=((t3_Vgatep_sourcep*t0)+(t3*t0_Vgatep_sourcep));
vdsat_Vdrainp_sourcep=((t3_Vdrainp_sourcep*t0)+(t3*t0_Vdrainp_sourcep));
vdsat_Vdrainb_sourcep=((t3_Vdrainb_sourcep*t0)+(t3*t0_Vdrainb_sourcep));
vdsat_Vbulkp_sourcep=((t3_Vbulkp_sourcep*t0)+(t3*t0_Vbulkp_sourcep));
vdsat_Vsourceb_sourcep=((t3_Vsourceb_sourcep*t0)+(t3*t0_Vsourceb_sourcep));
#endif
vdsat=(t3*t0);
}
else
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((abulk_Vgatep_sourcep*wvcoxrds)+(abulk*wvcoxrds_Vgatep_sourcep));
t9_Vdrainp_sourcep=((abulk_Vdrainp_sourcep*wvcoxrds)+(abulk*wvcoxrds_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((abulk_Vdrainb_sourcep*wvcoxrds)+(abulk*wvcoxrds_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((abulk_Vbulkp_sourcep*wvcoxrds)+(abulk*wvcoxrds_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((abulk_Vsourceb_sourcep*wvcoxrds)+(abulk*wvcoxrds_Vsourceb_sourcep));
#endif
t9=(abulk*wvcoxrds);
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((abulk_Vgatep_sourcep*t9)+(abulk*t9_Vgatep_sourcep));
t8_Vdrainp_sourcep=((abulk_Vdrainp_sourcep*t9)+(abulk*t9_Vdrainp_sourcep));
t8_Vdrainb_sourcep=((abulk_Vdrainb_sourcep*t9)+(abulk*t9_Vdrainb_sourcep));
t8_Vbulkp_sourcep=((abulk_Vbulkp_sourcep*t9)+(abulk*t9_Vbulkp_sourcep));
t8_Vsourceb_sourcep=((abulk_Vsourceb_sourcep*t9)+(abulk*t9_Vsourceb_sourcep));
#endif
t8=(abulk*t9);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((vgst2vtm_Vgatep_sourcep*t9)+(vgst2vtm*t9_Vgatep_sourcep));
t7_Vdrainp_sourcep=((vgst2vtm_Vdrainp_sourcep*t9)+(vgst2vtm*t9_Vdrainp_sourcep));
t7_Vdrainb_sourcep=((vgst2vtm_Vdrainb_sourcep*t9)+(vgst2vtm*t9_Vdrainb_sourcep));
t7_Vbulkp_sourcep=((vgst2vtm_Vbulkp_sourcep*t9)+(vgst2vtm*t9_Vbulkp_sourcep));
t7_Vsourceb_sourcep=((vgst2vtm_Vsourceb_sourcep*t9)+(vgst2vtm*t9_Vsourceb_sourcep));
#endif
t7=(vgst2vtm*t9);
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((vgst2vtm_Vgatep_sourcep*wvcoxrds)+(vgst2vtm*wvcoxrds_Vgatep_sourcep));
t6_Vdrainp_sourcep=((vgst2vtm_Vdrainp_sourcep*wvcoxrds)+(vgst2vtm*wvcoxrds_Vdrainp_sourcep));
t6_Vdrainb_sourcep=((vgst2vtm_Vdrainb_sourcep*wvcoxrds)+(vgst2vtm*wvcoxrds_Vdrainb_sourcep));
t6_Vbulkp_sourcep=((vgst2vtm_Vbulkp_sourcep*wvcoxrds)+(vgst2vtm*wvcoxrds_Vbulkp_sourcep));
t6_Vsourceb_sourcep=((vgst2vtm_Vsourceb_sourcep*wvcoxrds)+(vgst2vtm*wvcoxrds_Vsourceb_sourcep));
#endif
t6=(vgst2vtm*wvcoxrds);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(((2.0*abulk_Vgatep_sourcep)*((t9-1.0)+(1.0/Lambda)))+((2.0*abulk)*(t9_Vgatep_sourcep+(-Lambda_Vgatep_sourcep/Lambda/Lambda))));
t0_Vdrainp_sourcep=(((2.0*abulk_Vdrainp_sourcep)*((t9-1.0)+(1.0/Lambda)))+((2.0*abulk)*(t9_Vdrainp_sourcep+(-Lambda_Vdrainp_sourcep/Lambda/Lambda))));
t0_Vdrainb_sourcep=(((2.0*abulk_Vdrainb_sourcep)*((t9-1.0)+(1.0/Lambda)))+((2.0*abulk)*(t9_Vdrainb_sourcep+(-Lambda_Vdrainb_sourcep/Lambda/Lambda))));
t0_Vbulkp_sourcep=(((2.0*abulk_Vbulkp_sourcep)*((t9-1.0)+(1.0/Lambda)))+((2.0*abulk)*(t9_Vbulkp_sourcep+(-Lambda_Vbulkp_sourcep/Lambda/Lambda))));
t0_Vsourceb_sourcep=(((2.0*abulk_Vsourceb_sourcep)*((t9-1.0)+(1.0/Lambda)))+((2.0*abulk)*(t9_Vsourceb_sourcep+(-Lambda_Vsourceb_sourcep/Lambda/Lambda))));
#endif
t0=((2.0*abulk)*((t9-1.0)+(1.0/Lambda)));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((((vgst2vtm_Vgatep_sourcep*((2.0/Lambda)-1.0))+(vgst2vtm*(-2.0*Lambda_Vgatep_sourcep/Lambda/Lambda)))+((abulk_Vgatep_sourcep*esatl)+(abulk*esatl_Vgatep_sourcep)))+(3.0*t7_Vgatep_sourcep));
t1_Vdrainp_sourcep=((((vgst2vtm_Vdrainp_sourcep*((2.0/Lambda)-1.0))+(vgst2vtm*(-2.0*Lambda_Vdrainp_sourcep/Lambda/Lambda)))+((abulk_Vdrainp_sourcep*esatl)+(abulk*esatl_Vdrainp_sourcep)))+(3.0*t7_Vdrainp_sourcep));
t1_Vdrainb_sourcep=((((vgst2vtm_Vdrainb_sourcep*((2.0/Lambda)-1.0))+(vgst2vtm*(-2.0*Lambda_Vdrainb_sourcep/Lambda/Lambda)))+((abulk_Vdrainb_sourcep*esatl)+(abulk*esatl_Vdrainb_sourcep)))+(3.0*t7_Vdrainb_sourcep));
t1_Vbulkp_sourcep=((((vgst2vtm_Vbulkp_sourcep*((2.0/Lambda)-1.0))+(vgst2vtm*(-2.0*Lambda_Vbulkp_sourcep/Lambda/Lambda)))+((abulk_Vbulkp_sourcep*esatl)+(abulk*esatl_Vbulkp_sourcep)))+(3.0*t7_Vbulkp_sourcep));
t1_Vsourceb_sourcep=((((vgst2vtm_Vsourceb_sourcep*((2.0/Lambda)-1.0))+(vgst2vtm*(-2.0*Lambda_Vsourceb_sourcep/Lambda/Lambda)))+((abulk_Vsourceb_sourcep*esatl)+(abulk*esatl_Vsourceb_sourcep)))+(3.0*t7_Vsourceb_sourcep));
#endif
t1=(((vgst2vtm*((2.0/Lambda)-1.0))+(abulk*esatl))+(3.0*t7));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((vgst2vtm_Vgatep_sourcep*(esatl+(2.0*t6)))+(vgst2vtm*(esatl_Vgatep_sourcep+(2.0*t6_Vgatep_sourcep))));
t2_Vdrainp_sourcep=((vgst2vtm_Vdrainp_sourcep*(esatl+(2.0*t6)))+(vgst2vtm*(esatl_Vdrainp_sourcep+(2.0*t6_Vdrainp_sourcep))));
t2_Vdrainb_sourcep=((vgst2vtm_Vdrainb_sourcep*(esatl+(2.0*t6)))+(vgst2vtm*(esatl_Vdrainb_sourcep+(2.0*t6_Vdrainb_sourcep))));
t2_Vbulkp_sourcep=((vgst2vtm_Vbulkp_sourcep*(esatl+(2.0*t6)))+(vgst2vtm*(esatl_Vbulkp_sourcep+(2.0*t6_Vbulkp_sourcep))));
t2_Vsourceb_sourcep=((vgst2vtm_Vsourceb_sourcep*(esatl+(2.0*t6)))+(vgst2vtm*(esatl_Vsourceb_sourcep+(2.0*t6_Vsourceb_sourcep))));
#endif
t2=(vgst2vtm*(esatl+(2.0*t6)));
{
double m00_sqrt(d00_sqrt0,((t1*t1)-((2.0*t0)*t2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t1*t1)-((2.0*t0)*t2)))
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))-(((2.0*t0_Vgatep_sourcep)*t2)+((2.0*t0)*t2_Vgatep_sourcep)))*d10_sqrt0;
t3_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))-(((2.0*t0_Vdrainp_sourcep)*t2)+((2.0*t0)*t2_Vdrainp_sourcep)))*d10_sqrt0;
t3_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))-(((2.0*t0_Vdrainb_sourcep)*t2)+((2.0*t0)*t2_Vdrainb_sourcep)))*d10_sqrt0;
t3_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))-(((2.0*t0_Vbulkp_sourcep)*t2)+((2.0*t0)*t2_Vbulkp_sourcep)))*d10_sqrt0;
t3_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))-(((2.0*t0_Vsourceb_sourcep)*t2)+((2.0*t0)*t2_Vsourceb_sourcep)))*d10_sqrt0;
#endif
t3=d00_sqrt0;
}
#if defined(_DERIVATE)
vdsat_Vgatep_sourcep=(((t1_Vgatep_sourcep-t3_Vgatep_sourcep)*t0-(t1-t3)*t0_Vgatep_sourcep)/t0/t0);
vdsat_Vdrainp_sourcep=(((t1_Vdrainp_sourcep-t3_Vdrainp_sourcep)*t0-(t1-t3)*t0_Vdrainp_sourcep)/t0/t0);
vdsat_Vdrainb_sourcep=(((t1_Vdrainb_sourcep-t3_Vdrainb_sourcep)*t0-(t1-t3)*t0_Vdrainb_sourcep)/t0/t0);
vdsat_Vbulkp_sourcep=(((t1_Vbulkp_sourcep-t3_Vbulkp_sourcep)*t0-(t1-t3)*t0_Vbulkp_sourcep)/t0/t0);
vdsat_Vsourceb_sourcep=(((t1_Vsourceb_sourcep-t3_Vsourceb_sourcep)*t0-(t1-t3)*t0_Vsourceb_sourcep)/t0/t0);
#endif
vdsat=((t1-t3)/t0);
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=vdsat_Vgatep_sourcep;
t1_Vdrainp_sourcep=(vdsat_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
t1_Vdrainb_sourcep=vdsat_Vdrainb_sourcep;
t1_Vbulkp_sourcep=vdsat_Vbulkp_sourcep;
t1_Vsourceb_sourcep=vdsat_Vsourceb_sourcep;
#endif
t1=((vdsat-Vds)-delta);
{
double m00_sqrt(d00_sqrt0,((t1*t1)+((4.0*delta)*vdsat)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t1*t1)+((4.0*delta)*vdsat)))
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))+((4.0*delta)*vdsat_Vgatep_sourcep))*d10_sqrt0;
t2_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))+((4.0*delta)*vdsat_Vdrainp_sourcep))*d10_sqrt0;
t2_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))+((4.0*delta)*vdsat_Vdrainb_sourcep))*d10_sqrt0;
t2_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))+((4.0*delta)*vdsat_Vbulkp_sourcep))*d10_sqrt0;
t2_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))+((4.0*delta)*vdsat_Vsourceb_sourcep))*d10_sqrt0;
#endif
t2=d00_sqrt0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((t1_Vgatep_sourcep*t2-t1*t2_Vgatep_sourcep)/t2/t2);
t0_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t2-t1*t2_Vdrainp_sourcep)/t2/t2);
t0_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t2-t1*t2_Vdrainb_sourcep)/t2/t2);
t0_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t2-t1*t2_Vbulkp_sourcep)/t2/t2);
t0_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t2-t1*t2_Vsourceb_sourcep)/t2/t2);
#endif
t0=(t1/t2);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(2.0*delta);
if
((t1>=0.0))
{
#if defined(_DERIVATE)
vdseff_Vgatep_sourcep=(vdsat_Vgatep_sourcep-(0.5*(t1_Vgatep_sourcep+t2_Vgatep_sourcep)));
vdseff_Vdrainp_sourcep=(vdsat_Vdrainp_sourcep-(0.5*(t1_Vdrainp_sourcep+t2_Vdrainp_sourcep)));
vdseff_Vdrainb_sourcep=(vdsat_Vdrainb_sourcep-(0.5*(t1_Vdrainb_sourcep+t2_Vdrainb_sourcep)));
vdseff_Vbulkp_sourcep=(vdsat_Vbulkp_sourcep-(0.5*(t1_Vbulkp_sourcep+t2_Vbulkp_sourcep)));
vdseff_Vsourceb_sourcep=(vdsat_Vsourceb_sourcep-(0.5*(t1_Vsourceb_sourcep+t2_Vsourceb_sourcep)));
#endif
vdseff=(vdsat-(0.5*(t1+t2)));
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t9_Vgatep_sourcep*(t2-t1)-t9*(t2_Vgatep_sourcep-t1_Vgatep_sourcep))/(t2-t1)/(t2-t1));
t4_Vdrainp_sourcep=((t9_Vdrainp_sourcep*(t2-t1)-t9*(t2_Vdrainp_sourcep-t1_Vdrainp_sourcep))/(t2-t1)/(t2-t1));
t4_Vdrainb_sourcep=((t9_Vdrainb_sourcep*(t2-t1)-t9*(t2_Vdrainb_sourcep-t1_Vdrainb_sourcep))/(t2-t1)/(t2-t1));
t4_Vbulkp_sourcep=((t9_Vbulkp_sourcep*(t2-t1)-t9*(t2_Vbulkp_sourcep-t1_Vbulkp_sourcep))/(t2-t1)/(t2-t1));
t4_Vsourceb_sourcep=((t9_Vsourceb_sourcep*(t2-t1)-t9*(t2_Vsourceb_sourcep-t1_Vsourceb_sourcep))/(t2-t1)/(t2-t1));
#endif
t4=(t9/(t2-t1));
#if defined(_DERIVATE)
t5_Vgatep_sourcep=(-t4_Vgatep_sourcep);
t5_Vdrainp_sourcep=(-t4_Vdrainp_sourcep);
t5_Vdrainb_sourcep=(-t4_Vdrainb_sourcep);
t5_Vbulkp_sourcep=(-t4_Vbulkp_sourcep);
t5_Vsourceb_sourcep=(-t4_Vsourceb_sourcep);
#endif
t5=(1.0-t4);
#if defined(_DERIVATE)
vdseff_Vgatep_sourcep=((vdsat_Vgatep_sourcep*t5)+(vdsat*t5_Vgatep_sourcep));
vdseff_Vdrainp_sourcep=((vdsat_Vdrainp_sourcep*t5)+(vdsat*t5_Vdrainp_sourcep));
vdseff_Vdrainb_sourcep=((vdsat_Vdrainb_sourcep*t5)+(vdsat*t5_Vdrainb_sourcep));
vdseff_Vbulkp_sourcep=((vdsat_Vbulkp_sourcep*t5)+(vdsat*t5_Vbulkp_sourcep));
vdseff_Vsourceb_sourcep=((vdsat_Vsourceb_sourcep*t5)+(vdsat*t5_Vsourceb_sourcep));
#endif
vdseff=(vdsat*t5);
}
if
((Vds==0.0))
{
#if defined(_DERIVATE)
vdseff_Vgatep_sourcep=0.0;
vdseff_Vdrainp_sourcep=Vds_Vdrainp_sourcep;
vdseff_Vdrainb_sourcep=0.0;
vdseff_Vbulkp_sourcep=0.0;
vdseff_Vsourceb_sourcep=0.0;
#endif
vdseff=Vds;
}
if
((vdseff>Vds))
{
#if defined(_DERIVATE)
vdseff_Vgatep_sourcep=0.0;
vdseff_Vdrainp_sourcep=Vds_Vdrainp_sourcep;
vdseff_Vdrainb_sourcep=0.0;
vdseff_Vbulkp_sourcep=0.0;
vdseff_Vsourceb_sourcep=0.0;
#endif
vdseff=Vds;
}
#if defined(_DERIVATE)
diffvds_Vdrainp_sourcep=(Vds_Vdrainp_sourcep-vdseff_Vdrainp_sourcep);
diffvds_Vgatep_sourcep=(-vdseff_Vgatep_sourcep);
diffvds_Vdrainb_sourcep=(-vdseff_Vdrainb_sourcep);
diffvds_Vbulkp_sourcep=(-vdseff_Vbulkp_sourcep);
diffvds_Vsourceb_sourcep=(-vdseff_Vsourceb_sourcep);
#endif
diffvds=(Vds-vdseff);
if
(((lambda!=(-99.0))&&(lambda>0.0)))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(leff*ueff_Vgatep_sourcep);
t1_Vdrainp_sourcep=(leff*ueff_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(leff*ueff_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(leff*ueff_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(leff*ueff_Vsourceb_sourcep);
#endif
t1=(leff*ueff);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-lambda*t1_Vgatep_sourcep/t1/t1);
t2_Vdrainp_sourcep=(-lambda*t1_Vdrainp_sourcep/t1/t1);
t2_Vdrainb_sourcep=(-lambda*t1_Vdrainb_sourcep/t1/t1);
t2_Vbulkp_sourcep=(-lambda*t1_Vbulkp_sourcep/t1/t1);
t2_Vsourceb_sourcep=(-lambda*t1_Vsourceb_sourcep/t1/t1);
#endif
t2=(lambda/t1);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=(-esat_Vgatep_sourcep*litl/(esat*litl)/(esat*litl));
t5_Vdrainp_sourcep=(-esat_Vdrainp_sourcep*litl/(esat*litl)/(esat*litl));
t5_Vdrainb_sourcep=(-esat_Vdrainb_sourcep*litl/(esat*litl)/(esat*litl));
t5_Vbulkp_sourcep=(-esat_Vbulkp_sourcep*litl/(esat*litl)/(esat*litl));
t5_Vsourceb_sourcep=(-esat_Vsourceb_sourcep*litl/(esat*litl)/(esat*litl));
#endif
t5=(1.0/(esat*litl));
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((diffvds_Vgatep_sourcep*t5)+(diffvds*t5_Vgatep_sourcep));
t6_Vdrainp_sourcep=((diffvds_Vdrainp_sourcep*t5)+(diffvds*t5_Vdrainp_sourcep));
t6_Vdrainb_sourcep=((diffvds_Vdrainb_sourcep*t5)+(diffvds*t5_Vdrainb_sourcep));
t6_Vbulkp_sourcep=((diffvds_Vbulkp_sourcep*t5)+(diffvds*t5_Vbulkp_sourcep));
t6_Vsourceb_sourcep=((diffvds_Vsourceb_sourcep*t5)+(diffvds*t5_Vsourceb_sourcep));
#endif
t6=(1.0+(diffvds*t5));
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(-2.0*((t6_Vgatep_sourcep*t6)+(t6*t6_Vgatep_sourcep))/((t6*t6)+1.0)/((t6*t6)+1.0));
t7_Vdrainp_sourcep=(-2.0*((t6_Vdrainp_sourcep*t6)+(t6*t6_Vdrainp_sourcep))/((t6*t6)+1.0)/((t6*t6)+1.0));
t7_Vdrainb_sourcep=(-2.0*((t6_Vdrainb_sourcep*t6)+(t6*t6_Vdrainb_sourcep))/((t6*t6)+1.0)/((t6*t6)+1.0));
t7_Vbulkp_sourcep=(-2.0*((t6_Vbulkp_sourcep*t6)+(t6*t6_Vbulkp_sourcep))/((t6*t6)+1.0)/((t6*t6)+1.0));
t7_Vsourceb_sourcep=(-2.0*((t6_Vsourceb_sourcep*t6)+(t6*t6_Vsourceb_sourcep))/((t6*t6)+1.0)/((t6*t6)+1.0));
#endif
t7=(2.0/((t6*t6)+1.0));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=(-t7_Vgatep_sourcep);
t8_Vdrainp_sourcep=(-t7_Vdrainp_sourcep);
t8_Vdrainb_sourcep=(-t7_Vdrainb_sourcep);
t8_Vbulkp_sourcep=(-t7_Vbulkp_sourcep);
t8_Vsourceb_sourcep=(-t7_Vsourceb_sourcep);
#endif
t8=(1.0-t7);
#if defined(_DERIVATE)
t10_Vgatep_sourcep=((t2_Vgatep_sourcep*t8)+(t2*t8_Vgatep_sourcep));
t10_Vdrainp_sourcep=((t2_Vdrainp_sourcep*t8)+(t2*t8_Vdrainp_sourcep));
t10_Vdrainb_sourcep=((t2_Vdrainb_sourcep*t8)+(t2*t8_Vdrainb_sourcep));
t10_Vbulkp_sourcep=((t2_Vbulkp_sourcep*t8)+(t2*t8_Vbulkp_sourcep));
t10_Vsourceb_sourcep=((t2_Vsourceb_sourcep*t8)+(t2*t8_Vsourceb_sourcep));
#endif
t10=(1.0+(t2*t8));
#if defined(_DERIVATE)
esatl_Vgatep_sourcep=((esatl_Vgatep_sourcep*t10)+(esatl*t10_Vgatep_sourcep));
esatl_Vdrainp_sourcep=((esatl_Vdrainp_sourcep*t10)+(esatl*t10_Vdrainp_sourcep));
esatl_Vdrainb_sourcep=((esatl_Vdrainb_sourcep*t10)+(esatl*t10_Vdrainb_sourcep));
esatl_Vbulkp_sourcep=((esatl_Vbulkp_sourcep*t10)+(esatl*t10_Vbulkp_sourcep));
esatl_Vsourceb_sourcep=((esatl_Vsourceb_sourcep*t10)+(esatl*t10_Vsourceb_sourcep));
#endif
esatl=(esatl*t10);
}
#if defined(_DERIVATE)
tmp4_Vgatep_sourcep=(-(((((0.5*abulk_Vgatep_sourcep)*vdsat)+((0.5*abulk)*vdsat_Vgatep_sourcep))*vgst2vtm-((0.5*abulk)*vdsat)*vgst2vtm_Vgatep_sourcep)/vgst2vtm/vgst2vtm));
tmp4_Vdrainp_sourcep=(-(((((0.5*abulk_Vdrainp_sourcep)*vdsat)+((0.5*abulk)*vdsat_Vdrainp_sourcep))*vgst2vtm-((0.5*abulk)*vdsat)*vgst2vtm_Vdrainp_sourcep)/vgst2vtm/vgst2vtm));
tmp4_Vdrainb_sourcep=(-(((((0.5*abulk_Vdrainb_sourcep)*vdsat)+((0.5*abulk)*vdsat_Vdrainb_sourcep))*vgst2vtm-((0.5*abulk)*vdsat)*vgst2vtm_Vdrainb_sourcep)/vgst2vtm/vgst2vtm));
tmp4_Vbulkp_sourcep=(-(((((0.5*abulk_Vbulkp_sourcep)*vdsat)+((0.5*abulk)*vdsat_Vbulkp_sourcep))*vgst2vtm-((0.5*abulk)*vdsat)*vgst2vtm_Vbulkp_sourcep)/vgst2vtm/vgst2vtm));
tmp4_Vsourceb_sourcep=(-(((((0.5*abulk_Vsourceb_sourcep)*vdsat)+((0.5*abulk)*vdsat_Vsourceb_sourcep))*vgst2vtm-((0.5*abulk)*vdsat)*vgst2vtm_Vsourceb_sourcep)/vgst2vtm/vgst2vtm));
#endif
tmp4=(1.0-(((0.5*abulk)*vdsat)/vgst2vtm));
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((wvcoxrds_Vgatep_sourcep*vgsteff)+(wvcoxrds*vgsteff_Vgatep_sourcep));
t9_Vdrainp_sourcep=((wvcoxrds_Vdrainp_sourcep*vgsteff)+(wvcoxrds*vgsteff_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((wvcoxrds_Vdrainb_sourcep*vgsteff)+(wvcoxrds*vgsteff_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((wvcoxrds_Vbulkp_sourcep*vgsteff)+(wvcoxrds*vgsteff_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((wvcoxrds_Vsourceb_sourcep*vgsteff)+(wvcoxrds*vgsteff_Vsourceb_sourcep));
#endif
t9=(wvcoxrds*vgsteff);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((esatl_Vgatep_sourcep+vdsat_Vgatep_sourcep)+(((2.0*t9_Vgatep_sourcep)*tmp4)+((2.0*t9)*tmp4_Vgatep_sourcep)));
t0_Vdrainp_sourcep=((esatl_Vdrainp_sourcep+vdsat_Vdrainp_sourcep)+(((2.0*t9_Vdrainp_sourcep)*tmp4)+((2.0*t9)*tmp4_Vdrainp_sourcep)));
t0_Vdrainb_sourcep=((esatl_Vdrainb_sourcep+vdsat_Vdrainb_sourcep)+(((2.0*t9_Vdrainb_sourcep)*tmp4)+((2.0*t9)*tmp4_Vdrainb_sourcep)));
t0_Vbulkp_sourcep=((esatl_Vbulkp_sourcep+vdsat_Vbulkp_sourcep)+(((2.0*t9_Vbulkp_sourcep)*tmp4)+((2.0*t9)*tmp4_Vbulkp_sourcep)));
t0_Vsourceb_sourcep=((esatl_Vsourceb_sourcep+vdsat_Vsourceb_sourcep)+(((2.0*t9_Vsourceb_sourcep)*tmp4)+((2.0*t9)*tmp4_Vsourceb_sourcep)));
#endif
t0=((esatl+vdsat)+((2.0*t9)*tmp4));
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((wvcoxrds_Vgatep_sourcep*abulk)+(wvcoxrds*abulk_Vgatep_sourcep));
t9_Vdrainp_sourcep=((wvcoxrds_Vdrainp_sourcep*abulk)+(wvcoxrds*abulk_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((wvcoxrds_Vdrainb_sourcep*abulk)+(wvcoxrds*abulk_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((wvcoxrds_Vbulkp_sourcep*abulk)+(wvcoxrds*abulk_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((wvcoxrds_Vsourceb_sourcep*abulk)+(wvcoxrds*abulk_Vsourceb_sourcep));
#endif
t9=(wvcoxrds*abulk);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((-2.0*Lambda_Vgatep_sourcep/Lambda/Lambda)+t9_Vgatep_sourcep);
t1_Vdrainp_sourcep=((-2.0*Lambda_Vdrainp_sourcep/Lambda/Lambda)+t9_Vdrainp_sourcep);
t1_Vdrainb_sourcep=((-2.0*Lambda_Vdrainb_sourcep/Lambda/Lambda)+t9_Vdrainb_sourcep);
t1_Vbulkp_sourcep=((-2.0*Lambda_Vbulkp_sourcep/Lambda/Lambda)+t9_Vbulkp_sourcep);
t1_Vsourceb_sourcep=((-2.0*Lambda_Vsourceb_sourcep/Lambda/Lambda)+t9_Vsourceb_sourcep);
#endif
t1=(((2.0/Lambda)-1.0)+t9);
#if defined(_DERIVATE)
vasat_Vgatep_sourcep=((t0_Vgatep_sourcep*t1-t0*t1_Vgatep_sourcep)/t1/t1);
vasat_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t1-t0*t1_Vdrainp_sourcep)/t1/t1);
vasat_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t1-t0*t1_Vdrainb_sourcep)/t1/t1);
vasat_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t1-t0*t1_Vbulkp_sourcep)/t1/t1);
vasat_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t1-t0*t1_Vsourceb_sourcep)/t1/t1);
#endif
vasat=(t0/t1);
#if defined(_DERIVATE)
tmp1_Vgatep_sourcep=0.0;
tmp1_Vdrainp_sourcep=0.0;
tmp1_Vdrainb_sourcep=0.0;
tmp1_Vbulkp_sourcep=0.0;
tmp1_Vsourceb_sourcep=0.0;
#endif
tmp1=vtfbphi2;
#if defined(_DERIVATE)
tmp2_Vgatep_sourcep=0.0;
tmp2_Vdrainp_sourcep=0.0;
tmp2_Vdrainb_sourcep=0.0;
tmp2_Vbulkp_sourcep=0.0;
tmp2_Vsourceb_sourcep=0.0;
#endif
tmp2=(2.0e8*toxp);
#if defined(_DERIVATE)
dt0_dvg_Vgatep_sourcep=(-tmp2_Vgatep_sourcep/tmp2/tmp2);
dt0_dvg_Vdrainp_sourcep=(-tmp2_Vdrainp_sourcep/tmp2/tmp2);
dt0_dvg_Vdrainb_sourcep=(-tmp2_Vdrainb_sourcep/tmp2/tmp2);
dt0_dvg_Vbulkp_sourcep=(-tmp2_Vbulkp_sourcep/tmp2/tmp2);
dt0_dvg_Vsourceb_sourcep=(-tmp2_Vsourceb_sourcep/tmp2/tmp2);
#endif
dt0_dvg=(1.0/tmp2);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(((vgsteff_Vgatep_sourcep+tmp1_Vgatep_sourcep)*dt0_dvg)+((vgsteff+tmp1)*dt0_dvg_Vgatep_sourcep));
t0_Vdrainp_sourcep=(((vgsteff_Vdrainp_sourcep+tmp1_Vdrainp_sourcep)*dt0_dvg)+((vgsteff+tmp1)*dt0_dvg_Vdrainp_sourcep));
t0_Vdrainb_sourcep=(((vgsteff_Vdrainb_sourcep+tmp1_Vdrainb_sourcep)*dt0_dvg)+((vgsteff+tmp1)*dt0_dvg_Vdrainb_sourcep));
t0_Vbulkp_sourcep=(((vgsteff_Vbulkp_sourcep+tmp1_Vbulkp_sourcep)*dt0_dvg)+((vgsteff+tmp1)*dt0_dvg_Vbulkp_sourcep));
t0_Vsourceb_sourcep=(((vgsteff_Vsourceb_sourcep+tmp1_Vsourceb_sourcep)*dt0_dvg)+((vgsteff+tmp1)*dt0_dvg_Vsourceb_sourcep));
#endif
t0=((vgsteff+tmp1)*dt0_dvg);
{
double m00_logE(d00_logE0,t0)
double m00_exp(d00_exp1,(0.7*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,t0)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
tmp3_Vgatep_sourcep=(0.7*t0_Vgatep_sourcep*d10_logE0)*d10_exp1;
tmp3_Vdrainp_sourcep=(0.7*t0_Vdrainp_sourcep*d10_logE0)*d10_exp1;
tmp3_Vdrainb_sourcep=(0.7*t0_Vdrainb_sourcep*d10_logE0)*d10_exp1;
tmp3_Vbulkp_sourcep=(0.7*t0_Vbulkp_sourcep*d10_logE0)*d10_exp1;
tmp3_Vsourceb_sourcep=(0.7*t0_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
tmp3=d00_exp1;
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=tmp3_Vgatep_sourcep;
t1_Vdrainp_sourcep=tmp3_Vdrainp_sourcep;
t1_Vdrainb_sourcep=tmp3_Vdrainb_sourcep;
t1_Vbulkp_sourcep=tmp3_Vbulkp_sourcep;
t1_Vsourceb_sourcep=tmp3_Vsourceb_sourcep;
#endif
t1=(1.0+tmp3);
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=(-1.9e-9*t1_Vgatep_sourcep/t1/t1);
tcen_Vdrainp_sourcep=(-1.9e-9*t1_Vdrainp_sourcep/t1/t1);
tcen_Vdrainb_sourcep=(-1.9e-9*t1_Vdrainb_sourcep/t1/t1);
tcen_Vbulkp_sourcep=(-1.9e-9*t1_Vbulkp_sourcep/t1/t1);
tcen_Vsourceb_sourcep=(-1.9e-9*t1_Vsourceb_sourcep/t1/t1);
#endif
tcen=(1.9e-9/t1);
#if defined(_DERIVATE)
coxeff_Vgatep_sourcep=(-(1.03594e-10*coxp)*(coxp*tcen_Vgatep_sourcep)/(1.03594e-10+(coxp*tcen))/(1.03594e-10+(coxp*tcen)));
coxeff_Vdrainp_sourcep=(-(1.03594e-10*coxp)*(coxp*tcen_Vdrainp_sourcep)/(1.03594e-10+(coxp*tcen))/(1.03594e-10+(coxp*tcen)));
coxeff_Vdrainb_sourcep=(-(1.03594e-10*coxp)*(coxp*tcen_Vdrainb_sourcep)/(1.03594e-10+(coxp*tcen))/(1.03594e-10+(coxp*tcen)));
coxeff_Vbulkp_sourcep=(-(1.03594e-10*coxp)*(coxp*tcen_Vbulkp_sourcep)/(1.03594e-10+(coxp*tcen))/(1.03594e-10+(coxp*tcen)));
coxeff_Vsourceb_sourcep=(-(1.03594e-10*coxp)*(coxp*tcen_Vsourceb_sourcep)/(1.03594e-10+(coxp*tcen))/(1.03594e-10+(coxp*tcen)));
#endif
coxeff=((1.03594e-10*coxp)/(1.03594e-10+(coxp*tcen)));
#if defined(_DERIVATE)
coxeffwovl_Vgatep_sourcep=(((coxeff_Vgatep_sourcep*Weff)+(coxeff*Weff_Vgatep_sourcep))/leff);
coxeffwovl_Vdrainp_sourcep=(((coxeff_Vdrainp_sourcep*Weff)+(coxeff*Weff_Vdrainp_sourcep))/leff);
coxeffwovl_Vdrainb_sourcep=(((coxeff_Vdrainb_sourcep*Weff)+(coxeff*Weff_Vdrainb_sourcep))/leff);
coxeffwovl_Vbulkp_sourcep=(((coxeff_Vbulkp_sourcep*Weff)+(coxeff*Weff_Vbulkp_sourcep))/leff);
coxeffwovl_Vsourceb_sourcep=(((coxeff_Vsourceb_sourcep*Weff)+(coxeff*Weff_Vsourceb_sourcep))/leff);
#endif
coxeffwovl=((coxeff*Weff)/leff);
#if defined(_DERIVATE)
beta_Vgatep_sourcep=((ueff_Vgatep_sourcep*coxeffwovl)+(ueff*coxeffwovl_Vgatep_sourcep));
beta_Vdrainp_sourcep=((ueff_Vdrainp_sourcep*coxeffwovl)+(ueff*coxeffwovl_Vdrainp_sourcep));
beta_Vdrainb_sourcep=((ueff_Vdrainb_sourcep*coxeffwovl)+(ueff*coxeffwovl_Vdrainb_sourcep));
beta_Vbulkp_sourcep=((ueff_Vbulkp_sourcep*coxeffwovl)+(ueff*coxeffwovl_Vbulkp_sourcep));
beta_Vsourceb_sourcep=((ueff_Vsourceb_sourcep*coxeffwovl)+(ueff*coxeffwovl_Vsourceb_sourcep));
#endif
beta=(ueff*coxeffwovl);
#if defined(_DERIVATE)
abovvgst2vtm_Vgatep_sourcep=((abulk_Vgatep_sourcep*vgst2vtm-abulk*vgst2vtm_Vgatep_sourcep)/vgst2vtm/vgst2vtm);
abovvgst2vtm_Vdrainp_sourcep=((abulk_Vdrainp_sourcep*vgst2vtm-abulk*vgst2vtm_Vdrainp_sourcep)/vgst2vtm/vgst2vtm);
abovvgst2vtm_Vdrainb_sourcep=((abulk_Vdrainb_sourcep*vgst2vtm-abulk*vgst2vtm_Vdrainb_sourcep)/vgst2vtm/vgst2vtm);
abovvgst2vtm_Vbulkp_sourcep=((abulk_Vbulkp_sourcep*vgst2vtm-abulk*vgst2vtm_Vbulkp_sourcep)/vgst2vtm/vgst2vtm);
abovvgst2vtm_Vsourceb_sourcep=((abulk_Vsourceb_sourcep*vgst2vtm-abulk*vgst2vtm_Vsourceb_sourcep)/vgst2vtm/vgst2vtm);
#endif
abovvgst2vtm=(abulk/vgst2vtm);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-(((0.5*vdseff_Vgatep_sourcep)*abovvgst2vtm)+((0.5*vdseff)*abovvgst2vtm_Vgatep_sourcep)));
t0_Vdrainp_sourcep=(-(((0.5*vdseff_Vdrainp_sourcep)*abovvgst2vtm)+((0.5*vdseff)*abovvgst2vtm_Vdrainp_sourcep)));
t0_Vdrainb_sourcep=(-(((0.5*vdseff_Vdrainb_sourcep)*abovvgst2vtm)+((0.5*vdseff)*abovvgst2vtm_Vdrainb_sourcep)));
t0_Vbulkp_sourcep=(-(((0.5*vdseff_Vbulkp_sourcep)*abovvgst2vtm)+((0.5*vdseff)*abovvgst2vtm_Vbulkp_sourcep)));
t0_Vsourceb_sourcep=(-(((0.5*vdseff_Vsourceb_sourcep)*abovvgst2vtm)+((0.5*vdseff)*abovvgst2vtm_Vsourceb_sourcep)));
#endif
t0=(1.0-((0.5*vdseff)*abovvgst2vtm));
#if defined(_DERIVATE)
fgche1_Vgatep_sourcep=((vgsteff_Vgatep_sourcep*t0)+(vgsteff*t0_Vgatep_sourcep));
fgche1_Vdrainp_sourcep=((vgsteff_Vdrainp_sourcep*t0)+(vgsteff*t0_Vdrainp_sourcep));
fgche1_Vdrainb_sourcep=((vgsteff_Vdrainb_sourcep*t0)+(vgsteff*t0_Vdrainb_sourcep));
fgche1_Vbulkp_sourcep=((vgsteff_Vbulkp_sourcep*t0)+(vgsteff*t0_Vbulkp_sourcep));
fgche1_Vsourceb_sourcep=((vgsteff_Vsourceb_sourcep*t0)+(vgsteff*t0_Vsourceb_sourcep));
#endif
fgche1=(vgsteff*t0);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((vdseff_Vgatep_sourcep*esatl-vdseff*esatl_Vgatep_sourcep)/esatl/esatl);
t9_Vdrainp_sourcep=((vdseff_Vdrainp_sourcep*esatl-vdseff*esatl_Vdrainp_sourcep)/esatl/esatl);
t9_Vdrainb_sourcep=((vdseff_Vdrainb_sourcep*esatl-vdseff*esatl_Vdrainb_sourcep)/esatl/esatl);
t9_Vbulkp_sourcep=((vdseff_Vbulkp_sourcep*esatl-vdseff*esatl_Vbulkp_sourcep)/esatl/esatl);
t9_Vsourceb_sourcep=((vdseff_Vsourceb_sourcep*esatl-vdseff*esatl_Vsourceb_sourcep)/esatl/esatl);
#endif
t9=(vdseff/esatl);
#if defined(_DERIVATE)
fgche2_Vgatep_sourcep=t9_Vgatep_sourcep;
fgche2_Vdrainp_sourcep=t9_Vdrainp_sourcep;
fgche2_Vdrainb_sourcep=t9_Vdrainb_sourcep;
fgche2_Vbulkp_sourcep=t9_Vbulkp_sourcep;
fgche2_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
fgche2=(1.0+t9);
#if defined(_DERIVATE)
gche_Vgatep_sourcep=((((beta_Vgatep_sourcep*fgche1)+(beta*fgche1_Vgatep_sourcep))*fgche2-(beta*fgche1)*fgche2_Vgatep_sourcep)/fgche2/fgche2);
gche_Vdrainp_sourcep=((((beta_Vdrainp_sourcep*fgche1)+(beta*fgche1_Vdrainp_sourcep))*fgche2-(beta*fgche1)*fgche2_Vdrainp_sourcep)/fgche2/fgche2);
gche_Vdrainb_sourcep=((((beta_Vdrainb_sourcep*fgche1)+(beta*fgche1_Vdrainb_sourcep))*fgche2-(beta*fgche1)*fgche2_Vdrainb_sourcep)/fgche2/fgche2);
gche_Vbulkp_sourcep=((((beta_Vbulkp_sourcep*fgche1)+(beta*fgche1_Vbulkp_sourcep))*fgche2-(beta*fgche1)*fgche2_Vbulkp_sourcep)/fgche2/fgche2);
gche_Vsourceb_sourcep=((((beta_Vsourceb_sourcep*fgche1)+(beta*fgche1_Vsourceb_sourcep))*fgche2-(beta*fgche1)*fgche2_Vsourceb_sourcep)/fgche2/fgche2);
#endif
gche=((beta*fgche1)/fgche2);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((gche_Vgatep_sourcep*rds)+(gche*rds_Vgatep_sourcep));
t0_Vdrainp_sourcep=((gche_Vdrainp_sourcep*rds)+(gche*rds_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((gche_Vdrainb_sourcep*rds)+(gche*rds_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((gche_Vbulkp_sourcep*rds)+(gche*rds_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((gche_Vsourceb_sourcep*rds)+(gche*rds_Vsourceb_sourcep));
#endif
t0=(1.0+(gche*rds));
#if defined(_DERIVATE)
idl_Vgatep_sourcep=((gche_Vgatep_sourcep*t0-gche*t0_Vgatep_sourcep)/t0/t0);
idl_Vdrainp_sourcep=((gche_Vdrainp_sourcep*t0-gche*t0_Vdrainp_sourcep)/t0/t0);
idl_Vdrainb_sourcep=((gche_Vdrainb_sourcep*t0-gche*t0_Vdrainb_sourcep)/t0/t0);
idl_Vbulkp_sourcep=((gche_Vbulkp_sourcep*t0-gche*t0_Vbulkp_sourcep)/t0/t0);
idl_Vsourceb_sourcep=((gche_Vsourceb_sourcep*t0-gche*t0_Vsourceb_sourcep)/t0/t0);
#endif
idl=(gche/t0);
if
((fprout<=0.0))
{
#if defined(_DERIVATE)
fp_Vgatep_sourcep=0.0;
fp_Vdrainp_sourcep=0.0;
fp_Vdrainb_sourcep=0.0;
fp_Vbulkp_sourcep=0.0;
fp_Vsourceb_sourcep=0.0;
#endif
fp=1.0;
}
else
{
{
double m00_sqrt(d00_sqrt0,leff)
#if defined(_DERIVATE)
t9_Vgatep_sourcep=(-(fprout*d00_sqrt0)*vgst2vtm_Vgatep_sourcep/vgst2vtm/vgst2vtm);
t9_Vdrainp_sourcep=(-(fprout*d00_sqrt0)*vgst2vtm_Vdrainp_sourcep/vgst2vtm/vgst2vtm);
t9_Vdrainb_sourcep=(-(fprout*d00_sqrt0)*vgst2vtm_Vdrainb_sourcep/vgst2vtm/vgst2vtm);
t9_Vbulkp_sourcep=(-(fprout*d00_sqrt0)*vgst2vtm_Vbulkp_sourcep/vgst2vtm/vgst2vtm);
t9_Vsourceb_sourcep=(-(fprout*d00_sqrt0)*vgst2vtm_Vsourceb_sourcep/vgst2vtm/vgst2vtm);
#endif
t9=((fprout*d00_sqrt0)/vgst2vtm);
}
#if defined(_DERIVATE)
fp_Vgatep_sourcep=(-t9_Vgatep_sourcep/(1.0+t9)/(1.0+t9));
fp_Vdrainp_sourcep=(-t9_Vdrainp_sourcep/(1.0+t9)/(1.0+t9));
fp_Vdrainb_sourcep=(-t9_Vdrainb_sourcep/(1.0+t9)/(1.0+t9));
fp_Vbulkp_sourcep=(-t9_Vbulkp_sourcep/(1.0+t9)/(1.0+t9));
fp_Vsourceb_sourcep=(-t9_Vsourceb_sourcep/(1.0+t9)/(1.0+t9));
#endif
fp=(1.0/(1.0+t9));
}
#if defined(_DERIVATE)
t8_Vgatep_sourcep=(-pvag*esatl_Vgatep_sourcep/esatl/esatl);
t8_Vdrainp_sourcep=(-pvag*esatl_Vdrainp_sourcep/esatl/esatl);
t8_Vdrainb_sourcep=(-pvag*esatl_Vdrainb_sourcep/esatl/esatl);
t8_Vbulkp_sourcep=(-pvag*esatl_Vbulkp_sourcep/esatl/esatl);
t8_Vsourceb_sourcep=(-pvag*esatl_Vsourceb_sourcep/esatl/esatl);
#endif
t8=(pvag/esatl);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((t8_Vgatep_sourcep*vgsteff)+(t8*vgsteff_Vgatep_sourcep));
t9_Vdrainp_sourcep=((t8_Vdrainp_sourcep*vgsteff)+(t8*vgsteff_Vdrainp_sourcep));
t9_Vdrainb_sourcep=((t8_Vdrainb_sourcep*vgsteff)+(t8*vgsteff_Vdrainb_sourcep));
t9_Vbulkp_sourcep=((t8_Vbulkp_sourcep*vgsteff)+(t8*vgsteff_Vbulkp_sourcep));
t9_Vsourceb_sourcep=((t8_Vsourceb_sourcep*vgsteff)+(t8*vgsteff_Vsourceb_sourcep));
#endif
t9=(t8*vgsteff);
if
((t9>(-0.9)))
{
#if defined(_DERIVATE)
pvagterm_Vgatep_sourcep=t9_Vgatep_sourcep;
pvagterm_Vdrainp_sourcep=t9_Vdrainp_sourcep;
pvagterm_Vdrainb_sourcep=t9_Vdrainb_sourcep;
pvagterm_Vbulkp_sourcep=t9_Vbulkp_sourcep;
pvagterm_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
pvagterm=(1.0+t9);
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-(20.0*t9_Vgatep_sourcep)/(17.0+(20.0*t9))/(17.0+(20.0*t9)));
t4_Vdrainp_sourcep=(-(20.0*t9_Vdrainp_sourcep)/(17.0+(20.0*t9))/(17.0+(20.0*t9)));
t4_Vdrainb_sourcep=(-(20.0*t9_Vdrainb_sourcep)/(17.0+(20.0*t9))/(17.0+(20.0*t9)));
t4_Vbulkp_sourcep=(-(20.0*t9_Vbulkp_sourcep)/(17.0+(20.0*t9))/(17.0+(20.0*t9)));
t4_Vsourceb_sourcep=(-(20.0*t9_Vsourceb_sourcep)/(17.0+(20.0*t9))/(17.0+(20.0*t9)));
#endif
t4=(1.0/(17.0+(20.0*t9)));
#if defined(_DERIVATE)
pvagterm_Vgatep_sourcep=((t9_Vgatep_sourcep*t4)+((0.8+t9)*t4_Vgatep_sourcep));
pvagterm_Vdrainp_sourcep=((t9_Vdrainp_sourcep*t4)+((0.8+t9)*t4_Vdrainp_sourcep));
pvagterm_Vdrainb_sourcep=((t9_Vdrainb_sourcep*t4)+((0.8+t9)*t4_Vdrainb_sourcep));
pvagterm_Vbulkp_sourcep=((t9_Vbulkp_sourcep*t4)+((0.8+t9)*t4_Vbulkp_sourcep));
pvagterm_Vsourceb_sourcep=((t9_Vsourceb_sourcep*t4)+((0.8+t9)*t4_Vsourceb_sourcep));
#endif
pvagterm=((0.8+t9)*t4);
}
if
(((pclm>0.0)&&(diffvds>1.0e-10)))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((rds_Vgatep_sourcep*idl)+(rds*idl_Vgatep_sourcep));
t0_Vdrainp_sourcep=((rds_Vdrainp_sourcep*idl)+(rds*idl_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((rds_Vdrainb_sourcep*idl)+(rds*idl_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((rds_Vbulkp_sourcep*idl)+(rds*idl_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((rds_Vsourceb_sourcep*idl)+(rds*idl_Vsourceb_sourcep));
#endif
t0=(1.0+(rds*idl));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((vdsat_Vgatep_sourcep*esat-vdsat*esat_Vgatep_sourcep)/esat/esat);
t2_Vdrainp_sourcep=((vdsat_Vdrainp_sourcep*esat-vdsat*esat_Vdrainp_sourcep)/esat/esat);
t2_Vdrainb_sourcep=((vdsat_Vdrainb_sourcep*esat-vdsat*esat_Vdrainb_sourcep)/esat/esat);
t2_Vbulkp_sourcep=((vdsat_Vbulkp_sourcep*esat-vdsat*esat_Vbulkp_sourcep)/esat/esat);
t2_Vsourceb_sourcep=((vdsat_Vsourceb_sourcep*esat-vdsat*esat_Vsourceb_sourcep)/esat/esat);
#endif
t2=(vdsat/esat);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=t2_Vgatep_sourcep;
t1_Vdrainp_sourcep=t2_Vdrainp_sourcep;
t1_Vdrainb_sourcep=t2_Vdrainb_sourcep;
t1_Vbulkp_sourcep=t2_Vbulkp_sourcep;
t1_Vsourceb_sourcep=t2_Vsourceb_sourcep;
#endif
t1=(leff+t2);
#if defined(_DERIVATE)
cclm_Vgatep_sourcep=(((((((fp_Vgatep_sourcep*pvagterm)+(fp*pvagterm_Vgatep_sourcep))*t0)+((fp*pvagterm)*t0_Vgatep_sourcep))*t1)+(((fp*pvagterm)*t0)*t1_Vgatep_sourcep))/(pclm*litl));
cclm_Vdrainp_sourcep=(((((((fp_Vdrainp_sourcep*pvagterm)+(fp*pvagterm_Vdrainp_sourcep))*t0)+((fp*pvagterm)*t0_Vdrainp_sourcep))*t1)+(((fp*pvagterm)*t0)*t1_Vdrainp_sourcep))/(pclm*litl));
cclm_Vdrainb_sourcep=(((((((fp_Vdrainb_sourcep*pvagterm)+(fp*pvagterm_Vdrainb_sourcep))*t0)+((fp*pvagterm)*t0_Vdrainb_sourcep))*t1)+(((fp*pvagterm)*t0)*t1_Vdrainb_sourcep))/(pclm*litl));
cclm_Vbulkp_sourcep=(((((((fp_Vbulkp_sourcep*pvagterm)+(fp*pvagterm_Vbulkp_sourcep))*t0)+((fp*pvagterm)*t0_Vbulkp_sourcep))*t1)+(((fp*pvagterm)*t0)*t1_Vbulkp_sourcep))/(pclm*litl));
cclm_Vsourceb_sourcep=(((((((fp_Vsourceb_sourcep*pvagterm)+(fp*pvagterm_Vsourceb_sourcep))*t0)+((fp*pvagterm)*t0_Vsourceb_sourcep))*t1)+(((fp*pvagterm)*t0)*t1_Vsourceb_sourcep))/(pclm*litl));
#endif
cclm=((((fp*pvagterm)*t0)*t1)/(pclm*litl));
#if defined(_DERIVATE)
vaclm_Vgatep_sourcep=((cclm_Vgatep_sourcep*diffvds)+(cclm*diffvds_Vgatep_sourcep));
vaclm_Vdrainp_sourcep=((cclm_Vdrainp_sourcep*diffvds)+(cclm*diffvds_Vdrainp_sourcep));
vaclm_Vdrainb_sourcep=((cclm_Vdrainb_sourcep*diffvds)+(cclm*diffvds_Vdrainb_sourcep));
vaclm_Vbulkp_sourcep=((cclm_Vbulkp_sourcep*diffvds)+(cclm*diffvds_Vbulkp_sourcep));
vaclm_Vsourceb_sourcep=((cclm_Vsourceb_sourcep*diffvds)+(cclm*diffvds_Vsourceb_sourcep));
#endif
vaclm=(cclm*diffvds);
}
else
{
#if defined(_DERIVATE)
vaclm_Vgatep_sourcep=0.0;
vaclm_Vdrainp_sourcep=0.0;
vaclm_Vdrainb_sourcep=0.0;
vaclm_Vbulkp_sourcep=0.0;
vaclm_Vsourceb_sourcep=0.0;
#endif
vaclm=5.834617425e14;
#if defined(_DERIVATE)
cclm_Vgatep_sourcep=0.0;
cclm_Vdrainp_sourcep=0.0;
cclm_Vdrainb_sourcep=0.0;
cclm_Vbulkp_sourcep=0.0;
cclm_Vsourceb_sourcep=0.0;
#endif
cclm=5.834617425e14;
}
if
((thetarout>0.0))
{
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((abulk_Vgatep_sourcep*vdsat)+(abulk*vdsat_Vgatep_sourcep));
t8_Vdrainp_sourcep=((abulk_Vdrainp_sourcep*vdsat)+(abulk*vdsat_Vdrainp_sourcep));
t8_Vdrainb_sourcep=((abulk_Vdrainb_sourcep*vdsat)+(abulk*vdsat_Vdrainb_sourcep));
t8_Vbulkp_sourcep=((abulk_Vbulkp_sourcep*vdsat)+(abulk*vdsat_Vbulkp_sourcep));
t8_Vsourceb_sourcep=((abulk_Vsourceb_sourcep*vdsat)+(abulk*vdsat_Vsourceb_sourcep));
#endif
t8=(abulk*vdsat);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((vgst2vtm_Vgatep_sourcep*t8)+(vgst2vtm*t8_Vgatep_sourcep));
t0_Vdrainp_sourcep=((vgst2vtm_Vdrainp_sourcep*t8)+(vgst2vtm*t8_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((vgst2vtm_Vdrainb_sourcep*t8)+(vgst2vtm*t8_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((vgst2vtm_Vbulkp_sourcep*t8)+(vgst2vtm*t8_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((vgst2vtm_Vsourceb_sourcep*t8)+(vgst2vtm*t8_Vsourceb_sourcep));
#endif
t0=(vgst2vtm*t8);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vgst2vtm_Vgatep_sourcep+t8_Vgatep_sourcep);
t1_Vdrainp_sourcep=(vgst2vtm_Vdrainp_sourcep+t8_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(vgst2vtm_Vdrainb_sourcep+t8_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(vgst2vtm_Vbulkp_sourcep+t8_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(vgst2vtm_Vsourceb_sourcep+t8_Vsourceb_sourcep);
#endif
t1=(vgst2vtm+t8);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=thetarout;
#if defined(_DERIVATE)
vadibl_Vgatep_sourcep=(((vgst2vtm_Vgatep_sourcep-((t0_Vgatep_sourcep*t1-t0*t1_Vgatep_sourcep)/t1/t1))*t2-(vgst2vtm-(t0/t1))*t2_Vgatep_sourcep)/t2/t2);
vadibl_Vdrainp_sourcep=(((vgst2vtm_Vdrainp_sourcep-((t0_Vdrainp_sourcep*t1-t0*t1_Vdrainp_sourcep)/t1/t1))*t2-(vgst2vtm-(t0/t1))*t2_Vdrainp_sourcep)/t2/t2);
vadibl_Vdrainb_sourcep=(((vgst2vtm_Vdrainb_sourcep-((t0_Vdrainb_sourcep*t1-t0*t1_Vdrainb_sourcep)/t1/t1))*t2-(vgst2vtm-(t0/t1))*t2_Vdrainb_sourcep)/t2/t2);
vadibl_Vbulkp_sourcep=(((vgst2vtm_Vbulkp_sourcep-((t0_Vbulkp_sourcep*t1-t0*t1_Vbulkp_sourcep)/t1/t1))*t2-(vgst2vtm-(t0/t1))*t2_Vbulkp_sourcep)/t2/t2);
vadibl_Vsourceb_sourcep=(((vgst2vtm_Vsourceb_sourcep-((t0_Vsourceb_sourcep*t1-t0*t1_Vsourceb_sourcep)/t1/t1))*t2-(vgst2vtm-(t0/t1))*t2_Vsourceb_sourcep)/t2/t2);
#endif
vadibl=((vgst2vtm-(t0/t1))/t2);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=0.0;
t7_Vdrainp_sourcep=(pdiblb*vbseff_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(pdiblb*vbseff_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(pdiblb*vbseff_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(pdiblb*vbseff_Vsourceb_sourcep);
#endif
t7=(pdiblb*vbseff);
if
((t7>=(-0.9)))
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(-t7_Vgatep_sourcep/(1.0+t7)/(1.0+t7));
t3_Vdrainp_sourcep=(-t7_Vdrainp_sourcep/(1.0+t7)/(1.0+t7));
t3_Vdrainb_sourcep=(-t7_Vdrainb_sourcep/(1.0+t7)/(1.0+t7));
t3_Vbulkp_sourcep=(-t7_Vbulkp_sourcep/(1.0+t7)/(1.0+t7));
t3_Vsourceb_sourcep=(-t7_Vsourceb_sourcep/(1.0+t7)/(1.0+t7));
#endif
t3=(1.0/(1.0+t7));
#if defined(_DERIVATE)
vadibl_Vgatep_sourcep=((vadibl_Vgatep_sourcep*t3)+(vadibl*t3_Vgatep_sourcep));
vadibl_Vdrainp_sourcep=((vadibl_Vdrainp_sourcep*t3)+(vadibl*t3_Vdrainp_sourcep));
vadibl_Vdrainb_sourcep=((vadibl_Vdrainb_sourcep*t3)+(vadibl*t3_Vdrainb_sourcep));
vadibl_Vbulkp_sourcep=((vadibl_Vbulkp_sourcep*t3)+(vadibl*t3_Vbulkp_sourcep));
vadibl_Vsourceb_sourcep=((vadibl_Vsourceb_sourcep*t3)+(vadibl*t3_Vsourceb_sourcep));
#endif
vadibl=(vadibl*t3);
}
else
{
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-t7_Vgatep_sourcep/(0.8+t7)/(0.8+t7));
t4_Vdrainp_sourcep=(-t7_Vdrainp_sourcep/(0.8+t7)/(0.8+t7));
t4_Vdrainb_sourcep=(-t7_Vdrainb_sourcep/(0.8+t7)/(0.8+t7));
t4_Vbulkp_sourcep=(-t7_Vbulkp_sourcep/(0.8+t7)/(0.8+t7));
t4_Vsourceb_sourcep=(-t7_Vsourceb_sourcep/(0.8+t7)/(0.8+t7));
#endif
t4=(1.0/(0.8+t7));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((20.0*t7_Vgatep_sourcep)*t4)+((17.0+(20.0*t7))*t4_Vgatep_sourcep));
t3_Vdrainp_sourcep=(((20.0*t7_Vdrainp_sourcep)*t4)+((17.0+(20.0*t7))*t4_Vdrainp_sourcep));
t3_Vdrainb_sourcep=(((20.0*t7_Vdrainb_sourcep)*t4)+((17.0+(20.0*t7))*t4_Vdrainb_sourcep));
t3_Vbulkp_sourcep=(((20.0*t7_Vbulkp_sourcep)*t4)+((17.0+(20.0*t7))*t4_Vbulkp_sourcep));
t3_Vsourceb_sourcep=(((20.0*t7_Vsourceb_sourcep)*t4)+((17.0+(20.0*t7))*t4_Vsourceb_sourcep));
#endif
t3=((17.0+(20.0*t7))*t4);
#if defined(_DERIVATE)
vadibl_Vgatep_sourcep=((vadibl_Vgatep_sourcep*t3)+(vadibl*t3_Vgatep_sourcep));
vadibl_Vdrainp_sourcep=((vadibl_Vdrainp_sourcep*t3)+(vadibl*t3_Vdrainp_sourcep));
vadibl_Vdrainb_sourcep=((vadibl_Vdrainb_sourcep*t3)+(vadibl*t3_Vdrainb_sourcep));
vadibl_Vbulkp_sourcep=((vadibl_Vbulkp_sourcep*t3)+(vadibl*t3_Vbulkp_sourcep));
vadibl_Vsourceb_sourcep=((vadibl_Vsourceb_sourcep*t3)+(vadibl*t3_Vsourceb_sourcep));
#endif
vadibl=(vadibl*t3);
}
#if defined(_DERIVATE)
vadibl_Vgatep_sourcep=((vadibl_Vgatep_sourcep*pvagterm)+(vadibl*pvagterm_Vgatep_sourcep));
vadibl_Vdrainp_sourcep=((vadibl_Vdrainp_sourcep*pvagterm)+(vadibl*pvagterm_Vdrainp_sourcep));
vadibl_Vdrainb_sourcep=((vadibl_Vdrainb_sourcep*pvagterm)+(vadibl*pvagterm_Vdrainb_sourcep));
vadibl_Vbulkp_sourcep=((vadibl_Vbulkp_sourcep*pvagterm)+(vadibl*pvagterm_Vbulkp_sourcep));
vadibl_Vsourceb_sourcep=((vadibl_Vsourceb_sourcep*pvagterm)+(vadibl*pvagterm_Vsourceb_sourcep));
#endif
vadibl=(vadibl*pvagterm);
}
else
{
#if defined(_DERIVATE)
vadibl_Vgatep_sourcep=0.0;
vadibl_Vdrainp_sourcep=0.0;
vadibl_Vdrainb_sourcep=0.0;
vadibl_Vbulkp_sourcep=0.0;
vadibl_Vsourceb_sourcep=0.0;
#endif
vadibl=5.834617425e14;
}
if
(((pditsd*Vds)>34.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=5.834617425e14;
}
else
{
{
double m00_exp(d00_exp0,(pditsd*Vds))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=(pditsd*Vds_Vdrainp_sourcep)*d10_exp0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=d00_exp0;
}
}
if
((pdits>0.0))
{
#if defined(_DERIVATE)
vadits_Vgatep_sourcep=(((((1.0+(pditsl*leff))*t1_Vgatep_sourcep)/pdits)*fp)+(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp_Vgatep_sourcep));
vadits_Vdrainp_sourcep=(((((1.0+(pditsl*leff))*t1_Vdrainp_sourcep)/pdits)*fp)+(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp_Vdrainp_sourcep));
vadits_Vdrainb_sourcep=(((((1.0+(pditsl*leff))*t1_Vdrainb_sourcep)/pdits)*fp)+(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp_Vdrainb_sourcep));
vadits_Vbulkp_sourcep=(((((1.0+(pditsl*leff))*t1_Vbulkp_sourcep)/pdits)*fp)+(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp_Vbulkp_sourcep));
vadits_Vsourceb_sourcep=(((((1.0+(pditsl*leff))*t1_Vsourceb_sourcep)/pdits)*fp)+(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp_Vsourceb_sourcep));
#endif
vadits=(((1.0+((1.0+(pditsl*leff))*t1))/pdits)*fp);
}
else
{
#if defined(_DERIVATE)
vadits_Vgatep_sourcep=0.0;
vadits_Vdrainp_sourcep=0.0;
vadits_Vdrainb_sourcep=0.0;
vadits_Vbulkp_sourcep=0.0;
vadits_Vsourceb_sourcep=0.0;
#endif
vadits=5.834617425e14;
}
if
((pscbe2>0.0))
{
if
((diffvds>((pscbe1*litl)/34.0)))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-(pscbe1*litl)*diffvds_Vgatep_sourcep/diffvds/diffvds);
t0_Vdrainp_sourcep=(-(pscbe1*litl)*diffvds_Vdrainp_sourcep/diffvds/diffvds);
t0_Vdrainb_sourcep=(-(pscbe1*litl)*diffvds_Vdrainb_sourcep/diffvds/diffvds);
t0_Vbulkp_sourcep=(-(pscbe1*litl)*diffvds_Vbulkp_sourcep/diffvds/diffvds);
t0_Vsourceb_sourcep=(-(pscbe1*litl)*diffvds_Vsourceb_sourcep/diffvds/diffvds);
#endif
t0=((pscbe1*litl)/diffvds);
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
vascbe_Vgatep_sourcep=((leff*t0_Vgatep_sourcep*d10_exp0)/pscbe2);
vascbe_Vdrainp_sourcep=((leff*t0_Vdrainp_sourcep*d10_exp0)/pscbe2);
vascbe_Vdrainb_sourcep=((leff*t0_Vdrainb_sourcep*d10_exp0)/pscbe2);
vascbe_Vbulkp_sourcep=((leff*t0_Vbulkp_sourcep*d10_exp0)/pscbe2);
vascbe_Vsourceb_sourcep=((leff*t0_Vsourceb_sourcep*d10_exp0)/pscbe2);
#endif
vascbe=((leff*d00_exp0)/pscbe2);
}
}
else
{
#if defined(_DERIVATE)
vascbe_Vgatep_sourcep=0.0;
vascbe_Vdrainp_sourcep=0.0;
vascbe_Vdrainb_sourcep=0.0;
vascbe_Vbulkp_sourcep=0.0;
vascbe_Vsourceb_sourcep=0.0;
#endif
vascbe=((5.834617425e14*leff)/pscbe2);
}
}
else
{
#if defined(_DERIVATE)
vascbe_Vgatep_sourcep=0.0;
vascbe_Vdrainp_sourcep=0.0;
vascbe_Vdrainb_sourcep=0.0;
vascbe_Vbulkp_sourcep=0.0;
vascbe_Vsourceb_sourcep=0.0;
#endif
vascbe=5.834617425e14;
}
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((diffvds_Vgatep_sourcep*vadibl-diffvds*vadibl_Vgatep_sourcep)/vadibl/vadibl);
t9_Vdrainp_sourcep=((diffvds_Vdrainp_sourcep*vadibl-diffvds*vadibl_Vdrainp_sourcep)/vadibl/vadibl);
t9_Vdrainb_sourcep=((diffvds_Vdrainb_sourcep*vadibl-diffvds*vadibl_Vdrainb_sourcep)/vadibl/vadibl);
t9_Vbulkp_sourcep=((diffvds_Vbulkp_sourcep*vadibl-diffvds*vadibl_Vbulkp_sourcep)/vadibl/vadibl);
t9_Vsourceb_sourcep=((diffvds_Vsourceb_sourcep*vadibl-diffvds*vadibl_Vsourceb_sourcep)/vadibl/vadibl);
#endif
t9=(diffvds/vadibl);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=t9_Vgatep_sourcep;
t0_Vdrainp_sourcep=t9_Vdrainp_sourcep;
t0_Vdrainb_sourcep=t9_Vdrainb_sourcep;
t0_Vbulkp_sourcep=t9_Vbulkp_sourcep;
t0_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
t0=(1.0+t9);
#if defined(_DERIVATE)
idsa_Vgatep_sourcep=((idl_Vgatep_sourcep*t0)+(idl*t0_Vgatep_sourcep));
idsa_Vdrainp_sourcep=((idl_Vdrainp_sourcep*t0)+(idl*t0_Vdrainp_sourcep));
idsa_Vdrainb_sourcep=((idl_Vdrainb_sourcep*t0)+(idl*t0_Vdrainb_sourcep));
idsa_Vbulkp_sourcep=((idl_Vbulkp_sourcep*t0)+(idl*t0_Vbulkp_sourcep));
idsa_Vsourceb_sourcep=((idl_Vsourceb_sourcep*t0)+(idl*t0_Vsourceb_sourcep));
#endif
idsa=(idl*t0);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((diffvds_Vgatep_sourcep*vadits-diffvds*vadits_Vgatep_sourcep)/vadits/vadits);
t9_Vdrainp_sourcep=((diffvds_Vdrainp_sourcep*vadits-diffvds*vadits_Vdrainp_sourcep)/vadits/vadits);
t9_Vdrainb_sourcep=((diffvds_Vdrainb_sourcep*vadits-diffvds*vadits_Vdrainb_sourcep)/vadits/vadits);
t9_Vbulkp_sourcep=((diffvds_Vbulkp_sourcep*vadits-diffvds*vadits_Vbulkp_sourcep)/vadits/vadits);
t9_Vsourceb_sourcep=((diffvds_Vsourceb_sourcep*vadits-diffvds*vadits_Vsourceb_sourcep)/vadits/vadits);
#endif
t9=(diffvds/vadits);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=t9_Vgatep_sourcep;
t0_Vdrainp_sourcep=t9_Vdrainp_sourcep;
t0_Vdrainb_sourcep=t9_Vdrainb_sourcep;
t0_Vbulkp_sourcep=t9_Vbulkp_sourcep;
t0_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
t0=(1.0+t9);
#if defined(_DERIVATE)
idsa_Vgatep_sourcep=((idsa_Vgatep_sourcep*t0)+(idsa*t0_Vgatep_sourcep));
idsa_Vdrainp_sourcep=((idsa_Vdrainp_sourcep*t0)+(idsa*t0_Vdrainp_sourcep));
idsa_Vdrainb_sourcep=((idsa_Vdrainb_sourcep*t0)+(idsa*t0_Vdrainb_sourcep));
idsa_Vbulkp_sourcep=((idsa_Vbulkp_sourcep*t0)+(idsa*t0_Vbulkp_sourcep));
idsa_Vsourceb_sourcep=((idsa_Vsourceb_sourcep*t0)+(idsa*t0_Vsourceb_sourcep));
#endif
idsa=(idsa*t0);
{
double m00_logE(d00_logE0,((vasat+vaclm)/vasat))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,((vasat+vaclm)/vasat))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(((vasat_Vgatep_sourcep+vaclm_Vgatep_sourcep)*vasat-(vasat+vaclm)*vasat_Vgatep_sourcep)/vasat/vasat)*d10_logE0;
t0_Vdrainp_sourcep=(((vasat_Vdrainp_sourcep+vaclm_Vdrainp_sourcep)*vasat-(vasat+vaclm)*vasat_Vdrainp_sourcep)/vasat/vasat)*d10_logE0;
t0_Vdrainb_sourcep=(((vasat_Vdrainb_sourcep+vaclm_Vdrainb_sourcep)*vasat-(vasat+vaclm)*vasat_Vdrainb_sourcep)/vasat/vasat)*d10_logE0;
t0_Vbulkp_sourcep=(((vasat_Vbulkp_sourcep+vaclm_Vbulkp_sourcep)*vasat-(vasat+vaclm)*vasat_Vbulkp_sourcep)/vasat/vasat)*d10_logE0;
t0_Vsourceb_sourcep=(((vasat_Vsourceb_sourcep+vaclm_Vsourceb_sourcep)*vasat-(vasat+vaclm)*vasat_Vsourceb_sourcep)/vasat/vasat)*d10_logE0;
#endif
t0=d00_logE0;
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*cclm-t0*cclm_Vgatep_sourcep)/cclm/cclm);
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*cclm-t0*cclm_Vdrainp_sourcep)/cclm/cclm);
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*cclm-t0*cclm_Vdrainb_sourcep)/cclm/cclm);
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*cclm-t0*cclm_Vbulkp_sourcep)/cclm/cclm);
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*cclm-t0*cclm_Vsourceb_sourcep)/cclm/cclm);
#endif
t1=(t0/cclm);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=t1_Vgatep_sourcep;
t9_Vdrainp_sourcep=t1_Vdrainp_sourcep;
t9_Vdrainb_sourcep=t1_Vdrainb_sourcep;
t9_Vbulkp_sourcep=t1_Vbulkp_sourcep;
t9_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
t9=(1.0+t1);
#if defined(_DERIVATE)
idsa_Vgatep_sourcep=((idsa_Vgatep_sourcep*t9)+(idsa*t9_Vgatep_sourcep));
idsa_Vdrainp_sourcep=((idsa_Vdrainp_sourcep*t9)+(idsa*t9_Vdrainp_sourcep));
idsa_Vdrainb_sourcep=((idsa_Vdrainb_sourcep*t9)+(idsa*t9_Vdrainb_sourcep));
idsa_Vbulkp_sourcep=((idsa_Vbulkp_sourcep*t9)+(idsa*t9_Vbulkp_sourcep));
idsa_Vsourceb_sourcep=((idsa_Vsourceb_sourcep*t9)+(idsa*t9_Vsourceb_sourcep));
#endif
idsa=(idsa*t9);
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=0.0;
tmp_Vdrainp_sourcep=0.0;
tmp_Vdrainb_sourcep=0.0;
tmp_Vbulkp_sourcep=0.0;
tmp_Vsourceb_sourcep=0.0;
#endif
tmp=(alpha0+(alpha1*leff));
if
(((tmp<=0.0)||(beta0<=0.0)))
{
#if defined(_DERIVATE)
isub_Vgatep_sourcep=0.0;
isub_Vdrainp_sourcep=0.0;
isub_Vdrainb_sourcep=0.0;
isub_Vbulkp_sourcep=0.0;
isub_Vsourceb_sourcep=0.0;
#endif
isub=0.0;
}
else
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=(tmp/leff);
if
((diffvds>(beta0/34.0)))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-(-beta0)*diffvds_Vgatep_sourcep/diffvds/diffvds);
t0_Vdrainp_sourcep=(-(-beta0)*diffvds_Vdrainp_sourcep/diffvds/diffvds);
t0_Vdrainb_sourcep=(-(-beta0)*diffvds_Vdrainb_sourcep/diffvds/diffvds);
t0_Vbulkp_sourcep=(-(-beta0)*diffvds_Vbulkp_sourcep/diffvds/diffvds);
t0_Vsourceb_sourcep=(-(-beta0)*diffvds_Vsourceb_sourcep/diffvds/diffvds);
#endif
t0=((-beta0)/diffvds);
{
double m00_exp(d00_exp0,t0)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((((t2_Vgatep_sourcep*diffvds)+(t2*diffvds_Vgatep_sourcep))*d00_exp0)+((t2*diffvds)*t0_Vgatep_sourcep*d10_exp0));
t1_Vdrainp_sourcep=((((t2_Vdrainp_sourcep*diffvds)+(t2*diffvds_Vdrainp_sourcep))*d00_exp0)+((t2*diffvds)*t0_Vdrainp_sourcep*d10_exp0));
t1_Vdrainb_sourcep=((((t2_Vdrainb_sourcep*diffvds)+(t2*diffvds_Vdrainb_sourcep))*d00_exp0)+((t2*diffvds)*t0_Vdrainb_sourcep*d10_exp0));
t1_Vbulkp_sourcep=((((t2_Vbulkp_sourcep*diffvds)+(t2*diffvds_Vbulkp_sourcep))*d00_exp0)+((t2*diffvds)*t0_Vbulkp_sourcep*d10_exp0));
t1_Vsourceb_sourcep=((((t2_Vsourceb_sourcep*diffvds)+(t2*diffvds_Vsourceb_sourcep))*d00_exp0)+((t2*diffvds)*t0_Vsourceb_sourcep*d10_exp0));
#endif
t1=((t2*diffvds)*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=t2_Vgatep_sourcep*1.713908431e-15;
t3_Vdrainp_sourcep=t2_Vdrainp_sourcep*1.713908431e-15;
t3_Vdrainb_sourcep=t2_Vdrainb_sourcep*1.713908431e-15;
t3_Vbulkp_sourcep=t2_Vbulkp_sourcep*1.713908431e-15;
t3_Vsourceb_sourcep=t2_Vsourceb_sourcep*1.713908431e-15;
#endif
t3=(t2*1.713908431e-15);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t3_Vgatep_sourcep*diffvds)+(t3*diffvds_Vgatep_sourcep));
t1_Vdrainp_sourcep=((t3_Vdrainp_sourcep*diffvds)+(t3*diffvds_Vdrainp_sourcep));
t1_Vdrainb_sourcep=((t3_Vdrainb_sourcep*diffvds)+(t3*diffvds_Vdrainb_sourcep));
t1_Vbulkp_sourcep=((t3_Vbulkp_sourcep*diffvds)+(t3*diffvds_Vbulkp_sourcep));
t1_Vsourceb_sourcep=((t3_Vsourceb_sourcep*diffvds)+(t3*diffvds_Vsourceb_sourcep));
#endif
t1=(t3*diffvds);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((idsa_Vgatep_sourcep*vdseff)+(idsa*vdseff_Vgatep_sourcep));
t4_Vdrainp_sourcep=((idsa_Vdrainp_sourcep*vdseff)+(idsa*vdseff_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((idsa_Vdrainb_sourcep*vdseff)+(idsa*vdseff_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((idsa_Vbulkp_sourcep*vdseff)+(idsa*vdseff_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((idsa_Vsourceb_sourcep*vdseff)+(idsa*vdseff_Vsourceb_sourcep));
#endif
t4=(idsa*vdseff);
#if defined(_DERIVATE)
isub_Vgatep_sourcep=((t1_Vgatep_sourcep*t4)+(t1*t4_Vgatep_sourcep));
isub_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t4)+(t1*t4_Vdrainp_sourcep));
isub_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t4)+(t1*t4_Vdrainb_sourcep));
isub_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t4)+(t1*t4_Vbulkp_sourcep));
isub_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t4)+(t1*t4_Vsourceb_sourcep));
#endif
isub=(t1*t4);
}
#if defined(_DERIVATE)
csub_Vgatep_sourcep=isub_Vgatep_sourcep;
csub_Vdrainp_sourcep=isub_Vdrainp_sourcep;
csub_Vdrainb_sourcep=isub_Vdrainb_sourcep;
csub_Vbulkp_sourcep=isub_Vbulkp_sourcep;
csub_Vsourceb_sourcep=isub_Vsourceb_sourcep;
#endif
csub=isub;
#if defined(_DERIVATE)
t9_Vgatep_sourcep=((diffvds_Vgatep_sourcep*vascbe-diffvds*vascbe_Vgatep_sourcep)/vascbe/vascbe);
t9_Vdrainp_sourcep=((diffvds_Vdrainp_sourcep*vascbe-diffvds*vascbe_Vdrainp_sourcep)/vascbe/vascbe);
t9_Vdrainb_sourcep=((diffvds_Vdrainb_sourcep*vascbe-diffvds*vascbe_Vdrainb_sourcep)/vascbe/vascbe);
t9_Vbulkp_sourcep=((diffvds_Vbulkp_sourcep*vascbe-diffvds*vascbe_Vbulkp_sourcep)/vascbe/vascbe);
t9_Vsourceb_sourcep=((diffvds_Vsourceb_sourcep*vascbe-diffvds*vascbe_Vsourceb_sourcep)/vascbe/vascbe);
#endif
t9=(diffvds/vascbe);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=t9_Vgatep_sourcep;
t0_Vdrainp_sourcep=t9_Vdrainp_sourcep;
t0_Vdrainb_sourcep=t9_Vdrainb_sourcep;
t0_Vbulkp_sourcep=t9_Vbulkp_sourcep;
t0_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
t0=(1.0+t9);
#if defined(_DERIVATE)
ids_Vgatep_sourcep=((idsa_Vgatep_sourcep*t0)+(idsa*t0_Vgatep_sourcep));
ids_Vdrainp_sourcep=((idsa_Vdrainp_sourcep*t0)+(idsa*t0_Vdrainp_sourcep));
ids_Vdrainb_sourcep=((idsa_Vdrainb_sourcep*t0)+(idsa*t0_Vdrainb_sourcep));
ids_Vbulkp_sourcep=((idsa_Vbulkp_sourcep*t0)+(idsa*t0_Vbulkp_sourcep));
ids_Vsourceb_sourcep=((idsa_Vsourceb_sourcep*t0)+(idsa*t0_Vsourceb_sourcep));
#endif
ids=(idsa*t0);
#if defined(_DERIVATE)
cdrain_Vgatep_sourcep=((ids_Vgatep_sourcep*vdseff)+(ids*vdseff_Vgatep_sourcep));
cdrain_Vdrainp_sourcep=((ids_Vdrainp_sourcep*vdseff)+(ids*vdseff_Vdrainp_sourcep));
cdrain_Vdrainb_sourcep=((ids_Vdrainb_sourcep*vdseff)+(ids*vdseff_Vdrainb_sourcep));
cdrain_Vbulkp_sourcep=((ids_Vbulkp_sourcep*vdseff)+(ids*vdseff_Vbulkp_sourcep));
cdrain_Vsourceb_sourcep=((ids_Vsourceb_sourcep*vdseff)+(ids*vdseff_Vsourceb_sourcep));
#endif
cdrain=(ids*vdseff);
if
(((vtl!=(-99.0))&&(vtl>0.0)))
{
#if defined(_DERIVATE)
t12_Vgatep_sourcep=(-(1.0/leff)*coxeffwovl_Vgatep_sourcep/coxeffwovl/coxeffwovl);
t12_Vdrainp_sourcep=(-(1.0/leff)*coxeffwovl_Vdrainp_sourcep/coxeffwovl/coxeffwovl);
t12_Vdrainb_sourcep=(-(1.0/leff)*coxeffwovl_Vdrainb_sourcep/coxeffwovl/coxeffwovl);
t12_Vbulkp_sourcep=(-(1.0/leff)*coxeffwovl_Vbulkp_sourcep/coxeffwovl/coxeffwovl);
t12_Vsourceb_sourcep=(-(1.0/leff)*coxeffwovl_Vsourceb_sourcep/coxeffwovl/coxeffwovl);
#endif
t12=((1.0/leff)/coxeffwovl);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=((t12_Vgatep_sourcep*vgsteff-t12*vgsteff_Vgatep_sourcep)/vgsteff/vgsteff);
t11_Vdrainp_sourcep=((t12_Vdrainp_sourcep*vgsteff-t12*vgsteff_Vdrainp_sourcep)/vgsteff/vgsteff);
t11_Vdrainb_sourcep=((t12_Vdrainb_sourcep*vgsteff-t12*vgsteff_Vdrainb_sourcep)/vgsteff/vgsteff);
t11_Vbulkp_sourcep=((t12_Vbulkp_sourcep*vgsteff-t12*vgsteff_Vbulkp_sourcep)/vgsteff/vgsteff);
t11_Vsourceb_sourcep=((t12_Vsourceb_sourcep*vgsteff-t12*vgsteff_Vsourceb_sourcep)/vgsteff/vgsteff);
#endif
t11=(t12/vgsteff);
#if defined(_DERIVATE)
t10_Vgatep_sourcep=(((-t11_Vgatep_sourcep)*vgsteff-(-t11)*vgsteff_Vgatep_sourcep)/vgsteff/vgsteff);
t10_Vdrainp_sourcep=(((-t11_Vdrainp_sourcep)*vgsteff-(-t11)*vgsteff_Vdrainp_sourcep)/vgsteff/vgsteff);
t10_Vdrainb_sourcep=(((-t11_Vdrainb_sourcep)*vgsteff-(-t11)*vgsteff_Vdrainb_sourcep)/vgsteff/vgsteff);
t10_Vbulkp_sourcep=(((-t11_Vbulkp_sourcep)*vgsteff-(-t11)*vgsteff_Vbulkp_sourcep)/vgsteff/vgsteff);
t10_Vsourceb_sourcep=(((-t11_Vsourceb_sourcep)*vgsteff-(-t11)*vgsteff_Vsourceb_sourcep)/vgsteff/vgsteff);
#endif
t10=((-t11)/vgsteff);
#if defined(_DERIVATE)
vs_Vgatep_sourcep=((cdrain_Vgatep_sourcep*t11)+(cdrain*t11_Vgatep_sourcep));
vs_Vdrainp_sourcep=((cdrain_Vdrainp_sourcep*t11)+(cdrain*t11_Vdrainp_sourcep));
vs_Vdrainb_sourcep=((cdrain_Vdrainb_sourcep*t11)+(cdrain*t11_Vdrainb_sourcep));
vs_Vbulkp_sourcep=((cdrain_Vbulkp_sourcep*t11)+(cdrain*t11_Vbulkp_sourcep));
vs_Vsourceb_sourcep=((cdrain_Vsourceb_sourcep*t11)+(cdrain*t11_Vsourceb_sourcep));
#endif
vs=(cdrain*t11);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(2*3);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vs_Vgatep_sourcep/(vtl*tfactor));
t1_Vdrainp_sourcep=(vs_Vdrainp_sourcep/(vtl*tfactor));
t1_Vdrainb_sourcep=(vs_Vdrainb_sourcep/(vtl*tfactor));
t1_Vbulkp_sourcep=(vs_Vbulkp_sourcep/(vtl*tfactor));
t1_Vsourceb_sourcep=(vs_Vsourceb_sourcep/(vtl*tfactor));
#endif
t1=(vs/(vtl*tfactor));
if
((t1<=0))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=1.0;
}
else
{
{
double m00_logE(d00_logE0,t1)
double m00_exp(d00_exp1,(t0*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,t1)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t0_Vgatep_sourcep*d00_logE0)+(t0*t1_Vgatep_sourcep*d10_logE0))*d10_exp1;
t2_Vdrainp_sourcep=((t0_Vdrainp_sourcep*d00_logE0)+(t0*t1_Vdrainp_sourcep*d10_logE0))*d10_exp1;
t2_Vdrainb_sourcep=((t0_Vdrainb_sourcep*d00_logE0)+(t0*t1_Vdrainb_sourcep*d10_logE0))*d10_exp1;
t2_Vbulkp_sourcep=((t0_Vbulkp_sourcep*d00_logE0)+(t0*t1_Vbulkp_sourcep*d10_logE0))*d10_exp1;
t2_Vsourceb_sourcep=((t0_Vsourceb_sourcep*d00_logE0)+(t0*t1_Vsourceb_sourcep*d10_logE0))*d10_exp1;
#endif
t2=(1.0+d00_exp1);
}
}
{
double m00_logE(d00_logE0,t2)
double m00_exp(d00_exp1,(d00_logE0/t0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,t2)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
fsevl_Vgatep_sourcep=(-((t2_Vgatep_sourcep*d10_logE0*t0-d00_logE0*t0_Vgatep_sourcep)/t0/t0)*d10_exp1/d00_exp1/d00_exp1);
fsevl_Vdrainp_sourcep=(-((t2_Vdrainp_sourcep*d10_logE0*t0-d00_logE0*t0_Vdrainp_sourcep)/t0/t0)*d10_exp1/d00_exp1/d00_exp1);
fsevl_Vdrainb_sourcep=(-((t2_Vdrainb_sourcep*d10_logE0*t0-d00_logE0*t0_Vdrainb_sourcep)/t0/t0)*d10_exp1/d00_exp1/d00_exp1);
fsevl_Vbulkp_sourcep=(-((t2_Vbulkp_sourcep*d10_logE0*t0-d00_logE0*t0_Vbulkp_sourcep)/t0/t0)*d10_exp1/d00_exp1/d00_exp1);
fsevl_Vsourceb_sourcep=(-((t2_Vsourceb_sourcep*d10_logE0*t0-d00_logE0*t0_Vsourceb_sourcep)/t0/t0)*d10_exp1/d00_exp1/d00_exp1);
#endif
fsevl=(1.0/d00_exp1);
}
#if defined(_DERIVATE)
cdrain_Vgatep_sourcep=((cdrain_Vgatep_sourcep*fsevl)+(cdrain*fsevl_Vgatep_sourcep));
cdrain_Vdrainp_sourcep=((cdrain_Vdrainp_sourcep*fsevl)+(cdrain*fsevl_Vdrainp_sourcep));
cdrain_Vdrainb_sourcep=((cdrain_Vdrainb_sourcep*fsevl)+(cdrain*fsevl_Vdrainb_sourcep));
cdrain_Vbulkp_sourcep=((cdrain_Vbulkp_sourcep*fsevl)+(cdrain*fsevl_Vbulkp_sourcep));
cdrain_Vsourceb_sourcep=((cdrain_Vsourceb_sourcep*fsevl)+(cdrain*fsevl_Vsourceb_sourcep));
#endif
cdrain=(cdrain*fsevl);
}
if
((rgatemod>1))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(xrcrg2*vtm);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((t9_Vgatep_sourcep*beta)+(t9*beta_Vgatep_sourcep));
t0_Vdrainp_sourcep=((t9_Vdrainp_sourcep*beta)+(t9*beta_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((t9_Vdrainb_sourcep*beta)+(t9*beta_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((t9_Vbulkp_sourcep*beta)+(t9*beta_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((t9_Vsourceb_sourcep*beta)+(t9*beta_Vsourceb_sourcep));
#endif
t0=(t9*beta);
#if defined(_DERIVATE)
gcrg_Vgatep_sourcep=(xrcrg1*(t0_Vgatep_sourcep+ids_Vgatep_sourcep));
gcrg_Vdrainp_sourcep=(xrcrg1*(t0_Vdrainp_sourcep+ids_Vdrainp_sourcep));
gcrg_Vdrainb_sourcep=(xrcrg1*(t0_Vdrainb_sourcep+ids_Vdrainb_sourcep));
gcrg_Vbulkp_sourcep=(xrcrg1*(t0_Vbulkp_sourcep+ids_Vbulkp_sourcep));
gcrg_Vsourceb_sourcep=(xrcrg1*(t0_Vsourceb_sourcep+ids_Vsourceb_sourcep));
#endif
gcrg=(xrcrg1*(t0+ids));
if
((nf!=1.0))
{
#if defined(_DERIVATE)
gcrg_Vgatep_sourcep=gcrg_Vgatep_sourcep*nf;
gcrg_Vdrainp_sourcep=gcrg_Vdrainp_sourcep*nf;
gcrg_Vdrainb_sourcep=gcrg_Vdrainb_sourcep*nf;
gcrg_Vbulkp_sourcep=gcrg_Vbulkp_sourcep*nf;
gcrg_Vsourceb_sourcep=gcrg_Vsourceb_sourcep*nf;
#endif
gcrg=(gcrg*nf);
}
if
((rgatemod==2))
{
#if defined(_DERIVATE)
t10_Vgatep_sourcep=0.0;
t10_Vdrainp_sourcep=0.0;
t10_Vdrainb_sourcep=0.0;
t10_Vbulkp_sourcep=0.0;
t10_Vsourceb_sourcep=0.0;
#endif
t10=(grgeltd*grgeltd);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=gcrg_Vgatep_sourcep;
t11_Vdrainp_sourcep=gcrg_Vdrainp_sourcep;
t11_Vdrainb_sourcep=gcrg_Vdrainb_sourcep;
t11_Vbulkp_sourcep=gcrg_Vbulkp_sourcep;
t11_Vsourceb_sourcep=gcrg_Vsourceb_sourcep;
#endif
t11=(grgeltd+gcrg);
#if defined(_DERIVATE)
gcrg_Vgatep_sourcep=(((grgeltd*gcrg_Vgatep_sourcep)*t11-(grgeltd*gcrg)*t11_Vgatep_sourcep)/t11/t11);
gcrg_Vdrainp_sourcep=(((grgeltd*gcrg_Vdrainp_sourcep)*t11-(grgeltd*gcrg)*t11_Vdrainp_sourcep)/t11/t11);
gcrg_Vdrainb_sourcep=(((grgeltd*gcrg_Vdrainb_sourcep)*t11-(grgeltd*gcrg)*t11_Vdrainb_sourcep)/t11/t11);
gcrg_Vbulkp_sourcep=(((grgeltd*gcrg_Vbulkp_sourcep)*t11-(grgeltd*gcrg)*t11_Vbulkp_sourcep)/t11/t11);
gcrg_Vsourceb_sourcep=(((grgeltd*gcrg_Vsourceb_sourcep)*t11-(grgeltd*gcrg)*t11_Vsourceb_sourcep)/t11/t11);
#endif
gcrg=((grgeltd*gcrg)/t11);
}
}
if
((rdsmod==1.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgs_Vgatep_sourcep;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgs-vfbsd);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+1.0e-4))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+1.0e-4))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
vgs_eff_Vgatep_sourcep=(0.5*(t0_Vgatep_sourcep+t1_Vgatep_sourcep));
vgs_eff_Vdrainp_sourcep=(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep));
vgs_eff_Vdrainb_sourcep=(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep));
vgs_eff_Vbulkp_sourcep=(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep));
vgs_eff_Vsourceb_sourcep=(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep));
#endif
vgs_eff=(0.5*(t0+t1));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(prwg*vgs_eff_Vgatep_sourcep);
t0_Vdrainp_sourcep=(prwg*vgs_eff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(prwg*vgs_eff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(prwg*vgs_eff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(prwg*vgs_eff_Vsourceb_sourcep);
#endif
t0=(1.0+(prwg*vgs_eff));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=((-prwb)*vbs_Vbulkp_sourcep);
t1_Vsourceb_sourcep=0.0;
#endif
t1=((-prwb)*vbs);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-t0_Vgatep_sourcep/t0/t0)+t1_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-t0_Vdrainp_sourcep/t0/t0)+t1_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-t0_Vdrainb_sourcep/t0/t0)+t1_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-t0_Vbulkp_sourcep/t0/t0)+t1_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-t0_Vsourceb_sourcep/t0/t0)+t1_Vsourceb_sourcep);
#endif
t2=((1.0/t0)+t1);
{
double m00_sqrt(d00_sqrt0,((t2*t2)+0.01))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t2*t2)+0.01))
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(t2_Vgatep_sourcep+((t2_Vgatep_sourcep*t2)+(t2*t2_Vgatep_sourcep))*d10_sqrt0);
t3_Vdrainp_sourcep=(t2_Vdrainp_sourcep+((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep))*d10_sqrt0);
t3_Vdrainb_sourcep=(t2_Vdrainb_sourcep+((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep))*d10_sqrt0);
t3_Vbulkp_sourcep=(t2_Vbulkp_sourcep+((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep))*d10_sqrt0);
t3_Vsourceb_sourcep=(t2_Vsourceb_sourcep+((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep))*d10_sqrt0);
#endif
t3=(t2+d00_sqrt0);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(rs0*0.5);
#if defined(_DERIVATE)
rs_Vgatep_sourcep=((t3_Vgatep_sourcep*t4)+(t3*t4_Vgatep_sourcep));
rs_Vdrainp_sourcep=((t3_Vdrainp_sourcep*t4)+(t3*t4_Vdrainp_sourcep));
rs_Vdrainb_sourcep=((t3_Vdrainb_sourcep*t4)+(t3*t4_Vdrainb_sourcep));
rs_Vbulkp_sourcep=((t3_Vbulkp_sourcep*t4)+(t3*t4_Vbulkp_sourcep));
rs_Vsourceb_sourcep=((t3_Vsourceb_sourcep*t4)+(t3*t4_Vsourceb_sourcep));
#endif
rs=(rswmin+(t3*t4));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(gsdiff*rs_Vgatep_sourcep);
t0_Vdrainp_sourcep=(gsdiff*rs_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(gsdiff*rs_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(gsdiff*rs_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(gsdiff*rs_Vsourceb_sourcep);
#endif
t0=(1.0+(gsdiff*rs));
#if defined(_DERIVATE)
gstot_Vgatep_sourcep=(-gsdiff*t0_Vgatep_sourcep/t0/t0);
gstot_Vdrainp_sourcep=(-gsdiff*t0_Vdrainp_sourcep/t0/t0);
gstot_Vdrainb_sourcep=(-gsdiff*t0_Vdrainb_sourcep/t0/t0);
gstot_Vbulkp_sourcep=(-gsdiff*t0_Vbulkp_sourcep/t0/t0);
gstot_Vsourceb_sourcep=(-gsdiff*t0_Vsourceb_sourcep/t0/t0);
#endif
gstot=(gsdiff/t0);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgd_Vgatep_sourcep;
t0_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgd-vfbsd);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+1.0e-4))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+1.0e-4))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
vgd_eff_Vgatep_sourcep=(0.5*(t0_Vgatep_sourcep+t1_Vgatep_sourcep));
vgd_eff_Vdrainp_sourcep=(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep));
vgd_eff_Vdrainb_sourcep=(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep));
vgd_eff_Vbulkp_sourcep=(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep));
vgd_eff_Vsourceb_sourcep=(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep));
#endif
vgd_eff=(0.5*(t0+t1));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(prwg*vgd_eff_Vgatep_sourcep);
t0_Vdrainp_sourcep=(prwg*vgd_eff_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(prwg*vgd_eff_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(prwg*vgd_eff_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(prwg*vgd_eff_Vsourceb_sourcep);
#endif
t0=(1.0+(prwg*vgd_eff));
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=((-prwb)*vbd_Vdrainp_sourcep);
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=((-prwb)*vbd_Vbulkp_sourcep);
t1_Vsourceb_sourcep=0.0;
#endif
t1=((-prwb)*vbd);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-t0_Vgatep_sourcep/t0/t0)+t1_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-t0_Vdrainp_sourcep/t0/t0)+t1_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-t0_Vdrainb_sourcep/t0/t0)+t1_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-t0_Vbulkp_sourcep/t0/t0)+t1_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-t0_Vsourceb_sourcep/t0/t0)+t1_Vsourceb_sourcep);
#endif
t2=((1.0/t0)+t1);
{
double m00_sqrt(d00_sqrt0,((t2*t2)+0.01))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t2*t2)+0.01))
#endif
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(t2_Vgatep_sourcep+((t2_Vgatep_sourcep*t2)+(t2*t2_Vgatep_sourcep))*d10_sqrt0);
t3_Vdrainp_sourcep=(t2_Vdrainp_sourcep+((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep))*d10_sqrt0);
t3_Vdrainb_sourcep=(t2_Vdrainb_sourcep+((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep))*d10_sqrt0);
t3_Vbulkp_sourcep=(t2_Vbulkp_sourcep+((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep))*d10_sqrt0);
t3_Vsourceb_sourcep=(t2_Vsourceb_sourcep+((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep))*d10_sqrt0);
#endif
t3=(t2+d00_sqrt0);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(rd0*0.5);
#if defined(_DERIVATE)
rd_Vgatep_sourcep=((t3_Vgatep_sourcep*t4)+(t3*t4_Vgatep_sourcep));
rd_Vdrainp_sourcep=((t3_Vdrainp_sourcep*t4)+(t3*t4_Vdrainp_sourcep));
rd_Vdrainb_sourcep=((t3_Vdrainb_sourcep*t4)+(t3*t4_Vdrainb_sourcep));
rd_Vbulkp_sourcep=((t3_Vbulkp_sourcep*t4)+(t3*t4_Vbulkp_sourcep));
rd_Vsourceb_sourcep=((t3_Vsourceb_sourcep*t4)+(t3*t4_Vsourceb_sourcep));
#endif
rd=(rdwmin+(t3*t4));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(gddiff*rd_Vgatep_sourcep);
t0_Vdrainp_sourcep=(gddiff*rd_Vdrainp_sourcep);
t0_Vdrainb_sourcep=(gddiff*rd_Vdrainb_sourcep);
t0_Vbulkp_sourcep=(gddiff*rd_Vbulkp_sourcep);
t0_Vsourceb_sourcep=(gddiff*rd_Vsourceb_sourcep);
#endif
t0=(1.0+(gddiff*rd));
#if defined(_DERIVATE)
gdtot_Vgatep_sourcep=(-gddiff*t0_Vgatep_sourcep/t0/t0);
gdtot_Vdrainp_sourcep=(-gddiff*t0_Vdrainp_sourcep/t0/t0);
gdtot_Vdrainb_sourcep=(-gddiff*t0_Vdrainb_sourcep/t0/t0);
gdtot_Vbulkp_sourcep=(-gddiff*t0_Vbulkp_sourcep/t0/t0);
gdtot_Vsourceb_sourcep=(-gddiff*t0_Vsourceb_sourcep/t0/t0);
#endif
gdtot=(gddiff/t0);
}
else
{
#if defined(_DERIVATE)
gstot_Vgatep_sourcep=0.0;
gstot_Vdrainp_sourcep=0.0;
gstot_Vdrainb_sourcep=0.0;
gstot_Vbulkp_sourcep=0.0;
gstot_Vsourceb_sourcep=0.0;
#endif
gstot=0.0;
#if defined(_DERIVATE)
gdtot_Vgatep_sourcep=0.0;
gdtot_Vdrainp_sourcep=0.0;
gdtot_Vdrainb_sourcep=0.0;
gdtot_Vbulkp_sourcep=0.0;
gdtot_Vsourceb_sourcep=0.0;
#endif
gdtot=0.0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(3.0*toxe);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((-vgs_eff_Vgatep_sourcep)*t0-((vds-vgs_eff)-egidl)*t0_Vgatep_sourcep)/t0/t0);
t1_Vdrainp_sourcep=(((vds_Vdrainp_sourcep-vgs_eff_Vdrainp_sourcep)*t0-((vds-vgs_eff)-egidl)*t0_Vdrainp_sourcep)/t0/t0);
t1_Vdrainb_sourcep=(((-vgs_eff_Vdrainb_sourcep)*t0-((vds-vgs_eff)-egidl)*t0_Vdrainb_sourcep)/t0/t0);
t1_Vbulkp_sourcep=(((-vgs_eff_Vbulkp_sourcep)*t0-((vds-vgs_eff)-egidl)*t0_Vbulkp_sourcep)/t0/t0);
t1_Vsourceb_sourcep=(((-vgs_eff_Vsourceb_sourcep)*t0-((vds-vgs_eff)-egidl)*t0_Vsourceb_sourcep)/t0/t0);
#endif
t1=(((vds-vgs_eff)-egidl)/t0);
if
((((((agidl<=0.0)||(bgidl<=0.0))||(t1<=0.0))||(cgidl<=0.0))||(vbd>0.0)))
{
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=0.0;
igidl_Vdrainp_sourcep=0.0;
igidl_Vdrainb_sourcep=0.0;
igidl_Vbulkp_sourcep=0.0;
igidl_Vsourceb_sourcep=0.0;
#endif
igidl=0.0;
}
else
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-bgidl*t1_Vgatep_sourcep/t1/t1);
t2_Vdrainp_sourcep=(-bgidl*t1_Vdrainp_sourcep/t1/t1);
t2_Vdrainb_sourcep=(-bgidl*t1_Vdrainb_sourcep/t1/t1);
t2_Vbulkp_sourcep=(-bgidl*t1_Vbulkp_sourcep/t1/t1);
t2_Vsourceb_sourcep=(-bgidl*t1_Vsourceb_sourcep/t1/t1);
#endif
t2=(bgidl/t1);
if
((t2<100.0))
{
{
double m00_exp(d00_exp0,(-t2))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=((((agidl*weffcj)*t1_Vgatep_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vgatep_sourcep)*d10_exp0));
igidl_Vdrainp_sourcep=((((agidl*weffcj)*t1_Vdrainp_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vdrainp_sourcep)*d10_exp0));
igidl_Vdrainb_sourcep=((((agidl*weffcj)*t1_Vdrainb_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vdrainb_sourcep)*d10_exp0));
igidl_Vbulkp_sourcep=((((agidl*weffcj)*t1_Vbulkp_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vbulkp_sourcep)*d10_exp0));
igidl_Vsourceb_sourcep=((((agidl*weffcj)*t1_Vsourceb_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vsourceb_sourcep)*d10_exp0));
#endif
igidl=(((agidl*weffcj)*t1)*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=0.0;
igidl_Vdrainp_sourcep=0.0;
igidl_Vdrainb_sourcep=0.0;
igidl_Vbulkp_sourcep=0.0;
igidl_Vsourceb_sourcep=0.0;
#endif
igidl=((agidl*weffcj)*3.720075976e-44);
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=((igidl_Vgatep_sourcep*t1)+(igidl*t1_Vgatep_sourcep));
igidl_Vdrainp_sourcep=((igidl_Vdrainp_sourcep*t1)+(igidl*t1_Vdrainp_sourcep));
igidl_Vdrainb_sourcep=((igidl_Vdrainb_sourcep*t1)+(igidl*t1_Vdrainb_sourcep));
igidl_Vbulkp_sourcep=((igidl_Vbulkp_sourcep*t1)+(igidl*t1_Vbulkp_sourcep));
igidl_Vsourceb_sourcep=((igidl_Vsourceb_sourcep*t1)+(igidl*t1_Vsourceb_sourcep));
#endif
igidl=(igidl*t1);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=((vbd_Vdrainp_sourcep*vbd)+(vbd*vbd_Vdrainp_sourcep));
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=((vbd_Vbulkp_sourcep*vbd)+(vbd*vbd_Vbulkp_sourcep));
t4_Vsourceb_sourcep=0.0;
#endif
t4=(vbd*vbd);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((-vbd)*t4_Vgatep_sourcep);
t5_Vdrainp_sourcep=(((-vbd_Vdrainp_sourcep)*t4)+((-vbd)*t4_Vdrainp_sourcep));
t5_Vdrainb_sourcep=((-vbd)*t4_Vdrainb_sourcep);
t5_Vbulkp_sourcep=(((-vbd_Vbulkp_sourcep)*t4)+((-vbd)*t4_Vbulkp_sourcep));
t5_Vsourceb_sourcep=((-vbd)*t4_Vsourceb_sourcep);
#endif
t5=((-vbd)*t4);
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep;
#endif
t6=(cgidl+t5);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((t5_Vgatep_sourcep*t6-t5*t6_Vgatep_sourcep)/t6/t6);
t7_Vdrainp_sourcep=((t5_Vdrainp_sourcep*t6-t5*t6_Vdrainp_sourcep)/t6/t6);
t7_Vdrainb_sourcep=((t5_Vdrainb_sourcep*t6-t5*t6_Vdrainb_sourcep)/t6/t6);
t7_Vbulkp_sourcep=((t5_Vbulkp_sourcep*t6-t5*t6_Vbulkp_sourcep)/t6/t6);
t7_Vsourceb_sourcep=((t5_Vsourceb_sourcep*t6-t5*t6_Vsourceb_sourcep)/t6/t6);
#endif
t7=(t5/t6);
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((((((3.0*cgidl)*t4_Vgatep_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vgatep_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vgatep_sourcep)/t6/t6);
t8_Vdrainp_sourcep=((((((3.0*cgidl)*t4_Vdrainp_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vdrainp_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vdrainp_sourcep)/t6/t6);
t8_Vdrainb_sourcep=((((((3.0*cgidl)*t4_Vdrainb_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vdrainb_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vdrainb_sourcep)/t6/t6);
t8_Vbulkp_sourcep=((((((3.0*cgidl)*t4_Vbulkp_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vbulkp_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vbulkp_sourcep)/t6/t6);
t8_Vsourceb_sourcep=((((((3.0*cgidl)*t4_Vsourceb_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vsourceb_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vsourceb_sourcep)/t6/t6);
#endif
t8=((((3.0*cgidl)*t4)/t6)/t6);
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=((igidl_Vgatep_sourcep*t7)+(igidl*t7_Vgatep_sourcep));
igidl_Vdrainp_sourcep=((igidl_Vdrainp_sourcep*t7)+(igidl*t7_Vdrainp_sourcep));
igidl_Vdrainb_sourcep=((igidl_Vdrainb_sourcep*t7)+(igidl*t7_Vdrainb_sourcep));
igidl_Vbulkp_sourcep=((igidl_Vbulkp_sourcep*t7)+(igidl*t7_Vbulkp_sourcep));
igidl_Vsourceb_sourcep=((igidl_Vsourceb_sourcep*t7)+(igidl*t7_Vsourceb_sourcep));
#endif
igidl=(igidl*t7);
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((-vgd_eff_Vgatep_sourcep)*t0-(((-vds)-vgd_eff)-egidl)*t0_Vgatep_sourcep)/t0/t0);
t1_Vdrainp_sourcep=((((-vds_Vdrainp_sourcep)-vgd_eff_Vdrainp_sourcep)*t0-(((-vds)-vgd_eff)-egidl)*t0_Vdrainp_sourcep)/t0/t0);
t1_Vdrainb_sourcep=(((-vgd_eff_Vdrainb_sourcep)*t0-(((-vds)-vgd_eff)-egidl)*t0_Vdrainb_sourcep)/t0/t0);
t1_Vbulkp_sourcep=(((-vgd_eff_Vbulkp_sourcep)*t0-(((-vds)-vgd_eff)-egidl)*t0_Vbulkp_sourcep)/t0/t0);
t1_Vsourceb_sourcep=(((-vgd_eff_Vsourceb_sourcep)*t0-(((-vds)-vgd_eff)-egidl)*t0_Vsourceb_sourcep)/t0/t0);
#endif
t1=((((-vds)-vgd_eff)-egidl)/t0);
if
((((((agidl<=0.0)||(bgidl<=0.0))||(t1<=0.0))||(cgidl<=0.0))||(vbs>0.0)))
{
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=0.0;
igisl_Vdrainp_sourcep=0.0;
igisl_Vdrainb_sourcep=0.0;
igisl_Vbulkp_sourcep=0.0;
igisl_Vsourceb_sourcep=0.0;
#endif
igisl=0.0;
}
else
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-bgidl*t1_Vgatep_sourcep/t1/t1);
t2_Vdrainp_sourcep=(-bgidl*t1_Vdrainp_sourcep/t1/t1);
t2_Vdrainb_sourcep=(-bgidl*t1_Vdrainb_sourcep/t1/t1);
t2_Vbulkp_sourcep=(-bgidl*t1_Vbulkp_sourcep/t1/t1);
t2_Vsourceb_sourcep=(-bgidl*t1_Vsourceb_sourcep/t1/t1);
#endif
t2=(bgidl/t1);
if
((t2<100.0))
{
{
double m00_exp(d00_exp0,(-t2))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=((((agidl*weffcj)*t1_Vgatep_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vgatep_sourcep)*d10_exp0));
igisl_Vdrainp_sourcep=((((agidl*weffcj)*t1_Vdrainp_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vdrainp_sourcep)*d10_exp0));
igisl_Vdrainb_sourcep=((((agidl*weffcj)*t1_Vdrainb_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vdrainb_sourcep)*d10_exp0));
igisl_Vbulkp_sourcep=((((agidl*weffcj)*t1_Vbulkp_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vbulkp_sourcep)*d10_exp0));
igisl_Vsourceb_sourcep=((((agidl*weffcj)*t1_Vsourceb_sourcep)*d00_exp0)+(((agidl*weffcj)*t1)*(-t2_Vsourceb_sourcep)*d10_exp0));
#endif
igisl=(((agidl*weffcj)*t1)*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=0.0;
igisl_Vdrainp_sourcep=0.0;
igisl_Vdrainb_sourcep=0.0;
igisl_Vbulkp_sourcep=0.0;
igisl_Vsourceb_sourcep=0.0;
#endif
igisl=((agidl*weffcj)*3.720075976e-44);
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=((igisl_Vgatep_sourcep*t1)+(igisl*t1_Vgatep_sourcep));
igisl_Vdrainp_sourcep=((igisl_Vdrainp_sourcep*t1)+(igisl*t1_Vdrainp_sourcep));
igisl_Vdrainb_sourcep=((igisl_Vdrainb_sourcep*t1)+(igisl*t1_Vdrainb_sourcep));
igisl_Vbulkp_sourcep=((igisl_Vbulkp_sourcep*t1)+(igisl*t1_Vbulkp_sourcep));
igisl_Vsourceb_sourcep=((igisl_Vsourceb_sourcep*t1)+(igisl*t1_Vsourceb_sourcep));
#endif
igisl=(igisl*t1);
}
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=((vbs_Vbulkp_sourcep*vbs)+(vbs*vbs_Vbulkp_sourcep));
t4_Vsourceb_sourcep=0.0;
#endif
t4=(vbs*vbs);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((-vbs)*t4_Vgatep_sourcep);
t5_Vdrainp_sourcep=((-vbs)*t4_Vdrainp_sourcep);
t5_Vdrainb_sourcep=((-vbs)*t4_Vdrainb_sourcep);
t5_Vbulkp_sourcep=(((-vbs_Vbulkp_sourcep)*t4)+((-vbs)*t4_Vbulkp_sourcep));
t5_Vsourceb_sourcep=((-vbs)*t4_Vsourceb_sourcep);
#endif
t5=((-vbs)*t4);
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep;
#endif
t6=(cgidl+t5);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((t5_Vgatep_sourcep*t6-t5*t6_Vgatep_sourcep)/t6/t6);
t7_Vdrainp_sourcep=((t5_Vdrainp_sourcep*t6-t5*t6_Vdrainp_sourcep)/t6/t6);
t7_Vdrainb_sourcep=((t5_Vdrainb_sourcep*t6-t5*t6_Vdrainb_sourcep)/t6/t6);
t7_Vbulkp_sourcep=((t5_Vbulkp_sourcep*t6-t5*t6_Vbulkp_sourcep)/t6/t6);
t7_Vsourceb_sourcep=((t5_Vsourceb_sourcep*t6-t5*t6_Vsourceb_sourcep)/t6/t6);
#endif
t7=(t5/t6);
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((((((3.0*cgidl)*t4_Vgatep_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vgatep_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vgatep_sourcep)/t6/t6);
t8_Vdrainp_sourcep=((((((3.0*cgidl)*t4_Vdrainp_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vdrainp_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vdrainp_sourcep)/t6/t6);
t8_Vdrainb_sourcep=((((((3.0*cgidl)*t4_Vdrainb_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vdrainb_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vdrainb_sourcep)/t6/t6);
t8_Vbulkp_sourcep=((((((3.0*cgidl)*t4_Vbulkp_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vbulkp_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vbulkp_sourcep)/t6/t6);
t8_Vsourceb_sourcep=((((((3.0*cgidl)*t4_Vsourceb_sourcep)*t6-((3.0*cgidl)*t4)*t6_Vsourceb_sourcep)/t6/t6)*t6-(((3.0*cgidl)*t4)/t6)*t6_Vsourceb_sourcep)/t6/t6);
#endif
t8=((((3.0*cgidl)*t4)/t6)/t6);
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=((igisl_Vgatep_sourcep*t7)+(igisl*t7_Vgatep_sourcep));
igisl_Vdrainp_sourcep=((igisl_Vdrainp_sourcep*t7)+(igisl*t7_Vdrainp_sourcep));
igisl_Vdrainb_sourcep=((igisl_Vdrainb_sourcep*t7)+(igisl*t7_Vdrainb_sourcep));
igisl_Vbulkp_sourcep=((igisl_Vbulkp_sourcep*t7)+(igisl*t7_Vbulkp_sourcep));
igisl_Vsourceb_sourcep=((igisl_Vsourceb_sourcep*t7)+(igisl*t7_Vsourceb_sourcep));
#endif
igisl=(igisl*t7);
}
if
(((igcmod!=0.0)||(igbmod!=0.0)))
{
#if defined(_DERIVATE)
v3_Vgatep_sourcep=(-Vgs_eff_Vgatep_sourcep);
v3_Vdrainp_sourcep=((-Vgs_eff_Vdrainp_sourcep)+vbseff_Vdrainp_sourcep);
v3_Vdrainb_sourcep=((-Vgs_eff_Vdrainb_sourcep)+vbseff_Vdrainb_sourcep);
v3_Vbulkp_sourcep=((-Vgs_eff_Vbulkp_sourcep)+vbseff_Vbulkp_sourcep);
v3_Vsourceb_sourcep=((-Vgs_eff_Vsourceb_sourcep)+vbseff_Vsourceb_sourcep);
#endif
v3=(((vfbzb-Vgs_eff)+vbseff)-0.02);
if
((vfbzb<=0.0))
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
else
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
#if defined(_DERIVATE)
vfbeff_Vgatep_sourcep=(-(0.5*(v3_Vgatep_sourcep+t0_Vgatep_sourcep)));
vfbeff_Vdrainp_sourcep=(-(0.5*(v3_Vdrainp_sourcep+t0_Vdrainp_sourcep)));
vfbeff_Vdrainb_sourcep=(-(0.5*(v3_Vdrainb_sourcep+t0_Vdrainb_sourcep)));
vfbeff_Vbulkp_sourcep=(-(0.5*(v3_Vbulkp_sourcep+t0_Vbulkp_sourcep)));
vfbeff_Vsourceb_sourcep=(-(0.5*(v3_Vsourceb_sourcep+t0_Vsourceb_sourcep)));
#endif
vfbeff=(vfbzb-(0.5*(v3+t0)));
#if defined(_DERIVATE)
voxacc_Vgatep_sourcep=(-vfbeff_Vgatep_sourcep);
voxacc_Vdrainp_sourcep=(-vfbeff_Vdrainp_sourcep);
voxacc_Vdrainb_sourcep=(-vfbeff_Vdrainb_sourcep);
voxacc_Vbulkp_sourcep=(-vfbeff_Vbulkp_sourcep);
voxacc_Vsourceb_sourcep=(-vfbeff_Vsourceb_sourcep);
#endif
voxacc=(vfbzb-vfbeff);
if
((voxacc<0.0))
{
#if defined(_DERIVATE)
voxacc_Vgatep_sourcep=0.0;
voxacc_Vdrainp_sourcep=0.0;
voxacc_Vdrainb_sourcep=0.0;
voxacc_Vbulkp_sourcep=0.0;
voxacc_Vsourceb_sourcep=0.0;
#endif
voxacc=0.0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(0.5*k1ox);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep-vfbeff_Vgatep_sourcep)-vgsteff_Vgatep_sourcep);
t3_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-vfbeff_Vdrainp_sourcep)-vbseff_Vdrainp_sourcep)-vgsteff_Vdrainp_sourcep);
t3_Vdrainb_sourcep=(((Vgs_eff_Vdrainb_sourcep-vfbeff_Vdrainb_sourcep)-vbseff_Vdrainb_sourcep)-vgsteff_Vdrainb_sourcep);
t3_Vbulkp_sourcep=(((Vgs_eff_Vbulkp_sourcep-vfbeff_Vbulkp_sourcep)-vbseff_Vbulkp_sourcep)-vgsteff_Vbulkp_sourcep);
t3_Vsourceb_sourcep=(((Vgs_eff_Vsourceb_sourcep-vfbeff_Vsourceb_sourcep)-vbseff_Vsourceb_sourcep)-vgsteff_Vsourceb_sourcep);
#endif
t3=(((Vgs_eff-vfbeff)-vbseff)-vgsteff);
if
((k1ox==0.0))
{
#if defined(_DERIVATE)
voxdepinv_Vgatep_sourcep=0.0;
voxdepinv_Vdrainp_sourcep=0.0;
voxdepinv_Vdrainb_sourcep=0.0;
voxdepinv_Vbulkp_sourcep=0.0;
voxdepinv_Vsourceb_sourcep=0.0;
#endif
voxdepinv=0.0;
}
else
{
if
((t3<0.0))
{
#if defined(_DERIVATE)
voxdepinv_Vgatep_sourcep=(-t3_Vgatep_sourcep);
voxdepinv_Vdrainp_sourcep=(-t3_Vdrainp_sourcep);
voxdepinv_Vdrainb_sourcep=(-t3_Vdrainb_sourcep);
voxdepinv_Vbulkp_sourcep=(-t3_Vbulkp_sourcep);
voxdepinv_Vsourceb_sourcep=(-t3_Vsourceb_sourcep);
#endif
voxdepinv=(-t3);
}
else
{
{
double m00_sqrt(d00_sqrt0,((t0*t0)+t3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+t3))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))+t3_Vgatep_sourcep)*d10_sqrt0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+t3_Vdrainp_sourcep)*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+t3_Vdrainb_sourcep)*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+t3_Vbulkp_sourcep)*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+t3_Vsourceb_sourcep)*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t0_Vgatep_sourcep*t1-t0*t1_Vgatep_sourcep)/t1/t1);
t2_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t1-t0*t1_Vdrainp_sourcep)/t1/t1);
t2_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t1-t0*t1_Vdrainb_sourcep)/t1/t1);
t2_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t1-t0*t1_Vbulkp_sourcep)/t1/t1);
t2_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t1-t0*t1_Vsourceb_sourcep)/t1/t1);
#endif
t2=(t0/t1);
#if defined(_DERIVATE)
voxdepinv_Vgatep_sourcep=(k1ox*(t1_Vgatep_sourcep-t0_Vgatep_sourcep));
voxdepinv_Vdrainp_sourcep=(k1ox*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep));
voxdepinv_Vdrainb_sourcep=(k1ox*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep));
voxdepinv_Vbulkp_sourcep=(k1ox*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep));
voxdepinv_Vsourceb_sourcep=(k1ox*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep));
#endif
voxdepinv=(k1ox*(t1-t0));
}
}
#if defined(_DERIVATE)
voxdepinv_Vgatep_sourcep=(voxdepinv_Vgatep_sourcep+vgsteff_Vgatep_sourcep);
voxdepinv_Vdrainp_sourcep=(voxdepinv_Vdrainp_sourcep+vgsteff_Vdrainp_sourcep);
voxdepinv_Vdrainb_sourcep=(voxdepinv_Vdrainb_sourcep+vgsteff_Vdrainb_sourcep);
voxdepinv_Vbulkp_sourcep=(voxdepinv_Vbulkp_sourcep+vgsteff_Vbulkp_sourcep);
voxdepinv_Vsourceb_sourcep=(voxdepinv_Vsourceb_sourcep+vgsteff_Vsourceb_sourcep);
#endif
voxdepinv=(voxdepinv+vgsteff);
}
if
((igcmod==1.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vtm*nigc);
#if defined(_DERIVATE)
vxnvt_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep*t0-(Vgs_eff-(type*vth0))*t0_Vgatep_sourcep)/t0/t0);
vxnvt_Vdrainp_sourcep=((Vgs_eff_Vdrainp_sourcep*t0-(Vgs_eff-(type*vth0))*t0_Vdrainp_sourcep)/t0/t0);
vxnvt_Vdrainb_sourcep=((Vgs_eff_Vdrainb_sourcep*t0-(Vgs_eff-(type*vth0))*t0_Vdrainb_sourcep)/t0/t0);
vxnvt_Vbulkp_sourcep=((Vgs_eff_Vbulkp_sourcep*t0-(Vgs_eff-(type*vth0))*t0_Vbulkp_sourcep)/t0/t0);
vxnvt_Vsourceb_sourcep=((Vgs_eff_Vsourceb_sourcep*t0-(Vgs_eff-(type*vth0))*t0_Vsourceb_sourcep)/t0/t0);
#endif
vxnvt=((Vgs_eff-(type*vth0))/t0);
if
((vxnvt>34.0))
{
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=Vgs_eff_Vgatep_sourcep;
vaux_Vdrainp_sourcep=Vgs_eff_Vdrainp_sourcep;
vaux_Vdrainb_sourcep=Vgs_eff_Vdrainb_sourcep;
vaux_Vbulkp_sourcep=Vgs_eff_Vbulkp_sourcep;
vaux_Vsourceb_sourcep=Vgs_eff_Vsourceb_sourcep;
#endif
vaux=(Vgs_eff-(type*vth0));
}
else
{
if
((vxnvt<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=t0_Vgatep_sourcep*d00_logE0;
vaux_Vdrainp_sourcep=t0_Vdrainp_sourcep*d00_logE0;
vaux_Vdrainb_sourcep=t0_Vdrainb_sourcep*d00_logE0;
vaux_Vbulkp_sourcep=t0_Vbulkp_sourcep*d00_logE0;
vaux_Vsourceb_sourcep=t0_Vsourceb_sourcep*d00_logE0;
#endif
vaux=(t0*d00_logE0);
}
}
else
{
{
double m00_exp(d00_exp0,vxnvt)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
expvxnvt_Vgatep_sourcep=vxnvt_Vgatep_sourcep*d10_exp0;
expvxnvt_Vdrainp_sourcep=vxnvt_Vdrainp_sourcep*d10_exp0;
expvxnvt_Vdrainb_sourcep=vxnvt_Vdrainb_sourcep*d10_exp0;
expvxnvt_Vbulkp_sourcep=vxnvt_Vbulkp_sourcep*d10_exp0;
expvxnvt_Vsourceb_sourcep=vxnvt_Vsourceb_sourcep*d10_exp0;
#endif
expvxnvt=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+expvxnvt))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+expvxnvt))
#endif
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=((t0_Vgatep_sourcep*d00_logE0)+(t0*expvxnvt_Vgatep_sourcep*d10_logE0));
vaux_Vdrainp_sourcep=((t0_Vdrainp_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainp_sourcep*d10_logE0));
vaux_Vdrainb_sourcep=((t0_Vdrainb_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainb_sourcep*d10_logE0));
vaux_Vbulkp_sourcep=((t0_Vbulkp_sourcep*d00_logE0)+(t0*expvxnvt_Vbulkp_sourcep*d10_logE0));
vaux_Vsourceb_sourcep=((t0_Vsourceb_sourcep*d00_logE0)+(t0*expvxnvt_Vsourceb_sourcep*d10_logE0));
#endif
vaux=(t0*d00_logE0);
}
}
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep*vaux)+(Vgs_eff*vaux_Vgatep_sourcep));
t2_Vdrainp_sourcep=((Vgs_eff_Vdrainp_sourcep*vaux)+(Vgs_eff*vaux_Vdrainp_sourcep));
t2_Vdrainb_sourcep=((Vgs_eff_Vdrainb_sourcep*vaux)+(Vgs_eff*vaux_Vdrainb_sourcep));
t2_Vbulkp_sourcep=((Vgs_eff_Vbulkp_sourcep*vaux)+(Vgs_eff*vaux_Vbulkp_sourcep));
t2_Vsourceb_sourcep=((Vgs_eff_Vsourceb_sourcep*vaux)+(Vgs_eff*vaux_Vsourceb_sourcep));
#endif
t2=(Vgs_eff*vaux);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=0.0;
t11_Vdrainp_sourcep=0.0;
t11_Vdrainb_sourcep=0.0;
t11_Vbulkp_sourcep=0.0;
t11_Vsourceb_sourcep=0.0;
#endif
t11=aechvb;
#if defined(_DERIVATE)
t12_Vgatep_sourcep=0.0;
t12_Vdrainp_sourcep=0.0;
t12_Vdrainb_sourcep=0.0;
t12_Vbulkp_sourcep=0.0;
t12_Vsourceb_sourcep=0.0;
#endif
t12=bechvb;
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((aigc*cigc)-bigc);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(bigc*cigc);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t12_Vgatep_sourcep*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vgatep_sourcep*voxdepinv)+(t3*voxdepinv_Vgatep_sourcep))-((((t4_Vgatep_sourcep*voxdepinv)+(t4*voxdepinv_Vgatep_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vgatep_sourcep)))));
t5_Vdrainp_sourcep=((t12_Vdrainp_sourcep*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vdrainp_sourcep*voxdepinv)+(t3*voxdepinv_Vdrainp_sourcep))-((((t4_Vdrainp_sourcep*voxdepinv)+(t4*voxdepinv_Vdrainp_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vdrainp_sourcep)))));
t5_Vdrainb_sourcep=((t12_Vdrainb_sourcep*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vdrainb_sourcep*voxdepinv)+(t3*voxdepinv_Vdrainb_sourcep))-((((t4_Vdrainb_sourcep*voxdepinv)+(t4*voxdepinv_Vdrainb_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vdrainb_sourcep)))));
t5_Vbulkp_sourcep=((t12_Vbulkp_sourcep*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vbulkp_sourcep*voxdepinv)+(t3*voxdepinv_Vbulkp_sourcep))-((((t4_Vbulkp_sourcep*voxdepinv)+(t4*voxdepinv_Vbulkp_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vbulkp_sourcep)))));
t5_Vsourceb_sourcep=((t12_Vsourceb_sourcep*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vsourceb_sourcep*voxdepinv)+(t3*voxdepinv_Vsourceb_sourcep))-((((t4_Vsourceb_sourcep*voxdepinv)+(t4*voxdepinv_Vsourceb_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vsourceb_sourcep)))));
#endif
t5=(t12*((aigc+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)));
if
((t5>34.0))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=5.834617425e14;
}
else
{
if
((t5<(-34.0)))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t5)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep*d10_exp0;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep*d10_exp0;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep*d10_exp0;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep*d10_exp0;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep*d10_exp0;
#endif
t6=d00_exp0;
}
}
}
#if defined(_DERIVATE)
igc_Vgatep_sourcep=((((t11_Vgatep_sourcep*t2)+(t11*t2_Vgatep_sourcep))*t6)+((t11*t2)*t6_Vgatep_sourcep));
igc_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t2)+(t11*t2_Vdrainp_sourcep))*t6)+((t11*t2)*t6_Vdrainp_sourcep));
igc_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t2)+(t11*t2_Vdrainb_sourcep))*t6)+((t11*t2)*t6_Vdrainb_sourcep));
igc_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t2)+(t11*t2_Vbulkp_sourcep))*t6)+((t11*t2)*t6_Vbulkp_sourcep));
igc_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t2)+(t11*t2_Vsourceb_sourcep))*t6)+((t11*t2)*t6_Vsourceb_sourcep));
#endif
igc=((t11*t2)*t6);
if
((pigcd!=(-99.0)))
{
#if defined(_DERIVATE)
modif_pigcd_Vgatep_sourcep=0.0;
modif_pigcd_Vdrainp_sourcep=0.0;
modif_pigcd_Vdrainb_sourcep=0.0;
modif_pigcd_Vbulkp_sourcep=0.0;
modif_pigcd_Vsourceb_sourcep=0.0;
#endif
modif_pigcd=pigcd;
}
else
{
#if defined(_DERIVATE)
t11_Vgatep_sourcep=0.0;
t11_Vdrainp_sourcep=0.0;
t11_Vdrainb_sourcep=0.0;
t11_Vbulkp_sourcep=0.0;
t11_Vsourceb_sourcep=0.0;
#endif
t11=(bechvb*toxe);
#if defined(_DERIVATE)
t12_Vgatep_sourcep=vgsteff_Vgatep_sourcep;
t12_Vdrainp_sourcep=vgsteff_Vdrainp_sourcep;
t12_Vdrainb_sourcep=vgsteff_Vdrainb_sourcep;
t12_Vbulkp_sourcep=vgsteff_Vbulkp_sourcep;
t12_Vsourceb_sourcep=vgsteff_Vsourceb_sourcep;
#endif
t12=(vgsteff+1.0e-20);
#if defined(_DERIVATE)
t13_Vgatep_sourcep=((((t11_Vgatep_sourcep*t12-t11*t12_Vgatep_sourcep)/t12/t12)*t12-(t11/t12)*t12_Vgatep_sourcep)/t12/t12);
t13_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t12-t11*t12_Vdrainp_sourcep)/t12/t12)*t12-(t11/t12)*t12_Vdrainp_sourcep)/t12/t12);
t13_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t12-t11*t12_Vdrainb_sourcep)/t12/t12)*t12-(t11/t12)*t12_Vdrainb_sourcep)/t12/t12);
t13_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t12-t11*t12_Vbulkp_sourcep)/t12/t12)*t12-(t11/t12)*t12_Vbulkp_sourcep)/t12/t12);
t13_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t12-t11*t12_Vsourceb_sourcep)/t12/t12)*t12-(t11/t12)*t12_Vsourceb_sourcep)/t12/t12);
#endif
t13=((t11/t12)/t12);
#if defined(_DERIVATE)
modif_pigcd_Vgatep_sourcep=((t13_Vgatep_sourcep*(1.0-((0.5*vdseff)/t12)))+(t13*(-(((0.5*vdseff_Vgatep_sourcep)*t12-(0.5*vdseff)*t12_Vgatep_sourcep)/t12/t12))));
modif_pigcd_Vdrainp_sourcep=((t13_Vdrainp_sourcep*(1.0-((0.5*vdseff)/t12)))+(t13*(-(((0.5*vdseff_Vdrainp_sourcep)*t12-(0.5*vdseff)*t12_Vdrainp_sourcep)/t12/t12))));
modif_pigcd_Vdrainb_sourcep=((t13_Vdrainb_sourcep*(1.0-((0.5*vdseff)/t12)))+(t13*(-(((0.5*vdseff_Vdrainb_sourcep)*t12-(0.5*vdseff)*t12_Vdrainb_sourcep)/t12/t12))));
modif_pigcd_Vbulkp_sourcep=((t13_Vbulkp_sourcep*(1.0-((0.5*vdseff)/t12)))+(t13*(-(((0.5*vdseff_Vbulkp_sourcep)*t12-(0.5*vdseff)*t12_Vbulkp_sourcep)/t12/t12))));
modif_pigcd_Vsourceb_sourcep=((t13_Vsourceb_sourcep*(1.0-((0.5*vdseff)/t12)))+(t13*(-(((0.5*vdseff_Vsourceb_sourcep)*t12-(0.5*vdseff)*t12_Vsourceb_sourcep)/t12/t12))));
#endif
modif_pigcd=(t13*(1.0-((0.5*vdseff)/t12)));
}
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(((-modif_pigcd_Vgatep_sourcep)*vdseff)+((-modif_pigcd)*vdseff_Vgatep_sourcep));
t7_Vdrainp_sourcep=(((-modif_pigcd_Vdrainp_sourcep)*vdseff)+((-modif_pigcd)*vdseff_Vdrainp_sourcep));
t7_Vdrainb_sourcep=(((-modif_pigcd_Vdrainb_sourcep)*vdseff)+((-modif_pigcd)*vdseff_Vdrainb_sourcep));
t7_Vbulkp_sourcep=(((-modif_pigcd_Vbulkp_sourcep)*vdseff)+((-modif_pigcd)*vdseff_Vbulkp_sourcep));
t7_Vsourceb_sourcep=(((-modif_pigcd_Vsourceb_sourcep)*vdseff)+((-modif_pigcd)*vdseff_Vsourceb_sourcep));
#endif
t7=((-modif_pigcd)*vdseff);
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((t7_Vgatep_sourcep*t7)+(t7*t7_Vgatep_sourcep));
t8_Vdrainp_sourcep=((t7_Vdrainp_sourcep*t7)+(t7*t7_Vdrainp_sourcep));
t8_Vdrainb_sourcep=((t7_Vdrainb_sourcep*t7)+(t7*t7_Vdrainb_sourcep));
t8_Vbulkp_sourcep=((t7_Vbulkp_sourcep*t7)+(t7*t7_Vbulkp_sourcep));
t8_Vsourceb_sourcep=((t7_Vsourceb_sourcep*t7)+(t7*t7_Vsourceb_sourcep));
#endif
t8=((t7*t7)+2.0e-4);
if
((t7>34.0))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=5.834617425e14;
}
else
{
if
((t7<(-34.0)))
{
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t7)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t9_Vgatep_sourcep=t7_Vgatep_sourcep*d10_exp0;
t9_Vdrainp_sourcep=t7_Vdrainp_sourcep*d10_exp0;
t9_Vdrainb_sourcep=t7_Vdrainb_sourcep*d10_exp0;
t9_Vbulkp_sourcep=t7_Vbulkp_sourcep*d10_exp0;
t9_Vsourceb_sourcep=t7_Vsourceb_sourcep*d10_exp0;
#endif
t9=d00_exp0;
}
}
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((t8_Vgatep_sourcep*t8)+(t8*t8_Vgatep_sourcep));
t0_Vdrainp_sourcep=((t8_Vdrainp_sourcep*t8)+(t8*t8_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((t8_Vdrainb_sourcep*t8)+(t8*t8_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((t8_Vbulkp_sourcep*t8)+(t8*t8_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((t8_Vsourceb_sourcep*t8)+(t8*t8_Vsourceb_sourcep));
#endif
t0=(t8*t8);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=t9_Vgatep_sourcep;
t1_Vdrainp_sourcep=t9_Vdrainp_sourcep;
t1_Vdrainb_sourcep=t9_Vdrainb_sourcep;
t1_Vbulkp_sourcep=t9_Vbulkp_sourcep;
t1_Vsourceb_sourcep=t9_Vsourceb_sourcep;
#endif
t1=((t9-1.0)+1.0e-4);
#if defined(_DERIVATE)
t10_Vgatep_sourcep=(((t1_Vgatep_sourcep-t7_Vgatep_sourcep)*t8-(t1-t7)*t8_Vgatep_sourcep)/t8/t8);
t10_Vdrainp_sourcep=(((t1_Vdrainp_sourcep-t7_Vdrainp_sourcep)*t8-(t1-t7)*t8_Vdrainp_sourcep)/t8/t8);
t10_Vdrainb_sourcep=(((t1_Vdrainb_sourcep-t7_Vdrainb_sourcep)*t8-(t1-t7)*t8_Vdrainb_sourcep)/t8/t8);
t10_Vbulkp_sourcep=(((t1_Vbulkp_sourcep-t7_Vbulkp_sourcep)*t8-(t1-t7)*t8_Vbulkp_sourcep)/t8/t8);
t10_Vsourceb_sourcep=(((t1_Vsourceb_sourcep-t7_Vsourceb_sourcep)*t8-(t1-t7)*t8_Vsourceb_sourcep)/t8/t8);
#endif
t10=((t1-t7)/t8);
#if defined(_DERIVATE)
igcs_Vgatep_sourcep=((igc_Vgatep_sourcep*t10)+(igc*t10_Vgatep_sourcep));
igcs_Vdrainp_sourcep=((igc_Vdrainp_sourcep*t10)+(igc*t10_Vdrainp_sourcep));
igcs_Vdrainb_sourcep=((igc_Vdrainb_sourcep*t10)+(igc*t10_Vdrainb_sourcep));
igcs_Vbulkp_sourcep=((igc_Vbulkp_sourcep*t10)+(igc*t10_Vbulkp_sourcep));
igcs_Vsourceb_sourcep=((igc_Vsourceb_sourcep*t10)+(igc*t10_Vsourceb_sourcep));
#endif
igcs=(igc*t10);
#if defined(_DERIVATE)
t10_Vgatep_sourcep=(((((t7_Vgatep_sourcep*t9)+(t7*t9_Vgatep_sourcep))-t1_Vgatep_sourcep)*t8-((t7*t9)-t1)*t8_Vgatep_sourcep)/t8/t8);
t10_Vdrainp_sourcep=(((((t7_Vdrainp_sourcep*t9)+(t7*t9_Vdrainp_sourcep))-t1_Vdrainp_sourcep)*t8-((t7*t9)-t1)*t8_Vdrainp_sourcep)/t8/t8);
t10_Vdrainb_sourcep=(((((t7_Vdrainb_sourcep*t9)+(t7*t9_Vdrainb_sourcep))-t1_Vdrainb_sourcep)*t8-((t7*t9)-t1)*t8_Vdrainb_sourcep)/t8/t8);
t10_Vbulkp_sourcep=(((((t7_Vbulkp_sourcep*t9)+(t7*t9_Vbulkp_sourcep))-t1_Vbulkp_sourcep)*t8-((t7*t9)-t1)*t8_Vbulkp_sourcep)/t8/t8);
t10_Vsourceb_sourcep=(((((t7_Vsourceb_sourcep*t9)+(t7*t9_Vsourceb_sourcep))-t1_Vsourceb_sourcep)*t8-((t7*t9)-t1)*t8_Vsourceb_sourcep)/t8/t8);
#endif
t10=(((t7*t9)-t1)/t8);
#if defined(_DERIVATE)
igcd_Vgatep_sourcep=((igc_Vgatep_sourcep*t10)+(igc*t10_Vgatep_sourcep));
igcd_Vdrainp_sourcep=((igc_Vdrainp_sourcep*t10)+(igc*t10_Vdrainp_sourcep));
igcd_Vdrainb_sourcep=((igc_Vdrainb_sourcep*t10)+(igc*t10_Vdrainb_sourcep));
igcd_Vbulkp_sourcep=((igc_Vbulkp_sourcep*t10)+(igc*t10_Vbulkp_sourcep));
igcd_Vsourceb_sourcep=((igc_Vsourceb_sourcep*t10)+(igc*t10_Vsourceb_sourcep));
#endif
igcd=(igc*t10);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgs_Vgatep_sourcep;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgs-vfbsd);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+1.0e-4))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+1.0e-4))
#endif
#if defined(_DERIVATE)
vgs_eff_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
vgs_eff_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
vgs_eff_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
vgs_eff_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
vgs_eff_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
vgs_eff=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((vgs_Vgatep_sourcep*vgs_eff)+(vgs*vgs_eff_Vgatep_sourcep));
t2_Vdrainp_sourcep=(vgs*vgs_eff_Vdrainp_sourcep);
t2_Vdrainb_sourcep=(vgs*vgs_eff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(vgs*vgs_eff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(vgs*vgs_eff_Vsourceb_sourcep);
#endif
t2=(vgs*vgs_eff);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=0.0;
t11_Vdrainp_sourcep=0.0;
t11_Vdrainb_sourcep=0.0;
t11_Vbulkp_sourcep=0.0;
t11_Vsourceb_sourcep=0.0;
#endif
t11=aechvbedge;
#if defined(_DERIVATE)
t12_Vgatep_sourcep=0.0;
t12_Vdrainp_sourcep=0.0;
t12_Vdrainb_sourcep=0.0;
t12_Vbulkp_sourcep=0.0;
t12_Vsourceb_sourcep=0.0;
#endif
t12=bechvbedge;
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((aigsd*cigsd)-bigsd);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(bigsd*cigsd);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t12_Vgatep_sourcep*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)))+(t12*(((t3_Vgatep_sourcep*vgs_eff)+(t3*vgs_eff_Vgatep_sourcep))-((((t4_Vgatep_sourcep*vgs_eff)+(t4*vgs_eff_Vgatep_sourcep))*vgs_eff)+((t4*vgs_eff)*vgs_eff_Vgatep_sourcep)))));
t5_Vdrainp_sourcep=((t12_Vdrainp_sourcep*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)))+(t12*(((t3_Vdrainp_sourcep*vgs_eff)+(t3*vgs_eff_Vdrainp_sourcep))-((((t4_Vdrainp_sourcep*vgs_eff)+(t4*vgs_eff_Vdrainp_sourcep))*vgs_eff)+((t4*vgs_eff)*vgs_eff_Vdrainp_sourcep)))));
t5_Vdrainb_sourcep=((t12_Vdrainb_sourcep*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)))+(t12*(((t3_Vdrainb_sourcep*vgs_eff)+(t3*vgs_eff_Vdrainb_sourcep))-((((t4_Vdrainb_sourcep*vgs_eff)+(t4*vgs_eff_Vdrainb_sourcep))*vgs_eff)+((t4*vgs_eff)*vgs_eff_Vdrainb_sourcep)))));
t5_Vbulkp_sourcep=((t12_Vbulkp_sourcep*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)))+(t12*(((t3_Vbulkp_sourcep*vgs_eff)+(t3*vgs_eff_Vbulkp_sourcep))-((((t4_Vbulkp_sourcep*vgs_eff)+(t4*vgs_eff_Vbulkp_sourcep))*vgs_eff)+((t4*vgs_eff)*vgs_eff_Vbulkp_sourcep)))));
t5_Vsourceb_sourcep=((t12_Vsourceb_sourcep*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)))+(t12*(((t3_Vsourceb_sourcep*vgs_eff)+(t3*vgs_eff_Vsourceb_sourcep))-((((t4_Vsourceb_sourcep*vgs_eff)+(t4*vgs_eff_Vsourceb_sourcep))*vgs_eff)+((t4*vgs_eff)*vgs_eff_Vsourceb_sourcep)))));
#endif
t5=(t12*((aigsd+(t3*vgs_eff))-((t4*vgs_eff)*vgs_eff)));
if
((t5>34.0))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=5.834617425e14;
}
else
{
if
((t5<(-34.0)))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t5)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep*d10_exp0;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep*d10_exp0;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep*d10_exp0;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep*d10_exp0;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep*d10_exp0;
#endif
t6=d00_exp0;
}
}
}
#if defined(_DERIVATE)
igs_Vgatep_sourcep=((((t11_Vgatep_sourcep*t2)+(t11*t2_Vgatep_sourcep))*t6)+((t11*t2)*t6_Vgatep_sourcep));
igs_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t2)+(t11*t2_Vdrainp_sourcep))*t6)+((t11*t2)*t6_Vdrainp_sourcep));
igs_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t2)+(t11*t2_Vdrainb_sourcep))*t6)+((t11*t2)*t6_Vdrainb_sourcep));
igs_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t2)+(t11*t2_Vbulkp_sourcep))*t6)+((t11*t2)*t6_Vbulkp_sourcep));
igs_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t2)+(t11*t2_Vsourceb_sourcep))*t6)+((t11*t2)*t6_Vsourceb_sourcep));
#endif
igs=((t11*t2)*t6);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgd_Vgatep_sourcep;
t0_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgd-vfbsd);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+1.0e-4))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+1.0e-4))
#endif
#if defined(_DERIVATE)
vgd_eff_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
vgd_eff_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
vgd_eff_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
vgd_eff_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
vgd_eff_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
vgd_eff=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((vgd_Vgatep_sourcep*vgd_eff)+(vgd*vgd_eff_Vgatep_sourcep));
t2_Vdrainp_sourcep=((vgd_Vdrainp_sourcep*vgd_eff)+(vgd*vgd_eff_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(vgd*vgd_eff_Vdrainb_sourcep);
t2_Vbulkp_sourcep=(vgd*vgd_eff_Vbulkp_sourcep);
t2_Vsourceb_sourcep=(vgd*vgd_eff_Vsourceb_sourcep);
#endif
t2=(vgd*vgd_eff);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t12_Vgatep_sourcep*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)))+(t12*(((t3_Vgatep_sourcep*vgd_eff)+(t3*vgd_eff_Vgatep_sourcep))-((((t4_Vgatep_sourcep*vgd_eff)+(t4*vgd_eff_Vgatep_sourcep))*vgd_eff)+((t4*vgd_eff)*vgd_eff_Vgatep_sourcep)))));
t5_Vdrainp_sourcep=((t12_Vdrainp_sourcep*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)))+(t12*(((t3_Vdrainp_sourcep*vgd_eff)+(t3*vgd_eff_Vdrainp_sourcep))-((((t4_Vdrainp_sourcep*vgd_eff)+(t4*vgd_eff_Vdrainp_sourcep))*vgd_eff)+((t4*vgd_eff)*vgd_eff_Vdrainp_sourcep)))));
t5_Vdrainb_sourcep=((t12_Vdrainb_sourcep*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)))+(t12*(((t3_Vdrainb_sourcep*vgd_eff)+(t3*vgd_eff_Vdrainb_sourcep))-((((t4_Vdrainb_sourcep*vgd_eff)+(t4*vgd_eff_Vdrainb_sourcep))*vgd_eff)+((t4*vgd_eff)*vgd_eff_Vdrainb_sourcep)))));
t5_Vbulkp_sourcep=((t12_Vbulkp_sourcep*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)))+(t12*(((t3_Vbulkp_sourcep*vgd_eff)+(t3*vgd_eff_Vbulkp_sourcep))-((((t4_Vbulkp_sourcep*vgd_eff)+(t4*vgd_eff_Vbulkp_sourcep))*vgd_eff)+((t4*vgd_eff)*vgd_eff_Vbulkp_sourcep)))));
t5_Vsourceb_sourcep=((t12_Vsourceb_sourcep*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)))+(t12*(((t3_Vsourceb_sourcep*vgd_eff)+(t3*vgd_eff_Vsourceb_sourcep))-((((t4_Vsourceb_sourcep*vgd_eff)+(t4*vgd_eff_Vsourceb_sourcep))*vgd_eff)+((t4*vgd_eff)*vgd_eff_Vsourceb_sourcep)))));
#endif
t5=(t12*((aigsd+(t3*vgd_eff))-((t4*vgd_eff)*vgd_eff)));
if
((t5>34.0))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=5.834617425e14;
}
else
{
if
((t5<(-34.0)))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t5)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep*d10_exp0;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep*d10_exp0;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep*d10_exp0;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep*d10_exp0;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep*d10_exp0;
#endif
t6=d00_exp0;
}
}
}
#if defined(_DERIVATE)
igd_Vgatep_sourcep=((((t11_Vgatep_sourcep*t2)+(t11*t2_Vgatep_sourcep))*t6)+((t11*t2)*t6_Vgatep_sourcep));
igd_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t2)+(t11*t2_Vdrainp_sourcep))*t6)+((t11*t2)*t6_Vdrainp_sourcep));
igd_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t2)+(t11*t2_Vdrainb_sourcep))*t6)+((t11*t2)*t6_Vdrainb_sourcep));
igd_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t2)+(t11*t2_Vbulkp_sourcep))*t6)+((t11*t2)*t6_Vbulkp_sourcep));
igd_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t2)+(t11*t2_Vsourceb_sourcep))*t6)+((t11*t2)*t6_Vsourceb_sourcep));
#endif
igd=((t11*t2)*t6);
}
else
{
#if defined(_DERIVATE)
igcs_Vgatep_sourcep=0.0;
igcs_Vdrainp_sourcep=0.0;
igcs_Vdrainb_sourcep=0.0;
igcs_Vbulkp_sourcep=0.0;
igcs_Vsourceb_sourcep=0.0;
#endif
igcs=0.0;
#if defined(_DERIVATE)
igcd_Vgatep_sourcep=0.0;
igcd_Vdrainp_sourcep=0.0;
igcd_Vdrainb_sourcep=0.0;
igcd_Vbulkp_sourcep=0.0;
igcd_Vsourceb_sourcep=0.0;
#endif
igcd=0.0;
#if defined(_DERIVATE)
igs_Vgatep_sourcep=0.0;
igs_Vdrainp_sourcep=0.0;
igs_Vdrainb_sourcep=0.0;
igs_Vbulkp_sourcep=0.0;
igs_Vsourceb_sourcep=0.0;
#endif
igs=0.0;
#if defined(_DERIVATE)
igd_Vgatep_sourcep=0.0;
igd_Vdrainp_sourcep=0.0;
igd_Vdrainb_sourcep=0.0;
igd_Vbulkp_sourcep=0.0;
igd_Vsourceb_sourcep=0.0;
#endif
igd=0.0;
}
if
((igbmod==1.0))
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vtm*nigbacc);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(-Vgs_eff_Vgatep_sourcep);
t1_Vdrainp_sourcep=((-Vgs_eff_Vdrainp_sourcep)+vbseff_Vdrainp_sourcep);
t1_Vdrainb_sourcep=((-Vgs_eff_Vdrainb_sourcep)+vbseff_Vdrainb_sourcep);
t1_Vbulkp_sourcep=((-Vgs_eff_Vbulkp_sourcep)+vbseff_Vbulkp_sourcep);
t1_Vsourceb_sourcep=((-Vgs_eff_Vsourceb_sourcep)+vbseff_Vsourceb_sourcep);
#endif
t1=(((-Vgs_eff)+vbseff)+vfbzb);
#if defined(_DERIVATE)
vxnvt_Vgatep_sourcep=((t1_Vgatep_sourcep*t0-t1*t0_Vgatep_sourcep)/t0/t0);
vxnvt_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t0-t1*t0_Vdrainp_sourcep)/t0/t0);
vxnvt_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t0-t1*t0_Vdrainb_sourcep)/t0/t0);
vxnvt_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t0-t1*t0_Vbulkp_sourcep)/t0/t0);
vxnvt_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t0-t1*t0_Vsourceb_sourcep)/t0/t0);
#endif
vxnvt=(t1/t0);
if
((vxnvt>34.0))
{
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=t1_Vgatep_sourcep;
vaux_Vdrainp_sourcep=t1_Vdrainp_sourcep;
vaux_Vdrainb_sourcep=t1_Vdrainb_sourcep;
vaux_Vbulkp_sourcep=t1_Vbulkp_sourcep;
vaux_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
vaux=t1;
}
else
{
if
((vxnvt<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=t0_Vgatep_sourcep*d00_logE0;
vaux_Vdrainp_sourcep=t0_Vdrainp_sourcep*d00_logE0;
vaux_Vdrainb_sourcep=t0_Vdrainb_sourcep*d00_logE0;
vaux_Vbulkp_sourcep=t0_Vbulkp_sourcep*d00_logE0;
vaux_Vsourceb_sourcep=t0_Vsourceb_sourcep*d00_logE0;
#endif
vaux=(t0*d00_logE0);
}
}
else
{
{
double m00_exp(d00_exp0,vxnvt)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
expvxnvt_Vgatep_sourcep=vxnvt_Vgatep_sourcep*d10_exp0;
expvxnvt_Vdrainp_sourcep=vxnvt_Vdrainp_sourcep*d10_exp0;
expvxnvt_Vdrainb_sourcep=vxnvt_Vdrainb_sourcep*d10_exp0;
expvxnvt_Vbulkp_sourcep=vxnvt_Vbulkp_sourcep*d10_exp0;
expvxnvt_Vsourceb_sourcep=vxnvt_Vsourceb_sourcep*d10_exp0;
#endif
expvxnvt=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+expvxnvt))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+expvxnvt))
#endif
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=((t0_Vgatep_sourcep*d00_logE0)+(t0*expvxnvt_Vgatep_sourcep*d10_logE0));
vaux_Vdrainp_sourcep=((t0_Vdrainp_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainp_sourcep*d10_logE0));
vaux_Vdrainb_sourcep=((t0_Vdrainb_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainb_sourcep*d10_logE0));
vaux_Vbulkp_sourcep=((t0_Vbulkp_sourcep*d00_logE0)+(t0*expvxnvt_Vbulkp_sourcep*d10_logE0));
vaux_Vsourceb_sourcep=((t0_Vsourceb_sourcep*d00_logE0)+(t0*expvxnvt_Vsourceb_sourcep*d10_logE0));
#endif
vaux=(t0*d00_logE0);
}
}
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep*vaux)+((Vgs_eff-vbseff)*vaux_Vgatep_sourcep));
t2_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-vbseff_Vdrainp_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(((Vgs_eff_Vdrainb_sourcep-vbseff_Vdrainb_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(((Vgs_eff_Vbulkp_sourcep-vbseff_Vbulkp_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(((Vgs_eff_Vsourceb_sourcep-vbseff_Vsourceb_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vsourceb_sourcep));
#endif
t2=((Vgs_eff-vbseff)*vaux);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=0.0;
t11_Vdrainp_sourcep=0.0;
t11_Vdrainb_sourcep=0.0;
t11_Vbulkp_sourcep=0.0;
t11_Vsourceb_sourcep=0.0;
#endif
t11=(((4.97232e-7*weff)*leff)*toxratio);
#if defined(_DERIVATE)
t12_Vgatep_sourcep=0.0;
t12_Vdrainp_sourcep=0.0;
t12_Vdrainb_sourcep=0.0;
t12_Vbulkp_sourcep=0.0;
t12_Vsourceb_sourcep=0.0;
#endif
t12=((-7.45669e11)*toxe);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((aigbacc*cigbacc)-bigbacc);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(bigbacc*cigbacc);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t12_Vgatep_sourcep*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)))+(t12*(((t3_Vgatep_sourcep*voxacc)+(t3*voxacc_Vgatep_sourcep))-((((t4_Vgatep_sourcep*voxacc)+(t4*voxacc_Vgatep_sourcep))*voxacc)+((t4*voxacc)*voxacc_Vgatep_sourcep)))));
t5_Vdrainp_sourcep=((t12_Vdrainp_sourcep*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)))+(t12*(((t3_Vdrainp_sourcep*voxacc)+(t3*voxacc_Vdrainp_sourcep))-((((t4_Vdrainp_sourcep*voxacc)+(t4*voxacc_Vdrainp_sourcep))*voxacc)+((t4*voxacc)*voxacc_Vdrainp_sourcep)))));
t5_Vdrainb_sourcep=((t12_Vdrainb_sourcep*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)))+(t12*(((t3_Vdrainb_sourcep*voxacc)+(t3*voxacc_Vdrainb_sourcep))-((((t4_Vdrainb_sourcep*voxacc)+(t4*voxacc_Vdrainb_sourcep))*voxacc)+((t4*voxacc)*voxacc_Vdrainb_sourcep)))));
t5_Vbulkp_sourcep=((t12_Vbulkp_sourcep*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)))+(t12*(((t3_Vbulkp_sourcep*voxacc)+(t3*voxacc_Vbulkp_sourcep))-((((t4_Vbulkp_sourcep*voxacc)+(t4*voxacc_Vbulkp_sourcep))*voxacc)+((t4*voxacc)*voxacc_Vbulkp_sourcep)))));
t5_Vsourceb_sourcep=((t12_Vsourceb_sourcep*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)))+(t12*(((t3_Vsourceb_sourcep*voxacc)+(t3*voxacc_Vsourceb_sourcep))-((((t4_Vsourceb_sourcep*voxacc)+(t4*voxacc_Vsourceb_sourcep))*voxacc)+((t4*voxacc)*voxacc_Vsourceb_sourcep)))));
#endif
t5=(t12*((aigbacc+(t3*voxacc))-((t4*voxacc)*voxacc)));
if
((t5>34.0))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=5.834617425e14;
}
else
{
if
((t5<(-34.0)))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t5)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep*d10_exp0;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep*d10_exp0;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep*d10_exp0;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep*d10_exp0;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep*d10_exp0;
#endif
t6=d00_exp0;
}
}
}
#if defined(_DERIVATE)
igbacc_Vgatep_sourcep=((((t11_Vgatep_sourcep*t2)+(t11*t2_Vgatep_sourcep))*t6)+((t11*t2)*t6_Vgatep_sourcep));
igbacc_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t2)+(t11*t2_Vdrainp_sourcep))*t6)+((t11*t2)*t6_Vdrainp_sourcep));
igbacc_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t2)+(t11*t2_Vdrainb_sourcep))*t6)+((t11*t2)*t6_Vdrainb_sourcep));
igbacc_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t2)+(t11*t2_Vbulkp_sourcep))*t6)+((t11*t2)*t6_Vbulkp_sourcep));
igbacc_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t2)+(t11*t2_Vsourceb_sourcep))*t6)+((t11*t2)*t6_Vsourceb_sourcep));
#endif
igbacc=((t11*t2)*t6);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vtm*nigbinv);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=voxdepinv_Vgatep_sourcep;
t1_Vdrainp_sourcep=voxdepinv_Vdrainp_sourcep;
t1_Vdrainb_sourcep=voxdepinv_Vdrainb_sourcep;
t1_Vbulkp_sourcep=voxdepinv_Vbulkp_sourcep;
t1_Vsourceb_sourcep=voxdepinv_Vsourceb_sourcep;
#endif
t1=(voxdepinv-eigbinv);
#if defined(_DERIVATE)
vxnvt_Vgatep_sourcep=((t1_Vgatep_sourcep*t0-t1*t0_Vgatep_sourcep)/t0/t0);
vxnvt_Vdrainp_sourcep=((t1_Vdrainp_sourcep*t0-t1*t0_Vdrainp_sourcep)/t0/t0);
vxnvt_Vdrainb_sourcep=((t1_Vdrainb_sourcep*t0-t1*t0_Vdrainb_sourcep)/t0/t0);
vxnvt_Vbulkp_sourcep=((t1_Vbulkp_sourcep*t0-t1*t0_Vbulkp_sourcep)/t0/t0);
vxnvt_Vsourceb_sourcep=((t1_Vsourceb_sourcep*t0-t1*t0_Vsourceb_sourcep)/t0/t0);
#endif
vxnvt=(t1/t0);
if
((vxnvt>34.0))
{
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=t1_Vgatep_sourcep;
vaux_Vdrainp_sourcep=t1_Vdrainp_sourcep;
vaux_Vdrainb_sourcep=t1_Vdrainb_sourcep;
vaux_Vbulkp_sourcep=t1_Vbulkp_sourcep;
vaux_Vsourceb_sourcep=t1_Vsourceb_sourcep;
#endif
vaux=t1;
}
else
{
if
((vxnvt<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=t0_Vgatep_sourcep*d00_logE0;
vaux_Vdrainp_sourcep=t0_Vdrainp_sourcep*d00_logE0;
vaux_Vdrainb_sourcep=t0_Vdrainb_sourcep*d00_logE0;
vaux_Vbulkp_sourcep=t0_Vbulkp_sourcep*d00_logE0;
vaux_Vsourceb_sourcep=t0_Vsourceb_sourcep*d00_logE0;
#endif
vaux=(t0*d00_logE0);
}
}
else
{
{
double m00_exp(d00_exp0,vxnvt)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
expvxnvt_Vgatep_sourcep=vxnvt_Vgatep_sourcep*d10_exp0;
expvxnvt_Vdrainp_sourcep=vxnvt_Vdrainp_sourcep*d10_exp0;
expvxnvt_Vdrainb_sourcep=vxnvt_Vdrainb_sourcep*d10_exp0;
expvxnvt_Vbulkp_sourcep=vxnvt_Vbulkp_sourcep*d10_exp0;
expvxnvt_Vsourceb_sourcep=vxnvt_Vsourceb_sourcep*d10_exp0;
#endif
expvxnvt=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+expvxnvt))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+expvxnvt))
#endif
#if defined(_DERIVATE)
vaux_Vgatep_sourcep=((t0_Vgatep_sourcep*d00_logE0)+(t0*expvxnvt_Vgatep_sourcep*d10_logE0));
vaux_Vdrainp_sourcep=((t0_Vdrainp_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainp_sourcep*d10_logE0));
vaux_Vdrainb_sourcep=((t0_Vdrainb_sourcep*d00_logE0)+(t0*expvxnvt_Vdrainb_sourcep*d10_logE0));
vaux_Vbulkp_sourcep=((t0_Vbulkp_sourcep*d00_logE0)+(t0*expvxnvt_Vbulkp_sourcep*d10_logE0));
vaux_Vsourceb_sourcep=((t0_Vsourceb_sourcep*d00_logE0)+(t0*expvxnvt_Vsourceb_sourcep*d10_logE0));
#endif
vaux=(t0*d00_logE0);
}
}
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep*vaux)+((Vgs_eff-vbseff)*vaux_Vgatep_sourcep));
t2_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-vbseff_Vdrainp_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(((Vgs_eff_Vdrainb_sourcep-vbseff_Vdrainb_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(((Vgs_eff_Vbulkp_sourcep-vbseff_Vbulkp_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(((Vgs_eff_Vsourceb_sourcep-vbseff_Vsourceb_sourcep)*vaux)+((Vgs_eff-vbseff)*vaux_Vsourceb_sourcep));
#endif
t2=((Vgs_eff-vbseff)*vaux);
#if defined(_DERIVATE)
t11_Vgatep_sourcep=t11_Vgatep_sourcep*0.75610;
t11_Vdrainp_sourcep=t11_Vdrainp_sourcep*0.75610;
t11_Vdrainb_sourcep=t11_Vdrainb_sourcep*0.75610;
t11_Vbulkp_sourcep=t11_Vbulkp_sourcep*0.75610;
t11_Vsourceb_sourcep=t11_Vsourceb_sourcep*0.75610;
#endif
t11=(t11*0.75610);
#if defined(_DERIVATE)
t12_Vgatep_sourcep=t12_Vgatep_sourcep*1.31724;
t12_Vdrainp_sourcep=t12_Vdrainp_sourcep*1.31724;
t12_Vdrainb_sourcep=t12_Vdrainb_sourcep*1.31724;
t12_Vbulkp_sourcep=t12_Vbulkp_sourcep*1.31724;
t12_Vsourceb_sourcep=t12_Vsourceb_sourcep*1.31724;
#endif
t12=(t12*1.31724);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=((aigbinv*cigbinv)-bigbinv);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=0.0;
t4_Vdrainp_sourcep=0.0;
t4_Vdrainb_sourcep=0.0;
t4_Vbulkp_sourcep=0.0;
t4_Vsourceb_sourcep=0.0;
#endif
t4=(bigbinv*cigbinv);
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((t12_Vgatep_sourcep*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vgatep_sourcep*voxdepinv)+(t3*voxdepinv_Vgatep_sourcep))-((((t4_Vgatep_sourcep*voxdepinv)+(t4*voxdepinv_Vgatep_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vgatep_sourcep)))));
t5_Vdrainp_sourcep=((t12_Vdrainp_sourcep*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vdrainp_sourcep*voxdepinv)+(t3*voxdepinv_Vdrainp_sourcep))-((((t4_Vdrainp_sourcep*voxdepinv)+(t4*voxdepinv_Vdrainp_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vdrainp_sourcep)))));
t5_Vdrainb_sourcep=((t12_Vdrainb_sourcep*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vdrainb_sourcep*voxdepinv)+(t3*voxdepinv_Vdrainb_sourcep))-((((t4_Vdrainb_sourcep*voxdepinv)+(t4*voxdepinv_Vdrainb_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vdrainb_sourcep)))));
t5_Vbulkp_sourcep=((t12_Vbulkp_sourcep*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vbulkp_sourcep*voxdepinv)+(t3*voxdepinv_Vbulkp_sourcep))-((((t4_Vbulkp_sourcep*voxdepinv)+(t4*voxdepinv_Vbulkp_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vbulkp_sourcep)))));
t5_Vsourceb_sourcep=((t12_Vsourceb_sourcep*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)))+(t12*(((t3_Vsourceb_sourcep*voxdepinv)+(t3*voxdepinv_Vsourceb_sourcep))-((((t4_Vsourceb_sourcep*voxdepinv)+(t4*voxdepinv_Vsourceb_sourcep))*voxdepinv)+((t4*voxdepinv)*voxdepinv_Vsourceb_sourcep)))));
#endif
t5=(t12*((aigbinv+(t3*voxdepinv))-((t4*voxdepinv)*voxdepinv)));
if
((t5>34.0))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=5.834617425e14;
}
else
{
if
((t5<(-34.0)))
{
#if defined(_DERIVATE)
t6_Vgatep_sourcep=0.0;
t6_Vdrainp_sourcep=0.0;
t6_Vdrainb_sourcep=0.0;
t6_Vbulkp_sourcep=0.0;
t6_Vsourceb_sourcep=0.0;
#endif
t6=1.713908431e-15;
}
else
{
{
double m00_exp(d00_exp0,t5)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
t6_Vgatep_sourcep=t5_Vgatep_sourcep*d10_exp0;
t6_Vdrainp_sourcep=t5_Vdrainp_sourcep*d10_exp0;
t6_Vdrainb_sourcep=t5_Vdrainb_sourcep*d10_exp0;
t6_Vbulkp_sourcep=t5_Vbulkp_sourcep*d10_exp0;
t6_Vsourceb_sourcep=t5_Vsourceb_sourcep*d10_exp0;
#endif
t6=d00_exp0;
}
}
}
#if defined(_DERIVATE)
igbinv_Vgatep_sourcep=((((t11_Vgatep_sourcep*t2)+(t11*t2_Vgatep_sourcep))*t6)+((t11*t2)*t6_Vgatep_sourcep));
igbinv_Vdrainp_sourcep=((((t11_Vdrainp_sourcep*t2)+(t11*t2_Vdrainp_sourcep))*t6)+((t11*t2)*t6_Vdrainp_sourcep));
igbinv_Vdrainb_sourcep=((((t11_Vdrainb_sourcep*t2)+(t11*t2_Vdrainb_sourcep))*t6)+((t11*t2)*t6_Vdrainb_sourcep));
igbinv_Vbulkp_sourcep=((((t11_Vbulkp_sourcep*t2)+(t11*t2_Vbulkp_sourcep))*t6)+((t11*t2)*t6_Vbulkp_sourcep));
igbinv_Vsourceb_sourcep=((((t11_Vsourceb_sourcep*t2)+(t11*t2_Vsourceb_sourcep))*t6)+((t11*t2)*t6_Vsourceb_sourcep));
#endif
igbinv=((t11*t2)*t6);
#if defined(_DERIVATE)
igb_Vgatep_sourcep=(igbinv_Vgatep_sourcep+igbacc_Vgatep_sourcep);
igb_Vdrainp_sourcep=(igbinv_Vdrainp_sourcep+igbacc_Vdrainp_sourcep);
igb_Vdrainb_sourcep=(igbinv_Vdrainb_sourcep+igbacc_Vdrainb_sourcep);
igb_Vbulkp_sourcep=(igbinv_Vbulkp_sourcep+igbacc_Vbulkp_sourcep);
igb_Vsourceb_sourcep=(igbinv_Vsourceb_sourcep+igbacc_Vsourceb_sourcep);
#endif
igb=(igbinv+igbacc);
}
else
{
#if defined(_DERIVATE)
igb_Vgatep_sourcep=0.0;
igb_Vdrainp_sourcep=0.0;
igb_Vdrainb_sourcep=0.0;
igb_Vbulkp_sourcep=0.0;
igb_Vsourceb_sourcep=0.0;
#endif
igb=0.0;
}
if
((nf!=1.0))
{
#if defined(_DERIVATE)
cdrain_Vgatep_sourcep=cdrain_Vgatep_sourcep*nf;
cdrain_Vdrainp_sourcep=cdrain_Vdrainp_sourcep*nf;
cdrain_Vdrainb_sourcep=cdrain_Vdrainb_sourcep*nf;
cdrain_Vbulkp_sourcep=cdrain_Vbulkp_sourcep*nf;
cdrain_Vsourceb_sourcep=cdrain_Vsourceb_sourcep*nf;
#endif
cdrain=(cdrain*nf);
#if defined(_DERIVATE)
csub_Vgatep_sourcep=csub_Vgatep_sourcep*nf;
csub_Vdrainp_sourcep=csub_Vdrainp_sourcep*nf;
csub_Vdrainb_sourcep=csub_Vdrainb_sourcep*nf;
csub_Vbulkp_sourcep=csub_Vbulkp_sourcep*nf;
csub_Vsourceb_sourcep=csub_Vsourceb_sourcep*nf;
#endif
csub=(csub*nf);
#if defined(_DERIVATE)
igidl_Vgatep_sourcep=igidl_Vgatep_sourcep*nf;
igidl_Vdrainp_sourcep=igidl_Vdrainp_sourcep*nf;
igidl_Vdrainb_sourcep=igidl_Vdrainb_sourcep*nf;
igidl_Vbulkp_sourcep=igidl_Vbulkp_sourcep*nf;
igidl_Vsourceb_sourcep=igidl_Vsourceb_sourcep*nf;
#endif
igidl=(igidl*nf);
#if defined(_DERIVATE)
igisl_Vgatep_sourcep=igisl_Vgatep_sourcep*nf;
igisl_Vdrainp_sourcep=igisl_Vdrainp_sourcep*nf;
igisl_Vdrainb_sourcep=igisl_Vdrainb_sourcep*nf;
igisl_Vbulkp_sourcep=igisl_Vbulkp_sourcep*nf;
igisl_Vsourceb_sourcep=igisl_Vsourceb_sourcep*nf;
#endif
igisl=(igisl*nf);
#if defined(_DERIVATE)
igcs_Vgatep_sourcep=igcs_Vgatep_sourcep*nf;
igcs_Vdrainp_sourcep=igcs_Vdrainp_sourcep*nf;
igcs_Vdrainb_sourcep=igcs_Vdrainb_sourcep*nf;
igcs_Vbulkp_sourcep=igcs_Vbulkp_sourcep*nf;
igcs_Vsourceb_sourcep=igcs_Vsourceb_sourcep*nf;
#endif
igcs=(igcs*nf);
#if defined(_DERIVATE)
igcd_Vgatep_sourcep=igcd_Vgatep_sourcep*nf;
igcd_Vdrainp_sourcep=igcd_Vdrainp_sourcep*nf;
igcd_Vdrainb_sourcep=igcd_Vdrainb_sourcep*nf;
igcd_Vbulkp_sourcep=igcd_Vbulkp_sourcep*nf;
igcd_Vsourceb_sourcep=igcd_Vsourceb_sourcep*nf;
#endif
igcd=(igcd*nf);
#if defined(_DERIVATE)
igs_Vgatep_sourcep=igs_Vgatep_sourcep*nf;
igs_Vdrainp_sourcep=igs_Vdrainp_sourcep*nf;
igs_Vdrainb_sourcep=igs_Vdrainb_sourcep*nf;
igs_Vbulkp_sourcep=igs_Vbulkp_sourcep*nf;
igs_Vsourceb_sourcep=igs_Vsourceb_sourcep*nf;
#endif
igs=(igs*nf);
#if defined(_DERIVATE)
igd_Vgatep_sourcep=igd_Vgatep_sourcep*nf;
igd_Vdrainp_sourcep=igd_Vdrainp_sourcep*nf;
igd_Vdrainb_sourcep=igd_Vdrainb_sourcep*nf;
igd_Vbulkp_sourcep=igd_Vbulkp_sourcep*nf;
igd_Vsourceb_sourcep=igd_Vsourceb_sourcep*nf;
#endif
igd=(igd*nf);
#if defined(_DERIVATE)
igb_Vgatep_sourcep=igb_Vgatep_sourcep*nf;
igb_Vdrainp_sourcep=igb_Vdrainp_sourcep*nf;
igb_Vdrainb_sourcep=igb_Vdrainb_sourcep*nf;
igb_Vbulkp_sourcep=igb_Vbulkp_sourcep*nf;
igb_Vsourceb_sourcep=igb_Vsourceb_sourcep*nf;
#endif
igb=(igb*nf);
}
ccn=1;
if
(((xpart<0)||(ccn==0)))
{
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=0.0;
qgate_Vdrainp_sourcep=0.0;
qgate_Vdrainb_sourcep=0.0;
qgate_Vbulkp_sourcep=0.0;
qgate_Vsourceb_sourcep=0.0;
#endif
qgate=0.0;
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vdrainb_sourcep=0.0;
qdrn_Vbulkp_sourcep=0.0;
qdrn_Vsourceb_sourcep=0.0;
#endif
qdrn=0.0;
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=0.0;
qsrc_Vdrainp_sourcep=0.0;
qsrc_Vdrainb_sourcep=0.0;
qsrc_Vbulkp_sourcep=0.0;
qsrc_Vsourceb_sourcep=0.0;
#endif
qsrc=0.0;
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=0.0;
qbulk_Vdrainp_sourcep=0.0;
qbulk_Vdrainb_sourcep=0.0;
qbulk_Vbulkp_sourcep=0.0;
qbulk_Vsourceb_sourcep=0.0;
#endif
qbulk=0.0;
#if defined(_DERIVATE)
qgmid_Vdrainb_sourcep=0.0;
qgmid_Vbulkp_sourcep=0.0;
qgmid_Vsourceb_sourcep=0.0;
qgmid_Vgatep_sourcep=0.0;
qgmid_Vdrainp_sourcep=0.0;
qgmid_Vgatem_sourcep=0.0;
#endif
qgmid=0.0;
}
else
{
if
((capmod==0))
{
if
((vbseff<0.0))
{
#if defined(_DERIVATE)
vbseff_Vdrainp_sourcep=Vbs_Vdrainp_sourcep;
vbseff_Vdrainb_sourcep=0.0;
vbseff_Vbulkp_sourcep=Vbs_Vbulkp_sourcep;
vbseff_Vsourceb_sourcep=0.0;
#endif
vbseff=Vbs;
}
else
{
#if defined(_DERIVATE)
vbseff_Vdrainp_sourcep=(-phis_Vdrainp_sourcep);
vbseff_Vdrainb_sourcep=(-phis_Vdrainb_sourcep);
vbseff_Vbulkp_sourcep=(-phis_Vbulkp_sourcep);
vbseff_Vsourceb_sourcep=(-phis_Vsourceb_sourcep);
#endif
vbseff=(phi-phis);
}
vfb=vfbcv;
#if defined(_DERIVATE)
vth_Vdrainp_sourcep=(k1ox*sqrtphis_Vdrainp_sourcep);
vth_Vdrainb_sourcep=(k1ox*sqrtphis_Vdrainb_sourcep);
vth_Vbulkp_sourcep=(k1ox*sqrtphis_Vbulkp_sourcep);
vth_Vsourceb_sourcep=(k1ox*sqrtphis_Vsourceb_sourcep);
#endif
vth=((vfb+phi)+(k1ox*sqrtphis));
#if defined(_DERIVATE)
vgst_Vgatep_sourcep=Vgs_eff_Vgatep_sourcep;
vgst_Vdrainp_sourcep=(Vgs_eff_Vdrainp_sourcep-vth_Vdrainp_sourcep);
vgst_Vdrainb_sourcep=(Vgs_eff_Vdrainb_sourcep-vth_Vdrainb_sourcep);
vgst_Vbulkp_sourcep=(Vgs_eff_Vbulkp_sourcep-vth_Vbulkp_sourcep);
vgst_Vsourceb_sourcep=(Vgs_eff_Vsourceb_sourcep-vth_Vsourceb_sourcep);
#endif
vgst=(Vgs_eff-vth);
coxwl=(((coxe*weffcv)*leffcv)*nf);
#if defined(_DERIVATE)
arg1_Vgatep_sourcep=Vgs_eff_Vgatep_sourcep;
arg1_Vdrainp_sourcep=(Vgs_eff_Vdrainp_sourcep-vbseff_Vdrainp_sourcep);
arg1_Vdrainb_sourcep=(Vgs_eff_Vdrainb_sourcep-vbseff_Vdrainb_sourcep);
arg1_Vbulkp_sourcep=(Vgs_eff_Vbulkp_sourcep-vbseff_Vbulkp_sourcep);
arg1_Vsourceb_sourcep=(Vgs_eff_Vsourceb_sourcep-vbseff_Vsourceb_sourcep);
#endif
arg1=((Vgs_eff-vbseff)-vfb);
if
((arg1<=0.0))
{
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*arg1_Vgatep_sourcep);
qgate_Vdrainp_sourcep=(coxwl*arg1_Vdrainp_sourcep);
qgate_Vdrainb_sourcep=(coxwl*arg1_Vdrainb_sourcep);
qgate_Vbulkp_sourcep=(coxwl*arg1_Vbulkp_sourcep);
qgate_Vsourceb_sourcep=(coxwl*arg1_Vsourceb_sourcep);
#endif
qgate=(coxwl*arg1);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-qgate_Vgatep_sourcep);
qbulk_Vdrainp_sourcep=(-qgate_Vdrainp_sourcep);
qbulk_Vdrainb_sourcep=(-qgate_Vdrainb_sourcep);
qbulk_Vbulkp_sourcep=(-qgate_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=(-qgate_Vsourceb_sourcep);
#endif
qbulk=(-qgate);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vdrainb_sourcep=0.0;
qdrn_Vbulkp_sourcep=0.0;
qdrn_Vsourceb_sourcep=0.0;
#endif
qdrn=0.0;
}
else
{
if
((vgst<=0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=(0.5*k1ox);
{
double m00_sqrt(d00_sqrt0,((t1*t1)+arg1))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t1*t1)+arg1))
#endif
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))+arg1_Vgatep_sourcep)*d10_sqrt0;
t2_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))+arg1_Vdrainp_sourcep)*d10_sqrt0;
t2_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))+arg1_Vdrainb_sourcep)*d10_sqrt0;
t2_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))+arg1_Vbulkp_sourcep)*d10_sqrt0;
t2_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))+arg1_Vsourceb_sourcep)*d10_sqrt0;
#endif
t2=d00_sqrt0;
}
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=((coxwl*k1ox)*(t2_Vgatep_sourcep-t1_Vgatep_sourcep));
qgate_Vdrainp_sourcep=((coxwl*k1ox)*(t2_Vdrainp_sourcep-t1_Vdrainp_sourcep));
qgate_Vdrainb_sourcep=((coxwl*k1ox)*(t2_Vdrainb_sourcep-t1_Vdrainb_sourcep));
qgate_Vbulkp_sourcep=((coxwl*k1ox)*(t2_Vbulkp_sourcep-t1_Vbulkp_sourcep));
qgate_Vsourceb_sourcep=((coxwl*k1ox)*(t2_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
qgate=((coxwl*k1ox)*(t2-t1));
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-qgate_Vgatep_sourcep);
qbulk_Vdrainp_sourcep=(-qgate_Vdrainp_sourcep);
qbulk_Vdrainb_sourcep=(-qgate_Vdrainb_sourcep);
qbulk_Vbulkp_sourcep=(-qgate_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=(-qgate_Vsourceb_sourcep);
#endif
qbulk=(-qgate);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vdrainb_sourcep=0.0;
qdrn_Vbulkp_sourcep=0.0;
qdrn_Vsourceb_sourcep=0.0;
#endif
qdrn=0.0;
}
else
{
two_third_coxwl=((2.0*(((coxe*weffcv)*leffcv)*nf))/3.0);
#if defined(_DERIVATE)
abulkcv_Vgatep_sourcep=abulk0_Vgatep_sourcep*abulkcvfactor;
abulkcv_Vdrainp_sourcep=abulk0_Vdrainp_sourcep*abulkcvfactor;
abulkcv_Vdrainb_sourcep=abulk0_Vdrainb_sourcep*abulkcvfactor;
abulkcv_Vbulkp_sourcep=abulk0_Vbulkp_sourcep*abulkcvfactor;
abulkcv_Vsourceb_sourcep=abulk0_Vsourceb_sourcep*abulkcvfactor;
#endif
abulkcv=(abulk0*abulkcvfactor);
#if defined(_DERIVATE)
vdsat_Vgatep_sourcep=((vgst_Vgatep_sourcep*abulkcv-vgst*abulkcv_Vgatep_sourcep)/abulkcv/abulkcv);
vdsat_Vdrainp_sourcep=((vgst_Vdrainp_sourcep*abulkcv-vgst*abulkcv_Vdrainp_sourcep)/abulkcv/abulkcv);
vdsat_Vdrainb_sourcep=((vgst_Vdrainb_sourcep*abulkcv-vgst*abulkcv_Vdrainb_sourcep)/abulkcv/abulkcv);
vdsat_Vbulkp_sourcep=((vgst_Vbulkp_sourcep*abulkcv-vgst*abulkcv_Vbulkp_sourcep)/abulkcv/abulkcv);
vdsat_Vsourceb_sourcep=((vgst_Vsourceb_sourcep*abulkcv-vgst*abulkcv_Vsourceb_sourcep)/abulkcv/abulkcv);
#endif
vdsat=(vgst/abulkcv);
if
((xpart>0.5))
{
if
((vdsat<=Vds))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vdsat_Vgatep_sourcep/3.0);
t1_Vdrainp_sourcep=(vdsat_Vdrainp_sourcep/3.0);
t1_Vdrainb_sourcep=(vdsat_Vdrainb_sourcep/3.0);
t1_Vbulkp_sourcep=(vdsat_Vbulkp_sourcep/3.0);
t1_Vsourceb_sourcep=(vdsat_Vsourceb_sourcep/3.0);
#endif
t1=(vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-t1_Vgatep_sourcep));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-t1_Vdrainp_sourcep));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-t1_Vdrainb_sourcep));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-t1_Vbulkp_sourcep));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-t1));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-two_third_coxwl)*vgst_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-two_third_coxwl)*vgst_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-two_third_coxwl)*vgst_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-two_third_coxwl)*vgst_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-two_third_coxwl)*vgst_Vsourceb_sourcep);
#endif
t2=((-two_third_coxwl)*vgst);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-(qgate_Vgatep_sourcep+t2_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+t2_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(-(qgate_Vdrainb_sourcep+t2_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(-(qgate_Vbulkp_sourcep+t2_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(-(qgate_Vsourceb_sourcep+t2_Vsourceb_sourcep));
#endif
qbulk=(-(qgate+t2));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=0.0;
qdrn_Vdrainp_sourcep=0.0;
qdrn_Vdrainb_sourcep=0.0;
qdrn_Vbulkp_sourcep=0.0;
qdrn_Vsourceb_sourcep=0.0;
#endif
qdrn=0.0;
}
else
{
#if defined(_DERIVATE)
alphaz_Vgatep_sourcep=((vgst_Vgatep_sourcep*vdsat-vgst*vdsat_Vgatep_sourcep)/vdsat/vdsat);
alphaz_Vdrainp_sourcep=((vgst_Vdrainp_sourcep*vdsat-vgst*vdsat_Vdrainp_sourcep)/vdsat/vdsat);
alphaz_Vdrainb_sourcep=((vgst_Vdrainb_sourcep*vdsat-vgst*vdsat_Vdrainb_sourcep)/vdsat/vdsat);
alphaz_Vbulkp_sourcep=((vgst_Vbulkp_sourcep*vdsat-vgst*vdsat_Vbulkp_sourcep)/vdsat/vdsat);
alphaz_Vsourceb_sourcep=((vgst_Vsourceb_sourcep*vdsat-vgst*vdsat_Vsourceb_sourcep)/vdsat/vdsat);
#endif
alphaz=(vgst/vdsat);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(2.0*vdsat_Vgatep_sourcep);
t1_Vdrainp_sourcep=((2.0*vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(2.0*vdsat_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(2.0*vdsat_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(2.0*vdsat_Vsourceb_sourcep);
#endif
t1=((2.0*vdsat)-Vds);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-Vds*(3.0*t1_Vgatep_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*t1)-Vds*(3.0*t1_Vdrainp_sourcep))/(3.0*t1)/(3.0*t1));
t2_Vdrainb_sourcep=(-Vds*(3.0*t1_Vdrainb_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vbulkp_sourcep=(-Vds*(3.0*t1_Vbulkp_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vsourceb_sourcep=(-Vds*(3.0*t1_Vsourceb_sourcep)/(3.0*t1)/(3.0*t1));
#endif
t2=(Vds/(3.0*t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=t2_Vgatep_sourcep*Vds;
t3_Vdrainp_sourcep=((t2_Vdrainp_sourcep*Vds)+(t2*Vds_Vdrainp_sourcep));
t3_Vdrainb_sourcep=t2_Vdrainb_sourcep*Vds;
t3_Vbulkp_sourcep=t2_Vbulkp_sourcep*Vds;
t3_Vsourceb_sourcep=t2_Vsourceb_sourcep*Vds;
#endif
t3=(t2*Vds);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(0.25*coxwl);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t9_Vgatep_sourcep*alphaz)+(t9*alphaz_Vgatep_sourcep));
t4_Vdrainp_sourcep=((t9_Vdrainp_sourcep*alphaz)+(t9*alphaz_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((t9_Vdrainb_sourcep*alphaz)+(t9*alphaz_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((t9_Vbulkp_sourcep*alphaz)+(t9*alphaz_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((t9_Vsourceb_sourcep*alphaz)+(t9*alphaz_Vsourceb_sourcep));
#endif
t4=(t9*alphaz);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((-t1_Vgatep_sourcep)-(3.0*t3_Vgatep_sourcep));
t7_Vdrainp_sourcep=(((2.0*Vds_Vdrainp_sourcep)-t1_Vdrainp_sourcep)-(3.0*t3_Vdrainp_sourcep));
t7_Vdrainb_sourcep=((-t1_Vdrainb_sourcep)-(3.0*t3_Vdrainb_sourcep));
t7_Vbulkp_sourcep=((-t1_Vbulkp_sourcep)-(3.0*t3_Vbulkp_sourcep));
t7_Vsourceb_sourcep=((-t1_Vsourceb_sourcep)-(3.0*t3_Vsourceb_sourcep));
#endif
t7=(((2.0*Vds)-t1)-(3.0*t3));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=(t3_Vgatep_sourcep-t1_Vgatep_sourcep);
t8_Vdrainp_sourcep=((t3_Vdrainp_sourcep-t1_Vdrainp_sourcep)-(2.0*Vds_Vdrainp_sourcep));
t8_Vdrainb_sourcep=(t3_Vdrainb_sourcep-t1_Vdrainb_sourcep);
t8_Vbulkp_sourcep=(t3_Vbulkp_sourcep-t1_Vbulkp_sourcep);
t8_Vsourceb_sourcep=(t3_Vsourceb_sourcep-t1_Vsourceb_sourcep);
#endif
t8=((t3-t1)-(2.0*Vds));
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-(0.5*(-t3_Vgatep_sourcep))));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-(0.5*(Vds_Vdrainp_sourcep-t3_Vdrainp_sourcep))));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-(0.5*(-t3_Vdrainb_sourcep))));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-(0.5*(-t3_Vbulkp_sourcep))));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-(0.5*(-t3_Vsourceb_sourcep))));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-(0.5*(Vds-t3))));
#if defined(_DERIVATE)
t10_Vgatep_sourcep=((t4_Vgatep_sourcep*t8)+(t4*t8_Vgatep_sourcep));
t10_Vdrainp_sourcep=((t4_Vdrainp_sourcep*t8)+(t4*t8_Vdrainp_sourcep));
t10_Vdrainb_sourcep=((t4_Vdrainb_sourcep*t8)+(t4*t8_Vdrainb_sourcep));
t10_Vbulkp_sourcep=((t4_Vbulkp_sourcep*t8)+(t4*t8_Vbulkp_sourcep));
t10_Vsourceb_sourcep=((t4_Vsourceb_sourcep*t8)+(t4*t8_Vsourceb_sourcep));
#endif
t10=(t4*t8);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=((t4_Vgatep_sourcep*t7)+(t4*t7_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=((t4_Vdrainp_sourcep*t7)+(t4*t7_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=((t4_Vdrainb_sourcep*t7)+(t4*t7_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=((t4_Vbulkp_sourcep*t7)+(t4*t7_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=((t4_Vsourceb_sourcep*t7)+(t4*t7_Vsourceb_sourcep));
#endif
qdrn=(t4*t7);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qdrn_Vgatep_sourcep)+t10_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qdrn_Vdrainp_sourcep)+t10_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qdrn_Vdrainb_sourcep)+t10_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qdrn_Vbulkp_sourcep)+t10_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qdrn_Vsourceb_sourcep)+t10_Vsourceb_sourcep));
#endif
qbulk=(-((qgate+qdrn)+t10));
}
}
else
{
if
((xpart<0.5))
{
if
((Vds>=vdsat))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vdsat_Vgatep_sourcep/3.0);
t1_Vdrainp_sourcep=(vdsat_Vdrainp_sourcep/3.0);
t1_Vdrainb_sourcep=(vdsat_Vdrainb_sourcep/3.0);
t1_Vbulkp_sourcep=(vdsat_Vbulkp_sourcep/3.0);
t1_Vsourceb_sourcep=(vdsat_Vsourceb_sourcep/3.0);
#endif
t1=(vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-t1_Vgatep_sourcep));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-t1_Vdrainp_sourcep));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-t1_Vdrainb_sourcep));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-t1_Vbulkp_sourcep));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-t1));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-two_third_coxwl)*vgst_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-two_third_coxwl)*vgst_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-two_third_coxwl)*vgst_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-two_third_coxwl)*vgst_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-two_third_coxwl)*vgst_Vsourceb_sourcep);
#endif
t2=((-two_third_coxwl)*vgst);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-(qgate_Vgatep_sourcep+t2_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+t2_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(-(qgate_Vdrainb_sourcep+t2_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(-(qgate_Vbulkp_sourcep+t2_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(-(qgate_Vsourceb_sourcep+t2_Vsourceb_sourcep));
#endif
qbulk=(-(qgate+t2));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(0.4*t2_Vgatep_sourcep);
qdrn_Vdrainp_sourcep=(0.4*t2_Vdrainp_sourcep);
qdrn_Vdrainb_sourcep=(0.4*t2_Vdrainb_sourcep);
qdrn_Vbulkp_sourcep=(0.4*t2_Vbulkp_sourcep);
qdrn_Vsourceb_sourcep=(0.4*t2_Vsourceb_sourcep);
#endif
qdrn=(0.4*t2);
}
else
{
#if defined(_DERIVATE)
alphaz_Vgatep_sourcep=((vgst_Vgatep_sourcep*vdsat-vgst*vdsat_Vgatep_sourcep)/vdsat/vdsat);
alphaz_Vdrainp_sourcep=((vgst_Vdrainp_sourcep*vdsat-vgst*vdsat_Vdrainp_sourcep)/vdsat/vdsat);
alphaz_Vdrainb_sourcep=((vgst_Vdrainb_sourcep*vdsat-vgst*vdsat_Vdrainb_sourcep)/vdsat/vdsat);
alphaz_Vbulkp_sourcep=((vgst_Vbulkp_sourcep*vdsat-vgst*vdsat_Vbulkp_sourcep)/vdsat/vdsat);
alphaz_Vsourceb_sourcep=((vgst_Vsourceb_sourcep*vdsat-vgst*vdsat_Vsourceb_sourcep)/vdsat/vdsat);
#endif
alphaz=(vgst/vdsat);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(2.0*vdsat_Vgatep_sourcep);
t1_Vdrainp_sourcep=((2.0*vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(2.0*vdsat_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(2.0*vdsat_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(2.0*vdsat_Vsourceb_sourcep);
#endif
t1=((2.0*vdsat)-Vds);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-Vds*(3.0*t1_Vgatep_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*t1)-Vds*(3.0*t1_Vdrainp_sourcep))/(3.0*t1)/(3.0*t1));
t2_Vdrainb_sourcep=(-Vds*(3.0*t1_Vdrainb_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vbulkp_sourcep=(-Vds*(3.0*t1_Vbulkp_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vsourceb_sourcep=(-Vds*(3.0*t1_Vsourceb_sourcep)/(3.0*t1)/(3.0*t1));
#endif
t2=(Vds/(3.0*t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=t2_Vgatep_sourcep*Vds;
t3_Vdrainp_sourcep=((t2_Vdrainp_sourcep*Vds)+(t2*Vds_Vdrainp_sourcep));
t3_Vdrainb_sourcep=t2_Vdrainb_sourcep*Vds;
t3_Vbulkp_sourcep=t2_Vbulkp_sourcep*Vds;
t3_Vsourceb_sourcep=t2_Vsourceb_sourcep*Vds;
#endif
t3=(t2*Vds);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(0.25*coxwl);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t9_Vgatep_sourcep*alphaz)+(t9*alphaz_Vgatep_sourcep));
t4_Vdrainp_sourcep=((t9_Vdrainp_sourcep*alphaz)+(t9*alphaz_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((t9_Vdrainb_sourcep*alphaz)+(t9*alphaz_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((t9_Vbulkp_sourcep*alphaz)+(t9*alphaz_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((t9_Vsourceb_sourcep*alphaz)+(t9*alphaz_Vsourceb_sourcep));
#endif
t4=(t9*alphaz);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-(0.5*(-t3_Vgatep_sourcep))));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-(0.5*(Vds_Vdrainp_sourcep-t3_Vdrainp_sourcep))));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-(0.5*(-t3_Vdrainb_sourcep))));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-(0.5*(-t3_Vbulkp_sourcep))));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-(0.5*(-t3_Vsourceb_sourcep))));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-(0.5*(Vds-t3))));
#if defined(_DERIVATE)
t6_Vgatep_sourcep=((((8.0*vdsat_Vgatep_sourcep)*vdsat)+((8.0*vdsat)*vdsat_Vgatep_sourcep))-(6.0*vdsat_Vgatep_sourcep)*Vds);
t6_Vdrainp_sourcep=(((((8.0*vdsat_Vdrainp_sourcep)*vdsat)+((8.0*vdsat)*vdsat_Vdrainp_sourcep))-(((6.0*vdsat_Vdrainp_sourcep)*Vds)+((6.0*vdsat)*Vds_Vdrainp_sourcep)))+(((1.2*Vds_Vdrainp_sourcep)*Vds)+((1.2*Vds)*Vds_Vdrainp_sourcep)));
t6_Vdrainb_sourcep=((((8.0*vdsat_Vdrainb_sourcep)*vdsat)+((8.0*vdsat)*vdsat_Vdrainb_sourcep))-(6.0*vdsat_Vdrainb_sourcep)*Vds);
t6_Vbulkp_sourcep=((((8.0*vdsat_Vbulkp_sourcep)*vdsat)+((8.0*vdsat)*vdsat_Vbulkp_sourcep))-(6.0*vdsat_Vbulkp_sourcep)*Vds);
t6_Vsourceb_sourcep=((((8.0*vdsat_Vsourceb_sourcep)*vdsat)+((8.0*vdsat)*vdsat_Vsourceb_sourcep))-(6.0*vdsat_Vsourceb_sourcep)*Vds);
#endif
t6=((((8.0*vdsat)*vdsat)-((6.0*vdsat)*Vds))+((1.2*Vds)*Vds));
#if defined(_DERIVATE)
t8_Vgatep_sourcep=((t2_Vgatep_sourcep*t1-t2*t1_Vgatep_sourcep)/t1/t1);
t8_Vdrainp_sourcep=((t2_Vdrainp_sourcep*t1-t2*t1_Vdrainp_sourcep)/t1/t1);
t8_Vdrainb_sourcep=((t2_Vdrainb_sourcep*t1-t2*t1_Vdrainb_sourcep)/t1/t1);
t8_Vbulkp_sourcep=((t2_Vbulkp_sourcep*t1-t2*t1_Vbulkp_sourcep)/t1/t1);
t8_Vsourceb_sourcep=((t2_Vsourceb_sourcep*t1-t2*t1_Vsourceb_sourcep)/t1/t1);
#endif
t8=(t2/t1);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=((-t1_Vgatep_sourcep)-((t8_Vgatep_sourcep*t6)+(t8*t6_Vgatep_sourcep)));
t7_Vdrainp_sourcep=((Vds_Vdrainp_sourcep-t1_Vdrainp_sourcep)-((t8_Vdrainp_sourcep*t6)+(t8*t6_Vdrainp_sourcep)));
t7_Vdrainb_sourcep=((-t1_Vdrainb_sourcep)-((t8_Vdrainb_sourcep*t6)+(t8*t6_Vdrainb_sourcep)));
t7_Vbulkp_sourcep=((-t1_Vbulkp_sourcep)-((t8_Vbulkp_sourcep*t6)+(t8*t6_Vbulkp_sourcep)));
t7_Vsourceb_sourcep=((-t1_Vsourceb_sourcep)-((t8_Vsourceb_sourcep*t6)+(t8*t6_Vsourceb_sourcep)));
#endif
t7=((Vds-t1)-(t8*t6));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=((t4_Vgatep_sourcep*t7)+(t4*t7_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=((t4_Vdrainp_sourcep*t7)+(t4*t7_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=((t4_Vdrainb_sourcep*t7)+(t4*t7_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=((t4_Vbulkp_sourcep*t7)+(t4*t7_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=((t4_Vsourceb_sourcep*t7)+(t4*t7_Vsourceb_sourcep));
#endif
qdrn=(t4*t7);
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(2.0*(t1_Vgatep_sourcep+t3_Vgatep_sourcep));
t7_Vdrainp_sourcep=(2.0*(t1_Vdrainp_sourcep+t3_Vdrainp_sourcep));
t7_Vdrainb_sourcep=(2.0*(t1_Vdrainb_sourcep+t3_Vdrainb_sourcep));
t7_Vbulkp_sourcep=(2.0*(t1_Vbulkp_sourcep+t3_Vbulkp_sourcep));
t7_Vsourceb_sourcep=(2.0*(t1_Vsourceb_sourcep+t3_Vsourceb_sourcep));
#endif
t7=(2.0*(t1+t3));
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-(qgate_Vgatep_sourcep-((t4_Vgatep_sourcep*t7)+(t4*t7_Vgatep_sourcep))));
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep-((t4_Vdrainp_sourcep*t7)+(t4*t7_Vdrainp_sourcep))));
qbulk_Vdrainb_sourcep=(-(qgate_Vdrainb_sourcep-((t4_Vdrainb_sourcep*t7)+(t4*t7_Vdrainb_sourcep))));
qbulk_Vbulkp_sourcep=(-(qgate_Vbulkp_sourcep-((t4_Vbulkp_sourcep*t7)+(t4*t7_Vbulkp_sourcep))));
qbulk_Vsourceb_sourcep=(-(qgate_Vsourceb_sourcep-((t4_Vsourceb_sourcep*t7)+(t4*t7_Vsourceb_sourcep))));
#endif
qbulk=(-(qgate-(t4*t7)));
}
}
else
{
if
((Vds>=vdsat))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vdsat_Vgatep_sourcep/3.0);
t1_Vdrainp_sourcep=(vdsat_Vdrainp_sourcep/3.0);
t1_Vdrainb_sourcep=(vdsat_Vdrainb_sourcep/3.0);
t1_Vbulkp_sourcep=(vdsat_Vbulkp_sourcep/3.0);
t1_Vsourceb_sourcep=(vdsat_Vsourceb_sourcep/3.0);
#endif
t1=(vdsat/3.0);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-t1_Vgatep_sourcep));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-t1_Vdrainp_sourcep));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-t1_Vdrainb_sourcep));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-t1_Vbulkp_sourcep));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-t1));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((-two_third_coxwl)*vgst_Vgatep_sourcep);
t2_Vdrainp_sourcep=((-two_third_coxwl)*vgst_Vdrainp_sourcep);
t2_Vdrainb_sourcep=((-two_third_coxwl)*vgst_Vdrainb_sourcep);
t2_Vbulkp_sourcep=((-two_third_coxwl)*vgst_Vbulkp_sourcep);
t2_Vsourceb_sourcep=((-two_third_coxwl)*vgst_Vsourceb_sourcep);
#endif
t2=((-two_third_coxwl)*vgst);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-(qgate_Vgatep_sourcep+t2_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(-(qgate_Vdrainp_sourcep+t2_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(-(qgate_Vdrainb_sourcep+t2_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(-(qgate_Vbulkp_sourcep+t2_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(-(qgate_Vsourceb_sourcep+t2_Vsourceb_sourcep));
#endif
qbulk=(-(qgate+t2));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(0.5*t2_Vgatep_sourcep);
qdrn_Vdrainp_sourcep=(0.5*t2_Vdrainp_sourcep);
qdrn_Vdrainb_sourcep=(0.5*t2_Vdrainb_sourcep);
qdrn_Vbulkp_sourcep=(0.5*t2_Vbulkp_sourcep);
qdrn_Vsourceb_sourcep=(0.5*t2_Vsourceb_sourcep);
#endif
qdrn=(0.5*t2);
}
else
{
#if defined(_DERIVATE)
alphaz_Vgatep_sourcep=((vgst_Vgatep_sourcep*vdsat-vgst*vdsat_Vgatep_sourcep)/vdsat/vdsat);
alphaz_Vdrainp_sourcep=((vgst_Vdrainp_sourcep*vdsat-vgst*vdsat_Vdrainp_sourcep)/vdsat/vdsat);
alphaz_Vdrainb_sourcep=((vgst_Vdrainb_sourcep*vdsat-vgst*vdsat_Vdrainb_sourcep)/vdsat/vdsat);
alphaz_Vbulkp_sourcep=((vgst_Vbulkp_sourcep*vdsat-vgst*vdsat_Vbulkp_sourcep)/vdsat/vdsat);
alphaz_Vsourceb_sourcep=((vgst_Vsourceb_sourcep*vdsat-vgst*vdsat_Vsourceb_sourcep)/vdsat/vdsat);
#endif
alphaz=(vgst/vdsat);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(2.0*vdsat_Vgatep_sourcep);
t1_Vdrainp_sourcep=((2.0*vdsat_Vdrainp_sourcep)-Vds_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(2.0*vdsat_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(2.0*vdsat_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(2.0*vdsat_Vsourceb_sourcep);
#endif
t1=((2.0*vdsat)-Vds);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-Vds*(3.0*t1_Vgatep_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vdrainp_sourcep=((Vds_Vdrainp_sourcep*(3.0*t1)-Vds*(3.0*t1_Vdrainp_sourcep))/(3.0*t1)/(3.0*t1));
t2_Vdrainb_sourcep=(-Vds*(3.0*t1_Vdrainb_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vbulkp_sourcep=(-Vds*(3.0*t1_Vbulkp_sourcep)/(3.0*t1)/(3.0*t1));
t2_Vsourceb_sourcep=(-Vds*(3.0*t1_Vsourceb_sourcep)/(3.0*t1)/(3.0*t1));
#endif
t2=(Vds/(3.0*t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=t2_Vgatep_sourcep*Vds;
t3_Vdrainp_sourcep=((t2_Vdrainp_sourcep*Vds)+(t2*Vds_Vdrainp_sourcep));
t3_Vdrainb_sourcep=t2_Vdrainb_sourcep*Vds;
t3_Vbulkp_sourcep=t2_Vbulkp_sourcep*Vds;
t3_Vsourceb_sourcep=t2_Vsourceb_sourcep*Vds;
#endif
t3=(t2*Vds);
#if defined(_DERIVATE)
t9_Vgatep_sourcep=0.0;
t9_Vdrainp_sourcep=0.0;
t9_Vdrainb_sourcep=0.0;
t9_Vbulkp_sourcep=0.0;
t9_Vsourceb_sourcep=0.0;
#endif
t9=(0.25*coxwl);
#if defined(_DERIVATE)
t4_Vgatep_sourcep=((t9_Vgatep_sourcep*alphaz)+(t9*alphaz_Vgatep_sourcep));
t4_Vdrainp_sourcep=((t9_Vdrainp_sourcep*alphaz)+(t9*alphaz_Vdrainp_sourcep));
t4_Vdrainb_sourcep=((t9_Vdrainb_sourcep*alphaz)+(t9*alphaz_Vdrainb_sourcep));
t4_Vbulkp_sourcep=((t9_Vbulkp_sourcep*alphaz)+(t9*alphaz_Vbulkp_sourcep));
t4_Vsourceb_sourcep=((t9_Vsourceb_sourcep*alphaz)+(t9*alphaz_Vsourceb_sourcep));
#endif
t4=(t9*alphaz);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*(Vgs_eff_Vgatep_sourcep-(0.5*(-t3_Vgatep_sourcep))));
qgate_Vdrainp_sourcep=(coxwl*(Vgs_eff_Vdrainp_sourcep-(0.5*(Vds_Vdrainp_sourcep-t3_Vdrainp_sourcep))));
qgate_Vdrainb_sourcep=(coxwl*(Vgs_eff_Vdrainb_sourcep-(0.5*(-t3_Vdrainb_sourcep))));
qgate_Vbulkp_sourcep=(coxwl*(Vgs_eff_Vbulkp_sourcep-(0.5*(-t3_Vbulkp_sourcep))));
qgate_Vsourceb_sourcep=(coxwl*(Vgs_eff_Vsourceb_sourcep-(0.5*(-t3_Vsourceb_sourcep))));
#endif
qgate=(coxwl*(((Vgs_eff-vfb)-phi)-(0.5*(Vds-t3))));
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(t1_Vgatep_sourcep+t3_Vgatep_sourcep);
t7_Vdrainp_sourcep=(t1_Vdrainp_sourcep+t3_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(t1_Vdrainb_sourcep+t3_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(t1_Vbulkp_sourcep+t3_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(t1_Vsourceb_sourcep+t3_Vsourceb_sourcep);
#endif
t7=(t1+t3);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(((-t4_Vgatep_sourcep)*t7)+((-t4)*t7_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=(((-t4_Vdrainp_sourcep)*t7)+((-t4)*t7_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=(((-t4_Vdrainb_sourcep)*t7)+((-t4)*t7_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=(((-t4_Vbulkp_sourcep)*t7)+((-t4)*t7_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=(((-t4_Vsourceb_sourcep)*t7)+((-t4)*t7_Vsourceb_sourcep));
#endif
qdrn=((-t4)*t7);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qdrn_Vgatep_sourcep)+qdrn_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qdrn_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qdrn_Vdrainb_sourcep)+qdrn_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qdrn_Vbulkp_sourcep)+qdrn_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qdrn_Vsourceb_sourcep)+qdrn_Vsourceb_sourcep));
#endif
qbulk=(-((qgate+qdrn)+qdrn));
}
}
}
}
}
}
else
{
if
((vbseff<0.0))
{
#if defined(_DERIVATE)
vbseffcv_Vdrainp_sourcep=vbseff_Vdrainp_sourcep;
vbseffcv_Vdrainb_sourcep=vbseff_Vdrainb_sourcep;
vbseffcv_Vbulkp_sourcep=vbseff_Vbulkp_sourcep;
vbseffcv_Vsourceb_sourcep=vbseff_Vsourceb_sourcep;
#endif
vbseffcv=vbseff;
}
else
{
#if defined(_DERIVATE)
vbseffcv_Vdrainp_sourcep=(-phis_Vdrainp_sourcep);
vbseffcv_Vdrainb_sourcep=(-phis_Vdrainb_sourcep);
vbseffcv_Vbulkp_sourcep=(-phis_Vbulkp_sourcep);
vbseffcv_Vsourceb_sourcep=(-phis_Vsourceb_sourcep);
#endif
vbseffcv=(phi-phis);
}
coxwl=(((coxe*weffcv)*leffcv)*nf);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=(vtm*n_Vdrainp_sourcep)*noff;
t0_Vdrainb_sourcep=(vtm*n_Vdrainb_sourcep)*noff;
t0_Vbulkp_sourcep=(vtm*n_Vbulkp_sourcep)*noff;
t0_Vsourceb_sourcep=(vtm*n_Vsourceb_sourcep)*noff;
#endif
t0=((vtm*n)*noff);
#if defined(_DERIVATE)
vgstnvt_Vgatep_sourcep=((vgst_Vgatep_sourcep*t0-(vgst-voffcv)*t0_Vgatep_sourcep)/t0/t0);
vgstnvt_Vdrainp_sourcep=((vgst_Vdrainp_sourcep*t0-(vgst-voffcv)*t0_Vdrainp_sourcep)/t0/t0);
vgstnvt_Vdrainb_sourcep=((vgst_Vdrainb_sourcep*t0-(vgst-voffcv)*t0_Vdrainb_sourcep)/t0/t0);
vgstnvt_Vbulkp_sourcep=((vgst_Vbulkp_sourcep*t0-(vgst-voffcv)*t0_Vbulkp_sourcep)/t0/t0);
vgstnvt_Vsourceb_sourcep=((vgst_Vsourceb_sourcep*t0-(vgst-voffcv)*t0_Vsourceb_sourcep)/t0/t0);
#endif
vgstnvt=((vgst-voffcv)/t0);
if
((vgstnvt>34.0))
{
#if defined(_DERIVATE)
vgsteff_Vgatep_sourcep=vgst_Vgatep_sourcep;
vgsteff_Vdrainp_sourcep=vgst_Vdrainp_sourcep;
vgsteff_Vdrainb_sourcep=vgst_Vdrainb_sourcep;
vgsteff_Vbulkp_sourcep=vgst_Vbulkp_sourcep;
vgsteff_Vsourceb_sourcep=vgst_Vsourceb_sourcep;
#endif
vgsteff=(vgst-voffcv);
}
else
{
if
((vgstnvt<(-34.0)))
{
{
double m00_logE(d00_logE0,(1.0+1.713908431e-15))
#if defined(_DERIVATE)
vgsteff_Vgatep_sourcep=t0_Vgatep_sourcep*d00_logE0;
vgsteff_Vdrainp_sourcep=t0_Vdrainp_sourcep*d00_logE0;
vgsteff_Vdrainb_sourcep=t0_Vdrainb_sourcep*d00_logE0;
vgsteff_Vbulkp_sourcep=t0_Vbulkp_sourcep*d00_logE0;
vgsteff_Vsourceb_sourcep=t0_Vsourceb_sourcep*d00_logE0;
#endif
vgsteff=(t0*d00_logE0);
}
}
else
{
{
double m00_exp(d00_exp0,vgstnvt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vgsteff_Vgatep_sourcep=((t0_Vgatep_sourcep*d00_logE1)+(t0*vgstnvt_Vgatep_sourcep*d10_exp0*d10_logE1));
vgsteff_Vdrainp_sourcep=((t0_Vdrainp_sourcep*d00_logE1)+(t0*vgstnvt_Vdrainp_sourcep*d10_exp0*d10_logE1));
vgsteff_Vdrainb_sourcep=((t0_Vdrainb_sourcep*d00_logE1)+(t0*vgstnvt_Vdrainb_sourcep*d10_exp0*d10_logE1));
vgsteff_Vbulkp_sourcep=((t0_Vbulkp_sourcep*d00_logE1)+(t0*vgstnvt_Vbulkp_sourcep*d10_exp0*d10_logE1));
vgsteff_Vsourceb_sourcep=((t0_Vsourceb_sourcep*d00_logE1)+(t0*vgstnvt_Vsourceb_sourcep*d10_exp0*d10_logE1));
#endif
vgsteff=(t0*d00_logE1);
}
}
}
if
((capmod==1))
{
#if defined(_DERIVATE)
v3_Vgatep_sourcep=(-Vgs_eff_Vgatep_sourcep);
v3_Vdrainp_sourcep=((-Vgs_eff_Vdrainp_sourcep)+vbseffcv_Vdrainp_sourcep);
v3_Vdrainb_sourcep=((-Vgs_eff_Vdrainb_sourcep)+vbseffcv_Vdrainb_sourcep);
v3_Vbulkp_sourcep=((-Vgs_eff_Vbulkp_sourcep)+vbseffcv_Vbulkp_sourcep);
v3_Vsourceb_sourcep=((-Vgs_eff_Vsourceb_sourcep)+vbseffcv_Vsourceb_sourcep);
#endif
v3=(((vfbzb-Vgs_eff)+vbseffcv)-0.02);
if
((vfbzb<=0.0))
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
else
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(0.5*((v3_Vgatep_sourcep*t0-v3*t0_Vgatep_sourcep)/t0/t0));
t1_Vdrainp_sourcep=(0.5*((v3_Vdrainp_sourcep*t0-v3*t0_Vdrainp_sourcep)/t0/t0));
t1_Vdrainb_sourcep=(0.5*((v3_Vdrainb_sourcep*t0-v3*t0_Vdrainb_sourcep)/t0/t0));
t1_Vbulkp_sourcep=(0.5*((v3_Vbulkp_sourcep*t0-v3*t0_Vbulkp_sourcep)/t0/t0));
t1_Vsourceb_sourcep=(0.5*((v3_Vsourceb_sourcep*t0-v3*t0_Vsourceb_sourcep)/t0/t0));
#endif
t1=(0.5*(1.0+(v3/t0)));
#if defined(_DERIVATE)
vfbeff_Vgatep_sourcep=(-(0.5*(v3_Vgatep_sourcep+t0_Vgatep_sourcep)));
vfbeff_Vdrainp_sourcep=(-(0.5*(v3_Vdrainp_sourcep+t0_Vdrainp_sourcep)));
vfbeff_Vdrainb_sourcep=(-(0.5*(v3_Vdrainb_sourcep+t0_Vdrainb_sourcep)));
vfbeff_Vbulkp_sourcep=(-(0.5*(v3_Vbulkp_sourcep+t0_Vbulkp_sourcep)));
vfbeff_Vsourceb_sourcep=(-(0.5*(v3_Vsourceb_sourcep+t0_Vsourceb_sourcep)));
#endif
vfbeff=(vfbzb-(0.5*(v3+t0)));
#if defined(_DERIVATE)
qac0_Vgatep_sourcep=(coxwl*vfbeff_Vgatep_sourcep);
qac0_Vdrainp_sourcep=(coxwl*vfbeff_Vdrainp_sourcep);
qac0_Vdrainb_sourcep=(coxwl*vfbeff_Vdrainb_sourcep);
qac0_Vbulkp_sourcep=(coxwl*vfbeff_Vbulkp_sourcep);
qac0_Vsourceb_sourcep=(coxwl*vfbeff_Vsourceb_sourcep);
#endif
qac0=(coxwl*(vfbeff-vfbzb));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(0.5*k1ox);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep-vfbeff_Vgatep_sourcep)-vgsteff_Vgatep_sourcep);
t3_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-vfbeff_Vdrainp_sourcep)-vbseffcv_Vdrainp_sourcep)-vgsteff_Vdrainp_sourcep);
t3_Vdrainb_sourcep=(((Vgs_eff_Vdrainb_sourcep-vfbeff_Vdrainb_sourcep)-vbseffcv_Vdrainb_sourcep)-vgsteff_Vdrainb_sourcep);
t3_Vbulkp_sourcep=(((Vgs_eff_Vbulkp_sourcep-vfbeff_Vbulkp_sourcep)-vbseffcv_Vbulkp_sourcep)-vgsteff_Vbulkp_sourcep);
t3_Vsourceb_sourcep=(((Vgs_eff_Vsourceb_sourcep-vfbeff_Vsourceb_sourcep)-vbseffcv_Vsourceb_sourcep)-vgsteff_Vsourceb_sourcep);
#endif
t3=(((Vgs_eff-vfbeff)-vbseffcv)-vgsteff);
if
((k1ox==0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=0.0;
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=0.0;
}
else
{
if
((t3<0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(t0_Vgatep_sourcep+(t3_Vgatep_sourcep/k1ox));
t1_Vdrainp_sourcep=(t0_Vdrainp_sourcep+(t3_Vdrainp_sourcep/k1ox));
t1_Vdrainb_sourcep=(t0_Vdrainb_sourcep+(t3_Vdrainb_sourcep/k1ox));
t1_Vbulkp_sourcep=(t0_Vbulkp_sourcep+(t3_Vbulkp_sourcep/k1ox));
t1_Vsourceb_sourcep=(t0_Vsourceb_sourcep+(t3_Vsourceb_sourcep/k1ox));
#endif
t1=(t0+(t3/k1ox));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=coxwl;
}
else
{
{
double m00_sqrt(d00_sqrt0,((t0*t0)+t3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+t3))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))+t3_Vgatep_sourcep)*d10_sqrt0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+t3_Vdrainp_sourcep)*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+t3_Vdrainb_sourcep)*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+t3_Vbulkp_sourcep)*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+t3_Vsourceb_sourcep)*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(((coxwl*t0_Vgatep_sourcep)*t1-(coxwl*t0)*t1_Vgatep_sourcep)/t1/t1);
t2_Vdrainp_sourcep=(((coxwl*t0_Vdrainp_sourcep)*t1-(coxwl*t0)*t1_Vdrainp_sourcep)/t1/t1);
t2_Vdrainb_sourcep=(((coxwl*t0_Vdrainb_sourcep)*t1-(coxwl*t0)*t1_Vdrainb_sourcep)/t1/t1);
t2_Vbulkp_sourcep=(((coxwl*t0_Vbulkp_sourcep)*t1-(coxwl*t0)*t1_Vbulkp_sourcep)/t1/t1);
t2_Vsourceb_sourcep=(((coxwl*t0_Vsourceb_sourcep)*t1-(coxwl*t0)*t1_Vsourceb_sourcep)/t1/t1);
#endif
t2=((coxwl*t0)/t1);
}
}
#if defined(_DERIVATE)
qsub0_Vgatep_sourcep=((coxwl*k1ox)*(t1_Vgatep_sourcep-t0_Vgatep_sourcep));
qsub0_Vdrainp_sourcep=((coxwl*k1ox)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep));
qsub0_Vdrainb_sourcep=((coxwl*k1ox)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep));
qsub0_Vbulkp_sourcep=((coxwl*k1ox)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep));
qsub0_Vsourceb_sourcep=((coxwl*k1ox)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep));
#endif
qsub0=((coxwl*k1ox)*(t1-t0));
#if defined(_DERIVATE)
abulkcv_Vgatep_sourcep=abulk0_Vgatep_sourcep*abulkcvfactor;
abulkcv_Vdrainp_sourcep=abulk0_Vdrainp_sourcep*abulkcvfactor;
abulkcv_Vdrainb_sourcep=abulk0_Vdrainb_sourcep*abulkcvfactor;
abulkcv_Vbulkp_sourcep=abulk0_Vbulkp_sourcep*abulkcvfactor;
abulkcv_Vsourceb_sourcep=abulk0_Vsourceb_sourcep*abulkcvfactor;
#endif
abulkcv=(abulk0*abulkcvfactor);
#if defined(_DERIVATE)
vdsatcv_Vgatep_sourcep=((vgsteff_Vgatep_sourcep*abulkcv-vgsteff*abulkcv_Vgatep_sourcep)/abulkcv/abulkcv);
vdsatcv_Vdrainp_sourcep=((vgsteff_Vdrainp_sourcep*abulkcv-vgsteff*abulkcv_Vdrainp_sourcep)/abulkcv/abulkcv);
vdsatcv_Vdrainb_sourcep=((vgsteff_Vdrainb_sourcep*abulkcv-vgsteff*abulkcv_Vdrainb_sourcep)/abulkcv/abulkcv);
vdsatcv_Vbulkp_sourcep=((vgsteff_Vbulkp_sourcep*abulkcv-vgsteff*abulkcv_Vbulkp_sourcep)/abulkcv/abulkcv);
vdsatcv_Vsourceb_sourcep=((vgsteff_Vsourceb_sourcep*abulkcv-vgsteff*abulkcv_Vsourceb_sourcep)/abulkcv/abulkcv);
#endif
vdsatcv=(vgsteff/abulkcv);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vdsatcv_Vgatep_sourcep;
t0_Vdrainp_sourcep=(vdsatcv_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
t0_Vdrainb_sourcep=vdsatcv_Vdrainb_sourcep;
t0_Vbulkp_sourcep=vdsatcv_Vbulkp_sourcep;
t0_Vsourceb_sourcep=vdsatcv_Vsourceb_sourcep;
#endif
t0=((vdsatcv-Vds)-0.02);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+((4.0*0.02)*vdsatcv)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+((4.0*0.02)*vdsatcv)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))+((4.0*0.02)*vdsatcv_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+((4.0*0.02)*vdsatcv_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+((4.0*0.02)*vdsatcv_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+((4.0*0.02)*vdsatcv_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+((4.0*0.02)*vdsatcv_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
if
((t0>=0.0))
{
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=(vdsatcv_Vgatep_sourcep-(0.5*(t0_Vgatep_sourcep+t1_Vgatep_sourcep)));
vdseffcv_Vdrainp_sourcep=(vdsatcv_Vdrainp_sourcep-(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep)));
vdseffcv_Vdrainb_sourcep=(vdsatcv_Vdrainb_sourcep-(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep)));
vdseffcv_Vbulkp_sourcep=(vdsatcv_Vbulkp_sourcep-(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep)));
vdseffcv_Vsourceb_sourcep=(vdsatcv_Vsourceb_sourcep-(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep)));
#endif
vdseffcv=(vdsatcv-(0.5*(t0+t1)));
}
else
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(-(0.02+0.02)*(t1_Vgatep_sourcep-t0_Vgatep_sourcep)/(t1-t0)/(t1-t0));
t3_Vdrainp_sourcep=(-(0.02+0.02)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep)/(t1-t0)/(t1-t0));
t3_Vdrainb_sourcep=(-(0.02+0.02)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep)/(t1-t0)/(t1-t0));
t3_Vbulkp_sourcep=(-(0.02+0.02)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep)/(t1-t0)/(t1-t0));
t3_Vsourceb_sourcep=(-(0.02+0.02)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep)/(t1-t0)/(t1-t0));
#endif
t3=((0.02+0.02)/(t1-t0));
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-t3_Vgatep_sourcep);
t4_Vdrainp_sourcep=(-t3_Vdrainp_sourcep);
t4_Vdrainb_sourcep=(-t3_Vdrainb_sourcep);
t4_Vbulkp_sourcep=(-t3_Vbulkp_sourcep);
t4_Vsourceb_sourcep=(-t3_Vsourceb_sourcep);
#endif
t4=(1.0-t3);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
t5_Vgatep_sourcep=((((vdsatcv_Vgatep_sourcep*t3)+(vdsatcv*t3_Vgatep_sourcep))*(t1-t0)-(vdsatcv*t3)*(t1_Vgatep_sourcep-t0_Vgatep_sourcep))/(t1-t0)/(t1-t0));
t5_Vdrainp_sourcep=((((vdsatcv_Vdrainp_sourcep*t3)+(vdsatcv*t3_Vdrainp_sourcep))*(t1-t0)-(vdsatcv*t3)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep))/(t1-t0)/(t1-t0));
t5_Vdrainb_sourcep=((((vdsatcv_Vdrainb_sourcep*t3)+(vdsatcv*t3_Vdrainb_sourcep))*(t1-t0)-(vdsatcv*t3)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep))/(t1-t0)/(t1-t0));
t5_Vbulkp_sourcep=((((vdsatcv_Vbulkp_sourcep*t3)+(vdsatcv*t3_Vbulkp_sourcep))*(t1-t0)-(vdsatcv*t3)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep))/(t1-t0)/(t1-t0));
t5_Vsourceb_sourcep=((((vdsatcv_Vsourceb_sourcep*t3)+(vdsatcv*t3_Vsourceb_sourcep))*(t1-t0)-(vdsatcv*t3)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep))/(t1-t0)/(t1-t0));
#endif
t5=((vdsatcv*t3)/(t1-t0));
#endif
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=((vdsatcv_Vgatep_sourcep*t4)+(vdsatcv*t4_Vgatep_sourcep));
vdseffcv_Vdrainp_sourcep=((vdsatcv_Vdrainp_sourcep*t4)+(vdsatcv*t4_Vdrainp_sourcep));
vdseffcv_Vdrainb_sourcep=((vdsatcv_Vdrainb_sourcep*t4)+(vdsatcv*t4_Vdrainb_sourcep));
vdseffcv_Vbulkp_sourcep=((vdsatcv_Vbulkp_sourcep*t4)+(vdsatcv*t4_Vbulkp_sourcep));
vdseffcv_Vsourceb_sourcep=((vdsatcv_Vsourceb_sourcep*t4)+(vdsatcv*t4_Vsourceb_sourcep));
#endif
vdseffcv=(vdsatcv*t4);
}
if
((Vds==0.0))
{
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=0.0;
vdseffcv_Vdrainp_sourcep=0.0;
vdseffcv_Vdrainb_sourcep=0.0;
vdseffcv_Vbulkp_sourcep=0.0;
vdseffcv_Vsourceb_sourcep=0.0;
#endif
vdseffcv=0.0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((abulkcv_Vgatep_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vgatep_sourcep));
t0_Vdrainp_sourcep=((abulkcv_Vdrainp_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((abulkcv_Vdrainb_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((abulkcv_Vbulkp_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((abulkcv_Vsourceb_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vsourceb_sourcep));
#endif
t0=(abulkcv*vdseffcv);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(12.0*(vgsteff_Vgatep_sourcep-(0.5*t0_Vgatep_sourcep)));
t1_Vdrainp_sourcep=(12.0*(vgsteff_Vdrainp_sourcep-(0.5*t0_Vdrainp_sourcep)));
t1_Vdrainb_sourcep=(12.0*(vgsteff_Vdrainb_sourcep-(0.5*t0_Vdrainb_sourcep)));
t1_Vbulkp_sourcep=(12.0*(vgsteff_Vbulkp_sourcep-(0.5*t0_Vbulkp_sourcep)));
t1_Vsourceb_sourcep=(12.0*(vgsteff_Vsourceb_sourcep-(0.5*t0_Vsourceb_sourcep)));
#endif
t1=(12.0*((vgsteff-(0.5*t0))+1.0e-20));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((t0_Vgatep_sourcep*t1-t0*t1_Vgatep_sourcep)/t1/t1);
t2_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t1-t0*t1_Vdrainp_sourcep)/t1/t1);
t2_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t1-t0*t1_Vdrainb_sourcep)/t1/t1);
t2_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t1-t0*t1_Vbulkp_sourcep)/t1/t1);
t2_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t1-t0*t1_Vsourceb_sourcep)/t1/t1);
#endif
t2=(t0/t1);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((t0_Vgatep_sourcep*t2)+(t0*t2_Vgatep_sourcep));
t3_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t2)+(t0*t2_Vdrainp_sourcep));
t3_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t2)+(t0*t2_Vdrainb_sourcep));
t3_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t2)+(t0*t2_Vbulkp_sourcep));
t3_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t2)+(t0*t2_Vsourceb_sourcep));
#endif
t3=(t0*t2);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(coxwl*((vgsteff_Vgatep_sourcep-(0.5*t0_Vgatep_sourcep))+t3_Vgatep_sourcep));
qgate_Vdrainp_sourcep=(coxwl*((vgsteff_Vdrainp_sourcep-(0.5*t0_Vdrainp_sourcep))+t3_Vdrainp_sourcep));
qgate_Vdrainb_sourcep=(coxwl*((vgsteff_Vdrainb_sourcep-(0.5*t0_Vdrainb_sourcep))+t3_Vdrainb_sourcep));
qgate_Vbulkp_sourcep=(coxwl*((vgsteff_Vbulkp_sourcep-(0.5*t0_Vbulkp_sourcep))+t3_Vbulkp_sourcep));
qgate_Vsourceb_sourcep=(coxwl*((vgsteff_Vsourceb_sourcep-(0.5*t0_Vsourceb_sourcep))+t3_Vsourceb_sourcep));
#endif
qgate=(coxwl*((vgsteff-(0.5*t0))+t3));
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(-abulkcv_Vgatep_sourcep);
t7_Vdrainp_sourcep=(-abulkcv_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(-abulkcv_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(-abulkcv_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(-abulkcv_Vsourceb_sourcep);
#endif
t7=(1.0-abulkcv);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(((coxwl*t7_Vgatep_sourcep)*((0.5*vdseffcv)-t3))+((coxwl*t7)*((0.5*vdseffcv_Vgatep_sourcep)-t3_Vgatep_sourcep)));
qbulk_Vdrainp_sourcep=(((coxwl*t7_Vdrainp_sourcep)*((0.5*vdseffcv)-t3))+((coxwl*t7)*((0.5*vdseffcv_Vdrainp_sourcep)-t3_Vdrainp_sourcep)));
qbulk_Vdrainb_sourcep=(((coxwl*t7_Vdrainb_sourcep)*((0.5*vdseffcv)-t3))+((coxwl*t7)*((0.5*vdseffcv_Vdrainb_sourcep)-t3_Vdrainb_sourcep)));
qbulk_Vbulkp_sourcep=(((coxwl*t7_Vbulkp_sourcep)*((0.5*vdseffcv)-t3))+((coxwl*t7)*((0.5*vdseffcv_Vbulkp_sourcep)-t3_Vbulkp_sourcep)));
qbulk_Vsourceb_sourcep=(((coxwl*t7_Vsourceb_sourcep)*((0.5*vdseffcv)-t3))+((coxwl*t7)*((0.5*vdseffcv_Vsourceb_sourcep)-t3_Vsourceb_sourcep)));
#endif
qbulk=((coxwl*t7)*((0.5*vdseffcv)-t3));
if
((xpart>0.5))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(t1_Vgatep_sourcep+t1_Vgatep_sourcep);
t1_Vdrainp_sourcep=(t1_Vdrainp_sourcep+t1_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(t1_Vdrainb_sourcep+t1_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(t1_Vbulkp_sourcep+t1_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(t1_Vsourceb_sourcep+t1_Vsourceb_sourcep);
#endif
t1=(t1+t1);
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=((-coxwl)*(((0.5*vgsteff_Vgatep_sourcep)+(0.25*t0_Vgatep_sourcep))-((((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*t1-(t0*t0)*t1_Vgatep_sourcep)/t1/t1)));
qsrc_Vdrainp_sourcep=((-coxwl)*(((0.5*vgsteff_Vdrainp_sourcep)+(0.25*t0_Vdrainp_sourcep))-((((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*t1-(t0*t0)*t1_Vdrainp_sourcep)/t1/t1)));
qsrc_Vdrainb_sourcep=((-coxwl)*(((0.5*vgsteff_Vdrainb_sourcep)+(0.25*t0_Vdrainb_sourcep))-((((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*t1-(t0*t0)*t1_Vdrainb_sourcep)/t1/t1)));
qsrc_Vbulkp_sourcep=((-coxwl)*(((0.5*vgsteff_Vbulkp_sourcep)+(0.25*t0_Vbulkp_sourcep))-((((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*t1-(t0*t0)*t1_Vbulkp_sourcep)/t1/t1)));
qsrc_Vsourceb_sourcep=((-coxwl)*(((0.5*vgsteff_Vsourceb_sourcep)+(0.25*t0_Vsourceb_sourcep))-((((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*t1-(t0*t0)*t1_Vsourceb_sourcep)/t1/t1)));
#endif
qsrc=((-coxwl)*(((0.5*vgsteff)+(0.25*t0))-((t0*t0)/t1)));
}
else
{
if
((xpart<0.5))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(t1_Vgatep_sourcep/12.0);
t1_Vdrainp_sourcep=(t1_Vdrainp_sourcep/12.0);
t1_Vdrainb_sourcep=(t1_Vdrainb_sourcep/12.0);
t1_Vbulkp_sourcep=(t1_Vbulkp_sourcep/12.0);
t1_Vsourceb_sourcep=(t1_Vsourceb_sourcep/12.0);
#endif
t1=(t1/12.0);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-(0.5*coxwl)*((t1_Vgatep_sourcep*t1)+(t1*t1_Vgatep_sourcep))/(t1*t1)/(t1*t1));
t2_Vdrainp_sourcep=(-(0.5*coxwl)*((t1_Vdrainp_sourcep*t1)+(t1*t1_Vdrainp_sourcep))/(t1*t1)/(t1*t1));
t2_Vdrainb_sourcep=(-(0.5*coxwl)*((t1_Vdrainb_sourcep*t1)+(t1*t1_Vdrainb_sourcep))/(t1*t1)/(t1*t1));
t2_Vbulkp_sourcep=(-(0.5*coxwl)*((t1_Vbulkp_sourcep*t1)+(t1*t1_Vbulkp_sourcep))/(t1*t1)/(t1*t1));
t2_Vsourceb_sourcep=(-(0.5*coxwl)*((t1_Vsourceb_sourcep*t1)+(t1*t1_Vsourceb_sourcep))/(t1*t1)/(t1*t1));
#endif
t2=((0.5*coxwl)/(t1*t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((vgsteff_Vgatep_sourcep*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))+(vgsteff*(((((2.0*t0_Vgatep_sourcep)*t0)+((2.0*t0)*t0_Vgatep_sourcep))/3.0)+((vgsteff_Vgatep_sourcep*(vgsteff-((4.0*t0)/3.0)))+(vgsteff*(vgsteff_Vgatep_sourcep-((4.0*t0_Vgatep_sourcep)/3.0)))))))-((((((2.0*t0_Vgatep_sourcep)*t0)+((2.0*t0)*t0_Vgatep_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vgatep_sourcep))/15.0));
t3_Vdrainp_sourcep=(((vgsteff_Vdrainp_sourcep*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))+(vgsteff*(((((2.0*t0_Vdrainp_sourcep)*t0)+((2.0*t0)*t0_Vdrainp_sourcep))/3.0)+((vgsteff_Vdrainp_sourcep*(vgsteff-((4.0*t0)/3.0)))+(vgsteff*(vgsteff_Vdrainp_sourcep-((4.0*t0_Vdrainp_sourcep)/3.0)))))))-((((((2.0*t0_Vdrainp_sourcep)*t0)+((2.0*t0)*t0_Vdrainp_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vdrainp_sourcep))/15.0));
t3_Vdrainb_sourcep=(((vgsteff_Vdrainb_sourcep*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))+(vgsteff*(((((2.0*t0_Vdrainb_sourcep)*t0)+((2.0*t0)*t0_Vdrainb_sourcep))/3.0)+((vgsteff_Vdrainb_sourcep*(vgsteff-((4.0*t0)/3.0)))+(vgsteff*(vgsteff_Vdrainb_sourcep-((4.0*t0_Vdrainb_sourcep)/3.0)))))))-((((((2.0*t0_Vdrainb_sourcep)*t0)+((2.0*t0)*t0_Vdrainb_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vdrainb_sourcep))/15.0));
t3_Vbulkp_sourcep=(((vgsteff_Vbulkp_sourcep*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))+(vgsteff*(((((2.0*t0_Vbulkp_sourcep)*t0)+((2.0*t0)*t0_Vbulkp_sourcep))/3.0)+((vgsteff_Vbulkp_sourcep*(vgsteff-((4.0*t0)/3.0)))+(vgsteff*(vgsteff_Vbulkp_sourcep-((4.0*t0_Vbulkp_sourcep)/3.0)))))))-((((((2.0*t0_Vbulkp_sourcep)*t0)+((2.0*t0)*t0_Vbulkp_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vbulkp_sourcep))/15.0));
t3_Vsourceb_sourcep=(((vgsteff_Vsourceb_sourcep*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))+(vgsteff*(((((2.0*t0_Vsourceb_sourcep)*t0)+((2.0*t0)*t0_Vsourceb_sourcep))/3.0)+((vgsteff_Vsourceb_sourcep*(vgsteff-((4.0*t0)/3.0)))+(vgsteff*(vgsteff_Vsourceb_sourcep-((4.0*t0_Vsourceb_sourcep)/3.0)))))))-((((((2.0*t0_Vsourceb_sourcep)*t0)+((2.0*t0)*t0_Vsourceb_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vsourceb_sourcep))/15.0));
#endif
t3=((vgsteff*((((2.0*t0)*t0)/3.0)+(vgsteff*(vgsteff-((4.0*t0)/3.0)))))-((((2.0*t0)*t0)*t0)/15.0));
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=(((-t2_Vgatep_sourcep)*t3)+((-t2)*t3_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=(((-t2_Vdrainp_sourcep)*t3)+((-t2)*t3_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=(((-t2_Vdrainb_sourcep)*t3)+((-t2)*t3_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=(((-t2_Vbulkp_sourcep)*t3)+((-t2)*t3_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=(((-t2_Vsourceb_sourcep)*t3)+((-t2)*t3_Vsourceb_sourcep));
#endif
qsrc=((-t2)*t3);
}
else
{
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=((-0.5)*(qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=((-0.5)*(qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=((-0.5)*(qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=((-0.5)*(qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=((-0.5)*(qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep));
#endif
qsrc=((-0.5)*(qgate+qbulk));
}
}
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=((qgate_Vgatep_sourcep+qac0_Vgatep_sourcep)+qsub0_Vgatep_sourcep);
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+qac0_Vdrainp_sourcep)+qsub0_Vdrainp_sourcep);
qgate_Vdrainb_sourcep=((qgate_Vdrainb_sourcep+qac0_Vdrainb_sourcep)+qsub0_Vdrainb_sourcep);
qgate_Vbulkp_sourcep=((qgate_Vbulkp_sourcep+qac0_Vbulkp_sourcep)+qsub0_Vbulkp_sourcep);
qgate_Vsourceb_sourcep=((qgate_Vsourceb_sourcep+qac0_Vsourceb_sourcep)+qsub0_Vsourceb_sourcep);
#endif
qgate=((qgate+qac0)+qsub0);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(qbulk_Vgatep_sourcep-(qac0_Vgatep_sourcep+qsub0_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep-(qac0_Vdrainp_sourcep+qsub0_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(qbulk_Vdrainb_sourcep-(qac0_Vdrainb_sourcep+qsub0_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-(qac0_Vbulkp_sourcep+qsub0_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(qbulk_Vsourceb_sourcep-(qac0_Vsourceb_sourcep+qsub0_Vsourceb_sourcep));
#endif
qbulk=(qbulk-(qac0+qsub0));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep)+qsrc_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep)+qsrc_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep)+qsrc_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep)+qsrc_Vsourceb_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
}
else
{
if
((capmod==2))
{
#if defined(_DERIVATE)
v3_Vgatep_sourcep=(-Vgs_eff_Vgatep_sourcep);
v3_Vdrainp_sourcep=((-Vgs_eff_Vdrainp_sourcep)+vbseffcv_Vdrainp_sourcep);
v3_Vdrainb_sourcep=((-Vgs_eff_Vdrainb_sourcep)+vbseffcv_Vdrainb_sourcep);
v3_Vbulkp_sourcep=((-Vgs_eff_Vbulkp_sourcep)+vbseffcv_Vbulkp_sourcep);
v3_Vsourceb_sourcep=((-Vgs_eff_Vsourceb_sourcep)+vbseffcv_Vsourceb_sourcep);
#endif
v3=(((vfbzb-Vgs_eff)+vbseffcv)-0.02);
if
((vfbzb<=0.0))
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)-((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
else
{
{
double m00_sqrt(d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)+((4.0*0.02)*vfbzb)))
#endif
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
t0_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
t0_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
t0_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
t0_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
t0=d00_sqrt0;
}
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(0.5*((v3_Vgatep_sourcep*t0-v3*t0_Vgatep_sourcep)/t0/t0));
t1_Vdrainp_sourcep=(0.5*((v3_Vdrainp_sourcep*t0-v3*t0_Vdrainp_sourcep)/t0/t0));
t1_Vdrainb_sourcep=(0.5*((v3_Vdrainb_sourcep*t0-v3*t0_Vdrainb_sourcep)/t0/t0));
t1_Vbulkp_sourcep=(0.5*((v3_Vbulkp_sourcep*t0-v3*t0_Vbulkp_sourcep)/t0/t0));
t1_Vsourceb_sourcep=(0.5*((v3_Vsourceb_sourcep*t0-v3*t0_Vsourceb_sourcep)/t0/t0));
#endif
t1=(0.5*(1.0+(v3/t0)));
#if defined(_DERIVATE)
vfbeff_Vgatep_sourcep=(-(0.5*(v3_Vgatep_sourcep+t0_Vgatep_sourcep)));
vfbeff_Vdrainp_sourcep=(-(0.5*(v3_Vdrainp_sourcep+t0_Vdrainp_sourcep)));
vfbeff_Vdrainb_sourcep=(-(0.5*(v3_Vdrainb_sourcep+t0_Vdrainb_sourcep)));
vfbeff_Vbulkp_sourcep=(-(0.5*(v3_Vbulkp_sourcep+t0_Vbulkp_sourcep)));
vfbeff_Vsourceb_sourcep=(-(0.5*(v3_Vsourceb_sourcep+t0_Vsourceb_sourcep)));
#endif
vfbeff=(vfbzb-(0.5*(v3+t0)));
tox=(1.0e8*toxp);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(Vgs_eff_Vgatep_sourcep/tox);
t0_Vdrainp_sourcep=((Vgs_eff_Vdrainp_sourcep-vbseffcv_Vdrainp_sourcep)/tox);
t0_Vdrainb_sourcep=((Vgs_eff_Vdrainb_sourcep-vbseffcv_Vdrainb_sourcep)/tox);
t0_Vbulkp_sourcep=((Vgs_eff_Vbulkp_sourcep-vbseffcv_Vbulkp_sourcep)/tox);
t0_Vsourceb_sourcep=((Vgs_eff_Vsourceb_sourcep-vbseffcv_Vsourceb_sourcep)/tox);
#endif
t0=(((Vgs_eff-vbseffcv)-vfbzb)/tox);
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=t0_Vgatep_sourcep*acde;
tmp_Vdrainp_sourcep=t0_Vdrainp_sourcep*acde;
tmp_Vdrainb_sourcep=t0_Vdrainb_sourcep*acde;
tmp_Vbulkp_sourcep=t0_Vbulkp_sourcep*acde;
tmp_Vsourceb_sourcep=t0_Vsourceb_sourcep*acde;
#endif
tmp=(t0*acde);
if
((((-34.0)<tmp)&&(tmp<34.0)))
{
{
double m00_exp(d00_exp0,tmp)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=(ldeb*tmp_Vgatep_sourcep*d10_exp0);
tcen_Vdrainp_sourcep=(ldeb*tmp_Vdrainp_sourcep*d10_exp0);
tcen_Vdrainb_sourcep=(ldeb*tmp_Vdrainb_sourcep*d10_exp0);
tcen_Vbulkp_sourcep=(ldeb*tmp_Vbulkp_sourcep*d10_exp0);
tcen_Vsourceb_sourcep=(ldeb*tmp_Vsourceb_sourcep*d10_exp0);
#endif
tcen=(ldeb*d00_exp0);
}
}
else
{
if
((tmp<=(-34.0)))
{
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=0.0;
tcen_Vdrainp_sourcep=0.0;
tcen_Vdrainb_sourcep=0.0;
tcen_Vbulkp_sourcep=0.0;
tcen_Vsourceb_sourcep=0.0;
#endif
tcen=(ldeb*1.713908431e-15);
}
else
{
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=0.0;
tcen_Vdrainp_sourcep=0.0;
tcen_Vdrainb_sourcep=0.0;
tcen_Vbulkp_sourcep=0.0;
tcen_Vsourceb_sourcep=0.0;
#endif
tcen=(ldeb*5.834617425e14);
}
}
link=(1.0e-3*toxp);
#if defined(_DERIVATE)
v3_Vgatep_sourcep=(-tcen_Vgatep_sourcep);
v3_Vdrainp_sourcep=(-tcen_Vdrainp_sourcep);
v3_Vdrainb_sourcep=(-tcen_Vdrainb_sourcep);
v3_Vbulkp_sourcep=(-tcen_Vbulkp_sourcep);
v3_Vsourceb_sourcep=(-tcen_Vsourceb_sourcep);
#endif
v3=((ldeb-tcen)-link);
{
double m00_sqrt(d00_sqrt0,((v3*v3)+((4.0*link)*ldeb)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((v3*v3)+((4.0*link)*ldeb)))
#endif
#if defined(_DERIVATE)
v4_Vgatep_sourcep=((v3_Vgatep_sourcep*v3)+(v3*v3_Vgatep_sourcep))*d10_sqrt0;
v4_Vdrainp_sourcep=((v3_Vdrainp_sourcep*v3)+(v3*v3_Vdrainp_sourcep))*d10_sqrt0;
v4_Vdrainb_sourcep=((v3_Vdrainb_sourcep*v3)+(v3*v3_Vdrainb_sourcep))*d10_sqrt0;
v4_Vbulkp_sourcep=((v3_Vbulkp_sourcep*v3)+(v3*v3_Vbulkp_sourcep))*d10_sqrt0;
v4_Vsourceb_sourcep=((v3_Vsourceb_sourcep*v3)+(v3*v3_Vsourceb_sourcep))*d10_sqrt0;
#endif
v4=d00_sqrt0;
}
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=(-(0.5*(v3_Vgatep_sourcep+v4_Vgatep_sourcep)));
tcen_Vdrainp_sourcep=(-(0.5*(v3_Vdrainp_sourcep+v4_Vdrainp_sourcep)));
tcen_Vdrainb_sourcep=(-(0.5*(v3_Vdrainb_sourcep+v4_Vdrainb_sourcep)));
tcen_Vbulkp_sourcep=(-(0.5*(v3_Vbulkp_sourcep+v4_Vbulkp_sourcep)));
tcen_Vsourceb_sourcep=(-(0.5*(v3_Vsourceb_sourcep+v4_Vsourceb_sourcep)));
#endif
tcen=(ldeb-(0.5*(v3+v4)));
#if defined(_DERIVATE)
ccen_Vgatep_sourcep=(-1.03594e-10*tcen_Vgatep_sourcep/tcen/tcen);
ccen_Vdrainp_sourcep=(-1.03594e-10*tcen_Vdrainp_sourcep/tcen/tcen);
ccen_Vdrainb_sourcep=(-1.03594e-10*tcen_Vdrainb_sourcep/tcen/tcen);
ccen_Vbulkp_sourcep=(-1.03594e-10*tcen_Vbulkp_sourcep/tcen/tcen);
ccen_Vsourceb_sourcep=(-1.03594e-10*tcen_Vsourceb_sourcep/tcen/tcen);
#endif
ccen=(1.03594e-10/tcen);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(-coxp*ccen_Vgatep_sourcep/(coxp+ccen)/(coxp+ccen));
t2_Vdrainp_sourcep=(-coxp*ccen_Vdrainp_sourcep/(coxp+ccen)/(coxp+ccen));
t2_Vdrainb_sourcep=(-coxp*ccen_Vdrainb_sourcep/(coxp+ccen)/(coxp+ccen));
t2_Vbulkp_sourcep=(-coxp*ccen_Vbulkp_sourcep/(coxp+ccen)/(coxp+ccen));
t2_Vsourceb_sourcep=(-coxp*ccen_Vsourceb_sourcep/(coxp+ccen)/(coxp+ccen));
#endif
t2=(coxp/(coxp+ccen));
#if defined(_DERIVATE)
coxeff_Vgatep_sourcep=((t2_Vgatep_sourcep*ccen)+(t2*ccen_Vgatep_sourcep));
coxeff_Vdrainp_sourcep=((t2_Vdrainp_sourcep*ccen)+(t2*ccen_Vdrainp_sourcep));
coxeff_Vdrainb_sourcep=((t2_Vdrainb_sourcep*ccen)+(t2*ccen_Vdrainb_sourcep));
coxeff_Vbulkp_sourcep=((t2_Vbulkp_sourcep*ccen)+(t2*ccen_Vbulkp_sourcep));
coxeff_Vsourceb_sourcep=((t2_Vsourceb_sourcep*ccen)+(t2*ccen_Vsourceb_sourcep));
#endif
coxeff=(t2*ccen);
#if defined(_DERIVATE)
coxwlcen_Vgatep_sourcep=((coxwl*coxeff_Vgatep_sourcep)/coxe);
coxwlcen_Vdrainp_sourcep=((coxwl*coxeff_Vdrainp_sourcep)/coxe);
coxwlcen_Vdrainb_sourcep=((coxwl*coxeff_Vdrainb_sourcep)/coxe);
coxwlcen_Vbulkp_sourcep=((coxwl*coxeff_Vbulkp_sourcep)/coxe);
coxwlcen_Vsourceb_sourcep=((coxwl*coxeff_Vsourceb_sourcep)/coxe);
#endif
coxwlcen=((coxwl*coxeff)/coxe);
#if defined(_DERIVATE)
qac0_Vgatep_sourcep=((coxwlcen_Vgatep_sourcep*(vfbeff-vfbzb))+(coxwlcen*vfbeff_Vgatep_sourcep));
qac0_Vdrainp_sourcep=((coxwlcen_Vdrainp_sourcep*(vfbeff-vfbzb))+(coxwlcen*vfbeff_Vdrainp_sourcep));
qac0_Vdrainb_sourcep=((coxwlcen_Vdrainb_sourcep*(vfbeff-vfbzb))+(coxwlcen*vfbeff_Vdrainb_sourcep));
qac0_Vbulkp_sourcep=((coxwlcen_Vbulkp_sourcep*(vfbeff-vfbzb))+(coxwlcen*vfbeff_Vbulkp_sourcep));
qac0_Vsourceb_sourcep=((coxwlcen_Vsourceb_sourcep*(vfbeff-vfbzb))+(coxwlcen*vfbeff_Vsourceb_sourcep));
#endif
qac0=(coxwlcen*(vfbeff-vfbzb));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(0.5*k1ox);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((Vgs_eff_Vgatep_sourcep-vfbeff_Vgatep_sourcep)-vgsteff_Vgatep_sourcep);
t3_Vdrainp_sourcep=(((Vgs_eff_Vdrainp_sourcep-vfbeff_Vdrainp_sourcep)-vbseffcv_Vdrainp_sourcep)-vgsteff_Vdrainp_sourcep);
t3_Vdrainb_sourcep=(((Vgs_eff_Vdrainb_sourcep-vfbeff_Vdrainb_sourcep)-vbseffcv_Vdrainb_sourcep)-vgsteff_Vdrainb_sourcep);
t3_Vbulkp_sourcep=(((Vgs_eff_Vbulkp_sourcep-vfbeff_Vbulkp_sourcep)-vbseffcv_Vbulkp_sourcep)-vgsteff_Vbulkp_sourcep);
t3_Vsourceb_sourcep=(((Vgs_eff_Vsourceb_sourcep-vfbeff_Vsourceb_sourcep)-vbseffcv_Vsourceb_sourcep)-vgsteff_Vsourceb_sourcep);
#endif
t3=(((Vgs_eff-vfbeff)-vbseffcv)-vgsteff);
if
((k1ox==0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=0.0;
t1_Vsourceb_sourcep=0.0;
#endif
t1=0.0;
#if defined(_DERIVATE)
t2_Vgatep_sourcep=0.0;
t2_Vdrainp_sourcep=0.0;
t2_Vdrainb_sourcep=0.0;
t2_Vbulkp_sourcep=0.0;
t2_Vsourceb_sourcep=0.0;
#endif
t2=0.0;
}
else
{
if
((t3<0.0))
{
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(t0_Vgatep_sourcep+(t3_Vgatep_sourcep/k1ox));
t1_Vdrainp_sourcep=(t0_Vdrainp_sourcep+(t3_Vdrainp_sourcep/k1ox));
t1_Vdrainb_sourcep=(t0_Vdrainb_sourcep+(t3_Vdrainb_sourcep/k1ox));
t1_Vbulkp_sourcep=(t0_Vbulkp_sourcep+(t3_Vbulkp_sourcep/k1ox));
t1_Vsourceb_sourcep=(t0_Vsourceb_sourcep+(t3_Vsourceb_sourcep/k1ox));
#endif
t1=(t0+(t3/k1ox));
#if defined(_DERIVATE)
t2_Vgatep_sourcep=coxwlcen_Vgatep_sourcep;
t2_Vdrainp_sourcep=coxwlcen_Vdrainp_sourcep;
t2_Vdrainb_sourcep=coxwlcen_Vdrainb_sourcep;
t2_Vbulkp_sourcep=coxwlcen_Vbulkp_sourcep;
t2_Vsourceb_sourcep=coxwlcen_Vsourceb_sourcep;
#endif
t2=coxwlcen;
}
else
{
{
double m00_sqrt(d00_sqrt0,((t0*t0)+t3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+t3))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))+t3_Vgatep_sourcep)*d10_sqrt0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+t3_Vdrainp_sourcep)*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+t3_Vdrainb_sourcep)*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+t3_Vbulkp_sourcep)*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+t3_Vsourceb_sourcep)*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=((((coxwlcen_Vgatep_sourcep*t0)+(coxwlcen*t0_Vgatep_sourcep))*t1-(coxwlcen*t0)*t1_Vgatep_sourcep)/t1/t1);
t2_Vdrainp_sourcep=((((coxwlcen_Vdrainp_sourcep*t0)+(coxwlcen*t0_Vdrainp_sourcep))*t1-(coxwlcen*t0)*t1_Vdrainp_sourcep)/t1/t1);
t2_Vdrainb_sourcep=((((coxwlcen_Vdrainb_sourcep*t0)+(coxwlcen*t0_Vdrainb_sourcep))*t1-(coxwlcen*t0)*t1_Vdrainb_sourcep)/t1/t1);
t2_Vbulkp_sourcep=((((coxwlcen_Vbulkp_sourcep*t0)+(coxwlcen*t0_Vbulkp_sourcep))*t1-(coxwlcen*t0)*t1_Vbulkp_sourcep)/t1/t1);
t2_Vsourceb_sourcep=((((coxwlcen_Vsourceb_sourcep*t0)+(coxwlcen*t0_Vsourceb_sourcep))*t1-(coxwlcen*t0)*t1_Vsourceb_sourcep)/t1/t1);
#endif
t2=((coxwlcen*t0)/t1);
}
}
#if defined(_DERIVATE)
qsub0_Vgatep_sourcep=((coxwlcen_Vgatep_sourcep*k1ox*(t1-t0))+((coxwlcen*k1ox)*(t1_Vgatep_sourcep-t0_Vgatep_sourcep)));
qsub0_Vdrainp_sourcep=((coxwlcen_Vdrainp_sourcep*k1ox*(t1-t0))+((coxwlcen*k1ox)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep)));
qsub0_Vdrainb_sourcep=((coxwlcen_Vdrainb_sourcep*k1ox*(t1-t0))+((coxwlcen*k1ox)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep)));
qsub0_Vbulkp_sourcep=((coxwlcen_Vbulkp_sourcep*k1ox*(t1-t0))+((coxwlcen*k1ox)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep)));
qsub0_Vsourceb_sourcep=((coxwlcen_Vsourceb_sourcep*k1ox*(t1-t0))+((coxwlcen*k1ox)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep)));
#endif
qsub0=((coxwlcen*k1ox)*(t1-t0));
if
((k1ox<=0.0))
{
#if defined(_DERIVATE)
denomi_Vgatep_sourcep=0.0;
denomi_Vdrainp_sourcep=0.0;
denomi_Vdrainb_sourcep=0.0;
denomi_Vbulkp_sourcep=0.0;
denomi_Vsourceb_sourcep=0.0;
#endif
denomi=((0.25*moin)*vtm);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(0.5*sqrtphi);
}
else
{
#if defined(_DERIVATE)
denomi_Vgatep_sourcep=0.0;
denomi_Vdrainp_sourcep=0.0;
denomi_Vdrainb_sourcep=0.0;
denomi_Vbulkp_sourcep=0.0;
denomi_Vsourceb_sourcep=0.0;
#endif
denomi=(((moin*vtm)*k1ox)*k1ox);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(k1ox*sqrtphi);
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((2.0*t0_Vgatep_sourcep)+vgsteff_Vgatep_sourcep);
t1_Vdrainp_sourcep=((2.0*t0_Vdrainp_sourcep)+vgsteff_Vdrainp_sourcep);
t1_Vdrainb_sourcep=((2.0*t0_Vdrainb_sourcep)+vgsteff_Vdrainb_sourcep);
t1_Vbulkp_sourcep=((2.0*t0_Vbulkp_sourcep)+vgsteff_Vbulkp_sourcep);
t1_Vsourceb_sourcep=((2.0*t0_Vsourceb_sourcep)+vgsteff_Vsourceb_sourcep);
#endif
t1=((2.0*t0)+vgsteff);
{
double m00_logE(d00_logE0,(1.0+((t1*vgsteff)/denomi)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+((t1*vgsteff)/denomi)))
#endif
#if defined(_DERIVATE)
deltaphi_Vgatep_sourcep=(vtm*((((t1_Vgatep_sourcep*vgsteff)+(t1*vgsteff_Vgatep_sourcep))*denomi-(t1*vgsteff)*denomi_Vgatep_sourcep)/denomi/denomi)*d10_logE0);
deltaphi_Vdrainp_sourcep=(vtm*((((t1_Vdrainp_sourcep*vgsteff)+(t1*vgsteff_Vdrainp_sourcep))*denomi-(t1*vgsteff)*denomi_Vdrainp_sourcep)/denomi/denomi)*d10_logE0);
deltaphi_Vdrainb_sourcep=(vtm*((((t1_Vdrainb_sourcep*vgsteff)+(t1*vgsteff_Vdrainb_sourcep))*denomi-(t1*vgsteff)*denomi_Vdrainb_sourcep)/denomi/denomi)*d10_logE0);
deltaphi_Vbulkp_sourcep=(vtm*((((t1_Vbulkp_sourcep*vgsteff)+(t1*vgsteff_Vbulkp_sourcep))*denomi-(t1*vgsteff)*denomi_Vbulkp_sourcep)/denomi/denomi)*d10_logE0);
deltaphi_Vsourceb_sourcep=(vtm*((((t1_Vsourceb_sourcep*vgsteff)+(t1*vgsteff_Vsourceb_sourcep))*denomi-(t1*vgsteff)*denomi_Vsourceb_sourcep)/denomi/denomi)*d10_logE0);
#endif
deltaphi=(vtm*d00_logE0);
}
tox=(tox+tox);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(vgsteff_Vgatep_sourcep/tox);
t0_Vdrainp_sourcep=(vgsteff_Vdrainp_sourcep/tox);
t0_Vdrainb_sourcep=(vgsteff_Vdrainb_sourcep/tox);
t0_Vbulkp_sourcep=(vgsteff_Vbulkp_sourcep/tox);
t0_Vsourceb_sourcep=(vgsteff_Vsourceb_sourcep/tox);
#endif
t0=((vgsteff+vtfbphi2)/tox);
{
double m00_logE(d00_logE0,t0)
double m00_exp(d00_exp1,(0.7*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,t0)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
tmp_Vgatep_sourcep=(0.7*t0_Vgatep_sourcep*d10_logE0)*d10_exp1;
tmp_Vdrainp_sourcep=(0.7*t0_Vdrainp_sourcep*d10_logE0)*d10_exp1;
tmp_Vdrainb_sourcep=(0.7*t0_Vdrainb_sourcep*d10_logE0)*d10_exp1;
tmp_Vbulkp_sourcep=(0.7*t0_Vbulkp_sourcep*d10_logE0)*d10_exp1;
tmp_Vsourceb_sourcep=(0.7*t0_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
tmp=d00_exp1;
}
#if defined(_DERIVATE)
t1_Vgatep_sourcep=tmp_Vgatep_sourcep;
t1_Vdrainp_sourcep=tmp_Vdrainp_sourcep;
t1_Vdrainb_sourcep=tmp_Vdrainb_sourcep;
t1_Vbulkp_sourcep=tmp_Vbulkp_sourcep;
t1_Vsourceb_sourcep=tmp_Vsourceb_sourcep;
#endif
t1=(1.0+tmp);
#if defined(_DERIVATE)
tcen_Vgatep_sourcep=(-1.9e-9*t1_Vgatep_sourcep/t1/t1);
tcen_Vdrainp_sourcep=(-1.9e-9*t1_Vdrainp_sourcep/t1/t1);
tcen_Vdrainb_sourcep=(-1.9e-9*t1_Vdrainb_sourcep/t1/t1);
tcen_Vbulkp_sourcep=(-1.9e-9*t1_Vbulkp_sourcep/t1/t1);
tcen_Vsourceb_sourcep=(-1.9e-9*t1_Vsourceb_sourcep/t1/t1);
#endif
tcen=(1.9e-9/t1);
#if defined(_DERIVATE)
ccen_Vgatep_sourcep=(-1.03594e-10*tcen_Vgatep_sourcep/tcen/tcen);
ccen_Vdrainp_sourcep=(-1.03594e-10*tcen_Vdrainp_sourcep/tcen/tcen);
ccen_Vdrainb_sourcep=(-1.03594e-10*tcen_Vdrainb_sourcep/tcen/tcen);
ccen_Vbulkp_sourcep=(-1.03594e-10*tcen_Vbulkp_sourcep/tcen/tcen);
ccen_Vsourceb_sourcep=(-1.03594e-10*tcen_Vsourceb_sourcep/tcen/tcen);
#endif
ccen=(1.03594e-10/tcen);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=(-coxp*ccen_Vgatep_sourcep/(coxp+ccen)/(coxp+ccen));
t0_Vdrainp_sourcep=(-coxp*ccen_Vdrainp_sourcep/(coxp+ccen)/(coxp+ccen));
t0_Vdrainb_sourcep=(-coxp*ccen_Vdrainb_sourcep/(coxp+ccen)/(coxp+ccen));
t0_Vbulkp_sourcep=(-coxp*ccen_Vbulkp_sourcep/(coxp+ccen)/(coxp+ccen));
t0_Vsourceb_sourcep=(-coxp*ccen_Vsourceb_sourcep/(coxp+ccen)/(coxp+ccen));
#endif
t0=(coxp/(coxp+ccen));
#if defined(_DERIVATE)
coxeff_Vgatep_sourcep=((t0_Vgatep_sourcep*ccen)+(t0*ccen_Vgatep_sourcep));
coxeff_Vdrainp_sourcep=((t0_Vdrainp_sourcep*ccen)+(t0*ccen_Vdrainp_sourcep));
coxeff_Vdrainb_sourcep=((t0_Vdrainb_sourcep*ccen)+(t0*ccen_Vdrainb_sourcep));
coxeff_Vbulkp_sourcep=((t0_Vbulkp_sourcep*ccen)+(t0*ccen_Vbulkp_sourcep));
coxeff_Vsourceb_sourcep=((t0_Vsourceb_sourcep*ccen)+(t0*ccen_Vsourceb_sourcep));
#endif
coxeff=(t0*ccen);
#if defined(_DERIVATE)
coxwlcen_Vgatep_sourcep=((coxwl*coxeff_Vgatep_sourcep)/coxe);
coxwlcen_Vdrainp_sourcep=((coxwl*coxeff_Vdrainp_sourcep)/coxe);
coxwlcen_Vdrainb_sourcep=((coxwl*coxeff_Vdrainb_sourcep)/coxe);
coxwlcen_Vbulkp_sourcep=((coxwl*coxeff_Vbulkp_sourcep)/coxe);
coxwlcen_Vsourceb_sourcep=((coxwl*coxeff_Vsourceb_sourcep)/coxe);
#endif
coxwlcen=((coxwl*coxeff)/coxe);
#if defined(_DERIVATE)
abulkcv_Vgatep_sourcep=abulk0_Vgatep_sourcep*abulkcvfactor;
abulkcv_Vdrainp_sourcep=abulk0_Vdrainp_sourcep*abulkcvfactor;
abulkcv_Vdrainb_sourcep=abulk0_Vdrainb_sourcep*abulkcvfactor;
abulkcv_Vbulkp_sourcep=abulk0_Vbulkp_sourcep*abulkcvfactor;
abulkcv_Vsourceb_sourcep=abulk0_Vsourceb_sourcep*abulkcvfactor;
#endif
abulkcv=(abulk0*abulkcvfactor);
#if defined(_DERIVATE)
vdsatcv_Vgatep_sourcep=(((vgsteff_Vgatep_sourcep-deltaphi_Vgatep_sourcep)*abulkcv-(vgsteff-deltaphi)*abulkcv_Vgatep_sourcep)/abulkcv/abulkcv);
vdsatcv_Vdrainp_sourcep=(((vgsteff_Vdrainp_sourcep-deltaphi_Vdrainp_sourcep)*abulkcv-(vgsteff-deltaphi)*abulkcv_Vdrainp_sourcep)/abulkcv/abulkcv);
vdsatcv_Vdrainb_sourcep=(((vgsteff_Vdrainb_sourcep-deltaphi_Vdrainb_sourcep)*abulkcv-(vgsteff-deltaphi)*abulkcv_Vdrainb_sourcep)/abulkcv/abulkcv);
vdsatcv_Vbulkp_sourcep=(((vgsteff_Vbulkp_sourcep-deltaphi_Vbulkp_sourcep)*abulkcv-(vgsteff-deltaphi)*abulkcv_Vbulkp_sourcep)/abulkcv/abulkcv);
vdsatcv_Vsourceb_sourcep=(((vgsteff_Vsourceb_sourcep-deltaphi_Vsourceb_sourcep)*abulkcv-(vgsteff-deltaphi)*abulkcv_Vsourceb_sourcep)/abulkcv/abulkcv);
#endif
vdsatcv=((vgsteff-deltaphi)/abulkcv);
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vdsatcv_Vgatep_sourcep;
t0_Vdrainp_sourcep=(vdsatcv_Vdrainp_sourcep-Vds_Vdrainp_sourcep);
t0_Vdrainb_sourcep=vdsatcv_Vdrainb_sourcep;
t0_Vbulkp_sourcep=vdsatcv_Vbulkp_sourcep;
t0_Vsourceb_sourcep=vdsatcv_Vsourceb_sourcep;
#endif
t0=((vdsatcv-Vds)-0.02);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+((4.0*0.02)*vdsatcv)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+((4.0*0.02)*vdsatcv)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))+((4.0*0.02)*vdsatcv_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=(((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))+((4.0*0.02)*vdsatcv_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=(((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))+((4.0*0.02)*vdsatcv_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=(((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))+((4.0*0.02)*vdsatcv_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=(((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))+((4.0*0.02)*vdsatcv_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
if
((t0>=0.0))
{
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=(vdsatcv_Vgatep_sourcep-(0.5*(t0_Vgatep_sourcep+t1_Vgatep_sourcep)));
vdseffcv_Vdrainp_sourcep=(vdsatcv_Vdrainp_sourcep-(0.5*(t0_Vdrainp_sourcep+t1_Vdrainp_sourcep)));
vdseffcv_Vdrainb_sourcep=(vdsatcv_Vdrainb_sourcep-(0.5*(t0_Vdrainb_sourcep+t1_Vdrainb_sourcep)));
vdseffcv_Vbulkp_sourcep=(vdsatcv_Vbulkp_sourcep-(0.5*(t0_Vbulkp_sourcep+t1_Vbulkp_sourcep)));
vdseffcv_Vsourceb_sourcep=(vdsatcv_Vsourceb_sourcep-(0.5*(t0_Vsourceb_sourcep+t1_Vsourceb_sourcep)));
#endif
vdseffcv=(vdsatcv-(0.5*(t0+t1)));
}
else
{
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(-(0.02+0.02)*(t1_Vgatep_sourcep-t0_Vgatep_sourcep)/(t1-t0)/(t1-t0));
t3_Vdrainp_sourcep=(-(0.02+0.02)*(t1_Vdrainp_sourcep-t0_Vdrainp_sourcep)/(t1-t0)/(t1-t0));
t3_Vdrainb_sourcep=(-(0.02+0.02)*(t1_Vdrainb_sourcep-t0_Vdrainb_sourcep)/(t1-t0)/(t1-t0));
t3_Vbulkp_sourcep=(-(0.02+0.02)*(t1_Vbulkp_sourcep-t0_Vbulkp_sourcep)/(t1-t0)/(t1-t0));
t3_Vsourceb_sourcep=(-(0.02+0.02)*(t1_Vsourceb_sourcep-t0_Vsourceb_sourcep)/(t1-t0)/(t1-t0));
#endif
t3=((0.02+0.02)/(t1-t0));
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-t3_Vgatep_sourcep);
t4_Vdrainp_sourcep=(-t3_Vdrainp_sourcep);
t4_Vdrainb_sourcep=(-t3_Vdrainb_sourcep);
t4_Vbulkp_sourcep=(-t3_Vbulkp_sourcep);
t4_Vsourceb_sourcep=(-t3_Vsourceb_sourcep);
#endif
t4=(1.0-t3);
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=((vdsatcv_Vgatep_sourcep*t4)+(vdsatcv*t4_Vgatep_sourcep));
vdseffcv_Vdrainp_sourcep=((vdsatcv_Vdrainp_sourcep*t4)+(vdsatcv*t4_Vdrainp_sourcep));
vdseffcv_Vdrainb_sourcep=((vdsatcv_Vdrainb_sourcep*t4)+(vdsatcv*t4_Vdrainb_sourcep));
vdseffcv_Vbulkp_sourcep=((vdsatcv_Vbulkp_sourcep*t4)+(vdsatcv*t4_Vbulkp_sourcep));
vdseffcv_Vsourceb_sourcep=((vdsatcv_Vsourceb_sourcep*t4)+(vdsatcv*t4_Vsourceb_sourcep));
#endif
vdseffcv=(vdsatcv*t4);
}
if
((Vds==0.0))
{
#if defined(_DERIVATE)
vdseffcv_Vgatep_sourcep=0.0;
vdseffcv_Vdrainp_sourcep=0.0;
vdseffcv_Vdrainb_sourcep=0.0;
vdseffcv_Vbulkp_sourcep=0.0;
vdseffcv_Vsourceb_sourcep=0.0;
#endif
vdseffcv=0.0;
}
#if defined(_DERIVATE)
t0_Vgatep_sourcep=((abulkcv_Vgatep_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vgatep_sourcep));
t0_Vdrainp_sourcep=((abulkcv_Vdrainp_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vdrainp_sourcep));
t0_Vdrainb_sourcep=((abulkcv_Vdrainb_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vdrainb_sourcep));
t0_Vbulkp_sourcep=((abulkcv_Vbulkp_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vbulkp_sourcep));
t0_Vsourceb_sourcep=((abulkcv_Vsourceb_sourcep*vdseffcv)+(abulkcv*vdseffcv_Vsourceb_sourcep));
#endif
t0=(abulkcv*vdseffcv);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=(vgsteff_Vgatep_sourcep-deltaphi_Vgatep_sourcep);
t1_Vdrainp_sourcep=(vgsteff_Vdrainp_sourcep-deltaphi_Vdrainp_sourcep);
t1_Vdrainb_sourcep=(vgsteff_Vdrainb_sourcep-deltaphi_Vdrainb_sourcep);
t1_Vbulkp_sourcep=(vgsteff_Vbulkp_sourcep-deltaphi_Vbulkp_sourcep);
t1_Vsourceb_sourcep=(vgsteff_Vsourceb_sourcep-deltaphi_Vsourceb_sourcep);
#endif
t1=(vgsteff-deltaphi);
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(12.0*(t1_Vgatep_sourcep-(0.5*t0_Vgatep_sourcep)));
t2_Vdrainp_sourcep=(12.0*(t1_Vdrainp_sourcep-(0.5*t0_Vdrainp_sourcep)));
t2_Vdrainb_sourcep=(12.0*(t1_Vdrainb_sourcep-(0.5*t0_Vdrainb_sourcep)));
t2_Vbulkp_sourcep=(12.0*(t1_Vbulkp_sourcep-(0.5*t0_Vbulkp_sourcep)));
t2_Vsourceb_sourcep=(12.0*(t1_Vsourceb_sourcep-(0.5*t0_Vsourceb_sourcep)));
#endif
t2=(12.0*((t1-(0.5*t0))+1.0e-20));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=((t0_Vgatep_sourcep*t2-t0*t2_Vgatep_sourcep)/t2/t2);
t3_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t2-t0*t2_Vdrainp_sourcep)/t2/t2);
t3_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t2-t0*t2_Vdrainb_sourcep)/t2/t2);
t3_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t2-t0*t2_Vbulkp_sourcep)/t2/t2);
t3_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t2-t0*t2_Vsourceb_sourcep)/t2/t2);
#endif
t3=(t0/t2);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=((coxwlcen_Vgatep_sourcep*(t1-(t0*(0.5-t3))))+(coxwlcen*(t1_Vgatep_sourcep-((t0_Vgatep_sourcep*(0.5-t3))+(t0*(-t3_Vgatep_sourcep))))));
qgate_Vdrainp_sourcep=((coxwlcen_Vdrainp_sourcep*(t1-(t0*(0.5-t3))))+(coxwlcen*(t1_Vdrainp_sourcep-((t0_Vdrainp_sourcep*(0.5-t3))+(t0*(-t3_Vdrainp_sourcep))))));
qgate_Vdrainb_sourcep=((coxwlcen_Vdrainb_sourcep*(t1-(t0*(0.5-t3))))+(coxwlcen*(t1_Vdrainb_sourcep-((t0_Vdrainb_sourcep*(0.5-t3))+(t0*(-t3_Vdrainb_sourcep))))));
qgate_Vbulkp_sourcep=((coxwlcen_Vbulkp_sourcep*(t1-(t0*(0.5-t3))))+(coxwlcen*(t1_Vbulkp_sourcep-((t0_Vbulkp_sourcep*(0.5-t3))+(t0*(-t3_Vbulkp_sourcep))))));
qgate_Vsourceb_sourcep=((coxwlcen_Vsourceb_sourcep*(t1-(t0*(0.5-t3))))+(coxwlcen*(t1_Vsourceb_sourcep-((t0_Vsourceb_sourcep*(0.5-t3))+(t0*(-t3_Vsourceb_sourcep))))));
#endif
qgate=(coxwlcen*(t1-(t0*(0.5-t3))));
#if defined(_DERIVATE)
t7_Vgatep_sourcep=(-abulkcv_Vgatep_sourcep);
t7_Vdrainp_sourcep=(-abulkcv_Vdrainp_sourcep);
t7_Vdrainb_sourcep=(-abulkcv_Vdrainb_sourcep);
t7_Vbulkp_sourcep=(-abulkcv_Vbulkp_sourcep);
t7_Vsourceb_sourcep=(-abulkcv_Vsourceb_sourcep);
#endif
t7=(1.0-abulkcv);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=((((coxwlcen_Vgatep_sourcep*t7)+(coxwlcen*t7_Vgatep_sourcep))*((0.5*vdseffcv)-((t0*vdseffcv)/t2)))+((coxwlcen*t7)*((0.5*vdseffcv_Vgatep_sourcep)-((((t0_Vgatep_sourcep*vdseffcv)+(t0*vdseffcv_Vgatep_sourcep))*t2-(t0*vdseffcv)*t2_Vgatep_sourcep)/t2/t2))));
qbulk_Vdrainp_sourcep=((((coxwlcen_Vdrainp_sourcep*t7)+(coxwlcen*t7_Vdrainp_sourcep))*((0.5*vdseffcv)-((t0*vdseffcv)/t2)))+((coxwlcen*t7)*((0.5*vdseffcv_Vdrainp_sourcep)-((((t0_Vdrainp_sourcep*vdseffcv)+(t0*vdseffcv_Vdrainp_sourcep))*t2-(t0*vdseffcv)*t2_Vdrainp_sourcep)/t2/t2))));
qbulk_Vdrainb_sourcep=((((coxwlcen_Vdrainb_sourcep*t7)+(coxwlcen*t7_Vdrainb_sourcep))*((0.5*vdseffcv)-((t0*vdseffcv)/t2)))+((coxwlcen*t7)*((0.5*vdseffcv_Vdrainb_sourcep)-((((t0_Vdrainb_sourcep*vdseffcv)+(t0*vdseffcv_Vdrainb_sourcep))*t2-(t0*vdseffcv)*t2_Vdrainb_sourcep)/t2/t2))));
qbulk_Vbulkp_sourcep=((((coxwlcen_Vbulkp_sourcep*t7)+(coxwlcen*t7_Vbulkp_sourcep))*((0.5*vdseffcv)-((t0*vdseffcv)/t2)))+((coxwlcen*t7)*((0.5*vdseffcv_Vbulkp_sourcep)-((((t0_Vbulkp_sourcep*vdseffcv)+(t0*vdseffcv_Vbulkp_sourcep))*t2-(t0*vdseffcv)*t2_Vbulkp_sourcep)/t2/t2))));
qbulk_Vsourceb_sourcep=((((coxwlcen_Vsourceb_sourcep*t7)+(coxwlcen*t7_Vsourceb_sourcep))*((0.5*vdseffcv)-((t0*vdseffcv)/t2)))+((coxwlcen*t7)*((0.5*vdseffcv_Vsourceb_sourcep)-((((t0_Vsourceb_sourcep*vdseffcv)+(t0*vdseffcv_Vsourceb_sourcep))*t2-(t0*vdseffcv)*t2_Vsourceb_sourcep)/t2/t2))));
#endif
qbulk=((coxwlcen*t7)*((0.5*vdseffcv)-((t0*vdseffcv)/t2)));
if
((xpart>0.5))
{
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=(((-coxwlcen_Vgatep_sourcep)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)))+((-coxwlcen)*(((t1_Vgatep_sourcep/2.0)+(t0_Vgatep_sourcep/4.0))-(((((0.5*t0_Vgatep_sourcep)*t0)+((0.5*t0)*t0_Vgatep_sourcep))*t2-((0.5*t0)*t0)*t2_Vgatep_sourcep)/t2/t2))));
qsrc_Vdrainp_sourcep=(((-coxwlcen_Vdrainp_sourcep)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)))+((-coxwlcen)*(((t1_Vdrainp_sourcep/2.0)+(t0_Vdrainp_sourcep/4.0))-(((((0.5*t0_Vdrainp_sourcep)*t0)+((0.5*t0)*t0_Vdrainp_sourcep))*t2-((0.5*t0)*t0)*t2_Vdrainp_sourcep)/t2/t2))));
qsrc_Vdrainb_sourcep=(((-coxwlcen_Vdrainb_sourcep)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)))+((-coxwlcen)*(((t1_Vdrainb_sourcep/2.0)+(t0_Vdrainb_sourcep/4.0))-(((((0.5*t0_Vdrainb_sourcep)*t0)+((0.5*t0)*t0_Vdrainb_sourcep))*t2-((0.5*t0)*t0)*t2_Vdrainb_sourcep)/t2/t2))));
qsrc_Vbulkp_sourcep=(((-coxwlcen_Vbulkp_sourcep)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)))+((-coxwlcen)*(((t1_Vbulkp_sourcep/2.0)+(t0_Vbulkp_sourcep/4.0))-(((((0.5*t0_Vbulkp_sourcep)*t0)+((0.5*t0)*t0_Vbulkp_sourcep))*t2-((0.5*t0)*t0)*t2_Vbulkp_sourcep)/t2/t2))));
qsrc_Vsourceb_sourcep=(((-coxwlcen_Vsourceb_sourcep)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)))+((-coxwlcen)*(((t1_Vsourceb_sourcep/2.0)+(t0_Vsourceb_sourcep/4.0))-(((((0.5*t0_Vsourceb_sourcep)*t0)+((0.5*t0)*t0_Vsourceb_sourcep))*t2-((0.5*t0)*t0)*t2_Vsourceb_sourcep)/t2/t2))));
#endif
qsrc=((-coxwlcen)*(((t1/2.0)+(t0/4.0))-(((0.5*t0)*t0)/t2)));
}
else
{
if
((xpart<0.5))
{
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(t2_Vgatep_sourcep/12.0);
t2_Vdrainp_sourcep=(t2_Vdrainp_sourcep/12.0);
t2_Vdrainb_sourcep=(t2_Vdrainb_sourcep/12.0);
t2_Vbulkp_sourcep=(t2_Vbulkp_sourcep/12.0);
t2_Vsourceb_sourcep=(t2_Vsourceb_sourcep/12.0);
#endif
t2=(t2/12.0);
#if defined(_DERIVATE)
t3_Vgatep_sourcep=(((0.5*coxwlcen_Vgatep_sourcep)*(t2*t2)-(0.5*coxwlcen)*((t2_Vgatep_sourcep*t2)+(t2*t2_Vgatep_sourcep)))/(t2*t2)/(t2*t2));
t3_Vdrainp_sourcep=(((0.5*coxwlcen_Vdrainp_sourcep)*(t2*t2)-(0.5*coxwlcen)*((t2_Vdrainp_sourcep*t2)+(t2*t2_Vdrainp_sourcep)))/(t2*t2)/(t2*t2));
t3_Vdrainb_sourcep=(((0.5*coxwlcen_Vdrainb_sourcep)*(t2*t2)-(0.5*coxwlcen)*((t2_Vdrainb_sourcep*t2)+(t2*t2_Vdrainb_sourcep)))/(t2*t2)/(t2*t2));
t3_Vbulkp_sourcep=(((0.5*coxwlcen_Vbulkp_sourcep)*(t2*t2)-(0.5*coxwlcen)*((t2_Vbulkp_sourcep*t2)+(t2*t2_Vbulkp_sourcep)))/(t2*t2)/(t2*t2));
t3_Vsourceb_sourcep=(((0.5*coxwlcen_Vsourceb_sourcep)*(t2*t2)-(0.5*coxwlcen)*((t2_Vsourceb_sourcep*t2)+(t2*t2_Vsourceb_sourcep)))/(t2*t2)/(t2*t2));
#endif
t3=((0.5*coxwlcen)/(t2*t2));
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(((t1_Vgatep_sourcep*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))+(t1*(((((2.0*t0_Vgatep_sourcep)*t0)+((2.0*t0)*t0_Vgatep_sourcep))/3.0)+((t1_Vgatep_sourcep*(t1-((4.0*t0)/3.0)))+(t1*(t1_Vgatep_sourcep-((4.0*t0_Vgatep_sourcep)/3.0)))))))-((((((2.0*t0_Vgatep_sourcep)*t0)+((2.0*t0)*t0_Vgatep_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vgatep_sourcep))/15.0));
t4_Vdrainp_sourcep=(((t1_Vdrainp_sourcep*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))+(t1*(((((2.0*t0_Vdrainp_sourcep)*t0)+((2.0*t0)*t0_Vdrainp_sourcep))/3.0)+((t1_Vdrainp_sourcep*(t1-((4.0*t0)/3.0)))+(t1*(t1_Vdrainp_sourcep-((4.0*t0_Vdrainp_sourcep)/3.0)))))))-((((((2.0*t0_Vdrainp_sourcep)*t0)+((2.0*t0)*t0_Vdrainp_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vdrainp_sourcep))/15.0));
t4_Vdrainb_sourcep=(((t1_Vdrainb_sourcep*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))+(t1*(((((2.0*t0_Vdrainb_sourcep)*t0)+((2.0*t0)*t0_Vdrainb_sourcep))/3.0)+((t1_Vdrainb_sourcep*(t1-((4.0*t0)/3.0)))+(t1*(t1_Vdrainb_sourcep-((4.0*t0_Vdrainb_sourcep)/3.0)))))))-((((((2.0*t0_Vdrainb_sourcep)*t0)+((2.0*t0)*t0_Vdrainb_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vdrainb_sourcep))/15.0));
t4_Vbulkp_sourcep=(((t1_Vbulkp_sourcep*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))+(t1*(((((2.0*t0_Vbulkp_sourcep)*t0)+((2.0*t0)*t0_Vbulkp_sourcep))/3.0)+((t1_Vbulkp_sourcep*(t1-((4.0*t0)/3.0)))+(t1*(t1_Vbulkp_sourcep-((4.0*t0_Vbulkp_sourcep)/3.0)))))))-((((((2.0*t0_Vbulkp_sourcep)*t0)+((2.0*t0)*t0_Vbulkp_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vbulkp_sourcep))/15.0));
t4_Vsourceb_sourcep=(((t1_Vsourceb_sourcep*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))+(t1*(((((2.0*t0_Vsourceb_sourcep)*t0)+((2.0*t0)*t0_Vsourceb_sourcep))/3.0)+((t1_Vsourceb_sourcep*(t1-((4.0*t0)/3.0)))+(t1*(t1_Vsourceb_sourcep-((4.0*t0_Vsourceb_sourcep)/3.0)))))))-((((((2.0*t0_Vsourceb_sourcep)*t0)+((2.0*t0)*t0_Vsourceb_sourcep))*t0)+(((2.0*t0)*t0)*t0_Vsourceb_sourcep))/15.0));
#endif
t4=((t1*((((2.0*t0)*t0)/3.0)+(t1*(t1-((4.0*t0)/3.0)))))-((((2.0*t0)*t0)*t0)/15.0));
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=(((-t3_Vgatep_sourcep)*t4)+((-t3)*t4_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=(((-t3_Vdrainp_sourcep)*t4)+((-t3)*t4_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=(((-t3_Vdrainb_sourcep)*t4)+((-t3)*t4_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=(((-t3_Vbulkp_sourcep)*t4)+((-t3)*t4_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=(((-t3_Vsourceb_sourcep)*t4)+((-t3)*t4_Vsourceb_sourcep));
#endif
qsrc=((-t3)*t4);
}
else
{
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=((-0.5)*qgate_Vgatep_sourcep);
qsrc_Vdrainp_sourcep=((-0.5)*qgate_Vdrainp_sourcep);
qsrc_Vdrainb_sourcep=((-0.5)*qgate_Vdrainb_sourcep);
qsrc_Vbulkp_sourcep=((-0.5)*qgate_Vbulkp_sourcep);
qsrc_Vsourceb_sourcep=((-0.5)*qgate_Vsourceb_sourcep);
#endif
qsrc=((-0.5)*qgate);
}
}
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(((qgate_Vgatep_sourcep+qac0_Vgatep_sourcep)+qsub0_Vgatep_sourcep)-qbulk_Vgatep_sourcep);
qgate_Vdrainp_sourcep=(((qgate_Vdrainp_sourcep+qac0_Vdrainp_sourcep)+qsub0_Vdrainp_sourcep)-qbulk_Vdrainp_sourcep);
qgate_Vdrainb_sourcep=(((qgate_Vdrainb_sourcep+qac0_Vdrainb_sourcep)+qsub0_Vdrainb_sourcep)-qbulk_Vdrainb_sourcep);
qgate_Vbulkp_sourcep=(((qgate_Vbulkp_sourcep+qac0_Vbulkp_sourcep)+qsub0_Vbulkp_sourcep)-qbulk_Vbulkp_sourcep);
qgate_Vsourceb_sourcep=(((qgate_Vsourceb_sourcep+qac0_Vsourceb_sourcep)+qsub0_Vsourceb_sourcep)-qbulk_Vsourceb_sourcep);
#endif
qgate=(((qgate+qac0)+qsub0)-qbulk);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=0.0;
qbulk_Vgatep_sourcep=(qbulk_Vgatep_sourcep-(qac0_Vgatep_sourcep+qsub0_Vgatep_sourcep));
qbulk_Vdrainp_sourcep=(qbulk_Vdrainp_sourcep-(qac0_Vdrainp_sourcep+qsub0_Vdrainp_sourcep));
qbulk_Vdrainb_sourcep=(qbulk_Vdrainb_sourcep-(qac0_Vdrainb_sourcep+qsub0_Vdrainb_sourcep));
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-(qac0_Vbulkp_sourcep+qsub0_Vbulkp_sourcep));
qbulk_Vsourceb_sourcep=(qbulk_Vsourceb_sourcep-(qac0_Vsourceb_sourcep+qsub0_Vsourceb_sourcep));
#endif
qbulk=(qbulk-(qac0+qsub0));
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep)+qsrc_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep)+qsrc_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep)+qsrc_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep)+qsrc_Vsourceb_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
}
}
}
}
if
((ccn==1))
{
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep)+qdrn_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep)+qdrn_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep)+qdrn_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep)+qdrn_Vsourceb_sourcep));
#endif
qsrc=(-((qgate+qbulk)+qdrn));
}
if
((ccn==1))
{
czbd=(cjd_temp*adeff);
czbs=(cjs_temp*aseff);
czbdsw=(cjswd_temp*pdeff);
czbdswg=((cjswgd_temp*weffcj)*nf);
czbssw=(cjsws_temp*pseff);
czbsswg=((cjswgs_temp*weffcj)*nf);
if
((vbs_jct==0.0))
{
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vdrainb_sourcep=0.0;
qbs_Vbulkp_sourcep=0.0;
qbs_Vsourceb_sourcep=0.0;
#endif
qbs=0.0;
}
else
{
if
((vbs_jct<0.0))
{
if
((czbs>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vdrainb_sourcep=0.0;
arg_Vbulkp_sourcep=(-(vbs_jct_Vbulkp_sourcep/phibs));
arg_Vsourceb_sourcep=(-(vbs_jct_Vsourceb_sourcep/phibs));
#endif
arg=(1.0-(vbs_jct/phibs));
if
((mjs==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjs)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=((-mjs)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjs)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vdrainb_sourcep=0.0;
qbs_Vbulkp_sourcep=(((phibs*czbs)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjs));
qbs_Vsourceb_sourcep=(((phibs*czbs)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjs));
#endif
qbs=(((phibs*czbs)*(1.0-(arg*sarg)))/(1.0-mjs));
}
else
{
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vdrainb_sourcep=0.0;
qbs_Vbulkp_sourcep=0.0;
qbs_Vsourceb_sourcep=0.0;
#endif
qbs=0.0;
}
if
((czbssw>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vdrainb_sourcep=0.0;
arg_Vbulkp_sourcep=(-(vbs_jct_Vbulkp_sourcep/phibsws));
arg_Vsourceb_sourcep=(-(vbs_jct_Vsourceb_sourcep/phibsws));
#endif
arg=(1.0-(vbs_jct/phibsws));
if
((mjsws==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjsws)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=((-mjsws)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjsws)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vdrainb_sourcep=0.0;
qbs_Vbulkp_sourcep=(qbs_Vbulkp_sourcep+(((phibsws*czbssw)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjsws)));
qbs_Vsourceb_sourcep=(qbs_Vsourceb_sourcep+(((phibsws*czbssw)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjsws)));
#endif
qbs=(qbs+(((phibsws*czbssw)*(1.0-(arg*sarg)))/(1.0-mjsws)));
}
if
((czbsswg>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=0.0;
arg_Vdrainb_sourcep=0.0;
arg_Vbulkp_sourcep=(-(vbs_jct_Vbulkp_sourcep/phibswgs));
arg_Vsourceb_sourcep=(-(vbs_jct_Vsourceb_sourcep/phibswgs));
#endif
arg=(1.0-(vbs_jct/phibswgs));
if
((mjswgs==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjswgs)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=0.0;
sarg_Vdrainb_sourcep=0.0;
sarg_Vbulkp_sourcep=((-mjswgs)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjswgs)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=0.0;
qbs_Vdrainp_sourcep=0.0;
qbs_Vdrainb_sourcep=0.0;
qbs_Vbulkp_sourcep=(qbs_Vbulkp_sourcep+(((phibswgs*czbsswg)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjswgs)));
qbs_Vsourceb_sourcep=(qbs_Vsourceb_sourcep+(((phibswgs*czbsswg)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjswgs)));
#endif
qbs=(qbs+(((phibswgs*czbsswg)*(1.0-(arg*sarg)))/(1.0-mjswgs)));
}
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((czbs+czbssw)+czbsswg);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=0.0;
t1_Vdrainb_sourcep=0.0;
t1_Vbulkp_sourcep=vbs_jct_Vbulkp_sourcep*((((czbs*mjs)/phibs)+((czbssw*mjsws)/phibsws))+((czbsswg*mjswgs)/phibswgs));
t1_Vsourceb_sourcep=vbs_jct_Vsourceb_sourcep*((((czbs*mjs)/phibs)+((czbssw*mjsws)/phibsws))+((czbsswg*mjswgs)/phibswgs));
#endif
t1=(vbs_jct*((((czbs*mjs)/phibs)+((czbssw*mjsws)/phibsws))+((czbsswg*mjswgs)/phibswgs)));
#if defined(_DERIVATE)
qbs_Vgatep_sourcep=(vbs_jct*(t0_Vgatep_sourcep+(0.5*t1_Vgatep_sourcep)));
qbs_Vdrainp_sourcep=(vbs_jct*(t0_Vdrainp_sourcep+(0.5*t1_Vdrainp_sourcep)));
qbs_Vdrainb_sourcep=(vbs_jct*(t0_Vdrainb_sourcep+(0.5*t1_Vdrainb_sourcep)));
qbs_Vbulkp_sourcep=((vbs_jct_Vbulkp_sourcep*(t0+(0.5*t1)))+(vbs_jct*(t0_Vbulkp_sourcep+(0.5*t1_Vbulkp_sourcep))));
qbs_Vsourceb_sourcep=((vbs_jct_Vsourceb_sourcep*(t0+(0.5*t1)))+(vbs_jct*(t0_Vsourceb_sourcep+(0.5*t1_Vsourceb_sourcep))));
#endif
qbs=(vbs_jct*(t0+(0.5*t1)));
}
}
if
((vbd_jct==0.0))
{
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=0.0;
qbd_Vdrainp_sourcep=0.0;
qbd_Vdrainb_sourcep=0.0;
qbd_Vbulkp_sourcep=0.0;
qbd_Vsourceb_sourcep=0.0;
#endif
qbd=0.0;
}
else
{
if
((vbd_jct<0.0))
{
if
((czbd>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_jct_Vdrainp_sourcep/phibd));
arg_Vdrainb_sourcep=(-(vbd_jct_Vdrainb_sourcep/phibd));
arg_Vbulkp_sourcep=(-(vbd_jct_Vbulkp_sourcep/phibd));
arg_Vsourceb_sourcep=0.0;
#endif
arg=(1.0-(vbd_jct/phibd));
if
((mjd==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vdrainb_sourcep=(-arg_Vdrainb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjd)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mjd)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vdrainb_sourcep=((-mjd)*arg_Vdrainb_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulkp_sourcep=((-mjd)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjd)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=0.0;
qbd_Vdrainp_sourcep=(((phibd*czbd)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mjd));
qbd_Vdrainb_sourcep=(((phibd*czbd)*(-((arg_Vdrainb_sourcep*sarg)+(arg*sarg_Vdrainb_sourcep))))/(1.0-mjd));
qbd_Vbulkp_sourcep=(((phibd*czbd)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjd));
qbd_Vsourceb_sourcep=(((phibd*czbd)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjd));
#endif
qbd=(((phibd*czbd)*(1.0-(arg*sarg)))/(1.0-mjd));
}
else
{
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=0.0;
qbd_Vdrainp_sourcep=0.0;
qbd_Vdrainb_sourcep=0.0;
qbd_Vbulkp_sourcep=0.0;
qbd_Vsourceb_sourcep=0.0;
#endif
qbd=0.0;
}
if
((czbdsw>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_jct_Vdrainp_sourcep/phibswd));
arg_Vdrainb_sourcep=(-(vbd_jct_Vdrainb_sourcep/phibswd));
arg_Vbulkp_sourcep=(-(vbd_jct_Vbulkp_sourcep/phibswd));
arg_Vsourceb_sourcep=0.0;
#endif
arg=(1.0-(vbd_jct/phibswd));
if
((mjswd==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vdrainb_sourcep=(-arg_Vdrainb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjswd)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mjswd)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vdrainb_sourcep=((-mjswd)*arg_Vdrainb_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulkp_sourcep=((-mjswd)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjswd)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=0.0;
qbd_Vdrainp_sourcep=(qbd_Vdrainp_sourcep+(((phibswd*czbdsw)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mjswd)));
qbd_Vdrainb_sourcep=(qbd_Vdrainb_sourcep+(((phibswd*czbdsw)*(-((arg_Vdrainb_sourcep*sarg)+(arg*sarg_Vdrainb_sourcep))))/(1.0-mjswd)));
qbd_Vbulkp_sourcep=(qbd_Vbulkp_sourcep+(((phibswd*czbdsw)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjswd)));
qbd_Vsourceb_sourcep=(qbd_Vsourceb_sourcep+(((phibswd*czbdsw)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjswd)));
#endif
qbd=(qbd+(((phibswd*czbdsw)*(1.0-(arg*sarg)))/(1.0-mjswd)));
}
if
((czbdswg>0.0))
{
#if defined(_DERIVATE)
arg_Vdrainp_sourcep=(-(vbd_jct_Vdrainp_sourcep/phibswgd));
arg_Vdrainb_sourcep=(-(vbd_jct_Vdrainb_sourcep/phibswgd));
arg_Vbulkp_sourcep=(-(vbd_jct_Vbulkp_sourcep/phibswgd));
arg_Vsourceb_sourcep=0.0;
#endif
arg=(1.0-(vbd_jct/phibswgd));
if
((mjswgd==0.5))
{
{
double m00_sqrt(d00_sqrt0,arg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,arg)
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=(-arg_Vdrainp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vdrainb_sourcep=(-arg_Vdrainb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vbulkp_sourcep=(-arg_Vbulkp_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
sarg_Vsourceb_sourcep=(-arg_Vsourceb_sourcep*d10_sqrt0/d00_sqrt0/d00_sqrt0);
#endif
sarg=(1.0/d00_sqrt0);
}
}
else
{
{
double m00_logE(d00_logE0,arg)
double m00_exp(d00_exp1,((-mjswgd)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,arg)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
sarg_Vdrainp_sourcep=((-mjswgd)*arg_Vdrainp_sourcep*d10_logE0)*d10_exp1;
sarg_Vdrainb_sourcep=((-mjswgd)*arg_Vdrainb_sourcep*d10_logE0)*d10_exp1;
sarg_Vbulkp_sourcep=((-mjswgd)*arg_Vbulkp_sourcep*d10_logE0)*d10_exp1;
sarg_Vsourceb_sourcep=((-mjswgd)*arg_Vsourceb_sourcep*d10_logE0)*d10_exp1;
#endif
sarg=d00_exp1;
}
}
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=0.0;
qbd_Vdrainp_sourcep=(qbd_Vdrainp_sourcep+(((phibswgd*czbdswg)*(-((arg_Vdrainp_sourcep*sarg)+(arg*sarg_Vdrainp_sourcep))))/(1.0-mjswgd)));
qbd_Vdrainb_sourcep=(qbd_Vdrainb_sourcep+(((phibswgd*czbdswg)*(-((arg_Vdrainb_sourcep*sarg)+(arg*sarg_Vdrainb_sourcep))))/(1.0-mjswgd)));
qbd_Vbulkp_sourcep=(qbd_Vbulkp_sourcep+(((phibswgd*czbdswg)*(-((arg_Vbulkp_sourcep*sarg)+(arg*sarg_Vbulkp_sourcep))))/(1.0-mjswgd)));
qbd_Vsourceb_sourcep=(qbd_Vsourceb_sourcep+(((phibswgd*czbdswg)*(-((arg_Vsourceb_sourcep*sarg)+(arg*sarg_Vsourceb_sourcep))))/(1.0-mjswgd)));
#endif
qbd=(qbd+(((phibswgd*czbdswg)*(1.0-(arg*sarg)))/(1.0-mjswgd)));
}
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=0.0;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=((czbd+czbdsw)+czbdswg);
#if defined(_DERIVATE)
t1_Vgatep_sourcep=0.0;
t1_Vdrainp_sourcep=vbd_jct_Vdrainp_sourcep*((((czbd*mjd)/phibd)+((czbdsw*mjswd)/phibswd))+((czbdswg*mjswgd)/phibswgd));
t1_Vdrainb_sourcep=vbd_jct_Vdrainb_sourcep*((((czbd*mjd)/phibd)+((czbdsw*mjswd)/phibswd))+((czbdswg*mjswgd)/phibswgd));
t1_Vbulkp_sourcep=vbd_jct_Vbulkp_sourcep*((((czbd*mjd)/phibd)+((czbdsw*mjswd)/phibswd))+((czbdswg*mjswgd)/phibswgd));
t1_Vsourceb_sourcep=0.0;
#endif
t1=(vbd_jct*((((czbd*mjd)/phibd)+((czbdsw*mjswd)/phibswd))+((czbdswg*mjswgd)/phibswgd)));
#if defined(_DERIVATE)
qbd_Vgatep_sourcep=(vbd_jct*(t0_Vgatep_sourcep+(0.5*t1_Vgatep_sourcep)));
qbd_Vdrainp_sourcep=((vbd_jct_Vdrainp_sourcep*(t0+(0.5*t1)))+(vbd_jct*(t0_Vdrainp_sourcep+(0.5*t1_Vdrainp_sourcep))));
qbd_Vdrainb_sourcep=((vbd_jct_Vdrainb_sourcep*(t0+(0.5*t1)))+(vbd_jct*(t0_Vdrainb_sourcep+(0.5*t1_Vdrainb_sourcep))));
qbd_Vbulkp_sourcep=((vbd_jct_Vbulkp_sourcep*(t0+(0.5*t1)))+(vbd_jct*(t0_Vbulkp_sourcep+(0.5*t1_Vbulkp_sourcep))));
qbd_Vsourceb_sourcep=(vbd_jct*(t0_Vsourceb_sourcep+(0.5*t1_Vsourceb_sourcep)));
#endif
qbd=(vbd_jct*(t0+(0.5*t1)));
}
}
}
if
((ccn==1))
{
if
((capmod==0))
{
#if defined(_DYNAMIC)
cgdo=param_cgdo;
#endif
#if defined(_DYNAMIC)
cgso=param_cgso;
#endif
#if defined(_DERIVATE)
qgdo_Vdrainb_sourcep=0.0;
qgdo_Vbulkp_sourcep=0.0;
qgdo_Vsourceb_sourcep=0.0;
qgdo_Vgatep_sourcep=(param_cgdo*vgd_Vgatep_sourcep);
qgdo_Vdrainp_sourcep=(param_cgdo*vgd_Vdrainp_sourcep);
#endif
qgdo=(param_cgdo*vgd);
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=0.0;
qgso_Vdrainb_sourcep=0.0;
qgso_Vbulkp_sourcep=0.0;
qgso_Vsourceb_sourcep=0.0;
qgso_Vgatep_sourcep=(param_cgso*vgs_Vgatep_sourcep);
#endif
qgso=(param_cgso*vgs);
}
else
{
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgd_Vgatep_sourcep;
t0_Vdrainp_sourcep=vgd_Vdrainp_sourcep;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgd+0.02);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+(4.0*0.02)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+(4.0*0.02)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(0.5*(t0_Vgatep_sourcep-t1_Vgatep_sourcep));
t2_Vdrainp_sourcep=(0.5*(t0_Vdrainp_sourcep-t1_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(0.5*(t0_Vdrainb_sourcep-t1_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(0.5*(t0_Vbulkp_sourcep-t1_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(0.5*(t0_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
t2=(0.5*(t0-t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(weffcv*cgdl);
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*t2)/ckappad)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*t2)/ckappad)))
#endif
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-((4.0*t2_Vgatep_sourcep)/ckappad))*d10_sqrt0;
t4_Vdrainp_sourcep=(-((4.0*t2_Vdrainp_sourcep)/ckappad))*d10_sqrt0;
t4_Vdrainb_sourcep=(-((4.0*t2_Vdrainb_sourcep)/ckappad))*d10_sqrt0;
t4_Vbulkp_sourcep=(-((4.0*t2_Vbulkp_sourcep)/ckappad))*d10_sqrt0;
t4_Vsourceb_sourcep=(-((4.0*t2_Vsourceb_sourcep)/ckappad))*d10_sqrt0;
#endif
t4=d00_sqrt0;
}
#if defined(_DYNAMIC)
cgdo=((param_cgdo+t3)-((t3*(1.0-(1.0/t4)))*(0.5-((0.5*t0)/t1))));
#endif
#if defined(_DERIVATE)
qgdo_Vdrainb_sourcep=(t3_Vdrainb_sourcep*vgd-((t3_Vdrainb_sourcep*(t2+((0.5*ckappad)*(t4-1.0))))+(t3*(t2_Vdrainb_sourcep+((0.5*ckappad)*t4_Vdrainb_sourcep)))));
qgdo_Vbulkp_sourcep=(t3_Vbulkp_sourcep*vgd-((t3_Vbulkp_sourcep*(t2+((0.5*ckappad)*(t4-1.0))))+(t3*(t2_Vbulkp_sourcep+((0.5*ckappad)*t4_Vbulkp_sourcep)))));
qgdo_Vsourceb_sourcep=(t3_Vsourceb_sourcep*vgd-((t3_Vsourceb_sourcep*(t2+((0.5*ckappad)*(t4-1.0))))+(t3*(t2_Vsourceb_sourcep+((0.5*ckappad)*t4_Vsourceb_sourcep)))));
qgdo_Vgatep_sourcep=(((t3_Vgatep_sourcep*vgd)+((param_cgdo+t3)*vgd_Vgatep_sourcep))-((t3_Vgatep_sourcep*(t2+((0.5*ckappad)*(t4-1.0))))+(t3*(t2_Vgatep_sourcep+((0.5*ckappad)*t4_Vgatep_sourcep)))));
qgdo_Vdrainp_sourcep=(((t3_Vdrainp_sourcep*vgd)+((param_cgdo+t3)*vgd_Vdrainp_sourcep))-((t3_Vdrainp_sourcep*(t2+((0.5*ckappad)*(t4-1.0))))+(t3*(t2_Vdrainp_sourcep+((0.5*ckappad)*t4_Vdrainp_sourcep)))));
#endif
qgdo=(((param_cgdo+t3)*vgd)-(t3*(t2+((0.5*ckappad)*(t4-1.0)))));
#if defined(_DERIVATE)
t0_Vgatep_sourcep=vgs_Vgatep_sourcep;
t0_Vdrainp_sourcep=0.0;
t0_Vdrainb_sourcep=0.0;
t0_Vbulkp_sourcep=0.0;
t0_Vsourceb_sourcep=0.0;
#endif
t0=(vgs+0.02);
{
double m00_sqrt(d00_sqrt0,((t0*t0)+(4.0*0.02)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((t0*t0)+(4.0*0.02)))
#endif
#if defined(_DERIVATE)
t1_Vgatep_sourcep=((t0_Vgatep_sourcep*t0)+(t0*t0_Vgatep_sourcep))*d10_sqrt0;
t1_Vdrainp_sourcep=((t0_Vdrainp_sourcep*t0)+(t0*t0_Vdrainp_sourcep))*d10_sqrt0;
t1_Vdrainb_sourcep=((t0_Vdrainb_sourcep*t0)+(t0*t0_Vdrainb_sourcep))*d10_sqrt0;
t1_Vbulkp_sourcep=((t0_Vbulkp_sourcep*t0)+(t0*t0_Vbulkp_sourcep))*d10_sqrt0;
t1_Vsourceb_sourcep=((t0_Vsourceb_sourcep*t0)+(t0*t0_Vsourceb_sourcep))*d10_sqrt0;
#endif
t1=d00_sqrt0;
}
#if defined(_DERIVATE)
t2_Vgatep_sourcep=(0.5*(t0_Vgatep_sourcep-t1_Vgatep_sourcep));
t2_Vdrainp_sourcep=(0.5*(t0_Vdrainp_sourcep-t1_Vdrainp_sourcep));
t2_Vdrainb_sourcep=(0.5*(t0_Vdrainb_sourcep-t1_Vdrainb_sourcep));
t2_Vbulkp_sourcep=(0.5*(t0_Vbulkp_sourcep-t1_Vbulkp_sourcep));
t2_Vsourceb_sourcep=(0.5*(t0_Vsourceb_sourcep-t1_Vsourceb_sourcep));
#endif
t2=(0.5*(t0-t1));
#if defined(_DERIVATE)
t3_Vgatep_sourcep=0.0;
t3_Vdrainp_sourcep=0.0;
t3_Vdrainb_sourcep=0.0;
t3_Vbulkp_sourcep=0.0;
t3_Vsourceb_sourcep=0.0;
#endif
t3=(weffcv*cgsl);
{
double m00_sqrt(d00_sqrt0,(1.0-((4.0*t2)/ckappas)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0-((4.0*t2)/ckappas)))
#endif
#if defined(_DERIVATE)
t4_Vgatep_sourcep=(-((4.0*t2_Vgatep_sourcep)/ckappas))*d10_sqrt0;
t4_Vdrainp_sourcep=(-((4.0*t2_Vdrainp_sourcep)/ckappas))*d10_sqrt0;
t4_Vdrainb_sourcep=(-((4.0*t2_Vdrainb_sourcep)/ckappas))*d10_sqrt0;
t4_Vbulkp_sourcep=(-((4.0*t2_Vbulkp_sourcep)/ckappas))*d10_sqrt0;
t4_Vsourceb_sourcep=(-((4.0*t2_Vsourceb_sourcep)/ckappas))*d10_sqrt0;
#endif
t4=d00_sqrt0;
}
#if defined(_DYNAMIC)
cgso=((param_cgso+t3)-((t3*(1.0-(1.0/t4)))*(0.5-((0.5*t0)/t1))));
#endif
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=(t3_Vdrainp_sourcep*vgs-((t3_Vdrainp_sourcep*(t2+((0.5*ckappas)*(t4-1.0))))+(t3*(t2_Vdrainp_sourcep+((0.5*ckappas)*t4_Vdrainp_sourcep)))));
qgso_Vdrainb_sourcep=(t3_Vdrainb_sourcep*vgs-((t3_Vdrainb_sourcep*(t2+((0.5*ckappas)*(t4-1.0))))+(t3*(t2_Vdrainb_sourcep+((0.5*ckappas)*t4_Vdrainb_sourcep)))));
qgso_Vbulkp_sourcep=(t3_Vbulkp_sourcep*vgs-((t3_Vbulkp_sourcep*(t2+((0.5*ckappas)*(t4-1.0))))+(t3*(t2_Vbulkp_sourcep+((0.5*ckappas)*t4_Vbulkp_sourcep)))));
qgso_Vsourceb_sourcep=(t3_Vsourceb_sourcep*vgs-((t3_Vsourceb_sourcep*(t2+((0.5*ckappas)*(t4-1.0))))+(t3*(t2_Vsourceb_sourcep+((0.5*ckappas)*t4_Vsourceb_sourcep)))));
qgso_Vgatep_sourcep=(((t3_Vgatep_sourcep*vgs)+((param_cgso+t3)*vgs_Vgatep_sourcep))-((t3_Vgatep_sourcep*(t2+((0.5*ckappas)*(t4-1.0))))+(t3*(t2_Vgatep_sourcep+((0.5*ckappas)*t4_Vgatep_sourcep)))));
#endif
qgso=(((param_cgso+t3)*vgs)-(t3*(t2+((0.5*ckappas)*(t4-1.0)))));
}
if
((nf!=1.0))
{
#if defined(_DYNAMIC)
cgdo=(cgdo*nf);
#endif
#if defined(_DYNAMIC)
cgso=(cgso*nf);
#endif
#if defined(_DERIVATE)
qgdo_Vdrainb_sourcep=qgdo_Vdrainb_sourcep*nf;
qgdo_Vbulkp_sourcep=qgdo_Vbulkp_sourcep*nf;
qgdo_Vsourceb_sourcep=qgdo_Vsourceb_sourcep*nf;
qgdo_Vgatep_sourcep=qgdo_Vgatep_sourcep*nf;
qgdo_Vdrainp_sourcep=qgdo_Vdrainp_sourcep*nf;
#endif
qgdo=(qgdo*nf);
#if defined(_DERIVATE)
qgso_Vdrainp_sourcep=qgso_Vdrainp_sourcep*nf;
qgso_Vdrainb_sourcep=qgso_Vdrainb_sourcep*nf;
qgso_Vbulkp_sourcep=qgso_Vbulkp_sourcep*nf;
qgso_Vsourceb_sourcep=qgso_Vsourceb_sourcep*nf;
qgso_Vgatep_sourcep=qgso_Vgatep_sourcep*nf;
#endif
qgso=(qgso*nf);
}
if
((mode>0))
{
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=0.0;
qdrn_Vgatep_sourcep=(qdrn_Vgatep_sourcep-qgdo_Vgatep_sourcep);
qdrn_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qgdo_Vdrainp_sourcep);
qdrn_Vdrainb_sourcep=(qdrn_Vdrainb_sourcep-qgdo_Vdrainb_sourcep);
qdrn_Vbulkp_sourcep=(qdrn_Vbulkp_sourcep-qgdo_Vbulkp_sourcep);
qdrn_Vsourceb_sourcep=(qdrn_Vsourceb_sourcep-qgdo_Vsourceb_sourcep);
#endif
qdrn=(qdrn-qgdo);
if
((rgatemod==3))
{
#if defined(_DERIVATE)
qgmb_Vgatem_sourcep=(cgbo*vgmb_Vgatem_sourcep);
qgmb_Vbulkp_sourcep=(cgbo*vgmb_Vbulkp_sourcep);
#endif
qgmb=(cgbo*vgmb);
#if defined(_DERIVATE)
qgmid_Vdrainb_sourcep=(qgdo_Vdrainb_sourcep+qgso_Vdrainb_sourcep);
qgmid_Vbulkp_sourcep=((qgdo_Vbulkp_sourcep+qgso_Vbulkp_sourcep)+qgmb_Vbulkp_sourcep);
qgmid_Vsourceb_sourcep=(qgdo_Vsourceb_sourcep+qgso_Vsourceb_sourcep);
qgmid_Vgatep_sourcep=(qgdo_Vgatep_sourcep+qgso_Vgatep_sourcep);
qgmid_Vdrainp_sourcep=(qgdo_Vdrainp_sourcep+qgso_Vdrainp_sourcep);
qgmid_Vgatem_sourcep=qgmb_Vgatem_sourcep;
#endif
qgmid=((qgdo+qgso)+qgmb);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=(qbulk_Vgatem_sourcep-qgmb_Vgatem_sourcep);
qbulk_Vgatep_sourcep=qbulk_Vgatep_sourcep;
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vdrainb_sourcep=qbulk_Vdrainb_sourcep;
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-qgmb_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=qbulk_Vsourceb_sourcep;
#endif
qbulk=(qbulk-qgmb);
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=(-((qgmid_Vgatem_sourcep+qbulk_Vgatem_sourcep)+qdrn_Vgatem_sourcep));
qsrc_Vgatep_sourcep=(-(((qgate_Vgatep_sourcep+qgmid_Vgatep_sourcep)+qbulk_Vgatep_sourcep)+qdrn_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=(-(((qgate_Vdrainp_sourcep+qgmid_Vdrainp_sourcep)+qbulk_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=(-(((qgate_Vdrainb_sourcep+qgmid_Vdrainb_sourcep)+qbulk_Vdrainb_sourcep)+qdrn_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=(-(((qgate_Vbulkp_sourcep+qgmid_Vbulkp_sourcep)+qbulk_Vbulkp_sourcep)+qdrn_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=(-(((qgate_Vsourceb_sourcep+qgmid_Vsourceb_sourcep)+qbulk_Vsourceb_sourcep)+qdrn_Vsourceb_sourcep));
#endif
qsrc=(-(((qgate+qgmid)+qbulk)+qdrn));
}
else
{
#if defined(_DERIVATE)
qgb_Vgatep_sourcep=(cgbo*vgb_Vgatep_sourcep);
qgb_Vbulkp_sourcep=(cgbo*vgb_Vbulkp_sourcep);
#endif
qgb=(cgbo*vgb);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(((qgate_Vgatep_sourcep+qgdo_Vgatep_sourcep)+qgso_Vgatep_sourcep)+qgb_Vgatep_sourcep);
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+qgdo_Vdrainp_sourcep)+qgso_Vdrainp_sourcep);
qgate_Vdrainb_sourcep=((qgate_Vdrainb_sourcep+qgdo_Vdrainb_sourcep)+qgso_Vdrainb_sourcep);
qgate_Vbulkp_sourcep=(((qgate_Vbulkp_sourcep+qgdo_Vbulkp_sourcep)+qgso_Vbulkp_sourcep)+qgb_Vbulkp_sourcep);
qgate_Vsourceb_sourcep=((qgate_Vsourceb_sourcep+qgdo_Vsourceb_sourcep)+qgso_Vsourceb_sourcep);
#endif
qgate=(((qgate+qgdo)+qgso)+qgb);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=qbulk_Vgatem_sourcep;
qbulk_Vgatep_sourcep=(qbulk_Vgatep_sourcep-qgb_Vgatep_sourcep);
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vdrainb_sourcep=qbulk_Vdrainb_sourcep;
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-qgb_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=qbulk_Vsourceb_sourcep;
#endif
qbulk=(qbulk-qgb);
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=(-(qbulk_Vgatem_sourcep+qdrn_Vgatem_sourcep));
qsrc_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep)+qdrn_Vgatep_sourcep));
qsrc_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qdrn_Vdrainp_sourcep));
qsrc_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep)+qdrn_Vdrainb_sourcep));
qsrc_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep)+qdrn_Vbulkp_sourcep));
qsrc_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep)+qdrn_Vsourceb_sourcep));
#endif
qsrc=(-((qgate+qbulk)+qdrn));
}
}
else
{
#if defined(_DERIVATE)
qsrc_Vgatem_sourcep=0.0;
qsrc_Vgatep_sourcep=(qdrn_Vgatep_sourcep-qgso_Vgatep_sourcep);
qsrc_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qgso_Vdrainp_sourcep);
qsrc_Vdrainb_sourcep=(qdrn_Vdrainb_sourcep-qgso_Vdrainb_sourcep);
qsrc_Vbulkp_sourcep=(qdrn_Vbulkp_sourcep-qgso_Vbulkp_sourcep);
qsrc_Vsourceb_sourcep=(qdrn_Vsourceb_sourcep-qgso_Vsourceb_sourcep);
#endif
qsrc=(qdrn-qgso);
if
((rgatemod==3))
{
#if defined(_DERIVATE)
qgmb_Vgatem_sourcep=(cgbo*vgmb_Vgatem_sourcep);
qgmb_Vbulkp_sourcep=(cgbo*vgmb_Vbulkp_sourcep);
#endif
qgmb=(cgbo*vgmb);
#if defined(_DERIVATE)
qgmid_Vdrainb_sourcep=(qgdo_Vdrainb_sourcep+qgso_Vdrainb_sourcep);
qgmid_Vbulkp_sourcep=((qgdo_Vbulkp_sourcep+qgso_Vbulkp_sourcep)+qgmb_Vbulkp_sourcep);
qgmid_Vsourceb_sourcep=(qgdo_Vsourceb_sourcep+qgso_Vsourceb_sourcep);
qgmid_Vgatep_sourcep=(qgdo_Vgatep_sourcep+qgso_Vgatep_sourcep);
qgmid_Vdrainp_sourcep=(qgdo_Vdrainp_sourcep+qgso_Vdrainp_sourcep);
qgmid_Vgatem_sourcep=qgmb_Vgatem_sourcep;
#endif
qgmid=((qgdo+qgso)+qgmb);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=(qbulk_Vgatem_sourcep-qgmb_Vgatem_sourcep);
qbulk_Vgatep_sourcep=qbulk_Vgatep_sourcep;
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vdrainb_sourcep=qbulk_Vdrainb_sourcep;
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-qgmb_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=qbulk_Vsourceb_sourcep;
#endif
qbulk=(qbulk-qgmb);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=(-((qgmid_Vgatem_sourcep+qbulk_Vgatem_sourcep)+qsrc_Vgatem_sourcep));
qdrn_Vgatep_sourcep=(-(((qgate_Vgatep_sourcep+qgmid_Vgatep_sourcep)+qbulk_Vgatep_sourcep)+qsrc_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=(-(((qgate_Vdrainp_sourcep+qgmid_Vdrainp_sourcep)+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=(-(((qgate_Vdrainb_sourcep+qgmid_Vdrainb_sourcep)+qbulk_Vdrainb_sourcep)+qsrc_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=(-(((qgate_Vbulkp_sourcep+qgmid_Vbulkp_sourcep)+qbulk_Vbulkp_sourcep)+qsrc_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=(-(((qgate_Vsourceb_sourcep+qgmid_Vsourceb_sourcep)+qbulk_Vsourceb_sourcep)+qsrc_Vsourceb_sourcep));
#endif
qdrn=(-(((qgate+qgmid)+qbulk)+qsrc));
}
else
{
#if defined(_DERIVATE)
qgb_Vgatep_sourcep=(cgbo*vgb_Vgatep_sourcep);
qgb_Vbulkp_sourcep=(cgbo*vgb_Vbulkp_sourcep);
#endif
qgb=(cgbo*vgb);
#if defined(_DERIVATE)
qgate_Vgatep_sourcep=(((qgate_Vgatep_sourcep+qgdo_Vgatep_sourcep)+qgso_Vgatep_sourcep)+qgb_Vgatep_sourcep);
qgate_Vdrainp_sourcep=((qgate_Vdrainp_sourcep+qgdo_Vdrainp_sourcep)+qgso_Vdrainp_sourcep);
qgate_Vdrainb_sourcep=((qgate_Vdrainb_sourcep+qgdo_Vdrainb_sourcep)+qgso_Vdrainb_sourcep);
qgate_Vbulkp_sourcep=(((qgate_Vbulkp_sourcep+qgdo_Vbulkp_sourcep)+qgso_Vbulkp_sourcep)+qgb_Vbulkp_sourcep);
qgate_Vsourceb_sourcep=((qgate_Vsourceb_sourcep+qgdo_Vsourceb_sourcep)+qgso_Vsourceb_sourcep);
#endif
qgate=(((qgate+qgdo)+qgso)+qgb);
#if defined(_DERIVATE)
qbulk_Vgatem_sourcep=qbulk_Vgatem_sourcep;
qbulk_Vgatep_sourcep=(qbulk_Vgatep_sourcep-qgb_Vgatep_sourcep);
qbulk_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qbulk_Vdrainb_sourcep=qbulk_Vdrainb_sourcep;
qbulk_Vbulkp_sourcep=(qbulk_Vbulkp_sourcep-qgb_Vbulkp_sourcep);
qbulk_Vsourceb_sourcep=qbulk_Vsourceb_sourcep;
#endif
qbulk=(qbulk-qgb);
#if defined(_DERIVATE)
qdrn_Vgatem_sourcep=(-(qbulk_Vgatem_sourcep+qsrc_Vgatem_sourcep));
qdrn_Vgatep_sourcep=(-((qgate_Vgatep_sourcep+qbulk_Vgatep_sourcep)+qsrc_Vgatep_sourcep));
qdrn_Vdrainp_sourcep=(-((qgate_Vdrainp_sourcep+qbulk_Vdrainp_sourcep)+qsrc_Vdrainp_sourcep));
qdrn_Vdrainb_sourcep=(-((qgate_Vdrainb_sourcep+qbulk_Vdrainb_sourcep)+qsrc_Vdrainb_sourcep));
qdrn_Vbulkp_sourcep=(-((qgate_Vbulkp_sourcep+qbulk_Vbulkp_sourcep)+qsrc_Vbulkp_sourcep));
qdrn_Vsourceb_sourcep=(-((qgate_Vsourceb_sourcep+qbulk_Vsourceb_sourcep)+qsrc_Vsourceb_sourcep));
#endif
qdrn=(-((qgate+qbulk)+qsrc));
}
}
#if defined(_DERIVATE)
qd_Vgatem_sourcep=qdrn_Vgatem_sourcep;
qd_Vgatep_sourcep=(qdrn_Vgatep_sourcep-qbd_Vgatep_sourcep);
qd_Vdrainp_sourcep=(qdrn_Vdrainp_sourcep-qbd_Vdrainp_sourcep);
qd_Vdrainb_sourcep=(qdrn_Vdrainb_sourcep-qbd_Vdrainb_sourcep);
qd_Vbulkp_sourcep=(qdrn_Vbulkp_sourcep-qbd_Vbulkp_sourcep);
qd_Vsourceb_sourcep=(qdrn_Vsourceb_sourcep-qbd_Vsourceb_sourcep);
#endif
qd=(qdrn-qbd);
#if defined(_DERIVATE)
qs_Vgatem_sourcep=qsrc_Vgatem_sourcep;
qs_Vgatep_sourcep=(qsrc_Vgatep_sourcep-qbs_Vgatep_sourcep);
qs_Vdrainp_sourcep=(qsrc_Vdrainp_sourcep-qbs_Vdrainp_sourcep);
qs_Vdrainb_sourcep=(qsrc_Vdrainb_sourcep-qbs_Vdrainb_sourcep);
qs_Vbulkp_sourcep=(qsrc_Vbulkp_sourcep-qbs_Vbulkp_sourcep);
qs_Vsourceb_sourcep=(qsrc_Vsourceb_sourcep-qbs_Vsourceb_sourcep);
#endif
qs=(qsrc-qbs);
#ifdef _DYNAMIC
if
((rbodymod==0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb_Vgatem_sourcep=qbulk_Vgatem_sourcep;
qb_Vgatep_sourcep=((qbulk_Vgatep_sourcep+qbd_Vgatep_sourcep)+qbs_Vgatep_sourcep);
qb_Vdrainp_sourcep=((qbulk_Vdrainp_sourcep+qbd_Vdrainp_sourcep)+qbs_Vdrainp_sourcep);
qb_Vdrainb_sourcep=((qbulk_Vdrainb_sourcep+qbd_Vdrainb_sourcep)+qbs_Vdrainb_sourcep);
qb_Vbulkp_sourcep=((qbulk_Vbulkp_sourcep+qbd_Vbulkp_sourcep)+qbs_Vbulkp_sourcep);
qb_Vsourceb_sourcep=((qbulk_Vsourceb_sourcep+qbd_Vsourceb_sourcep)+qbs_Vsourceb_sourcep);
#endif
qb=((qbulk+qbd)+qbs);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb_Vgatem_sourcep=qbulk_Vgatem_sourcep;
qb_Vgatep_sourcep=qbulk_Vgatep_sourcep;
qb_Vdrainp_sourcep=qbulk_Vdrainp_sourcep;
qb_Vdrainb_sourcep=qbulk_Vdrainb_sourcep;
qb_Vbulkp_sourcep=qbulk_Vbulkp_sourcep;
qb_Vsourceb_sourcep=qbulk_Vsourceb_sourcep;
#endif
qb=qbulk;
#endif
}
#endif /* if (...) */
}
_load_static_residual2(sourceb,sourcep,(type*cbs));
#if defined(_DERIVATE)
_load_static_jacobian4(sourceb,sourcep,sourceb,sourcep,(type*cbs_Vsourceb_sourcep));
_load_static_jacobian4(sourceb,sourcep,bulkp,sourcep,(type*cbs_Vbulkp_sourcep));
#endif
_load_static_residual2(drainb,drainp,(type*cbd));
#if defined(_DERIVATE)
_load_static_jacobian4(drainb,drainp,drainb,sourcep,(type*cbd_Vdrainb_sourcep));
_load_static_jacobian4(drainb,drainp,drainp,sourcep,(type*cbd_Vdrainp_sourcep));
_load_static_jacobian4(drainb,drainp,bulkp,sourcep,(type*cbd_Vbulkp_sourcep));
_load_static_jacobian4(drainb,drainp,sourceb,sourcep,(type*cbd_Vsourceb_sourcep));
#endif
if
((igbmod==1))
{
_load_static_residual2(bulk,gatep,((-type)*igb));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,gatep,sourceb,sourcep,((-type)*igb_Vsourceb_sourcep));
_load_static_jacobian4(bulk,gatep,bulkp,sourcep,((-type)*igb_Vbulkp_sourcep));
_load_static_jacobian4(bulk,gatep,drainb,sourcep,((-type)*igb_Vdrainb_sourcep));
_load_static_jacobian4(bulk,gatep,drainp,sourcep,((-type)*igb_Vdrainp_sourcep));
_load_static_jacobian4(bulk,gatep,gatep,sourcep,((-type)*igb_Vgatep_sourcep));
#endif
}
if
((igcmod==1))
{
_load_static_residual2(source,gatep,((-type)*(igs+igcs)));
#if defined(_DERIVATE)
_load_static_jacobian4(source,gatep,sourceb,sourcep,((-type)*(igs_Vsourceb_sourcep+igcs_Vsourceb_sourcep)));
_load_static_jacobian4(source,gatep,bulkp,sourcep,((-type)*(igs_Vbulkp_sourcep+igcs_Vbulkp_sourcep)));
_load_static_jacobian4(source,gatep,drainb,sourcep,((-type)*(igs_Vdrainb_sourcep+igcs_Vdrainb_sourcep)));
_load_static_jacobian4(source,gatep,drainp,sourcep,((-type)*(igs_Vdrainp_sourcep+igcs_Vdrainp_sourcep)));
_load_static_jacobian4(source,gatep,gatep,sourcep,((-type)*(igs_Vgatep_sourcep+igcs_Vgatep_sourcep)));
#endif
_load_static_residual2(drain,gatep,((-type)*(igd+igcd)));
#if defined(_DERIVATE)
_load_static_jacobian4(drain,gatep,sourceb,sourcep,((-type)*(igd_Vsourceb_sourcep+igcd_Vsourceb_sourcep)));
_load_static_jacobian4(drain,gatep,bulkp,sourcep,((-type)*(igd_Vbulkp_sourcep+igcd_Vbulkp_sourcep)));
_load_static_jacobian4(drain,gatep,drainb,sourcep,((-type)*(igd_Vdrainb_sourcep+igcd_Vdrainb_sourcep)));
_load_static_jacobian4(drain,gatep,drainp,sourcep,((-type)*(igd_Vdrainp_sourcep+igcd_Vdrainp_sourcep)));
_load_static_jacobian4(drain,gatep,gatep,sourcep,((-type)*(igd_Vgatep_sourcep+igcd_Vgatep_sourcep)));
#endif
}
if
((mode==1))
{
_load_static_residual2(drainp,sourcep,(type*cdrain));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,sourcep,sourceb,sourcep,(type*cdrain_Vsourceb_sourcep));
_load_static_jacobian4(drainp,sourcep,bulkp,sourcep,(type*cdrain_Vbulkp_sourcep));
_load_static_jacobian4(drainp,sourcep,drainb,sourcep,(type*cdrain_Vdrainb_sourcep));
_load_static_jacobian4(drainp,sourcep,drainp,sourcep,(type*cdrain_Vdrainp_sourcep));
_load_static_jacobian4(drainp,sourcep,gatep,sourcep,(type*cdrain_Vgatep_sourcep));
#endif
_load_static_residual2(drainp,bulkp,(type*(csub+igidl)));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,bulkp,sourceb,sourcep,(type*(csub_Vsourceb_sourcep+igidl_Vsourceb_sourcep)));
_load_static_jacobian4(drainp,bulkp,bulkp,sourcep,(type*(csub_Vbulkp_sourcep+igidl_Vbulkp_sourcep)));
_load_static_jacobian4(drainp,bulkp,drainb,sourcep,(type*(csub_Vdrainb_sourcep+igidl_Vdrainb_sourcep)));
_load_static_jacobian4(drainp,bulkp,drainp,sourcep,(type*(csub_Vdrainp_sourcep+igidl_Vdrainp_sourcep)));
_load_static_jacobian4(drainp,bulkp,gatep,sourcep,(type*(csub_Vgatep_sourcep+igidl_Vgatep_sourcep)));
#endif
_load_static_residual2(sourcep,bulkp,(type*igisl));
#if defined(_DERIVATE)
_load_static_jacobian4(sourcep,bulkp,sourceb,sourcep,(type*igisl_Vsourceb_sourcep));
_load_static_jacobian4(sourcep,bulkp,bulkp,sourcep,(type*igisl_Vbulkp_sourcep));
_load_static_jacobian4(sourcep,bulkp,drainb,sourcep,(type*igisl_Vdrainb_sourcep));
_load_static_jacobian4(sourcep,bulkp,drainp,sourcep,(type*igisl_Vdrainp_sourcep));
_load_static_jacobian4(sourcep,bulkp,gatep,sourcep,(type*igisl_Vgatep_sourcep));
#endif
}
else
{
_load_static_residual2(drainp,sourcep,((-type)*cdrain));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,sourcep,sourceb,sourcep,((-type)*cdrain_Vsourceb_sourcep));
_load_static_jacobian4(drainp,sourcep,bulkp,sourcep,((-type)*cdrain_Vbulkp_sourcep));
_load_static_jacobian4(drainp,sourcep,drainb,sourcep,((-type)*cdrain_Vdrainb_sourcep));
_load_static_jacobian4(drainp,sourcep,drainp,sourcep,((-type)*cdrain_Vdrainp_sourcep));
_load_static_jacobian4(drainp,sourcep,gatep,sourcep,((-type)*cdrain_Vgatep_sourcep));
#endif
_load_static_residual2(drainp,bulkp,(type*(csub+igisl)));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,bulkp,sourceb,sourcep,(type*(csub_Vsourceb_sourcep+igisl_Vsourceb_sourcep)));
_load_static_jacobian4(drainp,bulkp,bulkp,sourcep,(type*(csub_Vbulkp_sourcep+igisl_Vbulkp_sourcep)));
_load_static_jacobian4(drainp,bulkp,drainb,sourcep,(type*(csub_Vdrainb_sourcep+igisl_Vdrainb_sourcep)));
_load_static_jacobian4(drainp,bulkp,drainp,sourcep,(type*(csub_Vdrainp_sourcep+igisl_Vdrainp_sourcep)));
_load_static_jacobian4(drainp,bulkp,gatep,sourcep,(type*(csub_Vgatep_sourcep+igisl_Vgatep_sourcep)));
#endif
_load_static_residual2(sourcep,bulkp,(type*igidl));
#if defined(_DERIVATE)
_load_static_jacobian4(sourcep,bulkp,sourceb,sourcep,(type*igidl_Vsourceb_sourcep));
_load_static_jacobian4(sourcep,bulkp,bulkp,sourcep,(type*igidl_Vbulkp_sourcep));
_load_static_jacobian4(sourcep,bulkp,drainb,sourcep,(type*igidl_Vdrainb_sourcep));
_load_static_jacobian4(sourcep,bulkp,drainp,sourcep,(type*igidl_Vdrainp_sourcep));
_load_static_jacobian4(sourcep,bulkp,gatep,sourcep,(type*igidl_Vgatep_sourcep));
#endif
}
if
((rdsmod==0))
{
_load_static_residual2(source,sourcep,(BP(source,sourcep)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(source,sourcep,source,sourcep,(1e3));
#endif
_load_static_residual2(drainp,drain,(BP(drainp,drain)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(drainp,drain,drainp,drain,(1e3));
#endif
}
else
{
_load_static_residual2(drain,drainp,((type*gdtot)*vded));
#if defined(_DERIVATE)
_load_static_jacobian4(drain,drainp,drain,sourcep,((type*gdtot)*vded_Vdrain_sourcep));
_load_static_jacobian4(drain,drainp,sourceb,sourcep,(type*gdtot_Vsourceb_sourcep)*vded);
_load_static_jacobian4(drain,drainp,bulkp,sourcep,(type*gdtot_Vbulkp_sourcep)*vded);
_load_static_jacobian4(drain,drainp,drainb,sourcep,(type*gdtot_Vdrainb_sourcep)*vded);
_load_static_jacobian4(drain,drainp,drainp,sourcep,(((type*gdtot_Vdrainp_sourcep)*vded)+((type*gdtot)*vded_Vdrainp_sourcep)));
_load_static_jacobian4(drain,drainp,gatep,sourcep,(type*gdtot_Vgatep_sourcep)*vded);
#endif
_load_static_residual2(source,sourcep,((type*gstot)*vses));
#if defined(_DERIVATE)
_load_static_jacobian4(source,sourcep,source,sourcep,((type*gstot)*vses_Vsource_sourcep));
_load_static_jacobian4(source,sourcep,sourceb,sourcep,(type*gstot_Vsourceb_sourcep)*vses);
_load_static_jacobian4(source,sourcep,bulkp,sourcep,(type*gstot_Vbulkp_sourcep)*vses);
_load_static_jacobian4(source,sourcep,drainb,sourcep,(type*gstot_Vdrainb_sourcep)*vses);
_load_static_jacobian4(source,sourcep,drainp,sourcep,(type*gstot_Vdrainp_sourcep)*vses);
_load_static_jacobian4(source,sourcep,gatep,sourcep,(type*gstot_Vgatep_sourcep)*vses);
#endif
}
if
((rgatemod==0))
{
_load_static_residual2(gate,gatem,(BP(gate,gatem)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(gate,gatem,gate,gatem,(1e3));
#endif
_load_static_residual2(gatem,gatep,(BP(gatem,gatep)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(gatem,gatep,gatem,gatep,(1e3));
#endif
}
else
{
if
((rgatemod==1))
{
_load_static_residual2(gatep,gatem,(BP(gatep,gatem)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(gatep,gatem,gatep,gatem,(1e3));
#endif
_load_static_residual2(gate,gatem,((type*grgeltd)*vgeg));
#if defined(_DERIVATE)
_load_static_jacobian4(gate,gatem,gatep,sourcep,((type*grgeltd)*vgeg_Vgatep_sourcep));
_load_static_jacobian4(gate,gatem,gate,sourcep,((type*grgeltd)*vgeg_Vgate_sourcep));
#endif
}
else
{
if
((rgatemod==2))
{
_load_static_residual2(gatep,gatem,(BP(gatep,gatem)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(gatep,gatem,gatep,gatem,(1e3));
#endif
_load_static_residual2(gate,gatem,((type*gcrg)*vgeg));
#if defined(_DERIVATE)
_load_static_jacobian4(gate,gatem,gate,sourcep,((type*gcrg)*vgeg_Vgate_sourcep));
_load_static_jacobian4(gate,gatem,sourceb,sourcep,(type*gcrg_Vsourceb_sourcep)*vgeg);
_load_static_jacobian4(gate,gatem,bulkp,sourcep,(type*gcrg_Vbulkp_sourcep)*vgeg);
_load_static_jacobian4(gate,gatem,drainb,sourcep,(type*gcrg_Vdrainb_sourcep)*vgeg);
_load_static_jacobian4(gate,gatem,drainp,sourcep,(type*gcrg_Vdrainp_sourcep)*vgeg);
_load_static_jacobian4(gate,gatem,gatep,sourcep,(((type*gcrg_Vgatep_sourcep)*vgeg)+((type*gcrg)*vgeg_Vgatep_sourcep)));
#endif
}
else
{
_load_static_residual2(gatem,gatep,((type*gcrg)*vgmg));
#if defined(_DERIVATE)
_load_static_jacobian4(gatem,gatep,gatem,sourcep,((type*gcrg)*vgmg_Vgatem_sourcep));
_load_static_jacobian4(gatem,gatep,sourceb,sourcep,(type*gcrg_Vsourceb_sourcep)*vgmg);
_load_static_jacobian4(gatem,gatep,bulkp,sourcep,(type*gcrg_Vbulkp_sourcep)*vgmg);
_load_static_jacobian4(gatem,gatep,drainb,sourcep,(type*gcrg_Vdrainb_sourcep)*vgmg);
_load_static_jacobian4(gatem,gatep,drainp,sourcep,(type*gcrg_Vdrainp_sourcep)*vgmg);
_load_static_jacobian4(gatem,gatep,gatep,sourcep,(((type*gcrg_Vgatep_sourcep)*vgmg)+((type*gcrg)*vgmg_Vgatep_sourcep)));
#endif
_load_static_residual2(gate,gatem,((type*grgeltd)*vgegm));
#if defined(_DERIVATE)
_load_static_jacobian4(gate,gatem,gatem,sourcep,((type*grgeltd)*vgegm_Vgatem_sourcep));
_load_static_jacobian4(gate,gatem,gatep,sourcep,((type*grgeltd)*vgegm_Vgatep_sourcep));
_load_static_jacobian4(gate,gatem,gate,sourcep,((type*grgeltd)*vgegm_Vgate_sourcep));
#endif
}
}
}
if
((rbodymod==0))
{
_load_static_residual2(bulk,bulkp,(BP(bulk,bulkp)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,bulkp,bulk,bulkp,(1e3));
#endif
_load_static_residual2(bulkp,sourceb,(BP(bulkp,sourceb)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(bulkp,sourceb,bulkp,sourceb,(1e3));
#endif
_load_static_residual2(bulkp,drainb,(BP(bulkp,drainb)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(bulkp,drainb,bulkp,drainb,(1e3));
#endif
}
else
{
_load_static_residual2(sourceb,bulkp,(((-type)*grbps)*vbsb));
#if defined(_DERIVATE)
_load_static_jacobian4(sourceb,bulkp,sourceb,sourcep,(((-type)*grbps)*vbsb_Vsourceb_sourcep));
_load_static_jacobian4(sourceb,bulkp,bulkp,sourcep,(((-type)*grbps)*vbsb_Vbulkp_sourcep));
#endif
_load_static_residual2(drainb,bulkp,(((-type)*grbpd)*vbdb));
#if defined(_DERIVATE)
_load_static_jacobian4(drainb,bulkp,drainb,sourcep,(((-type)*grbpd)*vbdb_Vdrainb_sourcep));
_load_static_jacobian4(drainb,bulkp,bulkp,sourcep,(((-type)*grbpd)*vbdb_Vbulkp_sourcep));
#endif
_load_static_residual2(bulk,bulkp,((type*grbpb)*vbeb));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,bulkp,bulkp,sourcep,((type*grbpb)*vbeb_Vbulkp_sourcep));
_load_static_jacobian4(bulk,bulkp,bulk,sourcep,((type*grbpb)*vbeb_Vbulk_sourcep));
#endif
_load_static_residual2(bulk,sourceb,((type*grbsb)*vbesb));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,sourceb,sourceb,sourcep,((type*grbsb)*vbesb_Vsourceb_sourcep));
_load_static_jacobian4(bulk,sourceb,bulk,sourcep,((type*grbsb)*vbesb_Vbulk_sourcep));
#endif
_load_static_residual2(bulk,drainb,((type*grbdb)*vbedb));
#if defined(_DERIVATE)
_load_static_jacobian4(bulk,drainb,drainb,sourcep,((type*grbdb)*vbedb_Vdrainb_sourcep));
_load_static_jacobian4(bulk,drainb,bulk,sourcep,((type*grbdb)*vbedb_Vbulk_sourcep));
#endif
}
#if defined(_DYNAMIC)
_load_dynamic_residual2(gatep,sourcep,(type*_DDT(qgate)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(gatep,sourcep,sourceb,sourcep,(type*qgate_Vsourceb_sourcep));
_load_dynamic_jacobian4(gatep,sourcep,bulkp,sourcep,(type*qgate_Vbulkp_sourcep));
_load_dynamic_jacobian4(gatep,sourcep,drainb,sourcep,(type*qgate_Vdrainb_sourcep));
_load_dynamic_jacobian4(gatep,sourcep,drainp,sourcep,(type*qgate_Vdrainp_sourcep));
_load_dynamic_jacobian4(gatep,sourcep,gatep,sourcep,(type*qgate_Vgatep_sourcep));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(drainp,sourcep,(type*_DDT(qd)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(drainp,sourcep,sourceb,sourcep,(type*qd_Vsourceb_sourcep));
_load_dynamic_jacobian4(drainp,sourcep,bulkp,sourcep,(type*qd_Vbulkp_sourcep));
_load_dynamic_jacobian4(drainp,sourcep,drainb,sourcep,(type*qd_Vdrainb_sourcep));
_load_dynamic_jacobian4(drainp,sourcep,drainp,sourcep,(type*qd_Vdrainp_sourcep));
_load_dynamic_jacobian4(drainp,sourcep,gatep,sourcep,(type*qd_Vgatep_sourcep));
_load_dynamic_jacobian4(drainp,sourcep,gatem,sourcep,(type*qd_Vgatem_sourcep));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bulkp,sourcep,(type*_DDT(qb)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bulkp,sourcep,sourceb,sourcep,(type*qb_Vsourceb_sourcep));
_load_dynamic_jacobian4(bulkp,sourcep,bulkp,sourcep,(type*qb_Vbulkp_sourcep));
_load_dynamic_jacobian4(bulkp,sourcep,drainb,sourcep,(type*qb_Vdrainb_sourcep));
_load_dynamic_jacobian4(bulkp,sourcep,drainp,sourcep,(type*qb_Vdrainp_sourcep));
_load_dynamic_jacobian4(bulkp,sourcep,gatep,sourcep,(type*qb_Vgatep_sourcep));
_load_dynamic_jacobian4(bulkp,sourcep,gatem,sourcep,(type*qb_Vgatem_sourcep));
#endif
#endif
if
((TNOIMOD==0))
{
fourkt=(5.5226012e-23*Temp);
leffx2=(leff*leff);
_save_whitenoise2(drainp,sourcep,((((fourkt*NTNOI)*ueff)*fabs((qd+qs)))/leffx2),"channel");
}
if
((FNOIMOD==0))
{
leffx2=(leff*leff);
_save_flickernoise2(drainp,sourcep,((KF*pow(fabs(cdrain),AF))/(cox*leffx2)),EF,"flicker");
}
_save_whitenoise2(drain,drainp,fabs((fourkt*gdtot)),"thermal");
_save_whitenoise2(sourcep,source,fabs((fourkt*gstot)),"thermal");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void bsim4v30nMOS::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 11; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 11; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void bsim4v30nMOS::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void bsim4v30nMOS::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void bsim4v30nMOS::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void bsim4v30nMOS::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void bsim4v30nMOS::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix bsim4v30nMOS::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (11);

  for (int i1 = 0; i1 < 11; i1++) {
    for (int i2 = 0; i2 < 11; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void bsim4v30nMOS::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void bsim4v30nMOS::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void bsim4v30nMOS::initTR (void)
{
  setStates (2 * 11 * 11);
  initDC ();
}

/* Perform transient analysis iteration step. */
void bsim4v30nMOS::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 11; i1++) {
  for (i2 = 0; i2 < 11; i2++) {
    state = 2 * (i2 + 11 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 11; i1++) {
    state = 2 * (i1 + 11 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 11; i1++) {
  for (i2 = 0; i2 < 11; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 11; i3++) {
  for (i4 = 0; i4 < 11; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 11; i1++) {
  for (i2 = 0; i2 < 11; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 11; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 11; i1++) {
  for (i3 = 0; i3 < 11; i3++) {
  for (i4 = 0; i4 < 11; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 11; i1++) {
  for (i3 = 0; i3 < 11; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix bsim4v30nMOS::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (11);

  _load_flickernoise2 (drainp,sourcep,_flicker_pwr[drainp][sourcep],_flicker_exp[drainp][sourcep]);
  _load_whitenoise2 (sourcep,source,_white_pwr[sourcep][source]);
  _load_whitenoise2 (drain,drainp,_white_pwr[drain][drainp]);
  _load_whitenoise2 (drainp,sourcep,_white_pwr[drainp][sourcep]);

  return cy;
}

/* Perform AC noise computations. */
void bsim4v30nMOS::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void bsim4v30nMOS::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void bsim4v30nMOS::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void bsim4v30nMOS::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 11; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 11; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "bsim4v30nMOS.defs.h"
