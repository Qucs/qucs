/*
 * precision.c - precision file
 *
 * Copyright (C) 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: precision.c,v 1.1 2008-01-15 19:14:01 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <math.h>

#include "precision.h"

nr_double_t nr_inf; /* the core's idea of infinity */
nr_double_t nr_nan; /* the core's idea of NaN */

void precinit (void) {
#ifdef INFINITY
  nr_inf = INFINITY;
#elif NR_INF_TESTED
  nr_inf = -log (0.0);
#else
  nr_double_t t = 1e+10;
  nr_inf = t;
  for (;;) {
    nr_inf *= 1e+10;
    if (nr_inf == t) break;
    t = nr_inf;
  }
#endif
  nr_nan = -log (-1.0);
}
