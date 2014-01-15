/*
 * HBT_X.core.cpp - device implementations for HBT_X module
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

#include "HBT_X.analogfunction.h"
#include "component.h"
#include "device.h"
#include "HBT_X.core.h"

#ifndef CIR_HBT_X
#define CIR_HBT_X -1
#endif

// external nodes
#define c 0
#define b 1
#define e 2
#define t 3
// internal nodes
#define ei 4
#define bi 5
#define bii 6
#define ci 7
#define ex 8
#define exx 9
#define cx 10
#define ni 11
#define nii 12
#define niix 13
#define niii 14
#define niiix 15
#define niv 16
#define nivx 17
#define ti 18

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
#define _modelname              "HBT_X"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
HBT_X::HBT_X() : circuit (19)
{
  type = CIR_HBT_X;
}

/* Initialization of model. */
void HBT_X::initModel (void)
{
  // create internal nodes
  setInternalNode (ei, "ei");
  setInternalNode (bi, "bi");
  setInternalNode (bii, "bii");
  setInternalNode (ci, "ci");
  setInternalNode (ex, "ex");
  setInternalNode (exx, "exx");
  setInternalNode (cx, "cx");
  setInternalNode (ni, "ni");
  setInternalNode (nii, "nii");
  setInternalNode (niix, "niix");
  setInternalNode (niii, "niii");
  setInternalNode (niiix, "niiix");
  setInternalNode (niv, "niv");
  setInternalNode (nivx, "nivx");
  setInternalNode (ti, "ti");

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
void HBT_X::initDC (void)
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
void HBT_X::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void HBT_X::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[nii][nii] = 0.0;
  _white_pwr[cx][bii] = 0.0;
  _white_pwr[exx][bii] = 0.0;
  _white_pwr[ex][bii] = 0.0;
  _white_pwr[c][ci] = 0.0;
  _white_pwr[e][ei] = 0.0;
  _white_pwr[b][bi] = 0.0;
  _white_pwr[bii][bi] = 0.0;
  _white_pwr[ni][ni] = 0.0;
  _white_pwr[bii][ei] = 0.0;
  _flicker_pwr[niv][niv] = 0.0;
  _flicker_exp[niv][niv] = 0.0;
  _flicker_pwr[niii][niii] = 0.0;
  _flicker_exp[niii][niii] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 19; i1++) {
  for (i2 = 0; i2 < 19; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 19; i1++) {
  for (i2 = 0; i2 < 19; i2++) {
  for (i3 = 0; i3 < 19; i3++) {
  for (i4 = 0; i4 < 19; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 19; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 19; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void HBT_X::loadVariables (void)
{
  Mode = getPropertyInteger ("Mode");
  Noise = getPropertyInteger ("Noise");
  Debug = getPropertyInteger ("Debug");
  DebugPlus = getPropertyInteger ("DebugPlus");
  Temp = getPropertyDouble ("Temp");
  Rth = getPropertyDouble ("Rth");
  Cth = getPropertyDouble ("Cth");
  N = getPropertyInteger ("N");
  L = getPropertyDouble ("L");
  W = getPropertyDouble ("W");
  Jsf = getPropertyDouble ("Jsf");
  nf = getPropertyDouble ("nf");
  Vg = getPropertyDouble ("Vg");
  Jse = getPropertyDouble ("Jse");
  ne = getPropertyDouble ("ne");
  Rbxx = getPropertyDouble ("Rbxx");
  Vgb = getPropertyDouble ("Vgb");
  Jsee = getPropertyDouble ("Jsee");
  nee = getPropertyDouble ("nee");
  Rbbxx = getPropertyDouble ("Rbbxx");
  Vgbb = getPropertyDouble ("Vgbb");
  Jsr = getPropertyDouble ("Jsr");
  nr = getPropertyDouble ("nr");
  Vgr = getPropertyDouble ("Vgr");
  XCjc = getPropertyDouble ("XCjc");
  Jsc = getPropertyDouble ("Jsc");
  nc = getPropertyDouble ("nc");
  Rcxx = getPropertyDouble ("Rcxx");
  Vgc = getPropertyDouble ("Vgc");
  Bf = getPropertyDouble ("Bf");
  kBeta = getPropertyDouble ("kBeta");
  Br = getPropertyDouble ("Br");
  VAF = getPropertyDouble ("VAF");
  VAR = getPropertyDouble ("VAR");
  IKF = getPropertyDouble ("IKF");
  IKR = getPropertyDouble ("IKR");
  Mc = getPropertyDouble ("Mc");
  BVceo = getPropertyDouble ("BVceo");
  kc = getPropertyDouble ("kc");
  BVebo = getPropertyDouble ("BVebo");
  Tr = getPropertyDouble ("Tr");
  Trx = getPropertyDouble ("Trx");
  Tf = getPropertyDouble ("Tf");
  Tft = getPropertyDouble ("Tft");
  Thcs = getPropertyDouble ("Thcs");
  Ahc = getPropertyDouble ("Ahc");
  Cje = getPropertyDouble ("Cje");
  mje = getPropertyDouble ("mje");
  Vje = getPropertyDouble ("Vje");
  Cjc = getPropertyDouble ("Cjc");
  mjc = getPropertyDouble ("mjc");
  Vjc = getPropertyDouble ("Vjc");
  kjc = getPropertyDouble ("kjc");
  Cmin = getPropertyDouble ("Cmin");
  J0 = getPropertyDouble ("J0");
  XJ0 = getPropertyDouble ("XJ0");
  Rci0 = getPropertyDouble ("Rci0");
  Jk = getPropertyDouble ("Jk");
  RJk = getPropertyDouble ("RJk");
  Vces = getPropertyDouble ("Vces");
  Rc = getPropertyDouble ("Rc");
  Re = getPropertyDouble ("Re");
  Rb = getPropertyDouble ("Rb");
  Rb2 = getPropertyDouble ("Rb2");
  Lc = getPropertyDouble ("Lc");
  Le = getPropertyDouble ("Le");
  Lb = getPropertyDouble ("Lb");
  Cq = getPropertyDouble ("Cq");
  Cpb = getPropertyDouble ("Cpb");
  Cpc = getPropertyDouble ("Cpc");
  Kfb = getPropertyDouble ("Kfb");
  Afb = getPropertyDouble ("Afb");
  Ffeb = getPropertyDouble ("Ffeb");
  Kb = getPropertyDouble ("Kb");
  Ab = getPropertyDouble ("Ab");
  Fb = getPropertyDouble ("Fb");
  Kfe = getPropertyDouble ("Kfe");
  Afe = getPropertyDouble ("Afe");
  Ffee = getPropertyDouble ("Ffee");
  Tnom = getPropertyDouble ("Tnom");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void HBT_X::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void HBT_X::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void HBT_X::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void HBT_X::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void HBT_X::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double Inivx;
#if defined(_DERIVATE)
double Inivx_Vbii_ei;
double Inivx_Tempti_GND;
double Inivx_Vbii_ci;
#endif
double Iniiix;
#if defined(_DERIVATE)
double Iniiix_Vbii_ei;
double Iniiix_Tempti_GND;
double Iniiix_Vbii_ci;
#endif
double Iniix;
#if defined(_DERIVATE)
double Iniix_Vbii_ei;
double Iniix_Tempti_GND;
double Iniix_Vbii_ci;
#endif
#if defined(_DYNAMIC)
double qbe;
#if defined(_DERIVATE)
double qbe_Vbii_ei;
double qbe_Tempti_GND;
double qbe_Vbii_ci;
double qbe_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double qbtf;
#if defined(_DERIVATE)
double qbtf_Tempti_GND;
double qbtf_Vbii_ei;
double qbtf_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double xtff;
#if defined(_DERIVATE)
double xtff_Vbii_ei;
double xtff_Tempti_GND;
double xtff_Vbii_ci;
double xtff_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double Wh;
#if defined(_DERIVATE)
double Wh_Vci_ei;
double Wh_Vbii_ei;
double Wh_Tempti_GND;
double Wh_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double Ih;
#if defined(_DERIVATE)
double Ih_Vci_ei;
double Ih_Vbii_ei;
double Ih_Tempti_GND;
double Ih_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double AHC;
#endif
#if defined(_DYNAMIC)
double RCIO;
#endif
#if defined(_DYNAMIC)
double InvVpt;
#endif
#if defined(_DYNAMIC)
double Vlim;
#endif
#if defined(_DYNAMIC)
double qb2;
#if defined(_DERIVATE)
double qb2_Vbii_ei;
double qb2_Tempti_GND;
double qb2_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double qbtra;
#if defined(_DERIVATE)
double qbtra_Vbi_ci;
double qbtra_Tempti_GND;
#endif
#endif
#if defined(_DYNAMIC)
double qbtr;
#if defined(_DERIVATE)
double qbtr_Vbii_ci;
double qbtr_Tempti_GND;
double qbtr_Vbii_ei;
#endif
#endif
#if defined(_DYNAMIC)
double qb1;
#if defined(_DERIVATE)
double qb1_Vbi_ci;
#endif
#endif
#if defined(_DYNAMIC)
double xix;
#if defined(_DERIVATE)
double xix_Vbii_ei;
double xix_Tempti_GND;
double xix_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double qb2med;
#if defined(_DERIVATE)
double qb2med_Vbii_ei;
double qb2med_Tempti_GND;
double qb2med_Vbii_ci;
#endif
#endif
#if defined(_DYNAMIC)
double I00;
#endif
double Ipdiss;
#if defined(_DERIVATE)
double Ipdiss_Vbii_ei;
double Ipdiss_Tempti_GND;
double Ipdiss_Vbii_ci;
double Ipdiss_Vci_ei;
double Ipdiss_Vbi_ci;
#endif
double Icdx;
#if defined(_DERIVATE)
double Icdx_Vcx_ci;
double Icdx_Tempti_GND;
#endif
double Ibdxx;
#if defined(_DERIVATE)
double Ibdxx_Vexx_ei;
double Ibdxx_Tempti_GND;
#endif
double Ibdx;
#if defined(_DERIVATE)
double Ibdx_Vex_ei;
double Ibdx_Tempti_GND;
#endif
double Ib1;
#if defined(_DERIVATE)
double Ib1_Vbii_ei;
double Ib1_Tempti_GND;
double Ib1_Vbii_ci;
#endif
double Ib0;
#if defined(_DERIVATE)
double Ib0_Vbii_ei;
double Ib0_Tempti_GND;
double Ib0_Vbii_ci;
#endif
double EdBeta;
#if defined(_DERIVATE)
double EdBeta_Tempti_GND;
double EdBeta_Vbii_ci;
#endif
double mm;
#if defined(_DERIVATE)
double mm_Vbii_ci;
#endif
double Vbclin;
double epsi;
double Ibx;
#if defined(_DERIVATE)
double Ibx_Vbi_ci;
double Ibx_Tempti_GND;
#endif
double Ib2;
#if defined(_DERIVATE)
double Ib2_Vbii_ci;
double Ib2_Tempti_GND;
#endif
double Ic1;
#if defined(_DERIVATE)
double Ic1_Vbii_ei;
double Ic1_Tempti_GND;
double Ic1_Vbii_ci;
#endif
double Ic1r;
#if defined(_DERIVATE)
double Ic1r_Vbii_ci;
double Ic1r_Tempti_GND;
double Ic1r_Vbii_ei;
#endif
double Ic0;
#if defined(_DERIVATE)
double Ic0_Vbii_ei;
double Ic0_Tempti_GND;
double Ic0_Vbii_ci;
#endif
double qb;
#if defined(_DERIVATE)
double qb_Vbii_ei;
double qb_Vbii_ci;
double qb_Tempti_GND;
#endif
double q2;
#if defined(_DERIVATE)
double q2_Vbii_ei;
double q2_Tempti_GND;
double q2_Vbii_ci;
#endif
double q1;
#if defined(_DERIVATE)
double q1_Vbii_ei;
double q1_Vbii_ci;
#endif
double Ic1ra;
#if defined(_DERIVATE)
double Ic1ra_Vbii_ci;
double Ic1ra_Tempti_GND;
#endif
double Ic0a;
#if defined(_DERIVATE)
double Ic0a_Vbii_ei;
double Ic0a_Tempti_GND;
#endif
double TWO_Q;
double FOUR_K;
double Area;
double Tex;
#if defined(_DERIVATE)
double Tex_Tempti_GND;
#endif
double TjK;
#if defined(_DERIVATE)
double TjK_Tempti_GND;
#endif
double Tj;
#if defined(_DERIVATE)
double Tj_Tempti_GND;
#endif
double Texi;
#if defined(_DERIVATE)
double Texi_Tempti_GND;
#endif
double vcei;
#if defined(_DERIVATE)
double vcei_Vci_ei;
#endif
double vxxe;
#if defined(_DERIVATE)
double vxxe_Vexx_ei;
#endif
double vxc;
#if defined(_DERIVATE)
double vxc_Vcx_ci;
#endif
double vxe;
#if defined(_DERIVATE)
double vxe_Vex_ei;
#endif
double vbei;
#if defined(_DERIVATE)
double vbei_Vbii_ei;
#endif
double vbci;
#if defined(_DERIVATE)
double vbci_Vbii_ci;
#endif
double vbcx;
#if defined(_DERIVATE)
double vbcx_Vbi_ci;
#endif
#if defined(_DERIVATE)
vbcx_Vbi_ci=1.0;
#endif
vbcx=BP(bi,ci);
#if defined(_DERIVATE)
vbci_Vbii_ci=1.0;
#endif
vbci=BP(bii,ci);
#if defined(_DERIVATE)
vbei_Vbii_ei=1.0;
#endif
vbei=BP(bii,ei);
#if defined(_DERIVATE)
vxe_Vex_ei=1.0;
#endif
vxe=BP(ex,ei);
#if defined(_DERIVATE)
vxc_Vcx_ci=1.0;
#endif
vxc=BP(cx,ci);
#if defined(_DERIVATE)
vxxe_Vexx_ei=1.0;
#endif
vxxe=BP(exx,ei);
#if defined(_DERIVATE)
vcei_Vci_ei=1.0;
#endif
vcei=BP(ci,ei);
#if defined(_DERIVATE)
Texi_Tempti_GND=1.0;
#endif
Texi=NP(ti);
#if defined(_DERIVATE)
Tj_Tempti_GND=Texi_Tempti_GND;
#endif
Tj=(Texi+Temp);
#if defined(_DERIVATE)
TjK_Tempti_GND=Tj_Tempti_GND;
#endif
TjK=(Tj+273.15);
#if defined(_DERIVATE)
Tex_Tempti_GND=Tj_Tempti_GND;
#endif
Tex=(Tj-Tnom);
Area=(((L*W)*1.0e12)*N);
FOUR_K=(4*1.3806226e-23);
TWO_Q=(2*1.6021918e-19);
#if defined(_DERIVATE)
Ic0a_Vbii_ei=HBT_X_d_diode(vbei,Jsf,Vg,nf,Area,Tj,Tnom,vbei_Vbii_ei,0.0,0.0,0.0,0.0,0.0,0.0);
Ic0a_Tempti_GND=HBT_X_d_diode(vbei,Jsf,Vg,nf,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0);
#endif
Ic0a=HBT_X_diode(vbei,Jsf,Vg,nf,Area,Tj,Tnom);
#if defined(_DERIVATE)
Ic1ra_Vbii_ci=HBT_X_d_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom,vbci_Vbii_ci,0.0,0.0,0.0,0.0,0.0,0.0);
Ic1ra_Tempti_GND=HBT_X_d_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0);
#endif
Ic1ra=HBT_X_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom);
if
(((VAF>0.0)&&(VAR>0.0)))
{
#if defined(_DERIVATE)
q1_Vbii_ei=(((HBT_X_d_charge(vbei,1.0,Vje,mje,1.0,vbei_Vbii_ei,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vje,mje,1.0,0.0,0.0,0.0,0.0,0.0))/VAR)+((HBT_X_d_charge(vbci,1.0,Vjc,mjc,1.0,0.0,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vjc,mjc,1.0,0.0,0.0,0.0,0.0,0.0))/VAF));
q1_Vbii_ci=(((HBT_X_d_charge(vbei,1.0,Vje,mje,1.0,0.0,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vje,mje,1.0,0.0,0.0,0.0,0.0,0.0))/VAR)+((HBT_X_d_charge(vbci,1.0,Vjc,mjc,1.0,vbci_Vbii_ci,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vjc,mjc,1.0,0.0,0.0,0.0,0.0,0.0))/VAF));
#endif
q1=((1.0+((HBT_X_charge(vbei,1.0,Vje,mje,1.0)-HBT_X_charge(0.0,1.0,Vje,mje,1.0))/VAR))+((HBT_X_charge(vbci,1.0,Vjc,mjc,1.0)-HBT_X_charge(0.0,1.0,Vjc,mjc,1.0))/VAF));
}
else
{
if
(((VAF>0.0)&&(VAR==0.0)))
{
#if defined(_DERIVATE)
q1_Vbii_ei=0.0;
q1_Vbii_ci=((HBT_X_d_charge(vbci,1.0,Vjc,mjc,1.0,vbci_Vbii_ci,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vjc,mjc,1.0,0.0,0.0,0.0,0.0,0.0))/VAF);
#endif
q1=(1.0+((HBT_X_charge(vbci,1.0,Vjc,mjc,1.0)-HBT_X_charge(0.0,1.0,Vjc,mjc,1.0))/VAF));
}
else
{
if
(((VAF==0.0)&&(VAR>0.0)))
{
#if defined(_DERIVATE)
q1_Vbii_ei=((HBT_X_d_charge(vbei,1.0,Vje,mje,1.0,vbei_Vbii_ei,0.0,0.0,0.0,0.0)-HBT_X_d_charge(0.0,1.0,Vje,mje,1.0,0.0,0.0,0.0,0.0,0.0))/VAR);
q1_Vbii_ci=0.0;
#endif
q1=(1.0+((HBT_X_charge(vbei,1.0,Vje,mje,1.0)-HBT_X_charge(0.0,1.0,Vje,mje,1.0))/VAR));
}
else
{
#if defined(_DERIVATE)
q1_Vbii_ei=0.0;
q1_Vbii_ci=0.0;
#endif
q1=1.0;
}
}
}
if
(((IKF>0.0)&&(IKR>0.0)))
{
#if defined(_DERIVATE)
q2_Vbii_ei=(Ic0a_Vbii_ei/(Area*IKF));
q2_Tempti_GND=((Ic0a_Tempti_GND/(Area*IKF))+(Ic1ra_Tempti_GND/(Area*IKR)));
q2_Vbii_ci=(Ic1ra_Vbii_ci/(Area*IKR));
#endif
q2=((Ic0a/(Area*IKF))+(Ic1ra/(Area*IKR)));
}
else
{
if
(((IKF>0.0)&&(IKR==0.0)))
{
#if defined(_DERIVATE)
q2_Vbii_ei=(Ic0a_Vbii_ei/(Area*IKF));
q2_Tempti_GND=(Ic0a_Tempti_GND/(Area*IKF));
q2_Vbii_ci=0.0;
#endif
q2=(Ic0a/(Area*IKF));
}
else
{
if
(((IKF==0.0)&&(IKR>0.0)))
{
#if defined(_DERIVATE)
q2_Vbii_ei=0.0;
q2_Tempti_GND=(Ic1ra_Tempti_GND/(Area*IKR));
q2_Vbii_ci=(Ic1ra_Vbii_ci/(Area*IKR));
#endif
q2=(Ic1ra/(Area*IKR));
}
else
{
#if defined(_DERIVATE)
q2_Vbii_ei=0.0;
q2_Tempti_GND=0.0;
q2_Vbii_ci=0.0;
#endif
q2=0.0;
}
}
}
{
double m00_sqrt(d00_sqrt0,((q1*q1)+(4.0*q2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((q1*q1)+(4.0*q2)))
#endif
#if defined(_DERIVATE)
qb_Vbii_ei=((q1_Vbii_ei+(((q1_Vbii_ei*q1)+(q1*q1_Vbii_ei))+(4.0*q2_Vbii_ei))*d10_sqrt0)/2.0);
qb_Vbii_ci=((q1_Vbii_ci+(((q1_Vbii_ci*q1)+(q1*q1_Vbii_ci))+(4.0*q2_Vbii_ci))*d10_sqrt0)/2.0);
qb_Tempti_GND=((4.0*q2_Tempti_GND)*d10_sqrt0/2.0);
#endif
qb=((q1+d00_sqrt0)/2.0);
}
#if defined(_DERIVATE)
Ic0_Vbii_ei=((Ic0a_Vbii_ei*qb-Ic0a*qb_Vbii_ei)/qb/qb);
Ic0_Tempti_GND=((Ic0a_Tempti_GND*qb-Ic0a*qb_Tempti_GND)/qb/qb);
Ic0_Vbii_ci=(-Ic0a*qb_Vbii_ci/qb/qb);
#endif
Ic0=(Ic0a/qb);
#if defined(_DERIVATE)
Ic1r_Vbii_ci=((Ic1ra_Vbii_ci*qb-Ic1ra*qb_Vbii_ci)/qb/qb);
Ic1r_Tempti_GND=((Ic1ra_Tempti_GND*qb-Ic1ra*qb_Tempti_GND)/qb/qb);
Ic1r_Vbii_ei=(-Ic1ra*qb_Vbii_ei/qb/qb);
#endif
Ic1r=(Ic1ra/qb);
#if defined(_DERIVATE)
Ic1_Vbii_ei=(Ic0_Vbii_ei-Ic1r_Vbii_ei);
Ic1_Tempti_GND=(Ic0_Tempti_GND-Ic1r_Tempti_GND);
Ic1_Vbii_ci=(Ic0_Vbii_ci-Ic1r_Vbii_ci);
#endif
Ic1=(Ic0-Ic1r);
#if defined(_DERIVATE)
Ib2_Vbii_ci=(HBT_X_d_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom,vbci_Vbii_ci,0.0,0.0,0.0,0.0,0.0,0.0)/Br);
Ib2_Tempti_GND=(HBT_X_d_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0)/Br);
#endif
Ib2=(HBT_X_diode(vbci,(XCjc*Jsr),Vgr,nr,Area,Tj,Tnom)/Br);
#if defined(_DERIVATE)
Ibx_Vbi_ci=(HBT_X_d_diode(vbcx,((1.0-XCjc)*Jsr),Vgr,nr,Area,Tj,Tnom,vbcx_Vbi_ci,0.0,0.0,0.0,0.0,0.0,0.0)/Br);
Ibx_Tempti_GND=(HBT_X_d_diode(vbcx,((1.0-XCjc)*Jsr),Vgr,nr,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0)/Br);
#endif
Ibx=(HBT_X_diode(vbcx,((1.0-XCjc)*Jsr),Vgr,nr,Area,Tj,Tnom)/Br);
epsi=1.0e-6;
{
double m00_pow(d00_pow0,(1.0-epsi),(1/Mc))
Vbclin=(BVceo*d00_pow0);
}
#if defined(_DERIVATE)
mm_Vbii_ci=HBT_X_d_MM(vbci,BVceo,Mc,Vbclin,Bf,kc,vbci_Vbii_ci,0.0,0.0,0.0,0.0,0.0);
#endif
mm=HBT_X_MM(vbci,BVceo,Mc,Vbclin,Bf,kc);
if
((mm>1.0))
{
if
((kBeta>0.0))
{
if
(((Bf-(kBeta*Tex))>1e-6))
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=((-1*(-(kBeta*Tex_Tempti_GND))/(Bf-(kBeta*Tex))/(Bf-(kBeta*Tex)))/((kc*(mm-1))+1));
EdBeta_Vbii_ci=(((-(kc*mm_Vbii_ci))*((kc*(mm-1))+1)-((1/(Bf-(kBeta*Tex)))-(kc*(mm-1)))*(kc*mm_Vbii_ci))/((kc*(mm-1))+1)/((kc*(mm-1))+1));
#endif
EdBeta=(((1/(Bf-(kBeta*Tex)))-(kc*(mm-1)))/((kc*(mm-1))+1));
}
else
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=0.0;
EdBeta_Vbii_ci=(((-(kc*mm_Vbii_ci))*((kc*(mm-1))+1)-(1e6-(kc*(mm-1)))*(kc*mm_Vbii_ci))/((kc*(mm-1))+1)/((kc*(mm-1))+1));
#endif
EdBeta=((1e6-(kc*(mm-1)))/((kc*(mm-1))+1));
}
}
else
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=0.0;
EdBeta_Vbii_ci=(((-(kc*mm_Vbii_ci))*((kc*(mm-1))+1)-((1/Bf)-(kc*(mm-1)))*(kc*mm_Vbii_ci))/((kc*(mm-1))+1)/((kc*(mm-1))+1));
#endif
EdBeta=(((1/Bf)-(kc*(mm-1)))/((kc*(mm-1))+1));
}
}
else
{
if
((kBeta>0.0))
{
if
(((Bf-(kBeta*Tex))>1e-6))
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=(-1*(-(kBeta*Tex_Tempti_GND))/(Bf-(kBeta*Tex))/(Bf-(kBeta*Tex)));
EdBeta_Vbii_ci=0.0;
#endif
EdBeta=(1/(Bf-(kBeta*Tex)));
}
else
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=0.0;
EdBeta_Vbii_ci=0.0;
#endif
EdBeta=1e6;
}
}
else
{
#if defined(_DERIVATE)
EdBeta_Tempti_GND=0.0;
EdBeta_Vbii_ci=0.0;
#endif
EdBeta=(1/Bf);
}
}
#if defined(_DERIVATE)
Ib0_Vbii_ei=Ic0a_Vbii_ei*EdBeta;
Ib0_Tempti_GND=((Ic0a_Tempti_GND*EdBeta)+(Ic0a*EdBeta_Tempti_GND));
Ib0_Vbii_ci=(Ic0a*EdBeta_Vbii_ci);
#endif
Ib0=(Ic0a*EdBeta);
if
((BVebo>0))
{
#if defined(_DERIVATE)
Ib1_Vbii_ei=(Ib0_Vbii_ei-HBT_X_d_diode(((-BVebo)-vbei),Jsf,0.0,1.0,Area,0.0,0.0,(-vbei_Vbii_ei),0.0,0.0,0.0,0.0,0.0,0.0));
Ib1_Tempti_GND=(Ib0_Tempti_GND-HBT_X_d_diode(((-BVebo)-vbei),Jsf,0.0,1.0,Area,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0));
Ib1_Vbii_ci=(Ib0_Vbii_ci-HBT_X_d_diode(((-BVebo)-vbei),Jsf,0.0,1.0,Area,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0));
#endif
Ib1=(Ib0-HBT_X_diode(((-BVebo)-vbei),Jsf,0.0,1.0,Area,0.0,0.0));
}
else
{
#if defined(_DERIVATE)
Ib1_Vbii_ei=Ib0_Vbii_ei;
Ib1_Tempti_GND=Ib0_Tempti_GND;
Ib1_Vbii_ci=Ib0_Vbii_ci;
#endif
Ib1=Ib0;
}
if
(((Jse>0.0)&&(ne>0)))
{
#if defined(_DERIVATE)
Ibdx_Vex_ei=HBT_X_d_diode(vxe,Jse,Vgb,ne,Area,Tj,Tnom,vxe_Vex_ei,0.0,0.0,0.0,0.0,0.0,0.0);
Ibdx_Tempti_GND=HBT_X_d_diode(vxe,Jse,Vgb,ne,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0);
#endif
Ibdx=HBT_X_diode(vxe,Jse,Vgb,ne,Area,Tj,Tnom);
}
else
{
#if defined(_DERIVATE)
Ibdx_Vex_ei=vxe_Vex_ei*1e-12;
Ibdx_Tempti_GND=0.0;
#endif
Ibdx=(vxe*1e-12);
}
if
(((Jsee>0.0)&&(nee>0)))
{
#if defined(_DERIVATE)
Ibdxx_Vexx_ei=HBT_X_d_diode(vxxe,Jsee,Vgbb,nee,Area,Tj,Tnom,vxxe_Vexx_ei,0.0,0.0,0.0,0.0,0.0,0.0);
Ibdxx_Tempti_GND=HBT_X_d_diode(vxxe,Jsee,Vgbb,nee,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0);
#endif
Ibdxx=HBT_X_diode(vxxe,Jsee,Vgbb,nee,Area,Tj,Tnom);
}
else
{
#if defined(_DERIVATE)
Ibdxx_Vexx_ei=vxxe_Vexx_ei*1e-12;
Ibdxx_Tempti_GND=0.0;
#endif
Ibdxx=(vxxe*1e-12);
}
if
(((Jsc>0.0)&&(nc>0)))
{
#if defined(_DERIVATE)
Icdx_Vcx_ci=HBT_X_d_diode(vxc,Jsc,Vgc,nc,Area,Tj,Tnom,vxc_Vcx_ci,0.0,0.0,0.0,0.0,0.0,0.0);
Icdx_Tempti_GND=HBT_X_d_diode(vxc,Jsc,Vgc,nc,Area,Tj,Tnom,0.0,0.0,0.0,0.0,0.0,Tj_Tempti_GND,0.0);
#endif
Icdx=HBT_X_diode(vxc,Jsc,Vgc,nc,Area,Tj,Tnom);
}
else
{
#if defined(_DERIVATE)
Icdx_Vcx_ci=vxc_Vcx_ci*1e-12;
Icdx_Tempti_GND=0.0;
#endif
Icdx=(vxc*1e-12);
}
#if defined(_DERIVATE)
Ipdiss_Vbii_ei=(Ic1_Vbii_ei*vcei+((Ib1_Vbii_ei*vbei)+(Ib1*vbei_Vbii_ei)));
Ipdiss_Tempti_GND=(((Ic1_Tempti_GND*vcei+Ib1_Tempti_GND*vbei)+Ib2_Tempti_GND*vbci)+Ibx_Tempti_GND*vbcx);
Ipdiss_Vbii_ci=((Ic1_Vbii_ci*vcei+Ib1_Vbii_ci*vbei)+((Ib2_Vbii_ci*vbci)+(Ib2*vbci_Vbii_ci)));
Ipdiss_Vci_ei=(Ic1*vcei_Vci_ei);
Ipdiss_Vbi_ci=((Ibx_Vbi_ci*vbcx)+(Ibx*vbcx_Vbi_ci));
#endif
Ipdiss=((((Ic1*vcei)+(Ib1*vbei))+(Ib2*vbci))+(Ibx*vbcx));
if
((Ipdiss<0.0))
{
#if defined(_DERIVATE)
Ipdiss_Vbii_ei=0.0;
Ipdiss_Tempti_GND=0.0;
Ipdiss_Vbii_ci=0.0;
Ipdiss_Vci_ei=0.0;
Ipdiss_Vbi_ci=0.0;
#endif
Ipdiss=0;
}
#if defined(_DYNAMIC)
I00=(J0*Area);
#endif
#ifdef _DYNAMIC
if
(((XCjc<1.0)&&(XCjc>0.0)))
{
#ifdef _DYNAMIC
if
(((J0<=0.0)||(Ic0<0.0)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb2med_Vbii_ei=0.0;
qb2med_Tempti_GND=0.0;
qb2med_Vbii_ci=((XCjc*HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,vbci_Vbii_ci,0.0,0.0,0.0,0.0))+(((XCjc*Area)*Cmin)*vbci_Vbii_ci));
#endif
qb2med=((XCjc*HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area))+(((XCjc*Area)*Cmin)*vbci));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
xix_Vbii_ei=(Ic0_Vbii_ei/I00);
xix_Tempti_GND=(Ic0_Tempti_GND/I00);
xix_Vbii_ci=(Ic0_Vbii_ci/I00);
#endif
xix=(Ic0/I00);
#endif
#if defined(_DYNAMIC)
{
double m00_tanh(d00_tanh0,xix)
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,xix)
#endif
#if defined(_DERIVATE)
qb2med_Vbii_ei=(((XCjc*(-xix_Vbii_ei*d10_tanh0))*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((XCjc*(1.0-d00_tanh0))*HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,0.0,0.0,0.0,0.0,0.0)));
qb2med_Tempti_GND=(((XCjc*(-xix_Tempti_GND*d10_tanh0))*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((XCjc*(1.0-d00_tanh0))*HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,0.0,0.0,0.0,0.0,0.0)));
qb2med_Vbii_ci=((((XCjc*(-xix_Vbii_ci*d10_tanh0))*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((XCjc*(1.0-d00_tanh0))*(HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,vbci_Vbii_ci,0.0,0.0,0.0,0.0)+((((1.0-XJ0)*Area)*Cmin)*vbci_Vbii_ci))))+((((XJ0*XCjc)*Area)*Cmin)*vbci_Vbii_ci));
#endif
qb2med=(((XCjc*(1.0-d00_tanh0))*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((((XJ0*XCjc)*Area)*Cmin)*vbci));
}
#endif
}
#endif /* if (...) */
}
else
{
#ifdef _DYNAMIC
if
(((J0<0.0)||(Ic0<0.0)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb2med_Vbii_ei=0.0;
qb2med_Tempti_GND=0.0;
qb2med_Vbii_ci=(HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,vbci_Vbii_ci,0.0,0.0,0.0,0.0)+((Area*Cmin)*vbci_Vbii_ci));
#endif
qb2med=(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((Area*Cmin)*vbci));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
xix_Vbii_ei=(Ic0_Vbii_ei/I00);
xix_Tempti_GND=(Ic0_Tempti_GND/I00);
xix_Vbii_ci=(Ic0_Vbii_ci/I00);
#endif
xix=(Ic0/I00);
#endif
#if defined(_DYNAMIC)
{
double m00_tanh(d00_tanh0,xix)
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,xix)
#endif
#if defined(_DERIVATE)
qb2med_Vbii_ei=(((-xix_Vbii_ei*d10_tanh0)*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((1.0-d00_tanh0)*HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,0.0,0.0,0.0,0.0,0.0)));
qb2med_Tempti_GND=(((-xix_Tempti_GND*d10_tanh0)*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((1.0-d00_tanh0)*HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,0.0,0.0,0.0,0.0,0.0)));
qb2med_Vbii_ci=((((-xix_Vbii_ci*d10_tanh0)*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+((1.0-d00_tanh0)*(HBT_X_d_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area,vbci_Vbii_ci,0.0,0.0,0.0,0.0)+((((1.0-XJ0)*Area)*Cmin)*vbci_Vbii_ci))))+(((XJ0*Area)*Cmin)*vbci_Vbii_ci));
#endif
qb2med=(((1.0-d00_tanh0)*(HBT_X_charge(vbci,(Cjc-Cmin),Vjc,mjc,Area)+((((1.0-XJ0)*Area)*Cmin)*vbci)))+(((XJ0*Area)*Cmin)*vbci));
}
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
(((XCjc<1.0)&&(XCjc>0.0)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb1_Vbi_ci=(((1.0-XCjc)*HBT_X_d_charge(vbcx,(Cjc-Cmin),Vjc,mjc,Area,vbcx_Vbi_ci,0.0,0.0,0.0,0.0))+((((1.0-XCjc)*Area)*Cmin)*vbcx_Vbi_ci));
#endif
qb1=(((1.0-XCjc)*HBT_X_charge(vbcx,(Cjc-Cmin),Vjc,mjc,Area))+((((1.0-XCjc)*Area)*Cmin)*vbcx));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb1_Vbi_ci=0.0;
#endif
qb1=0.0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbtr_Vbii_ci=(Tr*Ic1r_Vbii_ci);
qbtr_Tempti_GND=(Tr*Ic1r_Tempti_GND);
qbtr_Vbii_ei=(Tr*Ic1r_Vbii_ei);
#endif
qbtr=(Tr*Ic1r);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbtra_Vbi_ci=(Trx*Ibx_Vbi_ci);
qbtra_Tempti_GND=(Trx*Ibx_Tempti_GND);
#endif
qbtra=(Trx*Ibx);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb2_Vbii_ei=(qb2med_Vbii_ei+qbtr_Vbii_ei);
qb2_Tempti_GND=(qb2med_Tempti_GND+qbtr_Tempti_GND);
qb2_Vbii_ci=(qb2med_Vbii_ci+qbtr_Vbii_ci);
#endif
qb2=(qb2med+qbtr);
#endif
#ifdef _DYNAMIC
if
(((Jk>0.0)&&(Rci0>0.0)))
{
#ifdef _DYNAMIC
if
((RJk>0.0))
{
#if defined(_DYNAMIC)
Vlim=((Jk*Rci0)/(1.0-(Rci0/RJk)));
#endif
#if defined(_DYNAMIC)
InvVpt=((1.0-(Rci0/RJk))/(Jk*RJk));
#endif
}
else
{
#if defined(_DYNAMIC)
Vlim=((Jk*Rci0)/1.016);
#endif
#if defined(_DYNAMIC)
InvVpt=0.0;
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
(((((Thcs>0.0)&&(Ahc>0.0))&&(Jk>0.0))&&(Ic0>0.0)))
{
#if defined(_DYNAMIC)
RCIO=(Rci0/Area);
#endif
#if defined(_DYNAMIC)
AHC=(Area*Ahc);
#endif
#ifdef _DYNAMIC
if
(((Rci0<RJk)||(RJk<=0.0)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Ih_Vci_ei=(-(HBT_X_d_ICK(vcei,RCIO,Vlim,InvVpt,Vces,vcei_Vci_ei,0.0,0.0,0.0,0.0)/Ic0));
Ih_Vbii_ei=(-((HBT_X_d_ICK(vcei,RCIO,Vlim,InvVpt,Vces,0.0,0.0,0.0,0.0,0.0)*Ic0-HBT_X_ICK(vcei,RCIO,Vlim,InvVpt,Vces)*Ic0_Vbii_ei)/Ic0/Ic0));
Ih_Tempti_GND=(-((HBT_X_d_ICK(vcei,RCIO,Vlim,InvVpt,Vces,0.0,0.0,0.0,0.0,0.0)*Ic0-HBT_X_ICK(vcei,RCIO,Vlim,InvVpt,Vces)*Ic0_Tempti_GND)/Ic0/Ic0));
Ih_Vbii_ci=(-((HBT_X_d_ICK(vcei,RCIO,Vlim,InvVpt,Vces,0.0,0.0,0.0,0.0,0.0)*Ic0-HBT_X_ICK(vcei,RCIO,Vlim,InvVpt,Vces)*Ic0_Vbii_ci)/Ic0/Ic0));
#endif
Ih=(1.0-(HBT_X_ICK(vcei,RCIO,Vlim,InvVpt,Vces)/Ic0));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Ih_Vci_ei=(-(HBT_X_d_Vceff(vcei,Vces,vcei_Vci_ei,0.0)/(RCIO*Ic0)));
Ih_Vbii_ei=(-((HBT_X_d_Vceff(vcei,Vces,0.0,0.0)*(RCIO*Ic0)-HBT_X_Vceff(vcei,Vces)*(RCIO*Ic0_Vbii_ei))/(RCIO*Ic0)/(RCIO*Ic0)));
Ih_Tempti_GND=(-((HBT_X_d_Vceff(vcei,Vces,0.0,0.0)*(RCIO*Ic0)-HBT_X_Vceff(vcei,Vces)*(RCIO*Ic0_Tempti_GND))/(RCIO*Ic0)/(RCIO*Ic0)));
Ih_Vbii_ci=(-((HBT_X_d_Vceff(vcei,Vces,0.0,0.0)*(RCIO*Ic0)-HBT_X_Vceff(vcei,Vces)*(RCIO*Ic0_Vbii_ci))/(RCIO*Ic0)/(RCIO*Ic0)));
#endif
Ih=(1.0-(HBT_X_Vceff(vcei,Vces)/(RCIO*Ic0)));
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((Ih*Ih)+AHC))
double m00_sqrt(d00_sqrt1,(1.0+AHC))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Ih*Ih)+AHC))
#endif
#if defined(_DERIVATE)
Wh_Vci_ei=((Ih_Vci_ei+((Ih_Vci_ei*Ih)+(Ih*Ih_Vci_ei))*d10_sqrt0)/(1.0+d00_sqrt1));
Wh_Vbii_ei=((Ih_Vbii_ei+((Ih_Vbii_ei*Ih)+(Ih*Ih_Vbii_ei))*d10_sqrt0)/(1.0+d00_sqrt1));
Wh_Tempti_GND=((Ih_Tempti_GND+((Ih_Tempti_GND*Ih)+(Ih*Ih_Tempti_GND))*d10_sqrt0)/(1.0+d00_sqrt1));
Wh_Vbii_ci=((Ih_Vbii_ci+((Ih_Vbii_ci*Ih)+(Ih*Ih_Vbii_ci))*d10_sqrt0)/(1.0+d00_sqrt1));
#endif
Wh=((Ih+d00_sqrt0)/(1.0+d00_sqrt1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
xtff_Vbii_ei=(((Thcs*Ic0_Vbii_ei)*(Wh*Wh))+((Thcs*Ic0)*((Wh_Vbii_ei*Wh)+(Wh*Wh_Vbii_ei))));
xtff_Tempti_GND=(((Thcs*Ic0_Tempti_GND)*(Wh*Wh))+((Thcs*Ic0)*((Wh_Tempti_GND*Wh)+(Wh*Wh_Tempti_GND))));
xtff_Vbii_ci=(((Thcs*Ic0_Vbii_ci)*(Wh*Wh))+((Thcs*Ic0)*((Wh_Vbii_ci*Wh)+(Wh*Wh_Vbii_ci))));
xtff_Vci_ei=((Thcs*Ic0)*((Wh_Vci_ei*Wh)+(Wh*Wh_Vci_ei)));
#endif
xtff=((Thcs*Ic0)*(Wh*Wh));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
xtff_Vbii_ei=0.0;
xtff_Tempti_GND=0.0;
xtff_Vbii_ci=0.0;
xtff_Vci_ei=0.0;
#endif
xtff=0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbtf_Tempti_GND=(((Tft*Tex_Tempti_GND)*Ic0)+((Tf+(Tft*Tex))*Ic0_Tempti_GND));
qbtf_Vbii_ei=((Tf+(Tft*Tex))*Ic0_Vbii_ei);
qbtf_Vbii_ci=((Tf+(Tft*Tex))*Ic0_Vbii_ci);
#endif
qbtf=((Tf+(Tft*Tex))*Ic0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qbe_Vbii_ei=((xtff_Vbii_ei+qbtf_Vbii_ei)+HBT_X_d_charge(vbei,Cje,Vje,mje,Area,vbei_Vbii_ei,0.0,0.0,0.0,0.0));
qbe_Tempti_GND=((xtff_Tempti_GND+qbtf_Tempti_GND)+HBT_X_d_charge(vbei,Cje,Vje,mje,Area,0.0,0.0,0.0,0.0,0.0));
qbe_Vbii_ci=((xtff_Vbii_ci+qbtf_Vbii_ci)+HBT_X_d_charge(vbei,Cje,Vje,mje,Area,0.0,0.0,0.0,0.0,0.0));
qbe_Vci_ei=(xtff_Vci_ei+HBT_X_d_charge(vbei,Cje,Vje,mje,Area,0.0,0.0,0.0,0.0,0.0));
#endif
qbe=((xtff+qbtf)+HBT_X_charge(vbei,Cje,Vje,mje,Area));
#endif
_load_static_residual2(bi,ci,Ibx);
#if defined(_DERIVATE)
_load_static_jacobian2s(bi,ci,ti,Ibx_Tempti_GND);
_load_static_jacobian4(bi,ci,bi,ci,Ibx_Vbi_ci);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bi,ci,_DDT((qb1+qbtra)));
#if defined(_DERIVATE)
_load_dynamic_jacobian2s(bi,ci,ti,qbtra_Tempti_GND);
_load_dynamic_jacobian4(bi,ci,bi,ci,(qb1_Vbi_ci+qbtra_Vbi_ci));
#endif
#endif
_load_static_residual2(bii,ci,Ib2);
#if defined(_DERIVATE)
_load_static_jacobian2s(bii,ci,ti,Ib2_Tempti_GND);
_load_static_jacobian4(bii,ci,bii,ci,Ib2_Vbii_ci);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bii,ci,_DDT(qb2));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bii,ci,bii,ci,qb2_Vbii_ci);
_load_dynamic_jacobian2s(bii,ci,ti,qb2_Tempti_GND);
_load_dynamic_jacobian4(bii,ci,bii,ei,qb2_Vbii_ei);
#endif
#endif
_load_static_residual2(bii,ei,Ib1);
#if defined(_DERIVATE)
_load_static_jacobian4(bii,ei,bii,ci,Ib1_Vbii_ci);
_load_static_jacobian2s(bii,ei,ti,Ib1_Tempti_GND);
_load_static_jacobian4(bii,ei,bii,ei,Ib1_Vbii_ei);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bii,ei,_DDT(qbe));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bii,ei,ci,ei,qbe_Vci_ei);
_load_dynamic_jacobian4(bii,ei,bii,ci,qbe_Vbii_ci);
_load_dynamic_jacobian2s(bii,ei,ti,qbe_Tempti_GND);
_load_dynamic_jacobian4(bii,ei,bii,ei,qbe_Vbii_ei);
#endif
#endif
_load_static_residual2(ci,ei,Ic1);
#if defined(_DERIVATE)
_load_static_jacobian4(ci,ei,bii,ci,Ic1_Vbii_ci);
_load_static_jacobian2s(ci,ei,ti,Ic1_Tempti_GND);
_load_static_jacobian4(ci,ei,bii,ei,Ic1_Vbii_ei);
#endif
_load_static_residual2(ex,ei,Ibdx);
#if defined(_DERIVATE)
_load_static_jacobian2s(ex,ei,ti,Ibdx_Tempti_GND);
_load_static_jacobian4(ex,ei,ex,ei,Ibdx_Vex_ei);
#endif
_load_static_residual2(exx,ei,Ibdxx);
#if defined(_DERIVATE)
_load_static_jacobian2s(exx,ei,ti,Ibdxx_Tempti_GND);
_load_static_jacobian4(exx,ei,exx,ei,Ibdxx_Vexx_ei);
#endif
_load_static_residual2(cx,ci,Icdx);
#if defined(_DERIVATE)
_load_static_jacobian2s(cx,ci,ti,Icdx_Tempti_GND);
_load_static_jacobian4(cx,ci,cx,ci,Icdx_Vcx_ci);
#endif
_save_whitenoise2(bii,ei,(TWO_Q*Ib1),"Ib");
_load_static_residual1(ni,NP(ni));
#if defined(_DERIVATE)
_load_static_jacobian1(ni,ni,1.0);
#endif
_save_whitenoise1(ni,(TWO_Q*Ic0),"Ic");
_load_static_residual2(bii,ei,NP(ni));
#if defined(_DERIVATE)
_load_static_jacobian2s(bii,ei,ni,1.0);
#endif
_load_static_residual2(bii,ci,NP(ni));
#if defined(_DERIVATE)
_load_static_jacobian2s(bii,ci,ni,1.0);
#endif
_load_static_residual2(bii,bi,(BP(bii,bi)/(Rb2/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(bii,bi,bii,bi,(1/(Rb2/N)));
#endif
_save_whitenoise2(bii,bi,((FOUR_K*TjK)/(Rb2/N)),"thermal");
_load_static_residual2(b,bi,(BP(b,bi)/(Rb/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(b,bi,b,bi,(1/(Rb/N)));
#endif
_save_whitenoise2(b,bi,((FOUR_K*TjK)/(Rb/N)),"thermal");
_load_static_residual2(e,ei,(BP(e,ei)/(Re/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(e,ei,e,ei,(1/(Re/N)));
#endif
_save_whitenoise2(e,ei,((FOUR_K*TjK)/(Re/N)),"thermal");
_load_static_residual2(c,ci,(BP(c,ci)/(Rc/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(c,ci,c,ci,(1/(Rc/N)));
#endif
_save_whitenoise2(c,ci,((FOUR_K*TjK)/(Rc/N)),"thermal");
if
(((Jse>0.0)&&(ne>0)))
{
_load_static_residual2(ex,bii,(BP(ex,bii)/(Rbxx/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(ex,bii,ex,bii,(1/(Rbxx/N)));
#endif
_save_whitenoise2(ex,bii,((FOUR_K*TjK)/(Rbxx/N)),"thermal");
}
else
{
_load_static_residual2(ex,bii,(BP(ex,bii)*1e-12));
#if defined(_DERIVATE)
_load_static_jacobian4(ex,bii,ex,bii,(1e-12));
#endif
}
if
(((Jsee>0.0)&&(nee>0)))
{
_load_static_residual2(exx,bii,(BP(exx,bii)/(Rbbxx/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(exx,bii,exx,bii,(1/(Rbbxx/N)));
#endif
_save_whitenoise2(exx,bii,((FOUR_K*TjK)/(Rbbxx/N)),"thermal");
}
else
{
_load_static_residual2(exx,bii,(BP(exx,bii)*1e-12));
#if defined(_DERIVATE)
_load_static_jacobian4(exx,bii,exx,bii,(1e-12));
#endif
}
if
(((Jsc>0.0)&&(nc>0)))
{
_load_static_residual2(cx,bii,(BP(cx,bii)/(Rcxx/N)));
#if defined(_DERIVATE)
_load_static_jacobian4(cx,bii,cx,bii,(1/(Rcxx/N)));
#endif
_save_whitenoise2(cx,bii,((FOUR_K*TjK)/(Rcxx/N)),"thermal");
}
else
{
_load_static_residual2(cx,bii,(BP(cx,bii)*1e-12));
#if defined(_DERIVATE)
_load_static_jacobian4(cx,bii,cx,bii,(1e-12));
#endif
}
#if defined(_DYNAMIC)
_load_dynamic_residual1(b,_DDT((Cpb*NP(b))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(b,b,(Cpb));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(c,_DDT((Cpc*NP(c))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(c,c,(Cpc));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,c,_DDT((Cq*BP(b,c))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,c,b,c,(Cq));
#endif
#endif
_load_static_residual1(ti,(-Ipdiss));
#if defined(_DERIVATE)
_load_static_jacobian2p(ti,bi,ci,(-Ipdiss_Vbi_ci));
_load_static_jacobian2p(ti,ci,ei,(-Ipdiss_Vci_ei));
_load_static_jacobian2p(ti,bii,ci,(-Ipdiss_Vbii_ci));
_load_static_jacobian1(ti,ti,(-Ipdiss_Tempti_GND));
_load_static_jacobian2p(ti,bii,ei,(-Ipdiss_Vbii_ei));
#endif
if
(Rth)
{
_load_static_residual2(t,ti,(BP(t,ti)/Rth));
#if defined(_DERIVATE)
_load_static_jacobian4(t,ti,t,ti,(1/Rth));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(t,ti,(Cth*_DDT(BP(t,ti))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(t,ti,t,ti,(Cth));
#endif
#endif
}
else
{
_load_static_residual2(t,ti,(BP(t,ti)*1e3));
#if defined(_DERIVATE)
_load_static_jacobian4(t,ti,t,ti,(1e3));
#endif
}
if
((Ib0<=0))
{
#if defined(_DERIVATE)
Iniix_Vbii_ei=0.0;
Iniix_Tempti_GND=0.0;
Iniix_Vbii_ci=0.0;
#endif
Iniix=0;
#if defined(_DERIVATE)
Iniiix_Vbii_ei=0.0;
Iniiix_Tempti_GND=0.0;
Iniiix_Vbii_ci=0.0;
#endif
Iniiix=0;
}
if
(((Ib0+Ic1)<=0))
{
#if defined(_DERIVATE)
Inivx_Vbii_ei=0.0;
Inivx_Tempti_GND=0.0;
Inivx_Vbii_ci=0.0;
#endif
Inivx=0;
}
else
{
if
((Ab==2))
{
#if defined(_DERIVATE)
Iniix_Vbii_ei=Ib0_Vbii_ei;
Iniix_Tempti_GND=Ib0_Tempti_GND;
Iniix_Vbii_ci=Ib0_Vbii_ci;
#endif
Iniix=Ib0;
}
else
{
{
double m00_pow(d00_pow0,Ib0,(Ab*0.5))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ib0,(Ab*0.5))
#endif
#if defined(_DERIVATE)
Iniix_Vbii_ei=(d10_pow0*Ib0_Vbii_ei);
Iniix_Tempti_GND=(d10_pow0*Ib0_Tempti_GND);
Iniix_Vbii_ci=(d10_pow0*Ib0_Vbii_ci);
#endif
Iniix=d00_pow0;
}
}
if
((Afb==2))
{
#if defined(_DERIVATE)
Iniiix_Vbii_ei=Ib0_Vbii_ei;
Iniiix_Tempti_GND=Ib0_Tempti_GND;
Iniiix_Vbii_ci=Ib0_Vbii_ci;
#endif
Iniiix=Ib0;
}
else
{
{
double m00_pow(d00_pow0,Ib0,(Afb*0.5))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ib0,(Afb*0.5))
#endif
#if defined(_DERIVATE)
Iniiix_Vbii_ei=(d10_pow0*Ib0_Vbii_ei);
Iniiix_Tempti_GND=(d10_pow0*Ib0_Tempti_GND);
Iniiix_Vbii_ci=(d10_pow0*Ib0_Vbii_ci);
#endif
Iniiix=d00_pow0;
}
}
if
((Afe==2))
{
#if defined(_DERIVATE)
Inivx_Vbii_ei=(Ib0_Vbii_ei+Ic1_Vbii_ei);
Inivx_Tempti_GND=(Ib0_Tempti_GND+Ic1_Tempti_GND);
Inivx_Vbii_ci=(Ib0_Vbii_ci+Ic1_Vbii_ci);
#endif
Inivx=(Ib0+Ic1);
}
else
{
{
double m00_pow(d00_pow0,(Ib0+Ic1),(Afe*0.5))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(Ib0+Ic1),(Afe*0.5))
#endif
#if defined(_DERIVATE)
Inivx_Vbii_ei=(d10_pow0*(Ib0_Vbii_ei+Ic1_Vbii_ei));
Inivx_Tempti_GND=(d10_pow0*(Ib0_Tempti_GND+Ic1_Tempti_GND));
Inivx_Vbii_ci=(d10_pow0*(Ib0_Vbii_ci+Ic1_Vbii_ci));
#endif
Inivx=d00_pow0;
}
}
}
_load_static_residual1(nii,NP(nii));
#if defined(_DERIVATE)
_load_static_jacobian1(nii,nii,1.0);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(nii,_DDT((NP(nii)/((2.0*3.1415)*Fb))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(nii,nii,(1/((2.0*3.1415)*Fb)));
#endif
#endif
_save_whitenoise1(nii,(Area*Kb),NULL);
_load_static_residual1(niix,NP(niix));
#if defined(_DERIVATE)
_load_static_jacobian1(niix,niix,1.0);
#endif
_load_static_residual1(niix,Iniix);
#if defined(_DERIVATE)
_load_static_jacobian2p(niix,bii,ci,Iniix_Vbii_ci);
_load_static_jacobian1(niix,ti,Iniix_Tempti_GND);
_load_static_jacobian2p(niix,bii,ei,Iniix_Vbii_ei);
#endif
_load_static_residual1(niii,NP(niii));
#if defined(_DERIVATE)
_load_static_jacobian1(niii,niii,1.0);
#endif
_save_flickernoise1(niii,(Area*Kfb),Ffeb,"Flicker noise base-emitter junction (a)");
_load_static_residual1(niiix,NP(niiix));
#if defined(_DERIVATE)
_load_static_jacobian1(niiix,niiix,1.0);
#endif
_load_static_residual1(niiix,Iniiix);
#if defined(_DERIVATE)
_load_static_jacobian2p(niiix,bii,ci,Iniiix_Vbii_ci);
_load_static_jacobian1(niiix,ti,Iniiix_Tempti_GND);
_load_static_jacobian2p(niiix,bii,ei,Iniiix_Vbii_ei);
#endif
_load_static_residual2(bii,ei,(NP(nii)*NP(niix)));
#if defined(_DERIVATE)
_load_static_jacobian2s(bii,ei,niix,(NP(nii)));
_load_static_jacobian2s(bii,ei,nii,(NP(niix)));
#endif
_load_static_residual2(bii,ei,(NP(niii)*NP(niiix)));
#if defined(_DERIVATE)
_load_static_jacobian2s(bii,ei,niiix,(NP(niii)));
_load_static_jacobian2s(bii,ei,niii,(NP(niiix)));
#endif
_load_static_residual1(niv,NP(niv));
#if defined(_DERIVATE)
_load_static_jacobian1(niv,niv,1.0);
#endif
_save_flickernoise1(niv,(Area*Kfe),Ffee,"Hooge noise of emitter resistance");
_load_static_residual1(nivx,NP(nivx));
#if defined(_DERIVATE)
_load_static_jacobian1(nivx,nivx,1.0);
#endif
_load_static_residual1(nivx,Inivx);
#if defined(_DERIVATE)
_load_static_jacobian2p(nivx,bii,ci,Inivx_Vbii_ci);
_load_static_jacobian1(nivx,ti,Inivx_Tempti_GND);
_load_static_jacobian2p(nivx,bii,ei,Inivx_Vbii_ei);
#endif
_load_static_residual2(e,ei,((NP(niv)*NP(nivx))/(Re/N)));
#if defined(_DERIVATE)
_load_static_jacobian2s(e,ei,nivx,((NP(niv))/(Re/N)));
_load_static_jacobian2s(e,ei,niv,((NP(nivx))/(Re/N)));
#endif

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void HBT_X::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 19; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 19; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void HBT_X::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void HBT_X::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void HBT_X::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void HBT_X::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void HBT_X::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix HBT_X::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (19);

  for (int i1 = 0; i1 < 19; i1++) {
    for (int i2 = 0; i2 < 19; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void HBT_X::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void HBT_X::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void HBT_X::initTR (void)
{
  setStates (2 * 19 * 19);
  initDC ();
}

/* Perform transient analysis iteration step. */
void HBT_X::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 19; i1++) {
  for (i2 = 0; i2 < 19; i2++) {
    state = 2 * (i2 + 19 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 19; i1++) {
    state = 2 * (i1 + 19 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 19; i1++) {
  for (i2 = 0; i2 < 19; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 19; i3++) {
  for (i4 = 0; i4 < 19; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 19; i1++) {
  for (i2 = 0; i2 < 19; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 19; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 19; i1++) {
  for (i3 = 0; i3 < 19; i3++) {
  for (i4 = 0; i4 < 19; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 19; i1++) {
  for (i3 = 0; i3 < 19; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix HBT_X::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (19);

  _load_flickernoise1 (niv,_flicker_pwr[niv][niv],_flicker_exp[niv][niv]);
  _load_flickernoise1 (niii,_flicker_pwr[niii][niii],_flicker_exp[niii][niii]);
  _load_whitenoise1 (nii,_white_pwr[nii][nii]);
  _load_whitenoise2 (cx,bii,_white_pwr[cx][bii]);
  _load_whitenoise2 (exx,bii,_white_pwr[exx][bii]);
  _load_whitenoise2 (ex,bii,_white_pwr[ex][bii]);
  _load_whitenoise2 (c,ci,_white_pwr[c][ci]);
  _load_whitenoise2 (e,ei,_white_pwr[e][ei]);
  _load_whitenoise2 (b,bi,_white_pwr[b][bi]);
  _load_whitenoise2 (bii,bi,_white_pwr[bii][bi]);
  _load_whitenoise1 (ni,_white_pwr[ni][ni]);
  _load_whitenoise2 (bii,ei,_white_pwr[bii][ei]);

  return cy;
}

/* Perform AC noise computations. */
void HBT_X::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void HBT_X::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void HBT_X::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void HBT_X::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 19; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 19; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "HBT_X.defs.h"
