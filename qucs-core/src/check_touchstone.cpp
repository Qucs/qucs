/*
 * check_touchstone.cpp - checker for Touchstone files
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
 * $Id: check_touchstone.cpp,v 1.1 2003/12/20 19:03:27 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_touchstone.h"

strlist * touchstone_idents = NULL;
dataset * touchstone_result = NULL;
vector  * touchstone_vector = NULL;

/* default touchstone options */
struct touchstone_t touchstone_options = { "GHz", 'S', "MA", 50.0, 1e9, 0 };

/* avalaible touchstone options */
static char * touchstone_valid_options[] = {
  "hz", "khz", "mhz", "ghz", "s", "y", "z", "g", "h", "ma", "db", "ri", NULL };

/* This subroutine is going to join vectors on multiple lines.  The
   input and output list of vector of this function is the
   touchstone_vector variable. */
static void touchstone_join (void) {
  vector * yroot, * xroot, * next = NULL;
  /* go through each vector */
  for (yroot = touchstone_vector; yroot != NULL; yroot = next) {
    /* go through each trailing vector */
    next = (vector *) yroot->getNext ();
    for (xroot = next; xroot != NULL; xroot = next) {
      next = (vector *) xroot->getNext ();
      /* append xroot vector to yroot vector ? */
      if (xroot->getSize () % 2 == 0) {
	/* yes, delete the xroot vector and adjust list */
	yroot->add (xroot);
	yroot->setNext (next);
	delete xroot;
      }
      else {
	/* no, handle next vectors */
	next = xroot;
	break;
      }
    }
  }
}

/* This subroutine checks the size and overall conformance of each
   touchstone matrix at a given frequency derived from the first
   matrix.  The function return zero on success and non-zero
   otherwise. */
static int touchstone_vector_check (void) {
  vector * root = touchstone_vector, * next;
  int even = 0, errors = 0, size = root->getSize ();
  nr_double_t f = real (root->get (0));

  /* check size of first line */
  if ((size % 2) == 0) {
    logprint (LOG_ERROR, "checker error, first line has %d (even) values\n", 
	      size);
    errors++;
    even = 1;
  }
  touchstone_options.ports = (int) sqrt ((size - 1) / 2);

  /* check first frequency value */
  if (f < 0.0) {
    logprint (LOG_ERROR, "checker error, negative frequency value %e\n", f);
    errors++;
  }

  /* go through each vector */
  for (root = (vector *) root->getNext (); root != NULL; root = next) {
    next = (vector *) root->getNext ();
    /* check size of vector */
    if (!even && root->getSize () != size) {
      logprint (LOG_ERROR, "checker error, line (f = %e) has %d values, %d "
		"required\n", real (root->get (0)), root->getSize (), size);
      errors++;
    }
    /* check frequency value */
    if (real (root->get (0)) < 0.0) {
      logprint (LOG_ERROR, "checker error, negative frequency value %e\n",
		real (root->get (0)));
      errors++;
    }
    /* check increasing frequency value */
    if (f >= real (root->get (0))) {
      logprint (LOG_ERROR, "checker error, line (f = %e) has decreasing "
		"frequency value\n", real (root->get (0)));
      errors++;
    }
    f = real (root->get (0));
  }
  return errors;
}

/* The function evaluates the identifiers in the option line and fills
   the touchstone_options structure with appropriate values. */
static void touchstone_options_eval (void) {
  /* go through all identifiers */
  for (int i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    /* frequency unit */
    if (!strcmp (str, "hz")) {
      touchstone_options.factor = 1.0;
      touchstone_options.unit = "Hz";
    }
    else if (!strcmp (str, "khz")) {
      touchstone_options.factor = 1e3;
      touchstone_options.unit = "kHz";
    }
    else if (!strcmp (str, "mhz")) {
      touchstone_options.factor = 1e6;
      touchstone_options.unit = "MHz";
    }
    else if (!strcmp (str, "ghz")) {
      touchstone_options.factor = 1e9;
      touchstone_options.unit = "GHz";
    }
    /* parameter type */
    else if (!strcmp (str, "s")) {
      touchstone_options.parameter = 'S';
    }
    else if (!strcmp (str, "y")) {
      touchstone_options.parameter = 'Y';
    }
    else if (!strcmp (str, "z")) {
      touchstone_options.parameter = 'Z';
    }
    else if (!strcmp (str, "g")) {
      touchstone_options.parameter = 'G';
    }
    else if (!strcmp (str, "h")) {
      touchstone_options.parameter = 'H';
    }
    /* value formats */
    else if (!strcmp (str, "ma")) {
      touchstone_options.format = "MA";
    }
    else if (!strcmp (str, "db")) {
      touchstone_options.format = "dB";
    }
    else if (!strcmp (str, "ri")) {
      touchstone_options.format = "RI";
    }
  }
}

/* This little function returns a static string containing an
   appropriate variable name. */
