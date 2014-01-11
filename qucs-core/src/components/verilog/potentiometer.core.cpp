/*
 * potentiometer.core.cpp - device implementations for potentiometer module
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

#include "potentiometer.analogfunction.h"
#include "component.h"
#include "device.h"
#include "potentiometer.core.h"

#ifndef CIR_potentiometer
#define CIR_potentiometer -1
#endif

// external nodes
#define B 0
#define M 1
#define T 2
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
#define _modelname              "potentiometer"
#define _instancename           getName()
#define _circuit_temp	        (getPropertyDouble("Temp")+273.15)
#define _param_given(p)		(isPropertyGiven(p)?1:0)


// $vt and $vt() functions 
#define _vt_nom                 (kBoverQ*_circuit_temp)

using namespace device;

/* Device constructor. */
potentiometer::potentiometer() : circuit (4)
{
  type = CIR_potentiometer;
}

/* Initialization of model. */
void potentiometer::initModel (void)
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
void potentiometer::initDC (void)
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
void potentiometer::restartDC (void)
{
}

/* Initialize Verilog-AMS code. */
void potentiometer::initVerilog (void)
{
  // initialization of noise variables
  _white_pwr[M][n1] = 0.0;
  _white_pwr[B][n1] = 0.0;
  _white_pwr[T][n1] = 0.0;

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
void potentiometer::loadVariables (void)
{
  R_pot = getPropertyDouble ("R_pot");
  Rotation = getPropertyDouble ("Rotation");
  Taper_Coeff = getPropertyDouble ("Taper_Coeff");
  LEVEL = getPropertyInteger ("LEVEL");
  Max_Rotation = getPropertyDouble ("Max_Rotation");
  Conformity = getPropertyDouble ("Conformity");
  Linearity = getPropertyDouble ("Linearity");
  Contact_Res = getPropertyDouble ("Contact_Res");
  Temp_Coeff = getPropertyDouble ("Temp_Coeff");
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
void potentiometer::initializeModel (void)
{
}

/* Evaluate Verilog-AMS equations in instance initialization. */
void potentiometer::initializeInstance (void)
{
}

/* Evaluate Verilog-AMS equations in initial step. */
void potentiometer::initialStep (void)
{
}

/* Evaluate Verilog-AMS equations in final step. */
void potentiometer::finalStep (void)
{
}

/* Evaluate Verilog-AMS equations in analog block. */
void potentiometer::calcVerilog (void)
{

/* ----------------- evaluate verilog analog equations -------------------- */
double fourkt;
double Rbot;
double Rtop;
double RTT;
double RTB;
double error_term;
double Tpcoeff;
double R_pot_Temp;
double Rad_Angle;
double Rcontact;
Rcontact=(Contact_Res+1e-6);
Rad_Angle=((Rotation*3.14159265358979323846)/180);
R_pot_Temp=((R_pot+1e-6)*(1+((Temp_Coeff*(_circuit_temp-Tnom))/1e6)));
{
double m00_sin(d00_sin0,Rad_Angle)
Tpcoeff=(Taper_Coeff+((Conformity+(Linearity*d00_sin0))/100));
}
{
double m00_sin(d00_sin0,Rad_Angle)
error_term=(1+((Conformity+(Linearity*d00_sin0))/100));
}
if
((LEVEL)==(2))
{
RTB=(R_pot_Temp*Tpcoeff);
RTT=1e15;
Rtop=((1.000001-(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp);
Rbot=((0.000001+(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp);
}
else
if
((LEVEL)==(3))
{
RTB=1e15;
RTT=(R_pot_Temp*Tpcoeff);
Rtop=((1.000001-(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp);
Rbot=((0.000001+(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp);
}
else
{
RTB=1e15;
RTT=1e15;
Rtop=(((1.000001-(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp)*error_term);
Rbot=(((0.000001+(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp)*error_term);
}
if
((Taper_Coeff==0.0))
{
RTB=1e15;
RTT=1e15;
Rtop=(((1.000001-(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp)*error_term);
Rbot=(((0.000001+(Rotation/(Max_Rotation+1e-20)))*R_pot_Temp)*error_term);
}
_load_static_residual2(T,n1,(BP(T,n1)/Rtop));
#if defined(_DERIVATE)
_load_static_jacobian4(T,n1,T,n1,(1/Rtop));
#endif
_load_static_residual2(T,n1,(BP(T,n1)/RTT));
#if defined(_DERIVATE)
_load_static_jacobian4(T,n1,T,n1,(1/RTT));
#endif
_load_static_residual2(B,n1,(BP(B,n1)/Rbot));
#if defined(_DERIVATE)
_load_static_jacobian4(B,n1,B,n1,(1/Rbot));
#endif
_load_static_residual2(B,n1,(BP(B,n1)/RTB));
#if defined(_DERIVATE)
_load_static_jacobian4(B,n1,B,n1,(1/RTB));
#endif
_load_static_residual2(M,n1,(BP(M,n1)/Rcontact));
#if defined(_DERIVATE)
_load_static_jacobian4(M,n1,M,n1,(1/Rcontact));
#endif
fourkt=((4.0*1.3806503e-23)*_circuit_temp);
_save_whitenoise2(T,n1,(fourkt/Rtop),"thermal");
_save_whitenoise2(T,n1,(fourkt/RTT),"thermal");
_save_whitenoise2(B,n1,(fourkt/Rbot),"thermal");
_save_whitenoise2(B,n1,(fourkt/RTB),"thermal");
_save_whitenoise2(M,n1,(fourkt/Rcontact),"thermal");

/* ------------------ end of verilog analog equations --------------------- */

/* ------------------ evaluate verilog noise equations -------------------- */

/* ------------------- end of verilog noise equations --------------------- */
}

/* Perform DC iteration. */
void potentiometer::calcDC (void)
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
void potentiometer::saveOperatingPoints (void)
{
  // save global instance operating points
}

/* Load operating points. */
void potentiometer::loadOperatingPoints (void)
{
}

/* Calculate operating points. */
void potentiometer::calcOperatingPoints (void)
{
}

/* Initialization of AC analysis. */
void potentiometer::initAC (void)
{
  allocMatrixMNA ();
}

/* Perform AC calculations. */
void potentiometer::calcAC (nr_double_t frequency)
{
  setMatrixY (calcMatrixY (frequency));
}

/* Compute Y-matrix for AC analysis. */
matrix potentiometer::calcMatrixY (nr_double_t frequency)
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
void potentiometer::initSP (void)
{
  allocMatrixS ();
}

/* Perform S-parameter calculations. */
void potentiometer::calcSP (nr_double_t frequency)
{
  setMatrixS (ytos (calcMatrixY (frequency)));   
}

/* Initialization of transient analysis. */
void potentiometer::initTR (void)
{
  setStates (2 * 4 * 4);
  initDC ();
}

/* Perform transient analysis iteration step. */
void potentiometer::calcTR (nr_double_t)
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
matrix potentiometer::calcMatrixCy (nr_double_t frequency) 
{
  _freq = frequency;
  matrix cy (4);

  _load_whitenoise2 (M,n1,_white_pwr[M][n1]);
  _load_whitenoise2 (B,n1,_white_pwr[B][n1]);
  _load_whitenoise2 (T,n1,_white_pwr[T][n1]);

  return cy;
}

/* Perform AC noise computations. */
void potentiometer::calcNoiseAC (nr_double_t frequency) 
{
  setMatrixN (calcMatrixCy (frequency));
}

/* Perform S-parameter noise computations. */
void potentiometer::calcNoiseSP (nr_double_t frequency) 
{
  setMatrixN (cytocs (calcMatrixCy (frequency) * z0, getMatrixS ()));
}

/* Initialization of HB analysis. */
void potentiometer::initHB (int)
{
  initDC ();
  allocMatrixHB ();
}

/* Perform HB analysis. */
void potentiometer::calcHB (int)
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

#include "potentiometer.defs.h"
