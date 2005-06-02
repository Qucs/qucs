/*
 * qucs_producer.cpp - the Qucs netlist producer
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
 * the Free Software Foundation, Inc., 51 Franklin Street - Fifth Floor,
 * Boston, MA 02110-1301, USA.  
 *
 * $Id: qucs_producer.cpp,v 1.9 2005-06-02 18:17:56 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "netdefs.h"
#include "check_spice.h"

/* Global variables. */
FILE * qucs_out = NULL;
int    qucs_actions = 1;
char * qucs_gnd = "gnd";
struct node_t * qucs_nodes = NULL;

/* Looks through the given list if there is such a node already in the
   list and returns non-zero if so, otherwise the function returns
   zero. */
int qucs_find_node (struct node_t * root, char * node) {
  for (struct node_t * n = root; n; n = n->next) {
    if (!strcmp (node, n->node)) return 1;
  }
  return 0;
}

/* Adds the given node list to the overall node list without
   duplicating the nodes. */
void qucs_add_nodes (struct node_t * node) {
  struct node_t * n;
  while (node) {
    if (!qucs_find_node (qucs_nodes, node->node)) {
      n = create_node ();
      n->node = strdup (node->node);
      n->next = qucs_nodes;
      qucs_nodes = n;
    }
    node = node->next;
  }
}

/* Collects all nodes within the given definition root. */
static void qucs_collect_nodes (struct definition_t * root) {
  while (root) {
    qucs_add_nodes (root->nodes);
    root = root->next;
  }
}

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

/* Goes recursively through the netlist and applies additional
   reference node modifications to subcircuits and their instances. */
static void netlist_fix_reference (struct definition_t * root) {
  struct definition_t * def;
  struct node_t * n;

  // go through definitions
  for (def = root; def != NULL; def = def->next) {
    if (!strcmp (def->type, "Sub")) { // subcircuit instances
      n = create_node ();
      n->node = strdup (qucs_gnd);
      n->next = def->nodes;
      def->nodes = n;
    }
    else if (def->sub != NULL) {     // subcircuit definitions
      n = create_node ();
      n->node = strdup (qucs_gnd);
      n->next = def->nodes;
      def->nodes = n;
      netlist_fix_reference (def->sub);
    }
  }
}

/* Applies reference node modifications if necessary. */
static void netlist_reference_ground (void) {
  struct definition_t * def;

  // return if nothing todo
  if (qucs_gnd && !strcmp (qucs_gnd, "gnd")) return;

  netlist_fix_reference (definition_root);
  // go through subcircuits
  for (def = subcircuit_root; def != NULL; def = def->next) {
    netlist_fix_reference (def->sub);
  }
}

/* Prints the overall netlist representation. */
static void netlist_list (void) {
  struct node_t * n;
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
  /* Print overall (toplevel only) node list. */
  qucs_collect_nodes (definition_root);
  fprintf (qucs_out, "\n### TOPLEVEL NODELIST BEGIN\n");
  for (n = qucs_nodes; n; n = n->next) {
    fprintf (qucs_out, "# %s\n", n->node);
  }
  fprintf (qucs_out, "### TOPLEVEL NODELIST END\n");
  /* Print potential external node list. */
  fprintf (qucs_out, "\n### SPICE OUTPUT NODELIST BEGIN\n");
  for (n = spice_nodes; n; n = n->next) {
    fprintf (qucs_out, "# %s\n", n->node);
  }
  fprintf (qucs_out, "### SPICE OUTPUT NODELIST END\n");
}

/* This function is the overall Qucs netlist producer. */
void qucs_producer (void) {
  if (qucs_out != NULL) {
    netlist_reference_ground ();
    netlist_list ();
  }
}

