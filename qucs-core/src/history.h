/*
 * history.h - history class definitions
 *
 * Copyright (C) 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __HISTORY_H__
#define __HISTORY_H__

namespace qucs {

class history
{
 public:
  // constructor and destructor set
  history ();
  history (nr_double_t);
  history (const history &);
  ~history ();

  void append (nr_double_t);
  void truncate (int);
  void truncate (nr_double_t);
  void setAge (nr_double_t a) { age = a; }
  nr_double_t getAge (void) { return age; }
  int getSize (void);
  nr_double_t duration (void);
  nr_double_t last (void);
  nr_double_t first (void);

  int leftidx (void);
  int unused (void);
  void drop (void);
  void setTvector (tvector<nr_double_t> * v) { t = v; }
  tvector<nr_double_t> * getTvector (void) { return t; }
  void self (void) { t = values; }

  nr_double_t interpol (nr_double_t, int, bool);
  nr_double_t nearest (nr_double_t, bool interpolate = true);
  int seek (nr_double_t, int, int, nr_double_t&, int);
  nr_double_t getTfromidx (int);
  nr_double_t getValfromidx (int);

 private:
  bool sign;
  nr_double_t age;
  tvector<nr_double_t> * values;
  tvector<nr_double_t> * t;
};

} // namespace qucs

#endif /* __HISTORY_H__ */
