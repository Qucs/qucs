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
 * $Id: transient.h,v 1.1 2004/09/11 20:39:29 ela Exp $
 *
 */

#ifndef __TRANSIENT_H__
#define __TRANSIENT_H__

class circuit;

class transient
{
 public:
  friend void calcCoefficients (char *, int, nr_double_t *, nr_double_t);
  friend void integrateEuler (circuit *, int, nr_double_t, nr_double_t&,
			      nr_double_t&);
  friend void integrateBilinear (circuit *, int, nr_double_t, nr_double_t&,
				 nr_double_t&);
  friend void setIntegrationMethod (circuit *, char *);
};

#endif /* __TRANSIENT_H__ */
