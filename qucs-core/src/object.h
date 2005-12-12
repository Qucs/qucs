/*
 * object.h - generic object class definitions
 *
 * Copyright (C) 2003, 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: object.h,v 1.8 2005/12/12 07:46:53 raimi Exp $
 *
 */

#ifndef __OBJECT_H__
#define __OBJECT_H__

class property;
class variable;
class vector;

class object
{
 public:
  object ();
  object (char *);
  object (const object &);
  virtual ~object ();
  object * getNext (void) { return next; }
  void setNext (object * o) { next = o; }
  object * getPrev (void) { return prev; }
  void setPrev (object * o) { prev = o; }
  void setName (char *);
  char * getName (void);
  void addProperty (property *);
  void addProperty (char *, char *);
  void addProperty (char *, nr_double_t);
  void addProperty (char *, variable *);
  void setProperty (char *, char *);
  void setProperty (char *, nr_double_t);
  void setScaledProperty (char *, nr_double_t);
  void setProperty (char *, variable *);
  vector * getPropertyVector (char *);
  char * getPropertyString (char *);
  nr_double_t getPropertyDouble (char *);
  nr_double_t getScaledProperty (char *);
  int  getPropertyInteger (char *);
  int  hasProperty (char *);
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
