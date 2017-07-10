/*
 * vfile.h - file based voltage source class definitions
 *
 * Copyright (C) 2007 Gunther Kraut <gn.kraut@t-online.de>
 * Copyright (C) 2007, 2008, 2009 Stefan Jahn <stefan@lkcc.org>
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

#ifndef VFILE_H
#define VFILE_H

namespace qucs {
  class dataset;
  class interpolator;
}

class vfile : public qucs::circuit
{
 public:
  CREATOR (vfile);
  ~vfile ();
  void initSP (void);
  void initDC (void);
  void initAC (void);
  void initTR (void);
  void calcTR (nr_double_t);
  void prepare (void);

private:
  qucs::dataset * data;
  int dataType;
  int interpolType;
  qucs::interpolator * inter;
};

#endif /* VFILE_H */
