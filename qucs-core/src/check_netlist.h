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
 * $Id: check_netlist.h,v 1.7 2004/06/27 15:11:48 ela Exp $
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
  int var;
  int subst;
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
  int substrate;
  int nonlinear;
  struct define_t * define;
};

// Structure defining a key value pair.
struct property_t {
  char * key;       // key name
  struct {
    nr_double_t d;  // default value
    char * s;       // default string
  } defaultval;
  struct {
    nr_double_t l;  // lower bound of the value
    nr_double_t h;  // upper bound of the value
  } range;
};

// Structure defining an available component type.
struct define_t {
  char * type;                    // component name
  int nodes;                      // number of nodes
  int action;                     // is that an action?
  int substrate;                  // is that a substrate?
  int nonlinear;                  // is the component linear?
  struct property_t required[16]; // required properties
  struct property_t optional[16]; // optional proberties
};

#define PROP_ACTION       1
#define PROP_COMPONENT    0
#define PROP_SUBSTRATE    1
#define PROP_NO_SUBSTRATE 0
#define PROP_NONLINEAR    1
#define PROP_LINEAR       0
#define PROP_NO_PROP      { NULL, { PROP_NO_VAL, PROP_NO_STR }, PROP_NO_RANGE }
#define PROP_NO_VAL       0.0
#define PROP_NO_STR       ((char *) -1)
#define PROP_NO_RANGE     { 0, 0 }
#define PROP_VAL_MAX      DBL_MAX
#define PROP_VAL_MIN      DBL_MIN
#define PROP_POS_RANGE    { 0, +PROP_VAL_MAX }
#define PROP_NEG_RANGE    { -PROP_VAL_MAX, 0 }

#define PROP_IS_PROP(prop)   ((prop).key != NULL)
#define PROP_IS_VAL(prop)    ((prop).defaultval.s == PROP_NO_STR)
#define PROP_IS_STR(prop)    (!PROP_IS_VAL (prop))
#define PROP_HAS_RANGE(prop) ((prop).range.l != 0 || (prop).range.h != 0)

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
int netlist_checker_variables (void);

__END_DECLS

#endif /* __CHECKER_H__ */
