/*
 * property.h - generic property class definitions
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.  
 *
 * $Id: property.h,v 1.2 2004/02/13 20:31:45 ela Exp $
 *
 */

#ifndef __PROPERTY_H__
#define __PROPERTY_H__

class variable;

class property
{
 public:
  property ();
  property (char *);
  property (char *, char *);
  property (char *, nr_double_t);
  property (char *, variable *);
  property (const property &);
  virtual ~property ();
  property * getNext (void) { return next; }
  void setNext (property * p) { next = p; }
  void setName (char *);
  char * getName (void);
  nr_double_t getDouble (void);
  int getInteger (void);
  char * getString (void);
  property * findProperty (char *);

 private:
  char * name;
  char * str;
  nr_double_t value;
  variable * var;
  property * next;
};

#endif /* __PROPERTY_H__ */
