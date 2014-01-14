/*
 * EKV26MOS.core.cpp - device implementations for EKV26MOS module
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

#include "EKV26MOS.analogfunction.h"
#include "component.h"
#include "device.h"
#include "EKV26MOS.core.h"

#ifndef CIR_EKV26MOS
#define CIR_EKV26MOS -1
#endif

// external nodes
#define Drain 0
#define Gate 1
#define Source 2
#define Bulk 3
// internal nodes
#define Drain_int 4
#define Source_int 5

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
#define _modelname              "EKV26MOS"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
EKV26MOS::EKV26MOS() : circuit (6)
{
  type = CIR_EKV26MOS;
}

/* Initialization of model. */
void EKV26MOS::initModel (void)
{
  // create internal nodes
  setInternalNode (Drain_int, "Drain_int");
  setInternalNode (Source_int, "Source_int");

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
void EKV26MOS::initDC (void)
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
void EKV26MOS::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void EKV26MOS::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[Drain_int][Drain] = 0.0;
  _white_pwr[Source_int][Source] = 0.0;
  _white_pwr[Source_int][Drain_int] = 0.0;
  _white_pwr[Drain_int][Source_int] = 0.0;
  _white_pwr[Source][Source_int] = 0.0;
  _white_pwr[Drain][Drain_int] = 0.0;
  _flicker_pwr[Source_int][Drain_int] = 0.0;
  _flicker_exp[Source_int][Drain_int] = 0.0;
  _flicker_pwr[Drain_int][Source_int] = 0.0;
  _flicker_exp[Drain_int][Source_int] = 0.0;

  int i1, i2, i3, i4;

  // zero charges
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
    _charges[i1][i2] = 0.0;
  } }

  // zero capacitances
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    _caps[i1][i2][i3][i4] = 0.0;
  } } } }

  // zero right hand side, static and dynamic jacobian
  for (i1 = 0; i1 < 6; i1++) {
    _rhs[i1] = 0.0;
    _qhs[i1] = 0.0;
    _chs[i1] = 0.0;
    _ghs[i1] = 0.0;
    for (i2 = 0; i2 < 6; i2++) {
      _jstat[i1][i2] = 0.0;
      _jdyna[i1][i2] = 0.0;
    }
  }
}

/* Load device model input parameters. */
void EKV26MOS::loadVariables (void)
{
  LEVEL = getPropertyDouble ("LEVEL");
  L = getPropertyDouble ("L");
  W = getPropertyDouble ("W");
  Np = getPropertyDouble ("Np");
  Ns = getPropertyDouble ("Ns");
  Cox = getPropertyDouble ("Cox");
  Xj = getPropertyDouble ("Xj");
  Dw = getPropertyDouble ("Dw");
  Dl = getPropertyDouble ("Dl");
  Vto = getPropertyDouble ("Vto");
  Gamma = getPropertyDouble ("Gamma");
  Phi = getPropertyDouble ("Phi");
  Kp = getPropertyDouble ("Kp");
  Theta = getPropertyDouble ("Theta");
  EO = getPropertyDouble ("EO");
  Ucrit = getPropertyDouble ("Ucrit");
  Lambda = getPropertyDouble ("Lambda");
  Weta = getPropertyDouble ("Weta");
  Leta = getPropertyDouble ("Leta");
  Q0 = getPropertyDouble ("Q0");
  Lk = getPropertyDouble ("Lk");
  Tcv = getPropertyDouble ("Tcv");
  Bex = getPropertyDouble ("Bex");
  Ucex = getPropertyDouble ("Ucex");
  Ibbt = getPropertyDouble ("Ibbt");
  Hdif = getPropertyDouble ("Hdif");
  Rsh = getPropertyDouble ("Rsh");
  Rsc = getPropertyDouble ("Rsc");
  Rdc = getPropertyDouble ("Rdc");
  Cgso = getPropertyDouble ("Cgso");
  Cgdo = getPropertyDouble ("Cgdo");
  Cgbo = getPropertyDouble ("Cgbo");
  Iba = getPropertyDouble ("Iba");
  Ibb = getPropertyDouble ("Ibb");
  Ibn = getPropertyDouble ("Ibn");
  Kf = getPropertyDouble ("Kf");
  Af = getPropertyDouble ("Af");
  Avto = getPropertyDouble ("Avto");
  Akp = getPropertyDouble ("Akp");
  Agamma = getPropertyDouble ("Agamma");
  N = getPropertyDouble ("N");
  Is = getPropertyDouble ("Is");
  Bv = getPropertyDouble ("Bv");
  Ibv = getPropertyDouble ("Ibv");
  Vj = getPropertyDouble ("Vj");
  Cj0 = getPropertyDouble ("Cj0");
  M = getPropertyDouble ("M");
  Area = getPropertyDouble ("Area");
  Fc = getPropertyDouble ("Fc");
  Tt = getPropertyDouble ("Tt");
  Xti = getPropertyDouble ("Xti");
  Xpart = getPropertyDouble ("Xpart");
  Tnom = getPropertyDouble ("Tnom");
  nmos = getPropertyInteger ("nmos");
  pmos = getPropertyInteger ("pmos");
}

/* #define's for translated code */
#undef  _DDT
#define _DDT(q) q
#define _DYNAMIC
#define _DERIVATE
#define _DDX
#define _DERIVATEFORDDX

