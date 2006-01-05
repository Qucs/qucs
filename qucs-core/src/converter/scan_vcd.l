/* -*-c-*- */

%{
/*
 * scan_vcd.l - scanner for a VCD data file
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
 * $Id: scan_vcd.l,v 1.2 2006/01/05 07:43:31 raimi Exp $
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

#include "check_vcd.h"
#include "tokens_vcd.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

%}

WS       [ \t\n\r]
DIGIT    [0-9]
EXPONENT [Ee][+-]?{DIGIT}+
REAL     [Rr][+-]?{DIGIT}+("."{DIGIT}+)?{EXPONENT}?
BINARY   [Bb][01]+
DECIMAL  {DIGIT}+
SCALAR   [01xXzZ]
CODE     [!-~]+
IDENT    [a-zA-Z][a-zA-Z0-9_.-]*

%x COMMENT SCALE SCOPE IGNORE VAR VAR2 VAR3 DUMP TIMESTAMP CHANGE

%option yylineno noyywrap nounput prefix="vcd_"

%%

<INITIAL,COMMENT,SCALE,SCOPE,IGNORE,VAR3,DUMP>"$end" {
  BEGIN(INITIAL);
  return t_END;
}

<INITIAL>"$comment" {
  BEGIN(COMMENT);
  return t_COMMENT;
}

<INITIAL>"$date" {
  BEGIN(IGNORE);
  return t_DATE;
}

<INITIAL>"$enddefinitions" {
  return t_ENDDEFINITIONS;
}

<INITIAL>"$scope" {
  BEGIN(SCOPE);
  return t_SCOPE;
}

<INITIAL>"$timescale" {
  BEGIN(SCALE);
  return t_TIMESCALE;
}

<SCALE>"1"   { return ONE; }
<SCALE>"10"  { return TEN; }
<SCALE>"100" { return HUNDRET; }
<SCALE>"s"   { return SECOND; }
<SCALE>"ms"  { return MILLI;  }
<SCALE>"us"  { return MICRO;  }
<SCALE>"ns"  { return NANO;   }
<SCALE>"ps"  { return PICO;   }
<SCALE>"fs"  { return FEMTO;  }

<INITIAL>"$upscope" {
  return t_UPSCOPE;
}

<INITIAL>"$var" {
  BEGIN(VAR);
  return t_VAR;
}

<VAR>"event"     { return EVENT;     }
<VAR>"integer"   { return INTEGER;   }
<VAR>"parameter" { return PARAMETER; }
<VAR>"real"      { return REAL;      }
<VAR>"reg"       { return REG;       }
<VAR>"supply0"   { return SUPPLY0;   }
<VAR>"supply1"   { return SUPPLY1;   }
<VAR>"time"      { return TIME;      }
<VAR>"tri"       { return TRI;       }
<VAR>"triand"    { return TRIAND;    }
<VAR>"trior"     { return TRIOR;     }
<VAR>"trireg"    { return TRIREG;    }
<VAR>"tri0"      { return TRI0;      }
<VAR>"tri1"      { return TRI1;      }
<VAR>"wand"      { return WAND;      }
<VAR>"wire"      { return WIRE;      }
<VAR>"wor"       { return WOR;       }

<INITIAL,DUMP>"1" {
  vcd_lval.value = strdup (vcd_text);
  BEGIN(CHANGE);
  return ONE;
}

<INITIAL,DUMP>"0" {
  vcd_lval.value = strdup (vcd_text);
  BEGIN(CHANGE);
  return ZERO;
}

<INITIAL,DUMP>[xX] {
  vcd_lval.value = strdup (vcd_text);
  BEGIN(CHANGE);
  return X;
}

<INITIAL,DUMP>[zZ] {
  vcd_lval.value = strdup (vcd_text);
  return Z;
}

<VAR>{DECIMAL} {
  vcd_lval.integer = atoi (vcd_text);
  BEGIN(VAR2);
  return DecimalNumber;
}

<TIMESTAMP>{DECIMAL} {
  vcd_lval.integer = atoi (vcd_text);
  BEGIN(INITIAL);
  return DecimalNumber;
}

<INITIAL>"$version" {
  BEGIN(IGNORE);
  return t_VERSION;
}

<INITIAL>"$dumpall" {
  return t_DUMPALL;
}

<INITIAL>"$dumpoff" {
  return t_DUMPOFF;
}

<INITIAL>"$dumpon" {
  return t_DUMPON;
}

<INITIAL>"$dumpvars" {
  BEGIN(DUMP);
  return t_DUMPVARS;
}

<SCOPE>"module"   { return s_MODULE;   }
<SCOPE>"task"     { return s_TASK;     }
<SCOPE>"function" { return s_FUNCTION; }
<SCOPE>"fork"     { return s_FORK;     }
<SCOPE>"begin"    { return s_BEGIN;    }

<INITIAL>"#" {
  BEGIN(TIMESTAMP);
  return HASHMARK;
}

<SCOPE>{IDENT} {
  return Identifier;
}

<VAR3>{IDENT}("["{DECIMAL}"]"|"["{DECIMAL}":"{DECIMAL}"]")? {
  vcd_lval.ident = strdup (vcd_text);
  return Reference;
}

<VAR2>{CODE} {
  vcd_lval.ident = strdup (vcd_text);
  BEGIN(VAR3);
  return IdentifierCode;
}

<CHANGE>{CODE} {
  vcd_lval.ident = strdup (vcd_text);
  BEGIN(INITIAL);
  return IdentifierCode;
}

<COMMENT>. { /* skip any character in here */ }
<*>.       { /* skip any character in here */ }
<*>\r?\n   { /* skip end of line */ }

%%
