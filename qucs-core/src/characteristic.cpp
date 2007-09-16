/*
 * characteristic.cpp - characteristic value class implementation
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: characteristic.cpp,v 1.2 2007-09-16 16:49:37 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "characteristic.h"

// Constructor creates an unnamed instance of the characteristic class.
characteristic::characteristic () : pair () {
}

// Constructor creates a named instance of the characteristic class.
characteristic::characteristic (const char * n) : pair (n) {
}

/* This full qualified constructor creates an instance of the
   characteristic class containing both the key and the value of the
   characteristic. */
characteristic::characteristic (const char * n, nr_double_t val)
  : pair (n, val) {
}

/* The copy constructor creates a new instance of the characteristic
   class based on the given characteristic object. */
characteristic::characteristic (const characteristic & p) : pair (p) {
}
