/*
 * operatingpoint.h - operating point class definitions
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: operatingpoint.h,v 1.1 2004/02/17 15:30:57 ela Exp $
 *
 */

#ifndef __OPERATINGPOINT_H__
#define __OPERATINGPOINT_H__

class variable;

class operatingpoint
{
 public:
  operatingpoint ();
  operatingpoint (char *);
  operatingpoint (char *, nr_double_t);
  operatingpoint (const operatingpoint &);
  virtual ~operatingpoint ();
  operatingpoint * getNext (void) { return next; }
  void setNext (operatingpoint * p) { next = p; }
  void setName (char *);
  char * getName (void);
  nr_double_t getValue (void) { return value; }
  void setValue (nr_double_t val) { value = val; }
  operatingpoint * findOperatingPoint (char *);

 private:
  char * name;
  nr_double_t value;
  operatingpoint * next;
};

#endif /* __OPERATINGPOINT_H__ */
