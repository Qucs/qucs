/*
 * component_id.h - global component identifier header file
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
 *
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this package; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: component_id.h,v 1.8 2004/07/07 13:52:34 ela Exp $
 *
 */

#ifndef __COMPONENT_ID_H__
#define __COMPONENT_ID_H__

/* Enumerate component type identifiers. */
enum circuit_type {
  CIR_UNKNOWN = -1,

  // linear components
  CIR_GROUND,
  CIR_OPEN,
  CIR_TEE,
  CIR_CROSS,
  CIR_ITRAFO,
  CIR_RESISTOR,
  CIR_CAPACITOR,
  CIR_INDUCTOR,
  CIR_VCCS,
  CIR_CCCS,
  CIR_CCVS,
  CIR_VCVS,
  CIR_DCBLOCK,
  CIR_DCFEED,
  CIR_BIASTEE,
  CIR_PAC,
  CIR_ATTENUATOR,
  CIR_CIRCULATOR,
  CIR_ISOLATOR,
  CIR_TRAFO,
  CIR_STRAFO,
  CIR_VDC,
  CIR_IDC,
  CIR_VAC,
  CIR_IAC,
  CIR_PHASESHIFTER,
  CIR_GYRATOR,
  CIR_TLINE,
  CIR_IPROBE,

  // microstrip components
  CIR_MSLINE,

  // non-linear components
  CIR_DIODE,
  CIR_JFET,
  CIR_BJT,
};

#endif /* __COMPONENT_ID_H__ */
