/*
 * idc.h - DC current source class definitions
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
 * $Id: idc.h,v 1.6 2004/11/24 19:15:48 raimi Exp $
 *
 */

#ifndef __IDC_H__
#define __IDC_H__

class idc : public circuit
{
 public:
  idc ();
  void initSP (void);
  void initDC (void);
  void initAC (void);
  void initTR (void);
};

#endif /* __IDC_H__ */
