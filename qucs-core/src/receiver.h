/*
 * receiver.h - EMI receiver class definitions
 *
 * Copyright (C) 2009 Dirk Schaefer <schad@5pm.de>
 * Copyright (C) 2009 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __RECEIVER_H__
#define __RECEIVER_H__

namespace emi {

  // receiver setting
  struct settings {
    nr_double_t start;
    nr_double_t stop;
    nr_double_t stepsize;
    nr_double_t bandwidth;
  };

  // internal helper functions
  nr_int32_t nearestbin32 (int);
  nr_double_t f_2ndorder (nr_double_t, nr_double_t, nr_double_t);
  nr_double_t f_gauss (nr_double_t, nr_double_t, nr_double_t);
  nr_double_t f_ideal (nr_double_t, nr_double_t, nr_double_t);
  ::vector * receiver (nr_double_t *, nr_double_t, int);

  // external functionality
  ::vector * receiver (::vector *, ::vector *, int len = -1);

} // namespace

#endif /* __RECEIVER_H__ */
