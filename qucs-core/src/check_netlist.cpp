/*
 * check_netlist.cpp - checker for the Qucs netlist
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
 * $Id: check_netlist.cpp,v 1.13 2004-06-16 17:41:45 margraf Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "logging.h"
#include "strlist.h"
#include "equation.h"
#include "check_netlist.h"

struct definition_t * definition_root = NULL;
struct node_t * node_root = NULL;
struct pair_t * pair_root = NULL;

struct definition {
  char * type;
  int nodes;
  int action;
  int substrate;
  int nonlinear;
  char * required[16];
  char * optional[16];
};

struct definition definition_available[] =
  {
    /* resistor */
    { "R", 2, 0, 0, 0, { "R", NULL }, { "Symbol", NULL } },
    /* inductor */
    { "L", 2, 0, 0, 0, { "L", NULL }, { NULL } },
    /* capacitor */
    { "C", 2, 0, 0, 0, { "C", NULL }, { NULL } },
    /* voltage controlled current source */
    { "VCCS", 4, 0, 0, 0, { "G", NULL }, { "T", NULL } },
    /* current controlled current source */
    { "CCCS", 4, 0, 0, 0, { "G", NULL }, { "T", NULL } },
    /* voltage controlled voltage source */
    { "VCVS", 4, 0, 0, 0, { "G", NULL }, { "T", NULL } },
    /* current controlled voltage source */
    { "CCVS", 4, 0, 0, 0, { "G", NULL }, { "T", NULL } },
    /* power source */
    { "Pac", 2, 0, 0, 0, { "f", "Z", "Num", NULL }, { "P", NULL } },
    /* circulator */
    { "Circulator", 3, 0, 0, 0, { "Z1", "Z2", "Z3", NULL }, { NULL } },
    /* isolator */
    { "Isolator", 2, 0, 0, 0, { "Z1", "Z2", NULL }, { NULL } },
    /* attenuator */
    { "Attenuator", 2, 0, 0, 0, { "L", "Zref", NULL }, { NULL } },
    /* bias tee */
    { "BiasT", 3, 0, 0, 0, { NULL }, { NULL } },
    /* DC feed */
    { "DCFeed", 2, 0, 0, 0, { NULL }, { NULL } },
    /* DC block */
    { "DCBlock", 2, 0, 0, 0, { NULL }, { NULL } },
    /* transformator */
    { "Tr", 4, 0, 0, 0, { "T", NULL }, { NULL } },
    /* symmetrical transformator */
    { "sTr", 6, 0, 0, 0, { "T1", "T2", NULL }, { NULL } },
    /* DC voltage source */
    { "Vdc", 2, 0, 0, 0, { "U", NULL }, { NULL } },
    /* DC current source */
    { "Idc", 2, 0, 0, 0, { "I", NULL }, { NULL } },
    /* AC voltage source */
    { "Vac", 2, 0, 0, 0, { "U", "f", NULL }, { NULL } },
    /* AC current source */
    { "Iac", 2, 0, 0, 0, { "I", "f", NULL }, { NULL } },
    /* phase shifter */
    { "PShift", 2, 0, 0, 0, { "phi", "Zref", NULL }, { NULL } },
    /* gyrator */
    { "Gyrator", 4, 0, 0, 0, { "R", "Zref", NULL }, { NULL } },
    /* ideal transmission line */
    { "TLIN", 2, 0, 0, 0, { "Z", "L", NULL }, { NULL } },
    /* DC current probe */
    { "IProbe", 2, 0, 0, 0, { NULL }, { NULL } },

    /* diode */
    { "Diode", 2, 0, 0, 1, { "Is", "N", "M", "Cj0", "Vj", NULL },
      { "Rs", "Tt", NULL } },
    /* jfet */
    { "JFET", 3, 0, 0, 1, { "Is", "N", "Vt0", "Lambda", "Beta", "M", "Pb",
			    "Fc", "Cgs", "Cgd", NULL },
      { "Rd", "Rs", NULL } },

    /* microstrip substrate */
    { "SUBST", 0, 0, 1, 0, { "er", "h", "t", "tand", "rho", "D", NULL },
      { NULL } },
    /* microstrip line */
    { "MLIN", 2, 0, 0, 0, { "W", "L", "Subst", "Model", NULL }, { NULL } },

    /* s-parameter analysis */
    { "SP", 0, 1, 0, 0, { "Start", "Stop", "Step", NULL }, { NULL } },
    /* dc analysis */
    { "DC", 0, 1, 0, 0, { NULL }, { NULL } },
    /* parameter sweep */
    { "SW", 0, 1, 0, 0, { "Start", "Stop", "Step", "Param", "Sim", NULL }, 
      { NULL } },

    /* end of list */
    { NULL, 0, 0, 0, 0, { NULL }, { NULL } }
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

