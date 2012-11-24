/*
 * photodiode.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __photodiode_DEFS_H__
#define __photodiode_DEFS_H__

// required properties
PROP_REQ [] = {
  { "N", PROP_REAL, { 1.35, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Rseries", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Is", PROP_REAL, { 0.34e-12, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Bv", PROP_REAL, { 60, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Ibv", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Vj", PROP_REAL, { 0.7, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Cj0", PROP_REAL, { 60e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "M", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Area", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (1.0) },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (-273) },
  { "Fc", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Tt", PROP_REAL, { 10e-9, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Xti", PROP_REAL, { 3.0, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Eg", PROP_REAL, { 1.16, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Responsivity", PROP_REAL, { 0.5, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Rsh", PROP_REAL, { 5e8, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "QEpercent", PROP_REAL, { 80, PROP_NO_STR }, PROP_RNGII (0, 100) },
  { "Lambda", PROP_REAL, { 900, PROP_NO_STR }, PROP_RNGII (100, 2000) },
  { "LEVEL", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (1, 2) },
  { "Kf", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Af", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Ffe", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t photodiode::cirdef =
  { "photodiode",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __photodiode_DEFS_H__ */
