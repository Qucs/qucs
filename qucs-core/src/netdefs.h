/*
 * netdefs.h - netlist definitions for arbitrary netlists
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: netdefs.h,v 1.7 2005-10-31 16:15:31 ela Exp $
 *
 */

#ifndef __NETDEFS_H__
#define __NETDEFS_H__

/* Representation of a node list. */
struct node_t {
  char * node;
  char * xlate;
  int xlatenr;
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
  int hint;
  struct value_t * next;
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
  struct definition_t * sub;
  int duplicate;
  int action;
  int substrate;
  int nonlinear;
  int nodeset;
  int line;
  int copy;
  int ncount;
  char * text;
  char * subcircuit;
  struct value_t * values;
  struct define_t * define;
};

// Structure defining a key value pair.
struct property_t {
  char * key;       // key name
  int type;         // type of property
  struct {
    nr_double_t d;  // default value
    char * s;       // default string
  } defaultval;
  struct {
    char il;        // interval boundary
    nr_double_t l;  // lower bound of the value
    nr_double_t h;  // upper bound of the value
    char ih;        // interval boundary
  } range;
};

// Structure defining an available component type.
struct define_t {
  char * type;                    // component name
  int nodes;                      // number of nodes
  int action;                     // is that an action?
  int substrate;                  // is that a substrate?
  int nonlinear;                  // is the component linear?
  struct property_t required[64]; // required properties
  struct property_t optional[64]; // optional proberties
};

// Maximum number of S-parameter ports.
#define MAX_PORTS 256

#define PROP_ACTION       1
#define PROP_COMPONENT    0
#define PROP_SUBSTRATE    1
#define PROP_NO_SUBSTRATE 0
#define PROP_NONLINEAR    1
#define PROP_LINEAR       0
#define PROP_NODES        -1
#define PROP_NO_PROP      { NULL, PROP_REAL, { PROP_NO_VAL, PROP_NO_STR }, \
                            PROP_NO_RANGE }
#define PROP_NO_VAL       0.0
#define PROP_NO_STR       ((char *) -1)
#define PROP_NO_RANGE     { '.', 0, 0, '.' }
#define PROP_VAL_MAX      NR_MAX
#define PROP_VAL_MIN      NR_MIN
#define PROP_POS_RANGE    { '[', 0, 0, '.' }
#define PROP_NEG_RANGE    { '.', 0, 0, ']' }
#define PROP_POS_RANGEX   { ']', 0, 0, '.' }
#define PROP_NEG_RANGEX   { '.', 0, 0, '[' }
#define PROP_MIN_VAL(k)   { '[', k, 0, '.' }
#define PROP_MAX_VAL(k)   { '.', 0, k, ']' }
#define PROP_MIN_VALX(k)  { ']', k, 0, '.' }
#define PROP_MAX_VALX(k)  { '.', 0, k, '[' }
#define PROP_RNG_X01I     { ']', 0, 1, ']' }
#define PROP_INT          0
#define PROP_REAL         1
#define PROP_STR          2
#define PROP_LIST         3

#define PROP_IS_PROP(prop)   ((prop).key != NULL)
#define PROP_IS_VAL(prop)    ((prop).type != PROP_STR)
#define PROP_IS_INT(prop)    ((prop).type == PROP_INT)
#define PROP_IS_STR(prop)    (!PROP_IS_VAL (prop))
#define PROP_IS_LST(prop)    ((prop).type == PROP_LIST)
#define PROP_HAS_RANGE(prop) ((prop).range.l != 0 || (prop).range.h != 0 || \
                              (prop).range.il != '.' || (prop).range.ih != '.')

#define create_definition() \
  ((struct definition_t *) calloc (sizeof (struct definition_t), 1))
#define create_value() \
  ((struct value_t *) calloc (sizeof (struct value_t), 1))
#define create_node() \
  ((struct node_t *) calloc (sizeof (struct node_t), 1))
#define create_pair() \
  ((struct pair_t *) calloc (sizeof (struct pair_t), 1))

#endif /* __NETDEFS_H__ */
