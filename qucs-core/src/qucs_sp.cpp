/*
 * qucs_sp.cpp - main program implementation
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
 * $Id: qucs_sp.cpp,v 1.1 2003/12/20 19:03:25 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

using namespace std;

#include <stdio.h>
#include <iostream>

#include "logging.h"
#include "component.h"
#include "net.h"
#include "input.h"
#include "dataset.h"
#include "matrix.h"

int main (int argc, char ** argv) {

  char * infile = NULL, * outfile = NULL;
  net * subnet;
  input * in;
  circuit * gnd;
  dataset * out;

  loginit ();

  // check program arguments
  for (int i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-v")) {
      logprint (LOG_STATUS,
		"Qucs SP " VERSION "\n"
		"Copyright (C) 2003 Stefan Jahn <stefan@lkcc.org>\n"
		"\nThis is free software; see the source for copying "
		"conditions.  There is NO\n"
		"warranty; not even for MERCHANTABILITY or FITNESS FOR A "
		"PARTICULAR PURPOSE.\n");
      return 0;
    }
    else if (!strcmp (argv[i], "-i")) {
      infile = argv[++i];
    }
    else if (!strcmp (argv[i], "-o")) {
      outfile = argv[++i];
      file_status = stdout;
    }
  }

  // create netlist object and input
  subnet = new net ("subnet");
  in = infile ? new input (infile) : new input ();
  
  // get input netlist
  if (in->netlist (subnet) != 0)
    return -1;

  // attach a ground to the netlist
  gnd = new ground ();
  gnd->setNode (1, "gnd");
  gnd->setName ("GND");
  subnet->insertCircuit (gnd);

  // solve the netlist
  subnet->solve ();

  // evaluate output dataset
  out = subnet->getData ();
  out->setFile (outfile);
  out->print ();

  delete subnet;
  delete in;
  return 0;
}
