/*
 * mod_amp.core.cpp - device implementations for mod_amp module
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

#include "mod_amp.analogfunction.h"
#include "component.h"
#include "device.h"
#include "mod_amp.core.h"

#ifndef CIR_mod_amp
#define CIR_mod_amp -1
#endif

// external nodes
#define in_p 0
#define in_n 1
#define out_p 2
// internal nodes
#define n2 3
#define n3 4
#define n4 5
#define n5 6
#define n6 7
#define n7 8
#define n8 9
#define n9 10
#define n10 11
#define n11 12
#define n12 13

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
#define _modelname              "mod_amp"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
mod_amp::mod_amp() : circuit (14)
{
  type = CIR_mod_amp;
}

/* Initialization of model. */
void mod_amp::initModel (void)
{
  // create internal nodes
  setInternalNode (n2, "n2");
  setInternalNode (n3, "n3");
  setInternalNode (n4, "n4");
  setInternalNode (n5, "n5");
  setInternalNode (n6, "n6");
  setInternalNode (n7, "n7");
  setInternalNode (n8, "n8");
  setInternalNode (n9, "n9");
  setInternalNode (n10, "n10");
  setInternalNode (n11, "n11");
  setInternalNode (n12, "n12");

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
void mod_amp::initDC (void)
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
void mod_amp::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void mod_amp::initVerilog (void)
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
void mod_amp::loadVariables (void)
{
  GBP = getPropertyDouble ("GBP");
  AOLDC = getPropertyDouble ("AOLDC");
  FP2 = getPropertyDouble ("FP2");
  RO = getPropertyDouble ("RO");
  CD = getPropertyDouble ("CD");
  RD = getPropertyDouble ("RD");
  IOFF = getPropertyDouble ("IOFF");
  IB = getPropertyDouble ("IB");
  VOFF = getPropertyDouble ("VOFF");
  CMRRDC = getPropertyDouble ("CMRRDC");
  FCM = getPropertyDouble ("FCM");
  PSRT = getPropertyDouble ("PSRT");
  NSRT = getPropertyDouble ("NSRT");
  VLIMP = getPropertyDouble ("VLIMP");
  VLIMN = getPropertyDouble ("VLIMN");
  ILMAX = getPropertyDouble ("ILMAX");
  CSCALE = getPropertyDouble ("CSCALE");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void mod_amp::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void mod_amp::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void mod_amp::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void mod_amp::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void mod_amp::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double Slewratenegative;
double Slewratepositive;
#if defined(_DYNAMIC)
double CP2;
#endif
double RP2;
#if defined(_DYNAMIC)
double CP1;
#endif
double RP1;
#if defined(_DYNAMIC)
double QCM;
#if defined(_DERIVATE)
double QCM_Vn6_n10;
#endif
#endif
double CMgain;
double CMRR0;
double Rdiff;
double Voffset;
double MTWOPI;
MTWOPI=6.28318530717958647693;
Voffset=(VOFF*0.5);
Rdiff=(RD/2);
{
double m00_pow(d00_pow0,10,(CMRRDC/20))
CMRR0=d00_pow0;
}
CMgain=(1e6/CMRR0);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
QCM_Vn6_n10=((1.0/((MTWOPI*1e6)*FCM)));
#endif
QCM=((1.0/((MTWOPI*1e6)*FCM))*BP(n6,n10));
#endif
{
double m00_pow(d00_pow0,10,(AOLDC/20))
RP1=d00_pow0;
}
#if defined(_DYNAMIC)
CP1=(1/(MTWOPI*GBP));
#endif
RP2=1;
#if defined(_DYNAMIC)
CP2=(1/(MTWOPI*FP2));
#endif
Slewratepositive=(PSRT/(MTWOPI*GBP));
Slewratenegative=(NSRT/(MTWOPI*GBP));
_load_static_residual2(in_p,n7,BP(in_p,n7));
#if defined(_DERIVATE)
_load_static_jacobian4(in_p,n7,in_p,n7,1.0);
#endif
_load_static_residual2(in_p,n7,Voffset);
#if defined(_DERIVATE)
#endif
_load_static_residual2(in_n,n9,BP(in_n,n9));
#if defined(_DERIVATE)
_load_static_jacobian4(in_n,n9,in_n,n9,1.0);
#endif
_load_static_residual2(in_n,n9,(-Voffset));
#if defined(_DERIVATE)
#endif
_load_static_residual1(n7,IB);
#if defined(_DERIVATE)
#endif
_load_static_residual1(n9,IB);
#if defined(_DERIVATE)
#endif
_load_static_residual2(n7,n9,(IOFF/2));
#if defined(_DERIVATE)
#endif
_load_static_residual2(n7,n8,(BP(n7,n8)/Rdiff));
#if defined(_DERIVATE)
_load_static_jacobian4(n7,n8,n7,n8,(1/Rdiff));
#endif
_load_static_residual2(n9,n8,(BP(n9,n8)/Rdiff));
#if defined(_DERIVATE)
_load_static_jacobian4(n9,n8,n9,n8,(1/Rdiff));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n7,n9,_DDT((CD*BP(n7,n9))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n7,n9,n7,n9,(CD));
#endif
#endif
_load_static_residual1(n6,((-CMgain)*NP(n8)));
#if defined(_DERIVATE)
_load_static_jacobian1(n6,n8,((-CMgain)));
#endif
_load_static_residual1(n6,NP(n6));
#if defined(_DERIVATE)
_load_static_jacobian1(n6,n6,1.0);
#endif
_load_static_residual2(n6,n10,(BP(n6,n10)/1e6));
#if defined(_DERIVATE)
_load_static_jacobian4(n6,n10,n6,n10,(1/1e6));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n6,n10,_DDT(QCM));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n6,n10,n6,n10,QCM_Vn6_n10);
#endif
#endif
_load_static_residual1(n10,NP(n10));
#if defined(_DERIVATE)
_load_static_jacobian1(n10,n10,1.0);
#endif
_load_static_residual1(n11,(-NP(n10)));
#if defined(_DERIVATE)
_load_static_jacobian1(n11,n10,(-1.0));
#endif
_load_static_residual1(n11,(-BP(n7,n9)));
#if defined(_DERIVATE)
_load_static_jacobian2p(n11,n7,n9,(-1.0));
#endif
_load_static_residual1(n11,NP(n11));
#if defined(_DERIVATE)
_load_static_jacobian1(n11,n11,1.0);
#endif
if
((NP(n11)>Slewratepositive))
{
_load_static_residual1(n12,(-Slewratepositive));
#if defined(_DERIVATE)
#endif
}
else
{
if
((NP(n11)<(-Slewratenegative)))
{
_load_static_residual1(n12,Slewratenegative);
#if defined(_DERIVATE)
#endif
}
else
{
_load_static_residual1(n12,(-NP(n11)));
#if defined(_DERIVATE)
_load_static_jacobian1(n12,n11,(-1.0));
#endif
}
}
_load_static_residual1(n12,NP(n12));
#if defined(_DERIVATE)
_load_static_jacobian1(n12,n12,1.0);
#endif
_load_static_residual1(n3,(-NP(n12)));
#if defined(_DERIVATE)
_load_static_jacobian1(n3,n12,(-1.0));
#endif
_load_static_residual1(n3,(NP(n3)/RP1));
#if defined(_DERIVATE)
_load_static_jacobian1(n3,n3,(1/RP1));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(n3,_DDT((CP1*NP(n3))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(n3,n3,(CP1));
#endif
#endif
_load_static_residual1(n5,(-NP(n3)));
#if defined(_DERIVATE)
_load_static_jacobian1(n5,n3,(-1.0));
#endif
_load_static_residual1(n5,(NP(n5)/RP2));
#if defined(_DERIVATE)
_load_static_jacobian1(n5,n5,(1/RP2));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual1(n5,_DDT((CP2*NP(n5))));
#if defined(_DERIVATE)
_load_dynamic_jacobian1(n5,n5,(CP2));
#endif
#endif
if
((BP(n2,out_p)>=ILMAX))
{
_load_static_residual1(n4,(-NP(n5)));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n5,(-1.0));
#endif
_load_static_residual1(n4,((CSCALE*NP(n5))*(BP(n2,out_p)-ILMAX)));
#if defined(_DERIVATE)
_load_static_jacobian2p(n4,n2,out_p,((CSCALE*NP(n5))));
_load_static_jacobian1(n4,n5,(CSCALE)*(BP(n2,out_p)-ILMAX));
#endif
_load_static_residual1(n4,NP(n4));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n4,1.0);
#endif
}
else
{
if
((BP(n2,out_p)<=(-ILMAX)))
{
_load_static_residual1(n4,(-NP(n5)));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n5,(-1.0));
#endif
_load_static_residual1(n4,(((-CSCALE)*NP(n5))*(BP(n2,out_p)+ILMAX)));
#if defined(_DERIVATE)
_load_static_jacobian2p(n4,n2,out_p,(((-CSCALE)*NP(n5))));
_load_static_jacobian1(n4,n5,((-CSCALE))*(BP(n2,out_p)+ILMAX));
#endif
_load_static_residual1(n4,NP(n4));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n4,1.0);
#endif
}
else
{
_load_static_residual1(n4,(-NP(n5)));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n5,(-1.0));
#endif
_load_static_residual1(n4,NP(n4));
#if defined(_DERIVATE)
_load_static_jacobian1(n4,n4,1.0);
#endif
}
}
_load_static_residual2(n4,n2,(BP(n4,n2)/(RO-1)));
#if defined(_DERIVATE)
_load_static_jacobian4(n4,n2,n4,n2,(1/(RO-1)));
#endif
_load_static_residual2(n2,out_p,BP(n2,out_p));
#if defined(_DERIVATE)
_load_static_jacobian4(n2,out_p,n2,out_p,1.0);
#endif
if
((NP(out_p)>VLIMP))
{
_load_static_residual1(out_p,((-10.0)*VLIMP));
#if defined(_DERIVATE)
#endif
_load_static_residual1(out_p,(10.0*NP(out_p)));
#if defined(_DERIVATE)
_load_static_jacobian1(out_p,out_p,(10.0));
#endif
}
else
{
if
((NP(out_p)<VLIMN))
{
_load_static_residual1(out_p,((-10.0)*VLIMN));
#if defined(_DERIVATE)
#endif
_load_static_residual1(out_p,(10.0*NP(out_p)));
#if defined(_DERIVATE)
_load_static_jacobian1(out_p,out_p,(10.0));
#endif
}
}

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void mod_amp::calcDC (void)
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
void mod_amp::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void mod_amp::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void mod_amp::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void mod_amp::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void mod_amp::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix mod_amp::calcMatrixY (nr_double_t frequency)
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
void mod_amp::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void mod_amp::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void mod_amp::initTR (void)
{
  setStates (2 * 14 * 14);
  initDC ();
}

/* Perform transient analysis iteration step. */
void mod_amp::calcTR (nr_double_t)
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
matrix mod_amp::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (14);


  return cy;
}

/* Perform AC noise computations. */
void mod_amp::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void mod_amp::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void mod_amp::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void mod_amp::calcHB (int)
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

#include "mod_amp.defs.h"
