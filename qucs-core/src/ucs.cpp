/*
 * ucs.cpp - main program implementation
 *
 * Copyright (C) 2003-2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: ucs.cpp,v 1.33 2009/03/03 18:30:12 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>

#include "logging.h"
#include "precision.h"
#include "component.h"
#include "components.h"
#include "net.h"
#include "input.h"
#include "dataset.h"
#include "equation.h"
#include "environment.h"
#include "exceptionstack.h"
#include "check_netlist.h"
#include "module.h"

#if HAVE_UNISTD_H
#include <unistd.h>
#endif

using namespace qucs;

int main (int argc, char ** argv) {

  char * infile = NULL, * outfile = NULL;
  net * subnet;
  input * in;
  circuit * gnd;
  dataset * out;
  environment * root;
  int listing = 0;
  int ret = 0;

  loginit ();
  precinit ();
  ::srand (::time (NULL));

  // check program arguments
  for (int i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version")) {
      fprintf (stdout,
	"Qucsator " PACKAGE_VERSION "\n"
	"Copyright (C) 2003-2009 "
	"Stefan Jahn <stefan@lkcc.org>\n"
        "Copyright (C) 2006 Helene Parruitte <parruit@enseirb.fr>\n"
        "Copyright (C) 2006 Bastien Roucaries <roucaries.bastien@gmail.com>\n"
	"\nThis is free software; see the source for copying "
	"conditions.  There is NO\n"
	"warranty; not even for MERCHANTABILITY or FITNESS FOR A "
	"PARTICULAR PURPOSE.\n");
      return 0;
    }
    if (!strcmp (argv[i], "-h") || !strcmp (argv[i], "--help")) {
      fprintf (stdout,
	"Usage: %s [OPTION]...\n\n"
	"  -h, --help     display this help and exit\n"
	"  -v, --version  display version information and exit\n"
	"  -i FILENAME    use file as input netlist (default stdin)\n"
	"  -o FILENAME    use file as output dataset (default stdout)\n"
	"  -b, --bar      enable textual progress bar\n"
	"  -g, --gui      special progress bar used by gui\n"
	"  -c, --check    check the input netlist and exit\n"
	"\nReport bugs to <" PACKAGE_BUGREPORT ">.\n", argv[0]);
      return 0;
    }
    else if (!strcmp (argv[i], "-i")) {
      infile = argv[++i];
    }
    else if (!strcmp (argv[i], "-o")) {
      outfile = argv[++i];
      file_status = stdout;
    }
    else if (!strcmp (argv[i], "-b") || !strcmp (argv[i], "--bar")) {
      progressbar_enable = 1;
    }
    else if (!strcmp (argv[i], "-g") || !strcmp (argv[i], "--gui")) {
      progressbar_gui = 1;
    }
    else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--check")) {
      netlist_check = 1;
    }
    else if (!strcmp (argv[i], "-l") || !strcmp (argv[i], "--listing")) {
      listing = 1;
    }
  }

  // create static modules
  module::registerModules ();

#if DEBUG
  // emit C-code for available definitions if requested and exit
  if (listing) {
    module::print ();
    return ret;
  }
#endif /* DEBUG */

  // create root environment
  root = new environment ("root");

  // create netlist object and input
  subnet = new net ("subnet");
  in = infile ? new input (infile) : new input ();

  // pass root environment to netlist object and input
  subnet->setEnv (root);
  in->setEnv (root);
  
  // get input netlist
  if (in->netlist (subnet) != 0) {
    if (netlist_check) {
      logprint (LOG_STATUS, "checker notice, netlist check FAILED\n");
    }
    return -1;
  }
  if (netlist_check) {
    logprint (LOG_STATUS, "checker notice, netlist OK\n");
    return 0;
  }

  // attach a ground to the netlist
  gnd = new ground ();
  gnd->setNode (0, "gnd");
  gnd->setName ("GND");
  subnet->insertCircuit (gnd);

  // analyse the netlist
  int err = 0;
  out = subnet->runAnalysis (err);
  ret |= err;

  // evaluate output dataset
  ret |= root->equationSolver (out);
  out->setFile (outfile);
  out->print ();

  estack.print ("uncaught");

  delete subnet;
  delete in;
  delete out;
  delete root;

  // delete modules
  module::unregisterModules ();

  netlist_destroy_env ();
  return ret;
}
