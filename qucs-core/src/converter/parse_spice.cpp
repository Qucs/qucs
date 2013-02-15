/* A Bison parser, made by GNU Bison 2.5.  */

/* Bison implementation for Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2011 Free Software Foundation, Inc.
   
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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.5"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0

/* Substitute the variable and function names.  */
#define yyparse         spice_parse
#define yylex           spice_lex
#define yyerror         spice_error
#define yylval          spice_lval
#define yychar          spice_char
#define yydebug         spice_debug
#define yynerrs         spice_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 3 "parse_spice.y"

/*
 * parse_spice.y - parser for a Spice netlist
 *
 * Copyright (C) 2004, 2005, 2006, 2007, 2009 Stefan Jahn <stefan@lkcc.org>
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
 * $Id$
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
#define strcasecmp stricmp
#endif

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#include "check_spice.h"

// Converts the given string into upper case.
static char * spice_toupper (char * str) {
  for (unsigned int i = 0; i < strlen (str); i++) {
    if (str[i] >= 'a' && str[i] <= 'z') str[i] = toupper (str[i]);
  }
  return str;
}

// Creates a device instance.
static struct definition_t * spice_create_device (char * instance) {
  struct definition_t * def = create_definition ();
  def->action = PROP_COMPONENT;
  def->instance = spice_toupper (instance);
  def->type = (char *) calloc (2, 1);
  def->type[0] = def->instance[0];
  def->line = spice_lineno;
  return def;
}

// Creates an action instance.
static struct definition_t * spice_create_action (char * type,
						  char * instance) {
  struct definition_t * def = create_definition ();
  def->action = PROP_ACTION;
  def->instance = spice_toupper (instance);
  def->type = spice_toupper (type);
  def->line = spice_lineno;
  return def;
}

// Create a string value.
static struct value_t * spice_create_str_value (char * value, int hint) {
  struct value_t * val = create_value ();
  val->ident = spice_toupper (value);
  val->hint |= hint;
  return val;
}

// Create a real value.
static struct value_t * spice_create_val_value (char * value, int hint) {
  struct value_t * val = create_value ();
  val->ident = value;
  val->value = strtod (value, NULL);
  val->hint |= hint;
  return val;
}

// Create a key/value pair.
static struct value_t * spice_create_par_value (char * key, char * value) {
  struct value_t * val = spice_create_str_value (key, HINT_PAIR);
  val->unit = value;
  return val;
}

// Append a string value to the definition.
static void spice_append_str_value (struct definition_t * def,
				    char * value, int hint) {
  struct value_t * val = spice_create_str_value (value, hint);
  def->values = netlist_append_values (def->values, val);
}

// Append a string value to the given values.
static struct value_t * spice_append_str_values (struct value_t * values,
						 char * value, int hint) {
  struct value_t * val = spice_create_str_value (value, hint);
  return netlist_append_values (values, val);
}

// Append a real value to the definition.
static void spice_append_val_value (struct definition_t * def,
				    char * value, int hint) {
  struct value_t * val = spice_create_val_value (value, hint);
  def->values = netlist_append_values (def->values, val);
}

// Append a real value to the given values.
static struct value_t * spice_append_val_values (struct value_t * values,
						 char * value, int hint) {
  struct value_t * val = spice_create_val_value (value, hint);
  return netlist_append_values (values, val);
}



/* Line 268 of yacc.c  */
#line 208 "parse_spice.cpp"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
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

/* Line 293 of yacc.c  */
#line 146 "parse_spice.y"

  char * ident;
  char * str;
  double d;
  struct definition_t * definition;
  struct definition_t * subcircuit;
  struct value_t * value;



