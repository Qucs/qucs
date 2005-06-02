/*
 * substrate.cpp - microstrip substrate class implementation
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: substrate.cpp,v 1.2 2005-06-02 18:17:56 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "node.h"
#include "property.h"
#include "substrate.h"
#include "component_id.h"

// Constructor creates an unnamed instance of the substrate class.
substrate::substrate () : object () {
}

/* The copy constructor creates a new instance based on the given
   substrate object. */
substrate::substrate (const substrate & c) : object (c) {
}

// Destructor deletes a substrate object.
substrate::~substrate () {
}
