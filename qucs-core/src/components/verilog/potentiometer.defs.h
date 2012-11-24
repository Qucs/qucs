/*
 * potentiometer.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __potentiometer_DEFS_H__
#define __potentiometer_DEFS_H__

// required properties
PROP_REQ [] = {
  { "R_pot", PROP_REAL, { 1e4, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Rotation", PROP_REAL, { 120, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Taper_Coeff", PROP_REAL, { 0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "LEVEL", PROP_INT, { 1, PROP_NO_STR }, PROP_RNGII (1, 3) },
  { "Max_Rotation", PROP_REAL, { 240.0, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Conformity", PROP_REAL, { 0.2, PROP_NO_STR }, PROP_NO_RANGE },
  { "Linearity", PROP_REAL, { 0.2, PROP_NO_STR }, PROP_NO_RANGE },
  { "Contact_Res", PROP_REAL, { 1, PROP_NO_STR }, PROP_MIN_VAL (1e-6) },
  { "Temp_Coeff", PROP_REAL, { 100, PROP_NO_STR }, PROP_MIN_VAL (0) },
  { "Tnom", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (-273) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t potentiometer::cirdef =
  { "potentiometer",
    3, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __potentiometer_DEFS_H__ */
