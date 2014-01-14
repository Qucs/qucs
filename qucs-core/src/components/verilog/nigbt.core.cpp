/*
 * nigbt.core.cpp - device implementations for nigbt module
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

#include "nigbt.analogfunction.h"
#include "component.h"
#include "device.h"
#include "nigbt.core.h"

#ifndef CIR_nigbt
#define CIR_nigbt -1
#endif

// external nodes
#define C 0
#define G 1
#define E 2
// internal nodes
#define e 3
#define b 4
#define var_Ql 5
#define var_Mucinv 6
#define var_Nsat 7
#define var_dVecdt 8

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
#define _modelname              "nigbt"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
nigbt::nigbt() : circuit (9)
{
  type = CIR_nigbt;
}

/* Initialization of model. */
void nigbt::initModel (void)
{
  // create internal nodes
  setInternalNode (e, "e");
  setInternalNode (b, "b");
  setInternalNode (var_Ql, "var_Ql");
  setInternalNode (var_Mucinv, "var_Mucinv");
  setInternalNode (var_Nsat, "var_Nsat");
  setInternalNode (var_dVecdt, "var_dVecdt");

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
void nigbt::initDC (void)
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
void nigbt::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void nigbt::initVerilog (void)
{
  // initialization of noise variables

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 9; i1++) {
  for (i2 = 0; i2 < 9; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 9; i1++) {
  for (i2 = 0; i2 < 9; i2++) {
  for (i3 = 0; i3 < 9; i3++) {
  for (i4 = 0; i4 < 9; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 9; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 9; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void nigbt::loadVariables (void)
{
  Agd = getPropertyDouble ("Agd");
  Area = getPropertyDouble ("Area");
  Kp = getPropertyDouble ("Kp");
  Tau = getPropertyDouble ("Tau");
  Wb = getPropertyDouble ("Wb");
  BVf = getPropertyDouble ("BVf");
  BVn = getPropertyDouble ("BVn");
  Cgs = getPropertyDouble ("Cgs");
  Coxd = getPropertyDouble ("Coxd");
  Jsne = getPropertyDouble ("Jsne");
  Kf = getPropertyDouble ("Kf");
  Mun = getPropertyDouble ("Mun");
  Mup = getPropertyDouble ("Mup");
  Nb = getPropertyDouble ("Nb");
  Theta = getPropertyDouble ("Theta");
  Vt = getPropertyDouble ("Vt");
  Vtd = getPropertyDouble ("Vtd");
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
void nigbt::initializeModel (void)
{
double Dn;
double Nh;
#if defined(_DYNAMIC)
#endif
double To;
{
Tj=_circuit_temp;
To=(Tnom+273.15);
A=Area;
#if defined(_DYNAMIC)
Ads=(A-Agd);
#endif
B=(Mun/Mup);
#if defined(_DYNAMIC)
Nh=2.4e17;
#endif
Nl=Nb;
Ni=1.05e10;
{
double m00_pow(d00_pow0,Tj,1.5)
double m00_exp(d00_exp1,(7000.0/Tj))
Ni=((3.88e16*d00_pow0)/d00_exp1);
}
{
double m00_pow(d00_pow0,(300/Tj),0.87)
Vnsat=(10.0e6*d00_pow0);
}
{
double m00_pow(d00_pow0,(300/Tj),0.52)
Vpsat=(8.37e6*d00_pow0);
}
{
double m00_pow(d00_pow0,(Tj/300),1.5)
Alpha1=(1.04e21*d00_pow0);
}
{
double m00_pow(d00_pow0,(Tj/300),2.0)
Alpha2=(7.45e13*d00_pow0);
}
{
double m00_pow(d00_pow0,(Tj/To),(Jsne*A))
double m00_exp(d00_exp1,(14000*((1/Tj)-(1/To))))
Isne=(((Jsne*A)*d00_pow0)/d00_exp1);
}
{
double m00_pow(d00_pow0,(To/Tj),Kp)
KpT=(Kp*d00_pow0);
}
Wl=Wb;
Dp=(Mup*m00_vt(Tj));
#if defined(_DYNAMIC)
Dn=(Mun*m00_vt(Tj));
#endif
}
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void nigbt::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void nigbt::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void nigbt::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void nigbt::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double iNsat;
#if defined(_DERIVATE)
double iNsat_VC_e;
double iNsat_Vvar_Ql_GND;
double iNsat_Vb_E;
double iNsat_Vvar_Nsat_GND;
double iNsat_Vvar_Mucinv_GND;
double iNsat_Vvar_dVecdt_GND;
double iNsat_VG_E;
#endif
double iQl;
#if defined(_DERIVATE)
double iQl_Vb_E;
double iQl_Vvar_Nsat_GND;
double iQl_Vvar_Ql_GND;
double iQl_Vvar_Mucinv_GND;
#endif
double iMucinv;
#if defined(_DERIVATE)
double iMucinv_Vvar_Ql_GND;
double iMucinv_Vvar_Mucinv_GND;
double iMucinv_Vb_E;
double iMucinv_Vvar_Nsat_GND;
#endif
#if defined(_DYNAMIC)
double Qmult;
#if defined(_DERIVATE)
double Qmult_Vb_E;
double Qmult_Vvar_Nsat_GND;
double Qmult_Vvar_Ql_GND;
double Qmult_Ve_E;
#endif
#endif
#if defined(_DYNAMIC)
double Qcer;
#if defined(_DERIVATE)
double Qcer_Vvar_Ql_GND;
double Qcer_Vb_E;
double Qcer_Vvar_Nsat_GND;
double Qcer_Ve_E;
#endif
#endif
#if defined(_DYNAMIC)
double Qgd;
#if defined(_DERIVATE)
double Qgd_Vb_G;
double Qgd_Vvar_Nsat_GND;
#endif
#endif
double Imult;
#if defined(_DERIVATE)
double Imult_Vb_E;
double Imult_Vvar_Nsat_GND;
double Imult_VG_E;
double Imult_VC_e;
double Imult_Vvar_Ql_GND;
double Imult_Vvar_Mucinv_GND;
double Imult_Vvar_dVecdt_GND;
#endif
double Igen;
#if defined(_DERIVATE)
double Igen_Vb_E;
double Igen_Vvar_Nsat_GND;
#endif
double Imos;
#if defined(_DERIVATE)
double Imos_Vb_E;
double Imos_VG_E;
#endif
double Ibss;
#if defined(_DERIVATE)
double Ibss_Vvar_Ql_GND;
double Ibss_Vb_E;
double Ibss_Vvar_Nsat_GND;
#endif
double Ic;
#if defined(_DERIVATE)
double Ic_VC_e;
double Ic_Vvar_Ql_GND;
double Ic_Vb_E;
double Ic_Vvar_Nsat_GND;
double Ic_Vvar_Mucinv_GND;
double Ic_Vvar_dVecdt_GND;
#endif
double Icss;
#if defined(_DERIVATE)
double Icss_VC_e;
double Icss_Vvar_Ql_GND;
double Icss_Vb_E;
double Icss_Vvar_Nsat_GND;
double Icss_Vvar_Mucinv_GND;
#endif
double It;
#if defined(_DERIVATE)
double It_VC_e;
double It_Vvar_Ql_GND;
double It_Vb_E;
double It_Vvar_Nsat_GND;
double It_Vvar_Mucinv_GND;
#endif
double M;
#if defined(_DERIVATE)
double M_Vb_E;
double M_Vvar_Nsat_GND;
#endif
double Vnrt;
#if defined(_DERIVATE)
double Vnrt_Vb_E;
#endif
double BVcbo;
#if defined(_DERIVATE)
double BVcbo_Vvar_Nsat_GND;
#endif
double Vebq;
double Vebd;
double Vebj;
double Rb;
#if defined(_DERIVATE)
double Rb_Vvar_Ql_GND;
double Rb_Vb_E;
double Rb_Vvar_Nsat_GND;
double Rb_Vvar_Mucinv_GND;
#endif
double Neff;
#if defined(_DERIVATE)
double Neff_Vb_E;
double Neff_Vvar_Nsat_GND;
double Neff_Vvar_Mucinv_GND;
double Neff_Vvar_Ql_GND;
#endif
double Deltap;
#if defined(_DERIVATE)
double Deltap_Vvar_Ql_GND;
double Deltap_Vvar_Mucinv_GND;
double Deltap_Vb_E;
double Deltap_Vvar_Nsat_GND;
#endif
double Plo;
#if defined(_DERIVATE)
double Plo_Vvar_Ql_GND;
double Plo_Vvar_Mucinv_GND;
double Plo_Vb_E;
double Plo_Vvar_Nsat_GND;
#endif
double Tanh;
#if defined(_DERIVATE)
double Tanh_Vb_E;
double Tanh_Vvar_Nsat_GND;
double Tanh_Vvar_Mucinv_GND;
#endif
double WL2;
#if defined(_DERIVATE)
double WL2_Vb_E;
double WL2_Vvar_Nsat_GND;
double WL2_Vvar_Mucinv_GND;
#endif
double WL;
#if defined(_DERIVATE)
double WL_Vb_E;
double WL_Vvar_Nsat_GND;
double WL_Vvar_Mucinv_GND;
#endif
double L;
#if defined(_DERIVATE)
double L_Vvar_Mucinv_GND;
#endif
double Dc;
#if defined(_DERIVATE)
double Dc_Vvar_Mucinv_GND;
#endif
double Mueff;
#if defined(_DERIVATE)
double Mueff_Vvar_Mucinv_GND;
double Mueff_Vvar_Ql_GND;
double Mueff_Vb_E;
double Mueff_Vvar_Nsat_GND;
#endif
double Mupc;
#if defined(_DERIVATE)
double Mupc_Vvar_Mucinv_GND;
#endif
double Munc;
#if defined(_DERIVATE)
double Munc_Vvar_Mucinv_GND;
#endif
#if defined(_DYNAMIC)
double Cgd;
#if defined(_DERIVATE)
double Cgd_Vb_G;
double Cgd_Vvar_Nsat_GND;
#endif
#endif
#if defined(_DYNAMIC)
double Cgdj;
#if defined(_DERIVATE)
double Cgdj_Vb_G;
double Cgdj_Vvar_Nsat_GND;
#endif
#endif
double Cdsj;
double Ccer;
#if defined(_DERIVATE)
double Ccer_Vvar_Ql_GND;
double Ccer_Vb_E;
double Ccer_Vvar_Nsat_GND;
#endif
double Cbcj;
#if defined(_DERIVATE)
double Cbcj_Vb_E;
double Cbcj_Vvar_Nsat_GND;
#endif
double Qbi;
double Qb;
#if defined(_DERIVATE)
double Qb_Vb_E;
double Qb_Vvar_Nsat_GND;
#endif
#if defined(_DYNAMIC)
double Qds;
#if defined(_DERIVATE)
double Qds_Vb_E;
double Qds_Vvar_Nsat_GND;
#endif
#endif
#if defined(_DYNAMIC)
double Qgs;
#if defined(_DERIVATE)
double Qgs_VG_E;
#endif
#endif
double W;
#if defined(_DERIVATE)
double W_Vb_E;
double W_Vvar_Nsat_GND;
#endif
double Wbcj;
#if defined(_DERIVATE)
double Wbcj_Vb_E;
double Wbcj_Vvar_Nsat_GND;
#endif
double Wdsj;
#if defined(_DYNAMIC)
double Wgdj;
#if defined(_DERIVATE)
double Wgdj_Vb_G;
double Wgdj_Vvar_Nsat_GND;
#endif
#endif
double Nscl;
#if defined(_DERIVATE)
double Nscl_Vvar_Nsat_GND;
#endif
double dVecdt;
#if defined(_DERIVATE)
double dVecdt_Vvar_dVecdt_GND;
#endif
double Mucinv;
#if defined(_DERIVATE)
double Mucinv_Vvar_Mucinv_GND;
#endif
double Nsat;
#if defined(_DERIVATE)
double Nsat_Vvar_Nsat_GND;
#endif
double Ql;
#if defined(_DERIVATE)
double Ql_Vvar_Ql_GND;
#endif
double Vth;
#if defined(_DERIVATE)
double Vth_VG_E;
#endif
double Vbc;
#if defined(_DERIVATE)
double Vbc_Vb_E;
#endif
double Vec;
#if defined(_DERIVATE)
double Vec_Ve_E;
#endif
double Vae;
#if defined(_DERIVATE)
double Vae_VC_e;
#endif
double Veb;
#if defined(_DERIVATE)
double Veb_Ve_b;
#endif
double Vds;
#if defined(_DERIVATE)
double Vds_Vb_E;
#endif
double Vgs;
#if defined(_DERIVATE)
double Vgs_VG_E;
#endif
double Vdg;
#if defined(_DERIVATE)
double Vdg_Vb_G;
#endif
#if defined(_DERIVATE)
Vdg_Vb_G=1.0;
#endif
Vdg=BP(b,G);
#if defined(_DERIVATE)
Vgs_VG_E=1.0;
#endif
Vgs=BP(G,E);
#if defined(_DERIVATE)
Vds_Vb_E=1.0;
#endif
Vds=BP(b,E);
#if defined(_DERIVATE)
Veb_Ve_b=1.0;
#endif
Veb=BP(e,b);
#if defined(_DERIVATE)
Vae_VC_e=1.0;
#endif
Vae=BP(C,e);
#if defined(_DERIVATE)
Vec_Ve_E=1.0;
#endif
Vec=BP(e,E);
#if defined(_DERIVATE)
Vbc_Vb_E=Vds_Vb_E;
#endif
Vbc=Vds;
#if defined(_DERIVATE)
Vth_VG_E=Vgs_VG_E;
#endif
Vth=(Vgs-Vt);
#if defined(_DERIVATE)
Ql_Vvar_Ql_GND=1.0;
#endif
Ql=NP(var_Ql);
#if defined(_DERIVATE)
Nsat_Vvar_Nsat_GND=1.0;
#endif
Nsat=NP(var_Nsat);
#if defined(_DERIVATE)
Mucinv_Vvar_Mucinv_GND=1.0;
#endif
Mucinv=NP(var_Mucinv);
#if defined(_DERIVATE)
dVecdt_Vvar_dVecdt_GND=1.0;
#endif
dVecdt=NP(var_dVecdt);
{
double m00_max(d00_max0,(Nl+Nsat),1e-20)
#if defined(_DERIVATE)
double m10_max(d10_max0,d00_max0,(Nl+Nsat),1e-20)
#endif
#if defined(_DERIVATE)
Nscl_Vvar_Nsat_GND=(d10_max0*Nsat_Vvar_Nsat_GND);
#endif
Nscl=d00_max0;
}
#ifdef _DYNAMIC
if
((Vdg>(-Vtd)))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*(Vdg+Vtd))/1.602176462e-19)/Nscl))
double m00_min(d00_min1,d00_sqrt0,Wl)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((((2*1.03594e-10)*(Vdg+Vtd))/1.602176462e-19)/Nscl))
double m10_min(d10_min1,d00_min1,d00_sqrt0,Wl)
#endif
#if defined(_DERIVATE)
Wgdj_Vb_G=(d10_min1*((((2*1.03594e-10)*Vdg_Vb_G)/1.602176462e-19)/Nscl)*d10_sqrt0);
Wgdj_Vvar_Nsat_GND=(d10_min1*(-(((2*1.03594e-10)*(Vdg+Vtd))/1.602176462e-19)*Nscl_Vvar_Nsat_GND/Nscl/Nscl)*d10_sqrt0);
#endif
Wgdj=d00_min1;
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Wgdj_Vb_G=0.0;
Wgdj_Vvar_Nsat_GND=0.0;
#endif
Wgdj=0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((Vds>(-0.6)))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*(Vds+0.6))/1.602176462e-19)/Nscl))
double m00_min(d00_min1,d00_sqrt0,Wl)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((((2*1.03594e-10)*(Vds+0.6))/1.602176462e-19)/Nscl))
double m10_min(d10_min1,d00_min1,d00_sqrt0,Wl)
#endif
Wdsj=d00_min1;
}
#endif
}
else
{
#if defined(_DYNAMIC)
Wdsj=0;
#endif
}
#endif /* if (...) */
if
((Vbc>(-0.6)))
{
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*(Vbc+0.6))/1.602176462e-19)/Nscl))
double m00_min(d00_min1,d00_sqrt0,Wl)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((((2*1.03594e-10)*(Vbc+0.6))/1.602176462e-19)/Nscl))
double m10_min(d10_min1,d00_min1,d00_sqrt0,Wl)
#endif
#if defined(_DERIVATE)
Wbcj_Vb_E=(d10_min1*((((2*1.03594e-10)*Vbc_Vb_E)/1.602176462e-19)/Nscl)*d10_sqrt0);
Wbcj_Vvar_Nsat_GND=(d10_min1*(-(((2*1.03594e-10)*(Vbc+0.6))/1.602176462e-19)*Nscl_Vvar_Nsat_GND/Nscl/Nscl)*d10_sqrt0);
#endif
Wbcj=d00_min1;
}
}
else
{
#if defined(_DERIVATE)
Wbcj_Vb_E=0.0;
Wbcj_Vvar_Nsat_GND=0.0;
#endif
Wbcj=0;
}
#if defined(_DERIVATE)
W_Vb_E=(-Wbcj_Vb_E);
W_Vvar_Nsat_GND=(-Wbcj_Vvar_Nsat_GND);
#endif
W=(Wl-Wbcj);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs_VG_E=(Cgs*Vgs_VG_E);
#endif
Qgs=(Cgs*Vgs);
#endif
#ifdef _DYNAMIC
if
((Vds>(-0.6)))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*(Vds+0.6))*1.602176462e-19)*Nscl))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((((2*1.03594e-10)*(Vds+0.6))*1.602176462e-19)*Nscl))
#endif
#if defined(_DERIVATE)
Qds_Vb_E=(Ads*((2*1.03594e-10)*Vds_Vb_E)*1.602176462e-19*Nscl*d10_sqrt0);
Qds_Vvar_Nsat_GND=(Ads*((((2*1.03594e-10)*(Vds+0.6))*1.602176462e-19)*Nscl_Vvar_Nsat_GND)*d10_sqrt0);
#endif
Qds=(Ads*d00_sqrt0);
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qds_Vb_E=0.0;
Qds_Vvar_Nsat_GND=0.0;
#endif
Qds=0;
#endif
}
#endif /* if (...) */
#if defined(_DERIVATE)
Qb_Vb_E=((1.602176462e-19*A)*W_Vb_E)*Nscl;
Qb_Vvar_Nsat_GND=((((1.602176462e-19*A)*W_Vvar_Nsat_GND)*Nscl)+(((1.602176462e-19*A)*W)*Nscl_Vvar_Nsat_GND));
#endif
Qb=(((1.602176462e-19*A)*W)*Nscl);
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*1.602176462e-19)*Nl)*0.6))
Qbi=(A*d00_sqrt0);
}
#endif
#if defined(_DERIVATE)
Cbcj_Vb_E=(-(A*1.03594e-10)*Wbcj_Vb_E/(Wbcj+1e-20)/(Wbcj+1e-20));
Cbcj_Vvar_Nsat_GND=(-(A*1.03594e-10)*Wbcj_Vvar_Nsat_GND/(Wbcj+1e-20)/(Wbcj+1e-20));
#endif
Cbcj=((A*1.03594e-10)/(Wbcj+1e-20));
#if defined(_DERIVATE)
Ccer_Vvar_Ql_GND=((Ql_Vvar_Ql_GND*Cbcj/3)/(Qb+1e-20));
Ccer_Vb_E=((((Ql*Cbcj_Vb_E)/3)*(Qb+1e-20)-((Ql*Cbcj)/3)*Qb_Vb_E)/(Qb+1e-20)/(Qb+1e-20));
Ccer_Vvar_Nsat_GND=((((Ql*Cbcj_Vvar_Nsat_GND)/3)*(Qb+1e-20)-((Ql*Cbcj)/3)*Qb_Vvar_Nsat_GND)/(Qb+1e-20)/(Qb+1e-20));
#endif
Ccer=(((Ql*Cbcj)/3)/(Qb+1e-20));
#if defined(_DYNAMIC)
Cdsj=((Ads*1.03594e-10)/(Wdsj+1e-20));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Cgdj_Vb_G=(-(Agd*1.03594e-10)*Wgdj_Vb_G/(Wgdj+1e-20)/(Wgdj+1e-20));
Cgdj_Vvar_Nsat_GND=(-(Agd*1.03594e-10)*Wgdj_Vvar_Nsat_GND/(Wgdj+1e-20)/(Wgdj+1e-20));
#endif
Cgdj=((Agd*1.03594e-10)/(Wgdj+1e-20));
#endif
#ifdef _DYNAMIC
if
((Vds<(Vgs-Vtd)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Cgd_Vb_G=0.0;
Cgd_Vvar_Nsat_GND=0.0;
#endif
Cgd=Coxd;
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Cgd_Vb_G=(((Coxd*Cgdj_Vb_G)*(Coxd+Cgdj)-(Coxd*Cgdj)*Cgdj_Vb_G)/(Coxd+Cgdj)/(Coxd+Cgdj));
Cgd_Vvar_Nsat_GND=(((Coxd*Cgdj_Vvar_Nsat_GND)*(Coxd+Cgdj)-(Coxd*Cgdj)*Cgdj_Vvar_Nsat_GND)/(Coxd+Cgdj)/(Coxd+Cgdj));
#endif
Cgd=((Coxd*Cgdj)/(Coxd+Cgdj));
#endif
}
#endif /* if (...) */
#if defined(_DERIVATE)
Munc_Vvar_Mucinv_GND=(-1*Mucinv_Vvar_Mucinv_GND/((1/Mun)+Mucinv)/((1/Mun)+Mucinv));
#endif
Munc=(1/((1/Mun)+Mucinv));
#if defined(_DERIVATE)
Mupc_Vvar_Mucinv_GND=(-1*Mucinv_Vvar_Mucinv_GND/((1/Mup)+Mucinv)/((1/Mup)+Mucinv));
#endif
Mupc=(1/((1/Mup)+Mucinv));
if
((Ql>0))
{
#if defined(_DERIVATE)
Mueff_Vvar_Mucinv_GND=(Munc_Vvar_Mucinv_GND+(Mupc_Vvar_Mucinv_GND*Ql/(Ql+Qb)));
Mueff_Vvar_Ql_GND=(((Mupc*Ql_Vvar_Ql_GND)*(Ql+Qb)-(Mupc*Ql)*Ql_Vvar_Ql_GND)/(Ql+Qb)/(Ql+Qb));
Mueff_Vb_E=(-(Mupc*Ql)*Qb_Vb_E/(Ql+Qb)/(Ql+Qb));
Mueff_Vvar_Nsat_GND=(-(Mupc*Ql)*Qb_Vvar_Nsat_GND/(Ql+Qb)/(Ql+Qb));
#endif
Mueff=(Munc+((Mupc*Ql)/(Ql+Qb)));
}
else
{
#if defined(_DERIVATE)
Mueff_Vvar_Mucinv_GND=Munc_Vvar_Mucinv_GND;
Mueff_Vvar_Ql_GND=0.0;
Mueff_Vb_E=0.0;
Mueff_Vvar_Nsat_GND=0.0;
#endif
Mueff=Munc;
}
#if defined(_DERIVATE)
Dc_Vvar_Mucinv_GND=(((((((2*0.0*m10_vt(Tj))*Munc)+((2*m00_vt(Tj))*Munc_Vvar_Mucinv_GND))*Mupc)+(((2*m00_vt(Tj))*Munc)*Mupc_Vvar_Mucinv_GND))*(Munc+Mupc)-(((2*m00_vt(Tj))*Munc)*Mupc)*(Munc_Vvar_Mucinv_GND+Mupc_Vvar_Mucinv_GND))/(Munc+Mupc)/(Munc+Mupc));
#endif
Dc=((((2*m00_vt(Tj))*Munc)*Mupc)/(Munc+Mupc));
{
double m00_sqrt(d00_sqrt0,(Dc*Tau))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(Dc*Tau))
#endif
#if defined(_DERIVATE)
L_Vvar_Mucinv_GND=Dc_Vvar_Mucinv_GND*Tau*d10_sqrt0;
#endif
L=d00_sqrt0;
}
#if defined(_DERIVATE)
WL_Vb_E=(W_Vb_E/L);
WL_Vvar_Nsat_GND=(W_Vvar_Nsat_GND/L);
WL_Vvar_Mucinv_GND=(-W*L_Vvar_Mucinv_GND/L/L);
#endif
WL=(W/L);
#if defined(_DERIVATE)
WL2_Vb_E=(WL_Vb_E/2);
WL2_Vvar_Nsat_GND=(WL_Vvar_Nsat_GND/2);
WL2_Vvar_Mucinv_GND=(WL_Vvar_Mucinv_GND/2);
#endif
WL2=(WL/2);
{
double m00_tanh(d00_tanh0,WL2)
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,WL2)
#endif
#if defined(_DERIVATE)
Tanh_Vb_E=WL2_Vb_E*d10_tanh0;
Tanh_Vvar_Nsat_GND=WL2_Vvar_Nsat_GND*d10_tanh0;
Tanh_Vvar_Mucinv_GND=WL2_Vvar_Mucinv_GND*d10_tanh0;
#endif
Tanh=d00_tanh0;
}
if
((WL>0))
{
#if defined(_DERIVATE)
Plo_Vvar_Ql_GND=(Ql_Vvar_Ql_GND/(((1.602176462e-19*A)*L)*Tanh));
Plo_Vvar_Mucinv_GND=(-Ql*((((1.602176462e-19*A)*L_Vvar_Mucinv_GND)*Tanh)+(((1.602176462e-19*A)*L)*Tanh_Vvar_Mucinv_GND))/(((1.602176462e-19*A)*L)*Tanh)/(((1.602176462e-19*A)*L)*Tanh));
Plo_Vb_E=(-Ql*(((1.602176462e-19*A)*L)*Tanh_Vb_E)/(((1.602176462e-19*A)*L)*Tanh)/(((1.602176462e-19*A)*L)*Tanh));
Plo_Vvar_Nsat_GND=(-Ql*(((1.602176462e-19*A)*L)*Tanh_Vvar_Nsat_GND)/(((1.602176462e-19*A)*L)*Tanh)/(((1.602176462e-19*A)*L)*Tanh));
#endif
Plo=(Ql/(((1.602176462e-19*A)*L)*Tanh));
}
else
{
#if defined(_DERIVATE)
Plo_Vvar_Ql_GND=0.0;
Plo_Vvar_Mucinv_GND=0.0;
Plo_Vb_E=0.0;
Plo_Vvar_Nsat_GND=0.0;
#endif
Plo=0;
}
if
((WL>0))
{
{
double m00_sinh(d00_sinh0,WL2)
double m00_sinh(d00_sinh1,WL)
#if defined(_DERIVATE)
double m10_sinh(d10_sinh0,d00_sinh0,WL2)
double m10_sinh(d10_sinh1,d00_sinh1,WL)
#endif
#if defined(_DERIVATE)
Deltap_Vvar_Ql_GND=(Plo_Vvar_Ql_GND*d00_sinh0/d00_sinh1);
Deltap_Vvar_Mucinv_GND=((((Plo_Vvar_Mucinv_GND*d00_sinh0)+(Plo*WL2_Vvar_Mucinv_GND*d10_sinh0))*d00_sinh1-(Plo*d00_sinh0)*WL_Vvar_Mucinv_GND*d10_sinh1)/d00_sinh1/d00_sinh1);
Deltap_Vb_E=((((Plo_Vb_E*d00_sinh0)+(Plo*WL2_Vb_E*d10_sinh0))*d00_sinh1-(Plo*d00_sinh0)*WL_Vb_E*d10_sinh1)/d00_sinh1/d00_sinh1);
Deltap_Vvar_Nsat_GND=((((Plo_Vvar_Nsat_GND*d00_sinh0)+(Plo*WL2_Vvar_Nsat_GND*d10_sinh0))*d00_sinh1-(Plo*d00_sinh0)*WL_Vvar_Nsat_GND*d10_sinh1)/d00_sinh1/d00_sinh1);
#endif
Deltap=((Plo*d00_sinh0)/d00_sinh1);
}
}
else
{
#if defined(_DERIVATE)
Deltap_Vvar_Ql_GND=0.0;
Deltap_Vvar_Mucinv_GND=0.0;
Deltap_Vb_E=0.0;
Deltap_Vvar_Nsat_GND=0.0;
#endif
Deltap=0;
}
if
((WL>0))
{
double Nl2;
double Plo2;
double Con2;
double Con3;
double Con4;
#if defined(_DERIVATE)
double Plo2_Vvar_Ql_GND;
#endif
#if defined(_DERIVATE)
double Plo2_Vvar_Mucinv_GND;
#endif
#if defined(_DERIVATE)
double Plo2_Vb_E;
#endif
#if defined(_DERIVATE)
double Plo2_Vvar_Nsat_GND;
#endif
#if defined(_DERIVATE)
double Con2_Vb_E;
#endif
#if defined(_DERIVATE)
double Con2_Vvar_Nsat_GND;
#endif
#if defined(_DERIVATE)
double Con2_Vvar_Mucinv_GND;
#endif
#if defined(_DERIVATE)
double Con3_Vvar_Ql_GND;
#endif
#if defined(_DERIVATE)
double Con3_Vvar_Mucinv_GND;
#endif
#if defined(_DERIVATE)
double Con3_Vb_E;
#endif
#if defined(_DERIVATE)
double Con3_Vvar_Nsat_GND;
#endif
#if defined(_DERIVATE)
double Con4_Vvar_Ql_GND;
#endif
#if defined(_DERIVATE)
double Con4_Vvar_Mucinv_GND;
#endif
#if defined(_DERIVATE)
double Con4_Vb_E;
#endif
#if defined(_DERIVATE)
double Con4_Vvar_Nsat_GND;
#endif
Nl2=(Nl*Nl);
#if defined(_DERIVATE)
Plo2_Vvar_Ql_GND=((Plo_Vvar_Ql_GND*Plo)+(Plo*Plo_Vvar_Ql_GND));
Plo2_Vvar_Mucinv_GND=((Plo_Vvar_Mucinv_GND*Plo)+(Plo*Plo_Vvar_Mucinv_GND));
Plo2_Vb_E=((Plo_Vb_E*Plo)+(Plo*Plo_Vb_E));
Plo2_Vvar_Nsat_GND=((Plo_Vvar_Nsat_GND*Plo)+(Plo*Plo_Vvar_Nsat_GND));
#endif
Plo2=(Plo*Plo);
{
double m00_sinh(d00_sinh0,WL)
#if defined(_DERIVATE)
double m10_sinh(d10_sinh0,d00_sinh0,WL)
#endif
#if defined(_DERIVATE)
Con2_Vb_E=(-1*WL_Vb_E*d10_sinh0/d00_sinh0/d00_sinh0);
Con2_Vvar_Nsat_GND=(-1*WL_Vvar_Nsat_GND*d10_sinh0/d00_sinh0/d00_sinh0);
Con2_Vvar_Mucinv_GND=(-1*WL_Vvar_Mucinv_GND*d10_sinh0/d00_sinh0/d00_sinh0);
#endif
Con2=(1/d00_sinh0);
}
#if defined(_DERIVATE)
Con3_Vvar_Ql_GND=Plo2_Vvar_Ql_GND*Con2*Con2;
Con3_Vvar_Mucinv_GND=((((Plo2_Vvar_Mucinv_GND*Con2)+(Plo2*Con2_Vvar_Mucinv_GND))*Con2)+((Plo2*Con2)*Con2_Vvar_Mucinv_GND));
Con3_Vb_E=((((Plo2_Vb_E*Con2)+(Plo2*Con2_Vb_E))*Con2)+((Plo2*Con2)*Con2_Vb_E));
Con3_Vvar_Nsat_GND=((((Plo2_Vvar_Nsat_GND*Con2)+(Plo2*Con2_Vvar_Nsat_GND))*Con2)+((Plo2*Con2)*Con2_Vvar_Nsat_GND));
#endif
Con3=((Plo2*Con2)*Con2);
{
double m00_sqrt(d00_sqrt0,(Nl2+Con3))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(Nl2+Con3))
#endif
#if defined(_DERIVATE)
Con4_Vvar_Ql_GND=((Con3_Vvar_Ql_GND*d10_sqrt0*Tanh*(Nl+((Plo*Con2)*Tanh))-(d00_sqrt0*Tanh)*Plo_Vvar_Ql_GND*Con2*Tanh)/(Nl+((Plo*Con2)*Tanh))/(Nl+((Plo*Con2)*Tanh)));
Con4_Vvar_Mucinv_GND=((((Con3_Vvar_Mucinv_GND*d10_sqrt0*Tanh)+(d00_sqrt0*Tanh_Vvar_Mucinv_GND))*(Nl+((Plo*Con2)*Tanh))-(d00_sqrt0*Tanh)*((((Plo_Vvar_Mucinv_GND*Con2)+(Plo*Con2_Vvar_Mucinv_GND))*Tanh)+((Plo*Con2)*Tanh_Vvar_Mucinv_GND)))/(Nl+((Plo*Con2)*Tanh))/(Nl+((Plo*Con2)*Tanh)));
Con4_Vb_E=((((Con3_Vb_E*d10_sqrt0*Tanh)+(d00_sqrt0*Tanh_Vb_E))*(Nl+((Plo*Con2)*Tanh))-(d00_sqrt0*Tanh)*((((Plo_Vb_E*Con2)+(Plo*Con2_Vb_E))*Tanh)+((Plo*Con2)*Tanh_Vb_E)))/(Nl+((Plo*Con2)*Tanh))/(Nl+((Plo*Con2)*Tanh)));
Con4_Vvar_Nsat_GND=((((Con3_Vvar_Nsat_GND*d10_sqrt0*Tanh)+(d00_sqrt0*Tanh_Vvar_Nsat_GND))*(Nl+((Plo*Con2)*Tanh))-(d00_sqrt0*Tanh)*((((Plo_Vvar_Nsat_GND*Con2)+(Plo*Con2_Vvar_Nsat_GND))*Tanh)+((Plo*Con2)*Tanh_Vvar_Nsat_GND)))/(Nl+((Plo*Con2)*Tanh))/(Nl+((Plo*Con2)*Tanh)));
#endif
Con4=((d00_sqrt0*Tanh)/(Nl+((Plo*Con2)*Tanh)));
}
{
double m00_sqrt(d00_sqrt0,(Nl2+Con3))
double m00_atanh(d00_atanh1,Con4)
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(Nl2+Con3))
double m10_atanh(d10_atanh1,d00_atanh1,Con4)
#endif
#if defined(_DERIVATE)
Neff_Vb_E=((((WL2_Vb_E*d00_sqrt0)+(WL2*Con3_Vb_E*d10_sqrt0))*d00_atanh1-(WL2*d00_sqrt0)*Con4_Vb_E*d10_atanh1)/d00_atanh1/d00_atanh1);
Neff_Vvar_Nsat_GND=((((WL2_Vvar_Nsat_GND*d00_sqrt0)+(WL2*Con3_Vvar_Nsat_GND*d10_sqrt0))*d00_atanh1-(WL2*d00_sqrt0)*Con4_Vvar_Nsat_GND*d10_atanh1)/d00_atanh1/d00_atanh1);
Neff_Vvar_Mucinv_GND=((((WL2_Vvar_Mucinv_GND*d00_sqrt0)+(WL2*Con3_Vvar_Mucinv_GND*d10_sqrt0))*d00_atanh1-(WL2*d00_sqrt0)*Con4_Vvar_Mucinv_GND*d10_atanh1)/d00_atanh1/d00_atanh1);
Neff_Vvar_Ql_GND=(((WL2*Con3_Vvar_Ql_GND*d10_sqrt0)*d00_atanh1-(WL2*d00_sqrt0)*Con4_Vvar_Ql_GND*d10_atanh1)/d00_atanh1/d00_atanh1);
#endif
Neff=((WL2*d00_sqrt0)/d00_atanh1);
}
}
else
{
#if defined(_DERIVATE)
Neff_Vb_E=0.0;
Neff_Vvar_Nsat_GND=0.0;
Neff_Vvar_Mucinv_GND=0.0;
Neff_Vvar_Ql_GND=0.0;
#endif
Neff=0;
}
if
((Ql<0))
{
#if defined(_DERIVATE)
Rb_Vvar_Ql_GND=0.0;
Rb_Vb_E=(W_Vb_E/(((1.602176462e-19*Munc)*A)*Nl));
Rb_Vvar_Nsat_GND=(W_Vvar_Nsat_GND/(((1.602176462e-19*Munc)*A)*Nl));
Rb_Vvar_Mucinv_GND=(-W*(1.602176462e-19*Munc_Vvar_Mucinv_GND)*A*Nl/(((1.602176462e-19*Munc)*A)*Nl)/(((1.602176462e-19*Munc)*A)*Nl));
#endif
Rb=(W/(((1.602176462e-19*Munc)*A)*Nl));
}
else
{
#if defined(_DERIVATE)
Rb_Vvar_Ql_GND=(-W*(((1.602176462e-19*Mueff_Vvar_Ql_GND)*A*Neff)+(((1.602176462e-19*Mueff)*A)*Neff_Vvar_Ql_GND))/(((1.602176462e-19*Mueff)*A)*Neff)/(((1.602176462e-19*Mueff)*A)*Neff));
Rb_Vb_E=((W_Vb_E*(((1.602176462e-19*Mueff)*A)*Neff)-W*(((1.602176462e-19*Mueff_Vb_E)*A*Neff)+(((1.602176462e-19*Mueff)*A)*Neff_Vb_E)))/(((1.602176462e-19*Mueff)*A)*Neff)/(((1.602176462e-19*Mueff)*A)*Neff));
Rb_Vvar_Nsat_GND=((W_Vvar_Nsat_GND*(((1.602176462e-19*Mueff)*A)*Neff)-W*(((1.602176462e-19*Mueff_Vvar_Nsat_GND)*A*Neff)+(((1.602176462e-19*Mueff)*A)*Neff_Vvar_Nsat_GND)))/(((1.602176462e-19*Mueff)*A)*Neff)/(((1.602176462e-19*Mueff)*A)*Neff));
Rb_Vvar_Mucinv_GND=(-W*(((1.602176462e-19*Mueff_Vvar_Mucinv_GND)*A*Neff)+(((1.602176462e-19*Mueff)*A)*Neff_Vvar_Mucinv_GND))/(((1.602176462e-19*Mueff)*A)*Neff)/(((1.602176462e-19*Mueff)*A)*Neff));
#endif
Rb=(W/(((1.602176462e-19*Mueff)*A)*Neff));
}
#if defined(_DYNAMIC)
Vebj=(0.6-(((Ql-Qbi)*(Ql-Qbi))/(((((2*1.602176462e-19)*Nl)*1.03594e-10)*A)*A)));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,((((Plo/Ni)/Ni)+(1/Nl))*(Nl+Plo)))
double m00_logE(d00_logE1,((Plo+Nl)/Nl))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,((((Plo/Ni)/Ni)+(1/Nl))*(Nl+Plo)))
double m10_logE(d10_logE1,d00_logE1,((Plo+Nl)/Nl))
#endif
Vebd=((m00_vt(Tj)*d00_logE0)-((Dc/Munc)*d00_logE1));
}
#endif
#ifdef _DYNAMIC
if
((Ql<0))
{
#if defined(_DYNAMIC)
Vebq=Vebj;
#endif
}
else
{
#ifdef _DYNAMIC
if
((Ql>=Qbi))
{
#if defined(_DYNAMIC)
Vebq=Vebd;
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_min(d00_min0,Vebj,Vebd)
#if defined(_DERIVATE)
double m10_min(d10_min0,d00_min0,Vebj,Vebd)
double m11_min(d11_min0,d00_min0,Vebj,Vebd)
#endif
Vebq=d00_min0;
}
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
{
double m00_pow(d00_pow0,Nscl,0.75)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Nscl,0.75)
#endif
#if defined(_DERIVATE)
BVcbo_Vvar_Nsat_GND=(-(BVf*5.34e13)*(d10_pow0*Nscl_Vvar_Nsat_GND)/d00_pow0/d00_pow0);
#endif
BVcbo=((BVf*5.34e13)/d00_pow0);
}
#if defined(_DERIVATE)
Vnrt_Vb_E=Vbc_Vb_E;
#endif
Vnrt=Vbc;
{
double m00_pow(d00_pow0,(Vnrt/BVcbo),BVn)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(Vnrt/BVcbo),BVn)
#endif
#if defined(_DERIVATE)
M_Vb_E=(-1*(-(d10_pow0*(Vnrt_Vb_E/BVcbo)))/(1-d00_pow0)/(1-d00_pow0));
M_Vvar_Nsat_GND=(-1*(-(d10_pow0*(-Vnrt*BVcbo_Vvar_Nsat_GND/BVcbo/BVcbo)))/(1-d00_pow0)/(1-d00_pow0));
#endif
M=(1/(1-d00_pow0));
}
if
((Rb>0))
{
#if defined(_DERIVATE)
It_VC_e=(Vae_VC_e/Rb);
It_Vvar_Ql_GND=(-Vae*Rb_Vvar_Ql_GND/Rb/Rb);
It_Vb_E=(-Vae*Rb_Vb_E/Rb/Rb);
It_Vvar_Nsat_GND=(-Vae*Rb_Vvar_Nsat_GND/Rb/Rb);
It_Vvar_Mucinv_GND=(-Vae*Rb_Vvar_Mucinv_GND/Rb/Rb);
#endif
It=(Vae/Rb);
}
else
{
#if defined(_DERIVATE)
It_VC_e=(Vae_VC_e/1e-6);
It_Vvar_Ql_GND=0.0;
It_Vb_E=0.0;
It_Vvar_Nsat_GND=0.0;
It_Vvar_Mucinv_GND=0.0;
#endif
It=(Vae/1e-6);
}
if
((Veb>0))
{
#if defined(_DERIVATE)
Icss_VC_e=((1/(1+B))*It_VC_e);
Icss_Vvar_Ql_GND=(((1/(1+B))*It_Vvar_Ql_GND)+(((B/(1+B))*((4*Dp)/((W*W)+1e-20)))*Ql_Vvar_Ql_GND));
Icss_Vb_E=(((1/(1+B))*It_Vb_E)+((B/(1+B))*(-(4*Dp)*((W_Vb_E*W)+(W*W_Vb_E))/((W*W)+1e-20)/((W*W)+1e-20)))*Ql);
Icss_Vvar_Nsat_GND=(((1/(1+B))*It_Vvar_Nsat_GND)+((B/(1+B))*(-(4*Dp)*((W_Vvar_Nsat_GND*W)+(W*W_Vvar_Nsat_GND))/((W*W)+1e-20)/((W*W)+1e-20)))*Ql);
Icss_Vvar_Mucinv_GND=((1/(1+B))*It_Vvar_Mucinv_GND);
#endif
Icss=(((1/(1+B))*It)+(((B/(1+B))*((4*Dp)/((W*W)+1e-20)))*Ql));
}
else
{
#if defined(_DERIVATE)
Icss_VC_e=0.0;
Icss_Vvar_Ql_GND=0.0;
Icss_Vb_E=0.0;
Icss_Vvar_Nsat_GND=0.0;
Icss_Vvar_Mucinv_GND=0.0;
#endif
Icss=0;
}
#if defined(_DERIVATE)
Ic_VC_e=Icss_VC_e;
Ic_Vvar_Ql_GND=(Icss_Vvar_Ql_GND+Ccer_Vvar_Ql_GND*dVecdt);
Ic_Vb_E=(Icss_Vb_E+Ccer_Vb_E*dVecdt);
Ic_Vvar_Nsat_GND=(Icss_Vvar_Nsat_GND+Ccer_Vvar_Nsat_GND*dVecdt);
Ic_Vvar_Mucinv_GND=Icss_Vvar_Mucinv_GND;
Ic_Vvar_dVecdt_GND=(Ccer*dVecdt_Vvar_dVecdt_GND);
#endif
Ic=(Icss+(Ccer*dVecdt));
if
((Veb>0))
{
if
((Qb>0))
{
#if defined(_DERIVATE)
Ibss_Vvar_Ql_GND=((Ql_Vvar_Ql_GND/Tau)+((((((Ql_Vvar_Ql_GND*Ql)+(Ql*Ql_Vvar_Ql_GND))/Qb)/Qb)*4*Nscl*Nscl/Ni)/Ni)*Isne);
Ibss_Vb_E=(((((-(Ql*Ql)*Qb_Vb_E/Qb/Qb)*Qb-((Ql*Ql)/Qb)*Qb_Vb_E)/Qb/Qb)*4*Nscl*Nscl/Ni)/Ni)*Isne;
Ibss_Vvar_Nsat_GND=(((((((((-(Ql*Ql)*Qb_Vvar_Nsat_GND/Qb/Qb)*Qb-((Ql*Ql)/Qb)*Qb_Vvar_Nsat_GND)/Qb/Qb)*4*Nscl)+(((((Ql*Ql)/Qb)/Qb)*4)*Nscl_Vvar_Nsat_GND))*Nscl)+((((((Ql*Ql)/Qb)/Qb)*4)*Nscl)*Nscl_Vvar_Nsat_GND))/Ni)/Ni)*Isne;
#endif
Ibss=((Ql/Tau)+(((((((((Ql*Ql)/Qb)/Qb)*4)*Nscl)*Nscl)/Ni)/Ni)*Isne));
}
else
{
#if defined(_DERIVATE)
Ibss_Vvar_Ql_GND=(Ql_Vvar_Ql_GND/Tau);
Ibss_Vb_E=0.0;
Ibss_Vvar_Nsat_GND=0.0;
#endif
Ibss=(Ql/Tau);
}
}
else
{
#if defined(_DERIVATE)
Ibss_Vvar_Ql_GND=0.0;
Ibss_Vb_E=0.0;
Ibss_Vvar_Nsat_GND=0.0;
#endif
Ibss=0;
}
if
((Vgs<Vt))
{
#if defined(_DERIVATE)
Imos_Vb_E=0.0;
Imos_VG_E=0.0;
#endif
Imos=0;
}
else
{
if
((Vds>(Vth/Kf)))
{
#if defined(_DERIVATE)
Imos_Vb_E=0.0;
Imos_VG_E=((((((KpT*Vth_VG_E)*Vth)+((KpT*Vth)*Vth_VG_E))/2)*(1+(Theta*Vth))-(((KpT*Vth)*Vth)/2)*(Theta*Vth_VG_E))/(1+(Theta*Vth))/(1+(Theta*Vth)));
#endif
Imos=((((KpT*Vth)*Vth)/2)/(1+(Theta*Vth)));
}
else
{
#if defined(_DERIVATE)
Imos_Vb_E=(((Kf*KpT)*((Vth*Vds_Vb_E)-((((Kf*Vds_Vb_E)*Vds)+((Kf*Vds)*Vds_Vb_E))/2)))/(1+(Theta*Vth)));
Imos_VG_E=((((Kf*KpT)*Vth_VG_E*Vds)*(1+(Theta*Vth))-((Kf*KpT)*((Vth*Vds)-(((Kf*Vds)*Vds)/2)))*(Theta*Vth_VG_E))/(1+(Theta*Vth))/(1+(Theta*Vth)));
#endif
Imos=(((Kf*KpT)*((Vth*Vds)-(((Kf*Vds)*Vds)/2)))/(1+(Theta*Vth)));
}
}
if
((Vbc>0))
{
{
double m00_sqrt(d00_sqrt0,((((2*1.03594e-10)*Vbc)/1.602176462e-19)/Nscl))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((((2*1.03594e-10)*Vbc)/1.602176462e-19)/Nscl))
#endif
#if defined(_DERIVATE)
Igen_Vb_E=((((1.602176462e-19*Ni)*A)*((((2*1.03594e-10)*Vbc_Vb_E)/1.602176462e-19)/Nscl)*d10_sqrt0)/Tau);
Igen_Vvar_Nsat_GND=((((1.602176462e-19*Ni)*A)*(-(((2*1.03594e-10)*Vbc)/1.602176462e-19)*Nscl_Vvar_Nsat_GND/Nscl/Nscl)*d10_sqrt0)/Tau);
#endif
Igen=((((1.602176462e-19*Ni)*A)*d00_sqrt0)/Tau);
}
}
else
{
#if defined(_DERIVATE)
Igen_Vb_E=0.0;
Igen_Vvar_Nsat_GND=0.0;
#endif
Igen=0;
}
#if defined(_DERIVATE)
Imult_Vb_E=(((M_Vb_E*(Imos+Ic))+((M-1)*(Imos_Vb_E+Ic_Vb_E)))+((M_Vb_E*Igen)+(M*Igen_Vb_E)));
Imult_Vvar_Nsat_GND=(((M_Vvar_Nsat_GND*(Imos+Ic))+((M-1)*Ic_Vvar_Nsat_GND))+((M_Vvar_Nsat_GND*Igen)+(M*Igen_Vvar_Nsat_GND)));
Imult_VG_E=((M-1)*Imos_VG_E);
Imult_VC_e=((M-1)*Ic_VC_e);
Imult_Vvar_Ql_GND=((M-1)*Ic_Vvar_Ql_GND);
Imult_Vvar_Mucinv_GND=((M-1)*Ic_Vvar_Mucinv_GND);
Imult_Vvar_dVecdt_GND=((M-1)*Ic_Vvar_dVecdt_GND);
#endif
Imult=(((M-1)*(Imos+Ic))+(M*Igen));
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd_Vb_G=((Cgd_Vb_G*Vdg)+(Cgd*Vdg_Vb_G));
Qgd_Vvar_Nsat_GND=Cgd_Vvar_Nsat_GND*Vdg;
#endif
Qgd=(Cgd*Vdg);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qcer_Vvar_Ql_GND=Ccer_Vvar_Ql_GND*Vec;
Qcer_Vb_E=Ccer_Vb_E*Vec;
Qcer_Vvar_Nsat_GND=Ccer_Vvar_Nsat_GND*Vec;
Qcer_Ve_E=(Ccer*Vec_Ve_E);
#endif
Qcer=(Ccer*Vec);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qmult_Vb_E=((M_Vb_E*Qcer)+((M-1)*Qcer_Vb_E));
Qmult_Vvar_Nsat_GND=((M_Vvar_Nsat_GND*Qcer)+((M-1)*Qcer_Vvar_Nsat_GND));
Qmult_Vvar_Ql_GND=((M-1)*Qcer_Vvar_Ql_GND);
Qmult_Ve_E=((M-1)*Qcer_Ve_E);
#endif
Qmult=((M-1)*Qcer);
#endif
if
((Deltap>0))
{
{
double m00_pow(d00_pow0,Deltap,(2.0/3.0))
double m00_logE(d00_logE1,(1+(Alpha2/d00_pow0)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Deltap,(2.0/3.0))
double m10_logE(d10_logE1,d00_logE1,(1+(Alpha2/d00_pow0)))
#endif
#if defined(_DERIVATE)
iMucinv_Vvar_Ql_GND=(((Deltap_Vvar_Ql_GND*d00_logE1)+(Deltap*(-Alpha2*(d10_pow0*Deltap_Vvar_Ql_GND)/d00_pow0/d00_pow0)*d10_logE1))/Alpha1);
iMucinv_Vvar_Mucinv_GND=(((Deltap_Vvar_Mucinv_GND*d00_logE1)+(Deltap*(-Alpha2*(d10_pow0*Deltap_Vvar_Mucinv_GND)/d00_pow0/d00_pow0)*d10_logE1))/Alpha1);
iMucinv_Vb_E=(((Deltap_Vb_E*d00_logE1)+(Deltap*(-Alpha2*(d10_pow0*Deltap_Vb_E)/d00_pow0/d00_pow0)*d10_logE1))/Alpha1);
iMucinv_Vvar_Nsat_GND=(((Deltap_Vvar_Nsat_GND*d00_logE1)+(Deltap*(-Alpha2*(d10_pow0*Deltap_Vvar_Nsat_GND)/d00_pow0/d00_pow0)*d10_logE1))/Alpha1);
#endif
iMucinv=((Deltap*d00_logE1)/Alpha1);
}
}
else
{
#if defined(_DERIVATE)
iMucinv_Vvar_Ql_GND=0.0;
iMucinv_Vvar_Mucinv_GND=0.0;
iMucinv_Vb_E=0.0;
iMucinv_Vvar_Nsat_GND=0.0;
#endif
iMucinv=0;
}
#if defined(_DERIVATE)
iQl_Vb_E=(((((1.602176462e-19*A)*W_Vb_E)*Plo)+(((1.602176462e-19*A)*W)*Plo_Vb_E))/2);
iQl_Vvar_Nsat_GND=(((((1.602176462e-19*A)*W_Vvar_Nsat_GND)*Plo)+(((1.602176462e-19*A)*W)*Plo_Vvar_Nsat_GND))/2);
iQl_Vvar_Ql_GND=((((1.602176462e-19*A)*W)*Plo_Vvar_Ql_GND)/2);
iQl_Vvar_Mucinv_GND=((((1.602176462e-19*A)*W)*Plo_Vvar_Mucinv_GND)/2);
#endif
iQl=((((1.602176462e-19*A)*W)*Plo)/2);
#if defined(_DERIVATE)
iNsat_VC_e=(Ic_VC_e/((1.602176462e-19*A)*Vpsat));
iNsat_Vvar_Ql_GND=(Ic_Vvar_Ql_GND/((1.602176462e-19*A)*Vpsat));
iNsat_Vb_E=((Ic_Vb_E/((1.602176462e-19*A)*Vpsat))-(Imos_Vb_E/((1.602176462e-19*A)*Vnsat)));
iNsat_Vvar_Nsat_GND=(Ic_Vvar_Nsat_GND/((1.602176462e-19*A)*Vpsat));
iNsat_Vvar_Mucinv_GND=(Ic_Vvar_Mucinv_GND/((1.602176462e-19*A)*Vpsat));
iNsat_Vvar_dVecdt_GND=(Ic_Vvar_dVecdt_GND/((1.602176462e-19*A)*Vpsat));
iNsat_VG_E=(-(Imos_VG_E/((1.602176462e-19*A)*Vnsat)));
#endif
iNsat=((Ic/((1.602176462e-19*A)*Vpsat))-(Imos/((1.602176462e-19*A)*Vnsat)));
_load_static_residual2(G,E,(Vgs*1e-12));
#if defined(_DERIVATE)
_load_static_jacobian4(G,E,G,E,Vgs_VG_E*1e-12);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(G,E,_DDT(Qgs));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(G,E,G,E,Qgs_VG_E);
#endif
#endif
_load_static_residual2(b,G,(Vdg*1e-12));
#if defined(_DERIVATE)
_load_static_jacobian4(b,G,b,G,Vdg_Vb_G*1e-12);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,G,_DDT(Qgd));
#if defined(_DERIVATE)
_load_dynamic_jacobian2s(b,G,var_Nsat,Qgd_Vvar_Nsat_GND);
_load_dynamic_jacobian4(b,G,b,G,Qgd_Vb_G);
#endif
#endif
_load_static_residual2(b,E,((Imos+Imult)+(Vds*1e-12)));
#if defined(_DERIVATE)
_load_static_jacobian2s(b,E,var_dVecdt,Imult_Vvar_dVecdt_GND);
_load_static_jacobian2s(b,E,var_Mucinv,Imult_Vvar_Mucinv_GND);
_load_static_jacobian2s(b,E,var_Ql,Imult_Vvar_Ql_GND);
_load_static_jacobian4(b,E,C,e,Imult_VC_e);
_load_static_jacobian2s(b,E,var_Nsat,Imult_Vvar_Nsat_GND);
_load_static_jacobian4(b,E,G,E,(Imos_VG_E+Imult_VG_E));
_load_static_jacobian4(b,E,b,E,((Imos_Vb_E+Imult_Vb_E)+Vds_Vb_E*1e-12));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,E,_DDT(Qds));
#if defined(_DERIVATE)
_load_dynamic_jacobian2s(b,E,var_Nsat,Qds_Vvar_Nsat_GND);
_load_dynamic_jacobian4(b,E,b,E,Qds_Vb_E);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(b,E,_DDT(Qmult));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(b,E,e,E,Qmult_Ve_E);
_load_dynamic_jacobian2s(b,E,var_Ql,Qmult_Vvar_Ql_GND);
_load_dynamic_jacobian2s(b,E,var_Nsat,Qmult_Vvar_Nsat_GND);
_load_dynamic_jacobian4(b,E,b,E,Qmult_Vb_E);
#endif
#endif
_load_static_residual2(e,E,(Icss+(Vec*1e-12)));
#if defined(_DERIVATE)
_load_static_jacobian4(e,E,e,E,Vec_Ve_E*1e-12);
_load_static_jacobian2s(e,E,var_Mucinv,Icss_Vvar_Mucinv_GND);
_load_static_jacobian2s(e,E,var_Nsat,Icss_Vvar_Nsat_GND);
_load_static_jacobian4(e,E,b,E,Icss_Vb_E);
_load_static_jacobian2s(e,E,var_Ql,Icss_Vvar_Ql_GND);
_load_static_jacobian4(e,E,C,e,Icss_VC_e);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(e,E,_DDT(Qcer));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(e,E,e,E,Qcer_Ve_E);
_load_dynamic_jacobian2s(e,E,var_Nsat,Qcer_Vvar_Nsat_GND);
_load_dynamic_jacobian4(e,E,b,E,Qcer_Vb_E);
_load_dynamic_jacobian2s(e,E,var_Ql,Qcer_Vvar_Ql_GND);
#endif
#endif
_load_static_residual2(e,b,(Ibss+(Veb*1e-12)));
#if defined(_DERIVATE)
_load_static_jacobian4(e,b,e,b,Veb_Ve_b*1e-12);
_load_static_jacobian2s(e,b,var_Nsat,Ibss_Vvar_Nsat_GND);
_load_static_jacobian4(e,b,b,E,Ibss_Vb_E);
_load_static_jacobian2s(e,b,var_Ql,Ibss_Vvar_Ql_GND);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(e,b,_DDT(iQl));
#if defined(_DERIVATE)
_load_dynamic_jacobian2s(e,b,var_Mucinv,iQl_Vvar_Mucinv_GND);
_load_dynamic_jacobian2s(e,b,var_Ql,iQl_Vvar_Ql_GND);
_load_dynamic_jacobian2s(e,b,var_Nsat,iQl_Vvar_Nsat_GND);
_load_dynamic_jacobian4(e,b,b,E,iQl_Vb_E);
#endif
#endif
_load_static_residual2(C,e,It);
#if defined(_DERIVATE)
_load_static_jacobian2s(C,e,var_Mucinv,It_Vvar_Mucinv_GND);
_load_static_jacobian2s(C,e,var_Nsat,It_Vvar_Nsat_GND);
_load_static_jacobian4(C,e,b,E,It_Vb_E);
_load_static_jacobian2s(C,e,var_Ql,It_Vvar_Ql_GND);
_load_static_jacobian4(C,e,C,e,It_VC_e);
#endif
_load_static_residual1(var_Ql,(Ql-iQl));
#if defined(_DERIVATE)
_load_static_jacobian1(var_Ql,var_Mucinv,(-iQl_Vvar_Mucinv_GND));
_load_static_jacobian1(var_Ql,var_Nsat,(-iQl_Vvar_Nsat_GND));
_load_static_jacobian2p(var_Ql,b,E,(-iQl_Vb_E));
_load_static_jacobian1(var_Ql,var_Ql,(Ql_Vvar_Ql_GND-iQl_Vvar_Ql_GND));
#endif
_load_static_residual1(var_Mucinv,(Mucinv-iMucinv));
#if defined(_DERIVATE)
_load_static_jacobian1(var_Mucinv,var_Nsat,(-iMucinv_Vvar_Nsat_GND));
_load_static_jacobian2p(var_Mucinv,b,E,(-iMucinv_Vb_E));
_load_static_jacobian1(var_Mucinv,var_Ql,(-iMucinv_Vvar_Ql_GND));
_load_static_jacobian1(var_Mucinv,var_Mucinv,(Mucinv_Vvar_Mucinv_GND-iMucinv_Vvar_Mucinv_GND));
#endif
_load_static_residual1(var_Nsat,(Nsat-iNsat));
#if defined(_DERIVATE)
_load_static_jacobian2p(var_Nsat,G,E,(-iNsat_VG_E));
_load_static_jacobian1(var_Nsat,var_dVecdt,(-iNsat_Vvar_dVecdt_GND));
_load_static_jacobian1(var_Nsat,var_Mucinv,(-iNsat_Vvar_Mucinv_GND));
_load_static_jacobian2p(var_Nsat,b,E,(-iNsat_Vb_E));
_load_static_jacobian1(var_Nsat,var_Ql,(-iNsat_Vvar_Ql_GND));
_load_static_jacobian2p(var_Nsat,C,e,(-iNsat_VC_e));
_load_static_jacobian1(var_Nsat,var_Nsat,(Nsat_Vvar_Nsat_GND-iNsat_Vvar_Nsat_GND));
#endif
_load_static_residual1(var_dVecdt,dVecdt);
#if defined(_DERIVATE)
_load_static_jacobian1(var_dVecdt,var_dVecdt,dVecdt_Vvar_dVecdt_GND);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(var_dVecdt,(-_DDT(Vec)));
#if defined(_DERIVATE)
_load_dynamic_jacobian2p(var_dVecdt,e,E,(-Vec_Ve_E));
#endif
#endif

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void nigbt::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 9; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 9; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void nigbt::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void nigbt::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void nigbt::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void nigbt::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void nigbt::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix nigbt::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (9);

  for (int i1 = 0; i1 < 9; i1++) {
    for (int i2 = 0; i2 < 9; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void nigbt::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void nigbt::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void nigbt::initTR (void)
{
  setStates (2 * 9 * 9);
  initDC ();
}

/* Perform transient analysis iteration step. */
void nigbt::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 9; i1++) {
  for (i2 = 0; i2 < 9; i2++) {
    state = 2 * (i2 + 9 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 9; i1++) {
    state = 2 * (i1 + 9 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 9; i1++) {
  for (i2 = 0; i2 < 9; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 9; i3++) {
  for (i4 = 0; i4 < 9; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 9; i1++) {
  for (i2 = 0; i2 < 9; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 9; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 9; i1++) {
  for (i3 = 0; i3 < 9; i3++) {
  for (i4 = 0; i4 < 9; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 9; i1++) {
  for (i3 = 0; i3 < 9; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix nigbt::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (9);


  return cy;
}

/* Perform AC noise computations. */
void nigbt::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void nigbt::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void nigbt::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void nigbt::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 9; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 9; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "nigbt.defs.h"
