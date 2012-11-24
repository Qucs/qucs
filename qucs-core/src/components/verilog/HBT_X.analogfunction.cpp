/*
 * HBT_X.analogfunction.cpp - analog function implementations
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include <stdio.h>
#include "HBT_X.analogfunction.h"
#include <math.h>

/* math functions and appropriate derivatives */
inline double _cos(double arg)             { return  cos(arg); }
inline double _d0_cos(double arg)          { return (-sin(arg)); }
inline double _sin(double arg)             { return  sin(arg); }
inline double _d0_sin(double arg)          { return (cos(arg)); }
inline double _tan(double arg)             { return  tan(arg); }
inline double _d0_tan(double arg)          { return (1.0/cos(arg)/cos(arg)); }
inline double _cosh(double arg)            { return  cosh(arg); }
inline double _d0_cosh(double arg)         { return (sinh(arg)); }
inline double _sinh(double arg)            { return  sinh(arg); }
inline double _d0_sinh(double arg)         { return (cosh(arg)); }
inline double _tanh(double arg)            { return  tanh(arg); }
inline double _d0_tanh(double arg)         { return (1.0/cosh(arg)/cosh(arg)); }
inline double _acos(double arg)            { return  acos(arg); }
inline double _d0_acos(double arg)         { return (-1.0/sqrt(1-arg*arg)); }
inline double _asin(double arg)            { return  asin(arg); }
inline double _d0_asin(double arg)         { return (+1.0/sqrt(1-arg*arg)); }
inline double _atan(double arg)            { return  atan(arg); }
inline double _d0_atan(double arg)         { return (+1.0/(1+arg*arg)); }
inline double _atanh(double arg)           { return  atanh(arg); }
inline double _d0_atanh(double arg)        { return (+1.0/(1-arg*arg)); }
inline double _logE(double arg)            { return  log(arg); }
inline double _d0_logE(double arg)         { return (1.0/arg); }
inline double _log10(double arg)           { return  log10(arg); }
inline double _d0_log10(double arg)        { return (1.0/arg/log(10.0)); }
inline double _exp(double arg)             { return  exp(arg); }
inline double _d0_exp(double arg)          { return exp(arg); }
inline double _sqrt(double arg)            { return  sqrt(arg); }
inline double _d0_sqrt(double arg)         { return (1.0/sqrt(arg)/2.0); }
inline double _abs(double arg)             { return  fabs(arg); }
inline double _d0_abs(double arg)          { return (((arg)>=0)?(+1.0):(-1.0)); }
inline int _floor(double arg)              { return  floor(arg); }
inline int _d0_floor(double)               { return (1.0); }

inline double _hypot(double x,double y)    { return sqrt((x)*(x)+(y)*(y)); }
inline double _d0_hypot(double x,double y) { return (x)/sqrt((x)*(x)+(y)*(y)); }
inline double _d1_hypot(double x,double y) { return (y)/sqrt((x)*(x)+(y)*(y)); }

inline double _max(double x,double y)      { return ((x)>(y))?(x):(y); }
inline double _d0_max(double x,double y)   { return ((x)>(y))?1.0:0.0; }
inline double _d1_max(double x,double y)   { return ((x)>(y))?0.0:1.0; }

inline double _min(double x,double y)      { return ((x)<(y))?(x):(y); }
inline double _d0_min(double x,double y)   { return ((x)<(y))?1.0:0.0; }
inline double _d1_min(double x,double y)   { return ((x)<(y))?0.0:1.0; }

inline double _pow(double x,double y)      { return pow(x,y); }
inline double _d0_pow(double x,double y)   { return (x==0.0)?0.0:((y/x)*pow(x,y)); }
inline double _d1_pow(double x,double y)   { return (x==0.0)?0.0:((log(x)/exp(0.0))*pow(x,y)); }

inline double _limexp(double arg)          { return ((arg)<(80))?(exp(arg)):(exp(80.0)*(1.0+(arg-80))); }
inline double _d0_limexp(double arg)       { return ((arg)<(80))?(exp(arg)):(exp(80.0)); }

inline double _vt(double arg)              { return 1.3806503e-23*arg/1.602176462e-19; }
inline double _d0_vt(double)               { return 1.3806503e-23/1.602176462e-19; }


/*
 * analog function: exp_soft
 */
double HBT_X_exp_soft (double x)
{
double exp_soft=0.0;
double maxexp=0.0;
double maxarg=0.0;
{maxexp=1.0e25;
maxarg=_logE(maxexp);
if((x<maxarg))
{exp_soft=_exp(x);
}else
{exp_soft=(((x+1.0)-maxarg)*maxexp);
}}
return exp_soft;
}
double HBT_X_d_exp_soft (double x, double d_x)
{
double exp_soft=0.0;
double exp_soft_x=0.0;
double maxexp=0.0;
double maxexp_x=0.0;
double maxarg=0.0;
double maxarg_x=0.0;
{{maxexp_x=0.0;
maxexp=1.0e25;
}
{maxarg_x=_d0_logE(maxexp)*(maxexp_x);
maxarg=_logE(maxexp);
}
if((x<maxarg))
{{exp_soft_x=_d0_exp(x)*(1.0);
exp_soft=_exp(x);
}
}else
{{exp_soft_x=(((1.0-maxarg_x)*maxexp)+(((x+1.0)-maxarg)*maxexp_x));
exp_soft=(((x+1.0)-maxarg)*maxexp);
}
}}
return exp_soft_x*d_x;
}

/*
 * analog function: Vt
 */
