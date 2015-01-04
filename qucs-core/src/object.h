/*
 * object.h - generic object class definitions
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: object.h 1871 2013-03-06 12:53:05Z crobarcro $
 *
 */

 /*! \file object.h
  *
  * Contains the definition of the generic object class.
  *
  */

#ifndef __OBJECT_H__
#define __OBJECT_H__

#define MCREATOR(val) \
  val (); \
  static struct define_t miscdef; \
  static struct define_t * definition (void) { return &miscdef; }

namespace qucs {

class variable;
class vector;
class property;

/*! \class object
 * \brief generic object class.
 *
 * Generic object class from which many Qucs classes are
 * inheirited.
 *
 */
class object
{
 public:
  object ();
  object (const char *);
  object (const object &);
  virtual ~object ();
  object * getNext (void) const { return this->next; }
  void setNext (object * const o) { this->next = o; }
  object * getPrev (void) const { return prev; }
  void setPrev (object * const o) { this->prev = o; }
  void setName (const char * const);
  const char * getName (void) const { return this->name; };
  void addProperty (property * const);
  property * addProperty (const char * const, const char * const);
  property * addProperty (const char * const, const nr_double_t);
  property * addProperty (const char * const, variable * const);
  void setProperty (const char * const, const char * const);
  void setProperty (const char * const, nr_double_t);
  void setScaledProperty (const char * const, const nr_double_t);
  void setProperty (const char * const, variable * const);
  vector * getPropertyVector (const char * const) const;
  const char * getPropertyString (const char * const) const;
  const char * getPropertyReference (const char * const) const;
  nr_double_t getPropertyDouble (const char * const) const;
  nr_double_t getScaledProperty (const char * const) const;
  int  getPropertyInteger (const char * const) const;
  bool hasProperty (const char * const) const ;
  bool isPropertyGiven (const char * const) const;
  void copyProperties (property *);
  void deleteProperties (void);
  int  countProperties (void) const;
  const char *
    propertyList (void) const;

 private:
  char * name;
  object * next;
  object * prev;
  property * prop;
};

} // namespace qucs

#endif /* __OBJECT_H__ */
