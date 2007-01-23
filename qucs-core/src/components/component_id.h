/*
 * component_id.h - global component identifier header file
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: component_id.h,v 1.37 2007-01-23 10:23:35 raimi Exp $
 *
 */

#ifndef __COMPONENT_ID_H__
#define __COMPONENT_ID_H__

/* Enumerate component type identifiers. */
enum circuit_type {
  CIR_UNKNOWN = -1,

  // linear helper components
  CIR_GROUND,
  CIR_OPEN,
  CIR_SHORT,
  CIR_TEE,
  CIR_CROSS,
  CIR_ITRAFO,

  // linear components
  CIR_RESISTOR,
  CIR_CAPACITOR,
  CIR_INDUCTOR,
  CIR_MUTUAL,
  CIR_MUTUAL2,
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
  CIR_VAM,
  CIR_VPM,
  CIR_PHASESHIFTER,
  CIR_GYRATOR,
  CIR_TSWITCH,
  CIR_RELAIS,
  CIR_TLINE,
  CIR_COAXLINE,
  CIR_TWISTEDPAIR,
  CIR_TLINE4P,
  CIR_IPROBE,
  CIR_VPROBE,
  CIR_SPFILE,
  CIR_VPULSE,
  CIR_IPULSE,
  CIR_VRECT,
  CIR_IRECT,
  CIR_AMPLIFIER,
  CIR_COUPLER,

  // noise sources
  CIR_VNOISE,
  CIR_INOISE,
  CIR_IINOISE,
  CIR_IVNOISE,
  CIR_VVNOISE,

  // microstrip components
  CIR_MSLINE,
  CIR_MSCORNER,
  CIR_MSMBEND,
  CIR_MSSTEP,
  CIR_MSOPEN,
  CIR_MSGAP,
  CIR_MSCOUPLED,
  CIR_MSTEE,
  CIR_MSCROSS,
  CIR_MSVIA,
  CIR_BONDWIRE,

  // coplanar components
  CIR_CPWLINE,
  CIR_CPWOPEN,
  CIR_CPWSHORT,
  CIR_CPWGAP,
  CIR_CPWSTEP,

  // non-linear components
  CIR_OPAMP,
  CIR_DIODE,
  CIR_JFET,
  CIR_BJT,
  CIR_MOSFET,

  // digital components
  CIR_INVERTER,
  CIR_NOR,
  CIR_OR,
  CIR_NAND,
  CIR_AND,
  CIR_XNOR,
  CIR_XOR,
  CIR_DIGISOURCE,

  // verilog devices
  CIR_HBT_X,
  CIR_hicumL2p1
};

#endif /* __COMPONENT_ID_H__ */
