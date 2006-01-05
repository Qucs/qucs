/* -*-c++-*- */

%{
/*
 * parse_vcd.y - parser for a VCD data file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
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
 * $Id: parse_vcd.y,v 1.2 2006-01-05 07:43:31 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#include "check_vcd.h"

%}

%name-prefix="vcd_"

%token t_END
%token t_COMMENT
%token t_DATE
%token t_ENDDEFINITIONS
%token t_SCOPE
%token t_TIMESCALE
%token t_UPSCOPE
%token t_VAR
%token t_VERSION
%token t_DUMPALL
%token t_DUMPOFF
%token t_DUMPON
%token t_DUMPVARS

%token s_MODULE s_TASK s_FUNCTION s_FORK s_BEGIN
%token ONE B Z ZERO HASHMARK X R TEN HUNDRET

%token PICO MICRO NANO FEMTO SECOND MILLI

%token EVENT INTEGER PARAMETER REAL REG SUPPLY0 SUPPLY1 TIME TRI TRIAND
%token TRIOR TRIREG TRI0 TRI1 WAND WIRE WOR

%token Real
%token Binary
%token DecimalNumber
%token Identifier     /* proper identifiers */
%token IdentifierCode /* shortcuts used in the dump */
%token Reference

%union {
  char * ident;
  char * value;
  int integer;
  struct vcd_vardef * vardef;
  struct vcd_change * change;
  struct vcd_scope * scope;
}

%type <ident> Identifier IdentifierCode VarType Reference
%type <ident> EVENT INTEGER PARAMETER REAL REG SUPPLY0 SUPPLY1 TIME TRI
%type <ident> TRIAND TRIOR TRIREG TRI0 TRI1 WAND WIRE WOR
%type <ident> ScopeType
%type <ident> s_MODULE s_TASK s_FUNCTION s_BEGIN s_FORK

%type <change> ScalarValueChange
%type <scope> ScopeDeclaration

%type <value> Value ZERO ONE Z X
%type <vardef> VarDeclaration
%type <integer> Size DecimalNumber

%%

ValueChangeDumpDefinitions:
     DeclarationList SimulationCommandList
;


DeclarationList: /* empty */
   | Declaration DeclarationList
;


Declaration:
     t_COMMENT t_END
   | t_DATE t_END
   | t_ENDDEFINITIONS t_END
   | t_SCOPE ScopeDeclaration t_END {
   }
   | t_TIMESCALE TimeScaleDeclaration t_END
   | t_UPSCOPE t_END
   | t_VERSION t_END
   | t_VAR VarDeclaration t_END
;


ScopeDeclaration:
     ScopeType Identifier {
       $$ = (struct vcd_scope *) calloc (1, sizeof (struct vcd_scope));
       $$->type = $1;
       $$->name = $2;
     }
;

ScopeType:
     s_MODULE
   | s_TASK
   | s_FUNCTION
   | s_BEGIN
   | s_FORK
;

TimeScaleDeclaration:
   TimeScale TimeUnit
;

TimeScale:
     ONE
   | TEN
   | HUNDRET
;

TimeUnit:
     SECOND
   | MILLI
   | MICRO
   | NANO
   | PICO
   | FEMTO
;

VarDeclaration:
   VarType Size IdentifierCode Reference {
     $$ = (struct vcd_vardef *) calloc (1, sizeof (struct vcd_vardef));
     $$->type = $1;
     $$->size = $2;
     $$->code = $3;
     $$->name = $4;
   }
;

VarType:
     EVENT
   | INTEGER
   | PARAMETER
   | REAL
   | REG
   | SUPPLY0
   | SUPPLY1
   | TIME
   | TRI
   | TRIAND
   | TRIOR
   | TRIREG
   | TRI0
   | TRI1
   | WAND
   | WIRE
   | WOR
;

Size:
   DecimalNumber
;


SimulationCommandList: /* empty */
   | SimulationCommand SimulationCommandList
;

SimulationCommand:
    t_DUMPALL  ValueChangeList t_END /* probably unsupported */
  | t_DUMPOFF  ValueChangeList t_END /* probably unsupported */
  | t_DUMPON   ValueChangeList t_END /* probably unsupported */
  | t_DUMPVARS ValueChangeList t_END
  | SimulationTime
  | ValueChange
;


SimulationTime:
     HASHMARK DecimalNumber
;

ValueChangeList: /* empty */
   | ValueChange ValueChangeList {
   }
;

ValueChange:
    ScalarValueChange
  | VectorValueChange
;

ScalarValueChange:
    Value IdentifierCode {
      $$ = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      $$->value = $1;
      $$->var = NULL; // find appropriate vardef
      //fprintf (stderr, "val: %s, ident: %s\n", $1, $2);
    }
;

Value:
    ZERO
  | ONE
  | X
  | Z
;

VectorValueChange:
     B Binary IdentifierCode
   | R Real IdentifierCode
;


%%

int vcd_error (char * error) {
  fprintf (stderr, "line %d: %s\n", vcd_lineno, error);
  return 0;
}
