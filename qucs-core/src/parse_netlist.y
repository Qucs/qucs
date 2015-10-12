/* -*-c++-*- */

%{
/*
 * parse_netlist.y - parser for the Qucs netlist
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#include "check_netlist.h"
#include "logging.h"
#include "equation.h"
#include "range.h"

using namespace qucs;

%}

%name-prefix="netlist_"

%token InvalidCharacter
%token Identifier
%token Assign
%token ScaleOrUnit
%token Eol
%token Eqn
%token DefSub
%token EndSub
%token REAL
%token IMAG
%token COMPLEX
%token Character
%token STRING

%right '='
%right '?' ':'
%left  Or
%left  And
%left  NotEqual Equal
%left  Less Greater LessOrEqual GreaterOrEqual
%left  '-' '+'
%left  '*' '/' '%'
%right Not
%left  NEG     /* unary negation */
%left  POS     /* unary non-negation */
%right '^'

%union {
  char * ident;
  char * str;
  double d;
  char chr;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct node_t * node;
  struct pair_t * pair;
  struct value_t * value;
  struct {
    double r;
    double i;
  } c;
  qucs::eqn::node * eqn;
  qucs::eqn::constant * con;
  qucs::eqn::reference * ref;
  qucs::eqn::application * app;
  qucs::eqn::assignment * assign;
}

%type <ident> Identifier Assign NodeIdentifier InstanceIdentifier
%type <str> ScaleOrUnit
%type <d> REAL IMAG
%type <c> COMPLEX
%type <chr> Character
%type <str> STRING
%type <definition> DefinitionLine ActionLine DefBody DefBodyLine EquationLine
%type <definition> InputList InputLine
%type <subcircuit> DefBegin SubcircuitBody
%type <node> NodeList
%type <pair> PairList
%type <value> PropertyValue ValueList Value PropertyReal
%type <eqn> EquationList Expression ExpressionList
%type <assign> Equation
%type <con> Constant
%type <ref> Reference
%type <app> Application Range Matrix Vector
%type <eqn> ExpressionMatrix ExpressionVector
%type <eqn> ExpressionRowList ExpressionCol ExpressionColList

%%

Input:
  InputList {
    definition_root = $1;
  }
;

InputList: /* nothing */ { $$ = NULL; }
  | InputLine InputList {
    if ($1) {
      $1->next = $2;
      $$ = $1;
    } else {
      $$ = $2;
    }
  }
;

InputLine:
  SubcircuitBody {
    $$ = $1;
  }
  | EquationLine
  | ActionLine
  | DefinitionLine
  | Eol {
    $$ = NULL;
  }
;

/* An action line in the netlist, i.e. the specification of the type
   of simulation to be performed  */
ActionLine:
  '.' Identifier ':' InstanceIdentifier PairList Eol {
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->action = PROP_ACTION;
    $$->type = $2;
    $$->instance = $4;
    $$->pairs = $5;
    $$->line = netlist_lineno;
  }
;

/* A definition line in the netlist, i.e. a component specification
   such as R:R1 _net0 gnd R="50 Ohm" Temp="26.85" Tc1="0.0" Tc2="0.0" Tnom="26.85"
 */
DefinitionLine:
  Identifier ':' InstanceIdentifier NodeList PairList Eol {
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->action = PROP_COMPONENT;
    $$->type = $1;
    $$->instance = $3;
    $$->nodes = $4;
    $$->pairs = $5;
    $$->line = netlist_lineno;
  }
;

InstanceIdentifier:
    Identifier  { $$ = $1; }
  | ScaleOrUnit { $$ = $1; }
;

NodeIdentifier:
    Identifier  { $$ = $1; }
  | ScaleOrUnit { $$ = $1; }
;

/* List of nodes for a component */
NodeList: /* nothing */ { $$ = NULL; }
  | NodeIdentifier NodeList {
    $$ = (struct node_t *) calloc (sizeof (struct node_t), 1);
    $$->node = $1;
    $$->next = $2;
  }
;

/* Assigns the list of key-value pairs x="y" */
PairList: /* nothing */ { $$ = NULL; }
  | Assign Value PairList {
    $$ = (struct pair_t *) calloc (sizeof (struct pair_t), 1);
    $$->key = $1;
    $$->value = $2;
    $$->next = $3;
  }
  | Assign NoneValue PairList {
    if (0) {
      $$ = (struct pair_t *) calloc (sizeof (struct pair_t), 1);
      $$->key = $1;
      $$->value = NULL;
      $$->next = $3;
    } else {
      free ($1);
      $$ = $3;
    }
  }
;

/* A empty value in a pair list */
NoneValue:  { /* nothing */ }
  | '"' '"' { /* also nothing */ }
;

/* A property value, can either be on its own or enclosed in quotes */
Value:
  PropertyValue {
    $$ = $1;
  }
  | '"' PropertyValue '"' {
    $$ = $2;
  }
;

PropertyReal:
  REAL {
    $$ = create_value ();
    $$->value = $1;
  }
  | REAL ScaleOrUnit {
    $$ = create_value ();
    $$->value = $1;
    $$->scale = $2;
  }
  | REAL ScaleOrUnit ScaleOrUnit {
    $$ = create_value ();
    $$->value = $1;
    $$->scale = $2;
    $$->unit = $3;
  }
;

PropertyValue:
  PropertyReal {
    $$ = $1;
  }
  | InstanceIdentifier {
    $$ = create_value ();
    $$->ident = $1;
  }
  | '[' InstanceIdentifier ']' {
    $$ = create_value ();
    $$->ident = $2;
  }
  | '[' ValueList ']' {
    $$ = $2;
  }
;

ValueList: /* nothing */ { $$ = NULL; }
  | PropertyReal {
    $1->next = NULL;
    $$ = $1;
  }
  | PropertyReal ';' ValueList {
    $1->next = $3;
    $$ = $1;
  }
;

EquationLine:
  Eqn ':' InstanceIdentifier Equation EquationList Eol {
    /* create equation definition */
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->type = strdup ("Eqn");
    $$->instance = $3;
    $$->action = PROP_ACTION;
    $$->line = netlist_lineno;
    $4->setInstance ($3);
    $4->setNext ($5);
    $4->applyInstance ();
    $$->eqns = $4;
  }
;

EquationList: /* nothing */ { $$ = NULL; }
  | Equation EquationList {
    $1->setNext ($2);
    $$ = $1;
  }
;

Equation:
  Assign '"' Expression '"' {
    $$ = new eqn::assignment ();
    $$->result = $1;
    $$->body = $3;
  }
;

Expression:
    Constant {
    $$ = $1;
  }
  | Reference {
    $$ = $1;
  }
  | Application {
    $$ = $1;
  }
  | '(' Expression ')' {
    $$ = $2;
  }
  | '[' Vector ']' {
    $$ = $2;
  }
  | '[' Matrix ']' {
    $$ = $2;
  }
;

ExpressionCol:
  Expression ExpressionColList {
    $1->appendNodes ($2);
    $$ = $1;
  }
;

ExpressionColList:  /* nothing */ { $$ = NULL; }
  | ',' Expression ExpressionColList {
    $2->appendNodes ($3);
    $$ = $2;
  }
;

ExpressionVector:
  ExpressionCol
;

Vector:
  ExpressionVector {
    $$ = new eqn::application ();
    $$->n = strdup ("vector");
    $$->nargs = $1->count ();
    $$->args = $1;
  }
;

ExpressionRowList:  /* nothing */ { $$ = NULL; }
  | ';' ExpressionCol ExpressionRowList {
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = ';';
    c->appendNodes ($2);
    $2->appendNodes ($3);
    $$ = c;
  }
;

ExpressionMatrix:
  ExpressionCol ';' ExpressionCol ExpressionRowList {
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = ';';
    c->appendNodes ($3);
    $3->appendNodes ($4);
    $1->appendNodes (c);
    $$ = $1;
  }
;

Matrix:
  ExpressionMatrix {
    $$ = new eqn::application ();
    $$->n = strdup ("matrix");
    $$->nargs = $1->count ();
    $$->args = $1;
  }
;

Constant:
    REAL {
    $$ = new eqn::constant (eqn::TAG_DOUBLE);
    $$->d = $1;
  }
  | IMAG {
    $$ = new eqn::constant (eqn::TAG_COMPLEX);
    $$->c = new nr_complex_t (0.0, $1);
  }
  | Character {
    $$ = new eqn::constant (eqn::TAG_CHAR);
    $$->chr = $1;
  }
  | STRING {
    $$ = new eqn::constant (eqn::TAG_STRING);
    $$->s = $1;
  }
;

Range:
  Expression ':' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("range");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | ':' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("range");
    $$->nargs = 2;
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = '?';
    c->append ($2);
    $$->args = c;
  }
  | Expression ':' {
    $$ = new eqn::application ();
    $$->n = strdup ("range");
    $$->nargs = 2;
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = '?';
    $1->append (c);
    $$->args = $1;
  }
  | ':' {
    $$ = new eqn::application ();
    $$->n = strdup ("range");
    $$->nargs = 2;
    eqn::constant * c1 = new eqn::constant (eqn::TAG_CHAR);
    eqn::constant * c2 = new eqn::constant (eqn::TAG_CHAR);
    c1->chr = '?';
    c2->chr = '?';
    c1->append (c2);
    $$->args = c1;
  }
