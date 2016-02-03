/* -*-c++-*- */

%{
/*
 * parse_dataset.y - parser for the Qucs dataset
 *
 * Copyright (C) 2003, 2004, 2006 Stefan Jahn <stefan@lkcc.org>
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
#define YYMAXDEPTH 10000000

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_dataset.h"

using namespace qucs;

%}

%name-prefix="dataset_"

%token InvalidCharacter
%token Identifier
%token REAL
%token IMAG
%token COMPLEX
%token Integer
%token Eol
%token IndepBegin
%token DepBegin
%token IndepEnd
%token DepEnd
%token Version

%union {
  char * ident;
  double f;
  struct {
    double r;
    double i;
  } c;
  long n;
  qucs::vector * v;
  qucs::dataset * data;
  qucs::strlist * list;
}

%type <list> IdentifierList
%type <ident> Identifier
%type <f> REAL IMAG
%type <c> COMPLEX
%type <n> Integer
%type <v> FloatList
%type <data> VersionLine VariableList Variable

%%

Input:
  VersionLine VariableList { /* describes a valid dataset */ }
;

VersionLine:
  Version Eol { /* version line */
    $$ = dataset_result = new dataset ();
  }
;

VariableList: /* nothing */ { }
  | Variable VariableList { /* dependent and independent variable vectors */ }
  | Eol VariableList      { /* skip to next line */ }
;

Variable:
  '<' DepBegin Identifier IdentifierList '>' FloatList '<' DepEnd '>' {
    /* dependent variable vector */
    dataset_vector->setName ($3);
    dataset_vector->reverse ();
    dataset_vector->setDependencies (dataset_idents);
    dataset_result->appendVariable (dataset_vector);
    dataset_vector = NULL;
    dataset_idents = NULL;
    free ($3);
  }
  | '<' IndepBegin Identifier Integer '>' FloatList '<' IndepEnd '>' {
    /* independent variable vector */
    dataset_vector->setRequested ($4);
    dataset_vector->setName ($3);
    dataset_vector->reverse ();
    dataset_result->appendDependency (dataset_vector);
    dataset_vector = NULL;
    free ($3);
  }
;

FloatList: /* nothing */ { $$ = dataset_vector = new vector (); }
  | REAL FloatList {
    dataset_vector->add ($1);
  }
  | COMPLEX FloatList {
    dataset_vector->add (nr_complex_t ($1.r, $1.i));
  }
  | IMAG FloatList {
    dataset_vector->add (nr_complex_t (0.0, $1));
  }
  | Eol FloatList { /* skip to next line */ }
;

IdentifierList: /* nothing */ { $$ = dataset_idents = new strlist (); }
  | Identifier IdentifierList {
    dataset_idents->add ($1);
    free ($1);
  }
;

%%

int dataset_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", dataset_lineno, error);
  return 0;
}
