/*
 * hic0_full.core.cpp - device implementations for hic0_full module
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

#include "hic0_full.analogfunction.h"
#include "component.h"
#include "device.h"
#include "hic0_full.core.h"

#ifndef CIR_hic0_full
#define CIR_hic0_full -1
#endif

// external nodes
#define c 0
#define b 1
#define e 2
#define s 3
#define tnode 4
// internal nodes
#define ci 5
#define bi 6
#define ei 7

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
#define _modelname              "hic0_full"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
hic0_full::hic0_full() : circuit (8)
{
  type = CIR_hic0_full;
}

/* Initialization of model. */
void hic0_full::initModel (void)
{
  // create internal nodes
  setInternalNode (ci, "ci");
  setInternalNode (bi, "bi");
  setInternalNode (ei, "ei");

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
void hic0_full::initDC (void)
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
void hic0_full::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void hic0_full::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[ci][ei] = 0.0;
  _white_pwr[b][bi] = 0.0;
  _white_pwr[ci][c] = 0.0;
  _white_pwr[ei][e] = 0.0;
  _white_pwr[bi][ei] = 0.0;
  _flicker_pwr[bi][ei] = 0.0;
  _flicker_exp[bi][ei] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 8; i1++) {
  for (i2 = 0; i2 < 8; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 8; i1++) {
  for (i2 = 0; i2 < 8; i2++) {
  for (i3 = 0; i3 < 8; i3++) {
  for (i4 = 0; i4 < 8; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 8; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 8; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void hic0_full::loadVariables (void)
{
  is = getPropertyDouble ("is");
  mcf = getPropertyDouble ("mcf");
  mcr = getPropertyDouble ("mcr");
  vef = getPropertyDouble ("vef");
  iqf = getPropertyDouble ("iqf");
  iqr = getPropertyDouble ("iqr");
  iqfh = getPropertyDouble ("iqfh");
  tfh = getPropertyDouble ("tfh");
  ibes = getPropertyDouble ("ibes");
  mbe = getPropertyDouble ("mbe");
  ires = getPropertyDouble ("ires");
  mre = getPropertyDouble ("mre");
  ibcs = getPropertyDouble ("ibcs");
  mbc = getPropertyDouble ("mbc");
  cje0 = getPropertyDouble ("cje0");
  vde = getPropertyDouble ("vde");
  ze = getPropertyDouble ("ze");
  aje = getPropertyDouble ("aje");
  t0 = getPropertyDouble ("t0");
  dt0h = getPropertyDouble ("dt0h");
  tbvl = getPropertyDouble ("tbvl");
  tef0 = getPropertyDouble ("tef0");
  gte = getPropertyDouble ("gte");
  thcs = getPropertyDouble ("thcs");
  ahc = getPropertyDouble ("ahc");
  tr = getPropertyDouble ("tr");
  rci0 = getPropertyDouble ("rci0");
  vlim = getPropertyDouble ("vlim");
  vpt = getPropertyDouble ("vpt");
  vces = getPropertyDouble ("vces");
  cjci0 = getPropertyDouble ("cjci0");
  vdci = getPropertyDouble ("vdci");
  zci = getPropertyDouble ("zci");
  vptci = getPropertyDouble ("vptci");
  cjcx0 = getPropertyDouble ("cjcx0");
  vdcx = getPropertyDouble ("vdcx");
  zcx = getPropertyDouble ("zcx");
  vptcx = getPropertyDouble ("vptcx");
  fbc = getPropertyDouble ("fbc");
  rbi0 = getPropertyDouble ("rbi0");
  vr0e = getPropertyDouble ("vr0e");
  vr0c = getPropertyDouble ("vr0c");
  fgeo = getPropertyDouble ("fgeo");
  rbx = getPropertyDouble ("rbx");
  rcx = getPropertyDouble ("rcx");
  re = getPropertyDouble ("re");
  itss = getPropertyDouble ("itss");
  msf = getPropertyDouble ("msf");
  iscs = getPropertyDouble ("iscs");
  msc = getPropertyDouble ("msc");
  cjs0 = getPropertyDouble ("cjs0");
  vds = getPropertyDouble ("vds");
  zs = getPropertyDouble ("zs");
  vpts = getPropertyDouble ("vpts");
  cbcpar = getPropertyDouble ("cbcpar");
  cbepar = getPropertyDouble ("cbepar");
  eavl = getPropertyDouble ("eavl");
  kavl = getPropertyDouble ("kavl");
  kf = getPropertyDouble ("kf");
  af = getPropertyDouble ("af");
  vgb = getPropertyDouble ("vgb");
  vge = getPropertyDouble ("vge");
  vgc = getPropertyDouble ("vgc");
  vgs = getPropertyDouble ("vgs");
  f1vg = getPropertyDouble ("f1vg");
  f2vg = getPropertyDouble ("f2vg");
  alt0 = getPropertyDouble ("alt0");
  kt0 = getPropertyDouble ("kt0");
  zetact = getPropertyDouble ("zetact");
  zetabet = getPropertyDouble ("zetabet");
  zetaci = getPropertyDouble ("zetaci");
  alvs = getPropertyDouble ("alvs");
  alces = getPropertyDouble ("alces");
  zetarbi = getPropertyDouble ("zetarbi");
  zetarbx = getPropertyDouble ("zetarbx");
  zetarcx = getPropertyDouble ("zetarcx");
  zetare = getPropertyDouble ("zetare");
  alkav = getPropertyDouble ("alkav");
  aleav = getPropertyDouble ("aleav");
  flsh = getPropertyInteger ("flsh");
  rth = getPropertyDouble ("rth");
  cth = getPropertyDouble ("cth");
  npn = getPropertyInteger ("npn");
  pnp = getPropertyInteger ("pnp");
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
void hic0_full::initializeModel (void)
{
{
if
(_param_given("npn"))
{
HICUMtype=(+1);
}
else
{
if
(_param_given("pnp"))
{
HICUMtype=(-1);
}
else
{
HICUMtype=(+1);
}
}
}
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void hic0_full::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void hic0_full::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void hic0_full::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void hic0_full::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double flicker_Pwr;
#if defined(_DERIVATE)
double flicker_Pwr_Vtnode_GND;
double flicker_Pwr_Vbi_ei;
#endif
double twoq;
double fourkt;
#if defined(_DERIVATE)
double fourkt_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double Qbiei;
#if defined(_DERIVATE)
double Qbiei_Vtnode_GND;
double Qbiei_Vbi_ei;
double Qbiei_Vbi_ci;
double Qbiei_Vb_ci;
double Qbiei_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double Qbici;
#if defined(_DERIVATE)
double Qbici_Vbi_ci;
double Qbici_Vtnode_GND;
double Qbici_Vb_ci;
double Qbici_Vbi_ei;
double Qbici_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double Qbe;
#if defined(_DERIVATE)
double Qbe_Vb_e;
#endif
#endif
#if defined(_DYNAMIC)
double Qbci;
#if defined(_DERIVATE)
double Qbci_Vb_ci;
#endif
#endif
#if defined(_DERIVATE)
double Ibici_Vtnode_GND;
double Ibici_Vbi_ci;
double Ibici_Vbi_ei;
double Ibici_Vb_ci;
double Ibici_Vci_ei;
#endif
double pterm;
#if defined(_DERIVATE)
double pterm_Vci_c;
double pterm_Vei_e;
double pterm_Vb_bi;
double pterm_Vs_ci;
double pterm_Vtnode_GND;
double pterm_Vbi_ei;
double pterm_Vbi_ci;
double pterm_Vb_ci;
double pterm_Vci_ei;
#endif
#if defined(_DYNAMIC)
double qjs;
#if defined(_DERIVATE)
double qjs_Vci_ei;
double qjs_Vbi_ei;
double qjs_Vs_ci;
double qjs_Vbi_ci;
double qjs_Vtnode_GND;
double qjs_Vb_ci;
#endif
#endif
#if defined(_DERIVATE)
double ijsc_Vtnode_GND;
double ijsc_Vs_ci;
double ijsc_Vbi_ci;
double ijsc_Vbi_ei;
#endif
double HSI_Tsu;
#if defined(_DERIVATE)
double HSI_Tsu_Vtnode_GND;
double HSI_Tsu_Vb_ci;
double HSI_Tsu_Vs_ci;
#endif
double HSb;
#if defined(_DERIVATE)
double HSb_Vs_ci;
double HSb_Vtnode_GND;
#endif
double HSa;
#if defined(_DERIVATE)
double HSa_Vb_ci;
double HSa_Vtnode_GND;
#endif
double HSUM;
#if defined(_DERIVATE)
double HSUM_Vtnode_GND;
#endif
double rb;
#if defined(_DERIVATE)
double rb_Vtnode_GND;
double rb_Vbi_ei;
double rb_Vbi_ci;
double rb_Vb_ci;
double rb_Vci_ei;
#endif
double eta;
#if defined(_DERIVATE)
double eta_Vtnode_GND;
double eta_Vbi_ei;
double eta_Vbi_ci;
double eta_Vb_ci;
double eta_Vci_ei;
#endif
double rbi;
#if defined(_DERIVATE)
double rbi_Vtnode_GND;
double rbi_Vbi_ei;
double rbi_Vbi_ci;
double rbi_Vb_ci;
double rbi_Vci_ei;
#endif
double fQz;
#if defined(_DERIVATE)
double fQz_Vtnode_GND;
double fQz_Vbi_ei;
double fQz_Vbi_ci;
double fQz_Vb_ci;
double fQz_Vci_ei;
#endif
double Qz_nom;
#if defined(_DERIVATE)
double Qz_nom_Vtnode_GND;
double Qz_nom_Vbi_ei;
double Qz_nom_Vbi_ci;
double Qz_nom_Vb_ci;
double Qz_nom_Vci_ei;
#endif
double qje;
#if defined(_DERIVATE)
double qje_Vtnode_GND;
double qje_Vbi_ei;
double qje_Vbi_ci;
double qje_Vb_ci;
#endif
double lncc;
#if defined(_DERIVATE)
double lncc_Vtnode_GND;
double lncc_Vbi_ci;
double lncc_Vb_ci;
#endif
#if defined(_DERIVATE)
double iavl_Vtnode_GND;
double iavl_Vbi_ei;
double iavl_Vbi_ci;
double iavl_Vb_ci;
double iavl_Vci_ei;
#endif
double a_iavl;
#if defined(_DERIVATE)
double a_iavl_Vtnode_GND;
double a_iavl_Vbi_ci;
double a_iavl_Vb_ci;
#endif
double v_bord;
#if defined(_DERIVATE)
double v_bord_Vtnode_GND;
#endif
double ibi;
#if defined(_DERIVATE)
double ibi_Vtnode_GND;
double ibi_Vbi_ei;
double ibi_Vbi_ci;
#endif
#if defined(_DERIVATE)
double ijbc_Vtnode_GND;
double ijbc_Vbi_ci;
double ijbc_Vbi_ei;
#endif
#if defined(_DERIVATE)
double ijbe_Vtnode_GND;
double ijbe_Vbi_ei;
#endif
double ire;
#if defined(_DERIVATE)
double ire_Vtnode_GND;
double ire_Vbi_ei;
#endif
double ibe;
#if defined(_DERIVATE)
double ibe_Vtnode_GND;
double ibe_Vbi_ei;
#endif
double le;
#if defined(_DERIVATE)
double le_Vs_ci;
double le_Vbi_ci;
double le_Vbi_ei;
double le_Vtnode_GND;
#endif
double DIOY;
#if defined(_DERIVATE)
double DIOY_Vs_ci;
double DIOY_Vbi_ci;
double DIOY_Vbi_ei;
double DIOY_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double qr;
#if defined(_DERIVATE)
double qr_Vtnode_GND;
double qr_Vbi_ci;
double qr_Vb_ci;
double qr_Vbi_ei;
double qr_Vci_ei;
#endif
#endif
double tf;
#if defined(_DYNAMIC)
double qf;
#if defined(_DERIVATE)
double qf_Vtnode_GND;
double qf_Vbi_ci;
double qf_Vb_ci;
double qf_Vbi_ei;
double qf_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double dqef;
#if defined(_DERIVATE)
double dqef_Vtnode_GND;
double dqef_Vbi_ei;
double dqef_Vbi_ci;
double dqef_Vb_ci;
double dqef_Vci_ei;
#endif
#endif
#if defined(_DYNAMIC)
double dtef;
#if defined(_DERIVATE)
double dtef_Vtnode_GND;
double dtef_Vbi_ei;
double dtef_Vbi_ci;
double dtef_Vb_ci;
double dtef_Vci_ei;
#endif
#endif
double dtfh;
#if defined(_DYNAMIC)
double dqfh;
#if defined(_DERIVATE)
double dqfh_Vtnode_GND;
double dqfh_Vci_ei;
double dqfh_Vbi_ei;
double dqfh_Vbi_ci;
double dqfh_Vb_ci;
#endif
#endif
#if defined(_DYNAMIC)
double tww;
#if defined(_DERIVATE)
double tww_Vtnode_GND;
double tww_Vci_ei;
double tww_Vbi_ei;
double tww_Vbi_ci;
double tww_Vb_ci;
#endif
#endif
#if defined(_DYNAMIC)
double w;
#if defined(_DERIVATE)
double w_Vci_ei;
double w_Vbi_ei;
double w_Vbi_ci;
double w_Vb_ci;
double w_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double s3;
#if defined(_DERIVATE)
double s3_Vci_ei;
double s3_Vbi_ei;
double s3_Vbi_ci;
double s3_Vb_ci;
double s3_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double qf0;
#if defined(_DERIVATE)
double qf0_Vtnode_GND;
double qf0_Vbi_ci;
double qf0_Vb_ci;
double qf0_Vbi_ei;
double qf0_Vci_ei;
#endif
#endif
#if defined(_DERIVATE)
double it_Vtnode_GND;
double it_Vbi_ei;
double it_Vbi_ci;
double it_Vb_ci;
double it_Vci_ei;
#endif
double itr;
#if defined(_DERIVATE)
double itr_Vtnode_GND;
double itr_Vbi_ci;
double itr_Vb_ci;
double itr_Vbi_ei;
double itr_Vci_ei;
#endif
double itf;
#if defined(_DERIVATE)
double itf_Vtnode_GND;
double itf_Vbi_ei;
double itf_Vbi_ci;
double itf_Vb_ci;
double itf_Vci_ei;
#endif
double facl;
#if defined(_DERIVATE)
double facl_Vtnode_GND;
double facl_Vci_ei;
double facl_Vbi_ei;
double facl_Vbi_ci;
double facl_Vb_ci;
#endif
double d_qfh;
#if defined(_DERIVATE)
double d_qfh_Vtnode_GND;
double d_qfh_Vci_ei;
double d_qfh_Vbi_ei;
double d_qfh_Vbi_ci;
double d_qfh_Vb_ci;
#endif
double wl;
#if defined(_DERIVATE)
double wl_Vtnode_GND;
double wl_Vci_ei;
double wl_Vbi_ei;
double wl_Vbi_ci;
double wl_Vb_ci;
#endif
double s3l;
#if defined(_DERIVATE)
double s3l_Vtnode_GND;
double s3l_Vci_ei;
double s3l_Vbi_ei;
double s3l_Vbi_ci;
double s3l_Vb_ci;
#endif
double al;
#if defined(_DERIVATE)
double al_Vtnode_GND;
double al_Vci_ei;
double al_Vbi_ei;
double al_Vbi_ci;
double al_Vb_ci;
#endif
double itrl;
#if defined(_DERIVATE)
double itrl_Vtnode_GND;
double itrl_Vbi_ci;
double itrl_Vb_ci;
double itrl_Vbi_ei;
#endif
double itfl;
#if defined(_DERIVATE)
double itfl_Vtnode_GND;
double itfl_Vbi_ei;
double itfl_Vbi_ci;
double itfl_Vb_ci;
#endif
double qpt;
#if defined(_DERIVATE)
double qpt_Vbi_ci;
double qpt_Vtnode_GND;
double qpt_Vb_ci;
double qpt_Vbi_ei;
#endif
double qm;
#if defined(_DERIVATE)
double qm_Vtnode_GND;
double qm_Vbi_ei;
double qm_Vbi_ci;
#endif
double itri;
#if defined(_DERIVATE)
double itri_Vtnode_GND;
double itri_Vbi_ci;
#endif
double le2;
#if defined(_DERIVATE)
double le2_Vbi_ci;
double le2_Vtnode_GND;
#endif
double arg2;
#if defined(_DERIVATE)
double arg2_Vbi_ci;
double arg2_Vtnode_GND;
#endif
double itfi;
#if defined(_DERIVATE)
double itfi_Vtnode_GND;
double itfi_Vbi_ei;
#endif
double le1;
#if defined(_DERIVATE)
double le1_Vbi_ei;
double le1_Vtnode_GND;
#endif
double arg1;
#if defined(_DERIVATE)
double arg1_Vbi_ei;
double arg1_Vtnode_GND;
#endif
double ickr;
double ickf;
#if defined(_DYNAMIC)
double tf0;
#if defined(_DERIVATE)
double tf0_Vtnode_GND;
double tf0_Vbi_ci;
double tf0_Vb_ci;
#endif
#endif
double qj_2;
#if defined(_DERIVATE)
double qj_2_Vbi_ci;
double qj_2_Vtnode_GND;
double qj_2_Vb_ci;
#endif
double cc;
#if defined(_DERIVATE)
double cc_Vtnode_GND;
double cc_Vbi_ci;
double cc_Vb_ci;
#endif
double ick;
#if defined(_DERIVATE)
double ick_Vtnode_GND;
double ick_Vci_ei;
#endif
double vceff;
#if defined(_DERIVATE)
double vceff_Vtnode_GND;
double vceff_Vci_ei;
#endif
double uvc;
#if defined(_DERIVATE)
double uvc_Vci_ei;
double uvc_Vtnode_GND;
#endif
double vc;
#if defined(_DERIVATE)
double vc_Vci_ei;
double vc_Vtnode_GND;
#endif
#if defined(_DERIVATEFORDDX)
double qjei_Vtnode_GND;
double qjei_Vbi_ei;
double qjei_Vbi_ci;
double qjei_Vb_ci;
#if defined(_DERIVATE)
  double qjei_Vbi_ci_Vtnode_GND;
  double qjei_Vbi_ci_Vbi_ei;
  double qjei_Vbi_ci_Vbi_ci;
  double qjei_Vbi_ci_Vb_ci;
  double qjei_Vbi_ei_Vtnode_GND;
  double qjei_Vbi_ei_Vbi_ei;
  double qjei_Vbi_ei_Vbi_ci;
  double qjei_Vbi_ei_Vb_ci;
#endif
#endif
double cjcii;
#if defined(_DERIVATE)
double cjcii_Vtnode_GND;
double cjcii_Vbi_ci;
double cjcii_Vb_ci;
#endif
double dvj;
#if defined(_DERIVATE)
double dvj_Vbi_ci;
double dvj_Vtnode_GND;
double dvj_Vb_ci;
#endif
#if defined(_DYNAMIC)
double qjcii;
#if defined(_DERIVATE)
double qjcii_Vbi_ci;
double qjcii_Vtnode_GND;
double qjcii_Vb_ci;
#endif
#endif
#if defined(_DERIVATE)
double qjci_Vbi_ci;
double qjci_Vtnode_GND;
double qjci_Vb_ci;
#endif
double cjcx0_t_ii;
#if defined(_DERIVATE)
double cjcx0_t_ii_Vtnode_GND;
#endif
double y;
#if defined(_DERIVATEFORDDX)
double y_Vci_ei;
double y_Vs_ci;
double y_Vbi_ei;
double y_Vbi_ci;
double y_Vtnode_GND;
double y_Vb_ci;
#if defined(_DERIVATE)
  double y_Vbi_ci_Vci_ei;
  double y_Vbi_ci_Vs_ci;
  double y_Vbi_ci_Vbi_ei;
  double y_Vbi_ci_Vbi_ci;
  double y_Vbi_ci_Vtnode_GND;
  double y_Vbi_ci_Vb_ci;
  double y_Vbi_ei_Vci_ei;
  double y_Vbi_ei_Vs_ci;
  double y_Vbi_ei_Vbi_ei;
  double y_Vbi_ei_Vbi_ci;
  double y_Vbi_ei_Vtnode_GND;
  double y_Vbi_ei_Vb_ci;
#endif
#endif
double x;
#if defined(_DERIVATE)
double x_Vtnode_GND;
double x_Vci_ei;
#endif
double v_j;
#if defined(_DERIVATEFORDDX)
double v_j_Vs_ci;
double v_j_Vbi_ei;
double v_j_Vbi_ci;
double v_j_Vtnode_GND;
double v_j_Vb_ci;
#if defined(_DERIVATE)
  double v_j_Vbi_ci_Vs_ci;
  double v_j_Vbi_ci_Vbi_ei;
  double v_j_Vbi_ci_Vbi_ci;
  double v_j_Vbi_ci_Vtnode_GND;
  double v_j_Vbi_ci_Vb_ci;
  double v_j_Vbi_ei_Vs_ci;
  double v_j_Vbi_ei_Vbi_ei;
  double v_j_Vbi_ei_Vbi_ci;
  double v_j_Vbi_ei_Vtnode_GND;
  double v_j_Vbi_ei_Vb_ci;
#endif
#endif
double xvf2;
#if defined(_DERIVATEFORDDX)
double xvf2_Vs_ci;
double xvf2_Vbi_ei;
double xvf2_Vbi_ci;
double xvf2_Vtnode_GND;
double xvf2_Vb_ci;
#if defined(_DERIVATE)
  double xvf2_Vbi_ci_Vs_ci;
  double xvf2_Vbi_ci_Vbi_ei;
  double xvf2_Vbi_ci_Vbi_ci;
  double xvf2_Vbi_ci_Vtnode_GND;
  double xvf2_Vbi_ci_Vb_ci;
  double xvf2_Vbi_ei_Vs_ci;
  double xvf2_Vbi_ei_Vbi_ei;
  double xvf2_Vbi_ei_Vbi_ci;
  double xvf2_Vbi_ei_Vtnode_GND;
  double xvf2_Vbi_ei_Vb_ci;
#endif
#endif
double xvf;
#if defined(_DERIVATEFORDDX)
double xvf_Vs_ci;
double xvf_Vbi_ei;
double xvf_Vbi_ci;
double xvf_Vtnode_GND;
double xvf_Vb_ci;
#if defined(_DERIVATE)
  double xvf_Vbi_ci_Vs_ci;
  double xvf_Vbi_ci_Vbi_ei;
  double xvf_Vbi_ci_Vbi_ci;
  double xvf_Vbi_ci_Vtnode_GND;
  double xvf_Vbi_ci_Vb_ci;
  double xvf_Vbi_ei_Vs_ci;
  double xvf_Vbi_ei_Vbi_ei;
  double xvf_Vbi_ei_Vbi_ci;
  double xvf_Vbi_ei_Vtnode_GND;
  double xvf_Vbi_ei_Vb_ci;
#endif
#endif
#if defined(_DYNAMIC)
double qjcx;
#if defined(_DERIVATE)
double qjcx_Vbi_ci;
double qjcx_Vtnode_GND;
double qjcx_Vb_ci;
#endif
#endif
double qj3;
#if defined(_DERIVATE)
double qj3_Vci_ei;
double qj3_Vbi_ei;
double qj3_Vs_ci;
double qj3_Vbi_ci;
double qj3_Vtnode_GND;
double qj3_Vb_ci;
#endif
double qj2;
#if defined(_DERIVATE)
double qj2_Vci_ei;
double qj2_Vbi_ei;
double qj2_Vs_ci;
double qj2_Vbi_ci;
double qj2_Vtnode_GND;
double qj2_Vb_ci;
#endif
double qj1;
#if defined(_DERIVATE)
double qj1_Vci_ei;
double qj1_Vbi_ei;
double qj1_Vs_ci;
double qj1_Vbi_ci;
double qj1_Vtnode_GND;
double qj1_Vb_ci;
#endif
double vdj2;
#if defined(_DERIVATE)
double vdj2_Vci_ei;
double vdj2_Vbi_ei;
double vdj2_Vs_ci;
double vdj2_Vbi_ci;
double vdj2_Vtnode_GND;
double vdj2_Vb_ci;
#endif
double vdj1;
#if defined(_DERIVATE)
double vdj1_Vci_ei;
double vdj1_Vbi_ei;
double vdj1_Vs_ci;
double vdj1_Vbi_ci;
double vdj1_Vtnode_GND;
double vdj1_Vb_ci;
#endif
double ezr;
double ez;
double vj4;
#if defined(_DERIVATE)
double vj4_Vs_ci;
double vj4_Vci_ei;
double vj4_Vbi_ei;
double vj4_Vbi_ci;
double vj4_Vb_ci;
double vj4_Vtnode_GND;
#endif
double vj2;
#if defined(_DERIVATE)
double vj2_Vci_ei;
double vj2_Vbi_ei;
double vj2_Vs_ci;
double vj2_Vbi_ci;
double vj2_Vtnode_GND;
double vj2_Vb_ci;
#endif
double vr;
#if defined(_DERIVATE)
double vr_Vci_ei;
double vr_Vbi_ei;
double vr_Vs_ci;
double vr_Vbi_ci;
double vr_Vtnode_GND;
double vr_Vb_ci;
#endif
double vj1;
#if defined(_DERIVATE)
double vj1_Vci_ei;
double vj1_Vbi_ei;
double vj1_Vs_ci;
double vj1_Vbi_ci;
double vj1_Vtnode_GND;
double vj1_Vb_ci;
#endif
double ee1;
#if defined(_DERIVATE)
double ee1_Vs_ci;
double ee1_Vci_ei;
double ee1_Vbi_ei;
double ee1_Vbi_ci;
double ee1_Vtnode_GND;
double ee1_Vb_ci;
#endif
double ex1;
#if defined(_DERIVATE)
double ex1_Vs_ci;
double ex1_Vci_ei;
double ex1_Vbi_ei;
double ex1_Vbi_ci;
double ex1_Vtnode_GND;
double ex1_Vb_ci;
#endif
double ve;
#if defined(_DERIVATE)
double ve_Vs_ci;
double ve_Vci_ei;
double ve_Vbi_ei;
double ve_Vbi_ci;
double ve_Vtnode_GND;
double ve_Vb_ci;
#endif
double a;
#if defined(_DERIVATE)
double a_Vci_ei;
double a_Vbi_ei;
double a_Vbi_ci;
double a_Vb_ci;
double a_Vtnode_GND;
#endif
double cr;
#if defined(_DERIVATE)
double cr_Vtnode_GND;
#endif
double cmax;
#if defined(_DERIVATE)
double cmax_Vtnode_GND;
#endif
double vf;
#if defined(_DERIVATE)
double vf_Vtnode_GND;
#endif
double vp;
#if defined(_DERIVATE)
double vp_Vtnode_GND;
#endif
double zr;
double cjcx0_t_i;
#if defined(_DERIVATE)
double cjcx0_t_i_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double qjcxi;
#if defined(_DERIVATE)
double qjcxi_Vbi_ci;
double qjcxi_Vtnode_GND;
double qjcxi_Vb_ci;
#endif
#endif
double cjci0_t_ii;
#if defined(_DERIVATE)
double cjci0_t_ii_Vtnode_GND;
#endif
double kavl_t;
#if defined(_DERIVATE)
double kavl_t_Vtnode_GND;
#endif
double eavl_t;
#if defined(_DERIVATE)
double eavl_t_Vtnode_GND;
#endif
double re_t;
#if defined(_DERIVATE)
double re_t_Vtnode_GND;
#endif
double rbi0_t;
#if defined(_DERIVATE)
double rbi0_t_Vtnode_GND;
#endif
double rcx_t;
#if defined(_DERIVATE)
double rcx_t_Vtnode_GND;
#endif
double rbx_t;
#if defined(_DERIVATE)
double rbx_t_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double tef0_t;
#if defined(_DERIVATE)
double tef0_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double dvg;
#endif
#if defined(_DYNAMIC)
double zetatef;
#endif
#if defined(_DYNAMIC)
double thcs_t;
#if defined(_DERIVATE)
double thcs_t_Vtnode_GND;
#endif
#endif
#if defined(_DYNAMIC)
double t0_t;
#if defined(_DERIVATE)
double t0_t_Vtnode_GND;
#endif
#endif
double vces_t;
#if defined(_DERIVATE)
double vces_t_Vtnode_GND;
#endif
double vlim_t;
#if defined(_DERIVATE)
double vlim_t_Vtnode_GND;
#endif
double rci0_t;
#if defined(_DERIVATE)
double rci0_t_Vtnode_GND;
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
double cjcx0_t;
#if defined(_DERIVATE)
double cjcx0_t_Vtnode_GND;
#endif
double vdcx_t;
#if defined(_DERIVATE)
double vdcx_t_Vtnode_GND;
#endif
double cjci0_t;
#if defined(_DERIVATE)
double cjci0_t_Vtnode_GND;
#endif
double vdci_t;
#if defined(_DERIVATE)
double vdci_t_Vtnode_GND;
#endif
double aje_t;
#if defined(_DERIVATE)
double aje_t_Vtnode_GND;
#endif
double cje0_t;
#if defined(_DERIVATE)
double cje0_t_Vtnode_GND;
#endif
double vde_t;
#if defined(_DERIVATE)
double vde_t_Vtnode_GND;
#endif
double vdjt;
#if defined(_DERIVATE)
double vdjt_Vtnode_GND;
#endif
double vdj0;
double arg;
double iscs_t;
#if defined(_DERIVATE)
double iscs_t_Vtnode_GND;
#endif
double itss_t;
#if defined(_DERIVATE)
double itss_t_Vtnode_GND;
#endif
double ibcs_t;
#if defined(_DERIVATE)
double ibcs_t_Vtnode_GND;
#endif
double ires_t;
#if defined(_DERIVATE)
double ires_t_Vtnode_GND;
#endif
double ibes_t;
#if defined(_DERIVATE)
double ibes_t_Vtnode_GND;
#endif
double is_t;
#if defined(_DERIVATE)
double is_t_Vtnode_GND;
#endif
double zetasct;
double zetabci;
double mg;
#if defined(_DYNAMIC)
double vgsc;
#endif
double vgbc;
double vgbe;
double avs;
double k2;
double k1;
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
double VT0;
double Tdev;
#if defined(_DERIVATE)
double Tdev_Vtnode_GND;
#endif
double Tamb;
double Tnom;
double Vrth;
#if defined(_DERIVATE)
double Vrth_Vtnode_GND;
#endif
#if defined(_DYNAMIC)
double Vbe;
#if defined(_DERIVATE)
double Vbe_Vb_e;
#endif
#endif
double Vbbi;
#if defined(_DERIVATE)
double Vbbi_Vb_bi;
#endif
double Vcic;
#if defined(_DERIVATE)
double Vcic_Vci_c;
#endif
double Veie;
#if defined(_DERIVATE)
double Veie_Vei_e;
#endif
double Vsci;
#if defined(_DERIVATE)
double Vsci_Vs_ci;
#endif
double Vciei;
#if defined(_DERIVATE)
double Vciei_Vci_ei;
#endif
double Vbiei;
#if defined(_DERIVATEFORDDX)
double Vbiei_Vbi_ei;
#if defined(_DERIVATE)
  double Vbiei_Vbi_ei_Vbi_ei;
#endif
#endif
double Vbici;
#if defined(_DERIVATEFORDDX)
double Vbici_Vbi_ci;
#if defined(_DERIVATE)
  double Vbici_Vbi_ci_Vbi_ci;
#endif
#endif
double Vbci;
#if defined(_DERIVATE)
double Vbci_Vb_ci;
#endif
#if defined(_DERIVATE)
Vbci_Vb_ci=(HICUMtype);
#endif
Vbci=(HICUMtype*BP(b,ci));
#if defined(_DERIVATEFORDDX)
Vbici_Vbi_ci=(HICUMtype);
#if defined(_DERIVATE)
  Vbici_Vbi_ci_Vbi_ci=(+0.0*BP(bi,ci)+0.0*1.0+0.0*1.0+HICUMtype*0.0);
#endif
#endif
Vbici=(HICUMtype*BP(bi,ci));
#if defined(_DERIVATEFORDDX)
Vbiei_Vbi_ei=(HICUMtype);
#if defined(_DERIVATE)
  Vbiei_Vbi_ei_Vbi_ei=(+0.0*BP(bi,ei)+0.0*1.0+0.0*1.0+HICUMtype*0.0);
#endif
#endif
Vbiei=(HICUMtype*BP(bi,ei));
#if defined(_DERIVATE)
Vciei_Vci_ei=(HICUMtype);
#endif
Vciei=(HICUMtype*BP(ci,ei));
#if defined(_DERIVATE)
Vsci_Vs_ci=(HICUMtype);
#endif
Vsci=(HICUMtype*BP(s,ci));
#if defined(_DERIVATE)
Veie_Vei_e=1.0;
#endif
Veie=BP(ei,e);
#if defined(_DERIVATE)
Vcic_Vci_c=1.0;
#endif
Vcic=BP(ci,c);
#if defined(_DERIVATE)
Vbbi_Vb_bi=1.0;
#endif
Vbbi=BP(b,bi);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Vbe_Vb_e=(HICUMtype);
#endif
Vbe=(HICUMtype*BP(b,e));
#endif
#if defined(_DERIVATE)
Vrth_Vtnode_GND=1.0;
#endif
Vrth=NP(tnode);
Tnom=(tnom+273.15);
Tamb=_circuit_temp;
#if defined(_DERIVATE)
Tdev_Vtnode_GND=Vrth_Vtnode_GND;
#endif
Tdev=((Tamb+dt)+Vrth);
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
VT0=((1.3806503e-23*Tnom)/1.602176462e-19);
#if defined(_DERIVATE)
VT_Vtnode_GND=((1.3806503e-23*Tdev_Vtnode_GND)/1.602176462e-19);
#endif
VT=((1.3806503e-23*Tdev)/1.602176462e-19);
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
k1=(f1vg*Tnom);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,Tnom)
k2=((f2vg*Tnom)+(k1*d00_logE0));
}
#endif
avs=(alvs*Tnom);
vgbe=((vgb+vge)/2);
vgbc=((vgb+vgc)/2);
#if defined(_DYNAMIC)
vgsc=((vgs+vgc)/2);
#endif
mg=(3-((1.602176462e-19*f1vg)/1.3806503e-23));
zetabci=((mg+1)-zetaci);
zetasct=(mg-1.5);
{
double m00_exp(d00_exp0,((zetact*ln_qtt0)+((vgb/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
is_t_Vtnode_GND=(is*((zetact*ln_qtt0_Vtnode_GND)+(((-vgb*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((vgb/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
is_t=(is*d00_exp0);
}
{
double m00_exp(d00_exp0,((zetabet*ln_qtt0)+((vge/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibes_t_Vtnode_GND=(ibes*((zetabet*ln_qtt0_Vtnode_GND)+(((-vge*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((vge/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
ibes_t=(ibes*d00_exp0);
}
{
double m00_exp(d00_exp0,(((0.5*mg)*ln_qtt0)+(((0.5*vgbe)/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ires_t_Vtnode_GND=(ires*(((0.5*mg)*ln_qtt0_Vtnode_GND)+(((-(0.5*vgbe)*VT_Vtnode_GND/VT/VT)*(qtt0-1))+(((0.5*vgbe)/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
ires_t=(ires*d00_exp0);
}
{
double m00_exp(d00_exp0,((zetabci*ln_qtt0)+((vgc/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ibcs_t_Vtnode_GND=(ibcs*((zetabci*ln_qtt0_Vtnode_GND)+(((-vgc*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((vgc/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
ibcs_t=(ibcs*d00_exp0);
}
{
double m00_exp(d00_exp0,((zetasct*ln_qtt0)+((vgc/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
itss_t_Vtnode_GND=(itss*((zetasct*ln_qtt0_Vtnode_GND)+(((-vgc*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((vgc/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
itss_t=(itss*d00_exp0);
}
{
double m00_exp(d00_exp0,((zetasct*ln_qtt0)+((vgs/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
iscs_t_Vtnode_GND=(iscs*((zetasct*ln_qtt0_Vtnode_GND)+(((-vgs*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((vgs/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
iscs_t=(iscs*d00_exp0);
}
arg=((0.5*vde)/VT0);
{
double m00_exp(d00_exp0,arg)
double m00_exp(d00_exp1,(-arg))
double m00_logE(d00_logE2,(d00_exp0-d00_exp1))
vdj0=((2*VT0)*d00_logE2);
}
#if defined(_DERIVATE)
vdjt_Vtnode_GND=(((vdj0*qtt0_Vtnode_GND)+(vgbe*(-qtt0_Vtnode_GND)))-(((mg*VT_Vtnode_GND)*ln_qtt0)+((mg*VT)*ln_qtt0_Vtnode_GND)));
#endif
vdjt=(((vdj0*qtt0)+(vgbe*(1-qtt0)))-((mg*VT)*ln_qtt0));
{
double m00_exp(d00_exp0,((-vdjt)/VT))
double m00_sqrt(d00_sqrt1,(1+(4*d00_exp0)))
double m00_logE(d00_logE2,(0.5*(1+d00_sqrt1)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_sqrt(d10_sqrt1,d00_sqrt1,(1+(4*d00_exp0)))
double m10_logE(d10_logE2,d00_logE2,(0.5*(1+d00_sqrt1)))
#endif
#if defined(_DERIVATE)
vde_t_Vtnode_GND=(vdjt_Vtnode_GND+(((2*VT_Vtnode_GND)*d00_logE2)+((2*VT)*(0.5*(4*(((-vdjt_Vtnode_GND)*VT-(-vdjt)*VT_Vtnode_GND)/VT/VT)*d10_exp0)*d10_sqrt1)*d10_logE2)));
#endif
vde_t=(vdjt+((2*VT)*d00_logE2));
}
{
double m00_logE(d00_logE0,(vde/vde_t))
double m00_exp(d00_exp1,(ze*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vde/vde_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cje0_t_Vtnode_GND=(cje0*(ze*(-vde*vde_t_Vtnode_GND/vde_t/vde_t)*d10_logE0)*d10_exp1);
#endif
cje0_t=(cje0*d00_exp1);
}
#if defined(_DERIVATE)
aje_t_Vtnode_GND=((aje*vde_t_Vtnode_GND)/vde);
#endif
aje_t=((aje*vde_t)/vde);
arg=((0.5*vdci)/VT0);
{
double m00_exp(d00_exp0,arg)
double m00_exp(d00_exp1,(-arg))
double m00_logE(d00_logE2,(d00_exp0-d00_exp1))
vdj0=((2*VT0)*d00_logE2);
}
#if defined(_DERIVATE)
vdjt_Vtnode_GND=(((vdj0*qtt0_Vtnode_GND)+(vgbc*(-qtt0_Vtnode_GND)))-(((mg*VT_Vtnode_GND)*ln_qtt0)+((mg*VT)*ln_qtt0_Vtnode_GND)));
#endif
vdjt=(((vdj0*qtt0)+(vgbc*(1-qtt0)))-((mg*VT)*ln_qtt0));
{
double m00_exp(d00_exp0,((-vdjt)/VT))
double m00_sqrt(d00_sqrt1,(1+(4*d00_exp0)))
double m00_logE(d00_logE2,(0.5*(1+d00_sqrt1)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_sqrt(d10_sqrt1,d00_sqrt1,(1+(4*d00_exp0)))
double m10_logE(d10_logE2,d00_logE2,(0.5*(1+d00_sqrt1)))
#endif
#if defined(_DERIVATE)
vdci_t_Vtnode_GND=(vdjt_Vtnode_GND+(((2*VT_Vtnode_GND)*d00_logE2)+((2*VT)*(0.5*(4*(((-vdjt_Vtnode_GND)*VT-(-vdjt)*VT_Vtnode_GND)/VT/VT)*d10_exp0)*d10_sqrt1)*d10_logE2)));
#endif
vdci_t=(vdjt+((2*VT)*d00_logE2));
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
arg=((0.5*vdcx)/VT0);
{
double m00_exp(d00_exp0,arg)
double m00_exp(d00_exp1,(-arg))
double m00_logE(d00_logE2,(d00_exp0-d00_exp1))
vdj0=((2*VT0)*d00_logE2);
}
#if defined(_DERIVATE)
vdjt_Vtnode_GND=(((vdj0*qtt0_Vtnode_GND)+(vgbc*(-qtt0_Vtnode_GND)))-(((mg*VT_Vtnode_GND)*ln_qtt0)+((mg*VT)*ln_qtt0_Vtnode_GND)));
#endif
vdjt=(((vdj0*qtt0)+(vgbc*(1-qtt0)))-((mg*VT)*ln_qtt0));
{
double m00_exp(d00_exp0,((-vdjt)/VT))
double m00_sqrt(d00_sqrt1,(1+(4*d00_exp0)))
double m00_logE(d00_logE2,(0.5*(1+d00_sqrt1)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_sqrt(d10_sqrt1,d00_sqrt1,(1+(4*d00_exp0)))
double m10_logE(d10_logE2,d00_logE2,(0.5*(1+d00_sqrt1)))
#endif
#if defined(_DERIVATE)
vdcx_t_Vtnode_GND=(vdjt_Vtnode_GND+(((2*VT_Vtnode_GND)*d00_logE2)+((2*VT)*(0.5*(4*(((-vdjt_Vtnode_GND)*VT-(-vdjt)*VT_Vtnode_GND)/VT/VT)*d10_exp0)*d10_sqrt1)*d10_logE2)));
#endif
vdcx_t=(vdjt+((2*VT)*d00_logE2));
}
{
double m00_logE(d00_logE0,(vdcx/vdcx_t))
double m00_exp(d00_exp1,(zcx*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdcx/vdcx_t))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjcx0_t_Vtnode_GND=(cjcx0*(zcx*(-vdcx*vdcx_t_Vtnode_GND/vdcx_t/vdcx_t)*d10_logE0)*d10_exp1);
#endif
cjcx0_t=(cjcx0*d00_exp1);
}
#if defined(_DYNAMIC)
arg=((0.5*vds)/VT0);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,arg)
double m00_exp(d00_exp1,(-arg))
double m00_logE(d00_logE2,(d00_exp0-d00_exp1))
vdj0=((2*VT0)*d00_logE2);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vdjt_Vtnode_GND=(((vdj0*qtt0_Vtnode_GND)+(vgsc*(-qtt0_Vtnode_GND)))-(((mg*VT_Vtnode_GND)*ln_qtt0)+((mg*VT)*ln_qtt0_Vtnode_GND)));
#endif
vdjt=(((vdj0*qtt0)+(vgsc*(1-qtt0)))-((mg*VT)*ln_qtt0));
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((-vdjt)/VT))
double m00_sqrt(d00_sqrt1,(1+(4*d00_exp0)))
double m00_logE(d00_logE2,(0.5*(1+d00_sqrt1)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_sqrt(d10_sqrt1,d00_sqrt1,(1+(4*d00_exp0)))
double m10_logE(d10_logE2,d00_logE2,(0.5*(1+d00_sqrt1)))
#endif
#if defined(_DERIVATE)
vds_t_Vtnode_GND=(vdjt_Vtnode_GND+(((2*VT_Vtnode_GND)*d00_logE2)+((2*VT)*(0.5*(4*(((-vdjt_Vtnode_GND)*VT-(-vdjt)*VT_Vtnode_GND)/VT/VT)*d10_exp0)*d10_sqrt1)*d10_logE2)));
#endif
vds_t=(vdjt+((2*VT)*d00_logE2));
}
#endif
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
{
double m00_exp(d00_exp0,(zetaci*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
rci0_t_Vtnode_GND=(rci0*(zetaci*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
rci0_t=(rci0*d00_exp0);
}
{
double m00_exp(d00_exp0,((zetaci-avs)*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
vlim_t_Vtnode_GND=(vlim*((zetaci-avs)*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
vlim_t=(vlim*d00_exp0);
}
#if defined(_DERIVATE)
vces_t_Vtnode_GND=(vces*(alces*dT_Vtnode_GND));
#endif
vces_t=(vces*(1+(alces*dT)));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
t0_t_Vtnode_GND=(t0*((alt0*dT_Vtnode_GND)+(((kt0*dT_Vtnode_GND)*dT)+((kt0*dT)*dT_Vtnode_GND))));
#endif
t0_t=(t0*((1+(alt0*dT))+((kt0*dT)*dT)));
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((zetaci-1)*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
thcs_t_Vtnode_GND=(thcs*((zetaci-1)*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
thcs_t=(thcs*d00_exp0);
}
#endif
#if defined(_DYNAMIC)
zetatef=((zetabet-zetact)-0.5);
#endif
#if defined(_DYNAMIC)
dvg=(vgb-vge);
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,((zetatef*ln_qtt0)-((dvg/VT)*(qtt0-1))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
tef0_t_Vtnode_GND=(tef0*((zetatef*ln_qtt0_Vtnode_GND)-(((-dvg*VT_Vtnode_GND/VT/VT)*(qtt0-1))+((dvg/VT)*qtt0_Vtnode_GND)))*d10_exp0);
#endif
tef0_t=(tef0*d00_exp0);
}
#endif
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
double m00_exp(d00_exp0,(zetarbi*ln_qtt0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
rbi0_t_Vtnode_GND=(rbi0*(zetarbi*ln_qtt0_Vtnode_GND)*d10_exp0);
#endif
rbi0_t=(rbi0*d00_exp0);
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
{
double m00_exp(d00_exp0,(aleav*dT))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
eavl_t_Vtnode_GND=(eavl*(aleav*dT_Vtnode_GND)*d10_exp0);
#endif
eavl_t=(eavl*d00_exp0);
}
{
double m00_exp(d00_exp0,(alkav*dT))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
kavl_t_Vtnode_GND=(kavl*(alkav*dT_Vtnode_GND)*d10_exp0);
#endif
kavl_t=(kavl*d00_exp0);
}
if
((cjcx0_t==0))
{
#if defined(_DERIVATE)
cjci0_t_ii_Vtnode_GND=cjci0_t_Vtnode_GND*fbc;
#endif
cjci0_t_ii=(cjci0_t*fbc);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcxi_Vbi_ci=0.0;
qjcxi_Vtnode_GND=0.0;
qjcxi_Vb_ci=0.0;
#endif
qjcxi=0;
#endif
#if defined(_DERIVATE)
cjcx0_t_i_Vtnode_GND=cjci0_t_Vtnode_GND*(1-fbc);
#endif
cjcx0_t_i=(cjci0_t*(1-fbc));
if
((vptci<1.0e2))
{
if
((cjcx0_t_i>0.0))
{
zr=(zci/4.0);
#if defined(_DERIVATE)
vp_Vtnode_GND=(-vdci_t_Vtnode_GND);
#endif
vp=(vptci-vdci_t);
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
cmax_Vtnode_GND=(2.4*cjcx0_t_i_Vtnode_GND);
#endif
cmax=(2.4*cjcx0_t_i);
{
double m00_logE(d00_logE0,(vdci_t/vptci))
double m00_exp(d00_exp1,((zci-zr)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdci_t/vptci))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cr_Vtnode_GND=((cjcx0_t_i_Vtnode_GND*d00_exp1)+(cjcx0_t_i*((zci-zr)*(vdci_t_Vtnode_GND/vptci)*d10_logE0)*d10_exp1));
#endif
cr=(cjcx0_t_i*d00_exp1);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=VT_Vtnode_GND;
#endif
a=VT;
#if defined(_DERIVATE)
ve_Vs_ci=0.0;
ve_Vci_ei=0.0;
ve_Vbi_ei=0.0;
ve_Vbi_ci=0.0;
ve_Vtnode_GND=((vf_Vtnode_GND*a-(vf-Vbci)*a_Vtnode_GND)/a/a);
ve_Vb_ci=(((-Vbci_Vb_ci)*a-(vf-Vbci)*a_Vb_ci)/a/a);
#endif
ve=((vf-Vbci)/a);
if
((ve<=80.0))
{
{
double m00_exp(d00_exp0,ve)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=0.0;
ex1_Vtnode_GND=ve_Vtnode_GND*d10_exp0;
ex1_Vb_ci=ve_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=0.0;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=0.0;
vj1_Vtnode_GND=(vf_Vtnode_GND-((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj1_Vb_ci=(-((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0)));
#endif
vj1=(vf-(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=0.0;
vj1_Vtnode_GND=0.0;
vj1_Vb_ci=Vbci_Vb_ci;
#endif
vj1=Vbci;
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=((0.1*vp_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
a=((0.1*vp)+(4.0*VT));
#if defined(_DERIVATE)
vr_Vci_ei=0.0;
vr_Vbi_ei=0.0;
vr_Vs_ci=0.0;
vr_Vbi_ci=0.0;
vr_Vtnode_GND=(((vp_Vtnode_GND+vj1_Vtnode_GND)*a-(vp+vj1)*a_Vtnode_GND)/a/a);
vr_Vb_ci=((vj1_Vb_ci*a-(vp+vj1)*a_Vb_ci)/a/a);
#endif
vr=((vp+vj1)/a);
if
((vr<=80.0))
{
{
double m00_exp(d00_exp0,vr)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=0.0;
ex1_Vtnode_GND=vr_Vtnode_GND*d10_exp0;
ex1_Vb_ci=vr_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=0.0;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=0.0;
vj2_Vtnode_GND=((-vp_Vtnode_GND)+((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj2_Vb_ci=((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0));
#endif
vj2=((-vp)+(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=0.0;
vj2_Vtnode_GND=vj1_Vtnode_GND;
vj2_Vb_ci=vj1_Vb_ci;
#endif
vj2=vj1;
}
#if defined(_DERIVATE)
vj4_Vs_ci=0.0;
vj4_Vci_ei=0.0;
vj4_Vbi_ei=0.0;
vj4_Vbi_ci=0.0;
vj4_Vb_ci=(Vbci_Vb_ci-vj1_Vb_ci);
vj4_Vtnode_GND=(-vj1_Vtnode_GND);
#endif
vj4=(Vbci-vj1);
ez=(1.0-zci);
ezr=(1.0-zr);
{
double m00_logE(d00_logE0,(1.0-(vj1/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj1/vdci_t)))
#endif
#if defined(_DERIVATE)
vdj1_Vci_ei=0.0;
vdj1_Vbi_ei=0.0;
vdj1_Vs_ci=0.0;
vdj1_Vbi_ci=0.0;
vdj1_Vtnode_GND=(-((vj1_Vtnode_GND*vdci_t-vj1*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
vdj1_Vb_ci=(-(vj1_Vb_ci/vdci_t))*d10_logE0;
#endif
vdj1=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0-(vj2/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj2/vdci_t)))
#endif
#if defined(_DERIVATE)
vdj2_Vci_ei=0.0;
vdj2_Vbi_ei=0.0;
vdj2_Vs_ci=0.0;
vdj2_Vbi_ci=0.0;
vdj2_Vtnode_GND=(-((vj2_Vtnode_GND*vdci_t-vj2*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
vdj2_Vb_ci=(-(vj2_Vb_ci/vdci_t))*d10_logE0;
#endif
vdj2=d00_logE0;
}
{
double m00_exp(d00_exp0,(vdj2*ez))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj1_Vci_ei=0.0;
qj1_Vbi_ei=0.0;
qj1_Vs_ci=0.0;
qj1_Vbi_ci=0.0;
qj1_Vtnode_GND=(((cjcx0_t_i_Vtnode_GND*(1.0-d00_exp0))+(cjcx0_t_i*(-vdj2_Vtnode_GND*ez*d10_exp0)))/ez);
qj1_Vb_ci=((cjcx0_t_i*(-vdj2_Vb_ci*ez*d10_exp0))/ez);
#endif
qj1=((cjcx0_t_i*(1.0-d00_exp0))/ez);
}
{
double m00_exp(d00_exp0,(vdj1*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj2_Vci_ei=0.0;
qj2_Vbi_ei=0.0;
qj2_Vs_ci=0.0;
qj2_Vbi_ci=0.0;
qj2_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj1_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj2_Vb_ci=((cr*(-vdj1_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj2=((cr*(1.0-d00_exp0))/ezr);
}
{
double m00_exp(d00_exp0,(vdj2*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj3_Vci_ei=0.0;
qj3_Vbi_ei=0.0;
qj3_Vs_ci=0.0;
qj3_Vbi_ci=0.0;
qj3_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj2_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj3_Vb_ci=((cr*(-vdj2_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj3=((cr*(1.0-d00_exp0))/ezr);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=(((((qj1_Vtnode_GND+qj2_Vtnode_GND)-qj3_Vtnode_GND)*vdci_t)+(((qj1+qj2)-qj3)*vdci_t_Vtnode_GND))+((cmax_Vtnode_GND*vj4)+(cmax*vj4_Vtnode_GND)));
qjcx_Vb_ci=(((qj1_Vb_ci+qj2_Vb_ci)-qj3_Vb_ci)*vdci_t+(cmax*vj4_Vb_ci));
#endif
qjcx=((((qj1+qj2)-qj3)*vdci_t)+(cmax*vj4));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=0.0;
qjcx_Vb_ci=0.0;
#endif
qjcx=0.0;
#endif
}
}
else
{
if
((cjcx0_t_i>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=0.0;
  xvf_Vbi_ci_Vbi_ei=0.0;
  xvf_Vbi_ci_Vbi_ci=0.0;
  xvf_Vbi_ci_Vtnode_GND=0.0;
  xvf_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbci)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=((-Vbci_Vb_ci)/VT);
#endif
#endif
xvf=((vf-Vbci)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=0.0;
  xvf2_Vbi_ei_Vbi_ei=0.0;
  xvf2_Vbi_ei_Vbi_ci=0.0;
  xvf2_Vbi_ei_Vtnode_GND=0.0;
  xvf2_Vbi_ei_Vb_ci=0.0;
#endif
xvf2_Vbi_ci=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=0.0;
  xvf2_Vbi_ci_Vbi_ei=0.0;
  xvf2_Vbi_ci_Vbi_ci=0.0;
  xvf2_Vbi_ci_Vtnode_GND=0.0;
  xvf2_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=0.0;
  v_j_Vbi_ei_Vbi_ei=0.0;
  v_j_Vbi_ei_Vbi_ci=0.0;
  v_j_Vbi_ei_Vtnode_GND=0.0;
  v_j_Vbi_ei_Vb_ci=0.0;
#endif
v_j_Vbi_ci=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=0.0;
  v_j_Vbi_ci_Vbi_ei=0.0;
  v_j_Vbi_ci_Vbi_ci=0.0;
  v_j_Vbi_ci_Vtnode_GND=0.0;
  v_j_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-zci);
{
double m00_logE(d00_logE0,(1.0-(v_j/vdci_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vdci_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=0.0;
#endif
#if defined(_DERIVATE)
y_Vs_ci=0.0;
#endif
y_Vbi_ei=0.0;
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=0.0;
  y_Vbi_ei_Vs_ci=0.0;
  y_Vbi_ei_Vbi_ei=0.0;
  y_Vbi_ei_Vbi_ci=0.0;
  y_Vbi_ei_Vtnode_GND=0.0;
  y_Vbi_ei_Vb_ci=0.0;
#endif
y_Vbi_ci=0.0;
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=0.0;
  y_Vbi_ci_Vs_ci=0.0;
  y_Vbi_ci_Vbi_ei=0.0;
  y_Vbi_ci_Vbi_ci=0.0;
  y_Vbi_ci_Vtnode_GND=0.0;
  y_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vdci_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=(((((((cjcx0_t_i_Vtnode_GND*vdci_t)+(cjcx0_t_i*vdci_t_Vtnode_GND))*y)+((cjcx0_t_i*vdci_t)*y_Vtnode_GND))*x-((cjcx0_t_i*vdci_t)*y)*x_Vtnode_GND)/x/x)+(((2.4*cjcx0_t_i_Vtnode_GND)*(Vbci-v_j))+((2.4*cjcx0_t_i)*(-v_j_Vtnode_GND))));
qjcx_Vb_ci=((((cjcx0_t_i*vdci_t)*y_Vb_ci)/x)+((2.4*cjcx0_t_i)*(Vbci_Vb_ci-v_j_Vb_ci)));
#endif
qjcx=((((cjcx0_t_i*vdci_t)*y)/x)+((2.4*cjcx0_t_i)*(Vbci-v_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=0.0;
qjcx_Vb_ci=0.0;
#endif
qjcx=0.0;
#endif
}
}
}
else
{
#if defined(_DERIVATE)
cjci0_t_ii_Vtnode_GND=cjci0_t_Vtnode_GND;
#endif
cjci0_t_ii=cjci0_t;
#if defined(_DERIVATE)
cjcx0_t_ii_Vtnode_GND=cjcx0_t_Vtnode_GND*fbc;
#endif
cjcx0_t_ii=(cjcx0_t*fbc);
if
((vptcx<1.0e2))
{
if
((cjcx0_t_ii>0.0))
{
zr=(zcx/4.0);
#if defined(_DERIVATE)
vp_Vtnode_GND=(-vdcx_t_Vtnode_GND);
#endif
vp=(vptcx-vdcx_t);
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdcx_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
cmax_Vtnode_GND=(2.4*cjcx0_t_ii_Vtnode_GND);
#endif
cmax=(2.4*cjcx0_t_ii);
{
double m00_logE(d00_logE0,(vdcx_t/vptcx))
double m00_exp(d00_exp1,((zcx-zr)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdcx_t/vptcx))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cr_Vtnode_GND=((cjcx0_t_ii_Vtnode_GND*d00_exp1)+(cjcx0_t_ii*((zcx-zr)*(vdcx_t_Vtnode_GND/vptcx)*d10_logE0)*d10_exp1));
#endif
cr=(cjcx0_t_ii*d00_exp1);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=VT_Vtnode_GND;
#endif
a=VT;
#if defined(_DERIVATE)
ve_Vs_ci=0.0;
ve_Vci_ei=0.0;
ve_Vbi_ei=0.0;
ve_Vbi_ci=(((-Vbici_Vbi_ci)*a-(vf-Vbici)*a_Vbi_ci)/a/a);
ve_Vtnode_GND=((vf_Vtnode_GND*a-(vf-Vbici)*a_Vtnode_GND)/a/a);
ve_Vb_ci=0.0;
#endif
ve=((vf-Vbici)/a);
if
((ve<=80.0))
{
{
double m00_exp(d00_exp0,ve)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=ve_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=ve_Vtnode_GND*d10_exp0;
ex1_Vb_ci=ve_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=(-((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0)));
vj1_Vtnode_GND=(vf_Vtnode_GND-((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj1_Vb_ci=(-((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0)));
#endif
vj1=(vf-(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=Vbici_Vbi_ci;
vj1_Vtnode_GND=0.0;
vj1_Vb_ci=0.0;
#endif
vj1=Vbici;
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=((0.1*vp_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
a=((0.1*vp)+(4.0*VT));
#if defined(_DERIVATE)
vr_Vci_ei=0.0;
vr_Vbi_ei=0.0;
vr_Vs_ci=0.0;
vr_Vbi_ci=((vj1_Vbi_ci*a-(vp+vj1)*a_Vbi_ci)/a/a);
vr_Vtnode_GND=(((vp_Vtnode_GND+vj1_Vtnode_GND)*a-(vp+vj1)*a_Vtnode_GND)/a/a);
vr_Vb_ci=((vj1_Vb_ci*a-(vp+vj1)*a_Vb_ci)/a/a);
#endif
vr=((vp+vj1)/a);
if
((vr<=80.0))
{
{
double m00_exp(d00_exp0,vr)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=vr_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=vr_Vtnode_GND*d10_exp0;
ex1_Vb_ci=vr_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0));
vj2_Vtnode_GND=((-vp_Vtnode_GND)+((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj2_Vb_ci=((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0));
#endif
vj2=((-vp)+(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=vj1_Vbi_ci;
vj2_Vtnode_GND=vj1_Vtnode_GND;
vj2_Vb_ci=vj1_Vb_ci;
#endif
vj2=vj1;
}
#if defined(_DERIVATE)
vj4_Vs_ci=0.0;
vj4_Vci_ei=0.0;
vj4_Vbi_ei=0.0;
vj4_Vbi_ci=(Vbici_Vbi_ci-vj1_Vbi_ci);
vj4_Vb_ci=(-vj1_Vb_ci);
vj4_Vtnode_GND=(-vj1_Vtnode_GND);
#endif
vj4=(Vbici-vj1);
ez=(1.0-zcx);
ezr=(1.0-zr);
{
double m00_logE(d00_logE0,(1.0-(vj1/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj1/vdcx_t)))
#endif
#if defined(_DERIVATE)
vdj1_Vci_ei=0.0;
vdj1_Vbi_ei=0.0;
vdj1_Vs_ci=0.0;
vdj1_Vbi_ci=(-(vj1_Vbi_ci/vdcx_t))*d10_logE0;
vdj1_Vtnode_GND=(-((vj1_Vtnode_GND*vdcx_t-vj1*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
vdj1_Vb_ci=(-(vj1_Vb_ci/vdcx_t))*d10_logE0;
#endif
vdj1=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0-(vj2/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj2/vdcx_t)))
#endif
#if defined(_DERIVATE)
vdj2_Vci_ei=0.0;
vdj2_Vbi_ei=0.0;
vdj2_Vs_ci=0.0;
vdj2_Vbi_ci=(-(vj2_Vbi_ci/vdcx_t))*d10_logE0;
vdj2_Vtnode_GND=(-((vj2_Vtnode_GND*vdcx_t-vj2*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
vdj2_Vb_ci=(-(vj2_Vb_ci/vdcx_t))*d10_logE0;
#endif
vdj2=d00_logE0;
}
{
double m00_exp(d00_exp0,(vdj2*ez))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj1_Vci_ei=0.0;
qj1_Vbi_ei=0.0;
qj1_Vs_ci=0.0;
qj1_Vbi_ci=((cjcx0_t_ii*(-vdj2_Vbi_ci*ez*d10_exp0))/ez);
qj1_Vtnode_GND=(((cjcx0_t_ii_Vtnode_GND*(1.0-d00_exp0))+(cjcx0_t_ii*(-vdj2_Vtnode_GND*ez*d10_exp0)))/ez);
qj1_Vb_ci=((cjcx0_t_ii*(-vdj2_Vb_ci*ez*d10_exp0))/ez);
#endif
qj1=((cjcx0_t_ii*(1.0-d00_exp0))/ez);
}
{
double m00_exp(d00_exp0,(vdj1*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj2_Vci_ei=0.0;
qj2_Vbi_ei=0.0;
qj2_Vs_ci=0.0;
qj2_Vbi_ci=((cr*(-vdj1_Vbi_ci*ezr*d10_exp0))/ezr);
qj2_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj1_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj2_Vb_ci=((cr*(-vdj1_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj2=((cr*(1.0-d00_exp0))/ezr);
}
{
double m00_exp(d00_exp0,(vdj2*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj3_Vci_ei=0.0;
qj3_Vbi_ei=0.0;
qj3_Vs_ci=0.0;
qj3_Vbi_ci=((cr*(-vdj2_Vbi_ci*ezr*d10_exp0))/ezr);
qj3_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj2_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj3_Vb_ci=((cr*(-vdj2_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj3=((cr*(1.0-d00_exp0))/ezr);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcxi_Vbi_ci=(((qj1_Vbi_ci+qj2_Vbi_ci)-qj3_Vbi_ci)*vdcx_t+(cmax*vj4_Vbi_ci));
qjcxi_Vtnode_GND=(((((qj1_Vtnode_GND+qj2_Vtnode_GND)-qj3_Vtnode_GND)*vdcx_t)+(((qj1+qj2)-qj3)*vdcx_t_Vtnode_GND))+((cmax_Vtnode_GND*vj4)+(cmax*vj4_Vtnode_GND)));
qjcxi_Vb_ci=(((qj1_Vb_ci+qj2_Vb_ci)-qj3_Vb_ci)*vdcx_t+(cmax*vj4_Vb_ci));
#endif
qjcxi=((((qj1+qj2)-qj3)*vdcx_t)+(cmax*vj4));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcxi_Vbi_ci=0.0;
qjcxi_Vtnode_GND=0.0;
qjcxi_Vb_ci=0.0;
#endif
qjcxi=0.0;
#endif
}
}
else
{
if
((cjcx0_t_ii>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdcx_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=((-Vbici_Vbi_ci)/VT);
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ei=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ci=((+0.0-Vbici_Vbi_ci_Vbi_ci)/VT-((-Vbici_Vbi_ci)*0.0+(-Vbici_Vbi_ci)*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vtnode_GND=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*VT_Vtnode_GND+vf_Vtnode_GND*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*VT_Vtnode_GND/VT/VT/VT);
  xvf_Vbi_ci_Vb_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbici)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=0.0;
#endif
#endif
xvf=((vf-Vbici)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=0.0;
  xvf2_Vbi_ei_Vbi_ei=0.0;
  xvf2_Vbi_ei_Vbi_ci=0.0;
  xvf2_Vbi_ei_Vtnode_GND=0.0;
  xvf2_Vbi_ei_Vb_ci=0.0;
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=0.0;
  v_j_Vbi_ei_Vbi_ei=0.0;
  v_j_Vbi_ei_Vbi_ci=0.0;
  v_j_Vbi_ei_Vtnode_GND=0.0;
  v_j_Vbi_ei_Vb_ci=0.0;
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-zcx);
{
double m00_logE(d00_logE0,(1.0-(v_j/vdcx_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vdcx_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=0.0;
#endif
#if defined(_DERIVATE)
y_Vs_ci=0.0;
#endif
y_Vbi_ei=0.0;
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=0.0;
  y_Vbi_ei_Vs_ci=0.0;
  y_Vbi_ei_Vbi_ei=0.0;
  y_Vbi_ei_Vbi_ci=0.0;
  y_Vbi_ei_Vtnode_GND=0.0;
  y_Vbi_ei_Vb_ci=0.0;
#endif
y_Vbi_ci=(-(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*0.0)));
  y_Vbi_ci_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vs_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vs_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vbi_ei/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ei/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vbi_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0)))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vtnode_GND/vdcx_t-(v_j_Vbi_ci*vdcx_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*vdcx_t_Vtnode_GND/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vb_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vb_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vdcx_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcxi_Vbi_ci=((((cjcx0_t_ii*vdcx_t)*y_Vbi_ci)/x)+((2.4*cjcx0_t_ii)*(Vbici_Vbi_ci-v_j_Vbi_ci)));
qjcxi_Vtnode_GND=(((((((cjcx0_t_ii_Vtnode_GND*vdcx_t)+(cjcx0_t_ii*vdcx_t_Vtnode_GND))*y)+((cjcx0_t_ii*vdcx_t)*y_Vtnode_GND))*x-((cjcx0_t_ii*vdcx_t)*y)*x_Vtnode_GND)/x/x)+(((2.4*cjcx0_t_ii_Vtnode_GND)*(Vbici-v_j))+((2.4*cjcx0_t_ii)*(-v_j_Vtnode_GND))));
qjcxi_Vb_ci=((((cjcx0_t_ii*vdcx_t)*y_Vb_ci)/x)+((2.4*cjcx0_t_ii)*(-v_j_Vb_ci)));
#endif
qjcxi=((((cjcx0_t_ii*vdcx_t)*y)/x)+((2.4*cjcx0_t_ii)*(Vbici-v_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcxi_Vbi_ci=0.0;
qjcxi_Vtnode_GND=0.0;
qjcxi_Vb_ci=0.0;
#endif
qjcxi=0.0;
#endif
}
}
#if defined(_DERIVATE)
cjcx0_t_i_Vtnode_GND=cjcx0_t_Vtnode_GND*(1-fbc);
#endif
cjcx0_t_i=(cjcx0_t*(1-fbc));
if
((vptcx<1.0e2))
{
if
((cjcx0_t_i>0.0))
{
zr=(zcx/4.0);
#if defined(_DERIVATE)
vp_Vtnode_GND=(-vdcx_t_Vtnode_GND);
#endif
vp=(vptcx-vdcx_t);
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdcx_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
cmax_Vtnode_GND=(2.4*cjcx0_t_i_Vtnode_GND);
#endif
cmax=(2.4*cjcx0_t_i);
{
double m00_logE(d00_logE0,(vdcx_t/vptcx))
double m00_exp(d00_exp1,((zcx-zr)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdcx_t/vptcx))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cr_Vtnode_GND=((cjcx0_t_i_Vtnode_GND*d00_exp1)+(cjcx0_t_i*((zcx-zr)*(vdcx_t_Vtnode_GND/vptcx)*d10_logE0)*d10_exp1));
#endif
cr=(cjcx0_t_i*d00_exp1);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=VT_Vtnode_GND;
#endif
a=VT;
#if defined(_DERIVATE)
ve_Vs_ci=0.0;
ve_Vci_ei=0.0;
ve_Vbi_ei=0.0;
ve_Vbi_ci=0.0;
ve_Vtnode_GND=((vf_Vtnode_GND*a-(vf-Vbci)*a_Vtnode_GND)/a/a);
ve_Vb_ci=(((-Vbci_Vb_ci)*a-(vf-Vbci)*a_Vb_ci)/a/a);
#endif
ve=((vf-Vbci)/a);
if
((ve<=80.0))
{
{
double m00_exp(d00_exp0,ve)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=ve_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=ve_Vtnode_GND*d10_exp0;
ex1_Vb_ci=ve_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=(-((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0)));
vj1_Vtnode_GND=(vf_Vtnode_GND-((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj1_Vb_ci=(-((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0)));
#endif
vj1=(vf-(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=0.0;
vj1_Vtnode_GND=0.0;
vj1_Vb_ci=Vbci_Vb_ci;
#endif
vj1=Vbci;
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=((0.1*vp_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
a=((0.1*vp)+(4.0*VT));
#if defined(_DERIVATE)
vr_Vci_ei=0.0;
vr_Vbi_ei=0.0;
vr_Vs_ci=0.0;
vr_Vbi_ci=((vj1_Vbi_ci*a-(vp+vj1)*a_Vbi_ci)/a/a);
vr_Vtnode_GND=(((vp_Vtnode_GND+vj1_Vtnode_GND)*a-(vp+vj1)*a_Vtnode_GND)/a/a);
vr_Vb_ci=((vj1_Vb_ci*a-(vp+vj1)*a_Vb_ci)/a/a);
#endif
vr=((vp+vj1)/a);
if
((vr<=80.0))
{
{
double m00_exp(d00_exp0,vr)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=vr_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=vr_Vtnode_GND*d10_exp0;
ex1_Vb_ci=vr_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0));
vj2_Vtnode_GND=((-vp_Vtnode_GND)+((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj2_Vb_ci=((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0));
#endif
vj2=((-vp)+(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=vj1_Vbi_ci;
vj2_Vtnode_GND=vj1_Vtnode_GND;
vj2_Vb_ci=vj1_Vb_ci;
#endif
vj2=vj1;
}
#if defined(_DERIVATE)
vj4_Vs_ci=0.0;
vj4_Vci_ei=0.0;
vj4_Vbi_ei=0.0;
vj4_Vbi_ci=(-vj1_Vbi_ci);
vj4_Vb_ci=(Vbci_Vb_ci-vj1_Vb_ci);
vj4_Vtnode_GND=(-vj1_Vtnode_GND);
#endif
vj4=(Vbci-vj1);
ez=(1.0-zcx);
ezr=(1.0-zr);
{
double m00_logE(d00_logE0,(1.0-(vj1/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj1/vdcx_t)))
#endif
#if defined(_DERIVATE)
vdj1_Vci_ei=0.0;
vdj1_Vbi_ei=0.0;
vdj1_Vs_ci=0.0;
vdj1_Vbi_ci=(-(vj1_Vbi_ci/vdcx_t))*d10_logE0;
vdj1_Vtnode_GND=(-((vj1_Vtnode_GND*vdcx_t-vj1*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
vdj1_Vb_ci=(-(vj1_Vb_ci/vdcx_t))*d10_logE0;
#endif
vdj1=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0-(vj2/vdcx_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj2/vdcx_t)))
#endif
#if defined(_DERIVATE)
vdj2_Vci_ei=0.0;
vdj2_Vbi_ei=0.0;
vdj2_Vs_ci=0.0;
vdj2_Vbi_ci=(-(vj2_Vbi_ci/vdcx_t))*d10_logE0;
vdj2_Vtnode_GND=(-((vj2_Vtnode_GND*vdcx_t-vj2*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0;
vdj2_Vb_ci=(-(vj2_Vb_ci/vdcx_t))*d10_logE0;
#endif
vdj2=d00_logE0;
}
{
double m00_exp(d00_exp0,(vdj2*ez))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj1_Vci_ei=0.0;
qj1_Vbi_ei=0.0;
qj1_Vs_ci=0.0;
qj1_Vbi_ci=((cjcx0_t_i*(-vdj2_Vbi_ci*ez*d10_exp0))/ez);
qj1_Vtnode_GND=(((cjcx0_t_i_Vtnode_GND*(1.0-d00_exp0))+(cjcx0_t_i*(-vdj2_Vtnode_GND*ez*d10_exp0)))/ez);
qj1_Vb_ci=((cjcx0_t_i*(-vdj2_Vb_ci*ez*d10_exp0))/ez);
#endif
qj1=((cjcx0_t_i*(1.0-d00_exp0))/ez);
}
{
double m00_exp(d00_exp0,(vdj1*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj2_Vci_ei=0.0;
qj2_Vbi_ei=0.0;
qj2_Vs_ci=0.0;
qj2_Vbi_ci=((cr*(-vdj1_Vbi_ci*ezr*d10_exp0))/ezr);
qj2_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj1_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj2_Vb_ci=((cr*(-vdj1_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj2=((cr*(1.0-d00_exp0))/ezr);
}
{
double m00_exp(d00_exp0,(vdj2*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj3_Vci_ei=0.0;
qj3_Vbi_ei=0.0;
qj3_Vs_ci=0.0;
qj3_Vbi_ci=((cr*(-vdj2_Vbi_ci*ezr*d10_exp0))/ezr);
qj3_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj2_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj3_Vb_ci=((cr*(-vdj2_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj3=((cr*(1.0-d00_exp0))/ezr);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=(((qj1_Vbi_ci+qj2_Vbi_ci)-qj3_Vbi_ci)*vdcx_t+(cmax*vj4_Vbi_ci));
qjcx_Vtnode_GND=(((((qj1_Vtnode_GND+qj2_Vtnode_GND)-qj3_Vtnode_GND)*vdcx_t)+(((qj1+qj2)-qj3)*vdcx_t_Vtnode_GND))+((cmax_Vtnode_GND*vj4)+(cmax*vj4_Vtnode_GND)));
qjcx_Vb_ci=(((qj1_Vb_ci+qj2_Vb_ci)-qj3_Vb_ci)*vdcx_t+(cmax*vj4_Vb_ci));
#endif
qjcx=((((qj1+qj2)-qj3)*vdcx_t)+(cmax*vj4));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=0.0;
qjcx_Vb_ci=0.0;
#endif
qjcx=0.0;
#endif
}
}
else
{
if
((cjcx0_t_i>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zcx))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdcx_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdcx_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=0.0;
  xvf_Vbi_ci_Vbi_ei=0.0;
  xvf_Vbi_ci_Vbi_ci=0.0;
  xvf_Vbi_ci_Vtnode_GND=0.0;
  xvf_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbci)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=((-Vbci_Vb_ci)/VT);
#endif
#endif
xvf=((vf-Vbci)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=0.0;
  xvf2_Vbi_ei_Vbi_ei=0.0;
  xvf2_Vbi_ei_Vbi_ci=0.0;
  xvf2_Vbi_ei_Vtnode_GND=0.0;
  xvf2_Vbi_ei_Vb_ci=0.0;
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=0.0;
  v_j_Vbi_ei_Vbi_ei=0.0;
  v_j_Vbi_ei_Vbi_ci=0.0;
  v_j_Vbi_ei_Vtnode_GND=0.0;
  v_j_Vbi_ei_Vb_ci=0.0;
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-zcx);
{
double m00_logE(d00_logE0,(1.0-(v_j/vdcx_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vdcx_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=0.0;
#endif
#if defined(_DERIVATE)
y_Vs_ci=0.0;
#endif
y_Vbi_ei=0.0;
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=0.0;
  y_Vbi_ei_Vs_ci=0.0;
  y_Vbi_ei_Vbi_ei=0.0;
  y_Vbi_ei_Vbi_ci=0.0;
  y_Vbi_ei_Vtnode_GND=0.0;
  y_Vbi_ei_Vb_ci=0.0;
#endif
y_Vbi_ci=(-(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*0.0)));
  y_Vbi_ci_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vs_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vs_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vbi_ei/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ei/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vbi_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0)))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vtnode_GND/vdcx_t-(v_j_Vbi_ci*vdcx_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*vdcx_t_Vtnode_GND/vdcx_t/vdcx_t/vdcx_t))))));
  y_Vbi_ci_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vdcx_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vdcx_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdcx_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdcx_t)))*(-(v_j_Vb_ci/vdcx_t))*(-(v_j_Vbi_ci/vdcx_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vb_ci/vdcx_t-(v_j_Vbi_ci*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vdcx_t/vdcx_t+2*v_j*0.0*0.0/vdcx_t/vdcx_t/vdcx_t))))));
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdcx_t-v_j*vdcx_t_Vtnode_GND)/vdcx_t/vdcx_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vdcx_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=((((cjcx0_t_i*vdcx_t)*y_Vbi_ci)/x)+((2.4*cjcx0_t_i)*(-v_j_Vbi_ci)));
qjcx_Vtnode_GND=(((((((cjcx0_t_i_Vtnode_GND*vdcx_t)+(cjcx0_t_i*vdcx_t_Vtnode_GND))*y)+((cjcx0_t_i*vdcx_t)*y_Vtnode_GND))*x-((cjcx0_t_i*vdcx_t)*y)*x_Vtnode_GND)/x/x)+(((2.4*cjcx0_t_i_Vtnode_GND)*(Vbci-v_j))+((2.4*cjcx0_t_i)*(-v_j_Vtnode_GND))));
qjcx_Vb_ci=((((cjcx0_t_i*vdcx_t)*y_Vb_ci)/x)+((2.4*cjcx0_t_i)*(Vbci_Vb_ci-v_j_Vb_ci)));
#endif
qjcx=((((cjcx0_t_i*vdcx_t)*y)/x)+((2.4*cjcx0_t_i)*(Vbci-v_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=0.0;
qjcx_Vtnode_GND=0.0;
qjcx_Vb_ci=0.0;
#endif
qjcx=0.0;
#endif
}
}
}
if
((vptci<1.0e2))
{
if
((cjci0_t_ii>0.0))
{
zr=(zci/4.0);
#if defined(_DERIVATE)
vp_Vtnode_GND=(-vdci_t_Vtnode_GND);
#endif
vp=(vptci-vdci_t);
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATE)
cmax_Vtnode_GND=(2.4*cjci0_t_ii_Vtnode_GND);
#endif
cmax=(2.4*cjci0_t_ii);
{
double m00_logE(d00_logE0,(vdci_t/vptci))
double m00_exp(d00_exp1,((zci-zr)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vdci_t/vptci))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cr_Vtnode_GND=((cjci0_t_ii_Vtnode_GND*d00_exp1)+(cjci0_t_ii*((zci-zr)*(vdci_t_Vtnode_GND/vptci)*d10_logE0)*d10_exp1));
#endif
cr=(cjci0_t_ii*d00_exp1);
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=VT_Vtnode_GND;
#endif
a=VT;
#if defined(_DERIVATE)
ve_Vs_ci=0.0;
ve_Vci_ei=0.0;
ve_Vbi_ei=0.0;
ve_Vbi_ci=(((-Vbici_Vbi_ci)*a-(vf-Vbici)*a_Vbi_ci)/a/a);
ve_Vtnode_GND=((vf_Vtnode_GND*a-(vf-Vbici)*a_Vtnode_GND)/a/a);
ve_Vb_ci=0.0;
#endif
ve=((vf-Vbici)/a);
if
((ve<=80.0))
{
{
double m00_exp(d00_exp0,ve)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=ve_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=ve_Vtnode_GND*d10_exp0;
ex1_Vb_ci=ve_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=(-((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0)));
vj1_Vtnode_GND=(vf_Vtnode_GND-((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj1_Vb_ci=(-((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0)));
#endif
vj1=(vf-(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=0.0;
vj1_Vbi_ci=Vbici_Vbi_ci;
vj1_Vtnode_GND=0.0;
vj1_Vb_ci=0.0;
#endif
vj1=Vbici;
}
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=((0.1*vp_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
a=((0.1*vp)+(4.0*VT));
#if defined(_DERIVATE)
vr_Vci_ei=0.0;
vr_Vbi_ei=0.0;
vr_Vs_ci=0.0;
vr_Vbi_ci=((vj1_Vbi_ci*a-(vp+vj1)*a_Vbi_ci)/a/a);
vr_Vtnode_GND=(((vp_Vtnode_GND+vj1_Vtnode_GND)*a-(vp+vj1)*a_Vtnode_GND)/a/a);
vr_Vb_ci=((vj1_Vb_ci*a-(vp+vj1)*a_Vb_ci)/a/a);
#endif
vr=((vp+vj1)/a);
if
((vr<=80.0))
{
{
double m00_exp(d00_exp0,vr)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=0.0;
ex1_Vci_ei=0.0;
ex1_Vbi_ei=0.0;
ex1_Vbi_ci=vr_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=vr_Vtnode_GND*d10_exp0;
ex1_Vb_ci=vr_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#if defined(_DERIVATE)
ee1_Vs_ci=0.0;
ee1_Vci_ei=0.0;
ee1_Vbi_ei=0.0;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0));
vj2_Vtnode_GND=((-vp_Vtnode_GND)+((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj2_Vb_ci=((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0));
#endif
vj2=((-vp)+(a*d00_logE0));
}
}
else
{
#if defined(_DERIVATE)
vj2_Vci_ei=0.0;
vj2_Vbi_ei=0.0;
vj2_Vs_ci=0.0;
vj2_Vbi_ci=vj1_Vbi_ci;
vj2_Vtnode_GND=vj1_Vtnode_GND;
vj2_Vb_ci=vj1_Vb_ci;
#endif
vj2=vj1;
}
#if defined(_DERIVATE)
vj4_Vs_ci=0.0;
vj4_Vci_ei=0.0;
vj4_Vbi_ei=0.0;
vj4_Vbi_ci=(Vbici_Vbi_ci-vj1_Vbi_ci);
vj4_Vb_ci=(-vj1_Vb_ci);
vj4_Vtnode_GND=(-vj1_Vtnode_GND);
#endif
vj4=(Vbici-vj1);
ez=(1.0-zci);
ezr=(1.0-zr);
{
double m00_logE(d00_logE0,(1.0-(vj1/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj1/vdci_t)))
#endif
#if defined(_DERIVATE)
vdj1_Vci_ei=0.0;
vdj1_Vbi_ei=0.0;
vdj1_Vs_ci=0.0;
vdj1_Vbi_ci=(-(vj1_Vbi_ci/vdci_t))*d10_logE0;
vdj1_Vtnode_GND=(-((vj1_Vtnode_GND*vdci_t-vj1*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
vdj1_Vb_ci=(-(vj1_Vb_ci/vdci_t))*d10_logE0;
#endif
vdj1=d00_logE0;
}
{
double m00_logE(d00_logE0,(1.0-(vj2/vdci_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj2/vdci_t)))
#endif
#if defined(_DERIVATE)
vdj2_Vci_ei=0.0;
vdj2_Vbi_ei=0.0;
vdj2_Vs_ci=0.0;
vdj2_Vbi_ci=(-(vj2_Vbi_ci/vdci_t))*d10_logE0;
vdj2_Vtnode_GND=(-((vj2_Vtnode_GND*vdci_t-vj2*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0;
vdj2_Vb_ci=(-(vj2_Vb_ci/vdci_t))*d10_logE0;
#endif
vdj2=d00_logE0;
}
{
double m00_exp(d00_exp0,(vdj2*ez))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj1_Vci_ei=0.0;
qj1_Vbi_ei=0.0;
qj1_Vs_ci=0.0;
qj1_Vbi_ci=((cjci0_t_ii*(-vdj2_Vbi_ci*ez*d10_exp0))/ez);
qj1_Vtnode_GND=(((cjci0_t_ii_Vtnode_GND*(1.0-d00_exp0))+(cjci0_t_ii*(-vdj2_Vtnode_GND*ez*d10_exp0)))/ez);
qj1_Vb_ci=((cjci0_t_ii*(-vdj2_Vb_ci*ez*d10_exp0))/ez);
#endif
qj1=((cjci0_t_ii*(1.0-d00_exp0))/ez);
}
{
double m00_exp(d00_exp0,(vdj1*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj2_Vci_ei=0.0;
qj2_Vbi_ei=0.0;
qj2_Vs_ci=0.0;
qj2_Vbi_ci=((cr*(-vdj1_Vbi_ci*ezr*d10_exp0))/ezr);
qj2_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj1_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj2_Vb_ci=((cr*(-vdj1_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj2=((cr*(1.0-d00_exp0))/ezr);
}
{
double m00_exp(d00_exp0,(vdj2*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj3_Vci_ei=0.0;
qj3_Vbi_ei=0.0;
qj3_Vs_ci=0.0;
qj3_Vbi_ci=((cr*(-vdj2_Vbi_ci*ezr*d10_exp0))/ezr);
qj3_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj2_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj3_Vb_ci=((cr*(-vdj2_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj3=((cr*(1.0-d00_exp0))/ezr);
}
#if defined(_DERIVATE)
qjci_Vbi_ci=(((qj1_Vbi_ci+qj2_Vbi_ci)-qj3_Vbi_ci)*vdci_t+(cmax*vj4_Vbi_ci));
qjci_Vtnode_GND=(((((qj1_Vtnode_GND+qj2_Vtnode_GND)-qj3_Vtnode_GND)*vdci_t)+(((qj1+qj2)-qj3)*vdci_t_Vtnode_GND))+((cmax_Vtnode_GND*vj4)+(cmax*vj4_Vtnode_GND)));
qjci_Vb_ci=(((qj1_Vb_ci+qj2_Vb_ci)-qj3_Vb_ci)*vdci_t+(cmax*vj4_Vb_ci));
#endif
qjci=((((qj1+qj2)-qj3)*vdci_t)+(cmax*vj4));
}
else
{
#if defined(_DERIVATE)
qjci_Vbi_ci=0.0;
qjci_Vtnode_GND=0.0;
qjci_Vb_ci=0.0;
#endif
qjci=0.0;
}
}
else
{
if
((cjci0_t_ii>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=((-Vbici_Vbi_ci)/VT);
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ei=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ci=((+0.0-Vbici_Vbi_ci_Vbi_ci)/VT-((-Vbici_Vbi_ci)*0.0+(-Vbici_Vbi_ci)*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vtnode_GND=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*VT_Vtnode_GND+vf_Vtnode_GND*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*VT_Vtnode_GND/VT/VT/VT);
  xvf_Vbi_ci_Vb_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbici)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=0.0;
#endif
#endif
xvf=((vf-Vbici)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=0.0;
  xvf2_Vbi_ei_Vbi_ei=0.0;
  xvf2_Vbi_ei_Vbi_ci=0.0;
  xvf2_Vbi_ei_Vtnode_GND=0.0;
  xvf2_Vbi_ei_Vb_ci=0.0;
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=0.0;
  v_j_Vbi_ei_Vbi_ei=0.0;
  v_j_Vbi_ei_Vbi_ci=0.0;
  v_j_Vbi_ei_Vtnode_GND=0.0;
  v_j_Vbi_ei_Vb_ci=0.0;
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-zci);
{
double m00_logE(d00_logE0,(1.0-(v_j/vdci_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vdci_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=0.0;
#endif
#if defined(_DERIVATE)
y_Vs_ci=0.0;
#endif
y_Vbi_ei=0.0;
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=0.0;
  y_Vbi_ei_Vs_ci=0.0;
  y_Vbi_ei_Vbi_ei=0.0;
  y_Vbi_ei_Vbi_ci=0.0;
  y_Vbi_ei_Vtnode_GND=0.0;
  y_Vbi_ei_Vb_ci=0.0;
#endif
y_Vbi_ci=(-(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*0.0)));
  y_Vbi_ci_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vdci_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vdci_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdci_t)))*(-(v_j_Vs_ci/vdci_t))*(-(v_j_Vbi_ci/vdci_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vs_ci/vdci_t-(v_j_Vbi_ci*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vdci_t/vdci_t+2*v_j*0.0*0.0/vdci_t/vdci_t/vdci_t))))));
  y_Vbi_ci_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vdci_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vdci_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdci_t)))*(-(v_j_Vbi_ei/vdci_t))*(-(v_j_Vbi_ci/vdci_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ei/vdci_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vdci_t/vdci_t+2*v_j*0.0*0.0/vdci_t/vdci_t/vdci_t))))));
  y_Vbi_ci_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdci_t)))*(-(v_j_Vbi_ci/vdci_t))*(-(v_j_Vbi_ci/vdci_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ci/vdci_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vdci_t/vdci_t+2*v_j*0.0*0.0/vdci_t/vdci_t/vdci_t))))));
  y_Vbi_ci_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0)))*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdci_t)))*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*(-(v_j_Vbi_ci/vdci_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vtnode_GND/vdci_t-(v_j_Vbi_ci*vdci_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vdci_t/vdci_t+2*v_j*0.0*vdci_t_Vtnode_GND/vdci_t/vdci_t/vdci_t))))));
  y_Vbi_ci_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vdci_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vdci_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vdci_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vdci_t)))*(-(v_j_Vb_ci/vdci_t))*(-(v_j_Vbi_ci/vdci_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vb_ci/vdci_t-(v_j_Vbi_ci*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vdci_t/vdci_t+2*v_j*0.0*0.0/vdci_t/vdci_t/vdci_t))))));
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vdci_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#if defined(_DERIVATE)
qjci_Vbi_ci=((((cjci0_t_ii*vdci_t)*y_Vbi_ci)/x)+((2.4*cjci0_t_ii)*(Vbici_Vbi_ci-v_j_Vbi_ci)));
qjci_Vtnode_GND=(((((((cjci0_t_ii_Vtnode_GND*vdci_t)+(cjci0_t_ii*vdci_t_Vtnode_GND))*y)+((cjci0_t_ii*vdci_t)*y_Vtnode_GND))*x-((cjci0_t_ii*vdci_t)*y)*x_Vtnode_GND)/x/x)+(((2.4*cjci0_t_ii_Vtnode_GND)*(Vbici-v_j))+((2.4*cjci0_t_ii)*(-v_j_Vtnode_GND))));
qjci_Vb_ci=((((cjci0_t_ii*vdci_t)*y_Vb_ci)/x)+((2.4*cjci0_t_ii)*(-v_j_Vb_ci)));
#endif
qjci=((((cjci0_t_ii*vdci_t)*y)/x)+((2.4*cjci0_t_ii)*(Vbici-v_j)));
}
else
{
#if defined(_DERIVATE)
qjci_Vbi_ci=0.0;
qjci_Vtnode_GND=0.0;
qjci_Vb_ci=0.0;
#endif
qjci=0.0;
}
}
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcii_Vbi_ci=(qjci_Vbi_ci+qjcxi_Vbi_ci);
qjcii_Vtnode_GND=(qjci_Vtnode_GND+qjcxi_Vtnode_GND);
qjcii_Vb_ci=(qjci_Vb_ci+qjcxi_Vb_ci);
#endif
qjcii=(qjci+qjcxi);
#endif
if
((cjci0_t_ii>0.0))
{
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zci))
#if defined(_DERIVATE)
vf_Vtnode_GND=vdci_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vdci_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=((-Vbici_Vbi_ci)/VT);
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ei=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vbi_ci=((+0.0-Vbici_Vbi_ci_Vbi_ci)/VT-((-Vbici_Vbi_ci)*0.0+(-Vbici_Vbi_ci)*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ci_Vtnode_GND=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*VT_Vtnode_GND+vf_Vtnode_GND*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*VT_Vtnode_GND/VT/VT/VT);
  xvf_Vbi_ci_Vb_ci=((+0.0-0.0)/VT-((-Vbici_Vbi_ci)*0.0+0.0*0.0+(vf-Vbici)*0.0)/VT/VT+2*(vf-Vbici)*0.0*0.0/VT/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbici)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=0.0;
#endif
#endif
xvf=((vf-Vbici)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=0.0;
  xvf2_Vbi_ei_Vbi_ei=0.0;
  xvf2_Vbi_ei_Vbi_ci=0.0;
  xvf2_Vbi_ei_Vtnode_GND=0.0;
  xvf2_Vbi_ei_Vb_ci=0.0;
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=0.0;
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=0.0;
  v_j_Vbi_ei_Vbi_ei=0.0;
  v_j_Vbi_ei_Vbi_ci=0.0;
  v_j_Vbi_ei_Vtnode_GND=0.0;
  v_j_Vbi_ei_Vb_ci=0.0;
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
dvj_Vbi_ci=(((0.5*(xvf_Vbi_ci+xvf2_Vbi_ci))*xvf2-(0.5*(xvf+xvf2))*xvf2_Vbi_ci)/xvf2/xvf2);
dvj_Vtnode_GND=(((0.5*(xvf_Vtnode_GND+xvf2_Vtnode_GND))*xvf2-(0.5*(xvf+xvf2))*xvf2_Vtnode_GND)/xvf2/xvf2);
dvj_Vb_ci=(((0.5*(xvf_Vb_ci+xvf2_Vb_ci))*xvf2-(0.5*(xvf+xvf2))*xvf2_Vb_ci)/xvf2/xvf2);
#endif
dvj=((0.5*(xvf+xvf2))/xvf2);
{
double m00_logE(d00_logE0,(1-(v_j/vdci_t)))
double m00_exp(d00_exp1,((-zci)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1-(v_j/vdci_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cjcii_Vtnode_GND=(((((cjci0_t_ii_Vtnode_GND*d00_exp1)+(cjci0_t_ii*((-zci)*(-((v_j_Vtnode_GND*vdci_t-v_j*vdci_t_Vtnode_GND)/vdci_t/vdci_t))*d10_logE0)*d10_exp1))*dvj)+((cjci0_t_ii*d00_exp1)*dvj_Vtnode_GND))+(((2.4*cjci0_t_ii_Vtnode_GND)*(1-dvj))+((2.4*cjci0_t_ii)*(-dvj_Vtnode_GND))));
cjcii_Vbi_ci=((((cjci0_t_ii*((-zci)*(-(v_j_Vbi_ci/vdci_t))*d10_logE0)*d10_exp1)*dvj)+((cjci0_t_ii*d00_exp1)*dvj_Vbi_ci))+((2.4*cjci0_t_ii)*(-dvj_Vbi_ci)));
cjcii_Vb_ci=((((cjci0_t_ii*((-zci)*(-(v_j_Vb_ci/vdci_t))*d10_logE0)*d10_exp1)*dvj)+((cjci0_t_ii*d00_exp1)*dvj_Vb_ci))+((2.4*cjci0_t_ii)*(-dvj_Vb_ci)));
#endif
cjcii=(((cjci0_t_ii*d00_exp1)*dvj)+((2.4*cjci0_t_ii)*(1-dvj)));
}
}
else
{
#if defined(_DERIVATE)
cjcii_Vtnode_GND=0.0;
cjcii_Vbi_ci=0.0;
cjcii_Vb_ci=0.0;
#endif
cjcii=0.0;
}
if
((cje0_t>0.0))
{
{
double m00_logE(d00_logE0,aje_t)
double m00_exp(d00_exp1,((-d00_logE0)/ze))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,aje_t)
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
vf_Vtnode_GND=((vde_t_Vtnode_GND*(1.0-d00_exp1))+(vde_t*(-((-aje_t_Vtnode_GND*d10_logE0)/ze)*d10_exp1)));
#endif
vf=(vde_t*(1.0-d00_exp1));
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=0.0;
#endif
xvf_Vbi_ei=((-Vbiei_Vbi_ei)/VT);
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=((+0.0-0.0)/VT-((-Vbiei_Vbi_ei)*0.0+0.0*0.0+(vf-Vbiei)*0.0)/VT/VT+2*(vf-Vbiei)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ei_Vbi_ei=((+0.0-Vbiei_Vbi_ei_Vbi_ei)/VT-((-Vbiei_Vbi_ei)*0.0+(-Vbiei_Vbi_ei)*0.0+(vf-Vbiei)*0.0)/VT/VT+2*(vf-Vbiei)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ei_Vbi_ci=((+0.0-0.0)/VT-((-Vbiei_Vbi_ei)*0.0+0.0*0.0+(vf-Vbiei)*0.0)/VT/VT+2*(vf-Vbiei)*0.0*0.0/VT/VT/VT);
  xvf_Vbi_ei_Vtnode_GND=((+0.0-0.0)/VT-((-Vbiei_Vbi_ei)*VT_Vtnode_GND+vf_Vtnode_GND*0.0+(vf-Vbiei)*0.0)/VT/VT+2*(vf-Vbiei)*0.0*VT_Vtnode_GND/VT/VT/VT);
  xvf_Vbi_ei_Vb_ci=((+0.0-0.0)/VT-((-Vbiei_Vbi_ei)*0.0+0.0*0.0+(vf-Vbiei)*0.0)/VT/VT+2*(vf-Vbiei)*0.0*0.0/VT/VT/VT);
#endif
xvf_Vbi_ci=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=0.0;
  xvf_Vbi_ci_Vbi_ei=0.0;
  xvf_Vbi_ci_Vbi_ci=0.0;
  xvf_Vbi_ci_Vtnode_GND=0.0;
  xvf_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vbiei)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=0.0;
#endif
#endif
xvf=((vf-Vbiei)/VT);
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=0.0;
#endif
xvf2_Vbi_ei=((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vs_ci*xvf+xvf_Vbi_ei*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vs_ci)+0.0));
  xvf2_Vbi_ei_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vbi_ei*xvf+xvf_Vbi_ei*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vbi_ei)+0.0));
  xvf2_Vbi_ei_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vbi_ci*xvf+xvf_Vbi_ei*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vbi_ci)+0.0));
  xvf2_Vbi_ei_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vtnode_GND*xvf+xvf_Vbi_ei*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vtnode_GND)+0.0));
  xvf2_Vbi_ei_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vb_ci*xvf+xvf_Vbi_ei*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vb_ci)+0.0));
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=0.0;
#endif
v_j_Vbi_ei=(-(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vs_ci+xvf2_Vbi_ei_Vs_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vbi_ei+xvf2_Vbi_ei_Vbi_ei))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vbi_ci+xvf2_Vbi_ei_Vbi_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vtnode_GND+xvf2_Vbi_ei_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vb_ci+xvf2_Vbi_ei_Vb_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-ze);
{
double m00_logE(d00_logE0,(1.0-(v_j/vde_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vde_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=0.0;
#endif
#if defined(_DERIVATE)
y_Vs_ci=0.0;
#endif
y_Vbi_ei=(-(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*0.0)));
  y_Vbi_ei_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vs_ci/vde_t))*(-(v_j_Vbi_ei/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vs_ci/vde_t-(v_j_Vbi_ei*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ei_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vbi_ei/vde_t))*(-(v_j_Vbi_ei/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vbi_ei/vde_t-(v_j_Vbi_ei*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ei_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vbi_ci/vde_t))*(-(v_j_Vbi_ei/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vbi_ci/vde_t-(v_j_Vbi_ei*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ei_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*d10_logE0)))*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*(-(v_j_Vbi_ei/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vtnode_GND/vde_t-(v_j_Vbi_ei*vde_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*vde_t_Vtnode_GND/vde_t/vde_t/vde_t))))));
  y_Vbi_ei_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vb_ci/vde_t))*(-(v_j_Vbi_ei/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vb_ci/vde_t-(v_j_Vbi_ei*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
#endif
y_Vbi_ci=(-(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*0.0)));
  y_Vbi_ci_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vs_ci/vde_t))*(-(v_j_Vbi_ci/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vs_ci/vde_t-(v_j_Vbi_ci*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ci_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vbi_ei/vde_t))*(-(v_j_Vbi_ci/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ei/vde_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ci_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vbi_ci/vde_t))*(-(v_j_Vbi_ci/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ci/vde_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
  y_Vbi_ci_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*d10_logE0)))*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*(-(v_j_Vbi_ci/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vtnode_GND/vde_t-(v_j_Vbi_ci*vde_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*vde_t_Vtnode_GND/vde_t/vde_t/vde_t))))));
  y_Vbi_ci_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vde_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vde_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vde_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vde_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vde_t)))*(-(v_j_Vb_ci/vde_t))*(-(v_j_Vbi_ci/vde_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vb_ci/vde_t-(v_j_Vbi_ci*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vde_t/vde_t+2*v_j*0.0*0.0/vde_t/vde_t/vde_t))))));
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vde_t-v_j*vde_t_Vtnode_GND)/vde_t/vde_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vde_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
qjei_Vtnode_GND=(((((((cje0_t_Vtnode_GND*vde_t)+(cje0_t*vde_t_Vtnode_GND))*y)+((cje0_t*vde_t)*y_Vtnode_GND))*x-((cje0_t*vde_t)*y)*x_Vtnode_GND)/x/x)+((((aje_t_Vtnode_GND*cje0_t)+(aje_t*cje0_t_Vtnode_GND))*(Vbiei-v_j))+((aje_t*cje0_t)*(-v_j_Vtnode_GND))));
#endif
qjei_Vbi_ei=((((cje0_t*vde_t)*y_Vbi_ei)/x)+((aje_t*cje0_t)*(Vbiei_Vbi_ei-v_j_Vbi_ei)));
#if defined(_DERIVATE)
  qjei_Vbi_ei_Vtnode_GND=(+((+(+0.0*vde_t+0.0*vde_t_Vtnode_GND+cje0_t_Vtnode_GND*0.0+cje0_t*0.0)*y+0.0*y_Vtnode_GND+(((cje0_t_Vtnode_GND*vde_t)+(cje0_t*vde_t_Vtnode_GND)))*y_Vbi_ei+(cje0_t*vde_t)*y_Vbi_ei_Vtnode_GND)/x-(((cje0_t*vde_t)*y_Vbi_ei)*x_Vtnode_GND+((((((cje0_t_Vtnode_GND*vde_t)+(cje0_t*vde_t_Vtnode_GND)))*y)+((cje0_t*vde_t)*y_Vtnode_GND)))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*x_Vtnode_GND/x/x/x)+(+(+0.0*cje0_t+0.0*cje0_t_Vtnode_GND+aje_t_Vtnode_GND*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vtnode_GND)+(((aje_t_Vtnode_GND*cje0_t)+(aje_t*cje0_t_Vtnode_GND)))*(Vbiei_Vbi_ei-v_j_Vbi_ei)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ei_Vtnode_GND)));
  qjei_Vbi_ei_Vbi_ei=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vbi_ei+0.0*y_Vbi_ei+(cje0_t*vde_t)*y_Vbi_ei_Vbi_ei)/x-(((cje0_t*vde_t)*y_Vbi_ei)*0.0+(((cje0_t*vde_t)*y_Vbi_ei))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(Vbiei_Vbi_ei-v_j_Vbi_ei)+0.0*(Vbiei_Vbi_ei-v_j_Vbi_ei)+(aje_t*cje0_t)*(+Vbiei_Vbi_ei_Vbi_ei-v_j_Vbi_ei_Vbi_ei)));
  qjei_Vbi_ei_Vbi_ci=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vbi_ci+0.0*y_Vbi_ei+(cje0_t*vde_t)*y_Vbi_ei_Vbi_ci)/x-(((cje0_t*vde_t)*y_Vbi_ei)*0.0+(((cje0_t*vde_t)*y_Vbi_ci))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vbi_ci)+0.0*(Vbiei_Vbi_ei-v_j_Vbi_ei)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ei_Vbi_ci)));
  qjei_Vbi_ei_Vb_ci=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vb_ci+0.0*y_Vbi_ei+(cje0_t*vde_t)*y_Vbi_ei_Vb_ci)/x-(((cje0_t*vde_t)*y_Vbi_ei)*0.0+(((cje0_t*vde_t)*y_Vb_ci))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vb_ci)+0.0*(Vbiei_Vbi_ei-v_j_Vbi_ei)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ei_Vb_ci)));
#endif
qjei_Vbi_ci=((((cje0_t*vde_t)*y_Vbi_ci)/x)+((aje_t*cje0_t)*(-v_j_Vbi_ci)));
#if defined(_DERIVATE)
  qjei_Vbi_ci_Vtnode_GND=(+((+(+0.0*vde_t+0.0*vde_t_Vtnode_GND+cje0_t_Vtnode_GND*0.0+cje0_t*0.0)*y+0.0*y_Vtnode_GND+(((cje0_t_Vtnode_GND*vde_t)+(cje0_t*vde_t_Vtnode_GND)))*y_Vbi_ci+(cje0_t*vde_t)*y_Vbi_ci_Vtnode_GND)/x-(((cje0_t*vde_t)*y_Vbi_ci)*x_Vtnode_GND+((((((cje0_t_Vtnode_GND*vde_t)+(cje0_t*vde_t_Vtnode_GND)))*y)+((cje0_t*vde_t)*y_Vtnode_GND)))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*x_Vtnode_GND/x/x/x)+(+(+0.0*cje0_t+0.0*cje0_t_Vtnode_GND+aje_t_Vtnode_GND*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vtnode_GND)+(((aje_t_Vtnode_GND*cje0_t)+(aje_t*cje0_t_Vtnode_GND)))*(-v_j_Vbi_ci)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ci_Vtnode_GND)));
  qjei_Vbi_ci_Vbi_ei=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vbi_ei+0.0*y_Vbi_ci+(cje0_t*vde_t)*y_Vbi_ci_Vbi_ei)/x-(((cje0_t*vde_t)*y_Vbi_ci)*0.0+(((cje0_t*vde_t)*y_Vbi_ei))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(Vbiei_Vbi_ei-v_j_Vbi_ei)+0.0*(-v_j_Vbi_ci)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ci_Vbi_ei)));
  qjei_Vbi_ci_Vbi_ci=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vbi_ci+0.0*y_Vbi_ci+(cje0_t*vde_t)*y_Vbi_ci_Vbi_ci)/x-(((cje0_t*vde_t)*y_Vbi_ci)*0.0+(((cje0_t*vde_t)*y_Vbi_ci))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vbi_ci)+0.0*(-v_j_Vbi_ci)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ci_Vbi_ci)));
  qjei_Vbi_ci_Vb_ci=(+((+(+0.0*vde_t+0.0*0.0+0.0*0.0+cje0_t*0.0)*y+0.0*y_Vb_ci+0.0*y_Vbi_ci+(cje0_t*vde_t)*y_Vbi_ci_Vb_ci)/x-(((cje0_t*vde_t)*y_Vbi_ci)*0.0+(((cje0_t*vde_t)*y_Vb_ci))*0.0+((cje0_t*vde_t)*y)*0.0)/x/x+2*((cje0_t*vde_t)*y)*0.0*0.0/x/x/x)+(+(+0.0*cje0_t+0.0*0.0+0.0*0.0+aje_t*0.0)*(Vbiei-v_j)+0.0*(-v_j_Vb_ci)+0.0*(-v_j_Vbi_ci)+(aje_t*cje0_t)*(+0.0-v_j_Vbi_ci_Vb_ci)));
#endif
#if defined(_DERIVATE)
qjei_Vb_ci=((((cje0_t*vde_t)*y_Vb_ci)/x)+((aje_t*cje0_t)*(-v_j_Vb_ci)));
#endif
#endif
qjei=((((cje0_t*vde_t)*y)/x)+((aje_t*cje0_t)*(Vbiei-v_j)));
}
else
{
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
qjei_Vtnode_GND=0.0;
#endif
qjei_Vbi_ei=0.0;
#if defined(_DERIVATE)
  qjei_Vbi_ei_Vtnode_GND=0.0;
  qjei_Vbi_ei_Vbi_ei=0.0;
  qjei_Vbi_ei_Vbi_ci=0.0;
  qjei_Vbi_ei_Vb_ci=0.0;
#endif
qjei_Vbi_ci=0.0;
#if defined(_DERIVATE)
  qjei_Vbi_ci_Vtnode_GND=0.0;
  qjei_Vbi_ci_Vbi_ei=0.0;
  qjei_Vbi_ci_Vbi_ci=0.0;
  qjei_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
qjei_Vb_ci=0.0;
#endif
#endif
qjei=0.0;
}
#if defined(_DYNAMIC)
#if defined(_DDX)
cjei=+(qjei_Vbi_ei)+(qjei_Vbi_ci);
#endif
#endif
#if defined(_DERIVATE)
vc_Vci_ei=Vciei_Vci_ei;
vc_Vtnode_GND=(-vces_t_Vtnode_GND);
#endif
vc=(Vciei-vces_t);
#if defined(_DERIVATE)
uvc_Vci_ei=(vc_Vci_ei/VT);
uvc_Vtnode_GND=((vc_Vtnode_GND*VT-vc*VT_Vtnode_GND)/VT/VT);
#endif
uvc=((vc/VT)-1);
{
double m00_sqrt(d00_sqrt0,((uvc*uvc)+1.921812))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((uvc*uvc)+1.921812))
#endif
#if defined(_DERIVATE)
vceff_Vtnode_GND=((VT_Vtnode_GND*(1+(0.5*(uvc+d00_sqrt0))))+(VT*(0.5*(uvc_Vtnode_GND+((uvc_Vtnode_GND*uvc)+(uvc*uvc_Vtnode_GND))*d10_sqrt0))));
vceff_Vci_ei=(VT*(0.5*(uvc_Vci_ei+((uvc_Vci_ei*uvc)+(uvc*uvc_Vci_ei))*d10_sqrt0)));
#endif
vceff=(VT*(1+(0.5*(uvc+d00_sqrt0))));
}
#if defined(_DERIVATE)
x_Vtnode_GND=((vceff_Vtnode_GND-vlim_t_Vtnode_GND)/vpt);
x_Vci_ei=(vceff_Vci_ei/vpt);
#endif
x=((vceff-vlim_t)/vpt);
{
double m00_sqrt(d00_sqrt0,((x*x)+1e-3))
double m00_sqrt(d00_sqrt1,(1+(((vceff*vceff)/vlim_t)/vlim_t)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((x*x)+1e-3))
double m10_sqrt(d10_sqrt1,d00_sqrt1,(1+(((vceff*vceff)/vlim_t)/vlim_t)))
#endif
#if defined(_DERIVATE)
ick_Vtnode_GND=((((((vceff_Vtnode_GND*(1+(0.5*(x+d00_sqrt0))))+(vceff*(0.5*(x_Vtnode_GND+((x_Vtnode_GND*x)+(x*x_Vtnode_GND))*d10_sqrt0))))*rci0_t-(vceff*(1+(0.5*(x+d00_sqrt0))))*rci0_t_Vtnode_GND)/rci0_t/rci0_t)*d00_sqrt1-((vceff*(1+(0.5*(x+d00_sqrt0))))/rci0_t)*((((((vceff_Vtnode_GND*vceff)+(vceff*vceff_Vtnode_GND))*vlim_t-(vceff*vceff)*vlim_t_Vtnode_GND)/vlim_t/vlim_t)*vlim_t-((vceff*vceff)/vlim_t)*vlim_t_Vtnode_GND)/vlim_t/vlim_t)*d10_sqrt1)/d00_sqrt1/d00_sqrt1);
ick_Vci_ei=(((((vceff_Vci_ei*(1+(0.5*(x+d00_sqrt0))))+(vceff*(0.5*(x_Vci_ei+((x_Vci_ei*x)+(x*x_Vci_ei))*d10_sqrt0))))/rci0_t)*d00_sqrt1-((vceff*(1+(0.5*(x+d00_sqrt0))))/rci0_t)*((((vceff_Vci_ei*vceff)+(vceff*vceff_Vci_ei))/vlim_t)/vlim_t)*d10_sqrt1)/d00_sqrt1/d00_sqrt1);
#endif
ick=(((vceff*(1+(0.5*(x+d00_sqrt0))))/rci0_t)/d00_sqrt1);
}
#if defined(_DERIVATE)
cc_Vtnode_GND=((cjci0_t_ii_Vtnode_GND*cjcii-cjci0_t_ii*cjcii_Vtnode_GND)/cjcii/cjcii);
cc_Vbi_ci=(-cjci0_t_ii*cjcii_Vbi_ci/cjcii/cjcii);
cc_Vb_ci=(-cjci0_t_ii*cjcii_Vb_ci/cjcii/cjcii);
#endif
cc=(cjci0_t_ii/cjcii);
#if defined(_DERIVATE)
qjci_Vbi_ci=(qjci_Vbi_ci/cjci0_t_ii);
qjci_Vtnode_GND=((qjci_Vtnode_GND*cjci0_t_ii-qjci*cjci0_t_ii_Vtnode_GND)/cjci0_t_ii/cjci0_t_ii);
qjci_Vb_ci=(qjci_Vb_ci/cjci0_t_ii);
#endif
qjci=(qjci/cjci0_t_ii);
#if defined(_DERIVATE)
qj_2_Vbi_ci=((qjci_Vbi_ci/vef)/2);
qj_2_Vtnode_GND=((qjci_Vtnode_GND/vef)/2);
qj_2_Vb_ci=((qjci_Vb_ci/vef)/2);
#endif
qj_2=((1+(qjci/vef))/2);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
tf0_Vtnode_GND=((t0_t_Vtnode_GND+(dt0h*cc_Vtnode_GND))+(tbvl*(-1*cc_Vtnode_GND/cc/cc)));
tf0_Vbi_ci=((dt0h*cc_Vbi_ci)+(tbvl*(-1*cc_Vbi_ci/cc/cc)));
tf0_Vb_ci=((dt0h*cc_Vb_ci)+(tbvl*(-1*cc_Vb_ci/cc/cc)));
#endif
tf0=((t0_t+(dt0h*(cc-1)))+(tbvl*((1/cc)-1)));
#endif
ickf=iqf;
ickr=iqr;
#if defined(_DERIVATE)
arg1_Vbi_ei=(Vbiei_Vbi_ei/(mcf*VT));
arg1_Vtnode_GND=(-Vbiei*(mcf*VT_Vtnode_GND)/(mcf*VT)/(mcf*VT));
#endif
arg1=(Vbiei/(mcf*VT));
if
((arg1>80))
{
#if defined(_DERIVATE)
le1_Vbi_ei=arg1_Vbi_ei;
le1_Vtnode_GND=arg1_Vtnode_GND;
#endif
le1=(1+(arg1-80));
#if defined(_DERIVATE)
arg1_Vbi_ei=0.0;
arg1_Vtnode_GND=0.0;
#endif
arg1=80;
}
else
{
#if defined(_DERIVATE)
le1_Vbi_ei=0.0;
le1_Vtnode_GND=0.0;
#endif
le1=1;
}
{
double m00_limexp(d00_limexp0,arg1)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,arg1)
#endif
#if defined(_DERIVATE)
le1_Vbi_ei=((le1_Vbi_ei*d00_limexp0)+(le1*arg1_Vbi_ei*d10_limexp0));
le1_Vtnode_GND=((le1_Vtnode_GND*d00_limexp0)+(le1*arg1_Vtnode_GND*d10_limexp0));
#endif
le1=(le1*d00_limexp0);
}
#if defined(_DERIVATE)
itfi_Vtnode_GND=((is_t_Vtnode_GND*le1)+(is_t*le1_Vtnode_GND));
itfi_Vbi_ei=(is_t*le1_Vbi_ei);
#endif
itfi=(is_t*le1);
#if defined(_DERIVATE)
arg2_Vbi_ci=(Vbici_Vbi_ci/(mcr*VT));
arg2_Vtnode_GND=(-Vbici*(mcr*VT_Vtnode_GND)/(mcr*VT)/(mcr*VT));
#endif
arg2=(Vbici/(mcr*VT));
if
((arg2>80))
{
#if defined(_DERIVATE)
le2_Vbi_ci=arg2_Vbi_ci;
le2_Vtnode_GND=arg2_Vtnode_GND;
#endif
le2=(1+(arg2-80));
#if defined(_DERIVATE)
arg2_Vbi_ci=0.0;
arg2_Vtnode_GND=0.0;
#endif
arg2=80;
}
else
{
#if defined(_DERIVATE)
le2_Vbi_ci=0.0;
le2_Vtnode_GND=0.0;
#endif
le2=1;
}
{
double m00_limexp(d00_limexp0,arg2)
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,arg2)
#endif
#if defined(_DERIVATE)
le2_Vbi_ci=((le2_Vbi_ci*d00_limexp0)+(le2*arg2_Vbi_ci*d10_limexp0));
le2_Vtnode_GND=((le2_Vtnode_GND*d00_limexp0)+(le2*arg2_Vtnode_GND*d10_limexp0));
#endif
le2=(le2*d00_limexp0);
}
#if defined(_DERIVATE)
itri_Vtnode_GND=((is_t_Vtnode_GND*le2)+(is_t*le2_Vtnode_GND));
itri_Vbi_ci=(is_t*le2_Vbi_ci);
#endif
itri=(is_t*le2);
#if defined(_DERIVATE)
qm_Vtnode_GND=((itfi_Vtnode_GND/ickf)+(itri_Vtnode_GND/ickr));
qm_Vbi_ei=(itfi_Vbi_ei/ickf);
qm_Vbi_ci=(itri_Vbi_ci/ickr);
#endif
qm=((itfi/ickf)+(itri/ickr));
{
double m00_sqrt(d00_sqrt0,((qj_2*qj_2)+qm))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((qj_2*qj_2)+qm))
#endif
#if defined(_DERIVATE)
qpt_Vbi_ci=(qj_2_Vbi_ci+(((qj_2_Vbi_ci*qj_2)+(qj_2*qj_2_Vbi_ci))+qm_Vbi_ci)*d10_sqrt0);
qpt_Vtnode_GND=(qj_2_Vtnode_GND+(((qj_2_Vtnode_GND*qj_2)+(qj_2*qj_2_Vtnode_GND))+qm_Vtnode_GND)*d10_sqrt0);
qpt_Vb_ci=(qj_2_Vb_ci+((qj_2_Vb_ci*qj_2)+(qj_2*qj_2_Vb_ci))*d10_sqrt0);
qpt_Vbi_ei=qm_Vbi_ei*d10_sqrt0;
#endif
qpt=(qj_2+d00_sqrt0);
}
if
((qpt<=1e-20))
{
#if defined(_DERIVATE)
qpt_Vbi_ci=0.0;
qpt_Vtnode_GND=0.0;
qpt_Vb_ci=0.0;
qpt_Vbi_ei=0.0;
#endif
qpt=1e-20;
}
#if defined(_DERIVATE)
itfl_Vtnode_GND=((itfi_Vtnode_GND*qpt-itfi*qpt_Vtnode_GND)/qpt/qpt);
itfl_Vbi_ei=((itfi_Vbi_ei*qpt-itfi*qpt_Vbi_ei)/qpt/qpt);
itfl_Vbi_ci=(-itfi*qpt_Vbi_ci/qpt/qpt);
itfl_Vb_ci=(-itfi*qpt_Vb_ci/qpt/qpt);
#endif
itfl=(itfi/qpt);
#if defined(_DERIVATE)
itrl_Vtnode_GND=((itri_Vtnode_GND*qpt-itri*qpt_Vtnode_GND)/qpt/qpt);
itrl_Vbi_ci=((itri_Vbi_ci*qpt-itri*qpt_Vbi_ci)/qpt/qpt);
itrl_Vb_ci=(-itri*qpt_Vb_ci/qpt/qpt);
itrl_Vbi_ei=(-itri*qpt_Vbi_ei/qpt/qpt);
#endif
itrl=(itri/qpt);
if
((itfl<=1e-20))
{
#if defined(_DERIVATE)
itfl_Vtnode_GND=0.0;
itfl_Vbi_ei=0.0;
itfl_Vbi_ci=0.0;
itfl_Vb_ci=0.0;
#endif
itfl=1e-20;
}
#if defined(_DERIVATE)
al_Vtnode_GND=(-((ick_Vtnode_GND*itfl-ick*itfl_Vtnode_GND)/itfl/itfl));
al_Vci_ei=(-(ick_Vci_ei/itfl));
al_Vbi_ei=(-(-ick*itfl_Vbi_ei/itfl/itfl));
al_Vbi_ci=(-(-ick*itfl_Vbi_ci/itfl/itfl));
al_Vb_ci=(-(-ick*itfl_Vb_ci/itfl/itfl));
#endif
al=(1-(ick/itfl));
{
double m00_sqrt(d00_sqrt0,((al*al)+ahc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((al*al)+ahc))
#endif
#if defined(_DERIVATE)
s3l_Vtnode_GND=((al_Vtnode_GND*al)+(al*al_Vtnode_GND))*d10_sqrt0;
s3l_Vci_ei=((al_Vci_ei*al)+(al*al_Vci_ei))*d10_sqrt0;
s3l_Vbi_ei=((al_Vbi_ei*al)+(al*al_Vbi_ei))*d10_sqrt0;
s3l_Vbi_ci=((al_Vbi_ci*al)+(al*al_Vbi_ci))*d10_sqrt0;
s3l_Vb_ci=((al_Vb_ci*al)+(al*al_Vb_ci))*d10_sqrt0;
#endif
s3l=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(1+ahc))
#if defined(_DERIVATE)
wl_Vtnode_GND=((al_Vtnode_GND+s3l_Vtnode_GND)/(1+d00_sqrt0));
wl_Vci_ei=((al_Vci_ei+s3l_Vci_ei)/(1+d00_sqrt0));
wl_Vbi_ei=((al_Vbi_ei+s3l_Vbi_ei)/(1+d00_sqrt0));
wl_Vbi_ci=((al_Vbi_ci+s3l_Vbi_ci)/(1+d00_sqrt0));
wl_Vb_ci=((al_Vb_ci+s3l_Vb_ci)/(1+d00_sqrt0));
#endif
wl=((al+s3l)/(1+d00_sqrt0));
}
#if defined(_DERIVATE)
d_qfh_Vtnode_GND=((((((wl_Vtnode_GND*wl)+(wl*wl_Vtnode_GND))+(((tfh*itfl_Vtnode_GND)*ick-(tfh*itfl)*ick_Vtnode_GND)/ick/ick))*itfl)+(((wl*wl)+((tfh*itfl)/ick))*itfl_Vtnode_GND))/iqfh);
d_qfh_Vci_ei=((((wl_Vci_ei*wl)+(wl*wl_Vci_ei))+(-(tfh*itfl)*ick_Vci_ei/ick/ick))*itfl/iqfh);
d_qfh_Vbi_ei=((((((wl_Vbi_ei*wl)+(wl*wl_Vbi_ei))+((tfh*itfl_Vbi_ei)/ick))*itfl)+(((wl*wl)+((tfh*itfl)/ick))*itfl_Vbi_ei))/iqfh);
d_qfh_Vbi_ci=((((((wl_Vbi_ci*wl)+(wl*wl_Vbi_ci))+((tfh*itfl_Vbi_ci)/ick))*itfl)+(((wl*wl)+((tfh*itfl)/ick))*itfl_Vbi_ci))/iqfh);
d_qfh_Vb_ci=((((((wl_Vb_ci*wl)+(wl*wl_Vb_ci))+((tfh*itfl_Vb_ci)/ick))*itfl)+(((wl*wl)+((tfh*itfl)/ick))*itfl_Vb_ci))/iqfh);
#endif
d_qfh=((((wl*wl)+((tfh*itfl)/ick))*itfl)/iqfh);
#if defined(_DERIVATE)
facl_Vtnode_GND=(-1*((d_qfh_Vtnode_GND*qpt-d_qfh*qpt_Vtnode_GND)/qpt/qpt)/(1+(d_qfh/qpt))/(1+(d_qfh/qpt)));
facl_Vci_ei=(-1*(d_qfh_Vci_ei/qpt)/(1+(d_qfh/qpt))/(1+(d_qfh/qpt)));
facl_Vbi_ei=(-1*((d_qfh_Vbi_ei*qpt-d_qfh*qpt_Vbi_ei)/qpt/qpt)/(1+(d_qfh/qpt))/(1+(d_qfh/qpt)));
facl_Vbi_ci=(-1*((d_qfh_Vbi_ci*qpt-d_qfh*qpt_Vbi_ci)/qpt/qpt)/(1+(d_qfh/qpt))/(1+(d_qfh/qpt)));
facl_Vb_ci=(-1*((d_qfh_Vb_ci*qpt-d_qfh*qpt_Vb_ci)/qpt/qpt)/(1+(d_qfh/qpt))/(1+(d_qfh/qpt)));
#endif
facl=(1/(1+(d_qfh/qpt)));
#if defined(_DERIVATE)
itf_Vtnode_GND=((itfl_Vtnode_GND*facl)+(itfl*facl_Vtnode_GND));
itf_Vbi_ei=((itfl_Vbi_ei*facl)+(itfl*facl_Vbi_ei));
itf_Vbi_ci=((itfl_Vbi_ci*facl)+(itfl*facl_Vbi_ci));
itf_Vb_ci=((itfl_Vb_ci*facl)+(itfl*facl_Vb_ci));
itf_Vci_ei=(itfl*facl_Vci_ei);
#endif
itf=(itfl*facl);
#if defined(_DERIVATE)
itr_Vtnode_GND=((itrl_Vtnode_GND*facl)+(itrl*facl_Vtnode_GND));
itr_Vbi_ci=((itrl_Vbi_ci*facl)+(itrl*facl_Vbi_ci));
itr_Vb_ci=((itrl_Vb_ci*facl)+(itrl*facl_Vb_ci));
itr_Vbi_ei=((itrl_Vbi_ei*facl)+(itrl*facl_Vbi_ei));
itr_Vci_ei=(itrl*facl_Vci_ei);
#endif
itr=(itrl*facl);
if
((itf<=1e-20))
{
#if defined(_DERIVATE)
itf_Vtnode_GND=0.0;
itf_Vbi_ei=0.0;
itf_Vbi_ci=0.0;
itf_Vb_ci=0.0;
itf_Vci_ei=0.0;
#endif
itf=1e-20;
}
#if defined(_DERIVATE)
it_Vtnode_GND=(itf_Vtnode_GND-itr_Vtnode_GND);
it_Vbi_ei=(itf_Vbi_ei-itr_Vbi_ei);
it_Vbi_ci=(itf_Vbi_ci-itr_Vbi_ci);
it_Vb_ci=(itf_Vb_ci-itr_Vb_ci);
it_Vci_ei=(itf_Vci_ei-itr_Vci_ei);
#endif
it=(itf-itr);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qf0_Vtnode_GND=((tf0_Vtnode_GND*itf)+(tf0*itf_Vtnode_GND));
qf0_Vbi_ci=((tf0_Vbi_ci*itf)+(tf0*itf_Vbi_ci));
qf0_Vb_ci=((tf0_Vb_ci*itf)+(tf0*itf_Vb_ci));
qf0_Vbi_ei=(tf0*itf_Vbi_ei);
qf0_Vci_ei=(tf0*itf_Vci_ei);
#endif
qf0=(tf0*itf);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
a_Vci_ei=(-((ick_Vci_ei*itf-ick*itf_Vci_ei)/itf/itf));
a_Vbi_ei=(-(-ick*itf_Vbi_ei/itf/itf));
a_Vbi_ci=(-(-ick*itf_Vbi_ci/itf/itf));
a_Vb_ci=(-(-ick*itf_Vb_ci/itf/itf));
a_Vtnode_GND=(-((ick_Vtnode_GND*itf-ick*itf_Vtnode_GND)/itf/itf));
#endif
a=(1-(ick/itf));
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((a*a)+ahc))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((a*a)+ahc))
#endif
#if defined(_DERIVATE)
s3_Vci_ei=((a_Vci_ei*a)+(a*a_Vci_ei))*d10_sqrt0;
s3_Vbi_ei=((a_Vbi_ei*a)+(a*a_Vbi_ei))*d10_sqrt0;
s3_Vbi_ci=((a_Vbi_ci*a)+(a*a_Vbi_ci))*d10_sqrt0;
s3_Vb_ci=((a_Vb_ci*a)+(a*a_Vb_ci))*d10_sqrt0;
s3_Vtnode_GND=((a_Vtnode_GND*a)+(a*a_Vtnode_GND))*d10_sqrt0;
#endif
s3=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1+ahc))
#if defined(_DERIVATE)
w_Vci_ei=((a_Vci_ei+s3_Vci_ei)/(1+d00_sqrt0));
w_Vbi_ei=((a_Vbi_ei+s3_Vbi_ei)/(1+d00_sqrt0));
w_Vbi_ci=((a_Vbi_ci+s3_Vbi_ci)/(1+d00_sqrt0));
w_Vb_ci=((a_Vb_ci+s3_Vb_ci)/(1+d00_sqrt0));
w_Vtnode_GND=((a_Vtnode_GND+s3_Vtnode_GND)/(1+d00_sqrt0));
#endif
w=((a+s3)/(1+d00_sqrt0));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
tww_Vtnode_GND=((((thcs_t_Vtnode_GND*w)+(thcs_t*w_Vtnode_GND))*w)+((thcs_t*w)*w_Vtnode_GND));
tww_Vci_ei=(((thcs_t*w_Vci_ei)*w)+((thcs_t*w)*w_Vci_ei));
tww_Vbi_ei=(((thcs_t*w_Vbi_ei)*w)+((thcs_t*w)*w_Vbi_ei));
tww_Vbi_ci=(((thcs_t*w_Vbi_ci)*w)+((thcs_t*w)*w_Vbi_ci));
tww_Vb_ci=(((thcs_t*w_Vb_ci)*w)+((thcs_t*w)*w_Vb_ci));
#endif
tww=((thcs_t*w)*w);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
dqfh_Vtnode_GND=((tww_Vtnode_GND*itf)+(tww*itf_Vtnode_GND));
dqfh_Vci_ei=((tww_Vci_ei*itf)+(tww*itf_Vci_ei));
dqfh_Vbi_ei=((tww_Vbi_ei*itf)+(tww*itf_Vbi_ei));
dqfh_Vbi_ci=((tww_Vbi_ci*itf)+(tww*itf_Vbi_ci));
dqfh_Vb_ci=((tww_Vb_ci*itf)+(tww*itf_Vb_ci));
#endif
dqfh=(tww*itf);
#endif
#if defined(_DYNAMIC)
dtfh=(tww*(1+(((2*ick)/itf)/s3)));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(itf/ick))
double m00_exp(d00_exp1,(gte*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(itf/ick))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
dtef_Vtnode_GND=((tef0_t_Vtnode_GND*d00_exp1)+(tef0_t*(gte*((itf_Vtnode_GND*ick-itf*ick_Vtnode_GND)/ick/ick)*d10_logE0)*d10_exp1));
dtef_Vbi_ei=(tef0_t*(gte*(itf_Vbi_ei/ick)*d10_logE0)*d10_exp1);
dtef_Vbi_ci=(tef0_t*(gte*(itf_Vbi_ci/ick)*d10_logE0)*d10_exp1);
dtef_Vb_ci=(tef0_t*(gte*(itf_Vb_ci/ick)*d10_logE0)*d10_exp1);
dtef_Vci_ei=(tef0_t*(gte*((itf_Vci_ei*ick-itf*ick_Vci_ei)/ick/ick)*d10_logE0)*d10_exp1);
#endif
dtef=(tef0_t*d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
dqef_Vtnode_GND=(((dtef_Vtnode_GND*itf)+(dtef*itf_Vtnode_GND))/(gte+1.0));
dqef_Vbi_ei=(((dtef_Vbi_ei*itf)+(dtef*itf_Vbi_ei))/(gte+1.0));
dqef_Vbi_ci=(((dtef_Vbi_ci*itf)+(dtef*itf_Vbi_ci))/(gte+1.0));
dqef_Vb_ci=(((dtef_Vb_ci*itf)+(dtef*itf_Vb_ci))/(gte+1.0));
dqef_Vci_ei=(((dtef_Vci_ei*itf)+(dtef*itf_Vci_ei))/(gte+1.0));
#endif
dqef=((dtef*itf)/(gte+1.0));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qf_Vtnode_GND=((qf0_Vtnode_GND+dqef_Vtnode_GND)+dqfh_Vtnode_GND);
qf_Vbi_ci=((qf0_Vbi_ci+dqef_Vbi_ci)+dqfh_Vbi_ci);
qf_Vb_ci=((qf0_Vb_ci+dqef_Vb_ci)+dqfh_Vb_ci);
qf_Vbi_ei=((qf0_Vbi_ei+dqef_Vbi_ei)+dqfh_Vbi_ei);
qf_Vci_ei=((qf0_Vci_ei+dqef_Vci_ei)+dqfh_Vci_ei);
#endif
qf=((qf0+dqef)+dqfh);
#endif
#if defined(_DYNAMIC)
tf=((tf0+dtfh)+dtef);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qr_Vtnode_GND=(tr*itr_Vtnode_GND);
qr_Vbi_ci=(tr*itr_Vbi_ci);
qr_Vb_ci=(tr*itr_Vb_ci);
qr_Vbi_ei=(tr*itr_Vbi_ei);
qr_Vci_ei=(tr*itr_Vci_ei);
#endif
qr=(tr*itr);
#endif
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=(Vbiei_Vbi_ei/(mbe*VT));
DIOY_Vtnode_GND=(-Vbiei*(mbe*VT_Vtnode_GND)/(mbe*VT)/(mbe*VT));
#endif
DIOY=(Vbiei/(mbe*VT));
if
((ibes>0.0))
{
if
((DIOY>80))
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80));
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80;
}
else
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
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
le_Vs_ci=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ibe_Vtnode_GND=((ibes_t_Vtnode_GND*(le-1.0))+(ibes_t*le_Vtnode_GND));
ibe_Vbi_ei=(ibes_t*le_Vbi_ei);
#endif
ibe=(ibes_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ibe_Vtnode_GND=(-ibes_t_Vtnode_GND);
ibe_Vbi_ei=0.0;
#endif
ibe=(-ibes_t);
}
}
else
{
#if defined(_DERIVATE)
ibe_Vtnode_GND=0.0;
ibe_Vbi_ei=0.0;
#endif
ibe=0.0;
}
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=(Vbiei_Vbi_ei/(mre*VT));
DIOY_Vtnode_GND=(-Vbiei*(mre*VT_Vtnode_GND)/(mre*VT)/(mre*VT));
#endif
DIOY=(Vbiei/(mre*VT));
if
((ires>0.0))
{
if
((DIOY>80))
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80));
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80;
}
else
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
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
le_Vs_ci=0.0;
le_Vbi_ci=0.0;
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ire_Vtnode_GND=((ires_t_Vtnode_GND*(le-1.0))+(ires_t*le_Vtnode_GND));
ire_Vbi_ei=(ires_t*le_Vbi_ei);
#endif
ire=(ires_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ire_Vtnode_GND=(-ires_t_Vtnode_GND);
ire_Vbi_ei=0.0;
#endif
ire=(-ires_t);
}
}
else
{
#if defined(_DERIVATE)
ire_Vtnode_GND=0.0;
ire_Vbi_ei=0.0;
#endif
ire=0.0;
}
#if defined(_DERIVATE)
ijbe_Vtnode_GND=(ibe_Vtnode_GND+ire_Vtnode_GND);
ijbe_Vbi_ei=(ibe_Vbi_ei+ire_Vbi_ei);
#endif
ijbe=(ibe+ire);
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=(Vbici_Vbi_ci/(mbc*VT));
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-Vbici*(mbc*VT_Vtnode_GND)/(mbc*VT)/(mbc*VT));
#endif
DIOY=(Vbici/(mbc*VT));
if
((ibcs>0.0))
{
if
((DIOY>80))
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80));
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80;
}
else
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
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
le_Vs_ci=0.0;
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ijbc_Vtnode_GND=((ibcs_t_Vtnode_GND*(le-1.0))+(ibcs_t*le_Vtnode_GND));
ijbc_Vbi_ci=(ibcs_t*le_Vbi_ci);
ijbc_Vbi_ei=(ibcs_t*le_Vbi_ei);
#endif
ijbc=(ibcs_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ijbc_Vtnode_GND=(-ibcs_t_Vtnode_GND);
ijbc_Vbi_ci=0.0;
ijbc_Vbi_ei=0.0;
#endif
ijbc=(-ibcs_t);
}
}
else
{
#if defined(_DERIVATE)
ijbc_Vtnode_GND=0.0;
ijbc_Vbi_ci=0.0;
ijbc_Vbi_ei=0.0;
#endif
ijbc=0.0;
}
#if defined(_DERIVATE)
ibi_Vtnode_GND=(ijbe_Vtnode_GND+ijbc_Vtnode_GND);
ibi_Vbi_ei=(ijbe_Vbi_ei+ijbc_Vbi_ei);
ibi_Vbi_ci=ijbc_Vbi_ci;
#endif
ibi=(ijbe+ijbc);
if
((Vbici<0))
{
#if defined(_DERIVATE)
v_bord_Vtnode_GND=((eavl_t_Vtnode_GND*vdci_t)+(eavl_t*vdci_t_Vtnode_GND));
#endif
v_bord=(eavl_t*vdci_t);
if
(((vdci_t-Vbici)>v_bord))
{
{
double m00_exp(d00_exp0,(-cc))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
a_iavl_Vtnode_GND=((((kavl_t_Vtnode_GND*vdci_t-kavl_t*vdci_t_Vtnode_GND)/vdci_t/vdci_t)*d00_exp0)+((kavl_t/vdci_t)*(-cc_Vtnode_GND)*d10_exp0));
a_iavl_Vbi_ci=((kavl_t/vdci_t)*(-cc_Vbi_ci)*d10_exp0);
a_iavl_Vb_ci=((kavl_t/vdci_t)*(-cc_Vb_ci)*d10_exp0);
#endif
a_iavl=((kavl_t/vdci_t)*d00_exp0);
}
#if defined(_DERIVATE)
iavl_Vtnode_GND=((((itf_Vtnode_GND*a_iavl)+(itf*a_iavl_Vtnode_GND))*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord))))+((itf*a_iavl)*(v_bord_Vtnode_GND+((cc_Vtnode_GND*((vdci_t-Vbici)-v_bord))+((1+cc)*(vdci_t_Vtnode_GND-v_bord_Vtnode_GND))))));
iavl_Vbi_ei=itf_Vbi_ei*a_iavl*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord)));
iavl_Vbi_ci=((((itf_Vbi_ci*a_iavl)+(itf*a_iavl_Vbi_ci))*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord))))+((itf*a_iavl)*((cc_Vbi_ci*((vdci_t-Vbici)-v_bord))+((1+cc)*(-Vbici_Vbi_ci)))));
iavl_Vb_ci=((((itf_Vb_ci*a_iavl)+(itf*a_iavl_Vb_ci))*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord))))+((itf*a_iavl)*cc_Vb_ci*((vdci_t-Vbici)-v_bord)));
iavl_Vci_ei=itf_Vci_ei*a_iavl*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord)));
#endif
iavl=((itf*a_iavl)*(v_bord+((1+cc)*((vdci_t-Vbici)-v_bord))));
}
else
{
{
double m00_logE(d00_logE0,(1/cc))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1/cc))
#endif
#if defined(_DERIVATE)
lncc_Vtnode_GND=(-1*cc_Vtnode_GND/cc/cc)*d10_logE0;
lncc_Vbi_ci=(-1*cc_Vbi_ci/cc/cc)*d10_logE0;
lncc_Vb_ci=(-1*cc_Vb_ci/cc/cc)*d10_logE0;
#endif
lncc=d00_logE0;
}
{
double m00_exp(d00_exp0,(((1/zci)-1)*lncc))
double m00_exp(d00_exp1,((((-1)/zci)*lncc)-(eavl_t*d00_exp0)))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
iavl_Vtnode_GND=((((kavl_t_Vtnode_GND*itf)+(kavl_t*itf_Vtnode_GND))*d00_exp1)+((kavl_t*itf)*((((-1)/zci)*lncc_Vtnode_GND)-((eavl_t_Vtnode_GND*d00_exp0)+(eavl_t*(((1/zci)-1)*lncc_Vtnode_GND)*d10_exp0)))*d10_exp1));
iavl_Vbi_ei=(kavl_t*itf_Vbi_ei)*d00_exp1;
iavl_Vbi_ci=(((kavl_t*itf_Vbi_ci)*d00_exp1)+((kavl_t*itf)*((((-1)/zci)*lncc_Vbi_ci)-(eavl_t*(((1/zci)-1)*lncc_Vbi_ci)*d10_exp0))*d10_exp1));
iavl_Vb_ci=(((kavl_t*itf_Vb_ci)*d00_exp1)+((kavl_t*itf)*((((-1)/zci)*lncc_Vb_ci)-(eavl_t*(((1/zci)-1)*lncc_Vb_ci)*d10_exp0))*d10_exp1));
iavl_Vci_ei=(kavl_t*itf_Vci_ei)*d00_exp1;
#endif
iavl=((kavl_t*itf)*d00_exp1);
}
}
}
else
{
#if defined(_DERIVATE)
iavl_Vtnode_GND=0.0;
iavl_Vbi_ei=0.0;
iavl_Vbi_ci=0.0;
iavl_Vb_ci=0.0;
iavl_Vci_ei=0.0;
#endif
iavl=0;
}
if
((rbi0_t>0.0))
{
#if defined(_DERIVATE)
qje_Vtnode_GND=((qjei_Vtnode_GND*cje0_t-qjei*cje0_t_Vtnode_GND)/cje0_t/cje0_t);
qje_Vbi_ei=(qjei_Vbi_ei/cje0_t);
qje_Vbi_ci=(qjei_Vbi_ci/cje0_t);
qje_Vb_ci=(qjei_Vb_ci/cje0_t);
#endif
qje=(qjei/cje0_t);
#if defined(_DERIVATE)
Qz_nom_Vtnode_GND=((((qje_Vtnode_GND/vr0e)+(qjci_Vtnode_GND/vr0c))+(itf_Vtnode_GND/ickf))+(itr_Vtnode_GND/ickr));
Qz_nom_Vbi_ei=(((qje_Vbi_ei/vr0e)+(itf_Vbi_ei/ickf))+(itr_Vbi_ei/ickr));
Qz_nom_Vbi_ci=((((qje_Vbi_ci/vr0e)+(qjci_Vbi_ci/vr0c))+(itf_Vbi_ci/ickf))+(itr_Vbi_ci/ickr));
Qz_nom_Vb_ci=((((qje_Vb_ci/vr0e)+(qjci_Vb_ci/vr0c))+(itf_Vb_ci/ickf))+(itr_Vb_ci/ickr));
Qz_nom_Vci_ei=((itf_Vci_ei/ickf)+(itr_Vci_ei/ickr));
#endif
Qz_nom=((((1+(qje/vr0e))+(qjci/vr0c))+(itf/ickf))+(itr/ickr));
{
double m00_sqrt(d00_sqrt0,((Qz_nom*Qz_nom)+0.01))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Qz_nom*Qz_nom)+0.01))
#endif
#if defined(_DERIVATE)
fQz_Vtnode_GND=(0.5*(Qz_nom_Vtnode_GND+((Qz_nom_Vtnode_GND*Qz_nom)+(Qz_nom*Qz_nom_Vtnode_GND))*d10_sqrt0));
fQz_Vbi_ei=(0.5*(Qz_nom_Vbi_ei+((Qz_nom_Vbi_ei*Qz_nom)+(Qz_nom*Qz_nom_Vbi_ei))*d10_sqrt0));
fQz_Vbi_ci=(0.5*(Qz_nom_Vbi_ci+((Qz_nom_Vbi_ci*Qz_nom)+(Qz_nom*Qz_nom_Vbi_ci))*d10_sqrt0));
fQz_Vb_ci=(0.5*(Qz_nom_Vb_ci+((Qz_nom_Vb_ci*Qz_nom)+(Qz_nom*Qz_nom_Vb_ci))*d10_sqrt0));
fQz_Vci_ei=(0.5*(Qz_nom_Vci_ei+((Qz_nom_Vci_ei*Qz_nom)+(Qz_nom*Qz_nom_Vci_ei))*d10_sqrt0));
#endif
fQz=(0.5*(Qz_nom+d00_sqrt0));
}
#if defined(_DERIVATE)
rbi_Vtnode_GND=((rbi0_t_Vtnode_GND*fQz-rbi0_t*fQz_Vtnode_GND)/fQz/fQz);
rbi_Vbi_ei=(-rbi0_t*fQz_Vbi_ei/fQz/fQz);
rbi_Vbi_ci=(-rbi0_t*fQz_Vbi_ci/fQz/fQz);
rbi_Vb_ci=(-rbi0_t*fQz_Vb_ci/fQz/fQz);
rbi_Vci_ei=(-rbi0_t*fQz_Vci_ei/fQz/fQz);
#endif
rbi=(rbi0_t/fQz);
if
((ibi>0.0))
{
#if defined(_DERIVATE)
eta_Vtnode_GND=(((((fgeo*rbi_Vtnode_GND)*ibi)+((fgeo*rbi)*ibi_Vtnode_GND))*VT-((fgeo*rbi)*ibi)*VT_Vtnode_GND)/VT/VT);
eta_Vbi_ei=((((fgeo*rbi_Vbi_ei)*ibi)+((fgeo*rbi)*ibi_Vbi_ei))/VT);
eta_Vbi_ci=((((fgeo*rbi_Vbi_ci)*ibi)+((fgeo*rbi)*ibi_Vbi_ci))/VT);
eta_Vb_ci=((fgeo*rbi_Vb_ci)*ibi/VT);
eta_Vci_ei=((fgeo*rbi_Vci_ei)*ibi/VT);
#endif
eta=(((fgeo*rbi)*ibi)/VT);
if
((eta<1e-6))
{
#if defined(_DERIVATE)
rbi_Vtnode_GND=((rbi_Vtnode_GND*(1-(0.5*eta)))+(rbi*(-(0.5*eta_Vtnode_GND))));
rbi_Vbi_ei=((rbi_Vbi_ei*(1-(0.5*eta)))+(rbi*(-(0.5*eta_Vbi_ei))));
rbi_Vbi_ci=((rbi_Vbi_ci*(1-(0.5*eta)))+(rbi*(-(0.5*eta_Vbi_ci))));
rbi_Vb_ci=((rbi_Vb_ci*(1-(0.5*eta)))+(rbi*(-(0.5*eta_Vb_ci))));
rbi_Vci_ei=((rbi_Vci_ei*(1-(0.5*eta)))+(rbi*(-(0.5*eta_Vci_ei))));
#endif
rbi=(rbi*(1-(0.5*eta)));
}
else
{
{
double m00_logE(d00_logE0,(eta+1))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(eta+1))
#endif
#if defined(_DERIVATE)
rbi_Vtnode_GND=((((rbi_Vtnode_GND*d00_logE0)+(rbi*eta_Vtnode_GND*d10_logE0))*eta-(rbi*d00_logE0)*eta_Vtnode_GND)/eta/eta);
rbi_Vbi_ei=((((rbi_Vbi_ei*d00_logE0)+(rbi*eta_Vbi_ei*d10_logE0))*eta-(rbi*d00_logE0)*eta_Vbi_ei)/eta/eta);
rbi_Vbi_ci=((((rbi_Vbi_ci*d00_logE0)+(rbi*eta_Vbi_ci*d10_logE0))*eta-(rbi*d00_logE0)*eta_Vbi_ci)/eta/eta);
rbi_Vb_ci=((((rbi_Vb_ci*d00_logE0)+(rbi*eta_Vb_ci*d10_logE0))*eta-(rbi*d00_logE0)*eta_Vb_ci)/eta/eta);
rbi_Vci_ei=((((rbi_Vci_ei*d00_logE0)+(rbi*eta_Vci_ei*d10_logE0))*eta-(rbi*d00_logE0)*eta_Vci_ei)/eta/eta);
#endif
rbi=((rbi*d00_logE0)/eta);
}
}
}
}
else
{
#if defined(_DERIVATE)
rbi_Vtnode_GND=0.0;
rbi_Vbi_ei=0.0;
rbi_Vbi_ci=0.0;
rbi_Vb_ci=0.0;
rbi_Vci_ei=0.0;
#endif
rbi=0.0;
}
#if defined(_DERIVATE)
rb_Vtnode_GND=(rbi_Vtnode_GND+rbx_t_Vtnode_GND);
rb_Vbi_ei=rbi_Vbi_ei;
rb_Vbi_ci=rbi_Vbi_ci;
rb_Vb_ci=rbi_Vb_ci;
rb_Vci_ei=rbi_Vci_ei;
#endif
rb=(rbi+rbx_t);
if
((itss>0.0))
{
#if defined(_DERIVATE)
HSUM_Vtnode_GND=(msf*VT_Vtnode_GND);
#endif
HSUM=(msf*VT);
{
double m00_limexp(d00_limexp0,(Vbci/HSUM))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(Vbci/HSUM))
#endif
#if defined(_DERIVATE)
HSa_Vb_ci=(Vbci_Vb_ci/HSUM)*d10_limexp0;
HSa_Vtnode_GND=(-Vbci*HSUM_Vtnode_GND/HSUM/HSUM)*d10_limexp0;
#endif
HSa=d00_limexp0;
}
{
double m00_limexp(d00_limexp0,(Vsci/HSUM))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(Vsci/HSUM))
#endif
#if defined(_DERIVATE)
HSb_Vs_ci=(Vsci_Vs_ci/HSUM)*d10_limexp0;
HSb_Vtnode_GND=(-Vsci*HSUM_Vtnode_GND/HSUM/HSUM)*d10_limexp0;
#endif
HSb=d00_limexp0;
}
#if defined(_DERIVATE)
HSI_Tsu_Vtnode_GND=((itss_t_Vtnode_GND*(HSa-HSb))+(itss_t*(HSa_Vtnode_GND-HSb_Vtnode_GND)));
HSI_Tsu_Vb_ci=(itss_t*HSa_Vb_ci);
HSI_Tsu_Vs_ci=(itss_t*(-HSb_Vs_ci));
#endif
HSI_Tsu=(itss_t*(HSa-HSb));
}
else
{
#if defined(_DERIVATE)
HSI_Tsu_Vtnode_GND=0.0;
HSI_Tsu_Vb_ci=0.0;
HSI_Tsu_Vs_ci=0.0;
#endif
HSI_Tsu=0.0;
}
#if defined(_DERIVATE)
DIOY_Vs_ci=(Vsci_Vs_ci/(msc*VT));
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=(-Vsci*(msc*VT_Vtnode_GND)/(msc*VT)/(msc*VT));
#endif
DIOY=(Vsci/(msc*VT));
if
((iscs>0.0))
{
if
((DIOY>80))
{
#if defined(_DERIVATE)
le_Vs_ci=DIOY_Vs_ci;
le_Vbi_ci=DIOY_Vbi_ci;
le_Vbi_ei=DIOY_Vbi_ei;
le_Vtnode_GND=DIOY_Vtnode_GND;
#endif
le=(1+(DIOY-80));
#if defined(_DERIVATE)
DIOY_Vs_ci=0.0;
DIOY_Vbi_ci=0.0;
DIOY_Vbi_ei=0.0;
DIOY_Vtnode_GND=0.0;
#endif
DIOY=80;
}
else
{
#if defined(_DERIVATE)
le_Vs_ci=0.0;
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
le_Vs_ci=((le_Vs_ci*d00_limexp0)+(le*DIOY_Vs_ci*d10_limexp0));
le_Vbi_ci=((le_Vbi_ci*d00_limexp0)+(le*DIOY_Vbi_ci*d10_limexp0));
le_Vbi_ei=((le_Vbi_ei*d00_limexp0)+(le*DIOY_Vbi_ei*d10_limexp0));
le_Vtnode_GND=((le_Vtnode_GND*d00_limexp0)+(le*DIOY_Vtnode_GND*d10_limexp0));
#endif
le=(le*d00_limexp0);
}
#if defined(_DERIVATE)
ijsc_Vtnode_GND=((iscs_t_Vtnode_GND*(le-1.0))+(iscs_t*le_Vtnode_GND));
ijsc_Vs_ci=(iscs_t*le_Vs_ci);
ijsc_Vbi_ci=(iscs_t*le_Vbi_ci);
ijsc_Vbi_ei=(iscs_t*le_Vbi_ei);
#endif
ijsc=(iscs_t*(le-1.0));
if
((DIOY<=(-14.0)))
{
#if defined(_DERIVATE)
ijsc_Vtnode_GND=(-iscs_t_Vtnode_GND);
ijsc_Vs_ci=0.0;
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
ijsc_Vs_ci=0.0;
ijsc_Vbi_ci=0.0;
ijsc_Vbi_ei=0.0;
#endif
ijsc=0.0;
}
#ifdef _DYNAMIC
if
((vpts<1.0e2))
{
#ifdef _DYNAMIC
if
((cjs0_t>0.0))
{
#if defined(_DYNAMIC)
zr=(zs/4.0);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vp_Vtnode_GND=(-vds_t_Vtnode_GND);
#endif
vp=(vpts-vds_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,2.4)
double m00_exp(d00_exp1,((-d00_logE0)/zs))
#if defined(_DERIVATE)
vf_Vtnode_GND=vds_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vds_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
cmax_Vtnode_GND=(2.4*cjs0_t_Vtnode_GND);
#endif
cmax=(2.4*cjs0_t);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(vds_t/vpts))
double m00_exp(d00_exp1,((zs-zr)*d00_logE0))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(vds_t/vpts))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATE)
cr_Vtnode_GND=((cjs0_t_Vtnode_GND*d00_exp1)+(cjs0_t*((zs-zr)*(vds_t_Vtnode_GND/vpts)*d10_logE0)*d10_exp1));
#endif
cr=(cjs0_t*d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=VT_Vtnode_GND;
#endif
a=VT;
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
ve_Vs_ci=((-Vsci_Vs_ci)/a);
ve_Vci_ei=(-(vf-Vsci)*a_Vci_ei/a/a);
ve_Vbi_ei=(-(vf-Vsci)*a_Vbi_ei/a/a);
ve_Vbi_ci=(-(vf-Vsci)*a_Vbi_ci/a/a);
ve_Vtnode_GND=((vf_Vtnode_GND*a-(vf-Vsci)*a_Vtnode_GND)/a/a);
ve_Vb_ci=(-(vf-Vsci)*a_Vb_ci/a/a);
#endif
ve=((vf-Vsci)/a);
#endif
#ifdef _DYNAMIC
if
((ve<=80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,ve)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=ve_Vs_ci*d10_exp0;
ex1_Vci_ei=ve_Vci_ei*d10_exp0;
ex1_Vbi_ei=ve_Vbi_ei*d10_exp0;
ex1_Vbi_ci=ve_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=ve_Vtnode_GND*d10_exp0;
ex1_Vb_ci=ve_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
ee1_Vs_ci=ex1_Vs_ci;
ee1_Vci_ei=ex1_Vci_ei;
ee1_Vbi_ei=ex1_Vbi_ei;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj1_Vci_ei=(-((a_Vci_ei*d00_logE0)+(a*ee1_Vci_ei*d10_logE0)));
vj1_Vbi_ei=(-((a_Vbi_ei*d00_logE0)+(a*ee1_Vbi_ei*d10_logE0)));
vj1_Vs_ci=(-(a*ee1_Vs_ci*d10_logE0));
vj1_Vbi_ci=(-((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0)));
vj1_Vtnode_GND=(vf_Vtnode_GND-((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj1_Vb_ci=(-((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0)));
#endif
vj1=(vf-(a*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vj1_Vci_ei=0.0;
vj1_Vbi_ei=0.0;
vj1_Vs_ci=Vsci_Vs_ci;
vj1_Vbi_ci=0.0;
vj1_Vtnode_GND=0.0;
vj1_Vb_ci=0.0;
#endif
vj1=Vsci;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
a_Vci_ei=0.0;
a_Vbi_ei=0.0;
a_Vbi_ci=0.0;
a_Vb_ci=0.0;
a_Vtnode_GND=((0.1*vp_Vtnode_GND)+(4.0*VT_Vtnode_GND));
#endif
a=((0.1*vp)+(4.0*VT));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vr_Vci_ei=((vj1_Vci_ei*a-(vp+vj1)*a_Vci_ei)/a/a);
vr_Vbi_ei=((vj1_Vbi_ei*a-(vp+vj1)*a_Vbi_ei)/a/a);
vr_Vs_ci=(vj1_Vs_ci/a);
vr_Vbi_ci=((vj1_Vbi_ci*a-(vp+vj1)*a_Vbi_ci)/a/a);
vr_Vtnode_GND=(((vp_Vtnode_GND+vj1_Vtnode_GND)*a-(vp+vj1)*a_Vtnode_GND)/a/a);
vr_Vb_ci=((vj1_Vb_ci*a-(vp+vj1)*a_Vb_ci)/a/a);
#endif
vr=((vp+vj1)/a);
#endif
#ifdef _DYNAMIC
if
((vr<=80.0))
{
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,vr)
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
ex1_Vs_ci=vr_Vs_ci*d10_exp0;
ex1_Vci_ei=vr_Vci_ei*d10_exp0;
ex1_Vbi_ei=vr_Vbi_ei*d10_exp0;
ex1_Vbi_ci=vr_Vbi_ci*d10_exp0;
ex1_Vtnode_GND=vr_Vtnode_GND*d10_exp0;
ex1_Vb_ci=vr_Vb_ci*d10_exp0;
#endif
ex1=d00_exp0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
ee1_Vs_ci=ex1_Vs_ci;
ee1_Vci_ei=ex1_Vci_ei;
ee1_Vbi_ei=ex1_Vbi_ei;
ee1_Vbi_ci=ex1_Vbi_ci;
ee1_Vtnode_GND=ex1_Vtnode_GND;
ee1_Vb_ci=ex1_Vb_ci;
#endif
ee1=(1.0+ex1);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,ee1)
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,ee1)
#endif
#if defined(_DERIVATE)
vj2_Vci_ei=((a_Vci_ei*d00_logE0)+(a*ee1_Vci_ei*d10_logE0));
vj2_Vbi_ei=((a_Vbi_ei*d00_logE0)+(a*ee1_Vbi_ei*d10_logE0));
vj2_Vs_ci=(a*ee1_Vs_ci*d10_logE0);
vj2_Vbi_ci=((a_Vbi_ci*d00_logE0)+(a*ee1_Vbi_ci*d10_logE0));
vj2_Vtnode_GND=((-vp_Vtnode_GND)+((a_Vtnode_GND*d00_logE0)+(a*ee1_Vtnode_GND*d10_logE0)));
vj2_Vb_ci=((a_Vb_ci*d00_logE0)+(a*ee1_Vb_ci*d10_logE0));
#endif
vj2=((-vp)+(a*d00_logE0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vj2_Vci_ei=vj1_Vci_ei;
vj2_Vbi_ei=vj1_Vbi_ei;
vj2_Vs_ci=vj1_Vs_ci;
vj2_Vbi_ci=vj1_Vbi_ci;
vj2_Vtnode_GND=vj1_Vtnode_GND;
vj2_Vb_ci=vj1_Vb_ci;
#endif
vj2=vj1;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
vj4_Vs_ci=(Vsci_Vs_ci-vj1_Vs_ci);
vj4_Vci_ei=(-vj1_Vci_ei);
vj4_Vbi_ei=(-vj1_Vbi_ei);
vj4_Vbi_ci=(-vj1_Vbi_ci);
vj4_Vb_ci=(-vj1_Vb_ci);
vj4_Vtnode_GND=(-vj1_Vtnode_GND);
#endif
vj4=(Vsci-vj1);
#endif
#if defined(_DYNAMIC)
ez=(1.0-zs);
#endif
#if defined(_DYNAMIC)
ezr=(1.0-zr);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(vj1/vds_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj1/vds_t)))
#endif
#if defined(_DERIVATE)
vdj1_Vci_ei=(-(vj1_Vci_ei/vds_t))*d10_logE0;
vdj1_Vbi_ei=(-(vj1_Vbi_ei/vds_t))*d10_logE0;
vdj1_Vs_ci=(-(vj1_Vs_ci/vds_t))*d10_logE0;
vdj1_Vbi_ci=(-(vj1_Vbi_ci/vds_t))*d10_logE0;
vdj1_Vtnode_GND=(-((vj1_Vtnode_GND*vds_t-vj1*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0;
vdj1_Vb_ci=(-(vj1_Vb_ci/vds_t))*d10_logE0;
#endif
vdj1=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(vj2/vds_t)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0-(vj2/vds_t)))
#endif
#if defined(_DERIVATE)
vdj2_Vci_ei=(-(vj2_Vci_ei/vds_t))*d10_logE0;
vdj2_Vbi_ei=(-(vj2_Vbi_ei/vds_t))*d10_logE0;
vdj2_Vs_ci=(-(vj2_Vs_ci/vds_t))*d10_logE0;
vdj2_Vbi_ci=(-(vj2_Vbi_ci/vds_t))*d10_logE0;
vdj2_Vtnode_GND=(-((vj2_Vtnode_GND*vds_t-vj2*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0;
vdj2_Vb_ci=(-(vj2_Vb_ci/vds_t))*d10_logE0;
#endif
vdj2=d00_logE0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(vdj2*ez))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj1_Vci_ei=((cjs0_t*(-vdj2_Vci_ei*ez*d10_exp0))/ez);
qj1_Vbi_ei=((cjs0_t*(-vdj2_Vbi_ei*ez*d10_exp0))/ez);
qj1_Vs_ci=((cjs0_t*(-vdj2_Vs_ci*ez*d10_exp0))/ez);
qj1_Vbi_ci=((cjs0_t*(-vdj2_Vbi_ci*ez*d10_exp0))/ez);
qj1_Vtnode_GND=(((cjs0_t_Vtnode_GND*(1.0-d00_exp0))+(cjs0_t*(-vdj2_Vtnode_GND*ez*d10_exp0)))/ez);
qj1_Vb_ci=((cjs0_t*(-vdj2_Vb_ci*ez*d10_exp0))/ez);
#endif
qj1=((cjs0_t*(1.0-d00_exp0))/ez);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(vdj1*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj2_Vci_ei=((cr*(-vdj1_Vci_ei*ezr*d10_exp0))/ezr);
qj2_Vbi_ei=((cr*(-vdj1_Vbi_ei*ezr*d10_exp0))/ezr);
qj2_Vs_ci=((cr*(-vdj1_Vs_ci*ezr*d10_exp0))/ezr);
qj2_Vbi_ci=((cr*(-vdj1_Vbi_ci*ezr*d10_exp0))/ezr);
qj2_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj1_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj2_Vb_ci=((cr*(-vdj1_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj2=((cr*(1.0-d00_exp0))/ezr);
}
#endif
#if defined(_DYNAMIC)
{
double m00_exp(d00_exp0,(vdj2*ezr))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
qj3_Vci_ei=((cr*(-vdj2_Vci_ei*ezr*d10_exp0))/ezr);
qj3_Vbi_ei=((cr*(-vdj2_Vbi_ei*ezr*d10_exp0))/ezr);
qj3_Vs_ci=((cr*(-vdj2_Vs_ci*ezr*d10_exp0))/ezr);
qj3_Vbi_ci=((cr*(-vdj2_Vbi_ci*ezr*d10_exp0))/ezr);
qj3_Vtnode_GND=(((cr_Vtnode_GND*(1.0-d00_exp0))+(cr*(-vdj2_Vtnode_GND*ezr*d10_exp0)))/ezr);
qj3_Vb_ci=((cr*(-vdj2_Vb_ci*ezr*d10_exp0))/ezr);
#endif
qj3=((cr*(1.0-d00_exp0))/ezr);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjs_Vci_ei=(((qj1_Vci_ei+qj2_Vci_ei)-qj3_Vci_ei)*vds_t+(cmax*vj4_Vci_ei));
qjs_Vbi_ei=(((qj1_Vbi_ei+qj2_Vbi_ei)-qj3_Vbi_ei)*vds_t+(cmax*vj4_Vbi_ei));
qjs_Vs_ci=(((qj1_Vs_ci+qj2_Vs_ci)-qj3_Vs_ci)*vds_t+(cmax*vj4_Vs_ci));
qjs_Vbi_ci=(((qj1_Vbi_ci+qj2_Vbi_ci)-qj3_Vbi_ci)*vds_t+(cmax*vj4_Vbi_ci));
qjs_Vtnode_GND=(((((qj1_Vtnode_GND+qj2_Vtnode_GND)-qj3_Vtnode_GND)*vds_t)+(((qj1+qj2)-qj3)*vds_t_Vtnode_GND))+((cmax_Vtnode_GND*vj4)+(cmax*vj4_Vtnode_GND)));
qjs_Vb_ci=(((qj1_Vb_ci+qj2_Vb_ci)-qj3_Vb_ci)*vds_t+(cmax*vj4_Vb_ci));
#endif
qjs=((((qj1+qj2)-qj3)*vds_t)+(cmax*vj4));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjs_Vci_ei=0.0;
qjs_Vbi_ei=0.0;
qjs_Vs_ci=0.0;
qjs_Vbi_ci=0.0;
qjs_Vtnode_GND=0.0;
qjs_Vb_ci=0.0;
#endif
qjs=0.0;
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
vf_Vtnode_GND=vds_t_Vtnode_GND*(1.0-d00_exp1);
#endif
vf=(vds_t*(1.0-d00_exp1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf_Vs_ci=((-Vsci_Vs_ci)/VT);
#endif
xvf_Vbi_ei=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ei_Vs_ci=0.0;
  xvf_Vbi_ei_Vbi_ei=0.0;
  xvf_Vbi_ei_Vbi_ci=0.0;
  xvf_Vbi_ei_Vtnode_GND=0.0;
  xvf_Vbi_ei_Vb_ci=0.0;
#endif
xvf_Vbi_ci=0.0;
#if defined(_DERIVATE)
  xvf_Vbi_ci_Vs_ci=0.0;
  xvf_Vbi_ci_Vbi_ei=0.0;
  xvf_Vbi_ci_Vbi_ci=0.0;
  xvf_Vbi_ci_Vtnode_GND=0.0;
  xvf_Vbi_ci_Vb_ci=0.0;
#endif
#if defined(_DERIVATE)
xvf_Vtnode_GND=((vf_Vtnode_GND*VT-(vf-Vsci)*VT_Vtnode_GND)/VT/VT);
#endif
#if defined(_DERIVATE)
xvf_Vb_ci=0.0;
#endif
#endif
xvf=((vf-Vsci)/VT);
#endif
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((xvf*xvf)+1.921812))
#if defined(_DERIVATEFORDDX)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((xvf*xvf)+1.921812))
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
xvf2_Vs_ci=((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci))*d10_sqrt0;
#endif
xvf2_Vbi_ei=((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ei_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vs_ci*xvf+xvf_Vbi_ei*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vs_ci)+0.0));
  xvf2_Vbi_ei_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vbi_ei*xvf+xvf_Vbi_ei*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vbi_ei)+0.0));
  xvf2_Vbi_ei_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vbi_ci*xvf+xvf_Vbi_ei*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vbi_ci)+0.0));
  xvf2_Vbi_ei_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vtnode_GND*xvf+xvf_Vbi_ei*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vtnode_GND)+0.0));
  xvf2_Vbi_ei_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei))+d10_sqrt0*(+(+xvf_Vbi_ei_Vb_ci*xvf+xvf_Vbi_ei*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ei+xvf*xvf_Vbi_ei_Vb_ci)+0.0));
#endif
xvf2_Vbi_ci=((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))*d10_sqrt0;
#if defined(_DERIVATE)
  xvf2_Vbi_ci_Vs_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vs_ci*xvf)+(xvf*xvf_Vs_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vs_ci*xvf+xvf_Vbi_ci*xvf_Vs_ci+xvf_Vs_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vs_ci)+0.0));
  xvf2_Vbi_ci_Vbi_ei=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ei*xvf)+(xvf*xvf_Vbi_ei)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ei*xvf+xvf_Vbi_ci*xvf_Vbi_ei+xvf_Vbi_ei*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ei)+0.0));
  xvf2_Vbi_ci_Vbi_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vbi_ci*xvf+xvf_Vbi_ci*xvf_Vbi_ci+xvf_Vbi_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vbi_ci)+0.0));
  xvf2_Vbi_ci_Vtnode_GND=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vtnode_GND*xvf+xvf_Vbi_ci*xvf_Vtnode_GND+xvf_Vtnode_GND*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vtnode_GND)+0.0));
  xvf2_Vbi_ci_Vb_ci=(m20_sqrt(((xvf*xvf)+1.921812))*(((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci)))*((xvf_Vbi_ci*xvf)+(xvf*xvf_Vbi_ci))+d10_sqrt0*(+(+xvf_Vbi_ci_Vb_ci*xvf+xvf_Vbi_ci*xvf_Vb_ci+xvf_Vb_ci*xvf_Vbi_ci+xvf*xvf_Vbi_ci_Vb_ci)+0.0));
#endif
#if defined(_DERIVATE)
xvf2_Vtnode_GND=((xvf_Vtnode_GND*xvf)+(xvf*xvf_Vtnode_GND))*d10_sqrt0;
#endif
#if defined(_DERIVATE)
xvf2_Vb_ci=((xvf_Vb_ci*xvf)+(xvf*xvf_Vb_ci))*d10_sqrt0;
#endif
#endif
xvf2=d00_sqrt0;
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
v_j_Vs_ci=(-(VT*(xvf_Vs_ci+xvf2_Vs_ci))*0.5);
#endif
v_j_Vbi_ei=(-(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ei_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vs_ci+xvf2_Vbi_ei_Vs_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vbi_ei+xvf2_Vbi_ei_Vbi_ei))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vbi_ci+xvf2_Vbi_ei_Vbi_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vtnode_GND+xvf2_Vbi_ei_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ei_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+VT*(+xvf_Vbi_ei_Vb_ci+xvf2_Vbi_ei_Vb_ci))*0.5+(VT*(xvf_Vbi_ei+xvf2_Vbi_ei))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
v_j_Vbi_ci=(-(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.5);
#if defined(_DERIVATE)
  v_j_Vbi_ci_Vs_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vs_ci+xvf2_Vs_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vs_ci+xvf2_Vbi_ci_Vs_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vs_ci+xvf2_Vs_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ei=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ei+xvf2_Vbi_ei)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ei+xvf2_Vbi_ci_Vbi_ei))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ei+xvf2_Vbi_ei)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vbi_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vbi_ci+xvf2_Vbi_ci_Vbi_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vbi_ci+xvf2_Vbi_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vtnode_GND=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vtnode_GND+xvf2_Vtnode_GND)+VT_Vtnode_GND*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vtnode_GND+xvf2_Vbi_ci_Vtnode_GND))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+(((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND))))*0.0+(VT*(xvf+xvf2))*0.0));
  v_j_Vbi_ci_Vb_ci=(+0.0-(+(+0.0*(xvf+xvf2)+0.0*(xvf_Vb_ci+xvf2_Vb_ci)+0.0*(xvf_Vbi_ci+xvf2_Vbi_ci)+VT*(+xvf_Vbi_ci_Vb_ci+xvf2_Vbi_ci_Vb_ci))*0.5+(VT*(xvf_Vbi_ci+xvf2_Vbi_ci))*0.0+((VT*(xvf_Vb_ci+xvf2_Vb_ci)))*0.0+(VT*(xvf+xvf2))*0.0));
#endif
#if defined(_DERIVATE)
v_j_Vtnode_GND=(vf_Vtnode_GND-((VT_Vtnode_GND*(xvf+xvf2))+(VT*(xvf_Vtnode_GND+xvf2_Vtnode_GND)))*0.5);
#endif
#if defined(_DERIVATE)
v_j_Vb_ci=(-(VT*(xvf_Vb_ci+xvf2_Vb_ci))*0.5);
#endif
#endif
v_j=(vf-((VT*(xvf+xvf2))*0.5));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
x_Vtnode_GND=0.0;
x_Vci_ei=0.0;
#endif
x=(1.0-zs);
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,(1.0-(v_j/vds_t)))
double m00_exp(d00_exp1,(x*d00_logE0))
#if defined(_DERIVATEFORDDX)
double m10_logE(d10_logE0,d00_logE0,(1.0-(v_j/vds_t)))
#define d10_exp1 d00_exp1
#endif
#if defined(_DERIVATEFORDDX)
#if defined(_DERIVATE)
y_Vci_ei=(-x_Vci_ei*d00_logE0*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vs_ci=(-(x*(-(v_j_Vs_ci/vds_t))*d10_logE0)*d10_exp1);
#endif
y_Vbi_ei=(-(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ei_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*0.0)));
  y_Vbi_ei_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vs_ci/vds_t))*(-(v_j_Vbi_ei/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vs_ci/vds_t-(v_j_Vbi_ei*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ei_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vbi_ei/vds_t))*(-(v_j_Vbi_ei/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vbi_ei/vds_t-(v_j_Vbi_ei*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ei_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vbi_ci/vds_t))*(-(v_j_Vbi_ei/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vbi_ci/vds_t-(v_j_Vbi_ei*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ei_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0)))*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*(-(v_j_Vbi_ei/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vtnode_GND/vds_t-(v_j_Vbi_ei*vds_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*vds_t_Vtnode_GND/vds_t/vds_t/vds_t))))));
  y_Vbi_ei_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ei/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vb_ci/vds_t))*(-(v_j_Vbi_ei/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ei_Vb_ci/vds_t-(v_j_Vbi_ei*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
#endif
y_Vbi_ci=(-(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)*d10_exp1);
#if defined(_DERIVATE)
  y_Vbi_ci_Vci_ei=(+0.0-(m20_exp((x*d00_logE0))*(x_Vci_ei*d00_logE0)*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*0.0+x_Vci_ei*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*0.0)));
  y_Vbi_ci_Vs_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vs_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vs_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vs_ci/vds_t))*(-(v_j_Vbi_ci/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vs_ci/vds_t-(v_j_Vbi_ci*0.0+v_j_Vs_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ci_Vbi_ei=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ei/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ei/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vbi_ei/vds_t))*(-(v_j_Vbi_ci/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ei/vds_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ei*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ci_Vbi_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vbi_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vbi_ci/vds_t))*(-(v_j_Vbi_ci/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vbi_ci/vds_t-(v_j_Vbi_ci*0.0+v_j_Vbi_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
  y_Vbi_ci_Vtnode_GND=(+0.0-(m20_exp((x*d00_logE0))*(((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0)))*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0+x_Vtnode_GND*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*(-(v_j_Vbi_ci/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vtnode_GND/vds_t-(v_j_Vbi_ci*vds_t_Vtnode_GND+v_j_Vtnode_GND*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*vds_t_Vtnode_GND/vds_t/vds_t/vds_t))))));
  y_Vbi_ci_Vb_ci=(+0.0-(m20_exp((x*d00_logE0))*((x*(-(v_j_Vb_ci/vds_t))*d10_logE0))*(x*(-(v_j_Vbi_ci/vds_t))*d10_logE0)+d10_exp1*(+0.0*d00_logE0+0.0*(-(v_j_Vb_ci/vds_t))*d10_logE0+0.0*(-(v_j_Vbi_ci/vds_t))*d10_logE0+x*(m20_logE((1.0-(v_j/vds_t)))*(-(v_j_Vb_ci/vds_t))*(-(v_j_Vbi_ci/vds_t))+d10_logE0*(+0.0-(v_j_Vbi_ci_Vb_ci/vds_t-(v_j_Vbi_ci*0.0+v_j_Vb_ci*0.0+v_j*0.0)/vds_t/vds_t+2*v_j*0.0*0.0/vds_t/vds_t/vds_t))))));
#endif
#if defined(_DERIVATE)
y_Vtnode_GND=(-((x_Vtnode_GND*d00_logE0)+(x*(-((v_j_Vtnode_GND*vds_t-v_j*vds_t_Vtnode_GND)/vds_t/vds_t))*d10_logE0))*d10_exp1);
#endif
#if defined(_DERIVATE)
y_Vb_ci=(-(x*(-(v_j_Vb_ci/vds_t))*d10_logE0)*d10_exp1);
#endif
#endif
y=(1.0-d00_exp1);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjs_Vci_ei=((((cjs0_t*vds_t)*y_Vci_ei)*x-((cjs0_t*vds_t)*y)*x_Vci_ei)/x/x);
qjs_Vbi_ei=((((cjs0_t*vds_t)*y_Vbi_ei)/x)+((2.4*cjs0_t)*(-v_j_Vbi_ei)));
qjs_Vs_ci=((((cjs0_t*vds_t)*y_Vs_ci)/x)+((2.4*cjs0_t)*(Vsci_Vs_ci-v_j_Vs_ci)));
qjs_Vbi_ci=((((cjs0_t*vds_t)*y_Vbi_ci)/x)+((2.4*cjs0_t)*(-v_j_Vbi_ci)));
qjs_Vtnode_GND=(((((((cjs0_t_Vtnode_GND*vds_t)+(cjs0_t*vds_t_Vtnode_GND))*y)+((cjs0_t*vds_t)*y_Vtnode_GND))*x-((cjs0_t*vds_t)*y)*x_Vtnode_GND)/x/x)+(((2.4*cjs0_t_Vtnode_GND)*(Vsci-v_j))+((2.4*cjs0_t)*(-v_j_Vtnode_GND))));
qjs_Vb_ci=((((cjs0_t*vds_t)*y_Vb_ci)/x)+((2.4*cjs0_t)*(-v_j_Vb_ci)));
#endif
qjs=((((cjs0_t*vds_t)*y)/x)+((2.4*cjs0_t)*(Vsci-v_j)));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjs_Vci_ei=0.0;
qjs_Vbi_ei=0.0;
qjs_Vs_ci=0.0;
qjs_Vbi_ci=0.0;
qjs_Vtnode_GND=0.0;
qjs_Vb_ci=0.0;
#endif
qjs=0.0;
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
if
(((flsh==1)&&(rth>=0.001)))
{
#if defined(_DERIVATE)
pterm_Vci_c=0.0;
pterm_Vei_e=0.0;
pterm_Vb_bi=0.0;
pterm_Vs_ci=0.0;
pterm_Vtnode_GND=(it_Vtnode_GND*Vciei+((iavl_Vtnode_GND*(vdci_t-Vbici))+(iavl*vdci_t_Vtnode_GND)));
pterm_Vbi_ei=(it_Vbi_ei*Vciei+iavl_Vbi_ei*(vdci_t-Vbici));
pterm_Vbi_ci=(it_Vbi_ci*Vciei+((iavl_Vbi_ci*(vdci_t-Vbici))+(iavl*(-Vbici_Vbi_ci))));
pterm_Vb_ci=(it_Vb_ci*Vciei+iavl_Vb_ci*(vdci_t-Vbici));
pterm_Vci_ei=(((it_Vci_ei*Vciei)+(it*Vciei_Vci_ei))+iavl_Vci_ei*(vdci_t-Vbici));
#endif
pterm=((it*Vciei)+(iavl*(vdci_t-Vbici)));
}
else
{
if
(((flsh==2)&&(rth>=0.001)))
{
#if defined(_DERIVATE)
pterm_Vci_c=0.0;
pterm_Vei_e=0.0;
pterm_Vb_bi=0.0;
pterm_Vs_ci=((ijsc_Vs_ci*Vsci)+(ijsc*Vsci_Vs_ci));
pterm_Vtnode_GND=(((((Vciei*it_Vtnode_GND)+((vdci_t_Vtnode_GND*iavl)+((vdci_t-Vbici)*iavl_Vtnode_GND)))+ijbe_Vtnode_GND*Vbiei)+ijbc_Vtnode_GND*Vbici)+ijsc_Vtnode_GND*Vsci);
pterm_Vbi_ei=(((((Vciei*it_Vbi_ei)+((vdci_t-Vbici)*iavl_Vbi_ei))+((ijbe_Vbi_ei*Vbiei)+(ijbe*Vbiei_Vbi_ei)))+ijbc_Vbi_ei*Vbici)+ijsc_Vbi_ei*Vsci);
pterm_Vbi_ci=((((Vciei*it_Vbi_ci)+(((-Vbici_Vbi_ci)*iavl)+((vdci_t-Vbici)*iavl_Vbi_ci)))+((ijbc_Vbi_ci*Vbici)+(ijbc*Vbici_Vbi_ci)))+ijsc_Vbi_ci*Vsci);
pterm_Vb_ci=((Vciei*it_Vb_ci)+((vdci_t-Vbici)*iavl_Vb_ci));
pterm_Vci_ei=(((Vciei_Vci_ei*it)+(Vciei*it_Vci_ei))+((vdci_t-Vbici)*iavl_Vci_ei));
#endif
pterm=(((((Vciei*it)+((vdci_t-Vbici)*iavl))+(ijbe*Vbiei))+(ijbc*Vbici))+(ijsc*Vsci));
if
((rb>=0.001))
{
#if defined(_DERIVATE)
pterm_Vci_c=0.0;
pterm_Vei_e=0.0;
pterm_Vb_bi=(pterm_Vb_bi+(((Vbbi_Vb_bi*Vbbi)+(Vbbi*Vbbi_Vb_bi))/rb));
pterm_Vs_ci=pterm_Vs_ci;
pterm_Vtnode_GND=(pterm_Vtnode_GND+(-(Vbbi*Vbbi)*rb_Vtnode_GND/rb/rb));
pterm_Vbi_ei=(pterm_Vbi_ei+(-(Vbbi*Vbbi)*rb_Vbi_ei/rb/rb));
pterm_Vbi_ci=(pterm_Vbi_ci+(-(Vbbi*Vbbi)*rb_Vbi_ci/rb/rb));
pterm_Vb_ci=(pterm_Vb_ci+(-(Vbbi*Vbbi)*rb_Vb_ci/rb/rb));
pterm_Vci_ei=(pterm_Vci_ei+(-(Vbbi*Vbbi)*rb_Vci_ei/rb/rb));
#endif
pterm=(pterm+((Vbbi*Vbbi)/rb));
}
if
((re_t>=0.001))
{
#if defined(_DERIVATE)
pterm_Vci_c=0.0;
pterm_Vei_e=(pterm_Vei_e+(((Veie_Vei_e*Veie)+(Veie*Veie_Vei_e))/re_t));
pterm_Vb_bi=pterm_Vb_bi;
pterm_Vs_ci=pterm_Vs_ci;
pterm_Vtnode_GND=(pterm_Vtnode_GND+(-(Veie*Veie)*re_t_Vtnode_GND/re_t/re_t));
pterm_Vbi_ei=pterm_Vbi_ei;
pterm_Vbi_ci=pterm_Vbi_ci;
pterm_Vb_ci=pterm_Vb_ci;
pterm_Vci_ei=pterm_Vci_ei;
#endif
pterm=(pterm+((Veie*Veie)/re_t));
}
if
((rcx_t>=0.001))
{
#if defined(_DERIVATE)
pterm_Vci_c=(pterm_Vci_c+(((Vcic_Vci_c*Vcic)+(Vcic*Vcic_Vci_c))/rcx_t));
pterm_Vei_e=pterm_Vei_e;
pterm_Vb_bi=pterm_Vb_bi;
pterm_Vs_ci=pterm_Vs_ci;
pterm_Vtnode_GND=(pterm_Vtnode_GND+(-(Vcic*Vcic)*rcx_t_Vtnode_GND/rcx_t/rcx_t));
pterm_Vbi_ei=pterm_Vbi_ei;
pterm_Vbi_ci=pterm_Vbi_ci;
pterm_Vb_ci=pterm_Vb_ci;
pterm_Vci_ei=pterm_Vci_ei;
#endif
pterm=(pterm+((Vcic*Vcic)/rcx_t));
}
}
}
#if defined(_DERIVATE)
Ibici_Vtnode_GND=(ijbc_Vtnode_GND-iavl_Vtnode_GND);
Ibici_Vbi_ci=(ijbc_Vbi_ci-iavl_Vbi_ci);
Ibici_Vbi_ei=(ijbc_Vbi_ei-iavl_Vbi_ei);
Ibici_Vb_ci=(-iavl_Vb_ci);
Ibici_Vci_ei=(-iavl_Vci_ei);
#endif
Ibici=(ijbc-iavl);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbci_Vb_ci=(cbcpar*Vbci_Vb_ci);
#endif
Qbci=(cbcpar*Vbci);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbe_Vb_e=(cbepar*Vbe_Vb_e);
#endif
Qbe=(cbepar*Vbe);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbici_Vbi_ci=(qjcii_Vbi_ci+qr_Vbi_ci);
Qbici_Vtnode_GND=(qjcii_Vtnode_GND+qr_Vtnode_GND);
Qbici_Vb_ci=(qjcii_Vb_ci+qr_Vb_ci);
Qbici_Vbi_ei=qr_Vbi_ei;
Qbici_Vci_ei=qr_Vci_ei;
#endif
Qbici=(qjcii+qr);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbiei_Vtnode_GND=(qjei_Vtnode_GND+qf_Vtnode_GND);
Qbiei_Vbi_ei=(qjei_Vbi_ei+qf_Vbi_ei);
Qbiei_Vbi_ci=(qjei_Vbi_ci+qf_Vbi_ci);
Qbiei_Vb_ci=(qjei_Vb_ci+qf_Vb_ci);
Qbiei_Vci_ei=qf_Vci_ei;
#endif
Qbiei=(qjei+qf);
#endif
#if defined(_DERIVATE)
ijsc_Vtnode_GND=(HICUMtype*ijsc_Vtnode_GND);
ijsc_Vs_ci=(HICUMtype*ijsc_Vs_ci);
ijsc_Vbi_ci=(HICUMtype*ijsc_Vbi_ci);
ijsc_Vbi_ei=(HICUMtype*ijsc_Vbi_ei);
#endif
ijsc=(HICUMtype*ijsc);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjs_Vci_ei=(HICUMtype*qjs_Vci_ei);
qjs_Vbi_ei=(HICUMtype*qjs_Vbi_ei);
qjs_Vs_ci=(HICUMtype*qjs_Vs_ci);
qjs_Vbi_ci=(HICUMtype*qjs_Vbi_ci);
qjs_Vtnode_GND=(HICUMtype*qjs_Vtnode_GND);
qjs_Vb_ci=(HICUMtype*qjs_Vb_ci);
#endif
qjs=(HICUMtype*qjs);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qjcx_Vbi_ci=(HICUMtype*qjcx_Vbi_ci);
qjcx_Vtnode_GND=(HICUMtype*qjcx_Vtnode_GND);
qjcx_Vb_ci=(HICUMtype*qjcx_Vb_ci);
#endif
qjcx=(HICUMtype*qjcx);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbci_Vb_ci=(HICUMtype*Qbci_Vb_ci);
#endif
Qbci=(HICUMtype*Qbci);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbe_Vb_e=(HICUMtype*Qbe_Vb_e);
#endif
Qbe=(HICUMtype*Qbe);
#endif
#if defined(_DERIVATE)
Ibici_Vtnode_GND=(HICUMtype*Ibici_Vtnode_GND);
Ibici_Vbi_ci=(HICUMtype*Ibici_Vbi_ci);
Ibici_Vbi_ei=(HICUMtype*Ibici_Vbi_ei);
Ibici_Vb_ci=(HICUMtype*Ibici_Vb_ci);
Ibici_Vci_ei=(HICUMtype*Ibici_Vci_ei);
#endif
Ibici=(HICUMtype*Ibici);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbici_Vbi_ci=(HICUMtype*Qbici_Vbi_ci);
Qbici_Vtnode_GND=(HICUMtype*Qbici_Vtnode_GND);
Qbici_Vb_ci=(HICUMtype*Qbici_Vb_ci);
Qbici_Vbi_ei=(HICUMtype*Qbici_Vbi_ei);
Qbici_Vci_ei=(HICUMtype*Qbici_Vci_ei);
#endif
Qbici=(HICUMtype*Qbici);
#endif
#if defined(_DERIVATE)
ijbe_Vtnode_GND=(HICUMtype*ijbe_Vtnode_GND);
ijbe_Vbi_ei=(HICUMtype*ijbe_Vbi_ei);
#endif
ijbe=(HICUMtype*ijbe);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qbiei_Vtnode_GND=(HICUMtype*Qbiei_Vtnode_GND);
Qbiei_Vbi_ei=(HICUMtype*Qbiei_Vbi_ei);
Qbiei_Vbi_ci=(HICUMtype*Qbiei_Vbi_ci);
Qbiei_Vb_ci=(HICUMtype*Qbiei_Vb_ci);
Qbiei_Vci_ei=(HICUMtype*Qbiei_Vci_ei);
#endif
Qbiei=(HICUMtype*Qbiei);
#endif
#if defined(_DERIVATE)
it_Vtnode_GND=(HICUMtype*it_Vtnode_GND);
it_Vbi_ei=(HICUMtype*it_Vbi_ei);
it_Vbi_ci=(HICUMtype*it_Vbi_ci);
it_Vb_ci=(HICUMtype*it_Vb_ci);
it_Vci_ei=(HICUMtype*it_Vci_ei);
#endif
it=(HICUMtype*it);
_load_static_residual2(bi,ei,(1.0e-12*BP(bi,ei)));
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ei,bi,ei,(1.0e-12));
#endif
_load_static_residual2(bi,ci,(1.0e-12*BP(bi,ci)));
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ci,bi,ci,(1.0e-12));
#endif
_load_static_residual2(b,s,HSI_Tsu);
#if defined(_DERIVATE)
_load_static_jacobian4(b,s,s,ci,HSI_Tsu_Vs_ci);
_load_static_jacobian4(b,s,b,ci,HSI_Tsu_Vb_ci);
_load_static_jacobian2s(b,s,tnode,HSI_Tsu_Vtnode_GND);
#endif
_load_static_residual2(s,ci,ijsc);
#if defined(_DERIVATE)
_load_static_jacobian4(s,ci,bi,ei,ijsc_Vbi_ei);
_load_static_jacobian4(s,ci,bi,ci,ijsc_Vbi_ci);
_load_static_jacobian4(s,ci,s,ci,ijsc_Vs_ci);
_load_static_jacobian2s(s,ci,tnode,ijsc_Vtnode_GND);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(s,ci,_DDT(qjs));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(s,ci,b,ci,qjs_Vb_ci);
_load_dynamic_jacobian2s(s,ci,tnode,qjs_Vtnode_GND);
_load_dynamic_jacobian4(s,ci,bi,ci,qjs_Vbi_ci);
_load_dynamic_jacobian4(s,ci,s,ci,qjs_Vs_ci);
_load_dynamic_jacobian4(s,ci,bi,ei,qjs_Vbi_ei);
_load_dynamic_jacobian4(s,ci,ci,ei,qjs_Vci_ei);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,ci,_DDT(qjcx));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,ci,b,ci,qjcx_Vb_ci);
_load_dynamic_jacobian2s(b,ci,tnode,qjcx_Vtnode_GND);
_load_dynamic_jacobian4(b,ci,bi,ci,qjcx_Vbi_ci);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,ci,_DDT(Qbci));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,ci,b,ci,Qbci_Vb_ci);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,e,_DDT(Qbe));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,e,b,e,Qbe_Vb_e);
#endif
#endif
if
((re>=0.001))
{
_load_static_residual2(ei,e,(Veie/re_t));
#if defined(_DERIVATE)
_load_static_jacobian2s(ei,e,tnode,(-Veie*re_t_Vtnode_GND/re_t/re_t));
_load_static_jacobian4(ei,e,ei,e,(Veie_Vei_e/re_t));
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
((rcx>=0.001))
{
_load_static_residual2(ci,c,(Vcic/rcx_t));
#if defined(_DERIVATE)
_load_static_jacobian2s(ci,c,tnode,(-Vcic*rcx_t_Vtnode_GND/rcx_t/rcx_t));
_load_static_jacobian4(ci,c,ci,c,(Vcic_Vci_c/rcx_t));
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
(((rbi0>=0.001)||(rbx>=0.001)))
{
_load_static_residual2(b,bi,(Vbbi/rb));
#if defined(_DERIVATE)
_load_static_jacobian4(b,bi,ci,ei,(-Vbbi*rb_Vci_ei/rb/rb));
_load_static_jacobian4(b,bi,b,ci,(-Vbbi*rb_Vb_ci/rb/rb));
_load_static_jacobian4(b,bi,bi,ci,(-Vbbi*rb_Vbi_ci/rb/rb));
_load_static_jacobian4(b,bi,bi,ei,(-Vbbi*rb_Vbi_ei/rb/rb));
_load_static_jacobian2s(b,bi,tnode,(-Vbbi*rb_Vtnode_GND/rb/rb));
_load_static_jacobian4(b,bi,b,bi,(Vbbi_Vb_bi/rb));
#endif
}
else
{
_load_static_residual2(b,bi,(BP(b,bi)/1e-6));
#if defined(_DERIVATE)
_load_static_jacobian4(b,bi,b,bi,(1/1e-6));
#endif
}
_load_static_residual2(bi,ci,Ibici);
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ci,ci,ei,Ibici_Vci_ei);
_load_static_jacobian4(bi,ci,b,ci,Ibici_Vb_ci);
_load_static_jacobian4(bi,ci,bi,ei,Ibici_Vbi_ei);
_load_static_jacobian4(bi,ci,bi,ci,Ibici_Vbi_ci);
_load_static_jacobian2s(bi,ci,tnode,Ibici_Vtnode_GND);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bi,ci,_DDT(Qbici));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bi,ci,ci,ei,Qbici_Vci_ei);
_load_dynamic_jacobian4(bi,ci,bi,ei,Qbici_Vbi_ei);
_load_dynamic_jacobian4(bi,ci,b,ci,Qbici_Vb_ci);
_load_dynamic_jacobian2s(bi,ci,tnode,Qbici_Vtnode_GND);
_load_dynamic_jacobian4(bi,ci,bi,ci,Qbici_Vbi_ci);
#endif
#endif
_load_static_residual2(bi,ei,ijbe);
#if defined(_DERIVATE)
_load_static_jacobian4(bi,ei,bi,ei,ijbe_Vbi_ei);
_load_static_jacobian2s(bi,ei,tnode,ijbe_Vtnode_GND);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(bi,ei,_DDT(Qbiei));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(bi,ei,ci,ei,Qbiei_Vci_ei);
_load_dynamic_jacobian4(bi,ei,b,ci,Qbiei_Vb_ci);
_load_dynamic_jacobian4(bi,ei,bi,ci,Qbiei_Vbi_ci);
_load_dynamic_jacobian4(bi,ei,bi,ei,Qbiei_Vbi_ei);
_load_dynamic_jacobian2s(bi,ei,tnode,Qbiei_Vtnode_GND);
#endif
#endif
_load_static_residual2(ci,ei,it);
#if defined(_DERIVATE)
_load_static_jacobian4(ci,ei,ci,ei,it_Vci_ei);
_load_static_jacobian4(ci,ei,b,ci,it_Vb_ci);
_load_static_jacobian4(ci,ei,bi,ci,it_Vbi_ci);
_load_static_jacobian4(ci,ei,bi,ei,it_Vbi_ei);
_load_static_jacobian2s(ci,ei,tnode,it_Vtnode_GND);
#endif
if
(((flsh==0)||(rth<0.001)))
{
_load_static_residual1(tnode,(Vrth/0.001));
#if defined(_DERIVATE)
_load_static_jacobian1(tnode,tnode,(Vrth_Vtnode_GND/0.001));
#endif
}
else
{
_load_static_residual1(tnode,((Vrth/rth)-pterm));
#if defined(_DERIVATE)
_load_static_jacobian2p(tnode,ci,ei,(-pterm_Vci_ei));
_load_static_jacobian2p(tnode,b,ci,(-pterm_Vb_ci));
_load_static_jacobian2p(tnode,bi,ci,(-pterm_Vbi_ci));
_load_static_jacobian2p(tnode,bi,ei,(-pterm_Vbi_ei));
_load_static_jacobian2p(tnode,s,ci,(-pterm_Vs_ci));
_load_static_jacobian2p(tnode,b,bi,(-pterm_Vb_bi));
_load_static_jacobian2p(tnode,ei,e,(-pterm_Vei_e));
_load_static_jacobian2p(tnode,ci,c,(-pterm_Vci_c));
_load_static_jacobian1(tnode,tnode,((Vrth_Vtnode_GND/rth)-pterm_Vtnode_GND));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(tnode,_DDT((cth*Vrth)));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(tnode,tnode,(cth*Vrth_Vtnode_GND));
#endif
#endif
}
#if defined(_DERIVATE)
fourkt_Vtnode_GND=((4.0*1.3806503e-23)*Tdev_Vtnode_GND);
#endif
fourkt=((4.0*1.3806503e-23)*Tdev);
if
(((rbx>=0.001)||(rbi0>=0.001)))
{
_save_whitenoise2(b,bi,(fourkt/rb),NULL);
}
if
((rcx>=0.001))
{
_save_whitenoise2(ci,c,(fourkt/rcx_t),NULL);
}
if
((re>=0.001))
{
_save_whitenoise2(ei,e,(fourkt/re_t),NULL);
}
twoq=(2.0*1.602176462e-19);
_save_whitenoise2(bi,ei,(twoq*ijbe),NULL);
_save_whitenoise2(ci,ei,(twoq*it),NULL);
{
double m00_pow(d00_pow0,ijbe,af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,ijbe,af)
#endif
#if defined(_DERIVATE)
flicker_Pwr_Vtnode_GND=(kf*(d10_pow0*ijbe_Vtnode_GND));
flicker_Pwr_Vbi_ei=(kf*(d10_pow0*ijbe_Vbi_ei));
#endif
flicker_Pwr=(kf*d00_pow0);
}
_save_flickernoise2(bi,ei,flicker_Pwr,1.0,NULL);

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void hic0_full::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 8; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 8; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void hic0_full::saveOperatingPoints (void)
{
  // save global instance operating points
  setOperatingPoint ("qjci", qjci);
  setOperatingPoint ("qjei", qjei);
  setOperatingPoint ("cjei", cjei);
  setOperatingPoint ("it", it);
  setOperatingPoint ("ijbc", ijbc);
  setOperatingPoint ("iavl", iavl);
  setOperatingPoint ("ijsc", ijsc);
  setOperatingPoint ("Ieei", Ieei);
  setOperatingPoint ("Icci", Icci);
  setOperatingPoint ("Ibbi", Ibbi);
  setOperatingPoint ("Ibici", Ibici);
  setOperatingPoint ("ijbe", ijbe);
}

/* Load operating points. */
void hic0_full::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void hic0_full::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void hic0_full::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void hic0_full::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix hic0_full::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (8);

  for (int i1 = 0; i1 < 8; i1++) {
    for (int i2 = 0; i2 < 8; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void hic0_full::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void hic0_full::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void hic0_full::initTR (void)
{
  setStates (2 * 8 * 8);
  initDC ();
}

/* Perform transient analysis iteration step. */
void hic0_full::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 8; i1++) {
  for (i2 = 0; i2 < 8; i2++) {
    state = 2 * (i2 + 8 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 8; i1++) {
    state = 2 * (i1 + 8 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 8; i1++) {
  for (i2 = 0; i2 < 8; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 8; i3++) {
  for (i4 = 0; i4 < 8; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 8; i1++) {
  for (i2 = 0; i2 < 8; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 8; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 8; i1++) {
  for (i3 = 0; i3 < 8; i3++) {
  for (i4 = 0; i4 < 8; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 8; i1++) {
  for (i3 = 0; i3 < 8; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix hic0_full::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (8);

  _load_flickernoise2 (bi,ei,_flicker_pwr[bi][ei],_flicker_exp[bi][ei]);
  _load_whitenoise2 (ci,ei,_white_pwr[ci][ei]);
  _load_whitenoise2 (b,bi,_white_pwr[b][bi]);
  _load_whitenoise2 (ci,c,_white_pwr[ci][c]);
  _load_whitenoise2 (ei,e,_white_pwr[ei][e]);
  _load_whitenoise2 (bi,ei,_white_pwr[bi][ei]);

  return cy;
}

/* Perform AC noise computations. */
void hic0_full::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void hic0_full::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void hic0_full::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void hic0_full::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 8; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 8; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "hic0_full.defs.h"
