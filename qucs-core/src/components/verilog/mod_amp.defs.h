/*
 * mod_amp.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __mod_amp_DEFS_H__
#define __mod_amp_DEFS_H__

// required properties
PROP_REQ [] = {
  { "GBP", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "AOLDC", PROP_REAL, { 106.0, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "FP2", PROP_REAL, { 3e6, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "RO", PROP_REAL, { 75, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "CD", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "RD", PROP_REAL, { 2e6, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "IOFF", PROP_REAL, { 20e-9, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "IB", PROP_REAL, { 80e-9, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "VOFF", PROP_REAL, { 7e-4, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "CMRRDC", PROP_REAL, { 90.0, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "FCM", PROP_REAL, { 200, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "PSRT", PROP_REAL, { 5e5, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "NSRT", PROP_REAL, { 5e5, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "VLIMP", PROP_REAL, { 14, PROP_NO_STR }, PROP_MIN_VAL (0.01) },
  { "VLIMN", PROP_REAL, { -14, PROP_NO_STR }, PROP_MAX_VAL (0) },
  { "ILMAX", PROP_REAL, { 35e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "CSCALE", PROP_REAL, { 50, PROP_NO_STR }, PROP_MIN_VAL (0) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  PROP_NO_PROP };

// device definition
struct define_t mod_amp::cirdef =
  { "mod_amp",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __mod_amp_DEFS_H__ */