/* Line 293 of yacc.c  */
#line 397 "parse_spice.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 409 "parse_spice.cpp"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  113
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   679

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  72
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  42
/* YYNRULES -- Number of rules.  */
#define YYNRULES  148
/* YYNRULES -- Number of states.  */
#define YYNSTATES  355

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   326

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     6,    10,    12,    15,    16,    19,    21,
      23,    25,    32,    40,    47,    54,    61,    68,    76,    82,
      88,    95,   103,   109,   117,   126,   132,   139,   145,   151,
     158,   165,   173,   181,   188,   192,   197,   201,   204,   208,
     213,   218,   222,   227,   231,   235,   239,   245,   254,   258,
     267,   275,   283,   291,   299,   308,   311,   314,   317,   320,
     323,   326,   330,   334,   338,   341,   345,   348,   352,   357,
     363,   366,   368,   371,   375,   377,   379,   382,   384,   386,
     387,   390,   394,   397,   401,   402,   404,   406,   407,   410,
     413,   414,   417,   421,   422,   425,   429,   430,   435,   436,
     440,   443,   446,   449,   450,   454,   457,   460,   463,   467,
     468,   472,   476,   479,   482,   485,   487,   489,   494,   496,
     498,   500,   502,   504,   506,   508,   509,   513,   514,   517,
     518,   521,   524,   525,   528,   529,   532,   535,   536,   539,
     543,   548,   549,   552,   554,   556,   559,   561,   563
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      73,     0,    -1,    74,     5,    -1,     3,    74,     5,    -1,
      74,    -1,     3,    74,    -1,    -1,    75,    74,    -1,   108,
      -1,    76,    -1,     6,    -1,    19,    99,    99,    98,   101,
       6,    -1,    19,    99,    99,    98,    96,   101,     6,    -1,
      19,    99,    99,    96,    98,     6,    -1,    19,    99,    99,
      96,   101,     6,    -1,    19,    99,    99,    98,    77,     6,
      -1,    19,    99,    99,    98,    78,     6,    -1,    19,    99,
      99,    98,    69,   103,     6,    -1,    21,    20,    20,    98,
       6,    -1,    22,    99,    99,   107,     6,    -1,    22,    99,
      99,    98,   107,     6,    -1,    23,    99,    99,    69,     8,
     103,     6,    -1,    23,    99,    99,    69,     6,    -1,    23,
      99,    99,    99,    99,    98,     6,    -1,    24,   100,   100,
      69,     8,   105,   103,     6,    -1,    24,   100,   100,    69,
       6,    -1,    24,   100,   100,    25,    98,     6,    -1,    31,
      96,    32,    91,     6,    -1,    26,    99,    99,    96,    93,
      -1,    28,    99,    99,    99,    96,    94,    -1,    27,    99,
      99,    99,    96,    94,    -1,    27,    99,    99,    99,    99,
      96,    94,    -1,    29,    99,    99,    99,    99,    96,    95,
      -1,    30,    99,    99,    99,    96,    94,    -1,    33,   102,
       6,    -1,    34,    51,    87,     6,    -1,    15,   106,     6,
      -1,    37,     6,    -1,    37,    97,     6,    -1,    37,    97,
      97,     6,    -1,    38,    51,    89,     6,    -1,    38,    89,
       6,    -1,    38,    51,    45,     6,    -1,    39,    90,     6,
      -1,    71,   102,     6,    -1,    40,   101,     6,    -1,    41,
      84,    85,   106,     6,    -1,    42,    99,    99,    99,    99,
      43,    44,     6,    -1,    46,   104,     6,    -1,    62,    99,
      99,    99,    99,    96,    88,     6,    -1,    63,    99,    99,
      25,    96,    88,     6,    -1,    47,    99,    99,    99,    99,
      96,     6,    -1,    61,    99,    99,    99,    96,   101,     6,
      -1,    60,    99,    99,    99,    99,   101,     6,    -1,    60,
      99,    99,    99,    99,   101,    82,     6,    -1,    16,     6,
      -1,    18,     6,    -1,    66,     6,    -1,    65,     6,    -1,
      67,     6,    -1,    54,     6,    -1,    55,   106,     6,    -1,
      56,    57,     6,    -1,    59,    92,     6,    -1,    70,    98,
      -1,    70,    98,    98,    -1,    50,    98,    -1,    50,    98,
      98,    -1,    50,    98,    98,    98,    -1,    50,    98,    98,
      98,    98,    -1,    98,    98,    -1,    99,    -1,    35,    99,
      -1,    35,    99,    99,    -1,    17,    -1,    25,    -1,    36,
      25,    -1,    58,    -1,    68,    -1,    -1,    84,    87,    -1,
      84,    83,    87,    -1,    86,    87,    -1,    86,    83,    87,
      -1,    -1,    64,    -1,    49,    -1,    -1,    84,    87,    -1,
      86,    87,    -1,    -1,    11,    90,    -1,     7,    98,    90,
      -1,    -1,    48,    91,    -1,     7,    98,    91,    -1,    -1,
      35,    99,    98,    92,    -1,    -1,    52,    98,    93,    -1,
      98,    93,    -1,    49,    93,    -1,    79,    93,    -1,    -1,
      52,    98,    94,    -1,    98,    94,    -1,    49,    94,    -1,
      80,    94,    -1,    53,    98,    94,    -1,    -1,    52,    98,
      95,    -1,    53,    98,    95,    -1,    98,    95,    -1,    49,
      95,    -1,    81,    95,    -1,     7,    -1,    32,    -1,    85,
      98,    98,    98,    -1,     8,    -1,     9,    -1,     8,    -1,
      10,    -1,     7,    -1,    99,    -1,    25,    -1,    -1,     7,
      98,   101,    -1,    -1,    98,   102,    -1,    -1,    99,   103,
      -1,     9,   103,    -1,    -1,    99,   104,    -1,    -1,    25,
     105,    -1,    12,   102,    -1,    -1,   106,   107,    -1,   109,
     110,   112,    -1,    13,   111,   104,     6,    -1,    -1,   113,
     110,    -1,     7,    -1,    14,    -1,    14,   111,    -1,    76,
      -1,   108,    -1,     6,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   182,   182,   184,   187,   190,   196,   197,   201,   206,
     213,   217,   225,   234,   242,   250,   258,   266,   275,   282,
     289,   297,   312,   317,   326,   342,   347,   355,   362,   370,
     379,   388,   398,   408,   417,   422,   428,   433,   437,   442,
     447,   453,   458,   464,   469,   474,   479,   486,   496,   502,
     512,   521,   530,   539,   548,   558,   562,   566,   570,   574,
     578,   582,   587,   595,   603,   610,   618,   626,   634,   644,
     655,   663,   669,   675,   685,   685,   688,   694,   700,   708,
     709,   712,   716,   719,   725,   726,   729,   734,   735,   738,
     743,   744,   748,   754,   755,   759,   765,   766,   775,   776,
     780,   784,   788,   793,   794,   798,   802,   806,   809,   815,
     816,   820,   824,   828,   832,   837,   837,   840,   850,   850,
     852,   852,   852,   854,   854,   856,   857,   863,   864,   870,
     871,   875,   881,   882,   888,   889,   896,   903,   904,   910,
     918,   924,   925,   936,   939,   940,   944,   950,   951
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "TitleLine", "InvalidCharacter", "End",
  "Eol", "Identifier", "Digits", "Floats", "Nodes", "Options", "Function",
  "SUBCKT_Action", "ENDS_Action", "AC_Action", "OP_Action", "I_Source",
  "SAVE_Action", "RLC_Device", "L_Device", "K_Device", "IV_Source",
  "GE_Source", "FH_Source", "V_Source", "Diode_Device", "Bipolar_Device",
  "JFET_Device", "MOSFET_Device", "MESFET_Device", "MODEL_Action",
  "MODEL_Spec", "TRAN_Action", "PLOT_Action", "VoltFunc", "CurrFunc",
  "DC_Action", "PRINT_Action", "OPTIONS_Action", "WIDTH_Action",
  "NOISE_Action", "PZ_Action", "CurVol", "PoleZero", "ALL_Special",
  "X_Device", "O_Device", "ModelProps", "OFF_Special", "IC_Special",
  "SIM_Type", "TEMP_Special", "MOS_Special", "B_Source", "DISTO_Action",
  "INCLUDE_Action", "File", "BranchFunc", "NODESET_Action", "T_Device",
  "U_Device", "S_Device", "W_Device", "ON_Special", "TF_Action",
  "SENS_Action", "FOUR_Action", "OpFunc", "Behave", "TC_Special",
  "TEMP_Action", "$accept", "Input", "InputList", "InputLine",
  "DefinitionLine", "TC_Value_1", "TC_Value_2", "IC_Condition_1",
  "IC_Condition_2", "IC_Condition_3", "IC_Condition_4", "Output_Range",
  "VOLTAGE_Output", "IV_Reference", "CURRENT_Output", "PLOT_List",
  "SWITCH_State", "PRINT_List", "OPTIONS_List", "MODEL_List",
  "NODESET_List", "DEVICE_List_1", "DEVICE_List_2", "DEVICE_List_3",
  "MODEL_Ident", "DC_List", "Value", "Node", "FH_Node", "PairList",
  "ValueList", "NodeValueList", "NodeList", "VSourceList", "Expr",
  "ExprList", "Subcircuit", "BeginSub", "SubBody", "SubCkt_Ident",
  "EndSub", "SubBodyLine", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    72,    73,    73,    73,    73,    74,    74,    75,    75,
      75,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    76,    76,    76,    76,    76,    76,
      76,    76,    76,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    84,    84,    85,    85,    86,    86,    86,    87,
      87,    87,    87,    87,    88,    88,    88,    89,    89,    89,
      90,    90,    90,    91,    91,    91,    92,    92,    93,    93,
      93,    93,    93,    94,    94,    94,    94,    94,    94,    95,
      95,    95,    95,    95,    95,    96,    96,    97,    98,    98,
      99,    99,    99,   100,   100,   101,   101,   102,   102,   103,
     103,   103,   104,   104,   105,   105,   106,   107,   107,   108,
     109,   110,   110,   111,   112,   112,   113,   113,   113
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     3,     1,     2,     0,     2,     1,     1,
       1,     6,     7,     6,     6,     6,     6,     7,     5,     5,
       6,     7,     5,     7,     8,     5,     6,     5,     5,     6,
       6,     7,     7,     6,     3,     4,     3,     2,     3,     4,
       4,     3,     4,     3,     3,     3,     5,     8,     3,     8,
       7,     7,     7,     7,     8,     2,     2,     2,     2,     2,
       2,     3,     3,     3,     2,     3,     2,     3,     4,     5,
       2,     1,     2,     3,     1,     1,     2,     1,     1,     0,
       2,     3,     2,     3,     0,     1,     1,     0,     2,     2,
       0,     2,     3,     0,     2,     3,     0,     4,     0,     3,
       2,     2,     2,     0,     3,     2,     2,     2,     3,     0,
       3,     3,     2,     2,     2,     1,     1,     4,     1,     1,
       1,     1,     1,     1,     1,     0,     3,     0,     2,     0,
       2,     2,     0,     2,     0,     2,     2,     0,     2,     3,
       4,     0,     2,     1,     1,     2,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       6,     6,    10,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   127,     0,
       0,    87,    90,   125,     0,     0,   132,     0,     0,     0,
       0,    96,     0,     0,     0,     0,     0,     0,     0,   127,
       0,     4,     6,     9,     8,   141,     5,   143,   132,   127,
       0,    55,    56,   122,   120,   121,     0,     0,     0,     0,
     124,   123,     0,     0,     0,     0,     0,     0,   115,   116,
       0,   118,   119,   127,     0,    79,    37,    74,    75,     0,
       0,     0,     0,    87,    77,    78,    79,    79,     0,    71,
       0,    90,     0,     0,     0,     0,     0,   132,     0,     0,
      60,     0,     0,     0,     0,     0,     0,     0,     0,    58,
      57,    59,     0,     1,     2,     7,   148,   146,   147,     0,
     141,     3,     0,   136,    36,     0,     0,   137,     0,     0,
       0,     0,     0,     0,     0,    93,   128,    34,    79,    79,
       0,     0,    38,     0,    72,    76,     0,     0,    88,    89,
      41,    90,    91,    43,   125,    45,     0,     0,   133,    48,
       0,    61,    62,     0,    63,     0,     0,     0,     0,    44,
     144,   139,   142,   140,   125,   125,     0,   137,   137,     0,
       0,     0,     0,     0,    98,     0,     0,     0,     0,     0,
      93,     0,   120,    79,    80,     0,    79,    82,    35,     0,
      39,    73,    42,    40,    92,   126,     0,     0,     0,    96,
       0,     0,     0,     0,   145,     0,     0,   115,   129,     0,
       0,     0,   125,     0,    18,     0,   138,    19,    22,   129,
       0,     0,    25,   134,    98,     0,     0,    98,    28,    98,
     115,   103,     0,   103,     0,   103,    93,    94,    27,    81,
      70,    83,   117,    46,     0,     0,    97,   125,   125,     0,
      84,    13,    14,   129,   129,     0,    64,    15,    16,     0,
      11,    20,     0,     0,    26,   134,   129,   101,    66,    98,
     102,   100,   103,     0,     0,     0,   103,    30,   103,   103,
      29,   109,    33,    95,     0,     0,     0,     0,    84,    86,
      85,     0,   131,   130,    17,    65,    12,    21,    23,   135,
       0,    99,   106,     0,   103,   103,   107,   105,    31,   109,
       0,     0,     0,   109,    32,   109,     0,    51,    53,     0,
       0,    52,     0,    50,    24,    67,   104,   108,   113,     0,
     109,   109,   114,   112,    47,     0,    54,    49,     0,   110,
     111,     0,    68,     0,    69
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    40,    41,    42,    43,   220,   221,   237,   286,   323,
     330,   193,   138,    79,   139,   140,   301,    88,    92,   191,
     104,   238,   287,   324,    70,    80,   288,    89,    62,    94,
      74,   265,    98,   276,   178,   179,    44,    45,   119,    48,
     171,   120
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -227
static const yytype_int16 yypact[] =
{
     546,   216,  -227,    41,     4,    17,    47,   278,    62,   278,
     278,   198,   278,   278,   278,   278,   278,    13,   159,    64,
      33,    92,    30,    91,   168,   278,   278,   278,   116,     4,
      76,   103,   278,   278,   278,   278,   136,   138,   140,   159,
     152,   156,   216,  -227,  -227,   608,   158,  -227,   278,   159,
     173,  -227,  -227,  -227,  -227,  -227,   278,   145,   278,   278,
    -227,  -227,   198,   278,   278,   278,   278,   278,  -227,  -227,
     154,  -227,  -227,   159,   178,   122,  -227,  -227,  -227,   159,
      88,   278,   162,   104,  -227,  -227,   122,   122,   191,  -227,
     159,    30,   194,   159,   196,    19,   278,   278,   203,   278,
    -227,   209,   214,   278,   222,   278,   278,   278,   278,  -227,
    -227,  -227,   224,  -227,  -227,  -227,  -227,  -227,  -227,   190,
     608,  -227,   227,  -227,  -227,    78,   159,    34,    22,   -17,
      13,   278,   278,   278,   278,     3,  -227,  -227,   289,   289,
     230,   159,  -227,   235,   278,  -227,   245,   246,  -227,  -227,
    -227,    30,  -227,  -227,    91,  -227,     4,   278,  -227,  -227,
     278,  -227,  -227,   159,  -227,   278,   278,   278,   234,  -227,
      41,  -227,  -227,  -227,   282,    -5,   254,     4,     4,   258,
      84,   278,   159,    98,   146,   175,    13,   278,    13,   159,
       3,   262,   165,   122,  -227,   159,   122,  -227,  -227,   159,
    -227,  -227,  -227,  -227,  -227,  -227,   263,   278,   278,   103,
     278,    13,   278,    13,  -227,   268,   274,   159,   217,   159,
     286,   287,    91,   288,  -227,   294,  -227,  -227,  -227,   217,
     159,   300,  -227,   259,   146,   159,   159,   146,  -227,   146,
      15,   139,    13,   139,    13,   139,     3,  -227,  -227,  -227,
    -227,  -227,  -227,  -227,   265,    13,  -227,    91,    91,    13,
      87,  -227,  -227,   217,   217,   303,   159,  -227,  -227,   304,
    -227,  -227,   306,   307,  -227,   259,   217,  -227,  -227,   146,
    -227,  -227,   139,   159,   159,   159,   139,  -227,   139,   139,
    -227,   161,  -227,  -227,   270,   309,     9,   310,    87,  -227,
    -227,   311,  -227,  -227,  -227,  -227,  -227,  -227,  -227,  -227,
     312,  -227,  -227,   159,   139,   139,  -227,  -227,  -227,   161,
     159,   159,   159,   161,  -227,   161,   313,  -227,  -227,   159,
     314,  -227,   320,  -227,  -227,  -227,  -227,  -227,  -227,   159,
     161,   161,  -227,  -227,  -227,   159,  -227,  -227,   159,  -227,
    -227,   159,  -227,   159,  -227
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -227,  -227,    27,  -227,   -42,  -227,  -227,  -227,  -227,  -227,
    -227,   189,   -10,   236,   -12,   -62,    31,   249,   -85,  -172,
     135,  -199,  -226,  -222,  -118,   266,   -18,   327,   283,  -141,
      86,  -180,   -43,    73,    -3,    16,   -41,  -227,   229,   180,
    -227,  -227
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -123
static const yytype_int16 yytable[] =
{
      73,    50,   217,   117,   118,   122,   152,   174,   182,    87,
     189,    86,   184,   205,    95,   328,    49,   290,   247,   292,
      68,    73,  -122,    51,   148,   149,   101,    69,    46,    53,
      54,    73,    55,   216,   223,   277,    77,    90,   280,    76,
     281,    91,    71,    72,    78,    69,    49,  -122,    47,   272,
      77,   190,   183,    52,   158,    73,   312,   222,    78,   329,
     316,   141,   317,   318,   218,   219,   204,   241,   243,   115,
     245,    87,   151,    86,   293,   154,   194,   197,   117,   118,
     311,   269,    57,   302,   303,    68,    71,    72,   336,   337,
     228,   180,   229,   258,   142,   260,   310,   338,    93,    53,
      54,   342,    55,   343,   232,    77,   233,   175,   176,   177,
      69,    53,    54,    78,    55,    75,   296,   297,   349,   350,
     195,   195,   100,   199,   289,   112,   291,    81,    82,    53,
      54,   249,    55,   102,   251,   123,   299,   295,   103,    81,
      82,   298,   109,    83,   110,   209,   111,    71,    72,   146,
      84,   300,   113,   206,    71,    72,   215,    81,    82,   136,
      85,   114,    84,   121,   231,   126,   239,    71,    72,    71,
      72,   246,    85,  -118,  -118,    53,    54,   250,    55,   124,
      84,   252,   240,    54,   137,    55,   135,   145,   282,   283,
      85,   284,   285,   225,   226,   234,   235,   150,   236,   154,
     153,   266,   155,    81,   170,    53,    54,    69,    55,   159,
     319,   320,   273,   321,   322,   161,   239,   278,   279,   239,
     162,   239,     2,    60,    53,    54,   263,    55,   164,     3,
     169,     4,     5,   173,     6,     7,   198,     8,     9,    10,
      11,   200,    12,    13,    14,    15,    16,    17,   305,    18,
      19,   202,   203,    20,    21,    22,    23,    24,    25,   213,
     224,   239,    26,    27,   227,   313,   314,   315,   248,   253,
      28,    29,    30,   325,   261,    31,    32,    33,    34,    35,
     262,    36,    37,    38,   275,    53,    54,    39,    55,    93,
      71,    72,   267,   268,   270,   335,    53,   192,    72,    55,
     271,   325,   339,   340,   341,   325,   274,   325,   294,   304,
     306,   345,   307,   308,   326,   327,   331,   333,   334,   344,
     346,   348,   325,   325,    81,    82,   347,   351,   196,   332,
     352,   156,   147,   353,    56,   354,    58,    59,    61,    63,
      64,    65,    66,    67,   256,   129,   143,    84,   309,   172,
     214,     0,    96,    97,    99,     0,     0,    85,     0,   105,
     106,   107,   108,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    97,     0,     0,     0,     0,
       0,     0,     0,   125,     0,   127,   128,     0,     0,    61,
     130,   131,   132,   133,   134,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   144,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   157,    97,     0,   160,     0,     0,     0,
     163,     0,   165,   166,   167,   168,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   181,     0,     0,   185,   186,
     187,   188,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   201,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   207,     0,     0,   208,     0,     0,
       0,     0,   210,   211,   212,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   230,     0,
       0,     0,   242,     0,   244,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   254,   255,     0,   257,     0,   259,
       0,     0,     0,     0,     0,   264,     0,     0,     0,     1,
       0,     0,     2,     0,     0,     0,   264,     0,     0,     3,
       0,     4,     5,     0,     6,     7,     0,     8,     9,    10,
      11,     0,    12,    13,    14,    15,    16,    17,     0,    18,
      19,     0,     0,    20,    21,    22,    23,    24,    25,     0,
     264,   264,    26,    27,     0,     0,     0,     0,     0,     0,
      28,    29,    30,   264,     0,    31,    32,    33,    34,    35,
       0,    36,    37,    38,   116,     0,     0,    39,     0,     0,
       0,     3,     0,     4,     5,     0,     6,     7,     0,     8,
       9,    10,    11,     0,    12,    13,    14,    15,    16,    17,
       0,    18,    19,     0,     0,    20,    21,    22,    23,    24,
      25,     0,     0,     0,    26,    27,     0,     0,     0,     0,
       0,     0,    28,    29,    30,     0,     0,    31,    32,    33,
      34,    35,     0,    36,    37,    38,     0,     0,     0,    39
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-227))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      18,     4,     7,    45,    45,    48,    91,   125,    25,    21,
       7,    21,   130,   154,    24,     6,    12,   243,   190,   245,
       7,    39,     7,     6,    86,    87,    29,    32,     1,     7,
       8,    49,    10,   174,   175,   234,    17,     7,   237,     6,
     239,    11,     8,     9,    25,    32,    12,    32,     7,   229,
      17,    48,    69,     6,    97,    73,   282,   175,    25,    50,
     286,    79,   288,   289,    69,    70,   151,   185,   186,    42,
     188,    83,    90,    83,   246,    93,   138,   139,   120,   120,
     279,   222,    20,   263,   264,     7,     8,     9,   314,   315,
       6,    69,     8,   211,     6,   213,   276,   319,     7,     7,
       8,   323,    10,   325,     6,    17,     8,   125,   126,   127,
      32,     7,     8,    25,    10,    51,   257,   258,   340,   341,
     138,   139,     6,   141,   242,    39,   244,    35,    36,     7,
       8,   193,    10,    57,   196,    49,    49,   255,    35,    35,
      36,   259,     6,    51,     6,   163,     6,     8,     9,    45,
      58,    64,     0,   156,     8,     9,   174,    35,    36,    73,
      68,     5,    58,     5,   182,    20,   184,     8,     9,     8,
       9,   189,    68,     8,     9,     7,     8,   195,    10,     6,
      58,   199,     7,     8,     6,    10,    32,    25,    49,    50,
      68,    52,    53,   177,   178,    49,    50,     6,    52,   217,
       6,   219,     6,    35,    14,     7,     8,    32,    10,     6,
      49,    50,   230,    52,    53,     6,   234,   235,   236,   237,
       6,   239,     6,    25,     7,     8,     9,    10,     6,    13,
       6,    15,    16,     6,    18,    19,     6,    21,    22,    23,
      24,     6,    26,    27,    28,    29,    30,    31,   266,    33,
      34,     6,     6,    37,    38,    39,    40,    41,    42,    25,
       6,   279,    46,    47,     6,   283,   284,   285,     6,     6,
      54,    55,    56,   291,     6,    59,    60,    61,    62,    63,
       6,    65,    66,    67,    25,     7,     8,    71,    10,     7,
       8,     9,     6,     6,     6,   313,     7,     8,     9,    10,
       6,   319,   320,   321,   322,   323,     6,   325,    43,     6,
       6,   329,     6,     6,    44,     6,     6,     6,     6,     6,
       6,   339,   340,   341,    35,    36,     6,   345,   139,   298,
     348,    95,    83,   351,     7,   353,     9,    10,    11,    12,
      13,    14,    15,    16,   209,    62,    80,    58,   275,   120,
     170,    -1,    25,    26,    27,    -1,    -1,    68,    -1,    32,
      33,    34,    35,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    48,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    56,    -1,    58,    59,    -1,    -1,    62,
      63,    64,    65,    66,    67,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    81,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    96,    97,    -1,    99,    -1,    -1,    -1,
     103,    -1,   105,   106,   107,   108,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   128,    -1,    -1,   131,   132,
     133,   134,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   157,    -1,    -1,   160,    -1,    -1,
      -1,    -1,   165,   166,   167,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   181,    -1,
      -1,    -1,   185,    -1,   187,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   207,   208,    -1,   210,    -1,   212,
      -1,    -1,    -1,    -1,    -1,   218,    -1,    -1,    -1,     3,
      -1,    -1,     6,    -1,    -1,    -1,   229,    -1,    -1,    13,
      -1,    15,    16,    -1,    18,    19,    -1,    21,    22,    23,
      24,    -1,    26,    27,    28,    29,    30,    31,    -1,    33,
      34,    -1,    -1,    37,    38,    39,    40,    41,    42,    -1,
     263,   264,    46,    47,    -1,    -1,    -1,    -1,    -1,    -1,
      54,    55,    56,   276,    -1,    59,    60,    61,    62,    63,
      -1,    65,    66,    67,     6,    -1,    -1,    71,    -1,    -1,
      -1,    13,    -1,    15,    16,    -1,    18,    19,    -1,    21,
      22,    23,    24,    -1,    26,    27,    28,    29,    30,    31,
      -1,    33,    34,    -1,    -1,    37,    38,    39,    40,    41,
      42,    -1,    -1,    -1,    46,    47,    -1,    -1,    -1,    -1,
      -1,    -1,    54,    55,    56,    -1,    -1,    59,    60,    61,
      62,    63,    -1,    65,    66,    67,    -1,    -1,    -1,    71
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     6,    13,    15,    16,    18,    19,    21,    22,
      23,    24,    26,    27,    28,    29,    30,    31,    33,    34,
      37,    38,    39,    40,    41,    42,    46,    47,    54,    55,
      56,    59,    60,    61,    62,    63,    65,    66,    67,    71,
      73,    74,    75,    76,   108,   109,    74,     7,   111,    12,
     106,     6,     6,     7,     8,    10,    99,    20,    99,    99,
      25,    99,   100,    99,    99,    99,    99,    99,     7,    32,
      96,     8,     9,    98,   102,    51,     6,    17,    25,    85,
      97,    35,    36,    51,    58,    68,    84,    86,    89,    99,
       7,    11,    90,     7,   101,    84,    99,    99,   104,    99,
       6,   106,    57,    35,    92,    99,    99,    99,    99,     6,
       6,     6,   102,     0,     5,    74,     6,    76,   108,   110,
     113,     5,   104,   102,     6,    99,    20,    99,    99,   100,
      99,    99,    99,    99,    99,    32,   102,     6,    84,    86,
      87,    98,     6,    97,    99,    25,    45,    89,    87,    87,
       6,    98,    90,     6,    98,     6,    85,    99,   104,     6,
      99,     6,     6,    99,     6,    99,    99,    99,    99,     6,
      14,   112,   110,     6,    96,    98,    98,    98,   106,   107,
      69,    99,    25,    69,    96,    99,    99,    99,    99,     7,
      48,    91,     8,    83,    87,    98,    83,    87,     6,    98,
       6,    99,     6,     6,    90,   101,   106,    99,    99,    98,
      99,    99,    99,    25,   111,    98,   101,     7,    69,    70,
      77,    78,    96,   101,     6,   107,   107,     6,     6,     8,
      99,    98,     6,     8,    49,    50,    52,    79,    93,    98,
       7,    96,    99,    96,    99,    96,    98,    91,     6,    87,
      98,    87,    98,     6,    99,    99,    92,    99,    96,    99,
      96,     6,     6,     9,    99,   103,    98,     6,     6,   101,
       6,     6,   103,    98,     6,    25,   105,    93,    98,    98,
      93,    93,    49,    50,    52,    53,    80,    94,    98,    96,
      94,    96,    94,    91,    43,    96,   101,   101,    96,    49,
      64,    88,   103,   103,     6,    98,     6,     6,     6,   105,
     103,    93,    94,    98,    98,    98,    94,    94,    94,    49,
      50,    52,    53,    81,    95,    98,    44,     6,     6,    50,
      82,     6,    88,     6,     6,    98,    94,    94,    95,    98,
      98,    98,    95,    95,     6,    98,     6,     6,    98,    95,
      95,    98,    98,    98,    98
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* This macro is provided for backward compatibility. */

