/*
 * vfile.h - file based voltage source class definitions
 *
 * Copyright (C) 2007 Gunther Kraut <gn.kraut@t-online.de>
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: vfile.h,v 1.1 2007-08-13 20:45:47 ela Exp $
 *
 */

#ifndef __VFILE_H__
#define __VFILE_H__

class vector;
class dataset;
class spline;

class vfile : public circuit
{
 public:
  vfile ();
  ~vfile ();
  void initSP (void);
  void initDC (void);
  void initAC (void);
  void initTR (void);
  void calcTR (nr_double_t);
  void prepare (void);
  nr_double_t interpolate (vector * , vector * , nr_double_t);
  nr_double_t interpolate_lin (vector *, vector *, nr_double_t, int);

private:
  dataset * data;
  int  dataType;
  int  interpolType;
  vector * ts;    // time samples
  vector * s;     // voltage samples
  nr_double_t TW; // waveform duration
  spline * v1;
};

#endif /* __VFILE_H__ */
