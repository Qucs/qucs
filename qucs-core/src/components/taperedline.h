/*
* taperedline.h - ideal tapered transmission line class definition
*
* Copyright (C) 2015 Andres Martinez-Mera <andresmartinezmera@gmail.com>
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
 * $Id$
 *
 */

#ifndef __taperedline_H__
#define __taperedline_H__

class taperedline : public qucs::circuit
{
 public:
  CREATOR (taperedline);
  void calcSP (nr_double_t);
  void initDC (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void calcNoiseAC (nr_double_t);
  void calcNoiseSP (nr_double_t);
private:
  void calcABCDparams(nr_double_t);
  nr_double_t calcExponential(nr_double_t, nr_double_t, nr_double_t, nr_double_t);
  nr_double_t calcLinear(nr_double_t, nr_double_t, nr_double_t, nr_double_t);
  nr_double_t calcTriangular(nr_double_t, nr_double_t, nr_double_t, nr_double_t);
  nr_double_t calcKlopfenstein(nr_double_t, nr_double_t, nr_double_t, nr_double_t, nr_double_t);
  nr_double_t phi(nr_double_t, nr_double_t);
  nr_double_t besseli(nr_double_t, nr_double_t);
  long int factorial(int);
  nr_complex_t A, B, C, D;
};

#endif /* __taperedline_H__ */

