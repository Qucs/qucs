/*
 * phototransistor.core.cpp - device implementations for phototransistor module
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

#include "phototransistor.analogfunction.h"
#include "component.h"
#include "device.h"
#include "phototransistor.core.h"

#ifndef CIR_phototransistor
#define CIR_phototransistor -1
#endif

// external nodes
#define Collector 0
#define Base 1
#define Emitter 2
#define Power 3
#define Wavelength 4
// internal nodes
#define CI 5
#define BI 6
#define BI2 7
#define EI 8

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
#define _modelname              "phototransistor"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
phototransistor::phototransistor() : circuit (9)
{
  type = CIR_phototransistor;
}

/* Initialization of model. */
void phototransistor::initModel (void)
{
  // create internal nodes
  setInternalNode (CI, "CI");
  setInternalNode (BI, "BI");
  setInternalNode (BI2, "BI2");
  setInternalNode (EI, "EI");

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
void phototransistor::initDC (void)
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
void phototransistor::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void phototransistor::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[CI][EI] = 0.0;
  _white_pwr[BI][EI] = 0.0;
  _white_pwr[EI][Emitter] = 0.0;
  _white_pwr[BI2][BI] = 0.0;
  _white_pwr[Base][BI2] = 0.0;
  _white_pwr[Collector][CI] = 0.0;
  _flicker_pwr[BI][EI] = 0.0;
  _flicker_exp[BI][EI] = 0.0;

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
void phototransistor::loadVariables (void)
{
  Bf = getPropertyDouble ("Bf");
  Br = getPropertyDouble ("Br");
  Is = getPropertyDouble ("Is");
  Nf = getPropertyDouble ("Nf");
  Nr = getPropertyDouble ("Nr");
  Vaf = getPropertyDouble ("Vaf");
  Var = getPropertyDouble ("Var");
  Mje = getPropertyDouble ("Mje");
  Vje = getPropertyDouble ("Vje");
  Cje = getPropertyDouble ("Cje");
  Mjc = getPropertyDouble ("Mjc");
  Vjc = getPropertyDouble ("Vjc");
  Cjc = getPropertyDouble ("Cjc");
  Tr = getPropertyDouble ("Tr");
  Tf = getPropertyDouble ("Tf");
  Ikf = getPropertyDouble ("Ikf");
  Ikr = getPropertyDouble ("Ikr");
  Rc = getPropertyDouble ("Rc");
  Re = getPropertyDouble ("Re");
  Rb = getPropertyDouble ("Rb");
  Kf = getPropertyDouble ("Kf");
  Ffe = getPropertyDouble ("Ffe");
  Af = getPropertyDouble ("Af");
  Responsivity = getPropertyDouble ("Responsivity");
  P0 = getPropertyDouble ("P0");
  P1 = getPropertyDouble ("P1");
  P2 = getPropertyDouble ("P2");
  P3 = getPropertyDouble ("P3");
  P4 = getPropertyDouble ("P4");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void phototransistor::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void phototransistor::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void phototransistor::initialStep (void)
{
double con7;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
double VT;
{
VT=((1.3806503e-23*300)/1.602176462e-19);
con1=(1/(Nf*VT));
con2=(1/(Nr*VT));
#if defined(_DYNAMIC)
con3=(1-Mje);
#endif
#if defined(_DYNAMIC)
con4=(1-Mjc);
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,2,Mje)
con5=d00_pow0;
}
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,2,Mjc)
con6=d00_pow0;
}
#endif
#if defined(_DYNAMIC)
con7=(Rb/2);
#endif
con8=(2/Rb);
con9=(1/Rc);
con10=(1/Re);
TwoQ=(2*1.602176462e-19);
FourKT=((4*1.3806503e-23)*_circuit_temp);
GMIN=1e-12;
}
}

