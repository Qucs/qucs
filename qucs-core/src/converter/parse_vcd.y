/* -*-c++-*- */

%{
/*
 * parse_vcd.y - parser for a VCD data file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#include <ctype.h>

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 10000000

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
%token PositiveInteger
%token PositiveHugeInteger
%token Identifier     /* proper identifiers */
%token IdentifierCode /* shortcuts used in the dump */
%token Reference
%token InvalidCharacter

%union {
  char * ident;
  char * value;
  int integer;
  double real;
  enum vcd_vartypes vtype;
  enum vcd_scopes stype;
  struct vcd_vardef * vardef;
  struct vcd_change * change;
  struct vcd_scope * scope;
  struct vcd_changeset * changeset;
  struct vcd_range * range;
}

%type <ident> Identifier IdentifierCode Reference
%type <value> Value ZERO ONE Z X Binary Real
%type <integer> Size PositiveInteger TimeScale
%type <real> TimeUnit SimulationTime PositiveHugeInteger
%type <change> ScalarValueChange VectorValueChange ValueChangeList ValueChange
%type <changeset> ValueChangeset
%type <scope> ScopeDeclaration
%type <range> BitSelect
%type <vardef> VarDeclaration
%type <vtype> VarType
%type <stype> ScopeType

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
     if (!vcd->scopes) {
       /* no scope defined yet */
       vcd->scopes = (struct vcd_scope *)
	 calloc (1, sizeof (struct vcd_scope));
       vcd->scopes->ident = strdup (VCD_NOSCOPE);
       vcd->scopes->scopes = $2;
       $2->parent = vcd->scopes;
     } else {
       /* concatenate scope definitions */
       $2->next = vcd->currentscope->scopes;
       vcd->currentscope->scopes = $2;
       $2->parent = vcd->currentscope;
     }
     vcd->currentscope = $2;
   }
   | t_TIMESCALE TimeScaleDeclaration t_END
   | t_UPSCOPE t_END {
     if (vcd->currentscope->parent) {
       /* up one scope */
       vcd->currentscope = vcd->currentscope->parent;
     } else {
       fprintf (stderr, "vcd notice, unnecessary $upscope in line %d\n",
		vcd_lineno);
     }
   }
   | t_VERSION t_END
   | t_VAR VarDeclaration t_END {
     if (!vcd->scopes) {
       /* no scope defined yet */
       vcd->scopes = (struct vcd_scope *)
	 calloc (1, sizeof (struct vcd_scope));
       vcd->scopes->ident = strdup (VCD_NOSCOPE);
       vcd->currentscope = vcd->scopes;
     }
     /* concatenate variable definitions */
     $2->scope = vcd->currentscope;
     $2->next = vcd->currentscope->vardefs;
     vcd->currentscope->vardefs = $2;
   }
;

ScopeDeclaration:
   ScopeType Identifier {
     $$ = (struct vcd_scope *) calloc (1, sizeof (struct vcd_scope));
     $$->type = $1;
     $$->ident = $2;
   }
;

ScopeType:
     s_MODULE   { $$ = SCOPE_MODULE;   }
   | s_TASK     { $$ = SCOPE_TASK;     }
   | s_FUNCTION { $$ = SCOPE_FUNCTION; }
   | s_BEGIN    { $$ = SCOPE_BEGIN;    }
   | s_FORK     { $$ = SCOPE_FORK;     }
;

TimeScaleDeclaration:
   TimeScale TimeUnit {
     vcd->t = $1;
     vcd->scale = $2;
   }
;

TimeScale:
     ONE     { $$ = 1;   }
   | TEN     { $$ = 10;  }
   | HUNDRET { $$ = 100; }
;

TimeUnit:
     SECOND { $$ = 1;     }
   | MILLI  { $$ = 1e-3;  }
   | MICRO  { $$ = 1e-6;  }
   | NANO   { $$ = 1e-9;  }
   | PICO   { $$ = 1e-12; }
   | FEMTO  { $$ = 1e-15; }
;

