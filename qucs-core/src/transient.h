/*
 * transient.h - transient helper class definitions
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
 * $Id: transient.h,v 1.5 2004-10-07 19:49:35 ela Exp $
 *
 */

#ifndef __TRANSIENT_H__
#define __TRANSIENT_H__

enum integrator_type {
  INTEGRATOR_UNKNOWN       = -1,
  INTEGRATOR_EULER         =  0,
  INTEGRATOR_TRAPEZOIDAL   =  1,
  INTEGRATOR_GEAR          =  2,
  INTEGRATOR_ADAMSMOULTON  =  3,
  INTEGRATOR_ADAMSBASHFORD =  4
};

class circuit;
class integrator;

class transient
{
 public:
  friend void calcCorrectorCoeff (int, int, nr_double_t *, nr_double_t *,
				  int&);
  friend void calcPredictorCoeff (int, int, nr_double_t *, nr_double_t *);
  friend void integrateEuler (integrator *, int, nr_double_t, nr_double_t&,
			      nr_double_t&);
  friend void integrateBilinear (integrator *, int, nr_double_t, nr_double_t&,
				 nr_double_t&);
  friend void integrateGear (integrator *, int, nr_double_t, nr_double_t&,
			     nr_double_t&);
  friend void integrateMoulton (integrator *, int, nr_double_t, nr_double_t&,
				nr_double_t&);
  friend void setIntegrationMethod (circuit *, int);
  friend int  correctorType (char *, int&);
  friend int  correctorType (int, int);
  friend int  predictorType (int, int, int&);
  friend nr_double_t getCorrectorError (int, int);
  friend nr_double_t getPredictorError (int, int);
};

#endif /* __TRANSIENT_H__ */
