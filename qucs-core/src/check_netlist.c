/*
 * check_netlist.c - checker for the Qucs netlist
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: check_netlist.c,v 1.6 2004-02-09 18:26:08 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "check_netlist.h"
#include "logging.h"

struct definition_t * definition_root = NULL;
struct node_t * node_root = NULL;
struct pair_t * pair_root = NULL;

struct definition {
  char * type;
  int nodes;
  int action;
  char * required[16];
  char * optional[16];
};

struct definition definition_available[] =
  {
    /* resistor */
    { "R", 2, 0, { "R", NULL }, { NULL } },
    /* inductor */
    { "L", 2, 0, { "L", NULL }, { NULL } },
    /* capacitor */
    { "C", 2, 0, { "C", NULL }, { NULL } },
    /* voltage controlled current source */
    { "VCCS", 4, 0, { "G", NULL }, { "T", NULL } },
    /* current controlled current source */
    { "CCCS", 4, 0, { "G", NULL }, { "T", NULL } },
    /* voltage controlled voltage source */
    { "VCVS", 4, 0, { "G", NULL }, { "T", NULL } },
    /* current controlled voltage source */
    { "CCVS", 4, 0, { "G", NULL }, { "T", NULL } },
    /* power source */
    { "Pac", 2, 0, { "f", "Z", "Num", NULL }, { "P", NULL } },
    /* circulator */
    { "Circulator", 3, 0, { "Z1", "Z2", "Z3", NULL }, { NULL } },
    /* isolator */
    { "Isolator", 2, 0, { "Z1", "Z2", NULL }, { NULL } },
    /* attenuator */
    { "Attenuator", 2, 0, { "L", "Zref", NULL }, { NULL } },
    /* bias tee */
    { "BiasT", 3, 0, { NULL }, { NULL } },
    /* DC feed */
    { "DCFeed", 2, 0, { NULL }, { NULL } },
    /* DC block */
    { "DCBlock", 2, 0, { NULL }, { NULL } },
    /* transformator */
    { "Tr", 4, 0, { "T", NULL }, { NULL } },
    /* symmetrical transformator */
    { "sTr", 6, 0, { "T1", "T2", NULL }, { NULL } },
    /* DC voltage source */
    { "Vdc", 2, 0, { "U", NULL }, { NULL } },
    /* DC current source */
    { "Idc", 2, 0, { "I", NULL }, { NULL } },
    /* AC voltage source */
    { "Vac", 2, 0, { "U", "f", NULL }, { NULL } },
    /* phase shifter */
    { "PShift", 2, 0, { "phi", "Zref", NULL }, { NULL } },
    /* gyrator */
    { "Gyrator", 4, 0, { "R", "Zref", NULL }, { NULL } },
    /* ideal transmission line */
    { "TLIN", 2, 0, { "Z", "L", NULL }, { NULL } },
    /* DC current probe */
    { "IProbe", 2, 0, { NULL }, { NULL } },

    /* diode */
    { "Diode", 2, 0, { "Is", "n", NULL }, { NULL } },

    /* s-parameter analysis */
    { "SP", 0, 1, { "Start", "Stop", "Step", NULL }, { NULL } },
    /* dc analysis */
    { "DC", 0, 1, { NULL }, { NULL } },

    /* end of list */
    { NULL, 0, 0, { NULL }, { NULL } }
  };

/* The function counts the nodes in a definition line. */
static int checker_count_nodes (struct definition_t * def) {
  struct node_t * node;
  int count = 0;
  for (node = def->nodes; node != NULL; node = node->next)
    count++;
  return count;
}

/* The function returns an available definition line for the given
   type.  If there is no such definition type the function returns
   NULL. */
static struct definition * checker_find_definition (char * type, int action) {
  struct definition * def;
  for (def = definition_available; def->type != NULL; def++) {
    if (!strcmp (type, def->type) && action == def->action)
      return def;
  }
  return NULL;
}

/* The function returns the number of properties in a definition line
   specified by the given key. */
static int checker_find_property (char * key, struct pair_t * pair) {
  int count = 0;
  while (pair != NULL) {
    if (!strcmp (pair->key, key))
      count++;
    pair = pair->next;
  }
  return count;
}

/* Checks if the given property key is either optional or required for
   the given definition type. */
