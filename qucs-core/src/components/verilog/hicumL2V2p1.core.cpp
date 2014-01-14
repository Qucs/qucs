/*
 * hicumL2V2p1.core.cpp - device implementations for hicumL2V2p1 module
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

#include "hicumL2V2p1.analogfunction.h"
#include "component.h"
#include "device.h"
#include "hicumL2V2p1.core.h"

#ifndef CIR_hicumL2V2p1
#define CIR_hicumL2V2p1 -1
#endif

// external nodes
#define c 0
#define b 1
#define e 2
#define s 3
#define tnode 4
// internal nodes
#define ci 5
#define ei 6
#define bp 7
#define bi 8
#define si 9

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
#define _modelname              "hicumL2V2p1"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
hicumL2V2p1::hicumL2V2p1() : circuit (10)
{
  type = CIR_hicumL2V2p1;
}

/* Initialization of model. */
void hicumL2V2p1::initModel (void)
{
  // create internal nodes
  setInternalNode (ci, "ci");
  setInternalNode (ei, "ei");
  setInternalNode (bp, "bp");
  setInternalNode (bi, "bi");
  setInternalNode (si, "si");

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
void hicumL2V2p1::initDC (void)
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
void hicumL2V2p1::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void hicumL2V2p1::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[ci][bi] = 0.0;
  _white_pwr[si][s] = 0.0;
  _white_pwr[ei][e] = 0.0;
  _white_pwr[ci][c] = 0.0;
  _white_pwr[bp][bi] = 0.0;
  _white_pwr[b][bp] = 0.0;
  _white_pwr[ci][ei] = 0.0;
  _white_pwr[si][ci] = 0.0;
  _white_pwr[bi][ci] = 0.0;
  _white_pwr[bp][ci] = 0.0;
  _white_pwr[bi][ei] = 0.0;
  _white_pwr[bp][ei] = 0.0;
  _flicker_pwr[bi][ei] = 0.0;
  _flicker_exp[bi][ei] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 10; i1++) {
  for (i2 = 0; i2 < 10; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 10; i1++) {
  for (i2 = 0; i2 < 10; i2++) {
  for (i3 = 0; i3 < 10; i3++) {
  for (i4 = 0; i4 < 10; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 10; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 10; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void hicumL2V2p1::loadVariables (void)
{
  c10 = getPropertyDouble ("c10");
  qp0 = getPropertyDouble ("qp0");
  ich = getPropertyDouble ("ich");
  hfe = getPropertyDouble ("hfe");
  hfc = getPropertyDouble ("hfc");
  hjei = getPropertyDouble ("hjei");
  hjci = getPropertyDouble ("hjci");
  ibeis = getPropertyDouble ("ibeis");
  mbei = getPropertyDouble ("mbei");
  ireis = getPropertyDouble ("ireis");
  mrei = getPropertyDouble ("mrei");
  ibeps = getPropertyDouble ("ibeps");
  mbep = getPropertyDouble ("mbep");
  ireps = getPropertyDouble ("ireps");
  mrep = getPropertyDouble ("mrep");
  mcf = getPropertyDouble ("mcf");
  ibcis = getPropertyDouble ("ibcis");
  mbci = getPropertyDouble ("mbci");
  ibcxs = getPropertyDouble ("ibcxs");
  mbcx = getPropertyDouble ("mbcx");
  ibets = getPropertyDouble ("ibets");
  abet = getPropertyDouble ("abet");
  favl = getPropertyDouble ("favl");
  qavl = getPropertyDouble ("qavl");
  alfav = getPropertyDouble ("alfav");
  alqav = getPropertyDouble ("alqav");
  rbi0 = getPropertyDouble ("rbi0");
  rbx = getPropertyDouble ("rbx");
  fgeo = getPropertyDouble ("fgeo");
  fdqr0 = getPropertyDouble ("fdqr0");
  fcrbi = getPropertyDouble ("fcrbi");
  fqi = getPropertyDouble ("fqi");
  re = getPropertyDouble ("re");
  rcx = getPropertyDouble ("rcx");
  itss = getPropertyDouble ("itss");
  msf = getPropertyDouble ("msf");
  iscs = getPropertyDouble ("iscs");
  msc = getPropertyDouble ("msc");
  tsf = getPropertyDouble ("tsf");
  rsu = getPropertyDouble ("rsu");
  csu = getPropertyDouble ("csu");
  cjei0 = getPropertyDouble ("cjei0");
  vdei = getPropertyDouble ("vdei");
  zei = getPropertyDouble ("zei");
  aljei = getPropertyDouble ("aljei");
  cjep0 = getPropertyDouble ("cjep0");
  vdep = getPropertyDouble ("vdep");
  zep = getPropertyDouble ("zep");
  aljep = getPropertyDouble ("aljep");
  cjci0 = getPropertyDouble ("cjci0");
  vdci = getPropertyDouble ("vdci");
  zci = getPropertyDouble ("zci");
  vptci = getPropertyDouble ("vptci");
  cjcx0 = getPropertyDouble ("cjcx0");
  vdcx = getPropertyDouble ("vdcx");
  zcx = getPropertyDouble ("zcx");
  vptcx = getPropertyDouble ("vptcx");
  fbc = getPropertyDouble ("fbc");
  cjs0 = getPropertyDouble ("cjs0");
  vds = getPropertyDouble ("vds");
  zs = getPropertyDouble ("zs");
  vpts = getPropertyDouble ("vpts");
  t0 = getPropertyDouble ("t0");
  dt0h = getPropertyDouble ("dt0h");
  tbvl = getPropertyDouble ("tbvl");
  tef0 = getPropertyDouble ("tef0");
  gtfe = getPropertyDouble ("gtfe");
  thcs = getPropertyDouble ("thcs");
  alhc = getPropertyDouble ("alhc");
  fthc = getPropertyDouble ("fthc");
  rci0 = getPropertyDouble ("rci0");
  vlim = getPropertyDouble ("vlim");
  vces = getPropertyDouble ("vces");
  vpt = getPropertyDouble ("vpt");
  tr = getPropertyDouble ("tr");
  ceox = getPropertyDouble ("ceox");
  ccox = getPropertyDouble ("ccox");
  alqf = getPropertyDouble ("alqf");
  alit = getPropertyDouble ("alit");
  kf = getPropertyDouble ("kf");
  af = getPropertyDouble ("af");
  krbi = getPropertyDouble ("krbi");
  latb = getPropertyDouble ("latb");
  latl = getPropertyDouble ("latl");
  vgb = getPropertyDouble ("vgb");
  alt0 = getPropertyDouble ("alt0");
  kt0 = getPropertyDouble ("kt0");
  zetaci = getPropertyDouble ("zetaci");
  zetacx = getPropertyDouble ("zetacx");
  alvs = getPropertyDouble ("alvs");
  alces = getPropertyDouble ("alces");
  zetarbi = getPropertyDouble ("zetarbi");
  zetarbx = getPropertyDouble ("zetarbx");
  zetarcx = getPropertyDouble ("zetarcx");
  zetare = getPropertyDouble ("zetare");
  alb = getPropertyDouble ("alb");
  rth = getPropertyDouble ("rth");
  cth = getPropertyDouble ("cth");
  tnom = getPropertyDouble ("tnom");
  dt = getPropertyDouble ("dt");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void hicumL2V2p1::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void hicumL2V2p1::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void hicumL2V2p1::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void hicumL2V2p1::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void hicumL2V2p1::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double twoq;
double flicker_Pwr;
#if defined(_DERIVATE)
double flicker_Pwr_Vtnode_GND;
double flicker_Pwr_Vbi_ei;
double flicker_Pwr_Vbp_ei;
double flicker_Pwr_Vbi_ci;
#endif
double rbin;
#if defined(_DERIVATE)
double rbin_Vtnode_GND;
double rbin_Vbi_ei;
double rbin_Vci_ei;
double rbin_Vbi_ci;
#endif
double fourkt;
#if defined(_DERIVATE)
double fourkt_Vtnode_GND;
#endif
double pterm;
#if defined(_DERIVATE)
double pterm_Vci_ei;
double pterm_Vtnode_GND;
double pterm_Vbi_ei;
double pterm_Vbi_ci;
#endif
double ijsc;
#if defined(_DERIVATE)
double ijsc_Vtnode_GND;
double ijsc_Vsi_ci;
double ijsc_Vbp_ci;
double ijsc_Vbp_ei;
double ijsc_Vbi_ci;
double ijsc_Vbi_ei;
#endif
#if defined(_DYNAMIC)
double Qdsu;
#if defined(_DERIVATE)
double Qdsu_Vtnode_GND;
double Qdsu_Vbp_ci;
#endif
#endif
double HSI_Tsu;
#if defined(_DERIVATE)
double HSI_Tsu_Vtnode_GND;
double HSI_Tsu_Vbp_ci;
double HSI_Tsu_Vsi_ci;
#endif
double HSb;
#if defined(_DERIVATE)
double HSb_Vsi_ci;
double HSb_Vtnode_GND;
#endif
double HSa;
#if defined(_DERIVATE)
double HSa_Vbp_ci;
double HSa_Vtnode_GND;
#endif
double HSUM;
#if defined(_DERIVATE)
double HSUM_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double Qjs;
#if defined(_DERIVATE)
double Qjs_Vbp_ei;
double Qjs_Vbi_ei;
double Qjs_Vsi_ci;
double Qjs_Vb_ci;
double Qjs_Vbp_ci;
double Qjs_Vtnode_GND;
double Qjs_Vbi_ci;
#endif
#endif
double Cjs;
#if defined(_DYNAMIC)
double qjcx0_t_i;
#if defined(_DERIVATE)
double qjcx0_t_i_Vbp_ei;
double qjcx0_t_i_Vbi_ei;
double qjcx0_t_i_Vb_ci;
double qjcx0_t_i_Vbp_ci;
double qjcx0_t_i_Vtnode_GND;
double qjcx0_t_i_Vbi_ci;
#endif
#endif
double cjcx0_t_i;
double ijbcx;
#if defined(_DERIVATE)
double ijbcx_Vtnode_GND;
double ijbcx_Vbp_ci;
double ijbcx_Vbp_ei;
double ijbcx_Vbi_ci;
double ijbcx_Vbi_ei;
#endif
#if defined(_DYNAMIC)
double qjcx0_t_ii;
#if defined(_DERIVATE)
double qjcx0_t_ii_Vbp_ei;
double qjcx0_t_ii_Vbi_ei;
double qjcx0_t_ii_Vbp_ci;
double qjcx0_t_ii_Vtnode_GND;
double qjcx0_t_ii_Vbi_ci;
#endif
#endif
double cjcx0_t_ii;
double ibet;
#if defined(_DERIVATE)
double ibet_Vtnode_GND;
double ibet_Vbp_ei;
double ibet_Vbi_ci;
double ibet_Vbi_ei;
#endif
double czz;
#if defined(_DERIVATE)
double czz_Vtnode_GND;
double czz_Vbp_ei;
double czz_Vbi_ci;
double czz_Vbi_ei;
#endif
double pocce;
#if defined(_DERIVATE)
double pocce_Vbp_ei;
double pocce_Vbi_ci;
double pocce_Vtnode_GND;
double pocce_Vbi_ei;
#endif
double zex;
double cce;
#if defined(_DERIVATE)
double cce_Vbp_ei;
double cce_Vbi_ci;
double cce_Vtnode_GND;
double cce_Vbi_ei;
#endif
double vve;
#if defined(_DERIVATE)
double vve_Vbp_ei;
double vve_Vtnode_GND;
#endif
double ibets_t;
#if defined(_DERIVATE)
double ibets_t_Vtnode_GND;
#endif
double ab;
#if defined(_DERIVATE)
double ab_Vtnode_GND;
#endif
double a_eg;
#if defined(_DERIVATE)
double a_eg_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double Qjep;
#if defined(_DERIVATE)
double Qjep_Vbp_ei;
double Qjep_Vbi_ci;
double Qjep_Vtnode_GND;
double Qjep_Vbi_ei;
#endif
#endif
double Cjep;
#if defined(_DERIVATE)
double Cjep_Vbp_ei;
double Cjep_Vbi_ci;
double Cjep_Vtnode_GND;
double Cjep_Vbi_ei;
#endif
double irep;
#if defined(_DERIVATE)
double irep_Vtnode_GND;
double irep_Vbp_ei;
double irep_Vbi_ci;
double irep_Vbi_ei;
#endif
double ibep;
#if defined(_DERIVATE)
double ibep_Vtnode_GND;
double ibep_Vbp_ei;
double ibep_Vbi_ci;
double ibep_Vbi_ei;
#endif
double ETA;
#if defined(_DERIVATE)
double ETA_Vtnode_GND;
double ETA_Vbi_ei;
double ETA_Vci_ei;
double ETA_Vbi_ci;
#endif
double I_BI;
#if defined(_DERIVATE)
double I_BI_Vtnode_GND;
double I_BI_Vbi_ei;
#endif
double rbi;
#if defined(_DERIVATE)
double rbi_Vtnode_GND;
double rbi_Vbi_ei;
double rbi_Vci_ei;
double rbi_Vbi_ci;
#endif
double f_p;
#if defined(_DERIVATE)
double f_p_Vtnode_GND;
double f_p_Vbi_ei;
double f_p_Vci_ei;
double f_p_Vbi_ci;
#endif
double Q_fi;
#if defined(_DERIVATE)
double Q_fi_Vci_ei;
double Q_fi_Vtnode_GND;
double Q_fi_Vbi_ci;
double Q_fi_Vbi_ei;
#endif
double Qz;
#if defined(_DERIVATE)
double Qz_Vtnode_GND;
double Qz_Vbi_ei;
double Qz_Vci_ei;
double Qz_Vbi_ci;
#endif
double f_QR;
#if defined(_DERIVATE)
double f_QR_Vtnode_GND;
#endif
double iavl;
#if defined(_DERIVATE)
double iavl_Vci_ei;
double iavl_Vtnode_GND;
double iavl_Vbi_ei;
double iavl_Vbi_ci;
#endif
double AVLS_avl;
#if defined(_DERIVATE)
double AVLS_avl_Vtnode_GND;
double AVLS_avl_Vbi_ei;
double AVLS_avl_Vbi_ci;
#endif
double AVLa;
#if defined(_DERIVATE)
double AVLa_Vtnode_GND;
double AVLa_Vbi_ei;
double AVLa_Vbi_ci;
#endif
double AVLU0;
#if defined(_DERIVATE)
double AVLU0_Vtnode_GND;
#endif
double AVLv_q;
#if defined(_DERIVATE)
double AVLv_q_Vtnode_GND;
double AVLv_q_Vbi_ei;
double AVLv_q_Vbi_ci;
#endif
double AVLv_bord;
#if defined(_DERIVATE)
double AVLv_bord_Vtnode_GND;
double AVLv_bord_Vbi_ci;
#endif
double ibci;
#if defined(_DERIVATE)
double ibci_Vtnode_GND;
double ibci_Vbi_ci;
double ibci_Vbi_ei;
#endif
#if defined(_DYNAMIC)
double qrbi;
#if defined(_DERIVATE)
double qrbi_Vtnode_GND;
double qrbi_Vbi_ei;
double qrbi_Vci_ei;
double qrbi_Vbi_ci;
#endif
#endif
#if defined(_DYNAMIC)
double Qdci;
#if defined(_DERIVATE)
double Qdci_Vci_ei;
double Qdci_Vtnode_GND;
double Qdci_Vbi_ci;
double Qdci_Vbi_ei;
#endif
#endif
#if defined(_DYNAMIC)
double Qdei;
#if defined(_DERIVATE)
double Qdei_Vci_ei;
double Qdei_Vtnode_GND;
double Qdei_Vbi_ci;
double Qdei_Vbi_ei;
#endif
#endif
double it;
#if defined(_DERIVATE)
double it_Vci_ei;
double it_Vtnode_GND;
double it_Vbi_ei;
double it_Vbi_ci;
#endif
double d_Q0;
double d_Q;
#if defined(_DERIVATE)
double d_Q_Vtnode_GND;
double d_Q_Vbi_ei;
double d_Q_Vbi_ci;
double d_Q_Vci_ei;
#endif
double Q_pT;
#if defined(_DERIVATE)
double Q_pT_Vtnode_GND;
double Q_pT_Vbi_ei;
double Q_pT_Vbi_ci;
double Q_pT_Vci_ei;
#endif
double l_it;
double Qr;
#if defined(_DERIVATE)
double Qr_Vci_ei;
double Qr_Vtnode_GND;
double Qr_Vbi_ci;
double Qr_Vbi_ei;
#endif
double FCd_f;
#if defined(_DERIVATE)
double FCd_f_Vci_ei;
double FCd_f_Vtnode_GND;
double FCd_f_Vbi_ei;
double FCd_f_Vbi_ci;
#endif
double FCz_1;
#if defined(_DERIVATE)
double FCz_1_Vci_ei;
double FCz_1_Vtnode_GND;
double FCz_1_Vbi_ei;
double FCz_1_Vbi_ci;
#endif
double FCdfCT_ditf;
#if defined(_DERIVATE)
double FCdfCT_ditf_Vci_ei;
double FCdfCT_ditf_Vbi_ei;
double FCdfCT_ditf_Vbi_ci;
double FCdfCT_ditf_Vtnode_GND;
#endif
double FCdfCT_dw;
#if defined(_DERIVATE)
double FCdfCT_dw_Vci_ei;
double FCdfCT_dw_Vbi_ei;
double FCdfCT_dw_Vbi_ci;
double FCdfCT_dw_Vtnode_GND;
#endif
double FCdfc_ditf;
#if defined(_DERIVATE)
double FCdfc_ditf_Vci_ei;
double FCdfc_ditf_Vtnode_GND;
double FCdfc_ditf_Vbi_ei;
double FCdfc_ditf_Vbi_ci;
#endif
double FCdw_ditf;
#if defined(_DERIVATE)
double FCdw_ditf_Vci_ei;
double FCdw_ditf_Vtnode_GND;
double FCdw_ditf_Vbi_ei;
double FCdw_ditf_Vbi_ci;
#endif
double FCdfc_dw;
#if defined(_DERIVATE)
double FCdfc_dw_Vci_ei;
double FCdfc_dw_Vtnode_GND;
double FCdfc_dw_Vbi_ei;
double FCdfc_dw_Vbi_ci;
#endif
double FCf_ci;
#if defined(_DERIVATE)
double FCf_ci_Vci_ei;
double FCf_ci_Vtnode_GND;
double FCf_ci_Vbi_ei;
double FCf_ci_Vbi_ci;
#endif
double FCdf3_dw;
#if defined(_DERIVATE)
double FCdf3_dw_Vci_ei;
double FCdf3_dw_Vtnode_GND;
double FCdf3_dw_Vbi_ei;
double FCdf3_dw_Vbi_ci;
#endif
double FCf3;
#if defined(_DERIVATE)
double FCf3_Vci_ei;
double FCf3_Vbi_ei;
double FCf3_Vbi_ci;
double FCf3_Vtnode_GND;
#endif
double FCdf2_dw;
#if defined(_DERIVATE)
double FCdf2_dw_Vci_ei;
double FCdf2_dw_Vtnode_GND;
double FCdf2_dw_Vbi_ei;
double FCdf2_dw_Vbi_ci;
#endif
double FCf2;
#if defined(_DERIVATE)
double FCf2_Vci_ei;
double FCf2_Vbi_ei;
double FCf2_Vbi_ci;
double FCf2_Vtnode_GND;
#endif
double r;
double a3;
#if defined(_DERIVATE)
double a3_Vci_ei;
double a3_Vbi_ei;
double a3_Vbi_ci;
double a3_Vtnode_GND;
#endif
double a2;
#if defined(_DERIVATE)
double a2_Vci_ei;
double a2_Vbi_ei;
double a2_Vbi_ci;
double a2_Vtnode_GND;
#endif
double x;
#if defined(_DERIVATE)
double x_Vci_ei;
double x_Vtnode_GND;
double x_Vbi_ei;
double x_Vbi_ci;
#endif
double z;
#if defined(_DERIVATE)
double z_Vci_ei;
double z_Vtnode_GND;
double z_Vbi_ei;
double z_Vbi_ci;
#endif
double FCdf1_dw;
#if defined(_DERIVATE)
double FCdf1_dw_Vci_ei;
double FCdf1_dw_Vtnode_GND;
double FCdf1_dw_Vbi_ei;
double FCdf1_dw_Vbi_ci;
#endif
double FCw2;
#if defined(_DERIVATE)
double FCw2_Vci_ei;
double FCw2_Vtnode_GND;
double FCw2_Vbi_ei;
double FCw2_Vbi_ci;
#endif
double FCf_CT;
#if defined(_DERIVATE)
double FCf_CT_Vci_ei;
double FCf_CT_Vbi_ei;
double FCf_CT_Vbi_ci;
double FCf_CT_Vtnode_GND;
#endif
double FCf1;
#if defined(_DERIVATE)
double FCf1_Vci_ei;
double FCf1_Vtnode_GND;
double FCf1_Vbi_ei;
double FCf1_Vbi_ci;
#endif
double FCda1_dw;
#if defined(_DERIVATE)
double FCda1_dw_Vci_ei;
double FCda1_dw_Vtnode_GND;
double FCda1_dw_Vbi_ei;
double FCda1_dw_Vbi_ci;
#endif
double FClnl;
#if defined(_DERIVATE)
double FClnl_Vci_ei;
double FClnl_Vtnode_GND;
double FClnl_Vbi_ei;
double FClnl_Vbi_ci;
#endif
double FClnb;
#if defined(_DERIVATE)
double FClnb_Vci_ei;
double FClnb_Vtnode_GND;
double FClnb_Vbi_ei;
double FClnb_Vbi_ci;
#endif
double FCdw_daick;
#if defined(_DERIVATE)
double FCdw_daick_Vci_ei;
double FCdw_daick_Vtnode_GND;
double FCdw_daick_Vbi_ei;
double FCdw_daick_Vbi_ci;
#endif
double FCw;
#if defined(_DERIVATE)
double FCw_Vci_ei;
double FCw_Vtnode_GND;
double FCw_Vbi_ei;
double FCw_Vbi_ci;
#endif
double FCd_a;
#if defined(_DERIVATE)
double FCd_a_Vci_ei;
double FCd_a_Vtnode_GND;
double FCd_a_Vbi_ei;
double FCd_a_Vbi_ci;
#endif
double FCa1;
#if defined(_DERIVATE)
double FCa1_Vci_ei;
double FCa1_Vtnode_GND;
double FCa1_Vbi_ei;
double FCa1_Vbi_ci;
#endif
double FCln;
double FCxb;
double FCxl;
double FCz;
#if defined(_DERIVATE)
double FCz_Vci_ei;
double FCz_Vtnode_GND;
double FCz_Vbi_ei;
double FCz_Vbi_ci;
#endif
double FCdaick_ditf;
#if defined(_DERIVATE)
double FCdaick_ditf_Vci_ei;
double FCdaick_ditf_Vtnode_GND;
double FCdaick_ditf_Vbi_ei;
double FCdaick_ditf_Vbi_ci;
#endif
double FCa_ck;
#if defined(_DERIVATE)
double FCa_ck_Vci_ei;
double FCa_ck_Vtnode_GND;
double FCa_ck_Vbi_ei;
double FCa_ck_Vbi_ci;
#endif
double FCrt;
#if defined(_DERIVATE)
double FCrt_Vci_ei;
double FCrt_Vtnode_GND;
double FCrt_Vbi_ei;
double FCrt_Vbi_ci;
#endif
double FCa;
#if defined(_DERIVATE)
double FCa_Vci_ei;
double FCa_Vtnode_GND;
double FCa_Vbi_ei;
double FCa_Vbi_ci;
#endif
double FFT_cT;
#if defined(_DERIVATE)
double FFT_cT_Vtnode_GND;
double FFT_cT_Vci_ei;
double FFT_cT_Vbi_ei;
double FFT_cT_Vbi_ci;
#endif
double FFQ_cT;
#if defined(_DERIVATE)
double FFQ_cT_Vtnode_GND;
double FFQ_cT_Vbi_ei;
double FFQ_cT_Vbi_ci;
double FFQ_cT_Vci_ei;
#endif
double FFT_fC;
#if defined(_DERIVATE)
double FFT_fC_Vtnode_GND;
double FFT_fC_Vci_ei;
double FFT_fC_Vbi_ei;
double FFT_fC_Vbi_ci;
#endif
double FFQ_fC;
#if defined(_DERIVATE)
double FFQ_fC_Vtnode_GND;
double FFQ_fC_Vbi_ei;
double FFQ_fC_Vbi_ci;
double FFQ_fC_Vci_ei;
#endif
double FFT_pcS;
#if defined(_DERIVATE)
double FFT_pcS_Vtnode_GND;
#endif
double FFd_TfB;
#if defined(_DERIVATE)
double FFd_TfB_Vtnode_GND;
double FFd_TfB_Vci_ei;
double FFd_TfB_Vbi_ei;
double FFd_TfB_Vbi_ci;
#endif
double FFa_w;
#if defined(_DERIVATE)
double FFa_w_Vci_ei;
double FFa_w_Vtnode_GND;
double FFa_w_Vbi_ei;
double FFa_w_Vbi_ci;
#endif
double FFd_QfB;
#if defined(_DERIVATE)
double FFd_QfB_Vtnode_GND;
double FFd_QfB_Vbi_ei;
double FFd_QfB_Vbi_ci;
double FFd_QfB_Vci_ei;
#endif
double FFw_2;
#if defined(_DERIVATE)
double FFw_2_Vci_ei;
double FFw_2_Vtnode_GND;
double FFw_2_Vbi_ei;
double FFw_2_Vbi_ci;
#endif
double FFw;
#if defined(_DERIVATE)
double FFw_Vci_ei;
double FFw_Vtnode_GND;
double FFw_Vbi_ei;
double FFw_Vbi_ci;
#endif
double FFs;
#if defined(_DERIVATE)
double FFs_Vci_ei;
double FFs_Vtnode_GND;
double FFs_Vbi_ei;
double FFs_Vbi_ci;
#endif
double FFx;
#if defined(_DERIVATE)
double FFx_Vci_ei;
double FFx_Vtnode_GND;
double FFx_Vbi_ei;
double FFx_Vbi_ci;
#endif
double FFT_fbS;
#if defined(_DERIVATE)
double FFT_fbS_Vtnode_GND;
#endif
double FFd_QfE;
#if defined(_DERIVATE)
double FFd_QfE_Vtnode_GND;
double FFd_QfE_Vci_ei;
double FFd_QfE_Vbi_ei;
double FFd_QfE_Vbi_ci;
#endif
double FFd_TfE;
#if defined(_DERIVATE)
double FFd_TfE_Vtnode_GND;
double FFd_TfE_Vci_ei;
double FFd_TfE_Vbi_ei;
double FFd_TfE_Vbi_ci;
#endif
double FFa;
#if defined(_DERIVATE)
double FFa_Vci_ei;
double FFa_Vtnode_GND;
double FFa_Vbi_ei;
double FFa_Vbi_ci;
#endif
double T_fT;
#if defined(_DERIVATE)
double T_fT_Vci_ei;
double T_fT_Vtnode_GND;
double T_fT_Vbi_ci;
double T_fT_Vbi_ei;
#endif
double Q_fT;
#if defined(_DERIVATE)
double Q_fT_Vci_ei;
double Q_fT_Vtnode_GND;
double Q_fT_Vbi_ci;
double Q_fT_Vbi_ei;
#endif
double Qf;
#if defined(_DERIVATE)
double Qf_Vci_ei;
double Qf_Vtnode_GND;
double Qf_Vbi_ci;
double Qf_Vbi_ei;
#endif
double Tf;
#if defined(_DERIVATE)
double Tf_Vci_ei;
double Tf_Vtnode_GND;
double Tf_Vbi_ci;
double Tf_Vbi_ei;
#endif
double itr;
#if defined(_DERIVATE)
double itr_Vci_ei;
double itr_Vtnode_GND;
double itr_Vbi_ci;
double itr_Vbi_ei;
#endif
double itf;
#if defined(_DERIVATE)
double itf_Vci_ei;
double itf_Vtnode_GND;
double itf_Vbi_ei;
double itf_Vbi_ci;
#endif
double a_h;
#if defined(_DERIVATE)
double a_h_Vci_ei;
double a_h_Vtnode_GND;
double a_h_Vbi_ei;
double a_h_Vbi_ci;
#endif
double I_Tf1;
#if defined(_DERIVATE)
double I_Tf1_Vci_ei;
double I_Tf1_Vtnode_GND;
double I_Tf1_Vbi_ei;
double I_Tf1_Vbi_ci;
#endif
double Q_p;
#if defined(_DERIVATE)
double Q_p_Vtnode_GND;
double Q_p_Vbi_ei;
double Q_p_Vbi_ci;
#endif
double A;
#if defined(_DERIVATE)
double A_Vtnode_GND;
double A_Vbi_ei;
double A_Vbi_ci;
#endif
double ICKa;
#if defined(_DERIVATE)
double ICKa_Vci_ei;
double ICKa_Vtnode_GND;
#endif
double ick;
#if defined(_DERIVATE)
double ick_Vci_ei;
double ick_Vtnode_GND;
#endif
double vceff;
#if defined(_DERIVATE)
double vceff_Vci_ei;
double vceff_Vtnode_GND;
#endif
double ICKv;
#if defined(_DERIVATE)
double ICKv_Vci_ei;
double ICKv_Vtnode_GND;
#endif
double vc;
#if defined(_DERIVATE)
double vc_Vci_ei;
double vc_Vtnode_GND;
#endif
double T_f0;
#if defined(_DERIVATE)
double T_f0_Vtnode_GND;
double T_f0_Vbi_ci;
double T_f0_Vbi_ei;
#endif
double cc;
#if defined(_DERIVATE)
double cc_Vtnode_GND;
double cc_Vbi_ci;
double cc_Vbi_ei;
#endif
double d1;
double Cjcit;
#if defined(_DERIVATE)
double Cjcit_Vbi_ci;
double Cjcit_Vtnode_GND;
double Cjcit_Vbi_ei;
#endif
double b_q;
#if defined(_DERIVATE)
double b_q_Vtnode_GND;
double b_q_Vbi_ei;
double b_q_Vbi_ci;
#endif
double Q_bpt;
#if defined(_DERIVATE)
double Q_bpt_Vtnode_GND;
#endif
double a_bpt;
double Q_0;
#if defined(_DERIVATE)
double Q_0_Vtnode_GND;
double Q_0_Vbi_ei;
double Q_0_Vbi_ci;
#endif
double Qjci;
#if defined(_DERIVATE)
double Qjci_Vbi_ei;
double Qjci_Vtnode_GND;
double Qjci_Vbi_ci;
#endif
double DQ_j3;
#if defined(_DERIVATE)
double DQ_j3_Vsi_ci;
double DQ_j3_Vb_ci;
double DQ_j3_Vbp_ci;
double DQ_j3_Vtnode_GND;
double DQ_j3_Vbi_ci;
#endif
double DQ_j2;
#if defined(_DERIVATE)
double DQ_j2_Vsi_ci;
double DQ_j2_Vb_ci;
double DQ_j2_Vbp_ci;
double DQ_j2_Vtnode_GND;
double DQ_j2_Vbi_ci;
#endif
double DQ_j1;
#if defined(_DERIVATE)
double DQ_j1_Vsi_ci;
double DQ_j1_Vb_ci;
double DQ_j1_Vbp_ci;
double DQ_j1_Vtnode_GND;
double DQ_j1_Vbi_ci;
#endif
double Cjci;
#if defined(_DERIVATE)
double Cjci_Vbi_ei;
double Cjci_Vtnode_GND;
double Cjci_Vbi_ci;
#endif
double DC_j3;
#if defined(_DERIVATE)
double DC_j3_Vsi_ci;
double DC_j3_Vb_ci;
double DC_j3_Vbp_ci;
double DC_j3_Vtnode_GND;
double DC_j3_Vbi_ci;
#endif
double DC_j2;
#if defined(_DERIVATE)
double DC_j2_Vsi_ci;
double DC_j2_Vb_ci;
double DC_j2_Vbp_ci;
double DC_j2_Vtnode_GND;
double DC_j2_Vbi_ci;
#endif
double DC_j1;
#if defined(_DERIVATE)
double DC_j1_Vsi_ci;
double DC_j1_Vb_ci;
double DC_j1_Vbp_ci;
double DC_j1_Vtnode_GND;
double DC_j1_Vbi_ci;
#endif
double Dzr1;
double Dz1;
double DCln2;
#if defined(_DERIVATE)
double DCln2_Vsi_ci;
double DCln2_Vb_ci;
double DCln2_Vbp_ci;
double DCln2_Vtnode_GND;
double DCln2_Vbi_ci;
#endif
double DCln1;
#if defined(_DERIVATE)
double DCln1_Vsi_ci;
double DCln1_Vb_ci;
double DCln1_Vbp_ci;
double DCln1_Vtnode_GND;
double DCln1_Vbi_ci;
#endif
double Dv_j4;
#if defined(_DERIVATE)
double Dv_j4_Vsi_ci;
double Dv_j4_Vb_ci;
double Dv_j4_Vbp_ci;
double Dv_j4_Vbi_ci;
double Dv_j4_Vtnode_GND;
#endif
double Dv_j2;
#if defined(_DERIVATE)
double Dv_j2_Vsi_ci;
double Dv_j2_Vb_ci;
double Dv_j2_Vbp_ci;
double Dv_j2_Vtnode_GND;
double Dv_j2_Vbi_ci;
#endif
double De_2;
#if defined(_DERIVATE)
double De_2_Vsi_ci;
double De_2_Vb_ci;
double De_2_Vbp_ci;
double De_2_Vtnode_GND;
double De_2_Vbi_ci;
#endif
double Dv_r;
#if defined(_DERIVATE)
double Dv_r_Vsi_ci;
double Dv_r_Vb_ci;
double Dv_r_Vbp_ci;
double Dv_r_Vtnode_GND;
double Dv_r_Vbi_ci;
#endif
double Dv_j1;
#if defined(_DERIVATE)
double Dv_j1_Vsi_ci;
double Dv_j1_Vb_ci;
double Dv_j1_Vbp_ci;
double Dv_j1_Vtnode_GND;
double Dv_j1_Vbi_ci;
#endif
double De_1;
#if defined(_DERIVATE)
double De_1_Vsi_ci;
double De_1_Vb_ci;
double De_1_Vbp_ci;
double De_1_Vtnode_GND;
double De_1_Vbi_ci;
#endif
double De;
#if defined(_DERIVATE)
double De_Vsi_ci;
double De_Vb_ci;
double De_Vbp_ci;
double De_Vtnode_GND;
double De_Vbi_ci;
#endif
double Dv_e;
#if defined(_DERIVATE)
double Dv_e_Vsi_ci;
double Dv_e_Vb_ci;
double Dv_e_Vbp_ci;
double Dv_e_Vtnode_GND;
double Dv_e_Vbi_ci;
#endif
double Da;
#if defined(_DERIVATE)
double Da_Vtnode_GND;
#endif
double DC_c;
#if defined(_DERIVATE)
double DC_c_Vtnode_GND;
#endif
double DC_max;
#if defined(_DERIVATE)
double DC_max_Vtnode_GND;
#endif
double DV_f;
#if defined(_DERIVATE)
double DV_f_Vtnode_GND;
#endif
double Dv_p;
#if defined(_DERIVATE)
double Dv_p_Vtnode_GND;
#endif
double Dz_r;
double Qjei;
#if defined(_DERIVATE)
double Qjei_Vtnode_GND;
double Qjei_Vbi_ei;
#endif
double DFQ_j;
#if defined(_DERIVATE)
double DFQ_j_Vsi_ci;
double DFQ_j_Vb_ci;
double DFQ_j_Vbp_ci;
double DFQ_j_Vbp_ei;
double DFQ_j_Vbi_ci;
double DFQ_j_Vtnode_GND;
double DFQ_j_Vbi_ei;
#endif
double Cjei;
double DFC_j1;
#if defined(_DERIVATE)
double DFC_j1_Vsi_ci;
double DFC_j1_Vb_ci;
double DFC_j1_Vbp_ci;
double DFC_j1_Vbp_ei;
double DFC_j1_Vbi_ci;
double DFC_j1_Vtnode_GND;
double DFC_j1_Vbi_ei;
#endif
double DFb;
#if defined(_DERIVATE)
double DFb_Vsi_ci;
double DFb_Vb_ci;
double DFb_Vbp_ci;
double DFb_Vbp_ei;
double DFb_Vbi_ci;
double DFb_Vtnode_GND;
double DFb_Vbi_ei;
#endif
double DFdvj_dv;
#if defined(_DERIVATE)
double DFdvj_dv_Vsi_ci;
double DFdvj_dv_Vb_ci;
double DFdvj_dv_Vbp_ci;
double DFdvj_dv_Vbp_ei;
double DFdvj_dv_Vbi_ci;
double DFdvj_dv_Vtnode_GND;
double DFdvj_dv_Vbi_ei;
#endif
double DFv_j;
#if defined(_DERIVATE)
double DFv_j_Vsi_ci;
double DFv_j_Vb_ci;
double DFv_j_Vbp_ci;
double DFv_j_Vbp_ei;
double DFv_j_Vbi_ci;
double DFv_j_Vtnode_GND;
double DFv_j_Vbi_ei;
#endif
double DFe;
#if defined(_DERIVATE)
double DFe_Vsi_ci;
double DFe_Vb_ci;
double DFe_Vbp_ci;
double DFe_Vbp_ei;
double DFe_Vbi_ci;
double DFe_Vtnode_GND;
double DFe_Vbi_ei;
#endif
double DFv_e;
#if defined(_DERIVATE)
double DFv_e_Vsi_ci;
double DFv_e_Vb_ci;
double DFv_e_Vbp_ci;
double DFv_e_Vbp_ei;
double DFv_e_Vbi_ci;
double DFv_e_Vtnode_GND;
double DFv_e_Vbi_ei;
#endif
double DFC_max;
#if defined(_DERIVATE)
double DFC_max_Vtnode_GND;
#endif
double DFV_f;
#if defined(_DERIVATE)
double DFV_f_Vtnode_GND;
#endif
double i_0r;
#if defined(_DERIVATE)
double i_0r_Vtnode_GND;
double i_0r_Vbi_ci;
#endif
double i_0f;
#if defined(_DERIVATE)
double i_0f_Vtnode_GND;
double i_0f_Vbi_ei;
#endif
double VT_f;
#if defined(_DERIVATE)
double VT_f_Vtnode_GND;
#endif
double Tr;
double Ovpt;
double Oich;
double irei;
#if defined(_DERIVATE)
double irei_Vtnode_GND;
double irei_Vbi_ei;
#endif
double ibei;
#if defined(_DERIVATE)
double ibei_Vtnode_GND;
double ibei_Vbi_ei;
#endif
double le;
#if defined(_DERIVATE)
double le_Vsi_ci;
double le_Vbp_ci;
double le_Vbp_ei;
double le_Vbi_ci;
double le_Vbi_ei;
double le_Vtnode_GND;
#endif
double DIOY;
#if defined(_DERIVATE)
double DIOY_Vsi_ci;
double DIOY_Vbp_ci;
double DIOY_Vbp_ei;
double DIOY_Vbi_ci;
double DIOY_Vbi_ei;
double DIOY_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double vpts_t;
#if defined(_DERIVATE)
double vpts_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double cjs0_t;
#if defined(_DERIVATE)
double cjs0_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double vds_t;
#if defined(_DERIVATE)
double vds_t_Vtnode_GND;
#endif
#endif
double iscs_t;
#if defined(_DERIVATE)
double iscs_t_Vtnode_GND;
#endif
double itss_t;
#if defined(_DERIVATE)
double itss_t_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double tsf_t;
#if defined(_DERIVATE)
double tsf_t_Vtnode_GND;
#endif
#endif
double re_t;
#if defined(_DERIVATE)
double re_t_Vtnode_GND;
#endif
double rbx_t;
#if defined(_DERIVATE)
double rbx_t_Vtnode_GND;
#endif
double rcx_t;
#if defined(_DERIVATE)
double rcx_t_Vtnode_GND;
#endif
double ibcxs_t;
#if defined(_DERIVATE)
double ibcxs_t_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double cjcx02_t;
#if defined(_DERIVATE)
double cjcx02_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double cjcx01_t;
#if defined(_DERIVATE)
double cjcx01_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double vptcx_t;
#if defined(_DERIVATE)
double vptcx_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double cratio_t;
#if defined(_DERIVATE)
double cratio_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double vdcx_t;
#if defined(_DERIVATE)
double vdcx_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double cjcx02;
#endif
#if defined(_DYNAMIC)
double cjcx01;
#endif
#if defined(_DYNAMIC)
double ccox2;
#endif
#if defined(_DYNAMIC)
double ccox1;
#endif
#if defined(_DYNAMIC)
double C_1;
#endif
double aljep_t;
#if defined(_DERIVATE)
double aljep_t_Vtnode_GND;
#endif
double cjep0_t;
#if defined(_DERIVATE)
double cjep0_t_Vtnode_GND;
#endif
double vdep_t;
#if defined(_DERIVATE)
double vdep_t_Vtnode_GND;
#endif
double ireps_t;
#if defined(_DERIVATE)
double ireps_t_Vtnode_GND;
#endif
double ibeps_t;
#if defined(_DERIVATE)
double ibeps_t_Vtnode_GND;
#endif
double rbi0_t;
#if defined(_DERIVATE)
double rbi0_t_Vtnode_GND;
#endif
double qavl_t;
#if defined(_DERIVATE)
double qavl_t_Vtnode_GND;
#endif
double favl_t;
#if defined(_DERIVATE)
double favl_t_Vtnode_GND;
#endif
double thcs_t;
#if defined(_DERIVATE)
double thcs_t_Vtnode_GND;
#endif
double tef0_t;
#if defined(_DERIVATE)
double tef0_t_Vtnode_GND;
#endif
double t0_t;
#if defined(_DERIVATE)
double t0_t_Vtnode_GND;
#endif
double vptci_t;
#if defined(_DERIVATE)
double vptci_t_Vtnode_GND;
#endif
double cjci0_t;
#if defined(_DERIVATE)
double cjci0_t_Vtnode_GND;
#endif
double vdci_t;
#if defined(_DERIVATE)
double vdci_t_Vtnode_GND;
#endif
double ibcis_t;
#if defined(_DERIVATE)
double ibcis_t_Vtnode_GND;
#endif
double vces_t;
#if defined(_DERIVATE)
double vces_t_Vtnode_GND;
#endif
double vlim_t;
#if defined(_DERIVATE)
double vlim_t_Vtnode_GND;
#endif
double k;
#if defined(_DERIVATE)
double k_Vtnode_GND;
#endif
double Orci0_t;
#if defined(_DERIVATE)
double Orci0_t_Vtnode_GND;
#endif
double qtt0_zci;
#if defined(_DERIVATE)
double qtt0_zci_Vtnode_GND;
#endif
double qp0_t;
#if defined(_DERIVATE)
double qp0_t_Vtnode_GND;
#endif
double c10_t;
#if defined(_DERIVATE)
double c10_t_Vtnode_GND;
#endif
double aljei_t;
#if defined(_DERIVATE)
double aljei_t_Vtnode_GND;
#endif
double cjei0_t;
#if defined(_DERIVATE)
double cjei0_t_Vtnode_GND;
#endif
double vdei_t;
#if defined(_DERIVATE)
double vdei_t_Vtnode_GND;
#endif
double vdjt;
#if defined(_DERIVATE)
double vdjt_Vtnode_GND;
#endif
double vdj1;
double vdj0;
#if defined(_DERIVATE)
double vdj0_Vtnode_GND;
#endif
double ireis_t;
#if defined(_DERIVATE)
double ireis_t_Vtnode_GND;
#endif
double ibeis_t;
#if defined(_DERIVATE)
double ibeis_t_Vtnode_GND;
#endif
double a;
#if defined(_DERIVATE)
double a_Vci_ei;
double a_Vbi_ei;
double a_Vbi_ci;
double a_Vtnode_GND;
#endif
double r_VgVT;
#if defined(_DERIVATE)
double r_VgVT_Vtnode_GND;
#endif
double V_gT;
#if defined(_DERIVATE)
double V_gT_Vtnode_GND;
#endif
double avs;
double ln_qtt0;
#if defined(_DERIVATE)
double ln_qtt0_Vtnode_GND;
#endif
double qtt0;
#if defined(_DERIVATE)
double qtt0_Vtnode_GND;
#endif
double dT;
#if defined(_DERIVATE)
double dT_Vtnode_GND;
#endif
double VT;
#if defined(_DERIVATE)
double VT_Vtnode_GND;
#endif
double vt0;
double Tdev;
#if defined(_DERIVATE)
double Tdev_Vtnode_GND;
#endif
double Tamb;
double Tnom;
Tnom=(tnom+273.15);
Tamb=_circuit_temp;
#if defined(_DERIVATE)
Tdev_Vtnode_GND=1.0;
#endif
Tdev=((Tamb+dt)+NP(tnode));
if
((Tdev<((-100.0)+273.15)))
{
#if defined(_DERIVATE)
Tdev_Vtnode_GND=0.0;
#endif
Tdev=((-100.0)+273.15);
}
else
{
if
((Tdev>(326.85+273.15)))
{
#if defined(_DERIVATE)
Tdev_Vtnode_GND=0.0;
#endif
Tdev=(326.85+273.15);
}
}
#if defined(_DYNAMIC)
vt0=m00_vt(Tnom);
#endif
#if defined(_DERIVATE)
VT_Vtnode_GND=Tdev_Vtnode_GND*m10_vt(Tdev);
#endif
VT=m00_vt(Tdev);
#if defined(_DERIVATE)
dT_Vtnode_GND=Tdev_Vtnode_GND;
#endif
dT=(Tdev-Tnom);
#if defined(_DERIVATE)
qtt0_Vtnode_GND=(Tdev_Vtnode_GND/Tnom);
#endif
qtt0=(Tdev/Tnom);
{
double m00_logE(d00_logE0,qtt0)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,qtt0)
#endif
#if defined(_DERIVATE)
ln_qtt0_Vtnode_GND=qtt0_Vtnode_GND*d10_logE0;
#endif
ln_qtt0=d00_logE0;
}
#if defined(_DYNAMIC)
avs=(alvs*Tnom);
#endif
#if defined(_DERIVATE)
V_gT_Vtnode_GND=((((3.0*VT_Vtnode_GND)*ln_qtt0)+((3.0*VT)*ln_qtt0_Vtnode_GND))+(vgb*qtt0_Vtnode_GND));
#endif
V_gT=(((3.0*VT)*ln_qtt0)+(vgb*(qtt0-1.0)));
#if defined(_DERIVATE)
r_VgVT_Vtnode_GND=((V_gT_Vtnode_GND*VT-V_gT*VT_Vtnode_GND)/VT/VT);
#endif
r_VgVT=(V_gT/VT);
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(((mcf*r_VgVT_Vtnode_GND)/mbei)-(alb*dT_Vtnode_GND));
#endif
a=(((mcf*r_VgVT)/mbei)-(alb*dT));
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibeis_t_Vtnode_GND=(ibeis*a_Vtnode_GND*d10_exp0);
#endif
ibeis_t=(ibeis*d00_exp0);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(((mcf*r_VgVT_Vtnode_GND)/mrei)-(alb*dT_Vtnode_GND));
#endif
a=(((mcf*r_VgVT)/mrei)-(alb*dT));
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ireis_t_Vtnode_GND=(ireis*a_Vtnode_GND*d10_exp0);
#endif
ireis_t=(ireis*d00_exp0);
}
if
((cjei0>0.0))
{
#if defined(_DERIVATE)
vdj0_Vtnode_GND=((vdei*qtt0_Vtnode_GND)-V_gT_Vtnode_GND);
#endif
vdj0=((vdei*qtt0)-V_gT);
vdj1=(0.1*vdei);
#if defined(_DERIVATE)
vdjt_Vtnode_GND=((vdj0_Vtnode_GND*VT-(vdj0-vdj1)*VT_Vtnode_GND)/VT/VT);
#endif
vdjt=((vdj0-vdj1)/VT);
if
((vdjt<11.0))
{
{
double m00_exp(d00_exp0,vdjt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vdei_t_Vtnode_GND=((VT_Vtnode_GND*d00_logE1)+(VT*vdjt_Vtnode_GND*d10_exp0*d10_logE1));
#endif
vdei_t=(vdj1+(VT*d00_logE1));
}
}
else
{
#if defined(_DERIVATE)
vdei_t_Vtnode_GND=vdj0_Vtnode_GND;
#endif
vdei_t=vdj0;
}
{
double m00_logE(d00_logE0,(vdei/vdei_t))
double m00_exp(d00_exp1,(zei*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdei/vdei_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjei0_t_Vtnode_GND=(cjei0*(zei*(-vdei*vdei_t_Vtnode_GND/vdei_t/vdei_t)*d10_logE0)*d10_exp1);
#endif
cjei0_t=(cjei0*d00_exp1);
}
if
((1==1))
{
#if defined(_DERIVATE)
aljei_t_Vtnode_GND=((aljei*vdei_t_Vtnode_GND)/vdei);
#endif
aljei_t=((aljei*vdei_t)/vdei);
}
else
{
#if defined(_DERIVATE)
aljei_t_Vtnode_GND=0.0;
#endif
aljei_t=aljei;
}
}
else
{
#if defined(_DERIVATE)
cjei0_t_Vtnode_GND=0.0;
#endif
cjei0_t=cjei0;
#if defined(_DERIVATE)
vdei_t_Vtnode_GND=0.0;
#endif
vdei_t=vdei;
#if defined(_DERIVATE)
aljei_t_Vtnode_GND=0.0;
#endif
aljei_t=aljei;
}
{
double m00_exp(d00_exp0,r_VgVT)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
c10_t_Vtnode_GND=(c10*r_VgVT_Vtnode_GND*d10_exp0);
#endif
c10_t=(c10*d00_exp0);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(vdei_t_Vtnode_GND/vdei);
#endif
a=(vdei_t/vdei);
#if defined(_DERIVATE)
qp0_t_Vtnode_GND=(qp0*((0.5*zei)*(-a_Vtnode_GND)));
#endif
qp0_t=(qp0*(1.0+((0.5*zei)*(1.0-a))));
{
double m00_exp(d00_exp0,(zetaci*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qtt0_zci_Vtnode_GND=(zetaci*ln_qtt0_Vtnode_GND)*d10_exp0;
#endif
qtt0_zci=d00_exp0;
}
#if defined(_DERIVATE)
Orci0_t_Vtnode_GND=(-(rci0*qtt0_zci_Vtnode_GND)/(rci0*qtt0_zci)/(rci0*qtt0_zci));
#endif
Orci0_t=(1.0/(rci0*qtt0_zci));
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(((vlim*qtt0_zci_Vtnode_GND)*(1.0-(alvs*dT)))+((vlim*qtt0_zci)*(-(alvs*dT_Vtnode_GND))));
#endif
a=((vlim*qtt0_zci)*(1.0-(alvs*dT)));
#if defined(_DERIVATE)
k_Vtnode_GND=(((a_Vtnode_GND-VT_Vtnode_GND)*VT-(a-VT)*VT_Vtnode_GND)/VT/VT);
#endif
k=((a-VT)/VT);
if
((k<11.0))
{
{
double m00_exp(d00_exp0,k)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vlim_t_Vtnode_GND=(VT_Vtnode_GND+((VT_Vtnode_GND*d00_logE1)+(VT*k_Vtnode_GND*d10_exp0*d10_logE1)));
#endif
vlim_t=(VT+(VT*d00_logE1));
}
}
else
{
#if defined(_DERIVATE)
vlim_t_Vtnode_GND=a_Vtnode_GND;
#endif
vlim_t=a;
}
#if defined(_DERIVATE)
vces_t_Vtnode_GND=(vces*(alces*dT_Vtnode_GND));
#endif
vces_t=(vces*(1+(alces*dT)));
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(r_VgVT_Vtnode_GND/mbci);
#endif
a=(r_VgVT/mbci);
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibcis_t_Vtnode_GND=(ibcis*a_Vtnode_GND*d10_exp0);
#endif
ibcis_t=(ibcis*d00_exp0);
}
if
((cjci0>0.0))
{
#if defined(_DERIVATE)
vdj0_Vtnode_GND=((vdci*qtt0_Vtnode_GND)-V_gT_Vtnode_GND);
#endif
vdj0=((vdci*qtt0)-V_gT);
vdj1=(0.1*vdci);
#if defined(_DERIVATE)
vdjt_Vtnode_GND=((vdj0_Vtnode_GND*VT-(vdj0-vdj1)*VT_Vtnode_GND)/VT/VT);
#endif
vdjt=((vdj0-vdj1)/VT);
if
((vdjt<11.0))
{
{
double m00_exp(d00_exp0,vdjt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vdci_t_Vtnode_GND=((VT_Vtnode_GND*d00_logE1)+(VT*vdjt_Vtnode_GND*d10_exp0*d10_logE1));
#endif
vdci_t=(vdj1+(VT*d00_logE1));
}
}
else
{
#if defined(_DERIVATE)
vdci_t_Vtnode_GND=vdj0_Vtnode_GND;
#endif
vdci_t=vdj0;
}
{
double m00_logE(d00_logE0,(vdci/vdci_t))
double m00_exp(d00_exp1,(zci*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdci/vdci_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjci0_t_Vtnode_GND=(cjci0*(zci*(-vdci*vdci_t_Vtnode_GND/vdci_t/vdci_t)*d10_logE0)*d10_exp1);
#endif
cjci0_t=(cjci0*d00_exp1);
}
if
((0==1))
{
#if defined(_DERIVATE)
vptci_t_Vtnode_GND=((vptci*vdci_t_Vtnode_GND)/vdci);
#endif
vptci_t=((vptci*vdci_t)/vdci);
}
else
{
#if defined(_DERIVATE)
vptci_t_Vtnode_GND=0.0;
#endif
vptci_t=vptci;
}
}
else
{
#if defined(_DERIVATE)
cjci0_t_Vtnode_GND=0.0;
#endif
cjci0_t=cjci0;
#if defined(_DERIVATE)
vdci_t_Vtnode_GND=0.0;
#endif
vdci_t=vdci;
#if defined(_DERIVATE)
vptci_t_Vtnode_GND=0.0;
#endif
vptci_t=vptci;
}
#if defined(_DERIVATE)
t0_t_Vtnode_GND=(t0*((alt0*dT_Vtnode_GND)+(((kt0*dT_Vtnode_GND)*dT)+((kt0*dT)*dT_Vtnode_GND))));
#endif
t0_t=(t0*((1+(alt0*dT))+((kt0*dT)*dT)));
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(alb*dT_Vtnode_GND);
#endif
a=(1.0+(alb*dT));
{
double m00_sqrt(d00_sqrt0,((a*a)+0.01))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((a*a)+0.01))
#endif
#if defined(_DERIVATE)
k_Vtnode_GND=(0.5*(a_Vtnode_GND+((a_Vtnode_GND*a)+(a*a_Vtnode_GND))*d10_sqrt0));
#endif
k=(0.5*(a+d00_sqrt0));
}
#if defined(_DERIVATE)
tef0_t_Vtnode_GND=(((tef0*qtt0_Vtnode_GND)*k-(tef0*qtt0)*k_Vtnode_GND)/k/k);
#endif
tef0_t=((tef0*qtt0)/k);
#if defined(_DERIVATE)
thcs_t_Vtnode_GND=(((thcs*qtt0_zci_Vtnode_GND)*qtt0-(thcs*qtt0_zci)*qtt0_Vtnode_GND)/qtt0/qtt0);
#endif
thcs_t=((thcs*qtt0_zci)/qtt0);
{
double m00_exp(d00_exp0,(alfav*dT))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
favl_t_Vtnode_GND=(favl*(alfav*dT_Vtnode_GND)*d10_exp0);
#endif
favl_t=(favl*d00_exp0);
}
{
double m00_exp(d00_exp0,(alqav*dT))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qavl_t_Vtnode_GND=(qavl*(alqav*dT_Vtnode_GND)*d10_exp0);
#endif
qavl_t=(qavl*d00_exp0);
}
{
double m00_exp(d00_exp0,(zetarbi*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
rbi0_t_Vtnode_GND=(rbi0*(zetarbi*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
rbi0_t=(rbi0*d00_exp0);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(((mcf*r_VgVT_Vtnode_GND)/mbep)-(alb*dT_Vtnode_GND));
#endif
a=(((mcf*r_VgVT)/mbep)-(alb*dT));
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibeps_t_Vtnode_GND=(ibeps*a_Vtnode_GND*d10_exp0);
#endif
ibeps_t=(ibeps*d00_exp0);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(((mcf*r_VgVT_Vtnode_GND)/mrep)-(alb*dT_Vtnode_GND));
#endif
a=(((mcf*r_VgVT)/mrep)-(alb*dT));
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ireps_t_Vtnode_GND=(ireps*a_Vtnode_GND*d10_exp0);
#endif
ireps_t=(ireps*d00_exp0);
}
if
((cjep0>0.0))
{
#if defined(_DERIVATE)
vdj0_Vtnode_GND=((vdep*qtt0_Vtnode_GND)-V_gT_Vtnode_GND);
#endif
vdj0=((vdep*qtt0)-V_gT);
vdj1=(0.1*vdep);
#if defined(_DERIVATE)
vdjt_Vtnode_GND=((vdj0_Vtnode_GND*VT-(vdj0-vdj1)*VT_Vtnode_GND)/VT/VT);
#endif
vdjt=((vdj0-vdj1)/VT);
if
((vdjt<11.0))
{
{
double m00_exp(d00_exp0,vdjt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vdep_t_Vtnode_GND=((VT_Vtnode_GND*d00_logE1)+(VT*vdjt_Vtnode_GND*d10_exp0*d10_logE1));
#endif
vdep_t=(vdj1+(VT*d00_logE1));
}
}
else
{
#if defined(_DERIVATE)
vdep_t_Vtnode_GND=vdj0_Vtnode_GND;
#endif
vdep_t=vdj0;
}
{
double m00_logE(d00_logE0,(vdep/vdep_t))
double m00_exp(d00_exp1,(zep*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdep/vdep_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjep0_t_Vtnode_GND=(cjep0*(zep*(-vdep*vdep_t_Vtnode_GND/vdep_t/vdep_t)*d10_logE0)*d10_exp1);
#endif
cjep0_t=(cjep0*d00_exp1);
}
if
((1==1))
{
#if defined(_DERIVATE)
aljep_t_Vtnode_GND=((aljep*vdep_t_Vtnode_GND)/vdep);
#endif
aljep_t=((aljep*vdep_t)/vdep);
}
else
{
#if defined(_DERIVATE)
aljep_t_Vtnode_GND=0.0;
#endif
aljep_t=aljep;
}
}
else
{
#if defined(_DERIVATE)
cjep0_t_Vtnode_GND=0.0;
#endif
cjep0_t=cjep0;
#if defined(_DERIVATE)
vdep_t_Vtnode_GND=0.0;
#endif
vdep_t=vdep;
#if defined(_DERIVATE)
aljep_t_Vtnode_GND=0.0;
#endif
aljep_t=aljep;
}
#if defined(_DYNAMIC)
C_1=((1.0-fbc)*(cjcx0+ccox));
#endif
#ifdef _DYNAMIC
if
((C_1>=ccox))
{
#if defined(_DYNAMIC)
ccox1=ccox;
#endif
#if defined(_DYNAMIC)
ccox2=0;
#endif
#if defined(_DYNAMIC)
cjcx01=(C_1-ccox);
#endif
#if defined(_DYNAMIC)
cjcx02=(cjcx0-cjcx01);
#endif
}
else
{
#if defined(_DYNAMIC)
ccox1=C_1;
#endif
#if defined(_DYNAMIC)
ccox2=(ccox-ccox1);
#endif
#if defined(_DYNAMIC)
cjcx01=0.0;
#endif
#if defined(_DYNAMIC)
cjcx02=cjcx0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((1.0>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdj0_Vtnode_GND=((vdcx*qtt0_Vtnode_GND)-V_gT_Vtnode_GND);
#endif
vdj0=((vdcx*qtt0)-V_gT);
#endif
#if defined(_DYNAMIC)
vdj1=(0.1*vdcx);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdjt_Vtnode_GND=((vdj0_Vtnode_GND*VT-(vdj0-vdj1)*VT_Vtnode_GND)/VT/VT);
#endif
vdjt=((vdj0-vdj1)/VT);
#endif
#ifdef _DYNAMIC
if
((vdjt<11.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,vdjt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vdcx_t_Vtnode_GND=((VT_Vtnode_GND*d00_logE1)+(VT*vdjt_Vtnode_GND*d10_exp0*d10_logE1));
#endif
vdcx_t=(vdj1+(VT*d00_logE1));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdcx_t_Vtnode_GND=vdj0_Vtnode_GND;
#endif
vdcx_t=vdj0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vdcx/vdcx_t))
double m00_exp(d00_exp1,(zcx*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdcx/vdcx_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cratio_t_Vtnode_GND=(1.0*(zcx*(-vdcx*vdcx_t_Vtnode_GND/vdcx_t/vdcx_t)*d10_logE0)*d10_exp1);
#endif
cratio_t=(1.0*d00_exp1);
}
#endif
#ifdef _DYNAMIC
if
((0==1))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vptcx_t_Vtnode_GND=((vptcx*vdcx_t_Vtnode_GND)/vdcx);
#endif
vptcx_t=((vptcx*vdcx_t)/vdcx);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vptcx_t_Vtnode_GND=0.0;
#endif
vptcx_t=vptcx;
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
cratio_t_Vtnode_GND=0.0;
#endif
cratio_t=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdcx_t_Vtnode_GND=0.0;
#endif
vdcx_t=vdcx;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vptcx_t_Vtnode_GND=0.0;
#endif
vptcx_t=vptcx;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
cjcx01_t_Vtnode_GND=cratio_t_Vtnode_GND*cjcx01;
#endif
cjcx01_t=(cratio_t*cjcx01);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
cjcx02_t_Vtnode_GND=cratio_t_Vtnode_GND*cjcx02;
#endif
cjcx02_t=(cratio_t*cjcx02);
#endif
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(r_VgVT_Vtnode_GND/mbcx);
#endif
a=(r_VgVT/mbcx);
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibcxs_t_Vtnode_GND=(ibcxs*a_Vtnode_GND*d10_exp0);
#endif
ibcxs_t=(ibcxs*d00_exp0);
}
{
double m00_exp(d00_exp0,(zetarcx*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
rcx_t_Vtnode_GND=(rcx*(zetarcx*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
rcx_t=(rcx*d00_exp0);
}
{
double m00_exp(d00_exp0,(zetarbx*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
rbx_t_Vtnode_GND=(rbx*(zetarbx*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
rbx_t=(rbx*d00_exp0);
}
{
double m00_exp(d00_exp0,(zetare*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
re_t_Vtnode_GND=(re*(zetare*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
re_t=(re*d00_exp0);
}
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((zetacx-1.0)*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
tsf_t_Vtnode_GND=(tsf*((zetacx-1.0)*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
tsf_t=(tsf*d00_exp0);
}
#endif
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=(r_VgVT_Vtnode_GND/msf);
#endif
a=(r_VgVT/msf);
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
itss_t_Vtnode_GND=(itss*a_Vtnode_GND*d10_exp0);
#endif
itss_t=(itss*d00_exp0);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vtnode_GND=r_VgVT_Vtnode_GND*msc;
#endif
a=(r_VgVT*msc);
{
double m00_exp(d00_exp0,a)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
iscs_t_Vtnode_GND=(iscs*a_Vtnode_GND*d10_exp0);
#endif
iscs_t=(iscs*d00_exp0);
}
#ifdef _DYNAMIC
if
((cjs0>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdj0_Vtnode_GND=((vds*qtt0_Vtnode_GND)-V_gT_Vtnode_GND);
#endif
vdj0=((vds*qtt0)-V_gT);
#endif
#if defined(_DYNAMIC)
vdj1=(0.1*vds);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdjt_Vtnode_GND=((vdj0_Vtnode_GND*VT-(vdj0-vdj1)*VT_Vtnode_GND)/VT/VT);
#endif
vdjt=((vdj0-vdj1)/VT);
#endif
#ifdef _DYNAMIC
if
((vdjt<11.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,vdjt)
double m00_logE(d00_logE1,(1.0+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_exp0))
#endif
#if defined(_DERIVATE)
vds_t_Vtnode_GND=((VT_Vtnode_GND*d00_logE1)+(VT*vdjt_Vtnode_GND*d10_exp0*d10_logE1));
#endif
vds_t=(vdj1+(VT*d00_logE1));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vds_t_Vtnode_GND=vdj0_Vtnode_GND;
#endif
vds_t=vdj0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vds/vds_t))
double m00_exp(d00_exp1,(zs*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vds/vds_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjs0_t_Vtnode_GND=(cjs0*(zs*(-vds*vds_t_Vtnode_GND/vds_t/vds_t)*d10_logE0)*d10_exp1);
#endif
cjs0_t=(cjs0*d00_exp1);
}
#endif
#ifdef _DYNAMIC
if
((0==1))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vpts_t_Vtnode_GND=((vpts*vds_t_Vtnode_GND)/vds);
#endif
vpts_t=((vpts*vds_t)/vds);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vpts_t_Vtnode_GND=0.0;
#endif
vpts_t=vpts;
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
cjs0_t_Vtnode_GND=0.0;
#endif
cjs0_t=cjs0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vds_t_Vtnode_GND=0.0;
#endif
vds_t=vds;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vpts_t_Vtnode_GND=0.0;
#endif
vpts_t=vpts;
#endif
}
#endif /* if (...) */
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=(1/(mbei*VT));
DIOY_Vtnode_GND=(-BP(bi,ei)*(mbei*VT_Vtnode_GND)/(mbei*VT)/(mbei*VT));
#endif
DIOY=(BP(bi,ei)/(mbei*VT));
if
((ibeis>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ibei_Vtnode_GND=((ibeis_t_Vtnode_GND*(le-1.0))+(ibeis_t*le_Vtnode_GND));
ibei_Vbi_ei=(ibeis_t*le_Vbi_ei);
#endif
ibei=(ibeis_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ibei_Vtnode_GND=(-ibeis_t_Vtnode_GND);
ibei_Vbi_ei=0.0;
#endif
ibei=(-ibeis_t);
}
}
else
{
#if defined(_DERIVATE)
ibei_Vtnode_GND=0.0;
ibei_Vbi_ei=0.0;
#endif
ibei=0.0;
}
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=(1/(mrei*VT));
DIOY_Vtnode_GND=(-BP(bi,ei)*(mrei*VT_Vtnode_GND)/(mrei*VT)/(mrei*VT));
#endif
DIOY=(BP(bi,ei)/(mrei*VT));
if
((ireis>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
irei_Vtnode_GND=((ireis_t_Vtnode_GND*(le-1.0))+(ireis_t*le_Vtnode_GND));
irei_Vbi_ei=(ireis_t*le_Vbi_ei);
#endif
irei=(ireis_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
irei_Vtnode_GND=(-ireis_t_Vtnode_GND);
irei_Vbi_ei=0.0;
#endif
irei=(-ireis_t);
}
}
else
{
#if defined(_DERIVATE)
irei_Vtnode_GND=0.0;
irei_Vbi_ei=0.0;
#endif
irei=0.0;
}
if
((ich>0.0))
{
Oich=(1.0/ich);
}
else
{
Oich=0.0;
}
if
((vpt>0.0))
{
Ovpt=(1.0/vpt);
}
else
{
Ovpt=0.0;
}
Tr=tr;
#if defined(_DERIVATE)
VT_f_Vtnode_GND=(mcf*VT_Vtnode_GND);
#endif
VT_f=(mcf*VT);
{
double m00_limexp(d00_limexp0,(BP(bi,ei)/VT_f))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(bi,ei)/VT_f))
#endif
#if defined(_DERIVATE)
i_0f_Vtnode_GND=((c10_t_Vtnode_GND*d00_limexp0)+(c10_t*(-BP(bi,ei)*VT_f_Vtnode_GND/VT_f/VT_f)*d10_limexp0));
i_0f_Vbi_ei=(c10_t*(1/VT_f)*d10_limexp0);
#endif
i_0f=(c10_t*d00_limexp0);
}
{
double m00_limexp(d00_limexp0,(BP(bi,ci)/VT))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(bi,ci)/VT))
#endif
#if defined(_DERIVATE)
i_0r_Vtnode_GND=((c10_t_Vtnode_GND*d00_limexp0)+(c10_t*(-BP(bi,ci)*VT_Vtnode_GND/VT/VT)*d10_limexp0));
i_0r_Vbi_ci=(c10_t*(1/VT)*d10_limexp0);
#endif
i_0r=(c10_t*d00_limexp0);
}
if
((cjei0_t>0.0))
{
{
double m00_logE(d00_logE0,aljei_t)
double m00_exp(d00_exp1,((-d00_logE0)/zei))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,aljei_t)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=((vdei_t_Vtnode_GND*(1.0-d00_exp1))+(vdei_t*(-((-aljei_t_Vtnode_GND*d10_logE0)/zei)*d10_exp1)));
#endif
DFV_f=(vdei_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=((aljei_t_Vtnode_GND*cjei0_t)+(aljei_t*cjei0_t_Vtnode_GND));
#endif
DFC_max=(aljei_t*cjei0_t);
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=0.0;
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(bi,ei))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=((-1.0)/VT);
#endif
DFv_e=((DFV_f-BP(bi,ei))/VT);
if
((DFv_e<80.0))
{
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=0.0;
DFe_Vbp_ci=0.0;
DFe_Vbp_ei=0.0;
DFe_Vbi_ci=0.0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
}
else
{
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=1.0;
#endif
DFv_j=BP(bi,ei);
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
}
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdei_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdei_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=0.0;
DFb_Vbp_ci=0.0;
DFb_Vbp_ei=0.0;
DFb_Vbi_ci=0.0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdei_t-DFv_j*vdei_t_Vtnode_GND)/vdei_t/vdei_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdei_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
{
double m00_exp(d00_exp0,((-zei)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=0.0;
DFC_j1_Vbp_ci=0.0;
DFC_j1_Vbp_ei=0.0;
DFC_j1_Vbi_ci=0.0;
DFC_j1_Vtnode_GND=((((cjei0_t_Vtnode_GND*d00_exp0)+(cjei0_t*((-zei)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjei0_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjei0_t*((-zei)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjei0_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjei0_t*d00_exp0)*DFdvj_dv);
}
#if defined(_DYNAMIC)
Cjei=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#endif
{
double m00_exp(d00_exp0,(DFb*(1.0-zei)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=0.0;
DFQ_j_Vbp_ci=0.0;
DFQ_j_Vbp_ei=0.0;
DFQ_j_Vbi_ci=0.0;
DFQ_j_Vtnode_GND=(((((cjei0_t_Vtnode_GND*vdei_t)+(cjei0_t*vdei_t_Vtnode_GND))*(1.0-d00_exp0))+((cjei0_t*vdei_t)*(-DFb_Vtnode_GND*(1.0-zei)*d10_exp0)))/(1.0-zei));
DFQ_j_Vbi_ei=(((cjei0_t*vdei_t)*(-DFb_Vbi_ei*(1.0-zei)*d10_exp0))/(1.0-zei));
#endif
DFQ_j=(((cjei0_t*vdei_t)*(1.0-d00_exp0))/(1.0-zei));
}
#if defined(_DERIVATE)
Qjei_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(bi,ei)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
Qjei_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(1.0-DFv_j_Vbi_ei)));
#endif
Qjei=(DFQ_j+(DFC_max*(BP(bi,ei)-DFv_j)));
}
else
{
#if defined(_DYNAMIC)
Cjei=0.0;
#endif
#if defined(_DERIVATE)
Qjei_Vtnode_GND=0.0;
Qjei_Vbi_ei=0.0;
#endif
Qjei=0.0;
}
if
((vptci_t<1.0e2))
{
if
((cjci0_t>0.0))
{
Dz_r=(zci/4.0);
#if defined(_DERIVATE)
Dv_p_Vtnode_GND=(vptci_t_Vtnode_GND-vdci_t_Vtnode_GND);
#endif
Dv_p=(vptci_t-vdci_t);
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
DV_f_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DV_f=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
DC_max_Vtnode_GND=(2.4*cjci0_t_Vtnode_GND);
#endif
DC_max=(2.4*cjci0_t);
{
double m00_logE(d00_logE0,(vptci_t/vdci_t))
double m00_exp(d00_exp1,(d00_logE0*(Dz_r-zci)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vptci_t/vdci_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DC_c_Vtnode_GND=((cjci0_t_Vtnode_GND*d00_exp1)+(cjci0_t*((vptci_t_Vtnode_GND*vdci_t-vptci_t*vdci_t_Vtnode_GND)/vdci_t/vdci_t)*d10_logE0*(Dz_r-zci)*d10_exp1));
#endif
DC_c=(cjci0_t*d00_exp1);
}
#if defined(_DERIVATE)
Da_Vtnode_GND=VT_Vtnode_GND;
#endif
Da=VT;
#if defined(_DERIVATE)
Dv_e_Vsi_ci=0.0;
Dv_e_Vb_ci=0.0;
Dv_e_Vbp_ci=0.0;
Dv_e_Vtnode_GND=((DV_f_Vtnode_GND*Da-(DV_f-BP(bi,ci))*Da_Vtnode_GND)/Da/Da);
Dv_e_Vbi_ci=((-1.0)/Da);
#endif
Dv_e=((DV_f-BP(bi,ci))/Da);
if
((Dv_e<80.0))
{
{
double m00_exp(d00_exp0,Dv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=0.0;
De_Vbp_ci=0.0;
De_Vtnode_GND=Dv_e_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_e_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=0.0;
De_1_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_1_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_1=(De/(1.0+De));
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=0.0;
Dv_j1_Vbp_ci=0.0;
Dv_j1_Vtnode_GND=(DV_f_Vtnode_GND-((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j1_Vbi_ci=(-(Da*De_Vbi_ci*d10_logE0));
#endif
Dv_j1=(DV_f-(Da*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=0.0;
De_1_Vtnode_GND=0.0;
De_1_Vbi_ci=0.0;
#endif
De_1=1.0;
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=0.0;
Dv_j1_Vbp_ci=0.0;
Dv_j1_Vtnode_GND=0.0;
Dv_j1_Vbi_ci=1.0;
#endif
Dv_j1=BP(bi,ci);
}
#if defined(_DERIVATE)
Da_Vtnode_GND=((0.1*Dv_p_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
Da=((0.1*Dv_p)+(4.0*VT));
#if defined(_DERIVATE)
Dv_r_Vsi_ci=0.0;
Dv_r_Vb_ci=0.0;
Dv_r_Vbp_ci=0.0;
Dv_r_Vtnode_GND=(((Dv_p_Vtnode_GND+Dv_j1_Vtnode_GND)*Da-(Dv_p+Dv_j1)*Da_Vtnode_GND)/Da/Da);
Dv_r_Vbi_ci=(Dv_j1_Vbi_ci/Da);
#endif
Dv_r=((Dv_p+Dv_j1)/Da);
if
((Dv_r<80.0))
{
{
double m00_exp(d00_exp0,Dv_r)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=0.0;
De_Vbp_ci=0.0;
De_Vtnode_GND=Dv_r_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_r_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=0.0;
De_2_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_2_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_2=(De/(1.0+De));
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=0.0;
Dv_j2_Vbp_ci=0.0;
Dv_j2_Vtnode_GND=((-Dv_p_Vtnode_GND)+((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j2_Vbi_ci=(Da*De_Vbi_ci*d10_logE0);
#endif
Dv_j2=((-Dv_p)+(Da*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=0.0;
De_2_Vtnode_GND=0.0;
De_2_Vbi_ci=0.0;
#endif
De_2=1.0;
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=0.0;
Dv_j2_Vbp_ci=0.0;
Dv_j2_Vtnode_GND=Dv_j1_Vtnode_GND;
Dv_j2_Vbi_ci=Dv_j1_Vbi_ci;
#endif
Dv_j2=Dv_j1;
}
#if defined(_DERIVATE)
Dv_j4_Vsi_ci=0.0;
Dv_j4_Vb_ci=0.0;
Dv_j4_Vbp_ci=0.0;
Dv_j4_Vbi_ci=(1.0-Dv_j1_Vbi_ci);
Dv_j4_Vtnode_GND=(-Dv_j1_Vtnode_GND);
#endif
Dv_j4=(BP(bi,ci)-Dv_j1);
{
double m00_logE(d00_logE0,(1.0-(Dv_j1/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j1/vdci_t)))
#endif
#if defined(_DERIVATE)
DCln1_Vsi_ci=0.0;
DCln1_Vb_ci=0.0;
DCln1_Vbp_ci=0.0;
DCln1_Vtnode_GND=(-((Dv_j1_Vtnode_GND*vdci_t-Dv_j1*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
DCln1_Vbi_ci=(-(Dv_j1_Vbi_ci/vdci_t))*d10_logE0;
#endif
DCln1=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0-(Dv_j2/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j2/vdci_t)))
#endif
#if defined(_DERIVATE)
DCln2_Vsi_ci=0.0;
DCln2_Vb_ci=0.0;
DCln2_Vbp_ci=0.0;
DCln2_Vtnode_GND=(-((Dv_j2_Vtnode_GND*vdci_t-Dv_j2*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
DCln2_Vbi_ci=(-(Dv_j2_Vbi_ci/vdci_t))*d10_logE0;
#endif
DCln2=d00_logE0;
}
Dz1=(1.0-zci);
Dzr1=(1.0-Dz_r);
{
double m00_exp(d00_exp0,(DCln2*(-zci)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j1_Vsi_ci=0.0;
DC_j1_Vb_ci=0.0;
DC_j1_Vbp_ci=0.0;
DC_j1_Vtnode_GND=((((((cjci0_t_Vtnode_GND*d00_exp0)+(cjci0_t*DCln2_Vtnode_GND*(-zci)*d10_exp0))*De_1)+((cjci0_t*d00_exp0)*De_1_Vtnode_GND))*De_2)+(((cjci0_t*d00_exp0)*De_1)*De_2_Vtnode_GND));
DC_j1_Vbi_ci=(((((cjci0_t*DCln2_Vbi_ci*(-zci)*d10_exp0)*De_1)+((cjci0_t*d00_exp0)*De_1_Vbi_ci))*De_2)+(((cjci0_t*d00_exp0)*De_1)*De_2_Vbi_ci));
#endif
DC_j1=(((cjci0_t*d00_exp0)*De_1)*De_2);
}
{
double m00_exp(d00_exp0,(DCln1*(-Dz_r)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j2_Vsi_ci=0.0;
DC_j2_Vb_ci=0.0;
DC_j2_Vbp_ci=0.0;
DC_j2_Vtnode_GND=((((DC_c_Vtnode_GND*d00_exp0)+(DC_c*DCln1_Vtnode_GND*(-Dz_r)*d10_exp0))*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vtnode_GND)));
DC_j2_Vbi_ci=(((DC_c*DCln1_Vbi_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbi_ci)));
#endif
DC_j2=((DC_c*d00_exp0)*(1.0-De_2));
}
#if defined(_DERIVATE)
DC_j3_Vsi_ci=0.0;
DC_j3_Vb_ci=0.0;
DC_j3_Vbp_ci=0.0;
DC_j3_Vtnode_GND=((DC_max_Vtnode_GND*(1.0-De_1))+(DC_max*(-De_1_Vtnode_GND)));
DC_j3_Vbi_ci=(DC_max*(-De_1_Vbi_ci));
#endif
DC_j3=(DC_max*(1.0-De_1));
#if defined(_DERIVATE)
Cjci_Vbi_ei=0.0;
Cjci_Vtnode_GND=((DC_j1_Vtnode_GND+DC_j2_Vtnode_GND)+DC_j3_Vtnode_GND);
Cjci_Vbi_ci=((DC_j1_Vbi_ci+DC_j2_Vbi_ci)+DC_j3_Vbi_ci);
#endif
Cjci=((DC_j1+DC_j2)+DC_j3);
{
double m00_exp(d00_exp0,(DCln2*Dz1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j1_Vsi_ci=0.0;
DQ_j1_Vb_ci=0.0;
DQ_j1_Vbp_ci=0.0;
DQ_j1_Vtnode_GND=(((cjci0_t_Vtnode_GND*(1.0-d00_exp0))+(cjci0_t*(-DCln2_Vtnode_GND*Dz1*d10_exp0)))/Dz1);
DQ_j1_Vbi_ci=((cjci0_t*(-DCln2_Vbi_ci*Dz1*d10_exp0))/Dz1);
#endif
DQ_j1=((cjci0_t*(1.0-d00_exp0))/Dz1);
}
{
double m00_exp(d00_exp0,(DCln1*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j2_Vsi_ci=0.0;
DQ_j2_Vb_ci=0.0;
DQ_j2_Vbp_ci=0.0;
DQ_j2_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln1_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j2_Vbi_ci=((DC_c*(-DCln1_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j2=((DC_c*(1.0-d00_exp0))/Dzr1);
}
{
double m00_exp(d00_exp0,(DCln2*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j3_Vsi_ci=0.0;
DQ_j3_Vb_ci=0.0;
DQ_j3_Vbp_ci=0.0;
DQ_j3_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln2_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j3_Vbi_ci=((DC_c*(-DCln2_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j3=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#if defined(_DERIVATE)
Qjci_Vbi_ei=0.0;
Qjci_Vtnode_GND=(((((DQ_j1_Vtnode_GND+DQ_j2_Vtnode_GND)-DQ_j3_Vtnode_GND)*vdci_t)+(((DQ_j1+DQ_j2)-DQ_j3)*vdci_t_Vtnode_GND))+((DC_max_Vtnode_GND*Dv_j4)+(DC_max*Dv_j4_Vtnode_GND)));
Qjci_Vbi_ci=(((DQ_j1_Vbi_ci+DQ_j2_Vbi_ci)-DQ_j3_Vbi_ci)*vdci_t+(DC_max*Dv_j4_Vbi_ci));
#endif
Qjci=((((DQ_j1+DQ_j2)-DQ_j3)*vdci_t)+(DC_max*Dv_j4));
}
else
{
#if defined(_DERIVATE)
Cjci_Vbi_ei=0.0;
Cjci_Vtnode_GND=0.0;
Cjci_Vbi_ci=0.0;
#endif
Cjci=0.0;
#if defined(_DERIVATE)
Qjci_Vbi_ei=0.0;
Qjci_Vtnode_GND=0.0;
Qjci_Vbi_ci=0.0;
#endif
Qjci=0.0;
}
}
else
{
if
((cjci0_t>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DFV_f=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=(2.4*cjci0_t_Vtnode_GND);
#endif
DFC_max=(2.4*cjci0_t);
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=((-1.0)/VT);
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(bi,ci))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(bi,ci))/VT);
if
((DFv_e<80.0))
{
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=0.0;
DFe_Vbp_ci=0.0;
DFe_Vbp_ei=0.0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
}
else
{
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=1.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(bi,ci);
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
}
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdci_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=0.0;
DFb_Vbp_ci=0.0;
DFb_Vbp_ei=0.0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vdci_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdci_t-DFv_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdci_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
{
double m00_exp(d00_exp0,((-zci)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=0.0;
DFC_j1_Vbp_ci=0.0;
DFC_j1_Vbp_ei=0.0;
DFC_j1_Vbi_ci=(((cjci0_t*((-zci)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjci0_t_Vtnode_GND*d00_exp0)+(cjci0_t*((-zci)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjci0_t*((-zci)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjci0_t*d00_exp0)*DFdvj_dv);
}
#if defined(_DERIVATE)
Cjci_Vbi_ei=(DFC_j1_Vbi_ei+(DFC_max*(-DFdvj_dv_Vbi_ei)));
Cjci_Vtnode_GND=(DFC_j1_Vtnode_GND+((DFC_max_Vtnode_GND*(1.0-DFdvj_dv))+(DFC_max*(-DFdvj_dv_Vtnode_GND))));
Cjci_Vbi_ci=(DFC_j1_Vbi_ci+(DFC_max*(-DFdvj_dv_Vbi_ci)));
#endif
Cjci=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
{
double m00_exp(d00_exp0,(DFb*(1.0-zci)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=0.0;
DFQ_j_Vbp_ci=0.0;
DFQ_j_Vbp_ei=0.0;
DFQ_j_Vbi_ci=(((cjci0_t*vdci_t)*(-DFb_Vbi_ci*(1.0-zci)*d10_exp0))/(1.0-zci));
DFQ_j_Vtnode_GND=(((((cjci0_t_Vtnode_GND*vdci_t)+(cjci0_t*vdci_t_Vtnode_GND))*(1.0-d00_exp0))+((cjci0_t*vdci_t)*(-DFb_Vtnode_GND*(1.0-zci)*d10_exp0)))/(1.0-zci));
DFQ_j_Vbi_ei=(((cjci0_t*vdci_t)*(-DFb_Vbi_ei*(1.0-zci)*d10_exp0))/(1.0-zci));
#endif
DFQ_j=(((cjci0_t*vdci_t)*(1.0-d00_exp0))/(1.0-zci));
}
#if defined(_DERIVATE)
Qjci_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(-DFv_j_Vbi_ei)));
Qjci_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(bi,ci)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
Qjci_Vbi_ci=(DFQ_j_Vbi_ci+(DFC_max*(1.0-DFv_j_Vbi_ci)));
#endif
Qjci=(DFQ_j+(DFC_max*(BP(bi,ci)-DFv_j)));
}
else
{
#if defined(_DERIVATE)
Cjci_Vbi_ei=0.0;
Cjci_Vtnode_GND=0.0;
Cjci_Vbi_ci=0.0;
#endif
Cjci=0.0;
#if defined(_DERIVATE)
Qjci_Vbi_ei=0.0;
Qjci_Vtnode_GND=0.0;
Qjci_Vbi_ci=0.0;
#endif
Qjci=0.0;
}
}
#if defined(_DERIVATE)
Q_0_Vtnode_GND=((qp0_t_Vtnode_GND+(hjei*Qjei_Vtnode_GND))+(hjci*Qjci_Vtnode_GND));
Q_0_Vbi_ei=((hjei*Qjei_Vbi_ei)+(hjci*Qjci_Vbi_ei));
Q_0_Vbi_ci=(hjci*Qjci_Vbi_ci);
#endif
Q_0=((qp0_t+(hjei*Qjei))+(hjci*Qjci));
if
((Q_0<(0.6*qp0_t)))
{
a_bpt=0.05;
#if defined(_DERIVATE)
Q_bpt_Vtnode_GND=(a_bpt*qp0_t_Vtnode_GND);
#endif
Q_bpt=(a_bpt*qp0_t);
#if defined(_DERIVATE)
b_q_Vtnode_GND=((Q_0_Vtnode_GND*Q_bpt-Q_0*Q_bpt_Vtnode_GND)/Q_bpt/Q_bpt);
b_q_Vbi_ei=(Q_0_Vbi_ei/Q_bpt);
b_q_Vbi_ci=(Q_0_Vbi_ci/Q_bpt);
#endif
b_q=((Q_0/Q_bpt)-1.0);
{
double m00_exp(d00_exp0,b_q)
double m00_logE(d00_logE1,(1+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1+d00_exp0))
#endif
#if defined(_DERIVATE)
Q_0_Vtnode_GND=((Q_bpt_Vtnode_GND*(1+d00_logE1))+(Q_bpt*b_q_Vtnode_GND*d10_exp0*d10_logE1));
Q_0_Vbi_ei=(Q_bpt*b_q_Vbi_ei*d10_exp0*d10_logE1);
Q_0_Vbi_ci=(Q_bpt*b_q_Vbi_ci*d10_exp0*d10_logE1);
#endif
Q_0=(Q_bpt*(1+d00_logE1));
}
}
if
((cjci0_t>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DFV_f=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=(2.4*cjci0_t_Vtnode_GND);
#endif
DFC_max=(2.4*cjci0_t);
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=((-1.0)/VT);
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(bi,ci))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(bi,ci))/VT);
if
((DFv_e<80.0))
{
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=0.0;
DFe_Vbp_ci=0.0;
DFe_Vbp_ei=0.0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
}
else
{
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=1.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(bi,ci);
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
}
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdci_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=0.0;
DFb_Vbp_ci=0.0;
DFb_Vbp_ei=0.0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vdci_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdci_t-DFv_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdci_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
{
double m00_exp(d00_exp0,((-zci)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=0.0;
DFC_j1_Vbp_ci=0.0;
DFC_j1_Vbp_ei=0.0;
DFC_j1_Vbi_ci=(((cjci0_t*((-zci)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjci0_t_Vtnode_GND*d00_exp0)+(cjci0_t*((-zci)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjci0_t*((-zci)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjci0_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjci0_t*d00_exp0)*DFdvj_dv);
}
#if defined(_DERIVATE)
Cjcit_Vbi_ci=(DFC_j1_Vbi_ci+(DFC_max*(-DFdvj_dv_Vbi_ci)));
Cjcit_Vtnode_GND=(DFC_j1_Vtnode_GND+((DFC_max_Vtnode_GND*(1.0-DFdvj_dv))+(DFC_max*(-DFdvj_dv_Vtnode_GND))));
Cjcit_Vbi_ei=(DFC_j1_Vbi_ei+(DFC_max*(-DFdvj_dv_Vbi_ei)));
#endif
Cjcit=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DFb*(1.0-zci)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=0.0;
DFQ_j_Vbp_ci=0.0;
DFQ_j_Vbp_ei=0.0;
DFQ_j_Vbi_ci=(((cjci0_t*vdci_t)*(-DFb_Vbi_ci*(1.0-zci)*d10_exp0))/(1.0-zci));
DFQ_j_Vtnode_GND=(((((cjci0_t_Vtnode_GND*vdci_t)+(cjci0_t*vdci_t_Vtnode_GND))*(1.0-d00_exp0))+((cjci0_t*vdci_t)*(-DFb_Vtnode_GND*(1.0-zci)*d10_exp0)))/(1.0-zci));
DFQ_j_Vbi_ei=(((cjci0_t*vdci_t)*(-DFb_Vbi_ei*(1.0-zci)*d10_exp0))/(1.0-zci));
#endif
DFQ_j=(((cjci0_t*vdci_t)*(1.0-d00_exp0))/(1.0-zci));
}
#endif
#if defined(_DYNAMIC)
d1=(DFQ_j+(DFC_max*(BP(bi,ci)-DFv_j)));
#endif
}
else
{
#if defined(_DERIVATE)
Cjcit_Vbi_ci=0.0;
Cjcit_Vtnode_GND=0.0;
Cjcit_Vbi_ei=0.0;
#endif
Cjcit=0.0;
#if defined(_DYNAMIC)
d1=0.0;
#endif
}
if
((Cjcit>0.0))
{
#if defined(_DERIVATE)
cc_Vtnode_GND=((cjci0_t_Vtnode_GND*Cjcit-cjci0_t*Cjcit_Vtnode_GND)/Cjcit/Cjcit);
cc_Vbi_ci=(-cjci0_t*Cjcit_Vbi_ci/Cjcit/Cjcit);
cc_Vbi_ei=(-cjci0_t*Cjcit_Vbi_ei/Cjcit/Cjcit);
#endif
cc=(cjci0_t/Cjcit);
}
else
{
#if defined(_DERIVATE)
cc_Vtnode_GND=0.0;
cc_Vbi_ci=0.0;
cc_Vbi_ei=0.0;
#endif
cc=1.0;
}
#if defined(_DERIVATE)
T_f0_Vtnode_GND=((t0_t_Vtnode_GND+(dt0h*cc_Vtnode_GND))+(tbvl*(-1*cc_Vtnode_GND/cc/cc)));
T_f0_Vbi_ci=((dt0h*cc_Vbi_ci)+(tbvl*(-1*cc_Vbi_ci/cc/cc)));
T_f0_Vbi_ei=((dt0h*cc_Vbi_ei)+(tbvl*(-1*cc_Vbi_ei/cc/cc)));
#endif
T_f0=((t0_t+(dt0h*(cc-1.0)))+(tbvl*((1/cc)-1.0)));
#if defined(_DERIVATE)
vc_Vci_ei=1.0;
vc_Vtnode_GND=(-vces_t_Vtnode_GND);
#endif
vc=(BP(ci,ei)-vces_t);
#if defined(_DERIVATE)
ICKv_Vci_ei=(vc_Vci_ei/VT);
ICKv_Vtnode_GND=((vc_Vtnode_GND*VT-vc*VT_Vtnode_GND)/VT/VT);
#endif
ICKv=(vc/VT);
if
((ICKv<30.0))
{
{
double m00_exp(d00_exp0,(ICKv-1.0))
double m00_logE(d00_logE1,(1+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1+d00_exp0))
#endif
#if defined(_DERIVATE)
vceff_Vci_ei=ICKv_Vci_ei*d10_exp0*d10_logE1*VT;
vceff_Vtnode_GND=((ICKv_Vtnode_GND*d10_exp0*d10_logE1*VT)+((1.0+d00_logE1)*VT_Vtnode_GND));
#endif
vceff=((1.0+d00_logE1)*VT);
}
}
else
{
#if defined(_DERIVATE)
vceff_Vci_ei=vc_Vci_ei;
vceff_Vtnode_GND=vc_Vtnode_GND;
#endif
vceff=vc;
}
#if defined(_DERIVATE)
ICKv_Vci_ei=(vceff_Vci_ei/vlim_t);
ICKv_Vtnode_GND=((vceff_Vtnode_GND*vlim_t-vceff*vlim_t_Vtnode_GND)/vlim_t/vlim_t);
#endif
ICKv=(vceff/vlim_t);
{
double m00_sqrt(d00_sqrt0,(1.0+(ICKv*ICKv)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+(ICKv*ICKv)))
#endif
#if defined(_DERIVATE)
ick_Vci_ei=((vceff_Vci_ei*Orci0_t*d00_sqrt0-(vceff*Orci0_t)*((ICKv_Vci_ei*ICKv)+(ICKv*ICKv_Vci_ei))*d10_sqrt0)/d00_sqrt0/d00_sqrt0);
ick_Vtnode_GND=((((vceff_Vtnode_GND*Orci0_t)+(vceff*Orci0_t_Vtnode_GND))*d00_sqrt0-(vceff*Orci0_t)*((ICKv_Vtnode_GND*ICKv)+(ICKv*ICKv_Vtnode_GND))*d10_sqrt0)/d00_sqrt0/d00_sqrt0);
#endif
ick=((vceff*Orci0_t)/d00_sqrt0);
}
#if defined(_DERIVATE)
ICKa_Vci_ei=vceff_Vci_ei*Ovpt;
ICKa_Vtnode_GND=(vceff_Vtnode_GND-vlim_t_Vtnode_GND)*Ovpt;
#endif
ICKa=((vceff-vlim_t)*Ovpt);
{
double m00_sqrt(d00_sqrt0,((ICKa*ICKa)+1.0e-3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((ICKa*ICKa)+1.0e-3))
#endif
#if defined(_DERIVATE)
ick_Vci_ei=((ick_Vci_ei*(1.0+(0.5*(ICKa+d00_sqrt0))))+(ick*(0.5*(ICKa_Vci_ei+((ICKa_Vci_ei*ICKa)+(ICKa*ICKa_Vci_ei))*d10_sqrt0))));
ick_Vtnode_GND=((ick_Vtnode_GND*(1.0+(0.5*(ICKa+d00_sqrt0))))+(ick*(0.5*(ICKa_Vtnode_GND+((ICKa_Vtnode_GND*ICKa)+(ICKa*ICKa_Vtnode_GND))*d10_sqrt0))));
#endif
ick=(ick*(1.0+(0.5*(ICKa+d00_sqrt0))));
}
if
(((T_f0>0.0)||(Tr>0.0)))
{
#if defined(_DERIVATE)
A_Vtnode_GND=(0.5*Q_0_Vtnode_GND);
A_Vbi_ei=(0.5*Q_0_Vbi_ei);
A_Vbi_ci=(0.5*Q_0_Vbi_ci);
#endif
A=(0.5*Q_0);
{
double m00_sqrt(d00_sqrt0,(((A*A)+(T_f0*i_0f))+(Tr*i_0r)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(((A*A)+(T_f0*i_0f))+(Tr*i_0r)))
#endif
#if defined(_DERIVATE)
Q_p_Vtnode_GND=(A_Vtnode_GND+((((A_Vtnode_GND*A)+(A*A_Vtnode_GND))+((T_f0_Vtnode_GND*i_0f)+(T_f0*i_0f_Vtnode_GND)))+(Tr*i_0r_Vtnode_GND))*d10_sqrt0);
Q_p_Vbi_ei=(A_Vbi_ei+(((A_Vbi_ei*A)+(A*A_Vbi_ei))+((T_f0_Vbi_ei*i_0f)+(T_f0*i_0f_Vbi_ei)))*d10_sqrt0);
Q_p_Vbi_ci=(A_Vbi_ci+((((A_Vbi_ci*A)+(A*A_Vbi_ci))+T_f0_Vbi_ci*i_0f)+(Tr*i_0r_Vbi_ci))*d10_sqrt0);
#endif
Q_p=(A+d00_sqrt0);
}
}
else
{
#if defined(_DERIVATE)
Q_p_Vtnode_GND=Q_0_Vtnode_GND;
Q_p_Vbi_ei=Q_0_Vbi_ei;
Q_p_Vbi_ci=Q_0_Vbi_ci;
#endif
Q_p=Q_0;
}
#if defined(_DERIVATE)
I_Tf1_Vci_ei=0.0;
I_Tf1_Vtnode_GND=((i_0f_Vtnode_GND*Q_p-i_0f*Q_p_Vtnode_GND)/Q_p/Q_p);
I_Tf1_Vbi_ei=((i_0f_Vbi_ei*Q_p-i_0f*Q_p_Vbi_ei)/Q_p/Q_p);
I_Tf1_Vbi_ci=(-i_0f*Q_p_Vbi_ci/Q_p/Q_p);
#endif
I_Tf1=(i_0f/Q_p);
#if defined(_DERIVATE)
a_h_Vci_ei=0.0;
a_h_Vtnode_GND=(Oich*I_Tf1_Vtnode_GND);
a_h_Vbi_ei=(Oich*I_Tf1_Vbi_ei);
a_h_Vbi_ci=(Oich*I_Tf1_Vbi_ci);
#endif
a_h=(Oich*I_Tf1);
#if defined(_DERIVATE)
itf_Vci_ei=0.0;
itf_Vtnode_GND=((I_Tf1_Vtnode_GND*(1.0+a_h))+(I_Tf1*a_h_Vtnode_GND));
itf_Vbi_ei=((I_Tf1_Vbi_ei*(1.0+a_h))+(I_Tf1*a_h_Vbi_ei));
itf_Vbi_ci=((I_Tf1_Vbi_ci*(1.0+a_h))+(I_Tf1*a_h_Vbi_ci));
#endif
itf=(I_Tf1*(1.0+a_h));
#if defined(_DERIVATE)
itr_Vci_ei=0.0;
itr_Vtnode_GND=((i_0r_Vtnode_GND*Q_p-i_0r*Q_p_Vtnode_GND)/Q_p/Q_p);
itr_Vbi_ci=((i_0r_Vbi_ci*Q_p-i_0r*Q_p_Vbi_ci)/Q_p/Q_p);
itr_Vbi_ei=(-i_0r*Q_p_Vbi_ei/Q_p/Q_p);
#endif
itr=(i_0r/Q_p);
#if defined(_DERIVATE)
Tf_Vci_ei=0.0;
Tf_Vtnode_GND=T_f0_Vtnode_GND;
Tf_Vbi_ci=T_f0_Vbi_ci;
Tf_Vbi_ei=T_f0_Vbi_ei;
#endif
Tf=T_f0;
#if defined(_DERIVATE)
Qf_Vci_ei=0.0;
Qf_Vtnode_GND=((T_f0_Vtnode_GND*itf)+(T_f0*itf_Vtnode_GND));
Qf_Vbi_ci=((T_f0_Vbi_ci*itf)+(T_f0*itf_Vbi_ci));
Qf_Vbi_ei=((T_f0_Vbi_ei*itf)+(T_f0*itf_Vbi_ei));
#endif
Qf=(T_f0*itf);
if
((itf<(1.0e-6*ick)))
{
#if defined(_DERIVATE)
Q_fT_Vci_ei=0.0;
Q_fT_Vtnode_GND=Qf_Vtnode_GND;
Q_fT_Vbi_ci=Qf_Vbi_ci;
Q_fT_Vbi_ei=Qf_Vbi_ei;
#endif
Q_fT=Qf;
#if defined(_DERIVATE)
T_fT_Vci_ei=0.0;
T_fT_Vtnode_GND=Tf_Vtnode_GND;
T_fT_Vbi_ci=Tf_Vbi_ci;
T_fT_Vbi_ei=Tf_Vbi_ei;
#endif
T_fT=Tf;
}
else
{
#if defined(_DERIVATE)
FFa_Vci_ei=((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf);
FFa_Vtnode_GND=((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf);
FFa_Vbi_ei=(-ick*itf_Vbi_ei/itf/itf);
FFa_Vbi_ci=(-ick*itf_Vbi_ci/itf/itf);
#endif
FFa=(ick/itf);
{
double m00_logE(d00_logE0,FFa)
double m00_exp(d00_exp1,((-gtfe)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FFa)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
FFd_TfE_Vtnode_GND=((tef0_t_Vtnode_GND*d00_exp1)+(tef0_t*((-gtfe)*FFa_Vtnode_GND*d10_logE0)*d10_exp1));
FFd_TfE_Vci_ei=(tef0_t*((-gtfe)*FFa_Vci_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ei=(tef0_t*((-gtfe)*FFa_Vbi_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ci=(tef0_t*((-gtfe)*FFa_Vbi_ci*d10_logE0)*d10_exp1);
#endif
FFd_TfE=(tef0_t*d00_exp1);
}
#if defined(_DERIVATE)
FFd_QfE_Vtnode_GND=(((FFd_TfE_Vtnode_GND*itf)+(FFd_TfE*itf_Vtnode_GND))/(gtfe+1.0));
FFd_QfE_Vci_ei=(((FFd_TfE_Vci_ei*itf)+(FFd_TfE*itf_Vci_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ei=(((FFd_TfE_Vbi_ei*itf)+(FFd_TfE*itf_Vbi_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ci=(((FFd_TfE_Vbi_ci*itf)+(FFd_TfE*itf_Vbi_ci))/(gtfe+1.0));
#endif
FFd_QfE=((FFd_TfE*itf)/(gtfe+1.0));
#if defined(_DERIVATE)
FFT_fbS_Vtnode_GND=((1.0-fthc)*thcs_t_Vtnode_GND);
#endif
FFT_fbS=((1.0-fthc)*thcs_t);
#if defined(_DERIVATE)
FFx_Vci_ei=(-FFa_Vci_ei);
FFx_Vtnode_GND=(-FFa_Vtnode_GND);
FFx_Vbi_ei=(-FFa_Vbi_ei);
FFx_Vbi_ci=(-FFa_Vbi_ci);
#endif
FFx=(1.0-FFa);
{
double m00_sqrt(d00_sqrt0,((FFx*FFx)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FFx*FFx)+alhc))
#endif
#if defined(_DERIVATE)
FFs_Vci_ei=((FFx_Vci_ei*FFx)+(FFx*FFx_Vci_ei))*d10_sqrt0;
FFs_Vtnode_GND=((FFx_Vtnode_GND*FFx)+(FFx*FFx_Vtnode_GND))*d10_sqrt0;
FFs_Vbi_ei=((FFx_Vbi_ei*FFx)+(FFx*FFx_Vbi_ei))*d10_sqrt0;
FFs_Vbi_ci=((FFx_Vbi_ci*FFx)+(FFx*FFx_Vbi_ci))*d10_sqrt0;
#endif
FFs=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FFw_Vci_ei=((FFx_Vci_ei+FFs_Vci_ei)/(1.0+d00_sqrt0));
FFw_Vtnode_GND=((FFx_Vtnode_GND+FFs_Vtnode_GND)/(1.0+d00_sqrt0));
FFw_Vbi_ei=((FFx_Vbi_ei+FFs_Vbi_ei)/(1.0+d00_sqrt0));
FFw_Vbi_ci=((FFx_Vbi_ci+FFs_Vbi_ci)/(1.0+d00_sqrt0));
#endif
FFw=((FFx+FFs)/(1.0+d00_sqrt0));
}
#if defined(_DERIVATE)
FFw_2_Vci_ei=((FFw_Vci_ei*FFw)+(FFw*FFw_Vci_ei));
FFw_2_Vtnode_GND=((FFw_Vtnode_GND*FFw)+(FFw*FFw_Vtnode_GND));
FFw_2_Vbi_ei=((FFw_Vbi_ei*FFw)+(FFw*FFw_Vbi_ei));
FFw_2_Vbi_ci=((FFw_Vbi_ci*FFw)+(FFw*FFw_Vbi_ci));
#endif
FFw_2=(FFw*FFw);
#if defined(_DERIVATE)
FFd_QfB_Vtnode_GND=((((FFT_fbS_Vtnode_GND*itf)+(FFT_fbS*itf_Vtnode_GND))*FFw_2)+((FFT_fbS*itf)*FFw_2_Vtnode_GND));
FFd_QfB_Vbi_ei=(((FFT_fbS*itf_Vbi_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ei));
FFd_QfB_Vbi_ci=(((FFT_fbS*itf_Vbi_ci)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ci));
FFd_QfB_Vci_ei=(((FFT_fbS*itf_Vci_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vci_ei));
#endif
FFd_QfB=((FFT_fbS*itf)*FFw_2);
#if defined(_DERIVATE)
FFa_w_Vci_ei=((FFw_2_Vci_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vci_ei)*FFs-(2.0*FFa)*FFs_Vci_ei)/FFs/FFs)));
FFa_w_Vtnode_GND=((FFw_2_Vtnode_GND*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vtnode_GND)*FFs-(2.0*FFa)*FFs_Vtnode_GND)/FFs/FFs)));
FFa_w_Vbi_ei=((FFw_2_Vbi_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ei)*FFs-(2.0*FFa)*FFs_Vbi_ei)/FFs/FFs)));
FFa_w_Vbi_ci=((FFw_2_Vbi_ci*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ci)*FFs-(2.0*FFa)*FFs_Vbi_ci)/FFs/FFs)));
#endif
FFa_w=(FFw_2*(1.0+((2.0*FFa)/FFs)));
#if defined(_DERIVATE)
FFd_TfB_Vtnode_GND=((FFT_fbS_Vtnode_GND*FFa_w)+(FFT_fbS*FFa_w_Vtnode_GND));
FFd_TfB_Vci_ei=(FFT_fbS*FFa_w_Vci_ei);
FFd_TfB_Vbi_ei=(FFT_fbS*FFa_w_Vbi_ei);
FFd_TfB_Vbi_ci=(FFT_fbS*FFa_w_Vbi_ci);
#endif
FFd_TfB=(FFT_fbS*FFa_w);
#if defined(_DERIVATE)
FFT_pcS_Vtnode_GND=(fthc*thcs_t_Vtnode_GND);
#endif
FFT_pcS=(fthc*thcs_t);
if
(((latb<=0.0)&&(latl<=0.0)))
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND))*FFw_2)+((FFT_pcS*itf)*FFw_2_Vtnode_GND));
FFQ_fC_Vbi_ei=(((FFT_pcS*itf_Vbi_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ei));
FFQ_fC_Vbi_ci=(((FFT_pcS*itf_Vbi_ci)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ci));
FFQ_fC_Vci_ei=(((FFT_pcS*itf_Vci_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vci_ei));
#endif
FFQ_fC=((FFT_pcS*itf)*FFw_2);
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*FFa_w)+(FFT_pcS*FFa_w_Vtnode_GND));
FFT_fC_Vci_ei=(FFT_pcS*FFa_w_Vci_ei);
FFT_fC_Vbi_ei=(FFT_pcS*FFa_w_Vbi_ei);
FFT_fC_Vbi_ci=(FFT_pcS*FFa_w_Vbi_ci);
#endif
FFT_fC=(FFT_pcS*FFa_w);
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=FFQ_fC_Vtnode_GND;
FFQ_cT_Vbi_ei=FFQ_fC_Vbi_ei;
FFQ_cT_Vbi_ci=FFQ_fC_Vbi_ci;
FFQ_cT_Vci_ei=FFQ_fC_Vci_ei;
#endif
FFQ_cT=FFQ_fC;
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=FFT_fC_Vtnode_GND;
FFT_cT_Vci_ei=FFT_fC_Vci_ei;
FFT_cT_Vbi_ei=FFT_fC_Vbi_ei;
FFT_cT_Vbi_ci=FFT_fC_Vbi_ci;
#endif
FFT_cT=FFT_fC;
}
else
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND));
FFQ_fC_Vbi_ei=(FFT_pcS*itf_Vbi_ei);
FFQ_fC_Vbi_ci=(FFT_pcS*itf_Vbi_ci);
FFQ_fC_Vci_ei=0.0;
#endif
FFQ_fC=(FFT_pcS*itf);
#if defined(_DERIVATE)
FCa_Vci_ei=(-((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf));
FCa_Vtnode_GND=(-((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf));
FCa_Vbi_ei=(-(-ick*itf_Vbi_ei/itf/itf));
FCa_Vbi_ci=(-(-ick*itf_Vbi_ci/itf/itf));
#endif
FCa=(1.0-(ick/itf));
{
double m00_sqrt(d00_sqrt0,((FCa*FCa)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FCa*FCa)+alhc))
#endif
#if defined(_DERIVATE)
FCrt_Vci_ei=((FCa_Vci_ei*FCa)+(FCa*FCa_Vci_ei))*d10_sqrt0;
FCrt_Vtnode_GND=((FCa_Vtnode_GND*FCa)+(FCa*FCa_Vtnode_GND))*d10_sqrt0;
FCrt_Vbi_ei=((FCa_Vbi_ei*FCa)+(FCa*FCa_Vbi_ei))*d10_sqrt0;
FCrt_Vbi_ci=((FCa_Vbi_ci*FCa)+(FCa*FCa_Vbi_ci))*d10_sqrt0;
#endif
FCrt=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FCa_ck_Vci_ei=(-((FCa_Vci_ei+FCrt_Vci_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vtnode_GND=(-((FCa_Vtnode_GND+FCrt_Vtnode_GND)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ei=(-((FCa_Vbi_ei+FCrt_Vbi_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ci=(-((FCa_Vbi_ci+FCrt_Vbi_ci)/(1.0+d00_sqrt0)));
#endif
FCa_ck=(1.0-((FCa+FCrt)/(1.0+d00_sqrt0)));
}
#if defined(_DERIVATE)
FCdaick_ditf_Vci_ei=((((FCa_ck_Vci_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vci_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vci_ei*itf)+(FCrt*itf_Vci_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vtnode_GND=((((FCa_ck_Vtnode_GND*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vtnode_GND)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vtnode_GND*itf)+(FCrt*itf_Vtnode_GND)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ei=((((FCa_ck_Vbi_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ei*itf)+(FCrt*itf_Vbi_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ci=((((FCa_ck_Vbi_ci*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ci)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ci*itf)+(FCrt*itf_Vbi_ci)))/(FCrt*itf)/(FCrt*itf));
#endif
FCdaick_ditf=(((FCa_ck-1.0)*(1-FCa))/(FCrt*itf));
if
((latb>latl))
{
#if defined(_DERIVATE)
FCz_Vci_ei=0.0;
FCz_Vtnode_GND=0.0;
FCz_Vbi_ei=0.0;
FCz_Vbi_ci=0.0;
#endif
FCz=(latb-latl);
FCxl=(1.0+latl);
FCxb=(1.0+latb);
if
((latb>0.01))
{
{
double m00_logE(d00_logE0,(FCxb/FCxl))
FCln=d00_logE0;
}
{
double m00_exp(d00_exp0,((FCa_ck-1.0)*FCln))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
FCa1_Vci_ei=FCa_ck_Vci_ei*FCln*d10_exp0;
FCa1_Vtnode_GND=FCa_ck_Vtnode_GND*FCln*d10_exp0;
FCa1_Vbi_ei=FCa_ck_Vbi_ei*FCln*d10_exp0;
FCa1_Vbi_ci=FCa_ck_Vbi_ci*FCln*d10_exp0;
#endif
FCa1=d00_exp0;
}
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCa1_Vci_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vtnode_GND=(-(-FCa1_Vtnode_GND*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ei=(-(-FCa1_Vbi_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ci=(-(-FCa1_Vbi_ci*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
#endif
FCd_a=(1.0/(latl-(FCa1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCa1_Vci_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCa1_Vtnode_GND*FCd_a)+((FCa1-1.0)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCa1_Vbi_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCa1_Vbi_ci*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ci));
#endif
FCw=((FCa1-1.0)*FCd_a);
{
double m00_logE(d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#endif
#if defined(_DERIVATE)
FCa_Vci_ei=(((latb*FCw_Vci_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vci_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vtnode_GND=(((latb*FCw_Vtnode_GND)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vtnode_GND))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ei=(((latb*FCw_Vbi_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ci=(((latb*FCw_Vbi_ci)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ci))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
#endif
FCa=d00_logE0;
}
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=(((((((-FCz_Vci_ei)*FCa1)+((-FCz)*FCa1_Vci_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vci_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=(((((((-FCz_Vtnode_GND)*FCa1)+((-FCz)*FCa1_Vtnode_GND))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vtnode_GND))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=(((((((-FCz_Vbi_ei)*FCa1)+((-FCz)*FCa1_Vbi_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=(((((((-FCz_Vbi_ci)*FCa1)+((-FCz)*FCa1_Vbi_ci))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ci))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a);
{
double m00_logE(d00_logE0,(1.0+(latb*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latb*FCw)))
#endif
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei)*d10_logE0;
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND)*d10_logE0;
FClnb_Vbi_ei=(latb*FCw_Vbi_ei)*d10_logE0;
FClnb_Vbi_ci=(latb*FCw_Vbi_ci)*d10_logE0;
#endif
FClnb=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0+(latl*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latl*FCw)))
#endif
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei)*d10_logE0;
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND)*d10_logE0;
FClnl_Vbi_ei=(latl*FCw_Vbi_ei)*d10_logE0;
FClnl_Vbi_ci=(latl*FCw_Vbi_ci)*d10_logE0;
#endif
FClnl=d00_logE0;
}
#if defined(_DERIVATE)
FCa1_Vci_ei=(FClnb_Vci_ei-FClnl_Vci_ei);
FCa1_Vtnode_GND=(FClnb_Vtnode_GND-FClnl_Vtnode_GND);
FCa1_Vbi_ei=(FClnb_Vbi_ei-FClnl_Vbi_ei);
FCa1_Vbi_ci=(FClnb_Vbi_ci-FClnl_Vbi_ci);
#endif
FCa1=(FClnb-FClnl);
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=((-latb*(latb*FCw_Vci_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vci_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vtnode_GND=((-latb*(latb*FCw_Vtnode_GND)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vtnode_GND)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ei=((-latb*(latb*FCw_Vbi_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ci=((-latb*(latb*FCw_Vbi_ci)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ci)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
#endif
FCda1_dw=((latb/(1.0+(latb*FCw)))-(latl/(1.0+(latl*FCw))));
}
else
{
#if defined(_DERIVATE)
FCf1_Vci_ei=(-FCa_ck_Vci_ei);
FCf1_Vtnode_GND=(-FCa_ck_Vtnode_GND);
FCf1_Vbi_ei=(-FCa_ck_Vbi_ei);
FCf1_Vbi_ci=(-FCa_ck_Vbi_ci);
#endif
FCf1=(1.0-FCa_ck);
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCf1_Vci_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vtnode_GND=(-(-FCf1_Vtnode_GND*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ei=(-(-FCf1_Vbi_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ci=(-(-FCf1_Vbi_ci*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
#endif
FCd_a=(1.0/(1.0-(FCf1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCf1_Vci_ei*FCd_a)+(FCf1*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCf1_Vtnode_GND*FCd_a)+(FCf1*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCf1_Vbi_ei*FCd_a)+(FCf1*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCf1_Vbi_ci*FCd_a)+(FCf1*FCd_a_Vbi_ci));
#endif
FCw=(FCf1*FCd_a);
#if defined(_DERIVATE)
FCa_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa=(FCz*FCw);
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-1.0)*FCd_a_Vci_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-1.0)*FCd_a_Vtnode_GND)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-1.0)*FCd_a_Vbi_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-1.0)*FCd_a_Vbi_ci)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-1.0)*FCd_a)*FCd_a);
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei);
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND);
FClnb_Vbi_ei=(latb*FCw_Vbi_ei);
FClnb_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FClnb=(latb*FCw);
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei);
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND);
FClnl_Vbi_ei=(latl*FCw_Vbi_ei);
FClnl_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
FClnl=(latl*FCw);
#if defined(_DERIVATE)
FCa1_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa1_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa1_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa1_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa1=(FCz*FCw);
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=0.0;
FCda1_dw_Vtnode_GND=0.0;
FCda1_dw_Vbi_ei=0.0;
FCda1_dw_Vbi_ci=0.0;
#endif
FCda1_dw=FCz;
}
#if defined(_DERIVATE)
FCf_CT_Vci_ei=0.0;
FCf_CT_Vbi_ei=0.0;
FCf_CT_Vbi_ci=0.0;
FCf_CT_Vtnode_GND=0.0;
#endif
FCf_CT=(2.0/FCz);
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCf1_Vci_ei=(((((((latb*latl)*FCw_Vci_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vci_ei))/3.0)+(((latb+latl)*FCw2_Vci_ei)/2.0))+FCw_Vci_ei);
FCf1_Vtnode_GND=(((((((latb*latl)*FCw_Vtnode_GND)*FCw2)+(((latb*latl)*FCw)*FCw2_Vtnode_GND))/3.0)+(((latb+latl)*FCw2_Vtnode_GND)/2.0))+FCw_Vtnode_GND);
FCf1_Vbi_ei=(((((((latb*latl)*FCw_Vbi_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ei))/3.0)+(((latb+latl)*FCw2_Vbi_ei)/2.0))+FCw_Vbi_ei);
FCf1_Vbi_ci=(((((((latb*latl)*FCw_Vbi_ci)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ci))/3.0)+(((latb+latl)*FCw2_Vbi_ci)/2.0))+FCw_Vbi_ci);
#endif
FCf1=((((((latb*latl)*FCw)*FCw2)/3.0)+(((latb+latl)*FCw2)/2.0))+FCw);
#if defined(_DERIVATE)
FCdf1_dw_Vci_ei=(((latb*latl)*FCw2_Vci_ei)+((latb+latl)*FCw_Vci_ei));
FCdf1_dw_Vtnode_GND=(((latb*latl)*FCw2_Vtnode_GND)+((latb+latl)*FCw_Vtnode_GND));
FCdf1_dw_Vbi_ei=(((latb*latl)*FCw2_Vbi_ei)+((latb+latl)*FCw_Vbi_ei));
FCdf1_dw_Vbi_ci=(((latb*latl)*FCw2_Vbi_ci)+((latb+latl)*FCw_Vbi_ci));
#endif
FCdf1_dw=((((latb*latl)*FCw2)+((latb+latl)*FCw))+1.0);
#if defined(_DERIVATE)
z_Vci_ei=(latb*FCw_Vci_ei);
z_Vtnode_GND=(latb*FCw_Vtnode_GND);
z_Vbi_ei=(latb*FCw_Vbi_ei);
z_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
z=(latb*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnb)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnb)-1.0))+1.0)/9.0);
r=(latl/latb);
#if defined(_DERIVATE)
FCf2_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latb);
FCf2_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latb);
FCf2_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latb);
FCf2_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latb);
#endif
FCf2=((((1.0-r)*a2)+(r*a3))/latb);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vci_ei));
FCdf2_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vtnode_GND));
FCdf2_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ei));
FCdf2_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ci));
#endif
FCdf2_dw=((((1.0-r)*x)+(r*a))*FClnb);
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf2_Vci_ei=(((((((latb*a2_Vci_ei)+(latl*a3_Vci_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vci_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf2_Vbi_ei=(((((((latb*a2_Vbi_ei)+(latl*a3_Vbi_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf2_Vbi_ci=(((((((latb*a2_Vbi_ci)+(latl*a3_Vbi_ci))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ci))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf2_Vtnode_GND=(((((((latb*a2_Vtnode_GND)+(latl*a3_Vtnode_GND))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vtnode_GND))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf2=(((((latb*a2)+(latl*a3))*FCw)*FCw)/6.0);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latl*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf2_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latl*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf2_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latl*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf2_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latl*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf2_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latl*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
}
#if defined(_DERIVATE)
z_Vci_ei=(latl*FCw_Vci_ei);
z_Vtnode_GND=(latl*FCw_Vtnode_GND);
z_Vbi_ei=(latl*FCw_Vbi_ei);
z_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
z=(latl*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnl)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnl)-1.0))+1.0)/9.0);
r=(latb/latl);
#if defined(_DERIVATE)
FCf3_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latl);
FCf3_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latl);
FCf3_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latl);
FCf3_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latl);
#endif
FCf3=((((1.0-r)*a2)+(r*a3))/latl);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vci_ei));
FCdf3_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vtnode_GND));
FCdf3_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ei));
FCdf3_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ci));
#endif
FCdf3_dw=((((1.0-r)*x)+(r*a))*FClnl);
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf3_Vci_ei=(((((((latl*a2_Vci_ei)+(latb*a3_Vci_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vci_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf3_Vbi_ei=(((((((latl*a2_Vbi_ei)+(latb*a3_Vbi_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf3_Vbi_ci=(((((((latl*a2_Vbi_ci)+(latb*a3_Vbi_ci))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ci))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf3_Vtnode_GND=(((((((latl*a2_Vtnode_GND)+(latb*a3_Vtnode_GND))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vtnode_GND))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf3=(((((latl*a2)+(latb*a3))*FCw)*FCw)/6.0);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latb*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf3_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latb*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf3_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latb*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf3_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latb*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf3_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latb*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
}
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((FCf_CT_Vci_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vci_ei*FCf1)+(FCa*FCf1_Vci_ei))-FCf2_Vci_ei)+FCf3_Vci_ei)));
FCf_ci_Vtnode_GND=((FCf_CT_Vtnode_GND*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vtnode_GND*FCf1)+(FCa*FCf1_Vtnode_GND))-FCf2_Vtnode_GND)+FCf3_Vtnode_GND)));
FCf_ci_Vbi_ei=((FCf_CT_Vbi_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ei*FCf1)+(FCa*FCf1_Vbi_ei))-FCf2_Vbi_ei)+FCf3_Vbi_ei)));
FCf_ci_Vbi_ci=((FCf_CT_Vbi_ci*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ci*FCf1)+(FCa*FCf1_Vbi_ci))-FCf2_Vbi_ci)+FCf3_Vbi_ci)));
#endif
FCf_ci=(FCf_CT*(((FCa*FCf1)-FCf2)+FCf3));
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=((FCf_CT_Vci_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vci_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vci_ei))+((FCda1_dw_Vci_ei*FCf1)+(FCda1_dw*FCf1_Vci_ei)))-FCdf2_dw_Vci_ei)+FCdf3_dw_Vci_ei)));
FCdfc_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vtnode_GND*FCdf1_dw)+(FCa1*FCdf1_dw_Vtnode_GND))+((FCda1_dw_Vtnode_GND*FCf1)+(FCda1_dw*FCf1_Vtnode_GND)))-FCdf2_dw_Vtnode_GND)+FCdf3_dw_Vtnode_GND)));
FCdfc_dw_Vbi_ei=((FCf_CT_Vbi_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ei))+((FCda1_dw_Vbi_ei*FCf1)+(FCda1_dw*FCf1_Vbi_ei)))-FCdf2_dw_Vbi_ei)+FCdf3_dw_Vbi_ei)));
FCdfc_dw_Vbi_ci=((FCf_CT_Vbi_ci*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ci*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ci))+((FCda1_dw_Vbi_ci*FCf1)+(FCda1_dw*FCf1_Vbi_ci)))-FCdf2_dw_Vbi_ci)+FCdf3_dw_Vbi_ci)));
#endif
FCdfc_dw=(FCf_CT*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw));
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
#if defined(_DERIVATE)
a_Vci_ei=(latb*FCw_Vci_ei);
a_Vbi_ei=(latb*FCw_Vbi_ei);
a_Vbi_ci=(latb*FCw_Vbi_ci);
a_Vtnode_GND=(latb*FCw_Vtnode_GND);
#endif
a=(latb*FCw);
if
((a>1.0e-6))
{
#if defined(_DERIVATE)
FCf2_Vci_ei=((a_Vci_ei-FClnb_Vci_ei)/latb);
FCf2_Vbi_ei=((a_Vbi_ei-FClnb_Vbi_ei)/latb);
FCf2_Vbi_ci=((a_Vbi_ci-FClnb_Vbi_ci)/latb);
FCf2_Vtnode_GND=((a_Vtnode_GND-FClnb_Vtnode_GND)/latb);
#endif
FCf2=((a-FClnb)/latb);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf2_dw=(a/(1.0+a));
}
else
{
#if defined(_DERIVATE)
FCf2_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf2_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf2_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf2_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf2=((0.5*a)*FCw);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=a_Vci_ei;
FCdf2_dw_Vtnode_GND=a_Vtnode_GND;
FCdf2_dw_Vbi_ei=a_Vbi_ei;
FCdf2_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf2_dw=a;
}
#if defined(_DERIVATE)
a_Vci_ei=(latl*FCw_Vci_ei);
a_Vbi_ei=(latl*FCw_Vbi_ei);
a_Vbi_ci=(latl*FCw_Vbi_ci);
a_Vtnode_GND=(latl*FCw_Vtnode_GND);
#endif
a=(latl*FCw);
if
((a>1.0e-6))
{
#if defined(_DERIVATE)
FCf3_Vci_ei=((a_Vci_ei-FClnl_Vci_ei)/latl);
FCf3_Vbi_ei=((a_Vbi_ei-FClnl_Vbi_ei)/latl);
FCf3_Vbi_ci=((a_Vbi_ci-FClnl_Vbi_ci)/latl);
FCf3_Vtnode_GND=((a_Vtnode_GND-FClnl_Vtnode_GND)/latl);
#endif
FCf3=((a-FClnl)/latl);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf3_dw=(a/(1.0+a));
}
else
{
#if defined(_DERIVATE)
FCf3_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf3_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf3_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf3_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf3=((0.5*a)*FCw);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=a_Vci_ei;
FCdf3_dw_Vtnode_GND=a_Vtnode_GND;
FCdf3_dw_Vbi_ei=a_Vbi_ei;
FCdf3_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf3_dw=a;
}
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((FCf_CT_Vci_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vci_ei-FCf3_Vci_ei)));
FCf_CT_Vbi_ei=((FCf_CT_Vbi_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ei-FCf3_Vbi_ei)));
FCf_CT_Vbi_ci=((FCf_CT_Vbi_ci*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ci-FCf3_Vbi_ci)));
FCf_CT_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCf2-FCf3))+(FCf_CT*(FCf2_Vtnode_GND-FCf3_Vtnode_GND)));
#endif
FCf_CT=(FCf_CT*(FCf2-FCf3));
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=((FCf_CT_Vci_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vci_ei-FCdf3_dw_Vci_ei)));
FCdfCT_dw_Vbi_ei=((FCf_CT_Vbi_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ei-FCdf3_dw_Vbi_ei)));
FCdfCT_dw_Vbi_ci=((FCf_CT_Vbi_ci*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ci-FCdf3_dw_Vbi_ci)));
FCdfCT_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vtnode_GND-FCdf3_dw_Vtnode_GND)));
#endif
FCdfCT_dw=(FCf_CT*(FCdf2_dw-FCdf3_dw));
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
}
else
{
if
((latb>0.01))
{
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-FCa_ck_Vci_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vtnode_GND=(-FCa_ck_Vtnode_GND*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ei=(-FCa_ck_Vbi_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ci=(-FCa_ck_Vbi_ci*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
#endif
FCd_a=(1.0/(1.0+(FCa_ck*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=(((-FCa_ck_Vci_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vci_ei));
FCw_Vtnode_GND=(((-FCa_ck_Vtnode_GND)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=(((-FCa_ck_Vbi_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ei));
FCw_Vbi_ci=(((-FCa_ck_Vbi_ci)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ci));
#endif
FCw=((1.0-FCa_ck)*FCd_a);
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-(1.0+latb))*FCd_a_Vci_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-(1.0+latb))*FCd_a_Vtnode_GND)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-(1.0+latb))*FCd_a_Vbi_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-(1.0+latb))*FCd_a_Vbi_ci)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-(1.0+latb))*FCd_a)*FCd_a);
}
else
{
#if defined(_DERIVATE)
FCw_Vci_ei=((-FCa_ck_Vci_ei)-FCa_ck_Vci_ei*latb);
FCw_Vtnode_GND=((-FCa_ck_Vtnode_GND)-FCa_ck_Vtnode_GND*latb);
FCw_Vbi_ei=((-FCa_ck_Vbi_ei)-FCa_ck_Vbi_ei*latb);
FCw_Vbi_ci=((-FCa_ck_Vbi_ci)-FCa_ck_Vbi_ci*latb);
#endif
FCw=((1.0-FCa_ck)-(FCa_ck*latb));
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=0.0;
FCdw_daick_Vtnode_GND=0.0;
FCdw_daick_Vbi_ei=0.0;
FCdw_daick_Vbi_ci=0.0;
#endif
FCdw_daick=(-(1.0+latb));
}
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCz_Vci_ei=(latb*FCw_Vci_ei);
FCz_Vtnode_GND=(latb*FCw_Vtnode_GND);
FCz_Vbi_ei=(latb*FCw_Vbi_ei);
FCz_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FCz=(latb*FCw);
#if defined(_DERIVATE)
FCz_1_Vci_ei=FCz_Vci_ei;
FCz_1_Vtnode_GND=FCz_Vtnode_GND;
FCz_1_Vbi_ei=FCz_Vbi_ei;
FCz_1_Vbi_ci=FCz_Vbi_ci;
#endif
FCz_1=(1.0+FCz);
#if defined(_DERIVATE)
FCd_f_Vci_ei=(-FCz_1_Vci_ei/FCz_1/FCz_1);
FCd_f_Vtnode_GND=(-FCz_1_Vtnode_GND/FCz_1/FCz_1);
FCd_f_Vbi_ei=(-FCz_1_Vbi_ei/FCz_1/FCz_1);
FCd_f_Vbi_ci=(-FCz_1_Vbi_ci/FCz_1/FCz_1);
#endif
FCd_f=(1.0/FCz_1);
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((((FCw2_Vci_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vci_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vci_ei));
FCf_ci_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vtnode_GND/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vtnode_GND));
FCf_ci_Vbi_ei=((((FCw2_Vbi_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_ci_Vbi_ci=((((FCw2_Vbi_ci*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ci/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ci));
#endif
FCf_ci=((FCw2*(1.0+(FCz/3.0)))*FCd_f);
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vci_ei+(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfc_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vtnode_GND+(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
FCdfc_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ei+(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfc_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ci+(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
#endif
FCdfc_dw=((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
if
((FCz>0.001))
{
{
double m00_logE(d00_logE0,FCz_1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FCz_1)
#endif
#if defined(_DERIVATE)
FCf_CT_Vci_ei=(((2.0*(((FCz_1_Vci_ei*d00_logE0)+(FCz_1*FCz_1_Vci_ei*d10_logE0))-FCz_Vci_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vci_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ei=(((2.0*(((FCz_1_Vbi_ei*d00_logE0)+(FCz_1*FCz_1_Vbi_ei*d10_logE0))-FCz_Vbi_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ci=(((2.0*(((FCz_1_Vbi_ci*d00_logE0)+(FCz_1*FCz_1_Vbi_ci*d10_logE0))-FCz_Vbi_ci))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ci))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vtnode_GND=(((2.0*(((FCz_1_Vtnode_GND*d00_logE0)+(FCz_1*FCz_1_Vtnode_GND*d10_logE0))-FCz_Vtnode_GND))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vtnode_GND))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
#endif
FCf_CT=((2.0*((FCz_1*d00_logE0)-FCz))/((latb*latb)*FCz_1));
}
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((2.0*FCw_Vci_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vci_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((2.0*FCw_Vbi_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((2.0*FCw_Vbi_ci)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ci))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((2.0*FCw_Vtnode_GND)*FCd_f)+((2.0*FCw)*FCd_f_Vtnode_GND))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=(((2.0*FCw)*FCd_f)*FCd_f);
}
else
{
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((((FCw2_Vci_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vci_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vci_ei));
FCf_CT_Vbi_ei=((((FCw2_Vbi_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_CT_Vbi_ci=((((FCw2_Vbi_ci*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ci/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ci));
FCf_CT_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vtnode_GND/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vtnode_GND));
#endif
FCf_CT=((FCw2*(1.0-(FCz/3.0)))*FCd_f);
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
}
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
}
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_CT)+(FFQ_fC*FCf_CT_Vtnode_GND));
FFQ_cT_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ei));
FFQ_cT_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ci));
FFQ_cT_Vci_ei=((FFQ_fC_Vci_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vci_ei));
#endif
FFQ_cT=(FFQ_fC*FCf_CT);
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_ci)+(FFQ_fC*FCf_ci_Vtnode_GND));
FFQ_fC_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ei));
FFQ_fC_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ci));
FFQ_fC_Vci_ei=((FFQ_fC_Vci_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vci_ei));
#endif
FFQ_fC=(FFQ_fC*FCf_ci);
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_ci+(itf*FCdfc_ditf)))+(FFT_pcS*(FCf_ci_Vtnode_GND+((itf_Vtnode_GND*FCdfc_ditf)+(itf*FCdfc_ditf_Vtnode_GND)))));
FFT_fC_Vci_ei=(FFT_pcS*(FCf_ci_Vci_ei+((itf_Vci_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vci_ei))));
FFT_fC_Vbi_ei=(FFT_pcS*(FCf_ci_Vbi_ei+((itf_Vbi_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ei))));
FFT_fC_Vbi_ci=(FFT_pcS*(FCf_ci_Vbi_ci+((itf_Vbi_ci*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ci))));
#endif
FFT_fC=(FFT_pcS*(FCf_ci+(itf*FCdfc_ditf)));
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_CT+(itf*FCdfCT_ditf)))+(FFT_pcS*(FCf_CT_Vtnode_GND+((itf_Vtnode_GND*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vtnode_GND)))));
FFT_cT_Vci_ei=(FFT_pcS*(FCf_CT_Vci_ei+((itf_Vci_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vci_ei))));
FFT_cT_Vbi_ei=(FFT_pcS*(FCf_CT_Vbi_ei+((itf_Vbi_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ei))));
FFT_cT_Vbi_ci=(FFT_pcS*(FCf_CT_Vbi_ci+((itf_Vbi_ci*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ci))));
#endif
FFT_cT=(FFT_pcS*(FCf_CT+(itf*FCdfCT_ditf)));
}
#if defined(_DERIVATE)
Qf_Vci_ei=(Qf_Vci_ei+FFd_QfB_Vci_ei);
Qf_Vtnode_GND=(Qf_Vtnode_GND+FFd_QfB_Vtnode_GND);
Qf_Vbi_ci=(Qf_Vbi_ci+FFd_QfB_Vbi_ci);
Qf_Vbi_ei=(Qf_Vbi_ei+FFd_QfB_Vbi_ei);
#endif
Qf=(Qf+FFd_QfB);
#if defined(_DERIVATE)
Tf_Vci_ei=(Tf_Vci_ei+FFd_TfB_Vci_ei);
Tf_Vtnode_GND=(Tf_Vtnode_GND+FFd_TfB_Vtnode_GND);
Tf_Vbi_ci=(Tf_Vbi_ci+FFd_TfB_Vbi_ci);
Tf_Vbi_ei=(Tf_Vbi_ei+FFd_TfB_Vbi_ei);
#endif
Tf=(Tf+FFd_TfB);
#if defined(_DERIVATE)
Q_fT_Vci_ei=((Qf_Vci_ei+(hfe*FFd_QfE_Vci_ei))+(hfc*FFQ_cT_Vci_ei));
Q_fT_Vtnode_GND=((Qf_Vtnode_GND+(hfe*FFd_QfE_Vtnode_GND))+(hfc*FFQ_cT_Vtnode_GND));
Q_fT_Vbi_ci=((Qf_Vbi_ci+(hfe*FFd_QfE_Vbi_ci))+(hfc*FFQ_cT_Vbi_ci));
Q_fT_Vbi_ei=((Qf_Vbi_ei+(hfe*FFd_QfE_Vbi_ei))+(hfc*FFQ_cT_Vbi_ei));
#endif
Q_fT=((Qf+(hfe*FFd_QfE))+(hfc*FFQ_cT));
#if defined(_DERIVATE)
T_fT_Vci_ei=((Tf_Vci_ei+(hfe*FFd_TfE_Vci_ei))+(hfc*FFT_cT_Vci_ei));
T_fT_Vtnode_GND=((Tf_Vtnode_GND+(hfe*FFd_TfE_Vtnode_GND))+(hfc*FFT_cT_Vtnode_GND));
T_fT_Vbi_ci=((Tf_Vbi_ci+(hfe*FFd_TfE_Vbi_ci))+(hfc*FFT_cT_Vbi_ci));
T_fT_Vbi_ei=((Tf_Vbi_ei+(hfe*FFd_TfE_Vbi_ei))+(hfc*FFT_cT_Vbi_ei));
#endif
T_fT=((Tf+(hfe*FFd_TfE))+(hfc*FFT_cT));
#if defined(_DERIVATE)
Qf_Vci_ei=((Qf_Vci_ei+FFd_QfE_Vci_ei)+FFQ_fC_Vci_ei);
Qf_Vtnode_GND=((Qf_Vtnode_GND+FFd_QfE_Vtnode_GND)+FFQ_fC_Vtnode_GND);
Qf_Vbi_ci=((Qf_Vbi_ci+FFd_QfE_Vbi_ci)+FFQ_fC_Vbi_ci);
Qf_Vbi_ei=((Qf_Vbi_ei+FFd_QfE_Vbi_ei)+FFQ_fC_Vbi_ei);
#endif
Qf=((Qf+FFd_QfE)+FFQ_fC);
#if defined(_DERIVATE)
Tf_Vci_ei=((Tf_Vci_ei+FFd_TfE_Vci_ei)+FFT_fC_Vci_ei);
Tf_Vtnode_GND=((Tf_Vtnode_GND+FFd_TfE_Vtnode_GND)+FFT_fC_Vtnode_GND);
Tf_Vbi_ci=((Tf_Vbi_ci+FFd_TfE_Vbi_ci)+FFT_fC_Vbi_ci);
Tf_Vbi_ei=((Tf_Vbi_ei+FFd_TfE_Vbi_ei)+FFT_fC_Vbi_ei);
#endif
Tf=((Tf+FFd_TfE)+FFT_fC);
}
#if defined(_DERIVATE)
Qr_Vci_ei=0.0;
Qr_Vtnode_GND=(Tr*itr_Vtnode_GND);
Qr_Vbi_ci=(Tr*itr_Vbi_ci);
Qr_Vbi_ei=(Tr*itr_Vbi_ei);
#endif
Qr=(Tr*itr);
l_it=0;
if
(((Qf>(1.0e-5*Q_p))||(a_h>1.0e-5)))
{
{
double m00_sqrt(d00_sqrt0,((T_f0*itf)*Q_fT))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_f0*itf)*Q_fT))
#endif
#if defined(_DERIVATE)
Qf_Vci_ei=(((T_f0*itf_Vci_ei)*Q_fT)+((T_f0*itf)*Q_fT_Vci_ei))*d10_sqrt0;
Qf_Vtnode_GND=((((T_f0_Vtnode_GND*itf)+(T_f0*itf_Vtnode_GND))*Q_fT)+((T_f0*itf)*Q_fT_Vtnode_GND))*d10_sqrt0;
Qf_Vbi_ci=((((T_f0_Vbi_ci*itf)+(T_f0*itf_Vbi_ci))*Q_fT)+((T_f0*itf)*Q_fT_Vbi_ci))*d10_sqrt0;
Qf_Vbi_ei=((((T_f0_Vbi_ei*itf)+(T_f0*itf_Vbi_ei))*Q_fT)+((T_f0*itf)*Q_fT_Vbi_ei))*d10_sqrt0;
#endif
Qf=d00_sqrt0;
}
#if defined(_DERIVATE)
Q_pT_Vtnode_GND=((Q_0_Vtnode_GND+Qf_Vtnode_GND)+Qr_Vtnode_GND);
Q_pT_Vbi_ei=((Q_0_Vbi_ei+Qf_Vbi_ei)+Qr_Vbi_ei);
Q_pT_Vbi_ci=((Q_0_Vbi_ci+Qf_Vbi_ci)+Qr_Vbi_ci);
Q_pT_Vci_ei=(Qf_Vci_ei+Qr_Vci_ei);
#endif
Q_pT=((Q_0+Qf)+Qr);
#if defined(_DERIVATE)
d_Q_Vtnode_GND=Q_pT_Vtnode_GND;
d_Q_Vbi_ei=Q_pT_Vbi_ei;
d_Q_Vbi_ci=Q_pT_Vbi_ci;
d_Q_Vci_ei=Q_pT_Vci_ei;
#endif
d_Q=Q_pT;
while
(((fabs(d_Q)>=(1.0e-5*fabs(Q_pT)))&&(l_it<=100)))
{
#if defined(_DYNAMIC)
d_Q0=d_Q;
#endif
#if defined(_DERIVATE)
I_Tf1_Vci_ei=(-i_0f*Q_pT_Vci_ei/Q_pT/Q_pT);
I_Tf1_Vtnode_GND=((i_0f_Vtnode_GND*Q_pT-i_0f*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
I_Tf1_Vbi_ei=((i_0f_Vbi_ei*Q_pT-i_0f*Q_pT_Vbi_ei)/Q_pT/Q_pT);
I_Tf1_Vbi_ci=(-i_0f*Q_pT_Vbi_ci/Q_pT/Q_pT);
#endif
I_Tf1=(i_0f/Q_pT);
#if defined(_DERIVATE)
a_h_Vci_ei=(Oich*I_Tf1_Vci_ei);
a_h_Vtnode_GND=(Oich*I_Tf1_Vtnode_GND);
a_h_Vbi_ei=(Oich*I_Tf1_Vbi_ei);
a_h_Vbi_ci=(Oich*I_Tf1_Vbi_ci);
#endif
a_h=(Oich*I_Tf1);
#if defined(_DERIVATE)
itf_Vci_ei=((I_Tf1_Vci_ei*(1.0+a_h))+(I_Tf1*a_h_Vci_ei));
itf_Vtnode_GND=((I_Tf1_Vtnode_GND*(1.0+a_h))+(I_Tf1*a_h_Vtnode_GND));
itf_Vbi_ei=((I_Tf1_Vbi_ei*(1.0+a_h))+(I_Tf1*a_h_Vbi_ei));
itf_Vbi_ci=((I_Tf1_Vbi_ci*(1.0+a_h))+(I_Tf1*a_h_Vbi_ci));
#endif
itf=(I_Tf1*(1.0+a_h));
#if defined(_DERIVATE)
itr_Vci_ei=(-i_0r*Q_pT_Vci_ei/Q_pT/Q_pT);
itr_Vtnode_GND=((i_0r_Vtnode_GND*Q_pT-i_0r*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
itr_Vbi_ci=((i_0r_Vbi_ci*Q_pT-i_0r*Q_pT_Vbi_ci)/Q_pT/Q_pT);
itr_Vbi_ei=(-i_0r*Q_pT_Vbi_ei/Q_pT/Q_pT);
#endif
itr=(i_0r/Q_pT);
#if defined(_DERIVATE)
Tf_Vci_ei=0.0;
Tf_Vtnode_GND=T_f0_Vtnode_GND;
Tf_Vbi_ci=T_f0_Vbi_ci;
Tf_Vbi_ei=T_f0_Vbi_ei;
#endif
Tf=T_f0;
#if defined(_DERIVATE)
Qf_Vci_ei=(T_f0*itf_Vci_ei);
Qf_Vtnode_GND=((T_f0_Vtnode_GND*itf)+(T_f0*itf_Vtnode_GND));
Qf_Vbi_ci=((T_f0_Vbi_ci*itf)+(T_f0*itf_Vbi_ci));
Qf_Vbi_ei=((T_f0_Vbi_ei*itf)+(T_f0*itf_Vbi_ei));
#endif
Qf=(T_f0*itf);
if
((itf<(1.0e-6*ick)))
{
#if defined(_DERIVATE)
Q_fT_Vci_ei=Qf_Vci_ei;
Q_fT_Vtnode_GND=Qf_Vtnode_GND;
Q_fT_Vbi_ci=Qf_Vbi_ci;
Q_fT_Vbi_ei=Qf_Vbi_ei;
#endif
Q_fT=Qf;
#if defined(_DERIVATE)
T_fT_Vci_ei=Tf_Vci_ei;
T_fT_Vtnode_GND=Tf_Vtnode_GND;
T_fT_Vbi_ci=Tf_Vbi_ci;
T_fT_Vbi_ei=Tf_Vbi_ei;
#endif
T_fT=Tf;
}
else
{
#if defined(_DERIVATE)
FFa_Vci_ei=((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf);
FFa_Vtnode_GND=((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf);
FFa_Vbi_ei=(-ick*itf_Vbi_ei/itf/itf);
FFa_Vbi_ci=(-ick*itf_Vbi_ci/itf/itf);
#endif
FFa=(ick/itf);
{
double m00_logE(d00_logE0,FFa)
double m00_exp(d00_exp1,((-gtfe)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FFa)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
FFd_TfE_Vtnode_GND=((tef0_t_Vtnode_GND*d00_exp1)+(tef0_t*((-gtfe)*FFa_Vtnode_GND*d10_logE0)*d10_exp1));
FFd_TfE_Vci_ei=(tef0_t*((-gtfe)*FFa_Vci_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ei=(tef0_t*((-gtfe)*FFa_Vbi_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ci=(tef0_t*((-gtfe)*FFa_Vbi_ci*d10_logE0)*d10_exp1);
#endif
FFd_TfE=(tef0_t*d00_exp1);
}
#if defined(_DERIVATE)
FFd_QfE_Vtnode_GND=(((FFd_TfE_Vtnode_GND*itf)+(FFd_TfE*itf_Vtnode_GND))/(gtfe+1.0));
FFd_QfE_Vci_ei=(((FFd_TfE_Vci_ei*itf)+(FFd_TfE*itf_Vci_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ei=(((FFd_TfE_Vbi_ei*itf)+(FFd_TfE*itf_Vbi_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ci=(((FFd_TfE_Vbi_ci*itf)+(FFd_TfE*itf_Vbi_ci))/(gtfe+1.0));
#endif
FFd_QfE=((FFd_TfE*itf)/(gtfe+1.0));
#if defined(_DERIVATE)
FFT_fbS_Vtnode_GND=((1.0-fthc)*thcs_t_Vtnode_GND);
#endif
FFT_fbS=((1.0-fthc)*thcs_t);
#if defined(_DERIVATE)
FFx_Vci_ei=(-FFa_Vci_ei);
FFx_Vtnode_GND=(-FFa_Vtnode_GND);
FFx_Vbi_ei=(-FFa_Vbi_ei);
FFx_Vbi_ci=(-FFa_Vbi_ci);
#endif
FFx=(1.0-FFa);
{
double m00_sqrt(d00_sqrt0,((FFx*FFx)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FFx*FFx)+alhc))
#endif
#if defined(_DERIVATE)
FFs_Vci_ei=((FFx_Vci_ei*FFx)+(FFx*FFx_Vci_ei))*d10_sqrt0;
FFs_Vtnode_GND=((FFx_Vtnode_GND*FFx)+(FFx*FFx_Vtnode_GND))*d10_sqrt0;
FFs_Vbi_ei=((FFx_Vbi_ei*FFx)+(FFx*FFx_Vbi_ei))*d10_sqrt0;
FFs_Vbi_ci=((FFx_Vbi_ci*FFx)+(FFx*FFx_Vbi_ci))*d10_sqrt0;
#endif
FFs=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FFw_Vci_ei=((FFx_Vci_ei+FFs_Vci_ei)/(1.0+d00_sqrt0));
FFw_Vtnode_GND=((FFx_Vtnode_GND+FFs_Vtnode_GND)/(1.0+d00_sqrt0));
FFw_Vbi_ei=((FFx_Vbi_ei+FFs_Vbi_ei)/(1.0+d00_sqrt0));
FFw_Vbi_ci=((FFx_Vbi_ci+FFs_Vbi_ci)/(1.0+d00_sqrt0));
#endif
FFw=((FFx+FFs)/(1.0+d00_sqrt0));
}
#if defined(_DERIVATE)
FFw_2_Vci_ei=((FFw_Vci_ei*FFw)+(FFw*FFw_Vci_ei));
FFw_2_Vtnode_GND=((FFw_Vtnode_GND*FFw)+(FFw*FFw_Vtnode_GND));
FFw_2_Vbi_ei=((FFw_Vbi_ei*FFw)+(FFw*FFw_Vbi_ei));
FFw_2_Vbi_ci=((FFw_Vbi_ci*FFw)+(FFw*FFw_Vbi_ci));
#endif
FFw_2=(FFw*FFw);
#if defined(_DERIVATE)
FFd_QfB_Vtnode_GND=((((FFT_fbS_Vtnode_GND*itf)+(FFT_fbS*itf_Vtnode_GND))*FFw_2)+((FFT_fbS*itf)*FFw_2_Vtnode_GND));
FFd_QfB_Vbi_ei=(((FFT_fbS*itf_Vbi_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ei));
FFd_QfB_Vbi_ci=(((FFT_fbS*itf_Vbi_ci)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ci));
FFd_QfB_Vci_ei=(((FFT_fbS*itf_Vci_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vci_ei));
#endif
FFd_QfB=((FFT_fbS*itf)*FFw_2);
#if defined(_DERIVATE)
FFa_w_Vci_ei=((FFw_2_Vci_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vci_ei)*FFs-(2.0*FFa)*FFs_Vci_ei)/FFs/FFs)));
FFa_w_Vtnode_GND=((FFw_2_Vtnode_GND*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vtnode_GND)*FFs-(2.0*FFa)*FFs_Vtnode_GND)/FFs/FFs)));
FFa_w_Vbi_ei=((FFw_2_Vbi_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ei)*FFs-(2.0*FFa)*FFs_Vbi_ei)/FFs/FFs)));
FFa_w_Vbi_ci=((FFw_2_Vbi_ci*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ci)*FFs-(2.0*FFa)*FFs_Vbi_ci)/FFs/FFs)));
#endif
FFa_w=(FFw_2*(1.0+((2.0*FFa)/FFs)));
#if defined(_DERIVATE)
FFd_TfB_Vtnode_GND=((FFT_fbS_Vtnode_GND*FFa_w)+(FFT_fbS*FFa_w_Vtnode_GND));
FFd_TfB_Vci_ei=(FFT_fbS*FFa_w_Vci_ei);
FFd_TfB_Vbi_ei=(FFT_fbS*FFa_w_Vbi_ei);
FFd_TfB_Vbi_ci=(FFT_fbS*FFa_w_Vbi_ci);
#endif
FFd_TfB=(FFT_fbS*FFa_w);
#if defined(_DERIVATE)
FFT_pcS_Vtnode_GND=(fthc*thcs_t_Vtnode_GND);
#endif
FFT_pcS=(fthc*thcs_t);
if
(((latb<=0.0)&&(latl<=0.0)))
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND))*FFw_2)+((FFT_pcS*itf)*FFw_2_Vtnode_GND));
FFQ_fC_Vbi_ei=(((FFT_pcS*itf_Vbi_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ei));
FFQ_fC_Vbi_ci=(((FFT_pcS*itf_Vbi_ci)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ci));
FFQ_fC_Vci_ei=(((FFT_pcS*itf_Vci_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vci_ei));
#endif
FFQ_fC=((FFT_pcS*itf)*FFw_2);
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*FFa_w)+(FFT_pcS*FFa_w_Vtnode_GND));
FFT_fC_Vci_ei=(FFT_pcS*FFa_w_Vci_ei);
FFT_fC_Vbi_ei=(FFT_pcS*FFa_w_Vbi_ei);
FFT_fC_Vbi_ci=(FFT_pcS*FFa_w_Vbi_ci);
#endif
FFT_fC=(FFT_pcS*FFa_w);
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=FFQ_fC_Vtnode_GND;
FFQ_cT_Vbi_ei=FFQ_fC_Vbi_ei;
FFQ_cT_Vbi_ci=FFQ_fC_Vbi_ci;
FFQ_cT_Vci_ei=FFQ_fC_Vci_ei;
#endif
FFQ_cT=FFQ_fC;
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=FFT_fC_Vtnode_GND;
FFT_cT_Vci_ei=FFT_fC_Vci_ei;
FFT_cT_Vbi_ei=FFT_fC_Vbi_ei;
FFT_cT_Vbi_ci=FFT_fC_Vbi_ci;
#endif
FFT_cT=FFT_fC;
}
else
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND));
FFQ_fC_Vbi_ei=(FFT_pcS*itf_Vbi_ei);
FFQ_fC_Vbi_ci=(FFT_pcS*itf_Vbi_ci);
FFQ_fC_Vci_ei=(FFT_pcS*itf_Vci_ei);
#endif
FFQ_fC=(FFT_pcS*itf);
#if defined(_DERIVATE)
FCa_Vci_ei=(-((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf));
FCa_Vtnode_GND=(-((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf));
FCa_Vbi_ei=(-(-ick*itf_Vbi_ei/itf/itf));
FCa_Vbi_ci=(-(-ick*itf_Vbi_ci/itf/itf));
#endif
FCa=(1.0-(ick/itf));
{
double m00_sqrt(d00_sqrt0,((FCa*FCa)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FCa*FCa)+alhc))
#endif
#if defined(_DERIVATE)
FCrt_Vci_ei=((FCa_Vci_ei*FCa)+(FCa*FCa_Vci_ei))*d10_sqrt0;
FCrt_Vtnode_GND=((FCa_Vtnode_GND*FCa)+(FCa*FCa_Vtnode_GND))*d10_sqrt0;
FCrt_Vbi_ei=((FCa_Vbi_ei*FCa)+(FCa*FCa_Vbi_ei))*d10_sqrt0;
FCrt_Vbi_ci=((FCa_Vbi_ci*FCa)+(FCa*FCa_Vbi_ci))*d10_sqrt0;
#endif
FCrt=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FCa_ck_Vci_ei=(-((FCa_Vci_ei+FCrt_Vci_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vtnode_GND=(-((FCa_Vtnode_GND+FCrt_Vtnode_GND)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ei=(-((FCa_Vbi_ei+FCrt_Vbi_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ci=(-((FCa_Vbi_ci+FCrt_Vbi_ci)/(1.0+d00_sqrt0)));
#endif
FCa_ck=(1.0-((FCa+FCrt)/(1.0+d00_sqrt0)));
}
#if defined(_DERIVATE)
FCdaick_ditf_Vci_ei=((((FCa_ck_Vci_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vci_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vci_ei*itf)+(FCrt*itf_Vci_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vtnode_GND=((((FCa_ck_Vtnode_GND*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vtnode_GND)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vtnode_GND*itf)+(FCrt*itf_Vtnode_GND)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ei=((((FCa_ck_Vbi_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ei*itf)+(FCrt*itf_Vbi_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ci=((((FCa_ck_Vbi_ci*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ci)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ci*itf)+(FCrt*itf_Vbi_ci)))/(FCrt*itf)/(FCrt*itf));
#endif
FCdaick_ditf=(((FCa_ck-1.0)*(1-FCa))/(FCrt*itf));
if
((latb>latl))
{
#if defined(_DERIVATE)
FCz_Vci_ei=0.0;
FCz_Vtnode_GND=0.0;
FCz_Vbi_ei=0.0;
FCz_Vbi_ci=0.0;
#endif
FCz=(latb-latl);
FCxl=(1.0+latl);
FCxb=(1.0+latb);
if
((latb>0.01))
{
{
double m00_logE(d00_logE0,(FCxb/FCxl))
FCln=d00_logE0;
}
{
double m00_exp(d00_exp0,((FCa_ck-1.0)*FCln))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
FCa1_Vci_ei=FCa_ck_Vci_ei*FCln*d10_exp0;
FCa1_Vtnode_GND=FCa_ck_Vtnode_GND*FCln*d10_exp0;
FCa1_Vbi_ei=FCa_ck_Vbi_ei*FCln*d10_exp0;
FCa1_Vbi_ci=FCa_ck_Vbi_ci*FCln*d10_exp0;
#endif
FCa1=d00_exp0;
}
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCa1_Vci_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vtnode_GND=(-(-FCa1_Vtnode_GND*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ei=(-(-FCa1_Vbi_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ci=(-(-FCa1_Vbi_ci*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
#endif
FCd_a=(1.0/(latl-(FCa1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCa1_Vci_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCa1_Vtnode_GND*FCd_a)+((FCa1-1.0)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCa1_Vbi_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCa1_Vbi_ci*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ci));
#endif
FCw=((FCa1-1.0)*FCd_a);
{
double m00_logE(d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#endif
#if defined(_DERIVATE)
FCa_Vci_ei=(((latb*FCw_Vci_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vci_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vtnode_GND=(((latb*FCw_Vtnode_GND)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vtnode_GND))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ei=(((latb*FCw_Vbi_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ci=(((latb*FCw_Vbi_ci)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ci))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
#endif
FCa=d00_logE0;
}
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=(((((((-FCz_Vci_ei)*FCa1)+((-FCz)*FCa1_Vci_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vci_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=(((((((-FCz_Vtnode_GND)*FCa1)+((-FCz)*FCa1_Vtnode_GND))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vtnode_GND))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=(((((((-FCz_Vbi_ei)*FCa1)+((-FCz)*FCa1_Vbi_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=(((((((-FCz_Vbi_ci)*FCa1)+((-FCz)*FCa1_Vbi_ci))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ci))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a);
{
double m00_logE(d00_logE0,(1.0+(latb*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latb*FCw)))
#endif
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei)*d10_logE0;
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND)*d10_logE0;
FClnb_Vbi_ei=(latb*FCw_Vbi_ei)*d10_logE0;
FClnb_Vbi_ci=(latb*FCw_Vbi_ci)*d10_logE0;
#endif
FClnb=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0+(latl*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latl*FCw)))
#endif
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei)*d10_logE0;
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND)*d10_logE0;
FClnl_Vbi_ei=(latl*FCw_Vbi_ei)*d10_logE0;
FClnl_Vbi_ci=(latl*FCw_Vbi_ci)*d10_logE0;
#endif
FClnl=d00_logE0;
}
#if defined(_DERIVATE)
FCa1_Vci_ei=(FClnb_Vci_ei-FClnl_Vci_ei);
FCa1_Vtnode_GND=(FClnb_Vtnode_GND-FClnl_Vtnode_GND);
FCa1_Vbi_ei=(FClnb_Vbi_ei-FClnl_Vbi_ei);
FCa1_Vbi_ci=(FClnb_Vbi_ci-FClnl_Vbi_ci);
#endif
FCa1=(FClnb-FClnl);
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=((-latb*(latb*FCw_Vci_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vci_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vtnode_GND=((-latb*(latb*FCw_Vtnode_GND)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vtnode_GND)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ei=((-latb*(latb*FCw_Vbi_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ci=((-latb*(latb*FCw_Vbi_ci)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ci)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
#endif
FCda1_dw=((latb/(1.0+(latb*FCw)))-(latl/(1.0+(latl*FCw))));
}
else
{
#if defined(_DERIVATE)
FCf1_Vci_ei=(-FCa_ck_Vci_ei);
FCf1_Vtnode_GND=(-FCa_ck_Vtnode_GND);
FCf1_Vbi_ei=(-FCa_ck_Vbi_ei);
FCf1_Vbi_ci=(-FCa_ck_Vbi_ci);
#endif
FCf1=(1.0-FCa_ck);
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCf1_Vci_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vtnode_GND=(-(-FCf1_Vtnode_GND*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ei=(-(-FCf1_Vbi_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ci=(-(-FCf1_Vbi_ci*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
#endif
FCd_a=(1.0/(1.0-(FCf1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCf1_Vci_ei*FCd_a)+(FCf1*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCf1_Vtnode_GND*FCd_a)+(FCf1*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCf1_Vbi_ei*FCd_a)+(FCf1*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCf1_Vbi_ci*FCd_a)+(FCf1*FCd_a_Vbi_ci));
#endif
FCw=(FCf1*FCd_a);
#if defined(_DERIVATE)
FCa_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa=(FCz*FCw);
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-1.0)*FCd_a_Vci_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-1.0)*FCd_a_Vtnode_GND)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-1.0)*FCd_a_Vbi_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-1.0)*FCd_a_Vbi_ci)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-1.0)*FCd_a)*FCd_a);
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei);
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND);
FClnb_Vbi_ei=(latb*FCw_Vbi_ei);
FClnb_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FClnb=(latb*FCw);
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei);
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND);
FClnl_Vbi_ei=(latl*FCw_Vbi_ei);
FClnl_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
FClnl=(latl*FCw);
#if defined(_DERIVATE)
FCa1_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa1_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa1_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa1_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa1=(FCz*FCw);
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=FCz_Vci_ei;
FCda1_dw_Vtnode_GND=FCz_Vtnode_GND;
FCda1_dw_Vbi_ei=FCz_Vbi_ei;
FCda1_dw_Vbi_ci=FCz_Vbi_ci;
#endif
FCda1_dw=FCz;
}
#if defined(_DERIVATE)
FCf_CT_Vci_ei=(-2.0*FCz_Vci_ei/FCz/FCz);
FCf_CT_Vbi_ei=(-2.0*FCz_Vbi_ei/FCz/FCz);
FCf_CT_Vbi_ci=(-2.0*FCz_Vbi_ci/FCz/FCz);
FCf_CT_Vtnode_GND=(-2.0*FCz_Vtnode_GND/FCz/FCz);
#endif
FCf_CT=(2.0/FCz);
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCf1_Vci_ei=(((((((latb*latl)*FCw_Vci_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vci_ei))/3.0)+(((latb+latl)*FCw2_Vci_ei)/2.0))+FCw_Vci_ei);
FCf1_Vtnode_GND=(((((((latb*latl)*FCw_Vtnode_GND)*FCw2)+(((latb*latl)*FCw)*FCw2_Vtnode_GND))/3.0)+(((latb+latl)*FCw2_Vtnode_GND)/2.0))+FCw_Vtnode_GND);
FCf1_Vbi_ei=(((((((latb*latl)*FCw_Vbi_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ei))/3.0)+(((latb+latl)*FCw2_Vbi_ei)/2.0))+FCw_Vbi_ei);
FCf1_Vbi_ci=(((((((latb*latl)*FCw_Vbi_ci)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ci))/3.0)+(((latb+latl)*FCw2_Vbi_ci)/2.0))+FCw_Vbi_ci);
#endif
FCf1=((((((latb*latl)*FCw)*FCw2)/3.0)+(((latb+latl)*FCw2)/2.0))+FCw);
#if defined(_DERIVATE)
FCdf1_dw_Vci_ei=(((latb*latl)*FCw2_Vci_ei)+((latb+latl)*FCw_Vci_ei));
FCdf1_dw_Vtnode_GND=(((latb*latl)*FCw2_Vtnode_GND)+((latb+latl)*FCw_Vtnode_GND));
FCdf1_dw_Vbi_ei=(((latb*latl)*FCw2_Vbi_ei)+((latb+latl)*FCw_Vbi_ei));
FCdf1_dw_Vbi_ci=(((latb*latl)*FCw2_Vbi_ci)+((latb+latl)*FCw_Vbi_ci));
#endif
FCdf1_dw=((((latb*latl)*FCw2)+((latb+latl)*FCw))+1.0);
#if defined(_DERIVATE)
z_Vci_ei=(latb*FCw_Vci_ei);
z_Vtnode_GND=(latb*FCw_Vtnode_GND);
z_Vbi_ei=(latb*FCw_Vbi_ei);
z_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
z=(latb*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnb)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnb)-1.0))+1.0)/9.0);
r=(latl/latb);
#if defined(_DERIVATE)
FCf2_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latb);
FCf2_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latb);
FCf2_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latb);
FCf2_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latb);
#endif
FCf2=((((1.0-r)*a2)+(r*a3))/latb);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vci_ei));
FCdf2_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vtnode_GND));
FCdf2_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ei));
FCdf2_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ci));
#endif
FCdf2_dw=((((1.0-r)*x)+(r*a))*FClnb);
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf2_Vci_ei=(((((((latb*a2_Vci_ei)+(latl*a3_Vci_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vci_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf2_Vbi_ei=(((((((latb*a2_Vbi_ei)+(latl*a3_Vbi_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf2_Vbi_ci=(((((((latb*a2_Vbi_ci)+(latl*a3_Vbi_ci))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ci))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf2_Vtnode_GND=(((((((latb*a2_Vtnode_GND)+(latl*a3_Vtnode_GND))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vtnode_GND))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf2=(((((latb*a2)+(latl*a3))*FCw)*FCw)/6.0);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latl*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf2_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latl*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf2_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latl*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf2_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latl*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf2_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latl*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
}
#if defined(_DERIVATE)
z_Vci_ei=(latl*FCw_Vci_ei);
z_Vtnode_GND=(latl*FCw_Vtnode_GND);
z_Vbi_ei=(latl*FCw_Vbi_ei);
z_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
z=(latl*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnl)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnl)-1.0))+1.0)/9.0);
r=(latb/latl);
#if defined(_DERIVATE)
FCf3_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latl);
FCf3_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latl);
FCf3_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latl);
FCf3_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latl);
#endif
FCf3=((((1.0-r)*a2)+(r*a3))/latl);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vci_ei));
FCdf3_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vtnode_GND));
FCdf3_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ei));
FCdf3_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ci));
#endif
FCdf3_dw=((((1.0-r)*x)+(r*a))*FClnl);
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf3_Vci_ei=(((((((latl*a2_Vci_ei)+(latb*a3_Vci_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vci_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf3_Vbi_ei=(((((((latl*a2_Vbi_ei)+(latb*a3_Vbi_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf3_Vbi_ci=(((((((latl*a2_Vbi_ci)+(latb*a3_Vbi_ci))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ci))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf3_Vtnode_GND=(((((((latl*a2_Vtnode_GND)+(latb*a3_Vtnode_GND))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vtnode_GND))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf3=(((((latl*a2)+(latb*a3))*FCw)*FCw)/6.0);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latb*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf3_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latb*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf3_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latb*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf3_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latb*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf3_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latb*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
}
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((FCf_CT_Vci_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vci_ei*FCf1)+(FCa*FCf1_Vci_ei))-FCf2_Vci_ei)+FCf3_Vci_ei)));
FCf_ci_Vtnode_GND=((FCf_CT_Vtnode_GND*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vtnode_GND*FCf1)+(FCa*FCf1_Vtnode_GND))-FCf2_Vtnode_GND)+FCf3_Vtnode_GND)));
FCf_ci_Vbi_ei=((FCf_CT_Vbi_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ei*FCf1)+(FCa*FCf1_Vbi_ei))-FCf2_Vbi_ei)+FCf3_Vbi_ei)));
FCf_ci_Vbi_ci=((FCf_CT_Vbi_ci*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ci*FCf1)+(FCa*FCf1_Vbi_ci))-FCf2_Vbi_ci)+FCf3_Vbi_ci)));
#endif
FCf_ci=(FCf_CT*(((FCa*FCf1)-FCf2)+FCf3));
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=((FCf_CT_Vci_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vci_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vci_ei))+((FCda1_dw_Vci_ei*FCf1)+(FCda1_dw*FCf1_Vci_ei)))-FCdf2_dw_Vci_ei)+FCdf3_dw_Vci_ei)));
FCdfc_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vtnode_GND*FCdf1_dw)+(FCa1*FCdf1_dw_Vtnode_GND))+((FCda1_dw_Vtnode_GND*FCf1)+(FCda1_dw*FCf1_Vtnode_GND)))-FCdf2_dw_Vtnode_GND)+FCdf3_dw_Vtnode_GND)));
FCdfc_dw_Vbi_ei=((FCf_CT_Vbi_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ei))+((FCda1_dw_Vbi_ei*FCf1)+(FCda1_dw*FCf1_Vbi_ei)))-FCdf2_dw_Vbi_ei)+FCdf3_dw_Vbi_ei)));
FCdfc_dw_Vbi_ci=((FCf_CT_Vbi_ci*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ci*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ci))+((FCda1_dw_Vbi_ci*FCf1)+(FCda1_dw*FCf1_Vbi_ci)))-FCdf2_dw_Vbi_ci)+FCdf3_dw_Vbi_ci)));
#endif
FCdfc_dw=(FCf_CT*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw));
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
#if defined(_DERIVATE)
a_Vci_ei=(latb*FCw_Vci_ei);
a_Vbi_ei=(latb*FCw_Vbi_ei);
a_Vbi_ci=(latb*FCw_Vbi_ci);
a_Vtnode_GND=(latb*FCw_Vtnode_GND);
#endif
a=(latb*FCw);
if
((a>1.0e-6))
{
#if defined(_DERIVATE)
FCf2_Vci_ei=((a_Vci_ei-FClnb_Vci_ei)/latb);
FCf2_Vbi_ei=((a_Vbi_ei-FClnb_Vbi_ei)/latb);
FCf2_Vbi_ci=((a_Vbi_ci-FClnb_Vbi_ci)/latb);
FCf2_Vtnode_GND=((a_Vtnode_GND-FClnb_Vtnode_GND)/latb);
#endif
FCf2=((a-FClnb)/latb);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf2_dw=(a/(1.0+a));
}
else
{
#if defined(_DERIVATE)
FCf2_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf2_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf2_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf2_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf2=((0.5*a)*FCw);
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=a_Vci_ei;
FCdf2_dw_Vtnode_GND=a_Vtnode_GND;
FCdf2_dw_Vbi_ei=a_Vbi_ei;
FCdf2_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf2_dw=a;
}
#if defined(_DERIVATE)
a_Vci_ei=(latl*FCw_Vci_ei);
a_Vbi_ei=(latl*FCw_Vbi_ei);
a_Vbi_ci=(latl*FCw_Vbi_ci);
a_Vtnode_GND=(latl*FCw_Vtnode_GND);
#endif
a=(latl*FCw);
if
((a>1.0e-6))
{
#if defined(_DERIVATE)
FCf3_Vci_ei=((a_Vci_ei-FClnl_Vci_ei)/latl);
FCf3_Vbi_ei=((a_Vbi_ei-FClnl_Vbi_ei)/latl);
FCf3_Vbi_ci=((a_Vbi_ci-FClnl_Vbi_ci)/latl);
FCf3_Vtnode_GND=((a_Vtnode_GND-FClnl_Vtnode_GND)/latl);
#endif
FCf3=((a-FClnl)/latl);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf3_dw=(a/(1.0+a));
}
else
{
#if defined(_DERIVATE)
FCf3_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf3_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf3_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf3_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf3=((0.5*a)*FCw);
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=a_Vci_ei;
FCdf3_dw_Vtnode_GND=a_Vtnode_GND;
FCdf3_dw_Vbi_ei=a_Vbi_ei;
FCdf3_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf3_dw=a;
}
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((FCf_CT_Vci_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vci_ei-FCf3_Vci_ei)));
FCf_CT_Vbi_ei=((FCf_CT_Vbi_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ei-FCf3_Vbi_ei)));
FCf_CT_Vbi_ci=((FCf_CT_Vbi_ci*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ci-FCf3_Vbi_ci)));
FCf_CT_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCf2-FCf3))+(FCf_CT*(FCf2_Vtnode_GND-FCf3_Vtnode_GND)));
#endif
FCf_CT=(FCf_CT*(FCf2-FCf3));
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=((FCf_CT_Vci_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vci_ei-FCdf3_dw_Vci_ei)));
FCdfCT_dw_Vbi_ei=((FCf_CT_Vbi_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ei-FCdf3_dw_Vbi_ei)));
FCdfCT_dw_Vbi_ci=((FCf_CT_Vbi_ci*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ci-FCdf3_dw_Vbi_ci)));
FCdfCT_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vtnode_GND-FCdf3_dw_Vtnode_GND)));
#endif
FCdfCT_dw=(FCf_CT*(FCdf2_dw-FCdf3_dw));
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
}
else
{
if
((latb>0.01))
{
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-FCa_ck_Vci_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vtnode_GND=(-FCa_ck_Vtnode_GND*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ei=(-FCa_ck_Vbi_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ci=(-FCa_ck_Vbi_ci*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
#endif
FCd_a=(1.0/(1.0+(FCa_ck*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=(((-FCa_ck_Vci_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vci_ei));
FCw_Vtnode_GND=(((-FCa_ck_Vtnode_GND)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=(((-FCa_ck_Vbi_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ei));
FCw_Vbi_ci=(((-FCa_ck_Vbi_ci)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ci));
#endif
FCw=((1.0-FCa_ck)*FCd_a);
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-(1.0+latb))*FCd_a_Vci_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-(1.0+latb))*FCd_a_Vtnode_GND)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-(1.0+latb))*FCd_a_Vbi_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-(1.0+latb))*FCd_a_Vbi_ci)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-(1.0+latb))*FCd_a)*FCd_a);
}
else
{
#if defined(_DERIVATE)
FCw_Vci_ei=((-FCa_ck_Vci_ei)-FCa_ck_Vci_ei*latb);
FCw_Vtnode_GND=((-FCa_ck_Vtnode_GND)-FCa_ck_Vtnode_GND*latb);
FCw_Vbi_ei=((-FCa_ck_Vbi_ei)-FCa_ck_Vbi_ei*latb);
FCw_Vbi_ci=((-FCa_ck_Vbi_ci)-FCa_ck_Vbi_ci*latb);
#endif
FCw=((1.0-FCa_ck)-(FCa_ck*latb));
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=0.0;
FCdw_daick_Vtnode_GND=0.0;
FCdw_daick_Vbi_ei=0.0;
FCdw_daick_Vbi_ci=0.0;
#endif
FCdw_daick=(-(1.0+latb));
}
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCz_Vci_ei=(latb*FCw_Vci_ei);
FCz_Vtnode_GND=(latb*FCw_Vtnode_GND);
FCz_Vbi_ei=(latb*FCw_Vbi_ei);
FCz_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FCz=(latb*FCw);
#if defined(_DERIVATE)
FCz_1_Vci_ei=FCz_Vci_ei;
FCz_1_Vtnode_GND=FCz_Vtnode_GND;
FCz_1_Vbi_ei=FCz_Vbi_ei;
FCz_1_Vbi_ci=FCz_Vbi_ci;
#endif
FCz_1=(1.0+FCz);
#if defined(_DERIVATE)
FCd_f_Vci_ei=(-FCz_1_Vci_ei/FCz_1/FCz_1);
FCd_f_Vtnode_GND=(-FCz_1_Vtnode_GND/FCz_1/FCz_1);
FCd_f_Vbi_ei=(-FCz_1_Vbi_ei/FCz_1/FCz_1);
FCd_f_Vbi_ci=(-FCz_1_Vbi_ci/FCz_1/FCz_1);
#endif
FCd_f=(1.0/FCz_1);
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((((FCw2_Vci_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vci_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vci_ei));
FCf_ci_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vtnode_GND/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vtnode_GND));
FCf_ci_Vbi_ei=((((FCw2_Vbi_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_ci_Vbi_ci=((((FCw2_Vbi_ci*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ci/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ci));
#endif
FCf_ci=((FCw2*(1.0+(FCz/3.0)))*FCd_f);
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vci_ei+(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfc_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vtnode_GND+(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
FCdfc_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ei+(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfc_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ci+(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
#endif
FCdfc_dw=((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
if
((FCz>0.001))
{
{
double m00_logE(d00_logE0,FCz_1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FCz_1)
#endif
#if defined(_DERIVATE)
FCf_CT_Vci_ei=(((2.0*(((FCz_1_Vci_ei*d00_logE0)+(FCz_1*FCz_1_Vci_ei*d10_logE0))-FCz_Vci_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vci_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ei=(((2.0*(((FCz_1_Vbi_ei*d00_logE0)+(FCz_1*FCz_1_Vbi_ei*d10_logE0))-FCz_Vbi_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ci=(((2.0*(((FCz_1_Vbi_ci*d00_logE0)+(FCz_1*FCz_1_Vbi_ci*d10_logE0))-FCz_Vbi_ci))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ci))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vtnode_GND=(((2.0*(((FCz_1_Vtnode_GND*d00_logE0)+(FCz_1*FCz_1_Vtnode_GND*d10_logE0))-FCz_Vtnode_GND))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vtnode_GND))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
#endif
FCf_CT=((2.0*((FCz_1*d00_logE0)-FCz))/((latb*latb)*FCz_1));
}
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((2.0*FCw_Vci_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vci_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((2.0*FCw_Vbi_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((2.0*FCw_Vbi_ci)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ci))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((2.0*FCw_Vtnode_GND)*FCd_f)+((2.0*FCw)*FCd_f_Vtnode_GND))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=(((2.0*FCw)*FCd_f)*FCd_f);
}
else
{
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((((FCw2_Vci_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vci_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vci_ei));
FCf_CT_Vbi_ei=((((FCw2_Vbi_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_CT_Vbi_ci=((((FCw2_Vbi_ci*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ci/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ci));
FCf_CT_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vtnode_GND/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vtnode_GND));
#endif
FCf_CT=((FCw2*(1.0-(FCz/3.0)))*FCd_f);
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
}
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
}
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_CT)+(FFQ_fC*FCf_CT_Vtnode_GND));
FFQ_cT_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ei));
FFQ_cT_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ci));
FFQ_cT_Vci_ei=((FFQ_fC_Vci_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vci_ei));
#endif
FFQ_cT=(FFQ_fC*FCf_CT);
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_ci)+(FFQ_fC*FCf_ci_Vtnode_GND));
FFQ_fC_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ei));
FFQ_fC_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ci));
FFQ_fC_Vci_ei=((FFQ_fC_Vci_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vci_ei));
#endif
FFQ_fC=(FFQ_fC*FCf_ci);
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_ci+(itf*FCdfc_ditf)))+(FFT_pcS*(FCf_ci_Vtnode_GND+((itf_Vtnode_GND*FCdfc_ditf)+(itf*FCdfc_ditf_Vtnode_GND)))));
FFT_fC_Vci_ei=(FFT_pcS*(FCf_ci_Vci_ei+((itf_Vci_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vci_ei))));
FFT_fC_Vbi_ei=(FFT_pcS*(FCf_ci_Vbi_ei+((itf_Vbi_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ei))));
FFT_fC_Vbi_ci=(FFT_pcS*(FCf_ci_Vbi_ci+((itf_Vbi_ci*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ci))));
#endif
FFT_fC=(FFT_pcS*(FCf_ci+(itf*FCdfc_ditf)));
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_CT+(itf*FCdfCT_ditf)))+(FFT_pcS*(FCf_CT_Vtnode_GND+((itf_Vtnode_GND*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vtnode_GND)))));
FFT_cT_Vci_ei=(FFT_pcS*(FCf_CT_Vci_ei+((itf_Vci_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vci_ei))));
FFT_cT_Vbi_ei=(FFT_pcS*(FCf_CT_Vbi_ei+((itf_Vbi_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ei))));
FFT_cT_Vbi_ci=(FFT_pcS*(FCf_CT_Vbi_ci+((itf_Vbi_ci*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ci))));
#endif
FFT_cT=(FFT_pcS*(FCf_CT+(itf*FCdfCT_ditf)));
}
#if defined(_DERIVATE)
Qf_Vci_ei=(Qf_Vci_ei+FFd_QfB_Vci_ei);
Qf_Vtnode_GND=(Qf_Vtnode_GND+FFd_QfB_Vtnode_GND);
Qf_Vbi_ci=(Qf_Vbi_ci+FFd_QfB_Vbi_ci);
Qf_Vbi_ei=(Qf_Vbi_ei+FFd_QfB_Vbi_ei);
#endif
Qf=(Qf+FFd_QfB);
#if defined(_DERIVATE)
Tf_Vci_ei=(Tf_Vci_ei+FFd_TfB_Vci_ei);
Tf_Vtnode_GND=(Tf_Vtnode_GND+FFd_TfB_Vtnode_GND);
Tf_Vbi_ci=(Tf_Vbi_ci+FFd_TfB_Vbi_ci);
Tf_Vbi_ei=(Tf_Vbi_ei+FFd_TfB_Vbi_ei);
#endif
Tf=(Tf+FFd_TfB);
#if defined(_DERIVATE)
Q_fT_Vci_ei=((Qf_Vci_ei+(hfe*FFd_QfE_Vci_ei))+(hfc*FFQ_cT_Vci_ei));
Q_fT_Vtnode_GND=((Qf_Vtnode_GND+(hfe*FFd_QfE_Vtnode_GND))+(hfc*FFQ_cT_Vtnode_GND));
Q_fT_Vbi_ci=((Qf_Vbi_ci+(hfe*FFd_QfE_Vbi_ci))+(hfc*FFQ_cT_Vbi_ci));
Q_fT_Vbi_ei=((Qf_Vbi_ei+(hfe*FFd_QfE_Vbi_ei))+(hfc*FFQ_cT_Vbi_ei));
#endif
Q_fT=((Qf+(hfe*FFd_QfE))+(hfc*FFQ_cT));
#if defined(_DERIVATE)
T_fT_Vci_ei=((Tf_Vci_ei+(hfe*FFd_TfE_Vci_ei))+(hfc*FFT_cT_Vci_ei));
T_fT_Vtnode_GND=((Tf_Vtnode_GND+(hfe*FFd_TfE_Vtnode_GND))+(hfc*FFT_cT_Vtnode_GND));
T_fT_Vbi_ci=((Tf_Vbi_ci+(hfe*FFd_TfE_Vbi_ci))+(hfc*FFT_cT_Vbi_ci));
T_fT_Vbi_ei=((Tf_Vbi_ei+(hfe*FFd_TfE_Vbi_ei))+(hfc*FFT_cT_Vbi_ei));
#endif
T_fT=((Tf+(hfe*FFd_TfE))+(hfc*FFT_cT));
#if defined(_DERIVATE)
Qf_Vci_ei=((Qf_Vci_ei+FFd_QfE_Vci_ei)+FFQ_fC_Vci_ei);
Qf_Vtnode_GND=((Qf_Vtnode_GND+FFd_QfE_Vtnode_GND)+FFQ_fC_Vtnode_GND);
Qf_Vbi_ci=((Qf_Vbi_ci+FFd_QfE_Vbi_ci)+FFQ_fC_Vbi_ci);
Qf_Vbi_ei=((Qf_Vbi_ei+FFd_QfE_Vbi_ei)+FFQ_fC_Vbi_ei);
#endif
Qf=((Qf+FFd_QfE)+FFQ_fC);
#if defined(_DERIVATE)
Tf_Vci_ei=((Tf_Vci_ei+FFd_TfE_Vci_ei)+FFT_fC_Vci_ei);
Tf_Vtnode_GND=((Tf_Vtnode_GND+FFd_TfE_Vtnode_GND)+FFT_fC_Vtnode_GND);
Tf_Vbi_ci=((Tf_Vbi_ci+FFd_TfE_Vbi_ci)+FFT_fC_Vbi_ci);
Tf_Vbi_ei=((Tf_Vbi_ei+FFd_TfE_Vbi_ei)+FFT_fC_Vbi_ei);
#endif
Tf=((Tf+FFd_TfE)+FFT_fC);
}
#if defined(_DERIVATE)
Qr_Vci_ei=(Tr*itr_Vci_ei);
Qr_Vtnode_GND=(Tr*itr_Vtnode_GND);
Qr_Vbi_ci=(Tr*itr_Vbi_ci);
Qr_Vbi_ei=(Tr*itr_Vbi_ei);
#endif
Qr=(Tr*itr);
if
((Oich==0.0))
{
#if defined(_DERIVATE)
a_Vci_ei=(((((T_fT_Vci_ei*itf)+(T_fT*itf_Vci_ei))+Qr_Vci_ei)*Q_pT-((T_fT*itf)+Qr)*Q_pT_Vci_ei)/Q_pT/Q_pT);
a_Vbi_ei=(((((T_fT_Vbi_ei*itf)+(T_fT*itf_Vbi_ei))+Qr_Vbi_ei)*Q_pT-((T_fT*itf)+Qr)*Q_pT_Vbi_ei)/Q_pT/Q_pT);
a_Vbi_ci=(((((T_fT_Vbi_ci*itf)+(T_fT*itf_Vbi_ci))+Qr_Vbi_ci)*Q_pT-((T_fT*itf)+Qr)*Q_pT_Vbi_ci)/Q_pT/Q_pT);
a_Vtnode_GND=(((((T_fT_Vtnode_GND*itf)+(T_fT*itf_Vtnode_GND))+Qr_Vtnode_GND)*Q_pT-((T_fT*itf)+Qr)*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
#endif
a=(1.0+(((T_fT*itf)+Qr)/Q_pT));
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=(((((((T_fT_Vci_ei*I_Tf1)+(T_fT*I_Tf1_Vci_ei))*(1.0+(2.0*a_h)))+((T_fT*I_Tf1)*(2.0*a_h_Vci_ei)))+Qr_Vci_ei)*Q_pT-(((T_fT*I_Tf1)*(1.0+(2.0*a_h)))+Qr)*Q_pT_Vci_ei)/Q_pT/Q_pT);
a_Vbi_ei=(((((((T_fT_Vbi_ei*I_Tf1)+(T_fT*I_Tf1_Vbi_ei))*(1.0+(2.0*a_h)))+((T_fT*I_Tf1)*(2.0*a_h_Vbi_ei)))+Qr_Vbi_ei)*Q_pT-(((T_fT*I_Tf1)*(1.0+(2.0*a_h)))+Qr)*Q_pT_Vbi_ei)/Q_pT/Q_pT);
a_Vbi_ci=(((((((T_fT_Vbi_ci*I_Tf1)+(T_fT*I_Tf1_Vbi_ci))*(1.0+(2.0*a_h)))+((T_fT*I_Tf1)*(2.0*a_h_Vbi_ci)))+Qr_Vbi_ci)*Q_pT-(((T_fT*I_Tf1)*(1.0+(2.0*a_h)))+Qr)*Q_pT_Vbi_ci)/Q_pT/Q_pT);
a_Vtnode_GND=(((((((T_fT_Vtnode_GND*I_Tf1)+(T_fT*I_Tf1_Vtnode_GND))*(1.0+(2.0*a_h)))+((T_fT*I_Tf1)*(2.0*a_h_Vtnode_GND)))+Qr_Vtnode_GND)*Q_pT-(((T_fT*I_Tf1)*(1.0+(2.0*a_h)))+Qr)*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
#endif
a=(1.0+((((T_fT*I_Tf1)*(1.0+(2.0*a_h)))+Qr)/Q_pT));
}
#if defined(_DERIVATE)
d_Q_Vtnode_GND=(((-(Q_pT_Vtnode_GND-((Q_0_Vtnode_GND+Q_fT_Vtnode_GND)+Qr_Vtnode_GND)))*a-(-(Q_pT-((Q_0+Q_fT)+Qr)))*a_Vtnode_GND)/a/a);
d_Q_Vbi_ei=(((-(Q_pT_Vbi_ei-((Q_0_Vbi_ei+Q_fT_Vbi_ei)+Qr_Vbi_ei)))*a-(-(Q_pT-((Q_0+Q_fT)+Qr)))*a_Vbi_ei)/a/a);
d_Q_Vbi_ci=(((-(Q_pT_Vbi_ci-((Q_0_Vbi_ci+Q_fT_Vbi_ci)+Qr_Vbi_ci)))*a-(-(Q_pT-((Q_0+Q_fT)+Qr)))*a_Vbi_ci)/a/a);
d_Q_Vci_ei=(((-(Q_pT_Vci_ei-(Q_fT_Vci_ei+Qr_Vci_ei)))*a-(-(Q_pT-((Q_0+Q_fT)+Qr)))*a_Vci_ei)/a/a);
#endif
d_Q=((-(Q_pT-((Q_0+Q_fT)+Qr)))/a);
{
double m00_fabs(d00_fabs0,(0.3*Q_pT))
#if defined(_DERIVATE)
double m10_fabs(d10_fabs0,d00_fabs0,(0.3*Q_pT))
#endif
#if defined(_DERIVATE)
a_Vci_ei=(0.3*Q_pT_Vci_ei)*d10_fabs0;
a_Vbi_ei=(0.3*Q_pT_Vbi_ei)*d10_fabs0;
a_Vbi_ci=(0.3*Q_pT_Vbi_ci)*d10_fabs0;
a_Vtnode_GND=(0.3*Q_pT_Vtnode_GND)*d10_fabs0;
#endif
a=d00_fabs0;
}
{
double m00_fabs(d00_fabs0,d_Q)
if
((d00_fabs0>a))
{
if
((d_Q>=0))
{
#if defined(_DERIVATE)
d_Q_Vtnode_GND=a_Vtnode_GND;
d_Q_Vbi_ei=a_Vbi_ei;
d_Q_Vbi_ci=a_Vbi_ci;
d_Q_Vci_ei=a_Vci_ei;
#endif
d_Q=a;
}
else
{
#if defined(_DERIVATE)
d_Q_Vtnode_GND=(-a_Vtnode_GND);
d_Q_Vbi_ei=(-a_Vbi_ei);
d_Q_Vbi_ci=(-a_Vbi_ci);
d_Q_Vci_ei=(-a_Vci_ei);
#endif
d_Q=(-a);
}
}
}
#if defined(_DERIVATE)
Q_pT_Vtnode_GND=(Q_pT_Vtnode_GND+d_Q_Vtnode_GND);
Q_pT_Vbi_ei=(Q_pT_Vbi_ei+d_Q_Vbi_ei);
Q_pT_Vbi_ci=(Q_pT_Vbi_ci+d_Q_Vbi_ci);
Q_pT_Vci_ei=(Q_pT_Vci_ei+d_Q_Vci_ei);
#endif
Q_pT=(Q_pT+d_Q);
l_it=(l_it+1);
}
#if defined(_DERIVATE)
I_Tf1_Vci_ei=(-i_0f*Q_pT_Vci_ei/Q_pT/Q_pT);
I_Tf1_Vtnode_GND=((i_0f_Vtnode_GND*Q_pT-i_0f*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
I_Tf1_Vbi_ei=((i_0f_Vbi_ei*Q_pT-i_0f*Q_pT_Vbi_ei)/Q_pT/Q_pT);
I_Tf1_Vbi_ci=(-i_0f*Q_pT_Vbi_ci/Q_pT/Q_pT);
#endif
I_Tf1=(i_0f/Q_pT);
#if defined(_DERIVATE)
a_h_Vci_ei=(Oich*I_Tf1_Vci_ei);
a_h_Vtnode_GND=(Oich*I_Tf1_Vtnode_GND);
a_h_Vbi_ei=(Oich*I_Tf1_Vbi_ei);
a_h_Vbi_ci=(Oich*I_Tf1_Vbi_ci);
#endif
a_h=(Oich*I_Tf1);
#if defined(_DERIVATE)
itf_Vci_ei=((I_Tf1_Vci_ei*(1.0+a_h))+(I_Tf1*a_h_Vci_ei));
itf_Vtnode_GND=((I_Tf1_Vtnode_GND*(1.0+a_h))+(I_Tf1*a_h_Vtnode_GND));
itf_Vbi_ei=((I_Tf1_Vbi_ei*(1.0+a_h))+(I_Tf1*a_h_Vbi_ei));
itf_Vbi_ci=((I_Tf1_Vbi_ci*(1.0+a_h))+(I_Tf1*a_h_Vbi_ci));
#endif
itf=(I_Tf1*(1.0+a_h));
#if defined(_DERIVATE)
itr_Vci_ei=(-i_0r*Q_pT_Vci_ei/Q_pT/Q_pT);
itr_Vtnode_GND=((i_0r_Vtnode_GND*Q_pT-i_0r*Q_pT_Vtnode_GND)/Q_pT/Q_pT);
itr_Vbi_ci=((i_0r_Vbi_ci*Q_pT-i_0r*Q_pT_Vbi_ci)/Q_pT/Q_pT);
itr_Vbi_ei=(-i_0r*Q_pT_Vbi_ei/Q_pT/Q_pT);
#endif
itr=(i_0r/Q_pT);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Tf_Vci_ei=0.0;
Tf_Vtnode_GND=T_f0_Vtnode_GND;
Tf_Vbi_ci=T_f0_Vbi_ci;
Tf_Vbi_ei=T_f0_Vbi_ei;
#endif
Tf=T_f0;
#endif
#if defined(_DERIVATE)
Qf_Vci_ei=(T_f0*itf_Vci_ei);
Qf_Vtnode_GND=((T_f0_Vtnode_GND*itf)+(T_f0*itf_Vtnode_GND));
Qf_Vbi_ci=((T_f0_Vbi_ci*itf)+(T_f0*itf_Vbi_ci));
Qf_Vbi_ei=((T_f0_Vbi_ei*itf)+(T_f0*itf_Vbi_ei));
#endif
Qf=(T_f0*itf);
if
((itf<(1.0e-6*ick)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Q_fT_Vci_ei=Qf_Vci_ei;
Q_fT_Vtnode_GND=Qf_Vtnode_GND;
Q_fT_Vbi_ci=Qf_Vbi_ci;
Q_fT_Vbi_ei=Qf_Vbi_ei;
#endif
Q_fT=Qf;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T_fT_Vci_ei=Tf_Vci_ei;
T_fT_Vtnode_GND=Tf_Vtnode_GND;
T_fT_Vbi_ci=Tf_Vbi_ci;
T_fT_Vbi_ei=Tf_Vbi_ei;
#endif
T_fT=Tf;
#endif
}
else
{
#if defined(_DERIVATE)
FFa_Vci_ei=((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf);
FFa_Vtnode_GND=((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf);
FFa_Vbi_ei=(-ick*itf_Vbi_ei/itf/itf);
FFa_Vbi_ci=(-ick*itf_Vbi_ci/itf/itf);
#endif
FFa=(ick/itf);
{
double m00_logE(d00_logE0,FFa)
double m00_exp(d00_exp1,((-gtfe)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FFa)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
FFd_TfE_Vtnode_GND=((tef0_t_Vtnode_GND*d00_exp1)+(tef0_t*((-gtfe)*FFa_Vtnode_GND*d10_logE0)*d10_exp1));
FFd_TfE_Vci_ei=(tef0_t*((-gtfe)*FFa_Vci_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ei=(tef0_t*((-gtfe)*FFa_Vbi_ei*d10_logE0)*d10_exp1);
FFd_TfE_Vbi_ci=(tef0_t*((-gtfe)*FFa_Vbi_ci*d10_logE0)*d10_exp1);
#endif
FFd_TfE=(tef0_t*d00_exp1);
}
#if defined(_DERIVATE)
FFd_QfE_Vtnode_GND=(((FFd_TfE_Vtnode_GND*itf)+(FFd_TfE*itf_Vtnode_GND))/(gtfe+1.0));
FFd_QfE_Vci_ei=(((FFd_TfE_Vci_ei*itf)+(FFd_TfE*itf_Vci_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ei=(((FFd_TfE_Vbi_ei*itf)+(FFd_TfE*itf_Vbi_ei))/(gtfe+1.0));
FFd_QfE_Vbi_ci=(((FFd_TfE_Vbi_ci*itf)+(FFd_TfE*itf_Vbi_ci))/(gtfe+1.0));
#endif
FFd_QfE=((FFd_TfE*itf)/(gtfe+1.0));
#if defined(_DERIVATE)
FFT_fbS_Vtnode_GND=((1.0-fthc)*thcs_t_Vtnode_GND);
#endif
FFT_fbS=((1.0-fthc)*thcs_t);
#if defined(_DERIVATE)
FFx_Vci_ei=(-FFa_Vci_ei);
FFx_Vtnode_GND=(-FFa_Vtnode_GND);
FFx_Vbi_ei=(-FFa_Vbi_ei);
FFx_Vbi_ci=(-FFa_Vbi_ci);
#endif
FFx=(1.0-FFa);
{
double m00_sqrt(d00_sqrt0,((FFx*FFx)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FFx*FFx)+alhc))
#endif
#if defined(_DERIVATE)
FFs_Vci_ei=((FFx_Vci_ei*FFx)+(FFx*FFx_Vci_ei))*d10_sqrt0;
FFs_Vtnode_GND=((FFx_Vtnode_GND*FFx)+(FFx*FFx_Vtnode_GND))*d10_sqrt0;
FFs_Vbi_ei=((FFx_Vbi_ei*FFx)+(FFx*FFx_Vbi_ei))*d10_sqrt0;
FFs_Vbi_ci=((FFx_Vbi_ci*FFx)+(FFx*FFx_Vbi_ci))*d10_sqrt0;
#endif
FFs=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FFw_Vci_ei=((FFx_Vci_ei+FFs_Vci_ei)/(1.0+d00_sqrt0));
FFw_Vtnode_GND=((FFx_Vtnode_GND+FFs_Vtnode_GND)/(1.0+d00_sqrt0));
FFw_Vbi_ei=((FFx_Vbi_ei+FFs_Vbi_ei)/(1.0+d00_sqrt0));
FFw_Vbi_ci=((FFx_Vbi_ci+FFs_Vbi_ci)/(1.0+d00_sqrt0));
#endif
FFw=((FFx+FFs)/(1.0+d00_sqrt0));
}
#if defined(_DERIVATE)
FFw_2_Vci_ei=((FFw_Vci_ei*FFw)+(FFw*FFw_Vci_ei));
FFw_2_Vtnode_GND=((FFw_Vtnode_GND*FFw)+(FFw*FFw_Vtnode_GND));
FFw_2_Vbi_ei=((FFw_Vbi_ei*FFw)+(FFw*FFw_Vbi_ei));
FFw_2_Vbi_ci=((FFw_Vbi_ci*FFw)+(FFw*FFw_Vbi_ci));
#endif
FFw_2=(FFw*FFw);
#if defined(_DERIVATE)
FFd_QfB_Vtnode_GND=((((FFT_fbS_Vtnode_GND*itf)+(FFT_fbS*itf_Vtnode_GND))*FFw_2)+((FFT_fbS*itf)*FFw_2_Vtnode_GND));
FFd_QfB_Vbi_ei=(((FFT_fbS*itf_Vbi_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ei));
FFd_QfB_Vbi_ci=(((FFT_fbS*itf_Vbi_ci)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vbi_ci));
FFd_QfB_Vci_ei=(((FFT_fbS*itf_Vci_ei)*FFw_2)+((FFT_fbS*itf)*FFw_2_Vci_ei));
#endif
FFd_QfB=((FFT_fbS*itf)*FFw_2);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFa_w_Vci_ei=((FFw_2_Vci_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vci_ei)*FFs-(2.0*FFa)*FFs_Vci_ei)/FFs/FFs)));
FFa_w_Vtnode_GND=((FFw_2_Vtnode_GND*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vtnode_GND)*FFs-(2.0*FFa)*FFs_Vtnode_GND)/FFs/FFs)));
FFa_w_Vbi_ei=((FFw_2_Vbi_ei*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ei)*FFs-(2.0*FFa)*FFs_Vbi_ei)/FFs/FFs)));
FFa_w_Vbi_ci=((FFw_2_Vbi_ci*(1.0+((2.0*FFa)/FFs)))+(FFw_2*(((2.0*FFa_Vbi_ci)*FFs-(2.0*FFa)*FFs_Vbi_ci)/FFs/FFs)));
#endif
FFa_w=(FFw_2*(1.0+((2.0*FFa)/FFs)));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFd_TfB_Vtnode_GND=((FFT_fbS_Vtnode_GND*FFa_w)+(FFT_fbS*FFa_w_Vtnode_GND));
FFd_TfB_Vci_ei=(FFT_fbS*FFa_w_Vci_ei);
FFd_TfB_Vbi_ei=(FFT_fbS*FFa_w_Vbi_ei);
FFd_TfB_Vbi_ci=(FFT_fbS*FFa_w_Vbi_ci);
#endif
FFd_TfB=(FFT_fbS*FFa_w);
#endif
#if defined(_DERIVATE)
FFT_pcS_Vtnode_GND=(fthc*thcs_t_Vtnode_GND);
#endif
FFT_pcS=(fthc*thcs_t);
if
(((latb<=0.0)&&(latl<=0.0)))
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND))*FFw_2)+((FFT_pcS*itf)*FFw_2_Vtnode_GND));
FFQ_fC_Vbi_ei=(((FFT_pcS*itf_Vbi_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ei));
FFQ_fC_Vbi_ci=(((FFT_pcS*itf_Vbi_ci)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vbi_ci));
FFQ_fC_Vci_ei=(((FFT_pcS*itf_Vci_ei)*FFw_2)+((FFT_pcS*itf)*FFw_2_Vci_ei));
#endif
FFQ_fC=((FFT_pcS*itf)*FFw_2);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*FFa_w)+(FFT_pcS*FFa_w_Vtnode_GND));
FFT_fC_Vci_ei=(FFT_pcS*FFa_w_Vci_ei);
FFT_fC_Vbi_ei=(FFT_pcS*FFa_w_Vbi_ei);
FFT_fC_Vbi_ci=(FFT_pcS*FFa_w_Vbi_ci);
#endif
FFT_fC=(FFT_pcS*FFa_w);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=FFQ_fC_Vtnode_GND;
FFQ_cT_Vbi_ei=FFQ_fC_Vbi_ei;
FFQ_cT_Vbi_ci=FFQ_fC_Vbi_ci;
FFQ_cT_Vci_ei=FFQ_fC_Vci_ei;
#endif
FFQ_cT=FFQ_fC;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=FFT_fC_Vtnode_GND;
FFT_cT_Vci_ei=FFT_fC_Vci_ei;
FFT_cT_Vbi_ei=FFT_fC_Vbi_ei;
FFT_cT_Vbi_ci=FFT_fC_Vbi_ci;
#endif
FFT_cT=FFT_fC;
#endif
}
else
{
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*itf)+(FFT_pcS*itf_Vtnode_GND));
FFQ_fC_Vbi_ei=(FFT_pcS*itf_Vbi_ei);
FFQ_fC_Vbi_ci=(FFT_pcS*itf_Vbi_ci);
FFQ_fC_Vci_ei=(FFT_pcS*itf_Vci_ei);
#endif
FFQ_fC=(FFT_pcS*itf);
#if defined(_DERIVATE)
FCa_Vci_ei=(-((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf));
FCa_Vtnode_GND=(-((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf));
FCa_Vbi_ei=(-(-ick*itf_Vbi_ei/itf/itf));
FCa_Vbi_ci=(-(-ick*itf_Vbi_ci/itf/itf));
#endif
FCa=(1.0-(ick/itf));
{
double m00_sqrt(d00_sqrt0,((FCa*FCa)+alhc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((FCa*FCa)+alhc))
#endif
#if defined(_DERIVATE)
FCrt_Vci_ei=((FCa_Vci_ei*FCa)+(FCa*FCa_Vci_ei))*d10_sqrt0;
FCrt_Vtnode_GND=((FCa_Vtnode_GND*FCa)+(FCa*FCa_Vtnode_GND))*d10_sqrt0;
FCrt_Vbi_ei=((FCa_Vbi_ei*FCa)+(FCa*FCa_Vbi_ei))*d10_sqrt0;
FCrt_Vbi_ci=((FCa_Vbi_ci*FCa)+(FCa*FCa_Vbi_ci))*d10_sqrt0;
#endif
FCrt=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1.0+alhc))
#if defined(_DERIVATE)
FCa_ck_Vci_ei=(-((FCa_Vci_ei+FCrt_Vci_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vtnode_GND=(-((FCa_Vtnode_GND+FCrt_Vtnode_GND)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ei=(-((FCa_Vbi_ei+FCrt_Vbi_ei)/(1.0+d00_sqrt0)));
FCa_ck_Vbi_ci=(-((FCa_Vbi_ci+FCrt_Vbi_ci)/(1.0+d00_sqrt0)));
#endif
FCa_ck=(1.0-((FCa+FCrt)/(1.0+d00_sqrt0)));
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdaick_ditf_Vci_ei=((((FCa_ck_Vci_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vci_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vci_ei*itf)+(FCrt*itf_Vci_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vtnode_GND=((((FCa_ck_Vtnode_GND*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vtnode_GND)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vtnode_GND*itf)+(FCrt*itf_Vtnode_GND)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ei=((((FCa_ck_Vbi_ei*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ei)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ei*itf)+(FCrt*itf_Vbi_ei)))/(FCrt*itf)/(FCrt*itf));
FCdaick_ditf_Vbi_ci=((((FCa_ck_Vbi_ci*(1-FCa))+((FCa_ck-1.0)*(-FCa_Vbi_ci)))*(FCrt*itf)-((FCa_ck-1.0)*(1-FCa))*((FCrt_Vbi_ci*itf)+(FCrt*itf_Vbi_ci)))/(FCrt*itf)/(FCrt*itf));
#endif
FCdaick_ditf=(((FCa_ck-1.0)*(1-FCa))/(FCrt*itf));
#endif
if
((latb>latl))
{
#if defined(_DERIVATE)
FCz_Vci_ei=0.0;
FCz_Vtnode_GND=0.0;
FCz_Vbi_ei=0.0;
FCz_Vbi_ci=0.0;
#endif
FCz=(latb-latl);
FCxl=(1.0+latl);
FCxb=(1.0+latb);
if
((latb>0.01))
{
{
double m00_logE(d00_logE0,(FCxb/FCxl))
FCln=d00_logE0;
}
{
double m00_exp(d00_exp0,((FCa_ck-1.0)*FCln))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
FCa1_Vci_ei=FCa_ck_Vci_ei*FCln*d10_exp0;
FCa1_Vtnode_GND=FCa_ck_Vtnode_GND*FCln*d10_exp0;
FCa1_Vbi_ei=FCa_ck_Vbi_ei*FCln*d10_exp0;
FCa1_Vbi_ci=FCa_ck_Vbi_ci*FCln*d10_exp0;
#endif
FCa1=d00_exp0;
}
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCa1_Vci_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vtnode_GND=(-(-FCa1_Vtnode_GND*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ei=(-(-FCa1_Vbi_ei*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
FCd_a_Vbi_ci=(-(-FCa1_Vbi_ci*latb)/(latl-(FCa1*latb))/(latl-(FCa1*latb)));
#endif
FCd_a=(1.0/(latl-(FCa1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCa1_Vci_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCa1_Vtnode_GND*FCd_a)+((FCa1-1.0)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCa1_Vbi_ei*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCa1_Vbi_ci*FCd_a)+((FCa1-1.0)*FCd_a_Vbi_ci));
#endif
FCw=((FCa1-1.0)*FCd_a);
{
double m00_logE(d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,((1.0+(latb*FCw))/(1.0+(latl*FCw))))
#endif
#if defined(_DERIVATE)
FCa_Vci_ei=(((latb*FCw_Vci_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vci_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vtnode_GND=(((latb*FCw_Vtnode_GND)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vtnode_GND))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ei=(((latb*FCw_Vbi_ei)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ei))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
FCa_Vbi_ci=(((latb*FCw_Vbi_ci)*(1.0+(latl*FCw))-(1.0+(latb*FCw))*(latl*FCw_Vbi_ci))/(1.0+(latl*FCw))/(1.0+(latl*FCw)))*d10_logE0;
#endif
FCa=d00_logE0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=(((((((-FCz_Vci_ei)*FCa1)+((-FCz)*FCa1_Vci_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vci_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=(((((((-FCz_Vtnode_GND)*FCa1)+((-FCz)*FCa1_Vtnode_GND))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vtnode_GND))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=(((((((-FCz_Vbi_ei)*FCa1)+((-FCz)*FCa1_Vbi_ei))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ei))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=(((((((-FCz_Vbi_ci)*FCa1)+((-FCz)*FCa1_Vbi_ci))*FCln*FCd_a)+((((-FCz)*FCa1)*FCln)*FCd_a_Vbi_ci))*FCd_a)+(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((((-FCz)*FCa1)*FCln)*FCd_a)*FCd_a);
#endif
{
double m00_logE(d00_logE0,(1.0+(latb*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latb*FCw)))
#endif
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei)*d10_logE0;
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND)*d10_logE0;
FClnb_Vbi_ei=(latb*FCw_Vbi_ei)*d10_logE0;
FClnb_Vbi_ci=(latb*FCw_Vbi_ci)*d10_logE0;
#endif
FClnb=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0+(latl*FCw)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+(latl*FCw)))
#endif
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei)*d10_logE0;
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND)*d10_logE0;
FClnl_Vbi_ei=(latl*FCw_Vbi_ei)*d10_logE0;
FClnl_Vbi_ci=(latl*FCw_Vbi_ci)*d10_logE0;
#endif
FClnl=d00_logE0;
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCa1_Vci_ei=(FClnb_Vci_ei-FClnl_Vci_ei);
FCa1_Vtnode_GND=(FClnb_Vtnode_GND-FClnl_Vtnode_GND);
FCa1_Vbi_ei=(FClnb_Vbi_ei-FClnl_Vbi_ei);
FCa1_Vbi_ci=(FClnb_Vbi_ci-FClnl_Vbi_ci);
#endif
FCa1=(FClnb-FClnl);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=((-latb*(latb*FCw_Vci_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vci_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vtnode_GND=((-latb*(latb*FCw_Vtnode_GND)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vtnode_GND)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ei=((-latb*(latb*FCw_Vbi_ei)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ei)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
FCda1_dw_Vbi_ci=((-latb*(latb*FCw_Vbi_ci)/(1.0+(latb*FCw))/(1.0+(latb*FCw)))-(-latl*(latl*FCw_Vbi_ci)/(1.0+(latl*FCw))/(1.0+(latl*FCw))));
#endif
FCda1_dw=((latb/(1.0+(latb*FCw)))-(latl/(1.0+(latl*FCw))));
#endif
}
else
{
#if defined(_DERIVATE)
FCf1_Vci_ei=(-FCa_ck_Vci_ei);
FCf1_Vtnode_GND=(-FCa_ck_Vtnode_GND);
FCf1_Vbi_ei=(-FCa_ck_Vbi_ei);
FCf1_Vbi_ci=(-FCa_ck_Vbi_ci);
#endif
FCf1=(1.0-FCa_ck);
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-(-FCf1_Vci_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vtnode_GND=(-(-FCf1_Vtnode_GND*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ei=(-(-FCf1_Vbi_ei*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
FCd_a_Vbi_ci=(-(-FCf1_Vbi_ci*latb)/(1.0-(FCf1*latb))/(1.0-(FCf1*latb)));
#endif
FCd_a=(1.0/(1.0-(FCf1*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=((FCf1_Vci_ei*FCd_a)+(FCf1*FCd_a_Vci_ei));
FCw_Vtnode_GND=((FCf1_Vtnode_GND*FCd_a)+(FCf1*FCd_a_Vtnode_GND));
FCw_Vbi_ei=((FCf1_Vbi_ei*FCd_a)+(FCf1*FCd_a_Vbi_ei));
FCw_Vbi_ci=((FCf1_Vbi_ci*FCd_a)+(FCf1*FCd_a_Vbi_ci));
#endif
FCw=(FCf1*FCd_a);
#if defined(_DERIVATE)
FCa_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa=(FCz*FCw);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-1.0)*FCd_a_Vci_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-1.0)*FCd_a_Vtnode_GND)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-1.0)*FCd_a_Vbi_ei)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-1.0)*FCd_a_Vbi_ci)*FCd_a)+(((-1.0)*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-1.0)*FCd_a)*FCd_a);
#endif
#if defined(_DERIVATE)
FClnb_Vci_ei=(latb*FCw_Vci_ei);
FClnb_Vtnode_GND=(latb*FCw_Vtnode_GND);
FClnb_Vbi_ei=(latb*FCw_Vbi_ei);
FClnb_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FClnb=(latb*FCw);
#if defined(_DERIVATE)
FClnl_Vci_ei=(latl*FCw_Vci_ei);
FClnl_Vtnode_GND=(latl*FCw_Vtnode_GND);
FClnl_Vbi_ei=(latl*FCw_Vbi_ei);
FClnl_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
FClnl=(latl*FCw);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCa1_Vci_ei=((FCz_Vci_ei*FCw)+(FCz*FCw_Vci_ei));
FCa1_Vtnode_GND=((FCz_Vtnode_GND*FCw)+(FCz*FCw_Vtnode_GND));
FCa1_Vbi_ei=((FCz_Vbi_ei*FCw)+(FCz*FCw_Vbi_ei));
FCa1_Vbi_ci=((FCz_Vbi_ci*FCw)+(FCz*FCw_Vbi_ci));
#endif
FCa1=(FCz*FCw);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCda1_dw_Vci_ei=FCz_Vci_ei;
FCda1_dw_Vtnode_GND=FCz_Vtnode_GND;
FCda1_dw_Vbi_ei=FCz_Vbi_ei;
FCda1_dw_Vbi_ci=FCz_Vbi_ci;
#endif
FCda1_dw=FCz;
#endif
}
#if defined(_DERIVATE)
FCf_CT_Vci_ei=(-2.0*FCz_Vci_ei/FCz/FCz);
FCf_CT_Vbi_ei=(-2.0*FCz_Vbi_ei/FCz/FCz);
FCf_CT_Vbi_ci=(-2.0*FCz_Vbi_ci/FCz/FCz);
FCf_CT_Vtnode_GND=(-2.0*FCz_Vtnode_GND/FCz/FCz);
#endif
FCf_CT=(2.0/FCz);
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCf1_Vci_ei=(((((((latb*latl)*FCw_Vci_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vci_ei))/3.0)+(((latb+latl)*FCw2_Vci_ei)/2.0))+FCw_Vci_ei);
FCf1_Vtnode_GND=(((((((latb*latl)*FCw_Vtnode_GND)*FCw2)+(((latb*latl)*FCw)*FCw2_Vtnode_GND))/3.0)+(((latb+latl)*FCw2_Vtnode_GND)/2.0))+FCw_Vtnode_GND);
FCf1_Vbi_ei=(((((((latb*latl)*FCw_Vbi_ei)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ei))/3.0)+(((latb+latl)*FCw2_Vbi_ei)/2.0))+FCw_Vbi_ei);
FCf1_Vbi_ci=(((((((latb*latl)*FCw_Vbi_ci)*FCw2)+(((latb*latl)*FCw)*FCw2_Vbi_ci))/3.0)+(((latb+latl)*FCw2_Vbi_ci)/2.0))+FCw_Vbi_ci);
#endif
FCf1=((((((latb*latl)*FCw)*FCw2)/3.0)+(((latb+latl)*FCw2)/2.0))+FCw);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf1_dw_Vci_ei=(((latb*latl)*FCw2_Vci_ei)+((latb+latl)*FCw_Vci_ei));
FCdf1_dw_Vtnode_GND=(((latb*latl)*FCw2_Vtnode_GND)+((latb+latl)*FCw_Vtnode_GND));
FCdf1_dw_Vbi_ei=(((latb*latl)*FCw2_Vbi_ei)+((latb+latl)*FCw_Vbi_ei));
FCdf1_dw_Vbi_ci=(((latb*latl)*FCw2_Vbi_ci)+((latb+latl)*FCw_Vbi_ci));
#endif
FCdf1_dw=((((latb*latl)*FCw2)+((latb+latl)*FCw))+1.0);
#endif
#if defined(_DERIVATE)
z_Vci_ei=(latb*FCw_Vci_ei);
z_Vtnode_GND=(latb*FCw_Vtnode_GND);
z_Vbi_ei=(latb*FCw_Vbi_ei);
z_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
z=(latb*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnb)-1.0))+(a*(2.0*FClnb_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnb)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnb)-1.0))+((a*x)*(3.0*FClnb_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnb)-1.0))+1.0)/9.0);
r=(latl/latb);
#if defined(_DERIVATE)
FCf2_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latb);
FCf2_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latb);
FCf2_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latb);
FCf2_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latb);
#endif
FCf2=((((1.0-r)*a2)+(r*a3))/latb);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vci_ei));
FCdf2_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vtnode_GND));
FCdf2_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ei));
FCdf2_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnb)+((((1.0-r)*x)+(r*a))*FClnb_Vbi_ci));
#endif
FCdf2_dw=((((1.0-r)*x)+(r*a))*FClnb);
#endif
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf2_Vci_ei=(((((((latb*a2_Vci_ei)+(latl*a3_Vci_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vci_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf2_Vbi_ei=(((((((latb*a2_Vbi_ei)+(latl*a3_Vbi_ei))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ei))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf2_Vbi_ci=(((((((latb*a2_Vbi_ci)+(latl*a3_Vbi_ci))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vbi_ci))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf2_Vtnode_GND=(((((((latb*a2_Vtnode_GND)+(latl*a3_Vtnode_GND))*FCw)+(((latb*a2)+(latl*a3))*FCw_Vtnode_GND))*FCw)+((((latb*a2)+(latl*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf2=(((((latb*a2)+(latl*a3))*FCw)*FCw)/6.0);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latl*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf2_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latl*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf2_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latl*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf2_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latl*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latl*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf2_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latl*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
#endif
}
#if defined(_DERIVATE)
z_Vci_ei=(latl*FCw_Vci_ei);
z_Vtnode_GND=(latl*FCw_Vtnode_GND);
z_Vbi_ei=(latl*FCw_Vbi_ei);
z_Vbi_ci=(latl*FCw_Vbi_ci);
#endif
z=(latl*FCw);
if
((z>1.0e-6))
{
#if defined(_DERIVATE)
x_Vci_ei=z_Vci_ei;
x_Vtnode_GND=z_Vtnode_GND;
x_Vbi_ei=z_Vbi_ei;
x_Vbi_ci=z_Vbi_ci;
#endif
x=(1.0+z);
#if defined(_DERIVATE)
a_Vci_ei=((x_Vci_ei*x)+(x*x_Vci_ei));
a_Vbi_ei=((x_Vbi_ei*x)+(x*x_Vbi_ei));
a_Vbi_ci=((x_Vbi_ci*x)+(x*x_Vbi_ci));
a_Vtnode_GND=((x_Vtnode_GND*x)+(x*x_Vtnode_GND));
#endif
a=(x*x);
#if defined(_DERIVATE)
a2_Vci_ei=(0.250*((a_Vci_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vci_ei))));
a2_Vbi_ei=(0.250*((a_Vbi_ei*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ei))));
a2_Vbi_ci=(0.250*((a_Vbi_ci*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vbi_ci))));
a2_Vtnode_GND=(0.250*((a_Vtnode_GND*((2.0*FClnl)-1.0))+(a*(2.0*FClnl_Vtnode_GND))));
#endif
a2=(0.250*((a*((2.0*FClnl)-1.0))+1.0));
#if defined(_DERIVATE)
a3_Vci_ei=(((((a_Vci_ei*x)+(a*x_Vci_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vci_ei)))/9.0);
a3_Vbi_ei=(((((a_Vbi_ei*x)+(a*x_Vbi_ei))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ei)))/9.0);
a3_Vbi_ci=(((((a_Vbi_ci*x)+(a*x_Vbi_ci))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vbi_ci)))/9.0);
a3_Vtnode_GND=(((((a_Vtnode_GND*x)+(a*x_Vtnode_GND))*((3.0*FClnl)-1.0))+((a*x)*(3.0*FClnl_Vtnode_GND)))/9.0);
#endif
a3=((((a*x)*((3.0*FClnl)-1.0))+1.0)/9.0);
r=(latb/latl);
#if defined(_DERIVATE)
FCf3_Vci_ei=((((1.0-r)*a2_Vci_ei)+(r*a3_Vci_ei))/latl);
FCf3_Vbi_ei=((((1.0-r)*a2_Vbi_ei)+(r*a3_Vbi_ei))/latl);
FCf3_Vbi_ci=((((1.0-r)*a2_Vbi_ci)+(r*a3_Vbi_ci))/latl);
FCf3_Vtnode_GND=((((1.0-r)*a2_Vtnode_GND)+(r*a3_Vtnode_GND))/latl);
#endif
FCf3=((((1.0-r)*a2)+(r*a3))/latl);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=(((((1.0-r)*x_Vci_ei)+(r*a_Vci_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vci_ei));
FCdf3_dw_Vtnode_GND=(((((1.0-r)*x_Vtnode_GND)+(r*a_Vtnode_GND))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vtnode_GND));
FCdf3_dw_Vbi_ei=(((((1.0-r)*x_Vbi_ei)+(r*a_Vbi_ei))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ei));
FCdf3_dw_Vbi_ci=(((((1.0-r)*x_Vbi_ci)+(r*a_Vbi_ci))*FClnl)+((((1.0-r)*x)+(r*a))*FClnl_Vbi_ci));
#endif
FCdf3_dw=((((1.0-r)*x)+(r*a))*FClnl);
#endif
}
else
{
#if defined(_DERIVATE)
a_Vci_ei=((z_Vci_ei*z)+(z*z_Vci_ei));
a_Vbi_ei=((z_Vbi_ei*z)+(z*z_Vbi_ei));
a_Vbi_ci=((z_Vbi_ci*z)+(z*z_Vbi_ci));
a_Vtnode_GND=((z_Vtnode_GND*z)+(z*z_Vtnode_GND));
#endif
a=(z*z);
#if defined(_DERIVATE)
a2_Vci_ei=((z_Vci_ei-(0.5*a_Vci_ei))+((z_Vci_ei*a)+(z*a_Vci_ei)));
a2_Vbi_ei=((z_Vbi_ei-(0.5*a_Vbi_ei))+((z_Vbi_ei*a)+(z*a_Vbi_ei)));
a2_Vbi_ci=((z_Vbi_ci-(0.5*a_Vbi_ci))+((z_Vbi_ci*a)+(z*a_Vbi_ci)));
a2_Vtnode_GND=((z_Vtnode_GND-(0.5*a_Vtnode_GND))+((z_Vtnode_GND*a)+(z*a_Vtnode_GND)));
#endif
a2=(((3.0+z)-(0.5*a))+(z*a));
#if defined(_DERIVATE)
a3_Vci_ei=((((-2.0)*z_Vci_ei)+(1.5*a_Vci_ei))+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0));
a3_Vbi_ei=((((-2.0)*z_Vbi_ei)+(1.5*a_Vbi_ei))+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0));
a3_Vbi_ci=((((-2.0)*z_Vbi_ci)+(1.5*a_Vbi_ci))+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0));
a3_Vtnode_GND=((((-2.0)*z_Vtnode_GND)+(1.5*a_Vtnode_GND))+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0));
#endif
a3=((((-2.0)*z)+(1.5*a))+(((2.0*a)*a)/3.0));
#if defined(_DERIVATE)
FCf3_Vci_ei=(((((((latl*a2_Vci_ei)+(latb*a3_Vci_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vci_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vci_ei))/6.0);
FCf3_Vbi_ei=(((((((latl*a2_Vbi_ei)+(latb*a3_Vbi_ei))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ei))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ei))/6.0);
FCf3_Vbi_ci=(((((((latl*a2_Vbi_ci)+(latb*a3_Vbi_ci))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vbi_ci))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vbi_ci))/6.0);
FCf3_Vtnode_GND=(((((((latl*a2_Vtnode_GND)+(latb*a3_Vtnode_GND))*FCw)+(((latl*a2)+(latb*a3))*FCw_Vtnode_GND))*FCw)+((((latl*a2)+(latb*a3))*FCw)*FCw_Vtnode_GND))/6.0);
#endif
FCf3=(((((latl*a2)+(latb*a3))*FCw)*FCw)/6.0);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((((z_Vci_ei+(0.5*a_Vci_ei))-(((a_Vci_ei*z)+(a*z_Vci_ei))/3.0))+((((5.0*a_Vci_ei)*a)+((5.0*a)*a_Vci_ei))/6.0))+(((latb*FCw_Vci_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vci_ei-z_Vci_ei)+((((2.0*a_Vci_ei)*a)+((2.0*a)*a_Vci_ei))/3.0)))));
FCdf3_dw_Vtnode_GND=((((z_Vtnode_GND+(0.5*a_Vtnode_GND))-(((a_Vtnode_GND*z)+(a*z_Vtnode_GND))/3.0))+((((5.0*a_Vtnode_GND)*a)+((5.0*a)*a_Vtnode_GND))/6.0))+(((latb*FCw_Vtnode_GND)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vtnode_GND-z_Vtnode_GND)+((((2.0*a_Vtnode_GND)*a)+((2.0*a)*a_Vtnode_GND))/3.0)))));
FCdf3_dw_Vbi_ei=((((z_Vbi_ei+(0.5*a_Vbi_ei))-(((a_Vbi_ei*z)+(a*z_Vbi_ei))/3.0))+((((5.0*a_Vbi_ei)*a)+((5.0*a)*a_Vbi_ei))/6.0))+(((latb*FCw_Vbi_ei)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ei-z_Vbi_ei)+((((2.0*a_Vbi_ei)*a)+((2.0*a)*a_Vbi_ei))/3.0)))));
FCdf3_dw_Vbi_ci=((((z_Vbi_ci+(0.5*a_Vbi_ci))-(((a_Vbi_ci*z)+(a*z_Vbi_ci))/3.0))+((((5.0*a_Vbi_ci)*a)+((5.0*a)*a_Vbi_ci))/6.0))+(((latb*FCw_Vbi_ci)*((a-z)+(((2.0*a)*a)/3.0)))+((latb*FCw)*((a_Vbi_ci-z_Vbi_ci)+((((2.0*a_Vbi_ci)*a)+((2.0*a)*a_Vbi_ci))/3.0)))));
#endif
FCdf3_dw=((((z+(0.5*a))-((a*z)/3.0))+(((5.0*a)*a)/6.0))+((latb*FCw)*((a-z)+(((2.0*a)*a)/3.0))));
#endif
}
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((FCf_CT_Vci_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vci_ei*FCf1)+(FCa*FCf1_Vci_ei))-FCf2_Vci_ei)+FCf3_Vci_ei)));
FCf_ci_Vtnode_GND=((FCf_CT_Vtnode_GND*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vtnode_GND*FCf1)+(FCa*FCf1_Vtnode_GND))-FCf2_Vtnode_GND)+FCf3_Vtnode_GND)));
FCf_ci_Vbi_ei=((FCf_CT_Vbi_ei*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ei*FCf1)+(FCa*FCf1_Vbi_ei))-FCf2_Vbi_ei)+FCf3_Vbi_ei)));
FCf_ci_Vbi_ci=((FCf_CT_Vbi_ci*(((FCa*FCf1)-FCf2)+FCf3))+(FCf_CT*((((FCa_Vbi_ci*FCf1)+(FCa*FCf1_Vbi_ci))-FCf2_Vbi_ci)+FCf3_Vbi_ci)));
#endif
FCf_ci=(FCf_CT*(((FCa*FCf1)-FCf2)+FCf3));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=((FCf_CT_Vci_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vci_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vci_ei))+((FCda1_dw_Vci_ei*FCf1)+(FCda1_dw*FCf1_Vci_ei)))-FCdf2_dw_Vci_ei)+FCdf3_dw_Vci_ei)));
FCdfc_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vtnode_GND*FCdf1_dw)+(FCa1*FCdf1_dw_Vtnode_GND))+((FCda1_dw_Vtnode_GND*FCf1)+(FCda1_dw*FCf1_Vtnode_GND)))-FCdf2_dw_Vtnode_GND)+FCdf3_dw_Vtnode_GND)));
FCdfc_dw_Vbi_ei=((FCf_CT_Vbi_ei*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ei*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ei))+((FCda1_dw_Vbi_ei*FCf1)+(FCda1_dw*FCf1_Vbi_ei)))-FCdf2_dw_Vbi_ei)+FCdf3_dw_Vbi_ei)));
FCdfc_dw_Vbi_ci=((FCf_CT_Vbi_ci*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw))+(FCf_CT*(((((FCa1_Vbi_ci*FCdf1_dw)+(FCa1*FCdf1_dw_Vbi_ci))+((FCda1_dw_Vbi_ci*FCf1)+(FCda1_dw*FCf1_Vbi_ci)))-FCdf2_dw_Vbi_ci)+FCdf3_dw_Vbi_ci)));
#endif
FCdfc_dw=(FCf_CT*((((FCa1*FCdf1_dw)+(FCda1_dw*FCf1))-FCdf2_dw)+FCdf3_dw));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
a_Vci_ei=(latb*FCw_Vci_ei);
a_Vbi_ei=(latb*FCw_Vbi_ei);
a_Vbi_ci=(latb*FCw_Vbi_ci);
a_Vtnode_GND=(latb*FCw_Vtnode_GND);
#endif
a=(latb*FCw);
#endif
#ifdef _DYNAMIC
if
((a>1.0e-6))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf2_Vci_ei=((a_Vci_ei-FClnb_Vci_ei)/latb);
FCf2_Vbi_ei=((a_Vbi_ei-FClnb_Vbi_ei)/latb);
FCf2_Vbi_ci=((a_Vbi_ci-FClnb_Vbi_ci)/latb);
FCf2_Vtnode_GND=((a_Vtnode_GND-FClnb_Vtnode_GND)/latb);
#endif
FCf2=((a-FClnb)/latb);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf2_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf2_dw=(a/(1.0+a));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf2_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf2_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf2_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf2_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf2=((0.5*a)*FCw);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf2_dw_Vci_ei=a_Vci_ei;
FCdf2_dw_Vtnode_GND=a_Vtnode_GND;
FCdf2_dw_Vbi_ei=a_Vbi_ei;
FCdf2_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf2_dw=a;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
a_Vci_ei=(latl*FCw_Vci_ei);
a_Vbi_ei=(latl*FCw_Vbi_ei);
a_Vbi_ci=(latl*FCw_Vbi_ci);
a_Vtnode_GND=(latl*FCw_Vtnode_GND);
#endif
a=(latl*FCw);
#endif
#ifdef _DYNAMIC
if
((a>1.0e-6))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf3_Vci_ei=((a_Vci_ei-FClnl_Vci_ei)/latl);
FCf3_Vbi_ei=((a_Vbi_ei-FClnl_Vbi_ei)/latl);
FCf3_Vbi_ci=((a_Vbi_ci-FClnl_Vbi_ci)/latl);
FCf3_Vtnode_GND=((a_Vtnode_GND-FClnl_Vtnode_GND)/latl);
#endif
FCf3=((a-FClnl)/latl);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=((a_Vci_ei*(1.0+a)-a*a_Vci_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vtnode_GND=((a_Vtnode_GND*(1.0+a)-a*a_Vtnode_GND)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ei=((a_Vbi_ei*(1.0+a)-a*a_Vbi_ei)/(1.0+a)/(1.0+a));
FCdf3_dw_Vbi_ci=((a_Vbi_ci*(1.0+a)-a*a_Vbi_ci)/(1.0+a)/(1.0+a));
#endif
FCdf3_dw=(a/(1.0+a));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf3_Vci_ei=(((0.5*a_Vci_ei)*FCw)+((0.5*a)*FCw_Vci_ei));
FCf3_Vbi_ei=(((0.5*a_Vbi_ei)*FCw)+((0.5*a)*FCw_Vbi_ei));
FCf3_Vbi_ci=(((0.5*a_Vbi_ci)*FCw)+((0.5*a)*FCw_Vbi_ci));
FCf3_Vtnode_GND=(((0.5*a_Vtnode_GND)*FCw)+((0.5*a)*FCw_Vtnode_GND));
#endif
FCf3=((0.5*a)*FCw);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdf3_dw_Vci_ei=a_Vci_ei;
FCdf3_dw_Vtnode_GND=a_Vtnode_GND;
FCdf3_dw_Vbi_ei=a_Vbi_ei;
FCdf3_dw_Vbi_ci=a_Vbi_ci;
#endif
FCdf3_dw=a;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((FCf_CT_Vci_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vci_ei-FCf3_Vci_ei)));
FCf_CT_Vbi_ei=((FCf_CT_Vbi_ei*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ei-FCf3_Vbi_ei)));
FCf_CT_Vbi_ci=((FCf_CT_Vbi_ci*(FCf2-FCf3))+(FCf_CT*(FCf2_Vbi_ci-FCf3_Vbi_ci)));
FCf_CT_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCf2-FCf3))+(FCf_CT*(FCf2_Vtnode_GND-FCf3_Vtnode_GND)));
#endif
FCf_CT=(FCf_CT*(FCf2-FCf3));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=((FCf_CT_Vci_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vci_ei-FCdf3_dw_Vci_ei)));
FCdfCT_dw_Vbi_ei=((FCf_CT_Vbi_ei*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ei-FCdf3_dw_Vbi_ei)));
FCdfCT_dw_Vbi_ci=((FCf_CT_Vbi_ci*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vbi_ci-FCdf3_dw_Vbi_ci)));
FCdfCT_dw_Vtnode_GND=((FCf_CT_Vtnode_GND*(FCdf2_dw-FCdf3_dw))+(FCf_CT*(FCdf2_dw_Vtnode_GND-FCdf3_dw_Vtnode_GND)));
#endif
FCdfCT_dw=(FCf_CT*(FCdf2_dw-FCdf3_dw));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
#endif
}
else
{
if
((latb>0.01))
{
#if defined(_DERIVATE)
FCd_a_Vci_ei=(-FCa_ck_Vci_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vtnode_GND=(-FCa_ck_Vtnode_GND*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ei=(-FCa_ck_Vbi_ei*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
FCd_a_Vbi_ci=(-FCa_ck_Vbi_ci*latb/(1.0+(FCa_ck*latb))/(1.0+(FCa_ck*latb)));
#endif
FCd_a=(1.0/(1.0+(FCa_ck*latb)));
#if defined(_DERIVATE)
FCw_Vci_ei=(((-FCa_ck_Vci_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vci_ei));
FCw_Vtnode_GND=(((-FCa_ck_Vtnode_GND)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vtnode_GND));
FCw_Vbi_ei=(((-FCa_ck_Vbi_ei)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ei));
FCw_Vbi_ci=(((-FCa_ck_Vbi_ci)*FCd_a)+((1.0-FCa_ck)*FCd_a_Vbi_ci));
#endif
FCw=((1.0-FCa_ck)*FCd_a);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=((((-(1.0+latb))*FCd_a_Vci_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vci_ei));
FCdw_daick_Vtnode_GND=((((-(1.0+latb))*FCd_a_Vtnode_GND)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vtnode_GND));
FCdw_daick_Vbi_ei=((((-(1.0+latb))*FCd_a_Vbi_ei)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ei));
FCdw_daick_Vbi_ci=((((-(1.0+latb))*FCd_a_Vbi_ci)*FCd_a)+(((-(1.0+latb))*FCd_a)*FCd_a_Vbi_ci));
#endif
FCdw_daick=(((-(1.0+latb))*FCd_a)*FCd_a);
#endif
}
else
{
#if defined(_DERIVATE)
FCw_Vci_ei=((-FCa_ck_Vci_ei)-FCa_ck_Vci_ei*latb);
FCw_Vtnode_GND=((-FCa_ck_Vtnode_GND)-FCa_ck_Vtnode_GND*latb);
FCw_Vbi_ei=((-FCa_ck_Vbi_ei)-FCa_ck_Vbi_ei*latb);
FCw_Vbi_ci=((-FCa_ck_Vbi_ci)-FCa_ck_Vbi_ci*latb);
#endif
FCw=((1.0-FCa_ck)-(FCa_ck*latb));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_daick_Vci_ei=0.0;
FCdw_daick_Vtnode_GND=0.0;
FCdw_daick_Vbi_ei=0.0;
FCdw_daick_Vbi_ci=0.0;
#endif
FCdw_daick=(-(1.0+latb));
#endif
}
#if defined(_DERIVATE)
FCw2_Vci_ei=((FCw_Vci_ei*FCw)+(FCw*FCw_Vci_ei));
FCw2_Vtnode_GND=((FCw_Vtnode_GND*FCw)+(FCw*FCw_Vtnode_GND));
FCw2_Vbi_ei=((FCw_Vbi_ei*FCw)+(FCw*FCw_Vbi_ei));
FCw2_Vbi_ci=((FCw_Vbi_ci*FCw)+(FCw*FCw_Vbi_ci));
#endif
FCw2=(FCw*FCw);
#if defined(_DERIVATE)
FCz_Vci_ei=(latb*FCw_Vci_ei);
FCz_Vtnode_GND=(latb*FCw_Vtnode_GND);
FCz_Vbi_ei=(latb*FCw_Vbi_ei);
FCz_Vbi_ci=(latb*FCw_Vbi_ci);
#endif
FCz=(latb*FCw);
#if defined(_DERIVATE)
FCz_1_Vci_ei=FCz_Vci_ei;
FCz_1_Vtnode_GND=FCz_Vtnode_GND;
FCz_1_Vbi_ei=FCz_Vbi_ei;
FCz_1_Vbi_ci=FCz_Vbi_ci;
#endif
FCz_1=(1.0+FCz);
#if defined(_DERIVATE)
FCd_f_Vci_ei=(-FCz_1_Vci_ei/FCz_1/FCz_1);
FCd_f_Vtnode_GND=(-FCz_1_Vtnode_GND/FCz_1/FCz_1);
FCd_f_Vbi_ei=(-FCz_1_Vbi_ei/FCz_1/FCz_1);
FCd_f_Vbi_ci=(-FCz_1_Vbi_ci/FCz_1/FCz_1);
#endif
FCd_f=(1.0/FCz_1);
#if defined(_DERIVATE)
FCf_ci_Vci_ei=((((FCw2_Vci_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vci_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vci_ei));
FCf_ci_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vtnode_GND/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vtnode_GND));
FCf_ci_Vbi_ei=((((FCw2_Vbi_ei*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ei/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_ci_Vbi_ci=((((FCw2_Vbi_ci*(1.0+(FCz/3.0)))+(FCw2*(FCz_Vbi_ci/3.0)))*FCd_f)+((FCw2*(1.0+(FCz/3.0)))*FCd_f_Vbi_ci));
#endif
FCf_ci=((FCw2*(1.0+(FCz/3.0)))*FCd_f);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfc_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vci_ei+(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfc_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vtnode_GND+(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
FCdfc_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ei+(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfc_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(FCz_1+((FCz*FCz)/3.0)))+((2.0*FCw)*(FCz_1_Vbi_ci+(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
#endif
FCdfc_dw=((((2.0*FCw)*(FCz_1+((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdw_ditf_Vci_ei=((FCdw_daick_Vci_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vci_ei));
FCdw_ditf_Vtnode_GND=((FCdw_daick_Vtnode_GND*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vtnode_GND));
FCdw_ditf_Vbi_ei=((FCdw_daick_Vbi_ei*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ei));
FCdw_ditf_Vbi_ci=((FCdw_daick_Vbi_ci*FCdaick_ditf)+(FCdw_daick*FCdaick_ditf_Vbi_ci));
#endif
FCdw_ditf=(FCdw_daick*FCdaick_ditf);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfc_ditf_Vci_ei=((FCdfc_dw_Vci_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vci_ei));
FCdfc_ditf_Vtnode_GND=((FCdfc_dw_Vtnode_GND*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vtnode_GND));
FCdfc_ditf_Vbi_ei=((FCdfc_dw_Vbi_ei*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ei));
FCdfc_ditf_Vbi_ci=((FCdfc_dw_Vbi_ci*FCdw_ditf)+(FCdfc_dw*FCdw_ditf_Vbi_ci));
#endif
FCdfc_ditf=(FCdfc_dw*FCdw_ditf);
#endif
#ifdef _DYNAMIC
if
((FCz>0.001))
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,FCz_1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,FCz_1)
#endif
#if defined(_DERIVATE)
FCf_CT_Vci_ei=(((2.0*(((FCz_1_Vci_ei*d00_logE0)+(FCz_1*FCz_1_Vci_ei*d10_logE0))-FCz_Vci_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vci_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ei=(((2.0*(((FCz_1_Vbi_ei*d00_logE0)+(FCz_1*FCz_1_Vbi_ei*d10_logE0))-FCz_Vbi_ei))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ei))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vbi_ci=(((2.0*(((FCz_1_Vbi_ci*d00_logE0)+(FCz_1*FCz_1_Vbi_ci*d10_logE0))-FCz_Vbi_ci))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vbi_ci))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
FCf_CT_Vtnode_GND=(((2.0*(((FCz_1_Vtnode_GND*d00_logE0)+(FCz_1*FCz_1_Vtnode_GND*d10_logE0))-FCz_Vtnode_GND))*((latb*latb)*FCz_1)-(2.0*((FCz_1*d00_logE0)-FCz))*((latb*latb)*FCz_1_Vtnode_GND))/((latb*latb)*FCz_1)/((latb*latb)*FCz_1));
#endif
FCf_CT=((2.0*((FCz_1*d00_logE0)-FCz))/((latb*latb)*FCz_1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((2.0*FCw_Vci_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vci_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((2.0*FCw_Vbi_ei)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ei))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((2.0*FCw_Vbi_ci)*FCd_f)+((2.0*FCw)*FCd_f_Vbi_ci))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((2.0*FCw_Vtnode_GND)*FCd_f)+((2.0*FCw)*FCd_f_Vtnode_GND))*FCd_f)+(((2.0*FCw)*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=(((2.0*FCw)*FCd_f)*FCd_f);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCf_CT_Vci_ei=((((FCw2_Vci_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vci_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vci_ei));
FCf_CT_Vbi_ei=((((FCw2_Vbi_ei*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ei/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ei));
FCf_CT_Vbi_ci=((((FCw2_Vbi_ci*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vbi_ci/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vbi_ci));
FCf_CT_Vtnode_GND=((((FCw2_Vtnode_GND*(1.0-(FCz/3.0)))+(FCw2*(-(FCz_Vtnode_GND/3.0))))*FCd_f)+((FCw2*(1.0-(FCz/3.0)))*FCd_f_Vtnode_GND));
#endif
FCf_CT=((FCw2*(1.0-(FCz/3.0)))*FCd_f);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfCT_dw_Vci_ei=(((((((2.0*FCw_Vci_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vci_ei*FCz)+(FCz*FCz_Vci_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vci_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vci_ei));
FCdfCT_dw_Vbi_ei=(((((((2.0*FCw_Vbi_ei)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ei*FCz)+(FCz*FCz_Vbi_ei))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ei))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ei));
FCdfCT_dw_Vbi_ci=(((((((2.0*FCw_Vbi_ci)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vbi_ci*FCz)+(FCz*FCz_Vbi_ci))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vbi_ci))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vbi_ci));
FCdfCT_dw_Vtnode_GND=(((((((2.0*FCw_Vtnode_GND)*(1.0-((FCz*FCz)/3.0)))+((2.0*FCw)*(-(((FCz_Vtnode_GND*FCz)+(FCz*FCz_Vtnode_GND))/3.0))))*FCd_f)+(((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f_Vtnode_GND))*FCd_f)+((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f_Vtnode_GND));
#endif
FCdfCT_dw=((((2.0*FCw)*(1.0-((FCz*FCz)/3.0)))*FCd_f)*FCd_f);
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FCdfCT_ditf_Vci_ei=((FCdfCT_dw_Vci_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vci_ei));
FCdfCT_ditf_Vbi_ei=((FCdfCT_dw_Vbi_ei*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ei));
FCdfCT_ditf_Vbi_ci=((FCdfCT_dw_Vbi_ci*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vbi_ci));
FCdfCT_ditf_Vtnode_GND=((FCdfCT_dw_Vtnode_GND*FCdw_ditf)+(FCdfCT_dw*FCdw_ditf_Vtnode_GND));
#endif
FCdfCT_ditf=(FCdfCT_dw*FCdw_ditf);
#endif
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFQ_cT_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_CT)+(FFQ_fC*FCf_CT_Vtnode_GND));
FFQ_cT_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ei));
FFQ_cT_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_CT)+(FFQ_fC*FCf_CT_Vbi_ci));
FFQ_cT_Vci_ei=((FFQ_fC_Vci_ei*FCf_CT)+(FFQ_fC*FCf_CT_Vci_ei));
#endif
FFQ_cT=(FFQ_fC*FCf_CT);
#endif
#if defined(_DERIVATE)
FFQ_fC_Vtnode_GND=((FFQ_fC_Vtnode_GND*FCf_ci)+(FFQ_fC*FCf_ci_Vtnode_GND));
FFQ_fC_Vbi_ei=((FFQ_fC_Vbi_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ei));
FFQ_fC_Vbi_ci=((FFQ_fC_Vbi_ci*FCf_ci)+(FFQ_fC*FCf_ci_Vbi_ci));
FFQ_fC_Vci_ei=((FFQ_fC_Vci_ei*FCf_ci)+(FFQ_fC*FCf_ci_Vci_ei));
#endif
FFQ_fC=(FFQ_fC*FCf_ci);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFT_fC_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_ci+(itf*FCdfc_ditf)))+(FFT_pcS*(FCf_ci_Vtnode_GND+((itf_Vtnode_GND*FCdfc_ditf)+(itf*FCdfc_ditf_Vtnode_GND)))));
FFT_fC_Vci_ei=(FFT_pcS*(FCf_ci_Vci_ei+((itf_Vci_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vci_ei))));
FFT_fC_Vbi_ei=(FFT_pcS*(FCf_ci_Vbi_ei+((itf_Vbi_ei*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ei))));
FFT_fC_Vbi_ci=(FFT_pcS*(FCf_ci_Vbi_ci+((itf_Vbi_ci*FCdfc_ditf)+(itf*FCdfc_ditf_Vbi_ci))));
#endif
FFT_fC=(FFT_pcS*(FCf_ci+(itf*FCdfc_ditf)));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
FFT_cT_Vtnode_GND=((FFT_pcS_Vtnode_GND*(FCf_CT+(itf*FCdfCT_ditf)))+(FFT_pcS*(FCf_CT_Vtnode_GND+((itf_Vtnode_GND*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vtnode_GND)))));
FFT_cT_Vci_ei=(FFT_pcS*(FCf_CT_Vci_ei+((itf_Vci_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vci_ei))));
FFT_cT_Vbi_ei=(FFT_pcS*(FCf_CT_Vbi_ei+((itf_Vbi_ei*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ei))));
FFT_cT_Vbi_ci=(FFT_pcS*(FCf_CT_Vbi_ci+((itf_Vbi_ci*FCdfCT_ditf)+(itf*FCdfCT_ditf_Vbi_ci))));
#endif
FFT_cT=(FFT_pcS*(FCf_CT+(itf*FCdfCT_ditf)));
#endif
}
#if defined(_DERIVATE)
Qf_Vci_ei=(Qf_Vci_ei+FFd_QfB_Vci_ei);
Qf_Vtnode_GND=(Qf_Vtnode_GND+FFd_QfB_Vtnode_GND);
Qf_Vbi_ci=(Qf_Vbi_ci+FFd_QfB_Vbi_ci);
Qf_Vbi_ei=(Qf_Vbi_ei+FFd_QfB_Vbi_ei);
#endif
Qf=(Qf+FFd_QfB);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Tf_Vci_ei=(Tf_Vci_ei+FFd_TfB_Vci_ei);
Tf_Vtnode_GND=(Tf_Vtnode_GND+FFd_TfB_Vtnode_GND);
Tf_Vbi_ci=(Tf_Vbi_ci+FFd_TfB_Vbi_ci);
Tf_Vbi_ei=(Tf_Vbi_ei+FFd_TfB_Vbi_ei);
#endif
Tf=(Tf+FFd_TfB);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Q_fT_Vci_ei=((Qf_Vci_ei+(hfe*FFd_QfE_Vci_ei))+(hfc*FFQ_cT_Vci_ei));
Q_fT_Vtnode_GND=((Qf_Vtnode_GND+(hfe*FFd_QfE_Vtnode_GND))+(hfc*FFQ_cT_Vtnode_GND));
Q_fT_Vbi_ci=((Qf_Vbi_ci+(hfe*FFd_QfE_Vbi_ci))+(hfc*FFQ_cT_Vbi_ci));
Q_fT_Vbi_ei=((Qf_Vbi_ei+(hfe*FFd_QfE_Vbi_ei))+(hfc*FFQ_cT_Vbi_ei));
#endif
Q_fT=((Qf+(hfe*FFd_QfE))+(hfc*FFQ_cT));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
T_fT_Vci_ei=((Tf_Vci_ei+(hfe*FFd_TfE_Vci_ei))+(hfc*FFT_cT_Vci_ei));
T_fT_Vtnode_GND=((Tf_Vtnode_GND+(hfe*FFd_TfE_Vtnode_GND))+(hfc*FFT_cT_Vtnode_GND));
T_fT_Vbi_ci=((Tf_Vbi_ci+(hfe*FFd_TfE_Vbi_ci))+(hfc*FFT_cT_Vbi_ci));
T_fT_Vbi_ei=((Tf_Vbi_ei+(hfe*FFd_TfE_Vbi_ei))+(hfc*FFT_cT_Vbi_ei));
#endif
T_fT=((Tf+(hfe*FFd_TfE))+(hfc*FFT_cT));
#endif
#if defined(_DERIVATE)
Qf_Vci_ei=((Qf_Vci_ei+FFd_QfE_Vci_ei)+FFQ_fC_Vci_ei);
Qf_Vtnode_GND=((Qf_Vtnode_GND+FFd_QfE_Vtnode_GND)+FFQ_fC_Vtnode_GND);
Qf_Vbi_ci=((Qf_Vbi_ci+FFd_QfE_Vbi_ci)+FFQ_fC_Vbi_ci);
Qf_Vbi_ei=((Qf_Vbi_ei+FFd_QfE_Vbi_ei)+FFQ_fC_Vbi_ei);
#endif
Qf=((Qf+FFd_QfE)+FFQ_fC);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Tf_Vci_ei=((Tf_Vci_ei+FFd_TfE_Vci_ei)+FFT_fC_Vci_ei);
Tf_Vtnode_GND=((Tf_Vtnode_GND+FFd_TfE_Vtnode_GND)+FFT_fC_Vtnode_GND);
Tf_Vbi_ci=((Tf_Vbi_ci+FFd_TfE_Vbi_ci)+FFT_fC_Vbi_ci);
Tf_Vbi_ei=((Tf_Vbi_ei+FFd_TfE_Vbi_ei)+FFT_fC_Vbi_ei);
#endif
Tf=((Tf+FFd_TfE)+FFT_fC);
#endif
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qr_Vci_ei=(Tr*itr_Vci_ei);
Qr_Vtnode_GND=(Tr*itr_Vtnode_GND);
Qr_Vbi_ci=(Tr*itr_Vbi_ci);
Qr_Vbi_ei=(Tr*itr_Vbi_ei);
#endif
Qr=(Tr*itr);
#endif
}
#if defined(_DERIVATE)
it_Vci_ei=(itf_Vci_ei-itr_Vci_ei);
it_Vtnode_GND=(itf_Vtnode_GND-itr_Vtnode_GND);
it_Vbi_ei=(itf_Vbi_ei-itr_Vbi_ei);
it_Vbi_ci=(itf_Vbi_ci-itr_Vbi_ci);
#endif
it=(itf-itr);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdei_Vci_ei=Qf_Vci_ei;
Qdei_Vtnode_GND=Qf_Vtnode_GND;
Qdei_Vbi_ci=Qf_Vbi_ci;
Qdei_Vbi_ei=Qf_Vbi_ei;
#endif
Qdei=Qf;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdci_Vci_ei=Qr_Vci_ei;
Qdci_Vtnode_GND=Qr_Vtnode_GND;
Qdci_Vbi_ci=Qr_Vbi_ci;
Qdci_Vbi_ei=Qr_Vbi_ei;
#endif
Qdci=Qr;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qrbi_Vtnode_GND=(fcrbi*(((Qjei_Vtnode_GND+Qdei_Vtnode_GND)+Qjci_Vtnode_GND)+Qdci_Vtnode_GND));
qrbi_Vbi_ei=(fcrbi*(((Qjei_Vbi_ei+Qdei_Vbi_ei)+Qjci_Vbi_ei)+Qdci_Vbi_ei));
qrbi_Vci_ei=(fcrbi*(Qdei_Vci_ei+Qdci_Vci_ei));
qrbi_Vbi_ci=(fcrbi*((Qdei_Vbi_ci+Qjci_Vbi_ci)+Qdci_Vbi_ci));
#endif
qrbi=(fcrbi*(((Qjei+Qdei)+Qjci)+Qdci));
#endif
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=(1/(mbci*VT));
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-BP(bi,ci)*(mbci*VT_Vtnode_GND)/(mbci*VT)/(mbci*VT));
#endif
DIOY=(BP(bi,ci)/(mbci*VT));
if
((ibcis>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ibci_Vtnode_GND=((ibcis_t_Vtnode_GND*(le-1.0))+(ibcis_t*le_Vtnode_GND));
ibci_Vbi_ci=(ibcis_t*le_Vbi_ci);
ibci_Vbi_ei=(ibcis_t*le_Vbi_ei);
#endif
ibci=(ibcis_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ibci_Vtnode_GND=(-ibcis_t_Vtnode_GND);
ibci_Vbi_ci=0.0;
ibci_Vbi_ei=0.0;
#endif
ibci=(-ibcis_t);
}
}
else
{
#if defined(_DERIVATE)
ibci_Vtnode_GND=0.0;
ibci_Vbi_ci=0.0;
ibci_Vbi_ei=0.0;
#endif
ibci=0.0;
}
if
((((BP(bi,ci)<0.0)&&(favl_t>0.0))&&(cjci0_t>0.0)))
{
#if defined(_DERIVATE)
AVLv_bord_Vtnode_GND=vdci_t_Vtnode_GND;
AVLv_bord_Vbi_ci=(-1.0);
#endif
AVLv_bord=(vdci_t-BP(bi,ci));
#if defined(_DERIVATE)
AVLv_q_Vtnode_GND=((qavl_t_Vtnode_GND*Cjci-qavl_t*Cjci_Vtnode_GND)/Cjci/Cjci);
AVLv_q_Vbi_ei=(-qavl_t*Cjci_Vbi_ei/Cjci/Cjci);
AVLv_q_Vbi_ci=(-qavl_t*Cjci_Vbi_ci/Cjci/Cjci);
#endif
AVLv_q=(qavl_t/Cjci);
#if defined(_DERIVATE)
AVLU0_Vtnode_GND=((qavl_t_Vtnode_GND*cjci0_t-qavl_t*cjci0_t_Vtnode_GND)/cjci0_t/cjci0_t);
#endif
AVLU0=(qavl_t/cjci0_t);
if
((AVLv_bord>AVLU0))
{
{
double m00_exp(d00_exp0,((-AVLv_q)/AVLU0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
AVLa_Vtnode_GND=((favl_t_Vtnode_GND*d00_exp0)+(favl_t*(((-AVLv_q_Vtnode_GND)*AVLU0-(-AVLv_q)*AVLU0_Vtnode_GND)/AVLU0/AVLU0)*d10_exp0));
AVLa_Vbi_ei=(favl_t*((-AVLv_q_Vbi_ei)/AVLU0)*d10_exp0);
AVLa_Vbi_ci=(favl_t*((-AVLv_q_Vbi_ci)/AVLU0)*d10_exp0);
#endif
AVLa=(favl_t*d00_exp0);
}
#if defined(_DERIVATE)
AVLS_avl_Vtnode_GND=((AVLa_Vtnode_GND*(AVLU0+((1.0+(AVLv_q/AVLU0))*(AVLv_bord-AVLU0))))+(AVLa*(AVLU0_Vtnode_GND+((((AVLv_q_Vtnode_GND*AVLU0-AVLv_q*AVLU0_Vtnode_GND)/AVLU0/AVLU0)*(AVLv_bord-AVLU0))+((1.0+(AVLv_q/AVLU0))*(AVLv_bord_Vtnode_GND-AVLU0_Vtnode_GND))))));
AVLS_avl_Vbi_ei=((AVLa_Vbi_ei*(AVLU0+((1.0+(AVLv_q/AVLU0))*(AVLv_bord-AVLU0))))+(AVLa*(AVLv_q_Vbi_ei/AVLU0)*(AVLv_bord-AVLU0)));
AVLS_avl_Vbi_ci=((AVLa_Vbi_ci*(AVLU0+((1.0+(AVLv_q/AVLU0))*(AVLv_bord-AVLU0))))+(AVLa*(((AVLv_q_Vbi_ci/AVLU0)*(AVLv_bord-AVLU0))+((1.0+(AVLv_q/AVLU0))*AVLv_bord_Vbi_ci))));
#endif
AVLS_avl=(AVLa*(AVLU0+((1.0+(AVLv_q/AVLU0))*(AVLv_bord-AVLU0))));
}
else
{
{
double m00_exp(d00_exp0,((-AVLv_q)/AVLv_bord))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
AVLS_avl_Vtnode_GND=((((favl_t_Vtnode_GND*AVLv_bord)+(favl_t*AVLv_bord_Vtnode_GND))*d00_exp0)+((favl_t*AVLv_bord)*(((-AVLv_q_Vtnode_GND)*AVLv_bord-(-AVLv_q)*AVLv_bord_Vtnode_GND)/AVLv_bord/AVLv_bord)*d10_exp0));
AVLS_avl_Vbi_ei=((favl_t*AVLv_bord)*((-AVLv_q_Vbi_ei)/AVLv_bord)*d10_exp0);
AVLS_avl_Vbi_ci=(((favl_t*AVLv_bord_Vbi_ci)*d00_exp0)+((favl_t*AVLv_bord)*(((-AVLv_q_Vbi_ci)*AVLv_bord-(-AVLv_q)*AVLv_bord_Vbi_ci)/AVLv_bord/AVLv_bord)*d10_exp0));
#endif
AVLS_avl=((favl_t*AVLv_bord)*d00_exp0);
}
}
#if defined(_DERIVATE)
iavl_Vci_ei=itf_Vci_ei*AVLS_avl;
iavl_Vtnode_GND=((itf_Vtnode_GND*AVLS_avl)+(itf*AVLS_avl_Vtnode_GND));
iavl_Vbi_ei=((itf_Vbi_ei*AVLS_avl)+(itf*AVLS_avl_Vbi_ei));
iavl_Vbi_ci=((itf_Vbi_ci*AVLS_avl)+(itf*AVLS_avl_Vbi_ci));
#endif
iavl=(itf*AVLS_avl);
}
else
{
#if defined(_DERIVATE)
iavl_Vci_ei=0.0;
iavl_Vtnode_GND=0.0;
iavl_Vbi_ei=0.0;
iavl_Vbi_ci=0.0;
#endif
iavl=0.0;
}
if
((rbi0_t>0.0))
{
#if defined(_DERIVATE)
f_QR_Vtnode_GND=((1+fdqr0)*qp0_t_Vtnode_GND);
#endif
f_QR=((1+fdqr0)*qp0_t);
#if defined(_DERIVATE)
Qz_Vtnode_GND=(Qjei_Vtnode_GND+Qf_Vtnode_GND);
Qz_Vbi_ei=(Qjei_Vbi_ei+Qf_Vbi_ei);
Qz_Vci_ei=Qf_Vci_ei;
Qz_Vbi_ci=Qf_Vbi_ci;
#endif
Qz=(Qjei+Qf);
if
((Qf>0.0))
{
#if defined(_DERIVATE)
Q_fi_Vci_ei=Qf_Vci_ei*fqi;
Q_fi_Vtnode_GND=Qf_Vtnode_GND*fqi;
Q_fi_Vbi_ci=Qf_Vbi_ci*fqi;
Q_fi_Vbi_ei=Qf_Vbi_ei*fqi;
#endif
Q_fi=(Qf*fqi);
#if defined(_DERIVATE)
f_p_Vtnode_GND=(((Qjei_Vtnode_GND+Q_fi_Vtnode_GND)*Qz-(Qjei+Q_fi)*Qz_Vtnode_GND)/Qz/Qz);
f_p_Vbi_ei=(((Qjei_Vbi_ei+Q_fi_Vbi_ei)*Qz-(Qjei+Q_fi)*Qz_Vbi_ei)/Qz/Qz);
f_p_Vci_ei=((Q_fi_Vci_ei*Qz-(Qjei+Q_fi)*Qz_Vci_ei)/Qz/Qz);
f_p_Vbi_ci=((Q_fi_Vbi_ci*Qz-(Qjei+Q_fi)*Qz_Vbi_ci)/Qz/Qz);
#endif
f_p=((Qjei+Q_fi)/Qz);
}
else
{
#if defined(_DERIVATE)
f_p_Vtnode_GND=0.0;
f_p_Vbi_ei=0.0;
f_p_Vci_ei=0.0;
f_p_Vbi_ci=0.0;
#endif
f_p=1.0;
}
#if defined(_DERIVATE)
rbi_Vtnode_GND=((((rbi0_t_Vtnode_GND*f_QR)+(rbi0_t*f_QR_Vtnode_GND))*(f_QR+Qz)-(rbi0_t*f_QR)*(f_QR_Vtnode_GND+Qz_Vtnode_GND))/(f_QR+Qz)/(f_QR+Qz));
rbi_Vbi_ei=(-(rbi0_t*f_QR)*Qz_Vbi_ei/(f_QR+Qz)/(f_QR+Qz));
rbi_Vci_ei=(-(rbi0_t*f_QR)*Qz_Vci_ei/(f_QR+Qz)/(f_QR+Qz));
rbi_Vbi_ci=(-(rbi0_t*f_QR)*Qz_Vbi_ci/(f_QR+Qz)/(f_QR+Qz));
#endif
rbi=((rbi0_t*f_QR)/(f_QR+Qz));
#if defined(_DERIVATE)
I_BI_Vtnode_GND=ibei_Vtnode_GND;
I_BI_Vbi_ei=ibei_Vbi_ei;
#endif
I_BI=ibei;
if
((I_BI>=1.0e-20))
{
#if defined(_DERIVATE)
ETA_Vtnode_GND=((((rbi_Vtnode_GND*I_BI)+(rbi*I_BI_Vtnode_GND))*fgeo*VT-((rbi*I_BI)*fgeo)*VT_Vtnode_GND)/VT/VT);
ETA_Vbi_ei=(((rbi_Vbi_ei*I_BI)+(rbi*I_BI_Vbi_ei))*fgeo/VT);
ETA_Vci_ei=(rbi_Vci_ei*I_BI*fgeo/VT);
ETA_Vbi_ci=(rbi_Vbi_ci*I_BI*fgeo/VT);
#endif
ETA=(((rbi*I_BI)*fgeo)/VT);
if
((ETA>=1.0e-3))
{
if
((ETA<0.02))
{
#if defined(_DERIVATE)
rbi_Vtnode_GND=((rbi_Vtnode_GND*(1.0-(0.5*ETA)))+(rbi*(-(0.5*ETA_Vtnode_GND))));
rbi_Vbi_ei=((rbi_Vbi_ei*(1.0-(0.5*ETA)))+(rbi*(-(0.5*ETA_Vbi_ei))));
rbi_Vci_ei=((rbi_Vci_ei*(1.0-(0.5*ETA)))+(rbi*(-(0.5*ETA_Vci_ei))));
rbi_Vbi_ci=((rbi_Vbi_ci*(1.0-(0.5*ETA)))+(rbi*(-(0.5*ETA_Vbi_ci))));
#endif
rbi=(rbi*(1.0-(0.5*ETA)));
}
else
{
{
double m00_logE(d00_logE0,(1.0+ETA))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+ETA))
#endif
#if defined(_DERIVATE)
rbi_Vtnode_GND=((((rbi_Vtnode_GND*d00_logE0)+(rbi*ETA_Vtnode_GND*d10_logE0))*ETA-(rbi*d00_logE0)*ETA_Vtnode_GND)/ETA/ETA);
rbi_Vbi_ei=((((rbi_Vbi_ei*d00_logE0)+(rbi*ETA_Vbi_ei*d10_logE0))*ETA-(rbi*d00_logE0)*ETA_Vbi_ei)/ETA/ETA);
rbi_Vci_ei=((((rbi_Vci_ei*d00_logE0)+(rbi*ETA_Vci_ei*d10_logE0))*ETA-(rbi*d00_logE0)*ETA_Vci_ei)/ETA/ETA);
rbi_Vbi_ci=((((rbi_Vbi_ci*d00_logE0)+(rbi*ETA_Vbi_ci*d10_logE0))*ETA-(rbi*d00_logE0)*ETA_Vbi_ci)/ETA/ETA);
#endif
rbi=((rbi*d00_logE0)/ETA);
}
}
}
}
#if defined(_DERIVATE)
rbi_Vtnode_GND=((rbi_Vtnode_GND*f_p)+(rbi*f_p_Vtnode_GND));
rbi_Vbi_ei=((rbi_Vbi_ei*f_p)+(rbi*f_p_Vbi_ei));
rbi_Vci_ei=((rbi_Vci_ei*f_p)+(rbi*f_p_Vci_ei));
rbi_Vbi_ci=((rbi_Vbi_ci*f_p)+(rbi*f_p_Vbi_ci));
#endif
rbi=(rbi*f_p);
}
else
{
#if defined(_DERIVATE)
rbi_Vtnode_GND=0.0;
rbi_Vbi_ei=0.0;
rbi_Vci_ei=0.0;
rbi_Vbi_ci=0.0;
#endif
rbi=0.0;
}
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=(1/(mbep*VT));
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-BP(bp,ei)*(mbep*VT_Vtnode_GND)/(mbep*VT)/(mbep*VT));
#endif
DIOY=(BP(bp,ei)/(mbep*VT));
if
((ibeps>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=DIOY_Vbp_ei;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=((le_Vbp_ei*d00_limexp0)+(le*DIOY_Vbp_ei*d10_limexp0));
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ibep_Vtnode_GND=((ibeps_t_Vtnode_GND*(le-1.0))+(ibeps_t*le_Vtnode_GND));
ibep_Vbp_ei=(ibeps_t*le_Vbp_ei);
ibep_Vbi_ci=(ibeps_t*le_Vbi_ci);
ibep_Vbi_ei=(ibeps_t*le_Vbi_ei);
#endif
ibep=(ibeps_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ibep_Vtnode_GND=(-ibeps_t_Vtnode_GND);
ibep_Vbp_ei=0.0;
ibep_Vbi_ci=0.0;
ibep_Vbi_ei=0.0;
#endif
ibep=(-ibeps_t);
}
}
else
{
#if defined(_DERIVATE)
ibep_Vtnode_GND=0.0;
ibep_Vbp_ei=0.0;
ibep_Vbi_ci=0.0;
ibep_Vbi_ei=0.0;
#endif
ibep=0.0;
}
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=(1/(mrep*VT));
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-BP(bp,ei)*(mrep*VT_Vtnode_GND)/(mrep*VT)/(mrep*VT));
#endif
DIOY=(BP(bp,ei)/(mrep*VT));
if
((ireps>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=DIOY_Vbp_ei;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=((le_Vbp_ei*d00_limexp0)+(le*DIOY_Vbp_ei*d10_limexp0));
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
irep_Vtnode_GND=((ireps_t_Vtnode_GND*(le-1.0))+(ireps_t*le_Vtnode_GND));
irep_Vbp_ei=(ireps_t*le_Vbp_ei);
irep_Vbi_ci=(ireps_t*le_Vbi_ci);
irep_Vbi_ei=(ireps_t*le_Vbi_ei);
#endif
irep=(ireps_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
irep_Vtnode_GND=(-ireps_t_Vtnode_GND);
irep_Vbp_ei=0.0;
irep_Vbi_ci=0.0;
irep_Vbi_ei=0.0;
#endif
irep=(-ireps_t);
}
}
else
{
#if defined(_DERIVATE)
irep_Vtnode_GND=0.0;
irep_Vbp_ei=0.0;
irep_Vbi_ci=0.0;
irep_Vbi_ei=0.0;
#endif
irep=0.0;
}
if
((cjep0_t>0.0))
{
{
double m00_logE(d00_logE0,aljep_t)
double m00_exp(d00_exp1,((-d00_logE0)/zep))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,aljep_t)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=((vdep_t_Vtnode_GND*(1.0-d00_exp1))+(vdep_t*(-((-aljep_t_Vtnode_GND*d10_logE0)/zep)*d10_exp1)));
#endif
DFV_f=(vdep_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=((aljep_t_Vtnode_GND*cjep0_t)+(aljep_t*cjep0_t_Vtnode_GND));
#endif
DFC_max=(aljep_t*cjep0_t);
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=((-1.0)/VT);
DFv_e_Vbi_ci=0.0;
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(bp,ei))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(bp,ei))/VT);
if
((DFv_e<80.0))
{
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=0.0;
DFe_Vbp_ci=0.0;
DFe_Vbp_ei=DFv_e_Vbp_ei*d10_exp0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=(-(VT*DFe_Vbp_ei*d10_logE0));
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=((DFe_Vbp_ei*(1.0+DFe)-DFe*DFe_Vbp_ei)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
}
else
{
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=1.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(bp,ei);
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
}
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdep_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdep_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=0.0;
DFb_Vbp_ci=0.0;
DFb_Vbp_ei=(-(DFv_j_Vbp_ei/vdep_t))*d10_logE0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vdep_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdep_t-DFv_j*vdep_t_Vtnode_GND)/vdep_t/vdep_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdep_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
{
double m00_exp(d00_exp0,((-zep)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=0.0;
DFC_j1_Vbp_ci=0.0;
DFC_j1_Vbp_ei=(((cjep0_t*((-zep)*DFb_Vbp_ei)*d10_exp0)*DFdvj_dv)+((cjep0_t*d00_exp0)*DFdvj_dv_Vbp_ei));
DFC_j1_Vbi_ci=(((cjep0_t*((-zep)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjep0_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjep0_t_Vtnode_GND*d00_exp0)+(cjep0_t*((-zep)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjep0_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjep0_t*((-zep)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjep0_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjep0_t*d00_exp0)*DFdvj_dv);
}
#if defined(_DERIVATE)
Cjep_Vbp_ei=(DFC_j1_Vbp_ei+(DFC_max*(-DFdvj_dv_Vbp_ei)));
Cjep_Vbi_ci=(DFC_j1_Vbi_ci+(DFC_max*(-DFdvj_dv_Vbi_ci)));
Cjep_Vtnode_GND=(DFC_j1_Vtnode_GND+((DFC_max_Vtnode_GND*(1.0-DFdvj_dv))+(DFC_max*(-DFdvj_dv_Vtnode_GND))));
Cjep_Vbi_ei=(DFC_j1_Vbi_ei+(DFC_max*(-DFdvj_dv_Vbi_ei)));
#endif
Cjep=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DFb*(1.0-zep)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=0.0;
DFQ_j_Vbp_ci=0.0;
DFQ_j_Vbp_ei=(((cjep0_t*vdep_t)*(-DFb_Vbp_ei*(1.0-zep)*d10_exp0))/(1.0-zep));
DFQ_j_Vbi_ci=(((cjep0_t*vdep_t)*(-DFb_Vbi_ci*(1.0-zep)*d10_exp0))/(1.0-zep));
DFQ_j_Vtnode_GND=(((((cjep0_t_Vtnode_GND*vdep_t)+(cjep0_t*vdep_t_Vtnode_GND))*(1.0-d00_exp0))+((cjep0_t*vdep_t)*(-DFb_Vtnode_GND*(1.0-zep)*d10_exp0)))/(1.0-zep));
DFQ_j_Vbi_ei=(((cjep0_t*vdep_t)*(-DFb_Vbi_ei*(1.0-zep)*d10_exp0))/(1.0-zep));
#endif
DFQ_j=(((cjep0_t*vdep_t)*(1.0-d00_exp0))/(1.0-zep));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjep_Vbp_ei=(DFQ_j_Vbp_ei+(DFC_max*(1.0-DFv_j_Vbp_ei)));
Qjep_Vbi_ci=(DFQ_j_Vbi_ci+(DFC_max*(-DFv_j_Vbi_ci)));
Qjep_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(bp,ei)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
Qjep_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(-DFv_j_Vbi_ei)));
#endif
Qjep=(DFQ_j+(DFC_max*(BP(bp,ei)-DFv_j)));
#endif
}
else
{
#if defined(_DERIVATE)
Cjep_Vbp_ei=0.0;
Cjep_Vbi_ci=0.0;
Cjep_Vtnode_GND=0.0;
Cjep_Vbi_ei=0.0;
#endif
Cjep=0.0;
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjep_Vbp_ei=0.0;
Qjep_Vbi_ci=0.0;
Qjep_Vtnode_GND=0.0;
Qjep_Vbi_ei=0.0;
#endif
Qjep=0.0;
#endif
}
if
((BP(bp,ei)<0.0))
{
if
((cjep0>0.0))
{
#if defined(_DERIVATE)
a_eg_Vtnode_GND=(-(vgb-(2.55e-4*Tnom))*(-(2.55e-4*Tdev_Vtnode_GND))/(vgb-(2.55e-4*Tdev))/(vgb-(2.55e-4*Tdev)));
#endif
a_eg=((vgb-(2.55e-4*Tnom))/(vgb-(2.55e-4*Tdev)));
{
double m00_sqrt(d00_sqrt0,a_eg)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,a_eg)
#endif
#if defined(_DERIVATE)
ab_Vtnode_GND=((((((((cjep0_t_Vtnode_GND/cjep0)*d00_sqrt0)+((cjep0_t/cjep0)*a_eg_Vtnode_GND*d10_sqrt0))*vdep_t)+(((cjep0_t/cjep0)*d00_sqrt0)*vdep_t_Vtnode_GND))*vdep_t)+((((cjep0_t/cjep0)*d00_sqrt0)*vdep_t)*vdep_t_Vtnode_GND))/(vdep*vdep));
#endif
ab=(((((cjep0_t/cjep0)*d00_sqrt0)*vdep_t)*vdep_t)/(vdep*vdep));
}
#if defined(_DERIVATE)
ibets_t_Vtnode_GND=(ibets*ab_Vtnode_GND);
#endif
ibets_t=(ibets*ab);
}
else
{
#if defined(_DERIVATE)
ibets_t_Vtnode_GND=0.0;
#endif
ibets_t=ibets;
}
#if defined(_DERIVATE)
vve_Vbp_ei=((-1.0)/vdep_t);
vve_Vtnode_GND=(-(-BP(bp,ei))*vdep_t_Vtnode_GND/vdep_t/vdep_t);
#endif
vve=((-BP(bp,ei))/vdep_t);
#if defined(_DERIVATE)
cce_Vbp_ei=(Cjep_Vbp_ei/cjep0_t);
cce_Vbi_ci=(Cjep_Vbi_ci/cjep0_t);
cce_Vtnode_GND=((Cjep_Vtnode_GND*cjep0_t-Cjep*cjep0_t_Vtnode_GND)/cjep0_t/cjep0_t);
cce_Vbi_ei=(Cjep_Vbi_ei/cjep0_t);
#endif
cce=(Cjep/cjep0_t);
zex=(1.0/zep);
{
double m00_logE(d00_logE0,cce)
double m00_exp(d00_exp1,((1-zex)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,cce)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
pocce_Vbp_ei=((1-zex)*cce_Vbp_ei*d10_logE0)*d10_exp1;
pocce_Vbi_ci=((1-zex)*cce_Vbi_ci*d10_logE0)*d10_exp1;
pocce_Vtnode_GND=((1-zex)*cce_Vtnode_GND*d10_logE0)*d10_exp1;
pocce_Vbi_ei=((1-zex)*cce_Vbi_ei*d10_logE0)*d10_exp1;
#endif
pocce=d00_exp1;
}
#if defined(_DERIVATE)
czz_Vtnode_GND=((((ibets_t_Vtnode_GND*vve)+(ibets_t*vve_Vtnode_GND))*pocce)+((ibets_t*vve)*pocce_Vtnode_GND));
czz_Vbp_ei=(((ibets_t*vve_Vbp_ei)*pocce)+((ibets_t*vve)*pocce_Vbp_ei));
czz_Vbi_ci=((ibets_t*vve)*pocce_Vbi_ci);
czz_Vbi_ei=((ibets_t*vve)*pocce_Vbi_ei);
#endif
czz=((ibets_t*vve)*pocce);
{
double m00_exp(d00_exp0,((-abet)/pocce))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibet_Vtnode_GND=((czz_Vtnode_GND*d00_exp0)+(czz*(-(-abet)*pocce_Vtnode_GND/pocce/pocce)*d10_exp0));
ibet_Vbp_ei=((czz_Vbp_ei*d00_exp0)+(czz*(-(-abet)*pocce_Vbp_ei/pocce/pocce)*d10_exp0));
ibet_Vbi_ci=((czz_Vbi_ci*d00_exp0)+(czz*(-(-abet)*pocce_Vbi_ci/pocce/pocce)*d10_exp0));
ibet_Vbi_ei=((czz_Vbi_ei*d00_exp0)+(czz*(-(-abet)*pocce_Vbi_ei/pocce/pocce)*d10_exp0));
#endif
ibet=(czz*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
ibet_Vtnode_GND=0.0;
ibet_Vbp_ei=0.0;
ibet_Vbi_ci=0.0;
ibet_Vbi_ei=0.0;
#endif
ibet=0.0;
}
#ifdef _DYNAMIC
if
((vptcx_t<1.0e2))
{
#ifdef _DYNAMIC
if
((cjcx02_t>0.0))
{
#if defined(_DYNAMIC)
Dz_r=(zcx/4.0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_p_Vtnode_GND=(vptcx_t_Vtnode_GND-vdcx_t_Vtnode_GND);
#endif
Dv_p=(vptcx_t-vdcx_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
DV_f_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DV_f=(vdcx_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_max_Vtnode_GND=(2.4*cjcx02_t_Vtnode_GND);
#endif
DC_max=(2.4*cjcx02_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vptcx_t/vdcx_t))
double m00_exp(d00_exp1,(d00_logE0*(Dz_r-zcx)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vptcx_t/vdcx_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DC_c_Vtnode_GND=((cjcx02_t_Vtnode_GND*d00_exp1)+(cjcx02_t*((vptcx_t_Vtnode_GND*vdcx_t-vptcx_t*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t)*d10_logE0*(Dz_r-zcx)*d10_exp1));
#endif
DC_c=(cjcx02_t*d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=VT_Vtnode_GND;
#endif
Da=VT;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_e_Vsi_ci=0.0;
Dv_e_Vb_ci=0.0;
Dv_e_Vbp_ci=((-1.0)/Da);
Dv_e_Vtnode_GND=((DV_f_Vtnode_GND*Da-(DV_f-BP(bp,ci))*Da_Vtnode_GND)/Da/Da);
Dv_e_Vbi_ci=0.0;
#endif
Dv_e=((DV_f-BP(bp,ci))/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=0.0;
De_Vbp_ci=Dv_e_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_e_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_e_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_1_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_1_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_1=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=0.0;
Dv_j1_Vbp_ci=(-(Da*De_Vbp_ci*d10_logE0));
Dv_j1_Vtnode_GND=(DV_f_Vtnode_GND-((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j1_Vbi_ci=(-(Da*De_Vbi_ci*d10_logE0));
#endif
Dv_j1=(DV_f-(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=0.0;
De_1_Vtnode_GND=0.0;
De_1_Vbi_ci=0.0;
#endif
De_1=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=0.0;
Dv_j1_Vbp_ci=1.0;
Dv_j1_Vtnode_GND=0.0;
Dv_j1_Vbi_ci=0.0;
#endif
Dv_j1=BP(bp,ci);
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=((0.1*Dv_p_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
Da=((0.1*Dv_p)+(4.0*VT));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_r_Vsi_ci=0.0;
Dv_r_Vb_ci=0.0;
Dv_r_Vbp_ci=(Dv_j1_Vbp_ci/Da);
Dv_r_Vtnode_GND=(((Dv_p_Vtnode_GND+Dv_j1_Vtnode_GND)*Da-(Dv_p+Dv_j1)*Da_Vtnode_GND)/Da/Da);
Dv_r_Vbi_ci=(Dv_j1_Vbi_ci/Da);
#endif
Dv_r=((Dv_p+Dv_j1)/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_r<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_r)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=0.0;
De_Vbp_ci=Dv_r_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_r_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_r_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_2_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_2_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_2=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=0.0;
Dv_j2_Vbp_ci=(Da*De_Vbp_ci*d10_logE0);
Dv_j2_Vtnode_GND=((-Dv_p_Vtnode_GND)+((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j2_Vbi_ci=(Da*De_Vbi_ci*d10_logE0);
#endif
Dv_j2=((-Dv_p)+(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=0.0;
De_2_Vtnode_GND=0.0;
De_2_Vbi_ci=0.0;
#endif
De_2=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=0.0;
Dv_j2_Vbp_ci=Dv_j1_Vbp_ci;
Dv_j2_Vtnode_GND=Dv_j1_Vtnode_GND;
Dv_j2_Vbi_ci=Dv_j1_Vbi_ci;
#endif
Dv_j2=Dv_j1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j4_Vsi_ci=0.0;
Dv_j4_Vb_ci=0.0;
Dv_j4_Vbp_ci=(1.0-Dv_j1_Vbp_ci);
Dv_j4_Vbi_ci=(-Dv_j1_Vbi_ci);
Dv_j4_Vtnode_GND=(-Dv_j1_Vtnode_GND);
#endif
Dv_j4=(BP(bp,ci)-Dv_j1);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j1/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j1/vdcx_t)))
#endif
#if defined(_DERIVATE)
DCln1_Vsi_ci=0.0;
DCln1_Vb_ci=0.0;
DCln1_Vbp_ci=(-(Dv_j1_Vbp_ci/vdcx_t))*d10_logE0;
DCln1_Vtnode_GND=(-((Dv_j1_Vtnode_GND*vdcx_t-Dv_j1*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DCln1_Vbi_ci=(-(Dv_j1_Vbi_ci/vdcx_t))*d10_logE0;
#endif
DCln1=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j2/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j2/vdcx_t)))
#endif
#if defined(_DERIVATE)
DCln2_Vsi_ci=0.0;
DCln2_Vb_ci=0.0;
DCln2_Vbp_ci=(-(Dv_j2_Vbp_ci/vdcx_t))*d10_logE0;
DCln2_Vtnode_GND=(-((Dv_j2_Vtnode_GND*vdcx_t-Dv_j2*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DCln2_Vbi_ci=(-(Dv_j2_Vbi_ci/vdcx_t))*d10_logE0;
#endif
DCln2=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
Dz1=(1.0-zcx);
#endif
#if defined(_DYNAMIC)
Dzr1=(1.0-Dz_r);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*(-zcx)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j1_Vsi_ci=0.0;
DC_j1_Vb_ci=0.0;
DC_j1_Vbp_ci=(((((cjcx02_t*DCln2_Vbp_ci*(-zcx)*d10_exp0)*De_1)+((cjcx02_t*d00_exp0)*De_1_Vbp_ci))*De_2)+(((cjcx02_t*d00_exp0)*De_1)*De_2_Vbp_ci));
DC_j1_Vtnode_GND=((((((cjcx02_t_Vtnode_GND*d00_exp0)+(cjcx02_t*DCln2_Vtnode_GND*(-zcx)*d10_exp0))*De_1)+((cjcx02_t*d00_exp0)*De_1_Vtnode_GND))*De_2)+(((cjcx02_t*d00_exp0)*De_1)*De_2_Vtnode_GND));
DC_j1_Vbi_ci=(((((cjcx02_t*DCln2_Vbi_ci*(-zcx)*d10_exp0)*De_1)+((cjcx02_t*d00_exp0)*De_1_Vbi_ci))*De_2)+(((cjcx02_t*d00_exp0)*De_1)*De_2_Vbi_ci));
#endif
DC_j1=(((cjcx02_t*d00_exp0)*De_1)*De_2);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*(-Dz_r)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j2_Vsi_ci=0.0;
DC_j2_Vb_ci=0.0;
DC_j2_Vbp_ci=(((DC_c*DCln1_Vbp_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbp_ci)));
DC_j2_Vtnode_GND=((((DC_c_Vtnode_GND*d00_exp0)+(DC_c*DCln1_Vtnode_GND*(-Dz_r)*d10_exp0))*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vtnode_GND)));
DC_j2_Vbi_ci=(((DC_c*DCln1_Vbi_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbi_ci)));
#endif
DC_j2=((DC_c*d00_exp0)*(1.0-De_2));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_j3_Vsi_ci=0.0;
DC_j3_Vb_ci=0.0;
DC_j3_Vbp_ci=(DC_max*(-De_1_Vbp_ci));
DC_j3_Vtnode_GND=((DC_max_Vtnode_GND*(1.0-De_1))+(DC_max*(-De_1_Vtnode_GND)));
DC_j3_Vbi_ci=(DC_max*(-De_1_Vbi_ci));
#endif
DC_j3=(DC_max*(1.0-De_1));
#endif
#if defined(_DYNAMIC)
cjcx0_t_ii=((DC_j1+DC_j2)+DC_j3);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dz1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j1_Vsi_ci=0.0;
DQ_j1_Vb_ci=0.0;
DQ_j1_Vbp_ci=((cjcx02_t*(-DCln2_Vbp_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vtnode_GND=(((cjcx02_t_Vtnode_GND*(1.0-d00_exp0))+(cjcx02_t*(-DCln2_Vtnode_GND*Dz1*d10_exp0)))/Dz1);
DQ_j1_Vbi_ci=((cjcx02_t*(-DCln2_Vbi_ci*Dz1*d10_exp0))/Dz1);
#endif
DQ_j1=((cjcx02_t*(1.0-d00_exp0))/Dz1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j2_Vsi_ci=0.0;
DQ_j2_Vb_ci=0.0;
DQ_j2_Vbp_ci=((DC_c*(-DCln1_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln1_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j2_Vbi_ci=((DC_c*(-DCln1_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j2=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j3_Vsi_ci=0.0;
DQ_j3_Vb_ci=0.0;
DQ_j3_Vbp_ci=((DC_c*(-DCln2_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln2_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j3_Vbi_ci=((DC_c*(-DCln2_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j3=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_ii_Vbp_ei=0.0;
qjcx0_t_ii_Vbi_ei=0.0;
qjcx0_t_ii_Vbp_ci=(((DQ_j1_Vbp_ci+DQ_j2_Vbp_ci)-DQ_j3_Vbp_ci)*vdcx_t+(DC_max*Dv_j4_Vbp_ci));
qjcx0_t_ii_Vtnode_GND=(((((DQ_j1_Vtnode_GND+DQ_j2_Vtnode_GND)-DQ_j3_Vtnode_GND)*vdcx_t)+(((DQ_j1+DQ_j2)-DQ_j3)*vdcx_t_Vtnode_GND))+((DC_max_Vtnode_GND*Dv_j4)+(DC_max*Dv_j4_Vtnode_GND)));
qjcx0_t_ii_Vbi_ci=(((DQ_j1_Vbi_ci+DQ_j2_Vbi_ci)-DQ_j3_Vbi_ci)*vdcx_t+(DC_max*Dv_j4_Vbi_ci));
#endif
qjcx0_t_ii=((((DQ_j1+DQ_j2)-DQ_j3)*vdcx_t)+(DC_max*Dv_j4));
#endif
}
else
{
#if defined(_DYNAMIC)
cjcx0_t_ii=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_ii_Vbp_ei=0.0;
qjcx0_t_ii_Vbi_ei=0.0;
qjcx0_t_ii_Vbp_ci=0.0;
qjcx0_t_ii_Vtnode_GND=0.0;
qjcx0_t_ii_Vbi_ci=0.0;
#endif
qjcx0_t_ii=0.0;
#endif
}
#endif /* if (...) */
}
else
{
#ifdef _DYNAMIC
if
((cjcx02_t>0.0))
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DFV_f=(vdcx_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=(2.4*cjcx02_t_Vtnode_GND);
#endif
DFC_max=(2.4*cjcx02_t);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=((-1.0)/VT);
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=0.0;
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(bp,ci))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(bp,ci))/VT);
#endif
#ifdef _DYNAMIC
if
((DFv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=0.0;
DFe_Vbp_ci=DFv_e_Vbp_ci*d10_exp0;
DFe_Vbp_ei=DFv_e_Vbp_ei*d10_exp0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=(-(VT*DFe_Vbp_ci*d10_logE0));
DFv_j_Vbp_ei=(-(VT*DFe_Vbp_ei*d10_logE0));
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=((DFe_Vbp_ci*(1.0+DFe)-DFe*DFe_Vbp_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbp_ei=((DFe_Vbp_ei*(1.0+DFe)-DFe*DFe_Vbp_ei)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=1.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(bp,ci);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdcx_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=0.0;
DFb_Vbp_ci=(-(DFv_j_Vbp_ci/vdcx_t))*d10_logE0;
DFb_Vbp_ei=(-(DFv_j_Vbp_ei/vdcx_t))*d10_logE0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vdcx_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdcx_t-DFv_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdcx_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((-zcx)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=0.0;
DFC_j1_Vbp_ci=(((cjcx02_t*((-zcx)*DFb_Vbp_ci)*d10_exp0)*DFdvj_dv)+((cjcx02_t*d00_exp0)*DFdvj_dv_Vbp_ci));
DFC_j1_Vbp_ei=(((cjcx02_t*((-zcx)*DFb_Vbp_ei)*d10_exp0)*DFdvj_dv)+((cjcx02_t*d00_exp0)*DFdvj_dv_Vbp_ei));
DFC_j1_Vbi_ci=(((cjcx02_t*((-zcx)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjcx02_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjcx02_t_Vtnode_GND*d00_exp0)+(cjcx02_t*((-zcx)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjcx02_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjcx02_t*((-zcx)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjcx02_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjcx02_t*d00_exp0)*DFdvj_dv);
}
#endif
#if defined(_DYNAMIC)
cjcx0_t_ii=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DFb*(1.0-zcx)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=0.0;
DFQ_j_Vbp_ci=(((cjcx02_t*vdcx_t)*(-DFb_Vbp_ci*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vbp_ei=(((cjcx02_t*vdcx_t)*(-DFb_Vbp_ei*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vbi_ci=(((cjcx02_t*vdcx_t)*(-DFb_Vbi_ci*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vtnode_GND=(((((cjcx02_t_Vtnode_GND*vdcx_t)+(cjcx02_t*vdcx_t_Vtnode_GND))*(1.0-d00_exp0))+((cjcx02_t*vdcx_t)*(-DFb_Vtnode_GND*(1.0-zcx)*d10_exp0)))/(1.0-zcx));
DFQ_j_Vbi_ei=(((cjcx02_t*vdcx_t)*(-DFb_Vbi_ei*(1.0-zcx)*d10_exp0))/(1.0-zcx));
#endif
DFQ_j=(((cjcx02_t*vdcx_t)*(1.0-d00_exp0))/(1.0-zcx));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_ii_Vbp_ei=(DFQ_j_Vbp_ei+(DFC_max*(-DFv_j_Vbp_ei)));
qjcx0_t_ii_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(-DFv_j_Vbi_ei)));
qjcx0_t_ii_Vbp_ci=(DFQ_j_Vbp_ci+(DFC_max*(1.0-DFv_j_Vbp_ci)));
qjcx0_t_ii_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(bp,ci)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
qjcx0_t_ii_Vbi_ci=(DFQ_j_Vbi_ci+(DFC_max*(-DFv_j_Vbi_ci)));
#endif
qjcx0_t_ii=(DFQ_j+(DFC_max*(BP(bp,ci)-DFv_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
cjcx0_t_ii=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_ii_Vbp_ei=0.0;
qjcx0_t_ii_Vbi_ei=0.0;
qjcx0_t_ii_Vbp_ci=0.0;
qjcx0_t_ii_Vtnode_GND=0.0;
qjcx0_t_ii_Vbi_ci=0.0;
#endif
qjcx0_t_ii=0.0;
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=(1/(mbcx*VT));
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-BP(bp,ci)*(mbcx*VT_Vtnode_GND)/(mbcx*VT)/(mbcx*VT));
#endif
DIOY=(BP(bp,ci)/(mbcx*VT));
if
((ibcxs>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=DIOY_Vbp_ci;
le_Vbp_ei=DIOY_Vbp_ei;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=((le_Vbp_ci*d00_limexp0)+(le*DIOY_Vbp_ci*d10_limexp0));
le_Vbp_ei=((le_Vbp_ei*d00_limexp0)+(le*DIOY_Vbp_ei*d10_limexp0));
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ijbcx_Vtnode_GND=((ibcxs_t_Vtnode_GND*(le-1.0))+(ibcxs_t*le_Vtnode_GND));
ijbcx_Vbp_ci=(ibcxs_t*le_Vbp_ci);
ijbcx_Vbp_ei=(ibcxs_t*le_Vbp_ei);
ijbcx_Vbi_ci=(ibcxs_t*le_Vbi_ci);
ijbcx_Vbi_ei=(ibcxs_t*le_Vbi_ei);
#endif
ijbcx=(ibcxs_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ijbcx_Vtnode_GND=(-ibcxs_t_Vtnode_GND);
ijbcx_Vbp_ci=0.0;
ijbcx_Vbp_ei=0.0;
ijbcx_Vbi_ci=0.0;
ijbcx_Vbi_ei=0.0;
#endif
ijbcx=(-ibcxs_t);
}
}
else
{
#if defined(_DERIVATE)
ijbcx_Vtnode_GND=0.0;
ijbcx_Vbp_ci=0.0;
ijbcx_Vbp_ei=0.0;
ijbcx_Vbi_ci=0.0;
ijbcx_Vbi_ei=0.0;
#endif
ijbcx=0.0;
}
#ifdef _DYNAMIC
if
((vptcx_t<1.0e2))
{
#ifdef _DYNAMIC
if
((cjcx01_t>0.0))
{
#if defined(_DYNAMIC)
Dz_r=(zcx/4.0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_p_Vtnode_GND=(vptcx_t_Vtnode_GND-vdcx_t_Vtnode_GND);
#endif
Dv_p=(vptcx_t-vdcx_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
DV_f_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DV_f=(vdcx_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_max_Vtnode_GND=(2.4*cjcx01_t_Vtnode_GND);
#endif
DC_max=(2.4*cjcx01_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vptcx_t/vdcx_t))
double m00_exp(d00_exp1,(d00_logE0*(Dz_r-zcx)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vptcx_t/vdcx_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DC_c_Vtnode_GND=((cjcx01_t_Vtnode_GND*d00_exp1)+(cjcx01_t*((vptcx_t_Vtnode_GND*vdcx_t-vptcx_t*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t)*d10_logE0*(Dz_r-zcx)*d10_exp1));
#endif
DC_c=(cjcx01_t*d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=VT_Vtnode_GND;
#endif
Da=VT;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_e_Vsi_ci=0.0;
Dv_e_Vb_ci=((-1.0)/Da);
Dv_e_Vbp_ci=0.0;
Dv_e_Vtnode_GND=((DV_f_Vtnode_GND*Da-(DV_f-BP(b,ci))*Da_Vtnode_GND)/Da/Da);
Dv_e_Vbi_ci=0.0;
#endif
Dv_e=((DV_f-BP(b,ci))/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=Dv_e_Vb_ci*d10_exp0;
De_Vbp_ci=Dv_e_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_e_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_e_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=((De_Vb_ci*(1.0+De)-De*De_Vb_ci)/(1.0+De)/(1.0+De));
De_1_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_1_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_1_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_1=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=(-(Da*De_Vb_ci*d10_logE0));
Dv_j1_Vbp_ci=(-(Da*De_Vbp_ci*d10_logE0));
Dv_j1_Vtnode_GND=(DV_f_Vtnode_GND-((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j1_Vbi_ci=(-(Da*De_Vbi_ci*d10_logE0));
#endif
Dv_j1=(DV_f-(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=0.0;
De_1_Vtnode_GND=0.0;
De_1_Vbi_ci=0.0;
#endif
De_1=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=0.0;
Dv_j1_Vb_ci=1.0;
Dv_j1_Vbp_ci=0.0;
Dv_j1_Vtnode_GND=0.0;
Dv_j1_Vbi_ci=0.0;
#endif
Dv_j1=BP(b,ci);
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=((0.1*Dv_p_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
Da=((0.1*Dv_p)+(4.0*VT));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_r_Vsi_ci=0.0;
Dv_r_Vb_ci=(Dv_j1_Vb_ci/Da);
Dv_r_Vbp_ci=(Dv_j1_Vbp_ci/Da);
Dv_r_Vtnode_GND=(((Dv_p_Vtnode_GND+Dv_j1_Vtnode_GND)*Da-(Dv_p+Dv_j1)*Da_Vtnode_GND)/Da/Da);
Dv_r_Vbi_ci=(Dv_j1_Vbi_ci/Da);
#endif
Dv_r=((Dv_p+Dv_j1)/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_r<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_r)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=0.0;
De_Vb_ci=Dv_r_Vb_ci*d10_exp0;
De_Vbp_ci=Dv_r_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_r_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_r_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=((De_Vb_ci*(1.0+De)-De*De_Vb_ci)/(1.0+De)/(1.0+De));
De_2_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_2_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_2_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_2=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=(Da*De_Vb_ci*d10_logE0);
Dv_j2_Vbp_ci=(Da*De_Vbp_ci*d10_logE0);
Dv_j2_Vtnode_GND=((-Dv_p_Vtnode_GND)+((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j2_Vbi_ci=(Da*De_Vbi_ci*d10_logE0);
#endif
Dv_j2=((-Dv_p)+(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=0.0;
De_2_Vtnode_GND=0.0;
De_2_Vbi_ci=0.0;
#endif
De_2=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=0.0;
Dv_j2_Vb_ci=Dv_j1_Vb_ci;
Dv_j2_Vbp_ci=Dv_j1_Vbp_ci;
Dv_j2_Vtnode_GND=Dv_j1_Vtnode_GND;
Dv_j2_Vbi_ci=Dv_j1_Vbi_ci;
#endif
Dv_j2=Dv_j1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j4_Vsi_ci=0.0;
Dv_j4_Vb_ci=(1.0-Dv_j1_Vb_ci);
Dv_j4_Vbp_ci=(-Dv_j1_Vbp_ci);
Dv_j4_Vbi_ci=(-Dv_j1_Vbi_ci);
Dv_j4_Vtnode_GND=(-Dv_j1_Vtnode_GND);
#endif
Dv_j4=(BP(b,ci)-Dv_j1);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j1/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j1/vdcx_t)))
#endif
#if defined(_DERIVATE)
DCln1_Vsi_ci=0.0;
DCln1_Vb_ci=(-(Dv_j1_Vb_ci/vdcx_t))*d10_logE0;
DCln1_Vbp_ci=(-(Dv_j1_Vbp_ci/vdcx_t))*d10_logE0;
DCln1_Vtnode_GND=(-((Dv_j1_Vtnode_GND*vdcx_t-Dv_j1*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DCln1_Vbi_ci=(-(Dv_j1_Vbi_ci/vdcx_t))*d10_logE0;
#endif
DCln1=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j2/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j2/vdcx_t)))
#endif
#if defined(_DERIVATE)
DCln2_Vsi_ci=0.0;
DCln2_Vb_ci=(-(Dv_j2_Vb_ci/vdcx_t))*d10_logE0;
DCln2_Vbp_ci=(-(Dv_j2_Vbp_ci/vdcx_t))*d10_logE0;
DCln2_Vtnode_GND=(-((Dv_j2_Vtnode_GND*vdcx_t-Dv_j2*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DCln2_Vbi_ci=(-(Dv_j2_Vbi_ci/vdcx_t))*d10_logE0;
#endif
DCln2=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
Dz1=(1.0-zcx);
#endif
#if defined(_DYNAMIC)
Dzr1=(1.0-Dz_r);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*(-zcx)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j1_Vsi_ci=0.0;
DC_j1_Vb_ci=(((((cjcx01_t*DCln2_Vb_ci*(-zcx)*d10_exp0)*De_1)+((cjcx01_t*d00_exp0)*De_1_Vb_ci))*De_2)+(((cjcx01_t*d00_exp0)*De_1)*De_2_Vb_ci));
DC_j1_Vbp_ci=(((((cjcx01_t*DCln2_Vbp_ci*(-zcx)*d10_exp0)*De_1)+((cjcx01_t*d00_exp0)*De_1_Vbp_ci))*De_2)+(((cjcx01_t*d00_exp0)*De_1)*De_2_Vbp_ci));
DC_j1_Vtnode_GND=((((((cjcx01_t_Vtnode_GND*d00_exp0)+(cjcx01_t*DCln2_Vtnode_GND*(-zcx)*d10_exp0))*De_1)+((cjcx01_t*d00_exp0)*De_1_Vtnode_GND))*De_2)+(((cjcx01_t*d00_exp0)*De_1)*De_2_Vtnode_GND));
DC_j1_Vbi_ci=(((((cjcx01_t*DCln2_Vbi_ci*(-zcx)*d10_exp0)*De_1)+((cjcx01_t*d00_exp0)*De_1_Vbi_ci))*De_2)+(((cjcx01_t*d00_exp0)*De_1)*De_2_Vbi_ci));
#endif
DC_j1=(((cjcx01_t*d00_exp0)*De_1)*De_2);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*(-Dz_r)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j2_Vsi_ci=0.0;
DC_j2_Vb_ci=(((DC_c*DCln1_Vb_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vb_ci)));
DC_j2_Vbp_ci=(((DC_c*DCln1_Vbp_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbp_ci)));
DC_j2_Vtnode_GND=((((DC_c_Vtnode_GND*d00_exp0)+(DC_c*DCln1_Vtnode_GND*(-Dz_r)*d10_exp0))*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vtnode_GND)));
DC_j2_Vbi_ci=(((DC_c*DCln1_Vbi_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbi_ci)));
#endif
DC_j2=((DC_c*d00_exp0)*(1.0-De_2));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_j3_Vsi_ci=0.0;
DC_j3_Vb_ci=(DC_max*(-De_1_Vb_ci));
DC_j3_Vbp_ci=(DC_max*(-De_1_Vbp_ci));
DC_j3_Vtnode_GND=((DC_max_Vtnode_GND*(1.0-De_1))+(DC_max*(-De_1_Vtnode_GND)));
DC_j3_Vbi_ci=(DC_max*(-De_1_Vbi_ci));
#endif
DC_j3=(DC_max*(1.0-De_1));
#endif
#if defined(_DYNAMIC)
cjcx0_t_i=((DC_j1+DC_j2)+DC_j3);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dz1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j1_Vsi_ci=0.0;
DQ_j1_Vb_ci=((cjcx01_t*(-DCln2_Vb_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vbp_ci=((cjcx01_t*(-DCln2_Vbp_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vtnode_GND=(((cjcx01_t_Vtnode_GND*(1.0-d00_exp0))+(cjcx01_t*(-DCln2_Vtnode_GND*Dz1*d10_exp0)))/Dz1);
DQ_j1_Vbi_ci=((cjcx01_t*(-DCln2_Vbi_ci*Dz1*d10_exp0))/Dz1);
#endif
DQ_j1=((cjcx01_t*(1.0-d00_exp0))/Dz1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j2_Vsi_ci=0.0;
DQ_j2_Vb_ci=((DC_c*(-DCln1_Vb_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vbp_ci=((DC_c*(-DCln1_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln1_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j2_Vbi_ci=((DC_c*(-DCln1_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j2=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j3_Vsi_ci=0.0;
DQ_j3_Vb_ci=((DC_c*(-DCln2_Vb_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vbp_ci=((DC_c*(-DCln2_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln2_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j3_Vbi_ci=((DC_c*(-DCln2_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j3=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_i_Vbp_ei=0.0;
qjcx0_t_i_Vbi_ei=0.0;
qjcx0_t_i_Vb_ci=(((DQ_j1_Vb_ci+DQ_j2_Vb_ci)-DQ_j3_Vb_ci)*vdcx_t+(DC_max*Dv_j4_Vb_ci));
qjcx0_t_i_Vbp_ci=(((DQ_j1_Vbp_ci+DQ_j2_Vbp_ci)-DQ_j3_Vbp_ci)*vdcx_t+(DC_max*Dv_j4_Vbp_ci));
qjcx0_t_i_Vtnode_GND=(((((DQ_j1_Vtnode_GND+DQ_j2_Vtnode_GND)-DQ_j3_Vtnode_GND)*vdcx_t)+(((DQ_j1+DQ_j2)-DQ_j3)*vdcx_t_Vtnode_GND))+((DC_max_Vtnode_GND*Dv_j4)+(DC_max*Dv_j4_Vtnode_GND)));
qjcx0_t_i_Vbi_ci=(((DQ_j1_Vbi_ci+DQ_j2_Vbi_ci)-DQ_j3_Vbi_ci)*vdcx_t+(DC_max*Dv_j4_Vbi_ci));
#endif
qjcx0_t_i=((((DQ_j1+DQ_j2)-DQ_j3)*vdcx_t)+(DC_max*Dv_j4));
#endif
}
else
{
#if defined(_DYNAMIC)
cjcx0_t_i=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_i_Vbp_ei=0.0;
qjcx0_t_i_Vbi_ei=0.0;
qjcx0_t_i_Vb_ci=0.0;
qjcx0_t_i_Vbp_ci=0.0;
qjcx0_t_i_Vtnode_GND=0.0;
qjcx0_t_i_Vbi_ci=0.0;
#endif
qjcx0_t_i=0.0;
#endif
}
#endif /* if (...) */
}
else
{
#ifdef _DYNAMIC
if
((cjcx01_t>0.0))
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DFV_f=(vdcx_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=(2.4*cjcx01_t_Vtnode_GND);
#endif
DFC_max=(2.4*cjcx01_t);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_e_Vsi_ci=0.0;
DFv_e_Vb_ci=((-1.0)/VT);
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=0.0;
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(b,ci))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(b,ci))/VT);
#endif
#ifdef _DYNAMIC
if
((DFv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=0.0;
DFe_Vb_ci=DFv_e_Vb_ci*d10_exp0;
DFe_Vbp_ci=DFv_e_Vbp_ci*d10_exp0;
DFe_Vbp_ei=DFv_e_Vbp_ei*d10_exp0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=(-(VT*DFe_Vb_ci*d10_logE0));
DFv_j_Vbp_ci=(-(VT*DFe_Vbp_ci*d10_logE0));
DFv_j_Vbp_ei=(-(VT*DFe_Vbp_ei*d10_logE0));
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=((DFe_Vb_ci*(1.0+DFe)-DFe*DFe_Vb_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbp_ci=((DFe_Vbp_ci*(1.0+DFe)-DFe*DFe_Vbp_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbp_ei=((DFe_Vbp_ei*(1.0+DFe)-DFe*DFe_Vbp_ei)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_j_Vsi_ci=0.0;
DFv_j_Vb_ci=1.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(b,ci);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vdcx_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=0.0;
DFb_Vb_ci=(-(DFv_j_Vb_ci/vdcx_t))*d10_logE0;
DFb_Vbp_ci=(-(DFv_j_Vbp_ci/vdcx_t))*d10_logE0;
DFb_Vbp_ei=(-(DFv_j_Vbp_ei/vdcx_t))*d10_logE0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vdcx_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vdcx_t-DFv_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vdcx_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((-zcx)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=0.0;
DFC_j1_Vb_ci=(((cjcx01_t*((-zcx)*DFb_Vb_ci)*d10_exp0)*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vb_ci));
DFC_j1_Vbp_ci=(((cjcx01_t*((-zcx)*DFb_Vbp_ci)*d10_exp0)*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vbp_ci));
DFC_j1_Vbp_ei=(((cjcx01_t*((-zcx)*DFb_Vbp_ei)*d10_exp0)*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vbp_ei));
DFC_j1_Vbi_ci=(((cjcx01_t*((-zcx)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjcx01_t_Vtnode_GND*d00_exp0)+(cjcx01_t*((-zcx)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjcx01_t*((-zcx)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjcx01_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjcx01_t*d00_exp0)*DFdvj_dv);
}
#endif
#if defined(_DYNAMIC)
cjcx0_t_i=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DFb*(1.0-zcx)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=0.0;
DFQ_j_Vb_ci=(((cjcx01_t*vdcx_t)*(-DFb_Vb_ci*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vbp_ci=(((cjcx01_t*vdcx_t)*(-DFb_Vbp_ci*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vbp_ei=(((cjcx01_t*vdcx_t)*(-DFb_Vbp_ei*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vbi_ci=(((cjcx01_t*vdcx_t)*(-DFb_Vbi_ci*(1.0-zcx)*d10_exp0))/(1.0-zcx));
DFQ_j_Vtnode_GND=(((((cjcx01_t_Vtnode_GND*vdcx_t)+(cjcx01_t*vdcx_t_Vtnode_GND))*(1.0-d00_exp0))+((cjcx01_t*vdcx_t)*(-DFb_Vtnode_GND*(1.0-zcx)*d10_exp0)))/(1.0-zcx));
DFQ_j_Vbi_ei=(((cjcx01_t*vdcx_t)*(-DFb_Vbi_ei*(1.0-zcx)*d10_exp0))/(1.0-zcx));
#endif
DFQ_j=(((cjcx01_t*vdcx_t)*(1.0-d00_exp0))/(1.0-zcx));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_i_Vbp_ei=(DFQ_j_Vbp_ei+(DFC_max*(-DFv_j_Vbp_ei)));
qjcx0_t_i_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(-DFv_j_Vbi_ei)));
qjcx0_t_i_Vb_ci=(DFQ_j_Vb_ci+(DFC_max*(1.0-DFv_j_Vb_ci)));
qjcx0_t_i_Vbp_ci=(DFQ_j_Vbp_ci+(DFC_max*(-DFv_j_Vbp_ci)));
qjcx0_t_i_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(b,ci)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
qjcx0_t_i_Vbi_ci=(DFQ_j_Vbi_ci+(DFC_max*(-DFv_j_Vbi_ci)));
#endif
qjcx0_t_i=(DFQ_j+(DFC_max*(BP(b,ci)-DFv_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
cjcx0_t_i=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx0_t_i_Vbp_ei=0.0;
qjcx0_t_i_Vbi_ei=0.0;
qjcx0_t_i_Vb_ci=0.0;
qjcx0_t_i_Vbp_ci=0.0;
qjcx0_t_i_Vtnode_GND=0.0;
qjcx0_t_i_Vbi_ci=0.0;
#endif
qjcx0_t_i=0.0;
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((vpts_t<1.0e2))
{
#ifdef _DYNAMIC
if
((cjs0_t>0.0))
{
#if defined(_DYNAMIC)
Dz_r=(zs/4.0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_p_Vtnode_GND=(vpts_t_Vtnode_GND-vds_t_Vtnode_GND);
#endif
Dv_p=(vpts_t-vds_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zs))
#if defined(_DERIVATE)
DV_f_Vtnode_GND=vds_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DV_f=(vds_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_max_Vtnode_GND=(2.4*cjs0_t_Vtnode_GND);
#endif
DC_max=(2.4*cjs0_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vpts_t/vds_t))
double m00_exp(d00_exp1,(d00_logE0*(Dz_r-zs)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vpts_t/vds_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
DC_c_Vtnode_GND=((cjs0_t_Vtnode_GND*d00_exp1)+(cjs0_t*((vpts_t_Vtnode_GND*vds_t-vpts_t*vds_t_Vtnode_GND)/vds_t/vds_t)*d10_logE0*(Dz_r-zs)*d10_exp1));
#endif
DC_c=(cjs0_t*d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=VT_Vtnode_GND;
#endif
Da=VT;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_e_Vsi_ci=((-1.0)/Da);
Dv_e_Vb_ci=0.0;
Dv_e_Vbp_ci=0.0;
Dv_e_Vtnode_GND=((DV_f_Vtnode_GND*Da-(DV_f-BP(si,ci))*Da_Vtnode_GND)/Da/Da);
Dv_e_Vbi_ci=0.0;
#endif
Dv_e=((DV_f-BP(si,ci))/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=Dv_e_Vsi_ci*d10_exp0;
De_Vb_ci=Dv_e_Vb_ci*d10_exp0;
De_Vbp_ci=Dv_e_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_e_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_e_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=((De_Vsi_ci*(1.0+De)-De*De_Vsi_ci)/(1.0+De)/(1.0+De));
De_1_Vb_ci=((De_Vb_ci*(1.0+De)-De*De_Vb_ci)/(1.0+De)/(1.0+De));
De_1_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_1_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_1_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_1=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=(-(Da*De_Vsi_ci*d10_logE0));
Dv_j1_Vb_ci=(-(Da*De_Vb_ci*d10_logE0));
Dv_j1_Vbp_ci=(-(Da*De_Vbp_ci*d10_logE0));
Dv_j1_Vtnode_GND=(DV_f_Vtnode_GND-((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j1_Vbi_ci=(-(Da*De_Vbi_ci*d10_logE0));
#endif
Dv_j1=(DV_f-(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_1_Vsi_ci=0.0;
De_1_Vb_ci=0.0;
De_1_Vbp_ci=0.0;
De_1_Vtnode_GND=0.0;
De_1_Vbi_ci=0.0;
#endif
De_1=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j1_Vsi_ci=1.0;
Dv_j1_Vb_ci=0.0;
Dv_j1_Vbp_ci=0.0;
Dv_j1_Vtnode_GND=0.0;
Dv_j1_Vbi_ci=0.0;
#endif
Dv_j1=BP(si,ci);
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Da_Vtnode_GND=((0.1*Dv_p_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
Da=((0.1*Dv_p)+(4.0*VT));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_r_Vsi_ci=(Dv_j1_Vsi_ci/Da);
Dv_r_Vb_ci=(Dv_j1_Vb_ci/Da);
Dv_r_Vbp_ci=(Dv_j1_Vbp_ci/Da);
Dv_r_Vtnode_GND=(((Dv_p_Vtnode_GND+Dv_j1_Vtnode_GND)*Da-(Dv_p+Dv_j1)*Da_Vtnode_GND)/Da/Da);
Dv_r_Vbi_ci=(Dv_j1_Vbi_ci/Da);
#endif
Dv_r=((Dv_p+Dv_j1)/Da);
#endif
#ifdef _DYNAMIC
if
((Dv_r<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,Dv_r)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
De_Vsi_ci=Dv_r_Vsi_ci*d10_exp0;
De_Vb_ci=Dv_r_Vb_ci*d10_exp0;
De_Vbp_ci=Dv_r_Vbp_ci*d10_exp0;
De_Vtnode_GND=Dv_r_Vtnode_GND*d10_exp0;
De_Vbi_ci=Dv_r_Vbi_ci*d10_exp0;
#endif
De=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=((De_Vsi_ci*(1.0+De)-De*De_Vsi_ci)/(1.0+De)/(1.0+De));
De_2_Vb_ci=((De_Vb_ci*(1.0+De)-De*De_Vb_ci)/(1.0+De)/(1.0+De));
De_2_Vbp_ci=((De_Vbp_ci*(1.0+De)-De*De_Vbp_ci)/(1.0+De)/(1.0+De));
De_2_Vtnode_GND=((De_Vtnode_GND*(1.0+De)-De*De_Vtnode_GND)/(1.0+De)/(1.0+De));
De_2_Vbi_ci=((De_Vbi_ci*(1.0+De)-De*De_Vbi_ci)/(1.0+De)/(1.0+De));
#endif
De_2=(De/(1.0+De));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+De))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+De))
#endif
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=(Da*De_Vsi_ci*d10_logE0);
Dv_j2_Vb_ci=(Da*De_Vb_ci*d10_logE0);
Dv_j2_Vbp_ci=(Da*De_Vbp_ci*d10_logE0);
Dv_j2_Vtnode_GND=((-Dv_p_Vtnode_GND)+((Da_Vtnode_GND*d00_logE0)+(Da*De_Vtnode_GND*d10_logE0)));
Dv_j2_Vbi_ci=(Da*De_Vbi_ci*d10_logE0);
#endif
Dv_j2=((-Dv_p)+(Da*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
De_2_Vsi_ci=0.0;
De_2_Vb_ci=0.0;
De_2_Vbp_ci=0.0;
De_2_Vtnode_GND=0.0;
De_2_Vbi_ci=0.0;
#endif
De_2=1.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j2_Vsi_ci=Dv_j1_Vsi_ci;
Dv_j2_Vb_ci=Dv_j1_Vb_ci;
Dv_j2_Vbp_ci=Dv_j1_Vbp_ci;
Dv_j2_Vtnode_GND=Dv_j1_Vtnode_GND;
Dv_j2_Vbi_ci=Dv_j1_Vbi_ci;
#endif
Dv_j2=Dv_j1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Dv_j4_Vsi_ci=(1.0-Dv_j1_Vsi_ci);
Dv_j4_Vb_ci=(-Dv_j1_Vb_ci);
Dv_j4_Vbp_ci=(-Dv_j1_Vbp_ci);
Dv_j4_Vbi_ci=(-Dv_j1_Vbi_ci);
Dv_j4_Vtnode_GND=(-Dv_j1_Vtnode_GND);
#endif
Dv_j4=(BP(si,ci)-Dv_j1);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j1/vds_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j1/vds_t)))
#endif
#if defined(_DERIVATE)
DCln1_Vsi_ci=(-(Dv_j1_Vsi_ci/vds_t))*d10_logE0;
DCln1_Vb_ci=(-(Dv_j1_Vb_ci/vds_t))*d10_logE0;
DCln1_Vbp_ci=(-(Dv_j1_Vbp_ci/vds_t))*d10_logE0;
DCln1_Vtnode_GND=(-((Dv_j1_Vtnode_GND*vds_t-Dv_j1*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0;
DCln1_Vbi_ci=(-(Dv_j1_Vbi_ci/vds_t))*d10_logE0;
#endif
DCln1=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(Dv_j2/vds_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(Dv_j2/vds_t)))
#endif
#if defined(_DERIVATE)
DCln2_Vsi_ci=(-(Dv_j2_Vsi_ci/vds_t))*d10_logE0;
DCln2_Vb_ci=(-(Dv_j2_Vb_ci/vds_t))*d10_logE0;
DCln2_Vbp_ci=(-(Dv_j2_Vbp_ci/vds_t))*d10_logE0;
DCln2_Vtnode_GND=(-((Dv_j2_Vtnode_GND*vds_t-Dv_j2*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0;
DCln2_Vbi_ci=(-(Dv_j2_Vbi_ci/vds_t))*d10_logE0;
#endif
DCln2=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
Dz1=(1.0-zs);
#endif
#if defined(_DYNAMIC)
Dzr1=(1.0-Dz_r);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*(-zs)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j1_Vsi_ci=(((((cjs0_t*DCln2_Vsi_ci*(-zs)*d10_exp0)*De_1)+((cjs0_t*d00_exp0)*De_1_Vsi_ci))*De_2)+(((cjs0_t*d00_exp0)*De_1)*De_2_Vsi_ci));
DC_j1_Vb_ci=(((((cjs0_t*DCln2_Vb_ci*(-zs)*d10_exp0)*De_1)+((cjs0_t*d00_exp0)*De_1_Vb_ci))*De_2)+(((cjs0_t*d00_exp0)*De_1)*De_2_Vb_ci));
DC_j1_Vbp_ci=(((((cjs0_t*DCln2_Vbp_ci*(-zs)*d10_exp0)*De_1)+((cjs0_t*d00_exp0)*De_1_Vbp_ci))*De_2)+(((cjs0_t*d00_exp0)*De_1)*De_2_Vbp_ci));
DC_j1_Vtnode_GND=((((((cjs0_t_Vtnode_GND*d00_exp0)+(cjs0_t*DCln2_Vtnode_GND*(-zs)*d10_exp0))*De_1)+((cjs0_t*d00_exp0)*De_1_Vtnode_GND))*De_2)+(((cjs0_t*d00_exp0)*De_1)*De_2_Vtnode_GND));
DC_j1_Vbi_ci=(((((cjs0_t*DCln2_Vbi_ci*(-zs)*d10_exp0)*De_1)+((cjs0_t*d00_exp0)*De_1_Vbi_ci))*De_2)+(((cjs0_t*d00_exp0)*De_1)*De_2_Vbi_ci));
#endif
DC_j1=(((cjs0_t*d00_exp0)*De_1)*De_2);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*(-Dz_r)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DC_j2_Vsi_ci=(((DC_c*DCln1_Vsi_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vsi_ci)));
DC_j2_Vb_ci=(((DC_c*DCln1_Vb_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vb_ci)));
DC_j2_Vbp_ci=(((DC_c*DCln1_Vbp_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbp_ci)));
DC_j2_Vtnode_GND=((((DC_c_Vtnode_GND*d00_exp0)+(DC_c*DCln1_Vtnode_GND*(-Dz_r)*d10_exp0))*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vtnode_GND)));
DC_j2_Vbi_ci=(((DC_c*DCln1_Vbi_ci*(-Dz_r)*d10_exp0)*(1.0-De_2))+((DC_c*d00_exp0)*(-De_2_Vbi_ci)));
#endif
DC_j2=((DC_c*d00_exp0)*(1.0-De_2));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DC_j3_Vsi_ci=(DC_max*(-De_1_Vsi_ci));
DC_j3_Vb_ci=(DC_max*(-De_1_Vb_ci));
DC_j3_Vbp_ci=(DC_max*(-De_1_Vbp_ci));
DC_j3_Vtnode_GND=((DC_max_Vtnode_GND*(1.0-De_1))+(DC_max*(-De_1_Vtnode_GND)));
DC_j3_Vbi_ci=(DC_max*(-De_1_Vbi_ci));
#endif
DC_j3=(DC_max*(1.0-De_1));
#endif
#if defined(_DYNAMIC)
Cjs=((DC_j1+DC_j2)+DC_j3);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dz1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j1_Vsi_ci=((cjs0_t*(-DCln2_Vsi_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vb_ci=((cjs0_t*(-DCln2_Vb_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vbp_ci=((cjs0_t*(-DCln2_Vbp_ci*Dz1*d10_exp0))/Dz1);
DQ_j1_Vtnode_GND=(((cjs0_t_Vtnode_GND*(1.0-d00_exp0))+(cjs0_t*(-DCln2_Vtnode_GND*Dz1*d10_exp0)))/Dz1);
DQ_j1_Vbi_ci=((cjs0_t*(-DCln2_Vbi_ci*Dz1*d10_exp0))/Dz1);
#endif
DQ_j1=((cjs0_t*(1.0-d00_exp0))/Dz1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln1*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j2_Vsi_ci=((DC_c*(-DCln1_Vsi_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vb_ci=((DC_c*(-DCln1_Vb_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vbp_ci=((DC_c*(-DCln1_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j2_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln1_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j2_Vbi_ci=((DC_c*(-DCln1_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j2=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DCln2*Dzr1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DQ_j3_Vsi_ci=((DC_c*(-DCln2_Vsi_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vb_ci=((DC_c*(-DCln2_Vb_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vbp_ci=((DC_c*(-DCln2_Vbp_ci*Dzr1*d10_exp0))/Dzr1);
DQ_j3_Vtnode_GND=(((DC_c_Vtnode_GND*(1.0-d00_exp0))+(DC_c*(-DCln2_Vtnode_GND*Dzr1*d10_exp0)))/Dzr1);
DQ_j3_Vbi_ci=((DC_c*(-DCln2_Vbi_ci*Dzr1*d10_exp0))/Dzr1);
#endif
DQ_j3=((DC_c*(1.0-d00_exp0))/Dzr1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjs_Vbp_ei=0.0;
Qjs_Vbi_ei=0.0;
Qjs_Vsi_ci=(((DQ_j1_Vsi_ci+DQ_j2_Vsi_ci)-DQ_j3_Vsi_ci)*vds_t+(DC_max*Dv_j4_Vsi_ci));
Qjs_Vb_ci=(((DQ_j1_Vb_ci+DQ_j2_Vb_ci)-DQ_j3_Vb_ci)*vds_t+(DC_max*Dv_j4_Vb_ci));
Qjs_Vbp_ci=(((DQ_j1_Vbp_ci+DQ_j2_Vbp_ci)-DQ_j3_Vbp_ci)*vds_t+(DC_max*Dv_j4_Vbp_ci));
Qjs_Vtnode_GND=(((((DQ_j1_Vtnode_GND+DQ_j2_Vtnode_GND)-DQ_j3_Vtnode_GND)*vds_t)+(((DQ_j1+DQ_j2)-DQ_j3)*vds_t_Vtnode_GND))+((DC_max_Vtnode_GND*Dv_j4)+(DC_max*Dv_j4_Vtnode_GND)));
Qjs_Vbi_ci=(((DQ_j1_Vbi_ci+DQ_j2_Vbi_ci)-DQ_j3_Vbi_ci)*vds_t+(DC_max*Dv_j4_Vbi_ci));
#endif
Qjs=((((DQ_j1+DQ_j2)-DQ_j3)*vds_t)+(DC_max*Dv_j4));
#endif
}
else
{
#if defined(_DYNAMIC)
Cjs=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjs_Vbp_ei=0.0;
Qjs_Vbi_ei=0.0;
Qjs_Vsi_ci=0.0;
Qjs_Vb_ci=0.0;
Qjs_Vbp_ci=0.0;
Qjs_Vtnode_GND=0.0;
Qjs_Vbi_ci=0.0;
#endif
Qjs=0.0;
#endif
}
#endif /* if (...) */
}
else
{
#ifdef _DYNAMIC
if
((cjs0_t>0.0))
{
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zs))
#if defined(_DERIVATE)
DFV_f_Vtnode_GND=vds_t_Vtnode_GND*(1.0-d00_exp1);
#endif
DFV_f=(vds_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFC_max_Vtnode_GND=(2.4*cjs0_t_Vtnode_GND);
#endif
DFC_max=(2.4*cjs0_t);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_e_Vsi_ci=((-1.0)/VT);
DFv_e_Vb_ci=0.0;
DFv_e_Vbp_ci=0.0;
DFv_e_Vbp_ei=0.0;
DFv_e_Vbi_ci=0.0;
DFv_e_Vtnode_GND=((DFV_f_Vtnode_GND*VT-(DFV_f-BP(si,ci))*VT_Vtnode_GND)/VT/VT);
DFv_e_Vbi_ei=0.0;
#endif
DFv_e=((DFV_f-BP(si,ci))/VT);
#endif
#ifdef _DYNAMIC
if
((DFv_e<80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,DFv_e)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFe_Vsi_ci=DFv_e_Vsi_ci*d10_exp0;
DFe_Vb_ci=DFv_e_Vb_ci*d10_exp0;
DFe_Vbp_ci=DFv_e_Vbp_ci*d10_exp0;
DFe_Vbp_ei=DFv_e_Vbp_ei*d10_exp0;
DFe_Vbi_ci=DFv_e_Vbi_ci*d10_exp0;
DFe_Vtnode_GND=DFv_e_Vtnode_GND*d10_exp0;
DFe_Vbi_ei=DFv_e_Vbi_ei*d10_exp0;
#endif
DFe=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0+DFe))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+DFe))
#endif
#if defined(_DERIVATE)
DFv_j_Vsi_ci=(-(VT*DFe_Vsi_ci*d10_logE0));
DFv_j_Vb_ci=(-(VT*DFe_Vb_ci*d10_logE0));
DFv_j_Vbp_ci=(-(VT*DFe_Vbp_ci*d10_logE0));
DFv_j_Vbp_ei=(-(VT*DFe_Vbp_ei*d10_logE0));
DFv_j_Vbi_ci=(-(VT*DFe_Vbi_ci*d10_logE0));
DFv_j_Vtnode_GND=(DFV_f_Vtnode_GND-((VT_Vtnode_GND*d00_logE0)+(VT*DFe_Vtnode_GND*d10_logE0)));
DFv_j_Vbi_ei=(-(VT*DFe_Vbi_ei*d10_logE0));
#endif
DFv_j=(DFV_f-(VT*d00_logE0));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=((DFe_Vsi_ci*(1.0+DFe)-DFe*DFe_Vsi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vb_ci=((DFe_Vb_ci*(1.0+DFe)-DFe*DFe_Vb_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbp_ci=((DFe_Vbp_ci*(1.0+DFe)-DFe*DFe_Vbp_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbp_ei=((DFe_Vbp_ei*(1.0+DFe)-DFe*DFe_Vbp_ei)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ci=((DFe_Vbi_ci*(1.0+DFe)-DFe*DFe_Vbi_ci)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vtnode_GND=((DFe_Vtnode_GND*(1.0+DFe)-DFe*DFe_Vtnode_GND)/(1.0+DFe)/(1.0+DFe));
DFdvj_dv_Vbi_ei=((DFe_Vbi_ei*(1.0+DFe)-DFe*DFe_Vbi_ei)/(1.0+DFe)/(1.0+DFe));
#endif
DFdvj_dv=(DFe/(1.0+DFe));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFv_j_Vsi_ci=1.0;
DFv_j_Vb_ci=0.0;
DFv_j_Vbp_ci=0.0;
DFv_j_Vbp_ei=0.0;
DFv_j_Vbi_ci=0.0;
DFv_j_Vtnode_GND=0.0;
DFv_j_Vbi_ei=0.0;
#endif
DFv_j=BP(si,ci);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
DFdvj_dv_Vsi_ci=0.0;
DFdvj_dv_Vb_ci=0.0;
DFdvj_dv_Vbp_ci=0.0;
DFdvj_dv_Vbp_ei=0.0;
DFdvj_dv_Vbi_ci=0.0;
DFdvj_dv_Vtnode_GND=0.0;
DFdvj_dv_Vbi_ei=0.0;
#endif
DFdvj_dv=1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(DFv_j/vds_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(DFv_j/vds_t)))
#endif
#if defined(_DERIVATE)
DFb_Vsi_ci=(-(DFv_j_Vsi_ci/vds_t))*d10_logE0;
DFb_Vb_ci=(-(DFv_j_Vb_ci/vds_t))*d10_logE0;
DFb_Vbp_ci=(-(DFv_j_Vbp_ci/vds_t))*d10_logE0;
DFb_Vbp_ei=(-(DFv_j_Vbp_ei/vds_t))*d10_logE0;
DFb_Vbi_ci=(-(DFv_j_Vbi_ci/vds_t))*d10_logE0;
DFb_Vtnode_GND=(-((DFv_j_Vtnode_GND*vds_t-DFv_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0;
DFb_Vbi_ei=(-(DFv_j_Vbi_ei/vds_t))*d10_logE0;
#endif
DFb=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((-zs)*DFb))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFC_j1_Vsi_ci=(((cjs0_t*((-zs)*DFb_Vsi_ci)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vsi_ci));
DFC_j1_Vb_ci=(((cjs0_t*((-zs)*DFb_Vb_ci)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vb_ci));
DFC_j1_Vbp_ci=(((cjs0_t*((-zs)*DFb_Vbp_ci)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vbp_ci));
DFC_j1_Vbp_ei=(((cjs0_t*((-zs)*DFb_Vbp_ei)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vbp_ei));
DFC_j1_Vbi_ci=(((cjs0_t*((-zs)*DFb_Vbi_ci)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vbi_ci));
DFC_j1_Vtnode_GND=((((cjs0_t_Vtnode_GND*d00_exp0)+(cjs0_t*((-zs)*DFb_Vtnode_GND)*d10_exp0))*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vtnode_GND));
DFC_j1_Vbi_ei=(((cjs0_t*((-zs)*DFb_Vbi_ei)*d10_exp0)*DFdvj_dv)+((cjs0_t*d00_exp0)*DFdvj_dv_Vbi_ei));
#endif
DFC_j1=((cjs0_t*d00_exp0)*DFdvj_dv);
}
#endif
#if defined(_DYNAMIC)
Cjs=(DFC_j1+(DFC_max*(1.0-DFdvj_dv)));
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(DFb*(1.0-zs)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
DFQ_j_Vsi_ci=(((cjs0_t*vds_t)*(-DFb_Vsi_ci*(1.0-zs)*d10_exp0))/(1.0-zs));
DFQ_j_Vb_ci=(((cjs0_t*vds_t)*(-DFb_Vb_ci*(1.0-zs)*d10_exp0))/(1.0-zs));
DFQ_j_Vbp_ci=(((cjs0_t*vds_t)*(-DFb_Vbp_ci*(1.0-zs)*d10_exp0))/(1.0-zs));
DFQ_j_Vbp_ei=(((cjs0_t*vds_t)*(-DFb_Vbp_ei*(1.0-zs)*d10_exp0))/(1.0-zs));
DFQ_j_Vbi_ci=(((cjs0_t*vds_t)*(-DFb_Vbi_ci*(1.0-zs)*d10_exp0))/(1.0-zs));
DFQ_j_Vtnode_GND=(((((cjs0_t_Vtnode_GND*vds_t)+(cjs0_t*vds_t_Vtnode_GND))*(1.0-d00_exp0))+((cjs0_t*vds_t)*(-DFb_Vtnode_GND*(1.0-zs)*d10_exp0)))/(1.0-zs));
DFQ_j_Vbi_ei=(((cjs0_t*vds_t)*(-DFb_Vbi_ei*(1.0-zs)*d10_exp0))/(1.0-zs));
#endif
DFQ_j=(((cjs0_t*vds_t)*(1.0-d00_exp0))/(1.0-zs));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjs_Vbp_ei=(DFQ_j_Vbp_ei+(DFC_max*(-DFv_j_Vbp_ei)));
Qjs_Vbi_ei=(DFQ_j_Vbi_ei+(DFC_max*(-DFv_j_Vbi_ei)));
Qjs_Vsi_ci=(DFQ_j_Vsi_ci+(DFC_max*(1.0-DFv_j_Vsi_ci)));
Qjs_Vb_ci=(DFQ_j_Vb_ci+(DFC_max*(-DFv_j_Vb_ci)));
Qjs_Vbp_ci=(DFQ_j_Vbp_ci+(DFC_max*(-DFv_j_Vbp_ci)));
Qjs_Vtnode_GND=(DFQ_j_Vtnode_GND+((DFC_max_Vtnode_GND*(BP(si,ci)-DFv_j))+(DFC_max*(-DFv_j_Vtnode_GND))));
Qjs_Vbi_ci=(DFQ_j_Vbi_ci+(DFC_max*(-DFv_j_Vbi_ci)));
#endif
Qjs=(DFQ_j+(DFC_max*(BP(si,ci)-DFv_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
Cjs=0.0;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qjs_Vbp_ei=0.0;
Qjs_Vbi_ei=0.0;
Qjs_Vsi_ci=0.0;
Qjs_Vb_ci=0.0;
Qjs_Vbp_ci=0.0;
Qjs_Vtnode_GND=0.0;
Qjs_Vbi_ci=0.0;
#endif
Qjs=0.0;
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
if
((itss>0.0))
{
#if defined(_DERIVATE)
HSUM_Vtnode_GND=(msf*VT_Vtnode_GND);
#endif
HSUM=(msf*VT);
{
double m00_limexp(d00_limexp0,(BP(bp,ci)/HSUM))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(bp,ci)/HSUM))
#endif
#if defined(_DERIVATE)
HSa_Vbp_ci=(1/HSUM)*d10_limexp0;
HSa_Vtnode_GND=(-BP(bp,ci)*HSUM_Vtnode_GND/HSUM/HSUM)*d10_limexp0;
#endif
HSa=d00_limexp0;
}
{
double m00_limexp(d00_limexp0,(BP(si,ci)/HSUM))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(si,ci)/HSUM))
#endif
#if defined(_DERIVATE)
HSb_Vsi_ci=(1/HSUM)*d10_limexp0;
HSb_Vtnode_GND=(-BP(si,ci)*HSUM_Vtnode_GND/HSUM/HSUM)*d10_limexp0;
#endif
HSb=d00_limexp0;
}
#if defined(_DERIVATE)
HSI_Tsu_Vtnode_GND=((itss_t_Vtnode_GND*(HSa-HSb))+(itss_t*(HSa_Vtnode_GND-HSb_Vtnode_GND)));
HSI_Tsu_Vbp_ci=(itss_t*HSa_Vbp_ci);
HSI_Tsu_Vsi_ci=(itss_t*(-HSb_Vsi_ci));
#endif
HSI_Tsu=(itss_t*(HSa-HSb));
#ifdef _DYNAMIC
if
((tsf>0.0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdsu_Vtnode_GND=((((tsf_t_Vtnode_GND*itss_t)+(tsf_t*itss_t_Vtnode_GND))*HSa)+((tsf_t*itss_t)*HSa_Vtnode_GND));
Qdsu_Vbp_ci=((tsf_t*itss_t)*HSa_Vbp_ci);
#endif
Qdsu=((tsf_t*itss_t)*HSa);
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdsu_Vtnode_GND=0.0;
Qdsu_Vbp_ci=0.0;
#endif
Qdsu=0.0;
#endif
}
#endif /* if (...) */
}
else
{
#if defined(_DERIVATE)
HSI_Tsu_Vtnode_GND=0.0;
HSI_Tsu_Vbp_ci=0.0;
HSI_Tsu_Vsi_ci=0.0;
#endif
HSI_Tsu=0.0;
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qdsu_Vtnode_GND=0.0;
Qdsu_Vbp_ci=0.0;
#endif
Qdsu=0.0;
#endif
}
#if defined(_DERIVATE)
DIOY_Vsi_ci=(1/(msc*VT));
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-BP(si,ci)*(msc*VT_Vtnode_GND)/(msc*VT)/(msc*VT));
#endif
DIOY=(BP(si,ci)/(msc*VT));
if
((iscs>0.0))
{
if
((DIOY>80.0))
{
#if defined(_DERIVATE)
le_Vsi_ci=DIOY_Vsi_ci;
le_Vbp_ci=DIOY_Vbp_ci;
le_Vbp_ei=DIOY_Vbp_ei;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80.0));
#if defined(_DERIVATE)
DIOY_Vsi_ci=0.0;
DIOY_Vbp_ci=0.0;
DIOY_Vbp_ei=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80.0;
}
else
{
#if defined(_DERIVATE)
le_Vsi_ci=0.0;
le_Vbp_ci=0.0;
le_Vbp_ei=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=0.0;
le_Vtnode_GND=0.0;
#endif
le=1;
}
{
double m00_limexp(d00_limexp0,DIOY)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,DIOY)
#endif
#if defined(_DERIVATE)
le_Vsi_ci=((le_Vsi_ci*d00_limexp0)+(le*DIOY_Vsi_ci*d10_limexp0));
le_Vbp_ci=((le_Vbp_ci*d00_limexp0)+(le*DIOY_Vbp_ci*d10_limexp0));
le_Vbp_ei=((le_Vbp_ei*d00_limexp0)+(le*DIOY_Vbp_ei*d10_limexp0));
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ijsc_Vtnode_GND=((iscs_t_Vtnode_GND*(le-1.0))+(iscs_t*le_Vtnode_GND));
ijsc_Vsi_ci=(iscs_t*le_Vsi_ci);
ijsc_Vbp_ci=(iscs_t*le_Vbp_ci);
ijsc_Vbp_ei=(iscs_t*le_Vbp_ei);
ijsc_Vbi_ci=(iscs_t*le_Vbi_ci);
ijsc_Vbi_ei=(iscs_t*le_Vbi_ei);
#endif
ijsc=(iscs_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ijsc_Vtnode_GND=(-iscs_t_Vtnode_GND);
ijsc_Vsi_ci=0.0;
ijsc_Vbp_ci=0.0;
ijsc_Vbp_ei=0.0;
ijsc_Vbi_ci=0.0;
ijsc_Vbi_ei=0.0;
#endif
ijsc=(-iscs_t);
}
}
else
{
#if defined(_DERIVATE)
ijsc_Vtnode_GND=0.0;
ijsc_Vsi_ci=0.0;
ijsc_Vbp_ci=0.0;
ijsc_Vbp_ei=0.0;
ijsc_Vbi_ci=0.0;
ijsc_Vbi_ei=0.0;
#endif
ijsc=0.0;
}
if
((rth>0.0))
{
#if defined(_DERIVATE)
pterm_Vci_ei=((it+(it_Vci_ei*BP(ci,ei)))+((vdci_t-BP(bi,ci))*iavl_Vci_ei));
pterm_Vtnode_GND=((BP(ci,ei)*it_Vtnode_GND)+((vdci_t_Vtnode_GND*iavl)+((vdci_t-BP(bi,ci))*iavl_Vtnode_GND)));
pterm_Vbi_ei=((BP(ci,ei)*it_Vbi_ei)+((vdci_t-BP(bi,ci))*iavl_Vbi_ei));
pterm_Vbi_ci=((BP(ci,ei)*it_Vbi_ci)+(((-1.0)*iavl)+((vdci_t-BP(bi,ci))*iavl_Vbi_ci)));
#endif
pterm=((BP(ci,ei)*it)+((vdci_t-BP(bi,ci))*iavl));
}
_load_static_residual2(bp,ei,(((-ibet)+ibep)+irep));
#if defined(_DERIVATE)
_load_static_jacobian4(bp,ei,bi,ei,(((-ibet_Vbi_ei)+ibep_Vbi_ei)+irep_Vbi_ei));
_load_static_jacobian4(bp,ei,bi,ci,(((-ibet_Vbi_ci)+ibep_Vbi_ci)+irep_Vbi_ci));
_load_static_jacobian4(bp,ei,bp,ei,(((-ibet_Vbp_ei)+ibep_Vbp_ei)+irep_Vbp_ei));
_load_static_jacobian2s(bp,ei,tnode,(((-ibet_Vtnode_GND)+ibep_Vtnode_GND)+irep_Vtnode_GND));
#endif
_load_static_residual2(bi,ei,(ibei+irei));
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ei,bi,ei,(ibei_Vbi_ei+irei_Vbi_ei));
_load_static_jacobian2s(bi,ei,tnode,(ibei_Vtnode_GND+irei_Vtnode_GND));
#endif
_load_static_residual2(bp,si,HSI_Tsu);
#if defined(_DERIVATE)
_load_static_jacobian4(bp,si,si,ci,HSI_Tsu_Vsi_ci);
_load_static_jacobian4(bp,si,bp,ci,HSI_Tsu_Vbp_ci);
_load_static_jacobian2s(bp,si,tnode,HSI_Tsu_Vtnode_GND);
#endif
_load_static_residual2(bp,ci,ijbcx);
#if defined(_DERIVATE)
_load_static_jacobian4(bp,ci,bi,ei,ijbcx_Vbi_ei);
_load_static_jacobian4(bp,ci,bi,ci,ijbcx_Vbi_ci);
_load_static_jacobian4(bp,ci,bp,ei,ijbcx_Vbp_ei);
_load_static_jacobian4(bp,ci,bp,ci,ijbcx_Vbp_ci);
_load_static_jacobian2s(bp,ci,tnode,ijbcx_Vtnode_GND);
#endif
_load_static_residual2(bi,ci,(ibci-iavl));
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ci,ci,ei,(-iavl_Vci_ei));
_load_static_jacobian4(bi,ci,bi,ei,(ibci_Vbi_ei-iavl_Vbi_ei));
_load_static_jacobian4(bi,ci,bi,ci,(ibci_Vbi_ci-iavl_Vbi_ci));
_load_static_jacobian2s(bi,ci,tnode,(ibci_Vtnode_GND-iavl_Vtnode_GND));
#endif
_load_static_residual2(si,ci,ijsc);
#if defined(_DERIVATE)
_load_static_jacobian4(si,ci,bi,ei,ijsc_Vbi_ei);
_load_static_jacobian4(si,ci,bi,ci,ijsc_Vbi_ci);
_load_static_jacobian4(si,ci,bp,ei,ijsc_Vbp_ei);
_load_static_jacobian4(si,ci,bp,ci,ijsc_Vbp_ci);
_load_static_jacobian4(si,ci,si,ci,ijsc_Vsi_ci);
_load_static_jacobian2s(si,ci,tnode,ijsc_Vtnode_GND);
#endif
_load_static_residual2(ci,ei,it);
#if defined(_DERIVATE)
_load_static_jacobian4(ci,ei,bi,ci,it_Vbi_ci);
_load_static_jacobian4(ci,ei,bi,ei,it_Vbi_ei);
_load_static_jacobian2s(ci,ei,tnode,it_Vtnode_GND);
_load_static_jacobian4(ci,ei,ci,ei,it_Vci_ei);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,ci,_DDT((ccox1*BP(b,ci))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,ci,b,ci,(ccox1));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bp,ci,_DDT((ccox2*BP(bp,ci))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bp,ci,bp,ci,(ccox2));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bp,e,_DDT((ceox*BP(bp,e))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bp,e,bp,e,(ceox));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(si,ci,_DDT(Qjs));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(si,ci,bi,ci,Qjs_Vbi_ci);
_load_dynamic_jacobian2s(si,ci,tnode,Qjs_Vtnode_GND);
_load_dynamic_jacobian4(si,ci,bp,ci,Qjs_Vbp_ci);
_load_dynamic_jacobian4(si,ci,b,ci,Qjs_Vb_ci);
_load_dynamic_jacobian4(si,ci,si,ci,Qjs_Vsi_ci);
_load_dynamic_jacobian4(si,ci,bi,ei,Qjs_Vbi_ei);
_load_dynamic_jacobian4(si,ci,bp,ei,Qjs_Vbp_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bp,ei,_DDT(Qjep));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bp,ei,bi,ei,Qjep_Vbi_ei);
_load_dynamic_jacobian2s(bp,ei,tnode,Qjep_Vtnode_GND);
_load_dynamic_jacobian4(bp,ei,bi,ci,Qjep_Vbi_ci);
_load_dynamic_jacobian4(bp,ei,bp,ei,Qjep_Vbp_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bi,ei,_DDT((Qdei+Qjei)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bi,ei,bi,ei,(Qdei_Vbi_ei+Qjei_Vbi_ei));
_load_dynamic_jacobian4(bi,ei,bi,ci,Qdei_Vbi_ci);
_load_dynamic_jacobian2s(bi,ei,tnode,(Qdei_Vtnode_GND+Qjei_Vtnode_GND));
_load_dynamic_jacobian4(bi,ei,ci,ei,Qdei_Vci_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bp,ci,_DDT((qjcx0_t_ii+Qdsu)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bp,ci,bi,ci,qjcx0_t_ii_Vbi_ci);
_load_dynamic_jacobian2s(bp,ci,tnode,(qjcx0_t_ii_Vtnode_GND+Qdsu_Vtnode_GND));
_load_dynamic_jacobian4(bp,ci,bp,ci,(qjcx0_t_ii_Vbp_ci+Qdsu_Vbp_ci));
_load_dynamic_jacobian4(bp,ci,bi,ei,qjcx0_t_ii_Vbi_ei);
_load_dynamic_jacobian4(bp,ci,bp,ei,qjcx0_t_ii_Vbp_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bi,ci,_DDT((Qdci+Qjci)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bi,ci,bi,ei,(Qdci_Vbi_ei+Qjci_Vbi_ei));
_load_dynamic_jacobian4(bi,ci,bi,ci,(Qdci_Vbi_ci+Qjci_Vbi_ci));
_load_dynamic_jacobian2s(bi,ci,tnode,(Qdci_Vtnode_GND+Qjci_Vtnode_GND));
_load_dynamic_jacobian4(bi,ci,ci,ei,Qdci_Vci_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,ci,_DDT(qjcx0_t_i));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,ci,bi,ci,qjcx0_t_i_Vbi_ci);
_load_dynamic_jacobian2s(b,ci,tnode,qjcx0_t_i_Vtnode_GND);
_load_dynamic_jacobian4(b,ci,bp,ci,qjcx0_t_i_Vbp_ci);
_load_dynamic_jacobian4(b,ci,b,ci,qjcx0_t_i_Vb_ci);
_load_dynamic_jacobian4(b,ci,bi,ei,qjcx0_t_i_Vbi_ei);
_load_dynamic_jacobian4(b,ci,bp,ei,qjcx0_t_i_Vbp_ei);
#endif
#endif
if
((rbx>0.0))
{
_load_static_residual2(b,bp,(BP(b,bp)/rbx_t));
#if defined(_DERIVATE)
_load_static_jacobian2s(b,bp,tnode,(-BP(b,bp)*rbx_t_Vtnode_GND/rbx_t/rbx_t));
_load_static_jacobian4(b,bp,b,bp,(1/rbx_t));
#endif
}
else
{
_load_static_residual2(b,bp,(BP(b,bp)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(b,bp,b,bp,(1/1e-6));
#endif
}
if
((rbi0>0.0))
{
_load_static_residual2(bp,bi,(BP(bp,bi)/rbi));
#if defined(_DERIVATE)
_load_static_jacobian4(bp,bi,bi,ci,(-BP(bp,bi)*rbi_Vbi_ci/rbi/rbi));
_load_static_jacobian4(bp,bi,ci,ei,(-BP(bp,bi)*rbi_Vci_ei/rbi/rbi));
_load_static_jacobian4(bp,bi,bi,ei,(-BP(bp,bi)*rbi_Vbi_ei/rbi/rbi));
_load_static_jacobian2s(bp,bi,tnode,(-BP(bp,bi)*rbi_Vtnode_GND/rbi/rbi));
_load_static_jacobian4(bp,bi,bp,bi,(1/rbi));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bp,bi,_DDT(qrbi));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bp,bi,bi,ci,qrbi_Vbi_ci);
_load_dynamic_jacobian4(bp,bi,ci,ei,qrbi_Vci_ei);
_load_dynamic_jacobian4(bp,bi,bi,ei,qrbi_Vbi_ei);
_load_dynamic_jacobian2s(bp,bi,tnode,qrbi_Vtnode_GND);
#endif
#endif
}
else
{
_load_static_residual2(bp,bi,(BP(bp,bi)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(bp,bi,bp,bi,(1/1e-6));
#endif
}
if
((rcx>0.0))
{
_load_static_residual2(ci,c,(BP(ci,c)/rcx_t));
#if defined(_DERIVATE)
_load_static_jacobian2s(ci,c,tnode,(-BP(ci,c)*rcx_t_Vtnode_GND/rcx_t/rcx_t));
_load_static_jacobian4(ci,c,ci,c,(1/rcx_t));
#endif
}
else
{
_load_static_residual2(ci,c,(BP(ci,c)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(ci,c,ci,c,(1/1e-6));
#endif
}
if
((re>0.0))
{
_load_static_residual2(ei,e,(BP(ei,e)/re_t));
#if defined(_DERIVATE)
_load_static_jacobian2s(ei,e,tnode,(-BP(ei,e)*re_t_Vtnode_GND/re_t/re_t));
_load_static_jacobian4(ei,e,ei,e,(1/re_t));
#endif
}
else
{
_load_static_residual2(ei,e,(BP(ei,e)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(ei,e,ei,e,(1/1e-6));
#endif
}
if
((rsu>0.0))
{
_load_static_residual2(si,s,(BP(si,s)/rsu));
#if defined(_DERIVATE)
_load_static_jacobian4(si,s,si,s,(1/rsu));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(si,s,_DDT((csu*BP(si,s))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(si,s,si,s,(csu));
#endif
#endif
}
else
{
_load_static_residual2(si,s,(BP(si,s)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(si,s,si,s,(1/1e-6));
#endif
}
if
((rth>0.0))
{
_load_static_residual1(tnode,(NP(tnode)/rth));
#if defined(_DERIVATE)
_load_static_jacobian1(tnode,tnode,(1/rth));
#endif
_load_static_residual1(tnode,(-pterm));
#if defined(_DERIVATE)
_load_static_jacobian2p(tnode,bi,ci,(-pterm_Vbi_ci));
_load_static_jacobian2p(tnode,bi,ei,(-pterm_Vbi_ei));
_load_static_jacobian1(tnode,tnode,(-pterm_Vtnode_GND));
_load_static_jacobian2p(tnode,ci,ei,(-pterm_Vci_ei));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(tnode,_DDT((cth*NP(tnode))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(tnode,tnode,(cth));
#endif
#endif
}
else
{
_load_static_residual1(tnode,(NP(tnode)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian1(tnode,tnode,(1/1e-6));
#endif
}
#if defined(_DERIVATE)
fourkt_Vtnode_GND=((4.0*1.3806503e-23)*Tdev_Vtnode_GND);
#endif
fourkt=((4.0*1.3806503e-23)*Tdev);
if
((rbx>0.0))
{
_save_whitenoise2(b,bp,(fourkt/rbx_t),"thermal");
}
if
((rbi0>0.0))
{
#if defined(_DERIVATE)
rbin_Vtnode_GND=(krbi*rbi_Vtnode_GND);
rbin_Vbi_ei=(krbi*rbi_Vbi_ei);
rbin_Vci_ei=(krbi*rbi_Vci_ei);
rbin_Vbi_ci=(krbi*rbi_Vbi_ci);
#endif
rbin=(krbi*rbi);
_save_whitenoise2(bp,bi,(fourkt/rbin),"thermal");
}
if
((rcx>0.0))
{
_save_whitenoise2(ci,c,(fourkt/rcx_t),"thermal");
}
if
((re>0.0))
{
_save_whitenoise2(ei,e,(fourkt/re_t),"thermal");
}
if
((rsu>0.0))
{
_save_whitenoise2(si,s,(fourkt/rsu),"thermal");
}
{
double m00_pow(d00_pow0,(ibei+ibep),af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(ibei+ibep),af)
#endif
#if defined(_DERIVATE)
flicker_Pwr_Vtnode_GND=(kf*(d10_pow0*(ibei_Vtnode_GND+ibep_Vtnode_GND)));
flicker_Pwr_Vbi_ei=(kf*(d10_pow0*(ibei_Vbi_ei+ibep_Vbi_ei)));
flicker_Pwr_Vbp_ei=(kf*(d10_pow0*ibep_Vbp_ei));
flicker_Pwr_Vbi_ci=(kf*(d10_pow0*ibep_Vbi_ci));
#endif
flicker_Pwr=(kf*d00_pow0);
}
_save_flickernoise2(bi,ei,flicker_Pwr,1.0,NULL);
twoq=(2.0*1.602176462e-19);
_save_whitenoise2(ci,ei,(twoq*it),"shot");
_save_whitenoise2(ci,bi,(twoq*iavl),"shot");
_save_whitenoise2(bi,ei,(twoq*ibei),"shot");
_save_whitenoise2(bi,ci,(twoq*ibci),"shot");
_save_whitenoise2(bp,ei,(twoq*ibep),"shot");
_save_whitenoise2(bp,ci,(twoq*ijbcx),"shot");
_save_whitenoise2(si,ci,(twoq*ijsc),"shot");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void hicumL2V2p1::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 10; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 10; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void hicumL2V2p1::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void hicumL2V2p1::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void hicumL2V2p1::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void hicumL2V2p1::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void hicumL2V2p1::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix hicumL2V2p1::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (10);

  for (int i1 = 0; i1 < 10; i1++) {
    for (int i2 = 0; i2 < 10; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void hicumL2V2p1::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void hicumL2V2p1::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void hicumL2V2p1::initTR (void)
{
  setStates (2 * 10 * 10);
  initDC ();
}

/* Perform transient analysis iteration step. */
void hicumL2V2p1::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 10; i1++) {
  for (i2 = 0; i2 < 10; i2++) {
    state = 2 * (i2 + 10 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 10; i1++) {
    state = 2 * (i1 + 10 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 10; i1++) {
  for (i2 = 0; i2 < 10; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 10; i3++) {
  for (i4 = 0; i4 < 10; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 10; i1++) {
  for (i2 = 0; i2 < 10; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 10; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 10; i1++) {
  for (i3 = 0; i3 < 10; i3++) {
  for (i4 = 0; i4 < 10; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 10; i1++) {
  for (i3 = 0; i3 < 10; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix hicumL2V2p1::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (10);

  _load_flickernoise2 (bi,ei,_flicker_pwr[bi][ei],_flicker_exp[bi][ei]);
  _load_whitenoise2 (ci,bi,_white_pwr[ci][bi]);
  _load_whitenoise2 (si,s,_white_pwr[si][s]);
  _load_whitenoise2 (ei,e,_white_pwr[ei][e]);
  _load_whitenoise2 (ci,c,_white_pwr[ci][c]);
  _load_whitenoise2 (bp,bi,_white_pwr[bp][bi]);
  _load_whitenoise2 (b,bp,_white_pwr[b][bp]);
  _load_whitenoise2 (ci,ei,_white_pwr[ci][ei]);
  _load_whitenoise2 (si,ci,_white_pwr[si][ci]);
  _load_whitenoise2 (bi,ci,_white_pwr[bi][ci]);
  _load_whitenoise2 (bp,ci,_white_pwr[bp][ci]);
  _load_whitenoise2 (bi,ei,_white_pwr[bi][ei]);
  _load_whitenoise2 (bp,ei,_white_pwr[bp][ei]);

  return cy;
}

/* Perform AC noise computations. */
void hicumL2V2p1::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void hicumL2V2p1::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void hicumL2V2p1::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void hicumL2V2p1::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 10; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 10; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "hicumL2V2p1.defs.h"
