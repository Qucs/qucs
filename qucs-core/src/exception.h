/*
 * exception.h - exception class definitions
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
 * $Id: exception.h,v 1.4 2004-09-23 14:09:11 ela Exp $
 *
 */

#ifndef __EXCEPTION_H__
#define __EXCEPTION_H__

namespace qucs {

/* Enumerate exception type identifiers. */
enum exception_type {
  EXCEPTION_UNKNOWN = -1,
  EXCEPTION_PIVOT,
  EXCEPTION_NA_FAILED,
  EXCEPTION_NO_CONVERGENCE,
  EXCEPTION_ZERO_DIVISION,
  EXCEPTION_WRONG_VOLTAGE,
  EXCEPTION_SINGULAR,
};

class exception
{
 public:
  exception ();
  exception (int);
  exception (const exception &);
  ~exception ();
  int getCode (void) { return code; }
  void setCode (int c) { code = c; }
  char * getText (void) { return txt; }
  void setText (const char *, ...);
  exception * getNext (void) { return next; }
  void setNext (exception * e) { next = e; }
  void setData (int d) { data = d; }
  int getData (void) { return data; }

 private:
  int code;
  int data;
  char * txt;
  exception * next;
};

} /* namespace */

#endif /* __EXCEPTION_H__ */
