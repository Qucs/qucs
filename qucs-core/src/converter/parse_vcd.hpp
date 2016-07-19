/* A Bison parser, made by GNU Bison 2.7.12-4996.  */

/* Bison interface for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_VCD_PARSE_VCD_HPP_INCLUDED
# define YY_VCD_PARSE_VCD_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int vcd_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     t_END = 258,
     t_COMMENT = 259,
     t_DATE = 260,
     t_ENDDEFINITIONS = 261,
     t_SCOPE = 262,
     t_TIMESCALE = 263,
     t_UPSCOPE = 264,
     t_VAR = 265,
     t_VERSION = 266,
     t_DUMPALL = 267,
     t_DUMPOFF = 268,
     t_DUMPON = 269,
     t_DUMPVARS = 270,
     s_MODULE = 271,
     s_TASK = 272,
     s_FUNCTION = 273,
     s_FORK = 274,
     s_BEGIN = 275,
     ONE = 276,
     B = 277,
     Z = 278,
     ZERO = 279,
     HASHMARK = 280,
     X = 281,
     R = 282,
     TEN = 283,
     HUNDRET = 284,
     PICO = 285,
     MICRO = 286,
     NANO = 287,
     FEMTO = 288,
     SECOND = 289,
     MILLI = 290,
     EVENT = 291,
     INTEGER = 292,
     PARAMETER = 293,
     REAL = 294,
     REG = 295,
     SUPPLY0 = 296,
     SUPPLY1 = 297,
     TIME = 298,
     TRI = 299,
     TRIAND = 300,
     TRIOR = 301,
     TRIREG = 302,
     TRI0 = 303,
     TRI1 = 304,
     WAND = 305,
     WIRE = 306,
     WOR = 307,
     Real = 308,
     Binary = 309,
     PositiveInteger = 310,
     PositiveHugeInteger = 311,
     Identifier = 312,
     IdentifierCode = 313,
     Reference = 314,
     InvalidCharacter = 315
   };
#endif
/* Tokens.  */
#define t_END 258
#define t_COMMENT 259
#define t_DATE 260
#define t_ENDDEFINITIONS 261
#define t_SCOPE 262
#define t_TIMESCALE 263
#define t_UPSCOPE 264
#define t_VAR 265
#define t_VERSION 266
#define t_DUMPALL 267
#define t_DUMPOFF 268
#define t_DUMPON 269
#define t_DUMPVARS 270
#define s_MODULE 271
#define s_TASK 272
#define s_FUNCTION 273
#define s_FORK 274
#define s_BEGIN 275
#define ONE 276
#define B 277
#define Z 278
#define ZERO 279
#define HASHMARK 280
#define X 281
#define R 282
#define TEN 283
#define HUNDRET 284
#define PICO 285
#define MICRO 286
#define NANO 287
#define FEMTO 288
#define SECOND 289
#define MILLI 290
#define EVENT 291
#define INTEGER 292
#define PARAMETER 293
#define REAL 294
#define REG 295
#define SUPPLY0 296
#define SUPPLY1 297
#define TIME 298
#define TRI 299
#define TRIAND 300
#define TRIOR 301
#define TRIREG 302
#define TRI0 303
#define TRI1 304
#define WAND 305
#define WIRE 306
#define WOR 307
#define Real 308
#define Binary 309
#define PositiveInteger 310
#define PositiveHugeInteger 311
#define Identifier 312
#define IdentifierCode 313
#define Reference 314
#define InvalidCharacter 315



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 79 "parse_vcd.ypp"

  char * ident;
  char * value;
  int integer;
  double real;
  enum vcd_vartypes vtype;
  enum vcd_scopes stype;
  struct vcd_vardef * vardef;
  struct vcd_change * change;
  struct vcd_scope * scope;
  struct vcd_changeset * changeset;
  struct vcd_range * range;


/* Line 2060 of yacc.c  */
#line 192 "parse_vcd.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE vcd_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int vcd_parse (void *YYPARSE_PARAM);
#else
int vcd_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int vcd_parse (void);
#else
int vcd_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_VCD_PARSE_VCD_HPP_INCLUDED  */