#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (0, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  YYSIZE_T yysize1;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = 0;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - Assume YYFAIL is not used.  It's too flawed to consider.  See
       <http://lists.gnu.org/archive/html/bison-patches/2009-12/msg00024.html>
       for details.  YYERROR is fine as it does not invoke this
       function.
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                yysize1 = yysize + yytnamerr (0, yytname[yyx]);
                if (! (yysize <= yysize1
                       && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                  return 2;
                yysize = yysize1;
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  yysize1 = yysize + yystrlen (yyformat);
  if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
    return 2;
  yysize = yysize1;

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:

/* Line 1806 of yacc.c  */
#line 182 "parse_spice.y"
    {
  }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 184 "parse_spice.y"
    {
    spice_title = (yyvsp[(1) - (3)].str);
  }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 187 "parse_spice.y"
    {
    fprintf (stderr, "spice notice, no .END directive found, continuing\n");
  }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 190 "parse_spice.y"
    {
    spice_title = (yyvsp[(1) - (2)].str);
    fprintf (stderr, "spice notice, no .END directive found, continuing\n");
  }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 201 "parse_spice.y"
    {
    /* chain definition root */
    (yyvsp[(1) - (1)].definition)->next = definition_root;
    definition_root = (yyvsp[(1) - (1)].definition);
  }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 206 "parse_spice.y"
    {
    /* chain definition root */
    if ((yyvsp[(1) - (1)].definition)) {
      (yyvsp[(1) - (1)].definition)->next = definition_root;
      definition_root = (yyvsp[(1) - (1)].definition);
    }
  }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 213 "parse_spice.y"
    { /* nothing to do here */ }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 217 "parse_spice.y"
    {
    /* R, L and C definitions */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NUMBER);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (6)].value));
  }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 225 "parse_spice.y"
    {
    /* R, L and C definitions specified by a Model */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NUMBER);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
  }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 234 "parse_spice.y"
    {
    /* R, L and C definitions specified by a Model, a variant */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(5) - (6)].ident), HINT_NUMBER);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NAME);
  }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 242 "parse_spice.y"
    {
    /* R definitions specified by a Model, yet another variant */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (6)].value));
  }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 250 "parse_spice.y"
    {
    /* R definitions including TC1 */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NUMBER);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (6)].value));
  }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 258 "parse_spice.y"
    {
    /* R definitions including TC1/TC2 */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NUMBER);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (6)].value));
  }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 266 "parse_spice.y"
    {
    /* non-linear C and L poly definitions */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NUMBER);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
  }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 275 "parse_spice.y"
    {
    /* Mutual inductors */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (5)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (5)].ident), HINT_NAME);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (5)].ident), HINT_NAME);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (5)].ident), HINT_NUMBER);
  }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 282 "parse_spice.y"
    {
    /* independent current/voltage sources */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (5)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (5)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (5)].ident), HINT_NODE);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(4) - (5)].value));
  }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 289 "parse_spice.y"
    {
    /* independent current/voltage sources given the value */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NUMBER);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (6)].value));
  }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 297 "parse_spice.y"
    {
    /* voltage controlled source POLY */
    if (!strcasecmp ((yyvsp[(4) - (7)].ident), "POLY")) {
      (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
      spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
      spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
      spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NAME);
      spice_append_val_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NUMBER);
      (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
    }
    else {
      fprintf (stderr, "spice notice, behavioural %s source ignored\n", (yyvsp[(1) - (7)].ident));
      (yyval.definition) = NULL;
    }
  }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 312 "parse_spice.y"
    {
    /* voltage controlled sources OTHER behavioural */
    fprintf (stderr, "spice notice, behavioural %s source ignored\n", (yyvsp[(1) - (5)].ident));
    (yyval.definition) = NULL;
  }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 317 "parse_spice.y"
    {
    /* voltage controlled sources */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NODE);
    spice_append_val_value ((yyval.definition), (yyvsp[(6) - (7)].ident), HINT_NUMBER);
  }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 326 "parse_spice.y"
    {
    /* current controlled source POLY */
    if (!strcasecmp ((yyvsp[(4) - (8)].ident), "POLY")) {
      (yyval.definition) = spice_create_device ((yyvsp[(1) - (8)].ident));
      spice_append_str_value ((yyval.definition), (yyvsp[(2) - (8)].ident), HINT_NODE);
      spice_append_str_value ((yyval.definition), (yyvsp[(3) - (8)].ident), HINT_NODE);
      spice_append_str_value ((yyval.definition), (yyvsp[(4) - (8)].ident), HINT_NAME);
      spice_append_val_value ((yyval.definition), (yyvsp[(5) - (8)].ident), HINT_NUMBER);
      (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (8)].value));
      (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(7) - (8)].value));
    }
    else {
      fprintf (stderr, "spice notice, behavioural %s source ignored\n", (yyvsp[(1) - (8)].ident));
      (yyval.definition) = NULL;
    }
  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 342 "parse_spice.y"
    {
    /* current controlled sources OTHER behavioural */
    fprintf (stderr, "spice notice, behavioural %s source ignored\n", (yyvsp[(1) - (5)].ident));
    (yyval.definition) = NULL;
  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 347 "parse_spice.y"
    {
    /* current controlled sources */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NAME);
    spice_append_val_value ((yyval.definition), (yyvsp[(5) - (6)].ident), HINT_NUMBER);
  }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 355 "parse_spice.y"
    {
    /* device specification */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (5)].ident), (yyvsp[(2) - (5)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (5)].ident), HINT_NAME | HINT_MSTART);
    spice_add_last_hint ((yyvsp[(4) - (5)].value), HINT_MSTOP);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(4) - (5)].value));
  }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 362 "parse_spice.y"
    {
    /* diode */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (5)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (5)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (5)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (5)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(5) - (5)].value));
  }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 370 "parse_spice.y"
    {
    /* JFET */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (6)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (6)].value));
  }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 379 "parse_spice.y"
    {
    /* 3 node BJT */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (6)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (6)].value));
  }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 388 "parse_spice.y"
    {
    /* 4 node BJT */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(6) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(7) - (7)].value));
  }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 398 "parse_spice.y"
    {
    /* MOS */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(6) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(7) - (7)].value));
  }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 408 "parse_spice.y"
    {
    /* MES */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (6)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (6)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (6)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (6)].value));
  }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 417 "parse_spice.y"
    {
    /* transient analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 422 "parse_spice.y"
    {
    /* plotting */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (4)].ident), strdup ((yyvsp[(1) - (4)].ident)));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (4)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(3) - (4)].value));
  }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 428 "parse_spice.y"
    {
    /* AC analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 433 "parse_spice.y"
    {
    /* single DC analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 437 "parse_spice.y"
    {
    /* DC analysis first order */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 442 "parse_spice.y"
    {
    /* DC analysis second order */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (4)].ident), strdup ((yyvsp[(1) - (4)].ident)));
    (yyval.definition)->values = netlist_append_values ((yyvsp[(2) - (4)].value), (yyvsp[(3) - (4)].value));
  }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 447 "parse_spice.y"
    {
    /* printing specifying the analysis type */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (4)].ident), strdup ((yyvsp[(1) - (4)].ident)));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (4)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(3) - (4)].value));
  }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 453 "parse_spice.y"
    {
    /* printing */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 458 "parse_spice.y"
    {
    /* printing */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (4)].ident), strdup ((yyvsp[(1) - (4)].ident)));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (4)].ident), HINT_NAME);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (4)].ident), HINT_NAME);
  }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 464 "parse_spice.y"
    {
    /* general analysis options */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 469 "parse_spice.y"
    {
    /* temperatur analysis (Spice 2g6) */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 474 "parse_spice.y"
    {
    /* TODO: default width of ??? */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 479 "parse_spice.y"
    {
    /* noise analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (5)].ident), strdup ((yyvsp[(1) - (5)].ident)));
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(2) - (5)].value));
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (5)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(4) - (5)].value));
  }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 486 "parse_spice.y"
    {
    /* pole-zero analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (8)].ident), strdup ((yyvsp[(1) - (8)].ident)));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(6) - (8)].ident), HINT_NAME);
    spice_append_str_value ((yyval.definition), (yyvsp[(7) - (8)].ident), HINT_NAME);
  }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 496 "parse_spice.y"
    {
    /* subcircuit call */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (3)].ident));
    spice_set_last_hint ((yyvsp[(2) - (3)].value), HINT_NAME);
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 502 "parse_spice.y"
    {
    /* voltage controlled switch */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (8)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(6) - (8)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(7) - (8)].value));
  }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 512 "parse_spice.y"
    {
    /* current controlled switch */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NAME);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
  }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 521 "parse_spice.y"
    {
    /* lossy transline */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(6) - (7)].ident), HINT_NAME);
  }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 530 "parse_spice.y"
    {
    /* distributed lossy transline */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NAME);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
  }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 539 "parse_spice.y"
    {
    /* lossless transline */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (7)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (7)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (7)].ident), HINT_NODE);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (7)].value));
  }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 548 "parse_spice.y"
    {
    /* lossless transline and initial condition */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (8)].ident));
    spice_append_str_value ((yyval.definition), (yyvsp[(2) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(3) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(4) - (8)].ident), HINT_NODE);
    spice_append_str_value ((yyval.definition), (yyvsp[(5) - (8)].ident), HINT_NODE);
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(6) - (8)].value));
    (yyval.definition)->values = netlist_append_values ((yyval.definition)->values, (yyvsp[(7) - (8)].value));
  }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 558 "parse_spice.y"
    {
    /* operating point analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 562 "parse_spice.y"
    {
    /* saving action */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 566 "parse_spice.y"
    {
    /* sensitivity analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 570 "parse_spice.y"
    {
    /* transfer function analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 574 "parse_spice.y"
    {
    /* fourier analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (2)].ident), strdup ((yyvsp[(1) - (2)].ident)));
  }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 578 "parse_spice.y"
    {
    /* non-linear dependent sources */
    (yyval.definition) = spice_create_device ((yyvsp[(1) - (2)].ident));
  }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 582 "parse_spice.y"
    {
    /* distortion analysis */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 587 "parse_spice.y"
    {
    /* file include */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    struct value_t * file = create_value ();
    file->ident = (yyvsp[(2) - (3)].ident);
    file->hint = HINT_NAME;
    (yyval.definition)->values = file;
  }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 595 "parse_spice.y"
    {
    /* nodeset functionality */
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (3)].ident), strdup ((yyvsp[(1) - (3)].ident)));
    (yyval.definition)->values = (yyvsp[(2) - (3)].value);
  }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 603 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (2)].ident), (yyvsp[(2) - (2)].ident));
  }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 610 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(3) - (3)].ident), HINT_NUMBER);
  }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 618 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (2)].ident), HINT_NUMBER);
  }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 626 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (3)].ident), HINT_NAME);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (3)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(3) - (3)].ident), HINT_NUMBER);
  }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 634 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (4)].ident), HINT_NAME);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (4)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(3) - (4)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(4) - (4)].ident), HINT_NUMBER);
  }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 644 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (5)].ident), HINT_NAME);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (5)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(3) - (5)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(4) - (5)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(5) - (5)].ident), HINT_NUMBER);
  }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 655 "parse_spice.y"
    { /* range specification during plotting */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(1) - (2)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (2)].ident), HINT_NUMBER);
  }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 663 "parse_spice.y"
    { // TODO: 2 reduce/reduce, 2 shift/reduce
    /* print/plot specification of node voltage */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), strdup ("V"), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (1)].ident), HINT_NODE | HINT_MSTOP);
  }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 669 "parse_spice.y"
    { // TODO: 2 reduce/reduce
    /* print/plot specification of node voltage */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (2)].ident), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(2) - (2)].ident), HINT_NODE | HINT_MSTOP);
  }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 675 "parse_spice.y"
    {
    /* print/plot specification of differential node voltages */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (3)].ident), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(2) - (3)].ident), HINT_NODE);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(3) - (3)].ident), HINT_NODE | HINT_MSTOP);    
  }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 688 "parse_spice.y"
    {
    /* print/plot specification of branch current */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (2)].ident), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(2) - (2)].ident), HINT_NAME | HINT_MSTOP);
  }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 694 "parse_spice.y"
    {
    /* print/plot specification of branch current */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), strdup ("I"), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (1)].ident), HINT_NAME | HINT_MSTOP);
  }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 700 "parse_spice.y"
    {
    /* print/plot specification of operating point */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), strdup ("OP"), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (1)].ident), HINT_NAME | HINT_MSTOP);
  }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 708 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 709 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 712 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (3)].value), (yyvsp[(2) - (3)].value));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 716 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 719 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (3)].value), (yyvsp[(2) - (3)].value));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 725 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 726 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (1)].ident), HINT_NAME);
  }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 729 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (1)].ident), HINT_NAME);
  }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 734 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 735 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 738 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 743 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 744 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 748 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 754 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 755 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 759 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 765 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 766 "parse_spice.y"
    {
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (4)].ident), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(2) - (4)].ident), HINT_NODE | HINT_MSTOP);
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(3) - (4)].ident), HINT_NUMBER);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(4) - (4)].value));
  }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 775 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 776 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 780 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value (strdup ("Area"), (yyvsp[(1) - (2)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 101:

/* Line 1806 of yacc.c  */
#line 784 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 102:

/* Line 1806 of yacc.c  */
#line 788 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 103:

/* Line 1806 of yacc.c  */
#line 793 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 104:

/* Line 1806 of yacc.c  */
#line 794 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 105:

/* Line 1806 of yacc.c  */
#line 798 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value (strdup ("Area"), (yyvsp[(1) - (2)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 106:

/* Line 1806 of yacc.c  */
#line 802 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 107:

/* Line 1806 of yacc.c  */
#line 806 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 108:

/* Line 1806 of yacc.c  */
#line 809 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 109:

/* Line 1806 of yacc.c  */
#line 815 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 110:

/* Line 1806 of yacc.c  */
#line 816 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 111:

/* Line 1806 of yacc.c  */
#line 820 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(3) - (3)].value));
  }
    break;

  case 112:

/* Line 1806 of yacc.c  */
#line 824 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NUMBER);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 113:

/* Line 1806 of yacc.c  */
#line 828 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value) = netlist_append_values ((yyval.value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 114:

/* Line 1806 of yacc.c  */
#line 832 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 117:

/* Line 1806 of yacc.c  */
#line 840 "parse_spice.y"
    {
    /* identification of a DC sweep */
    (yyval.value) = NULL;
    (yyval.value) = spice_append_str_values ((yyval.value), (yyvsp[(1) - (4)].ident), HINT_NAME | HINT_MSTART);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(2) - (4)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(3) - (4)].ident), HINT_NUMBER);
    (yyval.value) = spice_append_val_values ((yyval.value), (yyvsp[(4) - (4)].ident), HINT_NUMBER | HINT_MSTOP);
  }
    break;

  case 125:

