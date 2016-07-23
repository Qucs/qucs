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

#ifndef YY_NETLIST_PARSE_NETLIST_HPP_INCLUDED
# define YY_NETLIST_PARSE_NETLIST_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int netlist_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     InvalidCharacter = 258,
     Identifier = 259,
     Assign = 260,
     ScaleOrUnit = 261,
     Eol = 262,
     Eqn = 263,
     DefSub = 264,
     EndSub = 265,
     REAL = 266,
     IMAG = 267,
     COMPLEX = 268,
     Character = 269,
     STRING = 270,
     Or = 271,
     And = 272,
     Equal = 273,
     NotEqual = 274,
     GreaterOrEqual = 275,
     LessOrEqual = 276,
     Greater = 277,
     Less = 278,
     Not = 279,
     NEG = 280,
     POS = 281
   };
#endif
/* Tokens.  */
#define InvalidCharacter 258
#define Identifier 259
#define Assign 260
#define ScaleOrUnit 261
#define Eol 262
#define Eqn 263
#define DefSub 264
#define EndSub 265
#define REAL 266
#define IMAG 267
#define COMPLEX 268
#define Character 269
#define STRING 270
#define Or 271
#define And 272
#define Equal 273
#define NotEqual 274
#define GreaterOrEqual 275
#define LessOrEqual 276
#define Greater 277
#define Less 278
#define Not 279
#define NEG 280
#define POS 281



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 78 "parse_netlist.ypp"

  char * ident;
  char * str;
  double d;
  char chr;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct node_t * node;
  struct pair_t * pair;
  struct value_t * value;
  struct {
    double r;
    double i;
  } c;
  qucs::eqn::node * eqn;
  qucs::eqn::constant * con;
  qucs::eqn::reference * ref;
  qucs::eqn::application * app;
  qucs::eqn::assignment * assign;


/* Line 2060 of yacc.c  */
#line 131 "parse_netlist.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE netlist_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int netlist_parse (void *YYPARSE_PARAM);
#else
int netlist_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int netlist_parse (void);
#else
int netlist_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_NETLIST_PARSE_NETLIST_HPP_INCLUDED  */
