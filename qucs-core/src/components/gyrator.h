/*
 * gyrator.h - gyrator class definitions
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: gyrator.h,v 1.7 2005/06/02 18:17:52 raimi Exp $
 *
 */

#ifndef __GYRATOR_H__
#define __GYRATOR_H__

class gyrator : public circuit
{
 public:
  gyrator ();
  void initSP (void);
  void initDC (void);
  void initAC (void);
  void initTR (void);
};

#endif /* __GYRATOR_H__ */