/* Returns the value for a given definition type, key and variable
   identifier if it is in the list of definitions.  Otherwise the
   function returns NULL. */
static struct value_t * checker_find_variable (char * type, char * key, 
					       char * ident) {
  struct definition_t * def;
  struct pair_t * pair;
  for (def = definition_root; def != NULL; def = def->next) {
    if (!strcmp (def->type, type)) {
      for (pair = def->pairs; pair != NULL; pair = pair->next) {
	if (!strcmp (pair->key, key))
	  if (pair->value->ident != NULL && ident != NULL &&
	      strcmp (pair->value->ident, ident) == 0)
	    return pair->value;
      }
    }
  }
  return NULL;
}

/* Resolves the variable of a property value.  Returns non-zero on
   success, otherwise zero. */
static int checker_resolve_variable (struct value_t * value) {
  struct value_t * val;
  if (value->ident != NULL) {
    int found = 0;
    /* 1. find variable in parameter sweeps */
    if ((val = checker_find_variable ("SW", "Param", value->ident))) {
      /* mark both the variable identifier and the parameter sweep
	 variable to be actually variables */
      val->var = 1;
      value->var = 1;
      found++;
    }
    /* 2. find analysis in parameter sweeps */
    if ((val = checker_find_variable ("SW", "Sim", value->ident))) {
      found++;
    }
    /* 3. find substrate in microstrip components */
    if ((val = checker_find_variable ("MLIN", "Subst", value->ident))) {
      value->subst = 1;
      found++;
    }
    /* 4. find microstrip model in microstrip components */
    if ((val = checker_find_variable ("MLIN", "Model", value->ident))) {
      found++;
    }
    /* TODO: find variable in equations */
    if (!found) {
      logprint (LOG_ERROR, "checker error, no such variable `%s'\n", 
		value->ident);
      return 0;
    }
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

/* The following function returns the number of circuit instances
   requiring a DC analysis (being nonlinear) in the list of definitions. */
static int checker_count_nonlinearities (void) {
  struct definition_t * def;
  int count = 0;
  for (def = definition_root; def != NULL; def = def->next) {
    if (def->nonlinear != 0) count++;
  }
  return count;
}

/* This function returns the number of action definitions with the
   given instance name. */
static int checker_count_action (char * instance) {
  struct definition_t * def;
  int count = 0;
  for (def = definition_root; def != NULL; def = def->next) {
    if (def->action == 1 && !strcmp (def->instance, instance))
      count++;
  }
  return count;
}

/* The function returns the appropriate value for a given key within
   the given netlist definition if the value is a reference (a
   string).  If there is no such key value pair the function returns
   NULL. */
static struct value_t * checker_find_reference (struct definition_t * def,
						char * key) {
  struct pair_t * pair;
  for (pair = def->pairs; pair != NULL; pair = pair->next) {
    if (!strcmp (pair->key, key))
      if (pair->value->ident != NULL)
	return pair->value;
  }
  return NULL;
}

/* This (recursive) function detects any kind of cyclic definitions of
   parameter sweeps for the given instance name.  The string list
   argument is used to pass the dependencies.  The function returns
   zero if the parameter sweep in non-cyclic. */
static int checker_validate_para_cycles (char * instance, strlist * deps) {
  struct definition_t * def;
  int errors = 0;
  struct value_t * val;
  for (def = definition_root; def != NULL; def = def->next) {
    /* find the appropriate definition for the given instance */
    if (def->action == 1 && !strcmp (def->instance, instance)) {
      /* emit error message if the instance is already in the dependencies */
      if (deps->contains (instance)) {
	logprint (LOG_ERROR, "checker error, cyclic definition of `%s' "
		  "detected, involves: %s\n", instance, deps->toString ());
	return ++errors;
      }
      deps->append (instance);
      /* recurse into parameter sweeps */
      if (!strcmp (def->type, "SW")) {
	if ((val = checker_find_reference (def, "Sim")) != NULL) {
	  return checker_validate_para_cycles (val->ident, deps);
	}
      }
    }
  }
  return errors;
}

/* This function validates each parameter sweep within the list of
   definitions and return non-zero on errors.  Emits appropriate error
   messages. */
static int checker_validate_para (void) {
  struct definition_t * def;
  int errors = 0;
  struct value_t * val;
  for (def = definition_root; def != NULL; def = def->next) {
    /* find parameter sweep */
    if (def->action == 1 && !strcmp (def->type, "SW")) {
      /* the 'Sim' property must be an identifier */
      if ((val = checker_find_reference (def, "Sim")) == NULL) {
	logprint (LOG_ERROR, "checker error, not a valid `Sim' property "
		  "found in `%s:%s'\n", def->type, def->instance);
	errors++;
      }
      else {
	/* check for self-referring sweeps */
	if (!strcmp (def->instance, val->ident)) {
	  logprint (LOG_ERROR, "checker error, definition `%s:%s' refers to "
		    "itself\n", def->type, def->instance);
	  errors++;
	}
	/* look for the referred analysis action definition */
	if (checker_count_action (val->ident) != 1) {
	  logprint (LOG_ERROR, "checker error, no such action `%s' found "
		    "as referred in `%s:%s'\n", val->ident, def->type,
		    def->instance);
	  errors++;
	}
	/* finally detect cyclic definitions */
	strlist * deps = new strlist ();
	errors += checker_validate_para_cycles (val->ident, deps);
	delete deps;
      }
    }
  } 
  return errors;
}

/* This function checks the actions to be taken in the netlist.  It
   returns zero on success, non-zero otherwise. */
static int checker_validate_actions (void) {
  int a, c, n, errors = 0;
  if ((n = checker_count_definitions (NULL, 1)) < 1) {
    logprint (LOG_ERROR, "checker error, no actions .XX defined\n");
    errors++;
  }
  else {
    // check requirements for s-parameter analysis
    if ((a = checker_count_definitions ("SP", 1)) >= 1) {
      if ((n = checker_count_definitions ("Pac", 0)) < 1) {
	logprint (LOG_ERROR, "checker error, %d `Pac' definitions found, at "
		  "least 1 required\n", n);
	errors++;
      }
    }
    // check dc-analysis requirements
    c = checker_count_nonlinearities ();
    n = checker_count_definitions ("DC", 1);
    if (n > 1) {
      logprint (LOG_ERROR, "checker error, the .DC action is defined %dx, "
		"single or none required\n", n);
      errors++;
    }
    if (a >=1 && c >= 1 && n < 1) {
      logprint (LOG_ERROR, "checker error, a .DC action is required for this "
		"circuit definition\n");
      errors++;
    }
  }
  errors += checker_validate_para ();
  return errors;
}

// List of available microstrip components.
static char * strip_available[] = {
  "MLIN", NULL };

// List of available microstrip models.
static char * strip_Model_available[] = {
  "Kirschning", "Kobayashi", "Yamashita", NULL };

/* This function checks the validity of each microstrip component and
   its substrate and model references.  It returns zero on success,
   emit error messages if necessary and returns non-zero on errors. */
static int checker_validate_strips (void) {
  struct definition_t * def;
  int errors = 0;
  struct value_t * val;
  for (def = definition_root; def != NULL; def = def->next) {
    if (!def->action) {
      /* find microstrip components */
      for (int i = 0; strip_available[i] != NULL; i++) {
	if (!strcmp (strip_available[i], def->type)) {
	  /* check validity of 'Subst' property */
	  if ((val = checker_find_reference (def, "Subst")) == NULL) {
	    logprint (LOG_ERROR, "checker error, not a valid `Subst' property "
		      "found in `%s:%s'\n", def->type, def->instance);
	    errors++;
	  }
	  else {
	    if (checker_count_definition ("SUBST", val->ident) != 1) {
	      logprint (LOG_ERROR, "checker error, no such substrate `%s' "
			"found as specified in `%s:%s'\n", val->ident,
			def->type, def->instance);
	      errors++;
	    }
	  }
	  /* check validity of 'Model' property */
	  if ((val = checker_find_reference (def, "Model")) == NULL) {
	    logprint (LOG_ERROR, "checker error, not a valid `Model' property "
		      "found in `%s:%s'\n", def->type, def->instance);
	    errors++;
	  }
	  else {
	    int found = 0;
	    for (int n = 0; strip_Model_available[n] != NULL; n++) {
	      if (!strcmp (strip_Model_available[n], val->ident))
		found++;
	    }
	    if (!found) {
	      logprint (LOG_ERROR, "checker error, `%s' is not a valid "
			"`Model' property as used in `%s:%s'\n", val->ident,
			def->type, def->instance);
	      errors++;
	    }
	  }
	}
      }
    }
  }
  return errors;
}

/* This function should be called after the netlist and the equation
   list have been checked.  It verifies that parameter sweep
   definitions and equation variable identifiers are unique.  The
   function returns zero on success and non-zero otherwise. */
int netlist_checker_variables (void) {
  struct definition_t * def;
  int errors = 0, pos;
  struct value_t * para, * ref;
  strlist * eqnvars = equation_variables ();
  strlist * instances = new strlist ();
  strlist * vars = new strlist ();
  strlist * refs = new strlist ();
  // go through list of netlist definitions
  for (def = definition_root; def != NULL; def = def->next) {
    // find parameters sweeps
    if (def->action == 1 && !strcmp (def->type, "SW")) {
      para = checker_find_reference (def, "Param");
      ref = checker_find_reference (def, "Sim");
      if (para != NULL && ref != NULL) {
	// check whether sweep variable collides with equations
	if (eqnvars && eqnvars->contains (para->ident)) {
	  logprint (LOG_ERROR, "checker error, equation variable `%s' "
		    "already defined by `%s:%s'\n", para->ident,
		    def->type, def->instance);
	  errors++;
	}
	// check for duplicate parameter names in parameter sweeps, but
	// allow them in same order sweeps
	if ((pos = vars->index (para->ident)) != -1) {
	  if (strcmp (ref->ident, refs->get (pos))) {
	    logprint (LOG_ERROR, "checker error, variable `%s' in `%s:%s' "
		      "already defined by `%s:%s'\n", para->ident, def->type,
		      def->instance, def->type, instances->get (pos));
	    errors++;
	  }
	}
	// collect parameter sweep variables for the above check
	instances->add (def->instance);
	vars->add (para->ident);
	refs->add (ref->ident);
      }
    }
  }
  delete eqnvars;
  delete refs;
  delete vars;
  delete instances;
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
      /* mark nonlinear circuit definitions */
      def->nonlinear = available->nonlinear;
      /* mark substrate definitions */
      def->substrate = available->substrate;
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
  errors += checker_validate_strips ();
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
