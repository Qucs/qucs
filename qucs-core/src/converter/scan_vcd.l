/* -*-c-*- */

%{
/*
 * scan_vcd.l - scanner for a VCD data file
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
 * $Id: scan_vcd.l,v 1.1 2005-12-26 21:13:31 raimi Exp $
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

#include "check_spice.h"
#include "tokens_spice.h"

#if !HAVE_STRCHR
# define strchr  index
# define strrchr rindex
#endif

%}

WS              [ \t\n\r]
END             "$end"
COMMENT         "$comment"
DATE            "$date"
ENDDEFINITIONS  "$enddefinitions"
SCOPE           "$scope"
TIMESCALE       "$timescale"
UPSCOPE         "$upscope"
VAR             "$var"
VERSION         "$version"
DUMPALL         "$dumpall"
DUMPOFF         "$dumpoff"
DUMPON          "$dumpon"
DUMPVARS        "$dumpvars"
DIGIT           [0-9]
EXPONENT        [Ee][+-]?{DIGIT}+
RealNumber      [Rr][+-]?{DIGIT}+("."{DIGIT}+)?{EXPONENT}?
BinaryNumber    [Bb][01]+
DecimalNumber   "#"[0-9]+
ScalarValue     [01xXzZ]
IdentifierCode  [!-~]+
Identifier      [a-zA-Z_.-]+
CommentText     


%option yylineno noyywrap nounput prefix="vcd_"

%%

%%