// Structure defining Qucs schematic entries of device models.
struct device_t {
  char * ntype;     // netlist type
  char * ltype;     // schematic type
  int nodes;        // number of nodes
  char * props[64]; // list of properties in schematic order
  char * symbol;    // symbol text
  char * coords;    // coordinates and other text
  char * ports;     // subcircuit ports
}
qucs_devices[] = {
  /* diode */
  { "Diode", "Diode", 2,
    {"Is", "N", "Cj0", "M", "Vj", "Fc", "Cp", "Isr", "Nr", "Rs", "Tt", "Temp",
     "Kf", "Af", "Ffe", "Bv", "Ibv", NULL },
    "  <.PortSym -30 0 1>\n"
    "  <.PortSym 30 0 2>\n"
    "  <Line -10 -10 0 20 #000080 2 1>\n"
    "  <Line -10 0 20 -10 #000080 2 1>\n"
    "  <Line -10 0 20 10 #000080 2 1>\n"
    "  <Line 10 0 20 0 #000080 2 1>\n"
    "  <Line 10 -10 0 20 #000080 2 1>\n"
    "  <Line -30 0 20 0 #000080 2 1>\n"
    "  <.ID -30 24 %s_>\n",
    "1 130 60 -26 13 0 0",
    "  <Port P1 1 100 60 -23 -44 1 0 \"1\" 1>\n"
    "  <Port P2 1 160 60 4 -44 0 2 \"2\" 1>\n"
  },
  /* bipolar transistor */
  { "BJT", "BJT", 4,
    { "Type", "Is", "Nf", "Nr", "Ikf", "Ikr", "Vaf", "Var", "Ise", "Ne", "Isc",
      "Nc", "Bf", "Br", "Rbm", "Irb", "Rc", "Re", "Rb", "Cje", "Vje", "Mje",
      "Cjc", "Vjc", "Mjc", "Xcjc", "Cjs", "Vjs", "Mjs", "Fc", "Tf", "Xtf",
      "Vtf", "Itf", "Tr", "Temp", "Kf", "Af", "Ffe", "Kb", "Ab", "Fb", "Ptf",
      NULL },
    "  <.PortSym -50 0 1>\n"
    "  <.PortSym 0 -50 2>\n"
    "  <.PortSym 0 50 3>\n"
    "  <.PortSym 30 0 4>\n"
    "  <Line 0 -20 0 -30 #000080 2 1>\n"
    "  <Line -50 0 30 0 #000080 2 1>\n"
    "  <Line 0 50 0 -30 #000080 2 1>\n"
    "  <Line -20 -10 20 -10 #000080 2 1>\n"
    "  <Line -20 10 20 10 #000080 2 1>\n"
    "  <Line -20 -20 0 40 #000080 2 1>\n"
    "  <Line 30 0 -30 0 #000080 2 1>\n"
    "  <Line 0 -10 0 20 #000080 2 1>\n"
    "  <Ellipse -40 -30 60 60 #000080 0 1 #c0c0c0 1 0>\n"
    "  <.ID 13 28 %s_>\n",
    "1 170 160 92 -49 0 0",
    "  <Port P2 1 170 130 -51 -23 0 3 \"2\" 1>\n"
    "  <Port P1 1 140 160 -23 12 0 0 \"1\" 1>\n"
    "  <Port P3 1 170 190 12 4 0 1 \"3\" 1>\n"
    "  <Port P4 1 200 160 4 -44 0 2 \"4\" 1>\n"
  },
  /* junction FET */
  { "JFET", "JFET", 3,
    { "Type", "Vt0", "Beta", "Lambda", "Rd", "Rs", "Is", "N", "Isr", "Nr",
      "Cgs", "Cgd", "Pb", "Fc", "M", "Kf", "Af", "Ffe", "Temp", NULL },
    "  <.PortSym -50 0 1>\n"
    "  <.PortSym 0 -50 2>\n"
    "  <.PortSym 0 50 3>\n"
    "  <Line -20 -20 0 40 #000080 2 1>\n"
    "  <Line -20 -10 20 0 #000080 2 1>\n"
    "  <Line 0 -10 0 -40 #000080 2 1>\n"
    "  <Line -50 0 30 0 #000080 2 1>\n"
    "  <Line -20 10 20 0 #000080 2 1>\n"
    "  <Line 0 50 0 -40 #000080 2 1>\n"
    "  <Ellipse -40 -30 60 60 #000080 0 1 #c0c0c0 1 0>\n"
    "  <.ID 20 24 %s_>\n",
    "1 190 140 30 -49 0 0",
    "  <Port P1 1 160 140 -23 12 0 0 \"1\" 1>\n"
    "  <Port P2 1 190 110 -51 -23 0 3 \"2\" 1>\n"
    "  <Port P3 1 190 170 -33 32 0 1 \"3\" 1>\n"
  },
  /* MOSFET */
  { "MOSFET", "MOSFET", 4,
    { "Type", "Vt0", "Kp", "Gamma", "Phi", "Lambda", "Rd", "Rs", "Rg", "Is",
      "N", "W", "L", "Ld", "Tox", "Cgso", "Cgdo", "Cgbo", "Cbd", "Cbs", "Pb",
      "Mj", "Fc", "Cjsw", "Mjsw", "Tt", "Nsub", "Nss", "Tpg", "Uo", "Rsh",
      "Nrd", "Nrs", "Cj", "Js", "Ad", "As", "Pd", "Ps", "Kf", "Af", "Ffe",
      "Temp", NULL },
    "  <.PortSym -40 0 1>\n"
    "  <.PortSym 10 -50 2>\n"
    "  <.PortSym 10 50 3>\n"
    "  <.PortSym 50 0 4>\n"
    "  <Line 10 -10 0 -40 #000080 2 1>\n"
    "  <Line -40 0 30 0 #000080 2 1>\n"
    "  <Line -5 10 15 0 #000080 2 1>\n"
    "  <Line 10 50 0 -40 #000080 2 1>\n"
    "  <Line -5 0 55 0 #000080 2 1>\n"
    "  <Line -5 -20 0 40 #000080 2 1>\n"
    "  <Line -5 -10 15 0 #000080 2 1>\n"
    "  <Line -10 -15 0 30 #000080 2 1>\n"
    "  <Ellipse -30 -30 60 60 #000080 0 1 #c0c0c0 1 0>\n"
    "  <.ID 30 34 %s_>\n",
    "1 180 170 73 -51 0 0",
    "  <Port P1 1 150 170 -23 12 0 0 \"1\" 1>\n"
    "  <Port P2 1 180 140 -51 -23 0 3 \"2\" 1>\n"
    "  <Port P3 1 180 200 12 4 0 1 \"3\" 1>\n"
    "  <Port P4 1 200 170 4 -44 0 2 \"4\" 1>\n"
  },
  { NULL, NULL, 0, { NULL }, NULL, NULL, NULL }
};

