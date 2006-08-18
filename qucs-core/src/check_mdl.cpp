/*
 * check_mdl.cpp - iterate an IC-CAP MDL file
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: check_mdl.cpp,v 1.1 2006/08/18 08:20:17 raimi Exp $
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
#include <ctype.h>

#include "strlist.h"
#include "object.h"
#include "complex.h"
#include "vector.h"
#include "dataset.h"
#include "sweep.h"
#include "valuelist.h"
#include "constants.h"
#include "check_mdl.h"
#include "tokens_mdl.h"

// Global variables.
dataset * mdl_result = NULL;
struct mdl_link_t * mdl_root = NULL;

// Creates an independent data vector.
static void mdl_create_depdataset (sweep * data, char * name) {
  vector v (name);
  for (int i = 0; i < data->getSize (); i++) v.add (data->get (i));
  mdl_result->appendDependency (new vector (v));
}

// Creates an independent data vector with a single element.
static void mdl_create_condataset (double val, char * name) {
  vector v (name);
  v.add (val);
  mdl_result->appendDependency (new vector (v));
}

// The functions creates dependent data vector(s).
static void mdl_create_vardataset (struct mdl_point_t * point,
				   struct mdl_datasize_t * dsize,
				   char * name, char * type, strlist * deps) {
  vector v[dsize->x * dsize->y];
  // create vectors
  for (struct mdl_point_t * p = point; p != NULL; p = p->next) {
    int n = (p->y - 1) * 2 + p->x - 1;
    v[n].add (rect (p->r, p->i));
  }
  // go through indices
  for (int x = 1; x < dsize->x + 1; x++) {
    for (int y = 1; y < dsize->y + 1; y++) {
      // create vector description
      int n = (y - 1) * 2 + x - 1;
      char * txt = (char *) malloc (strlen (name) + strlen (type) + 5 + 2 * 3);
      if (dsize->x > 1 || dsize->y > 1)
	sprintf (txt, "%s.%s[%d,%d]", name, type, x, y);
      else
	sprintf (txt, "%s.%s", name, type);
      v[n].setName (txt);
      free (txt);
      // put vector into dataset
      if (v[n].getSize () > 1) {
	v[n].setDependencies (new strlist (*deps));
	mdl_result->appendVariable (new vector (v[n]));
      } else {
	v[n].setDependencies (new strlist ());
	mdl_result->appendDependency (new vector (v[n]));
      }
    }
  }
}

// Look through hypertable elements for a name and return value.
static char * mdl_find_helement (struct mdl_element_t * root, char * name) {
  for (; root != NULL; root = root->next) {
    if (!strcmp (root->name, name)) return root->value;
  }
  return NULL;
}

// Look through table elements for a name and return value.
static char * mdl_find_telement (struct mdl_element_t * root, char * name) {
  for (; root != NULL; root = root->next) {
    if (!strcmp (root->name, "Name") && !strcmp (root->value, name)) {
      if (root->next && !strcmp (root->next->name, "Value")) {
	return root->next->value;
      }
    }
  }
  return NULL;
}

// Convert number suffix into a multiplication factor.
static double mdl_convert_factor (char * end) {
  double f = 1.0;
  if (end) {
    switch (*end) {
    case 'K': f = 1e+03; break;
    case 'M': f = 1e+06; break;
    case 'G': f = 1e+09; break;
    case 'T': f = 1e+12; break;
    case 'm': f = 1e-03; break;
    case 'u': f = 1e-06; break;
    case 'n': f = 1e-09; break;
    case 'p': f = 1e-12; break;
    case 'f': f = 1e-15; break;
    case 'a': f = 1e-18; break;
    }
  }
  return f;
}

// Forward declaration.
static double
mdl_telement_dvalue (struct mdl_link_t *, struct mdl_element_t *, char *);

// The function resolves the given variable trying upscope resolving.
static double mdl_resolve_variable (struct mdl_link_t * link, char * name) {
  int done = 0;
  double val = 0.0;
  struct mdl_lcontent_t * root;
  // try finding variable in current link
  for (root = link->content; !done && root != NULL; root = root->next) {
    if (root->type == t_TABLE) {
      struct mdl_element_t * eroot = root->table->data;
      if (mdl_find_telement (eroot, name)) {
	val = mdl_telement_dvalue (link, eroot, name);
	done++;
      }
    }
  }
  // resolve variable in upper scope recursively
  if (!done && link->parent) {
    val = mdl_resolve_variable (link->parent, name);
    done++;
  }
  return val;
}

// Converts a string into a valid value.  Uses variable substitutions.
static double mdl_variable_value (struct mdl_link_t * link, char * txt) {
  double val = 0.0;
  char * end = NULL;
  if (txt != NULL) {
    val = strtod (txt, &end);
    // not a value, a variable
    if (end == txt) {
      val = mdl_resolve_variable (link, txt);
    }
    // normal value with probably a suffix
    else {
      val *= mdl_convert_factor (end);
    }
  }
  return val;
}

// Returns a double variable value stored in a hypertable.
static double mdl_helement_dvalue (struct mdl_link_t * link,
				   struct mdl_element_t * eroot, char * name) {
  char * txt = mdl_find_helement (eroot, name);
  return mdl_variable_value (link, txt);
}

// Returns a double variable value stored in a table.
static double mdl_telement_dvalue (struct mdl_link_t * link,
				   struct mdl_element_t * eroot, char * name) {
  char * txt = mdl_find_telement (eroot, name);
  return mdl_variable_value (link, txt);
}

// Returns a integer variable value stored in a hypertable.
static int mdl_helement_ivalue (struct mdl_link_t * link,
				struct mdl_element_t  * eroot, char * name) {
  return (int) mdl_helement_dvalue (link, eroot, name);
}

// Looks for dependent data vectors and creates them.
static void mdl_find_vardataset (struct mdl_dcontent_t * droot, char * name,
				 strlist * deps) {
  struct mdl_dcontent_t * root;
  // go through dataset content
  for (root = droot; root != NULL; root = root->next) {
    if (root->type == t_DATASET) {
      // create possibly both - MEAS and SIMU - data vectors
      struct mdl_dataset_t * dset = root->data;
      if (dset->data1)
	mdl_create_vardataset (dset->data1, dset->dsize, name, dset->type1,
			       deps);
      if (dset->data2)
	mdl_create_vardataset (dset->data2, dset->dsize, name, dset->type2,
			       deps);
    }
  }
}

// Looks for independent data vectors and creates them.
valuelist<int> * mdl_find_depdataset (struct mdl_link_t * link,
				      struct mdl_dcontent_t * droot,
				      char * name) {
  char * stype = NULL;
  double val, start, stop, ratio, offset, step;
  int nof, order = 0;
  valuelist<int> * deps = new valuelist<int> ();
  struct mdl_dcontent_t * root;

  // go through dataset content
  for (root = droot; root != NULL; root = root->next) {
    if (root->type == t_HYPTABLE) {
      struct mdl_hyptable_t * hyptab = root->hyptable;
      // found a sweep definition?
      if (!strcmp (hyptab->name, "Edit Sweep Def")) {
	char * txt;
	if (!strcmp (stype, "LIN")) {
	  // linear sweep
	  order = mdl_helement_ivalue (link, hyptab->data, "Sweep Order");
	  start = mdl_helement_dvalue (link, hyptab->data, "Start");
	  stop = mdl_helement_dvalue (link, hyptab->data, "Stop");
	  nof = mdl_helement_ivalue (link, hyptab->data, "# of Points");
	  step = mdl_helement_dvalue (link, hyptab->data, "Step Size");
	  if (nof <= 0) nof = (int) ((stop - start) / step) + 1;
	  deps->append (name, new int (order));
	  linsweep * sw = new linsweep ();
	  sw->create (start, stop, nof);
	  mdl_create_depdataset (sw, name);
	}
	else if (!strcmp (stype, "CON")) {
	  // constant sweep
	  val = mdl_helement_dvalue (link, hyptab->data, "Value");
	  mdl_create_condataset (val, name);
	}
	else if (!strcmp (stype, "LOG")) {
	  // logarithmic sweep
	  order = mdl_helement_ivalue (link, hyptab->data, "Sweep Order");
	  start = mdl_helement_dvalue (link, hyptab->data, "Start");
	  stop = mdl_helement_dvalue (link, hyptab->data, "Stop");
	  nof = mdl_helement_ivalue (link, hyptab->data, "Total Pts");
	  deps->append (name, new int (order));
	  logsweep * sw = new logsweep ();
	  sw->create (start, stop, nof);
	  mdl_create_depdataset (sw, name);
	}
	else if (!strcmp (stype, "SYNC")) {
	  // sync sweep
	  ratio = mdl_helement_dvalue (link, hyptab->data, "Ratio");
	  offset = mdl_helement_dvalue (link, hyptab->data, "Offset");
	  txt = mdl_find_helement (hyptab->data, "Master Sweep");
	}
      }
      // found a sweep information?
      else if (!strcmp (hyptab->name, "Edit Sweep Info")) {
	stype = mdl_find_helement (hyptab->data, "Sweep Type");
      }
    }
  }
  return deps;
}

// Composes a link name.
static char * mdl_create_linkname (char * base, char * name) {
  char * txt = (char *) malloc (strlen (base) + 2 + strlen (name));
  sprintf (txt, "%s.%s", base, name);
  return txt;
}

// Collects dependency links.
static void mdl_find_deplink (struct mdl_link_t * link, char * name,
			      valuelist<int> * deps) {
  struct mdl_lcontent_t * root;
  valuelist<int> * d;
  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    // independent data vector
    if (root->type == t_DATA) {
      d = mdl_find_depdataset (link, root->data->content, name);
      if (d != NULL) {
	deps->append (d);
	delete d;
      }
    }
    // link to independent data vector
    else if (root->type == t_LINK && !strcmp (root->link->type, "SWEEP")) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_deplink (root->link, txt, deps);
      free (txt);
    }
  }
}

// Collects variable links.
static void mdl_find_varlink (struct mdl_link_t * link, char * name,
			      strlist * deps) {
  struct mdl_lcontent_t * root;
  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    // dependent data vector
    if (root->type == t_DATA) {
      mdl_find_vardataset (root->data->content, name, deps);
    }
    // link to dependent data vector
    else if (root->type == t_LINK && (!strcmp (root->link->type, "OUT") ||
				      !strcmp (root->link->type, "XFORM"))) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_varlink (root->link, txt, deps);
      free (txt);
    }
  }
}

// Sorts a dependency list according to their sweep order.
static strlist * mdl_sort_deps (valuelist<int> * d) {
  strlist * deps = new strlist ();
  for (int i = 0; i < d->length (); i++) {
    for (valuelistiterator<int> it (*d); *it; ++it) {
      if (*(it.currentVal ()) == i + 1) {
	deps->append (it.currentKey ());
      }
    }
  }
  return deps;
}

// Iterates the MDL file recursively.
static void mdl_find_link (struct mdl_link_t * link, char * name) {
  struct mdl_lcontent_t * root;

  // collect independent data
  valuelist<int> * vdeps = new valuelist<int> ();
  mdl_find_deplink (link, name, vdeps);
  strlist * deps = mdl_sort_deps (vdeps);
  delete vdeps;

  // collect dependent data
  mdl_find_varlink (link, name, deps);
  delete deps;

  // go through link content
  for (root = link->content; root != NULL; root = root->next) {
    if (root->type == t_LINK &&
	strcmp (root->link->type, "OUT") &&
	strcmp (root->link->type, "SWEEP") &&
	strcmp (root->link->type, "XFORM")) {
      char * txt = mdl_create_linkname (name, root->link->name);
      root->link->parent = link;
      mdl_find_link (root->link, txt);
      free (txt);
    }
  }
}

/* This function is the overall MDL data checker.  It returns zero on
   success, non-zero otherwise. */
int mdl_check (void) {
  int errors = 0;
  char * name = mdl_root->name;
  mdl_result = new dataset ();
  mdl_find_link (mdl_root, name);
  return errors ? -1 : 0;
}

// Destroys data used by the MDL checker.
void mdl_destroy (void) {
  if (mdl_result != NULL) {
    // delete associated dataset
    delete mdl_result;
    mdl_result = NULL;
  }
  if (mdl_root != NULL) {
    // release internal data structures
  }
}

// Initializes the MDL checker.
void mdl_init (void) {
  mdl_root = NULL;
  mdl_result = NULL;
}

/*
   TODO list:
   - release internal data structures
   - check SYNC sweeps
   - check dependency list of XFORMs
   - what about LIST sweeps?
   - free() unused data in the parser
*/
