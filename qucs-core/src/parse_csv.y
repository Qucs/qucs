/* -*-c++-*- */

%{
/*
 * parse_csv.y - parser for CSV files
 *
 * Copyright (C) 2007 Stefan Jahn <stefan@lkcc.org>
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
#include <string.h>

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_csv.h"

using namespace qucs;

%}

%name-prefix="csv_"

%token InvalidCharacter
%token Float
%token Identifier
%token Eol

%union {
  char * ident;
  double f;
  qucs::vector * v;
  qucs::strlist * list;
}

%type <list> HeaderList HeaderLine
%type <ident> Identifier Header
%type <f> Float
%type <v> DataLine DataSet DataList

%%

Input:
    HeaderLine DataSet { /* describes a valid csv */
    csv_header = $1;
    csv_vector = $2;
  }
  | DataSet {
    csv_vector = $1;
  }
;

Header:
    '"' Identifier '"' {
    $$ = strdup ($2);
  }
  | Identifier {
    $$ = strdup ($1);
  }
;

HeaderList: /* nothing */ { $$ = NULL; }
  | Header HeaderList {
    if ($2 == NULL) $2 = new strlist ();
    $2->add ($1);
    $$ = $2;
    free ($1);
  }
;

HeaderLine: /* header line */
    HeaderList Eol {
    $$ = $1;
  }
  | Eol HeaderLine { /* skip this line */
    $$ = $2;
  }
;

DataSet: /* nothing */ { $$ = NULL; }
  | DataLine Eol DataSet { /* append vector lines */
    $1->setNext ($3);
    $$ = $1;
  }
  | DataLine { /* last line, no trailing end-of-line */
    $$ = $1;
  }
  | Eol DataSet { /* skip this line */
    $$ = $2;
  }
;

DataLine: DataList
;

DataList: /* nothing */ { $$ = NULL; }
  | Float DataList {
    if ($2 == NULL) $2 = new vector ();
    $2->add ($1);
    $$ = $2;
  }
;

%%

int csv_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", csv_lineno, error);
  return 0;
}