;

Reference:
  Identifier {
    $$ = new eqn::reference ();
    $$->n = $1;
  }
;

Application:
    Identifier '(' ExpressionList ')' {
    $$ = new eqn::application ();
    $$->n = $1;
    $$->nargs = $3->count ();
    $$->args = $3;
  }
  | Reference '[' ExpressionList ']' {
    $$ = new eqn::application ();
    $$->n = strdup ("array");
    $$->nargs = 1 + $3->count ();
    $1->setNext ($3);
    $$->args = $1;
  }
  | Expression '+' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("+");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '-' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("-");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '*' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("*");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '/' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("/");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '%' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("%");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | '+' Expression %prec POS {
    $$ = new eqn::application ();
    $$->n = strdup ("+");
    $$->nargs = 1;
    $$->args = $2;
  }
  | '-' Expression %prec NEG {
    $$ = new eqn::application ();
    $$->n = strdup ("-");
    $$->nargs = 1;
    $$->args = $2;
  }
  | Expression '^' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("^");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression '?' Expression ':' Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("?:");
    $$->nargs = 3;
    $1->append ($3);
    $1->append ($5);
    $$->args = $1;
  }
  | Expression Less Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("<");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression Greater Expression {
    $$ = new eqn::application ();
    $$->n = strdup (">");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression GreaterOrEqual Expression {
    $$ = new eqn::application ();
    $$->n = strdup (">=");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression LessOrEqual Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("<=");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression Equal Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("==");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression NotEqual Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("!=");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Not Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("!");
    $$->nargs = 1;
    $$->args = $2;
  }
  | Expression And Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("&&");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
  | Expression Or Expression {
    $$ = new eqn::application ();
    $$->n = strdup ("||");
    $$->nargs = 2;
    $1->append ($3);
    $$->args = $1;
  }
;

ExpressionList: /* nothing */ { $$ = NULL; }
  | Expression {
    $$ = $1;
  }
  | Range {
    $$ = $1;
  }
  | Expression ',' ExpressionList {
    $1->setNext ($3);
    $$ = $1;
  }
  | Range ',' ExpressionList {
    $1->setNext ($3);
    $$ = $1;
  }
;

SubcircuitBody:
  DefBegin DefBody DefEnd { /* a full subcircuit definition found */
    $1->sub = $2;
    $$ = $1;
    $2 = NULL;
  }
;

DefBegin:
  DefSub InstanceIdentifier NodeList PairList Eol {
    /* create subcircuit definition right here */
    $$ = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    $$->type = strdup ("Def");
    $$->instance = $2;
    $$->nodes = $3;
    $$->pairs = $4;
    $$->action = PROP_ACTION;
    $$->line = netlist_lineno;
  }
;

DefBody: /* nothing */ { $$ = NULL; }
  | DefBodyLine DefBody { /* chain definitions here */
    if ($1) {
      $1->next = $2;
      $$ = $1;
    }
    else {
      $$ = $2;
    }
  }
;

DefEnd:
  EndSub Eol { /* nothing to do */ }
;

DefBodyLine:
  DefinitionLine { /* chain definitions here */
    $1->next = $$;
    $$ = $1;
  }
  | EquationLine { /* chain definitions here */
    $1->next = $$;
    $$ = $1;
  }
  | SubcircuitBody { /* do nothing here, see subcircuit rule */ }
  | Eol {
    $$ = NULL;
  }
;


%%

int netlist_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", netlist_lineno, error);
  return 0;
}
