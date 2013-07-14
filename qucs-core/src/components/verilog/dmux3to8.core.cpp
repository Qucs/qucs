/*
 * dmux3to8.core.cpp - device implementations for dmux3to8 module
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

#include "dmux3to8.analogfunction.h"
#include "component.h"
#include "device.h"
#include "dmux3to8.core.h"

#ifndef CIR_dmux3to8
#define CIR_dmux3to8 -1
#endif

// external nodes
#define EN 0
#define A 1
#define B 2
#define C 3
#define Y7 4
#define Y6 5
#define Y5 6
#define Y4 7
#define Y3 8
#define Y2 9
#define Y1 10
#define Y0 11
// internal nodes
#define Y0n1 12
#define Y0n2 13
#define Y1n1 14
#define Y1n2 15
#define Y2n1 16
#define Y2n2 17
#define Y3n1 18
#define Y3n2 19
#define Y4n1 20
#define Y4n2 21
#define Y5n1 22
#define Y5n2 23
#define Y6n1 24
#define Y6n2 25
#define Y7n1 26
#define Y7n2 27

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
#define _modelname              "dmux3to8"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
dmux3to8::dmux3to8() : circuit (28)
{
  type = CIR_dmux3to8;
}

/* Initialization of model. */
void dmux3to8::initModel (void)
{
  // create internal nodes
  setInternalNode (Y0n1, "Y0n1");
  setInternalNode (Y0n2, "Y0n2");
  setInternalNode (Y1n1, "Y1n1");
  setInternalNode (Y1n2, "Y1n2");
  setInternalNode (Y2n1, "Y2n1");
  setInternalNode (Y2n2, "Y2n2");
  setInternalNode (Y3n1, "Y3n1");
  setInternalNode (Y3n2, "Y3n2");
  setInternalNode (Y4n1, "Y4n1");
  setInternalNode (Y4n2, "Y4n2");
  setInternalNode (Y5n1, "Y5n1");
  setInternalNode (Y5n2, "Y5n2");
  setInternalNode (Y6n1, "Y6n1");
  setInternalNode (Y6n2, "Y6n2");
  setInternalNode (Y7n1, "Y7n1");
  setInternalNode (Y7n2, "Y7n2");

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
void dmux3to8::initDC (void)
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
void dmux3to8::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void dmux3to8::initVerilog (void)
{
  // initialization of noise variables

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 28; i1++) {
  for (i2 = 0; i2 < 28; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 28; i1++) {
  for (i2 = 0; i2 < 28; i2++) {
  for (i3 = 0; i3 < 28; i3++) {
  for (i4 = 0; i4 < 28; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 28; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 28; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void dmux3to8::loadVariables (void)
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
void dmux3to8::initializeModel (void)
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
void dmux3to8::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void dmux3to8::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void dmux3to8::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void dmux3to8::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double IY7;
#if defined(_DERIVATE)
double IY7_VEN_GND;
double IY7_VC_GND;
double IY7_VB_GND;
double IY7_VA_GND;
#endif
double IY6;
#if defined(_DERIVATE)
double IY6_VEN_GND;
double IY6_VC_GND;
double IY6_VB_GND;
double IY6_VA_GND;
#endif
double IY5;
#if defined(_DERIVATE)
double IY5_VEN_GND;
double IY5_VC_GND;
double IY5_VB_GND;
double IY5_VA_GND;
#endif
double IY4;
#if defined(_DERIVATE)
double IY4_VEN_GND;
double IY4_VC_GND;
double IY4_VB_GND;
double IY4_VA_GND;
#endif
double IY3;
#if defined(_DERIVATE)
double IY3_VEN_GND;
double IY3_VC_GND;
double IY3_VB_GND;
double IY3_VA_GND;
#endif
double IY2;
#if defined(_DERIVATE)
double IY2_VEN_GND;
double IY2_VC_GND;
double IY2_VB_GND;
double IY2_VA_GND;
#endif
double IY1;
#if defined(_DERIVATE)
double IY1_VEN_GND;
double IY1_VC_GND;
double IY1_VB_GND;
double IY1_VA_GND;
#endif
double IY0;
#if defined(_DERIVATE)
double IY0_VEN_GND;
double IY0_VC_GND;
double IY0_VB_GND;
double IY0_VA_GND;
#endif
double VCI;
#if defined(_DERIVATE)
double VCI_VC_GND;
#endif
double VBI;
#if defined(_DERIVATE)
double VBI_VB_GND;
#endif
double VAI;
#if defined(_DERIVATE)
double VAI_VA_GND;
#endif
double VENI;
#if defined(_DERIVATE)
double VENI_VEN_GND;
#endif
#if defined(_DERIVATE)
VENI_VEN_GND=(-1.0);
#endif
VENI=(1-NP(EN));
#if defined(_DERIVATE)
VAI_VA_GND=(-1.0);
#endif
VAI=(1-NP(A));
#if defined(_DERIVATE)
VBI_VB_GND=(-1.0);
#endif
VBI=(1-NP(B));
#if defined(_DERIVATE)
VCI_VC_GND=(-1.0);
#endif
VCI=(1-NP(C));
#if defined(_DERIVATE)
IY0_VEN_GND=VENI_VEN_GND*VCI*VBI*VAI;
IY0_VC_GND=(VENI*VCI_VC_GND)*VBI*VAI;
IY0_VB_GND=((VENI*VCI)*VBI_VB_GND)*VAI;
IY0_VA_GND=(((VENI*VCI)*VBI)*VAI_VA_GND);
#endif
IY0=(((VENI*VCI)*VBI)*VAI);
{
double m00_tanh(d00_tanh0,(TR*(IY0-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY0-0.5)))
#endif
_load_static_residual1(Y0n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y0n1,A,((-0.5)*(TR*IY0_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y0n1,B,((-0.5)*(TR*IY0_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y0n1,C,((-0.5)*(TR*IY0_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y0n1,EN,((-0.5)*(TR*IY0_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y0n1,NP(Y0n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y0n1,Y0n1,1.0);
#endif
_load_static_residual2(Y0n1,Y0n2,(BP(Y0n1,Y0n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y0n1,Y0n2,Y0n1,Y0n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y0n2,_DDT((Cd*NP(Y0n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y0n2,Y0n2,(Cd));
#endif
#endif
_load_static_residual1(Y0,(-NP(Y0n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y0,Y0n2,(-1.0));
#endif
_load_static_residual1(Y0,NP(Y0));
#if defined(_DERIVATE)
_load_static_jacobian1(Y0,Y0,1.0);
#endif
#if defined(_DERIVATE)
IY1_VEN_GND=VENI_VEN_GND*VCI*VBI*NP(A);
IY1_VC_GND=(VENI*VCI_VC_GND)*VBI*NP(A);
IY1_VB_GND=((VENI*VCI)*VBI_VB_GND)*NP(A);
IY1_VA_GND=(((VENI*VCI)*VBI));
#endif
IY1=(((VENI*VCI)*VBI)*NP(A));
{
double m00_tanh(d00_tanh0,(TR*(IY1-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY1-0.5)))
#endif
_load_static_residual1(Y1n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y1n1,A,((-0.5)*(TR*IY1_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y1n1,B,((-0.5)*(TR*IY1_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y1n1,C,((-0.5)*(TR*IY1_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y1n1,EN,((-0.5)*(TR*IY1_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y1n1,NP(Y1n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y1n1,Y1n1,1.0);
#endif
_load_static_residual2(Y1n1,Y1n2,(BP(Y1n1,Y1n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y1n1,Y1n2,Y1n1,Y1n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y1n2,_DDT((Cd*NP(Y1n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y1n2,Y1n2,(Cd));
#endif
#endif
_load_static_residual1(Y1,(-NP(Y1n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y1,Y1n2,(-1.0));
#endif
_load_static_residual1(Y1,NP(Y1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y1,Y1,1.0);
#endif
#if defined(_DERIVATE)
IY2_VEN_GND=VENI_VEN_GND*VCI*NP(B)*VAI;
IY2_VC_GND=(VENI*VCI_VC_GND)*NP(B)*VAI;
IY2_VB_GND=((VENI*VCI))*VAI;
IY2_VA_GND=(((VENI*VCI)*NP(B))*VAI_VA_GND);
#endif
IY2=(((VENI*VCI)*NP(B))*VAI);
{
double m00_tanh(d00_tanh0,(TR*(IY2-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY2-0.5)))
#endif
_load_static_residual1(Y2n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y2n1,A,((-0.5)*(TR*IY2_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y2n1,B,((-0.5)*(TR*IY2_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y2n1,C,((-0.5)*(TR*IY2_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y2n1,EN,((-0.5)*(TR*IY2_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y2n1,NP(Y2n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y2n1,Y2n1,1.0);
#endif
_load_static_residual2(Y2n1,Y2n2,(BP(Y2n1,Y2n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y2n1,Y2n2,Y2n1,Y2n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y2n2,_DDT((Cd*NP(Y2n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y2n2,Y2n2,(Cd));
#endif
#endif
_load_static_residual1(Y2,(-NP(Y2n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y2,Y2n2,(-1.0));
#endif
_load_static_residual1(Y2,NP(Y2));
#if defined(_DERIVATE)
_load_static_jacobian1(Y2,Y2,1.0);
#endif
#if defined(_DERIVATE)
IY3_VEN_GND=VENI_VEN_GND*VCI*NP(B)*NP(A);
IY3_VC_GND=(VENI*VCI_VC_GND)*NP(B)*NP(A);
IY3_VB_GND=((VENI*VCI))*NP(A);
IY3_VA_GND=(((VENI*VCI)*NP(B)));
#endif
IY3=(((VENI*VCI)*NP(B))*NP(A));
{
double m00_tanh(d00_tanh0,(TR*(IY3-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY3-0.5)))
#endif
_load_static_residual1(Y3n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y3n1,A,((-0.5)*(TR*IY3_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y3n1,B,((-0.5)*(TR*IY3_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y3n1,C,((-0.5)*(TR*IY3_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y3n1,EN,((-0.5)*(TR*IY3_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y3n1,NP(Y3n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y3n1,Y3n1,1.0);
#endif
_load_static_residual2(Y3n1,Y3n2,(BP(Y3n1,Y3n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y3n1,Y3n2,Y3n1,Y3n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y3n2,_DDT((Cd*NP(Y3n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y3n2,Y3n2,(Cd));
#endif
#endif
_load_static_residual1(Y3,(-NP(Y3n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y3,Y3n2,(-1.0));
#endif
_load_static_residual1(Y3,NP(Y3));
#if defined(_DERIVATE)
_load_static_jacobian1(Y3,Y3,1.0);
#endif
#if defined(_DERIVATE)
IY4_VEN_GND=VENI_VEN_GND*NP(C)*VBI*VAI;
IY4_VC_GND=(VENI)*VBI*VAI;
IY4_VB_GND=((VENI*NP(C))*VBI_VB_GND)*VAI;
IY4_VA_GND=(((VENI*NP(C))*VBI)*VAI_VA_GND);
#endif
IY4=(((VENI*NP(C))*VBI)*VAI);
{
double m00_tanh(d00_tanh0,(TR*(IY4-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY4-0.5)))
#endif
_load_static_residual1(Y4n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y4n1,A,((-0.5)*(TR*IY4_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y4n1,B,((-0.5)*(TR*IY4_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y4n1,C,((-0.5)*(TR*IY4_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y4n1,EN,((-0.5)*(TR*IY4_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y4n1,NP(Y4n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y4n1,Y4n1,1.0);
#endif
_load_static_residual2(Y4n1,Y4n2,(BP(Y4n1,Y4n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y4n1,Y4n2,Y4n1,Y4n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y4n2,_DDT((Cd*NP(Y4n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y4n2,Y4n2,(Cd));
#endif
#endif
_load_static_residual1(Y4,(-NP(Y4n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y4,Y4n2,(-1.0));
#endif
_load_static_residual1(Y4,NP(Y4));
#if defined(_DERIVATE)
_load_static_jacobian1(Y4,Y4,1.0);
#endif
#if defined(_DERIVATE)
IY5_VEN_GND=VENI_VEN_GND*NP(C)*VBI*NP(A);
IY5_VC_GND=(VENI)*VBI*NP(A);
IY5_VB_GND=((VENI*NP(C))*VBI_VB_GND)*NP(A);
IY5_VA_GND=(((VENI*NP(C))*VBI));
#endif
IY5=(((VENI*NP(C))*VBI)*NP(A));
{
double m00_tanh(d00_tanh0,(TR*(IY5-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY5-0.5)))
#endif
_load_static_residual1(Y5n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y5n1,A,((-0.5)*(TR*IY5_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y5n1,B,((-0.5)*(TR*IY5_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y5n1,C,((-0.5)*(TR*IY5_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y5n1,EN,((-0.5)*(TR*IY5_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y5n1,NP(Y5n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y5n1,Y5n1,1.0);
#endif
_load_static_residual2(Y5n1,Y5n2,(BP(Y5n1,Y5n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y5n1,Y5n2,Y5n1,Y5n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y5n2,_DDT((Cd*NP(Y5n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y5n2,Y5n2,(Cd));
#endif
#endif
_load_static_residual1(Y5,(-NP(Y5n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y5,Y5n2,(-1.0));
#endif
_load_static_residual1(Y5,NP(Y5));
#if defined(_DERIVATE)
_load_static_jacobian1(Y5,Y5,1.0);
#endif
#if defined(_DERIVATE)
IY6_VEN_GND=VENI_VEN_GND*NP(C)*NP(B)*VAI;
IY6_VC_GND=(VENI)*NP(B)*VAI;
IY6_VB_GND=((VENI*NP(C)))*VAI;
IY6_VA_GND=(((VENI*NP(C))*NP(B))*VAI_VA_GND);
#endif
IY6=(((VENI*NP(C))*NP(B))*VAI);
{
double m00_tanh(d00_tanh0,(TR*(IY6-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY6-0.5)))
#endif
_load_static_residual1(Y6n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y6n1,A,((-0.5)*(TR*IY6_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y6n1,B,((-0.5)*(TR*IY6_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y6n1,C,((-0.5)*(TR*IY6_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y6n1,EN,((-0.5)*(TR*IY6_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y6n1,NP(Y6n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y6n1,Y6n1,1.0);
#endif
_load_static_residual2(Y6n1,Y6n2,(BP(Y6n1,Y6n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y6n1,Y6n2,Y6n1,Y6n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y6n2,_DDT((Cd*NP(Y6n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y6n2,Y6n2,(Cd));
#endif
#endif
_load_static_residual1(Y6,(-NP(Y6n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y6,Y6n2,(-1.0));
#endif
_load_static_residual1(Y6,NP(Y6));
#if defined(_DERIVATE)
_load_static_jacobian1(Y6,Y6,1.0);
#endif
#if defined(_DERIVATE)
IY7_VEN_GND=VENI_VEN_GND*NP(C)*NP(B)*NP(A);
IY7_VC_GND=(VENI)*NP(B)*NP(A);
IY7_VB_GND=((VENI*NP(C)))*NP(A);
IY7_VA_GND=(((VENI*NP(C))*NP(B)));
#endif
IY7=(((VENI*NP(C))*NP(B))*NP(A));
{
double m00_tanh(d00_tanh0,(TR*(IY7-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(IY7-0.5)))
#endif
_load_static_residual1(Y7n1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y7n1,A,((-0.5)*(TR*IY7_VA_GND)*d10_tanh0));
_load_static_jacobian1(Y7n1,B,((-0.5)*(TR*IY7_VB_GND)*d10_tanh0));
_load_static_jacobian1(Y7n1,C,((-0.5)*(TR*IY7_VC_GND)*d10_tanh0));
_load_static_jacobian1(Y7n1,EN,((-0.5)*(TR*IY7_VEN_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Y7n1,NP(Y7n1));
#if defined(_DERIVATE)
_load_static_jacobian1(Y7n1,Y7n1,1.0);
#endif
_load_static_residual2(Y7n1,Y7n2,(BP(Y7n1,Y7n2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Y7n1,Y7n2,Y7n1,Y7n2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Y7n2,_DDT((Cd*NP(Y7n2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Y7n2,Y7n2,(Cd));
#endif
#endif
_load_static_residual1(Y7,(-NP(Y7n2)));
#if defined(_DERIVATE)
_load_static_jacobian1(Y7,Y7n2,(-1.0));
#endif
_load_static_residual1(Y7,NP(Y7));
#if defined(_DERIVATE)
_load_static_jacobian1(Y7,Y7,1.0);
#endif

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void dmux3to8::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 28; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 28; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void dmux3to8::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void dmux3to8::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void dmux3to8::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void dmux3to8::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void dmux3to8::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix dmux3to8::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (28);

  for (int i1 = 0; i1 < 28; i1++) {
    for (int i2 = 0; i2 < 28; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void dmux3to8::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void dmux3to8::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void dmux3to8::initTR (void)
{
  setStates (2 * 28 * 28);
  initDC ();
}

/* Perform transient analysis iteration step. */
void dmux3to8::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 28; i1++) {
  for (i2 = 0; i2 < 28; i2++) {
    state = 2 * (i2 + 28 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 28; i1++) {
    state = 2 * (i1 + 28 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 28; i1++) {
  for (i2 = 0; i2 < 28; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 28; i3++) {
  for (i4 = 0; i4 < 28; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 28; i1++) {
  for (i2 = 0; i2 < 28; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 28; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 28; i1++) {
  for (i3 = 0; i3 < 28; i3++) {
  for (i4 = 0; i4 < 28; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 28; i1++) {
  for (i3 = 0; i3 < 28; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix dmux3to8::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (28);


  return cy;
}

/* Perform AC noise computations. */
void dmux3to8::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void dmux3to8::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void dmux3to8::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void dmux3to8::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 28; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 28; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "dmux3to8.defs.h"
