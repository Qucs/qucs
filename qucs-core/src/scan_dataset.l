/* -*-c-*- */

%{
/*
 * scan_dataset.l - scanner for the Qucs dataset
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
 * $Id: scan_dataset.l,v 1.1.1.1 2003-12-20 19:03:26 ela Exp $
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
#include "complex.h"
#include "object.h"
#include "vector.h"
#include "dataset.h"
#include "check_dataset.h"
#include "tokens_dataset.h"

%}

WS       [ \t\n\r]
ID       [a-zA-Z_][a-zA-Z0-9_]*
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
RINT     [+-]?{DIGIT}+
IINT     [+-]?[ij]{1}{DIGIT}+
RFLOAT1  [+-]?{DIGIT}+{EXPONENT}
RFLOAT2  [+-]?{DIGIT}*"."{DIGIT}+({EXPONENT})?
IFLOAT1  [+-]?[ij]{1}{DIGIT}+{EXPONENT}
IFLOAT2  [+-]?[ij]{1}{DIGIT}*"."{DIGIT}+({EXPONENT})?
SPACE    [ \t]
VERSION  "<Qucs Dataset "{DIGIT}+"."{DIGIT}+"."{DIGIT}+">"
DBEGIN   "dep"
IBEGIN   "indep"
DEND     "/dep"
IEND     "/indep"


%x COMMENT DESCRIPTION
%option yylineno noyywrap nounput prefix="dataset_"

%%

<INITIAL>{VERSION} {
    return Version;
  }

<DESCRIPTION>{DBEGIN} {
    return DepBegin;
  }

<DESCRIPTION>{IBEGIN} {
    return IndepBegin;
  }

<DESCRIPTION>{DEND} {
    return DepEnd;
  }

<DESCRIPTION>{IEND} {
    return IndepEnd;
  }

<INITIAL,DESCRIPTION>{ID} { /* identify identifier */
    dataset_lval.ident = strdup (dataset_text); 
    return Identifier; 
  }

<INITIAL>({RFLOAT1}|{RFLOAT2}|{RINT}) { /* identify float */
    dataset_lval.f = strtod (dataset_text, NULL);
    return REAL;
  }

<INITIAL>({IFLOAT1}|{IFLOAT2}|{IINT}) { /* identify float */
    if (dataset_text[0] == 'i' || dataset_text[0] == 'j')
      dataset_text[0] = '0';
    else
      dataset_text[1] = '0';
    dataset_lval.f = strtod (dataset_text, NULL);
    return IMAG;
  }

<DESCRIPTION>{RINT} { /* identify integer */
    dataset_lval.n = strtol (dataset_text, NULL, 10);
    return Integer;
  }

<INITIAL>"<" { /* pass the '<' to the parser */ 
    BEGIN(DESCRIPTION);
    return '<';
  }
<DESCRIPTION>">" { /* pass the '>' to the parser */ 
    BEGIN(INITIAL);
    return '>';
  }
<INITIAL>\r?\n { /* detect end of line */ return Eol; }

<*>{SPACE}|\\\r?\n /* skip spaces and the trailing '\' */

<INITIAL>"#" { /* leave these characters */
    BEGIN(COMMENT);
  }
<INITIAL,DESCRIPTION>. { /* any other character in invalid */
    logprint (LOG_ERROR, 
	      "line %d: syntax error, unrecognized character: `%s'\n", 
	      dataset_lineno, dataset_text);
    return InvalidCharacter;
  }

<COMMENT>.     { /* skip any character in here */ }
<COMMENT>\r?\n { BEGIN(INITIAL); /* skipping ends here */ }

%%
