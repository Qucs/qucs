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
 * $Id: parse_spice.y,v 1.7 2004/11/10 20:26:38 ela Exp $
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
%token Eol
%token Float
%token Node
%token Action
%token Function
%token StartSub
%token StopSub

%union {
  char * ident;
  char * str;
  double d;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct value_t * value;
}

%type <ident> Identifier Node Action Function Value Float
%type <str> TitleLines
%type <definition> DefinitionLine BeginSub SubBody Subcircuit SubBodyLine
%type <value> NodeList PairList Model Expr DcList

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
  Subcircuit   { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | DefinitionLine { /* chain definition root */
    $1->next = definition_root;
    definition_root = $1;
  }
  | Eol { /* nothing to do here */ }
;

DefinitionLine:
  Identifier NodeList PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    $$->values = netlist_append_values ($2, $3);
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Identifier NodeList Identifier PairList Eol { 
    /* e.g. M5 5 7 3 0 MOD W=50U L=5U */
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    struct value_t * here = create_value ();
    here->ident = $3;
    here->hint = HINT_NAME;
    here = netlist_append_values ($2, here);
    here = netlist_append_values (here, $4);
    $$->values = here;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Identifier NodeList PairList Expr PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_COMPONENT;
    $$->type = (char *) calloc (2, 1);
    $$->type[0] = $1[0];
    $$->instance = $1;
    struct value_t * here = netlist_append_values ($2, $3);
    here = netlist_append_values (here, $4);
    here = netlist_append_values (here, $5);
    $$->values = here;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Action NodeList Eol { 
    /* e.g. .TRAN 2.0NS 200NS */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = strdup ($1);
    $$->values = $2;
    $$->line = spice_lineno;
    value_root = NULL;
  }
  | Action Identifier Model Eol {
    /* e.g. .MODEL QNL NPN BF=80 RB=100 CCS=2PF */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $2;
    $$->line = spice_lineno;
    $$->values = $3;
   }
  | Action Expr Eol { 
    /* e.g. .AC DEC 10 1 10GHZ */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = strdup ($1);
    $$->line = spice_lineno;
    $$->values = $2;
  }
  | Action DcList Eol { 
    /* e.g. .DC VIN -0.25 0.25 0.005 */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = strdup ($1);
    $$->line = spice_lineno;
    $$->values = $2;
  }
  | Action DcList DcList Eol { 
    /* e.g. .DC VCE 0 10 .25 IB 0 10U 1U */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = strdup ($1);
    $$->line = spice_lineno;
    $$->values = netlist_append_values ($2, $3);
  }
  | Action Identifier Identifier Identifier Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
  | Action Identifier PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
  | Action Identifier Value Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
  | Action Node NodeList Identifier Identifier Eol { 
    /* e.g. .PZ 1 0 3 0 CUR POL */
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
  | Action Expr Identifier Expr Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
  | Action Identifier Value Expr PairList Eol { 
    $$ = create_definition ();
    $$->action = PROP_ACTION;
    $$->type = $1;
    $$->instance = $1;
    $$->line = spice_lineno;
  }
;

DcList:
  Identifier Value Value Value {
    struct value_t * here;
    here = create_value ();
    here->ident = $4;
    here->value = strtod ($4, NULL);
    here->hint = HINT_NUMBER | HINT_MSTOP;
    $$ = here;
    here = create_value ();
    here->ident = $3;
    here->value = strtod ($3, NULL);
    here->hint = HINT_NUMBER;
    here->next = $$;
    $$ = here;
    here = create_value ();
    here->ident = $2;
    here->value = strtod ($2, NULL);
    here->hint = HINT_NUMBER;
    here->next = $$;
    $$ = here;
    here = create_value ();
    here->ident = $1;
    here->hint = HINT_NAME | HINT_MSTART;
    here->next = $$;
    $$ = here;
  }
;

Value: Float | Node;

PairList: /* nothing */ { $$ = NULL; }
  | Identifier Value PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->unit = $2;
    $$->hint = HINT_PAIR;
    $$->next = $3;
  }
;

NodeList: /* nothing */ { $$ = NULL; }
  | Node NodeList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($1, NULL);
    $$->hint = HINT_NODE | HINT_NUMBER;
    $$->next = $2;
  }
  | Float NodeList {
    $$ = create_value ();
    $$->ident = $1;
    $$->value = strtod ($1, NULL);
    $$->hint = HINT_NUMBER;
    $$->next = $2;
  }
;

Expr:
  Function NodeList {
    $$ = create_value ();
    $$->ident = $1;
    $$->hint = HINT_NAME | HINT_MSTART;
    spice_set_last_hint ($2, HINT_MSTOP);
    $$->next = $2;    
  }
;

Model:
  Identifier PairList {
    $$ = create_value ();
    $$->ident = $1;
    $$->hint = HINT_NAME | HINT_MSTART;
    spice_set_last_hint ($2, HINT_MSTOP);
    $$->next = $2;
  }
;

Subcircuit:
  BeginSub SubBody EndSub {
    $1->sub = $2;
    $$ = $1;
    $2 = NULL;
  }
;

BeginSub:
  StartSub Identifier NodeList Eol {
    $$ = create_definition ();
    $$->type = strdup ("Def");
    $$->instance = $2;
    $$->values = $3;
    $$->action = PROP_ACTION;
    $$->line = spice_lineno;
  }
;

SubBody: /* nothing */ { $$ = NULL; }
  | SubBodyLine SubBody { /* chain definitions here */
    if ($1) {
      $1->next = $2;
      $$ = $1;
    }
  }
;

EndSub:
  StopSub Eol { /* nothing to do */ }
;

SubBodyLine:
  DefinitionLine { /* chain definitions here */
    $1->next = $$;
    $$ = $1;
  }
  | Subcircuit { /* do nothing here, see subcircuit rule */ }
  | Eol {
    $$ = NULL;
  }
;

%%

int spice_error (char * error) {
  fprintf (stderr, "line %d: %s\n", spice_lineno, error);
  return 0;
}
