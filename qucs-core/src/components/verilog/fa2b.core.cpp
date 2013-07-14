/*
 * fa2b.core.cpp - device implementations for fa2b module
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

#include "fa2b.analogfunction.h"
#include "component.h"
#include "device.h"
#include "fa2b.core.h"

#ifndef CIR_fa2b
#define CIR_fa2b -1
#endif

// external nodes
#define D 0
#define C 1
#define B 2
#define A 3
#define CI 4
#define CO 5
#define S1 6
#define S0 7
// internal nodes
#define S0n1 8
#define S0n2 9
#define S1n1 10
#define S1n2 11
#define COn1 12
#define COn2 13

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
#define _modelname              "fa2b"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
fa2b::fa2b() : circuit (14)
{
  type = CIR_fa2b;
}

/* Initialization of model. */
void fa2b::initModel (void)
{
  // create internal nodes
  setInternalNode (S0n1, "S0n1");
  setInternalNode (S0n2, "S0n2");
  setInternalNode (S1n1, "S1n1");
  setInternalNode (S1n2, "S1n2");
  setInternalNode (COn1, "COn1");
  setInternalNode (COn2, "COn2");

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
void fa2b::initDC (void)
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
void fa2b::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void fa2b::initVerilog (void)
{
  // initialization of noise variables

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 14; i1++) {
  for (i2 = 0; i2 < 14; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 14; i1++) {
  for (i2 = 0; i2 < 14; i2++) {
  for (i3 = 0; i3 < 14; i3++) {
  for (i4 = 0; i4 < 14; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 14; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 14; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void fa2b::loadVariables (void)
{
  TR = getPropertyDouble ("TR");
  Delay = getPropertyDouble ("Delay");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void fa2b::initializeModel (void)
{
#if defined(_DYNAMIC)
#endif
{
Rd=1e3;
#if defined(_DYNAMIC)
Cd=((Delay*1.43)/Rd);
#endif
}
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void fa2b::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void fa2b::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void fa2b::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void fa2b::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double mCO;
#if defined(_DERIVATE)
double mCO_VA_GND;
double mCO_VC_GND;
double mCO_VB_GND;
double mCO_VD_GND;
double mCO_VCI_GND;
#endif
double mS1;
#if defined(_DERIVATE)
double mS1_VB_GND;
double mS1_VD_GND;
double mS1_VCI_GND;
double mS1_VA_GND;
double mS1_VC_GND;
#endif
double mS1b;
#if defined(_DERIVATE)
double mS1b_VA_GND;
double mS1b_VC_GND;
#endif
double mS1a;
#if defined(_DERIVATE)
double mS1a_VB_GND;
double mS1a_VD_GND;
double mS1a_VCI_GND;
#endif
double mS0;
#if defined(_DERIVATE)
double mS0_VCI_GND;
double mS0_VB_GND;
double mS0_VD_GND;
#endif
double mS0a;
#if defined(_DERIVATE)
double mS0a_VB_GND;
double mS0a_VD_GND;
#endif
#if defined(_DERIVATE)
mS0a_VB_GND=(((1-NP(D)))+(-1.0)*NP(D));
mS0a_VD_GND=((NP(B)*(-1.0))+((1-NP(B))));
#endif
mS0a=((NP(B)*(1-NP(D)))+((1-NP(B))*NP(D)));
#if defined(_DERIVATE)
mS0_VCI_GND=(((1-mS0a))+(-1.0)*mS0a);
mS0_VB_GND=((NP(CI)*(-mS0a_VB_GND))+((1-NP(CI))*mS0a_VB_GND));
mS0_VD_GND=((NP(CI)*(-mS0a_VD_GND))+((1-NP(CI))*mS0a_VD_GND));
#endif
mS0=((NP(CI)*(1-mS0a))+((1-NP(CI))*mS0a));
#if defined(_DERIVATE)
mS1a_VB_GND=((NP(D))+(NP(CI)));
mS1a_VD_GND=((NP(B))+(NP(CI)));
mS1a_VCI_GND=((NP(B))+(NP(D)));
#endif
mS1a=(((NP(B)*NP(D))+(NP(CI)*NP(B)))+(NP(CI)*NP(D)));
#if defined(_DERIVATE)
mS1b_VA_GND=(((1-NP(C)))+(-1.0)*NP(C));
mS1b_VC_GND=((NP(A)*(-1.0))+((1-NP(A))));
#endif
mS1b=((NP(A)*(1-NP(C)))+((1-NP(A))*NP(C)));
#if defined(_DERIVATE)
mS1_VB_GND=(mS1a_VB_GND*(1-mS1b)+(-mS1a_VB_GND)*mS1b);
mS1_VD_GND=(mS1a_VD_GND*(1-mS1b)+(-mS1a_VD_GND)*mS1b);
mS1_VCI_GND=(mS1a_VCI_GND*(1-mS1b)+(-mS1a_VCI_GND)*mS1b);
mS1_VA_GND=((mS1a*(-mS1b_VA_GND))+((1-mS1a)*mS1b_VA_GND));
mS1_VC_GND=((mS1a*(-mS1b_VC_GND))+((1-mS1a)*mS1b_VC_GND));
#endif
mS1=((mS1a*(1-mS1b))+((1-mS1a)*mS1b));
#if defined(_DERIVATE)
mCO_VA_GND=((NP(C))+(mS1a));
mCO_VC_GND=((NP(A))+(mS1a));
mCO_VB_GND=((NP(A)+NP(C))*mS1a_VB_GND);
mCO_VD_GND=((NP(A)+NP(C))*mS1a_VD_GND);
mCO_VCI_GND=((NP(A)+NP(C))*mS1a_VCI_GND);
#endif
mCO=((NP(A)*NP(C))+((NP(A)+NP(C))*mS1a));
if
((mS0>=0.5))
{
#if defined(_DERIVATE)
mS0_VCI_GND=0.0;
mS0_VB_GND=0.0;
mS0_VD_GND=0.0;
#endif
mS0=1;
}
else
{
#if defined(_DERIVATE)
mS0_VCI_GND=0.0;
mS0_VB_GND=0.0;
mS0_VD_GND=0.0;
#endif
mS0=0;
}
if
((mS1>=0.5))
{
#if defined(_DERIVATE)
mS1_VB_GND=0.0;
mS1_VD_GND=0.0;
mS1_VCI_GND=0.0;
mS1_VA_GND=0.0;
mS1_VC_GND=0.0;
#endif
mS1=1;
}
else
{
#if defined(_DERIVATE)
mS1_VB_GND=0.0;
mS1_VD_GND=0.0;
mS1_VCI_GND=0.0;
mS1_VA_GND=0.0;
mS1_VC_GND=0.0;
#endif
mS1=0;
}
if
((mCO>=0.5))
{
#if defined(_DERIVATE)
mCO_VA_GND=0.0;
mCO_VC_GND=0.0;
mCO_VB_GND=0.0;
mCO_VD_GND=0.0;
mCO_VCI_GND=0.0;
#endif
mCO=1;
}
else
{
#if defined(_DERIVATE)
mCO_VA_GND=0.0;
mCO_VC_GND=0.0;
mCO_VB_GND=0.0;
mCO_VD_GND=0.0;
mCO_VCI_GND=0.0;
#endif
mCO=0;
}
{
double m00_tanh(d00_tanh0,(TR*(mS0-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mS0-0.5)))
#endif
_load_static_residual1(S0n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(S0n1,D,((-0.5)*(TR*mS0_VD_GND)*d10_tanh0));
_load_static_jacobian1(S0n1,B,((-0.5)*(TR*mS0_VB_GND)*d10_tanh0));
_load_static_jacobian1(S0n1,CI,((-0.5)*(TR*mS0_VCI_GND)*d10_tanh0));
#endif
}
_load_static_residual1(S0n1,NP(S0n1));
#if defined(_DERIVATE)
_load_static_jacobian1(S0n1,S0n1,1.0);
#endif
_load_static_residual2(S0n1,S0n2,(BP(S0n1,S0n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(S0n1,S0n2,S0n1,S0n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(S0n2,_DDT((Cd*NP(S0n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(S0n2,S0n2,(Cd));
#endif
#endif
_load_static_residual1(S0,(-NP(S0n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(S0,S0n2,(-1.0));
#endif
_load_static_residual1(S0,NP(S0));
#if defined(_DERIVATE)
_load_static_jacobian1(S0,S0,1.0);
#endif
{
double m00_tanh(d00_tanh0,(TR*(mS1-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mS1-0.5)))
#endif
_load_static_residual1(S1n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(S1n1,C,((-0.5)*(TR*mS1_VC_GND)*d10_tanh0));
_load_static_jacobian1(S1n1,A,((-0.5)*(TR*mS1_VA_GND)*d10_tanh0));
_load_static_jacobian1(S1n1,CI,((-0.5)*(TR*mS1_VCI_GND)*d10_tanh0));
_load_static_jacobian1(S1n1,D,((-0.5)*(TR*mS1_VD_GND)*d10_tanh0));
_load_static_jacobian1(S1n1,B,((-0.5)*(TR*mS1_VB_GND)*d10_tanh0));
#endif
}
_load_static_residual1(S1n1,NP(S1n1));
#if defined(_DERIVATE)
_load_static_jacobian1(S1n1,S1n1,1.0);
#endif
_load_static_residual2(S1n1,S1n2,(BP(S1n1,S1n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(S1n1,S1n2,S1n1,S1n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(S1n2,_DDT((Cd*NP(S1n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(S1n2,S1n2,(Cd));
#endif
#endif
_load_static_residual1(S1,(-NP(S1n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(S1,S1n2,(-1.0));
#endif
_load_static_residual1(S1,NP(S1));
#if defined(_DERIVATE)
_load_static_jacobian1(S1,S1,1.0);
#endif
{
double m00_tanh(d00_tanh0,(TR*(mCO-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mCO-0.5)))
#endif
_load_static_residual1(COn1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(COn1,CI,((-0.5)*(TR*mCO_VCI_GND)*d10_tanh0));
_load_static_jacobian1(COn1,D,((-0.5)*(TR*mCO_VD_GND)*d10_tanh0));
_load_static_jacobian1(COn1,B,((-0.5)*(TR*mCO_VB_GND)*d10_tanh0));
_load_static_jacobian1(COn1,C,((-0.5)*(TR*mCO_VC_GND)*d10_tanh0));
_load_static_jacobian1(COn1,A,((-0.5)*(TR*mCO_VA_GND)*d10_tanh0));
#endif
}
_load_static_residual1(COn1,NP(COn1));
#if defined(_DERIVATE)
_load_static_jacobian1(COn1,COn1,1.0);
#endif
_load_static_residual2(COn1,COn2,(BP(COn1,COn2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(COn1,COn2,COn1,COn2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(COn2,_DDT((Cd*NP(COn2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(COn2,COn2,(Cd));
#endif
#endif
_load_static_residual1(CO,(-NP(COn2)));
#if defined(_DERIVATE)
_load_static_jacobian1(CO,COn2,(-1.0));
#endif
_load_static_residual1(CO,NP(CO));
#if defined(_DERIVATE)
_load_static_jacobian1(CO,CO,1.0);
#endif

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void fa2b::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 14; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 14; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void fa2b::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void fa2b::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void fa2b::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void fa2b::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void fa2b::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix fa2b::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (14);

  for (int i1 = 0; i1 < 14; i1++) {
    for (int i2 = 0; i2 < 14; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void fa2b::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void fa2b::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void fa2b::initTR (void)
{
  setStates (2 * 14 * 14);
  initDC ();
}

/* Perform transient analysis iteration step. */
void fa2b::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 14; i1++) {
  for (i2 = 0; i2 < 14; i2++) {
    state = 2 * (i2 + 14 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 14; i1++) {
    state = 2 * (i1 + 14 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 14; i1++) {
  for (i2 = 0; i2 < 14; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 14; i3++) {
  for (i4 = 0; i4 < 14; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 14; i1++) {
  for (i2 = 0; i2 < 14; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 14; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 14; i1++) {
  for (i3 = 0; i3 < 14; i3++) {
  for (i4 = 0; i4 < 14; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 14; i1++) {
  for (i3 = 0; i3 < 14; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix fa2b::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (14);


  return cy;
}

/* Perform AC noise computations. */
void fa2b::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void fa2b::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void fa2b::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void fa2b::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 14; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 14; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "fa2b.defs.h"
