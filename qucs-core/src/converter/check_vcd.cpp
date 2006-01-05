/*
 * check_vcd.cpp - iterate a vcd file
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
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
 * $Id: check_vcd.cpp,v 1.2 2006-01-05 07:43:31 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <float.h>
#include <ctype.h>

#include "check_vcd.h"

struct vcd_file * vcd = NULL;

int vcd_checker (void) {
  return -1;
}

void vcd_destroy (void) {
}

void vcd_init (void) {
  vcd = (struct vcd_file *) calloc (1, sizeof (struct vcd_file));
}