double HBT_X_Vt (double U, double Ud)
{
double Vt=0.0;
double Vch=0.0;
double VF=0.0;
{Vch=(0.1*Ud);
VF=(0.9*Ud);
if((U<VF))
Vt=(U-(Vch*_logE((1.0+_exp(((U-VF)/Vch))))));
else
Vt=(VF-(Vch*_logE((1.0+_exp(((VF-U)/Vch))))));
}
return Vt;
}
double HBT_X_d_Vt (double U, double Ud, double d_U, double d_Ud)
{
double Vt=0.0;
double Vt_U=0.0;
double Vt_Ud=0.0;
double Vch=0.0;
double Vch_U=0.0;
double Vch_Ud=0.0;
double VF=0.0;
double VF_U=0.0;
double VF_Ud=0.0;
{{Vch_U=0.0;
Vch_Ud=(0.1);
Vch=(0.1*Ud);
}
{VF_U=0.0;
VF_Ud=(0.9);
VF=(0.9*Ud);
}
if((U<VF))
{Vt_U=(1.0-((Vch_U*_logE((1.0+_exp(((U-VF)/Vch)))))+(Vch*_d0_logE((1.0+_exp(((U-VF)/Vch))))*((+_d0_exp(((U-VF)/Vch))*(((1.0-VF_U)*Vch-(U-VF)*Vch_U)/(Vch*Vch)))))));
Vt_Ud=(-((Vch_Ud*_logE((1.0+_exp(((U-VF)/Vch)))))+(Vch*_d0_logE((1.0+_exp(((U-VF)/Vch))))*((+_d0_exp(((U-VF)/Vch))*(((-VF_Ud)*Vch-(U-VF)*Vch_Ud)/(Vch*Vch)))))));
Vt=(U-(Vch*_logE((1.0+_exp(((U-VF)/Vch))))));
}
else
{Vt_U=(VF_U-((Vch_U*_logE((1.0+_exp(((VF-U)/Vch)))))+(Vch*_d0_logE((1.0+_exp(((VF-U)/Vch))))*((+_d0_exp(((VF-U)/Vch))*(((VF_U-1.0)*Vch-(VF-U)*Vch_U)/(Vch*Vch)))))));
Vt_Ud=(VF_Ud-((Vch_Ud*_logE((1.0+_exp(((VF-U)/Vch)))))+(Vch*_d0_logE((1.0+_exp(((VF-U)/Vch))))*((+_d0_exp(((VF-U)/Vch))*((VF_Ud*Vch-(VF-U)*Vch_Ud)/(Vch*Vch)))))));
Vt=(VF-(Vch*_logE((1.0+_exp(((VF-U)/Vch))))));
}
}
return Vt_U*d_U+Vt_Ud*d_Ud;
}

/*
 * analog function: diode
 */
