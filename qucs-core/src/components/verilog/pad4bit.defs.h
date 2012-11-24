/*
 * pad4bit.defs.h - device definition header
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 */

#ifndef __pad4bit_DEFS_H__
#define __pad4bit_DEFS_H__

// required properties
PROP_REQ [] = {
  { "Number", PROP_INT, { 0, PROP_NO_STR }, PROP_RNGII (0, 15) },
  PROP_NO_PROP };

// optional properties
PROP_OPT [] = {
  { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, PROP_MIN_VAL (K) },
  PROP_NO_PROP };

// device definition
struct define_t pad4bit::cirdef =
  { "pad4bit",
    4, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_NONLINEAR, PROP_DEF };

#endif /* __pad4bit_DEFS_H__ */
