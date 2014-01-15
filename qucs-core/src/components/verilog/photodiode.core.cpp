/*
 * photodiode.core.cpp - device implementations for photodiode module
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

#include "photodiode.analogfunction.h"
#include "component.h"
#include "device.h"
#include "photodiode.core.h"

#ifndef CIR_photodiode
#define CIR_photodiode -1
#endif

// external nodes
#define Anode 0
#define Cathode 1
#define Light 2
// internal nodes
#define n1 3

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
#define _modelname              "photodiode"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
photodiode::photodiode() : circuit (4)
{
  type = CIR_photodiode;
}

/* Initialization of model. */
void photodiode::initModel (void)
{
  // create internal nodes
  setInternalNode (n1, "n1");

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
void photodiode::initDC (void)
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
void photodiode::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void photodiode::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[Anode][n1] = 0.0;
  _white_pwr[n1][Cathode] = 0.0;
  _flicker_pwr[n1][Cathode] = 0.0;
  _flicker_exp[n1][Cathode] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 4; i1++) {
  for (i2 = 0; i2 < 4; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 4; i1++) {
  for (i2 = 0; i2 < 4; i2++) {
  for (i3 = 0; i3 < 4; i3++) {
  for (i4 = 0; i4 < 4; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 4; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 4; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void photodiode::loadVariables (void)
{
  N = getPropertyDouble ("N");
  Rseries = getPropertyDouble ("Rseries");
  Is = getPropertyDouble ("Is");
  Bv = getPropertyDouble ("Bv");
  Ibv = getPropertyDouble ("Ibv");
  Vj = getPropertyDouble ("Vj");
  Cj0 = getPropertyDouble ("Cj0");
  M = getPropertyDouble ("M");
  Area = getPropertyDouble ("Area");
  Tnom = getPropertyDouble ("Tnom");
  Fc = getPropertyDouble ("Fc");
  Tt = getPropertyDouble ("Tt");
  Xti = getPropertyDouble ("Xti");
  Eg = getPropertyDouble ("Eg");
  Responsivity = getPropertyDouble ("Responsivity");
  Rsh = getPropertyDouble ("Rsh");
  QEpercent = getPropertyDouble ("QEpercent");
  Lambda = getPropertyDouble ("Lambda");
  LEVEL = getPropertyInteger ("LEVEL");
  Kf = getPropertyDouble ("Kf");
  Af = getPropertyDouble ("Af");
  Ffe = getPropertyDouble ("Ffe");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void photodiode::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void photodiode::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void photodiode::initialStep (void)
{
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
double Res2;
double Res1;
double Is_T2;
#if defined(_DYNAMIC)
double Cj0_T2;
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
double Eg_T2;
#endif
double Eg_T1;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
double Vt;
double T2;
double T1;
double B;
double A;
{
Rseries_Area=((Rseries+1e-10)/Area);
A=7.02e-4;
B=1108.0;
T1=(Tnom+273.15);
T2=_circuit_temp;
Vt=((1.3806503e-23*300.0)/1.602176462e-19);
Vt_T2=((1.3806503e-23*T2)/1.602176462e-19);
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-Fc),(1-M))
F1=((Vj/(1-M))*(1-d00_pow0));
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
Eg_T1=(Eg-(((A*T1)*T1)/(B+T1)));
#if defined(_DYNAMIC)
Eg_T2=(Eg-(((A*T2)*T2)/(B+T2)));
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(T2/T1),1.5)
double m00_logE(d00_logE1,d00_pow0)
Vj_T2=((((T2/T1)*Vj)-((2*_vt_nom)*d00_logE1))-(((T2/T1)*Eg_T1)-Eg_T2));
}
#endif
GMIN=1e-12;
#if defined(_DYNAMIC)
Cj0_T2=(Cj0*(1+(M*((400e-6*(T2-T1))-((Vj_T2-Vj)/Vj)))));
#endif
{
double m00_pow(d00_pow0,(T2/T1),(Xti/N))
double m00_limexp(d00_limexp1,(((-Eg_T1)/_vt_nom)*(1-(T2/T1))))
Is_T2=((Is*d00_pow0)*d00_limexp1);
}
Res1=((QEpercent!=0)?((QEpercent*Lambda)/1.2398e5):Responsivity);
Res2=((QEpercent*Lambda)/1.2938e5);
Res=((LEVEL==1)?Res1:Res2);
con1=(((-5.0)*N)*Vt);
con2=(Area*Is_T2);
#if defined(_DYNAMIC)
con3=(Area*Cj0_T2);
#endif
#if defined(_DYNAMIC)
con4=(Fc*Vj);
#endif
#if defined(_DYNAMIC)
con5=(Fc*Vj_T2);
#endif
con6=(Bv/Vt_T2);
}
}

/* Evaluate Verilog-AMS equations in final step. */
void photodiode::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void photodiode::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double I_flicker;
#if defined(_DERIVATE)
double I_flicker_Vn1_Cathode;
#endif
double TwoQ;
double fourkt;
double Id;
#if defined(_DERIVATE)
double Id_Vn1_Cathode;
#endif
double I5;
#if defined(_DERIVATE)
double I5_VLight_GND;
#endif
#if defined(_DYNAMIC)
double Q2;
#if defined(_DERIVATE)
double Q2_Vn1_Cathode;
#endif
#endif
#if defined(_DYNAMIC)
double Q1;
#if defined(_DERIVATE)
double Q1_Vn1_Cathode;
#endif
#endif
double I4;
#if defined(_DERIVATE)
double I4_Vn1_Cathode;
#endif
double I3;
#if defined(_DERIVATE)
double I3_Vn1_Cathode;
#endif
double I2;
#if defined(_DERIVATE)
double I2_Vn1_Cathode;
#endif
double I1;
#if defined(_DERIVATE)
double I1_Vn1_Cathode;
#endif
double V1;
#if defined(_DERIVATE)
double V1_Vn1_Cathode;
#endif
#if defined(_DERIVATE)
V1_Vn1_Cathode=1.0;
#endif
V1=BP(n1,Cathode);
{
double m00_limexp(d00_limexp0,(V1/(N*Vt_T2)))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(V1/(N*Vt_T2)))
#endif
#if defined(_DERIVATE)
I1_Vn1_Cathode=((V1>con1)?((con2*(V1_Vn1_Cathode/(N*Vt_T2))*d10_limexp0)+(GMIN*V1_Vn1_Cathode)):0.0);
#endif
I1=((V1>con1)?((con2*(d00_limexp0-1.0))+(GMIN*V1)):0);
}
#if defined(_DERIVATE)
I2_Vn1_Cathode=((V1<=con1)?(GMIN*V1_Vn1_Cathode):0.0);
#endif
I2=((V1<=con1)?((-con2)+(GMIN*V1)):0);
#if defined(_DERIVATE)
I3_Vn1_Cathode=((V1==(-Bv))?0.0:0.0);
#endif
I3=((V1==(-Bv))?(-Ibv):0);
{
double m00_limexp(d00_limexp0,((-(Bv+V1))/Vt_T2))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,((-(Bv+V1))/Vt_T2))
#endif
#if defined(_DERIVATE)
I4_Vn1_Cathode=((V1<(-Bv))?((-con2)*((-V1_Vn1_Cathode)/Vt_T2)*d10_limexp0):0.0);
#endif
I4=((V1<(-Bv))?((-con2)*((d00_limexp0-1.0)+con6)):0);
}
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-(V1/Vj_T2)),(1-M))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-(V1/Vj_T2)),(1-M))
#endif
#if defined(_DERIVATE)
Q1_Vn1_Cathode=((V1<con4)?((Tt*I1_Vn1_Cathode)+((con3*(Vj_T2/(1-M)))*(-(d10_pow0*(-(V1_Vn1_Cathode/Vj_T2)))))):0.0);
#endif
Q1=((V1<con4)?((Tt*I1)+((con3*(Vj_T2/(1-M)))*(1-d00_pow0))):0);
}
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Q2_Vn1_Cathode=((V1>=con4)?((Tt*I1_Vn1_Cathode)+(con3*((1/F2)*((F3*V1_Vn1_Cathode)+((M/(2.0*Vj_T2))*((V1_Vn1_Cathode*V1)+(V1*V1_Vn1_Cathode))))))):0.0);
#endif
Q2=((V1>=con4)?((Tt*I1)+(con3*(F1+((1/F2)*((F3*(V1-con5))+((M/(2.0*Vj_T2))*((V1*V1)-(con5*con5)))))))):0);
#endif
#if defined(_DERIVATE)
I5_VLight_GND=(Res);
#endif
I5=(NP(Light)*Res);
#if defined(_DERIVATE)
Id_Vn1_Cathode=(((I1_Vn1_Cathode+I2_Vn1_Cathode)+I3_Vn1_Cathode)+I4_Vn1_Cathode);
#endif
Id=(((I1+I2)+I3)+I4);
_load_static_residual2(n1,Cathode,(-I5));
#if defined(_DERIVATE)
_load_static_jacobian2s(n1,Cathode,Light,(-I5_VLight_GND));
#endif
_load_static_residual2(n1,Cathode,(BP(n1,Cathode)/Rsh));
#if defined(_DERIVATE)
_load_static_jacobian4(n1,Cathode,n1,Cathode,(1/Rsh));
#endif
_load_static_residual2(Anode,n1,(BP(Anode,n1)/Rseries_Area));
#if defined(_DERIVATE)
_load_static_jacobian4(Anode,n1,Anode,n1,(1/Rseries_Area));
#endif
_load_static_residual2(n1,Cathode,Id);
#if defined(_DERIVATE)
_load_static_jacobian4(n1,Cathode,n1,Cathode,Id_Vn1_Cathode);
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(n1,Cathode,_DDT((Q1+Q2)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(n1,Cathode,n1,Cathode,(Q1_Vn1_Cathode+Q2_Vn1_Cathode));
#endif
#endif
_load_static_residual1(Light,(NP(Light)/1e10));
#if defined(_DERIVATE)
_load_static_jacobian1(Light,Light,(1/1e10));
#endif
fourkt=((4.0*1.3806503e-23)*_circuit_temp);
TwoQ=(2.0*1.602176462e-19);
{
double m00_pow(d00_pow0,Id,Af)
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,Id,Af)
#endif
#if defined(_DERIVATE)
I_flicker_Vn1_Cathode=(d10_pow0*Id_Vn1_Cathode);
#endif
I_flicker=d00_pow0;
}
_save_whitenoise2(Anode,n1,(fourkt/Rseries_Area),"thermal");
_save_whitenoise2(n1,Cathode,(fourkt/Rsh),"thermal");
_save_whitenoise2(n1,Cathode,(TwoQ*Id),"shot");
_save_flickernoise2(n1,Cathode,(Kf*I_flicker),Ffe,"flicker");
_save_whitenoise2(n1,Cathode,(TwoQ*I5),"shot");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void photodiode::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 4; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 4; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void photodiode::saveOperatingPoints (void)
{
  // save global instance operating points
  setOperatingPoint ("F1", F1);
  setOperatingPoint ("F2", F2);
  setOperatingPoint ("F3", F3);
  setOperatingPoint ("Rseries_Area", Rseries_Area);
  setOperatingPoint ("Vt_T2", Vt_T2);
  setOperatingPoint ("Vj_T2", Vj_T2);
  setOperatingPoint ("GMIN", GMIN);
  setOperatingPoint ("Res", Res);
  setOperatingPoint ("con1", con1);
  setOperatingPoint ("con2", con2);
  setOperatingPoint ("con3", con3);
  setOperatingPoint ("con4", con4);
  setOperatingPoint ("con5", con5);
  setOperatingPoint ("con6", con6);
}

/* Load operating points. */
void photodiode::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void photodiode::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void photodiode::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void photodiode::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix photodiode::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (4);

  for (int i1 = 0; i1 < 4; i1++) {
    for (int i2 = 0; i2 < 4; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void photodiode::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void photodiode::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void photodiode::initTR (void)
{
  setStates (2 * 4 * 4);
  initDC ();
}

/* Perform transient analysis iteration step. */
void photodiode::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 4; i1++) {
  for (i2 = 0; i2 < 4; i2++) {
    state = 2 * (i2 + 4 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 4; i1++) {
    state = 2 * (i1 + 4 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 4; i1++) {
  for (i2 = 0; i2 < 4; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 4; i3++) {
  for (i4 = 0; i4 < 4; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 4; i1++) {
  for (i2 = 0; i2 < 4; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 4; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 4; i1++) {
  for (i3 = 0; i3 < 4; i3++) {
  for (i4 = 0; i4 < 4; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 4; i1++) {
  for (i3 = 0; i3 < 4; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix photodiode::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (4);

  _load_flickernoise2 (n1,Cathode,_flicker_pwr[n1][Cathode],_flicker_exp[n1][Cathode]);
  _load_whitenoise2 (Anode,n1,_white_pwr[Anode][n1]);
  _load_whitenoise2 (n1,Cathode,_white_pwr[n1][Cathode]);

  return cy;
}

/* Perform AC noise computations. */
void photodiode::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void photodiode::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void photodiode::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void photodiode::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 4; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 4; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "photodiode.defs.h"
