/*
 * MESFET.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __MESFET_DEFS_H__
#define __MESFET_DEFS_H__

// required properties
PROP_REQ [] = {
  { "LEVEL", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (1, 5) },
  { "Vto", PROP_REAL, { -1.8, PROP_NO_STR }, PROP_NO_RANGE },
  { "Beta", PROP_REAL, { 3e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Alpha", PROP_REAL, { 2.25, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Lambda", PROP_REAL, { 0.05, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "B", PROP_REAL, { 0.3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Qp", PROP_REAL, { 2.1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Delta", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Vmax", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Vdelta1", PROP_REAL, { 0.3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Vdelta2", PROP_REAL, { 0.2, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Gamma", PROP_REAL, { 0.015, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Nsc", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Is", PROP_REAL, { 1e-14, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "N", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Vbi", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Bv", PROP_REAL, { 60, PROP_NO_STR }, PROP_NO_RANGE },
  { "Xti", PROP_REAL, { 3.0, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Fc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Tau", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Rin", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Area", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "Eg", PROP_REAL, { 1.11, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "M", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Cgd", PROP_REAL, { 0.2e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Cgs", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Cds", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Betatc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Alphatc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Gammatc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ng", PROP_REAL, { 2.65, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "Nd", PROP_REAL, { -0.19, PROP_NO_STR }, PROP_NO_RANGE },
  { "ILEVELS", PROP_INT, { 3, PROP_NO_STR }, PROP_RNGII (0, 4) },
  { "ILEVELD", PROP_INT, { 3, PROP_NO_STR }, PROP_RNGII (0, 4) },
  { "QLEVELS", PROP_INT, { 2, PROP_NO_STR }, PROP_RNGII (0, 3) },
  { "QLEVELD", PROP_INT, { 2, PROP_NO_STR }, PROP_RNGII (0, 3) },
  { "QLEVELDS", PROP_INT, { 2, PROP_NO_STR }, PROP_RNGII (0, 3) },
  { "Vtotc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Rg", PROP_REAL, { 5.1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Rd", PROP_REAL, { 1.3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Rs", PROP_REAL, { 1.3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Rgtc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Rdtc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Rstc", PROP_REAL, { 0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ibv", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-25) },
  { "Rf", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "R1", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Af", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Kf", PROP_REAL, { 0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Gdsnoi", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (-273) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t MESFET::cirdef =
  { "MESFET",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __MESFET_DEFS_H__ */
