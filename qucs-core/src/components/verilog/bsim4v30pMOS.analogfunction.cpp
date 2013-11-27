/*
 * bsim4v30pMOS.analogfunction.cpp - analog function implementations
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include "bsim4v30pMOS.analogfunction.h"
#include <math.h>

#include "real.h"
using namespace qucs;


/* math functions and appropriate derivatives */
inline double _cos(double arg)             { return  qucs::cos(arg); }
inline double _d0_cos(double arg)          { return (-qucs::sin(arg)); }
inline double _sin(double arg)             { return  qucs::sin(arg); }
inline double _d0_sin(double arg)          { return (qucs::cos(arg)); }
inline double _tan(double arg)             { return  qucs::tan(arg); }
inline double _d0_tan(double arg)          { return (1.0/qucs::cos(arg)/qucs::cos(arg)); }
inline double _cosh(double arg)            { return  qucs::cosh(arg); }
inline double _d0_cosh(double arg)         { return (qucs::sinh(arg)); }
inline double _sinh(double arg)            { return  qucs::sinh(arg); }
inline double _d0_sinh(double arg)         { return (qucs::cosh(arg)); }
inline double _tanh(double arg)            { return  qucs::tanh(arg); }
inline double _d0_tanh(double arg)         { return (1.0/qucs::cosh(arg)/qucs::cosh(arg)); }
inline double _acos(double arg)            { return  std::acos(arg); }
inline double _d0_acos(double arg)         { return (-1.0/qucs::sqrt(1-arg*arg)); }
inline double _asin(double arg)            { return  std::asin(arg); }
inline double _d0_asin(double arg)         { return (+1.0/qucs::sqrt(1-arg*arg)); }
inline double _atan(double arg)            { return  std::atan(arg); }
inline double _d0_atan(double arg)         { return (+1.0/(1+arg*arg)); }
inline double _atanh(double arg)           { return  ::atanh(arg); }
inline double _d0_atanh(double arg)        { return (+1.0/(1-arg*arg)); }
inline double _logE(double arg)            { return  qucs::log(arg); }
inline double _d0_logE(double arg)         { return (1.0/arg); }
inline double _log10(double arg)           { return  std::log10(arg); }
inline double _d0_log10(double arg)        { return (1.0/arg/qucs::log(10.0)); }
inline double _exp(double arg)             { return  qucs::exp(arg); }
inline double _d0_exp(double arg)          { return qucs::exp(arg); }
inline double _sqrt(double arg)            { return  qucs::sqrt(arg); }
inline double _d0_sqrt(double arg)         { return (1.0/qucs::sqrt(arg)/2.0); }
inline double _abs(double arg)             { return  qucs::abs(arg); }
inline double _d0_abs(double arg)          { return (((arg)>=0)?(+1.0):(-1.0)); }
inline int _floor(double arg)              { return  std::floor(arg); }
inline int _d0_floor(double)               { return (1.0); }

inline double _hypot(double x,double y)    { return qucs::sqrt((x)*(x)+(y)*(y)); }
inline double _d0_hypot(double x,double y) { return (x)/qucs::sqrt((x)*(x)+(y)*(y)); }
inline double _d1_hypot(double x,double y) { return (y)/qucs::sqrt((x)*(x)+(y)*(y)); }

inline double _max(double x,double y)      { return ((x)>(y))?(x):(y); }
inline double _d0_max(double x,double y)   { return ((x)>(y))?1.0:0.0; }
inline double _d1_max(double x,double y)   { return ((x)>(y))?0.0:1.0; }

inline double _min(double x,double y)      { return ((x)<(y))?(x):(y); }
inline double _d0_min(double x,double y)   { return ((x)<(y))?1.0:0.0; }
inline double _d1_min(double x,double y)   { return ((x)<(y))?0.0:1.0; }

inline double _pow(double x,double y)      { return pow(x,y); }
inline double _d0_pow(double x,double y)   { return (x==0.0)?0.0:((y/x)*pow(x,y)); }
inline double _d1_pow(double x,double y)   { return (x==0.0)?0.0:((qucs::log(x)/qucs::exp(0.0))*pow(x,y)); }

inline double _limexp(double arg)          { return ((arg)<(80))?(qucs::exp(arg)):(qucs::exp(80.0)*(1.0+(arg-80))); }
inline double _d0_limexp(double arg)       { return ((arg)<(80))?(qucs::exp(arg)):(qucs::exp(80.0)); }

inline double _vt(double arg)              { return 1.3806503e-23*arg/1.602176462e-19; }
inline double _d0_vt(double)               { return 1.3806503e-23/1.602176462e-19; }


/*
 * analog function: get_nuintd
 */
double bsim4v30pMOS_get_nuintd (double nf, double minsd)
{
double get_nuintd=0.0;
{if((((int)nf%(int)2)!=0))
get_nuintd=(2.0*_max(((nf-1.0)/2.0),0.0));
else
{if((minsd==1))
get_nuintd=(2.0*_max(((nf/2.0)-1.0),0.0));
else
get_nuintd=nf;
}}
return get_nuintd;
}
double bsim4v30pMOS_d_get_nuintd (double nf, double minsd, double d_nf, double d_minsd)
{
double get_nuintd=0.0;
double get_nuintd_nf=0.0;
double get_nuintd_minsd=0.0;
{if((((int)nf%(int)2)!=0))
{get_nuintd_nf=(2.0*_d0_max(((nf-1.0)/2.0),0.0)*((1/2.0))+_d1_max(((nf-1.0)/2.0),0.0)*(0.0));
get_nuintd_minsd=(2.0*_d0_max(((nf-1.0)/2.0),0.0)*(0.0)+_d1_max(((nf-1.0)/2.0),0.0)*(0.0));
get_nuintd=(2.0*_max(((nf-1.0)/2.0),0.0));
}
else
{if((minsd==1))
{get_nuintd_nf=(2.0*_d0_max(((nf/2.0)-1.0),0.0)*((1/2.0))+_d1_max(((nf/2.0)-1.0),0.0)*(0.0));
get_nuintd_minsd=(2.0*_d0_max(((nf/2.0)-1.0),0.0)*(0.0)+_d1_max(((nf/2.0)-1.0),0.0)*(0.0));
get_nuintd=(2.0*_max(((nf/2.0)-1.0),0.0));
}
else
{get_nuintd_nf=1.0;
get_nuintd_minsd=0.0;
get_nuintd=nf;
}
}}
return get_nuintd_nf*d_nf+get_nuintd_minsd*d_minsd;
}

/*
 * analog function: get_nuendd
 */
double bsim4v30pMOS_get_nuendd (double nf, double minsd)
{
double get_nuendd=0.0;
{if((((int)nf%(int)2)!=0))
get_nuendd=1.0;
else
{if((minsd==1))
get_nuendd=2.0;
else
get_nuendd=0.0;
}}
return get_nuendd;
}
double bsim4v30pMOS_d_get_nuendd (double nf, double minsd, double d_nf, double d_minsd)
{
double get_nuendd=0.0;
double get_nuendd_nf=0.0;
double get_nuendd_minsd=0.0;
{if((((int)nf%(int)2)!=0))
{get_nuendd_nf=0.0;
get_nuendd_minsd=0.0;
get_nuendd=1.0;
}
else
{if((minsd==1))
{get_nuendd_nf=0.0;
get_nuendd_minsd=0.0;
get_nuendd=2.0;
}
else
{get_nuendd_nf=0.0;
get_nuendd_minsd=0.0;
get_nuendd=0.0;
}
}}
return get_nuendd_nf*d_nf+get_nuendd_minsd*d_minsd;
}

/*
 * analog function: get_nuints
 */
double bsim4v30pMOS_get_nuints (double nf, double minsd)
{
double get_nuints=0.0;
{if((((int)nf%(int)2)!=0))
get_nuints=(2.0*_max(((nf-1.0)/2.0),0.0));
else
{if((minsd==1))
get_nuints=nf;
else
get_nuints=(2.0*_max(((nf/2.0)-1.0),0.0));
}}
return get_nuints;
}
double bsim4v30pMOS_d_get_nuints (double nf, double minsd, double d_nf, double d_minsd)
{
double get_nuints=0.0;
double get_nuints_nf=0.0;
double get_nuints_minsd=0.0;
{if((((int)nf%(int)2)!=0))
{get_nuints_nf=(2.0*_d0_max(((nf-1.0)/2.0),0.0)*((1/2.0))+_d1_max(((nf-1.0)/2.0),0.0)*(0.0));
get_nuints_minsd=(2.0*_d0_max(((nf-1.0)/2.0),0.0)*(0.0)+_d1_max(((nf-1.0)/2.0),0.0)*(0.0));
get_nuints=(2.0*_max(((nf-1.0)/2.0),0.0));
}
else
{if((minsd==1))
{get_nuints_nf=1.0;
get_nuints_minsd=0.0;
get_nuints=nf;
}
else
{get_nuints_nf=(2.0*_d0_max(((nf/2.0)-1.0),0.0)*((1/2.0))+_d1_max(((nf/2.0)-1.0),0.0)*(0.0));
get_nuints_minsd=(2.0*_d0_max(((nf/2.0)-1.0),0.0)*(0.0)+_d1_max(((nf/2.0)-1.0),0.0)*(0.0));
get_nuints=(2.0*_max(((nf/2.0)-1.0),0.0));
}
}}
return get_nuints_nf*d_nf+get_nuints_minsd*d_minsd;
}

/*
 * analog function: get_nuends
 */
double bsim4v30pMOS_get_nuends (double nf, double minsd)
{
double get_nuends=0.0;
{if((((int)nf%(int)2)!=0))
get_nuends=1.0;
else
{if((minsd==1))
get_nuends=0.0;
else
get_nuends=2.0;
}}
return get_nuends;
}
double bsim4v30pMOS_d_get_nuends (double nf, double minsd, double d_nf, double d_minsd)
{
double get_nuends=0.0;
double get_nuends_nf=0.0;
double get_nuends_minsd=0.0;
{if((((int)nf%(int)2)!=0))
{get_nuends_nf=0.0;
get_nuends_minsd=0.0;
get_nuends=1.0;
}
else
{if((minsd==1))
{get_nuends_nf=0.0;
get_nuends_minsd=0.0;
get_nuends=0.0;
}
else
{get_nuends_nf=0.0;
get_nuends_minsd=0.0;
get_nuends=2.0;
}
}}
return get_nuends_nf*d_nf+get_nuends_minsd*d_minsd;
}

/*
 * analog function: get_ps
 */
