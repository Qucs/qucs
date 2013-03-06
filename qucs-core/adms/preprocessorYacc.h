/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TK_PRAGMA_NAME = 258,
     TK_IDENT = 259,
     TK_STRING = 260,
     TK_NOT_IDENT = 261,
     TK_ARG = 262,
     TK_ARG_NULL = 263,
     TK_SUBSTITUTOR_NOARG = 264,
     TK_SUBSTITUTOR_NULLARG = 265,
     TK_SUBSTITUTOR_NULLARG_ALONE = 266,
     TK_SUBSTITUTOR_WITHARG = 267,
     TK_SUBSTITUTOR_WITHARG_ALONE = 268,
     TK_CONTINUATOR = 269,
     TK_NOPRAGMA_CONTINUATOR = 270,
     TK_EOL = 271,
     TK_EOF = 272,
     TK_COMMENT = 273,
     TK_INCLUDE = 274,
     TK_SPACE = 275,
     TK_ERROR_PRAGMA_DEFINITION = 276,
     TK_ERROR_PRAGMA_NOT_FOUND = 277,
     TK_ERROR_UNEXPECTED_CHAR = 278,
     TK_ERROR_FILE_OPEN = 279,
     TK_DEFINE = 280,
     TK_DEFINE_END = 281,
     TK_UNDEF = 282,
     TK_IFDEF = 283,
     TK_IFNDEF = 284,
     TK_ELSE = 285,
     TK_ENDIF = 286
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 17 "preprocessorYacc.y"

  p_slist slist;
  char* mystr;



/* Line 1685 of yacc.c  */
#line 89 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE preprocessorlval;