/* Looks through the list of available Qucs devices.  Returns NULL if
   there is no such device. */
static struct device_t * qucslib_find_device (char * type) {
  struct device_t * dev;
  for (dev = qucs_devices; dev->ntype; dev++) {
    if (!strcmp (dev->ntype, type))
      return dev;
  }
  return NULL;
}

/* This function tries to find the given property in the key/value
   pairs of the given netlist entry and returns NULL if there is no
   such property. */
static struct pair_t *
qucslib_find_prop (struct definition_t * def, char * key) {
  struct pair_t * pair;
  for (pair = def->pairs; pair; pair = pair->next) {
    if (!strcmp (pair->key, key))
      return pair;
  }
  return NULL;
}

/* The function outputs a device library entry in the Qucs schematic
   data format. */
static void qucslib_list_device (struct definition_t * def) {
  struct device_t * dev;
  if (!(dev = qucslib_find_device (def->type))) return;
  struct pair_t * pair;
  char txt[1024];
  // print symbol definition
  fprintf (qucs_out, "<Symbol>\n");
  sprintf (txt, dev->symbol, def->instance);
  fprintf (qucs_out, txt);
  fprintf (qucs_out, "</Symbol>\n");
  // print coponent definitions
  fprintf (qucs_out, "<Components>\n");
  fprintf (qucs_out, "  <%s %s %s", dev->ltype, def->instance, dev->coords);
  for (int i = 0; dev->props[i]; i++) {
    if ((pair = qucslib_find_prop (def, dev->props[i])) != NULL) {
      fprintf (qucs_out, " \"");
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\" 1");
    }
  }
  fprintf (qucs_out, ">\n");
  // print subcircuit port definitions
  fprintf (qucs_out, dev->ports);
  fprintf (qucs_out, "</Components>\n");
}

/* This function is the overall Qucs library producer. */
void qucslib_producer (void) {
  struct definition_t * def;
  time_t t = time (NULL);
  fprintf (qucs_out, "# converted Qucs library processed at %s\n", ctime (&t));
  for (def = device_root; def; def = def->next) {
    fprintf (qucs_out, "<Device>\n");
    qucslib_list_device (def);
    fprintf (qucs_out, "</Device>\n\n");
  }
}
