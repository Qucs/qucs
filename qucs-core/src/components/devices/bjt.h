/*
 * bjt.h - bipolar junction transistor class definitions
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
 * $Id: bjt.h,v 1.3 2004-07-10 14:45:27 ela Exp $
 *
 */

#ifndef __BJT_H__
#define __BJT_H__

class net;

class bjt : public circuit
{
 public:
  bjt ();
  void calcSP (nr_double_t);
  void calcDC (void);
  void initDC (dcsolver *);
  void calcOperatingPoints (void);

 private:
  nr_double_t UbePrev;
  nr_double_t UbcPrev;
  circuit * re;
  circuit * rc;
  circuit * rb;
  nr_double_t dQbdUbe, dQbdUbc, If, Qb;
  nr_double_t gbei, gben, gbci, gbcn, gitf, gitr, gif, gir, Rbb;
};

#endif /* __BJT_H__ */
