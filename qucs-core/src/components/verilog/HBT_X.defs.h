/*
 * HBT_X.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __HBT_X_DEFS_H__
#define __HBT_X_DEFS_H__

// required properties
PROP_REQ [] = {
  { "Mode", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (0, 4) },
  { "Noise", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (0, 4) },
  { "Debug", PROP_INT, { 0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "DebugPlus", PROP_INT, { 0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Temp", PROP_REAL, { 25.0, PROP_NO_STR }, PROP_MIN_VAL (-273.15) },
  { "Rth", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cth", PROP_REAL, { (700*1.0e-9), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "N", PROP_INT, { 1, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "L", PROP_REAL, { (30*1.0e-6), PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "W", PROP_REAL, { (3*1.0e-6), PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Jsf", PROP_REAL, { 20e-24, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "nf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Vg", PROP_REAL, { 1.3, PROP_NO_STR }, PROP_MIN_VAL (-2.0) },
  { "Jse", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "ne", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rbxx", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Vgb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Jsee", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "nee", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rbbxx", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Vgbb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Jsr", PROP_REAL, { 20e-18, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "nr", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Vgr", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "XCjc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGIX (0.0, 1.0) },
  { "Jsc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "nc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rcxx", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Vgc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Bf", PROP_REAL, { 100.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "kBeta", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Br", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "VAF", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "VAR", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "IKF", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "IKR", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Mc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "BVceo", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "kc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "BVebo", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Tr", PROP_REAL, { (1*1.0e-15), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Trx", PROP_REAL, { (1*1.0e-15), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Tf", PROP_REAL, { (1*1.0e-12), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Tft", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Thcs", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Ahc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cje", PROP_REAL, { (1*1.0e-15), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "mje", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_RNGIX (0.0, 1) },
  { "Vje", PROP_REAL, { 1.3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cjc", PROP_REAL, { (1*1.0e-15), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "mjc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Vjc", PROP_REAL, { 1.3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "kjc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Cmin", PROP_REAL, { (0.1*1.0e-15), PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "J0", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "XJ0", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_RNGII (0.0, 1.0) },
  { "Rci0", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Jk", PROP_REAL, { 4e-4, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "RJk", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Vces", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rc", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Re", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Rb", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Rb2", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Lc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Le", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Lb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cq", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cpb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cpc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Kfb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Afb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Ffeb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Kb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Ab", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Fb", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VALX (0.0) },
  { "Kfe", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Afe", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Ffee", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Tnom", PROP_REAL, { 20.0, PROP_NO_STR }, PROP_MIN_VAL (-273.15) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  PROP_NO_PROP };

// device definition
struct define_t HBT_X::cirdef =
  { "HBT_X",
    4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __HBT_X_DEFS_H__ */
