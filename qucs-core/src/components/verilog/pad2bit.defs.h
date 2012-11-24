/*
 * pad2bit.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __pad2bit_DEFS_H__
#define __pad2bit_DEFS_H__

// required properties
PROP_REQ [] = {
  { "Number", PROP_INT, { 0, PROP_NO_STR }, PROP_RNGII (0, 15) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t pad2bit::cirdef =
  { "pad2bit",
    2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __pad2bit_DEFS_H__ */
