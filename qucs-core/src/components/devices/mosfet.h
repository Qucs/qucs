/*
 * mosfet.h - mosfet class definitions
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
 * $Id: mosfet.h,v 1.8 2004/10/17 09:44:30 ela Exp $
 *
 */

#ifndef __MOSFET_H__
#define __MOSFET_H__

class mosfet : public circuit
{
 public:
  mosfet ();
  void calcSP (nr_double_t);
  void calcNoise (nr_double_t);
  void calcDC (void);
  void initDC (void);
  void initModel (void);
  void calcOperatingPoints (void);
  void initAC (void);
  void calcAC (nr_double_t);
  matrix calcMatrixY (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);

 private:
  nr_double_t UbsPrev, UbdPrev, UgsPrev, UgdPrev, UdsPrev, UgbPrev, Udsat, Uon;
  nr_double_t gbs, gbd, gm, gds, gmb, Ids, DrainControl, SourceControl;
  nr_double_t Leff, MOSdir, beta, Cox, Phi, Ga, Vto, Rs, Rd;
  nr_double_t Qgd, Qgs, Qbd, Qbs, Qgb, Ibs, Ibd;
  circuit * rs;
  circuit * rd;
  circuit * rg;
};

#endif /* __MOSFET_H__ */
