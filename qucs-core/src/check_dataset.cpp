/*
 * check_dataset.cpp - checker for the Qucs dataset
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
 * $Id: check_dataset.cpp,v 1.1 2003/12/20 19:03:26 ela Exp $
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
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_dataset.h"

strlist * dataset_idents = NULL;
dataset * dataset_result = NULL;
vector  * dataset_vector = NULL;

/* This function is the checker routine for a parsed dataset.  It
   returns zero on success or non-zero if the parsed dataset contained
   errors. */
int dataset_check (dataset * data) {

  int errors = 0;
  vector * v, * d;

  /* check actual size and requested size of independent vectors */
  for (v = data->getDependencies (); v != NULL; v = (vector *) v->getNext ()) {
    if (v->getSize () != v->getRequested ()) {
      logprint (LOG_ERROR, "checker error, vector `%s' contains %d values, "
		"%d have been stated\n", v->getName (), v->getSize (),
		v->getRequested ());
      errors++;
    }
  }

  /* check dependencies of dependent vectors */
  for (v = data->getVariables (); v != NULL; v = (vector *) v->getNext ()) {
    strlist * s = v->getDependencies ();
    /* any dependencies at all ? */
    if (s == NULL || s->length () == 0) {
      logprint (LOG_ERROR, "checker error, vector `%s' contains no "
		"dependencies\n", v->getName ());
      errors++;
    }
    /* yes */
    else {
      int n = 0;
      /* go through each dependency and check it */
      for (struct strlist_t * root = s->getRoot (); 
	   root != NULL; root = root->next) {
	if ((d = data->findDependency (root->str)) == NULL) {
	  logprint (LOG_ERROR, "checker error, no such dependency `%s' as "
		    "stated in `%s'\n", root->str, v->getName ());
	  errors++;
	}
	else {
	  n += d->getSize ();
	}
      }
      if (n != 0) {
	if (v->getSize () % n != 0) {
	  logprint (LOG_ERROR, "checker error, size of vector `%s' %d should "
		    "be dividable by %d\n", v->getName (), v->getSize (), n);
	  errors++;
	}
      }
    }
  }

  return errors ? -1 : 0;
}
