/* -*-c-*- */

%{
/*
 * scan_spice.l - scanner for a Spice netlist
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
 * $Id: scan_spice.l,v 1.7 2004/11/10 20:26:38 ela Exp $
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

#include "check_spice.h"
#include "tokens_spice.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

%}

WS       [ \t\n\r]
TITLE    [A-Za-z][A-Za-z\- \t]*\r?\n
SPACE    [ \t]
NAME     [A-Z][A-Z0-9]*
IDENT    [A-Za-z][A-Za-z0-9]*
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
INT      [+-]?{DIGIT}+
FLOAT1   [+-]?{DIGIT}+{EXPONENT}
FLOAT2   [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?
FLOAT3   [+-]?{DIGIT}+"."
NUMBER   ({INT}|{FLOAT1}|{FLOAT2}|{FLOAT3}){SFX}?[A-Za-z]*
SFX      ("T"|"G"|"M"|"K"|"m"|"u"|"n"|"p"|"f"|"mil"|"g"|"k")
NODE     {DIGIT}+
EOL      \r?\n
FUN1     ([dD][eE][cC]|[lL][iI][nN]|[oO][cC][tT])
FUN2     ([sS][iI][nN]|[pP][uU][lL][sS][eE]|[aA][cC][cC][tT]|[pP][wW][lL])
FUNCTION ({FUN1}|{FUN2})

%x COMMENT
%option yylineno noyywrap nounput prefix="spice_"

%%

<INITIAL>^{SPACE}+ { /* Spice3 ignores any lines starting spaces */
    BEGIN(COMMENT);
  }

<INITIAL>^"*" { /* ignored region begines here */
    BEGIN(COMMENT);
  }

<INITIAL>^{TITLE}+ {
    spice_lval.str = strdup (spice_text);
    return TitleLines;
}

<INITIAL>^"."[sS][uU][bB][cC][kK][tT] {
    return StartSub;
}

<INITIAL>^"."[eE][nN][dD][sS] {
    return StopSub;
}

<INITIAL>"."[Ee][Nn][Dd]{EOL}* { /* identify end of netlist */
    return End;
}

<INITIAL>{NODE} { /* identify node */
    spice_lval.ident = strdup (spice_text);
    return Node;
}

<INITIAL>{NUMBER} { /* identify float */
    spice_lval.ident = strdup (spice_text);
    return Float;
  }

<INITIAL>"."{IDENT} {
    spice_lval.ident = strdup (&spice_text[1]);
    return Action;
}

<INITIAL>{FUNCTION} {
    spice_lval.ident = strdup (spice_text);
    return Function;
}

<INITIAL>{IDENT} {
    spice_lval.ident = strdup (spice_text);
    return Identifier;
}

<INITIAL>\r?\n { /* detect end of line */ return Eol; }

<INITIAL>{SPACE}|\r?\n\+|[()=,] {
    /* skip spaces, '=', '(', ')', ',' and the leading '+' */
  }

<INITIAL>. { /* any other character is invalid */
    fprintf (stderr, 
	     "line %d: syntax error, unrecognized character: `%s'\n", 
	     spice_lineno, spice_text);
    return InvalidCharacter;
  }

<COMMENT>. { /* skip any character in here */ }
<COMMENT>\r?\n { BEGIN(INITIAL); /* skipping ends here */ }

%%
