/* -*-c-*- */

%{
/*
 * scan_netlist.l - scanner for the Qucs netlist
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
 * $Id: scan_netlist.l,v 1.20 2007/04/25 18:47:36 ela Exp $
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

#include "logging.h"
#include "equation.h"
#include "check_netlist.h"
#include "tokens_netlist.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

%}

WS       [ \t\n\r]
SIMPLEID [a-zA-Z_][a-zA-Z0-9_]*
POSTID   "."[a-zA-Z0-9_]+
ID       {SIMPLEID}{POSTID}*
FILE     "{"[^\t\n\r\}]+"}"
CHR      "'"[^\n\r\']{1}"'"
STR      "'"[^\n\r\']*"'"
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
PFX      ("dBm"|"dB"|"T"|"G"|"M"|"k"|"m"|"u"|"n"|"p"|"f"|"a")
UNT      ("Ohm"|"S"|"s"|"K"|"H"|"F"|"Hz"|"V"|"A"|"W"|"m")
EXCEPT   ("mm")
SU       ({PFX}|{UNT}|{PFX}{UNT}|{EXCEPT})
SPACE    [ \t]
RINT     [+-]?{DIGIT}+
IINT     [+-]?[ij]{1}{DIGIT}*
RFLOAT1  [+-]?{DIGIT}+{EXPONENT}
RFLOAT2  [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?
IFLOAT1  [+-]?[ij]{1}{DIGIT}+{EXPONENT}
IFLOAT2  [+-]?[ij]{1}{DIGIT}*"."{DIGIT}+({EXPONENT})?
CREAL    ({RFLOAT1}|{RFLOAT2}|{RINT})
CIMAG    ({IFLOAT1}|{IFLOAT2}|{IINT})
COMPLEX  {CREAL}{CIMAG}
URINT    {DIGIT}+
UIINT    [ij]{1}{DIGIT}*
URFLOAT1 {DIGIT}+{EXPONENT}
URFLOAT2 {DIGIT}*"."{DIGIT}+({EXPONENT})?
UIFLOAT1 [ij]{1}{DIGIT}+{EXPONENT}
UIFLOAT2 [ij]{1}{DIGIT}*"."{DIGIT}+({EXPONENT})?
UCREAL   ({URFLOAT1}|{URFLOAT2}|{URINT})
UCIMAG   ({UIFLOAT1}|{UIFLOAT2}|{UIINT})
UCOMPLEX {UCREAL}{UCIMAG}


%x COMMENT STR EQN
%option yylineno noyywrap nounput prefix="netlist_"

%%

<INITIAL,STR>{SU} { /* identify scale and/or unit */
    netlist_lval.str = strdup (netlist_text);
    return ScaleOrUnit;
  }
<INITIAL>"Eqn" { /* special equation case */
    BEGIN(EQN);
    return Eqn;
  }
<INITIAL>"."{SPACE}*"Def"{SPACE}*":" {
    /* subcircuit definition begins */
    return DefSub;
  }
<INITIAL>"."{SPACE}*"Def"{SPACE}*":"{SPACE}*"End" {
    /* subcircuit definition ends */
    return EndSub;
  }
<INITIAL,STR>{ID} { /* identify identifier */
    netlist_lval.ident = strdup (netlist_text);
    return Identifier;
  }
<INITIAL,STR>{FILE} { /* identify file reference */
    char * p = strrchr (netlist_text, '}'); *p = '\0';
    netlist_lval.ident = strdup (&netlist_text[1]);
    return Identifier;
  }
<INITIAL,STR>{CREAL} { /* identify (signed) real float */
    netlist_lval.d = strtod (netlist_text, NULL);
    return REAL;
  }
<INITIAL,STR>{CIMAG} { /* identify (signed) imaginary float */
    if (netlist_text[0] == 'i' || netlist_text[0] == 'j')
      netlist_text[0] = (netlist_text[1] == '\0') ? '1' : '0';
    else
      netlist_text[1] = '0';
    netlist_lval.d = strtod (netlist_text, NULL);
    return IMAG;
  }
