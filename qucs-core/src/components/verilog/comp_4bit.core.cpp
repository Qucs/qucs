/*
 * comp_4bit.core.cpp - device implementations for comp_4bit module
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

#include "comp_4bit.analogfunction.h"
#include "component.h"
#include "device.h"
#include "comp_4bit.core.h"

#ifndef CIR_comp_4bit
#define CIR_comp_4bit -1
#endif

// external nodes
#define X0 0
#define X1 1
#define X2 2
#define X3 3
#define Y0 4
#define Y1 5
#define Y2 6
#define Y3 7
#define L 8
#define G 9
#define E 10
// internal nodes
#define Ln1 11
#define Ln2 12
#define Gn1 13
#define Gn2 14
#define En1 15
#define En2 16

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
#define _modelname              "comp_4bit"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
comp_4bit::comp_4bit() : circuit (17)
{
  type = CIR_comp_4bit;
}

/* Initialization of model. */
void comp_4bit::initModel (void)
{
  // create internal nodes
  setInternalNode (Ln1, "Ln1");
  setInternalNode (Ln2, "Ln2");
  setInternalNode (Gn1, "Gn1");
  setInternalNode (Gn2, "Gn2");
  setInternalNode (En1, "En1");
  setInternalNode (En2, "En2");

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
void comp_4bit::initDC (void)
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
void comp_4bit::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void comp_4bit::initVerilog (void)
{
  // initialization of noise variables

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 17; i1++) {
  for (i2 = 0; i2 < 17; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 17; i1++) {
  for (i2 = 0; i2 < 17; i2++) {
  for (i3 = 0; i3 < 17; i3++) {
  for (i4 = 0; i4 < 17; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 17; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 17; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void comp_4bit::loadVariables (void)
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
void comp_4bit::initializeModel (void)
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
void comp_4bit::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void comp_4bit::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void comp_4bit::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void comp_4bit::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double mL;
#if defined(_DERIVATE)
double mL_VX3_GND;
double mL_VY3_GND;
double mL_VX2_GND;
double mL_VY2_GND;
double mL_VX1_GND;
double mL_VY1_GND;
double mL_VX0_GND;
double mL_VY0_GND;
#endif
double mG;
#if defined(_DERIVATE)
double mG_VX3_GND;
double mG_VY3_GND;
double mG_VX2_GND;
double mG_VY2_GND;
double mG_VX1_GND;
double mG_VY1_GND;
double mG_VX0_GND;
double mG_VY0_GND;
#endif
double mE;
#if defined(_DERIVATE)
double mE_VX0_GND;
double mE_VY0_GND;
double mE_VX3_GND;
double mE_VY3_GND;
double mE_VX2_GND;
double mE_VY2_GND;
double mE_VX1_GND;
double mE_VY1_GND;
#endif
double t2;
#if defined(_DERIVATE)
double t2_VX3_GND;
double t2_VY3_GND;
double t2_VX2_GND;
double t2_VY2_GND;
double t2_VX1_GND;
double t2_VY1_GND;
#endif
double t1;
#if defined(_DERIVATE)
double t1_VX3_GND;
double t1_VY3_GND;
double t1_VX2_GND;
double t1_VY2_GND;
#endif
double xor3b;
#if defined(_DERIVATE)
double xor3b_VX3_GND;
double xor3b_VY3_GND;
#endif
double xor2b;
#if defined(_DERIVATE)
double xor2b_VX2_GND;
double xor2b_VY2_GND;
#endif
double xor1b;
#if defined(_DERIVATE)
double xor1b_VX1_GND;
double xor1b_VY1_GND;
#endif
double xor0b;
#if defined(_DERIVATE)
double xor0b_VX0_GND;
double xor0b_VY0_GND;
#endif
#if defined(_DERIVATE)
xor0b_VX0_GND=(-(((1-NP(Y0)))+(-1.0)*NP(Y0)));
xor0b_VY0_GND=(-((NP(X0)*(-1.0))+((1-NP(X0)))));
#endif
xor0b=(1-((NP(X0)*(1-NP(Y0)))+((1-NP(X0))*NP(Y0))));
#if defined(_DERIVATE)
xor1b_VX1_GND=(-(((1-NP(Y1)))+(-1.0)*NP(Y1)));
xor1b_VY1_GND=(-((NP(X1)*(-1.0))+((1-NP(X1)))));
#endif
xor1b=(1-((NP(X1)*(1-NP(Y1)))+((1-NP(X1))*NP(Y1))));
#if defined(_DERIVATE)
xor2b_VX2_GND=(-(((1-NP(Y2)))+(-1.0)*NP(Y2)));
xor2b_VY2_GND=(-((NP(X2)*(-1.0))+((1-NP(X2)))));
#endif
xor2b=(1-((NP(X2)*(1-NP(Y2)))+((1-NP(X2))*NP(Y2))));
#if defined(_DERIVATE)
xor3b_VX3_GND=(-(((1-NP(Y3)))+(-1.0)*NP(Y3)));
xor3b_VY3_GND=(-((NP(X3)*(-1.0))+((1-NP(X3)))));
#endif
xor3b=(1-((NP(X3)*(1-NP(Y3)))+((1-NP(X3))*NP(Y3))));
#if defined(_DERIVATE)
t1_VX3_GND=xor3b_VX3_GND*xor2b;
t1_VY3_GND=xor3b_VY3_GND*xor2b;
t1_VX2_GND=(xor3b*xor2b_VX2_GND);
t1_VY2_GND=(xor3b*xor2b_VY2_GND);
#endif
t1=(xor3b*xor2b);
#if defined(_DERIVATE)
t2_VX3_GND=t1_VX3_GND*xor1b;
t2_VY3_GND=t1_VY3_GND*xor1b;
t2_VX2_GND=t1_VX2_GND*xor1b;
t2_VY2_GND=t1_VY2_GND*xor1b;
t2_VX1_GND=(t1*xor1b_VX1_GND);
t2_VY1_GND=(t1*xor1b_VY1_GND);
#endif
t2=(t1*xor1b);
#if defined(_DERIVATE)
mE_VX0_GND=xor0b_VX0_GND*t2;
mE_VY0_GND=xor0b_VY0_GND*t2;
mE_VX3_GND=(xor0b*t2_VX3_GND);
mE_VY3_GND=(xor0b*t2_VY3_GND);
mE_VX2_GND=(xor0b*t2_VX2_GND);
mE_VY2_GND=(xor0b*t2_VY2_GND);
mE_VX1_GND=(xor0b*t2_VX1_GND);
mE_VY1_GND=(xor0b*t2_VY1_GND);
#endif
mE=(xor0b*t2);
#if defined(_DERIVATE)
mG_VX3_GND=(((((1-NP(Y3)))+xor3b_VX3_GND*NP(X2)*(1-NP(Y2)))+t1_VX3_GND*NP(X1)*(1-NP(Y1)))+t2_VX3_GND*NP(X0)*(1-NP(Y0)));
mG_VY3_GND=((((NP(X3)*(-1.0))+xor3b_VY3_GND*NP(X2)*(1-NP(Y2)))+t1_VY3_GND*NP(X1)*(1-NP(Y1)))+t2_VY3_GND*NP(X0)*(1-NP(Y0)));
mG_VX2_GND=(((xor3b)*(1-NP(Y2))+t1_VX2_GND*NP(X1)*(1-NP(Y1)))+t2_VX2_GND*NP(X0)*(1-NP(Y0)));
mG_VY2_GND=((((xor3b*NP(X2))*(-1.0))+t1_VY2_GND*NP(X1)*(1-NP(Y1)))+t2_VY2_GND*NP(X0)*(1-NP(Y0)));
mG_VX1_GND=((t1)*(1-NP(Y1))+t2_VX1_GND*NP(X0)*(1-NP(Y0)));
mG_VY1_GND=(((t1*NP(X1))*(-1.0))+t2_VY1_GND*NP(X0)*(1-NP(Y0)));
mG_VX0_GND=(t2)*(1-NP(Y0));
mG_VY0_GND=((t2*NP(X0))*(-1.0));
#endif
mG=((((NP(X3)*(1-NP(Y3)))+((xor3b*NP(X2))*(1-NP(Y2))))+((t1*NP(X1))*(1-NP(Y1))))+((t2*NP(X0))*(1-NP(Y0))));
#if defined(_DERIVATE)
mL_VX3_GND=((((-1.0)*NP(Y3)+xor3b_VX3_GND*(1-NP(X2))*NP(Y2))+t1_VX3_GND*(1-NP(X1))*NP(Y1))+t2_VX3_GND*(1-NP(X0))*NP(Y0));
mL_VY3_GND=(((((1-NP(X3)))+xor3b_VY3_GND*(1-NP(X2))*NP(Y2))+t1_VY3_GND*(1-NP(X1))*NP(Y1))+t2_VY3_GND*(1-NP(X0))*NP(Y0));
mL_VX2_GND=(((xor3b*(-1.0))*NP(Y2)+t1_VX2_GND*(1-NP(X1))*NP(Y1))+t2_VX2_GND*(1-NP(X0))*NP(Y0));
mL_VY2_GND=((((xor3b*(1-NP(X2))))+t1_VY2_GND*(1-NP(X1))*NP(Y1))+t2_VY2_GND*(1-NP(X0))*NP(Y0));
mL_VX1_GND=((t1*(-1.0))*NP(Y1)+t2_VX1_GND*(1-NP(X0))*NP(Y0));
mL_VY1_GND=(((t1*(1-NP(X1))))+t2_VY1_GND*(1-NP(X0))*NP(Y0));
mL_VX0_GND=(t2*(-1.0))*NP(Y0);
mL_VY0_GND=((t2*(1-NP(X0))));
#endif
mL=(((((1-NP(X3))*NP(Y3))+((xor3b*(1-NP(X2)))*NP(Y2)))+((t1*(1-NP(X1)))*NP(Y1)))+((t2*(1-NP(X0)))*NP(Y0)));
{
double m00_tanh(d00_tanh0,(TR*(mE-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mE-0.5)))
#endif
_load_static_residual1(En1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(En1,Y1,((-0.5)*(TR*mE_VY1_GND)*d10_tanh0));
_load_static_jacobian1(En1,X1,((-0.5)*(TR*mE_VX1_GND)*d10_tanh0));
_load_static_jacobian1(En1,Y2,((-0.5)*(TR*mE_VY2_GND)*d10_tanh0));
_load_static_jacobian1(En1,X2,((-0.5)*(TR*mE_VX2_GND)*d10_tanh0));
_load_static_jacobian1(En1,Y3,((-0.5)*(TR*mE_VY3_GND)*d10_tanh0));
_load_static_jacobian1(En1,X3,((-0.5)*(TR*mE_VX3_GND)*d10_tanh0));
_load_static_jacobian1(En1,Y0,((-0.5)*(TR*mE_VY0_GND)*d10_tanh0));
_load_static_jacobian1(En1,X0,((-0.5)*(TR*mE_VX0_GND)*d10_tanh0));
#endif
}
_load_static_residual1(En1,NP(En1));
#if defined(_DERIVATE)
_load_static_jacobian1(En1,En1,1.0);
#endif
_load_static_residual2(En1,En2,(BP(En1,En2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(En1,En2,En1,En2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(En2,_DDT((Cd*NP(En2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(En2,En2,(Cd));
#endif
#endif
_load_static_residual1(E,(-NP(En2)));
#if defined(_DERIVATE)
_load_static_jacobian1(E,En2,(-1.0));
#endif
_load_static_residual1(E,NP(E));
#if defined(_DERIVATE)
_load_static_jacobian1(E,E,1.0);
#endif
{
double m00_tanh(d00_tanh0,(TR*(mG-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mG-0.5)))
#endif
_load_static_residual1(Gn1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Gn1,Y0,((-0.5)*(TR*mG_VY0_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,X0,((-0.5)*(TR*mG_VX0_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,Y1,((-0.5)*(TR*mG_VY1_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,X1,((-0.5)*(TR*mG_VX1_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,Y2,((-0.5)*(TR*mG_VY2_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,X2,((-0.5)*(TR*mG_VX2_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,Y3,((-0.5)*(TR*mG_VY3_GND)*d10_tanh0));
_load_static_jacobian1(Gn1,X3,((-0.5)*(TR*mG_VX3_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Gn1,NP(Gn1));
#if defined(_DERIVATE)
_load_static_jacobian1(Gn1,Gn1,1.0);
#endif
_load_static_residual2(Gn1,Gn2,(BP(Gn1,Gn2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Gn1,Gn2,Gn1,Gn2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Gn2,_DDT((Cd*NP(Gn2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Gn2,Gn2,(Cd));
#endif
#endif
_load_static_residual1(G,(-NP(Gn2)));
#if defined(_DERIVATE)
_load_static_jacobian1(G,Gn2,(-1.0));
#endif
_load_static_residual1(G,NP(G));
#if defined(_DERIVATE)
_load_static_jacobian1(G,G,1.0);
#endif
{
double m00_tanh(d00_tanh0,(TR*(mL-0.5)))
#if defined(_DERIVATE)
double m10_tanh(d10_tanh0,d00_tanh0,(TR*(mL-0.5)))
#endif
_load_static_residual1(Ln1,((-0.5)*(1+d00_tanh0)));
#if defined(_DERIVATE)
_load_static_jacobian1(Ln1,Y0,((-0.5)*(TR*mL_VY0_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,X0,((-0.5)*(TR*mL_VX0_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,Y1,((-0.5)*(TR*mL_VY1_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,X1,((-0.5)*(TR*mL_VX1_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,Y2,((-0.5)*(TR*mL_VY2_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,X2,((-0.5)*(TR*mL_VX2_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,Y3,((-0.5)*(TR*mL_VY3_GND)*d10_tanh0));
_load_static_jacobian1(Ln1,X3,((-0.5)*(TR*mL_VX3_GND)*d10_tanh0));
#endif
}
_load_static_residual1(Ln1,NP(Ln1));
#if defined(_DERIVATE)
_load_static_jacobian1(Ln1,Ln1,1.0);
#endif
_load_static_residual2(Ln1,Ln2,(BP(Ln1,Ln2)/Rd));
#if defined(_DERIVATE)
_load_static_jacobian4(Ln1,Ln2,Ln1,Ln2,(1/Rd));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(Ln2,_DDT((Cd*NP(Ln2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(Ln2,Ln2,(Cd));
#endif
#endif
_load_static_residual1(L,(-NP(Ln2)));
#if defined(_DERIVATE)
_load_static_jacobian1(L,Ln2,(-1.0));
#endif
_load_static_residual1(L,NP(L));
#if defined(_DERIVATE)
_load_static_jacobian1(L,L,1.0);
#endif

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void comp_4bit::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 17; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 17; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void comp_4bit::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void comp_4bit::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void comp_4bit::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void comp_4bit::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void comp_4bit::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix comp_4bit::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (17);

  for (int i1 = 0; i1 < 17; i1++) {
    for (int i2 = 0; i2 < 17; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void comp_4bit::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void comp_4bit::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void comp_4bit::initTR (void)
{
  setStates (2 * 17 * 17);
  initDC ();
}

/* Perform transient analysis iteration step. */
void comp_4bit::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 17; i1++) {
  for (i2 = 0; i2 < 17; i2++) {
    state = 2 * (i2 + 17 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 17; i1++) {
    state = 2 * (i1 + 17 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 17; i1++) {
  for (i2 = 0; i2 < 17; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 17; i3++) {
  for (i4 = 0; i4 < 17; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 17; i1++) {
  for (i2 = 0; i2 < 17; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 17; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 17; i1++) {
  for (i3 = 0; i3 < 17; i3++) {
  for (i4 = 0; i4 < 17; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 17; i1++) {
  for (i3 = 0; i3 < 17; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix comp_4bit::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (17);


  return cy;
}

/* Perform AC noise computations. */
void comp_4bit::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void comp_4bit::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void comp_4bit::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void comp_4bit::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 17; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 17; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "comp_4bit.defs.h"