static char * touchstone_create_set (int i, int j) {
  static char text[16];
  sprintf (text, "%c%d%d", touchstone_options.parameter, i, j);
  if (touchstone_options.ports == 2) {
    /* handle special case for 2-port touchstone data, '21' data
       precedes the '12' data */
    if (i != j)
      sprintf (text, "%c%d%d", touchstone_options.parameter, j, i);
  }
  return text;
}

/* The function actually creates the resulting dataset. */
static void touchstone_create (void) {
  vector * f, * v, * root, * next;
  int ports = touchstone_options.ports;
  complex val;
  strlist * s;

  /* create dataset and frequency vector */
  touchstone_result = new dataset ();
  f = new vector ("frequency");
  touchstone_result->appendDependency (f);
  s = new strlist ();
  s->add (f->getName ());
  /* create variable vectors for the resulting dataset */
  for (int i = 0; i < ports; i++) {
    for (int j = 0; j < ports; j++) {
      v = new vector ();
      v->setName (touchstone_create_set (i + 1, j + 1));
      v->setDependencies (s);
      touchstone_result->appendVariable (v);
    }
  }

  /* go through each vector */
  for (root = touchstone_vector; root != NULL; root = next) {
    next = (vector *) root->getNext ();
    /* fill frequency vector */
    f->add (real (root->get (0)) * touchstone_options.factor);
    /* go through each variable vector */
    v = touchstone_result->getVariables ();
    for (int i = 0; i < ports; i++) {
      for (int j = 0; j < ports; j++) {
	int pos = 1 + j * 2 + i * 2 * ports;
	/* depending on the touchstone data format */
	if (!strcmp (touchstone_options.format, "RI")) {
	  val = rect (real (root->get (pos + 0)),
		      real (root->get (pos + 1)));
	}
	else if (!strcmp (touchstone_options.format, "MA")) {
	  val = polar (real (root->get (pos + 0)),
		       real (root->get (pos + 1)) * M_PI / 180.0);
	}
	else if (!strcmp (touchstone_options.format, "dB")) {
	  val = polar (pow (10.0, real (root->get (pos + 0)) / 20.0),
		       real (root->get (pos + 1)) * M_PI / 180.0);
	}
	/* correct the reference impedance */
	val *= (touchstone_options.resistance / 50.0);
	v->add (val);
	v = (vector *) v->getNext ();
      }
    }
  }
}

/* Removes temporary data items from memory if necessary. */
static void touchstone_finalize (void) {
  vector * root, * next;
  for (root = touchstone_vector; root != NULL; root = next) {
    next = (vector *) root->getNext ();
    delete root;
  }
  touchstone_vector = NULL;
  if (touchstone_idents != NULL) {
    delete touchstone_idents;
    touchstone_idents = NULL;
  }
  /* apply default values again */
  touchstone_options.unit = "GHz";
  touchstone_options.parameter = 'S';
  touchstone_options.format = "MA";
  touchstone_options.resistance = 50.0;
  touchstone_options.factor = 1e9;
  touchstone_options.ports = 0;
}


/* This function is the checker routine for a parsed touchstone.  It
   returns zero on success or non-zero if the parsed touchstone
   contained errors. */
int touchstone_check (void) {

  int n, errors = 0;

  /* first checking the options */
  if (touchstone_idents->length () > 3) {
    logprint (LOG_ERROR, "checker error, found %d options\n", 
	      touchstone_idents->length ());
    errors++;
  }
  /* touchstone is case insenstive */
  for (int i = 0; i < touchstone_idents->length (); i++) {
    for (char * p = touchstone_idents->get (i); *p != '\0'; p++)
      *p = tolower (*p);
  }
  /* check duplicate options */
  for (int i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    if ((n = touchstone_idents->contains (str)) != 1) {
      logprint (LOG_ERROR, "checker error, option `%s' occurred %dx\n", 
		str, n);
      errors++;
    }
  }
  /* check valid options */
  for (int i = 0; i < touchstone_idents->length (); i++) {
    char * str = touchstone_idents->get (i);
    int valid = 0;
    for (int v = 0; touchstone_valid_options[v] != NULL; v++) {
      if (!strcmp (touchstone_valid_options[v], str))
	valid = 1;
    }
    if (!valid) {
      logprint (LOG_ERROR, "checker error, invalid option `%s'\n", str);
      errors++;
    }
  }

  if (touchstone_vector == NULL) {
    logprint (LOG_ERROR, "checker error, no data in touchstone file\n");
    errors++;
  }
  else {
    /* join vectors on multiple lines */
    touchstone_join ();

    /* check each vector */
    errors += touchstone_vector_check ();
  }

  /* evaluate the option line and put values into touchstone_options
     structure */
  touchstone_options_eval ();

  /* finally create a dataset */
  if (!errors) touchstone_create ();

  /* free temporary memory */
  touchstone_finalize ();

  return errors ? -1 : 0;
}
