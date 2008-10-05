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
 * $Id: object.h,v 1.12 2008/10/05 17:52:11 ela Exp $
 *
 */

#ifndef __OBJECT_H__
#define __OBJECT_H__

#define MCREATOR(val) \
  val (); \
  static struct define_t miscdef; \
  static struct define_t * definition (void) { return &miscdef; }

class property;
class variable;
class vector;

class object
{
 public:
  object ();
  object (const char *);
  object (const object &);
  virtual ~object ();
  object * getNext (void) { return next; }
  void setNext (object * o) { next = o; }
  object * getPrev (void) { return prev; }
  void setPrev (object * o) { prev = o; }
  void setName (const char *);
  char * getName (void);
  void addProperty (property *);
  property * addProperty (const char *, const char *);
  property * addProperty (const char *, nr_double_t);
  property * addProperty (const char *, variable *);
  void setProperty (const char *, char *);
  void setProperty (const char *, nr_double_t);
  void setScaledProperty (const char *, nr_double_t);
  void setProperty (const char *, variable *);
  vector * getPropertyVector (const char *);
  char * getPropertyString (const char *);
  char * getPropertyReference (const char *);
  nr_double_t getPropertyDouble (const char *);
  nr_double_t getScaledProperty (const char *);
  int  getPropertyInteger (const char *);
  bool hasProperty (const char *);
  bool isPropertyGiven (const char *);
  void copyProperties (property *);
  void deleteProperties (void);
  int  countProperties (void);
  char * propertyList (void);

 private:
  char * name;
  object * next;
  object * prev;
  property * prop;
  char * ptxt;
};

#endif /* __OBJECT_H__ */