static int checker_is_property (struct definition * available, char * key) {
  int i;
  for (i = 0; available->required[i] != NULL; i++) {
    if (!strcmp (available->required[i], key))
      return 1;
  }
  for (i = 0; available->optional[i] != NULL; i++) {
    if (!strcmp (available->optional[i], key))
      return 1;
  }
  return 0;
}

/* Counts the number of definitions given by the specified type and
   instance name in the definition list. */
static int checker_count_definition (char * type, char * instance) {
  struct definition_t * def;
  int count = 0;
  for (def = definition_root; def != NULL; def = def->next) {
    if (!strcmp (def->type, type) && !strcmp (def->instance, instance)) {
      if (++count > 1)
	def->duplicate = 1;
    }
  }
  return count;
}

/* Resolves the variable of a property value.  Returns non-zero on
   success, otherwise zero. */
static int checker_resolve_variable (struct value_t * value) {
  if (value->ident != NULL) {
    logprint (LOG_ERROR, "checker error, no such variable `%s'\n", 
	      value->ident);
    return 0;
  }
  return 1;
}

/* Evaluates the unit scale in a property value.  It adjusts the
   actual value and omits the scale.  The function returns non-zero
   on success and zero otherwise. */
static int checker_evaluate_scale (struct value_t * value) {
  double val = value->value, factor = 1.0;
  char * scale;
  if (value->scale != NULL) {
    scale = value->scale;
    switch (*scale) {
    case 'T':
      scale++; factor = 1e12; break;
    case 'G':
      scale++; factor = 1e9; break;
    case 'M':
      scale++; factor = 1e6; break;
    case 'k':
      scale++; factor = 1e3; break;
    case 'm':
      scale++; factor = 1e-3; break;
    case 'u':
      scale++; factor = 1e-6; break;
    case 'n':
      scale++; factor = 1e-9; break;
    case 'p':
      scale++; factor = 1e-12; break;
    case 'f':
      scale++; factor = 1e-15; break;
    case 'a':
      scale++; factor = 1e-18; break;
    case 'd':
      scale++;
      if (*scale == 'B') {
	scale++; val = pow (10.0, val / 10.0);
	if (*scale == 'm') {
	  scale++; factor = 1e-3;
	}
      }
      break;
    }
    if (*scale != '\0') {
      value->unit = strdup (scale);
    }
    free (value->scale);
    value->scale = NULL;
  }
  value->value = val * factor;
  return 1;
}

/* The function returns the number of instances of the given type within
   the list of definitions. */
static int checker_count_definitions (char * type, int action) {
  struct definition_t * def;
  int count = 0;
  for (def = definition_root; def != NULL; def = def->next) {
    if (def->action == action) {
      if (type == NULL)
	count++;
      else if (!strcmp (def->type, type))
	count++;
    }
  }
  return count;
}

/* This function checks the actions to be taken in the netlist.  It
   returns zero on success, non-zero otherwise. */
static int checker_validate_actions (void) {
  int n, errors = 0;
  if ((n = checker_count_definitions (NULL, 1)) < 1) {
    logprint (LOG_ERROR, "checker error, no actions .XX defined\n");
    errors++;
  }
  else {
    if ((n = checker_count_definitions ("SP", 1)) >= 1) {
      if ((n = checker_count_definitions ("Pac", 0)) < 1) {
	logprint (LOG_ERROR, "checker error, %d `Pac' definitions found, at "
		  "least 1 required\n", n);
	errors++;
      }
    }
  }
  return errors;
}

/* This function is the checker routine for a parsed netlist.  It
   returns zero on success or non-zero if the parsed netlist contained
   errors. */