/* Line 1806 of yacc.c  */
#line 856 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 126:

/* Line 1806 of yacc.c  */
#line 857 "parse_spice.y"
    {
    (yyval.value) = spice_create_par_value ((yyvsp[(1) - (3)].ident), (yyvsp[(2) - (3)].ident));
    (yyval.value)->next = (yyvsp[(3) - (3)].value);
  }
    break;

  case 127:

/* Line 1806 of yacc.c  */
#line 863 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 128:

/* Line 1806 of yacc.c  */
#line 864 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NUMBER);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 129:

/* Line 1806 of yacc.c  */
#line 870 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 130:

/* Line 1806 of yacc.c  */
#line 871 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NODE);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 131:

/* Line 1806 of yacc.c  */
#line 875 "parse_spice.y"
    {
    (yyval.value) = spice_create_val_value ((yyvsp[(1) - (2)].ident), HINT_NUMBER);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 132:

/* Line 1806 of yacc.c  */
#line 881 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 133:

/* Line 1806 of yacc.c  */
#line 882 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NODE);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 134:

/* Line 1806 of yacc.c  */
#line 888 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 135:

/* Line 1806 of yacc.c  */
#line 889 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NAME);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 136:

/* Line 1806 of yacc.c  */
#line 896 "parse_spice.y"
    {
    (yyval.value) = spice_create_str_value ((yyvsp[(1) - (2)].ident), HINT_NAME | HINT_MSTART);
    spice_add_last_hint ((yyvsp[(2) - (2)].value), HINT_MSTOP);
    (yyval.value)->next = (yyvsp[(2) - (2)].value);
  }
    break;

  case 137:

/* Line 1806 of yacc.c  */
#line 903 "parse_spice.y"
    { (yyval.value) = NULL; }
    break;

  case 138:

/* Line 1806 of yacc.c  */
#line 904 "parse_spice.y"
    {
    (yyval.value) = netlist_append_values ((yyvsp[(1) - (2)].value), (yyvsp[(2) - (2)].value));
  }
    break;

  case 139:

/* Line 1806 of yacc.c  */
#line 910 "parse_spice.y"
    {
    (yyvsp[(1) - (3)].definition)->sub = (yyvsp[(2) - (3)].definition);
    (yyval.definition) = (yyvsp[(1) - (3)].definition);
    (yyvsp[(2) - (3)].definition) = NULL;
  }
    break;

  case 140:

/* Line 1806 of yacc.c  */
#line 918 "parse_spice.y"
    {
    (yyval.definition) = spice_create_action ((yyvsp[(1) - (4)].ident), (yyvsp[(2) - (4)].ident));
    (yyval.definition)->values = (yyvsp[(3) - (4)].value);
  }
    break;

  case 141:

/* Line 1806 of yacc.c  */
#line 924 "parse_spice.y"
    { (yyval.definition) = NULL; }
    break;

  case 142:

/* Line 1806 of yacc.c  */
#line 925 "parse_spice.y"
    { /* chain definitions here */
    if ((yyvsp[(1) - (2)].definition)) {
      (yyvsp[(1) - (2)].definition)->next = (yyvsp[(2) - (2)].definition);
      (yyval.definition) = (yyvsp[(1) - (2)].definition);
    }
    else {
      (yyval.definition) = (yyvsp[(2) - (2)].definition);
    }
  }
    break;

  case 144:

/* Line 1806 of yacc.c  */
#line 939 "parse_spice.y"
    { /* nothing to do */ }
    break;

  case 145:

/* Line 1806 of yacc.c  */
#line 940 "parse_spice.y"
    { free ((yyvsp[(2) - (2)].ident)); /* nothing to do */ }
    break;

  case 146:

/* Line 1806 of yacc.c  */
#line 944 "parse_spice.y"
    { /* chain definitions here */
    if ((yyvsp[(1) - (1)].definition)) {
      (yyvsp[(1) - (1)].definition)->next = (yyval.definition);
      (yyval.definition) = (yyvsp[(1) - (1)].definition);
    }
  }
    break;

  case 147:

/* Line 1806 of yacc.c  */
#line 950 "parse_spice.y"
    { /* do nothing here, see subcircuit rule */ }
    break;

  case 148:

/* Line 1806 of yacc.c  */
#line 951 "parse_spice.y"
    {
    (yyval.definition) = NULL;
  }
    break;



/* Line 1806 of yacc.c  */
#line 3484 "parse_spice.cpp"
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 2067 of yacc.c  */
#line 956 "parse_spice.y"


int spice_error (const char * error) {
  fprintf (stderr, "line %d: %s\n", spice_lineno, error);
  return 0;
}

