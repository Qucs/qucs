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
 * $Id: spfile.h,v 1.2 2004-07-26 06:30:29 ela Exp $
 *
 */

#ifndef __SPFILE_H__
#define __SPFILE_H__

class vector;
class matvec;
class dataset;
class spsolver;

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
  void initSP (spsolver *);
  void calcSP (nr_double_t);
  void calcNoise (nr_double_t);
  void createIndex (void);
  complex interpolate (vector *, vector *, nr_double_t);
  complex interpolate_lin (vector *, vector *, nr_double_t, int);
  complex fetchS (int, int, int);
  vector& fetchS (int, int);
  matrix& fetchS (int);
  void storeS (int, int, complex, int);
  void expandPort (void);
  void createVector (int, int);
  void createNoiseMatrix (void);
  matrix& correlationMatrix (nr_double_t, complex, nr_double_t, matrix&);
  nr_double_t noisePara (matrix&, matrix&, nr_double_t&,
			 complex&, nr_double_t&);
  matrix& expandNoiseMatrix (matrix&, matrix&);
  matrix& shrinkNoiseMatrix (matrix&, matrix&);
  matrix& expandSParaMatrix (matrix&);
  matrix& shrinkSParaMatrix (matrix&);
  matrix& getInterpolatedMatrixS (nr_double_t);

 private:
  dataset * data;
  struct spfile_index_t * index;
  vector * sfreq;
  vector * nfreq;
  vector * Rn;
  vector * Fmin;
  vector * Gopt;
  matvec * ndata;
};

#endif /* __SPFILE_H__ */
