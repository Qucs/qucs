/*
 * hicumL2V2p1.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __hicumL2V2p1_DEFS_H__
#define __hicumL2V2p1_DEFS_H__

// required properties
PROP_REQ [] = {
  { "c10", PROP_REAL, { 1.516E-31, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "qp0", PROP_REAL, { 5.939E-15, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "ich", PROP_REAL, { 1.0E11, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "hfe", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "hfc", PROP_REAL, { 0.03999, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "hjei", PROP_REAL, { 0.435, PROP_NO_STR }, PROP_RNGII (0, 100) },
  { "hjci", PROP_REAL, { 0.09477, PROP_NO_STR }, PROP_RNGII (0, 100) },
  { "ibeis", PROP_REAL, { 3.47E-20, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mbei", PROP_REAL, { 1.025, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ireis", PROP_REAL, { 390E-12, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mrei", PROP_REAL, { 3, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ibeps", PROP_REAL, { 4.18321E-21, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mbep", PROP_REAL, { 1.045, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ireps", PROP_REAL, { 1.02846E-14, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mrep", PROP_REAL, { 3, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "mcf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ibcis", PROP_REAL, { 3.02613E-18, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "mbci", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ibcxs", PROP_REAL, { 4.576E-29, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "mbcx", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ibets", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "abet", PROP_REAL, { 36.74, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "favl", PROP_REAL, { 14.97, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "qavl", PROP_REAL, { 7.2407E-14, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "alfav", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "alqav", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "rbi0", PROP_REAL, { 7.9, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "rbx", PROP_REAL, { 13.15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "fgeo", PROP_REAL, { 0.724, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "fdqr0", PROP_REAL, { 200, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "fcrbi", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "fqi", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "re", PROP_REAL, { 9.77, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "rcx", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "itss", PROP_REAL, { 2.81242E-19, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "msf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "iscs", PROP_REAL, { 7.6376E-17, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "msc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "tsf", PROP_REAL, { 1.733E-8, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "rsu", PROP_REAL, { 800, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "csu", PROP_REAL, { 1.778E-14, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "cjei0", PROP_REAL, { 5.24382E-14, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vdei", PROP_REAL, { 0.9956, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zei", PROP_REAL, { 0.4, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "aljei", PROP_REAL, { 2.5, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "cjep0", PROP_REAL, { 0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vdep", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zep", PROP_REAL, { 0.01, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "aljep", PROP_REAL, { 2.5, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "cjci0", PROP_REAL, { 4.46887E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vdci", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zci", PROP_REAL, { 0.38, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vptci", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "cjcx0", PROP_REAL, { 1.55709E-14, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vdcx", PROP_REAL, { 0.733, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zcx", PROP_REAL, { 0.34, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vptcx", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "fbc", PROP_REAL, { 0.3487, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "cjs0", PROP_REAL, { 17.68E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vds", PROP_REAL, { 0.621625, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zs", PROP_REAL, { 0.122136, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vpts", PROP_REAL, { 1000, PROP_NO_STR }, PROP_RNGXI (0, 1000) },
  { "t0", PROP_REAL, { 1.28E-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "dt0h", PROP_REAL, { 260E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "tbvl", PROP_REAL, { 2.0E-13, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "tef0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "gtfe", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "thcs", PROP_REAL, { 46E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "alhc", PROP_REAL, { 0.08913, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "fthc", PROP_REAL, { 0.8778, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "rci0", PROP_REAL, { 50.4277, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "vlim", PROP_REAL, { 0.9, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vces", PROP_REAL, { 0.01, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "vpt", PROP_REAL, { 10, PROP_NO_STR }, PROP_RNGII (0, 100) },
  { "tr", PROP_REAL, { 1.0E-11, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "ceox", PROP_REAL, { 1.71992E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "ccox", PROP_REAL, { 4.9E-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "alqf", PROP_REAL, { 0.1288, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "alit", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "kf", PROP_REAL, { 2.83667E-9, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "af", PROP_REAL, { 2.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "krbi", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "latb", PROP_REAL, { 10.479, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "latl", PROP_REAL, { 0.300012, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "vgb", PROP_REAL, { 1.112, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "alt0", PROP_REAL, { 0.0017580, PROP_NO_STR }, PROP_NO_RANGE },
  { "kt0", PROP_REAL, { 4.07E-6, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetaci", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetacx", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "alvs", PROP_REAL, { 0.001, PROP_NO_STR }, PROP_NO_RANGE },
  { "alces", PROP_REAL, { 0.000125, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarbi", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarbx", PROP_REAL, { 0.2, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarcx", PROP_REAL, { 0.21, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetare", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_NO_RANGE },
  { "alb", PROP_REAL, { 0.007, PROP_NO_STR }, PROP_NO_RANGE },
  { "rth", PROP_REAL, { 1293.95, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "cth", PROP_REAL, { 7.22203E-11, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "tnom", PROP_REAL, { 27.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "dt", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 27.0, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t hicumL2V2p1::cirdef =
  { "hicumL2V2p1",
    5, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __hicumL2V2p1_DEFS_H__ */
