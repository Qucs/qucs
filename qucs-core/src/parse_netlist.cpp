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
#define yyparse         netlist_parse
#define yylex           netlist_lex
#define yyerror         netlist_error
#define yylval          netlist_lval
#define yychar          netlist_char
#define yydebug         netlist_debug
#define yynerrs         netlist_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 3 "parse_netlist.y"

/*
 * parse_netlist.y - parser for the Qucs netlist
 *
 * Copyright (C) 2003, 2004, 2005, 2006, 2007 Stefan Jahn <stefan@lkcc.org>
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

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#include "check_netlist.h"
#include "logging.h"
#include "equation.h"
#include "range.h"



/* Line 268 of yacc.c  */
#line 124 "parse_netlist.cpp"

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

/* Line 293 of yacc.c  */
#line 76 "parse_netlist.y"

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
  eqn::node * eqn;
  eqn::constant * con;
  eqn::reference * ref;
  eqn::application * app;
  eqn::assignment * assign;



/* Line 293 of yacc.c  */
#line 235 "parse_netlist.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 247 "parse_netlist.cpp"

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
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   292

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  44
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  36
/* YYNRULES -- Number of rules.  */
#define YYNRULES  96
/* YYNRULES -- Number of states.  */
#define YYNSTATES  164

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   281

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    37,     2,     2,    31,     2,     2,
      41,    42,    29,    28,    43,    27,    36,    30,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    18,    40,
       2,    16,     2,    17,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    38,     2,    39,    35,     2,     2,     2,     2,     2,
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
      15,    19,    20,    21,    22,    23,    24,    25,    26,    32,
      33,    34
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    17,
      19,    26,    33,    35,    37,    39,    41,    42,    45,    46,
      50,    54,    55,    58,    60,    64,    66,    69,    73,    75,
      77,    81,    85,    86,    88,    92,    99,   100,   103,   108,
     110,   112,   114,   118,   122,   126,   129,   130,   134,   136,
     138,   139,   143,   148,   150,   152,   154,   156,   158,   162,
     165,   168,   170,   172,   177,   182,   186,   190,   194,   198,
     202,   205,   208,   212,   218,   222,   226,   230,   234,   238,
     242,   245,   249,   253,   254,   256,   258,   262,   266,   270,
     276,   277,   280,   283,   285,   287,   289
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      45,     0,    -1,    46,    -1,    -1,    47,    46,    -1,    75,
      -1,    59,    -1,    48,    -1,    49,    -1,     7,    -1,    36,
       4,    18,    50,    53,     7,    -1,     4,    18,    50,    52,
      53,     7,    -1,     4,    -1,     6,    -1,     4,    -1,     6,
      -1,    -1,    51,    52,    -1,    -1,     5,    55,    53,    -1,
       5,    54,    53,    -1,    -1,    37,    37,    -1,    57,    -1,
      37,    57,    37,    -1,    11,    -1,    11,     6,    -1,    11,
       6,     6,    -1,    56,    -1,    50,    -1,    38,    50,    39,
      -1,    38,    58,    39,    -1,    -1,    56,    -1,    56,    40,
      58,    -1,     8,    18,    50,    61,    60,     7,    -1,    -1,
      61,    60,    -1,     5,    37,    62,    37,    -1,    70,    -1,
      72,    -1,    73,    -1,    41,    62,    42,    -1,    38,    66,
      39,    -1,    38,    69,    39,    -1,    62,    64,    -1,    -1,
      43,    62,    64,    -1,    63,    -1,    65,    -1,    -1,    40,
      63,    67,    -1,    63,    40,    63,    67,    -1,    68,    -1,
      11,    -1,    12,    -1,    14,    -1,    15,    -1,    62,    18,
      62,    -1,    18,    62,    -1,    62,    18,    -1,    18,    -1,
       4,    -1,     4,    41,    74,    42,    -1,    72,    38,    74,
      39,    -1,    62,    28,    62,    -1,    62,    27,    62,    -1,
      62,    29,    62,    -1,    62,    30,    62,    -1,    62,    31,
      62,    -1,    28,    62,    -1,    27,    62,    -1,    62,    35,
      62,    -1,    62,    17,    62,    18,    62,    -1,    62,    26,
      62,    -1,    62,    25,    62,    -1,    62,    23,    62,    -1,
      62,    24,    62,    -1,    62,    21,    62,    -1,    62,    22,
      62,    -1,    32,    62,    -1,    62,    20,    62,    -1,    62,
      19,    62,    -1,    -1,    62,    -1,    71,    -1,    62,    43,
      74,    -1,    71,    43,    74,    -1,    76,    77,    78,    -1,
       9,    50,    52,    53,     7,    -1,    -1,    79,    77,    -1,
      10,     7,    -1,    49,    -1,    59,    -1,    75,    -1,     7,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   120,   120,   125,   126,   137,   140,   141,   142,   143,
     149,   160,   172,   173,   177,   178,   181,   182,   189,   190,
     196,   209,   210,   214,   217,   223,   227,   232,   241,   244,
     248,   252,   257,   258,   262,   269,   283,   284,   291,   299,
     302,   305,   308,   311,   314,   320,   326,   327,   334,   338,
     346,   347,   357,   368,   377,   381,   385,   389,   396,   403,
     412,   421,   435,   442,   448,   455,   462,   469,   476,   483,
     490,   496,   502,   509,   517,   524,   531,   538,   545,   552,
     559,   565,   572,   581,   582,   585,   588,   592,   599,   607,
     619,   620,   632,   636,   640,   644,   645
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "InvalidCharacter", "Identifier",
  "Assign", "ScaleOrUnit", "Eol", "Eqn", "DefSub", "EndSub", "REAL",
  "IMAG", "COMPLEX", "Character", "STRING", "'='", "'?'", "':'", "Or",
  "And", "Equal", "NotEqual", "GreaterOrEqual", "LessOrEqual", "Greater",
  "Less", "'-'", "'+'", "'*'", "'/'", "'%'", "Not", "NEG", "POS", "'^'",
  "'.'", "'\"'", "'['", "']'", "';'", "'('", "')'", "','", "$accept",
  "Input", "InputList", "InputLine", "ActionLine", "DefinitionLine",
  "InstanceIdentifier", "NodeIdentifier", "NodeList", "PairList",
  "NoneValue", "Value", "PropertyReal", "PropertyValue", "ValueList",
  "EquationLine", "EquationList", "Equation", "Expression",
  "ExpressionCol", "ExpressionColList", "ExpressionVector", "Vector",
  "ExpressionRowList", "ExpressionMatrix", "Matrix", "Constant", "Range",
  "Reference", "Application", "ExpressionList", "SubcircuitBody",
  "DefBegin", "DefBody", "DefEnd", "DefBodyLine", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,    61,    63,    58,   271,
     272,   273,   274,   275,   276,   277,   278,    45,    43,    42,
      47,    37,   279,   280,   281,    94,    46,    34,    91,    93,
      59,    40,    41,    44
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    44,    45,    46,    46,    47,    47,    47,    47,    47,
      48,    49,    50,    50,    51,    51,    52,    52,    53,    53,
      53,    54,    54,    55,    55,    56,    56,    56,    57,    57,
      57,    57,    58,    58,    58,    59,    60,    60,    61,    62,
      62,    62,    62,    62,    62,    63,    64,    64,    65,    66,
      67,    67,    68,    69,    70,    70,    70,    70,    71,    71,
      71,    71,    72,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    73,    73,    73,    73,    73,    73,    73,
      73,    73,    73,    74,    74,    74,    74,    74,    75,    76,
      77,    77,    78,    79,    79,    79,    79
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     1,     1,
       6,     6,     1,     1,     1,     1,     0,     2,     0,     3,
       3,     0,     2,     1,     3,     1,     2,     3,     1,     1,
       3,     3,     0,     1,     3,     6,     0,     2,     4,     1,
       1,     1,     3,     3,     3,     2,     0,     3,     1,     1,
       0,     3,     4,     1,     1,     1,     1,     1,     3,     2,
       2,     1,     1,     4,     4,     3,     3,     3,     3,     3,
       2,     2,     3,     5,     3,     3,     3,     3,     3,     3,
       2,     3,     3,     0,     1,     1,     3,     3,     3,     5,
       0,     2,     2,     1,     1,     1,     1
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     9,     0,     0,     0,     0,     2,     3,     7,
       8,     6,     5,    90,     0,     0,    12,    13,    16,     0,
       1,     4,    96,    93,    94,    95,     0,    90,    16,     0,
      14,    15,    16,    18,     0,     0,    88,    91,    18,     0,
      36,    17,    21,     0,    18,    92,     0,     0,     0,    36,
      25,     0,    32,    29,    18,    18,    28,    23,    89,     0,
      11,    62,    54,    55,    56,    57,     0,     0,     0,     0,
       0,     0,    39,    40,    41,    35,    37,    26,    22,     0,
       0,    33,     0,    20,    19,    10,    83,    71,    70,    80,
      46,    48,    49,     0,    53,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    38,    83,    27,    24,    30,    32,    31,    61,
      84,    85,     0,     0,    45,     0,    43,    44,    42,     0,
      82,    81,    78,    79,    76,    77,    75,    74,    66,    65,
      67,    68,    69,    72,     0,    34,    59,    60,    83,    83,
      63,    46,    50,     0,    64,    58,    86,    87,    47,     0,
      52,    73,    50,    51
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     6,     7,     8,     9,    10,    53,    32,    33,    43,
      54,    55,    56,    57,    82,    11,    48,    49,   120,    91,
     124,    92,    93,   160,    94,    95,    72,   121,    73,    74,
     122,    12,    13,    26,    36,    27
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -118
static const yytype_int16 yypact[] =
{
      81,    -4,  -118,    15,    98,    39,    44,  -118,    81,  -118,
    -118,  -118,  -118,   107,    98,    98,  -118,  -118,    99,    30,
    -118,  -118,  -118,  -118,  -118,  -118,    58,   107,    99,    65,
    -118,  -118,    99,    66,    98,    42,  -118,  -118,    66,    49,
      65,  -118,     0,    72,    66,  -118,    80,    69,    91,    65,
     131,    71,    63,  -118,    66,    66,  -118,  -118,  -118,   106,
    -118,    97,  -118,  -118,  -118,  -118,    69,    69,    69,    69,
      69,   175,  -118,   102,  -118,  -118,  -118,   135,  -118,   105,
     119,   120,   123,  -118,  -118,  -118,    -2,   124,   124,   124,
     126,   125,  -118,   127,  -118,   128,   151,    69,    69,    69,
      69,    69,    69,    69,    69,    69,    69,    69,    69,    69,
      69,    69,  -118,    -2,  -118,  -118,  -118,   152,  -118,    69,
     101,   121,   141,    69,  -118,    69,  -118,  -118,  -118,   196,
     229,   244,   257,   257,    64,    64,    64,    64,   104,   104,
     124,   124,   124,   124,   145,  -118,   213,    69,    -2,    -2,
    -118,   126,   147,    69,  -118,   213,  -118,  -118,  -118,    69,
    -118,   213,   147,  -118
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -118,  -118,   177,  -118,  -118,     2,    13,  -118,   -25,   -20,
    -118,  -118,   -51,   137,    73,    18,   140,   162,   -47,  -117,
      56,  -118,  -118,    46,  -118,  -118,  -118,  -118,  -118,  -118,
    -108,    19,  -118,   182,  -118,  -118
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      71,    81,    61,    38,    16,   144,    17,    41,   152,    62,
      63,    50,    64,    65,    14,    23,   119,    18,    46,    87,
      88,    89,    90,    96,    59,    66,    67,    28,    29,    23,
      68,    24,    25,    15,    83,    84,    69,    51,    52,    70,
     156,   157,   162,    19,    20,    24,    25,    44,    34,    45,
     129,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,    80,    81,    16,    35,    17,
      39,    42,   146,    61,    50,    16,   151,    17,    90,    58,
      62,    63,    50,    64,    65,     1,    47,    60,     2,     3,
       4,   106,   107,   108,   109,   110,    66,    67,    75,   111,
     155,    68,    16,    30,    17,    31,   161,    69,    78,    52,
      70,     1,    90,    85,    22,     3,     4,     5,    97,   147,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   108,   109,   110,   111,    77,    86,   111,
     113,   114,   115,    97,   148,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,   116,   111,
     117,   111,   118,    50,   149,   125,   126,   127,    97,   123,
      98,    99,   100,   101,   102,   103,   104,   105,   106,   107,
     108,   109,   110,   150,   154,    21,   111,   159,    79,    76,
     145,    40,    97,   128,    98,    99,   100,   101,   102,   103,
     104,   105,   106,   107,   108,   109,   110,   158,   163,    37,
     111,     0,   112,    97,   153,    98,    99,   100,   101,   102,
     103,   104,   105,   106,   107,   108,   109,   110,     0,     0,
      97,   111,    98,    99,   100,   101,   102,   103,   104,   105,
     106,   107,   108,   109,   110,     0,     0,     0,   111,    99,
     100,   101,   102,   103,   104,   105,   106,   107,   108,   109,
     110,     0,     0,     0,   111,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,     0,     0,     0,   111,
     102,   103,   104,   105,   106,   107,   108,   109,   110,     0,
       0,     0,   111
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-118))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
      47,    52,     4,    28,     4,   113,     6,    32,   125,    11,
      12,    11,    14,    15,    18,    13,    18,     4,    38,    66,
      67,    68,    69,    70,    44,    27,    28,    14,    15,    27,
      32,    13,    13,    18,    54,    55,    38,    37,    38,    41,
     148,   149,   159,     4,     0,    27,    27,    34,    18,     7,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,    52,   117,     4,    10,     6,
       5,     5,   119,     4,    11,     4,   123,     6,   125,     7,
      11,    12,    11,    14,    15,     4,    37,     7,     7,     8,
       9,    27,    28,    29,    30,    31,    27,    28,     7,    35,
     147,    32,     4,     4,     6,     6,   153,    38,    37,    38,
      41,     4,   159,     7,     7,     8,     9,    36,    17,    18,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    29,    30,    31,    35,     6,    41,    35,
      38,     6,    37,    17,    43,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    39,    35,
      40,    35,    39,    11,    43,    40,    39,    39,    17,    43,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    42,    39,     8,    35,    40,    51,    49,
     117,    29,    17,    42,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,   151,   162,    27,
      35,    -1,    37,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    -1,    -1,
      17,    35,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    -1,    -1,    -1,    35,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    -1,    -1,    -1,    35,    21,    22,    23,    24,    25,
      26,    27,    28,    29,    30,    31,    -1,    -1,    -1,    35,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    -1,
      -1,    -1,    35
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     7,     8,     9,    36,    45,    46,    47,    48,
      49,    59,    75,    76,    18,    18,     4,     6,    50,     4,
       0,    46,     7,    49,    59,    75,    77,    79,    50,    50,
       4,     6,    51,    52,    18,    10,    78,    77,    52,     5,
      61,    52,     5,    53,    50,     7,    53,    37,    60,    61,
      11,    37,    38,    50,    54,    55,    56,    57,     7,    53,
       7,     4,    11,    12,    14,    15,    27,    28,    32,    38,
      41,    62,    70,    72,    73,     7,    60,     6,    37,    57,
      50,    56,    58,    53,    53,     7,    41,    62,    62,    62,
      62,    63,    65,    66,    68,    69,    62,    17,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    35,    37,    38,     6,    37,    39,    40,    39,    18,
      62,    71,    74,    43,    64,    40,    39,    39,    42,    62,
      62,    62,    62,    62,    62,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    74,    58,    62,    18,    43,    43,
      42,    62,    63,    18,    39,    62,    74,    74,    64,    40,
      67,    62,    63,    67
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
#line 120 "parse_netlist.y"
    {
    definition_root = (yyvsp[(1) - (1)].definition);
  }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 125 "parse_netlist.y"
    { (yyval.definition) = NULL; }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 126 "parse_netlist.y"
    {
    if ((yyvsp[(1) - (2)].definition)) {
      (yyvsp[(1) - (2)].definition)->next = (yyvsp[(2) - (2)].definition);
      (yyval.definition) = (yyvsp[(1) - (2)].definition);
    } else {
      (yyval.definition) = (yyvsp[(2) - (2)].definition);
    }
  }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 137 "parse_netlist.y"
    {
    (yyval.definition) = (yyvsp[(1) - (1)].subcircuit);
  }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 143 "parse_netlist.y"
    {
    (yyval.definition) = NULL;
  }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 149 "parse_netlist.y"
    { 
    (yyval.definition) = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    (yyval.definition)->action = PROP_ACTION;
    (yyval.definition)->type = (yyvsp[(2) - (6)].ident);
    (yyval.definition)->instance = (yyvsp[(4) - (6)].ident);
    (yyval.definition)->pairs = (yyvsp[(5) - (6)].pair);
    (yyval.definition)->line = netlist_lineno;
  }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 160 "parse_netlist.y"
    { 
    (yyval.definition) = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    (yyval.definition)->action = PROP_COMPONENT;
    (yyval.definition)->type = (yyvsp[(1) - (6)].ident);
    (yyval.definition)->instance = (yyvsp[(3) - (6)].ident);
    (yyval.definition)->nodes = (yyvsp[(4) - (6)].node);
    (yyval.definition)->pairs = (yyvsp[(5) - (6)].pair);
    (yyval.definition)->line = netlist_lineno;
  }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 172 "parse_netlist.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].ident); }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 173 "parse_netlist.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].str); }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 177 "parse_netlist.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].ident); }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 178 "parse_netlist.y"
    { (yyval.ident) = (yyvsp[(1) - (1)].str); }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 181 "parse_netlist.y"
    { (yyval.node) = NULL; }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 182 "parse_netlist.y"
    {
    (yyval.node) = (struct node_t *) calloc (sizeof (struct node_t), 1);
    (yyval.node)->node = (yyvsp[(1) - (2)].ident);
    (yyval.node)->next = (yyvsp[(2) - (2)].node);
  }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 189 "parse_netlist.y"
    { (yyval.pair) = NULL; }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 190 "parse_netlist.y"
    {
    (yyval.pair) = (struct pair_t *) calloc (sizeof (struct pair_t), 1);
    (yyval.pair)->key = (yyvsp[(1) - (3)].ident);
    (yyval.pair)->value = (yyvsp[(2) - (3)].value);
    (yyval.pair)->next = (yyvsp[(3) - (3)].pair);
  }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 196 "parse_netlist.y"
    {
    if (0) {
      (yyval.pair) = (struct pair_t *) calloc (sizeof (struct pair_t), 1);
      (yyval.pair)->key = (yyvsp[(1) - (3)].ident);
      (yyval.pair)->value = NULL;
      (yyval.pair)->next = (yyvsp[(3) - (3)].pair);
    } else {
      free ((yyvsp[(1) - (3)].ident));
      (yyval.pair) = (yyvsp[(3) - (3)].pair);
    }
  }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 209 "parse_netlist.y"
    { /* nothing */ }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 210 "parse_netlist.y"
    { /* also nothing */ }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 214 "parse_netlist.y"
    {
    (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 217 "parse_netlist.y"
    {
    (yyval.value) = (yyvsp[(2) - (3)].value);
  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 223 "parse_netlist.y"
    {
    (yyval.value) = create_value ();
    (yyval.value)->value = (yyvsp[(1) - (1)].d);
  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 227 "parse_netlist.y"
    {
    (yyval.value) = create_value ();
    (yyval.value)->value = (yyvsp[(1) - (2)].d);
    (yyval.value)->scale = (yyvsp[(2) - (2)].str);
  }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 232 "parse_netlist.y"
    {
    (yyval.value) = create_value ();
    (yyval.value)->value = (yyvsp[(1) - (3)].d);
    (yyval.value)->scale = (yyvsp[(2) - (3)].str);
    (yyval.value)->unit = (yyvsp[(3) - (3)].str);
  }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 241 "parse_netlist.y"
    {
    (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 244 "parse_netlist.y"
    {
    (yyval.value) = create_value ();
    (yyval.value)->ident = (yyvsp[(1) - (1)].ident);
  }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 248 "parse_netlist.y"
    {
    (yyval.value) = create_value ();
    (yyval.value)->ident = (yyvsp[(2) - (3)].ident);
  }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 252 "parse_netlist.y"
    {
    (yyval.value) = (yyvsp[(2) - (3)].value);
  }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 257 "parse_netlist.y"
    { (yyval.value) = NULL; }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 258 "parse_netlist.y"
    {
    (yyvsp[(1) - (1)].value)->next = NULL;
    (yyval.value) = (yyvsp[(1) - (1)].value);
  }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 262 "parse_netlist.y"
    {
    (yyvsp[(1) - (3)].value)->next = (yyvsp[(3) - (3)].value);
    (yyval.value) = (yyvsp[(1) - (3)].value);
  }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 269 "parse_netlist.y"
    {
    /* create equation definition */
    (yyval.definition) = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    (yyval.definition)->type = strdup ("Eqn");
    (yyval.definition)->instance = (yyvsp[(3) - (6)].ident);
    (yyval.definition)->action = PROP_ACTION;
    (yyval.definition)->line = netlist_lineno;
    (yyvsp[(4) - (6)].assign)->setInstance ((yyvsp[(3) - (6)].ident));
    (yyvsp[(4) - (6)].assign)->setNext ((yyvsp[(5) - (6)].eqn));
    (yyvsp[(4) - (6)].assign)->applyInstance ();
    (yyval.definition)->eqns = (yyvsp[(4) - (6)].assign);
  }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 283 "parse_netlist.y"
    { (yyval.eqn) = NULL; }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 284 "parse_netlist.y"
    { 
    (yyvsp[(1) - (2)].assign)->setNext ((yyvsp[(2) - (2)].eqn));
    (yyval.eqn) = (yyvsp[(1) - (2)].assign);
  }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 291 "parse_netlist.y"
    {
    (yyval.assign) = new eqn::assignment ();
    (yyval.assign)->result = (yyvsp[(1) - (4)].ident);
    (yyval.assign)->body = (yyvsp[(3) - (4)].eqn);
  }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 299 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(1) - (1)].con);
  }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 302 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(1) - (1)].ref);
  }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 305 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(1) - (1)].app);
  }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 308 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(2) - (3)].eqn);
  }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 311 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(2) - (3)].app);
  }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 314 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(2) - (3)].app);
  }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 320 "parse_netlist.y"
    {
    (yyvsp[(1) - (2)].eqn)->appendNodes ((yyvsp[(2) - (2)].eqn));
    (yyval.eqn) = (yyvsp[(1) - (2)].eqn);
  }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 326 "parse_netlist.y"
    { (yyval.eqn) = NULL; }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 327 "parse_netlist.y"
    {
    (yyvsp[(2) - (3)].eqn)->appendNodes ((yyvsp[(3) - (3)].eqn));
    (yyval.eqn) = (yyvsp[(2) - (3)].eqn);
  }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 338 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("vector");
    (yyval.app)->nargs = (yyvsp[(1) - (1)].eqn)->count ();
    (yyval.app)->args = (yyvsp[(1) - (1)].eqn);
  }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 346 "parse_netlist.y"
    { (yyval.eqn) = NULL; }
    break;

  case 51:

/* Line 1806 of yacc.c  */
#line 347 "parse_netlist.y"
    {
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = ';';
    c->appendNodes ((yyvsp[(2) - (3)].eqn));
    (yyvsp[(2) - (3)].eqn)->appendNodes ((yyvsp[(3) - (3)].eqn));
    (yyval.eqn) = c;
  }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 357 "parse_netlist.y"
    {
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = ';';
    c->appendNodes ((yyvsp[(3) - (4)].eqn));
    (yyvsp[(3) - (4)].eqn)->appendNodes ((yyvsp[(4) - (4)].eqn));
    (yyvsp[(1) - (4)].eqn)->appendNodes (c);
    (yyval.eqn) = (yyvsp[(1) - (4)].eqn);
  }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 368 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("matrix");
    (yyval.app)->nargs = (yyvsp[(1) - (1)].eqn)->count ();
    (yyval.app)->args = (yyvsp[(1) - (1)].eqn);
  }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 377 "parse_netlist.y"
    { 
    (yyval.con) = new eqn::constant (eqn::TAG_DOUBLE);
    (yyval.con)->d = (yyvsp[(1) - (1)].d);
  }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 381 "parse_netlist.y"
    {
    (yyval.con) = new eqn::constant (eqn::TAG_COMPLEX);
    (yyval.con)->c = new nr_complex_t (0.0, (yyvsp[(1) - (1)].d));
  }
    break;

  case 56:

