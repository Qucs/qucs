/*
 * device.h - device class definitions
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: device.h,v 1.13 2005/12/19 07:55:14 raimi Exp $
 *
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

class circuit;
class node;
class net;

class device
{
 public:
  friend circuit * splitResistance (circuit *, circuit *, net *,
				    char *, char *, int);
  friend void disableResistance (circuit *, circuit *, net *, int);
  friend circuit * splitCapacitance (circuit *, circuit *, net *,
				     char *, node *, node *);
  friend void disableCapacitance (circuit *, circuit *, net *);
  friend int deviceEnabled (circuit *);
  friend void pnJunctionMOS (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  friend void pnJunctionBIP (nr_double_t, nr_double_t, nr_double_t,
			     nr_double_t&, nr_double_t&);
  friend nr_double_t pnVoltage (nr_double_t, nr_double_t, nr_double_t,
				nr_double_t);
  friend nr_double_t pnCurrent (nr_double_t, nr_double_t, nr_double_t);
  friend nr_double_t pnConductance (nr_double_t, nr_double_t, nr_double_t);
  friend nr_double_t pnCapacitance (nr_double_t, nr_double_t, nr_double_t,
				    nr_double_t, nr_double_t);
  friend nr_double_t pnCharge (nr_double_t, nr_double_t, nr_double_t,
			       nr_double_t, nr_double_t);
  friend nr_double_t pnCapacitance (nr_double_t, nr_double_t, nr_double_t,
				    nr_double_t);
  friend nr_double_t pnCharge (nr_double_t, nr_double_t, nr_double_t,
			       nr_double_t);
  friend nr_double_t pnCriticalVoltage (nr_double_t, nr_double_t);
  friend nr_double_t fetVoltage (nr_double_t, nr_double_t, nr_double_t);
  friend nr_double_t fetVoltageDS (nr_double_t, nr_double_t);
  friend void fetCapacitanceMeyer (nr_double_t, nr_double_t, nr_double_t,
				   nr_double_t, nr_double_t, nr_double_t,
				   nr_double_t&, nr_double_t&, nr_double_t&);
  friend nr_double_t Egap (nr_double_t, nr_double_t Eg0 = 1.16);
  friend nr_double_t intrinsicDensity (nr_double_t, nr_double_t Eg0 = 1.16);
  friend nr_double_t pnCurrent_T (nr_double_t, nr_double_t, nr_double_t,
				  nr_double_t, nr_double_t N = 1,
				  nr_double_t Xti = 0);
  friend nr_double_t pnPotential_T (nr_double_t, nr_double_t, nr_double_t,
				    nr_double_t Eg0 = 1.16);
  friend nr_double_t pnCapacitance_T (nr_double_t, nr_double_t, nr_double_t,
				      nr_double_t, nr_double_t);
  friend nr_double_t pnCapacitance_F (nr_double_t, nr_double_t, nr_double_t,
				      nr_double_t);
};

#endif /* __DEVICE_H__ */
