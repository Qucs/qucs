/*
 * check_spice.h - checker definitions for a Spice netlist
 *
 * Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: check_spice.h,v 1.5 2005-03-02 07:36:20 raimi Exp $
 *
 */

#ifndef __CHECK_SPICE_H__
#define __CHECK_SPICE_H__

#include "netdefs.h"

#define HINT_NODE         1
#define HINT_NUMBER       2
#define HINT_NAME         4
#define HINT_PAIR         8
#define HINT_DONE        16
#define HINT_MSTART      32
#define HINT_MSTOP       64

/* Externalize variables used by the scanner and parser. */
extern int spice_lineno;
extern FILE * spice_in;

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern struct definition_t * definition_root;
extern struct definition_t * subcircuit_root;
extern char * spice_title;

/* Available functions of the checker. */
int  spice_checker (void);
int  spice_parse (void);
int  spice_error (char *);
int  spice_lex (void);

/* Local functionality. */
void spice_add_last_hint (struct value_t *, int);
void spice_set_last_hint (struct value_t *, int);
struct value_t * netlist_append_values (struct value_t *, struct value_t *);
struct definition_t * spice_checker_intern (struct definition_t *);
extern int spice_errors;

__END_DECLS

#endif /* __CHECK_SPICE_H__ */
