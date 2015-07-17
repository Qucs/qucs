/*
 * qucs_producer.cpp - the Qucs netlist producer
 *
 * Copyright (C) 2004-2011 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"
#include "netdefs.h"
#include "check_spice.h"
#include "check_vcd.h"
#include "hash.h"

/* Global variables. */
FILE * qucs_out = NULL;
int    qucs_actions = 1;
const char * qucs_gnd = "gnd";

qucs::hash<struct node_t> qucs_nodes;

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
  while (node) {
    if (!qucs_nodes.get (node->node))
      qucs_nodes.put (node->node, node);
    node = node->next;
  }
}

/* Deletes the given node list. */
void qucs_free_nodes (struct node_t * node) {
  struct node_t * n;
  for ( ; node; node = n) {
    n = node->next;
    free (node->node);
    free (node);
  }
}

/* The function deletes the collected qucs nodes. */
static void qucs_delete_nodes (void) {
  qucs_nodes.clear ();
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
  if (value == NULL)
    fprintf (qucs_out, "[]");
  else if (value->ident)
    fprintf (qucs_out, "%s", value->ident);
  else if (value->next) {
    fprintf (qucs_out, "[");
    for (; value != NULL; value = value->next)
      fprintf (qucs_out, "%g%s", value->value, value->next ? ";" : "");
    fprintf (qucs_out, "]");
  } else {
    fprintf (qucs_out, "%g", value->value);
    if (value->scale)
      fprintf (qucs_out, "%s", value->scale);
    if (value->unit)
      fprintf (qucs_out, "%s", value->unit);
  }
}

/* Returns a static string for an instance identifier usable in
   netlists based upon the given identifier. */
static char * netlist_instance (const char * instance) {
  static char ret[256];
  sprintf (ret, "%s%s", isdigit (instance[0]) ? "X" : "", instance);
  return ret;
}

