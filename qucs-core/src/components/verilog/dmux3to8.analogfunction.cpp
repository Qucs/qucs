/*
 * dmux3to8.analogfunction.cpp - analog function implementations
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#include <stdio.h>
#include "dmux3to8.analogfunction.h"
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