/* Evaluate Verilog-AMS equations in final step. */
void phototransistor::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void phototransistor::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double RelSensitivity;
#if defined(_DERIVATE)
double RelSensitivity_VWavelength_GND;
#endif
#if defined(_DYNAMIC)
double Q1;
#if defined(_DERIVATE)
double Q1_VBI_EI;
double Q1_VBI_CI;
#endif
#endif
double IE;
#if defined(_DERIVATE)
double IE_VEI_Emitter;
#endif
double IC;
#if defined(_DERIVATE)
double IC_VCollector_CI;
#endif
double IB;
#if defined(_DERIVATE)
double IB_VBI2_BI;
#endif
double q2;
#if defined(_DERIVATE)
double q2_VBI_EI;
double q2_VBI_CI;
#endif
double q1;
#if defined(_DERIVATE)
double q1_VBI_CI;
double q1_VBI_EI;
#endif
double IEC;
#if defined(_DERIVATE)
double IEC_VBI_CI;
#endif
double ICC;
#if defined(_DERIVATE)
double ICC_VBI_EI;
#endif
{
double m00_limexp(d00_limexp0,(BP(BI,EI)*con1))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(BI,EI)*con1))
#endif
#if defined(_DERIVATE)
ICC_VBI_EI=(Is*(con1)*d10_limexp0);
#endif
ICC=(Is*(d00_limexp0-1));
}
{
double m00_limexp(d00_limexp0,(BP(BI,CI)*con2))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(BP(BI,CI)*con2))
#endif
#if defined(_DERIVATE)
IEC_VBI_CI=(Is*(con2)*d10_limexp0);
#endif
IEC=(Is*(d00_limexp0-1));
}
#if defined(_DERIVATE)
q1_VBI_CI=(1/Vaf);
q1_VBI_EI=(1/Var);
#endif
q1=((1+(BP(BI,CI)/Vaf))+(BP(BI,EI)/Var));
#if defined(_DERIVATE)
q2_VBI_EI=(ICC_VBI_EI/Ikf);
q2_VBI_CI=(IEC_VBI_CI/Ikr);
#endif
q2=((ICC/Ikf)+(IEC/Ikr));
#if defined(_DERIVATE)
IB_VBI2_BI=(con8);
#endif
IB=(BP(BI2,BI)*con8);
#if defined(_DERIVATE)
IC_VCollector_CI=(con9);
#endif
IC=(BP(Collector,CI)*con9);
#if defined(_DERIVATE)
IE_VEI_Emitter=(con10);
#endif
IE=(BP(EI,Emitter)*con10);
_load_static_residual2(Collector,CI,IC);
#if defined(_DERIVATE)
_load_static_jacobian4(Collector,CI,Collector,CI,IC_VCollector_CI);
#endif
_load_static_residual2(Base,BI2,(BP(Base,BI2)*con8));
#if defined(_DERIVATE)
_load_static_jacobian4(Base,BI2,Base,BI2,(con8));
#endif
_load_static_residual2(BI2,BI,IB);
#if defined(_DERIVATE)
_load_static_jacobian4(BI2,BI,BI2,BI,IB_VBI2_BI);
#endif
_load_static_residual2(EI,Emitter,IE);
#if defined(_DERIVATE)
_load_static_jacobian4(EI,Emitter,EI,Emitter,IE_VEI_Emitter);
#endif
_load_static_residual2(BI,CI,((IEC/Br)+(GMIN*BP(BI,CI))));
#if defined(_DERIVATE)
_load_static_jacobian4(BI,CI,BI,CI,((IEC_VBI_CI/Br)+(GMIN)));
#endif
_load_static_residual2(BI,EI,((ICC/Bf)+(GMIN*BP(BI,EI))));
#if defined(_DERIVATE)
_load_static_jacobian4(BI,EI,BI,EI,((ICC_VBI_EI/Bf)+(GMIN)));
#endif
{
double m00_sqrt(d00_sqrt0,(1+(4*q2)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(1+(4*q2)))
#endif
_load_static_residual2(CI,EI,((ICC-IEC)/(1e-20+((q1/2)*(1+d00_sqrt0)))));
#if defined(_DERIVATE)
_load_static_jacobian4(CI,EI,BI,CI,(((-IEC_VBI_CI)*(1e-20+((q1/2)*(1+d00_sqrt0)))-(ICC-IEC)*(((q1_VBI_CI/2)*(1+d00_sqrt0))+((q1/2)*(4*q2_VBI_CI)*d10_sqrt0)))/(1e-20+((q1/2)*(1+d00_sqrt0)))/(1e-20+((q1/2)*(1+d00_sqrt0)))));
_load_static_jacobian4(CI,EI,BI,EI,((ICC_VBI_EI*(1e-20+((q1/2)*(1+d00_sqrt0)))-(ICC-IEC)*(((q1_VBI_EI/2)*(1+d00_sqrt0))+((q1/2)*(4*q2_VBI_EI)*d10_sqrt0)))/(1e-20+((q1/2)*(1+d00_sqrt0)))/(1e-20+((q1/2)*(1+d00_sqrt0)))));
#endif
}
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-BP(BI,CI)),con4)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-BP(BI,CI)),con4)
#endif
#if defined(_DERIVATE)
Q1_VBI_EI=0.0;
Q1_VBI_CI=((BP(BI,CI)>(Vjc/2))?((Tr*IEC_VBI_CI)+((Cjc*con6)*(((((Mjc)*BP(BI,CI))+(Mjc*BP(BI,CI)))/Vjc)+(con4)))):((Tr*IEC_VBI_CI)+(Cjc*((Vjc/con4)*(-(d10_pow0*(-1.0)))))));
#endif
Q1=((BP(BI,CI)>(Vjc/2))?((Tr*IEC)+((Cjc*con6)*((((Mjc*BP(BI,CI))*BP(BI,CI))/Vjc)+(con4*BP(BI,CI))))):((Tr*IEC)+(Cjc*((Vjc/con4)*(1-d00_pow0)))));
}
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(BI,CI,_DDT(Q1));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(BI,CI,BI,CI,Q1_VBI_CI);
#endif
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-BP(BI,EI)),con3)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-BP(BI,EI)),con3)
#endif
#if defined(_DERIVATE)
Q1_VBI_EI=((BP(BI,EI)>(Vje/2))?((Tf*ICC_VBI_EI)+((Cje*con5)*(((((Mje)*BP(BI,EI))+(Mje*BP(BI,EI)))/Vje)+(con3)))):((Tf*ICC_VBI_EI)+(Cje*((Vje/con3)*(-(d10_pow0*(-1.0)))))));
Q1_VBI_CI=0.0;
#endif
Q1=((BP(BI,EI)>(Vje/2))?((Tf*ICC)+((Cje*con5)*((((Mje*BP(BI,EI))*BP(BI,EI))/Vje)+(con3*BP(BI,EI))))):((Tf*ICC)+(Cje*((Vje/con3)*(1-d00_pow0)))));
}
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(BI,EI,_DDT(Q1));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(BI,EI,BI,CI,Q1_VBI_CI);
_load_dynamic_jacobian4(BI,EI,BI,EI,Q1_VBI_EI);
#endif
#endif
{
double m00_pow(d00_pow0,NP(Wavelength),2)
double m00_pow(d00_pow1,NP(Wavelength),3)
double m00_pow(d00_pow2,NP(Wavelength),4)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,NP(Wavelength),2)
double m10_pow(d10_pow1,d00_pow1,NP(Wavelength),3)
double m10_pow(d10_pow2,d00_pow2,NP(Wavelength),4)
#endif
#if defined(_DERIVATE)
RelSensitivity_VWavelength_GND=((((P1)+(P2*(d10_pow0*1.0)))+(P3*(d10_pow1*1.0)))+(P4*(d10_pow2*1.0)));
#endif
RelSensitivity=((((P0+(P1*NP(Wavelength)))+(P2*d00_pow0))+(P3*d00_pow1))+(P4*d00_pow2));
}
_load_static_residual2(CI,BI2,(((Responsivity*RelSensitivity)/(Bf*100))*NP(Power)));
#if defined(_DERIVATE)
_load_static_jacobian2s(CI,BI2,Power,(((Responsivity*RelSensitivity)/(Bf*100))));
_load_static_jacobian2s(CI,BI2,Wavelength,((Responsivity*RelSensitivity_VWavelength_GND)/(Bf*100))*NP(Power));
#endif
_save_whitenoise2(Collector,CI,(FourKT*con9),"thermal");
_save_whitenoise2(Base,BI2,(FourKT*con8),"thermal");
_save_whitenoise2(BI2,BI,(FourKT*con8),"thermal");
_save_whitenoise2(EI,Emitter,(FourKT*con10),"thermal");
_save_whitenoise2(CI,EI,(TwoQ*IC),"shot");
_save_whitenoise2(BI,EI,(TwoQ*IB),"shot");
_save_flickernoise2(BI,EI,(Kf*pow(IB,Af)),Ffe,"flicker");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void phototransistor::calcDC (void)
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
void phototransistor::saveOperatingPoints (void)
{
  // save global instance operating points
  setOperatingPoint ("con1", con1);
  setOperatingPoint ("con2", con2);
  setOperatingPoint ("con3", con3);
  setOperatingPoint ("con4", con4);
  setOperatingPoint ("con5", con5);
  setOperatingPoint ("con6", con6);
  setOperatingPoint ("con8", con8);
  setOperatingPoint ("con9", con9);
  setOperatingPoint ("con10", con10);
  setOperatingPoint ("TwoQ", TwoQ);
  setOperatingPoint ("FourKT", FourKT);
  setOperatingPoint ("GMIN", GMIN);
}

