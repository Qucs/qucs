/*
 * hic0_full.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __hic0_full_DEFS_H__
#define __hic0_full_DEFS_H__

// required properties
PROP_REQ [] = {
  { "is", PROP_REAL, { 1.0e-16, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mcf", PROP_REAL, { 1.00, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "mcr", PROP_REAL, { 1.00, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vef", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "iqf", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "iqr", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "iqfh", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "tfh", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "ibes", PROP_REAL, { 1e-18, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mbe", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ires", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mre", PROP_REAL, { 2.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ibcs", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "mbc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "cje0", PROP_REAL, { 1.0e-20, PROP_NO_STR }, PROP_RNGXX (0, 1.0e6) },
  { "vde", PROP_REAL, { 0.9, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "ze", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "aje", PROP_REAL, { 2.5, PROP_NO_STR }, PROP_RNGIX (1, 1.0e6) },
  { "t0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "dt0h", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "tbvl", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "tef0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "gte", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "thcs", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "ahc", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "tr", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "rci0", PROP_REAL, { 150, PROP_NO_STR }, PROP_RNGXX (0, 1.0e6) },
  { "vlim", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vpt", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "vces", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "cjci0", PROP_REAL, { 1.0e-20, PROP_NO_STR }, PROP_RNGXX (0, 1.0e6) },
  { "vdci", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zci", PROP_REAL, { 0.333, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vptci", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "cjcx0", PROP_REAL, { 1.0e-20, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "vdcx", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zcx", PROP_REAL, { 0.333, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vptcx", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "fbc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "rbi0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "vr0e", PROP_REAL, { 2.5, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "vr0c", PROP_REAL, { 1.0e6, PROP_NO_STR }, PROP_RNGXI (0, 1.0e6) },
  { "fgeo", PROP_REAL, { 0.656, PROP_NO_STR }, PROP_RNGII (0, 1.0e6) },
  { "rbx", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "rcx", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "re", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "itss", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "msf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "iscs", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGII (0, 1.0) },
  { "msc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "cjs0", PROP_REAL, { 1.0e-20, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "vds", PROP_REAL, { 0.3, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "zs", PROP_REAL, { 0.3, PROP_NO_STR }, PROP_RNGXI (0, 1) },
  { "vpts", PROP_REAL, { 100, PROP_NO_STR }, PROP_RNGXI (0, 100) },
  { "cbcpar", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "cbepar", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "eavl", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "kavl", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "kf", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "af", PROP_REAL, { 2.0, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vgb", PROP_REAL, { 1.2, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vge", PROP_REAL, { 1.17, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vgc", PROP_REAL, { 1.17, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "vgs", PROP_REAL, { 1.17, PROP_NO_STR }, PROP_RNGXI (0, 10) },
  { "f1vg", PROP_REAL, { -1.02377e-4, PROP_NO_STR }, PROP_NO_RANGE },
  { "f2vg", PROP_REAL, { 4.3215e-4, PROP_NO_STR }, PROP_NO_RANGE },
  { "alt0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "kt0", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetact", PROP_REAL, { 3.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetabet", PROP_REAL, { 3.5, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetaci", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "alvs", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "alces", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarbi", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarbx", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetarcx", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "zetare", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "alkav", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "aleav", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "flsh", PROP_INT, { 0, PROP_NO_STR }, PROP_RNGII (0, 2) },
  { "rth", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "cth", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_RNGIX (0, 1.0e6) },
  { "tnom", PROP_REAL, { 27, PROP_NO_STR }, PROP_NO_RANGE },
  { "dt", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "npn", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "pnp", PROP_INT, { 0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "Temp", PROP_REAL, { 27, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t hic0_full::cirdef =
  { "hic0_full",
    5, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __hic0_full_DEFS_H__ */