<INITIAL,STR>{COMPLEX} { /* identify complete (signed) complex number */
    int i = 0;
    while (netlist_text[i] != 'i' && netlist_text[i] != 'j') i++;
    netlist_text[i] = netlist_text[i - 1];
    netlist_text[i - 1] = '\0';
    netlist_lval.c.r = strtod (netlist_text, NULL);
    netlist_lval.c.i = strtod (&netlist_text[i], NULL);
    return COMPLEX;
  }
<INITIAL,EQN>{ID}{SPACE}*= {  /* identify 'identifier =' assign */
    int len = netlist_leng - 2;
    while (isspace (netlist_text[len])) len--;
    netlist_lval.ident = (char *) calloc (len + 2, 1);
    memcpy (netlist_lval.ident, netlist_text, len + 1);
    return Assign; 
  }

<INITIAL,STR>"[" { /* special token for the value list */ return '['; }
<INITIAL,STR>"]" { /* special token for the value list */ return ']'; }
<INITIAL,STR>";" { /* special token for the value list */ return ';'; }

<INITIAL>"."   { /* pass the '.' to the parser */ return '.'; }
<INITIAL>":"   { /* pass the ':' to the parser */ return ':'; }
<INITIAL>"="   { /* pass the '=' to the parser */ return '='; }
<INITIAL>\r?\n { /* detect end of line */ return Eol; }

<INITIAL,EQN>{SPACE}|\\\r?\n /* skip spaces and the trailing '\' */

<INITIAL>"#" { /* leave these characters */
    BEGIN(COMMENT);
  }
<INITIAL>\" { /* string constant starts here */
    BEGIN(STR);  
    return '"';
  }
<INITIAL>. { /* any other character in invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      netlist_lineno, netlist_text);
    return InvalidCharacter;
  }

<COMMENT>. { /* skip any character in here */ }
<COMMENT>\r?\n { BEGIN(INITIAL); /* skipping ends here */ }

<STR>\" { /* string constant ends here */
    BEGIN(INITIAL); 
    return '"';
  }
<STR>\r?\n { /* string in a single line only */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unterminated string constant\n",
	      netlist_lineno);
    return Eol;
  }
<STR,EQN>{SPACE} /* skip spaces */

<STR>. { /* any other character is invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      netlist_lineno, netlist_text);
    return InvalidCharacter;
  }

<EQN>[-+*/%(),^:\"\[\]\?] { /* return operators unchanged */
    return netlist_text[0];
  }

<EQN>">=" { return GreaterOrEqual; }
<EQN>"<=" { return LessOrEqual; }
<EQN>"!=" { return NotEqual; }
<EQN>"==" { return Equal; }
<EQN>"&&" { return And; }
<EQN>"||" { return Or; }
<EQN>"<"  { return Less; }
<EQN>">"  { return Greater; }
<EQN>"!"  { return Not; }

<EQN>{UCREAL} { /* identify unsigned real float */
    netlist_lval.d = strtod (netlist_text, NULL);
    return REAL;
  }
<EQN>{UCIMAG} { /* identify unsigned imaginary float */
    if (netlist_text[0] == 'i' || netlist_text[0] == 'j')
      netlist_text[0] = (netlist_text[1] == '\0') ? '1' : '0';
    else
      netlist_text[1] = '0';
    netlist_lval.d = strtod (netlist_text, NULL);
    return IMAG;
  }
<EQN>{ID} { /* identify identifier */
    netlist_lval.ident = strdup (netlist_text);
    return Identifier;
  }
<EQN>{CHR} {
    netlist_lval.chr = netlist_text[1];
    return Character;
  }
<EQN>{STR} {
    netlist_lval.str = strdup (&netlist_text[1]);
    netlist_lval.str[strlen (netlist_lval.str) - 1] = '\0';
    return STRING;
  }
<EQN>\r?\n { /* detect end of line */ BEGIN(INITIAL); return Eol; }

<EQN>. { /* any other character in invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      netlist_lineno, netlist_text);
    return InvalidCharacter;
  }

%%
