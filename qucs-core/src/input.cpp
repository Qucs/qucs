/*
 * input.cpp - input netlist class implementation
 *
 * Copyright (C) 2003, 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: input.cpp,v 1.54 2006-01-27 09:32:01 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "logging.h"
#include "component.h"
#include "net.h"
#include "analysis.h"
#include "spsolver.h"
#include "dcsolver.h"
#include "parasweep.h"
#include "acsolver.h"
#include "trsolver.h"
#include "hbsolver.h"
#include "variable.h"
#include "environment.h"
#include "nodeset.h"
#include "input.h"
#include "check_netlist.h"
#include "equation.h"

// Global variables.
int netlist_check = 0;

// Constructor creates an unnamed instance of the input class.
input::input () : object () {
  fd = stdin;
  subnet = NULL;
  env = NULL;
}

// Constructor creates an named instance of the input class.
input::input (char * file) : object (file) {
  if ((fd = fopen (file, "r")) == NULL) {
    logprint (LOG_ERROR, "cannot open file `%s': %s\n", 
	      file, strerror (errno));
    fd = stdin;
  }
  subnet = NULL;
  env = NULL;
}

// Destructor deletes an input object.
input::~input () {
  if (fd != stdin) fclose (fd);
}

/* This function scans, parses and checks a netlist from the input
   file (specified by the constructor call) or stdin if there is no
   such file.  Afterwards the function builds the netlist
   representation and stores it into the given netlist object.  The
   function returns zero on success and non-zero otherwise. */
int input::netlist (net * netlist) {

  // tell the scanner to use the specified file
  netlist_in = getFile ();

  // save the netlist object
  subnet = netlist;

  logprint (LOG_STATUS, "parsing netlist...\n");
  if (netlist_parse () != 0)
    return -1;

  logprint (LOG_STATUS, "checking netlist...\n");
  if (netlist_checker () != 0)
    return -1;

  // add equation constants and then check the equations
  equation_constants ();
  logprint (LOG_STATUS, "checking equations...\n");
  if (equation_checker (0) != 0)
    return -1;
  if (netlist_checker_variables () != 0)
    return -1;

#if DEBUG
  netlist_list ();
#endif /* DEBUG */
  netlist_status ();

  logprint (LOG_STATUS, "creating netlist...\n");
  factory ();

  netlist_destroy ();
  return 0;
}

/* The little helper function creates a vector given by the list of
   values. */
vector * input::createVector (struct value_t * values) {
  vector * v = new vector ();
  for (; values != NULL; values = values->next) v->add (values->value);
  return v;
}

/* This function builds up the netlist representation from the checked
   netlist input.  It creates circuit components as necessary. */