/* Prints a single definition on a single line. */
static void netlist_list_def (struct definition_t * def, const char * prefix) {
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
	     def->type, netlist_instance (def->instance));
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
static void netlist_lister (struct definition_t * root, const char * prefix) {
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

/* Look for a single subcircuit definition in a netlist. */
static struct definition_t *
netlist_get_single_subcircuit (struct definition_t * root) {
  int count = 0;
  struct definition_t * ret = NULL;
  for (struct definition_t * def = root; def != NULL; def = def->next) {
    if (!def->action) {
      count++;
    } else if (def->sub) {
      count++;
      ret = def;
    }
  }
  return (count == 1) ? ret : NULL;
}

/* Prints the overall netlist representation. */
static void netlist_list (void) {
  struct node_t * n;
  struct definition_t * def;
  time_t t = time (NULL);
  fprintf (qucs_out, "# converted Qucs netlist processed at %s\n", ctime (&t));
  if (spice_title != NULL) {
    fprintf (qucs_out, "#\n# %s#\n\n", spice_title);
  }
  netlist_lister (definition_root, "");
  for (def = subcircuit_root; def != NULL; def = def->next) {
    fprintf (qucs_out, ".Def:%s\n", netlist_instance (def->instance));
    netlist_lister (def->sub, "  ");
    fprintf (qucs_out, ".Def:End\n");
  }
  /* Instantiate subcircuit if there is just a single definition. */
  if (definition_root != NULL && subcircuit_root == NULL &&
      (def = netlist_get_single_subcircuit (definition_root)) != NULL) {
    fprintf (qucs_out, "\n# no instance of subcircuit \"%s\" found, "
	     "creating it\n", netlist_instance (def->instance));
    fprintf (qucs_out, "Sub:X1");
    for (n = def->nodes; n; n = n->next) fprintf (qucs_out, " %s", n->node);
    fprintf (qucs_out, " Type=\"%s\"\n", netlist_instance (def->instance));
  }
  /* Print overall (toplevel only) node list. */
  qucs_collect_nodes (definition_root);
  fprintf (qucs_out, "\n### TOPLEVEL NODELIST BEGIN\n");
  for (qucs::hashiterator<struct node_t> it (qucs_nodes); *it; ++it) {
    n = it.currentVal ();
    fprintf (qucs_out, "# %s\n", n->node);
  }
  qucs_delete_nodes ();
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
  const char * ntype;      // netlist type
  const char * ltype;      // schematic type
  const char * stype;      // spice type
  int nodes;               // number of nodes
  const char * props[741]; // list of properties in schematic order
  const char * symbol;     // symbol text
  const char * coords;     // coordinates and other text
  const char * ports;      // subcircuit ports
}
qucs_devices[] = {
  /* diode */
  { "Diode", "Diode", "D", 2,
    {"Is", "N", "Cj0", "M", "Vj", "Fc", "Cp", "Isr", "Nr", "Rs", "Tt", "Ikf",
     "Kf", "Af", "Ffe", "Bv", "Ibv",  "Temp", "Xti", "Eg", "Tbv", "Trs",
     "Ttt1", "Ttt2", "Tm1", "Tm2", "Tnom", "Area", NULL },
    NULL,
    "1 0 0 -26 13 0 0",
    NULL
  },
  /* bipolar transistor */
  { "BJT", "_BJT", "Q", 4,
    { "Type", "Is", "Nf", "Nr", "Ikf", "Ikr", "Vaf", "Var", "Ise", "Ne", "Isc",
      "Nc", "Bf", "Br", "Rbm", "Irb", "Rc", "Re", "Rb", "Cje", "Vje", "Mje",
      "Cjc", "Vjc", "Mjc", "Xcjc", "Cjs", "Vjs", "Mjs", "Fc", "Tf", "Xtf",
      "Vtf", "Itf", "Tr", "Temp", "Kf", "Af", "Ffe", "Kb", "Ab", "Fb", "Ptf",
      "Xtb", "Xti", "Eg", "Tnom", "Area", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l0 bipolar transistor */
  { "hic0_full", "hic0_full", "Q", 5,
    { "Type", "is", "mcf", "mcr", "vef", "iqf", "iqr", "iqfh", "tfh", "ibes",
      "mbe", "ires", "mre", "ibcs", "mbc", "cje0", "vde", "ze", "aje", "t0",
      "dt0h", "tbvl", "tef0", "gte", "thcs", "ahc", "tr", "rci0", "vlim",
      "vpt", "vces", "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx",
      "vptcx", "fbc", "rbi0", "vr0e", "vr0c", "fgeo", "rbx", "rcx", "re",
      "itss", "msf", "iscs", "msc", "cjs0", "vds", "zs", "vpts", "cbcpar",
      "cbepar", "eavl", "kavl", "kf", "af", "vgb", "vge", "vgc", "vgs",
      "f1vg", "f2vg", "alt0", "kt0", "zetact", "zetabet", "zetaci", "alvs",
      "alces", "zetarbi", "zetarbx", "zetarcx", "zetare", "alkav", "aleav",
      "flsh", "rth", "cth", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l0 v1.2 bipolar transistor */
  { "hicumL0V1p2", "hicumL0V1p2", "Q", 5,
    { "Type", "is", "mcf", "mcr", "vef", "ver", "iqf", "fiqf", "iqr", "iqfh",
      "tfh", "ahq", "ibes", "mbe", "ires", "mre", "ibcs", "mbc", "cje0", "vde",
      "ze", "aje", "vdedc", "zedc", "ajedc", "t0", "dt0h", "tbvl", "tef0",
      "gte", "thcs", "ahc", "tr", "rci0", "vlim", "vpt", "vces", "cjci0",
      "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx", "fbc", "rbi0",
      "vr0e", "vr0c", "fgeo", "rbx", "rcx", "re", "itss", "msf", "iscs", "msc",
      "cjs0", "vds", "zs", "vpts", "cbcpar", "cbepar", "eavl", "kavl", "kf",
      "af", "vgb", "vge", "vgc", "vgs", "f1vg", "f2vg", "alt0", "kt0",
      "zetact", "zetabet", "zetaci", "alvs", "alces", "zetarbi", "zetarbx",
      "zetarcx", "zetare", "zetaiqf", "alkav", "aleav", "zetarth", "flsh",
      "rth", "cth", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l0 v1.2g bipolar transistor */
  { "hicumL0V1p2g", "hicumL0V1p2g", "Q", 5,
    { "Type", "is", "mcf", "mcr", "vef", "ver", "iqf", "fiqf", "iqr", "iqfh",
      "iqfe", "ahq", "ibes", "mbe", "ires", "mre", "ibcs", "mbc", "cje0",
      "vde", "ze", "aje", "vdedc", "zedc", "ajedc", "t0", "dt0h", "tbvl",
      "tef0", "gte", "thcs", "ahc", "tr", "rci0", "vlim", "vpt", "vces",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx", "fbc",
      "rbi0", "vr0e", "vr0c", "fgeo", "rbx", "rcx", "re", "itss", "msf",
      "iscs", "msc", "cjs0", "vds", "zs", "vpts", "cbcpar", "cbepar", "eavl",
      "kavl", "kf", "af", "vgb", "vge", "vgc", "vgs", "f1vg", "f2vg", "alt0",
      "kt0", "zetact", "zetabet", "zetaci", "alvs", "alces", "zetarbi",
      "zetarbx", "zetarcx", "zetare", "zetaiqf", "alkav", "aleav", "flsh",
      "rth", "zetarth", "cth", "tnom", "dt", "delte", "deltc", "zetaver",
      "zetavef", "ibhrec", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l0 v1.3 bipolar transistor */
  { "hicumL0V1p3", "hicumL0V1p3", "Q", 5,
    { "Type", "is", "it_mod", "mcf", "mcr", "vef", "ver", "aver", "iqf",
      "fiqf", "iqr", "iqfh", "tfh", "ahq", "ibes", "mbe", "ires", "mre",
      "ibcs", "mbc", "cje0", "vde", "ze", "aje", "vdedc", "zedc", "ajedc",
      "t0", "dt0h", "tbvl", "tef0", "gte", "thcs", "ahc", "tr", "rci0",
      "vlim", "vpt", "vces", "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx",
      "zcx", "vptcx", "fbc", "rbi0", "vr0e", "vr0c", "fgeo", "rbx", "rcx",
      "re", "itss", "msf", "iscs", "msc", "cjs0", "vds", "zs", "vpts",
      "cbcpar", "cbepar", "eavl", "kavl", "kf", "af", "vgb", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "alt0", "kt0", "zetact", "zetabet", "zetaci",
      "alvs", "alces", "zetarbi", "zetarbx", "zetarcx", "zetare", "zetaiqf",
      "alkav", "aleav", "zetarth", "tef_temp", "zetaver", "zetavgbe", "dvgbe",
      "aliqfh", "kiqfh", "flsh", "rth", "cth", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.22 bipolar transistor */
  { "hic2_full", "hic2_full", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.1 bipolar transistor */
  { "hicumL2V2p1", "hicumL2V2p1", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis",
      "mbei", "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "favl", "qavl",
      "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi", "fqi", "re",
      "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu", "cjei0",
      "vdei", "zei", "aljei", "cjep0", "vdep", "zep", "aljep", "cjci0",
      "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx", "fbc", "cjs0",
      "vds", "zs", "vpts", "t0", "dt0h", "tbvl", "tef0", "gtfe", "thcs",
      "alhc", "fthc", "rci0", "vlim", "vces", "vpt", "tr", "ceox", "ccox",
      "alqf", "alit", "kf", "af", "krbi", "latb", "latl", "vgb", "alt0",
      "kt0", "zetaci", "zetacx", "alvs", "alces", "zetarbi", "zetarbx",
      "zetarcx", "zetare", "alb", "rth", "cth", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.23 bipolar transistor */
  { "hicumL2V2p23", "hicumL2V2p23", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* hicum/l2 v2.24 bipolar transistor */
  { "hicumL2V2p24", "hicumL2V2p24", "Q", 5,
    { "c10", "qp0", "ich", "hfe", "hfc", "hjei", "hjci", "ibeis", "mbei",
      "ireis", "mrei", "ibeps", "mbep", "ireps", "mrep", "mcf", "tbhrec",
      "ibcis", "mbci", "ibcxs", "mbcx", "ibets", "abet", "tunode", "favl",
      "qavl", "alfav", "alqav", "rbi0", "rbx", "fgeo", "fdqr0", "fcrbi",
      "fqi", "re", "rcx", "itss", "msf", "iscs", "msc", "tsf", "rsu", "csu",
      "cjei0", "vdei", "zei", "ajei", "cjep0", "vdep", "zep", "ajep",
      "cjci0", "vdci", "zci", "vptci", "cjcx0", "vdcx", "zcx", "vptcx",
      "fbcpar", "fbepar", "cjs0", "vds", "zs", "vpts", "t0", "dt0h", "tbvl",
      "tef0", "gtfe", "thcs", "ahc", "fthc", "rci0", "vlim", "vces", "vpt",
      "tr", "cbepar", "cbcpar", "alqf", "alit", "flnqs", "kf", "af", "cfbe",
      "latb", "latl", "vgb", "alt0", "kt0", "zetaci", "alvs", "alces",
      "zetarbi", "zetarbx", "zetarcx", "zetare", "zetacx", "vge", "vgc",
      "vgs", "f1vg", "f2vg", "zetact", "zetabet", "alb", "flsh", "rth", "cth",
      "flcomp", "tnom", "dt", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* junction FET */
  { "JFET", "JFET", "J", 3,
    { "Type", "Vt0", "Beta", "Lambda", "Rd", "Rs", "Is", "N", "Isr", "Nr",
      "Cgs", "Cgd", "Pb", "Fc", "M", "Kf", "Af", "Ffe", "Temp", "Xti",
      "Vt0tc", "Betatce", "Tnom", "Area", NULL },
    NULL,
    "1 190 140 30 -49 0 0",
    NULL
  },
  /* MOSFET */
  { "MOSFET", "_MOSFET", "M", 4,
    { "Type", "Vt0", "Kp", "Gamma", "Phi", "Lambda", "Rd", "Rs", "Rg", "Is",
      "N", "W", "L", "Ld", "Tox", "Cgso", "Cgdo", "Cgbo", "Cbd", "Cbs", "Pb",
      "Mj", "Fc", "Cjsw", "Mjsw", "Tt", "Nsub", "Nss", "Tpg", "Uo", "Rsh",
      "Nrd", "Nrs", "Cj", "Js", "Ad", "As", "Pd", "Ps", "Kf", "Af", "Ffe",
      "Temp", "Tnom", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  },
  /* bsim6.1.0 NMOS transistor*/
  { "bsim6v10nMOS", "bsim6v10nMOS", "M", 4,
    { "CVMOD", "GEOMOD", "RGEOMOD", "COVMOD", "RDSMOD", "GIDLMOD", "RGATEMOD",
      "RBODYMOD", "IGCMOD", "IGBMOD", "TNOIMOD", "SHMOD", "L", "LLONG", "LMLT",
      "WMLT", "XL", "W", "WWIDE", "XW", "LINT", "LL", "LW", "LWL", "LLN", "LWN",
      "WINT", "WL", "WW", "WWL", "WLN", "WWN", "DLC", "LLC", "LWC", "LWLC", "DWC",
      "WLC", "WWC", "WWLC", "TOXE", "TOXP", "DTOX", "NF", "NDEP", "NDEPL1",
      "NDEPLEXP1", "NDEPL2", "NDEPLEXP2", "NDEPW", "NDEPWEXP", "NDEPWL", "NDEPWLEXP",
      "LNDEP", "WNDEP", "PNDEP", "NDEPCV", "NDEPCVL1", "NDEPCVLEXP1", "NDEPCVL2",
      "NDEPCVLEXP2", "NDEPCVW", "NDEPCVWEXP", "NDEPCVWL", "NDEPCVWLEXP", "LNDEPCV",
      "WNDEPCV", "PNDEPCV", "NGATE", "LNGATE", "WNGATE", "PNGATE", "EASUB", "NI0SUB",
      "BG0SUB", "EPSRSUB", "EPSROX", "XJ", "LXJ", "WXJ", "PXJ", "VFB", "LVFB", "WVFB",
      "PVFB", "VFBCV", "LVFBCV", "WVFBCV", "PVFBCV", "VFBCVL", "VFBCVLEXP", "VFBCVW",
      "VFBCVWEXP", "VFBCVWL", "VFBCVWLEXP", "PERMOD", "AS", "AD", "PS", "PD", "DWJ",
      "VFBSDOFF", "NRS", "NRD", "MINZ", "NSD", "LNSD", "WNSD", "PNSD", "DVTP0",
      "LDVTP0", "WDVTP0", "PDVTP0", "DVTP1", "LDVTP1", "WDVTP1", "PDVTP1", "DVTP2",
      "LDVTP2", "WDVTP2", "PDVTP2", "DVTP3", "LDVTP3", "WDVTP3", "PDVTP3", "DVTP4",
      "LDVTP4", "WDVTP4", "PDVTP4", "DVTP5", "LDVTP5", "WDVTP5", "PDVTP5", "PHIN",
      "LPHIN", "WPHIN", "PPHIN", "ETA0", "LETA0", "WETA0", "PETA0", "DSUB", "ETAB",
      "ETABEXP", "LETAB", "WETAB", "PETAB", "K2", "K2L", "K2LEXP", "K2W", "K2WEXP",
      "K2WL", "K2WLEXP", "LK2", "WK2", "PK2", "ADOS", "BDOS", "QM0", "ETAQM", "CIT",
      "LCIT", "WCIT", "PCIT", "NFACTOR", "NFACTORL", "NFACTORLEXP", "NFACTORW",
      "NFACTORWEXP", "NFACTORWL", "NFACTORWLEXP", "LNFACTOR", "WNFACTOR", "PNFACTOR",
      "CDSCD", "CDSCDL", "CDSCDLEXP", "LCDSCD", "WCDSCD", "PCDSCD", "CDSCB", "CDSCBL",
      "CDSCBLEXP", "LCDSCB", "WCDSCB", "PCDSCB", "VSAT", "LVSAT", "WVSAT", "PVSAT",
      "VSATL", "VSATLEXP", "VSATW", "VSATWEXP", "VSATWL", "VSATWLEXP", "DELTA",
      "LDELTA", "WDELTA", "PDELTA", "DELTAL", "DELTALEXP", "VSATCV", "LVSATCV",
      "WVSATCV", "PVSATCV", "VSATCVL", "VSATCVLEXP", "VSATCVW", "VSATCVWEXP",
      "VSATCVWL", "VSATCVWLEXP", "U0", "U0L", "U0LEXP", "LU0", "WU0", "PU0", "ETAMOB",
      "UA", "UAL", "UALEXP", "UAW", "UAWEXP", "UAWL", "UAWLEXP", "LUA", "WUA", "PUA",
      "EU", "LEU", "WEU", "PEU", "EUL", "EULEXP", "EUW", "EUWEXP", "EUWL", "EUWLEXP",
      "UD", "UDL", "UDLEXP", "LUD", "WUD", "PUD", "UCS", "LUCS", "WUCS", "PUCS", "UC",
      "UCL", "UCLEXP", "UCW", "UCWEXP", "UCWL", "UCWLEXP", "LUC", "WUC", "PUC",
      "PCLM", "PCLML", "PCLMLEXP", "LPCLM", "WPCLM", "PPCLM", "PCLMG", "PCLMCV",
      "PCLMCVL", "PCLMCVLEXP", "LPCLMCV", "WPCLMCV", "PPCLMCV", "PSCBE1", "LPSCBE1",
      "WPSCBE1", "PPSCBE1", "PSCBE2", "LPSCBE2", "WPSCBE2", "PPSCBE2", "PDITS",
      "LPDITS", "WPDITS", "PPDITS", "PDITSL", "PDITSD", "LPDITSD", "WPDITSD",
      "PPDITSD", "RSH", "PRWG", "LPRWG", "WPRWG", "PPRWG", "PRWB", "LPRWB", "WPRWB",
      "PPRWB", "PRWBL", "PRWBLEXP", "WR", "LWR", "WWR", "PWR", "RSWMIN", "LRSWMIN",
      "WRSWMIN", "PRSWMIN", "RSW", "LRSW", "WRSW", "PRSW", "RSWL", "RSWLEXP",
      "RDWMIN", "LRDWMIN", "WRDWMIN", "PRDWMIN", "RDW", "LRDW", "WRDW", "PRDW",
      "RDWL", "RDWLEXP", "RDSWMIN", "LRDSWMIN", "WRDSWMIN", "PRDSWMIN", "RDSW",
      "RDSWL", "RDSWLEXP", "LRDSW", "WRDSW", "PRDSW", "PSAT", "LPSAT", "WPSAT",
      "PPSAT", "PSATL", "PSATLEXP", "PSATB", "LPSATB", "WPSATB", "PPSATB", "PSATX",
      "PTWG", "LPTWG", "WPTWG", "PPTWG", "PTWGL", "PTWGLEXP", "PDIBLC", "PDIBLCL",
      "PDIBLCLEXP", "LPDIBLC", "WPDIBLC", "PPDIBLC", "PDIBLCB", "LPDIBLCB",
      "WPDIBLCB", "PPDIBLCB", "PVAG", "LPVAG", "WPVAG", "PPVAG", "FPROUT", "FPROUTL",
      "FPROUTLEXP", "LFPROUT", "WFPROUT", "PFPROUT", "ALPHA0", "ALPHA0L",
      "ALPHA0LEXP", "LALPHA0", "WALPHA0", "PALPHA0", "BETA0", "LBETA0", "WBETA0",
      "PBETA0", "AIGBACC", "BIGBACC", "CIGBACC", "NIGBACC", "AIGBINV", "BIGBINV",
      "CIGBINV", "EIGBINV", "NIGBINV", "AIGC", "BIGC", "CIGC", "AIGS", "BIGS", "CIGS",
      "AIGD", "BIGD", "CIGD", "DLCIG", "DLCIGD", "POXEDGE", "NTOX", "TOXREF", "PIGCD",
      "AIGCL", "AIGCW", "AIGSL", "AIGSW", "AIGDL", "AIGDW", "PIGCDL", "LAIGBINV",
      "WAIGBINV", "PAIGBINV", "LBIGBINV", "WBIGBINV", "PBIGBINV", "LCIGBINV",
      "WCIGBINV", "PCIGBINV", "LEIGBINV", "WEIGBINV", "PEIGBINV", "LNIGBINV",
      "WNIGBINV", "PNIGBINV", "LAIGBACC", "WAIGBACC", "PAIGBACC", "LBIGBACC",
      "WBIGBACC", "PBIGBACC", "LCIGBACC", "WCIGBACC", "PCIGBACC", "LNIGBACC",
      "WNIGBACC", "PNIGBACC", "LAIGC", "WAIGC", "PAIGC", "LBIGC", "WBIGC", "PBIGC",
      "LCIGC", "WCIGC", "PCIGC", "LAIGS", "WAIGS", "PAIGS", "LBIGS", "WBIGS", "PBIGS",
      "LCIGS", "WCIGS", "PCIGS", "LAIGD", "WAIGD", "PAIGD", "LBIGD", "WBIGD", "PBIGD",
      "LCIGD", "WCIGD", "PCIGD", "LPOXEDGE", "WPOXEDGE", "PPOXEDGE", "LDLCIG",
      "WDLCIG", "PDLCIG", "LDLCIGD", "WDLCIGD", "PDLCIGD", "LNTOX", "WNTOX", "PNTOX",
      "AGIDL", "AGIDLL", "AGIDLW", "LAGIDL", "WAGIDL", "PAGIDL", "BGIDL", "LBGIDL",
      "WBGIDL", "PBGIDL", "CGIDL", "LCGIDL", "WCGIDL", "PCGIDL", "EGIDL", "LEGIDL",
      "WEGIDL", "PEGIDL", "AGISL", "AGISLL", "AGISLW", "LAGISL", "WAGISL", "PAGISL",
      "BGISL", "LBGISL", "WBGISL", "PBGISL", "CGISL", "LCGISL", "WCGISL", "PCGISL",
      "EGISL", "LEGISL", "WEGISL", "PEGISL", "CF", "LCF", "WCF", "PCF", "CFRCOEFF",
      "CGSO", "CGDO", "CGBO", "CGSL", "LCGSL", "WCGSL", "PCGSL", "CGDL", "LCGDL",
      "WCGDL", "PCGDL", "CKAPPAS", "LCKAPPAS", "WCKAPPAS", "PCKAPPAS", "CKAPPAD",
      "LCKAPPAD", "WCKAPPAD", "PCKAPPAD", "DMCG", "DMCI", "DMDG", "DMCGT", "CJS",
      "CJD", "CJSWS", "CJSWD", "CJSWGS", "CJSWGD", "PBS", "PBD", "PBSWS", "PBSWD",
      "PBSWGS", "PBSWGD", "MJS", "MJD", "MJSWS", "MJSWD", "MJSWGS", "MJSWGD", "JSS",
      "JSD", "JSWS", "JSWD", "JSWGS", "JSWGD", "NJS", "NJD", "IJTHSFWD", "IJTHDFWD",
      "IJTHSREV", "IJTHDREV", "BVS", "BVD", "XJBVS", "XJBVD", "JTSS", "JTSD",
      "JTSSWS", "JTSSWD", "JTSSWGS", "JTSSWGD", "JTWEFF", "NJTS", "NJTSD", "NJTSSW",
      "NJTSSWD", "NJTSSWG", "NJTSSWGD", "VTSS", "VTSD", "VTSSWS", "VTSSWD", "VTSSWGS",
      "VTSSWGD", "EF", "EM", "NOIA", "NOIB", "NOIC", "LINTNOI", "NTNOI", "BINUNIT",
      "DLBIN", "DWBIN", "TNOM", "DTEMP", "TBGASUB", "TBGBSUB", "TNFACTOR", "UTE",
      "LUTE", "WUTE", "PUTE", "UTEL", "UA1", "LUA1", "WUA1", "PUA1", "UA1L", "UC1",
      "LUC1", "WUC1", "PUC1", "UD1", "LUD1", "WUD1", "PUD1", "UD1L", "UCSTE",
      "LUCSTE", "WUCSTE", "PUCSTE", "TETA0", "PRT", "LPRT", "WPRT", "PPRT", "AT",
      "LAT", "WAT", "PAT", "ATL", "TDELTA", "PTWGT", "LPTWGT", "WPTWGT", "PPTWGT",
      "PTWGTL", "KT1", "KT1EXP", "KT1L", "LKT1", "WKT1", "PKT1", "KT2", "LKT2",
      "WKT2", "PKT2", "IIT", "LIIT", "WIIT", "PIIT", "IGT", "LIGT", "WIGT", "PIGT",
      "TGIDL", "LTGIDL", "WTGIDL", "PTGIDL", "TCJ", "TCJSW", "TCJSWG", "TPB", "TPBSW",
      "TPBSWG", "XTIS", "XTID", "XTSS", "XTSD", "XTSSWS", "XTSSWD", "XTSSWGS",
      "XTSSWGD", "TNJTS", "TNJTSD", "TNJTSSW", "TNJTSSWD", "TNJTSSWG", "TNJTSSWGD",
      "RTH0", "CTH0", "WTH0", "SA", "SB", "SD", "SAREF", "SBREF", "WLOD", "KU0",
      "KVSAT", "TKU0", "LKU0", "WKU0", "PKU0", "LLODKU0", "WLODKU0", "KVTH0",
      "LKVTH0", "WKVTH0", "PKVTH0", "LLODVTH", "WLODVTH", "STK2", "LODK2", "STETA0",
      "LODETA0", "SCA", "SCB", "SCC", "SC", "WEB", "WEC", "KVTH0WE", "LKVTH0WE",
      "WKVTH0WE", "PKVTH0WE", "K2WE", "LK2WE", "WK2WE", "PK2WE", "KU0WE", "LKU0WE",
      "WKU0WE", "PKU0WE", "SCREF", "GMIN", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  }, // bsim6v10nMOS
  /* bsim6.1.0 PMOS transistor*/
  { "bsim6v10pMOS", "bsim6v10pMOS", "M", 4,
    { "CVMOD", "GEOMOD", "RGEOMOD", "COVMOD", "RDSMOD", "GIDLMOD", "RGATEMOD",
      "RBODYMOD", "IGCMOD", "IGBMOD", "TNOIMOD", "SHMOD", "L", "LLONG", "LMLT",
      "WMLT", "XL", "W", "WWIDE", "XW", "LINT", "LL", "LW", "LWL", "LLN", "LWN",
      "WINT", "WL", "WW", "WWL", "WLN", "WWN", "DLC", "LLC", "LWC", "LWLC", "DWC",
      "WLC", "WWC", "WWLC", "TOXE", "TOXP", "DTOX", "NF", "NDEP", "NDEPL1",
      "NDEPLEXP1", "NDEPL2", "NDEPLEXP2", "NDEPW", "NDEPWEXP", "NDEPWL", "NDEPWLEXP",
      "LNDEP", "WNDEP", "PNDEP", "NDEPCV", "NDEPCVL1", "NDEPCVLEXP1", "NDEPCVL2",
      "NDEPCVLEXP2", "NDEPCVW", "NDEPCVWEXP", "NDEPCVWL", "NDEPCVWLEXP", "LNDEPCV",
      "WNDEPCV", "PNDEPCV", "NGATE", "LNGATE", "WNGATE", "PNGATE", "EASUB", "NI0SUB",
      "BG0SUB", "EPSRSUB", "EPSROX", "XJ", "LXJ", "WXJ", "PXJ", "VFB", "LVFB", "WVFB",
      "PVFB", "VFBCV", "LVFBCV", "WVFBCV", "PVFBCV", "VFBCVL", "VFBCVLEXP", "VFBCVW",
      "VFBCVWEXP", "VFBCVWL", "VFBCVWLEXP", "PERMOD", "AS", "AD", "PS", "PD", "DWJ",
      "VFBSDOFF", "NRS", "NRD", "MINZ", "NSD", "LNSD", "WNSD", "PNSD", "DVTP0",
      "LDVTP0", "WDVTP0", "PDVTP0", "DVTP1", "LDVTP1", "WDVTP1", "PDVTP1", "DVTP2",
      "LDVTP2", "WDVTP2", "PDVTP2", "DVTP3", "LDVTP3", "WDVTP3", "PDVTP3", "DVTP4",
      "LDVTP4", "WDVTP4", "PDVTP4", "DVTP5", "LDVTP5", "WDVTP5", "PDVTP5", "PHIN",
      "LPHIN", "WPHIN", "PPHIN", "ETA0", "LETA0", "WETA0", "PETA0", "DSUB", "ETAB",
      "ETABEXP", "LETAB", "WETAB", "PETAB", "K2", "K2L", "K2LEXP", "K2W", "K2WEXP",
      "K2WL", "K2WLEXP", "LK2", "WK2", "PK2", "ADOS", "BDOS", "QM0", "ETAQM", "CIT",
      "LCIT", "WCIT", "PCIT", "NFACTOR", "NFACTORL", "NFACTORLEXP", "NFACTORW",
      "NFACTORWEXP", "NFACTORWL", "NFACTORWLEXP", "LNFACTOR", "WNFACTOR", "PNFACTOR",
      "CDSCD", "CDSCDL", "CDSCDLEXP", "LCDSCD", "WCDSCD", "PCDSCD", "CDSCB", "CDSCBL",
      "CDSCBLEXP", "LCDSCB", "WCDSCB", "PCDSCB", "VSAT", "LVSAT", "WVSAT", "PVSAT",
      "VSATL", "VSATLEXP", "VSATW", "VSATWEXP", "VSATWL", "VSATWLEXP", "DELTA",
      "LDELTA", "WDELTA", "PDELTA", "DELTAL", "DELTALEXP", "VSATCV", "LVSATCV",
      "WVSATCV", "PVSATCV", "VSATCVL", "VSATCVLEXP", "VSATCVW", "VSATCVWEXP",
      "VSATCVWL", "VSATCVWLEXP", "U0", "U0L", "U0LEXP", "LU0", "WU0", "PU0", "ETAMOB",
      "UA", "UAL", "UALEXP", "UAW", "UAWEXP", "UAWL", "UAWLEXP", "LUA", "WUA", "PUA",
      "EU", "LEU", "WEU", "PEU", "EUL", "EULEXP", "EUW", "EUWEXP", "EUWL", "EUWLEXP",
      "UD", "UDL", "UDLEXP", "LUD", "WUD", "PUD", "UCS", "LUCS", "WUCS", "PUCS", "UC",
      "UCL", "UCLEXP", "UCW", "UCWEXP", "UCWL", "UCWLEXP", "LUC", "WUC", "PUC",
      "PCLM", "PCLML", "PCLMLEXP", "LPCLM", "WPCLM", "PPCLM", "PCLMG", "PCLMCV",
      "PCLMCVL", "PCLMCVLEXP", "LPCLMCV", "WPCLMCV", "PPCLMCV", "PSCBE1", "LPSCBE1",
      "WPSCBE1", "PPSCBE1", "PSCBE2", "LPSCBE2", "WPSCBE2", "PPSCBE2", "PDITS",
      "LPDITS", "WPDITS", "PPDITS", "PDITSL", "PDITSD", "LPDITSD", "WPDITSD",
      "PPDITSD", "RSH", "PRWG", "LPRWG", "WPRWG", "PPRWG", "PRWB", "LPRWB", "WPRWB",
      "PPRWB", "PRWBL", "PRWBLEXP", "WR", "LWR", "WWR", "PWR", "RSWMIN", "LRSWMIN",
      "WRSWMIN", "PRSWMIN", "RSW", "LRSW", "WRSW", "PRSW", "RSWL", "RSWLEXP",
      "RDWMIN", "LRDWMIN", "WRDWMIN", "PRDWMIN", "RDW", "LRDW", "WRDW", "PRDW",
      "RDWL", "RDWLEXP", "RDSWMIN", "LRDSWMIN", "WRDSWMIN", "PRDSWMIN", "RDSW",
      "RDSWL", "RDSWLEXP", "LRDSW", "WRDSW", "PRDSW", "PSAT", "LPSAT", "WPSAT",
      "PPSAT", "PSATL", "PSATLEXP", "PSATB", "LPSATB", "WPSATB", "PPSATB", "PSATX",
      "PTWG", "LPTWG", "WPTWG", "PPTWG", "PTWGL", "PTWGLEXP", "PDIBLC", "PDIBLCL",
      "PDIBLCLEXP", "LPDIBLC", "WPDIBLC", "PPDIBLC", "PDIBLCB", "LPDIBLCB",
      "WPDIBLCB", "PPDIBLCB", "PVAG", "LPVAG", "WPVAG", "PPVAG", "FPROUT", "FPROUTL",
      "FPROUTLEXP", "LFPROUT", "WFPROUT", "PFPROUT", "ALPHA0", "ALPHA0L",
      "ALPHA0LEXP", "LALPHA0", "WALPHA0", "PALPHA0", "BETA0", "LBETA0", "WBETA0",
      "PBETA0", "AIGBACC", "BIGBACC", "CIGBACC", "NIGBACC", "AIGBINV", "BIGBINV",
      "CIGBINV", "EIGBINV", "NIGBINV", "AIGC", "BIGC", "CIGC", "AIGS", "BIGS", "CIGS",
      "AIGD", "BIGD", "CIGD", "DLCIG", "DLCIGD", "POXEDGE", "NTOX", "TOXREF", "PIGCD",
      "AIGCL", "AIGCW", "AIGSL", "AIGSW", "AIGDL", "AIGDW", "PIGCDL", "LAIGBINV",
      "WAIGBINV", "PAIGBINV", "LBIGBINV", "WBIGBINV", "PBIGBINV", "LCIGBINV",
      "WCIGBINV", "PCIGBINV", "LEIGBINV", "WEIGBINV", "PEIGBINV", "LNIGBINV",
      "WNIGBINV", "PNIGBINV", "LAIGBACC", "WAIGBACC", "PAIGBACC", "LBIGBACC",
      "WBIGBACC", "PBIGBACC", "LCIGBACC", "WCIGBACC", "PCIGBACC", "LNIGBACC",
      "WNIGBACC", "PNIGBACC", "LAIGC", "WAIGC", "PAIGC", "LBIGC", "WBIGC", "PBIGC",
      "LCIGC", "WCIGC", "PCIGC", "LAIGS", "WAIGS", "PAIGS", "LBIGS", "WBIGS", "PBIGS",
      "LCIGS", "WCIGS", "PCIGS", "LAIGD", "WAIGD", "PAIGD", "LBIGD", "WBIGD", "PBIGD",
      "LCIGD", "WCIGD", "PCIGD", "LPOXEDGE", "WPOXEDGE", "PPOXEDGE", "LDLCIG",
      "WDLCIG", "PDLCIG", "LDLCIGD", "WDLCIGD", "PDLCIGD", "LNTOX", "WNTOX", "PNTOX",
      "AGIDL", "AGIDLL", "AGIDLW", "LAGIDL", "WAGIDL", "PAGIDL", "BGIDL", "LBGIDL",
      "WBGIDL", "PBGIDL", "CGIDL", "LCGIDL", "WCGIDL", "PCGIDL", "EGIDL", "LEGIDL",
      "WEGIDL", "PEGIDL", "AGISL", "AGISLL", "AGISLW", "LAGISL", "WAGISL", "PAGISL",
      "BGISL", "LBGISL", "WBGISL", "PBGISL", "CGISL", "LCGISL", "WCGISL", "PCGISL",
      "EGISL", "LEGISL", "WEGISL", "PEGISL", "CF", "LCF", "WCF", "PCF", "CFRCOEFF",
      "CGSO", "CGDO", "CGBO", "CGSL", "LCGSL", "WCGSL", "PCGSL", "CGDL", "LCGDL",
      "WCGDL", "PCGDL", "CKAPPAS", "LCKAPPAS", "WCKAPPAS", "PCKAPPAS", "CKAPPAD",
      "LCKAPPAD", "WCKAPPAD", "PCKAPPAD", "DMCG", "DMCI", "DMDG", "DMCGT", "CJS",
      "CJD", "CJSWS", "CJSWD", "CJSWGS", "CJSWGD", "PBS", "PBD", "PBSWS", "PBSWD",
      "PBSWGS", "PBSWGD", "MJS", "MJD", "MJSWS", "MJSWD", "MJSWGS", "MJSWGD", "JSS",
      "JSD", "JSWS", "JSWD", "JSWGS", "JSWGD", "NJS", "NJD", "IJTHSFWD", "IJTHDFWD",
      "IJTHSREV", "IJTHDREV", "BVS", "BVD", "XJBVS", "XJBVD", "JTSS", "JTSD",
      "JTSSWS", "JTSSWD", "JTSSWGS", "JTSSWGD", "JTWEFF", "NJTS", "NJTSD", "NJTSSW",
      "NJTSSWD", "NJTSSWG", "NJTSSWGD", "VTSS", "VTSD", "VTSSWS", "VTSSWD", "VTSSWGS",
      "VTSSWGD", "EF", "EM", "NOIA", "NOIB", "NOIC", "LINTNOI", "NTNOI", "BINUNIT",
      "DLBIN", "DWBIN", "TNOM", "DTEMP", "TBGASUB", "TBGBSUB", "TNFACTOR", "UTE",
      "LUTE", "WUTE", "PUTE", "UTEL", "UA1", "LUA1", "WUA1", "PUA1", "UA1L", "UC1",
      "LUC1", "WUC1", "PUC1", "UD1", "LUD1", "WUD1", "PUD1", "UD1L", "UCSTE",
      "LUCSTE", "WUCSTE", "PUCSTE", "TETA0", "PRT", "LPRT", "WPRT", "PPRT", "AT",
      "LAT", "WAT", "PAT", "ATL", "TDELTA", "PTWGT", "LPTWGT", "WPTWGT", "PPTWGT",
      "PTWGTL", "KT1", "KT1EXP", "KT1L", "LKT1", "WKT1", "PKT1", "KT2", "LKT2",
      "WKT2", "PKT2", "IIT", "LIIT", "WIIT", "PIIT", "IGT", "LIGT", "WIGT", "PIGT",
      "TGIDL", "LTGIDL", "WTGIDL", "PTGIDL", "TCJ", "TCJSW", "TCJSWG", "TPB", "TPBSW",
      "TPBSWG", "XTIS", "XTID", "XTSS", "XTSD", "XTSSWS", "XTSSWD", "XTSSWGS",
      "XTSSWGD", "TNJTS", "TNJTSD", "TNJTSSW", "TNJTSSWD", "TNJTSSWG", "TNJTSSWGD",
      "RTH0", "CTH0", "WTH0", "SA", "SB", "SD", "SAREF", "SBREF", "WLOD", "KU0",
      "KVSAT", "TKU0", "LKU0", "WKU0", "PKU0", "LLODKU0", "WLODKU0", "KVTH0",
      "LKVTH0", "WKVTH0", "PKVTH0", "LLODVTH", "WLODVTH", "STK2", "LODK2", "STETA0",
      "LODETA0", "SCA", "SCB", "SCC", "SC", "WEB", "WEC", "KVTH0WE", "LKVTH0WE",
      "WKVTH0WE", "PKVTH0WE", "K2WE", "LK2WE", "WK2WE", "PK2WE", "KU0WE", "LKU0WE",
      "WKU0WE", "PKU0WE", "SCREF", "GMIN", "Temp", NULL },
    NULL,
    "1 0 0 8 -26 0 0",
    NULL
  }, // bsim6v10pMOS
  { NULL, NULL, NULL, 0, { NULL }, NULL, NULL, NULL }
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
qucslib_find_prop (struct definition_t * def, const char * key) {
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
  
  sprintf (txt, "\n<Component %s>\n", def->instance[0] == dev->stype[0] ?
	   &def->instance[1] : def->instance);
  fprintf (qucs_out, "%s", txt);
  fprintf (qucs_out, "  <Description>\n");
  fprintf (qucs_out, "  </Description>\n");
  fprintf (qucs_out, "  <Model>\n");
  fprintf (qucs_out, "    <%s %s_ %s", dev->ltype,
	   netlist_instance (def->instance), dev->coords);
  for (int i = 0; dev->props[i]; i++) {
    if ((pair = qucslib_find_prop (def, dev->props[i])) != NULL) {
      fprintf (qucs_out, " \"");
      netlist_list_value (pair->value);
      fprintf (qucs_out, "\" 0");
    }
  }
  fprintf (qucs_out, ">\n");
  fprintf (qucs_out, "  </Model>\n");
  fprintf (qucs_out, "</Component>\n");
}

/* This function is the overall Qucs library producer. */
void qucslib_producer (void) {
  struct definition_t * def;
  fprintf (qucs_out, "<Qucs Library " PACKAGE_VERSION " \"Generic\">\n");
  for (def = device_root; def; def = def->next) {
    qucslib_list_device (def);
  }
}

/* This function is the Qucs dataset producer for VCD files. */
void qucsdata_producer_vcd (void) {
  struct dataset_variable * ds;
  struct dataset_value * dv;
  fprintf (qucs_out, "<Qucs Dataset " PACKAGE_VERSION ">\n");
  for (ds = dataset_root; ds; ds = ds->next) {
    if (!ds->output || ds->type == DATA_UNKNOWN)
      continue;
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "<indep %s %d>\n", ds->ident, ds->size);
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "<dep %s.%s %s>\n", ds->ident, ds->isreal ? "R" : "X",
	       ds->dependencies);
    for (dv = ds->values; dv; dv = dv->next) {
      fprintf (qucs_out, "  %s\n", dv->value);
    }
    if (ds->type == DATA_INDEPENDENT)
      fprintf (qucs_out, "</indep>\n");
    else if (ds->type == DATA_DEPENDENT)
      fprintf (qucs_out, "</dep>\n");
  }
}

/* This function is the Qucs dataset producer. */
void qucsdata_producer (dataset * data) {
  data->print ();
}
