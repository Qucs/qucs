/* -*-c++-*- */

%{
/*
 * parse_spice.y - parser for a Spice netlist
 *
 * Copyright (C) 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: parse_spice.y,v 1.1 2004-10-29 18:01:29 ela Exp $
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

#include "check_spice.h"

%}

%name-prefix="spice_"

%token TitleLines
%token End
%token InvalidCharacter
%token Identifier
%token Capitals
%token Eol
%token Float
%token Spacer
%token Node
%token ModelStart
%token ModelEnd
%token Action

%union {
  char * ident;
  char * str;
  double d;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct value_t * value;
}

%type <ident> Identifier Node Capitals Action
%type <str> TitleLines Spacer Float
%type <definition> DefinitionLine
%type <value> NodeList PairList

%%

Input:
  End {
  }
  | InputList {
  }
  | TitleLines InputList {
  }
;

InputList: /* nothing */
  | InputLine Input
;

InputLine:
  DefinitionLine { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | Eol { /* nothing to do here */ }
;

DefinitionLine:
  Capitals NodeList PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    $$->values = value_root;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Identifier NodeList PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    $$->values = value_root;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Identifier NodeList PairList Model PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    $$->values = $3;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Identifier NodeList Identifier Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
  }
  | Action NodeList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
    $$->values = $2;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Action Identifier ModelList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
  }
  | Action Identifier Identifier PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
  }
  | Action Identifier Float NodeList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
  }
  | Action Identifier Node NodeList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = strdup (&$1[1]);
    $$->instance = $1;
  }
;

PairList: /* nothing */ { /* $$ = value_root = NULL; */ }
  | Identifier Spacer Float PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($3, NULL);
    $$->hint = HINT_PAIR;
    $$->next = value_root;
    value_root = $$;
  }
  | Identifier Float PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($2, NULL);
    $$->hint = HINT_PAIR;
    $$->next = value_root;
    value_root = $$;
  }
  | Identifier Spacer Node PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($3, NULL);
    $$->hint = HINT_PAIR;
    $$->next = value_root;
    value_root = $$;
  }
  | Identifier Node PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($2, NULL);
    $$->hint = HINT_PAIR;
    $$->next = value_root;
    value_root = $$;
  }
;

NodeList: /* nothing */ { /* $$ = value_root = NULL; */ }
  | Node NodeList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($1, NULL);
    $$->hint = HINT_NODE | HINT_NUMBER;
    $$->next = value_root;
    value_root = $$;
  }
  | Float NodeList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($1, NULL);
    $$->hint = HINT_NUMBER;
    $$->next = value_root;
    value_root = $$;
  }
;

Model:
    ModelStart Identifier ModelEnd {
  }
  | ModelStart PairList ModelEnd {
  }
  | ModelStart Node NodeList ModelEnd {
  }
;

ModelList: /* nothing */ { }
  | Model ModelList {
  }
;

%%

int spice_error (char * error) {
  fprintf (stderr, "line %d: %s\n", spice_lineno, error);
  return 0;
}
