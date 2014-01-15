/*
 * MESFET.core.cpp - device implementations for MESFET module
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

#include "MESFET.analogfunction.h"
#include "component.h"
#include "device.h"
#include "MESFET.core.h"

#ifndef CIR_MESFET
#define CIR_MESFET -1
#endif

// external nodes
#define Drain 0
#define Gate 1
#define Source 2
// internal nodes
#define n1 3
#define n2 4
#define n3 5
#define n4 6

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
#define _modelname              "MESFET"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
MESFET::MESFET() : circuit (7)
{
  type = CIR_MESFET;
}

/* Initialization of model. */
void MESFET::initModel (void)
{
  // create internal nodes
  setInternalNode (n1, "n1");
  setInternalNode (n2, "n2");
  setInternalNode (n3, "n3");
  setInternalNode (n4, "n4");

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
void MESFET::initDC (void)
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
void MESFET::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void MESFET::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[n4][Source] = 0.0;
  _white_pwr[Drain][n3] = 0.0;
  _white_pwr[Gate][n2] = 0.0;
  _white_pwr[n3][n4] = 0.0;
  _flicker_pwr[n3][n4] = 0.0;
  _flicker_exp[n3][n4] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 7; i1++) {
  for (i2 = 0; i2 < 7; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 7; i1++) {
  for (i2 = 0; i2 < 7; i2++) {
  for (i3 = 0; i3 < 7; i3++) {
  for (i4 = 0; i4 < 7; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 7; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 7; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void MESFET::loadVariables (void)
{
  LEVEL = getPropertyInteger ("LEVEL");
  Vto = getPropertyDouble ("Vto");
  Beta = getPropertyDouble ("Beta");
  Alpha = getPropertyDouble ("Alpha");
  Lambda = getPropertyDouble ("Lambda");
  B = getPropertyDouble ("B");
  Qp = getPropertyDouble ("Qp");
  Delta = getPropertyDouble ("Delta");
  Vmax = getPropertyDouble ("Vmax");
  Vdelta1 = getPropertyDouble ("Vdelta1");
  Vdelta2 = getPropertyDouble ("Vdelta2");
  Gamma = getPropertyDouble ("Gamma");
  Nsc = getPropertyDouble ("Nsc");
  Is = getPropertyDouble ("Is");
  N = getPropertyDouble ("N");
  Vbi = getPropertyDouble ("Vbi");
  Bv = getPropertyDouble ("Bv");
  Xti = getPropertyDouble ("Xti");
  Fc = getPropertyDouble ("Fc");
  Tau = getPropertyDouble ("Tau");
  Rin = getPropertyDouble ("Rin");
  Area = getPropertyDouble ("Area");
  Eg = getPropertyDouble ("Eg");
  M = getPropertyDouble ("M");
  Cgd = getPropertyDouble ("Cgd");
  Cgs = getPropertyDouble ("Cgs");
  Cds = getPropertyDouble ("Cds");
  Betatc = getPropertyDouble ("Betatc");
  Alphatc = getPropertyDouble ("Alphatc");
  Gammatc = getPropertyDouble ("Gammatc");
  Ng = getPropertyDouble ("Ng");
  Nd = getPropertyDouble ("Nd");
  ILEVELS = getPropertyInteger ("ILEVELS");
  ILEVELD = getPropertyInteger ("ILEVELD");
  QLEVELS = getPropertyInteger ("QLEVELS");
  QLEVELD = getPropertyInteger ("QLEVELD");
  QLEVELDS = getPropertyInteger ("QLEVELDS");
  Vtotc = getPropertyDouble ("Vtotc");
  Rg = getPropertyDouble ("Rg");
  Rd = getPropertyDouble ("Rd");
  Rs = getPropertyDouble ("Rs");
  Rgtc = getPropertyDouble ("Rgtc");
  Rdtc = getPropertyDouble ("Rdtc");
  Rstc = getPropertyDouble ("Rstc");
  Ibv = getPropertyDouble ("Ibv");
  Rf = getPropertyDouble ("Rf");
  R1 = getPropertyDouble ("R1");
  Af = getPropertyDouble ("Af");
  Kf = getPropertyDouble ("Kf");
  Gdsnoi = getPropertyDouble ("Gdsnoi");
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
void MESFET::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void MESFET::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void MESFET::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void MESFET::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void MESFET::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double gm1;
#if defined(_DERIVATE)
double gm1_Vn2_n4;
double gm1_Vn3_n4;
#endif
double H1;
#if defined(_DERIVATE)
double H1_Vn3_n4;
double H1_Vn2_n4;
#endif
double flicker_pwr;
#if defined(_DERIVATE)
double flicker_pwr_Vn2_n4;
double flicker_pwr_Vn3_n4;
#endif
double thermal_pwr;
#if defined(_DERIVATE)
double thermal_pwr_Vn2_n4;
double thermal_pwr_Vn3_n4;
#endif
double An;
#if defined(_DERIVATE)
double An_Vn3_n4;
double An_Vn2_n4;
#endif
double gm;
#if defined(_DERIVATE)
double gm_Vn2_n4;
double gm_Vn3_n4;
#endif
double fourkt;
#if defined(_DYNAMIC)
double Qds;
#if defined(_DERIVATE)
double Qds_Vn2_n4;
double Qds_Vn3_n4;
#endif
#endif
#if defined(_DYNAMIC)
double Veff2;
#if defined(_DERIVATE)
double Veff2_Vn2_n1;
double Veff2_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Qgd2;
#if defined(_DERIVATE)
double Qgd2_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Qgd1;
#if defined(_DERIVATE)
double Qgd1_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Qgd;
#if defined(_DERIVATE)
double Qgd_Vn2_n1;
double Qgd_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Vnew;
#if defined(_DERIVATE)
double Vnew_Vn2_n1;
double Vnew_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Veff1;
#if defined(_DERIVATE)
double Veff1_Vn2_n1;
double Veff1_Vn2_n3;
#endif
#endif
#if defined(_DYNAMIC)
double Qgs2;
#if defined(_DERIVATE)
double Qgs2_Vn2_n1;
#endif
#endif
#if defined(_DYNAMIC)
double Qgs1;
#if defined(_DERIVATE)
double Qgs1_Vn2_n1;
#endif
#endif
#if defined(_DYNAMIC)
double Qgs;
#if defined(_DERIVATE)
double Qgs_Vn2_n3;
double Qgs_Vn2_n1;
#endif
#endif
double Igd4;
#if defined(_DERIVATE)
double Igd4_Vn2_n3;
#endif
double Igd3;
double Igd2;
#if defined(_DERIVATE)
double Igd2_Vn2_n3;
#endif
double Igd1;
#if defined(_DERIVATE)
double Igd1_Vn2_n3;
#endif
double Igd;
#if defined(_DERIVATE)
double Igd_Vn2_n3;
#endif
double Igs4;
#if defined(_DERIVATE)
double Igs4_Vn2_n4;
#endif
double Igs3;
double Igs2;
#if defined(_DERIVATE)
double Igs2_Vn2_n4;
#endif
double Igs1;
#if defined(_DERIVATE)
double Igs1_Vn2_n4;
#endif
double Igs;
#if defined(_DERIVATE)
double Igs_Vn2_n4;
#endif
double Fd;
#if defined(_DERIVATE)
double Fd_Vn3_n4;
#endif
double Al;
#if defined(_DERIVATE)
double Al_Vn3_n4;
#endif
double Vg;
#if defined(_DERIVATE)
double Vg_Vn3_n4;
double Vg_Vn2_n4;
#endif
double Vst;
#if defined(_DERIVATE)
double Vst_Vn3_n4;
#endif
double Nst;
#if defined(_DERIVATE)
double Nst_Vn3_n4;
#endif
double Ids1;
#if defined(_DERIVATE)
double Ids1_Vn2_n4;
double Ids1_Vn3_n4;
#endif
double Vf;
#if defined(_DERIVATE)
double Vf_Vn2_n4;
#endif
double Ah;
double Ids;
#if defined(_DERIVATE)
double Ids_Vn2_n4;
double Ids_Vn3_n4;
#endif
double Alpha_T2;
double Gamma_T2;
double Vto_T2;
#if defined(_DYNAMIC)
double Cgd_T2;
#endif
#if defined(_DYNAMIC)
double Cgs_T2;
#endif
double Is_T2;
#if defined(_DYNAMIC)
double Vbi_T2;
#endif
#if defined(_DYNAMIC)
double F3;
#endif
#if defined(_DYNAMIC)
double F2;
#endif
#if defined(_DYNAMIC)
double F1;
#endif
#if defined(_DYNAMIC)
double Eg_T2;
#endif
double Eg_T1;
double Vt_T2;
double Beta_T2;
double Rs_T2;
double Rd_T2;
double Rg_T2;
#if defined(_DYNAMIC)
double con1;
#endif
double Tr;
double T2;
double T1;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
{
double m00_min(d00_min0,(Fc*Vbi),Vmax)
Vmax=d00_min0;
}
#endif
T1=(Tnom+273.15);
T2=_circuit_temp;
Tr=(T2/T1);
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,Tr,1.5)
con1=d00_pow0;
}
#endif
Rg_T2=(Rg*(1+(Rgtc*(T2-T1))));
Rd_T2=(Rd*(1+(Rdtc*(T2-T1))));
Rs_T2=(Rs*(1+(Rstc*(T2-T1))));
{
double m00_pow(d00_pow0,1.01,(Betatc*(T2-T1)))
Beta_T2=((Area*Beta)*d00_pow0);
}
Vt_T2=_vt_nom;
Eg_T1=(Eg-(((7.02e-4*T1)*T1)/(1108.0+T1)));
#if defined(_DYNAMIC)
Eg_T2=(Eg-(((7.02e-4*T2)*T2)/(1108.0+T2)));
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-Fc),(1-M))
F1=((Vbi/(1-M))*(1-d00_pow0));
}
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-Fc),(1+M))
F2=d00_pow0;
}
#endif
#if defined(_DYNAMIC)
F3=(1-(Fc*(1+M)));
#endif
#if defined(_DYNAMIC)
{
double m00_logE(d00_logE0,con1)
Vbi_T2=(((Tr*Vbi)-((2*Vt_T2)*d00_logE0))-((Tr*Eg_T1)-Eg_T2));
}
#endif
{
double m00_pow(d00_pow0,Tr,(Xti/N))
double m00_limexp(d00_limexp1,(((-(1.602176462e-19*Eg_T1))*(1-Tr))/(1.3806503e-23*T2)))
Is_T2=(((Area*Is)*d00_pow0)*d00_limexp1);
}
#if defined(_DYNAMIC)
Cgs_T2=((Area*Cgs)*(1+(M*((400e-6*(T2-T1))-((Vbi_T2-Vbi)/Vbi)))));
#endif
#if defined(_DYNAMIC)
Cgd_T2=((Area*Cgd)*(1+(M*((400e-6*(T2-T1))-((Vbi_T2-Vbi)/Vbi)))));
#endif
Vto_T2=(Vto+(Vtotc*(T2-T1)));
Gamma_T2=(Gamma*(1+(Gammatc*(T2-T1))));
{
double m00_pow(d00_pow0,1.01,(Alphatc*(T2-T1)))
Alpha_T2=(Alpha*d00_pow0);
}
if
((LEVEL)==(1))
{
if
(((BP(n2,n4)-Vto_T2)>0))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m00_tanh(d00_tanh1,(Alpha*BP(n3,n4)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m10_tanh(d10_tanh1,d00_tanh1,(Alpha*BP(n3,n4)))
#endif
#if defined(_DERIVATE)
Ids_Vn2_n4=(Beta_T2*(d10_pow0*1.0))*(1+(Lambda*BP(n3,n4)))*d00_tanh1;
Ids_Vn3_n4=((((Beta_T2*d00_pow0)*(Lambda))*d00_tanh1)+(((Beta_T2*d00_pow0)*(1+(Lambda*BP(n3,n4))))*(Alpha)*d10_tanh1));
#endif
Ids=(((Beta_T2*d00_pow0)*(1+(Lambda*BP(n3,n4))))*d00_tanh1);
}
}
else
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
if
((LEVEL)==(2))
{
Ah=(1/((2*Vt_T2)*Nsc));
{
double m00_exp(d00_exp0,(Ah*(BP(n2,n4)-Vto_T2)))
double m00_logE(d00_logE1,(1+d00_exp0))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(1+d00_exp0))
#endif
#if defined(_DERIVATE)
Vf_Vn2_n4=((Ah)*d10_exp0*d10_logE1/Ah);
#endif
Vf=(d00_logE1/Ah);
}
{
double m00_pow(d00_pow0,Vf,2)
double m00_tanh(d00_tanh1,(Alpha*BP(n3,n4)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Vf,2)
double m10_tanh(d10_tanh1,d00_tanh1,(Alpha*BP(n3,n4)))
#endif
#if defined(_DERIVATE)
Ids_Vn2_n4=(Beta_T2*(d10_pow0*Vf_Vn2_n4))*(1+(Lambda*BP(n3,n4)))*d00_tanh1;
Ids_Vn3_n4=((((Beta_T2*d00_pow0)*(Lambda))*d00_tanh1)+(((Beta_T2*d00_pow0)*(1+(Lambda*BP(n3,n4))))*(Alpha)*d10_tanh1));
#endif
Ids=(((Beta_T2*d00_pow0)*(1+(Lambda*BP(n3,n4))))*d00_tanh1);
}
}
else
if
((LEVEL)==(3))
{
if
(((BP(n2,n4)-Vto_T2)>0))
{
if
(((0<BP(n3,n4))&&(BP(n3,n4)<(3/Alpha))))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m00_pow(d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m10_pow(d10_pow1,d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#endif
#if defined(_DERIVATE)
Ids_Vn2_n4=((((Beta_T2*(1+(Lambda*BP(n3,n4))))*(d10_pow0*1.0))*(1-d00_pow1)*(1+(B*(BP(n2,n4)-Vto_T2)))-(((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*(1-d00_pow1))*(B))/(1+(B*(BP(n2,n4)-Vto_T2)))/(1+(B*(BP(n2,n4)-Vto_T2))));
Ids_Vn3_n4=((((Beta_T2*(Lambda))*d00_pow0*(1-d00_pow1))+(((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*(-(d10_pow1*(-((Alpha)/3))))))/(1+(B*(BP(n2,n4)-Vto_T2))));
#endif
Ids=((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*(1-d00_pow1))/(1+(B*(BP(n2,n4)-Vto_T2))));
}
}
if
((BP(n3,n4)>=(3/Alpha)))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),2)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),2)
#endif
#if defined(_DERIVATE)
Ids_Vn2_n4=((((Beta_T2*(1+(Lambda*BP(n3,n4))))*(d10_pow0*1.0))*(1+(B*(BP(n2,n4)-Vto_T2)))-((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*(B))/(1+(B*(BP(n2,n4)-Vto_T2)))/(1+(B*(BP(n2,n4)-Vto_T2))));
Ids_Vn3_n4=((Beta_T2*(Lambda))*d00_pow0/(1+(B*(BP(n2,n4)-Vto_T2))));
#endif
Ids=(((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)/(1+(B*(BP(n2,n4)-Vto_T2))));
}
}
if
((BP(n3,n4)<0))
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
if
((LEVEL)==(4))
{
if
(((BP(n2,n4)-Vto_T2)>0))
{
if
(((0<BP(n3,n4))&&(BP(n3,n4)<(3/Alpha))))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
double m00_pow(d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
double m10_pow(d10_pow1,d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*1.0))*(1-d00_pow1);
Ids1_Vn3_n4=((Beta_T2*d00_pow0)*(-(d10_pow1*(-((Alpha)/3)))));
#endif
Ids1=((Beta_T2*d00_pow0)*(1-d00_pow1));
}
#if defined(_DERIVATE)
Ids_Vn2_n4=((Ids1_Vn2_n4*(1+(Lambda*BP(n3,n4)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*((Delta*BP(n3,n4))*Ids1_Vn2_n4))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
Ids_Vn3_n4=((((Ids1_Vn3_n4*(1+(Lambda*BP(n3,n4))))+(Ids1*(Lambda)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4)))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
#endif
Ids=((Ids1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n3,n4))*Ids1)));
}
if
((BP(n3,n4)>=(3/Alpha)))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*1.0));
Ids1_Vn3_n4=0.0;
#endif
Ids1=(Beta_T2*d00_pow0);
}
#if defined(_DERIVATE)
Ids_Vn2_n4=((Ids1_Vn2_n4*(1+(Lambda*BP(n3,n4)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*((Delta*BP(n3,n4))*Ids1_Vn2_n4))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
Ids_Vn3_n4=((((Ids1_Vn3_n4*(1+(Lambda*BP(n3,n4))))+(Ids1*(Lambda)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4)))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
#endif
Ids=((Ids1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n3,n4))*Ids1)));
}
if
((BP(n3,n4)<0))
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
if
((LEVEL)==(5))
{
if
(((BP(n2,n4)-Vto_T2)>0))
{
#if defined(_DERIVATE)
Nst_Vn3_n4=(Nd);
#endif
Nst=(Ng+(Nd*BP(n3,n4)));
if
((Nst<1.0))
{
#if defined(_DERIVATE)
Nst_Vn3_n4=0.0;
#endif
Nst=1.0;
}
#if defined(_DERIVATE)
Vst_Vn3_n4=Nst_Vn3_n4*Vt_T2;
#endif
Vst=(Nst*Vt_T2);
{
double m00_exp(d00_exp0,(((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))/(Qp*Vst)))
double m00_logE(d00_logE1,(d00_exp0+1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(d00_exp0+1))
#endif
#if defined(_DERIVATE)
Vg_Vn3_n4=(((Qp*Vst_Vn3_n4)*d00_logE1)+((Qp*Vst)*(((Gamma_T2)*(Qp*Vst)-((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))*(Qp*Vst_Vn3_n4))/(Qp*Vst)/(Qp*Vst))*d10_exp0*d10_logE1));
Vg_Vn2_n4=((Qp*Vst)*(1/(Qp*Vst))*d10_exp0*d10_logE1);
#endif
Vg=((Qp*Vst)*d00_logE1);
}
#if defined(_DERIVATE)
Al_Vn3_n4=(Alpha_T2);
#endif
Al=(Alpha_T2*BP(n3,n4));
{
double m00_sqrt(d00_sqrt0,(1.0+(Al*Al)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+(Al*Al)))
#endif
#if defined(_DERIVATE)
Fd_Vn3_n4=((Al_Vn3_n4*d00_sqrt0-Al*((Al_Vn3_n4*Al)+(Al*Al_Vn3_n4))*d10_sqrt0)/d00_sqrt0/d00_sqrt0);
#endif
Fd=(Al/d00_sqrt0);
}
{
double m00_pow(d00_pow0,Vg,Qp)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Vg,Qp)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*Vg_Vn2_n4))*Fd;
Ids1_Vn3_n4=(((Beta_T2*(d10_pow0*Vg_Vn3_n4))*Fd)+((Beta_T2*d00_pow0)*Fd_Vn3_n4));
#endif
Ids1=((Beta_T2*d00_pow0)*Fd);
}
#if defined(_DERIVATE)
Ids_Vn2_n4=((Ids1_Vn2_n4*(1+(Lambda*BP(n3,n4)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*((Delta*BP(n3,n4))*Ids1_Vn2_n4))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
Ids_Vn3_n4=((((Ids1_Vn3_n4*(1+(Lambda*BP(n3,n4))))+(Ids1*(Lambda)))*(1+((Delta*BP(n3,n4))*Ids1))-(Ids1*(1+(Lambda*BP(n3,n4))))*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4)))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)));
#endif
Ids=((Ids1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n3,n4))*Ids1)));
}
else
{
#if defined(_DERIVATE)
Ids_Vn2_n4=0.0;
Ids_Vn3_n4=0.0;
#endif
Ids=0;
}
}
else
{ /* no default */ }
if
((ILEVELS)==(0))
{
#if defined(_DERIVATE)
Igs_Vn2_n4=0.0;
#endif
Igs=0;
}
else
if
((ILEVELS)==(1))
{
if
((BP(n2,n4)>Vbi))
{
#if defined(_DERIVATE)
Igs_Vn2_n4=(1/Rf);
#endif
Igs=((BP(n2,n4)-Vbi)/Rf);
}
else
{
#if defined(_DERIVATE)
Igs_Vn2_n4=(1e-12);
#endif
Igs=(((-Area)*Is)+(BP(n2,n4)*1e-12));
}
}
else
if
((ILEVELS)==(2))
{
if
((BP(n2,n4)>Vbi))
{
#if defined(_DERIVATE)
Igs1_Vn2_n4=(1/Rf);
#endif
Igs1=((BP(n2,n4)-Vbi)/Rf);
}
else
{
#if defined(_DERIVATE)
Igs1_Vn2_n4=(1e-12);
#endif
Igs1=(((-Area)*Is)+(BP(n2,n4)*1e-12));
}
if
((BP(n2,n4)<(-Bv)))
{
#if defined(_DERIVATE)
Igs2_Vn2_n4=(1/R1);
#endif
Igs2=((BP(n2,n4)+Bv)/R1);
}
#if defined(_DERIVATE)
Igs_Vn2_n4=(Igs1_Vn2_n4+Igs2_Vn2_n4);
#endif
Igs=(Igs1+Igs2);
}
else
if
((ILEVELS)==(3))
{
if
((BP(n2,n4)>(((-5.0)*N)*Vt_T2)))
{
{
double m00_limexp(d00_limexp0,(BP(n2,n4)/(N*Vt_T2)))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(n2,n4)/(N*Vt_T2)))
#endif
#if defined(_DERIVATE)
Igs_Vn2_n4=((Is_T2*(1/(N*Vt_T2))*d10_limexp0)+(1e-12));
#endif
Igs=((Is_T2*(d00_limexp0-1.0))+(BP(n2,n4)*1e-12));
}
}
else
{
#if defined(_DERIVATE)
Igs_Vn2_n4=(1e-12);
#endif
Igs=((-Is_T2)+(BP(n2,n4)*1e-12));
}
}
else
if
((ILEVELS)==(4))
{
if
((BP(n2,n4)>(((-5.0)*N)*Vt_T2)))
{
{
double m00_limexp(d00_limexp0,(BP(n2,n4)/(N*Vt_T2)))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(n2,n4)/(N*Vt_T2)))
#endif
#if defined(_DERIVATE)
Igs1_Vn2_n4=(((Area*Is_T2)*(1/(N*Vt_T2))*d10_limexp0)+(1e-12));
#endif
Igs1=(((Area*Is_T2)*(d00_limexp0-1.0))+(BP(n2,n4)*1e-12));
}
}
else
{
#if defined(_DERIVATE)
Igs1_Vn2_n4=0.0;
#endif
Igs1=0;
}
if
((((-Bv)<BP(n2,n4))&&(BP(n2,n4)<(((-5.0)*N)*Vt_T2))))
{
#if defined(_DERIVATE)
Igs2_Vn2_n4=(1e-12);
#endif
Igs2=(((-Area)*Is_T2)+(BP(n2,n4)*1e-12));
}
else
{
#if defined(_DERIVATE)
Igs2_Vn2_n4=0.0;
#endif
Igs2=0;
}
if
((BP(n2,n4)==(-Bv)))
{
Igs3=(-Ibv);
}
else
{
Igs3=0;
}
if
((BP(n2,n4)<(-Bv)))
{
{
double m00_limexp(d00_limexp0,((-(Bv+BP(n2,n4)))/Vt_T2))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,((-(Bv+BP(n2,n4)))/Vt_T2))
#endif
#if defined(_DERIVATE)
Igs4_Vn2_n4=(((-Area)*Is_T2)*((-1.0)/Vt_T2)*d10_limexp0);
#endif
Igs4=(((-Area)*Is_T2)*((d00_limexp0-1.0)+(Bv/Vt_T2)));
}
}
else
{
#if defined(_DERIVATE)
Igs4_Vn2_n4=0.0;
#endif
Igs4=0;
}
#if defined(_DERIVATE)
Igs_Vn2_n4=((Igs1_Vn2_n4+Igs2_Vn2_n4)+Igs4_Vn2_n4);
#endif
Igs=(((Igs1+Igs2)+Igs3)+Igs4);
}
else
{ /* no default */ }
if
((ILEVELD)==(0))
{
#if defined(_DERIVATE)
Igd_Vn2_n3=0.0;
#endif
Igd=0;
}
else
if
((ILEVELD)==(1))
{
if
((BP(n2,n3)>Vbi))
{
#if defined(_DERIVATE)
Igd_Vn2_n3=(1/Rf);
#endif
Igd=((BP(n2,n3)-Vbi)/Rf);
}
else
{
#if defined(_DERIVATE)
Igd_Vn2_n3=(1e-12);
#endif
Igd=(((-Area)*Is)+(BP(n2,n3)*1e-12));
}
}
else
if
((ILEVELD)==(2))
{
if
((BP(n2,n3)>Vbi))
{
#if defined(_DERIVATE)
Igd1_Vn2_n3=(1/Rf);
#endif
Igd1=((BP(n2,n3)-Vbi)/Rf);
}
else
{
#if defined(_DERIVATE)
Igd1_Vn2_n3=(1e-12);
#endif
Igd1=(((-Area)*Is)+(BP(n2,n3)*1e-12));
}
if
((BP(n2,n3)<(-Bv)))
{
#if defined(_DERIVATE)
Igd2_Vn2_n3=(1/R1);
#endif
Igd2=((BP(n2,n3)+Bv)/R1);
}
#if defined(_DERIVATE)
Igd_Vn2_n3=(Igd1_Vn2_n3+Igd2_Vn2_n3);
#endif
Igd=(Igd1+Igd2);
}
else
if
((ILEVELD)==(3))
{
if
((BP(n2,n3)>(((-5.0)*N)*Vt_T2)))
{
{
double m00_limexp(d00_limexp0,(BP(n2,n3)/(N*Vt_T2)))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(n2,n3)/(N*Vt_T2)))
#endif
#if defined(_DERIVATE)
Igd_Vn2_n3=(((Area*Is_T2)*(1/(N*Vt_T2))*d10_limexp0)+(1e-12));
#endif
Igd=(((Area*Is_T2)*(d00_limexp0-1.0))+(BP(n2,n3)*1e-12));
}
}
else
{
#if defined(_DERIVATE)
Igd_Vn2_n3=(1e-12);
#endif
Igd=((-Is_T2)+(BP(n2,n3)*1e-12));
}
}
else
if
((ILEVELD)==(4))
{
if
((BP(n2,n3)>(((-5.0)*N)*Vt_T2)))
{
{
double m00_limexp(d00_limexp0,(BP(n2,n3)/(N*Vt_T2)))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(n2,n3)/(N*Vt_T2)))
#endif
#if defined(_DERIVATE)
Igd1_Vn2_n3=(((Area*Is_T2)*(1/(N*Vt_T2))*d10_limexp0)+(1e-12));
#endif
Igd1=(((Area*Is_T2)*(d00_limexp0-1.0))+(BP(n2,n3)*1e-12));
}
}
else
{
#if defined(_DERIVATE)
Igd1_Vn2_n3=0.0;
#endif
Igd1=0;
}
if
((((-Bv)<BP(n2,n3))&&(BP(n2,n3)<(((-5.0)*N)*Vt_T2))))
{
#if defined(_DERIVATE)
Igd2_Vn2_n3=(1e-12);
#endif
Igd2=(((-Area)*Is_T2)+(BP(n2,n3)*1e-12));
}
else
{
#if defined(_DERIVATE)
Igd2_Vn2_n3=0.0;
#endif
Igd2=0;
}
if
((BP(n2,n3)==(-Bv)))
{
Igd3=(-Ibv);
}
else
{
Igd3=0;
}
if
((BP(n2,n3)<(-Bv)))
{
{
double m00_limexp(d00_limexp0,((-(Bv+BP(n2,n3)))/Vt_T2))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,((-(Bv+BP(n2,n3)))/Vt_T2))
#endif
#if defined(_DERIVATE)
Igd4_Vn2_n3=(((-Area)*Is_T2)*((-1.0)/Vt_T2)*d10_limexp0);
#endif
Igd4=(((-Area)*Is_T2)*((d00_limexp0-1.0)+(Bv/Vt_T2)));
}
}
else
{
#if defined(_DERIVATE)
Igd4_Vn2_n3=0.0;
#endif
Igd4=0;
}
#if defined(_DERIVATE)
Igd_Vn2_n3=((Igd1_Vn2_n3+Igd2_Vn2_n3)+Igd4_Vn2_n3);
#endif
Igd=(((Igd1+Igd2)+Igd3)+Igd4);
}
else
{ /* no default */ }
#ifdef _DYNAMIC
if
((QLEVELS==0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs_Vn2_n3=0.0;
Qgs_Vn2_n1=0.0;
#endif
Qgs=0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELS==1))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs_Vn2_n3=0.0;
Qgs_Vn2_n1=((Area*Cgs));
#endif
Qgs=((Area*Cgs)*BP(n2,n1));
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELS==2))
{
#ifdef _DYNAMIC
if
((BP(n2,n1)<(Fc*Vbi)))
{
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-(BP(n2,n1)/Vbi_T2)),(1-M))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-(BP(n2,n1)/Vbi_T2)),(1-M))
#endif
#if defined(_DERIVATE)
Qgs1_Vn2_n1=(((Cgs_T2*Vbi_T2)/(1-M))*(-(d10_pow0*(-(1/Vbi_T2)))));
#endif
Qgs1=(((Cgs_T2*Vbi_T2)/(1-M))*(1-d00_pow0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs1_Vn2_n1=0.0;
#endif
Qgs1=0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((BP(n2,n1)>=(Fc*Vbi)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs2_Vn2_n1=(Cgs_T2*((1/F2)*((F3)+((M/(2*Vbi_T2))*(BP(n2,n1)+BP(n2,n1))))));
#endif
Qgs2=(Cgs_T2*(F1+((1/F2)*((F3*(BP(n2,n1)-(Fc*Vbi_T2)))+((M/(2*Vbi_T2))*((BP(n2,n1)*BP(n2,n1))-(((Fc*Vbi_T2)*Fc)*Vbi_T2)))))));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs2_Vn2_n1=0.0;
#endif
Qgs2=0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgs_Vn2_n3=0.0;
Qgs_Vn2_n1=(Qgs1_Vn2_n1+Qgs2_Vn2_n1);
#endif
Qgs=(Qgs1+Qgs2);
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELS==3))
{
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(BP(n2,n1)-BP(n2,n3)),2)
double m00_sqrt(d00_sqrt1,(d00_pow0+(Vdelta1*Vdelta1)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n1)-BP(n2,n3)),2)
double m10_sqrt(d10_sqrt1,d00_sqrt1,(d00_pow0+(Vdelta1*Vdelta1)))
#endif
#if defined(_DERIVATE)
Veff1_Vn2_n1=(0.5*(1.0+(d10_pow0*1.0)*d10_sqrt1));
Veff1_Vn2_n3=(0.5*(1.0+(d10_pow0*(-1.0))*d10_sqrt1));
#endif
Veff1=(0.5*((BP(n2,n1)+BP(n2,n3))+d00_sqrt1));
}
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(Veff1-Vto),2)
double m00_sqrt(d00_sqrt1,(d00_pow0+(Vdelta2*Vdelta2)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(Veff1-Vto),2)
double m10_sqrt(d10_sqrt1,d00_sqrt1,(d00_pow0+(Vdelta2*Vdelta2)))
#endif
#if defined(_DERIVATE)
Vnew_Vn2_n1=(0.5*(Veff1_Vn2_n1+(d10_pow0*Veff1_Vn2_n1)*d10_sqrt1));
Vnew_Vn2_n3=(0.5*(Veff1_Vn2_n3+(d10_pow0*Veff1_Vn2_n3)*d10_sqrt1));
#endif
Vnew=(0.5*((Veff1+Vto_T2)+d00_sqrt1));
}
#endif
#ifdef _DYNAMIC
if
((Vnew>Vmax))
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1-(Vmax/Vbi_T2)))
double m00_sqrt(d00_sqrt1,(1-(Vmax/Vbi)))
#if defined(_DERIVATE)
Qgs_Vn2_n3=(Cgs_T2*(Vnew_Vn2_n3/d00_sqrt1));
Qgs_Vn2_n1=(Cgs_T2*(Vnew_Vn2_n1/d00_sqrt1));
#endif
Qgs=(Cgs_T2*(((2*Vbi_T2)*(1-d00_sqrt0))+((Vnew-Vmax)/d00_sqrt1)));
}
#endif
}
else
{
#if defined(_DYNAMIC)
{
double m00_sqrt(d00_sqrt0,(1-(Vnew/Vbi)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1-(Vnew/Vbi)))
#endif
#if defined(_DERIVATE)
Qgs_Vn2_n3=(((Cgs_T2*2)*Vbi)*(-(-(Vnew_Vn2_n3/Vbi))*d10_sqrt0));
Qgs_Vn2_n1=(((Cgs_T2*2)*Vbi)*(-(-(Vnew_Vn2_n1/Vbi))*d10_sqrt0));
#endif
Qgs=(((Cgs_T2*2)*Vbi)*(1-d00_sqrt0));
}
#endif
}
#endif /* if (...) */
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELD==0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd_Vn2_n1=0.0;
Qgd_Vn2_n3=0.0;
#endif
Qgd=0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELD==1))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd_Vn2_n1=0.0;
Qgd_Vn2_n3=((Area*Cgd));
#endif
Qgd=((Area*Cgd)*BP(n2,n3));
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELD==2))
{
#ifdef _DYNAMIC
if
((BP(n2,n3)<(Fc*Vbi)))
{
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-(BP(n2,n3)/Vbi_T2)),(1-M))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-(BP(n2,n3)/Vbi_T2)),(1-M))
#endif
#if defined(_DERIVATE)
Qgd1_Vn2_n3=(((Cgd_T2*Vbi_T2)/(1-M))*(-(d10_pow0*(-(1/Vbi_T2)))));
#endif
Qgd1=(((Cgd_T2*Vbi_T2)/(1-M))*(1-d00_pow0));
}
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd1_Vn2_n3=0.0;
#endif
Qgd1=0;
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((BP(n2,n3)>=(Fc*Vbi)))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd2_Vn2_n3=(Cgd_T2*((1/F2)*((F3)+((M/(2*Vbi_T2))*(BP(n2,n3)+BP(n2,n3))))));
#endif
Qgd2=(Cgd_T2*(F1+((1/F2)*((F3*(BP(n2,n3)-(Fc*Vbi_T2)))+((M/(2*Vbi_T2))*((BP(n2,n3)*BP(n2,n3))-(((Fc*Vbi_T2)*Fc)*Vbi_T2)))))));
#endif
}
else
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd2_Vn2_n3=0.0;
#endif
Qgd2=0;
#endif
}
#endif /* if (...) */
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd_Vn2_n1=0.0;
Qgd_Vn2_n3=(Qgd1_Vn2_n3+Qgd2_Vn2_n3);
#endif
Qgd=(Qgd1+Qgd2);
#endif
}
#endif /* if (...) */
#ifdef _DYNAMIC
if
((QLEVELD==3))
{
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(BP(n2,n1)-BP(n2,n3)),2)
double m00_sqrt(d00_sqrt1,(d00_pow0+(Vdelta1*Vdelta1)))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n1)-BP(n2,n3)),2)
double m10_sqrt(d10_sqrt1,d00_sqrt1,(d00_pow0+(Vdelta1*Vdelta1)))
#endif
#if defined(_DERIVATE)
Veff2_Vn2_n1=(0.5*(1.0-(d10_pow0*1.0)*d10_sqrt1));
Veff2_Vn2_n3=(0.5*(1.0-(d10_pow0*(-1.0))*d10_sqrt1));
#endif
Veff2=(0.5*((BP(n2,n1)+BP(n2,n3))-d00_sqrt1));
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qgd_Vn2_n1=(Cgd_T2*Veff2_Vn2_n1);
Qgd_Vn2_n3=(Cgd_T2*Veff2_Vn2_n3);
#endif
Qgd=(Cgd_T2*Veff2);
#endif
}
#endif /* if (...) */
if
((QLEVELDS)==(0))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qds_Vn2_n4=0.0;
Qds_Vn3_n4=0.0;
#endif
Qds=0;
#endif
}
else
if
((QLEVELDS)==(1))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qds_Vn2_n4=0.0;
Qds_Vn3_n4=((Area*Cds));
#endif
Qds=((Area*Cds)*BP(n3,n4));
#endif
}
else
if
((QLEVELDS)==(2))
{
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qds_Vn2_n4=(Tau*Ids_Vn2_n4);
Qds_Vn3_n4=(((Area*Cds))+(Tau*Ids_Vn3_n4));
#endif
Qds=(((Area*Cds)*BP(n3,n4))+(Tau*Ids));
#endif
}
else
{ /* no default */ }
_load_static_residual2(n2,n4,Igs);
#if defined(_DERIVATE)
_load_static_jacobian4(n2,n4,n2,n4,Igs_Vn2_n4);
#endif
_load_static_residual2(n2,n3,Igd);
#if defined(_DERIVATE)
_load_static_jacobian4(n2,n3,n2,n3,Igd_Vn2_n3);
#endif
_load_static_residual2(n3,n4,Ids);
#if defined(_DERIVATE)
_load_static_jacobian4(n3,n4,n3,n4,Ids_Vn3_n4);
_load_static_jacobian4(n3,n4,n2,n4,Ids_Vn2_n4);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n3,n4,_DDT(Qds));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n3,n4,n3,n4,Qds_Vn3_n4);
_load_dynamic_jacobian4(n3,n4,n2,n4,Qds_Vn2_n4);
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n2,n1,_DDT(Qgs));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n2,n1,n2,n1,Qgs_Vn2_n1);
_load_dynamic_jacobian4(n2,n1,n2,n3,Qgs_Vn2_n3);
#endif
#endif
_load_static_residual2(n1,n4,((Area*BP(n1,n4))/Rin));
#if defined(_DERIVATE)
_load_static_jacobian4(n1,n4,n1,n4,((Area)/Rin));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n2,n3,_DDT(Qgd));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n2,n3,n2,n3,Qgd_Vn2_n3);
_load_dynamic_jacobian4(n2,n3,n2,n1,Qgd_Vn2_n1);
#endif
#endif
_load_static_residual2(Gate,n2,(BP(Gate,n2)/Rg_T2));
#if defined(_DERIVATE)
_load_static_jacobian4(Gate,n2,Gate,n2,(1/Rg_T2));
#endif
_load_static_residual2(Drain,n3,((Area*BP(Drain,n3))/Rd_T2));
#if defined(_DERIVATE)
_load_static_jacobian4(Drain,n3,Drain,n3,((Area)/Rd_T2));
#endif
_load_static_residual2(n4,Source,((Area*BP(n4,Source))/Rs_T2));
#if defined(_DERIVATE)
_load_static_jacobian4(n4,Source,n4,Source,((Area)/Rs_T2));
#endif
fourkt=((4.0*1.3806503e-23)*T2);
if
(((LEVEL==1)||(LEVEL==2)))
{
{
double m00_tanh(d00_tanh0,(Alpha_T2*BP(n3,n4)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(Alpha_T2*BP(n3,n4)))
#endif
#if defined(_DERIVATE)
gm_Vn2_n4=((2*Beta_T2))*(1+(Lambda*BP(n3,n4)))*d00_tanh0;
gm_Vn3_n4=(((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(Lambda))*d00_tanh0)+((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*(Alpha_T2)*d10_tanh0));
#endif
gm=((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*d00_tanh0);
}
if
((BP(n3,n4)<(3/Alpha)))
{
#if defined(_DERIVATE)
An_Vn3_n4=(-(1/(BP(n2,n4)-Vto_T2)));
An_Vn2_n4=(-(-BP(n3,n4)/(BP(n2,n4)-Vto_T2)/(BP(n2,n4)-Vto_T2)));
#endif
An=(1-(BP(n3,n4)/(BP(n2,n4)-Vto_T2)));
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn2_n4+((An_Vn2_n4*An)+(An*An_Vn2_n4)))*(1+An)-((1+An)+(An*An))*An_Vn2_n4)/(1+An)/(1+An))))*Gdsnoi;
thermal_pwr_Vn3_n4=((((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn3_n4+((An_Vn3_n4*An)+(An*An_Vn3_n4)))*(1+An)-((1+An)+(An*An))*An_Vn3_n4)/(1+An)/(1+An))))*Gdsnoi;
#endif
thermal_pwr=((((((8*1.3806503e-23)*T2)*gm)/3)*(((1+An)+(An*An))/(1+An)))*Gdsnoi);
}
else
{
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*Gdsnoi;
thermal_pwr_Vn3_n4=((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*Gdsnoi;
#endif
thermal_pwr=(((((8*1.3806503e-23)*T2)*gm)/3)*Gdsnoi);
}
_save_whitenoise2(n3,n4,thermal_pwr,"thermal");
{
double m00_pow(d00_pow0,Ids,Af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ids,Af)
#endif
#if defined(_DERIVATE)
flicker_pwr_Vn2_n4=(Kf*(d10_pow0*Ids_Vn2_n4));
flicker_pwr_Vn3_n4=(Kf*(d10_pow0*Ids_Vn3_n4));
#endif
flicker_pwr=(Kf*d00_pow0);
}
_save_flickernoise2(n3,n4,flicker_pwr,1.0,"flicker");
}
if
((LEVEL==3))
{
if
((BP(n3,n4)<(3/Alpha)))
{
#if defined(_DERIVATE)
H1_Vn3_n4=((-(-((Alpha)/3)))/(1+(B*(BP(n2,n4)-Vto_T2))));
H1_Vn2_n4=(-(1-(1-((Alpha*BP(n3,n4))/3)))*(B)/(1+(B*(BP(n2,n4)-Vto_T2)))/(1+(B*(BP(n2,n4)-Vto_T2))));
#endif
H1=((1-(1-((Alpha*BP(n3,n4))/3)))/(1+(B*(BP(n2,n4)-Vto_T2))));
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),2)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),2)
#endif
#if defined(_DERIVATE)
gm_Vn2_n4=(((((2*Beta_T2))*(1+(Lambda*BP(n3,n4)))*H1)+((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*H1_Vn2_n4))+((((((Beta_T2*(1+(Lambda*BP(n3,n4))))*(d10_pow0*1.0))*B*H1)+((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)*H1_Vn2_n4))*(1+(B*(BP(n2,n4)-Vto_T2)))-((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)*H1)*(B))/(1+(B*(BP(n2,n4)-Vto_T2)))/(1+(B*(BP(n2,n4)-Vto_T2)))));
gm_Vn3_n4=((((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(Lambda))*H1)+((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*H1_Vn3_n4))+((((Beta_T2*(Lambda))*d00_pow0*B*H1)+((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)*H1_Vn3_n4))/(1+(B*(BP(n2,n4)-Vto_T2)))));
#endif
gm=(((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*H1)+(((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)*H1)/(1+(B*(BP(n2,n4)-Vto_T2)))));
}
#if defined(_DERIVATE)
An_Vn3_n4=(-(1/(BP(n2,n4)-Vto_T2)));
An_Vn2_n4=(-(-BP(n3,n4)/(BP(n2,n4)-Vto_T2)/(BP(n2,n4)-Vto_T2)));
#endif
An=(1-(BP(n3,n4)/(BP(n2,n4)-Vto_T2)));
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn2_n4+((An_Vn2_n4*An)+(An*An_Vn2_n4)))*(1+An)-((1+An)+(An*An))*An_Vn2_n4)/(1+An)/(1+An))))*Gdsnoi;
thermal_pwr_Vn3_n4=((((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn3_n4+((An_Vn3_n4*An)+(An*An_Vn3_n4)))*(1+An)-((1+An)+(An*An))*An_Vn3_n4)/(1+An)/(1+An))))*Gdsnoi;
#endif
thermal_pwr=((((((8*1.3806503e-23)*T2)*gm)/3)*(((1+An)+(An*An))/(1+An)))*Gdsnoi);
}
else
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m00_pow(d00_pow1,(1+(B*(BP(n2,n4)-Vto_T2))),2)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),2)
double m10_pow(d10_pow1,d00_pow1,(1+(B*(BP(n2,n4)-Vto_T2))),2)
#endif
#if defined(_DERIVATE)
gm_Vn2_n4=(((((2*Beta_T2))*(1+(Lambda*BP(n3,n4)))*(1+(B*(BP(n2,n4)-Vto_T2)))-(((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))*(B))/(1+(B*(BP(n2,n4)-Vto_T2)))/(1+(B*(BP(n2,n4)-Vto_T2))))+((((Beta_T2*(1+(Lambda*BP(n3,n4))))*(d10_pow0*1.0))*B*d00_pow1-(((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)*(d10_pow1*(B)))/d00_pow1/d00_pow1));
gm_Vn3_n4=(((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(Lambda))/(1+(B*(BP(n2,n4)-Vto_T2))))+((Beta_T2*(Lambda))*d00_pow0*B/d00_pow1));
#endif
gm=(((((2*Beta_T2)*(BP(n2,n4)-Vto_T2))*(1+(Lambda*BP(n3,n4))))/(1+(B*(BP(n2,n4)-Vto_T2))))+((((Beta_T2*(1+(Lambda*BP(n3,n4))))*d00_pow0)*B)/d00_pow1));
}
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*Gdsnoi;
thermal_pwr_Vn3_n4=((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*Gdsnoi;
#endif
thermal_pwr=(((((8*1.3806503e-23)*T2)*gm)/3)*Gdsnoi);
}
_save_whitenoise2(n3,n4,thermal_pwr,"thermal");
{
double m00_pow(d00_pow0,Ids,Af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ids,Af)
#endif
#if defined(_DERIVATE)
flicker_pwr_Vn2_n4=(Kf*(d10_pow0*Ids_Vn2_n4));
flicker_pwr_Vn3_n4=(Kf*(d10_pow0*Ids_Vn3_n4));
#endif
flicker_pwr=(Kf*d00_pow0);
}
_save_flickernoise2(n3,n4,flicker_pwr,1.0,"flicker");
}
if
((LEVEL==4))
{
if
((BP(n3,n4)<(3/Alpha)))
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
double m00_pow(d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
double m10_pow(d10_pow1,d00_pow1,(1-((Alpha*BP(n3,n4))/3)),3)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*1.0))*(1-d00_pow1);
Ids1_Vn3_n4=((Beta_T2*d00_pow0)*(-(d10_pow1*(-((Alpha)/3)))));
#endif
Ids1=((Beta_T2*d00_pow0)*(1-d00_pow1));
}
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),(Qp-1))
double m00_pow(d00_pow1,((Alpha*BP(n3,n4))/3),3)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),(Qp-1))
double m10_pow(d10_pow1,d00_pow1,((Alpha*BP(n3,n4))/3),3)
#endif
#if defined(_DERIVATE)
gm1_Vn2_n4=((Qp*Beta_T2)*(d10_pow0*1.0))*(1-(1-d00_pow1));
gm1_Vn3_n4=(((Qp*Beta_T2)*d00_pow0)*(-(-(d10_pow1*((Alpha)/3)))));
#endif
gm1=(((Qp*Beta_T2)*d00_pow0)*(1-(1-d00_pow1)));
}
#if defined(_DERIVATE)
gm_Vn2_n4=((((gm1_Vn2_n4*(1+(Lambda*BP(n3,n4)))*(1+((Delta*BP(n2,n4))*Ids1))-(gm1*(1+(Lambda*BP(n3,n4))))*(((Delta)*Ids1)+((Delta*BP(n2,n4))*Ids1_Vn2_n4)))/(1+((Delta*BP(n2,n4))*Ids1))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))))+(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*((((Delta*BP(n3,n4))*Ids1_Vn2_n4)*(1+((Delta*BP(n3,n4))*Ids1))-((Delta*BP(n3,n4))*Ids1)*((Delta*BP(n3,n4))*Ids1_Vn2_n4))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)))));
gm_Vn3_n4=((((((gm1_Vn3_n4*(1+(Lambda*BP(n3,n4))))+(gm1*(Lambda)))*(1+((Delta*BP(n2,n4))*Ids1))-(gm1*(1+(Lambda*BP(n3,n4))))*((Delta*BP(n2,n4))*Ids1_Vn3_n4))/(1+((Delta*BP(n2,n4))*Ids1))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))))+(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*(((((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4))*(1+((Delta*BP(n3,n4))*Ids1))-((Delta*BP(n3,n4))*Ids1)*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4)))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)))));
#endif
gm=(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))));
#if defined(_DERIVATE)
An_Vn3_n4=(-(1/(BP(n2,n4)-Vto_T2)));
An_Vn2_n4=(-(-BP(n3,n4)/(BP(n2,n4)-Vto_T2)/(BP(n2,n4)-Vto_T2)));
#endif
An=(1-(BP(n3,n4)/(BP(n2,n4)-Vto_T2)));
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn2_n4+((An_Vn2_n4*An)+(An*An_Vn2_n4)))*(1+An)-((1+An)+(An*An))*An_Vn2_n4)/(1+An)/(1+An))))*Gdsnoi;
thermal_pwr_Vn3_n4=((((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn3_n4+((An_Vn3_n4*An)+(An*An_Vn3_n4)))*(1+An)-((1+An)+(An*An))*An_Vn3_n4)/(1+An)/(1+An))))*Gdsnoi;
#endif
thermal_pwr=((((((8*1.3806503e-23)*T2)*gm)/3)*(((1+An)+(An*An))/(1+An)))*Gdsnoi);
}
else
{
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),Qp)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*1.0));
Ids1_Vn3_n4=0.0;
#endif
Ids1=(Beta_T2*d00_pow0);
}
{
double m00_pow(d00_pow0,(BP(n2,n4)-Vto_T2),(Qp-1))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(BP(n2,n4)-Vto_T2),(Qp-1))
#endif
#if defined(_DERIVATE)
gm1_Vn2_n4=((Qp*Beta_T2)*(d10_pow0*1.0));
gm1_Vn3_n4=0.0;
#endif
gm1=((Qp*Beta_T2)*d00_pow0);
}
#if defined(_DERIVATE)
gm_Vn2_n4=((((gm1_Vn2_n4*(1+(Lambda*BP(n3,n4)))*(1+((Delta*BP(n2,n4))*Ids1))-(gm1*(1+(Lambda*BP(n3,n4))))*(((Delta)*Ids1)+((Delta*BP(n2,n4))*Ids1_Vn2_n4)))/(1+((Delta*BP(n2,n4))*Ids1))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))))+(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*((((Delta*BP(n3,n4))*Ids1_Vn2_n4)*(1+((Delta*BP(n3,n4))*Ids1))-((Delta*BP(n3,n4))*Ids1)*((Delta*BP(n3,n4))*Ids1_Vn2_n4))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)))));
gm_Vn3_n4=((((((gm1_Vn3_n4*(1+(Lambda*BP(n3,n4))))+(gm1*(Lambda)))*(1+((Delta*BP(n2,n4))*Ids1))-(gm1*(1+(Lambda*BP(n3,n4))))*((Delta*BP(n2,n4))*Ids1_Vn3_n4))/(1+((Delta*BP(n2,n4))*Ids1))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))))+(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*(((((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4))*(1+((Delta*BP(n3,n4))*Ids1))-((Delta*BP(n3,n4))*Ids1)*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4)))/(1+((Delta*BP(n3,n4))*Ids1))/(1+((Delta*BP(n3,n4))*Ids1)))));
#endif
gm=(((gm1*(1+(Lambda*BP(n3,n4))))/(1+((Delta*BP(n2,n4))*Ids1)))*(1+(((Delta*BP(n3,n4))*Ids1)/(1+((Delta*BP(n3,n4))*Ids1)))));
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*Gdsnoi;
thermal_pwr_Vn3_n4=((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*Gdsnoi;
#endif
thermal_pwr=(((((8*1.3806503e-23)*T2)*gm)/3)*Gdsnoi);
}
_save_whitenoise2(n3,n4,thermal_pwr,"thermal");
{
double m00_pow(d00_pow0,Ids,Af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ids,Af)
#endif
#if defined(_DERIVATE)
flicker_pwr_Vn2_n4=(Kf*(d10_pow0*Ids_Vn2_n4));
flicker_pwr_Vn3_n4=(Kf*(d10_pow0*Ids_Vn3_n4));
#endif
flicker_pwr=(Kf*d00_pow0);
}
_save_flickernoise2(n3,n4,flicker_pwr,1.0,"flicker");
}
if
((LEVEL==5))
{
if
((BP(n3,n4)<(3/Alpha)))
{
#if defined(_DERIVATE)
Nst_Vn3_n4=(Nd);
#endif
Nst=(Ng+(Nd*BP(n3,n4)));
if
((Nst<1.0))
{
#if defined(_DERIVATE)
Nst_Vn3_n4=0.0;
#endif
Nst=1.0;
}
#if defined(_DERIVATE)
Vst_Vn3_n4=Nst_Vn3_n4*Vt_T2;
#endif
Vst=(Nst*Vt_T2);
{
double m00_exp(d00_exp0,(((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))/(Qp*Vst)))
double m00_logE(d00_logE1,(d00_exp0+1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(d00_exp0+1))
#endif
#if defined(_DERIVATE)
Vg_Vn3_n4=(((Qp*Vst_Vn3_n4)*d00_logE1)+((Qp*Vst)*(((Gamma_T2)*(Qp*Vst)-((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))*(Qp*Vst_Vn3_n4))/(Qp*Vst)/(Qp*Vst))*d10_exp0*d10_logE1));
Vg_Vn2_n4=((Qp*Vst)*(1/(Qp*Vst))*d10_exp0*d10_logE1);
#endif
Vg=((Qp*Vst)*d00_logE1);
}
#if defined(_DERIVATE)
Al_Vn3_n4=(Alpha_T2);
#endif
Al=(Alpha_T2*BP(n3,n4));
{
double m00_sqrt(d00_sqrt0,(1.0+(Al*Al)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+(Al*Al)))
#endif
#if defined(_DERIVATE)
Fd_Vn3_n4=((Al_Vn3_n4*d00_sqrt0-Al*((Al_Vn3_n4*Al)+(Al*Al_Vn3_n4))*d10_sqrt0)/d00_sqrt0/d00_sqrt0);
#endif
Fd=(Al/d00_sqrt0);
}
{
double m00_pow(d00_pow0,Vg,Qp)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Vg,Qp)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*Vg_Vn2_n4))*Fd;
Ids1_Vn3_n4=(((Beta_T2*(d10_pow0*Vg_Vn3_n4))*Fd)+((Beta_T2*d00_pow0)*Fd_Vn3_n4));
#endif
Ids1=((Beta_T2*d00_pow0)*Fd);
}
{
double m00_exp(d00_exp0,(-(((BP(n2,n4)-Vto_T2)+(Delta*BP(n3,n4)))/(Qp*Vst))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
gm1_Vn2_n4=((((Ids1_Vn2_n4*Vg-Ids1*Vg_Vn2_n4)/Vg/Vg)*Qp*(d00_exp0+1)-((Ids1/Vg)*Qp)*(-(1/(Qp*Vst)))*d10_exp0)/(d00_exp0+1)/(d00_exp0+1));
gm1_Vn3_n4=((((Ids1_Vn3_n4*Vg-Ids1*Vg_Vn3_n4)/Vg/Vg)*Qp*(d00_exp0+1)-((Ids1/Vg)*Qp)*(-(((Delta)*(Qp*Vst)-((BP(n2,n4)-Vto_T2)+(Delta*BP(n3,n4)))*(Qp*Vst_Vn3_n4))/(Qp*Vst)/(Qp*Vst)))*d10_exp0)/(d00_exp0+1)/(d00_exp0+1));
#endif
gm1=(((Ids1/Vg)*Qp)/(d00_exp0+1));
}
{
double m00_pow(d00_pow0,(1+((Delta*BP(n3,n4))*Ids1)),2)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1+((Delta*BP(n3,n4))*Ids1)),2)
#endif
#if defined(_DERIVATE)
gm_Vn2_n4=((gm1_Vn2_n4*d00_pow0-gm1*(d10_pow0*((Delta*BP(n3,n4))*Ids1_Vn2_n4)))/d00_pow0/d00_pow0);
gm_Vn3_n4=((gm1_Vn3_n4*d00_pow0-gm1*(d10_pow0*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4))))/d00_pow0/d00_pow0);
#endif
gm=(gm1/d00_pow0);
}
#if defined(_DERIVATE)
An_Vn3_n4=(-(1/(BP(n2,n4)-Vto_T2)));
An_Vn2_n4=(-(-BP(n3,n4)/(BP(n2,n4)-Vto_T2)/(BP(n2,n4)-Vto_T2)));
#endif
An=(1-(BP(n3,n4)/(BP(n2,n4)-Vto_T2)));
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn2_n4+((An_Vn2_n4*An)+(An*An_Vn2_n4)))*(1+An)-((1+An)+(An*An))*An_Vn2_n4)/(1+An)/(1+An))))*Gdsnoi;
thermal_pwr_Vn3_n4=((((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*(((1+An)+(An*An))/(1+An)))+(((((8*1.3806503e-23)*T2)*gm)/3)*(((An_Vn3_n4+((An_Vn3_n4*An)+(An*An_Vn3_n4)))*(1+An)-((1+An)+(An*An))*An_Vn3_n4)/(1+An)/(1+An))))*Gdsnoi;
#endif
thermal_pwr=((((((8*1.3806503e-23)*T2)*gm)/3)*(((1+An)+(An*An))/(1+An)))*Gdsnoi);
}
else
{
#if defined(_DERIVATE)
Nst_Vn3_n4=(Nd);
#endif
Nst=(Ng+(Nd*BP(n3,n4)));
if
((Nst<1.0))
{
#if defined(_DERIVATE)
Nst_Vn3_n4=0.0;
#endif
Nst=1.0;
}
#if defined(_DERIVATE)
Vst_Vn3_n4=Nst_Vn3_n4*Vt_T2;
#endif
Vst=(Nst*Vt_T2);
{
double m00_exp(d00_exp0,(((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))/(Qp*Vst)))
double m00_logE(d00_logE1,(d00_exp0+1))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
double m10_logE(d10_logE1,d00_logE1,(d00_exp0+1))
#endif
#if defined(_DERIVATE)
Vg_Vn3_n4=(((Qp*Vst_Vn3_n4)*d00_logE1)+((Qp*Vst)*(((Gamma_T2)*(Qp*Vst)-((BP(n2,n4)-Vto_T2)+(Gamma_T2*BP(n3,n4)))*(Qp*Vst_Vn3_n4))/(Qp*Vst)/(Qp*Vst))*d10_exp0*d10_logE1));
Vg_Vn2_n4=((Qp*Vst)*(1/(Qp*Vst))*d10_exp0*d10_logE1);
#endif
Vg=((Qp*Vst)*d00_logE1);
}
#if defined(_DERIVATE)
Al_Vn3_n4=(Alpha_T2);
#endif
Al=(Alpha_T2*BP(n3,n4));
{
double m00_sqrt(d00_sqrt0,(1.0+(Al*Al)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1.0+(Al*Al)))
#endif
#if defined(_DERIVATE)
Fd_Vn3_n4=((Al_Vn3_n4*d00_sqrt0-Al*((Al_Vn3_n4*Al)+(Al*Al_Vn3_n4))*d10_sqrt0)/d00_sqrt0/d00_sqrt0);
#endif
Fd=(Al/d00_sqrt0);
}
{
double m00_pow(d00_pow0,Vg,Qp)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Vg,Qp)
#endif
#if defined(_DERIVATE)
Ids1_Vn2_n4=(Beta_T2*(d10_pow0*Vg_Vn2_n4))*Fd;
Ids1_Vn3_n4=(((Beta_T2*(d10_pow0*Vg_Vn3_n4))*Fd)+((Beta_T2*d00_pow0)*Fd_Vn3_n4));
#endif
Ids1=((Beta_T2*d00_pow0)*Fd);
}
{
double m00_exp(d00_exp0,(-(((BP(n2,n4)-Vto_T2)+(Delta*BP(n3,n4)))/(Qp*Vst))))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
gm1_Vn2_n4=((((Ids1_Vn2_n4*Vg-Ids1*Vg_Vn2_n4)/Vg/Vg)*Qp*(d00_exp0+1)-((Ids1/Vg)*Qp)*(-(1/(Qp*Vst)))*d10_exp0)/(d00_exp0+1)/(d00_exp0+1));
gm1_Vn3_n4=((((Ids1_Vn3_n4*Vg-Ids1*Vg_Vn3_n4)/Vg/Vg)*Qp*(d00_exp0+1)-((Ids1/Vg)*Qp)*(-(((Delta)*(Qp*Vst)-((BP(n2,n4)-Vto_T2)+(Delta*BP(n3,n4)))*(Qp*Vst_Vn3_n4))/(Qp*Vst)/(Qp*Vst)))*d10_exp0)/(d00_exp0+1)/(d00_exp0+1));
#endif
gm1=(((Ids1/Vg)*Qp)/(d00_exp0+1));
}
{
double m00_pow(d00_pow0,(1+((Delta*BP(n3,n4))*Ids1)),2)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1+((Delta*BP(n3,n4))*Ids1)),2)
#endif
#if defined(_DERIVATE)
gm_Vn2_n4=((gm1_Vn2_n4*d00_pow0-gm1*(d10_pow0*((Delta*BP(n3,n4))*Ids1_Vn2_n4)))/d00_pow0/d00_pow0);
gm_Vn3_n4=((gm1_Vn3_n4*d00_pow0-gm1*(d10_pow0*(((Delta)*Ids1)+((Delta*BP(n3,n4))*Ids1_Vn3_n4))))/d00_pow0/d00_pow0);
#endif
gm=(gm1/d00_pow0);
}
#if defined(_DERIVATE)
thermal_pwr_Vn2_n4=((((8*1.3806503e-23)*T2)*gm_Vn2_n4)/3)*Gdsnoi;
thermal_pwr_Vn3_n4=((((8*1.3806503e-23)*T2)*gm_Vn3_n4)/3)*Gdsnoi;
#endif
thermal_pwr=(((((8*1.3806503e-23)*T2)*gm)/3)*Gdsnoi);
}
_save_whitenoise2(n3,n4,thermal_pwr,"thermal");
{
double m00_pow(d00_pow0,Ids,Af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Ids,Af)
#endif
#if defined(_DERIVATE)
flicker_pwr_Vn2_n4=(Kf*(d10_pow0*Ids_Vn2_n4));
flicker_pwr_Vn3_n4=(Kf*(d10_pow0*Ids_Vn3_n4));
#endif
flicker_pwr=(Kf*d00_pow0);
}
_save_flickernoise2(n3,n4,flicker_pwr,1.0,"flicker");
}
_save_whitenoise2(Drain,n3,((Area*fourkt)/Rd_T2),"thermal");
_save_whitenoise2(Gate,n2,((Area*fourkt)/Rg_T2),"thermal");
_save_whitenoise2(n4,Source,((Area*fourkt)/Rs_T2),"thermal");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void MESFET::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 7; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 7; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void MESFET::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void MESFET::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void MESFET::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void MESFET::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void MESFET::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix MESFET::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (7);

  for (int i1 = 0; i1 < 7; i1++) {
    for (int i2 = 0; i2 < 7; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void MESFET::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void MESFET::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void MESFET::initTR (void)
{
  setStates (2 * 7 * 7);
  initDC ();
}

/* Perform transient analysis iteration step. */
void MESFET::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 7; i1++) {
  for (i2 = 0; i2 < 7; i2++) {
    state = 2 * (i2 + 7 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 7; i1++) {
    state = 2 * (i1 + 7 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 7; i1++) {
  for (i2 = 0; i2 < 7; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 7; i3++) {
  for (i4 = 0; i4 < 7; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 7; i1++) {
  for (i2 = 0; i2 < 7; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 7; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 7; i1++) {
  for (i3 = 0; i3 < 7; i3++) {
  for (i4 = 0; i4 < 7; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 7; i1++) {
  for (i3 = 0; i3 < 7; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix MESFET::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (7);

  _load_flickernoise2 (n3,n4,_flicker_pwr[n3][n4],_flicker_exp[n3][n4]);
  _load_whitenoise2 (n4,Source,_white_pwr[n4][Source]);
  _load_whitenoise2 (Drain,n3,_white_pwr[Drain][n3]);
  _load_whitenoise2 (Gate,n2,_white_pwr[Gate][n2]);
  _load_whitenoise2 (n3,n4,_white_pwr[n3][n4]);

  return cy;
}

/* Perform AC noise computations. */
void MESFET::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void MESFET::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void MESFET::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void MESFET::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 7; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 7; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "MESFET.defs.h"
