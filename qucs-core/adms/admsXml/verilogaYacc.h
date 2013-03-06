
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
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
     PREC_IF_THEN = 258,
     tk_else = 259,
     tk_from = 260,
     tk_branch = 261,
     tk_number = 262,
     tk_nature = 263,
     tk_aliasparameter = 264,
     tk_output = 265,
     tk_anystring = 266,
     tk_dollar_ident = 267,
     tk_or = 268,
     tk_aliasparam = 269,
     tk_if = 270,
     tk_analog = 271,
     tk_parameter = 272,
     tk_discipline = 273,
     tk_char = 274,
     tk_anytext = 275,
     tk_for = 276,
     tk_while = 277,
     tk_real = 278,
     tk_op_shr = 279,
     tk_case = 280,
     tk_potential = 281,
     tk_endcase = 282,
     tk_inf = 283,
     tk_exclude = 284,
     tk_ground = 285,
     tk_endmodule = 286,
     tk_begin = 287,
     tk_enddiscipline = 288,
     tk_domain = 289,
     tk_ident = 290,
     tk_op_shl = 291,
     tk_string = 292,
     tk_integer = 293,
     tk_module = 294,
     tk_endattribute = 295,
     tk_end = 296,
     tk_inout = 297,
     tk_and = 298,
     tk_bitwise_equr = 299,
     tk_default = 300,
     tk_function = 301,
     tk_input = 302,
     tk_beginattribute = 303,
     tk_endnature = 304,
     tk_endfunction = 305,
     tk_flow = 306
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1676 of yacc.c  */
#line 119 "verilogaYacc.y"

  p_lexval _lexval;
  p_yaccval _yaccval;



/* Line 1676 of yacc.c  */
#line 110 "y.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE verilogalval;


