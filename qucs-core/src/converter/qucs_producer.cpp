/*
 * qucs_producer.cpp - the Qucs netlist producer
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
 * $Id: qucs_producer.cpp,v 1.4 2004/11/24 19:16:05 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "netdefs.h"
#include "check_spice.h"

/* Global variables. */
FILE * qucs_out = NULL;
int    qucs_actions = 1;

/* Prints value representation. */
static void netlist_list_value (struct value_t * value) {
  if (value->ident)
    fprintf (qucs_out, "%s", value->ident);
  else {
    fprintf (qucs_out, "%g", value->value);
    if (value->scale)
      fprintf (qucs_out, "%s", value->scale);
    if (value->unit)
      fprintf (qucs_out, "%s", value->unit);
  }
}

/* Prints a single definition on a single line. */
static void netlist_list_def (struct definition_t * def, char * prefix) {
  struct node_t * node;
  struct pair_t * pair;
  if (def->define == NULL) {
    if (def->text != NULL)
      fprintf (qucs_out, "%s# %s\n", prefix, def->text);
  }
  else {
    if (!qucs_actions) {
      // skip specific actions if required
      if (def->action && strcmp (def->type, "Def")) return;
    }
    fprintf (qucs_out, "%s%s%s:%s", prefix, def->action ? "." : "",
	     def->type, def->instance);
    for (node = def->nodes; node != NULL; node = node->next)
      fprintf (qucs_out, " %s", node->node);
    for (pair = def->pairs; pair != NULL; pair = pair->next) {
      fprintf (qucs_out, " %s=\"", pair->key);
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\"");
    }
    fprintf (qucs_out, "\n");
  }
}

/* Prints definition list representation. */
static void netlist_lister (struct definition_t * root, char * prefix) {
  struct definition_t * def;
  for (def = root; def != NULL; def = def->next) {
    if (def->sub != NULL) {
      netlist_list_def (def, prefix);
      netlist_lister (def->sub, "  ");
      fprintf (qucs_out, ".Def:End\n");
    }
    else {
      netlist_list_def (def, prefix);
    }
  }
}

/* Prints the overall netlist representation. */
static void netlist_list (void) {
  struct definition_t * def;
  time_t t = time (NULL);
  fprintf (qucs_out, "# converted Qucs netlist processed at %s\n", ctime (&t));
  if (spice_title != NULL) {
    fprintf (qucs_out, "#\n# %s#\n\n", spice_title);
    free (spice_title);
  }
  netlist_lister (definition_root, "");
  for (def = subcircuit_root; def != NULL; def = def->next) {
    fprintf (qucs_out, ".Def:%s\n", def->instance);
    netlist_lister (def->sub, "  ");
    fprintf (qucs_out, ".Def:End\n");
  }
}

/* This function is the overall Qucs netlist producer. */
void qucs_producer (void) {
  if (qucs_out != NULL) {
    netlist_list ();
  }
}
