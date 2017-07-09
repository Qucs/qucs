/*
 * ohmmeter.h - AC/DC and Transient ohmmeter class definitions
 *
 * Copyright (C) 2016, Nelson Martins, macedo.pp@gmail.com
 *		 2017, Alberto Silva, 1120196@isep.ipp.pt	
 *		
 * This file is part of Qucs
 *
 * Qucs is free software; you can redistribute it and/or modify
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
 * along with Qucs.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __OHMMETER_H__
#define __OHMMETER_H__

class ohmmeter : public qucs::circuit
{
 public:
  CREATOR (ohmmeter);
  void initDC (void);
  void initDC2 (void);
  void initAC (void);
  void initAC2 (void);
  void initTR (void);
  void calcOperatingPoints (void);
  void saveOperatingPoints (void);
  void setstate(int);
  int getstate();
 private:
  int on;
};

#endif
