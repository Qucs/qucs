/*
 * diode.h - diode class definitions
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
 * $Id: diode.h,v 1.2 2004/07/26 06:30:29 ela Exp $
 *
 */

#ifndef __DIODE_H__
#define __DIODE_H__

class net;

class diode : public circuit
{
 public:
  diode ();
  void calcSP (nr_double_t);
  void calcNoise (nr_double_t);
  void calcDC (void);
  void initDC (dcsolver *);
  void calcOperatingPoints (void);

 private:
  nr_double_t Uprev;
  circuit * rs;
};

#endif /* __DIODE_H__ */
