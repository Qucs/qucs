/*
 * check_spice.cpp - checker for a Spice netlist
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
 * $Id: check_spice.cpp,v 1.1 2004/10/29 18:01:29 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <float.h>

#ifdef __MINGW32__
#define strcasecmp stricmp
#endif

#include "constants.h"
#include "check_spice.h"

/* Global definitions for parser and checker. */
struct definition_t * definition_root = NULL;
struct definition_t * subcircuit_root = NULL;
struct value_t * value_root = NULL;

// List of available Spice components.
struct define_t spice_definition_available[] =
{
  /* resistor */
  { "R", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR, 
    { { "R", PROP_REAL, { 50, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP },
    { { "Temp", PROP_REAL, { 26.85, PROP_NO_STR }, { K, PROP_VAL_MAX } },
      PROP_NO_PROP }
  },
  /* inductor */
  { "L", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    { { "L", PROP_REAL, { 1e-9, PROP_NO_STR }, PROP_NO_RANGE }, PROP_NO_PROP },
    { PROP_NO_PROP }
  },
  /* capacitor */
  { "C", 2, PROP_COMPONENT, PROP_NO_SUBSTRATE, PROP_LINEAR,
    { { "C", PROP_REAL, { 1e-12, PROP_NO_STR }, PROP_NO_RANGE },
      PROP_NO_PROP },
    { PROP_NO_PROP }
  },

  /* end of list */
  { NULL, 0, 0, 0, 0, { PROP_NO_PROP }, { PROP_NO_PROP } }
};

/* This function goes through the list of available definitions and
   checks whether the given component type can be found. */
struct define_t * spice_find_definition (char * n) {
  struct define_t * def;
  for (def = spice_definition_available; def->type != NULL; def++) {
    if (!strcasecmp (n, def->type)) return def;
  }
  return NULL;
}

/* The function reverses the order of the given value list and returns
   the reversed list. */
struct value_t * netlist_reverse_values (struct value_t * values) {
  struct value_t * root, * next;
  for (root = NULL; values != NULL; values = next) {
    next = values->next;
    values->next = root;
    root = values;
  }
  return root;
}

/* The function reverses the order of the given node list and returns
   the reversed list. */
struct node_t * netlist_reverse_nodes (struct node_t * nodes) {
  struct node_t * root, * next;
  for (root = NULL; nodes != NULL; nodes = next) {
    next = nodes->next;
    nodes->next = root;
    root = nodes;
  }
  return root;
}

/* The function creates a single translated spice node. */
struct node_t * spice_translated_node (char * node) {
  struct node_t * n = create_node ();
  if (!strcmp (node, "0")) {
    n->node = strdup ("gnd");
  }
  else {
    n->node = (char *) malloc (5 + strlen (node));
    strcpy (n->node, "_net");
    strcat (n->node, node);
  }
  return n;
}

/* The function creates the nodes list for the given component. */
struct node_t * spice_get_nodes (struct definition_t * def) {
  if (def->define->nodes <= 0) return NULL;
  struct value_t * val;
  struct node_t * root = NULL;
  int i = 0;
  for (val = def->values; val != NULL; val = val->next) {
    if (val->hint & (HINT_NODE | HINT_NUMBER)) {
      struct node_t * n = spice_translated_node (val->ident);
      n->next = root;
      root = n;
      val->hint = 0;
      if (++i >= def->define->nodes) break;
    }
  }
  return netlist_reverse_nodes (root);
}

struct pair_t * spice_get_pairs (struct definition_t * def) {
  struct value_t * val;
  struct pair_t * root = NULL;
  struct property_t * prop;
  int i = 0;
  for (val = def->values; val != NULL; val = val->next) {
    if (val->hint == 0) continue;
    if (val->hint & HINT_NUMBER) {
      prop = &def->define->required[i++];
      struct pair_t * p = create_pair ();
      p->key = strdup (prop->key);
      p->value = create_value ();
      p->value->value = strtod (val->ident, NULL);
      p->next = root;
      root = p;
      val->hint = 0;
    }
    else break;
  }
  return root;
}

/* This is the overall Spice netlist translator.  It adjusts the list
   of definitions into usable structures. */
int spice_translator (void) {
  int error = 0;
  struct definition_t * def;
  for (def = definition_root; def != NULL; def = def->next) {
    if ((def->define = spice_find_definition (def->type)) != NULL) {
      strcpy (def->type, def->define->type);
      def->nodes = spice_get_nodes (def);
      def->pairs = spice_get_pairs (def);
    }
    else {
      // skipped
    }
  }
  return error;
}

int spice_checker (void) {
  return spice_translator ();
}
