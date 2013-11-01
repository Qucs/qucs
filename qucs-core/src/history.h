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
  /*! default constructor */
  history () : age(0), 
    values(NULL), t(NULL)
    {};
  
  /*! The copy constructor creates a new instance based on the given
   history object. */
  history (const history &h)
    {
      this->age = h.age;
      this->t = h.t;
      this->values = h.values ? new tvector<nr_double_t> (*(h.values)) : NULL;
    }

  /*! Destructor deletes a history object. */
  ~history () {
    if (this->values) 
      delete this->values;
  }

  /*! The function appends the given value to the history. */
  void append (const nr_double_t val) {
    if (values == NULL) 
      this->values = new tvector<nr_double_t>;
    this->values->add (val);
    if (this->values != this->t) 
      this->drop ();
  }

  /* This function drops the most recent n values in the history. */
  void truncate (const int n)
  {
    this->t->truncate (n);
    this->values->truncate (n);
  }

  int getSize (void) const
  {
    return this->t == NULL ? 0.0 : t->getSize ();
  }

  void setAge (const nr_double_t a) { this->age = a; }
  nr_double_t getAge (void) const { return this->age; }

  // apply history
  void apply (const history & h) {
    this->t = h.t;
  }

  // Returns left-most valid index into the time value vector.
  int leftidx (void) const {
    int ts = this->t->getSize ();
    int vs = this->values->getSize ();
    return ts - vs > 0 ? ts - vs : 0;
  }

  /*! Returns number of unused values (time value vector shorter than
   value vector). */
  int unused (void) const {
    int ts = t->getSize ();
    int vs = values->getSize ();
    return vs - ts > 0 ? vs - ts : 0;
  }

  //! Returns the first (oldest) time value in the history.
  nr_double_t first (void) const {
    return (this->t != NULL) ? (*this->t)(leftidx ()) : 0.0;
  }

  //! Returns the last (youngest) time value in the history
  nr_double_t last (void) const {
    return (t != NULL) ? (*this->t)(t->getSize () - 1) : 0.0;
  }
  
  //! Returns the duration of the history.
  nr_double_t duration(void) const {
     return last () - first ();
  }
  
  void truncate (const nr_double_t);
  
  void drop (void);
  void self (void) { this->t = this->values; }

  nr_double_t interpol (nr_double_t, int, bool);
  nr_double_t nearest (nr_double_t, bool interpolate = true);
  int seek (nr_double_t, int, int, nr_double_t&, int);

  nr_double_t getTfromidx (const int idx)  {
    return this->t == NULL ? 0.0 : this->t->get(idx);
  }
  nr_double_t getValfromidx (const int idx) {
    return this->values == NULL ? 0.0 : this->values->get(idx);
  }

 private:
  bool sign;
  nr_double_t age;
  tvector<nr_double_t> * values;
  tvector<nr_double_t> * t;
};

} // namespace qucs

#endif /* __HISTORY_H__ */
