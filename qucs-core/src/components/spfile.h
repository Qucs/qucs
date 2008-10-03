/*
 * spfile.h - S-parameter file class definitions
 *
 * Copyright (C) 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: spfile.h,v 1.15 2008-10-03 14:49:49 ela Exp $
 *
 */

#ifndef __SPFILE_H__
#define __SPFILE_H__

class vector;
class matvec;
class dataset;
class spline;

struct spfile_index_t {
  vector * v;
  vector * f;
  spline * v1;
  spline * v2;
  int r;
  int c;
};

class spfile : public circuit
{
 public:
  spfile ();
  ~spfile ();
  void initSP (void);
  void calcSP (nr_double_t);
  void calcNoiseSP (nr_double_t);
  void initDC (void);
  void initTR (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void createIndex (void);
  void prepare (void);
  nr_complex_t interpolate (struct spfile_index_t * , nr_double_t);
  nr_complex_t interpolate_lin (vector *, vector *, nr_double_t, int);
  nr_complex_t interpolate_spl (spline *, spline *, nr_double_t);
  void createVector (int, int);
  matrix correlationMatrix (nr_double_t, nr_complex_t, nr_double_t, matrix);
  nr_double_t noiseFigure (matrix, matrix, nr_double_t&, nr_complex_t&,
			   nr_double_t&);
  matrix expandNoiseMatrix (matrix, matrix);
  matrix shrinkNoiseMatrix (matrix, matrix);
  matrix expandSParaMatrix (matrix);
  matrix shrinkSParaMatrix (matrix);
  matrix getInterpolMatrixS (nr_double_t);
  matrix calcMatrixCs (nr_double_t);

  CREATOR (spfile);

 private:
  dataset * data;
  vector * sfreq;
  vector * nfreq;
  struct spfile_index_t * spara;
  struct spfile_index_t * RN;
  struct spfile_index_t * FMIN;
  struct spfile_index_t * SOPT;
  char paraType;
  int  dataType;
  int  interpolType;
};

#endif /* __SPFILE_H__ */
