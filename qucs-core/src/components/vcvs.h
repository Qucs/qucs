/*
 * vcvs.h - vcvs class definitions
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: vcvs.h,v 1.3 2004/01/28 18:19:07 ela Exp $
 *
 */

#ifndef __VCVS_H__
#define __VCVS_H__

class vcvs : public circuit
{
 public:
  vcvs ();
  void calcS (nr_double_t);
  void calcY (void);
};

#endif /* __VCVS_H__ */
