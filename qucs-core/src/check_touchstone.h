/*
 * check_touchstone.h - checker definitions for Touchstone files
 *
 * Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: check_touchstone.h,v 1.1.1.1 2003-12-20 19:03:27 ela Exp $
 *
 */

#ifndef __CHECK_TOUCHSTONE_H__
#define __CHECK_TOUCHSTONE_H__

extern dataset * touchstone_result;
extern vector  * touchstone_vector;
extern strlist * touchstone_idents;

struct touchstone_t {
  char * unit;
  char parameter;
  char * format;
  float resistance;
  float factor;
  int ports;
};

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern int touchstone_lineno;
extern FILE * touchstone_in;
extern struct touchstone_t touchstone_options;

/* Available functions of the checker. */
int touchstone_parse (void);
int touchstone_error (char *);
int touchstone_lex (void);
int touchstone_check (void);

__END_DECLS

#endif /* __CHECK_TOUCHSTONE_H__ */