double bsim4v30pMOS_get_ps (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg)
{
double get_ps=0.0;
double psiso=0.0;
double pssha=0.0;
double psmer=0.0;
double t0=0.0;
double t1=0.0;
double t2=0.0;
double nuints=0.0;
double nuends=0.0;
{nuints=0.0;
nuends=0.0;
if((geo<9))
{nuints=bsim4v30pMOS_get_nuints(nf,minsd);
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
}t0=(dmcg+dmci);
t1=(dmcg+dmcg);
t2=(dmdg+dmdg);
psiso=((t0+t0)+weffcj);
pssha=t1;
psmer=t2;
switch ((int)geo) {
  case 0: 
get_ps=((nuends*psiso)+(nuints*pssha));
 break;
  case 1: 
get_ps=((nuends*psiso)+(nuints*pssha));
 break;
  case 2: 
get_ps=((nuends+nuints)*pssha);
 break;
  case 3: 
get_ps=((nuends+nuints)*pssha);
 break;
  case 4: 
get_ps=((nuends*psiso)+(nuints*pssha));
 break;
  case 5: 
get_ps=((nuends+nuints)*pssha);
 break;
  case 6: 
get_ps=((nuends*psmer)+(nuints*pssha));
 break;
  case 7: 
get_ps=((nuends*psmer)+(nuints*pssha));
 break;
  case 8: 
get_ps=((nuends*psmer)+(nuints*pssha));
 break;
  case 9: 
get_ps=(psiso+((nf-1.0)*pssha));
 break;
  case 10: 
get_ps=(nf*pssha);
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %e not matched",geo); fprintf(stdout,"\n");
 break;
 }}
return get_ps;
}
double bsim4v30pMOS_d_get_ps (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg, double d_nf, double d_geo, double d_minsd, double d_weffcj, double d_dmcg, double d_dmci, double d_dmdg)
{
double get_ps=0.0;
double get_ps_nf=0.0;
double get_ps_geo=0.0;
double get_ps_minsd=0.0;
double get_ps_weffcj=0.0;
double get_ps_dmcg=0.0;
double get_ps_dmci=0.0;
double get_ps_dmdg=0.0;
double psiso=0.0;
double psiso_nf=0.0;
double psiso_geo=0.0;
double psiso_minsd=0.0;
double psiso_weffcj=0.0;
double psiso_dmcg=0.0;
double psiso_dmci=0.0;
double psiso_dmdg=0.0;
double pssha=0.0;
double pssha_nf=0.0;
double pssha_geo=0.0;
double pssha_minsd=0.0;
double pssha_weffcj=0.0;
double pssha_dmcg=0.0;
double pssha_dmci=0.0;
double pssha_dmdg=0.0;
double psmer=0.0;
double psmer_nf=0.0;
double psmer_geo=0.0;
double psmer_minsd=0.0;
double psmer_weffcj=0.0;
double psmer_dmcg=0.0;
double psmer_dmci=0.0;
double psmer_dmdg=0.0;
double t0=0.0;
double t0_nf=0.0;
double t0_geo=0.0;
double t0_minsd=0.0;
double t0_weffcj=0.0;
double t0_dmcg=0.0;
double t0_dmci=0.0;
double t0_dmdg=0.0;
double t1=0.0;
double t1_nf=0.0;
double t1_geo=0.0;
double t1_minsd=0.0;
double t1_weffcj=0.0;
double t1_dmcg=0.0;
double t1_dmci=0.0;
double t1_dmdg=0.0;
double t2=0.0;
double t2_nf=0.0;
double t2_geo=0.0;
double t2_minsd=0.0;
double t2_weffcj=0.0;
double t2_dmcg=0.0;
double t2_dmci=0.0;
double t2_dmdg=0.0;
double nuints=0.0;
double nuints_nf=0.0;
double nuints_geo=0.0;
double nuints_minsd=0.0;
double nuints_weffcj=0.0;
double nuints_dmcg=0.0;
double nuints_dmci=0.0;
double nuints_dmdg=0.0;
double nuends=0.0;
double nuends_nf=0.0;
double nuends_geo=0.0;
double nuends_minsd=0.0;
double nuends_weffcj=0.0;
double nuends_dmcg=0.0;
double nuends_dmci=0.0;
double nuends_dmdg=0.0;
{{nuints_nf=0.0;
nuints_geo=0.0;
nuints_minsd=0.0;
nuints_weffcj=0.0;
nuints_dmcg=0.0;
nuints_dmci=0.0;
nuints_dmdg=0.0;
nuints=0.0;
}
{nuends_nf=0.0;
nuends_geo=0.0;
nuends_minsd=0.0;
nuends_weffcj=0.0;
nuends_dmcg=0.0;
nuends_dmci=0.0;
nuends_dmdg=0.0;
nuends=0.0;
}
if((geo<9))
{{nuints_nf=bsim4v30pMOS_d_get_nuints(nf,minsd,(1.0),(0.0));
nuints_geo=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_minsd=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(1.0));
nuints_weffcj=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmcg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmci=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmdg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints=bsim4v30pMOS_get_nuints(nf,minsd);
}
{nuends_nf=bsim4v30pMOS_d_get_nuends(nf,minsd,(1.0),(0.0));
nuends_geo=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_minsd=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(1.0));
nuends_weffcj=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmcg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmci=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmdg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
}
}{t0_nf=0.0;
t0_geo=0.0;
t0_minsd=0.0;
t0_weffcj=0.0;
t0_dmcg=1.0;
t0_dmci=(+1.0);
t0_dmdg=0.0;
t0=(dmcg+dmci);
}
{t1_nf=0.0;
t1_geo=0.0;
t1_minsd=0.0;
t1_weffcj=0.0;
t1_dmcg=(1.0+1.0);
t1_dmci=0.0;
t1_dmdg=0.0;
t1=(dmcg+dmcg);
}
{t2_nf=0.0;
t2_geo=0.0;
t2_minsd=0.0;
t2_weffcj=0.0;
t2_dmcg=0.0;
t2_dmci=0.0;
t2_dmdg=(1.0+1.0);
t2=(dmdg+dmdg);
}
{psiso_nf=(t0_nf+t0_nf);
psiso_geo=(t0_geo+t0_geo);
psiso_minsd=(t0_minsd+t0_minsd);
psiso_weffcj=((t0_weffcj+t0_weffcj)+1.0);
psiso_dmcg=(t0_dmcg+t0_dmcg);
psiso_dmci=(t0_dmci+t0_dmci);
psiso_dmdg=(t0_dmdg+t0_dmdg);
psiso=((t0+t0)+weffcj);
}
{pssha_nf=t1_nf;
pssha_geo=t1_geo;
pssha_minsd=t1_minsd;
pssha_weffcj=t1_weffcj;
pssha_dmcg=t1_dmcg;
pssha_dmci=t1_dmci;
pssha_dmdg=t1_dmdg;
pssha=t1;
}
{psmer_nf=t2_nf;
psmer_geo=t2_geo;
psmer_minsd=t2_minsd;
psmer_weffcj=t2_weffcj;
psmer_dmcg=t2_dmcg;
psmer_dmci=t2_dmci;
psmer_dmdg=t2_dmdg;
psmer=t2;
}
switch ((int)geo) {
  case 0: 
{get_ps_nf=(((nuends_nf*psiso)+(nuends*psiso_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psiso)+(nuends*psiso_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psiso)+(nuends*psiso_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psiso)+(nuends*psiso_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psiso)+(nuends*psiso_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psiso)+(nuends*psiso_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psiso)+(nuends*psiso_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psiso)+(nuints*pssha));
}
 break;
  case 1: 
{get_ps_nf=(((nuends_nf*psiso)+(nuends*psiso_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psiso)+(nuends*psiso_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psiso)+(nuends*psiso_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psiso)+(nuends*psiso_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psiso)+(nuends*psiso_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psiso)+(nuends*psiso_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psiso)+(nuends*psiso_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psiso)+(nuints*pssha));
}
 break;
  case 2: 
{get_ps_nf=(((nuends_nf+nuints_nf)*pssha)+((nuends+nuints)*pssha_nf));
get_ps_geo=(((nuends_geo+nuints_geo)*pssha)+((nuends+nuints)*pssha_geo));
get_ps_minsd=(((nuends_minsd+nuints_minsd)*pssha)+((nuends+nuints)*pssha_minsd));
get_ps_weffcj=(((nuends_weffcj+nuints_weffcj)*pssha)+((nuends+nuints)*pssha_weffcj));
get_ps_dmcg=(((nuends_dmcg+nuints_dmcg)*pssha)+((nuends+nuints)*pssha_dmcg));
get_ps_dmci=(((nuends_dmci+nuints_dmci)*pssha)+((nuends+nuints)*pssha_dmci));
get_ps_dmdg=(((nuends_dmdg+nuints_dmdg)*pssha)+((nuends+nuints)*pssha_dmdg));
get_ps=((nuends+nuints)*pssha);
}
 break;
  case 3: 
{get_ps_nf=(((nuends_nf+nuints_nf)*pssha)+((nuends+nuints)*pssha_nf));
get_ps_geo=(((nuends_geo+nuints_geo)*pssha)+((nuends+nuints)*pssha_geo));
get_ps_minsd=(((nuends_minsd+nuints_minsd)*pssha)+((nuends+nuints)*pssha_minsd));
get_ps_weffcj=(((nuends_weffcj+nuints_weffcj)*pssha)+((nuends+nuints)*pssha_weffcj));
get_ps_dmcg=(((nuends_dmcg+nuints_dmcg)*pssha)+((nuends+nuints)*pssha_dmcg));
get_ps_dmci=(((nuends_dmci+nuints_dmci)*pssha)+((nuends+nuints)*pssha_dmci));
get_ps_dmdg=(((nuends_dmdg+nuints_dmdg)*pssha)+((nuends+nuints)*pssha_dmdg));
get_ps=((nuends+nuints)*pssha);
}
 break;
  case 4: 
{get_ps_nf=(((nuends_nf*psiso)+(nuends*psiso_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psiso)+(nuends*psiso_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psiso)+(nuends*psiso_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psiso)+(nuends*psiso_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psiso)+(nuends*psiso_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psiso)+(nuends*psiso_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psiso)+(nuends*psiso_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psiso)+(nuints*pssha));
}
 break;
  case 5: 
{get_ps_nf=(((nuends_nf+nuints_nf)*pssha)+((nuends+nuints)*pssha_nf));
get_ps_geo=(((nuends_geo+nuints_geo)*pssha)+((nuends+nuints)*pssha_geo));
get_ps_minsd=(((nuends_minsd+nuints_minsd)*pssha)+((nuends+nuints)*pssha_minsd));
get_ps_weffcj=(((nuends_weffcj+nuints_weffcj)*pssha)+((nuends+nuints)*pssha_weffcj));
get_ps_dmcg=(((nuends_dmcg+nuints_dmcg)*pssha)+((nuends+nuints)*pssha_dmcg));
get_ps_dmci=(((nuends_dmci+nuints_dmci)*pssha)+((nuends+nuints)*pssha_dmci));
get_ps_dmdg=(((nuends_dmdg+nuints_dmdg)*pssha)+((nuends+nuints)*pssha_dmdg));
get_ps=((nuends+nuints)*pssha);
}
 break;
  case 6: 
{get_ps_nf=(((nuends_nf*psmer)+(nuends*psmer_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psmer)+(nuends*psmer_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psmer)+(nuends*psmer_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psmer)+(nuends*psmer_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psmer)+(nuends*psmer_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psmer)+(nuends*psmer_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psmer)+(nuends*psmer_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psmer)+(nuints*pssha));
}
 break;
  case 7: 
{get_ps_nf=(((nuends_nf*psmer)+(nuends*psmer_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psmer)+(nuends*psmer_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psmer)+(nuends*psmer_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psmer)+(nuends*psmer_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psmer)+(nuends*psmer_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psmer)+(nuends*psmer_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psmer)+(nuends*psmer_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psmer)+(nuints*pssha));
}
 break;
  case 8: 
{get_ps_nf=(((nuends_nf*psmer)+(nuends*psmer_nf))+((nuints_nf*pssha)+(nuints*pssha_nf)));
get_ps_geo=(((nuends_geo*psmer)+(nuends*psmer_geo))+((nuints_geo*pssha)+(nuints*pssha_geo)));
get_ps_minsd=(((nuends_minsd*psmer)+(nuends*psmer_minsd))+((nuints_minsd*pssha)+(nuints*pssha_minsd)));
get_ps_weffcj=(((nuends_weffcj*psmer)+(nuends*psmer_weffcj))+((nuints_weffcj*pssha)+(nuints*pssha_weffcj)));
get_ps_dmcg=(((nuends_dmcg*psmer)+(nuends*psmer_dmcg))+((nuints_dmcg*pssha)+(nuints*pssha_dmcg)));
get_ps_dmci=(((nuends_dmci*psmer)+(nuends*psmer_dmci))+((nuints_dmci*pssha)+(nuints*pssha_dmci)));
get_ps_dmdg=(((nuends_dmdg*psmer)+(nuends*psmer_dmdg))+((nuints_dmdg*pssha)+(nuints*pssha_dmdg)));
get_ps=((nuends*psmer)+(nuints*pssha));
}
 break;
  case 9: 
{get_ps_nf=(psiso_nf+(pssha+(pssha_nf*(nf-1.0))));
get_ps_geo=(psiso_geo+((nf-1.0)*pssha_geo));
get_ps_minsd=(psiso_minsd+((nf-1.0)*pssha_minsd));
get_ps_weffcj=(psiso_weffcj+((nf-1.0)*pssha_weffcj));
get_ps_dmcg=(psiso_dmcg+((nf-1.0)*pssha_dmcg));
get_ps_dmci=(psiso_dmci+((nf-1.0)*pssha_dmci));
get_ps_dmdg=(psiso_dmdg+((nf-1.0)*pssha_dmdg));
get_ps=(psiso+((nf-1.0)*pssha));
}
 break;
  case 10: 
{get_ps_nf=(pssha+(pssha_nf*nf));
get_ps_geo=(nf*pssha_geo);
get_ps_minsd=(nf*pssha_minsd);
get_ps_weffcj=(nf*pssha_weffcj);
get_ps_dmcg=(nf*pssha_dmcg);
get_ps_dmci=(nf*pssha_dmci);
get_ps_dmdg=(nf*pssha_dmdg);
get_ps=(nf*pssha);
}
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %e not matched",geo);   fprintf(stdout,"\n");
 break;
 }}
return get_ps_nf*d_nf+get_ps_geo*d_geo+get_ps_minsd*d_minsd+get_ps_weffcj*d_weffcj+get_ps_dmcg*d_dmcg+get_ps_dmci*d_dmci+get_ps_dmdg*d_dmdg;
}

/*
 * analog function: get_pd
 */
double bsim4v30pMOS_get_pd (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg)
{
double get_pd=0.0;
double pdiso=0.0;
double pdsha=0.0;
double pdmer=0.0;
double t0=0.0;
double t1=0.0;
double t2=0.0;
double nuintd=0.0;
double nuendd=0.0;
{nuintd=0.0;
nuendd=0.0;
if((geo<9))
{nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
}t0=(dmcg+dmci);
t1=(dmcg+dmcg);
t2=(dmdg+dmdg);
pdiso=((t0+t0)+weffcj);
pdsha=t1;
pdmer=t2;
switch ((int)geo) {
  case 0: 
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
 break;
  case 1: 
get_pd=((nuendd+nuintd)*pdsha);
 break;
  case 2: 
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
 break;
  case 3: 
get_pd=((nuendd+nuintd)*pdsha);
 break;
  case 4: 
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
 break;
  case 5: 
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
 break;
  case 6: 
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
 break;
  case 7: 
get_pd=((nuendd+nuintd)*pdsha);
 break;
  case 8: 
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
 break;
  case 9: 
get_pd=(nf*pdsha);
 break;
  case 10: 
get_pd=(pdiso+((nf-1.0)*pdsha));
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo); fprintf(stdout,"\n");
 break;
 }}
return get_pd;
}
double bsim4v30pMOS_d_get_pd (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg, double d_nf, double d_geo, double d_minsd, double d_weffcj, double d_dmcg, double d_dmci, double d_dmdg)
{
double get_pd=0.0;
double get_pd_nf=0.0;
double get_pd_geo=0.0;
double get_pd_minsd=0.0;
double get_pd_weffcj=0.0;
double get_pd_dmcg=0.0;
double get_pd_dmci=0.0;
double get_pd_dmdg=0.0;
double pdiso=0.0;
double pdiso_nf=0.0;
double pdiso_geo=0.0;
double pdiso_minsd=0.0;
double pdiso_weffcj=0.0;
double pdiso_dmcg=0.0;
double pdiso_dmci=0.0;
double pdiso_dmdg=0.0;
double pdsha=0.0;
double pdsha_nf=0.0;
double pdsha_geo=0.0;
double pdsha_minsd=0.0;
double pdsha_weffcj=0.0;
double pdsha_dmcg=0.0;
double pdsha_dmci=0.0;
double pdsha_dmdg=0.0;
double pdmer=0.0;
double pdmer_nf=0.0;
double pdmer_geo=0.0;
double pdmer_minsd=0.0;
double pdmer_weffcj=0.0;
double pdmer_dmcg=0.0;
double pdmer_dmci=0.0;
double pdmer_dmdg=0.0;
double t0=0.0;
double t0_nf=0.0;
double t0_geo=0.0;
double t0_minsd=0.0;
double t0_weffcj=0.0;
double t0_dmcg=0.0;
double t0_dmci=0.0;
double t0_dmdg=0.0;
double t1=0.0;
double t1_nf=0.0;
double t1_geo=0.0;
double t1_minsd=0.0;
double t1_weffcj=0.0;
double t1_dmcg=0.0;
double t1_dmci=0.0;
double t1_dmdg=0.0;
double t2=0.0;
double t2_nf=0.0;
double t2_geo=0.0;
double t2_minsd=0.0;
double t2_weffcj=0.0;
double t2_dmcg=0.0;
double t2_dmci=0.0;
double t2_dmdg=0.0;
double nuintd=0.0;
double nuintd_nf=0.0;
double nuintd_geo=0.0;
double nuintd_minsd=0.0;
double nuintd_weffcj=0.0;
double nuintd_dmcg=0.0;
double nuintd_dmci=0.0;
double nuintd_dmdg=0.0;
double nuendd=0.0;
double nuendd_nf=0.0;
double nuendd_geo=0.0;
double nuendd_minsd=0.0;
double nuendd_weffcj=0.0;
double nuendd_dmcg=0.0;
double nuendd_dmci=0.0;
double nuendd_dmdg=0.0;
{{nuintd_nf=0.0;
nuintd_geo=0.0;
nuintd_minsd=0.0;
nuintd_weffcj=0.0;
nuintd_dmcg=0.0;
nuintd_dmci=0.0;
nuintd_dmdg=0.0;
nuintd=0.0;
}
{nuendd_nf=0.0;
nuendd_geo=0.0;
nuendd_minsd=0.0;
nuendd_weffcj=0.0;
nuendd_dmcg=0.0;
nuendd_dmci=0.0;
nuendd_dmdg=0.0;
nuendd=0.0;
}
if((geo<9))
{{nuintd_nf=bsim4v30pMOS_d_get_nuintd(nf,minsd,(1.0),(0.0));
nuintd_geo=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_minsd=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(1.0));
nuintd_weffcj=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmcg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmci=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmdg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
}
{nuendd_nf=bsim4v30pMOS_d_get_nuendd(nf,minsd,(1.0),(0.0));
nuendd_geo=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_minsd=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(1.0));
nuendd_weffcj=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmcg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmci=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmdg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
}
}{t0_nf=0.0;
t0_geo=0.0;
t0_minsd=0.0;
t0_weffcj=0.0;
t0_dmcg=1.0;
t0_dmci=(+1.0);
t0_dmdg=0.0;
t0=(dmcg+dmci);
}
{t1_nf=0.0;
t1_geo=0.0;
t1_minsd=0.0;
t1_weffcj=0.0;
t1_dmcg=(1.0+1.0);
t1_dmci=0.0;
t1_dmdg=0.0;
t1=(dmcg+dmcg);
}
{t2_nf=0.0;
t2_geo=0.0;
t2_minsd=0.0;
t2_weffcj=0.0;
t2_dmcg=0.0;
t2_dmci=0.0;
t2_dmdg=(1.0+1.0);
t2=(dmdg+dmdg);
}
{pdiso_nf=(t0_nf+t0_nf);
pdiso_geo=(t0_geo+t0_geo);
pdiso_minsd=(t0_minsd+t0_minsd);
pdiso_weffcj=((t0_weffcj+t0_weffcj)+1.0);
pdiso_dmcg=(t0_dmcg+t0_dmcg);
pdiso_dmci=(t0_dmci+t0_dmci);
pdiso_dmdg=(t0_dmdg+t0_dmdg);
pdiso=((t0+t0)+weffcj);
}
{pdsha_nf=t1_nf;
pdsha_geo=t1_geo;
pdsha_minsd=t1_minsd;
pdsha_weffcj=t1_weffcj;
pdsha_dmcg=t1_dmcg;
pdsha_dmci=t1_dmci;
pdsha_dmdg=t1_dmdg;
pdsha=t1;
}
{pdmer_nf=t2_nf;
pdmer_geo=t2_geo;
pdmer_minsd=t2_minsd;
pdmer_weffcj=t2_weffcj;
pdmer_dmcg=t2_dmcg;
pdmer_dmci=t2_dmci;
pdmer_dmdg=t2_dmdg;
pdmer=t2;
}
switch ((int)geo) {
  case 0: 
{get_pd_nf=(((nuendd_nf*pdiso)+(nuendd*pdiso_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdiso)+(nuendd*pdiso_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdiso)+(nuendd*pdiso_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdiso)+(nuendd*pdiso_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdiso)+(nuendd*pdiso_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdiso)+(nuendd*pdiso_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdiso)+(nuendd*pdiso_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
}
 break;
  case 1: 
{get_pd_nf=(((nuendd_nf+nuintd_nf)*pdsha)+((nuendd+nuintd)*pdsha_nf));
get_pd_geo=(((nuendd_geo+nuintd_geo)*pdsha)+((nuendd+nuintd)*pdsha_geo));
get_pd_minsd=(((nuendd_minsd+nuintd_minsd)*pdsha)+((nuendd+nuintd)*pdsha_minsd));
get_pd_weffcj=(((nuendd_weffcj+nuintd_weffcj)*pdsha)+((nuendd+nuintd)*pdsha_weffcj));
get_pd_dmcg=(((nuendd_dmcg+nuintd_dmcg)*pdsha)+((nuendd+nuintd)*pdsha_dmcg));
get_pd_dmci=(((nuendd_dmci+nuintd_dmci)*pdsha)+((nuendd+nuintd)*pdsha_dmci));
get_pd_dmdg=(((nuendd_dmdg+nuintd_dmdg)*pdsha)+((nuendd+nuintd)*pdsha_dmdg));
get_pd=((nuendd+nuintd)*pdsha);
}
 break;
  case 2: 
{get_pd_nf=(((nuendd_nf*pdiso)+(nuendd*pdiso_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdiso)+(nuendd*pdiso_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdiso)+(nuendd*pdiso_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdiso)+(nuendd*pdiso_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdiso)+(nuendd*pdiso_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdiso)+(nuendd*pdiso_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdiso)+(nuendd*pdiso_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
}
 break;
  case 3: 
{get_pd_nf=(((nuendd_nf+nuintd_nf)*pdsha)+((nuendd+nuintd)*pdsha_nf));
get_pd_geo=(((nuendd_geo+nuintd_geo)*pdsha)+((nuendd+nuintd)*pdsha_geo));
get_pd_minsd=(((nuendd_minsd+nuintd_minsd)*pdsha)+((nuendd+nuintd)*pdsha_minsd));
get_pd_weffcj=(((nuendd_weffcj+nuintd_weffcj)*pdsha)+((nuendd+nuintd)*pdsha_weffcj));
get_pd_dmcg=(((nuendd_dmcg+nuintd_dmcg)*pdsha)+((nuendd+nuintd)*pdsha_dmcg));
get_pd_dmci=(((nuendd_dmci+nuintd_dmci)*pdsha)+((nuendd+nuintd)*pdsha_dmci));
get_pd_dmdg=(((nuendd_dmdg+nuintd_dmdg)*pdsha)+((nuendd+nuintd)*pdsha_dmdg));
get_pd=((nuendd+nuintd)*pdsha);
}
 break;
  case 4: 
{get_pd_nf=(((nuendd_nf*pdmer)+(nuendd*pdmer_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdmer)+(nuendd*pdmer_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdmer)+(nuendd*pdmer_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdmer)+(nuendd*pdmer_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdmer)+(nuendd*pdmer_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdmer)+(nuendd*pdmer_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdmer)+(nuendd*pdmer_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
}
 break;
  case 5: 
{get_pd_nf=(((nuendd_nf*pdmer)+(nuendd*pdmer_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdmer)+(nuendd*pdmer_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdmer)+(nuendd*pdmer_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdmer)+(nuendd*pdmer_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdmer)+(nuendd*pdmer_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdmer)+(nuendd*pdmer_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdmer)+(nuendd*pdmer_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
}
 break;
  case 6: 
{get_pd_nf=(((nuendd_nf*pdiso)+(nuendd*pdiso_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdiso)+(nuendd*pdiso_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdiso)+(nuendd*pdiso_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdiso)+(nuendd*pdiso_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdiso)+(nuendd*pdiso_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdiso)+(nuendd*pdiso_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdiso)+(nuendd*pdiso_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdiso)+(nuintd*pdsha));
}
 break;
  case 7: 
{get_pd_nf=(((nuendd_nf+nuintd_nf)*pdsha)+((nuendd+nuintd)*pdsha_nf));
get_pd_geo=(((nuendd_geo+nuintd_geo)*pdsha)+((nuendd+nuintd)*pdsha_geo));
get_pd_minsd=(((nuendd_minsd+nuintd_minsd)*pdsha)+((nuendd+nuintd)*pdsha_minsd));
get_pd_weffcj=(((nuendd_weffcj+nuintd_weffcj)*pdsha)+((nuendd+nuintd)*pdsha_weffcj));
get_pd_dmcg=(((nuendd_dmcg+nuintd_dmcg)*pdsha)+((nuendd+nuintd)*pdsha_dmcg));
get_pd_dmci=(((nuendd_dmci+nuintd_dmci)*pdsha)+((nuendd+nuintd)*pdsha_dmci));
get_pd_dmdg=(((nuendd_dmdg+nuintd_dmdg)*pdsha)+((nuendd+nuintd)*pdsha_dmdg));
get_pd=((nuendd+nuintd)*pdsha);
}
 break;
  case 8: 
{get_pd_nf=(((nuendd_nf*pdmer)+(nuendd*pdmer_nf))+((nuintd_nf*pdsha)+(nuintd*pdsha_nf)));
get_pd_geo=(((nuendd_geo*pdmer)+(nuendd*pdmer_geo))+((nuintd_geo*pdsha)+(nuintd*pdsha_geo)));
get_pd_minsd=(((nuendd_minsd*pdmer)+(nuendd*pdmer_minsd))+((nuintd_minsd*pdsha)+(nuintd*pdsha_minsd)));
get_pd_weffcj=(((nuendd_weffcj*pdmer)+(nuendd*pdmer_weffcj))+((nuintd_weffcj*pdsha)+(nuintd*pdsha_weffcj)));
get_pd_dmcg=(((nuendd_dmcg*pdmer)+(nuendd*pdmer_dmcg))+((nuintd_dmcg*pdsha)+(nuintd*pdsha_dmcg)));
get_pd_dmci=(((nuendd_dmci*pdmer)+(nuendd*pdmer_dmci))+((nuintd_dmci*pdsha)+(nuintd*pdsha_dmci)));
get_pd_dmdg=(((nuendd_dmdg*pdmer)+(nuendd*pdmer_dmdg))+((nuintd_dmdg*pdsha)+(nuintd*pdsha_dmdg)));
get_pd=((nuendd*pdmer)+(nuintd*pdsha));
}
 break;
  case 9: 
{get_pd_nf=(pdsha+(pdsha_nf*nf));
get_pd_geo=(nf*pdsha_geo);
get_pd_minsd=(nf*pdsha_minsd);
get_pd_weffcj=(nf*pdsha_weffcj);
get_pd_dmcg=(nf*pdsha_dmcg);
get_pd_dmci=(nf*pdsha_dmci);
get_pd_dmdg=(nf*pdsha_dmdg);
get_pd=(nf*pdsha);
}
 break;
  case 10: 
{get_pd_nf=(pdiso_nf+(pdsha+(pdsha_nf*(nf-1.0))));
get_pd_geo=(pdiso_geo+((nf-1.0)*pdsha_geo));
get_pd_minsd=(pdiso_minsd+((nf-1.0)*pdsha_minsd));
get_pd_weffcj=(pdiso_weffcj+((nf-1.0)*pdsha_weffcj));
get_pd_dmcg=(pdiso_dmcg+((nf-1.0)*pdsha_dmcg));
get_pd_dmci=(pdiso_dmci+((nf-1.0)*pdsha_dmci));
get_pd_dmdg=(pdiso_dmdg+((nf-1.0)*pdsha_dmdg));
get_pd=(pdiso+((nf-1.0)*pdsha));
}
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo);   fprintf(stdout,"\n");
 break;
 }}
return get_pd_nf*d_nf+get_pd_geo*d_geo+get_pd_minsd*d_minsd+get_pd_weffcj*d_weffcj+get_pd_dmcg*d_dmcg+get_pd_dmci*d_dmci+get_pd_dmdg*d_dmdg;
}

/*
 * analog function: get_as
 */
double bsim4v30pMOS_get_as (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg)
{
double get_as=0.0;
double asiso=0.0;
double assha=0.0;
double asmer=0.0;
double t0=0.0;
double nuints=0.0;
double nuends=0.0;
{nuints=0.0;
nuends=0.0;
if((geo<9))
{nuints=bsim4v30pMOS_get_nuints(nf,minsd);
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
}t0=(dmcg+dmci);
asiso=(t0*weffcj);
assha=(dmcg*weffcj);
asmer=(dmdg*weffcj);
switch ((int)geo) {
  case 0: 
get_as=((nuends*asiso)+(nuints*assha));
 break;
  case 1: 
get_as=((nuends*asiso)+(nuints*assha));
 break;
  case 2: 
get_as=((nuends+nuints)*assha);
 break;
  case 3: 
get_as=((nuends+nuints)*assha);
 break;
  case 4: 
get_as=((nuends*asiso)+(nuints*assha));
 break;
  case 5: 
get_as=((nuends+nuints)*assha);
 break;
  case 6: 
get_as=((nuends*asmer)+(nuints*assha));
 break;
  case 7: 
get_as=((nuends*asmer)+(nuints*assha));
 break;
  case 8: 
get_as=((nuends*asmer)+(nuints*assha));
 break;
  case 9: 
get_as=(asiso+((nf-1.0)*assha));
 break;
  case 10: 
get_as=(nf*assha);
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo); fprintf(stdout,"\n");
 break;
 }}
return get_as;
}
double bsim4v30pMOS_d_get_as (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg, double d_nf, double d_geo, double d_minsd, double d_weffcj, double d_dmcg, double d_dmci, double d_dmdg)
{
double get_as=0.0;
double get_as_nf=0.0;
double get_as_geo=0.0;
double get_as_minsd=0.0;
double get_as_weffcj=0.0;
double get_as_dmcg=0.0;
double get_as_dmci=0.0;
double get_as_dmdg=0.0;
double asiso=0.0;
double asiso_nf=0.0;
double asiso_geo=0.0;
double asiso_minsd=0.0;
double asiso_weffcj=0.0;
double asiso_dmcg=0.0;
double asiso_dmci=0.0;
double asiso_dmdg=0.0;
double assha=0.0;
double assha_nf=0.0;
double assha_geo=0.0;
double assha_minsd=0.0;
double assha_weffcj=0.0;
double assha_dmcg=0.0;
double assha_dmci=0.0;
double assha_dmdg=0.0;
double asmer=0.0;
double asmer_nf=0.0;
double asmer_geo=0.0;
double asmer_minsd=0.0;
double asmer_weffcj=0.0;
double asmer_dmcg=0.0;
double asmer_dmci=0.0;
double asmer_dmdg=0.0;
double t0=0.0;
double t0_nf=0.0;
double t0_geo=0.0;
double t0_minsd=0.0;
double t0_weffcj=0.0;
double t0_dmcg=0.0;
double t0_dmci=0.0;
double t0_dmdg=0.0;
double nuints=0.0;
double nuints_nf=0.0;
double nuints_geo=0.0;
double nuints_minsd=0.0;
double nuints_weffcj=0.0;
double nuints_dmcg=0.0;
double nuints_dmci=0.0;
double nuints_dmdg=0.0;
double nuends=0.0;
double nuends_nf=0.0;
double nuends_geo=0.0;
double nuends_minsd=0.0;
double nuends_weffcj=0.0;
double nuends_dmcg=0.0;
double nuends_dmci=0.0;
double nuends_dmdg=0.0;
{{nuints_nf=0.0;
nuints_geo=0.0;
nuints_minsd=0.0;
nuints_weffcj=0.0;
nuints_dmcg=0.0;
nuints_dmci=0.0;
nuints_dmdg=0.0;
nuints=0.0;
}
{nuends_nf=0.0;
nuends_geo=0.0;
nuends_minsd=0.0;
nuends_weffcj=0.0;
nuends_dmcg=0.0;
nuends_dmci=0.0;
nuends_dmdg=0.0;
nuends=0.0;
}
if((geo<9))
{{nuints_nf=bsim4v30pMOS_d_get_nuints(nf,minsd,(1.0),(0.0));
nuints_geo=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_minsd=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(1.0));
nuints_weffcj=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmcg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmci=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmdg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints=bsim4v30pMOS_get_nuints(nf,minsd);
}
{nuends_nf=bsim4v30pMOS_d_get_nuends(nf,minsd,(1.0),(0.0));
nuends_geo=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_minsd=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(1.0));
nuends_weffcj=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmcg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmci=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmdg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
}
}{t0_nf=0.0;
t0_geo=0.0;
t0_minsd=0.0;
t0_weffcj=0.0;
t0_dmcg=1.0;
t0_dmci=(+1.0);
t0_dmdg=0.0;
t0=(dmcg+dmci);
}
{asiso_nf=t0_nf*weffcj;
asiso_geo=t0_geo*weffcj;
asiso_minsd=t0_minsd*weffcj;
asiso_weffcj=(t0_weffcj*weffcj)+t0;
asiso_dmcg=t0_dmcg*weffcj;
asiso_dmci=t0_dmci*weffcj;
asiso_dmdg=t0_dmdg*weffcj;
asiso=(t0*weffcj);
}
{assha_nf=0.0;
assha_geo=0.0;
assha_minsd=0.0;
assha_weffcj=(dmcg);
assha_dmcg=(weffcj);
assha_dmci=0.0;
assha_dmdg=0.0;
assha=(dmcg*weffcj);
}
{asmer_nf=0.0;
asmer_geo=0.0;
asmer_minsd=0.0;
asmer_weffcj=(dmdg);
asmer_dmcg=0.0;
asmer_dmci=0.0;
asmer_dmdg=(weffcj);
asmer=(dmdg*weffcj);
}
switch ((int)geo) {
  case 0: 
{get_as_nf=(((nuends_nf*asiso)+(nuends*asiso_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asiso)+(nuends*asiso_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asiso)+(nuends*asiso_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asiso)+(nuends*asiso_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asiso)+(nuends*asiso_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asiso)+(nuends*asiso_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asiso)+(nuends*asiso_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asiso)+(nuints*assha));
}
 break;
  case 1: 
{get_as_nf=(((nuends_nf*asiso)+(nuends*asiso_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asiso)+(nuends*asiso_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asiso)+(nuends*asiso_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asiso)+(nuends*asiso_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asiso)+(nuends*asiso_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asiso)+(nuends*asiso_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asiso)+(nuends*asiso_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asiso)+(nuints*assha));
}
 break;
  case 2: 
{get_as_nf=(((nuends_nf+nuints_nf)*assha)+((nuends+nuints)*assha_nf));
get_as_geo=(((nuends_geo+nuints_geo)*assha)+((nuends+nuints)*assha_geo));
get_as_minsd=(((nuends_minsd+nuints_minsd)*assha)+((nuends+nuints)*assha_minsd));
get_as_weffcj=(((nuends_weffcj+nuints_weffcj)*assha)+((nuends+nuints)*assha_weffcj));
get_as_dmcg=(((nuends_dmcg+nuints_dmcg)*assha)+((nuends+nuints)*assha_dmcg));
get_as_dmci=(((nuends_dmci+nuints_dmci)*assha)+((nuends+nuints)*assha_dmci));
get_as_dmdg=(((nuends_dmdg+nuints_dmdg)*assha)+((nuends+nuints)*assha_dmdg));
get_as=((nuends+nuints)*assha);
}
 break;
  case 3: 
{get_as_nf=(((nuends_nf+nuints_nf)*assha)+((nuends+nuints)*assha_nf));
get_as_geo=(((nuends_geo+nuints_geo)*assha)+((nuends+nuints)*assha_geo));
get_as_minsd=(((nuends_minsd+nuints_minsd)*assha)+((nuends+nuints)*assha_minsd));
get_as_weffcj=(((nuends_weffcj+nuints_weffcj)*assha)+((nuends+nuints)*assha_weffcj));
get_as_dmcg=(((nuends_dmcg+nuints_dmcg)*assha)+((nuends+nuints)*assha_dmcg));
get_as_dmci=(((nuends_dmci+nuints_dmci)*assha)+((nuends+nuints)*assha_dmci));
get_as_dmdg=(((nuends_dmdg+nuints_dmdg)*assha)+((nuends+nuints)*assha_dmdg));
get_as=((nuends+nuints)*assha);
}
 break;
  case 4: 
{get_as_nf=(((nuends_nf*asiso)+(nuends*asiso_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asiso)+(nuends*asiso_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asiso)+(nuends*asiso_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asiso)+(nuends*asiso_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asiso)+(nuends*asiso_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asiso)+(nuends*asiso_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asiso)+(nuends*asiso_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asiso)+(nuints*assha));
}
 break;
  case 5: 
{get_as_nf=(((nuends_nf+nuints_nf)*assha)+((nuends+nuints)*assha_nf));
get_as_geo=(((nuends_geo+nuints_geo)*assha)+((nuends+nuints)*assha_geo));
get_as_minsd=(((nuends_minsd+nuints_minsd)*assha)+((nuends+nuints)*assha_minsd));
get_as_weffcj=(((nuends_weffcj+nuints_weffcj)*assha)+((nuends+nuints)*assha_weffcj));
get_as_dmcg=(((nuends_dmcg+nuints_dmcg)*assha)+((nuends+nuints)*assha_dmcg));
get_as_dmci=(((nuends_dmci+nuints_dmci)*assha)+((nuends+nuints)*assha_dmci));
get_as_dmdg=(((nuends_dmdg+nuints_dmdg)*assha)+((nuends+nuints)*assha_dmdg));
get_as=((nuends+nuints)*assha);
}
 break;
  case 6: 
{get_as_nf=(((nuends_nf*asmer)+(nuends*asmer_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asmer)+(nuends*asmer_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asmer)+(nuends*asmer_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asmer)+(nuends*asmer_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asmer)+(nuends*asmer_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asmer)+(nuends*asmer_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asmer)+(nuends*asmer_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asmer)+(nuints*assha));
}
 break;
  case 7: 
{get_as_nf=(((nuends_nf*asmer)+(nuends*asmer_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asmer)+(nuends*asmer_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asmer)+(nuends*asmer_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asmer)+(nuends*asmer_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asmer)+(nuends*asmer_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asmer)+(nuends*asmer_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asmer)+(nuends*asmer_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asmer)+(nuints*assha));
}
 break;
  case 8: 
{get_as_nf=(((nuends_nf*asmer)+(nuends*asmer_nf))+((nuints_nf*assha)+(nuints*assha_nf)));
get_as_geo=(((nuends_geo*asmer)+(nuends*asmer_geo))+((nuints_geo*assha)+(nuints*assha_geo)));
get_as_minsd=(((nuends_minsd*asmer)+(nuends*asmer_minsd))+((nuints_minsd*assha)+(nuints*assha_minsd)));
get_as_weffcj=(((nuends_weffcj*asmer)+(nuends*asmer_weffcj))+((nuints_weffcj*assha)+(nuints*assha_weffcj)));
get_as_dmcg=(((nuends_dmcg*asmer)+(nuends*asmer_dmcg))+((nuints_dmcg*assha)+(nuints*assha_dmcg)));
get_as_dmci=(((nuends_dmci*asmer)+(nuends*asmer_dmci))+((nuints_dmci*assha)+(nuints*assha_dmci)));
get_as_dmdg=(((nuends_dmdg*asmer)+(nuends*asmer_dmdg))+((nuints_dmdg*assha)+(nuints*assha_dmdg)));
get_as=((nuends*asmer)+(nuints*assha));
}
 break;
  case 9: 
{get_as_nf=(asiso_nf+(assha+(assha_nf*(nf-1.0))));
get_as_geo=(asiso_geo+((nf-1.0)*assha_geo));
get_as_minsd=(asiso_minsd+((nf-1.0)*assha_minsd));
get_as_weffcj=(asiso_weffcj+((nf-1.0)*assha_weffcj));
get_as_dmcg=(asiso_dmcg+((nf-1.0)*assha_dmcg));
get_as_dmci=(asiso_dmci+((nf-1.0)*assha_dmci));
get_as_dmdg=(asiso_dmdg+((nf-1.0)*assha_dmdg));
get_as=(asiso+((nf-1.0)*assha));
}
 break;
  case 10: 
{get_as_nf=(assha+(assha_nf*nf));
get_as_geo=(nf*assha_geo);
get_as_minsd=(nf*assha_minsd);
get_as_weffcj=(nf*assha_weffcj);
get_as_dmcg=(nf*assha_dmcg);
get_as_dmci=(nf*assha_dmci);
get_as_dmdg=(nf*assha_dmdg);
get_as=(nf*assha);
}
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo);   fprintf(stdout,"\n");
 break;
 }}
return get_as_nf*d_nf+get_as_geo*d_geo+get_as_minsd*d_minsd+get_as_weffcj*d_weffcj+get_as_dmcg*d_dmcg+get_as_dmci*d_dmci+get_as_dmdg*d_dmdg;
}

/*
 * analog function: get_ad
 */
double bsim4v30pMOS_get_ad (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg)
{
double get_ad=0.0;
double adiso=0.0;
double adsha=0.0;
double admer=0.0;
double t0=0.0;
double nuintd=0.0;
double nuendd=0.0;
{nuintd=0.0;
nuendd=0.0;
if((geo<9))
{nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
}t0=(dmcg+dmci);
adiso=(t0*weffcj);
adsha=(dmcg*weffcj);
admer=(dmdg*weffcj);
switch ((int)geo) {
  case 0: 
get_ad=((nuendd*adiso)+(nuintd*adsha));
 break;
  case 1: 
get_ad=((nuendd+nuintd)*adsha);
 break;
  case 2: 
get_ad=((nuendd*adiso)+(nuintd*adsha));
 break;
  case 3: 
get_ad=((nuendd+nuintd)*adsha);
 break;
  case 4: 
get_ad=((nuendd*admer)+(nuintd*adsha));
 break;
  case 5: 
get_ad=((nuendd*admer)+(nuintd*adsha));
 break;
  case 6: 
get_ad=((nuendd*adiso)+(nuintd*adsha));
 break;
  case 7: 
get_ad=((nuendd+nuintd)*adsha);
 break;
  case 8: 
get_ad=((nuendd*admer)+(nuintd*adsha));
 break;
  case 9: 
get_ad=(nf*adsha);
 break;
  case 10: 
get_ad=(adiso+((nf-1.0)*adsha));
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo); fprintf(stdout,"\n");
 break;
 }}
return get_ad;
}
double bsim4v30pMOS_d_get_ad (double nf, double geo, double minsd, double weffcj, double dmcg, double dmci, double dmdg, double d_nf, double d_geo, double d_minsd, double d_weffcj, double d_dmcg, double d_dmci, double d_dmdg)
{
double get_ad=0.0;
double get_ad_nf=0.0;
double get_ad_geo=0.0;
double get_ad_minsd=0.0;
double get_ad_weffcj=0.0;
double get_ad_dmcg=0.0;
double get_ad_dmci=0.0;
double get_ad_dmdg=0.0;
double adiso=0.0;
double adiso_nf=0.0;
double adiso_geo=0.0;
double adiso_minsd=0.0;
double adiso_weffcj=0.0;
double adiso_dmcg=0.0;
double adiso_dmci=0.0;
double adiso_dmdg=0.0;
double adsha=0.0;
double adsha_nf=0.0;
double adsha_geo=0.0;
double adsha_minsd=0.0;
double adsha_weffcj=0.0;
double adsha_dmcg=0.0;
double adsha_dmci=0.0;
double adsha_dmdg=0.0;
double admer=0.0;
double admer_nf=0.0;
double admer_geo=0.0;
double admer_minsd=0.0;
double admer_weffcj=0.0;
double admer_dmcg=0.0;
double admer_dmci=0.0;
double admer_dmdg=0.0;
double t0=0.0;
double t0_nf=0.0;
double t0_geo=0.0;
double t0_minsd=0.0;
double t0_weffcj=0.0;
double t0_dmcg=0.0;
double t0_dmci=0.0;
double t0_dmdg=0.0;
double nuintd=0.0;
double nuintd_nf=0.0;
double nuintd_geo=0.0;
double nuintd_minsd=0.0;
double nuintd_weffcj=0.0;
double nuintd_dmcg=0.0;
double nuintd_dmci=0.0;
double nuintd_dmdg=0.0;
double nuendd=0.0;
double nuendd_nf=0.0;
double nuendd_geo=0.0;
double nuendd_minsd=0.0;
double nuendd_weffcj=0.0;
double nuendd_dmcg=0.0;
double nuendd_dmci=0.0;
double nuendd_dmdg=0.0;
{{nuintd_nf=0.0;
nuintd_geo=0.0;
nuintd_minsd=0.0;
nuintd_weffcj=0.0;
nuintd_dmcg=0.0;
nuintd_dmci=0.0;
nuintd_dmdg=0.0;
nuintd=0.0;
}
{nuendd_nf=0.0;
nuendd_geo=0.0;
nuendd_minsd=0.0;
nuendd_weffcj=0.0;
nuendd_dmcg=0.0;
nuendd_dmci=0.0;
nuendd_dmdg=0.0;
nuendd=0.0;
}
if((geo<9))
{{nuintd_nf=bsim4v30pMOS_d_get_nuintd(nf,minsd,(1.0),(0.0));
nuintd_geo=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_minsd=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(1.0));
nuintd_weffcj=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmcg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmci=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmdg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
}
{nuendd_nf=bsim4v30pMOS_d_get_nuendd(nf,minsd,(1.0),(0.0));
nuendd_geo=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_minsd=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(1.0));
nuendd_weffcj=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmcg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmci=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmdg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
}
}{t0_nf=0.0;
t0_geo=0.0;
t0_minsd=0.0;
t0_weffcj=0.0;
t0_dmcg=1.0;
t0_dmci=(+1.0);
t0_dmdg=0.0;
t0=(dmcg+dmci);
}
{adiso_nf=t0_nf*weffcj;
adiso_geo=t0_geo*weffcj;
adiso_minsd=t0_minsd*weffcj;
adiso_weffcj=(t0_weffcj*weffcj)+t0;
adiso_dmcg=t0_dmcg*weffcj;
adiso_dmci=t0_dmci*weffcj;
adiso_dmdg=t0_dmdg*weffcj;
adiso=(t0*weffcj);
}
{adsha_nf=0.0;
adsha_geo=0.0;
adsha_minsd=0.0;
adsha_weffcj=(dmcg);
adsha_dmcg=(weffcj);
adsha_dmci=0.0;
adsha_dmdg=0.0;
adsha=(dmcg*weffcj);
}
{admer_nf=0.0;
admer_geo=0.0;
admer_minsd=0.0;
admer_weffcj=(dmdg);
admer_dmcg=0.0;
admer_dmci=0.0;
admer_dmdg=(weffcj);
admer=(dmdg*weffcj);
}
switch ((int)geo) {
  case 0: 
{get_ad_nf=(((nuendd_nf*adiso)+(nuendd*adiso_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*adiso)+(nuendd*adiso_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*adiso)+(nuendd*adiso_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*adiso)+(nuendd*adiso_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*adiso)+(nuendd*adiso_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*adiso)+(nuendd*adiso_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*adiso)+(nuendd*adiso_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*adiso)+(nuintd*adsha));
}
 break;
  case 1: 
{get_ad_nf=(((nuendd_nf+nuintd_nf)*adsha)+((nuendd+nuintd)*adsha_nf));
get_ad_geo=(((nuendd_geo+nuintd_geo)*adsha)+((nuendd+nuintd)*adsha_geo));
get_ad_minsd=(((nuendd_minsd+nuintd_minsd)*adsha)+((nuendd+nuintd)*adsha_minsd));
get_ad_weffcj=(((nuendd_weffcj+nuintd_weffcj)*adsha)+((nuendd+nuintd)*adsha_weffcj));
get_ad_dmcg=(((nuendd_dmcg+nuintd_dmcg)*adsha)+((nuendd+nuintd)*adsha_dmcg));
get_ad_dmci=(((nuendd_dmci+nuintd_dmci)*adsha)+((nuendd+nuintd)*adsha_dmci));
get_ad_dmdg=(((nuendd_dmdg+nuintd_dmdg)*adsha)+((nuendd+nuintd)*adsha_dmdg));
get_ad=((nuendd+nuintd)*adsha);
}
 break;
  case 2: 
{get_ad_nf=(((nuendd_nf*adiso)+(nuendd*adiso_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*adiso)+(nuendd*adiso_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*adiso)+(nuendd*adiso_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*adiso)+(nuendd*adiso_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*adiso)+(nuendd*adiso_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*adiso)+(nuendd*adiso_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*adiso)+(nuendd*adiso_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*adiso)+(nuintd*adsha));
}
 break;
  case 3: 
{get_ad_nf=(((nuendd_nf+nuintd_nf)*adsha)+((nuendd+nuintd)*adsha_nf));
get_ad_geo=(((nuendd_geo+nuintd_geo)*adsha)+((nuendd+nuintd)*adsha_geo));
get_ad_minsd=(((nuendd_minsd+nuintd_minsd)*adsha)+((nuendd+nuintd)*adsha_minsd));
get_ad_weffcj=(((nuendd_weffcj+nuintd_weffcj)*adsha)+((nuendd+nuintd)*adsha_weffcj));
get_ad_dmcg=(((nuendd_dmcg+nuintd_dmcg)*adsha)+((nuendd+nuintd)*adsha_dmcg));
get_ad_dmci=(((nuendd_dmci+nuintd_dmci)*adsha)+((nuendd+nuintd)*adsha_dmci));
get_ad_dmdg=(((nuendd_dmdg+nuintd_dmdg)*adsha)+((nuendd+nuintd)*adsha_dmdg));
get_ad=((nuendd+nuintd)*adsha);
}
 break;
  case 4: 
{get_ad_nf=(((nuendd_nf*admer)+(nuendd*admer_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*admer)+(nuendd*admer_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*admer)+(nuendd*admer_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*admer)+(nuendd*admer_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*admer)+(nuendd*admer_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*admer)+(nuendd*admer_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*admer)+(nuendd*admer_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*admer)+(nuintd*adsha));
}
 break;
  case 5: 
{get_ad_nf=(((nuendd_nf*admer)+(nuendd*admer_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*admer)+(nuendd*admer_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*admer)+(nuendd*admer_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*admer)+(nuendd*admer_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*admer)+(nuendd*admer_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*admer)+(nuendd*admer_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*admer)+(nuendd*admer_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*admer)+(nuintd*adsha));
}
 break;
  case 6: 
{get_ad_nf=(((nuendd_nf*adiso)+(nuendd*adiso_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*adiso)+(nuendd*adiso_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*adiso)+(nuendd*adiso_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*adiso)+(nuendd*adiso_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*adiso)+(nuendd*adiso_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*adiso)+(nuendd*adiso_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*adiso)+(nuendd*adiso_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*adiso)+(nuintd*adsha));
}
 break;
  case 7: 
{get_ad_nf=(((nuendd_nf+nuintd_nf)*adsha)+((nuendd+nuintd)*adsha_nf));
get_ad_geo=(((nuendd_geo+nuintd_geo)*adsha)+((nuendd+nuintd)*adsha_geo));
get_ad_minsd=(((nuendd_minsd+nuintd_minsd)*adsha)+((nuendd+nuintd)*adsha_minsd));
get_ad_weffcj=(((nuendd_weffcj+nuintd_weffcj)*adsha)+((nuendd+nuintd)*adsha_weffcj));
get_ad_dmcg=(((nuendd_dmcg+nuintd_dmcg)*adsha)+((nuendd+nuintd)*adsha_dmcg));
get_ad_dmci=(((nuendd_dmci+nuintd_dmci)*adsha)+((nuendd+nuintd)*adsha_dmci));
get_ad_dmdg=(((nuendd_dmdg+nuintd_dmdg)*adsha)+((nuendd+nuintd)*adsha_dmdg));
get_ad=((nuendd+nuintd)*adsha);
}
 break;
  case 8: 
{get_ad_nf=(((nuendd_nf*admer)+(nuendd*admer_nf))+((nuintd_nf*adsha)+(nuintd*adsha_nf)));
get_ad_geo=(((nuendd_geo*admer)+(nuendd*admer_geo))+((nuintd_geo*adsha)+(nuintd*adsha_geo)));
get_ad_minsd=(((nuendd_minsd*admer)+(nuendd*admer_minsd))+((nuintd_minsd*adsha)+(nuintd*adsha_minsd)));
get_ad_weffcj=(((nuendd_weffcj*admer)+(nuendd*admer_weffcj))+((nuintd_weffcj*adsha)+(nuintd*adsha_weffcj)));
get_ad_dmcg=(((nuendd_dmcg*admer)+(nuendd*admer_dmcg))+((nuintd_dmcg*adsha)+(nuintd*adsha_dmcg)));
get_ad_dmci=(((nuendd_dmci*admer)+(nuendd*admer_dmci))+((nuintd_dmci*adsha)+(nuintd*adsha_dmci)));
get_ad_dmdg=(((nuendd_dmdg*admer)+(nuendd*admer_dmdg))+((nuintd_dmdg*adsha)+(nuintd*adsha_dmdg)));
get_ad=((nuendd*admer)+(nuintd*adsha));
}
 break;
  case 9: 
{get_ad_nf=(adsha+(adsha_nf*nf));
get_ad_geo=(nf*adsha_geo);
get_ad_minsd=(nf*adsha_minsd);
get_ad_weffcj=(nf*adsha_weffcj);
get_ad_dmcg=(nf*adsha_dmcg);
get_ad_dmci=(nf*adsha_dmci);
get_ad_dmdg=(nf*adsha_dmdg);
get_ad=(nf*adsha);
}
 break;
  case 10: 
{get_ad_nf=(adiso_nf+(adsha+(adsha_nf*(nf-1.0))));
get_ad_geo=(adiso_geo+((nf-1.0)*adsha_geo));
get_ad_minsd=(adiso_minsd+((nf-1.0)*adsha_minsd));
get_ad_weffcj=(adiso_weffcj+((nf-1.0)*adsha_weffcj));
get_ad_dmcg=(adiso_dmcg+((nf-1.0)*adsha_dmcg));
get_ad_dmci=(adiso_dmci+((nf-1.0)*adsha_dmci));
get_ad_dmdg=(adiso_dmdg+((nf-1.0)*adsha_dmdg));
get_ad=(adiso+((nf-1.0)*adsha));
}
 break;
  default: 
fprintf(stdout,"Warning: Specified GEO = %d not matched",geo);   fprintf(stdout,"\n");
 break;
 }}
return get_ad_nf*d_nf+get_ad_geo*d_geo+get_ad_minsd*d_minsd+get_ad_weffcj*d_weffcj+get_ad_dmcg*d_dmcg+get_ad_dmci*d_dmci+get_ad_dmdg*d_dmdg;
}

/*
 * analog function: get_rendi
 */
double bsim4v30pMOS_get_rendi (double weffcj, double rsh, double dmcg, double dmci, double dmdg, double nuend, double rgeo, double type)
{
double get_rendi=0.0;
{if((type==1))
{switch ((int)rgeo) {
  case 1: case 2: case 5: 
{if((nuend==0.0))
get_rendi=0.0;
else
get_rendi=((rsh*dmcg)/(weffcj*nuend));
} break;
  case 3: case 4: case 6: 
{if(((dmcg+dmci)==0.0))
fprintf(stdout,"(dmcg + dmci) can not be equal to zero"); fprintf(stdout,"\n");
if((nuend==0.0))
get_rendi=0.0;
else
get_rendi=((rsh*weffcj)/((3.0*nuend)*(dmcg+dmci)));
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo); fprintf(stdout,"\n");
 break;
 }}else
{switch ((int)rgeo) {
  case 1: case 3: case 7: 
{if((nuend==0.0))
get_rendi=0.0;
else
get_rendi=((rsh*dmcg)/(weffcj*nuend));
} break;
  case 2: case 4: case 8: 
{if(((dmcg+dmci)==0.0))
fprintf(stdout,"(dmcg + dmci) can not be equal to zero"); fprintf(stdout,"\n");
if((nuend==0.0))
get_rendi=0.0;
else
get_rendi=((rsh*weffcj)/((3.0*nuend)*(dmcg+dmci)));
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo); fprintf(stdout,"\n");
 break;
 }}}
return get_rendi;
}
double bsim4v30pMOS_d_get_rendi (double weffcj, double rsh, double dmcg, double dmci, double dmdg, double nuend, double rgeo, double type, double d_weffcj, double d_rsh, double d_dmcg, double d_dmci, double d_dmdg, double d_nuend, double d_rgeo, double d_type)
{
double get_rendi=0.0;
double get_rendi_weffcj=0.0;
double get_rendi_rsh=0.0;
double get_rendi_dmcg=0.0;
double get_rendi_dmci=0.0;
double get_rendi_dmdg=0.0;
double get_rendi_nuend=0.0;
double get_rendi_rgeo=0.0;
double get_rendi_type=0.0;
{if((type==1))
{switch ((int)rgeo) {
  case 1: case 2: case 5: 
{if((nuend==0.0))
{get_rendi_weffcj=0.0;
get_rendi_rsh=0.0;
get_rendi_dmcg=0.0;
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=0.0;
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=0.0;
}
else
{get_rendi_weffcj=(-((rsh*dmcg)*(nuend))/((weffcj*nuend)*(weffcj*nuend)));
get_rendi_rsh=(dmcg)/(weffcj*nuend);
get_rendi_dmcg=(rsh)/(weffcj*nuend);
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=(-((rsh*dmcg)*(weffcj))/((weffcj*nuend)*(weffcj*nuend)));
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=((rsh*dmcg)/(weffcj*nuend));
}
} break;
  case 3: case 4: case 6: 
{if(((dmcg+dmci)==0.0))
fprintf(stdout,"(dmcg + dmci) can not be equal to zero");   fprintf(stdout,"\n");
if((nuend==0.0))
{get_rendi_weffcj=0.0;
get_rendi_rsh=0.0;
get_rendi_dmcg=0.0;
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=0.0;
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=0.0;
}
else
{get_rendi_weffcj=(rsh)/((3.0*nuend)*(dmcg+dmci));
get_rendi_rsh=(weffcj)/((3.0*nuend)*(dmcg+dmci));
get_rendi_dmcg=(-((rsh*weffcj)*((3.0*nuend)))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_dmci=(-((rsh*weffcj)*((3.0*nuend)*(+1.0)))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_dmdg=0.0;
get_rendi_nuend=(-((rsh*weffcj)*(3.0)*(dmcg+dmci))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=((rsh*weffcj)/((3.0*nuend)*(dmcg+dmci)));
}
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo);   fprintf(stdout,"\n");
 break;
 }}else
{switch ((int)rgeo) {
  case 1: case 3: case 7: 
{if((nuend==0.0))
{get_rendi_weffcj=0.0;
get_rendi_rsh=0.0;
get_rendi_dmcg=0.0;
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=0.0;
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=0.0;
}
else
{get_rendi_weffcj=(-((rsh*dmcg)*(nuend))/((weffcj*nuend)*(weffcj*nuend)));
get_rendi_rsh=(dmcg)/(weffcj*nuend);
get_rendi_dmcg=(rsh)/(weffcj*nuend);
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=(-((rsh*dmcg)*(weffcj))/((weffcj*nuend)*(weffcj*nuend)));
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=((rsh*dmcg)/(weffcj*nuend));
}
} break;
  case 2: case 4: case 8: 
{if(((dmcg+dmci)==0.0))
fprintf(stdout,"(dmcg + dmci) can not be equal to zero");   fprintf(stdout,"\n");
if((nuend==0.0))
{get_rendi_weffcj=0.0;
get_rendi_rsh=0.0;
get_rendi_dmcg=0.0;
get_rendi_dmci=0.0;
get_rendi_dmdg=0.0;
get_rendi_nuend=0.0;
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=0.0;
}
else
{get_rendi_weffcj=(rsh)/((3.0*nuend)*(dmcg+dmci));
get_rendi_rsh=(weffcj)/((3.0*nuend)*(dmcg+dmci));
get_rendi_dmcg=(-((rsh*weffcj)*((3.0*nuend)))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_dmci=(-((rsh*weffcj)*((3.0*nuend)*(+1.0)))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_dmdg=0.0;
get_rendi_nuend=(-((rsh*weffcj)*(3.0)*(dmcg+dmci))/(((3.0*nuend)*(dmcg+dmci))*((3.0*nuend)*(dmcg+dmci))));
get_rendi_rgeo=0.0;
get_rendi_type=0.0;
get_rendi=((rsh*weffcj)/((3.0*nuend)*(dmcg+dmci)));
}
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo);   fprintf(stdout,"\n");
 break;
 }}}
return get_rendi_weffcj*d_weffcj+get_rendi_rsh*d_rsh+get_rendi_dmcg*d_dmcg+get_rendi_dmci*d_dmci+get_rendi_dmdg*d_dmdg+get_rendi_nuend*d_nuend+get_rendi_rgeo*d_rgeo+get_rendi_type*d_type;
}

/*
 * analog function: get_renda
 */
double bsim4v30pMOS_get_renda (double weffcj, double rsh, double dmcg, double dmci, double dmdg, double nuend, double rgeo, double type)
{
double get_renda=0.0;
{if((type==1))
{switch ((int)rgeo) {
  case 1: case 2: case 5: 
{if((nuend==0.0))
get_renda=0.0;
else
get_renda=((rsh*dmcg)/(weffcj*nuend));
} break;
  case 3: case 4: case 6: 
{if((dmcg==0.0))
fprintf(stdout,"dmcg can not be equal to zero"); fprintf(stdout,"\n");
if((nuend==0.0))
get_renda=0.0;
else
get_renda=((rsh*weffcj)/((6.0*nuend)*dmcg));
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo); fprintf(stdout,"\n");
 break;
 }}else
{switch ((int)rgeo) {
  case 1: case 3: case 7: 
{if((nuend==0.0))
get_renda=0.0;
else
get_renda=((rsh*dmcg)/(weffcj*nuend));
} break;
  case 2: case 4: case 8: 
{if((dmcg==0.0))
fprintf(stdout,"dmcg can not be equal to zero"); fprintf(stdout,"\n");
if((nuend==0.0))
get_renda=0.0;
else
get_renda=((rsh*weffcj)/((6.0*nuend)*dmcg));
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo); fprintf(stdout,"\n");
 break;
 }}}
return get_renda;
}
double bsim4v30pMOS_d_get_renda (double weffcj, double rsh, double dmcg, double dmci, double dmdg, double nuend, double rgeo, double type, double d_weffcj, double d_rsh, double d_dmcg, double d_dmci, double d_dmdg, double d_nuend, double d_rgeo, double d_type)
{
double get_renda=0.0;
double get_renda_weffcj=0.0;
double get_renda_rsh=0.0;
double get_renda_dmcg=0.0;
double get_renda_dmci=0.0;
double get_renda_dmdg=0.0;
double get_renda_nuend=0.0;
double get_renda_rgeo=0.0;
double get_renda_type=0.0;
{if((type==1))
{switch ((int)rgeo) {
  case 1: case 2: case 5: 
{if((nuend==0.0))
{get_renda_weffcj=0.0;
get_renda_rsh=0.0;
get_renda_dmcg=0.0;
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=0.0;
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=0.0;
}
else
{get_renda_weffcj=(-((rsh*dmcg)*(nuend))/((weffcj*nuend)*(weffcj*nuend)));
get_renda_rsh=(dmcg)/(weffcj*nuend);
get_renda_dmcg=(rsh)/(weffcj*nuend);
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=(-((rsh*dmcg)*(weffcj))/((weffcj*nuend)*(weffcj*nuend)));
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=((rsh*dmcg)/(weffcj*nuend));
}
} break;
  case 3: case 4: case 6: 
{if((dmcg==0.0))
fprintf(stdout,"dmcg can not be equal to zero");   fprintf(stdout,"\n");
if((nuend==0.0))
{get_renda_weffcj=0.0;
get_renda_rsh=0.0;
get_renda_dmcg=0.0;
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=0.0;
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=0.0;
}
else
{get_renda_weffcj=(rsh)/((6.0*nuend)*dmcg);
get_renda_rsh=(weffcj)/((6.0*nuend)*dmcg);
get_renda_dmcg=(-((rsh*weffcj)*((6.0*nuend)))/(((6.0*nuend)*dmcg)*((6.0*nuend)*dmcg)));
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=(-((rsh*weffcj)*(6.0)*dmcg)/(((6.0*nuend)*dmcg)*((6.0*nuend)*dmcg)));
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=((rsh*weffcj)/((6.0*nuend)*dmcg));
}
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo);   fprintf(stdout,"\n");
 break;
 }}else
{switch ((int)rgeo) {
  case 1: case 3: case 7: 
{if((nuend==0.0))
{get_renda_weffcj=0.0;
get_renda_rsh=0.0;
get_renda_dmcg=0.0;
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=0.0;
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=0.0;
}
else
{get_renda_weffcj=(-((rsh*dmcg)*(nuend))/((weffcj*nuend)*(weffcj*nuend)));
get_renda_rsh=(dmcg)/(weffcj*nuend);
get_renda_dmcg=(rsh)/(weffcj*nuend);
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=(-((rsh*dmcg)*(weffcj))/((weffcj*nuend)*(weffcj*nuend)));
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=((rsh*dmcg)/(weffcj*nuend));
}
} break;
  case 2: case 4: case 8: 
{if((dmcg==0.0))
fprintf(stdout,"dmcg can not be equal to zero");   fprintf(stdout,"\n");
if((nuend==0.0))
{get_renda_weffcj=0.0;
get_renda_rsh=0.0;
get_renda_dmcg=0.0;
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=0.0;
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=0.0;
}
else
{get_renda_weffcj=(rsh)/((6.0*nuend)*dmcg);
get_renda_rsh=(weffcj)/((6.0*nuend)*dmcg);
get_renda_dmcg=(-((rsh*weffcj)*((6.0*nuend)))/(((6.0*nuend)*dmcg)*((6.0*nuend)*dmcg)));
get_renda_dmci=0.0;
get_renda_dmdg=0.0;
get_renda_nuend=(-((rsh*weffcj)*(6.0)*dmcg)/(((6.0*nuend)*dmcg)*((6.0*nuend)*dmcg)));
get_renda_rgeo=0.0;
get_renda_type=0.0;
get_renda=((rsh*weffcj)/((6.0*nuend)*dmcg));
}
} break;
  default: 
fprintf(stdout,"warning: specified rgeo = %d not matched",rgeo);   fprintf(stdout,"\n");
 break;
 }}}
return get_renda_weffcj*d_weffcj+get_renda_rsh*d_rsh+get_renda_dmcg*d_dmcg+get_renda_dmci*d_dmci+get_renda_dmdg*d_dmdg+get_renda_nuend*d_nuend+get_renda_rgeo*d_rgeo+get_renda_type*d_type;
}

/*
 * analog function: get_rtot
 */
double bsim4v30pMOS_get_rtot (double nf, double geo, double rgeo, double minsd, double weffcj, double rsh, double dmcg, double dmci, double dmdg, double type)
{
double get_rtot=0.0;
double rint=0.0;
double rend=0.0;
double nuintd=0.0;
double nuendd=0.0;
double nuints=0.0;
double nuends=0.0;
double DUMMY=0.0;
double rtot=0.0;
{rend=0.0;
nuintd=0.0;
nuendd=0.0;
nuints=0.0;
nuends=0.0;
if((geo<9))
{nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
nuints=bsim4v30pMOS_get_nuints(nf,minsd);
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
if((type==1))
{if((nuints==0.0))
rint=0.0;
else
rint=((rsh*dmcg)/(weffcj*nuints));
}else
{if((nuintd==0.0))
rint=0.0;
else
rint=((rsh*dmcg)/(weffcj*nuintd));
}}switch ((int)geo) {
  case 0: 
{if((type==1))
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 1: 
{if((type==1))
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 2: 
{if((type==1))
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 3: 
{if((type==1))
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 4: 
{if((type==1))
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=((rsh*dmdg)/weffcj);
} break;
  case 5: 
{if((type==1))
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
else
rend=((rsh*dmdg)/(weffcj*nuendd));
} break;
  case 6: 
{if((type==1))
rend=((rsh*dmdg)/weffcj);
else
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 7: 
{if((type==1))
rend=((rsh*dmdg)/(weffcj*nuends));
else
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
} break;
  case 8: 
{rend=((rsh*dmdg)/weffcj);
} break;
  case 9: 
{if((type==1))
{rend=(((0.5*rsh)*dmcg)/weffcj);
if((nf==2.0))
rint=0.0;
else
rint=((rsh*dmcg)/(weffcj*(nf-2.0)));
}else
{rend=0.0;
rint=((rsh*dmcg)/(weffcj*nf));
}} break;
  case 10: 
{if((type==1))
{rend=0.0;
rint=((rsh*dmcg)/(weffcj*nf));
}else
{rend=(((0.5*rsh)*dmcg)/weffcj);
;if((nf==2.0))
rint=0.0;
else
rint=((rsh*dmcg)/(weffcj*(nf-2.0)));
}} break;
  default: 
fprintf(stdout,"Warning: specified geo = %d not matched",geo); fprintf(stdout,"\n");
 break;
 }if((rint<=0.0))
get_rtot=rend;
else
if((rend<=0.0))
get_rtot=rint;
else
get_rtot=((rint*rend)/(rint+rend));
if((get_rtot==0.0))
fprintf(stdout,"Warning: zero resistance returned from get_rtot"); fprintf(stdout,"\n");
}
return get_rtot;
}
double bsim4v30pMOS_d_get_rtot (double nf, double geo, double rgeo, double minsd, double weffcj, double rsh, double dmcg, double dmci, double dmdg, double type, double d_nf, double d_geo, double d_rgeo, double d_minsd, double d_weffcj, double d_rsh, double d_dmcg, double d_dmci, double d_dmdg, double d_type)
{
double get_rtot=0.0;
double get_rtot_nf=0.0;
double get_rtot_geo=0.0;
double get_rtot_rgeo=0.0;
double get_rtot_minsd=0.0;
double get_rtot_weffcj=0.0;
double get_rtot_rsh=0.0;
double get_rtot_dmcg=0.0;
double get_rtot_dmci=0.0;
double get_rtot_dmdg=0.0;
double get_rtot_type=0.0;
double rint=0.0;
double rint_nf=0.0;
double rint_geo=0.0;
double rint_rgeo=0.0;
double rint_minsd=0.0;
double rint_weffcj=0.0;
double rint_rsh=0.0;
double rint_dmcg=0.0;
double rint_dmci=0.0;
double rint_dmdg=0.0;
double rint_type=0.0;
double rend=0.0;
double rend_nf=0.0;
double rend_geo=0.0;
double rend_rgeo=0.0;
double rend_minsd=0.0;
double rend_weffcj=0.0;
double rend_rsh=0.0;
double rend_dmcg=0.0;
double rend_dmci=0.0;
double rend_dmdg=0.0;
double rend_type=0.0;
double nuintd=0.0;
double nuintd_nf=0.0;
double nuintd_geo=0.0;
double nuintd_rgeo=0.0;
double nuintd_minsd=0.0;
double nuintd_weffcj=0.0;
double nuintd_rsh=0.0;
double nuintd_dmcg=0.0;
double nuintd_dmci=0.0;
double nuintd_dmdg=0.0;
double nuintd_type=0.0;
double nuendd=0.0;
double nuendd_nf=0.0;
double nuendd_geo=0.0;
double nuendd_rgeo=0.0;
double nuendd_minsd=0.0;
double nuendd_weffcj=0.0;
double nuendd_rsh=0.0;
double nuendd_dmcg=0.0;
double nuendd_dmci=0.0;
double nuendd_dmdg=0.0;
double nuendd_type=0.0;
double nuints=0.0;
double nuints_nf=0.0;
double nuints_geo=0.0;
double nuints_rgeo=0.0;
double nuints_minsd=0.0;
double nuints_weffcj=0.0;
double nuints_rsh=0.0;
double nuints_dmcg=0.0;
double nuints_dmci=0.0;
double nuints_dmdg=0.0;
double nuints_type=0.0;
double nuends=0.0;
double nuends_nf=0.0;
double nuends_geo=0.0;
double nuends_rgeo=0.0;
double nuends_minsd=0.0;
double nuends_weffcj=0.0;
double nuends_rsh=0.0;
double nuends_dmcg=0.0;
double nuends_dmci=0.0;
double nuends_dmdg=0.0;
double nuends_type=0.0;
double DUMMY=0.0;
double DUMMY_nf=0.0;
double DUMMY_geo=0.0;
double DUMMY_rgeo=0.0;
double DUMMY_minsd=0.0;
double DUMMY_weffcj=0.0;
double DUMMY_rsh=0.0;
double DUMMY_dmcg=0.0;
double DUMMY_dmci=0.0;
double DUMMY_dmdg=0.0;
double DUMMY_type=0.0;
double rtot=0.0;
double rtot_nf=0.0;
double rtot_geo=0.0;
double rtot_rgeo=0.0;
double rtot_minsd=0.0;
double rtot_weffcj=0.0;
double rtot_rsh=0.0;
double rtot_dmcg=0.0;
double rtot_dmci=0.0;
double rtot_dmdg=0.0;
double rtot_type=0.0;
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=0.0;
rend_rsh=0.0;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=0.0;
rend_type=0.0;
rend=0.0;
}
{nuintd_nf=0.0;
nuintd_geo=0.0;
nuintd_rgeo=0.0;
nuintd_minsd=0.0;
nuintd_weffcj=0.0;
nuintd_rsh=0.0;
nuintd_dmcg=0.0;
nuintd_dmci=0.0;
nuintd_dmdg=0.0;
nuintd_type=0.0;
nuintd=0.0;
}
{nuendd_nf=0.0;
nuendd_geo=0.0;
nuendd_rgeo=0.0;
nuendd_minsd=0.0;
nuendd_weffcj=0.0;
nuendd_rsh=0.0;
nuendd_dmcg=0.0;
nuendd_dmci=0.0;
nuendd_dmdg=0.0;
nuendd_type=0.0;
nuendd=0.0;
}
{nuints_nf=0.0;
nuints_geo=0.0;
nuints_rgeo=0.0;
nuints_minsd=0.0;
nuints_weffcj=0.0;
nuints_rsh=0.0;
nuints_dmcg=0.0;
nuints_dmci=0.0;
nuints_dmdg=0.0;
nuints_type=0.0;
nuints=0.0;
}
{nuends_nf=0.0;
nuends_geo=0.0;
nuends_rgeo=0.0;
nuends_minsd=0.0;
nuends_weffcj=0.0;
nuends_rsh=0.0;
nuends_dmcg=0.0;
nuends_dmci=0.0;
nuends_dmdg=0.0;
nuends_type=0.0;
nuends=0.0;
}
if((geo<9))
{{nuintd_nf=bsim4v30pMOS_d_get_nuintd(nf,minsd,(1.0),(0.0));
nuintd_geo=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_rgeo=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_minsd=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(1.0));
nuintd_weffcj=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_rsh=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmcg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmci=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_dmdg=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd_type=bsim4v30pMOS_d_get_nuintd(nf,minsd,(0.0),(0.0));
nuintd=bsim4v30pMOS_get_nuintd(nf,minsd);
}
{nuints_nf=bsim4v30pMOS_d_get_nuints(nf,minsd,(1.0),(0.0));
nuints_geo=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_rgeo=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_minsd=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(1.0));
nuints_weffcj=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_rsh=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmcg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmci=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_dmdg=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints_type=bsim4v30pMOS_d_get_nuints(nf,minsd,(0.0),(0.0));
nuints=bsim4v30pMOS_get_nuints(nf,minsd);
}
{nuendd_nf=bsim4v30pMOS_d_get_nuendd(nf,minsd,(1.0),(0.0));
nuendd_geo=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_rgeo=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_minsd=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(1.0));
nuendd_weffcj=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_rsh=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmcg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmci=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_dmdg=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd_type=bsim4v30pMOS_d_get_nuendd(nf,minsd,(0.0),(0.0));
nuendd=bsim4v30pMOS_get_nuendd(nf,minsd);
}
{nuends_nf=bsim4v30pMOS_d_get_nuends(nf,minsd,(1.0),(0.0));
nuends_geo=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_rgeo=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_minsd=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(1.0));
nuends_weffcj=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_rsh=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmcg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmci=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_dmdg=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends_type=bsim4v30pMOS_d_get_nuends(nf,minsd,(0.0),(0.0));
nuends=bsim4v30pMOS_get_nuends(nf,minsd);
}
if((type==1))
{if((nuints==0.0))
{rint_nf=0.0;
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=0.0;
rint_rsh=0.0;
rint_dmcg=0.0;
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=0.0;
}
else
{rint_nf=(-((rsh*dmcg)*(weffcj*nuints_nf))/((weffcj*nuints)*(weffcj*nuints)));
rint_geo=(-((rsh*dmcg)*(weffcj*nuints_geo))/((weffcj*nuints)*(weffcj*nuints)));
rint_rgeo=(-((rsh*dmcg)*(weffcj*nuints_rgeo))/((weffcj*nuints)*(weffcj*nuints)));
rint_minsd=(-((rsh*dmcg)*(weffcj*nuints_minsd))/((weffcj*nuints)*(weffcj*nuints)));
rint_weffcj=(-((rsh*dmcg)*(nuints+(nuints_weffcj*weffcj)))/((weffcj*nuints)*(weffcj*nuints)));
rint_rsh=((dmcg)*(weffcj*nuints)-(rsh*dmcg)*(weffcj*nuints_rsh))/((weffcj*nuints)*(weffcj*nuints));
rint_dmcg=((rsh)*(weffcj*nuints)-(rsh*dmcg)*(weffcj*nuints_dmcg))/((weffcj*nuints)*(weffcj*nuints));
rint_dmci=(-((rsh*dmcg)*(weffcj*nuints_dmci))/((weffcj*nuints)*(weffcj*nuints)));
rint_dmdg=(-((rsh*dmcg)*(weffcj*nuints_dmdg))/((weffcj*nuints)*(weffcj*nuints)));
rint_type=(-((rsh*dmcg)*(weffcj*nuints_type))/((weffcj*nuints)*(weffcj*nuints)));
rint=((rsh*dmcg)/(weffcj*nuints));
}
}else
{if((nuintd==0.0))
{rint_nf=0.0;
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=0.0;
rint_rsh=0.0;
rint_dmcg=0.0;
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=0.0;
}
else
{rint_nf=(-((rsh*dmcg)*(weffcj*nuintd_nf))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_geo=(-((rsh*dmcg)*(weffcj*nuintd_geo))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_rgeo=(-((rsh*dmcg)*(weffcj*nuintd_rgeo))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_minsd=(-((rsh*dmcg)*(weffcj*nuintd_minsd))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_weffcj=(-((rsh*dmcg)*(nuintd+(nuintd_weffcj*weffcj)))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_rsh=((dmcg)*(weffcj*nuintd)-(rsh*dmcg)*(weffcj*nuintd_rsh))/((weffcj*nuintd)*(weffcj*nuintd));
rint_dmcg=((rsh)*(weffcj*nuintd)-(rsh*dmcg)*(weffcj*nuintd_dmcg))/((weffcj*nuintd)*(weffcj*nuintd));
rint_dmci=(-((rsh*dmcg)*(weffcj*nuintd_dmci))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_dmdg=(-((rsh*dmcg)*(weffcj*nuintd_dmdg))/((weffcj*nuintd)*(weffcj*nuintd)));
rint_type=(-((rsh*dmcg)*(weffcj*nuintd_type))/((weffcj*nuintd)*(weffcj*nuintd)));
rint=((rsh*dmcg)/(weffcj*nuintd));
}
}}switch ((int)geo) {
  case 0: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 1: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 2: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 3: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 4: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=(-(rsh*dmdg)/(weffcj*weffcj));
rend_rsh=(dmdg)/weffcj;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=(rsh)/weffcj;
rend_type=0.0;
rend=((rsh*dmdg)/weffcj);
}
} break;
  case 5: 
{if((type==1))
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(1.0),(0.0),(0.0),(0.0),(0.0),(nuends_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(1.0),(0.0),(0.0),(0.0),(nuends_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(1.0),(0.0),(0.0),(nuends_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(1.0),(0.0),(nuends_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(1.0),(nuends_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1,(0.0),(0.0),(0.0),(0.0),(0.0),(nuends_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuends,rgeo,1);
}
else
{rend_nf=(-((rsh*dmdg)*(weffcj*nuendd_nf))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_geo=(-((rsh*dmdg)*(weffcj*nuendd_geo))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_rgeo=(-((rsh*dmdg)*(weffcj*nuendd_rgeo))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_minsd=(-((rsh*dmdg)*(weffcj*nuendd_minsd))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_weffcj=(-((rsh*dmdg)*(nuendd+(nuendd_weffcj*weffcj)))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_rsh=((dmdg)*(weffcj*nuendd)-(rsh*dmdg)*(weffcj*nuendd_rsh))/((weffcj*nuendd)*(weffcj*nuendd));
rend_dmcg=(-((rsh*dmdg)*(weffcj*nuendd_dmcg))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_dmci=(-((rsh*dmdg)*(weffcj*nuendd_dmci))/((weffcj*nuendd)*(weffcj*nuendd)));
rend_dmdg=((rsh)*(weffcj*nuendd)-(rsh*dmdg)*(weffcj*nuendd_dmdg))/((weffcj*nuendd)*(weffcj*nuendd));
rend_type=(-((rsh*dmdg)*(weffcj*nuendd_type))/((weffcj*nuendd)*(weffcj*nuendd)));
rend=((rsh*dmdg)/(weffcj*nuendd));
}
} break;
  case 6: 
{if((type==1))
{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=(-(rsh*dmdg)/(weffcj*weffcj));
rend_rsh=(dmdg)/weffcj;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=(rsh)/weffcj;
rend_type=0.0;
rend=((rsh*dmdg)/weffcj);
}
else
{rend_nf=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_rendi(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 7: 
{if((type==1))
{rend_nf=(-((rsh*dmdg)*(weffcj*nuends_nf))/((weffcj*nuends)*(weffcj*nuends)));
rend_geo=(-((rsh*dmdg)*(weffcj*nuends_geo))/((weffcj*nuends)*(weffcj*nuends)));
rend_rgeo=(-((rsh*dmdg)*(weffcj*nuends_rgeo))/((weffcj*nuends)*(weffcj*nuends)));
rend_minsd=(-((rsh*dmdg)*(weffcj*nuends_minsd))/((weffcj*nuends)*(weffcj*nuends)));
rend_weffcj=(-((rsh*dmdg)*(nuends+(nuends_weffcj*weffcj)))/((weffcj*nuends)*(weffcj*nuends)));
rend_rsh=((dmdg)*(weffcj*nuends)-(rsh*dmdg)*(weffcj*nuends_rsh))/((weffcj*nuends)*(weffcj*nuends));
rend_dmcg=(-((rsh*dmdg)*(weffcj*nuends_dmcg))/((weffcj*nuends)*(weffcj*nuends)));
rend_dmci=(-((rsh*dmdg)*(weffcj*nuends_dmci))/((weffcj*nuends)*(weffcj*nuends)));
rend_dmdg=((rsh)*(weffcj*nuends)-(rsh*dmdg)*(weffcj*nuends_dmdg))/((weffcj*nuends)*(weffcj*nuends));
rend_type=(-((rsh*dmdg)*(weffcj*nuends_type))/((weffcj*nuends)*(weffcj*nuends)));
rend=((rsh*dmdg)/(weffcj*nuends));
}
else
{rend_nf=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_nf),(0.0),(0.0));
rend_geo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_geo),(0.0),(0.0));
rend_rgeo=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_rgeo),(1.0),(0.0));
rend_minsd=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_minsd),(0.0),(0.0));
rend_weffcj=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(1.0),(0.0),(0.0),(0.0),(0.0),(nuendd_weffcj),(0.0),(0.0));
rend_rsh=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(1.0),(0.0),(0.0),(0.0),(nuendd_rsh),(0.0),(0.0));
rend_dmcg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(1.0),(0.0),(0.0),(nuendd_dmcg),(0.0),(0.0));
rend_dmci=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(1.0),(0.0),(nuendd_dmci),(0.0),(0.0));
rend_dmdg=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(1.0),(nuendd_dmdg),(0.0),(0.0));
rend_type=bsim4v30pMOS_d_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0,(0.0),(0.0),(0.0),(0.0),(0.0),(nuendd_type),(0.0),(0.0));
rend=bsim4v30pMOS_get_renda(weffcj,rsh,dmcg,dmci,dmdg,nuendd,rgeo,0);
}
} break;
  case 8: 
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=(-(rsh*dmdg)/(weffcj*weffcj));
rend_rsh=(dmdg)/weffcj;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=(rsh)/weffcj;
rend_type=0.0;
rend=((rsh*dmdg)/weffcj);
}
} break;
  case 9: 
{if((type==1))
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=(-((0.5*rsh)*dmcg)/(weffcj*weffcj));
rend_rsh=(0.5)*dmcg/weffcj;
rend_dmcg=((0.5*rsh))/weffcj;
rend_dmci=0.0;
rend_dmdg=0.0;
rend_type=0.0;
rend=(((0.5*rsh)*dmcg)/weffcj);
}
if((nf==2.0))
{rint_nf=0.0;
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=0.0;
rint_rsh=0.0;
rint_dmcg=0.0;
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=0.0;
}
else
{rint_nf=(-((rsh*dmcg)*(weffcj))/((weffcj*(nf-2.0))*(weffcj*(nf-2.0))));
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=(-((rsh*dmcg)*((nf-2.0)))/((weffcj*(nf-2.0))*(weffcj*(nf-2.0))));
rint_rsh=(dmcg)/(weffcj*(nf-2.0));
rint_dmcg=(rsh)/(weffcj*(nf-2.0));
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=((rsh*dmcg)/(weffcj*(nf-2.0)));
}
}else
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=0.0;
rend_rsh=0.0;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=0.0;
rend_type=0.0;
rend=0.0;
}
{rint_nf=(-((rsh*dmcg)*(weffcj))/((weffcj*nf)*(weffcj*nf)));
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=(-((rsh*dmcg)*(nf))/((weffcj*nf)*(weffcj*nf)));
rint_rsh=(dmcg)/(weffcj*nf);
rint_dmcg=(rsh)/(weffcj*nf);
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=((rsh*dmcg)/(weffcj*nf));
}
}} break;
  case 10: 
{if((type==1))
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=0.0;
rend_rsh=0.0;
rend_dmcg=0.0;
rend_dmci=0.0;
rend_dmdg=0.0;
rend_type=0.0;
rend=0.0;
}
{rint_nf=(-((rsh*dmcg)*(weffcj))/((weffcj*nf)*(weffcj*nf)));
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=(-((rsh*dmcg)*(nf))/((weffcj*nf)*(weffcj*nf)));
rint_rsh=(dmcg)/(weffcj*nf);
rint_dmcg=(rsh)/(weffcj*nf);
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=((rsh*dmcg)/(weffcj*nf));
}
}else
{{rend_nf=0.0;
rend_geo=0.0;
rend_rgeo=0.0;
rend_minsd=0.0;
rend_weffcj=(-((0.5*rsh)*dmcg)/(weffcj*weffcj));
rend_rsh=(0.5)*dmcg/weffcj;
rend_dmcg=((0.5*rsh))/weffcj;
rend_dmci=0.0;
rend_dmdg=0.0;
rend_type=0.0;
rend=(((0.5*rsh)*dmcg)/weffcj);
}
;if((nf==2.0))
{rint_nf=0.0;
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=0.0;
rint_rsh=0.0;
rint_dmcg=0.0;
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=0.0;
}
else
{rint_nf=(-((rsh*dmcg)*(weffcj))/((weffcj*(nf-2.0))*(weffcj*(nf-2.0))));
rint_geo=0.0;
rint_rgeo=0.0;
rint_minsd=0.0;
rint_weffcj=(-((rsh*dmcg)*((nf-2.0)))/((weffcj*(nf-2.0))*(weffcj*(nf-2.0))));
rint_rsh=(dmcg)/(weffcj*(nf-2.0));
rint_dmcg=(rsh)/(weffcj*(nf-2.0));
rint_dmci=0.0;
rint_dmdg=0.0;
rint_type=0.0;
rint=((rsh*dmcg)/(weffcj*(nf-2.0)));
}
}} break;
  default: 
fprintf(stdout,"Warning: specified geo = %d not matched",geo);   fprintf(stdout,"\n");
 break;
 }if((rint<=0.0))
{get_rtot_nf=rend_nf;
get_rtot_geo=rend_geo;
get_rtot_rgeo=rend_rgeo;
get_rtot_minsd=rend_minsd;
get_rtot_weffcj=rend_weffcj;
get_rtot_rsh=rend_rsh;
get_rtot_dmcg=rend_dmcg;
get_rtot_dmci=rend_dmci;
get_rtot_dmdg=rend_dmdg;
get_rtot_type=rend_type;
get_rtot=rend;
}
else
if((rend<=0.0))
{get_rtot_nf=rint_nf;
get_rtot_geo=rint_geo;
get_rtot_rgeo=rint_rgeo;
get_rtot_minsd=rint_minsd;
get_rtot_weffcj=rint_weffcj;
get_rtot_rsh=rint_rsh;
get_rtot_dmcg=rint_dmcg;
get_rtot_dmci=rint_dmci;
get_rtot_dmdg=rint_dmdg;
get_rtot_type=rint_type;
get_rtot=rint;
}
else
{get_rtot_nf=(((rint_nf*rend)+(rint*rend_nf))*(rint+rend)-(rint*rend)*(rint_nf+rend_nf))/((rint+rend)*(rint+rend));
get_rtot_geo=(((rint_geo*rend)+(rint*rend_geo))*(rint+rend)-(rint*rend)*(rint_geo+rend_geo))/((rint+rend)*(rint+rend));
get_rtot_rgeo=(((rint_rgeo*rend)+(rint*rend_rgeo))*(rint+rend)-(rint*rend)*(rint_rgeo+rend_rgeo))/((rint+rend)*(rint+rend));
get_rtot_minsd=(((rint_minsd*rend)+(rint*rend_minsd))*(rint+rend)-(rint*rend)*(rint_minsd+rend_minsd))/((rint+rend)*(rint+rend));
get_rtot_weffcj=(((rint_weffcj*rend)+(rint*rend_weffcj))*(rint+rend)-(rint*rend)*(rint_weffcj+rend_weffcj))/((rint+rend)*(rint+rend));
get_rtot_rsh=(((rint_rsh*rend)+(rint*rend_rsh))*(rint+rend)-(rint*rend)*(rint_rsh+rend_rsh))/((rint+rend)*(rint+rend));
get_rtot_dmcg=(((rint_dmcg*rend)+(rint*rend_dmcg))*(rint+rend)-(rint*rend)*(rint_dmcg+rend_dmcg))/((rint+rend)*(rint+rend));
get_rtot_dmci=(((rint_dmci*rend)+(rint*rend_dmci))*(rint+rend)-(rint*rend)*(rint_dmci+rend_dmci))/((rint+rend)*(rint+rend));
get_rtot_dmdg=(((rint_dmdg*rend)+(rint*rend_dmdg))*(rint+rend)-(rint*rend)*(rint_dmdg+rend_dmdg))/((rint+rend)*(rint+rend));
get_rtot_type=(((rint_type*rend)+(rint*rend_type))*(rint+rend)-(rint*rend)*(rint_type+rend_type))/((rint+rend)*(rint+rend));
get_rtot=((rint*rend)/(rint+rend));
}
if((get_rtot==0.0))
fprintf(stdout,"Warning: zero resistance returned from get_rtot");   fprintf(stdout,"\n");
}
return get_rtot_nf*d_nf+get_rtot_geo*d_geo+get_rtot_rgeo*d_rgeo+get_rtot_minsd*d_minsd+get_rtot_weffcj*d_weffcj+get_rtot_rsh*d_rsh+get_rtot_dmcg*d_dmcg+get_rtot_dmci*d_dmci+get_rtot_dmdg*d_dmdg+get_rtot_type*d_type;
}

/*
 * analog function: get_vjm
 */
double bsim4v30pMOS_get_vjm (double nvtm, double ijth, double isb, double xexpbv)
{
double get_vjm=0.0;
double tb=0.0;
double tc=0.0;
double evjmovnv=0.0;
{tc=xexpbv;
tb=((1.0+(ijth/isb))-tc);
evjmovnv=(0.5*(tb+_sqrt(((tb*tb)+(4.0*tc)))));
get_vjm=(nvtm*_logE(evjmovnv));
}
return get_vjm;
}
double bsim4v30pMOS_d_get_vjm (double nvtm, double ijth, double isb, double xexpbv, double d_nvtm, double d_ijth, double d_isb, double d_xexpbv)
{
double get_vjm=0.0;
double get_vjm_nvtm=0.0;
double get_vjm_ijth=0.0;
double get_vjm_isb=0.0;
double get_vjm_xexpbv=0.0;
double tb=0.0;
double tb_nvtm=0.0;
double tb_ijth=0.0;
double tb_isb=0.0;
double tb_xexpbv=0.0;
double tc=0.0;
double tc_nvtm=0.0;
double tc_ijth=0.0;
double tc_isb=0.0;
double tc_xexpbv=0.0;
double evjmovnv=0.0;
double evjmovnv_nvtm=0.0;
double evjmovnv_ijth=0.0;
double evjmovnv_isb=0.0;
double evjmovnv_xexpbv=0.0;
{{tc_nvtm=0.0;
tc_ijth=0.0;
tc_isb=0.0;
tc_xexpbv=1.0;
tc=xexpbv;
}
{tb_nvtm=(-tc_nvtm);
tb_ijth=((+(1/isb))-tc_ijth);
tb_isb=((+(-ijth/(isb*isb)))-tc_isb);
tb_xexpbv=(-tc_xexpbv);
tb=((1.0+(ijth/isb))-tc);
}
{evjmovnv_nvtm=(0.5*(tb_nvtm+_d0_sqrt(((tb*tb)+(4.0*tc)))*((((tb_nvtm*tb)+(tb*tb_nvtm))+(4.0*tc_nvtm)))));
evjmovnv_ijth=(0.5*(tb_ijth+_d0_sqrt(((tb*tb)+(4.0*tc)))*((((tb_ijth*tb)+(tb*tb_ijth))+(4.0*tc_ijth)))));
evjmovnv_isb=(0.5*(tb_isb+_d0_sqrt(((tb*tb)+(4.0*tc)))*((((tb_isb*tb)+(tb*tb_isb))+(4.0*tc_isb)))));
evjmovnv_xexpbv=(0.5*(tb_xexpbv+_d0_sqrt(((tb*tb)+(4.0*tc)))*((((tb_xexpbv*tb)+(tb*tb_xexpbv))+(4.0*tc_xexpbv)))));
evjmovnv=(0.5*(tb+_sqrt(((tb*tb)+(4.0*tc)))));
}
{get_vjm_nvtm=(_logE(evjmovnv)+(_d0_logE(evjmovnv)*(evjmovnv_nvtm)*nvtm));
get_vjm_ijth=(nvtm*_d0_logE(evjmovnv)*(evjmovnv_ijth));
get_vjm_isb=(nvtm*_d0_logE(evjmovnv)*(evjmovnv_isb));
get_vjm_xexpbv=(nvtm*_d0_logE(evjmovnv)*(evjmovnv_xexpbv));
get_vjm=(nvtm*_logE(evjmovnv));
}
}
return get_vjm_nvtm*d_nvtm+get_vjm_ijth*d_ijth+get_vjm_isb*d_isb+get_vjm_xexpbv*d_xexpbv;
}
