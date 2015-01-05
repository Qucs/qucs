/*
 * property.h - generic property class definitions
 *
 * Copyright (C) 2003, 2004, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

#include <string>

namespace qucs {

class variable;
class vector;

enum property_type {
  PROPERTY_UNKNOWN = -1,
  PROPERTY_INT,
  PROPERTY_DOUBLE,
  PROPERTY_STR,
  PROPERTY_VAR
};

class property
{
 public:
  property ();
  property (const std::string &);
  property (const std::string &, const std::string &);
  property (const std::string &, nr_double_t);
  property (const std::string &, variable *);
  property (const property &);
  virtual ~property ();
  property * getNext (void) const { return next; }
  void setNext (property * p) { next = p; }

  //! Sets the name of the property.
  void setName (const std::string &n) {
    this->name = n;
  };

  //! Returns the name of the property.
  std::string getName (void) const {
    return this->name;
  }

  qucs::vector * getVector (void) const;
  nr_double_t getDouble (void) const;
  int getInteger (void) const;
  const char * getString (void) const;
  const char * getReference (void) const;
  void set (const nr_double_t);
  void set (int);
  void set (const std::string &);
  void set (variable *);
  property * findProperty (const std::string &n);
  std::string toString (void) const;
  bool isDefault (void) const { return def; }
  void setDefault (bool d) { def = d; }

 private:
  bool def;
  int type;
  std::string name;
  std::string str;
  nr_double_t value;
  variable * var;
  property * next;
};

} // namespace qucs

#endif /* __PROPERTY_H__ */