int netlist_checker (void) {

  struct definition_t * def;
  struct pair_t * pair;
  struct definition * available;
  int i, n, errors = 0;

  /* go through all definitions */
  for (def = definition_root; def != NULL; def = def->next) {

    /* check whether the definition type is known */
    available = checker_find_definition (def->type, def->action);
    if (available == NULL) {
      logprint (LOG_ERROR, "checker error, invalid definition type `%s'\n", 
		def->type);
      errors++;
    }
    else {
      /* check whether the number of nodes is correct */
      n = checker_count_nodes (def);
      if (available->nodes != n) {
	logprint (LOG_ERROR, 
		  "checker error, %d nodes required in `%s:%s', found %d\n",
		  available->nodes, def->type, def->instance, n);
	errors++;
      }
      /* check whether the required properties are given */
      for (i = 0; available->required[i] != NULL; i++) {
	n = checker_find_property (available->required[i], def->pairs);
	if (n != 1) {
	  logprint (LOG_ERROR, "checker error, required property `%s' "
		    "occurred %dx in `%s:%s'\n",
		    available->required[i], n, def->type, def->instance);
	  errors++;
	}
      }
      /* check whether the optional properties are given zero/once */
      for (i = 0; available->optional[i] != NULL; i++) {
	n = checker_find_property (available->optional[i], def->pairs);
	if (n >= 2) {
	  logprint (LOG_ERROR, "checker error, optional property `%s' "
		    "occurred %dx in `%s:%s'\n",
		    available->optional[i], n, def->type, def->instance);
	  errors++;
	}
      }
      /* check the properties */
      for (pair = def->pairs; pair != NULL; pair = pair->next) {
	/* check whether properties are either required or optional */
	if (!checker_is_property (available, pair->key)) {
	  logprint (LOG_ERROR, 
		    "checker error, extraneous property `%s' is invalid "
		    "in `%s:%s'\n",
		    pair->key, def->type, def->instance);
	  errors++;
	}
	/* check variables in properties */
	if (!checker_resolve_variable (pair->value))
	  errors++;
	/* check and evaluate the unit scale in a property */
	if (!checker_evaluate_scale (pair->value))
	  errors++;
      }
    }
    /* check the number of definitions */
    n = checker_count_definition (def->type, def->instance);
    if (n != 1 && def->duplicate == 0) {
      logprint (LOG_ERROR, "checker error, found %d definitions of `%s:%s'\n",
		n, def->type, def->instance);
      errors++;
    }
  }

  /* check actions */
  errors += checker_validate_actions ();
  return errors ? -1 : 0;
}

#if DEBUG
/* Debug function: Prints value representation. */
static void netlist_list_value (struct value_t * value) {
  if (value->ident)
    logprint (LOG_STATUS, "%s", value->ident);
  else {
    logprint (LOG_STATUS, "%.3e", value->value);
    if (value->scale)
      logprint (LOG_STATUS, "%s", value->scale);
    if (value->unit)
      logprint (LOG_STATUS, "%s", value->unit);
  }
}

/* Debug function: Prints definition list representation. */
void netlist_list (void) {
  struct definition_t * def;
  struct node_t * node;
  struct pair_t * pair;
  for (def = definition_root; def != NULL; def = def->next) {
    logprint (LOG_STATUS, "%s:%s", def->type, def->instance);
    for (node = def->nodes; node != NULL; node = node->next) {
      logprint (LOG_STATUS, " %s", node->node);
    }
    for (pair = def->pairs; pair != NULL; pair = pair->next) {
      logprint (LOG_STATUS, " %s=\"", pair->key);
      netlist_list_value (pair->value);
      logprint (LOG_STATUS, "\"");
    }
    logprint (LOG_STATUS, "\n");
  }
}
#endif /* DEBUG */

/* Deletes node list of a definition. */
static void netlist_free_nodes (struct node_t * node) {
  struct node_t * n;
  for (; node != NULL; node = n) {
    n = node->next;
    free (node->node);
    free (node);
  }
}

/* Deletes pair list of a definition. */
static void netlist_free_pairs (struct pair_t * pair) {
  struct pair_t * n;
  for (; pair != NULL; pair = n) {
    n = pair->next;
    if (pair->value) {
      if (pair->value->ident) free (pair->value->ident);
      if (pair->value->unit)  free (pair->value->unit);
      if (pair->value->scale) free (pair->value->scale);
      free (pair->value);
    }
    free (pair->key);
    free (pair);
  }
}

/* Deletes the definition list. */
void netlist_destroy (void) {
  struct definition_t * def, * n;
  for (def = definition_root; def != NULL; def = n) {
    n = def->next;
    netlist_free_nodes (def->nodes);
    netlist_free_pairs (def->pairs);
    free (def->type);
    free (def->instance);
    free (def);
  }
  definition_root = NULL;
  node_root = NULL;
  pair_root = NULL;
}
