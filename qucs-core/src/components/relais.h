/*
 * relais.h - relais class definitions
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: relais.h,v 1.1 2006/03/02 08:06:03 raimi Exp $
 *
 */

#ifndef __RELAIS_H__
#define __RELAIS_H__

class relais : public circuit
{
 public:
  relais ();
  void initSP (void);
  void initDC (void);
  void calcDC (void);
  void initAC (void);
  void initTR (void);
  void calcTR (nr_double_t);

 private:
  int state;
};

#endif /* __RELAIS_H__ */
