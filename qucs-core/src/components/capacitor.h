/*
 * capacitor.h - capacitor class definitions
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
 * $Id: capacitor.h,v 1.6 2004/09/11 20:39:30 ela Exp $
 *
 */

#ifndef __CAPACITOR_H__
#define __CAPACITOR_H__

class capacitor : public circuit
{
 public:
  capacitor ();
  void calcSP (nr_double_t);
  void initDC (dcsolver *);
  void calcAC (nr_double_t);
  void initAC (acsolver *);
  void calcTR (nr_double_t);
  void initTR (trsolver *);
};

#endif /* __CAPACITOR_H__ */