void input::factory (void) {

  struct definition_t * def, * next;
  struct node_t * nodes;
  struct pair_t * pairs;
  circuit * c;
  object * o;
  analysis * a;
  substrate * s;
  nodeset * n;
  int i;

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle actions
    if (def->action) {
      if ((a = createAnalysis (def->type)) != NULL) {
	a->setName (def->instance);

	// add the properties to analysis
	for (pairs = def->pairs; pairs != NULL; pairs = pairs->next)
	  if (pairs->value->ident) {
	    if (pairs->value->var) {
	      // put new parameter sweep variable into environment
	      variable * v = new variable (pairs->value->ident);
	      constant * c = new constant (TAG_DOUBLE);
	      c->d = 0; // initialize the variable
	      v->setConstant (c);
	      env->addVariable (v);
	    }
	    a->addProperty (pairs->key, pairs->value->ident);
	  } else {
	    if (pairs->value->var) {
	      // add list sweeps and constants to the properties
	      variable * v = new variable (pairs->key);
	      constant * c = new constant (TAG_VECTOR);
	      c->v = createVector (pairs->value);
	      v->setConstant (c);
	      a->addProperty (pairs->key, v);
	    }
	    else {
	      a->addProperty (pairs->key, pairs->value->value);
	    }
	  }
	// additionally add missing optional properties
	assignDefaultProperties (a, def->define);

	subnet->insertAnalysis (a);
      }
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle substrate definitions
    if (!def->action && def->substrate) {
      if ((s = createSubstrate (def->type)) != NULL) {
	s->setName (def->instance);

	// add the properties to substrate
	for (pairs = def->pairs; pairs != NULL; pairs = pairs->next)
	  if (pairs->value->ident) {
	    s->addProperty (pairs->key, pairs->value->ident);
	  } else {
	    s->addProperty (pairs->key, pairs->value->value);
	  }
	// additionally add missing optional properties
	assignDefaultProperties (s, def->define);

	// put new substrate definition into environment
	variable * v = new variable (def->instance);
	v->setSubstrate (s);
	env->addVariable (v);
      }
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
    // handle nodeset definitions
    else if (!def->action && def->nodeset) {
      n = new nodeset ();
      n->setName (def->nodes->node);
      n->setValue (def->pairs->value->value);
      subnet->addNodeset (n);
      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }

  // go through the list of input definitions
  for (def = definition_root; def != NULL; def = next) {
    next = def->next;
    // handle component definitions
    if (!def->action && !def->substrate && !def->nodeset) {
      c = createCircuit (def->type);
      assert (c != NULL);
      o = (object *) c;
      c->setName (def->instance);
      c->setNonLinear (def->nonlinear != 0);
      c->setSubcircuit (def->subcircuit);

      // change size (number of ports) of variable sized components
      if (c->isVariableSized ()) {
	c->setSize (def->ncount);
      }
      // add appropriate nodes to circuit
      for (i = 0, nodes = def->nodes; nodes; nodes = nodes->next, i++)
	if (i < c->getSize ())
	  c->setNode (i, nodes->node);

      // add the properties to circuit
      for (pairs = def->pairs; pairs != NULL; pairs = pairs->next)
	if (pairs->value->ident) {
	  if (pairs->value->var) {
	    // at this stage it should be ensured that the variable is
	    // already in the root environment
	    variable * v = env->getVariable (pairs->value->ident);
	    o->addProperty (pairs->key, v);
	  } else {
	    if (pairs->value->subst) {
	      variable * v = env->getVariable (pairs->value->ident);
	      c->setSubstrate (v->getSubstrate ());
	    }
	    o->addProperty (pairs->key, pairs->value->ident);
	  }
	} else {
	  if (pairs->value->var) {
	    // add value lists to the properties
	    variable * v = new variable (pairs->key);
	    constant * c = new constant (TAG_VECTOR);
	    c->v = createVector (pairs->value);
	    v->setConstant (c);
	    o->addProperty (pairs->key, v);
	  } else {
	    o->addProperty (pairs->key, pairs->value->value);
	  }
	}
      // additionally add missing optional properties
      assignDefaultProperties (c, def->define);

      // insert the circuit into the netlist object
      subnet->insertCircuit (c);

      // remove this definition from the list
      definition_root = netlist_unchain_definition (definition_root, def);
    }
  }
}

/* This static function applies the optional missing properties's
   default values to the given object. */
void input::assignDefaultProperties (object * obj, struct define_t * def) {
  // go through optional properties
  for (int i = 0; PROP_IS_PROP (def->optional[i]); i++) {
    // is the property already assigned ?
    if (!obj->hasProperty (def->optional[i].key)) {
      if (PROP_IS_VAL (def->optional[i])) {
	// add double property
	obj->addProperty (def->optional[i].key, def->optional[i].defaultval.d);
      }
      else {
	// add string property
	obj->addProperty (def->optional[i].key, def->optional[i].defaultval.s);
      }
    }
  }
}

// The function creates components specified by the type of component. 
circuit * input::createCircuit (char * type) {
  if (!strcmp (type, "Pac"))
    return new pac ();
  else if (!strcmp (type, "R"))
    return new resistor ();
  else if (!strcmp (type, "C"))
    return new capacitor ();
  else if (!strcmp (type, "L"))
    return new inductor ();
  else if (!strcmp (type, "VCCS"))
    return new vccs ();
  else if (!strcmp (type, "CCCS"))
    return new cccs ();
  else if (!strcmp (type, "VCVS"))
    return new vcvs ();
  else if (!strcmp (type, "CCVS"))
    return new ccvs ();
  else if (!strcmp (type, "BiasT"))
    return new biastee ();
  else if (!strcmp (type, "DCFeed"))
    return new dcfeed ();
  else if (!strcmp (type, "DCBlock"))
    return new dcblock ();
  else if (!strcmp (type, "Circulator"))
    return new circulator ();
  else if (!strcmp (type, "Attenuator"))
    return new attenuator ();
  else if (!strcmp (type, "Isolator"))
    return new isolator ();
  else if (!strcmp (type, "Tr"))
    return new trafo ();
  else if (!strcmp (type, "sTr"))
    return new strafo ();
  else if (!strcmp (type, "Vdc"))
    return new vdc ();
  else if (!strcmp (type, "Idc"))
    return new idc ();
  else if (!strcmp (type, "Vac"))
    return new vac ();
  else if (!strcmp (type, "Iac"))
    return new iac ();
  else if (!strcmp (type, "Vpulse"))
    return new vpulse ();
  else if (!strcmp (type, "Ipulse"))
    return new ipulse ();
  else if (!strcmp (type, "Vrect"))
    return new vrect ();
  else if (!strcmp (type, "Irect"))
    return new irect ();
  else if (!strcmp (type, "Gyrator"))
    return new gyrator ();
  else if (!strcmp (type, "PShift"))
    return new phaseshifter ();
  else if (!strcmp (type, "TLIN"))
    return new tline ();
  else if (!strcmp (type, "COAX"))
    return new coaxline ();
  else if (!strcmp (type, "Coupler"))
    return new coupler ();
  else if (!strcmp (type, "Diode"))
    return new diode ();
  else if (!strcmp (type, "MLIN"))
    return new msline ();
  else if (!strcmp (type, "MCORN"))
    return new mscorner ();
  else if (!strcmp (type, "MSTEP"))
    return new msstep ();
  else if (!strcmp (type, "MOPEN"))
    return new msopen ();
  else if (!strcmp (type, "MGAP"))
    return new msgap ();
  else if (!strcmp (type, "MMBEND"))
    return new msmbend ();
  else if (!strcmp (type, "MCOUPLED"))
    return new mscoupled ();
  else if (!strcmp (type, "MTEE"))
    return new mstee ();
  else if (!strcmp (type, "MCROSS"))
    return new mscross ();
  else if (!strcmp (type, "MVIA"))
    return new msvia ();
  else if (!strcmp (type, "CLIN"))
    return new cpwline ();
  else if (!strcmp (type, "COPEN"))
    return new cpwopen ();
  else if (!strcmp (type, "CSHORT"))
    return new cpwshort ();
  else if (!strcmp (type, "CGAP"))
    return new cpwgap ();
  else if (!strcmp (type, "CSTEP"))
    return new cpwstep ();
  else if (!strcmp (type, "IProbe"))
    return new iprobe ();
  else if (!strcmp (type, "JFET"))
    return new jfet ();
  else if (!strcmp (type, "BJT"))
    return new bjt ();
  else if (!strcmp (type, "SPfile"))
    return new spfile ();
  else if (!strcmp (type, "Vnoise"))
    return new vnoise ();
  else if (!strcmp (type, "Inoise"))
    return new inoise ();
  else if (!strcmp (type, "MOSFET"))
    return new mosfet ();
  else if (!strcmp (type, "Amp"))
    return new amplifier ();
  else if (!strcmp (type, "OpAmp"))
    return new opamp ();
  else if (!strcmp (type, "IInoise"))
    return new iinoise ();
  else if (!strcmp (type, "MUT"))
    return new mutual ();
  else if (!strcmp (type, "MUT2"))
    return new mutual2 ();
  else if (!strcmp (type, "IVnoise"))
    return new ivnoise ();
  else if (!strcmp (type, "VVnoise"))
    return new vvnoise ();
  else if (!strcmp (type, "Inv"))
    return new inverter ();
  else if (!strcmp (type, "NOR"))
    return new logicnor ();
  else if (!strcmp (type, "OR"))
    return new logicor ();
  else if (!strcmp (type, "NAND"))
    return new logicnand ();
  else if (!strcmp (type, "AND"))
    return new logicand ();
  else if (!strcmp (type, "XNOR"))
    return new logicxnor ();
  else if (!strcmp (type, "XOR"))
    return new logicxor ();
  else if (!strcmp (type, "DigiSource"))
    return new digisource ();

  logprint (LOG_ERROR, "no such circuit type `%s'\n", type);
  return NULL;
}

// The function creates an analysis specified by the type of analysis.
analysis * input::createAnalysis (char * type) {
  if (!strcmp (type, "SP"))
    return new spsolver ();
  else if (!strcmp (type, "DC"))
    return new dcsolver ();
  else if (!strcmp (type, "SW"))
    return new parasweep ();
  else if (!strcmp (type, "AC"))
    return new acsolver ();
  else if (!strcmp (type, "TR"))
    return new trsolver ();
  else if (!strcmp (type, "HB"))
    return new hbsolver ();

  logprint (LOG_ERROR, "no such analysis type `%s'\n", type);
  return NULL;
}

// The function creates a substrate specified by the type of substrate.
substrate * input::createSubstrate (char * type) {
  if (!strcmp (type, "SUBST"))
    return new substrate ();

  logprint (LOG_ERROR, "no such substrate type `%s'\n", type);
  return NULL;
}