/* Evaluate Verilog-AMS equations in model initialization. */
void EKV26MOS::initializeModel (void)
{
double Qb0;
double deltaV_RSCE;
double nnn;
double xi;
double C_epsilon;
double Kpa;
double Vtoa;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
#endif
double con2;
double Is_T2;
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
double F2;
#endif
#if defined(_DYNAMIC)
double F1;
#endif
#if defined(_DYNAMIC)
double Cj0_T2;
#endif
#if defined(_DYNAMIC)
#endif
#if defined(_DYNAMIC)
double Eg_T2;
#endif
double Eg_T1;
double Vt_T2;
double con1;
double Leff;
double Eg;
double Egnom;
double Vto_T;
double Tratio;
double T2;
double Tnomk;
double epsilonox;
double epsilonsi;
double B;
double A;
{
if
(_param_given("nmos"))
{
p_n_MOS=(+1);
}
else
{
if
(_param_given("pmos"))
{
p_n_MOS=(-1);
}
else
{
p_n_MOS=(+1);
}
}
A=7.02e-4;
B=1108.0;
epsilonsi=1.0359e-10;
#if defined(_DYNAMIC)
epsilonox=3.453143e-11;
#endif
Tnomk=(Tnom+273.15);
T2=_circuit_temp;
Tratio=(T2/Tnomk);
Vto_T=(p_n_MOS*(Vto-(Tcv*(T2-Tnomk))));
Egnom=(1.16-(((0.000702*Tnomk)*Tnomk)/(Tnomk+1108)));
Eg=(1.16-(((0.000702*T2)*T2)/(T2+1108)));
{
double m00_logE(d00_logE0,Tratio)
Phi_T=((((Phi*Tratio)-((3.0*_vt_nom)*d00_logE0))-(Egnom*Tratio))+Eg);
}
Ibb_T=(Ibb*(1.0+(Ibbt*(T2-Tnomk))));
Weff=(W+Dw);
Leff=(L+Dl);
RDeff=(((((Hdif*Rsh)/Weff)/Np)+Rdc)+1e-9);
RSeff=(((((Hdif*Rsh)/Weff)/Np)+Rsc)+1e-9);
{
double m00_sqrt(d00_sqrt0,(((Np*Weff)*Ns)*Leff))
con1=d00_sqrt0;
}
Vt_T2=((1.3806503e-23*T2)/1.602176462e-19);
Eg_T1=(Eg-(((A*Tnomk)*Tnomk)/(B+Tnomk)));
#if defined(_DYNAMIC)
Eg_T2=(Eg-(((A*T2)*T2)/(B+T2)));
#endif
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(T2/Tnomk),1.5)
double m00_logE(d00_logE1,d00_pow0)
Vj_T2=((((T2/Tnomk)*Vj)-((2*Vt_T2)*d00_logE1))-(((T2/Tnomk)*Eg_T1)-Eg_T2));
}
#endif
#if defined(_DYNAMIC)
Cj0_T2=(Cj0*(1+(M*((400e-6*(T2-Tnomk))-((Vj_T2-Vj)/Vj)))));
#endif
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
{
double m00_pow(d00_pow0,(T2/Tnomk),(Xti/N))
double m00_limexp(d00_limexp1,(((-Eg_T1)/Vt_T2)*(1-(T2/Tnomk))))
Is_T2=((Is*d00_pow0)*d00_limexp1);
}
GMIN=1e-12;
con2=((((Cox*Ns)*Np)*Weff)*Leff);
fourkt=((4.0*1.3806503e-23)*T2);
#if defined(_DYNAMIC)
Area1=(Area*((Cj0_T2*Vj_T2)/(1-M)));
#endif
Area2=(Area*Is_T2);
#if defined(_DYNAMIC)
Area3=(Area*Cj0_T2);
#endif
#if defined(_DYNAMIC)
DP1=(((Fc*Fc)*Vj_T2)*Vj_T2);
#endif
#if defined(_DYNAMIC)
DP2=(M/(2.0*Vj_T2));
#endif
#if defined(_DYNAMIC)
DP3=(Fc*Vj_T2);
#endif
#if defined(_DYNAMIC)
DP4=(F1+(1/F2));
#endif
con4=((-Vto_T)+Phi_T);
con7=(Gamma/2.0);
con8=(con7*con7);
con9=((16.0*_vt_nom)*_vt_nom);
con10=(epsilonsi/Cox);
con12=(0.1*_vt_nom);
con14=(4.0*_vt_nom);
con22=(Kp*(Weff/Leff));
con23=(Kf/con2);
#if defined(_DYNAMIC)
con24=(con2*_vt_nom);
#endif
#if defined(_DYNAMIC)
Spart=(1.0-Xpart);
#endif
con25=(fourkt/RDeff);
con26=(fourkt/RSeff);
#if defined(_DYNAMIC)
con27=((Cgso*Weff)*Np);
#endif
#if defined(_DYNAMIC)
con28=((Cgdo*Weff)*Np);
#endif
#if defined(_DYNAMIC)
con29=((Cgbo*Leff)*Np);
#endif
con30=(4.0/3.0);
con31=(1.0/(N*Vt_T2));
con32=((5.0*N)*_vt_nom);
#if defined(_DYNAMIC)
con33=(Fc*Vj);
#endif
if
((LEVEL==2))
{
{
double m00_pow(d00_pow0,Tratio,Ucex)
Ucrit_T=(Ucrit*d00_pow0);
}
#if defined(_DYNAMIC)
Vtoa=((p_n_MOS*Vto)+(Avto/con1));
#endif
Kpa=(Kp*(1.0+(Akp/con1)));
{
double m00_pow(d00_pow0,Tratio,Bex)
Kpa_T=(Kpa*d00_pow0);
}
Gammaa=(Gamma+(Agamma/con1));
{
double m00_pow(d00_pow0,22e-3,2)
C_epsilon=(4.0*d00_pow0);
}
xi=(0.028*((10.0*(Leff/Lk))-1.0));
{
double m00_pow(d00_pow0,xi,2)
double m00_sqrt(d00_sqrt1,(d00_pow0+C_epsilon))
nnn=(1.0+(0.5*(xi+d00_sqrt1)));
}
{
double m00_pow(d00_pow0,nnn,2)
deltaV_RSCE=(((2.0*Q0)/Cox)*(1.0/d00_pow0));
}
con3=(((-Vto_T)-deltaV_RSCE)+Phi_T);
con5=(Gammaa/2.0);
con6=(con5*con5);
con11=(Leta/Leff);
con13=((3.0*Weta)/Weff);
Vc=((Ucrit_T*Ns)*Leff);
{
double m00_logE(d00_logE0,(Vc/(2.0*_vt_nom)))
con15=(_vt_nom*(d00_logE0-0.6));
}
con16=(1.0/64.0);
{
double m00_sqrt(d00_sqrt0,((epsilonsi/Cox)*Xj))
Lc=d00_sqrt0;
}
Lmin=((Ns*Leff)/10.0);
eta=((p_n_MOS==(+1))?0.5:(1/3));
{
double m00_sqrt(d00_sqrt0,Phi_T)
Qb0=(Gammaa*d00_sqrt0);
}
con17=(1.0+((Cox/(EO*epsilonsi))*Qb0));
con18=(Lambda*Lc);
con19=(1.0/(Lc*Ucrit_T));
con20=(Ns*Leff);
con21=((Cox/(EO*epsilonsi))*_vt_nom);
}
}
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void EKV26MOS::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void EKV26MOS::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void EKV26MOS::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void EKV26MOS::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
#if defined(_DYNAMIC)
double Qs2;
#if defined(_DERIVATE)
double Qs2_VBulk_Drain_int;
double Qs2_VBulk_Source_int;
#endif
#endif
double Is2;
#if defined(_DERIVATE)
double Is2_VBulk_Drain_int;
double Is2_VBulk_Source_int;
#endif
#if defined(_DYNAMIC)
double Qs1;
#if defined(_DERIVATE)
double Qs1_VBulk_Drain_int;
double Qs1_VBulk_Source_int;
#endif
#endif
double Is1;
#if defined(_DERIVATE)
double Is1_VBulk_Drain_int;
double Is1_VBulk_Source_int;
#endif
#if defined(_DYNAMIC)
double Qd2;
#if defined(_DERIVATE)
double Qd2_VBulk_Source_int;
double Qd2_VBulk_Drain_int;
#endif
#endif
double Id2;
#if defined(_DERIVATE)
double Id2_VBulk_Source_int;
double Id2_VBulk_Drain_int;
#endif
#if defined(_DYNAMIC)
double Qd1;
#if defined(_DERIVATE)
double Qd1_VBulk_Source_int;
double Qd1_VBulk_Drain_int;
#endif
#endif
double Id1;
#if defined(_DERIVATE)
double Id1_VBulk_Source_int;
double Id1_VBulk_Drain_int;
#endif
double V2;
#if defined(_DERIVATE)
double V2_VBulk_Drain_int;
double V2_VBulk_Source_int;
#endif
double V1;
#if defined(_DERIVATE)
double V1_VBulk_Source_int;
double V1_VBulk_Drain_int;
#endif
#if defined(_DYNAMIC)
double qgbo;
#if defined(_DERIVATE)
double qgbo_VGate_Bulk;
#endif
#endif
#if defined(_DYNAMIC)
double qgdo;
#if defined(_DERIVATE)
double qgdo_VGate_Bulk;
double qgdo_VSource_int_Bulk;
double qgdo_VDrain_int_Bulk;
#endif
#endif
#if defined(_DYNAMIC)
double qgso;
#if defined(_DERIVATE)
double qgso_VGate_Bulk;
double qgso_VDrain_int_Bulk;
double qgso_VSource_int_Bulk;
#endif
#endif
#if defined(_DYNAMIC)
double qg;
#if defined(_DERIVATE)
double qg_VGate_Bulk;
double qg_VDrain_int_Bulk;
double qg_VSource_int_Bulk;
#endif
#endif
#if defined(_DYNAMIC)
double qb;
#if defined(_DERIVATE)
double qb_VGate_Bulk;
double qb_VDrain_int_Bulk;
double qb_VSource_int_Bulk;
#endif
#endif
double Sflicker;
#if defined(_DERIVATE)
double Sflicker_VSource_int_Bulk;
double Sflicker_VDrain_int_Bulk;
double Sflicker_VGate_Bulk;
#endif
double gm;
#if defined(_DERIVATE)
double gm_VSource_int_Bulk;
double gm_VDrain_int_Bulk;
double gm_VGate_Bulk;
#endif
double Sthermal;
#if defined(_DERIVATE)
double Sthermal_VSource_int_Bulk;
double Sthermal_VDrain_int_Bulk;
double Sthermal_VGate_Bulk;
#endif
double Idb;
#if defined(_DERIVATE)
double Idb_VGate_Bulk;
double Idb_VDrain_int_Bulk;
double Idb_VSource_int_Bulk;
#endif
double Vib;
#if defined(_DERIVATE)
double Vib_VSource_int_Bulk;
double Vib_VDrain_int_Bulk;
double Vib_VGate_Bulk;
#endif
double Ids;
#if defined(_DERIVATE)
double Ids_VGate_Bulk;
double Ids_VDrain_int_Bulk;
double Ids_VSource_int_Bulk;
#endif
double Ispecific;
#if defined(_DERIVATE)
double Ispecific_VGate_Bulk;
double Ispecific_VDrain_int_Bulk;
double Ispecific_VSource_int_Bulk;
#endif
double Beta;
#if defined(_DERIVATE)
double Beta_VSource_int_Bulk;
double Beta_VDrain_int_Bulk;
double Beta_VGate_Bulk;
#endif
double qB;
#if defined(_DERIVATE)
double qB_VGate_Bulk;
double qB_VDrain_int_Bulk;
double qB_VSource_int_Bulk;
#endif
double qI;
#if defined(_DERIVATE)
double qI_VGate_Bulk;
double qI_VDrain_int_Bulk;
double qI_VSource_int_Bulk;
#endif
double Xr;
#if defined(_DERIVATE)
double Xr_VGate_Bulk;
double Xr_VDrain_int_Bulk;
double Xr_VSource_int_Bulk;
#endif
double Xf;
#if defined(_DERIVATE)
double Xf_VGate_Bulk;
double Xf_VDrain_int_Bulk;
double Xf_VSource_int_Bulk;
#endif
double nq;
#if defined(_DERIVATE)
double nq_VGate_Bulk;
double nq_VDrain_int_Bulk;
double nq_VSource_int_Bulk;
#endif
double Beta0prime;
#if defined(_DERIVATE)
double Beta0prime_VSource_int_Bulk;
double Beta0prime_VDrain_int_Bulk;
double Beta0prime_VGate_Bulk;
#endif
double Beta0;
#if defined(_DERIVATE)
double Beta0_VSource_int_Bulk;
double Beta0_VDrain_int_Bulk;
double Beta0_VGate_Bulk;
#endif
double irprime;
#if defined(_DERIVATE)
double irprime_VGate_Bulk;
double irprime_VDrain_int_Bulk;
double irprime_VSource_int_Bulk;
#endif
double X3;
#if defined(_DERIVATE)
double X3_VGate_Bulk;
double X3_VDrain_int_Bulk;
double X3_VSource_int_Bulk;
#endif
double Leq;
#if defined(_DERIVATE)
double Leq_VSource_int_Bulk;
double Leq_VDrain_int_Bulk;
double Leq_VGate_Bulk;
#endif
double Lprime;
#if defined(_DERIVATE)
double Lprime_VSource_int_Bulk;
double Lprime_VDrain_int_Bulk;
double Lprime_VGate_Bulk;
#endif
double DeltaL;
#if defined(_DERIVATE)
double DeltaL_VSource_int_Bulk;
double DeltaL_VDrain_int_Bulk;
double DeltaL_VGate_Bulk;
#endif
double Vip;
#if defined(_DERIVATE)
double Vip_VGate_Bulk;
double Vip_VDrain_int_Bulk;
double Vip_VSource_int_Bulk;
#endif
double Vdso2;
#if defined(_DERIVATE)
double Vdso2_VSource_int_Bulk;
double Vdso2_VDrain_int_Bulk;
#endif
double deltaV;
#if defined(_DERIVATE)
double deltaV_VGate_Bulk;
double deltaV_VDrain_int_Bulk;
double deltaV_VSource_int_Bulk;
#endif
double Vdssprime;
#if defined(_DERIVATE)
double Vdssprime_VGate_Bulk;
double Vdssprime_VDrain_int_Bulk;
double Vdssprime_VSource_int_Bulk;
#endif
double Vdss;
#if defined(_DERIVATE)
double Vdss_VGate_Bulk;
double Vdss_VDrain_int_Bulk;
double Vdss_VSource_int_Bulk;
#endif
double ir;
#if defined(_DERIVATE)
double ir_VGate_Bulk;
double ir_VDrain_int_Bulk;
double ir_VSource_int_Bulk;
#endif
double X2;
#if defined(_DERIVATE)
double X2_VGate_Bulk;
double X2_VDrain_int_Bulk;
double X2_VSource_int_Bulk;
#endif
double iff;
#if defined(_DERIVATE)
double iff_VGate_Bulk;
double iff_VDrain_int_Bulk;
double iff_VSource_int_Bulk;
#endif
double T_1;
#if defined(_DERIVATE)
double T_1_VGate_Bulk;
double T_1_VDrain_int_Bulk;
double T_1_VSource_int_Bulk;
#endif
double X1;
#if defined(_DERIVATE)
double X1_VGate_Bulk;
double X1_VDrain_int_Bulk;
double X1_VSource_int_Bulk;
#endif
double n;
#if defined(_DERIVATE)
double n_VGate_Bulk;
double n_VDrain_int_Bulk;
double n_VSource_int_Bulk;
#endif
double Vp;
#if defined(_DERIVATE)
double Vp_VGate_Bulk;
double Vp_VDrain_int_Bulk;
double Vp_VSource_int_Bulk;
#endif
double Gammaprime;
#if defined(_DERIVATE)
double Gammaprime_VDrain_int_Bulk;
double Gammaprime_VSource_int_Bulk;
double Gammaprime_VGate_Bulk;
#endif
double Gamma0;
#if defined(_DERIVATE)
double Gamma0_VDrain_int_Bulk;
double Gamma0_VSource_int_Bulk;
double Gamma0_VGate_Bulk;
#endif
double VDprime;
#if defined(_DERIVATE)
double VDprime_VSource_int_Bulk;
double VDprime_VDrain_int_Bulk;
#endif
double VSprime;
#if defined(_DERIVATE)
double VSprime_VDrain_int_Bulk;
double VSprime_VSource_int_Bulk;
#endif
double VP0;
#if defined(_DERIVATE)
double VP0_VGate_Bulk;
#endif
double VGprime;
#if defined(_DERIVATE)
double VGprime_VGate_Bulk;
#endif
double VD;
#if defined(_DERIVATE)
double VD_VSource_int_Bulk;
double VD_VDrain_int_Bulk;
#endif
double VS;
#if defined(_DERIVATE)
double VS_VDrain_int_Bulk;
double VS_VSource_int_Bulk;
#endif
double StoDswap;
double VG;
#if defined(_DERIVATE)
double VG_VGate_Bulk;
#endif
double Vd;
#if defined(_DERIVATE)
double Vd_VDrain_int_Bulk;
#endif
double Vs;
#if defined(_DERIVATE)
double Vs_VSource_int_Bulk;
#endif
double Vg;
#if defined(_DERIVATE)
double Vg_VGate_Bulk;
#endif
#if defined(_DERIVATE)
Vg_VGate_Bulk=(p_n_MOS);
#endif
Vg=(p_n_MOS*BP(Gate,Bulk));
#if defined(_DERIVATE)
Vs_VSource_int_Bulk=(p_n_MOS);
#endif
Vs=(p_n_MOS*BP(Source_int,Bulk));
#if defined(_DERIVATE)
Vd_VDrain_int_Bulk=(p_n_MOS);
#endif
Vd=(p_n_MOS*BP(Drain_int,Bulk));
#if defined(_DERIVATE)
VG_VGate_Bulk=Vg_VGate_Bulk;
#endif
VG=Vg;
if
(((Vd-Vs)>=0.0))
{
StoDswap=1.0;
#if defined(_DERIVATE)
VS_VDrain_int_Bulk=0.0;
VS_VSource_int_Bulk=Vs_VSource_int_Bulk;
#endif
VS=Vs;
#if defined(_DERIVATE)
VD_VSource_int_Bulk=0.0;
VD_VDrain_int_Bulk=Vd_VDrain_int_Bulk;
#endif
VD=Vd;
}
else
{
StoDswap=(-1.0);
#if defined(_DERIVATE)
VD_VSource_int_Bulk=Vs_VSource_int_Bulk;
VD_VDrain_int_Bulk=0.0;
#endif
VD=Vs;
#if defined(_DERIVATE)
VS_VDrain_int_Bulk=Vd_VDrain_int_Bulk;
VS_VSource_int_Bulk=0.0;
#endif
VS=Vd;
}
if
((LEVEL==2))
{
{
double m00_sqrt(d00_sqrt0,Phi_T)
#if defined(_DERIVATE)
VGprime_VGate_Bulk=VG_VGate_Bulk;
#endif
VGprime=((VG+con3)+(Gammaa*d00_sqrt0));
}
}
else
{
{
double m00_sqrt(d00_sqrt0,Phi_T)
#if defined(_DERIVATE)
VGprime_VGate_Bulk=VG_VGate_Bulk;
#endif
VGprime=((VG+con4)+(Gamma*d00_sqrt0));
}
}
if
((LEVEL==2))
{
if
((VGprime>0))
{
{
double m00_sqrt(d00_sqrt0,(VGprime+con6))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(VGprime+con6))
#endif
#if defined(_DERIVATE)
VP0_VGate_Bulk=(VGprime_VGate_Bulk-(Gammaa*VGprime_VGate_Bulk*d10_sqrt0));
#endif
VP0=((VGprime-Phi_T)-(Gammaa*(d00_sqrt0-con5)));
}
}
else
{
#if defined(_DERIVATE)
VP0_VGate_Bulk=0.0;
#endif
VP0=(-Phi_T);
}
{
double m00_sqrt(d00_sqrt0,(((VS+Phi_T)*(VS+Phi_T))+con9))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(((VS+Phi_T)*(VS+Phi_T))+con9))
#endif
#if defined(_DERIVATE)
VSprime_VDrain_int_Bulk=(0.5*(VS_VDrain_int_Bulk+((VS_VDrain_int_Bulk*(VS+Phi_T))+((VS+Phi_T)*VS_VDrain_int_Bulk))*d10_sqrt0));
VSprime_VSource_int_Bulk=(0.5*(VS_VSource_int_Bulk+((VS_VSource_int_Bulk*(VS+Phi_T))+((VS+Phi_T)*VS_VSource_int_Bulk))*d10_sqrt0));
#endif
VSprime=(0.5*((VS+Phi_T)+d00_sqrt0));
}
{
double m00_sqrt(d00_sqrt0,(((VD+Phi_T)*(VD+Phi_T))+con9))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(((VD+Phi_T)*(VD+Phi_T))+con9))
#endif
#if defined(_DERIVATE)
VDprime_VSource_int_Bulk=(0.5*(VD_VSource_int_Bulk+((VD_VSource_int_Bulk*(VD+Phi_T))+((VD+Phi_T)*VD_VSource_int_Bulk))*d10_sqrt0));
VDprime_VDrain_int_Bulk=(0.5*(VD_VDrain_int_Bulk+((VD_VDrain_int_Bulk*(VD+Phi_T))+((VD+Phi_T)*VD_VDrain_int_Bulk))*d10_sqrt0));
#endif
VDprime=(0.5*((VD+Phi_T)+d00_sqrt0));
}
{
double m00_sqrt(d00_sqrt0,VSprime)
double m00_sqrt(d00_sqrt1,VDprime)
double m00_sqrt(d00_sqrt2,(VP0+Phi_T))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,VSprime)
double m10_sqrt(d10_sqrt1,d00_sqrt1,VDprime)
double m10_sqrt(d10_sqrt2,d00_sqrt2,(VP0+Phi_T))
#endif
#if defined(_DERIVATE)
Gamma0_VDrain_int_Bulk=(-(con10*(con11*(VSprime_VDrain_int_Bulk*d10_sqrt0+VDprime_VDrain_int_Bulk*d10_sqrt1))));
Gamma0_VSource_int_Bulk=(-(con10*(con11*(VSprime_VSource_int_Bulk*d10_sqrt0+VDprime_VSource_int_Bulk*d10_sqrt1))));
Gamma0_VGate_Bulk=(-(con10*(-(con13*VP0_VGate_Bulk*d10_sqrt2))));
#endif
Gamma0=(Gammaa-(con10*((con11*(d00_sqrt0+d00_sqrt1))-(con13*d00_sqrt2))));
}
{
double m00_sqrt(d00_sqrt0,((Gamma0*Gamma0)+con12))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Gamma0*Gamma0)+con12))
#endif
#if defined(_DERIVATE)
Gammaprime_VDrain_int_Bulk=(0.5*(Gamma0_VDrain_int_Bulk+((Gamma0_VDrain_int_Bulk*Gamma0)+(Gamma0*Gamma0_VDrain_int_Bulk))*d10_sqrt0));
Gammaprime_VSource_int_Bulk=(0.5*(Gamma0_VSource_int_Bulk+((Gamma0_VSource_int_Bulk*Gamma0)+(Gamma0*Gamma0_VSource_int_Bulk))*d10_sqrt0));
Gammaprime_VGate_Bulk=(0.5*(Gamma0_VGate_Bulk+((Gamma0_VGate_Bulk*Gamma0)+(Gamma0*Gamma0_VGate_Bulk))*d10_sqrt0));
#endif
Gammaprime=(0.5*(Gamma0+d00_sqrt0));
}
if
((VGprime>0.0))
{
{
double m00_sqrt(d00_sqrt0,(VGprime+((0.25*Gammaprime)*Gammaprime)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(VGprime+((0.25*Gammaprime)*Gammaprime)))
#endif
#if defined(_DERIVATE)
Vp_VGate_Bulk=(VGprime_VGate_Bulk-((Gammaprime_VGate_Bulk*(d00_sqrt0-(0.5*Gammaprime)))+(Gammaprime*((VGprime_VGate_Bulk+(((0.25*Gammaprime_VGate_Bulk)*Gammaprime)+((0.25*Gammaprime)*Gammaprime_VGate_Bulk)))*d10_sqrt0-(0.5*Gammaprime_VGate_Bulk)))));
Vp_VDrain_int_Bulk=(-((Gammaprime_VDrain_int_Bulk*(d00_sqrt0-(0.5*Gammaprime)))+(Gammaprime*((((0.25*Gammaprime_VDrain_int_Bulk)*Gammaprime)+((0.25*Gammaprime)*Gammaprime_VDrain_int_Bulk))*d10_sqrt0-(0.5*Gammaprime_VDrain_int_Bulk)))));
Vp_VSource_int_Bulk=(-((Gammaprime_VSource_int_Bulk*(d00_sqrt0-(0.5*Gammaprime)))+(Gammaprime*((((0.25*Gammaprime_VSource_int_Bulk)*Gammaprime)+((0.25*Gammaprime)*Gammaprime_VSource_int_Bulk))*d10_sqrt0-(0.5*Gammaprime_VSource_int_Bulk)))));
#endif
Vp=((VGprime-Phi_T)-(Gammaprime*(d00_sqrt0-(0.5*Gammaprime))));
}
}
else
{
#if defined(_DERIVATE)
Vp_VGate_Bulk=0.0;
Vp_VDrain_int_Bulk=0.0;
Vp_VSource_int_Bulk=0.0;
#endif
Vp=(-Phi_T);
}
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+con14))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+con14))
#endif
#if defined(_DERIVATE)
n_VGate_Bulk=(-Gammaa*(2.0*Vp_VGate_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
n_VDrain_int_Bulk=(-Gammaa*(2.0*Vp_VDrain_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
n_VSource_int_Bulk=(-Gammaa*(2.0*Vp_VSource_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
#endif
n=(1.0+(Gammaa/(2.0*d00_sqrt0)));
}
}
else
{
if
((VGprime>0))
{
{
double m00_sqrt(d00_sqrt0,(VGprime+con8))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(VGprime+con8))
#endif
#if defined(_DERIVATE)
Vp_VGate_Bulk=(VGprime_VGate_Bulk-(Gamma*VGprime_VGate_Bulk*d10_sqrt0));
Vp_VDrain_int_Bulk=0.0;
Vp_VSource_int_Bulk=0.0;
#endif
Vp=((VGprime-Phi_T)-(Gamma*(d00_sqrt0-con7)));
}
}
else
{
#if defined(_DERIVATE)
Vp_VGate_Bulk=0.0;
Vp_VDrain_int_Bulk=0.0;
Vp_VSource_int_Bulk=0.0;
#endif
Vp=(-Phi_T);
}
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+con14))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+con14))
#endif
#if defined(_DERIVATE)
n_VGate_Bulk=(-Gamma*(2.0*Vp_VGate_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
n_VDrain_int_Bulk=(-Gamma*(2.0*Vp_VDrain_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
n_VSource_int_Bulk=(-Gamma*(2.0*Vp_VSource_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
#endif
n=(1.0+(Gamma/(2.0*d00_sqrt0)));
}
}
#if defined(_DERIVATE)
X1_VGate_Bulk=(Vp_VGate_Bulk/_vt_nom);
X1_VDrain_int_Bulk=((Vp_VDrain_int_Bulk-VS_VDrain_int_Bulk)/_vt_nom);
X1_VSource_int_Bulk=((Vp_VSource_int_Bulk-VS_VSource_int_Bulk)/_vt_nom);
#endif
X1=((Vp-VS)/_vt_nom);
{
double m00_limexp(d00_limexp0,(X1/2.0))
double m00_logE(d00_logE1,(1.0+d00_limexp0))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(X1/2.0))
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_limexp0))
#endif
#if defined(_DERIVATE)
T_1_VGate_Bulk=(X1_VGate_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VDrain_int_Bulk=(X1_VDrain_int_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VSource_int_Bulk=(X1_VSource_int_Bulk/2.0)*d10_limexp0*d10_logE1;
#endif
T_1=d00_logE1;
}
#if defined(_DERIVATE)
iff_VGate_Bulk=((T_1_VGate_Bulk*T_1)+(T_1*T_1_VGate_Bulk));
iff_VDrain_int_Bulk=((T_1_VDrain_int_Bulk*T_1)+(T_1*T_1_VDrain_int_Bulk));
iff_VSource_int_Bulk=((T_1_VSource_int_Bulk*T_1)+(T_1*T_1_VSource_int_Bulk));
#endif
iff=(T_1*T_1);
#if defined(_DERIVATE)
X2_VGate_Bulk=(Vp_VGate_Bulk/_vt_nom);
X2_VDrain_int_Bulk=((Vp_VDrain_int_Bulk-VD_VDrain_int_Bulk)/_vt_nom);
X2_VSource_int_Bulk=((Vp_VSource_int_Bulk-VD_VSource_int_Bulk)/_vt_nom);
#endif
X2=((Vp-VD)/_vt_nom);
{
double m00_limexp(d00_limexp0,(X2/2.0))
double m00_logE(d00_logE1,(1.0+d00_limexp0))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(X2/2.0))
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_limexp0))
#endif
#if defined(_DERIVATE)
T_1_VGate_Bulk=(X2_VGate_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VDrain_int_Bulk=(X2_VDrain_int_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VSource_int_Bulk=(X2_VSource_int_Bulk/2.0)*d10_limexp0*d10_logE1;
#endif
T_1=d00_logE1;
}
#if defined(_DERIVATE)
ir_VGate_Bulk=((T_1_VGate_Bulk*T_1)+(T_1*T_1_VGate_Bulk));
ir_VDrain_int_Bulk=((T_1_VDrain_int_Bulk*T_1)+(T_1*T_1_VDrain_int_Bulk));
ir_VSource_int_Bulk=((T_1_VSource_int_Bulk*T_1)+(T_1*T_1_VSource_int_Bulk));
#endif
ir=(T_1*T_1);
if
((LEVEL==2))
{
{
double m00_sqrt(d00_sqrt0,iff)
double m00_sqrt(d00_sqrt1,(0.25+((_vt_nom/Vc)*d00_sqrt0)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,iff)
double m10_sqrt(d10_sqrt1,d00_sqrt1,(0.25+((_vt_nom/Vc)*d00_sqrt0)))
#endif
#if defined(_DERIVATE)
Vdss_VGate_Bulk=(Vc*((_vt_nom/Vc)*iff_VGate_Bulk*d10_sqrt0)*d10_sqrt1);
Vdss_VDrain_int_Bulk=(Vc*((_vt_nom/Vc)*iff_VDrain_int_Bulk*d10_sqrt0)*d10_sqrt1);
Vdss_VSource_int_Bulk=(Vc*((_vt_nom/Vc)*iff_VSource_int_Bulk*d10_sqrt0)*d10_sqrt1);
#endif
Vdss=(Vc*(d00_sqrt1-0.5));
}
{
double m00_sqrt(d00_sqrt0,iff)
double m00_logE(d00_logE1,iff)
double m00_sqrt(d00_sqrt2,(0.25+((_vt_nom/Vc)*(d00_sqrt0-(0.75*d00_logE1)))))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,iff)
double m10_logE(d10_logE1,d00_logE1,iff)
double m10_sqrt(d10_sqrt2,d00_sqrt2,(0.25+((_vt_nom/Vc)*(d00_sqrt0-(0.75*d00_logE1)))))
#endif
#if defined(_DERIVATE)
Vdssprime_VGate_Bulk=(Vc*((_vt_nom/Vc)*(iff_VGate_Bulk*d10_sqrt0-(0.75*iff_VGate_Bulk*d10_logE1)))*d10_sqrt2);
Vdssprime_VDrain_int_Bulk=(Vc*((_vt_nom/Vc)*(iff_VDrain_int_Bulk*d10_sqrt0-(0.75*iff_VDrain_int_Bulk*d10_logE1)))*d10_sqrt2);
Vdssprime_VSource_int_Bulk=(Vc*((_vt_nom/Vc)*(iff_VSource_int_Bulk*d10_sqrt0-(0.75*iff_VSource_int_Bulk*d10_logE1)))*d10_sqrt2);
#endif
Vdssprime=((Vc*(d00_sqrt2-0.5))+con15);
}
{
double m00_sqrt(d00_sqrt0,iff)
if
((Lambda*(d00_sqrt0>(Vdss/_vt_nom))))
{
{
double m00_sqrt(d00_sqrt0,iff)
double m00_sqrt(d00_sqrt1,((Lambda*(d00_sqrt0-(Vdss/_vt_nom)))+con16))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,iff)
double m10_sqrt(d10_sqrt1,d00_sqrt1,((Lambda*(d00_sqrt0-(Vdss/_vt_nom)))+con16))
#endif
#if defined(_DERIVATE)
deltaV_VGate_Bulk=(con14*(Lambda*(iff_VGate_Bulk*d10_sqrt0-(Vdss_VGate_Bulk/_vt_nom)))*d10_sqrt1);
deltaV_VDrain_int_Bulk=(con14*(Lambda*(iff_VDrain_int_Bulk*d10_sqrt0-(Vdss_VDrain_int_Bulk/_vt_nom)))*d10_sqrt1);
deltaV_VSource_int_Bulk=(con14*(Lambda*(iff_VSource_int_Bulk*d10_sqrt0-(Vdss_VSource_int_Bulk/_vt_nom)))*d10_sqrt1);
#endif
deltaV=(con14*d00_sqrt1);
}
}
else
{
#if defined(_DERIVATE)
deltaV_VGate_Bulk=0.0;
deltaV_VDrain_int_Bulk=0.0;
deltaV_VSource_int_Bulk=0.0;
#endif
deltaV=con16;
}
}
#if defined(_DERIVATE)
Vdso2_VSource_int_Bulk=(0.5*(VD_VSource_int_Bulk-VS_VSource_int_Bulk));
Vdso2_VDrain_int_Bulk=(0.5*(VD_VDrain_int_Bulk-VS_VDrain_int_Bulk));
#endif
Vdso2=(0.5*(VD-VS));
{
double m00_sqrt(d00_sqrt0,((Vdss*Vdss)+(deltaV*deltaV)))
double m00_sqrt(d00_sqrt1,(((Vdso2-Vdss)*(Vdso2-Vdss))+(deltaV*deltaV)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vdss*Vdss)+(deltaV*deltaV)))
double m10_sqrt(d10_sqrt1,d00_sqrt1,(((Vdso2-Vdss)*(Vdso2-Vdss))+(deltaV*deltaV)))
#endif
#if defined(_DERIVATE)
Vip_VGate_Bulk=((((Vdss_VGate_Bulk*Vdss)+(Vdss*Vdss_VGate_Bulk))+((deltaV_VGate_Bulk*deltaV)+(deltaV*deltaV_VGate_Bulk)))*d10_sqrt0-((((-Vdss_VGate_Bulk)*(Vdso2-Vdss))+((Vdso2-Vdss)*(-Vdss_VGate_Bulk)))+((deltaV_VGate_Bulk*deltaV)+(deltaV*deltaV_VGate_Bulk)))*d10_sqrt1);
Vip_VDrain_int_Bulk=((((Vdss_VDrain_int_Bulk*Vdss)+(Vdss*Vdss_VDrain_int_Bulk))+((deltaV_VDrain_int_Bulk*deltaV)+(deltaV*deltaV_VDrain_int_Bulk)))*d10_sqrt0-((((Vdso2_VDrain_int_Bulk-Vdss_VDrain_int_Bulk)*(Vdso2-Vdss))+((Vdso2-Vdss)*(Vdso2_VDrain_int_Bulk-Vdss_VDrain_int_Bulk)))+((deltaV_VDrain_int_Bulk*deltaV)+(deltaV*deltaV_VDrain_int_Bulk)))*d10_sqrt1);
Vip_VSource_int_Bulk=((((Vdss_VSource_int_Bulk*Vdss)+(Vdss*Vdss_VSource_int_Bulk))+((deltaV_VSource_int_Bulk*deltaV)+(deltaV*deltaV_VSource_int_Bulk)))*d10_sqrt0-((((Vdso2_VSource_int_Bulk-Vdss_VSource_int_Bulk)*(Vdso2-Vdss))+((Vdso2-Vdss)*(Vdso2_VSource_int_Bulk-Vdss_VSource_int_Bulk)))+((deltaV_VSource_int_Bulk*deltaV)+(deltaV*deltaV_VSource_int_Bulk)))*d10_sqrt1);
#endif
Vip=(d00_sqrt0-d00_sqrt1);
}
{
double m00_logE(d00_logE0,(1.0+((Vdso2-Vip)*con19)))
#if defined(_DERIVATE)
double m10_logE(d10_logE0,d00_logE0,(1.0+((Vdso2-Vip)*con19)))
#endif
#if defined(_DERIVATE)
DeltaL_VSource_int_Bulk=(con18*(Vdso2_VSource_int_Bulk-Vip_VSource_int_Bulk)*con19*d10_logE0);
DeltaL_VDrain_int_Bulk=(con18*(Vdso2_VDrain_int_Bulk-Vip_VDrain_int_Bulk)*con19*d10_logE0);
DeltaL_VGate_Bulk=(con18*(-Vip_VGate_Bulk)*con19*d10_logE0);
#endif
DeltaL=(con18*d00_logE0);
}
#if defined(_DERIVATE)
Lprime_VSource_int_Bulk=((-DeltaL_VSource_int_Bulk)+((Vdso2_VSource_int_Bulk+Vip_VSource_int_Bulk)/Ucrit_T));
Lprime_VDrain_int_Bulk=((-DeltaL_VDrain_int_Bulk)+((Vdso2_VDrain_int_Bulk+Vip_VDrain_int_Bulk)/Ucrit_T));
Lprime_VGate_Bulk=((-DeltaL_VGate_Bulk)+(Vip_VGate_Bulk/Ucrit_T));
#endif
Lprime=((con20-DeltaL)+((Vdso2+Vip)/Ucrit_T));
{
double m00_sqrt(d00_sqrt0,((Lprime*Lprime)+(Lmin*Lmin)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Lprime*Lprime)+(Lmin*Lmin)))
#endif
#if defined(_DERIVATE)
Leq_VSource_int_Bulk=(0.5*(Lprime_VSource_int_Bulk+((Lprime_VSource_int_Bulk*Lprime)+(Lprime*Lprime_VSource_int_Bulk))*d10_sqrt0));
Leq_VDrain_int_Bulk=(0.5*(Lprime_VDrain_int_Bulk+((Lprime_VDrain_int_Bulk*Lprime)+(Lprime*Lprime_VDrain_int_Bulk))*d10_sqrt0));
Leq_VGate_Bulk=(0.5*(Lprime_VGate_Bulk+((Lprime_VGate_Bulk*Lprime)+(Lprime*Lprime_VGate_Bulk))*d10_sqrt0));
#endif
Leq=(0.5*(Lprime+d00_sqrt0));
}
{
double m00_sqrt(d00_sqrt0,((Vdssprime*Vdssprime)+(deltaV*deltaV)))
double m00_sqrt(d00_sqrt1,(((Vdso2-Vdssprime)*(Vdso2-Vdssprime))+(deltaV*deltaV)))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vdssprime*Vdssprime)+(deltaV*deltaV)))
double m10_sqrt(d10_sqrt1,d00_sqrt1,(((Vdso2-Vdssprime)*(Vdso2-Vdssprime))+(deltaV*deltaV)))
#endif
#if defined(_DERIVATE)
X3_VGate_Bulk=(((Vp_VGate_Bulk-(((Vdssprime_VGate_Bulk*Vdssprime)+(Vdssprime*Vdssprime_VGate_Bulk))+((deltaV_VGate_Bulk*deltaV)+(deltaV*deltaV_VGate_Bulk)))*d10_sqrt0)+((((-Vdssprime_VGate_Bulk)*(Vdso2-Vdssprime))+((Vdso2-Vdssprime)*(-Vdssprime_VGate_Bulk)))+((deltaV_VGate_Bulk*deltaV)+(deltaV*deltaV_VGate_Bulk)))*d10_sqrt1)/_vt_nom);
X3_VDrain_int_Bulk=(((((Vp_VDrain_int_Bulk-Vdso2_VDrain_int_Bulk)-VS_VDrain_int_Bulk)-(((Vdssprime_VDrain_int_Bulk*Vdssprime)+(Vdssprime*Vdssprime_VDrain_int_Bulk))+((deltaV_VDrain_int_Bulk*deltaV)+(deltaV*deltaV_VDrain_int_Bulk)))*d10_sqrt0)+((((Vdso2_VDrain_int_Bulk-Vdssprime_VDrain_int_Bulk)*(Vdso2-Vdssprime))+((Vdso2-Vdssprime)*(Vdso2_VDrain_int_Bulk-Vdssprime_VDrain_int_Bulk)))+((deltaV_VDrain_int_Bulk*deltaV)+(deltaV*deltaV_VDrain_int_Bulk)))*d10_sqrt1)/_vt_nom);
X3_VSource_int_Bulk=(((((Vp_VSource_int_Bulk-Vdso2_VSource_int_Bulk)-VS_VSource_int_Bulk)-(((Vdssprime_VSource_int_Bulk*Vdssprime)+(Vdssprime*Vdssprime_VSource_int_Bulk))+((deltaV_VSource_int_Bulk*deltaV)+(deltaV*deltaV_VSource_int_Bulk)))*d10_sqrt0)+((((Vdso2_VSource_int_Bulk-Vdssprime_VSource_int_Bulk)*(Vdso2-Vdssprime))+((Vdso2-Vdssprime)*(Vdso2_VSource_int_Bulk-Vdssprime_VSource_int_Bulk)))+((deltaV_VSource_int_Bulk*deltaV)+(deltaV*deltaV_VSource_int_Bulk)))*d10_sqrt1)/_vt_nom);
#endif
X3=(((((Vp-Vdso2)-VS)-d00_sqrt0)+d00_sqrt1)/_vt_nom);
}
{
double m00_limexp(d00_limexp0,(X3/2.0))
double m00_logE(d00_logE1,(1.0+d00_limexp0))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(X3/2.0))
double m10_logE(d10_logE1,d00_logE1,(1.0+d00_limexp0))
#endif
#if defined(_DERIVATE)
T_1_VGate_Bulk=(X3_VGate_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VDrain_int_Bulk=(X3_VDrain_int_Bulk/2.0)*d10_limexp0*d10_logE1;
T_1_VSource_int_Bulk=(X3_VSource_int_Bulk/2.0)*d10_limexp0*d10_logE1;
#endif
T_1=d00_logE1;
}
#if defined(_DERIVATE)
irprime_VGate_Bulk=((T_1_VGate_Bulk*T_1)+(T_1*T_1_VGate_Bulk));
irprime_VDrain_int_Bulk=((T_1_VDrain_int_Bulk*T_1)+(T_1*T_1_VDrain_int_Bulk));
irprime_VSource_int_Bulk=((T_1_VSource_int_Bulk*T_1)+(T_1*T_1_VSource_int_Bulk));
#endif
irprime=(T_1*T_1);
#if defined(_DERIVATE)
Beta0_VSource_int_Bulk=(Kpa_T*(-(Np*Weff)*Leq_VSource_int_Bulk/Leq/Leq));
Beta0_VDrain_int_Bulk=(Kpa_T*(-(Np*Weff)*Leq_VDrain_int_Bulk/Leq/Leq));
Beta0_VGate_Bulk=(Kpa_T*(-(Np*Weff)*Leq_VGate_Bulk/Leq/Leq));
#endif
Beta0=(Kpa_T*((Np*Weff)/Leq));
#if defined(_DERIVATE)
Beta0prime_VSource_int_Bulk=Beta0_VSource_int_Bulk*con17;
Beta0prime_VDrain_int_Bulk=Beta0_VDrain_int_Bulk*con17;
Beta0prime_VGate_Bulk=Beta0_VGate_Bulk*con17;
#endif
Beta0prime=(Beta0*con17);
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+1e-6))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+1e-6))
#endif
#if defined(_DERIVATE)
nq_VGate_Bulk=(-Gammaa*(2.0*Vp_VGate_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
nq_VDrain_int_Bulk=(-Gammaa*(2.0*Vp_VDrain_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
nq_VSource_int_Bulk=(-Gammaa*(2.0*Vp_VSource_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
#endif
nq=(1.0+(Gammaa/(2.0*d00_sqrt0)));
}
}
else
{
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+1e-6))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+1e-6))
#endif
#if defined(_DERIVATE)
nq_VGate_Bulk=(-Gamma*(2.0*Vp_VGate_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
nq_VDrain_int_Bulk=(-Gamma*(2.0*Vp_VDrain_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
nq_VSource_int_Bulk=(-Gamma*(2.0*Vp_VSource_int_Bulk*d10_sqrt0)/(2.0*d00_sqrt0)/(2.0*d00_sqrt0));
#endif
nq=(1.0+(Gamma/(2.0*d00_sqrt0)));
}
}
{
double m00_sqrt(d00_sqrt0,(0.25+iff))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(0.25+iff))
#endif
#if defined(_DERIVATE)
Xf_VGate_Bulk=iff_VGate_Bulk*d10_sqrt0;
Xf_VDrain_int_Bulk=iff_VDrain_int_Bulk*d10_sqrt0;
Xf_VSource_int_Bulk=iff_VSource_int_Bulk*d10_sqrt0;
#endif
Xf=d00_sqrt0;
}
{
double m00_sqrt(d00_sqrt0,(0.25+ir))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,(0.25+ir))
#endif
#if defined(_DERIVATE)
Xr_VGate_Bulk=ir_VGate_Bulk*d10_sqrt0;
Xr_VDrain_int_Bulk=ir_VDrain_int_Bulk*d10_sqrt0;
Xr_VSource_int_Bulk=ir_VSource_int_Bulk*d10_sqrt0;
#endif
Xr=d00_sqrt0;
}
#if defined(_DERIVATE)
qI_VGate_Bulk=(((-nq_VGate_Bulk)*(((con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))/(Xf+Xr))-1.0))+((-nq)*(((con30*((((Xf_VGate_Bulk*Xf)+(Xf*Xf_VGate_Bulk))+((Xf_VGate_Bulk*Xr)+(Xf*Xr_VGate_Bulk)))+((Xr_VGate_Bulk*Xr)+(Xr*Xr_VGate_Bulk))))*(Xf+Xr)-(con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))*(Xf_VGate_Bulk+Xr_VGate_Bulk))/(Xf+Xr)/(Xf+Xr))));
qI_VDrain_int_Bulk=(((-nq_VDrain_int_Bulk)*(((con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))/(Xf+Xr))-1.0))+((-nq)*(((con30*((((Xf_VDrain_int_Bulk*Xf)+(Xf*Xf_VDrain_int_Bulk))+((Xf_VDrain_int_Bulk*Xr)+(Xf*Xr_VDrain_int_Bulk)))+((Xr_VDrain_int_Bulk*Xr)+(Xr*Xr_VDrain_int_Bulk))))*(Xf+Xr)-(con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))*(Xf_VDrain_int_Bulk+Xr_VDrain_int_Bulk))/(Xf+Xr)/(Xf+Xr))));
qI_VSource_int_Bulk=(((-nq_VSource_int_Bulk)*(((con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))/(Xf+Xr))-1.0))+((-nq)*(((con30*((((Xf_VSource_int_Bulk*Xf)+(Xf*Xf_VSource_int_Bulk))+((Xf_VSource_int_Bulk*Xr)+(Xf*Xr_VSource_int_Bulk)))+((Xr_VSource_int_Bulk*Xr)+(Xr*Xr_VSource_int_Bulk))))*(Xf+Xr)-(con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))*(Xf_VSource_int_Bulk+Xr_VSource_int_Bulk))/(Xf+Xr)/(Xf+Xr))));
#endif
qI=((-nq)*(((con30*(((Xf*Xf)+(Xf*Xr))+(Xr*Xr)))/(Xf+Xr))-1.0));
if
((LEVEL==2))
{
if
((VGprime>0))
{
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+1e-6))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+1e-6))
#endif
#if defined(_DERIVATE)
qB_VGate_Bulk=((((-Gammaa)*Vp_VGate_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VGate_Bulk*(nq-1.0))+(qI*nq_VGate_Bulk))*nq-(qI*(nq-1.0))*nq_VGate_Bulk)/nq/nq));
qB_VDrain_int_Bulk=((((-Gammaa)*Vp_VDrain_int_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VDrain_int_Bulk*(nq-1.0))+(qI*nq_VDrain_int_Bulk))*nq-(qI*(nq-1.0))*nq_VDrain_int_Bulk)/nq/nq));
qB_VSource_int_Bulk=((((-Gammaa)*Vp_VSource_int_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VSource_int_Bulk*(nq-1.0))+(qI*nq_VSource_int_Bulk))*nq-(qI*(nq-1.0))*nq_VSource_int_Bulk)/nq/nq));
#endif
qB=((((-Gammaa)*d00_sqrt0)/_vt_nom)-((qI*(nq-1.0))/nq));
}
}
else
{
#if defined(_DERIVATE)
qB_VGate_Bulk=((-VGprime_VGate_Bulk)/_vt_nom);
qB_VDrain_int_Bulk=0.0;
qB_VSource_int_Bulk=0.0;
#endif
qB=((-VGprime)/_vt_nom);
}
}
else
{
if
((VGprime>0))
{
{
double m00_sqrt(d00_sqrt0,((Vp+Phi_T)+1e-6))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((Vp+Phi_T)+1e-6))
#endif
#if defined(_DERIVATE)
qB_VGate_Bulk=((((-Gamma)*Vp_VGate_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VGate_Bulk*(nq-1.0))+(qI*nq_VGate_Bulk))*nq-(qI*(nq-1.0))*nq_VGate_Bulk)/nq/nq));
qB_VDrain_int_Bulk=((((-Gamma)*Vp_VDrain_int_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VDrain_int_Bulk*(nq-1.0))+(qI*nq_VDrain_int_Bulk))*nq-(qI*(nq-1.0))*nq_VDrain_int_Bulk)/nq/nq));
qB_VSource_int_Bulk=((((-Gamma)*Vp_VSource_int_Bulk*d10_sqrt0)/_vt_nom)-((((qI_VSource_int_Bulk*(nq-1.0))+(qI*nq_VSource_int_Bulk))*nq-(qI*(nq-1.0))*nq_VSource_int_Bulk)/nq/nq));
#endif
qB=((((-Gamma)*d00_sqrt0)/_vt_nom)-((qI*(nq-1.0))/nq));
}
}
else
{
#if defined(_DERIVATE)
qB_VGate_Bulk=((-VGprime_VGate_Bulk)/_vt_nom);
qB_VDrain_int_Bulk=0.0;
qB_VSource_int_Bulk=0.0;
#endif
qB=((-VGprime)/_vt_nom);
}
}
if
((LEVEL==2))
{
{
double m00_fabs(d00_fabs0,(qB+(eta*qI)))
#if defined(_DERIVATE)
double m10_fabs(d10_fabs0,d00_fabs0,(qB+(eta*qI)))
#endif
#if defined(_DERIVATE)
Beta_VSource_int_Bulk=((Beta0prime_VSource_int_Bulk*(1.0+(con21*d00_fabs0))-Beta0prime*(con21*(qB_VSource_int_Bulk+(eta*qI_VSource_int_Bulk))*d10_fabs0))/(1.0+(con21*d00_fabs0))/(1.0+(con21*d00_fabs0)));
Beta_VDrain_int_Bulk=((Beta0prime_VDrain_int_Bulk*(1.0+(con21*d00_fabs0))-Beta0prime*(con21*(qB_VDrain_int_Bulk+(eta*qI_VDrain_int_Bulk))*d10_fabs0))/(1.0+(con21*d00_fabs0))/(1.0+(con21*d00_fabs0)));
Beta_VGate_Bulk=((Beta0prime_VGate_Bulk*(1.0+(con21*d00_fabs0))-Beta0prime*(con21*(qB_VGate_Bulk+(eta*qI_VGate_Bulk))*d10_fabs0))/(1.0+(con21*d00_fabs0))/(1.0+(con21*d00_fabs0)));
#endif
Beta=(Beta0prime/(1.0+(con21*d00_fabs0)));
}
}
else
{
#if defined(_DERIVATE)
Beta_VSource_int_Bulk=(-con22*(Theta*Vp_VSource_int_Bulk)/(1+(Theta*Vp))/(1+(Theta*Vp)));
Beta_VDrain_int_Bulk=(-con22*(Theta*Vp_VDrain_int_Bulk)/(1+(Theta*Vp))/(1+(Theta*Vp)));
Beta_VGate_Bulk=(-con22*(Theta*Vp_VGate_Bulk)/(1+(Theta*Vp))/(1+(Theta*Vp)));
#endif
Beta=(con22/(1+(Theta*Vp)));
}
#if defined(_DERIVATE)
Ispecific_VGate_Bulk=(((2.0*n_VGate_Bulk)*Beta)+((2.0*n)*Beta_VGate_Bulk))*_vt_nom*_vt_nom;
Ispecific_VDrain_int_Bulk=(((2.0*n_VDrain_int_Bulk)*Beta)+((2.0*n)*Beta_VDrain_int_Bulk))*_vt_nom*_vt_nom;
Ispecific_VSource_int_Bulk=(((2.0*n_VSource_int_Bulk)*Beta)+((2.0*n)*Beta_VSource_int_Bulk))*_vt_nom*_vt_nom;
#endif
Ispecific=((((2.0*n)*Beta)*_vt_nom)*_vt_nom);
if
((LEVEL==2))
{
#if defined(_DERIVATE)
Ids_VGate_Bulk=((Ispecific_VGate_Bulk*(iff-irprime))+(Ispecific*(iff_VGate_Bulk-irprime_VGate_Bulk)));
Ids_VDrain_int_Bulk=((Ispecific_VDrain_int_Bulk*(iff-irprime))+(Ispecific*(iff_VDrain_int_Bulk-irprime_VDrain_int_Bulk)));
Ids_VSource_int_Bulk=((Ispecific_VSource_int_Bulk*(iff-irprime))+(Ispecific*(iff_VSource_int_Bulk-irprime_VSource_int_Bulk)));
#endif
Ids=(Ispecific*(iff-irprime));
#if defined(_DERIVATE)
Vib_VSource_int_Bulk=((VD_VSource_int_Bulk-VS_VSource_int_Bulk)-((Ibn*2.0)*Vdss_VSource_int_Bulk));
Vib_VDrain_int_Bulk=((VD_VDrain_int_Bulk-VS_VDrain_int_Bulk)-((Ibn*2.0)*Vdss_VDrain_int_Bulk));
Vib_VGate_Bulk=(-((Ibn*2.0)*Vdss_VGate_Bulk));
#endif
Vib=((VD-VS)-((Ibn*2.0)*Vdss));
if
((Vib>0.0))
{
{
double m00_exp(d00_exp0,(((-Ibb_T)*Lc)/Vib))
#if defined(_DERIVATE)
#define d10_exp0 d00_exp0
#endif
#if defined(_DERIVATE)
Idb_VGate_Bulk=((((Ids_VGate_Bulk*(Iba/Ibb_T)*Vib)+((Ids*(Iba/Ibb_T))*Vib_VGate_Bulk))*d00_exp0)+(((Ids*(Iba/Ibb_T))*Vib)*(-((-Ibb_T)*Lc)*Vib_VGate_Bulk/Vib/Vib)*d10_exp0));
Idb_VDrain_int_Bulk=((((Ids_VDrain_int_Bulk*(Iba/Ibb_T)*Vib)+((Ids*(Iba/Ibb_T))*Vib_VDrain_int_Bulk))*d00_exp0)+(((Ids*(Iba/Ibb_T))*Vib)*(-((-Ibb_T)*Lc)*Vib_VDrain_int_Bulk/Vib/Vib)*d10_exp0));
Idb_VSource_int_Bulk=((((Ids_VSource_int_Bulk*(Iba/Ibb_T)*Vib)+((Ids*(Iba/Ibb_T))*Vib_VSource_int_Bulk))*d00_exp0)+(((Ids*(Iba/Ibb_T))*Vib)*(-((-Ibb_T)*Lc)*Vib_VSource_int_Bulk/Vib/Vib)*d10_exp0));
#endif
Idb=(((Ids*(Iba/Ibb_T))*Vib)*d00_exp0);
}
}
else
{
#if defined(_DERIVATE)
Idb_VGate_Bulk=0.0;
Idb_VDrain_int_Bulk=0.0;
Idb_VSource_int_Bulk=0.0;
#endif
Idb=0.0;
}
}
else
{
#if defined(_DERIVATE)
Ids_VGate_Bulk=((Ispecific_VGate_Bulk*(iff-ir))+(Ispecific*(iff_VGate_Bulk-ir_VGate_Bulk)));
Ids_VDrain_int_Bulk=((Ispecific_VDrain_int_Bulk*(iff-ir))+(Ispecific*(iff_VDrain_int_Bulk-ir_VDrain_int_Bulk)));
Ids_VSource_int_Bulk=((Ispecific_VSource_int_Bulk*(iff-ir))+(Ispecific*(iff_VSource_int_Bulk-ir_VSource_int_Bulk)));
#endif
Ids=(Ispecific*(iff-ir));
}
{
double m00_fabs(d00_fabs0,qI)
#if defined(_DERIVATE)
double m10_fabs(d10_fabs0,d00_fabs0,qI)
#endif
#if defined(_DERIVATE)
Sthermal_VSource_int_Bulk=(((fourkt*Beta_VSource_int_Bulk)*d00_fabs0)+((fourkt*Beta)*qI_VSource_int_Bulk*d10_fabs0));
Sthermal_VDrain_int_Bulk=(((fourkt*Beta_VDrain_int_Bulk)*d00_fabs0)+((fourkt*Beta)*qI_VDrain_int_Bulk*d10_fabs0));
Sthermal_VGate_Bulk=(((fourkt*Beta_VGate_Bulk)*d00_fabs0)+((fourkt*Beta)*qI_VGate_Bulk*d10_fabs0));
#endif
Sthermal=((fourkt*Beta)*d00_fabs0);
}
#if defined(_DERIVATE)
T_1_VGate_Bulk=(-4.0*Ispecific_VGate_Bulk/Ispecific/Ispecific);
T_1_VDrain_int_Bulk=(-4.0*Ispecific_VDrain_int_Bulk/Ispecific/Ispecific);
T_1_VSource_int_Bulk=(-4.0*Ispecific_VSource_int_Bulk/Ispecific/Ispecific);
#endif
T_1=(4.0/Ispecific);
{
double m00_sqrt(d00_sqrt0,((T_1*iff)+1.0))
double m00_sqrt(d00_sqrt1,((T_1*ir)+1.0))
#if defined(_DERIVATE)
double m10_sqrt(d10_sqrt0,d00_sqrt0,((T_1*iff)+1.0))
double m10_sqrt(d10_sqrt1,d00_sqrt1,((T_1*ir)+1.0))
#endif
#if defined(_DERIVATE)
gm_VSource_int_Bulk=((Beta_VSource_int_Bulk*_vt_nom*(d00_sqrt0-d00_sqrt1))+((Beta*_vt_nom)*(((T_1_VSource_int_Bulk*iff)+(T_1*iff_VSource_int_Bulk))*d10_sqrt0-((T_1_VSource_int_Bulk*ir)+(T_1*ir_VSource_int_Bulk))*d10_sqrt1)));
gm_VDrain_int_Bulk=((Beta_VDrain_int_Bulk*_vt_nom*(d00_sqrt0-d00_sqrt1))+((Beta*_vt_nom)*(((T_1_VDrain_int_Bulk*iff)+(T_1*iff_VDrain_int_Bulk))*d10_sqrt0-((T_1_VDrain_int_Bulk*ir)+(T_1*ir_VDrain_int_Bulk))*d10_sqrt1)));
gm_VGate_Bulk=((Beta_VGate_Bulk*_vt_nom*(d00_sqrt0-d00_sqrt1))+((Beta*_vt_nom)*(((T_1_VGate_Bulk*iff)+(T_1*iff_VGate_Bulk))*d10_sqrt0-((T_1_VGate_Bulk*ir)+(T_1*ir_VGate_Bulk))*d10_sqrt1)));
#endif
gm=((Beta*_vt_nom)*(d00_sqrt0-d00_sqrt1));
}
#if defined(_DERIVATE)
Sflicker_VSource_int_Bulk=(((con23*gm_VSource_int_Bulk)*gm)+((con23*gm)*gm_VSource_int_Bulk));
Sflicker_VDrain_int_Bulk=(((con23*gm_VDrain_int_Bulk)*gm)+((con23*gm)*gm_VDrain_int_Bulk));
Sflicker_VGate_Bulk=(((con23*gm_VGate_Bulk)*gm)+((con23*gm)*gm_VGate_Bulk));
#endif
Sflicker=((con23*gm)*gm);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qb_VGate_Bulk=(con24*qB_VGate_Bulk);
qb_VDrain_int_Bulk=(con24*qB_VDrain_int_Bulk);
qb_VSource_int_Bulk=(con24*qB_VSource_int_Bulk);
#endif
qb=(con24*qB);
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qg_VGate_Bulk=(con24*((-qI_VGate_Bulk)-qB_VGate_Bulk));
qg_VDrain_int_Bulk=(con24*((-qI_VDrain_int_Bulk)-qB_VDrain_int_Bulk));
qg_VSource_int_Bulk=(con24*((-qI_VSource_int_Bulk)-qB_VSource_int_Bulk));
#endif
qg=(con24*((-qI)-qB));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgso_VGate_Bulk=(con27*VG_VGate_Bulk);
qgso_VDrain_int_Bulk=(con27*(-VS_VDrain_int_Bulk));
qgso_VSource_int_Bulk=(con27*(-VS_VSource_int_Bulk));
#endif
qgso=(con27*(VG-VS));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgdo_VGate_Bulk=(con28*VG_VGate_Bulk);
qgdo_VSource_int_Bulk=(con28*(-VD_VSource_int_Bulk));
qgdo_VDrain_int_Bulk=(con28*(-VD_VDrain_int_Bulk));
#endif
qgdo=(con28*(VG-VD));
#endif
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
qgbo_VGate_Bulk=(con29*VG_VGate_Bulk);
#endif
qgbo=(con29*VG);
#endif
if
((StoDswap>0.0))
{
#if defined(_DERIVATE)
V1_VBulk_Source_int=0.0;
V1_VBulk_Drain_int=(p_n_MOS);
#endif
V1=(p_n_MOS*BP(Bulk,Drain_int));
#if defined(_DERIVATE)
V2_VBulk_Drain_int=0.0;
V2_VBulk_Source_int=(p_n_MOS);
#endif
V2=(p_n_MOS*BP(Bulk,Source_int));
}
else
{
#if defined(_DERIVATE)
V2_VBulk_Drain_int=(p_n_MOS);
V2_VBulk_Source_int=0.0;
#endif
V2=(p_n_MOS*BP(Bulk,Drain_int));
#if defined(_DERIVATE)
V1_VBulk_Source_int=(p_n_MOS);
V1_VBulk_Drain_int=0.0;
#endif
V1=(p_n_MOS*BP(Bulk,Source_int));
}
{
double m00_limexp(d00_limexp0,(V1*con31))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(V1*con31))
#endif
#if defined(_DERIVATE)
Id1_VBulk_Source_int=((V1>(-con32))?((Area2*V1_VBulk_Source_int*con31*d10_limexp0)+(GMIN*V1_VBulk_Source_int)):0.0);
Id1_VBulk_Drain_int=((V1>(-con32))?((Area2*V1_VBulk_Drain_int*con31*d10_limexp0)+(GMIN*V1_VBulk_Drain_int)):0.0);
#endif
Id1=((V1>(-con32))?((Area2*(d00_limexp0-1.0))+(GMIN*V1)):0);
}
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-(V1/Vj_T2)),(1-M))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-(V1/Vj_T2)),(1-M))
#endif
#if defined(_DERIVATE)
Qd1_VBulk_Source_int=((V1<con33)?((Tt*Id1_VBulk_Source_int)+(Area1*(-(d10_pow0*(-(V1_VBulk_Source_int/Vj_T2)))))):0.0);
Qd1_VBulk_Drain_int=((V1<con33)?((Tt*Id1_VBulk_Drain_int)+(Area1*(-(d10_pow0*(-(V1_VBulk_Drain_int/Vj_T2)))))):0.0);
#endif
Qd1=((V1<con33)?((Tt*Id1)+(Area1*(1-d00_pow0))):0);
}
#endif
#if defined(_DERIVATE)
Id2_VBulk_Source_int=((V1<=(-con32))?(GMIN*V1_VBulk_Source_int):0.0);
Id2_VBulk_Drain_int=((V1<=(-con32))?(GMIN*V1_VBulk_Drain_int):0.0);
#endif
Id2=((V1<=(-con32))?((-Area2)+(GMIN*V1)):0);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qd2_VBulk_Source_int=((V1>=con33)?((Tt*Id1_VBulk_Source_int)+(Area3*(DP4*((F3*V1_VBulk_Source_int)+(DP2*((V1_VBulk_Source_int*V1)+(V1*V1_VBulk_Source_int))))))):0.0);
Qd2_VBulk_Drain_int=((V1>=con33)?((Tt*Id1_VBulk_Drain_int)+(Area3*(DP4*((F3*V1_VBulk_Drain_int)+(DP2*((V1_VBulk_Drain_int*V1)+(V1*V1_VBulk_Drain_int))))))):0.0);
#endif
Qd2=((V1>=con33)?((Tt*Id1)+(Area3*(DP4*((F3*(V1-DP3))+(DP2*((V1*V1)-DP1)))))):0);
#endif
{
double m00_limexp(d00_limexp0,(V2*con31))
#if defined(_DERIVATE)
double m10_limexp(d10_limexp0,d00_limexp0,(V2*con31))
#endif
#if defined(_DERIVATE)
Is1_VBulk_Drain_int=((V2>(-con32))?((Area2*V2_VBulk_Drain_int*con31*d10_limexp0)+(GMIN*V2_VBulk_Drain_int)):0.0);
Is1_VBulk_Source_int=((V2>(-con32))?((Area2*V2_VBulk_Source_int*con31*d10_limexp0)+(GMIN*V2_VBulk_Source_int)):0.0);
#endif
Is1=((V2>(-con32))?((Area2*(d00_limexp0-1.0))+(GMIN*V2)):0);
}
#if defined(_DYNAMIC)
{
double m00_pow(d00_pow0,(1-(V2/Vj_T2)),(1-M))
#if defined(_DERIVATE)
double m10_pow(d10_pow0,d00_pow0,(1-(V2/Vj_T2)),(1-M))
#endif
#if defined(_DERIVATE)
Qs1_VBulk_Drain_int=((V2<con33)?((Tt*Is1_VBulk_Drain_int)+(Area1*(-(d10_pow0*(-(V2_VBulk_Drain_int/Vj_T2)))))):0.0);
Qs1_VBulk_Source_int=((V2<con33)?((Tt*Is1_VBulk_Source_int)+(Area1*(-(d10_pow0*(-(V2_VBulk_Source_int/Vj_T2)))))):0.0);
#endif
Qs1=((V2<con33)?((Tt*Is1)+(Area1*(1-d00_pow0))):0);
}
#endif
#if defined(_DERIVATE)
Is2_VBulk_Drain_int=((V2<=(-con32))?(GMIN*V2_VBulk_Drain_int):0.0);
Is2_VBulk_Source_int=((V2<=(-con32))?(GMIN*V2_VBulk_Source_int):0.0);
#endif
Is2=((V2<=(-con32))?((-Area2)+(GMIN*V2)):0);
#if defined(_DYNAMIC)
#if defined(_DERIVATE)
Qs2_VBulk_Drain_int=((V2>=con33)?((Tt*Is1_VBulk_Drain_int)+(Area3*(DP4*((F3*V2_VBulk_Drain_int)+(DP2*((V2_VBulk_Drain_int*V2)+(V2*V2_VBulk_Drain_int))))))):0.0);
Qs2_VBulk_Source_int=((V2>=con33)?((Tt*Is1_VBulk_Source_int)+(Area3*(DP4*((F3*V2_VBulk_Source_int)+(DP2*((V2_VBulk_Source_int*V2)+(V2*V2_VBulk_Source_int))))))):0.0);
#endif
Qs2=((V2>=con33)?((Tt*Is1)+(Area3*(DP4*((F3*(V2-DP3))+(DP2*((V2*V2)-DP1)))))):0);
#endif
if
((StoDswap>0.0))
{
_load_static_residual2(Drain,Drain_int,(BP(Drain,Drain_int)/RDeff));
#if defined(_DERIVATE)
_load_static_jacobian4(Drain,Drain_int,Drain,Drain_int,(1/RDeff));
#endif
_load_static_residual2(Source,Source_int,(BP(Source,Source_int)/RSeff));
#if defined(_DERIVATE)
_load_static_jacobian4(Source,Source_int,Source,Source_int,(1/RSeff));
#endif
_load_static_residual2(Drain_int,Source_int,(p_n_MOS*Ids));
#if defined(_DERIVATE)
_load_static_jacobian4(Drain_int,Source_int,Source_int,Bulk,(p_n_MOS*Ids_VSource_int_Bulk));
_load_static_jacobian4(Drain_int,Source_int,Drain_int,Bulk,(p_n_MOS*Ids_VDrain_int_Bulk));
_load_static_jacobian4(Drain_int,Source_int,Gate,Bulk,(p_n_MOS*Ids_VGate_Bulk));
#endif
if
((LEVEL==2))
{
_load_static_residual2(Drain_int,Bulk,(p_n_MOS*Idb));
#if defined(_DERIVATE)
_load_static_jacobian4(Drain_int,Bulk,Source_int,Bulk,(p_n_MOS*Idb_VSource_int_Bulk));
_load_static_jacobian4(Drain_int,Bulk,Drain_int,Bulk,(p_n_MOS*Idb_VDrain_int_Bulk));
_load_static_jacobian4(Drain_int,Bulk,Gate,Bulk,(p_n_MOS*Idb_VGate_Bulk));
#endif
}
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Drain_int,((p_n_MOS*Xpart)*_DDT(qg)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Drain_int,Source_int,Bulk,((p_n_MOS*Xpart)*qg_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Drain_int,Bulk,((p_n_MOS*Xpart)*qg_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Gate,Bulk,((p_n_MOS*Xpart)*qg_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Source_int,((p_n_MOS*Spart)*_DDT(qg)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Source_int,Source_int,Bulk,((p_n_MOS*Spart)*qg_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Drain_int,Bulk,((p_n_MOS*Spart)*qg_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Gate,Bulk,((p_n_MOS*Spart)*qg_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Drain_int,Bulk,((p_n_MOS*Xpart)*_DDT(qb)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Drain_int,Bulk,Source_int,Bulk,((p_n_MOS*Xpart)*qb_VSource_int_Bulk));
_load_dynamic_jacobian4(Drain_int,Bulk,Drain_int,Bulk,((p_n_MOS*Xpart)*qb_VDrain_int_Bulk));
_load_dynamic_jacobian4(Drain_int,Bulk,Gate,Bulk,((p_n_MOS*Xpart)*qb_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Source_int,Bulk,((p_n_MOS*Spart)*_DDT(qb)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Source_int,Bulk,Source_int,Bulk,((p_n_MOS*Spart)*qb_VSource_int_Bulk));
_load_dynamic_jacobian4(Source_int,Bulk,Drain_int,Bulk,((p_n_MOS*Spart)*qb_VDrain_int_Bulk));
_load_dynamic_jacobian4(Source_int,Bulk,Gate,Bulk,((p_n_MOS*Spart)*qb_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Source_int,(p_n_MOS*_DDT(qgso)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Source_int,Source_int,Bulk,(p_n_MOS*qgso_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Drain_int,Bulk,(p_n_MOS*qgso_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Gate,Bulk,(p_n_MOS*qgso_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Drain_int,(p_n_MOS*_DDT(qgdo)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Drain_int,Drain_int,Bulk,(p_n_MOS*qgdo_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Source_int,Bulk,(p_n_MOS*qgdo_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Gate,Bulk,(p_n_MOS*qgdo_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Bulk,(p_n_MOS*_DDT(qgbo)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Bulk,Gate,Bulk,(p_n_MOS*qgbo_VGate_Bulk));
#endif
#endif
_load_static_residual2(Bulk,Drain_int,(p_n_MOS*(Id1+Id2)));
#if defined(_DERIVATE)
_load_static_jacobian4(Bulk,Drain_int,Bulk,Drain_int,(p_n_MOS*(Id1_VBulk_Drain_int+Id2_VBulk_Drain_int)));
_load_static_jacobian4(Bulk,Drain_int,Bulk,Source_int,(p_n_MOS*(Id1_VBulk_Source_int+Id2_VBulk_Source_int)));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Bulk,Drain_int,(p_n_MOS*_DDT((Qd1+Qd2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Bulk,Drain_int,Bulk,Drain_int,(p_n_MOS*(Qd1_VBulk_Drain_int+Qd2_VBulk_Drain_int)));
_load_dynamic_jacobian4(Bulk,Drain_int,Bulk,Source_int,(p_n_MOS*(Qd1_VBulk_Source_int+Qd2_VBulk_Source_int)));
#endif
#endif
_load_static_residual2(Bulk,Source_int,(p_n_MOS*(Is1+Is2)));
#if defined(_DERIVATE)
_load_static_jacobian4(Bulk,Source_int,Bulk,Source_int,(p_n_MOS*(Is1_VBulk_Source_int+Is2_VBulk_Source_int)));
_load_static_jacobian4(Bulk,Source_int,Bulk,Drain_int,(p_n_MOS*(Is1_VBulk_Drain_int+Is2_VBulk_Drain_int)));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Bulk,Source_int,(p_n_MOS*_DDT((Qs1+Qs2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Bulk,Source_int,Bulk,Source_int,(p_n_MOS*(Qs1_VBulk_Source_int+Qs2_VBulk_Source_int)));
_load_dynamic_jacobian4(Bulk,Source_int,Bulk,Drain_int,(p_n_MOS*(Qs1_VBulk_Drain_int+Qs2_VBulk_Drain_int)));
#endif
#endif
_save_whitenoise2(Drain_int,Source_int,Sthermal,"thermal");
_save_flickernoise2(Drain_int,Source_int,Sflicker,Af,"flicker");
_save_whitenoise2(Drain,Drain_int,con25,"thermal");
_save_whitenoise2(Source,Source_int,con26,"thermal");
}
else
{
_load_static_residual2(Drain,Drain_int,(BP(Drain,Drain_int)/RSeff));
#if defined(_DERIVATE)
_load_static_jacobian4(Drain,Drain_int,Drain,Drain_int,(1/RSeff));
#endif
_load_static_residual2(Source,Source_int,(BP(Source,Source_int)/RDeff));
#if defined(_DERIVATE)
_load_static_jacobian4(Source,Source_int,Source,Source_int,(1/RDeff));
#endif
_load_static_residual2(Source_int,Drain_int,(p_n_MOS*Ids));
#if defined(_DERIVATE)
_load_static_jacobian4(Source_int,Drain_int,Source_int,Bulk,(p_n_MOS*Ids_VSource_int_Bulk));
_load_static_jacobian4(Source_int,Drain_int,Drain_int,Bulk,(p_n_MOS*Ids_VDrain_int_Bulk));
_load_static_jacobian4(Source_int,Drain_int,Gate,Bulk,(p_n_MOS*Ids_VGate_Bulk));
#endif
if
((LEVEL==2))
{
_load_static_residual2(Source_int,Bulk,(p_n_MOS*Idb));
#if defined(_DERIVATE)
_load_static_jacobian4(Source_int,Bulk,Source_int,Bulk,(p_n_MOS*Idb_VSource_int_Bulk));
_load_static_jacobian4(Source_int,Bulk,Drain_int,Bulk,(p_n_MOS*Idb_VDrain_int_Bulk));
_load_static_jacobian4(Source_int,Bulk,Gate,Bulk,(p_n_MOS*Idb_VGate_Bulk));
#endif
}
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Source_int,((p_n_MOS*Xpart)*_DDT(qg)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Source_int,Source_int,Bulk,((p_n_MOS*Xpart)*qg_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Drain_int,Bulk,((p_n_MOS*Xpart)*qg_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Gate,Bulk,((p_n_MOS*Xpart)*qg_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Drain_int,((p_n_MOS*Spart)*_DDT(qg)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Drain_int,Source_int,Bulk,((p_n_MOS*Spart)*qg_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Drain_int,Bulk,((p_n_MOS*Spart)*qg_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Gate,Bulk,((p_n_MOS*Spart)*qg_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Source_int,Bulk,((p_n_MOS*Xpart)*_DDT(qb)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Source_int,Bulk,Source_int,Bulk,((p_n_MOS*Xpart)*qb_VSource_int_Bulk));
_load_dynamic_jacobian4(Source_int,Bulk,Drain_int,Bulk,((p_n_MOS*Xpart)*qb_VDrain_int_Bulk));
_load_dynamic_jacobian4(Source_int,Bulk,Gate,Bulk,((p_n_MOS*Xpart)*qb_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Drain_int,Bulk,((p_n_MOS*Spart)*_DDT(qb)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Drain_int,Bulk,Source_int,Bulk,((p_n_MOS*Spart)*qb_VSource_int_Bulk));
_load_dynamic_jacobian4(Drain_int,Bulk,Drain_int,Bulk,((p_n_MOS*Spart)*qb_VDrain_int_Bulk));
_load_dynamic_jacobian4(Drain_int,Bulk,Gate,Bulk,((p_n_MOS*Spart)*qb_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Drain_int,(p_n_MOS*_DDT(qgso)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Drain_int,Source_int,Bulk,(p_n_MOS*qgso_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Drain_int,Bulk,(p_n_MOS*qgso_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Drain_int,Gate,Bulk,(p_n_MOS*qgso_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Source_int,(p_n_MOS*_DDT(qgdo)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Source_int,Drain_int,Bulk,(p_n_MOS*qgdo_VDrain_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Source_int,Bulk,(p_n_MOS*qgdo_VSource_int_Bulk));
_load_dynamic_jacobian4(Gate,Source_int,Gate,Bulk,(p_n_MOS*qgdo_VGate_Bulk));
#endif
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Gate,Bulk,(p_n_MOS*_DDT(qgbo)));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Gate,Bulk,Gate,Bulk,(p_n_MOS*qgbo_VGate_Bulk));
#endif
#endif
_load_static_residual2(Bulk,Source_int,(p_n_MOS*(Id1+Id2)));
#if defined(_DERIVATE)
_load_static_jacobian4(Bulk,Source_int,Bulk,Drain_int,(p_n_MOS*(Id1_VBulk_Drain_int+Id2_VBulk_Drain_int)));
_load_static_jacobian4(Bulk,Source_int,Bulk,Source_int,(p_n_MOS*(Id1_VBulk_Source_int+Id2_VBulk_Source_int)));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Bulk,Source_int,(p_n_MOS*_DDT((Qd1+Qd2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Bulk,Source_int,Bulk,Drain_int,(p_n_MOS*(Qd1_VBulk_Drain_int+Qd2_VBulk_Drain_int)));
_load_dynamic_jacobian4(Bulk,Source_int,Bulk,Source_int,(p_n_MOS*(Qd1_VBulk_Source_int+Qd2_VBulk_Source_int)));
#endif
#endif
_load_static_residual2(Bulk,Drain_int,(p_n_MOS*(Is1+Is2)));
#if defined(_DERIVATE)
_load_static_jacobian4(Bulk,Drain_int,Bulk,Source_int,(p_n_MOS*(Is1_VBulk_Source_int+Is2_VBulk_Source_int)));
_load_static_jacobian4(Bulk,Drain_int,Bulk,Drain_int,(p_n_MOS*(Is1_VBulk_Drain_int+Is2_VBulk_Drain_int)));
#endif
#if defined(_DYNAMIC)
_load_dynamic_residual2(Bulk,Drain_int,(p_n_MOS*_DDT((Qs1+Qs2))));
#if defined(_DERIVATE)
_load_dynamic_jacobian4(Bulk,Drain_int,Bulk,Source_int,(p_n_MOS*(Qs1_VBulk_Source_int+Qs2_VBulk_Source_int)));
_load_dynamic_jacobian4(Bulk,Drain_int,Bulk,Drain_int,(p_n_MOS*(Qs1_VBulk_Drain_int+Qs2_VBulk_Drain_int)));
#endif
#endif
_save_whitenoise2(Source_int,Drain_int,Sthermal,"thermal");
_save_flickernoise2(Source_int,Drain_int,Sflicker,Af,"flicker");
_save_whitenoise2(Source_int,Source,con25,"thermal");
_save_whitenoise2(Drain_int,Drain,con26,"thermal");
}

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void EKV26MOS::calcDC (void)
{
  // evaluate Verilog code
  initVerilog ();		
  calcVerilog ();		

  // fill right hand side and static jacobian
  for (int i1 = 0; i1 < 6; i1++) {
    setI (i1, _rhs[i1]);
    for (int i2 = 0; i2 < 6; i2++) {
      setY (i1, i2, _jstat[i1][i2]);
    }
  }
}

/* Save operating points. */
void EKV26MOS::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void EKV26MOS::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void EKV26MOS::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void EKV26MOS::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void EKV26MOS::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix EKV26MOS::calcMatrixY (nr_double_t frequency)
{
  _freq = frequency;
  saveOperatingPoints ();
  matrix y (6);

  for (int i1 = 0; i1 < 6; i1++) {
    for (int i2 = 0; i2 < 6; i2++) {
      y (i1,i2) = nr_complex_t (_jstat[i1][i2], _jdyna[i1][i2] * 2 * M_PI * _freq);
    }
  }

  return y;
}

/* Initialization of S-parameter analysis. */
void EKV26MOS::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void EKV26MOS::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void EKV26MOS::initTR (void)
{
  setStates (2 * 6 * 6);
  initDC ();
}

/* Perform transient analysis iteration step. */
void EKV26MOS::calcTR (nr_double_t)
{
  doHB = 0;
  doAC = 1;
  doTR = 1;
  calcDC ();

  int i1, i2, i3, i4, state;

  // 2-node charge integrations
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
    state = 2 * (i2 + 6 * i1);
    if (i1 != i2)
    if (_charges[i1][i2] != 0.0)
      transientCapacitanceQ (state, i1, i2, _charges[i1][i2]);
  } }

  // 1-node charge integrations
  for (i1 = 0; i1 < 6; i1++) {
    state = 2 * (i1 + 6 * i1);
    if (_charges[i1][i1] != 0.0)
      transientCapacitanceQ (state, i1, _charges[i1][i1]);
  }

  // charge: 2-node, voltage: 2-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    if (i3 != i4)
    if (_caps[i1][i2][i3][i4] != 0.0)
      transientCapacitanceC (i1, i2, i3, i4, _caps[i1][i2][i3][i4], BP(i3,i4));
  } } } }

  // charge: 2-node, voltage: 1-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i2 = 0; i2 < 6; i2++) {
  if (i1 != i2)
  for (i3 = 0; i3 < 6; i3++) {
    if (_caps[i1][i2][i3][i3] != 0.0)
      transientCapacitanceC2Q (i1, i2, i3, _caps[i1][i2][i3][i3], NP(i3));
  } } }

  // charge: 1-node, voltage: 2-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i3 = 0; i3 < 6; i3++) {
  for (i4 = 0; i4 < 6; i4++) {
    if (i3 != i4)
    if (_caps[i1][i1][i3][i4] != 0.0)
      transientCapacitanceC2V (i1, i3, i4, _caps[i1][i1][i3][i4], BP(i3,i4));
  } } }
     
  // charge: 1-node, voltage: 1-node
  for (i1 = 0; i1 < 6; i1++) {
  for (i3 = 0; i3 < 6; i3++) {
    if (_caps[i1][i1][i3][i3] != 0.0)
      transientCapacitanceC (i1, i3, _caps[i1][i1][i3][i3], NP(i3));
  } }
}

/* Compute Cy-matrix for AC noise analysis. */
matrix EKV26MOS::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (6);

  _load_flickernoise2 (Source_int,Drain_int,_flicker_pwr[Source_int][Drain_int],_flicker_exp[Source_int][Drain_int]);
  _load_flickernoise2 (Drain_int,Source_int,_flicker_pwr[Drain_int][Source_int],_flicker_exp[Drain_int][Source_int]);
  _load_whitenoise2 (Drain_int,Drain,_white_pwr[Drain_int][Drain]);
  _load_whitenoise2 (Source_int,Source,_white_pwr[Source_int][Source]);
  _load_whitenoise2 (Source_int,Drain_int,_white_pwr[Source_int][Drain_int]);
  _load_whitenoise2 (Drain_int,Source_int,_white_pwr[Drain_int][Source_int]);
  _load_whitenoise2 (Source,Source_int,_white_pwr[Source][Source_int]);
  _load_whitenoise2 (Drain,Drain_int,_white_pwr[Drain][Drain_int]);

  return cy;
}

/* Perform AC noise computations. */
void EKV26MOS::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void EKV26MOS::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void EKV26MOS::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void EKV26MOS::calcHB (int)
{
  doHB = 1;
  doAC = 1;
  doTR = 0;

  // jacobian dI/dV and currents get filled
  calcDC ();
  saveOperatingPoints ();

  // fill in HB matrices
  for (int i1 = 0; i1 < 6; i1++) {
    setQ  (i1, _qhs[i1]); // charges
    setCV (i1, _chs[i1]); // jacobian dQ/dV * V
    setGV (i1, _ghs[i1]); // jacobian dI/dV * V
    for (int i2 = 0; i2 < 6; i2++) {
      setQV (i1, i2, _jdyna[i1][i2]); // jacobian dQ/dV
    }
  }
}

#include "EKV26MOS.defs.h"
