/*
 * nor.h - logical nor class definitions
 *
 * Copyright (C) 2005, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: nor.h,v 1.3 2008/10/03 14:49:49 ela Exp $
 *
 */

#ifndef __NOR_H__
#define __NOR_H__

class logicnor : public digital
{
 public:
  logicnor ();
  void calcOutput (void);
  void calcDerivatives (void);

  CREATOR (logicnor);
};

#endif /* __NOR_H__ */
