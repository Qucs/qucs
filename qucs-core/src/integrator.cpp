/*
 * integrator.cpp - integrator class implementation
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
 * $Id: integrator.cpp,v 1.2 2004/09/12 18:10:21 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "integrator.h"

// Some definitions for the save-state variables.
#define STATE_SHIFT 3
#define STATE_NUM   8
#define STATE_MASK  7

// Constructor creates an unnamed instance of the integrator class.
integrator::integrator () : states<nr_double_t> () {
  coefficients = NULL;
  order = 0;
  integrate_func = NULL;
}

/* The copy constructor creates a new instance based on the given
   integrator object. */
integrator::integrator (const integrator & c) : states<nr_double_t> (c) {
  coefficients = c.coefficients;
  order = c.order;
  integrate_func = c.integrate_func;
}

// Destructor deletes a integrator object.
integrator::~integrator () {
}
