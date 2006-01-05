/*
 * check_vcd.h - checker definitions for a vcd file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
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
 * $Id: check_vcd.h,v 1.2 2006/01/05 07:43:31 raimi Exp $
 *
 */

#ifndef __CHECK_VCD_H__
#define __CHECK_VCD_H__

/* Externalize variables used by the scanner and parser. */
extern int vcd_lineno;
extern FILE * vcd_in;

extern struct vcd_file * vcd;

__BEGIN_DECLS

/* Available functions of the checker. */
int  vcd_checker (void);
int  vcd_parse (void);
int  vcd_error (char *);
int  vcd_lex (void);
int  vcd_lex_destroy (void);
void vcd_destroy (void);
void vcd_init (void);

__END_DECLS

/* VCD data structures. */
struct vcd_vardef {
  char * type;
  int size;
  char * code;
  char * name;
  struct vcd_vardef * next;
};

struct vcd_scope {
  char * type;
  char * name;
  struct vcd_vardef * root;
};

struct vcd_change {
  struct vcd_vardef * var;
  char * value;
  struct vcd_change * next;
};

struct vcd_changeset {
  /* Note: .vcd may contain multiple mergable sets for a single time */
  int time;
  struct vcd_change * root;
};

struct vcd_file {
  int time;
  char * scale;
  struct vcd_scope * root;
};

#endif /* __CHECK_VCD_H__ */
