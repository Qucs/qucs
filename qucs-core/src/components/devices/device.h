/*
 * device.h - device class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: device.h,v 1.1 2004-06-19 07:34:55 ela Exp $
 *
 */

#ifndef __DEVICE_H__
#define __DEVICE_H__

class circuit;
class net;

class device
{
 public:
  static nr_double_t pnVoltage (nr_double_t, nr_double_t,
				nr_double_t, nr_double_t);
  static circuit * splitResistance (circuit *, circuit *, net *,
				    char *, char *, int);
  static void disableResistance (circuit *, circuit *, net *, int);
  static void applyResistance (circuit *, nr_double_t);
};

#endif /* __DEVICE_H__ */