VarDeclaration:
   VarType Size IdentifierCode Reference BitSelect {
     $$ = (struct vcd_vardef *) calloc (1, sizeof (struct vcd_vardef));
     $$->type = $1;
     $$->size = $2;
     $$->code = $3;
     $$->ident = $4;
     $$->range = $5;
   }
;

BitSelect: /* nothing */ { $$ = NULL; }
    | '[' PositiveInteger ']' {
      $$ = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      $$->l = -1;
      $$->h = $2;
    }
    | '[' PositiveInteger ':' PositiveInteger ']' {
      $$ = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      $$->l = $2;
      $$->h = $4;
    }
    | '(' PositiveInteger ')' {
      $$ = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      $$->l = $2;
      $$->h = -1;
    }
;

VarType:
     EVENT     { $$ = VAR_EVENT;
     /* a special type to synchronize different statement blocks */ }
   | INTEGER   { $$ = VAR_INTEGER;
     /* signed 32-bit variable */ }
   | PARAMETER { $$ = VAR_PARAMETER;
     /* a named constant - the default value of a parameter can be
	overwritten, when declaring an instance of the associated module */ }
   | REAL      { $$ = VAR_REAL;
     /* double-precision floating point */ }
   | REG       { $$ = VAR_REG;
     /* unsigned variable of any bit size */ }
   | SUPPLY0   { $$ = VAR_SUPPLY0;
     /* constant logic 0 (supply strength) */ }
   | SUPPLY1   { $$ = VAR_SUPPLY1;
     /* constant logic 1 (supply strength) */ }
   | TIME      { $$ = VAR_TIME;
     /* unsigned 64-bit variable */ }
   | TRI       { $$ = VAR_TRI;
     /* simple interconnecting wire */ }
   | TRIAND    { $$ = VAR_TRIAND;
     /* wired outputs AND together */ }
   | TRIOR     { $$ = VAR_TRIOR;
     /* wired outputs OR together */ }
   | TRIREG    { $$ = VAR_TRIREG;
     /* stores last value when tri-stated (capacitance strength) */ }
   | TRI0      { $$ = VAR_TRI0;
     /* pulls down when tri-stated */ }
   | TRI1      { $$ = VAR_TRI1;
     /* pulls up when tri-stated */ }
   | WAND      { $$ = VAR_WAND;
     /* wired outputs AND together */ }
   | WIRE      { $$ = VAR_WIRE;
     /* simple interconnecting wire */ }
   | WOR       { $$ = VAR_WOR;
     /* wired outputs OR together */ }
;

Size:
   PositiveInteger
;

SimulationCommandList: /* empty */
   | SimulationCommand SimulationCommandList
;

SimulationCommand:
    t_DUMPALL  ValueChangeList t_END /* probably unsupported */
  | t_DUMPOFF  ValueChangeList t_END /* probably unsupported */
  | t_DUMPON   ValueChangeList t_END /* probably unsupported */
  | t_DUMPVARS ValueChangeList t_END {
      vcd->changesets->changes = $2;
  }
  | ValueChangeset {
      $1->next = vcd->changesets;
      vcd->changesets = $1;
  }
;

ValueChangeset:
    SimulationTime ValueChangeList {
      $$ = (struct vcd_changeset *) calloc (1, sizeof (struct vcd_changeset));
      $$->t = $1;
      $$->changes = $2;
    }
;

SimulationTime:
    HASHMARK PositiveHugeInteger {
      $$ = $2;
    }
;

ValueChangeList: /* nothing */ { $$ = NULL; }
    | ValueChange ValueChangeList {
      $1->next = $2;
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
      $$->code = $2;
    }
;

Value:
    ZERO  /* low level       */
  | ONE   /* high level      */
  | X     /* undefined/error */
  | Z     /* high impedance  */
;

VectorValueChange:
    'B' Binary IdentifierCode {
      $$ = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      $$->value = $2;
      $$->code = $3;
    }
    | 'R' Real IdentifierCode {
      $$ = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      $$->value = $2;
      $$->code = $3;
      $$->isreal = 1;
    }
;


%%

int vcd_error (const char * error) {
  fprintf (stderr, "line %d: %s\n", vcd_lineno, error);
  return 0;
}
