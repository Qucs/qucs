/*
 * msvia.h - microstrip via hole class definitions
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 * $Id: msvia.h,v 1.3 2005/02/03 20:40:20 raimi Exp $
 *
 */

#ifndef __MSVIA_H__
#define __MSVIA_H__

class msvia : public circuit
{
 public:
  msvia ();
  void calcSP (nr_double_t);
  void initSP (void);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  nr_double_t calcResistance (void);
  complex calcImpedance (nr_double_t);

 private:
  nr_double_t R;
  complex Z;
};

#endif /* __MSVIA_H__ */
