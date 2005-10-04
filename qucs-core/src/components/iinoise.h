/*
 * iinoise.h - correlated noise current sources class definitions
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: iinoise.h,v 1.1 2005/10/04 10:52:29 raimi Exp $
 *
 */

#ifndef __IINOISE_H__
#define __IINOISE_H__

class iinoise : public circuit
{
 public:
  iinoise ();
  void initSP (void);
  void calcNoiseSP (nr_double_t);
  void calcNoiseAC (nr_double_t);

 private:
  matrix calcMatrixCy (nr_double_t);
};

#endif /* __IINOISE_H__ */
