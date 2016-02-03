/* -*-c++-*- */

%{
/*
 * parse_touchstone.y - parser for Touchstone files
 *
 * Copyright (C) 2003, 2005, 2006 Stefan Jahn <stefan@lkcc.org>
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
#include "check_touchstone.h"

using namespace qucs;

%}

%name-prefix="touchstone_"

%token InvalidCharacter
%token Float
%token Option
%token Eol

%union {
  char * ident;
  double f;
  qucs::vector * v;
  qucs::strlist * list;
}

%type <list> OptionList
%type <ident> Option
%type <f> Float
%type <v> DataLine Dataset

%%

Input:
  OptionLine Dataset { /* describes a valid touchstone */ }
;

OptionLine: /* option line */
    '#' OptionList 'R' Float OptionList Eol {
    touchstone_vector = NULL;
    touchstone_options.resistance = $4;
  }
  | '#' OptionList Eol {
    touchstone_vector = NULL;
    touchstone_options.resistance = 50.0;
  }
  | Eol OptionLine { /* skip this line */ }
;

OptionList: /* nothing */ { }
  | Option OptionList {
    if (touchstone_idents == NULL) touchstone_idents = new strlist ();
    touchstone_idents->add ($1);
    free ($1);
  }
;

Dataset: /* nothing */ { }
  | DataLine Eol Dataset { /* append vector lines */
    $1->setNext (touchstone_vector);
    touchstone_vector = $1;
  }
  | DataLine { /* last line, no trailing end-of-line */
    $1->setNext (touchstone_vector);
    touchstone_vector = $1;
    logprint (LOG_ERROR, "line %d: no trailing end-of-line found, "
	      "continuing...\n", touchstone_lineno);
  }
  | Eol Dataset { /* skip this line */ }
;

DataLine:
    Float Float Float {
    /* 1-port start */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
  }
  | Float Float Float Float Float {
    /* noise parameters */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
  }
  | Float Float Float Float Float Float Float Float Float {
    /* 2-port and 4- to n-port start */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
    $$->add ($6);
    $$->add ($7);
    $$->add ($8);
    $$->add ($9);
  }
  | Float Float Float Float Float Float Float {
    /* 3-port start */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
    $$->add ($6);
    $$->add ($7);
  }
  | Float Float Float Float Float Float Float Float {
    /* 4- and n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
    $$->add ($6);
    $$->add ($7);
    $$->add ($8);
  }
  | Float Float Float Float Float Float {
    /* 3- and n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
    $$->add ($6);
  }
  | Float Float Float Float {
    /* n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
  }
  | Float Float {
    /* n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
  }
;

%%

int touchstone_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", touchstone_lineno, error);
  return 0;
}
