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
  property (const char *);
  property (const char *, const char *);
  property (const char *, nr_double_t);
  property (const char *, variable *);
  property (const property &);
  virtual ~property ();
  property * getNext (void) { return next; }
  void setNext (property * p) { next = p; }
  void setName (char *);
  char * getName (void);
  ::vector * getVector (void);
  nr_double_t getDouble (void);
  int getInteger (void);
  char * getString (void);
  char * getReference (void);
  void set (nr_double_t);
  void set (int);
  void set (char *);
  void set (variable *);
  property * findProperty (const char *);
  char * toString (void);
  bool isDefault (void) { return def; }
  void setDefault (bool d) { def = d; }

 private:
  bool def;
  int type;
  char * name;
  char * str;
  char * txt;
  nr_double_t value;
  variable * var;
  property * next;
};

#endif /* __PROPERTY_H__ */
