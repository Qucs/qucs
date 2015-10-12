/* -*-c++-*- */

%{
/*
 * parse_citi.y - parser for CITIfiles
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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
#include "check_citi.h"

using namespace qucs;

%}

%name-prefix="citi_"

%token InvalidCharacter
%token Float
%token Eol
%token DATA
%token VAR
%token NAME
%token Begin
%token End
%token Version
%token Identifier
%token Integer
%token CITIFILE
%token VarType
%token SegListBegin
%token VarListBegin
%token SegListEnd
%token VarListEnd
%token COMMENT
%token CONSTANT
%token SEG

%union {
  char * ident;
  double f;
  int d;
  qucs::vector * v;
  struct citi_package_t * p;
  struct citi_header_t * h;
}

%type <p> Package PackageList
%type <h> HeaderLine Header HeaderList
%type <d> Integer
%type <f> Float
%type <ident> Identifier VarType
%type <v> List VarList Data FloatList DataList

%%

Input:
  PackageList {
    citi_root = $1; /* describes a valid CITIfile */
  }
;

PackageList: /* nothing */ { $$ = NULL; }
  | Package PackageList {
    if ($1) {
      $1->next = $2;
      $$ = $1;
    } else {
      $$ = $2;
    }
  }
  | Eol PackageList {
    $$ = $2;
  }
;

Package:
  Header DataList {
    $$ = (struct citi_package_t *) calloc (sizeof (struct citi_package_t), 1);
    $$->head = $1;
    $$->data = $2;
  }
;

Header:
  CITIFILE Version Eol HeaderList {
    $$ = $4;
  }
;

HeaderList: { $$ = NULL; }
  | HeaderLine HeaderList {
    if ($1) {
      $1->next = $2;
      $$ = $1;
    } else {
      $$ = $2;
    }
  }
;

HeaderLine:
  NAME Identifier Eol {
    $$ = (struct citi_header_t *) calloc (sizeof (struct citi_header_t), 1);
    $$->package = $2;
  }
  | VAR Identifier VarType Integer Eol {
    $$ = (struct citi_header_t *) calloc (sizeof (struct citi_header_t), 1);
    $$->var = $2;
    $$->type = $3;
    $$->n = $4;
    $$->i1 = $$->i2 = -1;
  }
  | DATA Identifier VarType Eol {
    $$ = (struct citi_header_t *) calloc (sizeof (struct citi_header_t), 1);
    $$->var = $2;
    $$->type = $3;
    $$->n = $$->i1 = $$->i2 = -1;
  }
  | DATA Identifier '[' Integer ',' Integer ']' VarType Eol {
    $$ = (struct citi_header_t *) calloc (sizeof (struct citi_header_t), 1);
    $$->var = $2;
    $$->i1 = $4;
    $$->i2 = $6;
    $$->type = $8;
    $$->n = -1;
  }
  | DATA Identifier '[' Integer ']' VarType Eol {
    $$ = (struct citi_header_t *) calloc (sizeof (struct citi_header_t), 1);
    $$->var = $2;
    $$->i1 = $4;
    $$->type = $6;
    $$->n = $$->i2 = -1;
  }
  | CONSTANT Identifier Float ValueList Eol {
    /* ignore constants */
  }
;

ValueList: /* nothing */ { }
  | Float ValueList {
  }
;

List:
  SegListBegin Eol SEG Float Float Float Eol SegListEnd Eol {
    $$ = new vector (qucs::linspace ($5, $4, (int) $6));
  }
  | VarListBegin Eol VarList VarListEnd Eol {
    $$ = $3;
  }
;

DataList: { $$ = NULL; }
  | Data DataList {
    if ($1) {
      $1->setNext ($2);
      $$ = $1;
    } else {
      $$ = $2;
    }
  }
;

Data:
  Begin Eol FloatList End Eol {
    $$ = $3;
  }
  | List {
    $$ = $1;
  }
;

FloatList: { $$ = new vector (); }
  | Float Eol FloatList {
    $3->add ($1);
    $$ = $3;
  }
  | Float ',' Float Eol FloatList {
    $5->add (nr_complex_t ($1, $3));
    $$ = $5;
  }
;

VarList: { $$ = new vector (); }
  | Float Eol VarList {
    $3->add ($1);
    $$ = $3;
  }
;

%%

int citi_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", citi_lineno, error);
  return 0;
}
