/* -*-c-*- */

%{
/*
 * scanner.l - scanner for the Qucs netlist
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
 * $Id: scan_netlist.l,v 1.1.1.1 2003-12-20 19:03:25 ela Exp $
 *
 */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "logging.h"
#include "check_netlist.h"
#include "tokens_netlist.h"

%}

WS       [ \t\n\r]
ID       [a-zA-Z_][a-zA-Z0-9_]*
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
INT      [+-]?{DIGIT}+
FLOAT1   [+-]?{DIGIT}+{EXPONENT}
FLOAT2   [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?
PFX      ("dBm"|"dB"|"T"|"G"|"M"|"k"|"m"|"u"|"n"|"p"|"f"|"a")
UNT      ("Ohm"|"S"|"s"|"K"|"H"|"F"|"Hz"|"V"|"A"|"W"|"m")
EXCEPT   ("mm")
SU       ({PFX}|{UNT}|{PFX}{UNT}|{EXCEPT})
SPACE    [ \t]


%x COMMENT STR
%option yylineno noyywrap nounput prefix="netlist_"

%%

<INITIAL,STR>{SU} { /* identify scale and/or unit */
    netlist_lval.str = strdup (netlist_text);
    return ScaleOrUnit;
  }
<INITIAL,STR>{ID} { /* identify identifier */
    netlist_lval.ident = strdup (netlist_text); 
    return Identifier; 
  }
<INITIAL,STR>({INT}|{FLOAT1}|{FLOAT2}) { /* identify float or integer */
    netlist_lval.d = strtod (netlist_text, NULL);
    return Float;
  }
<INITIAL>{ID}{SPACE}*= {  /* identify 'identifier =' assign */
    int len = netlist_leng - 2;
    while (isspace (netlist_text[len])) len--;
    netlist_lval.ident = calloc (len + 2, 1);
    memcpy (netlist_lval.ident, netlist_text, len + 1);
    return Assign; 
  }

<INITIAL>"."   { /* pass the '.' to the parser */ return '.'; }
<INITIAL>":"   { /* pass the ':' to the parser */ return ':'; }
<INITIAL>"="   { /* pass the '=' to the parser */ return '='; }
<INITIAL>\r?\n { /* detect end of line */ return Eol; }

<INITIAL>{SPACE}|\\\r?\n /* skip spaces and the trailing '\' */

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
<STR>{SPACE} /* skip spaces */
<STR>. { /* any other character is invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      netlist_lineno, netlist_text);
    return InvalidCharacter;
  }

%%
