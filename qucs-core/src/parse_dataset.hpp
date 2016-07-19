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

#ifndef YY_DATASET_PARSE_DATASET_HPP_INCLUDED
# define YY_DATASET_PARSE_DATASET_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int dataset_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     InvalidCharacter = 258,
     Identifier = 259,
     REAL = 260,
     IMAG = 261,
     COMPLEX = 262,
     Integer = 263,
     Eol = 264,
     IndepBegin = 265,
     DepBegin = 266,
     IndepEnd = 267,
     DepEnd = 268,
     Version = 269
   };
#endif
/* Tokens.  */
#define InvalidCharacter 258
#define Identifier 259
#define REAL 260
#define IMAG 261
#define COMPLEX 262
#define Integer 263
#define Eol 264
#define IndepBegin 265
#define DepBegin 266
#define IndepEnd 267
#define DepEnd 268
#define Version 269



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 67 "parse_dataset.ypp"

  char * ident;
  double f;
  struct {
    double r;
    double i;
  } c;
  long n;
  qucs::vector * v;
  qucs::dataset * data;
  qucs::strlist * list;


/* Line 2060 of yacc.c  */
#line 99 "parse_dataset.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE dataset_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int dataset_parse (void *YYPARSE_PARAM);
#else
int dataset_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int dataset_parse (void);
#else
int dataset_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_DATASET_PARSE_DATASET_HPP_INCLUDED  */
