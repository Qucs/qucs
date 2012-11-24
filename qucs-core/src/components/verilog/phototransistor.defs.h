/*
 * phototransistor.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __phototransistor_DEFS_H__
#define __phototransistor_DEFS_H__

// required properties
PROP_REQ [] = {
  { "Bf", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (1) },
  { "Br", PROP_REAL, { 0.1, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Is", PROP_REAL, { 1e-10, PROP_NO_STR }, PROP_MIN_VAL (1e-30) },
  { "Nf", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-30) },
  { "Nr", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-30) },
  { "Vaf", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Var", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Mje", PROP_REAL, { 0.33, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Vje", PROP_REAL, { 0.75, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Cje", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Mjc", PROP_REAL, { 0.33, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Vjc", PROP_REAL, { 0.75, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Cjc", PROP_REAL, { 2e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Tr", PROP_REAL, { (100*1.0e-9), PROP_NO_STR }, PROP_MIN_VAL (1e-15) },
  { "Tf", PROP_REAL, { (0.1*1.0e-9), PROP_NO_STR }, PROP_MIN_VAL (1e-15) },
  { "Ikf", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Ikr", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1e-20) },
  { "Rc", PROP_REAL, { 10, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Re", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Rb", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (1e-9) },
  { "Kf", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Ffe", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Af", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Responsivity", PROP_REAL, { 1.5, PROP_NO_STR }, PROP_MIN_VAL (1e-30) },
  { "P0", PROP_REAL, { 2.6122e3, PROP_NO_STR }, PROP_NO_RANGE },
  { "P1", PROP_REAL, { -1.489e1, PROP_NO_STR }, PROP_NO_RANGE },
  { "P2", PROP_REAL, { 3.0332e-2, PROP_NO_STR }, PROP_NO_RANGE },
  { "P3", PROP_REAL, { -2.5708e-5, PROP_NO_STR }, PROP_NO_RANGE },
  { "P4", PROP_REAL, { 7.6923e-9, PROP_NO_STR }, PROP_NO_RANGE },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t phototransistor::cirdef =
  { "phototransistor",
    5, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __phototransistor_DEFS_H__ */