double HBT_X_diode (double U, double Is, double Ug, double N, double AREA, double TJ, double TNOM)
{
double diode=0.0;
double VTH0=0.0;
double VTHJ=0.0;
double VTHNOM=0.0;
double maxi=0.0;
double Tmax=0.0;
double TJM=0.0;
double KDURCHQ=0.0;
double lnIs=0.0;
{VTH0=_vt((20.0+273.15));
VTHNOM=_vt((TNOM+273.15));
KDURCHQ=0.861708692e-4;
lnIs=_logE((Is*AREA));
maxi=_logE(1e6);
if(((maxi<(Ug/VTHNOM))&&(U<0.0)))
{Tmax=(((Ug*VTHNOM)/((Ug-(maxi*VTHNOM))*KDURCHQ))-273.15);
TJM=HBT_X_Vt(TJ,Tmax);
}else
{TJM=TJ;
}VTHJ=_vt((TJM+273.15));
if((Ug>0.0))
{diode=(HBT_X_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs))-HBT_X_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs)));
}else
{diode=(HBT_X_exp_soft(((U/(N*VTH0))+lnIs))-(Is*AREA));
}}
return diode;
}
double HBT_X_d_diode (double U, double Is, double Ug, double N, double AREA, double TJ, double TNOM, double d_U, double d_Is, double d_Ug, double d_N, double d_AREA, double d_TJ, double d_TNOM)
{
double diode=0.0;
double diode_U=0.0;
double diode_Is=0.0;
double diode_Ug=0.0;
double diode_N=0.0;
double diode_AREA=0.0;
double diode_TJ=0.0;
double diode_TNOM=0.0;
double VTH0=0.0;
double VTH0_U=0.0;
double VTH0_Is=0.0;
double VTH0_Ug=0.0;
double VTH0_N=0.0;
double VTH0_AREA=0.0;
double VTH0_TJ=0.0;
double VTH0_TNOM=0.0;
double VTHJ=0.0;
double VTHJ_U=0.0;
double VTHJ_Is=0.0;
double VTHJ_Ug=0.0;
double VTHJ_N=0.0;
double VTHJ_AREA=0.0;
double VTHJ_TJ=0.0;
double VTHJ_TNOM=0.0;
double VTHNOM=0.0;
double VTHNOM_U=0.0;
double VTHNOM_Is=0.0;
double VTHNOM_Ug=0.0;
double VTHNOM_N=0.0;
double VTHNOM_AREA=0.0;
double VTHNOM_TJ=0.0;
double VTHNOM_TNOM=0.0;
double maxi=0.0;
double maxi_U=0.0;
double maxi_Is=0.0;
double maxi_Ug=0.0;
double maxi_N=0.0;
double maxi_AREA=0.0;
double maxi_TJ=0.0;
double maxi_TNOM=0.0;
double Tmax=0.0;
double Tmax_U=0.0;
double Tmax_Is=0.0;
double Tmax_Ug=0.0;
double Tmax_N=0.0;
double Tmax_AREA=0.0;
double Tmax_TJ=0.0;
double Tmax_TNOM=0.0;
double TJM=0.0;
double TJM_U=0.0;
double TJM_Is=0.0;
double TJM_Ug=0.0;
double TJM_N=0.0;
double TJM_AREA=0.0;
double TJM_TJ=0.0;
double TJM_TNOM=0.0;
double KDURCHQ=0.0;
double KDURCHQ_U=0.0;
double KDURCHQ_Is=0.0;
double KDURCHQ_Ug=0.0;
double KDURCHQ_N=0.0;
double KDURCHQ_AREA=0.0;
double KDURCHQ_TJ=0.0;
double KDURCHQ_TNOM=0.0;
double lnIs=0.0;
double lnIs_U=0.0;
double lnIs_Is=0.0;
double lnIs_Ug=0.0;
double lnIs_N=0.0;
double lnIs_AREA=0.0;
double lnIs_TJ=0.0;
double lnIs_TNOM=0.0;
{{VTH0_U=_d0_vt((20.0+273.15))*(0.0);
VTH0_Is=_d0_vt((20.0+273.15))*(0.0);
VTH0_Ug=_d0_vt((20.0+273.15))*(0.0);
VTH0_N=_d0_vt((20.0+273.15))*(0.0);
VTH0_AREA=_d0_vt((20.0+273.15))*(0.0);
VTH0_TJ=_d0_vt((20.0+273.15))*(0.0);
VTH0_TNOM=_d0_vt((20.0+273.15))*(0.0);
VTH0=_vt((20.0+273.15));
}
{VTHNOM_U=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_Is=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_Ug=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_N=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_AREA=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_TJ=_d0_vt((TNOM+273.15))*(0.0);
VTHNOM_TNOM=_d0_vt((TNOM+273.15))*(1.0);
VTHNOM=_vt((TNOM+273.15));
}
{KDURCHQ_U=0.0;
KDURCHQ_Is=0.0;
KDURCHQ_Ug=0.0;
KDURCHQ_N=0.0;
KDURCHQ_AREA=0.0;
KDURCHQ_TJ=0.0;
KDURCHQ_TNOM=0.0;
KDURCHQ=0.861708692e-4;
}
{lnIs_U=_d0_logE((Is*AREA))*(0.0);
lnIs_Is=_d0_logE((Is*AREA))*((AREA));
lnIs_Ug=_d0_logE((Is*AREA))*(0.0);
lnIs_N=_d0_logE((Is*AREA))*(0.0);
lnIs_AREA=_d0_logE((Is*AREA))*((Is));
lnIs_TJ=_d0_logE((Is*AREA))*(0.0);
lnIs_TNOM=_d0_logE((Is*AREA))*(0.0);
lnIs=_logE((Is*AREA));
}
{maxi_U=_d0_logE(1e6)*(0.0);
maxi_Is=_d0_logE(1e6)*(0.0);
maxi_Ug=_d0_logE(1e6)*(0.0);
maxi_N=_d0_logE(1e6)*(0.0);
maxi_AREA=_d0_logE(1e6)*(0.0);
maxi_TJ=_d0_logE(1e6)*(0.0);
maxi_TNOM=_d0_logE(1e6)*(0.0);
maxi=_logE(1e6);
}
if(((maxi<(Ug/VTHNOM))&&(U<0.0)))
{{Tmax_U=((Ug*VTHNOM_U)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_U*VTHNOM)+(maxi*VTHNOM_U)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_U)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_Is=((Ug*VTHNOM_Is)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_Is*VTHNOM)+(maxi*VTHNOM_Is)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_Is)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_Ug=((VTHNOM+(VTHNOM_Ug*Ug))*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((1.0-((maxi_Ug*VTHNOM)+(maxi*VTHNOM_Ug)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_Ug)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_N=((Ug*VTHNOM_N)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_N*VTHNOM)+(maxi*VTHNOM_N)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_N)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_AREA=((Ug*VTHNOM_AREA)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_AREA*VTHNOM)+(maxi*VTHNOM_AREA)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_AREA)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_TJ=((Ug*VTHNOM_TJ)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_TJ*VTHNOM)+(maxi*VTHNOM_TJ)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_TJ)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax_TNOM=((Ug*VTHNOM_TNOM)*((Ug-(maxi*VTHNOM))*KDURCHQ)-(Ug*VTHNOM)*(((-((maxi_TNOM*VTHNOM)+(maxi*VTHNOM_TNOM)))*KDURCHQ)+((Ug-(maxi*VTHNOM))*KDURCHQ_TNOM)))/(((Ug-(maxi*VTHNOM))*KDURCHQ)*((Ug-(maxi*VTHNOM))*KDURCHQ));
Tmax=(((Ug*VTHNOM)/((Ug-(maxi*VTHNOM))*KDURCHQ))-273.15);
}
{TJM_U=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_U));
TJM_Is=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_Is));
TJM_Ug=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_Ug));
TJM_N=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_N));
TJM_AREA=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_AREA));
TJM_TJ=HBT_X_d_Vt(TJ,Tmax,(1.0),(Tmax_TJ));
TJM_TNOM=HBT_X_d_Vt(TJ,Tmax,(0.0),(Tmax_TNOM));
TJM=HBT_X_Vt(TJ,Tmax);
}
}else
{{TJM_U=0.0;
TJM_Is=0.0;
TJM_Ug=0.0;
TJM_N=0.0;
TJM_AREA=0.0;
TJM_TJ=1.0;
TJM_TNOM=0.0;
TJM=TJ;
}
}{VTHJ_U=_d0_vt((TJM+273.15))*(TJM_U);
VTHJ_Is=_d0_vt((TJM+273.15))*(TJM_Is);
VTHJ_Ug=_d0_vt((TJM+273.15))*(TJM_Ug);
VTHJ_N=_d0_vt((TJM+273.15))*(TJM_N);
VTHJ_AREA=_d0_vt((TJM+273.15))*(TJM_AREA);
VTHJ_TJ=_d0_vt((TJM+273.15))*(TJM_TJ);
VTHJ_TNOM=_d0_vt((TJM+273.15))*(TJM_TNOM);
VTHJ=_vt((TJM+273.15));
}
if((Ug>0.0))
{{diode_U=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((((N*VTHJ)-(U*(N*VTHJ_U)))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_U)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_U)/(VTHJ*VTHJ)))+lnIs_U)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_U)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_U)/(VTHJ*VTHJ)))+lnIs_U))));
diode_Is=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(N*VTHJ_Is))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_Is)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_Is)/(VTHJ*VTHJ)))+lnIs_Is)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_Is)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_Is)/(VTHJ*VTHJ)))+lnIs_Is))));
diode_Ug=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(N*VTHJ_Ug))/((N*VTHJ)*(N*VTHJ)))+((VTHNOM-(Ug*VTHNOM_Ug))/(VTHNOM*VTHNOM)))-((VTHJ-(Ug*VTHJ_Ug))/(VTHJ*VTHJ)))+lnIs_Ug)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),(((((VTHNOM-(Ug*VTHNOM_Ug))/(VTHNOM*VTHNOM))-((VTHJ-(Ug*VTHJ_Ug))/(VTHJ*VTHJ)))+lnIs_Ug))));
diode_N=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(VTHJ+(VTHJ_N*N)))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_N)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_N)/(VTHJ*VTHJ)))+lnIs_N)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_N)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_N)/(VTHJ*VTHJ)))+lnIs_N))));
diode_AREA=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(N*VTHJ_AREA))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_AREA)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_AREA)/(VTHJ*VTHJ)))+lnIs_AREA)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_AREA)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_AREA)/(VTHJ*VTHJ)))+lnIs_AREA))));
diode_TJ=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(N*VTHJ_TJ))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_TJ)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_TJ)/(VTHJ*VTHJ)))+lnIs_TJ)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_TJ)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_TJ)/(VTHJ*VTHJ)))+lnIs_TJ))));
diode_TNOM=(HBT_X_d_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs),(((((-(U*(N*VTHJ_TNOM))/((N*VTHJ)*(N*VTHJ)))+(-(Ug*VTHNOM_TNOM)/(VTHNOM*VTHNOM)))-(-(Ug*VTHJ_TNOM)/(VTHJ*VTHJ)))+lnIs_TNOM)))-HBT_X_d_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs),((((-(Ug*VTHNOM_TNOM)/(VTHNOM*VTHNOM))-(-(Ug*VTHJ_TNOM)/(VTHJ*VTHJ)))+lnIs_TNOM))));
diode=(HBT_X_exp_soft(((((U/(N*VTHJ))+(Ug/VTHNOM))-(Ug/VTHJ))+lnIs))-HBT_X_exp_soft((((Ug/VTHNOM)-(Ug/VTHJ))+lnIs)));
}
}else
{{diode_U=HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((((N*VTH0)-(U*(N*VTH0_U)))/((N*VTH0)*(N*VTH0)))+lnIs_U)));
diode_Is=(HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(N*VTH0_Is))/((N*VTH0)*(N*VTH0)))+lnIs_Is)))-(AREA));
diode_Ug=HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(N*VTH0_Ug))/((N*VTH0)*(N*VTH0)))+lnIs_Ug)));
diode_N=HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(VTH0+(VTH0_N*N)))/((N*VTH0)*(N*VTH0)))+lnIs_N)));
diode_AREA=(HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(N*VTH0_AREA))/((N*VTH0)*(N*VTH0)))+lnIs_AREA)))-(Is));
diode_TJ=HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(N*VTH0_TJ))/((N*VTH0)*(N*VTH0)))+lnIs_TJ)));
diode_TNOM=HBT_X_d_exp_soft(((U/(N*VTH0))+lnIs),(((-(U*(N*VTH0_TNOM))/((N*VTH0)*(N*VTH0)))+lnIs_TNOM)));
diode=(HBT_X_exp_soft(((U/(N*VTH0))+lnIs))-(Is*AREA));
}
}}
return diode_U*d_U+diode_Is*d_Is+diode_Ug*d_Ug+diode_N*d_N+diode_AREA*d_AREA+diode_TJ*d_TJ+diode_TNOM*d_TNOM;
}

