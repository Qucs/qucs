/* -*-c-*- */

%{
/*
 * parser.y - parser for the Qucs netlist
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
 * $Id: parse_netlist.y,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
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

#include "check_netlist.h"
#include "logging.h"

%}

%name-prefix="netlist_"

%token InvalidCharacter
%token Identifier
%token Assign
%token Float
%token ScaleOrUnit
%token Eol

%union {
  char * ident;
  char * str;
  double d;
  struct definition_t * definition;
  struct node_t * node;
  struct pair_t * pair;
  struct value_t * value;
}

%type <ident> Identifier Assign
%type <str> ScaleOrUnit
%type <d> Float
%type <definition> DefinitionLine ActionLine
%type <node> IdentifierList
%type <pair> PairList
%type <value> String

%%

Input: /* nothing */
| InputLine Input
;

InputLine:
  ActionLine     { }
| DefinitionLine { }
| Eol            { }
;

ActionLine:
  '.' Identifier ':' Identifier PairList Eol { 
    $$ = calloc (sizeof (struct definition_t), 1);
    $$->action = 1;
    $$->type = $2;
    $$->instance = $4;
    $$->pairs = $5;
    $$->next = definition_root;
    definition_root = $$;
    pair_root = NULL;
  }
;

DefinitionLine:
  Identifier ':' Identifier IdentifierList PairList Eol { 
    $$ = calloc (sizeof (struct definition_t), 1);
    $$->action = 0;
    $$->type = $1;
    $$->instance = $3;
    $$->nodes = $4;
    $$->pairs = $5;
    $$->next = definition_root;
    definition_root = $$;
    node_root = NULL;
    pair_root = NULL;
  }
;

IdentifierList: /* nothing */ { $$ = node_root = NULL; }
  | Identifier IdentifierList {
    $$ = calloc (sizeof (struct node_t), 1);
    $$->node = $1;
    $$->next = node_root;
    node_root = $$;
  }
;

PairList: /* nothing */ { $$ = pair_root = NULL; }
  | Assign String PairList {
    $$ = calloc (sizeof (struct pair_t), 1);
    $$->key = $1;
    $$->value = $2;
    $$->next = pair_root;
    pair_root = $$;
  }    
;

String:
  Float {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $1;
  }
  | '"' Float '"' {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $2;
  }
  | Float ScaleOrUnit {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $1;
    $$->scale = $2;
  }
  | '"' Float ScaleOrUnit '"' {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $2;
    $$->scale = $3;
  }
  | Float ScaleOrUnit ScaleOrUnit {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $1;
    $$->scale = $2;
    $$->unit = $3;
  }
  | '"' Float ScaleOrUnit ScaleOrUnit '"' {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->value = $2;
    $$->scale = $3;
    $$->unit = $4;
  }
  | Identifier {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->ident = $1;
  }
  | '"' Identifier '"' {
    $$ = calloc (sizeof (struct value_t), 1);
    $$->ident = $2;
  }

;

%%

int netlist_error (char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", netlist_lineno, error);
  return 0;
}
