/*
 * fourier.h - fourier transformation class definitions
 *
 * Copyright (C) 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: fourier.h,v 1.1 2005-10-17 08:42:03 raimi Exp $
 *
 */

#ifndef __FOURIER_H__
#define __FOURIER_H__

class vector;

class fourier
{
 public:
  friend vector fft_1d (vector, int isign = 1);
  friend vector ifft_1d (vector var) { return fft_1d (var, -1); }

 private:
  friend void  _fft_1d (nr_double_t *, int, int isign = 1);
  friend void _ifft_1d (nr_double_t * data, int len) {
    _fft_1d (data, len, -1); }
};

#endif /* __FOURIER_H__ */
