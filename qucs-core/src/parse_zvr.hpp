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

#ifndef YY_ZVR_PARSE_ZVR_HPP_INCLUDED
# define YY_ZVR_PARSE_ZVR_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int zvr_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ZVR = 258,
     Version = 259,
     Identifier = 260,
     Real = 261,
     Unit = 262,
     DataFMT = 263,
     DataTYP = 264,
     DataIDN = 265,
     InvalidCharacter = 266
   };
#endif
/* Tokens.  */
#define ZVR 258
#define Version 259
#define Identifier 260
#define Real 261
#define Unit 262
#define DataFMT 263
#define DataTYP 264
#define DataIDN 265
#define InvalidCharacter 266



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 62 "parse_zvr.ypp"

  char * ident;
  double f;
  struct zvr_data_t * data;
  struct zvr_header_t * head;
  struct zvr_vector_t * vec;
  struct zvr_line_t * line;


/* Line 2060 of yacc.c  */
#line 89 "parse_zvr.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE zvr_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int zvr_parse (void *YYPARSE_PARAM);
#else
int zvr_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int zvr_parse (void);
#else
int zvr_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_ZVR_PARSE_ZVR_HPP_INCLUDED  */
