/*
 * ccvs.h - ccvs class definitions
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
 * $Id: ccvs.h,v 1.2 2003/12/26 14:04:07 ela Exp $
 *
 */

#ifndef __CCVS_H__
#define __CCVS_H__

class ccvs : public circuit
{
 public:
  ccvs ();
  void calcS (nr_double_t);
};

#endif /* __CCVS_H__ */
