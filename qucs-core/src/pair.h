/*
 * pair.h - key/value pair class definitions
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: pair.h,v 1.1 2006-01-09 09:11:07 raimi Exp $
 *
 */

#ifndef __PAIR_H__
#define __PAIR_H__

class pair
{
 public:
  pair ();
  pair (char *);
  pair (char *, nr_double_t);
  pair (const pair &);
  ~pair ();

  void setName (char *);
  char * getName (void);
  nr_double_t getValue (void) { return value; }
  void setValue (nr_double_t val) { value = val; }

 private:
  char * name;
  nr_double_t value;
};

#endif /* __PAIR_H__ */
