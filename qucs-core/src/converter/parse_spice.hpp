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

#ifndef YY_SPICE_PARSE_SPICE_HPP_INCLUDED
# define YY_SPICE_PARSE_SPICE_HPP_INCLUDED
/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int spice_debug;
#endif

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     TitleLine = 258,
     InvalidCharacter = 259,
     End = 260,
     Eol = 261,
     Identifier = 262,
     Digits = 263,
     Floats = 264,
     Nodes = 265,
     Options = 266,
     Function = 267,
     SUBCKT_Action = 268,
     ENDS_Action = 269,
     AC_Action = 270,
     OP_Action = 271,
     I_Source = 272,
     SAVE_Action = 273,
     RLC_Device = 274,
     L_Device = 275,
     K_Device = 276,
     IV_Source = 277,
     GE_Source = 278,
     FH_Source = 279,
     V_Source = 280,
     Diode_Device = 281,
     Bipolar_Device = 282,
     JFET_Device = 283,
     MOSFET_Device = 284,
     MESFET_Device = 285,
     MODEL_Action = 286,
     MODEL_Spec = 287,
     TRAN_Action = 288,
     PLOT_Action = 289,
     VoltFunc = 290,
     CurrFunc = 291,
     DC_Action = 292,
     PRINT_Action = 293,
     OPTIONS_Action = 294,
     WIDTH_Action = 295,
     NOISE_Action = 296,
     PZ_Action = 297,
     CurVol = 298,
     PoleZero = 299,
     ALL_Special = 300,
     X_Device = 301,
     O_Device = 302,
     ModelProps = 303,
     OFF_Special = 304,
     IC_Special = 305,
     SIM_Type = 306,
     TEMP_Special = 307,
     MOS_Special = 308,
     B_Source = 309,
     DISTO_Action = 310,
     INCLUDE_Action = 311,
     File = 312,
     BranchFunc = 313,
     NODESET_Action = 314,
     T_Device = 315,
     U_Device = 316,
     S_Device = 317,
     W_Device = 318,
     ON_Special = 319,
     TF_Action = 320,
     SENS_Action = 321,
     FOUR_Action = 322,
     OpFunc = 323,
     Behave = 324,
     TC_Special = 325,
     TEMP_Action = 326
   };
#endif
/* Tokens.  */
#define TitleLine 258
#define InvalidCharacter 259
#define End 260
#define Eol 261
#define Identifier 262
#define Digits 263
#define Floats 264
#define Nodes 265
#define Options 266
#define Function 267
#define SUBCKT_Action 268
#define ENDS_Action 269
#define AC_Action 270
#define OP_Action 271
#define I_Source 272
#define SAVE_Action 273
#define RLC_Device 274
#define L_Device 275
#define K_Device 276
#define IV_Source 277
#define GE_Source 278
#define FH_Source 279
#define V_Source 280
#define Diode_Device 281
#define Bipolar_Device 282
#define JFET_Device 283
#define MOSFET_Device 284
#define MESFET_Device 285
#define MODEL_Action 286
#define MODEL_Spec 287
#define TRAN_Action 288
#define PLOT_Action 289
#define VoltFunc 290
#define CurrFunc 291
#define DC_Action 292
#define PRINT_Action 293
#define OPTIONS_Action 294
#define WIDTH_Action 295
#define NOISE_Action 296
#define PZ_Action 297
#define CurVol 298
#define PoleZero 299
#define ALL_Special 300
#define X_Device 301
#define O_Device 302
#define ModelProps 303
#define OFF_Special 304
#define IC_Special 305
#define SIM_Type 306
#define TEMP_Special 307
#define MOS_Special 308
#define B_Source 309
#define DISTO_Action 310
#define INCLUDE_Action 311
#define File 312
#define BranchFunc 313
#define NODESET_Action 314
#define T_Device 315
#define U_Device 316
#define S_Device 317
#define W_Device 318
#define ON_Special 319
#define TF_Action 320
#define SENS_Action 321
#define FOUR_Action 322
#define OpFunc 323
#define Behave 324
#define TC_Special 325
#define TEMP_Action 326



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{
/* Line 2060 of yacc.c  */
#line 146 "parse_spice.ypp"

  char * ident;
  char * str;
  double d;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct value_t * value;


/* Line 2060 of yacc.c  */
#line 209 "parse_spice.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE spice_lval;

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int spice_parse (void *YYPARSE_PARAM);
#else
int spice_parse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int spice_parse (void);
#else
int spice_parse ();
#endif
#endif /* ! YYPARSE_PARAM */

#endif /* !YY_SPICE_PARSE_SPICE_HPP_INCLUDED  */
