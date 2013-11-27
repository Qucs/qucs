/*
 * nigbt.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 */

#ifndef __nigbt_DEFS_H__
#define __nigbt_DEFS_H__


// required properties
PROP_REQ [] = {
  { "Agd", PROP_REAL, { 5.0e-6, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "Area", PROP_REAL, { 1.0e-5, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "Kp", PROP_REAL, { 0.38, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Tau", PROP_REAL, { 7.1e-6, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Wb", PROP_REAL, { 9.0e-5, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "BVf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "BVn", PROP_REAL, { 4.0, PROP_NO_STR }, PROP_MIN_VALX (0) },
  { "Cgs", PROP_REAL, { 1.24e-8, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Coxd", PROP_REAL, { 3.5e-8, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Jsne", PROP_REAL, { 6.5e-13, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Kf", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Mun", PROP_REAL, { 1.5e-3, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Mup", PROP_REAL, { 4.5e-2, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Nb", PROP_REAL, { 2.0e14, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Theta", PROP_REAL, { 0.02, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Vt", PROP_REAL, { 4.7, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Vtd", PROP_REAL, { 1.0e-3, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (-273.15) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t nigbt::cirdef =
  { "nigbt",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __nigbt_DEFS_H__ */