/* Line 1806 of yacc.c  */
#line 385 "parse_netlist.y"
    {
    (yyval.con) = new eqn::constant (eqn::TAG_CHAR);
    (yyval.con)->chr = (yyvsp[(1) - (1)].chr);
  }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 389 "parse_netlist.y"
    {
    (yyval.con) = new eqn::constant (eqn::TAG_STRING);
    (yyval.con)->s = (yyvsp[(1) - (1)].str);
  }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 396 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("range");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 403 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("range");
    (yyval.app)->nargs = 2;
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = '?';
    c->append ((yyvsp[(2) - (2)].eqn));
    (yyval.app)->args = c;
  }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 412 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("range");
    (yyval.app)->nargs = 2;
    eqn::constant * c = new eqn::constant (eqn::TAG_CHAR);
    c->chr = '?';
    (yyvsp[(1) - (2)].eqn)->append (c);
    (yyval.app)->args = (yyvsp[(1) - (2)].eqn);
  }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 421 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("range");
    (yyval.app)->nargs = 2;
    eqn::constant * c1 = new eqn::constant (eqn::TAG_CHAR);
    eqn::constant * c2 = new eqn::constant (eqn::TAG_CHAR);
    c1->chr = '?';
    c2->chr = '?';
    c1->append (c2);
    (yyval.app)->args = c1;
  }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 435 "parse_netlist.y"
    {
    (yyval.ref) = new eqn::reference ();
    (yyval.ref)->n = (yyvsp[(1) - (1)].ident);
  }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 442 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = (yyvsp[(1) - (4)].ident);
    (yyval.app)->nargs = (yyvsp[(3) - (4)].eqn)->count ();
    (yyval.app)->args = (yyvsp[(3) - (4)].eqn);
  }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 448 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("array");
    (yyval.app)->nargs = 1 + (yyvsp[(3) - (4)].eqn)->count ();
    (yyvsp[(1) - (4)].ref)->setNext ((yyvsp[(3) - (4)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (4)].ref);
  }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 455 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("+");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 462 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("-");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 469 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("*");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 476 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("/");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 69:

