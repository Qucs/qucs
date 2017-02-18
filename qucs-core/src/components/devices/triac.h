/*
 * triac.h - triac class definitions
 *
 * Copyright (C) 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef TRIAC_H
#define TRIAC_H

#include "devstates.h"

class triac : public qucs::circuit, public qucs::devstates
{
 public:
  CREATOR (triac);
  void calcSP (nr_double_t);
  void initDC (void);
  void calcDC (void);
  void saveOperatingPoints (void);
  void loadOperatingPoints (void);
  void calcOperatingPoints (void);
  void initAC (void);
  void calcAC (nr_double_t);
  void initTR (void);
  void calcTR (nr_double_t);

 private:
  nr_double_t Ud, gd, Id, Qi, gi, Ui;

  nr_double_t time_prev, Ud_last;
  void calcTheModel (bool);

 private:
  qucs::matrix calcMatrixY (nr_double_t);
};

#endif /* TRIAC_H */
