/* -*-c-*- */

%{
/*
 * scan_touchstone.l - scanner for Touchstone files
 *
 * Copyright (C) 2003, 2004 Stefan Jahn <stefan@lkcc.org>
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
 * $Id: scan_touchstone.l,v 1.2 2004-06-01 17:07:21 ela Exp $
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

#include "logging.h"
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "check_touchstone.h"
#include "tokens_touchstone.h"

%}

WS       [ \t\n\r]
ID       [a-zA-Z_][a-zA-Z0-9_]*
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
INT      [+-]?{DIGIT}+
FLOAT1   [+-]?{DIGIT}+{EXPONENT}
FLOAT2   [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?
SPACE    [ \t]

%x COMMENT
%option yylineno noyywrap nounput prefix="touchstone_"

%%

<INITIAL>R     { /* pass the 'R' to the parser */ return 'R'; }
<INITIAL>^#    { /* pass the leading '#' to the parser */ return '#'; }
<INITIAL>\r?\n { /* detect end of line */ return Eol; }

<*>{SPACE} /* skip spaces */

<INITIAL>{ID} { /* identify identifier */
    touchstone_lval.ident = strdup (touchstone_text); 
    return Option;
  }

<INITIAL>({FLOAT1}|{FLOAT2}|{INT}) { /* identify float */
    touchstone_lval.f = strtod (touchstone_text, NULL);
    return Float;
  }

<INITIAL>"!" { /* leave these characters */
    BEGIN(COMMENT);
  }

<INITIAL>. { /* any other character in invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      touchstone_lineno, touchstone_text);
    return InvalidCharacter;
  }

<COMMENT>.     { /* skip any character in here */ }
<COMMENT>\r?\n { BEGIN(INITIAL); /* skipping ends here */ return Eol; }

%%
