/*
 * logging.c - logging facility class implementation
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: logging.c,v 1.6 2004-08-30 20:55:18 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "logging.h"

/* This function prints the given messages format and the appropriate
   arguments to a FILE stream depending on the given log level. */
void logprint (int level, const char * format, ...) {
  FILE * f;
  va_list args;

  f = level == LOG_STATUS ? file_status : file_error;
  if (f != NULL) {
    va_start (args, format);
    vfprintf (f, format, args);
    va_end (args);
    fflush (f);
  }
}

/* Initialization of the logging interface. */
void loginit (void) {
  file_error = file_status = stderr;
}

/* Both of the log level dependent FILE streams. */
FILE * file_status = NULL;
FILE * file_error = NULL;

/* Print a tiny progress-bar depending on the arguments. */
void logprogressbar (nr_double_t current, nr_double_t final, int points) {
  int i;
  if (progressbar_enable) {
    logprint (LOG_STATUS, "[");
    for (i = 0; i < (current / final) * points; i++)
      logprint (LOG_STATUS, "*");
    for (; i < points; i++) logprint (LOG_STATUS, " ");
    logprint (LOG_STATUS, "] %.2f%%      \r", current * 100.0 / final);
  }
}

/* If non-zero then progress bars are painted... */
int progressbar_enable = 0;

/* Clears up the progress bar if requested. */
void logprogressclear (int points) {
  int i;
  if (progressbar_enable) {
    for (i = 0; i < points + 15; i++) logprint (LOG_STATUS, " ");
    logprint (LOG_STATUS, "\r");
  }
}

