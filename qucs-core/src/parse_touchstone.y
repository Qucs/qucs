/* -*-c++-*- */

%{
/*
 * parse_touchstone.y - parser for Touchstone Files
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
 * $Id: parse_touchstone.y,v 1.1 2003/12/20 19:03:27 ela Exp $
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

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "strlist.h"
#include "check_touchstone.h"

%}

%name-prefix="touchstone_"

%token InvalidCharacter
%token Float
%token Option
%token Eol

%union {
  char * ident;
  double f;
  vector * v;
  strlist * list;
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
  | DataLine Dataset { /* append vector lines */
    $1->setNext (touchstone_vector);
    touchstone_vector = $1;
  }
  | Eol Dataset { /* skip this line */ }
;

DataLine:
    Float Float Float Eol { 
    /* 1-port start */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);    
  }
  | Float Float Float Float Float Float Float Float Float Eol {
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
  | Float Float Float Float Float Float Float Eol {
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
  | Float Float Float Float Float Float Float Float Eol { 
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
  | Float Float Float Float Float Float Eol { 
    /* 3- and n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
    $$->add ($5);
    $$->add ($6);
  }
  | Float Float Float Float Eol { 
    /* n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
    $$->add ($3);
    $$->add ($4);
  }
  | Float Float Eol { 
    /* n-port continued */
    $$ = new vector ();
    $$->add ($1);
    $$->add ($2);
  }
;

%%

int touchstone_error (char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", touchstone_lineno, error);
  return 0;
}
