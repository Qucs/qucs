/*
 * resistor.h - resistor class definitions
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
 * $Id: resistor.h,v 1.9 2004-09-12 14:09:19 ela Exp $
 *
 */

#ifndef __RESISTOR_H__
#define __RESISTOR_H__

class resistor : public circuit
{
 public:
  resistor ();
  void calcSP (nr_double_t);
  void calcNoise (nr_double_t);
  void calcDC (void);
  void initDC (void);
  void calcAC (nr_double_t);
  void initAC (void);
  void calcTR (nr_double_t);
  void initTR (void);
};

#endif /* __RESISTOR_H__ */
