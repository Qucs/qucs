/*
 * check_netlist.h - checker definitions for the Qucs netlist
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
 * $Id: check_netlist.h,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
 *
 */

#ifndef __CHECKER_H__
#define __CHECKER_H__

/* Representation of a node list. */
struct node_t {
  char * node;
  struct node_t * next;
};

/* Representation of a value. */
struct value_t {
  char * ident;
  char * unit;
  char * scale;
  double value;
};

/* Representation of a key / value pair. */
struct pair_t {
  char * key;
  struct value_t * value;
  struct pair_t * next;
};

/* Representation of a definition line in the netlist. */
struct definition_t {
  char * type;
  char * instance;
  struct node_t * nodes;
  struct pair_t * pairs;
  struct definition_t * next;
  int duplicate;
  int action;
};

__BEGIN_DECLS

/* Externalize variables used by the scanner and parser. */
extern struct definition_t * definition_root;
extern struct node_t * node_root;
extern struct pair_t * pair_root;
extern int netlist_lineno;
extern FILE * netlist_in;

/* Available functions of the checker. */
void netlist_list (void);
void netlist_destroy (void);
int netlist_checker (void);
int netlist_parse (void);
int netlist_error (char *);
int netlist_lex (void);

__END_DECLS

#endif /* __CHECKER_H__ */
