/*
 * qucsconv.cpp - main converter program implementation
 *
 * Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: qucsconv.cpp,v 1.23 2006/07/20 10:47:36 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

#include "logging.h"
#include "precision.h"
#include "check_spice.h"
#include "check_vcd.h"
#include "check_citi.h"
#include "check_touchstone.h"
#include "check_zvr.h"
#include "check_dataset.h"
#include "qucs_producer.h"
#include "csv_producer.h"

/* structure defining a conversion */
struct actionset_t {
  char * in;  /* -if parameter */
  char * out; /* -of parameter */

  /* callback for the logic, return error code of application */
  int (* execute) (struct actionset_t *, char * infile, char * outfile);
};

/* required forward declarations */
int spice2qucs (struct actionset_t *, char *, char *);
int vcd2qucs   (struct actionset_t *, char *, char *);
int qucs2csv   (struct actionset_t *, char *, char *);
int citi2qucs  (struct actionset_t *, char *, char *);
int touch2qucs (struct actionset_t *, char *, char *);
int zvr2qucs   (struct actionset_t *, char *, char *);

/* conversion definitions */
struct actionset_t actionset[] = {
  { "spice",      "qucs",     spice2qucs },
  { "spice",      "qucslib",  spice2qucs },
  { "vcd",        "qucsdata", vcd2qucs   },
  { "qucsdata",   "csv",      qucs2csv   },
  { "citi",       "qucsdata", citi2qucs  },
  { "touchstone", "qucsdata", touch2qucs },
  { "zvr",        "qucsdata", zvr2qucs   },
  { NULL, NULL, NULL}
};

/* opens the given file, fallback to stdin/stdout */
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

/* main entry point */
int main (int argc, char ** argv) {

  char * infile = NULL, * outfile = NULL, * input = NULL, * output = NULL;

  loginit ();
  precinit ();

  // check program arguments
  for (int i = 1; i < argc; i++) {
    if (!strcmp (argv[i], "-v") || !strcmp (argv[i], "--version")) {
      fprintf (stdout,
	"QucsConverter " PACKAGE_VERSION "\n"
	"Copyright (C) 2004, 2005, 2006 Stefan Jahn <stefan@lkcc.org>\n"
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
	"  -d  DATANAME    data variable specification\n"
	"  -c, --correct   enable node correction\n"
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
    else if (!strcmp (argv[i], "-d")) {
      if (argv[++i]) csv_var = argv[i];
    }
    else if (!strcmp (argv[i], "-c") || !strcmp (argv[i], "--correct")) {
      vcd_correct = 1;
    }
  }

  // check input/output formats
  int infound = 0;
  int outfound = 0;
  for (int j = 0; actionset[j].in != NULL; j++) {
    int in = 0, out = 0;
    if (input && !strcmp (input, actionset[j].in)) {
      in = infound = 1;
    }
    if (output && !strcmp (output, actionset[j].out)) {
      out = outfound = 1;
    }
    if (in && out) {
      return actionset[j].execute (&actionset[j], infile, outfile);
    }
  }

  // no appropriate conversion found
  if (!infound) {
    fprintf (stderr, "invalid input data specification `%s'\n",
	     input ? input : "not given");
  }
  if (!outfound) {
    fprintf (stderr, "invalid output data specification `%s'\n",
	     output ? output : "not given");
  }
  fprintf (stderr, "invalid input/output data specification `%s->%s'\n",
	   input ? input : "not given", output ? output : "not given");
  return -1;
}

// SPICE to Qucs conversion.
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
  if (spice_in)
    fclose (spice_in);
  if (ret) {
    spice_destroy ();
    return -1;
  }

  if ((qucs_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "qucs"))
    qucs_producer ();
  else /* "qucslib" */
    qucslib_producer ();
  fclose (qucs_out);
  spice_destroy ();
  return 0;
}

// VCD to Qucs conversion.
int vcd2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  vcd_init ();
  if ((vcd_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (vcd_parse () != 0) {
    ret = -1;
  } else if (vcd_checker () != 0) {
    ret = -1;
  }
  vcd_lex_destroy ();
  if (vcd_in)
    fclose (vcd_in);
  if (ret) {
    vcd_destroy ();
    return -1;
  }

  if ((qucs_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "qucsdata"))
    qucsdata_producer_vcd ();
  fclose (qucs_out);
  vcd_destroy ();
  return 0;
}

// Qucs dataset to CSV conversion.
int qucs2csv (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  if ((dataset_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (dataset_parse () != 0) {
    ret = -1;
  } else if (dataset_result == NULL) {
    ret = -1;
  } else if (dataset_check (dataset_result) != 0) {
    delete dataset_result;
    dataset_result = NULL;
    ret = -1;
  }
  qucs_data = dataset_result;
  dataset_result = NULL;
  dataset_lex_destroy ();
  if (dataset_in)
    fclose (dataset_in);
  if (ret)
    return -1;

  if ((csv_out = open_file (outfile, "w")) == NULL)
    return -1;
  if (!strcmp (action->out, "csv")) {
    if (csv_var != NULL)
      csv_producer (csv_var, ";");
    else {
      fprintf (stderr, "no data variable given (passed by -d option)\n");
      ret = -1;
    }
    fclose (csv_out);
    return ret;
  }
  return -1;
}

// CITIfile to Qucs conversion.
int citi2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  citi_init ();
  if ((citi_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (citi_parse () != 0) {
    ret = -1;
  } else if (citi_check () != 0) {
    ret = -1;
  }
  citi_lex_destroy ();
  if (citi_in)
    fclose (citi_in);
  if (ret) {
    citi_destroy ();
    return -1;
  }

  if (!strcmp (action->out, "qucsdata")) {
    citi_result->setFile (outfile);
    qucsdata_producer (citi_result);
  }
  citi_destroy ();
  return 0;
}

// Touchstone to Qucs conversion.
int touch2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  touchstone_init ();
  if ((touchstone_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (touchstone_parse () != 0) {
    ret = -1;
  } else if (touchstone_check () != 0) {
    ret = -1;
  }
  touchstone_lex_destroy ();
  if (touchstone_in)
    fclose (touchstone_in);
  if (ret) {
    touchstone_destroy ();
    return -1;
  }

  if (!strcmp (action->out, "qucsdata")) {
    touchstone_result->setFile (outfile);
    qucsdata_producer (touchstone_result);
  }
  touchstone_destroy ();
  return 0;
}

// ZVR to Qucs conversion.
int zvr2qucs (struct actionset_t * action, char * infile, char * outfile) {
  int ret = 0;
  zvr_init ();
  if ((zvr_in = open_file (infile, "r")) == NULL) {
    ret = -1;
  } else if (zvr_parse () != 0) {
    ret = -1;
  } else if (zvr_check () != 0) {
    ret = -1;
  }
  zvr_lex_destroy ();
  if (zvr_in)
    fclose (zvr_in);
  if (ret) {
    zvr_destroy ();
    return -1;
  }
  if (!strcmp (action->out, "qucsdata")) {
    zvr_result->setFile (outfile);
    qucsdata_producer (zvr_result);
  }
  zvr_destroy ();
  return 0;
}