/* Load operating points. */
void phototransistor::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void phototransistor::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void phototransistor::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void phototransistor::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix phototransistor::calcMatrixY (nr_double_t frequency)
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
void phototransistor::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void phototransistor::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void phototransistor::initTR (void)
{
  setStates (2 * 9 * 9);
  initDC ();
}

/* Perform transient analysis iteration step. */
void phototransistor::calcTR (nr_double_t)
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
matrix phototransistor::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (9);

  _load_flickernoise2 (BI,EI,_flicker_pwr[BI][EI],_flicker_exp[BI][EI]);
  _load_whitenoise2 (CI,EI,_white_pwr[CI][EI]);
  _load_whitenoise2 (BI,EI,_white_pwr[BI][EI]);
  _load_whitenoise2 (EI,Emitter,_white_pwr[EI][Emitter]);
  _load_whitenoise2 (BI2,BI,_white_pwr[BI2][BI]);
  _load_whitenoise2 (Base,BI2,_white_pwr[Base][BI2]);
  _load_whitenoise2 (Collector,CI,_white_pwr[Collector][CI]);

  return cy;
}

/* Perform AC noise computations. */
void phototransistor::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void phototransistor::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void phototransistor::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void phototransistor::calcHB (int)
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

#include "phototransistor.defs.h"