/*
 * analog function: MM
 */
double HBT_X_MM (double VBCI, double VCBO, double MC, double VCBLIN, double BF, double KC)
{
double MM=0.0;
double FBD=0.0;
double vcbi=0.0;
{if((((KC>0.0)&&(MC>0.0))&&(VCBO>0.0)))
{vcbi=VBCI;
FBD=(VCBLIN/VCBO);
if((VBCI>0.0))
MM=1.0;
else
if((VBCI>(-VCBLIN)))
{if((MC==1))
MM=(1.0/(1.0-(vcbi/(-VCBO))));
else
MM=(1.0/(1.0-_pow((vcbi/(-VCBO)),MC)));
}else
if((VBCI<=(-VCBLIN)))
{if((MC==1))
{MM=((1.0/(1.0-FBD))-((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi+(FBD*VCBO))));
}else
{MM=((1.0/(1.0-_pow(FBD,MC)))-((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO))));
}}}else
MM=1.0;
}
return MM;
}
double HBT_X_d_MM (double VBCI, double VCBO, double MC, double VCBLIN, double BF, double KC, double d_VBCI, double d_VCBO, double d_MC, double d_VCBLIN, double d_BF, double d_KC)
{
double MM=0.0;
double MM_VBCI=0.0;
double MM_VCBO=0.0;
double MM_MC=0.0;
double MM_VCBLIN=0.0;
double MM_BF=0.0;
double MM_KC=0.0;
double FBD=0.0;
double FBD_VBCI=0.0;
double FBD_VCBO=0.0;
double FBD_MC=0.0;
double FBD_VCBLIN=0.0;
double FBD_BF=0.0;
double FBD_KC=0.0;
double vcbi=0.0;
double vcbi_VBCI=0.0;
double vcbi_VCBO=0.0;
double vcbi_MC=0.0;
double vcbi_VCBLIN=0.0;
double vcbi_BF=0.0;
double vcbi_KC=0.0;
{if((((KC>0.0)&&(MC>0.0))&&(VCBO>0.0)))
{{vcbi_VBCI=1.0;
vcbi_VCBO=0.0;
vcbi_MC=0.0;
vcbi_VCBLIN=0.0;
vcbi_BF=0.0;
vcbi_KC=0.0;
vcbi=VBCI;
}
{FBD_VBCI=0.0;
FBD_VCBO=(-VCBLIN/(VCBO*VCBO));
FBD_MC=0.0;
FBD_VCBLIN=(1/VCBO);
FBD_BF=0.0;
FBD_KC=0.0;
FBD=(VCBLIN/VCBO);
}
if((VBCI>0.0))
{MM_VBCI=0.0;
MM_VCBO=0.0;
MM_MC=0.0;
MM_VCBLIN=0.0;
MM_BF=0.0;
MM_KC=0.0;
MM=1.0;
}
else
if((VBCI>(-VCBLIN)))
{if((MC==1))
{MM_VBCI=(-(-vcbi_VBCI/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM_VCBO=(-(-vcbi_VCBO/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM_MC=(-(-vcbi_MC/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM_VCBLIN=(-(-vcbi_VCBLIN/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM_BF=(-(-vcbi_BF/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM_KC=(-(-vcbi_KC/(-VCBO))/((1.0-(vcbi/(-VCBO)))*(1.0-(vcbi/(-VCBO)))));
MM=(1.0/(1.0-(vcbi/(-VCBO))));
}
else
{MM_VBCI=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_VBCI/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(0.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM_VCBO=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_VCBO/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(0.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM_MC=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_MC/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(1.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM_VCBLIN=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_VCBLIN/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(0.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM_BF=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_BF/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(0.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM_KC=(-(-_d0_pow((vcbi/(-VCBO)),MC)*(vcbi_KC/(-VCBO))+_d1_pow((vcbi/(-VCBO)),MC)*(0.0))/((1.0-_pow((vcbi/(-VCBO)),MC))*(1.0-_pow((vcbi/(-VCBO)),MC))));
MM=(1.0/(1.0-_pow((vcbi/(-VCBO)),MC)));
}
}else
if((VBCI<=(-VCBLIN)))
{if((MC==1))
{{MM_VBCI=((-(-FBD_VBCI)/((1.0-FBD)*(1.0-FBD)))-(((-(((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_VBCI))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0)))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_VBCI+FBD_VBCI*VCBO))));
MM_VCBO=((-(-FBD_VCBO)/((1.0-FBD)*(1.0-FBD)))-((((-1/(VCBO*VCBO))*1.0*_pow((1.0-FBD),2.0)-((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_VCBO))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_VCBO+(FBD_VCBO*VCBO)+FBD))));
MM_MC=((-(-FBD_MC)/((1.0-FBD)*(1.0-FBD)))-(((-(((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_MC))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0)))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_MC+FBD_MC*VCBO))));
MM_VCBLIN=((-(-FBD_VCBLIN)/((1.0-FBD)*(1.0-FBD)))-(((-(((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_VCBLIN))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0)))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_VCBLIN+FBD_VCBLIN*VCBO))));
MM_BF=((-(-FBD_BF)/((1.0-FBD)*(1.0-FBD)))-(((-(((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_BF))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0)))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_BF+FBD_BF*VCBO))));
MM_KC=((-(-FBD_KC)/((1.0-FBD)*(1.0-FBD)))-(((-(((1.0/VCBO)*1.0)*_d0_pow((1.0-FBD),2.0)*((-FBD_KC))+_d1_pow((1.0-FBD),2.0)*(0.0))/(_pow((1.0-FBD),2.0)*_pow((1.0-FBD),2.0)))*(vcbi+(FBD*VCBO)))+((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi_KC+FBD_KC*VCBO))));
MM=((1.0/(1.0-FBD))-((((1.0/VCBO)*1.0)/_pow((1.0-FBD),2.0))*(vcbi+(FBD*VCBO))));
}
}else
{{MM_VBCI=((-(-_d0_pow(FBD,MC)*(FBD_VBCI)+_d1_pow(FBD,MC)*(0.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-(((((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_VBCI)+_d1_pow(FBD,(MC-1.0))*(0.0))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_VBCI)+_d1_pow(FBD,MC)*(0.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_VBCI+FBD_VBCI*VCBO))));
MM_VCBO=((-(-_d0_pow(FBD,MC)*(FBD_VCBO)+_d1_pow(FBD,MC)*(0.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-((((((-MC/(VCBO*VCBO))*_pow(FBD,(MC-1.0)))+((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_VCBO)+_d1_pow(FBD,(MC-1.0))*(0.0)))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_VCBO)+_d1_pow(FBD,MC)*(0.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_VCBO+(FBD_VCBO*VCBO)+FBD))));
MM_MC=((-(-_d0_pow(FBD,MC)*(FBD_MC)+_d1_pow(FBD,MC)*(1.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-((((((1/VCBO)*_pow(FBD,(MC-1.0)))+((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_MC)+_d1_pow(FBD,(MC-1.0))*(1.0)))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_MC)+_d1_pow(FBD,MC)*(1.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_MC+FBD_MC*VCBO))));
MM_VCBLIN=((-(-_d0_pow(FBD,MC)*(FBD_VCBLIN)+_d1_pow(FBD,MC)*(0.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-(((((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_VCBLIN)+_d1_pow(FBD,(MC-1.0))*(0.0))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_VCBLIN)+_d1_pow(FBD,MC)*(0.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_VCBLIN+FBD_VCBLIN*VCBO))));
MM_BF=((-(-_d0_pow(FBD,MC)*(FBD_BF)+_d1_pow(FBD,MC)*(0.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-(((((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_BF)+_d1_pow(FBD,(MC-1.0))*(0.0))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_BF)+_d1_pow(FBD,MC)*(0.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_BF+FBD_BF*VCBO))));
MM_KC=((-(-_d0_pow(FBD,MC)*(FBD_KC)+_d1_pow(FBD,MC)*(0.0))/((1.0-_pow(FBD,MC))*(1.0-_pow(FBD,MC))))-(((((MC/VCBO)*_d0_pow(FBD,(MC-1.0))*(FBD_KC)+_d1_pow(FBD,(MC-1.0))*(0.0))*_pow((1.0-_pow(FBD,MC)),2.0)-((MC/VCBO)*_pow(FBD,(MC-1.0)))*_d0_pow((1.0-_pow(FBD,MC)),2.0)*((-_d0_pow(FBD,MC)*(FBD_KC)+_d1_pow(FBD,MC)*(0.0)))+_d1_pow((1.0-_pow(FBD,MC)),2.0)*(0.0))/(_pow((1.0-_pow(FBD,MC)),2.0)*_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO)))+((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi_KC+FBD_KC*VCBO))));
MM=((1.0/(1.0-_pow(FBD,MC)))-((((MC/VCBO)*_pow(FBD,(MC-1.0)))/_pow((1.0-_pow(FBD,MC)),2.0))*(vcbi+(FBD*VCBO))));
}
}}}else
{MM_VBCI=0.0;
MM_VCBO=0.0;
MM_MC=0.0;
MM_VCBLIN=0.0;
MM_BF=0.0;
MM_KC=0.0;
MM=1.0;
}
}
return MM_VBCI*d_VBCI+MM_VCBO*d_VCBO+MM_MC*d_MC+MM_VCBLIN*d_VCBLIN+MM_BF*d_BF+MM_KC*d_KC;
}

/*
 * analog function: charge
 */
double HBT_X_charge (double U, double C0, double Ud, double m, double Area)
{
double charge=0.0;
double Vj=0.0;
double Vjo=0.0;
double VF=0.0;
{Vj=HBT_X_Vt(U,Ud);
Vjo=HBT_X_Vt(0.0,Ud);
VF=(0.9*Ud);
if((m==1.0))
{charge=((Area*C0)*((Ud*(_logE((1.0-(Vjo/Ud)))-_logE((1.0-(Vj/Ud)))))+((1.0/(1.0-(VF/Ud)))*((U-Vj)+Vjo))));
}else
{charge=((Area*C0)*((((Ud/(1.0-m))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+(_pow((1.0-(VF/Ud)),(-m))*((U-Vj)+Vjo)))-(Ud*(1.0/(1.0-m)))));
}}
return charge;
}
double HBT_X_d_charge (double U, double C0, double Ud, double m, double Area, double d_U, double d_C0, double d_Ud, double d_m, double d_Area)
{
double charge=0.0;
double charge_U=0.0;
double charge_C0=0.0;
double charge_Ud=0.0;
double charge_m=0.0;
double charge_Area=0.0;
double Vj=0.0;
double Vj_U=0.0;
double Vj_C0=0.0;
double Vj_Ud=0.0;
double Vj_m=0.0;
double Vj_Area=0.0;
double Vjo=0.0;
double Vjo_U=0.0;
double Vjo_C0=0.0;
double Vjo_Ud=0.0;
double Vjo_m=0.0;
double Vjo_Area=0.0;
double VF=0.0;
double VF_U=0.0;
double VF_C0=0.0;
double VF_Ud=0.0;
double VF_m=0.0;
double VF_Area=0.0;
{{Vj_U=HBT_X_d_Vt(U,Ud,(1.0),(0.0));
Vj_C0=HBT_X_d_Vt(U,Ud,(0.0),(0.0));
Vj_Ud=HBT_X_d_Vt(U,Ud,(0.0),(1.0));
Vj_m=HBT_X_d_Vt(U,Ud,(0.0),(0.0));
Vj_Area=HBT_X_d_Vt(U,Ud,(0.0),(0.0));
Vj=HBT_X_Vt(U,Ud);
}
{Vjo_U=HBT_X_d_Vt(0.0,Ud,(0.0),(0.0));
Vjo_C0=HBT_X_d_Vt(0.0,Ud,(0.0),(0.0));
Vjo_Ud=HBT_X_d_Vt(0.0,Ud,(0.0),(1.0));
Vjo_m=HBT_X_d_Vt(0.0,Ud,(0.0),(0.0));
Vjo_Area=HBT_X_d_Vt(0.0,Ud,(0.0),(0.0));
Vjo=HBT_X_Vt(0.0,Ud);
}
{VF_U=0.0;
VF_C0=0.0;
VF_Ud=(0.9);
VF_m=0.0;
VF_Area=0.0;
VF=(0.9*Ud);
}
if((m==1.0))
{{charge_U=((Area*C0)*((Ud*(_d0_logE((1.0-(Vjo/Ud)))*((-Vjo_U/Ud))-_d0_logE((1.0-(Vj/Ud)))*((-Vj_U/Ud))))+(((-(-VF_U/Ud)/((1.0-(VF/Ud))*(1.0-(VF/Ud))))*((U-Vj)+Vjo))+((1.0/(1.0-(VF/Ud)))*((1.0-Vj_U)+Vjo_U)))));
charge_C0=(((Area)*((Ud*(_logE((1.0-(Vjo/Ud)))-_logE((1.0-(Vj/Ud)))))+((1.0/(1.0-(VF/Ud)))*((U-Vj)+Vjo))))+((Area*C0)*((Ud*(_d0_logE((1.0-(Vjo/Ud)))*((-Vjo_C0/Ud))-_d0_logE((1.0-(Vj/Ud)))*((-Vj_C0/Ud))))+(((-(-VF_C0/Ud)/((1.0-(VF/Ud))*(1.0-(VF/Ud))))*((U-Vj)+Vjo))+((1.0/(1.0-(VF/Ud)))*((-Vj_C0)+Vjo_C0))))));
charge_Ud=((Area*C0)*(((_logE((1.0-(Vjo/Ud)))-_logE((1.0-(Vj/Ud))))+((_d0_logE((1.0-(Vjo/Ud)))*((-((Vjo_Ud*Ud)-Vjo)/(Ud*Ud)))-_d0_logE((1.0-(Vj/Ud)))*((-((Vj_Ud*Ud)-Vj)/(Ud*Ud))))*Ud))+(((-(-((VF_Ud*Ud)-VF)/(Ud*Ud))/((1.0-(VF/Ud))*(1.0-(VF/Ud))))*((U-Vj)+Vjo))+((1.0/(1.0-(VF/Ud)))*((-Vj_Ud)+Vjo_Ud)))));
charge_m=((Area*C0)*((Ud*(_d0_logE((1.0-(Vjo/Ud)))*((-Vjo_m/Ud))-_d0_logE((1.0-(Vj/Ud)))*((-Vj_m/Ud))))+(((-(-VF_m/Ud)/((1.0-(VF/Ud))*(1.0-(VF/Ud))))*((U-Vj)+Vjo))+((1.0/(1.0-(VF/Ud)))*((-Vj_m)+Vjo_m)))));
charge_Area=(((C0)*((Ud*(_logE((1.0-(Vjo/Ud)))-_logE((1.0-(Vj/Ud)))))+((1.0/(1.0-(VF/Ud)))*((U-Vj)+Vjo))))+((Area*C0)*((Ud*(_d0_logE((1.0-(Vjo/Ud)))*((-Vjo_Area/Ud))-_d0_logE((1.0-(Vj/Ud)))*((-Vj_Area/Ud))))+(((-(-VF_Area/Ud)/((1.0-(VF/Ud))*(1.0-(VF/Ud))))*((U-Vj)+Vjo))+((1.0/(1.0-(VF/Ud)))*((-Vj_Area)+Vjo_Area))))));
charge=((Area*C0)*((Ud*(_logE((1.0-(Vjo/Ud)))-_logE((1.0-(Vj/Ud)))))+((1.0/(1.0-(VF/Ud)))*((U-Vj)+Vjo))));
}
}else
{{charge_U=((Area*C0)*(((Ud/(1.0-m))*(_d0_pow((1.0-(Vjo/Ud)),(1.0-m))*((-Vjo_U/Ud))+_d1_pow((1.0-(Vjo/Ud)),(1.0-m))*(0.0)-_d0_pow((1.0-(Vj/Ud)),(1.0-m))*((-Vj_U/Ud))+_d1_pow((1.0-(Vj/Ud)),(1.0-m))*(0.0)))+((_d0_pow((1.0-(VF/Ud)),(-m))*((-VF_U/Ud))+_d1_pow((1.0-(VF/Ud)),(-m))*(0.0)*((U-Vj)+Vjo))+(_pow((1.0-(VF/Ud)),(-m))*((1.0-Vj_U)+Vjo_U)))));
charge_C0=(((Area)*((((Ud/(1.0-m))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+(_pow((1.0-(VF/Ud)),(-m))*((U-Vj)+Vjo)))-(Ud*(1.0/(1.0-m)))))+((Area*C0)*(((Ud/(1.0-m))*(_d0_pow((1.0-(Vjo/Ud)),(1.0-m))*((-Vjo_C0/Ud))+_d1_pow((1.0-(Vjo/Ud)),(1.0-m))*(0.0)-_d0_pow((1.0-(Vj/Ud)),(1.0-m))*((-Vj_C0/Ud))+_d1_pow((1.0-(Vj/Ud)),(1.0-m))*(0.0)))+((_d0_pow((1.0-(VF/Ud)),(-m))*((-VF_C0/Ud))+_d1_pow((1.0-(VF/Ud)),(-m))*(0.0)*((U-Vj)+Vjo))+(_pow((1.0-(VF/Ud)),(-m))*((-Vj_C0)+Vjo_C0))))));
charge_Ud=((Area*C0)*(((((1/(1.0-m))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+((Ud/(1.0-m))*(_d0_pow((1.0-(Vjo/Ud)),(1.0-m))*((-((Vjo_Ud*Ud)-Vjo)/(Ud*Ud)))+_d1_pow((1.0-(Vjo/Ud)),(1.0-m))*(0.0)-_d0_pow((1.0-(Vj/Ud)),(1.0-m))*((-((Vj_Ud*Ud)-Vj)/(Ud*Ud)))+_d1_pow((1.0-(Vj/Ud)),(1.0-m))*(0.0))))+((_d0_pow((1.0-(VF/Ud)),(-m))*((-((VF_Ud*Ud)-VF)/(Ud*Ud)))+_d1_pow((1.0-(VF/Ud)),(-m))*(0.0)*((U-Vj)+Vjo))+(_pow((1.0-(VF/Ud)),(-m))*((-Vj_Ud)+Vjo_Ud))))-((1.0/(1.0-m)))));
charge_m=((Area*C0)*(((((-(Ud*(-1.0))/((1.0-m)*(1.0-m)))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+((Ud/(1.0-m))*(_d0_pow((1.0-(Vjo/Ud)),(1.0-m))*((-Vjo_m/Ud))+_d1_pow((1.0-(Vjo/Ud)),(1.0-m))*((-1.0))-_d0_pow((1.0-(Vj/Ud)),(1.0-m))*((-Vj_m/Ud))+_d1_pow((1.0-(Vj/Ud)),(1.0-m))*((-1.0)))))+((_d0_pow((1.0-(VF/Ud)),(-m))*((-VF_m/Ud))+_d1_pow((1.0-(VF/Ud)),(-m))*(0.0)*((U-Vj)+Vjo))+(_pow((1.0-(VF/Ud)),(-m))*((-Vj_m)+Vjo_m))))-(Ud*(-(-1.0)/((1.0-m)*(1.0-m))))));
charge_Area=(((C0)*((((Ud/(1.0-m))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+(_pow((1.0-(VF/Ud)),(-m))*((U-Vj)+Vjo)))-(Ud*(1.0/(1.0-m)))))+((Area*C0)*(((Ud/(1.0-m))*(_d0_pow((1.0-(Vjo/Ud)),(1.0-m))*((-Vjo_Area/Ud))+_d1_pow((1.0-(Vjo/Ud)),(1.0-m))*(0.0)-_d0_pow((1.0-(Vj/Ud)),(1.0-m))*((-Vj_Area/Ud))+_d1_pow((1.0-(Vj/Ud)),(1.0-m))*(0.0)))+((_d0_pow((1.0-(VF/Ud)),(-m))*((-VF_Area/Ud))+_d1_pow((1.0-(VF/Ud)),(-m))*(0.0)*((U-Vj)+Vjo))+(_pow((1.0-(VF/Ud)),(-m))*((-Vj_Area)+Vjo_Area))))));
charge=((Area*C0)*((((Ud/(1.0-m))*(_pow((1.0-(Vjo/Ud)),(1.0-m))-_pow((1.0-(Vj/Ud)),(1.0-m))))+(_pow((1.0-(VF/Ud)),(-m))*((U-Vj)+Vjo)))-(Ud*(1.0/(1.0-m)))));
}
}}
return charge_U*d_U+charge_C0*d_C0+charge_Ud*d_Ud+charge_m*d_m+charge_Area*d_Area;
}

/*
 * analog function: Vceff
 */
double HBT_X_Vceff (double U, double VCES)
{
double Vceff=0.0;
double Vth0=0.0;
{Vth0=0.025;
if((U<VCES))
Vceff=(Vth0+(Vth0*_logE((1.0+_exp((((U-VCES)/Vth0)-1.0))))));
else
Vceff=((U-VCES)+(Vth0*_logE((1.0+_exp((1.0-((U-VCES)/Vth0)))))));
}
return Vceff;
}
double HBT_X_d_Vceff (double U, double VCES, double d_U, double d_VCES)
{
double Vceff=0.0;
double Vceff_U=0.0;
double Vceff_VCES=0.0;
double Vth0=0.0;
double Vth0_U=0.0;
double Vth0_VCES=0.0;
{{Vth0_U=0.0;
Vth0_VCES=0.0;
Vth0=0.025;
}
if((U<VCES))
{Vceff_U=(Vth0_U+((Vth0_U*_logE((1.0+_exp((((U-VCES)/Vth0)-1.0)))))+(Vth0*_d0_logE((1.0+_exp((((U-VCES)/Vth0)-1.0))))*((+_d0_exp((((U-VCES)/Vth0)-1.0))*(((Vth0-((U-VCES)*Vth0_U))/(Vth0*Vth0))))))));
Vceff_VCES=(Vth0_VCES+((Vth0_VCES*_logE((1.0+_exp((((U-VCES)/Vth0)-1.0)))))+(Vth0*_d0_logE((1.0+_exp((((U-VCES)/Vth0)-1.0))))*((+_d0_exp((((U-VCES)/Vth0)-1.0))*(((-1.0)*Vth0-(U-VCES)*Vth0_VCES)/(Vth0*Vth0)))))));
Vceff=(Vth0+(Vth0*_logE((1.0+_exp((((U-VCES)/Vth0)-1.0))))));
}
else
{Vceff_U=(1.0+((Vth0_U*_logE((1.0+_exp((1.0-((U-VCES)/Vth0))))))+(Vth0*_d0_logE((1.0+_exp((1.0-((U-VCES)/Vth0)))))*((+_d0_exp((1.0-((U-VCES)/Vth0)))*((-((Vth0-((U-VCES)*Vth0_U))/(Vth0*Vth0)))))))));
Vceff_VCES=((-1.0)+((Vth0_VCES*_logE((1.0+_exp((1.0-((U-VCES)/Vth0))))))+(Vth0*_d0_logE((1.0+_exp((1.0-((U-VCES)/Vth0)))))*((+_d0_exp((1.0-((U-VCES)/Vth0)))*((-((-1.0)*Vth0-(U-VCES)*Vth0_VCES)/(Vth0*Vth0))))))));
Vceff=((U-VCES)+(Vth0*_logE((1.0+_exp((1.0-((U-VCES)/Vth0)))))));
}
}
return Vceff_U*d_U+Vceff_VCES*d_VCES;
}

/*
 * analog function: ICK
 */
double HBT_X_ICK (double U, double RCI0, double VLIM, double InvVPT, double VCES)
{
double ICK=0.0;
double VC=0.0;
double x=0.0;
{VC=HBT_X_Vceff(U,VCES);
x=((VC-VLIM)*InvVPT);
ICK=(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)));
}
return ICK;
}
double HBT_X_d_ICK (double U, double RCI0, double VLIM, double InvVPT, double VCES, double d_U, double d_RCI0, double d_VLIM, double d_InvVPT, double d_VCES)
{
double ICK=0.0;
double ICK_U=0.0;
double ICK_RCI0=0.0;
double ICK_VLIM=0.0;
double ICK_InvVPT=0.0;
double ICK_VCES=0.0;
double VC=0.0;
double VC_U=0.0;
double VC_RCI0=0.0;
double VC_VLIM=0.0;
double VC_InvVPT=0.0;
double VC_VCES=0.0;
double x=0.0;
double x_U=0.0;
double x_RCI0=0.0;
double x_VLIM=0.0;
double x_InvVPT=0.0;
double x_VCES=0.0;
{{VC_U=HBT_X_d_Vceff(U,VCES,(1.0),(0.0));
VC_RCI0=HBT_X_d_Vceff(U,VCES,(0.0),(0.0));
VC_VLIM=HBT_X_d_Vceff(U,VCES,(0.0),(0.0));
VC_InvVPT=HBT_X_d_Vceff(U,VCES,(0.0),(0.0));
VC_VCES=HBT_X_d_Vceff(U,VCES,(0.0),(1.0));
VC=HBT_X_Vceff(U,VCES);
}
{x_U=VC_U*InvVPT;
x_RCI0=VC_RCI0*InvVPT;
x_VLIM=(VC_VLIM-1.0)*InvVPT;
x_InvVPT=(VC_InvVPT*InvVPT)+(VC-VLIM);
x_VCES=VC_VCES*InvVPT;
x=((VC-VLIM)*InvVPT);
}
{ICK_U=((((VC_U/RCI0*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))+((VC/RCI0)*(-_d0_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*((+((VC_U/VLIM*(VC/VLIM))+((VC/VLIM)*VC_U/VLIM))))/(_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)))+(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(+(x_U+_d0_sqrt(((x*x)+0.001))*(((x_U*x)+(x*x_U))))/2.0)));
ICK_RCI0=((((((VC_RCI0*RCI0)-VC)/(RCI0*RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))+((VC/RCI0)*(-_d0_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*((+((VC_RCI0/VLIM*(VC/VLIM))+((VC/VLIM)*VC_RCI0/VLIM))))/(_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)))+(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(+(x_RCI0+_d0_sqrt(((x*x)+0.001))*(((x_RCI0*x)+(x*x_RCI0))))/2.0)));
ICK_VLIM=((((VC_VLIM/RCI0*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))+((VC/RCI0)*(-_d0_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*((+((((VC_VLIM*VLIM)-VC)/(VLIM*VLIM)*(VC/VLIM))+((VC/VLIM)*((VC_VLIM*VLIM)-VC)/(VLIM*VLIM)))))/(_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)))+(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(+(x_VLIM+_d0_sqrt(((x*x)+0.001))*(((x_VLIM*x)+(x*x_VLIM))))/2.0)));
ICK_InvVPT=((((VC_InvVPT/RCI0*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))+((VC/RCI0)*(-_d0_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*((+((VC_InvVPT/VLIM*(VC/VLIM))+((VC/VLIM)*VC_InvVPT/VLIM))))/(_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)))+(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(+(x_InvVPT+_d0_sqrt(((x*x)+0.001))*(((x_InvVPT*x)+(x*x_InvVPT))))/2.0)));
ICK_VCES=((((VC_VCES/RCI0*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))+((VC/RCI0)*(-_d0_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*((+((VC_VCES/VLIM*(VC/VLIM))+((VC/VLIM)*VC_VCES/VLIM))))/(_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))*_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)))+(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(+(x_VCES+_d0_sqrt(((x*x)+0.001))*(((x_VCES*x)+(x*x_VCES))))/2.0)));
ICK=(((VC/RCI0)*(1.0/_sqrt((1.0+((VC/VLIM)*(VC/VLIM))))))*(1.0+((x+_sqrt(((x*x)+0.001)))/2.0)));
}
}
return ICK_U*d_U+ICK_RCI0*d_RCI0+ICK_VLIM*d_VLIM+ICK_InvVPT*d_InvVPT+ICK_VCES*d_VCES;
}
