/* -*-c-*- */

%{
/*
 * scan_mdl.l - scanner for an IC-CAP MDL data file
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
 * $Id: scan_mdl.l,v 1.1 2006/08/18 08:20:17 raimi Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef __MINGW32__
#include <io.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "check_mdl.h"
#include "tokens_mdl.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

%}

WS       [ \t\n\r]
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
INT      [+-]?{DIGIT}+
REAL     [+-]?{DIGIT}+("."{DIGIT}+)?{EXPONENT}?
DECIMAL  {DIGIT}+
IDENT    [a-zA-Z][a-zA-Z0-9_]*
DIGITS   {DIGIT}+
PIDENT   {IDENT}("."{IDENT})+

%x VERSION BLKEDIT BLKEDIT1 CIRCUIT CIRCUIT1

%option yylineno noyywrap nounput prefix="mdl_"

%%

<INITIAL>^"LINK" {
  return LINK;
}

<INITIAL>"MODEL"|"CIRC"|"PS"|"DUT"|"DPS"|"DAT"|"OUT"|"SWEEP"|"XFORM"|"MACRO"|"TCIRC"|"CONN"|"PLOT" {
  mdl_lval.ident = strdup (mdl_text);
  return t_LINK;
}

<INITIAL,BLKEDIT>\"[^\"]*\" {
  mdl_text[strlen (mdl_text) - 1] = '\0';
  mdl_text[0] = '\0';
  mdl_lval.ident = strdup (&mdl_text[1]);
  return String;
}

<INITIAL>^"View" {
  return t_VIEW;
}

<INITIAL>^"TABLE" {
  return t_TABLE;
}

<INITIAL>^"PSTABLE" {
  return t_PSTABLE;
}

<INITIAL>^"BLKEDIT" {
  BEGIN(BLKEDIT);
  return t_BLKEDIT;
}

<INITIAL>^"CNTABLE" {
  return t_CNTABLE;
}

<INITIAL>^"HYPTABLE" {
  return t_HYPTABLE;
}

<INITIAL>^"element" {
  return t_ELEMENT;
}

<INITIAL>^"data" {
  return t_DATA;
}

<INITIAL>^"dataset" {
  return t_DATASET;
}

<INITIAL>^"datasize" {
  return t_DATASIZE;
}

<INITIAL>^"point" {
  return t_POINT;
}

<INITIAL>^"member" {
  return t_MEMBER;
}

<INITIAL>^"list" {
  return t_LIST;
}

<INITIAL>^"PlotOptimizerOpt" {
  return t_PLOTOPTIMIZEROPT;
}

<INITIAL>^"PlotError" {
  return t_PLOTERROR;
}

<INITIAL>^"type" {
  return t_TYPE;
}

<INITIAL>^"param" {
  return t_PARAM;
}

<INITIAL>^"connpair" {
  return t_CONNPAIR;
}

<INITIAL>^"circuitdeck" {
  BEGIN(CIRCUIT);
  return t_CIRCUITDECK;
}

<INITIAL>{REAL} { /* identify float */
  mdl_lval.f = strtod (mdl_text, NULL);
  return Real;
}

<INITIAL>{IDENT}|{PIDENT} {
  mdl_lval.ident = strdup (mdl_text);
  return Identifier;
}

<BLKEDIT>"{"   { BEGIN(BLKEDIT1); return '{'; }
<CIRCUIT>"{"   { BEGIN(CIRCUIT1); return '{'; }

<INITIAL>"{"   { /* pass the '{' to the parser */ return '{'; }
<INITIAL>"}"   { /* pass the '{' to the parser */ return '}'; }

<*>\r?\n|{WS}  { /* skip end of line and spaces */ }

<BLKEDIT1,CIRCUIT1>"}"  { BEGIN(INITIAL); return '}'; }
<BLKEDIT1,CIRCUIT1>.    { /* ignore */ }

<*>. { /* any other character is invalid */
  fprintf (stderr, 
	   "line %d: syntax error, unrecognized character: `%s'\n", 
	   mdl_lineno, mdl_text);
  return InvalidCharacter;
}

%%
