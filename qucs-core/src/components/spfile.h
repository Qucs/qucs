/*
 * spfile.h - S-parameter file class definitions
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: spfile.h,v 1.6 2004-09-12 14:09:19 ela Exp $
 *
 */

#ifndef __SPFILE_H__
#define __SPFILE_H__

class vector;
class matvec;
class dataset;

struct spfile_index_t {
  vector * v;
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
  void calcNoise (nr_double_t);
  void createIndex (void);
  complex interpolate (vector *, vector *, nr_double_t);
  complex interpolate_lin (vector *, vector *, nr_double_t, int);
  complex fetch (int, int, int);
  vector& fetch (int, int);
  matrix& fetch (int);
  void store (int, int, complex, int);
  void createVector (int, int);
  matrix& correlationMatrix (nr_double_t, complex, nr_double_t, matrix&);
  nr_double_t noiseFigure (matrix&, matrix&, nr_double_t&, complex&,
			   nr_double_t&);
  matrix& expandNoiseMatrix (matrix&, matrix&);
  matrix& shrinkNoiseMatrix (matrix&, matrix&);
  matrix& expandSParaMatrix (matrix&);
  matrix& shrinkSParaMatrix (matrix&);
  matrix& getInterpolMatrixS (nr_double_t);

 private:
  dataset * data;
  struct spfile_index_t * index;
  vector * sfreq;
  vector * nfreq;
  vector * Rn;
  vector * Fmin;
  vector * Sopt;
  char paraType;
};

#endif /* __SPFILE_H__ */
