/* -*-c++-*- */

%{
/*
 * parse_vcd.y - parser for a VCD data file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
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
 * $Id: parse_vcd.y,v 1.1 2005/12/26 21:13:31 raimi Exp $
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

%}

%name-prefix="spice_"

%token END
%token COMMENT
%token DATE
%token ENDDEFINITIONS
%token SCOPE
%token TIMESCALE
%token UPSCOPE
%token VAR
%token VERSION
%token DUMPALL
%token DUMPOFF
%token DUMPON
%token DUMPVARS

%token RealNumber
%token BinaryNumber
%token DecimalNumber
%token Identifier /* proper identifiers */
%token IdentifierCode /* shortcuts used in the dump */
%token CommentText // start with $date/$comment, consume until $end, trim

%union {

}

// %type

%%

ValueChangeDumpDefinitions:
     DeclarationList SimulationCommandList
;


DeclarationList: /* empty */
   | Declaration DeclarationList
;


Declaration:
     COMMENT CommentText END
   | DATE CommentText END
   | ENDDEFINITIONS END
   | SCOPE ScopeDeclaration END
   | TIMESCALE TimeScaleDeclaration END
   | UPSCOPE END
   | VERSION CommentTextOrEmpty END
   | VAR VarDeclaration END
;


ScopeDeclaration:
   ScopeType Identifier
;

ScopeType:
     MODULE
   | TASK
   | FUNCTION
   | BEGIN
   | FORK
;

TimeScaleDeclaration:
   Integer /* actually only 1, 10, 100 */ TimeUnit
;

TimeUnit:
   SECOND
   MILLI
   MICRO
   NANO
   PICO
   FEMTO
;

VarDeclaration:
   VarType Size IdentifierCode Reference
;

VarType:
     EVENT
   | INTEGER
   | PARAMETER
   | REAl
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


Reference:
    Identifier
  | Identifier BitSelect /* [0] */
  | Identifier BitRangeSelect /* [15:13] */
;


SimulationCommand:
    DUMPALL ValueChangeList END /* probably unsupported */
  | DUMPOFF ValueChangeList END /* probably unsupported */
  | DUMPON ValueChangeList END /* probably unsupported */
  | DUMPVARS ValueChangeList END // important !
  | COMMENT CommentText END
  | SimulationTime  // a timestep, it seems
  | ValueChange
;


SimulationKeyword:
;

SimulationTime:
   HASHMARK DecimalNumber
;

ValueChange:
    ScalarValueChange
  | VectorValueChange
;

ScalarValueChange:
  Value Identifier
;

Value:
    ZERO
  | ONE
  | X
  | Z
;

VectorValueChange:
     B BinaryNumber Identifier
   | R RealNumber Identifier
;


%%
