/*
 * qucsconv.cpp - main converter program implementation
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
 * $Id: qucsconv.cpp,v 1.10 2005-12-26 21:13:31 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "check_spice.h"
#include "qucs_producer.h"

struct actionset_t {
  char * informat; /* -if parameter */
  char * outformat; /* -of parameter */

  /* Callback for the logic, return errorcode of program */
  int (* execute) (struct actionset_t *, char * infile, char * outfile);
};

int spice2qucs (struct actionset_t *, char *, char *);
int vcd2qucs (struct actionset_t *, char *, char *);

struct actionset_t actionset[] = {
  {"spice", "qucs",    spice2qucs},
  {"spice", "qucslib", spice2qucs},
  {"vcd", "qucsdata",  vcd2qucs},
  {NULL, NULL, NULL}
};


FILE * open_file (char * file, char * flag) {
  FILE * fd = NULL;
  if (file) {
    if ((fd = fopen (file, flag)) == NULL) {
      fprintf (stderr, "cannot open file `%s': %s, using %s instead\n",
	       file, strerror (errno), flag[0] == 'r' ? "stdin" : "stdout");
      fd = flag[0] == 'r' ? stdin : stdout;
    }
  }
  else {
    fd = flag[0] == 'r' ? stdin : stdout;
  }
  return fd;
}

int main (int argc, char ** argv) {

  char * infile = NULL, * outfile = NULL, * input = NULL, * output = NULL;

  // check program arguments
  for (int i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version")) {
      fprintf (stdout,
	"QucsConverter " PACKAGE_VERSION "\n"
	"Copyright (C) 2004, 2005 Stefan Jahn <stefan@lkcc.org>\n"
	"\nThis is free software; see the source for copying "
	"conditions.  There is NO\n"
	"warranty; not even for MERCHANTABILITY or FITNESS FOR A "
	"PARTICULAR PURPOSE.\n");
      return 0;
    }
    if (!strcmp (argv[i], "-h") || !strcmp (argv[i], "--help")) {
      fprintf (stdout,
	"Usage: %s [OPTION]...\n\n"
	"  -h, --help      display this help and exit\n"
	"  -v, --version   display version information and exit\n"
	"  -i  FILENAME    use file as input file (default stdin)\n"
	"  -o  FILENAME    use file as output file (default stdout)\n"
	"  -if FORMAT      input data specification (e.g. spice)\n"
	"  -of FORMAT      output data specification (e.g. qucs)\n"
	"  -a, --noaction  do not include netlist actions in the output\n"
	"  -g  GNDNODE     replace ground node\n"
	"\nReport bugs to <" PACKAGE_BUGREPORT ">.\n", argv[0]);
      return 0;
    }
    else if (!strcmp (argv[i], "-i")) {
      infile = argv[++i];
    }
    else if (!strcmp (argv[i], "-o")) {
      outfile = argv[++i];
    }
    else if (!strcmp (argv[i], "-if")) {
      input = argv[++i];
    }
    else if (!strcmp (argv[i], "-of")) {
      output = argv[++i];
    }
    else if (!strcmp (argv[i], "-a") || !strcmp (argv[i], "--noaction")) {
      qucs_actions = 0;
    }
    else if (!strcmp (argv[i], "-g")) {
      if (argv[++i]) qucs_gnd = argv[i];
    }
  }

  int inknown = 0;
  int outknown = 0;
  for (int j = 0; actionset[j].informat != NULL; j++) {
    int in = 0, out = 0;

    if (input && !strcmp (input, actionset[j].informat)) {
      in = 1;
      inknown = 1;
    }

    if (output && !strcmp (output, actionset[j].outformat)) {
      out = 1;
      outknown = 1;
    }

    if (in && out) {
      return actionset[j].execute (&actionset[j], infile, outfile);
    }
  }

  if (!inknown) {
    fprintf (stderr, "invalid input data specification `%s'\n",
	     input ? input : "not given");
  }

  if (!outknown) {
    fprintf (stderr, "invalid output data specification `%s'\n",
	     output ? output : "not given");
  }

  return -1;
}


int spice2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((spice_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (spice_parse () != 0) {
    ret = -1;
  } else if (spice_checker () != 0) {
    ret = -1;
  }
  spice_lex_destroy ();
  fclose (spice_in);
  if (ret) {
    spice_destroy ();
    return -1;
  }

  if ((qucs_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->outformat, "qucs"))
    qucs_producer ();
  else /* "qucslib" */
    qucslib_producer ();
  fclose (qucs_out);
  spice_destroy ();
  return 0;
}


int vcd2qucs (struct actionset_t * action, char * infile, char * outfile) {
  /* TBI */
  return 0;
}

