/*
 * log_amp.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __log_amp_DEFS_H__
#define __log_amp_DEFS_H__

// required properties
PROP_REQ [] = {
  { "Kv", PROP_REAL, { 1.0, PROP_NO_STR }, PROP_NO_RANGE },
  { "Dk", PROP_REAL, { 0.3, PROP_NO_STR }, PROP_RNGII (-100, 100) },
  { "Ib1", PROP_REAL, { 5e-12, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ibr", PROP_REAL, { 5e-12, PROP_NO_STR }, PROP_NO_RANGE },
  { "M", PROP_REAL, { 5, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "N", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_RNGII (0, 100) },
  { "Vosout", PROP_REAL, { 3e-3, PROP_NO_STR }, PROP_NO_RANGE },
  { "Rinp", PROP_REAL, { 1e6, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "Fc", PROP_REAL, { 1e3, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "Ro", PROP_REAL, { 1e-3, PROP_NO_STR }, PROP_MIN_VAL (1e-3) },
  { "Ntc", PROP_REAL, { 0.002, PROP_NO_STR }, PROP_RNGII (-100, 100) },
  { "Vosouttc", PROP_REAL, { 80e-6, PROP_NO_STR }, PROP_NO_RANGE },
  { "Dktc", PROP_REAL, { 0.03, PROP_NO_STR }, PROP_RNGII (-100, 100) },
  { "Ib1tc", PROP_REAL, { 0.5e-12, PROP_NO_STR }, PROP_NO_RANGE },
  { "Ibrtc", PROP_REAL, { 0.5e-12, PROP_NO_STR }, PROP_NO_RANGE },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (-273) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t log_amp::cirdef =
  { "log_amp",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __log_amp_DEFS_H__ */