/* Line 1806 of yacc.c  */
#line 483 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("%");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 490 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("+");
    (yyval.app)->nargs = 1;
    (yyval.app)->args = (yyvsp[(2) - (2)].eqn);
  }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 496 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("-");
    (yyval.app)->nargs = 1;
    (yyval.app)->args = (yyvsp[(2) - (2)].eqn);
  }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 502 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("^");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 509 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("?:");
    (yyval.app)->nargs = 3;
    (yyvsp[(1) - (5)].eqn)->append ((yyvsp[(3) - (5)].eqn));
    (yyvsp[(1) - (5)].eqn)->append ((yyvsp[(5) - (5)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (5)].eqn);
  }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 517 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("<");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 524 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup (">");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 531 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup (">=");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 538 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("<=");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 545 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("==");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 552 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("!=");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 559 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("!");
    (yyval.app)->nargs = 1;
    (yyval.app)->args = (yyvsp[(2) - (2)].eqn);
  }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 565 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("&&");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 572 "parse_netlist.y"
    {
    (yyval.app) = new eqn::application ();
    (yyval.app)->n = strdup ("||");
    (yyval.app)->nargs = 2;
    (yyvsp[(1) - (3)].eqn)->append ((yyvsp[(3) - (3)].eqn));
    (yyval.app)->args = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 581 "parse_netlist.y"
    { (yyval.eqn) = NULL; }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 582 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(1) - (1)].eqn);
  }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 585 "parse_netlist.y"
    {
    (yyval.eqn) = (yyvsp[(1) - (1)].app);
  }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 588 "parse_netlist.y"
    {
    (yyvsp[(1) - (3)].eqn)->setNext ((yyvsp[(3) - (3)].eqn));
    (yyval.eqn) = (yyvsp[(1) - (3)].eqn);
  }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 592 "parse_netlist.y"
    {
    (yyvsp[(1) - (3)].app)->setNext ((yyvsp[(3) - (3)].eqn));
    (yyval.eqn) = (yyvsp[(1) - (3)].app);
  }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 599 "parse_netlist.y"
    { /* a full subcircuit definition found */
    (yyvsp[(1) - (3)].subcircuit)->sub = (yyvsp[(2) - (3)].definition);
    (yyval.subcircuit) = (yyvsp[(1) - (3)].subcircuit);
    (yyvsp[(2) - (3)].definition) = NULL;
  }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 607 "parse_netlist.y"
    {
    /* create subcircuit definition right here */
    (yyval.subcircuit) = (struct definition_t *) calloc (sizeof (struct definition_t), 1);
    (yyval.subcircuit)->type = strdup ("Def");
    (yyval.subcircuit)->instance = (yyvsp[(2) - (5)].ident);
    (yyval.subcircuit)->nodes = (yyvsp[(3) - (5)].node);
    (yyval.subcircuit)->pairs = (yyvsp[(4) - (5)].pair);
    (yyval.subcircuit)->action = PROP_ACTION;
    (yyval.subcircuit)->line = netlist_lineno;
  }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 619 "parse_netlist.y"
    { (yyval.definition) = NULL; }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 620 "parse_netlist.y"
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

  case 92:

/* Line 1806 of yacc.c  */
#line 632 "parse_netlist.y"
    { /* nothing to do */ }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 636 "parse_netlist.y"
    { /* chain definitions here */
    (yyvsp[(1) - (1)].definition)->next = (yyval.definition);
    (yyval.definition) = (yyvsp[(1) - (1)].definition);
  }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 640 "parse_netlist.y"
    { /* chain definitions here */
    (yyvsp[(1) - (1)].definition)->next = (yyval.definition);
    (yyval.definition) = (yyvsp[(1) - (1)].definition);
  }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 644 "parse_netlist.y"
    { /* do nothing here, see subcircuit rule */ }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 645 "parse_netlist.y"
    {
    (yyval.definition) = NULL;
  }
    break;



/* Line 1806 of yacc.c  */
#line 2630 "parse_netlist.cpp"
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
#line 651 "parse_netlist.y"


int netlist_error (const char * error) {
  logprint (LOG_ERROR, "line %d: %s\n", netlist_lineno, error);
  return 0;
}

