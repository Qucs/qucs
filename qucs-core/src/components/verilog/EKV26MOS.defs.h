/*
 * EKV26MOS.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __EKV26MOS_DEFS_H__
#define __EKV26MOS_DEFS_H__

// required properties
PROP_REQ [] = {
  { "LEVEL", PROP_REAL, { 1, PROP_NO_STR }, PROP_RNGII (1, 2) },
  { "L", PROP_REAL, { 0.5e-6, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "W", PROP_REAL, { 10e-6, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Np", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1.0) },
  { "Ns", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1.0) },
  { "Cox", PROP_REAL, { 3.45e-3, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Xj", PROP_REAL, { 0.15e-6, PROP_NO_STR }, PROP_RNGII (0.01e-6, 1.0e-6) },
  { "Dw", PROP_REAL, { -0.02e-6, PROP_NO_STR }, PROP_MAX_VAL (0.0) },
  { "Dl", PROP_REAL, { -0.05e-6, PROP_NO_STR }, PROP_MAX_VAL (0.0) },
  { "Vto", PROP_REAL, { 0.6, PROP_NO_STR }, PROP_NO_RANGE },
  { "Gamma", PROP_REAL, { 0.71, PROP_NO_STR }, PROP_RNGII (0.0, 2.0) },
  { "Phi", PROP_REAL, { 0.97, PROP_NO_STR }, PROP_RNGII (0.3, 2.0) },
  { "Kp", PROP_REAL, { 150e-6, PROP_NO_STR }, PROP_MIN_VAL (10e-6) },
  { "Theta", PROP_REAL, { 50e-3, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "EO", PROP_REAL, { 88.0e6, PROP_NO_STR }, PROP_MIN_VAL (1.0e6) },
  { "Ucrit", PROP_REAL, { 4.5e6, PROP_NO_STR }, PROP_RNGII (2.0e6, 25.0e6) },
  { "Lambda", PROP_REAL, { 0.23, PROP_NO_STR }, PROP_MIN_VAL (0.1) },
  { "Weta", PROP_REAL, { 0.05, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Leta", PROP_REAL, { 0.28, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Q0", PROP_REAL, { 280e-6, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Lk", PROP_REAL, { 0.5e-6, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Tcv", PROP_REAL, { 1.5e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "Bex", PROP_REAL, { -1.5, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ucex", PROP_REAL, { 1.7, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ibbt", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Hdif", PROP_REAL, { 0.9e-6, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rsh", PROP_REAL, { 510.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rsc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Rdc", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cgso", PROP_REAL, { 1.5e-10, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cgdo", PROP_REAL, { 1.5e-10, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Cgbo", PROP_REAL, { 4.0e-10, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Iba", PROP_REAL, { 2e8, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Ibb", PROP_REAL, { 3.5e8, PROP_NO_STR }, PROP_MIN_VAL (1.0e8) },
  { "Ibn", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0.1) },
  { "Kf", PROP_REAL, { 1.0e-27, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Af", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Avto", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Akp", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "Agamma", PROP_REAL, { 0.0, PROP_NO_STR }, PROP_MIN_VAL (0.0) },
  { "N", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Is", PROP_REAL, { 1e-14, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Bv", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Ibv", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Vj", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Cj0", PROP_REAL, { 300e-15, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "M", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Area", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1e-3) },
  { "Fc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Tt", PROP_REAL, { 0.1e-9, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Xti", PROP_REAL, { 3.0, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Xpart", PROP_REAL, { 0.4, PROP_NO_STR }, PROP_RNGII (0.0, 1.0) },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "nmos", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "pmos", PROP_INT, { 0, PROP_NO_STR }, PROP_RNGII (0, 1) },
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t EKV26MOS::cirdef =
  { "EKV26MOS",
    4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __EKV26MOS_DEFS_H__ */
