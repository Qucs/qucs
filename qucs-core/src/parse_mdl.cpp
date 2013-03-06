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
#define yyparse         mdl_parse
#define yylex           mdl_lex
#define yyerror         mdl_error
#define yylval          mdl_lval
#define yychar          mdl_char
#define yydebug         mdl_debug
#define yynerrs         mdl_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 3 "parse_mdl.y"

/*
 * parse_mdl.y - parser for an IC-CAP MDL data file
 *
 * Copyright (C) 2006 Stefan Jahn <stefan@lkcc.org>
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

#define YYERROR_VERBOSE 42
#define YYDEBUG 1
#define YYMAXDEPTH 1000000

#define __NOEXTENSIONS__ 1

#include "object.h"
#include "complex.h"
#include "vector.h"
#include "check_mdl.h"



/* Line 268 of yacc.c  */
#line 127 "parse_mdl.cpp"

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
     LINK = 258,
     Identifier = 259,
     String = 260,
     InvalidCharacter = 261,
     Real = 262,
     t_LINK = 263,
     t_VIEW = 264,
     t_TABLE = 265,
     t_PSTABLE = 266,
     t_CNTABLE = 267,
     t_OPTIMEDIT = 268,
     t_BLKEDIT = 269,
     t_HYPTABLE = 270,
     t_ELEMENT = 271,
     t_DATA = 272,
     t_DATASET = 273,
     t_DATASIZE = 274,
     t_POINT = 275,
     t_MEMBER = 276,
     t_LIST = 277,
     t_PLOTOPTIMIZEROPT = 278,
     t_PLOTOPTIMIZERTRACESET = 279,
     t_PLOTOPTIMIZERTRACEREGSET = 280,
     t_PLOTOPTIMIZERTRACENATREGSET = 281,
     t_PLOTERROR = 282,
     t_TYPE = 283,
     t_CIRCUITDECK = 284,
     t_PARAM = 285,
     t_RANGE = 286,
     t_CONNPAIR = 287,
     t_CALDATA = 288,
     t_CALSET = 289,
     t_TERM = 290,
     t_APPLIC = 291,
     t_SUBAPP = 292,
     t_EDITSIZE = 293,
     t_PLOTSIZE = 294,
     t_OPTRANGE = 295
   };
#endif
/* Tokens.  */
#define LINK 258
#define Identifier 259
#define String 260
#define InvalidCharacter 261
#define Real 262
#define t_LINK 263
#define t_VIEW 264
#define t_TABLE 265
#define t_PSTABLE 266
#define t_CNTABLE 267
#define t_OPTIMEDIT 268
#define t_BLKEDIT 269
#define t_HYPTABLE 270
#define t_ELEMENT 271
#define t_DATA 272
#define t_DATASET 273
#define t_DATASIZE 274
#define t_POINT 275
#define t_MEMBER 276
#define t_LIST 277
#define t_PLOTOPTIMIZEROPT 278
#define t_PLOTOPTIMIZERTRACESET 279
#define t_PLOTOPTIMIZERTRACEREGSET 280
#define t_PLOTOPTIMIZERTRACENATREGSET 281
#define t_PLOTERROR 282
#define t_TYPE 283
#define t_CIRCUITDECK 284
#define t_PARAM 285
#define t_RANGE 286
#define t_CONNPAIR 287
#define t_CALDATA 288
#define t_CALSET 289
#define t_TERM 290
#define t_APPLIC 291
#define t_SUBAPP 292
#define t_EDITSIZE 293
#define t_PLOTSIZE 294
#define t_OPTRANGE 295




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 293 of yacc.c  */
#line 91 "parse_mdl.y"

  char * ident;
  double f;
  struct mdl_point_t * point;
  struct mdl_dataset_t * dset;
  struct mdl_datasize_t * dsize;
  struct mdl_link_t * link;
  struct mdl_data_t * data;
  struct mdl_dcontent_t * dcontent;
  struct mdl_lcontent_t * lcontent;
  struct mdl_element_t * element;
  struct mdl_hyptable_t * hyptable;
  struct mdl_table_t * table;



/* Line 293 of yacc.c  */
#line 260 "parse_mdl.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 272 "parse_mdl.cpp"

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
#define YYFINAL  6
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   180

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  43
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  56
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNRULES -- Number of states.  */
#define YYNSTATES  218

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   295

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
       2,     2,     2,    41,     2,    42,     2,     2,     2,     2,
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
      35,    36,    37,    38,    39,    40
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    16,
      19,    21,    23,    24,    27,    29,    30,    33,    35,    37,
      38,    41,    43,    45,    47,    49,    51,    53,    55,    57,
      58,    61,    63,    65,    67,    69,    71,    73,    75,    77,
      79,    81,    83,    85,    87,    89,    91,    93,    94,    97,
      99,   101,   103,   104,   107,   111,   117,   119,   120,   123,
     129,   134,   138,   143,   149,   156,   160,   167,   172,   178,
     182,   187,   193,   198,   204,   212,   217,   218,   221,   224,
     227,   233,   237,   243,   248,   254,   257,   264,   268,   272,
     277,   281,   289,   297,   301,   305,   309,   313,   317,   322,
     327
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      44,     0,    -1,    45,    -1,    -1,    66,    45,    -1,    64,
      -1,    63,    -1,    65,    -1,    -1,    46,    47,    -1,    64,
      -1,    63,    -1,    -1,    48,    49,    -1,    98,    -1,    -1,
      50,    51,    -1,    64,    -1,    65,    -1,    -1,    52,    53,
      -1,    64,    -1,    79,    -1,    80,    -1,    86,    -1,    94,
      -1,    65,    -1,    66,    -1,    67,    -1,    -1,    54,    55,
      -1,    87,    -1,    88,    -1,    89,    -1,    90,    -1,    91,
      -1,    92,    -1,    93,    -1,    65,    -1,    68,    -1,    71,
      -1,    72,    -1,    73,    -1,    81,    -1,    82,    -1,    70,
      -1,    69,    -1,    -1,    56,    57,    -1,    95,    -1,    96,
      -1,    97,    -1,    -1,    58,    59,    -1,    83,    84,    62,
      -1,    83,    84,    62,    84,    62,    -1,    85,    -1,    -1,
      61,    62,    -1,    16,     7,     5,     5,     5,    -1,    16,
       7,     5,     5,    -1,    16,     5,     5,    -1,     9,     4,
       7,     5,    -1,    10,     5,    41,    47,    42,    -1,    10,
       5,     7,    41,    47,    42,    -1,    10,    41,    42,    -1,
       3,     8,     5,    41,    55,    42,    -1,    17,    41,    57,
      42,    -1,    11,     5,    41,    59,    42,    -1,    29,    41,
      42,    -1,    14,     5,    41,    42,    -1,    12,     5,    41,
      51,    42,    -1,    13,    41,    53,    42,    -1,    34,     5,
      41,    74,    42,    -1,     4,     7,     4,     7,     4,     7,
      75,    -1,    33,    41,    76,    42,    -1,    -1,    77,    76,
      -1,    78,    62,    -1,    35,     7,    -1,    36,     5,     7,
       7,     7,    -1,    37,     5,     7,    -1,    15,     5,    41,
      49,    42,    -1,    18,    41,    60,    42,    -1,    19,     4,
       7,     7,     7,    -1,    28,     4,    -1,    20,     7,     7,
       7,     7,     7,    -1,    22,     8,     5,    -1,    23,     4,
       7,    -1,    24,     5,     5,     5,    -1,    24,     5,     5,
      -1,    25,     5,     7,     5,     5,     5,     5,    -1,    26,
       5,     7,     5,     5,     5,     5,    -1,    27,     4,     7,
      -1,    38,     7,     7,    -1,    39,     7,     7,    -1,    21,
       8,     5,    -1,    30,     4,     5,    -1,    31,     4,     5,
       5,    -1,    40,     4,     5,     5,    -1,    32,     5,     5,
      -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   123,   123,   128,   129,   136,   139,   142,   147,   148,
     159,   162,   167,   168,   179,   182,   183,   188,   189,   192,
     193,   198,   201,   204,   207,   210,   213,   218,   223,   230,
     231,   242,   245,   248,   251,   254,   257,   260,   263,   266,
     269,   272,   275,   278,   283,   288,   291,   296,   297,   308,
     309,   310,   313,   314,   319,   325,   336,   339,   340,   347,
     354,   360,   368,   375,   380,   385,   391,   400,   407,   413,
     417,   423,   429,   434,   440,   446,   450,   451,   456,   461,
     466,   472,   478,   486,   492,   502,   508,   519,   526,   532,
     537,   544,   554,   564,   570,   575,   580,   587,   594,   602,
     610
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "LINK", "Identifier", "String",
  "InvalidCharacter", "Real", "t_LINK", "t_VIEW", "t_TABLE", "t_PSTABLE",
  "t_CNTABLE", "t_OPTIMEDIT", "t_BLKEDIT", "t_HYPTABLE", "t_ELEMENT",
  "t_DATA", "t_DATASET", "t_DATASIZE", "t_POINT", "t_MEMBER", "t_LIST",
  "t_PLOTOPTIMIZEROPT", "t_PLOTOPTIMIZERTRACESET",
  "t_PLOTOPTIMIZERTRACEREGSET", "t_PLOTOPTIMIZERTRACENATREGSET",
  "t_PLOTERROR", "t_TYPE", "t_CIRCUITDECK", "t_PARAM", "t_RANGE",
  "t_CONNPAIR", "t_CALDATA", "t_CALSET", "t_TERM", "t_APPLIC", "t_SUBAPP",
  "t_EDITSIZE", "t_PLOTSIZE", "t_OPTRANGE", "'{'", "'}'", "$accept",
  "Input", "LinkList", "TableContent", "TableContentList",
  "HypTableContent", "HypTableContentList", "ConnTableContent",
  "ConnTableContentList", "OptEditContent", "OptEditContentList",
  "LinkContent", "LinkContentList", "DataContent", "DataContentList",
  "PSContent", "PSContentList", "DSContent", "Point", "PointList",
  "ELEMENT_Line", "VIEW_Line", "TABLE_Definition", "LINK_Definition",
  "DATA_Definition", "PSTABLE_Definition", "CIRCUITDECK_Definition",
  "BLKEDIT_Definition", "CNTABLE_Definition", "OPTIMEDIT_Definition",
  "CALSET_Definition", "CalSetContent", "CALDATA_Definition",
  "CalDataContentList", "CalDataContent", "TERM_Line", "APPLIC_Line",
  "SUBAPP_Line", "HYPTABLE_Definition", "DATASET_Definition",
  "DATASIZE_Line", "TYPE_Line", "POINT_Line", "LIST_Line",
  "PLOTOPTIMIZEROPT_Line", "PLOTOPTIMIZERTRACESET_Line",
  "PLOTOPTIMIZERTRACEREGSET_Line", "PLOTOPTIMIZERTRACENATREGSET_Line",
  "PLOTERROR_Line", "EDITSIZE_Line", "PLOTSIZE_Line", "MEMBER_Line",
  "PARAM_Line", "RANGE_Line", "OPTRANGE_Line", "CONNPAIR_Line", 0
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
     295,   123,   125
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    43,    44,    45,    45,    46,    46,    46,    47,    47,
      48,    48,    49,    49,    50,    51,    51,    52,    52,    53,
      53,    54,    54,    54,    54,    54,    54,    54,    54,    55,
      55,    56,    56,    56,    56,    56,    56,    56,    56,    56,
      56,    56,    56,    56,    56,    56,    56,    57,    57,    58,
      58,    58,    59,    59,    60,    60,    61,    62,    62,    63,
      63,    63,    64,    65,    65,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    76,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      88,    89,    90,    91,    92,    93,    94,    95,    96,    97,
      98
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     0,     2,
       1,     1,     0,     2,     1,     0,     2,     1,     1,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     0,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     0,     2,     1,
       1,     1,     0,     2,     3,     5,     1,     0,     2,     5,
       4,     3,     4,     5,     6,     3,     6,     4,     5,     3,
       4,     5,     4,     5,     7,     4,     0,     2,     2,     2,
       5,     3,     5,     4,     5,     2,     6,     3,     3,     4,
       3,     7,     7,     3,     3,     3,     3,     3,     4,     4,
       3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     2,     3,     0,     1,     4,     0,    29,
       0,     0,     0,     0,     0,     0,     0,    29,     0,    21,
      26,    27,    28,    22,    23,    24,    25,     0,     0,     0,
      47,     0,     0,     0,     0,    30,    66,     0,     0,     8,
      65,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    47,     0,    38,    39,
      46,    45,    40,    41,    42,    43,    44,    31,    32,    33,
      34,    35,    36,    37,    96,    87,     0,    81,    62,     8,
       0,     8,     0,     6,     5,     7,     0,     0,    19,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    48,    67,     0,     0,     0,     0,     9,    63,    52,
      15,    19,     0,    17,    18,     0,    12,     0,     0,     0,
      88,    90,     0,     0,    93,    69,     0,    94,    95,    80,
      64,    61,     0,     0,     0,     0,    52,     0,    49,    50,
      51,     0,    15,     0,    14,    20,    72,    70,    12,     0,
      11,    10,     0,    83,     0,    57,    89,     0,     0,     0,
       0,    60,     0,     0,     0,    53,    68,     0,    16,    71,
      13,    82,     0,    85,     0,    57,    54,    56,     0,     0,
       0,    73,    59,    97,     0,     0,   100,     0,     0,    58,
      57,     0,     0,     0,    98,    99,    84,     0,    55,    91,
      92,     0,     0,     0,     0,     0,    86,     0,    74,    76,
       0,     0,    76,    57,    79,    75,    77,    78
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     2,     3,    81,    82,   148,   149,   142,   143,   111,
     112,    17,    18,    56,    57,   136,   137,   118,   175,   176,
      83,    84,    85,     4,    22,    59,    60,    61,    62,    63,
      64,   160,   208,   211,   212,   213,    23,    24,    65,    66,
     119,   155,   177,    25,    67,    68,    69,    70,    71,    72,
      73,    26,   138,   139,   140,   144
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -174
static const yytype_int16 yypact[] =
{
      11,     7,    20,  -174,    11,    19,  -174,  -174,   -18,     9,
      25,    -2,    -5,    34,    36,    42,    50,     9,    14,  -174,
    -174,  -174,  -174,  -174,  -174,  -174,  -174,    51,    -3,    17,
      39,    55,    56,    60,    62,  -174,  -174,    65,    30,    18,
    -174,    67,    69,    35,    70,    76,    41,    79,    80,    81,
      82,    84,    43,    85,    86,    87,    39,    47,  -174,  -174,
    -174,  -174,  -174,  -174,  -174,  -174,  -174,  -174,  -174,  -174,
    -174,  -174,  -174,  -174,  -174,  -174,    88,  -174,  -174,    18,
      28,    18,    49,  -174,  -174,  -174,    57,    58,     1,    59,
      63,    73,    89,    92,    94,    98,    99,    66,    68,   103,
     104,  -174,  -174,   105,    71,   109,   110,  -174,  -174,   -24,
      90,     1,    74,  -174,  -174,    75,    16,   114,    77,    93,
    -174,   115,   118,   119,  -174,  -174,   121,  -174,  -174,  -174,
    -174,  -174,   122,   124,   125,   126,   -24,    91,  -174,  -174,
    -174,   127,    90,    95,  -174,  -174,  -174,  -174,    16,    96,
    -174,  -174,   128,  -174,   130,   106,  -174,   131,   135,   134,
     100,   138,   139,   140,   141,  -174,  -174,   142,  -174,  -174,
    -174,  -174,   143,  -174,   144,   106,    93,  -174,   147,   148,
     145,  -174,  -174,  -174,   149,   150,  -174,   151,   152,  -174,
     106,   155,   156,   157,  -174,  -174,  -174,   158,  -174,  -174,
    -174,   153,   159,   160,   161,   123,  -174,   107,  -174,   136,
     162,   120,   136,   106,  -174,  -174,  -174,  -174
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -174,  -174,   166,  -174,   -38,  -174,   -17,  -174,    21,  -174,
      61,  -174,   163,  -174,   117,  -174,    38,  -174,  -174,  -173,
    -111,    -9,    -8,     4,  -174,  -174,  -174,  -174,  -174,  -174,
    -174,  -174,  -174,   -37,  -174,  -174,  -174,  -174,  -174,  -174,
    -174,     0,  -174,  -174,  -174,  -174,  -174,  -174,  -174,  -174,
    -174,  -174,  -174,  -174,  -174,  -174
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      19,    20,   189,    28,    38,   150,   133,   134,    19,    20,
      10,    11,     1,    21,     1,     5,   135,   198,    10,    11,
       6,    21,    58,     9,     8,    10,    12,    10,    11,    27,
      13,    14,    80,   105,    80,   106,    30,   150,    39,    29,
     217,   104,    31,   107,    32,    15,    16,    33,    58,    11,
      41,    42,    43,    44,    45,    34,    36,    46,    37,    40,
      74,    75,    47,    48,    49,    50,    51,    76,    52,    77,
      78,    79,    86,    53,    87,    89,    88,    54,    55,   113,
     114,    90,    91,    92,    97,    93,    94,    95,    96,   102,
      98,   108,   117,    99,   100,   103,   120,   121,   109,   110,
     115,   122,   113,   114,   116,   123,   124,   151,   125,   126,
     127,   128,   129,   130,   131,   132,   146,   147,   152,   153,
     156,   154,   141,   157,   158,   159,   174,   161,   162,   163,
     164,   170,   167,   166,   173,   172,   178,   169,   171,   151,
     179,   180,   181,   182,   183,   184,   185,   186,   209,   193,
     187,   188,   191,   192,   194,   195,   207,   203,   196,   197,
     199,   200,   215,   168,   201,   202,   204,   205,   206,   214,
       7,   210,   145,   101,   165,   216,   190,     0,     0,     0,
      35
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-174))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int16 yycheck[] =
{
       9,     9,   175,     5,     7,   116,    30,    31,    17,    17,
       9,    10,     3,     9,     3,     8,    40,   190,     9,    10,
       0,    17,    30,    41,     5,     9,    17,     9,    10,     4,
      21,    22,    16,     5,    16,     7,    41,   148,    41,    41,
     213,    79,     8,    81,     8,    36,    37,     5,    56,    10,
      11,    12,    13,    14,    15,     5,    42,    18,     7,    42,
       5,     5,    23,    24,    25,    26,    27,     7,    29,     7,
       5,    41,     5,    34,     5,     5,    41,    38,    39,    88,
      88,     5,    41,     4,    41,     5,     5,     5,     4,    42,
       5,    42,    19,     7,     7,     7,     7,     5,    41,    41,
      41,     7,   111,   111,    41,     7,     7,   116,    42,    41,
       7,     7,     7,    42,     5,     5,    42,    42,     4,    42,
       5,    28,    32,     5,     5,     4,    20,     5,     4,     4,
       4,   148,     5,    42,     4,     7,     5,    42,    42,   148,
       5,     7,    42,     5,     5,     5,     5,     5,    41,     4,
       7,     7,     5,     5,     5,     5,    33,     4,     7,     7,
       5,     5,    42,   142,     7,     7,     7,     7,     7,     7,
       4,    35,   111,    56,   136,   212,   176,    -1,    -1,    -1,
      17
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,    44,    45,    66,     8,     0,    45,     5,    41,
       9,    10,    17,    21,    22,    36,    37,    54,    55,    64,
      65,    66,    67,    79,    80,    86,    94,     4,     5,    41,
      41,     8,     8,     5,     5,    55,    42,     7,     7,    41,
      42,    11,    12,    13,    14,    15,    18,    23,    24,    25,
      26,    27,    29,    34,    38,    39,    56,    57,    65,    68,
      69,    70,    71,    72,    73,    81,    82,    87,    88,    89,
      90,    91,    92,    93,     5,     5,     7,     7,     5,    41,
      16,    46,    47,    63,    64,    65,     5,     5,    41,     5,
       5,    41,     4,     5,     5,     5,     4,    41,     5,     7,
       7,    57,    42,     7,    47,     5,     7,    47,    42,    41,
      41,    52,    53,    64,    65,    41,    41,    19,    60,    83,
       7,     5,     7,     7,     7,    42,    41,     7,     7,     7,
      42,     5,     5,    30,    31,    40,    58,    59,    95,    96,
      97,    32,    50,    51,    98,    53,    42,    42,    48,    49,
      63,    64,     4,    42,    28,    84,     5,     5,     5,     4,
      74,     5,     4,     4,     4,    59,    42,     5,    51,    42,
      49,    42,     7,     4,    20,    61,    62,    85,     5,     5,
       7,    42,     5,     5,     5,     5,     5,     7,     7,    62,
      84,     5,     5,     4,     5,     5,     7,     7,    62,     5,
       5,     7,     7,     4,     7,     7,     7,    33,    75,    41,
      35,    76,    77,    78,     7,    42,    76,    62
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
#line 123 "parse_mdl.y"
    {
    mdl_root = (yyvsp[(1) - (1)].link);
  }
    break;

  case 3:

/* Line 1806 of yacc.c  */
#line 128 "parse_mdl.y"
    { (yyval.link) = NULL; }
    break;

  case 4:

/* Line 1806 of yacc.c  */
#line 129 "parse_mdl.y"
    {
    (yyvsp[(1) - (2)].link)->next = (yyvsp[(2) - (2)].link);
    (yyval.link) = (yyvsp[(1) - (2)].link);
  }
    break;

  case 5:

/* Line 1806 of yacc.c  */
#line 136 "parse_mdl.y"
    {
    (yyval.element) = NULL;
  }
    break;

  case 6:

/* Line 1806 of yacc.c  */
#line 139 "parse_mdl.y"
    {
    (yyval.element) = (yyvsp[(1) - (1)].element);
  }
    break;

  case 7:

/* Line 1806 of yacc.c  */
#line 142 "parse_mdl.y"
    {
    (yyval.element) = NULL;
  }
    break;

  case 8:

/* Line 1806 of yacc.c  */
#line 147 "parse_mdl.y"
    { (yyval.element) = NULL; }
    break;

  case 9:

/* Line 1806 of yacc.c  */
#line 148 "parse_mdl.y"
    {
    if ((yyvsp[(1) - (2)].element)) {
      (yyvsp[(1) - (2)].element)->next = (yyvsp[(2) - (2)].element);
      (yyval.element) = (yyvsp[(1) - (2)].element);
    } else {
      (yyval.element) = (yyvsp[(2) - (2)].element);
    }
  }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 159 "parse_mdl.y"
    {
    (yyval.element) = NULL;
  }
    break;

  case 11:

/* Line 1806 of yacc.c  */
#line 162 "parse_mdl.y"
    {
    (yyval.element) = (yyvsp[(1) - (1)].element);
  }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 167 "parse_mdl.y"
    { (yyval.element) = NULL; }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 168 "parse_mdl.y"
    {
    if ((yyvsp[(1) - (2)].element)) {
      (yyvsp[(1) - (2)].element)->next = (yyvsp[(2) - (2)].element);
      (yyval.element) = (yyvsp[(1) - (2)].element);
    } else {
      (yyval.element) = (yyvsp[(2) - (2)].element);
    }
  }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 182 "parse_mdl.y"
    { }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 183 "parse_mdl.y"
    {
  }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 192 "parse_mdl.y"
    { }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 193 "parse_mdl.y"
    {
  }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 198 "parse_mdl.y"
    {
    (yyval.lcontent) = NULL;
  }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 201 "parse_mdl.y"
    {
    (yyval.lcontent) = NULL;
  }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 204 "parse_mdl.y"
    {
    (yyval.lcontent) = NULL;
  }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 207 "parse_mdl.y"
    {
    (yyval.lcontent) = NULL;
  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 210 "parse_mdl.y"
    {
    (yyval.lcontent) = NULL;
  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 213 "parse_mdl.y"
    {
    (yyval.lcontent) = (struct mdl_lcontent_t *) calloc (sizeof (struct mdl_lcontent_t), 1);
    (yyval.lcontent)->type = t_TABLE;
    (yyval.lcontent)->table = (yyvsp[(1) - (1)].table);
  }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 218 "parse_mdl.y"
    {
    (yyval.lcontent) = (struct mdl_lcontent_t *) calloc (sizeof (struct mdl_lcontent_t), 1);
    (yyval.lcontent)->type = t_LINK;
    (yyval.lcontent)->link = (yyvsp[(1) - (1)].link);
  }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 223 "parse_mdl.y"
    {
    (yyval.lcontent) = (struct mdl_lcontent_t *) calloc (sizeof (struct mdl_lcontent_t), 1);
    (yyval.lcontent)->type = t_DATA;
    (yyval.lcontent)->data = (yyvsp[(1) - (1)].data);
  }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 230 "parse_mdl.y"
    { (yyval.lcontent) = NULL; }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 231 "parse_mdl.y"
    {
    if ((yyvsp[(1) - (2)].lcontent)) {
      (yyvsp[(1) - (2)].lcontent)->next = (yyvsp[(2) - (2)].lcontent);
      (yyval.lcontent) = (yyvsp[(1) - (2)].lcontent);
    } else {
      (yyval.lcontent) = (yyvsp[(2) - (2)].lcontent);
    }
  }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 242 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 245 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 248 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 251 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 254 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 257 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 260 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 263 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 266 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 269 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 272 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 275 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 278 "parse_mdl.y"
    {
    (yyval.dcontent) = (struct mdl_dcontent_t *) calloc (sizeof (struct mdl_dcontent_t), 1);
    (yyval.dcontent)->type = t_HYPTABLE;
    (yyval.dcontent)->hyptable = (yyvsp[(1) - (1)].hyptable);
  }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 283 "parse_mdl.y"
    {
    (yyval.dcontent) = (struct mdl_dcontent_t *) calloc (sizeof (struct mdl_dcontent_t), 1);
    (yyval.dcontent)->type = t_DATASET;
    (yyval.dcontent)->data = (yyvsp[(1) - (1)].dset);
  }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 288 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 291 "parse_mdl.y"
    {
    (yyval.dcontent) = NULL;
  }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 296 "parse_mdl.y"
    { (yyval.dcontent) = NULL; }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 297 "parse_mdl.y"
    {
    if ((yyvsp[(1) - (2)].dcontent)) {
      (yyvsp[(1) - (2)].dcontent)->next = (yyvsp[(2) - (2)].dcontent);
      (yyval.dcontent) = (yyvsp[(1) - (2)].dcontent);
    } else {
      (yyval.dcontent) = (yyvsp[(2) - (2)].dcontent);
    }
  }
    break;

  case 52:

/* Line 1806 of yacc.c  */
#line 313 "parse_mdl.y"
    { }
    break;

  case 53:

/* Line 1806 of yacc.c  */
#line 314 "parse_mdl.y"
    {
  }
    break;

  case 54:

/* Line 1806 of yacc.c  */
#line 319 "parse_mdl.y"
    {
    (yyval.dset) = (struct mdl_dataset_t *) calloc (sizeof (struct mdl_dataset_t), 1);
    (yyval.dset)->dsize = (yyvsp[(1) - (3)].dsize);
    (yyval.dset)->type1 = (yyvsp[(2) - (3)].ident);
    (yyval.dset)->data1 = (yyvsp[(3) - (3)].point);
  }
    break;

  case 55:

/* Line 1806 of yacc.c  */
#line 325 "parse_mdl.y"
    {
    (yyval.dset) = (struct mdl_dataset_t *) calloc (sizeof (struct mdl_dataset_t), 1);
    (yyval.dset)->dsize = (yyvsp[(1) - (5)].dsize);
    (yyval.dset)->type1 = (yyvsp[(2) - (5)].ident);
    (yyval.dset)->data1 = (yyvsp[(3) - (5)].point);
    (yyval.dset)->type2 = (yyvsp[(4) - (5)].ident);
    (yyval.dset)->data2 = (yyvsp[(5) - (5)].point);
  }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 339 "parse_mdl.y"
    { (yyval.point) = NULL; }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 340 "parse_mdl.y"
    {
    (yyvsp[(1) - (2)].point)->next = (yyvsp[(2) - (2)].point);
    (yyval.point) = (yyvsp[(1) - (2)].point);
  }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 347 "parse_mdl.y"
    {
    (yyval.element) = (struct mdl_element_t *) calloc (sizeof (struct mdl_element_t), 1);
    (yyval.element)->number = (int) (yyvsp[(2) - (5)].f);
    (yyval.element)->name = (yyvsp[(3) - (5)].ident);
    (yyval.element)->value = (yyvsp[(4) - (5)].ident);
    (yyval.element)->attr = (yyvsp[(5) - (5)].ident);
  }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 354 "parse_mdl.y"
    {
    (yyval.element) = (struct mdl_element_t *) calloc (sizeof (struct mdl_element_t), 1);
    (yyval.element)->number = (int) (yyvsp[(2) - (4)].f);
    (yyval.element)->name = (yyvsp[(3) - (4)].ident);
    (yyval.element)->value = (yyvsp[(4) - (4)].ident);
  }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 360 "parse_mdl.y"
    {
    (yyval.element) = (struct mdl_element_t *) calloc (sizeof (struct mdl_element_t), 1);
    (yyval.element)->name = (yyvsp[(2) - (3)].ident);
    (yyval.element)->value = (yyvsp[(3) - (3)].ident);
  }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 368 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (4)].ident));
    free ((yyvsp[(4) - (4)].ident));
  }
    break;

  case 63:

/* Line 1806 of yacc.c  */
#line 375 "parse_mdl.y"
    {
    (yyval.table) = (struct mdl_table_t *) calloc (sizeof (struct mdl_table_t), 1);
    (yyval.table)->name = (yyvsp[(2) - (5)].ident);
    (yyval.table)->data = (yyvsp[(4) - (5)].element);
  }
    break;

  case 64:

/* Line 1806 of yacc.c  */
#line 380 "parse_mdl.y"
    {
    (yyval.table) = (struct mdl_table_t *) calloc (sizeof (struct mdl_table_t), 1);
    (yyval.table)->name = (yyvsp[(2) - (6)].ident);
    (yyval.table)->data = (yyvsp[(5) - (6)].element);
  }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 385 "parse_mdl.y"
    {
    (yyval.table) = (struct mdl_table_t *) calloc (sizeof (struct mdl_table_t), 1);
  }
    break;

  case 66:

/* Line 1806 of yacc.c  */
#line 391 "parse_mdl.y"
    {
    (yyval.link) = (struct mdl_link_t *) calloc (sizeof (struct mdl_link_t), 1);
    (yyval.link)->name = (yyvsp[(3) - (6)].ident);
    (yyval.link)->type = (yyvsp[(2) - (6)].ident);
    (yyval.link)->content = (yyvsp[(5) - (6)].lcontent);
  }
    break;

  case 67:

/* Line 1806 of yacc.c  */
#line 400 "parse_mdl.y"
    {
    (yyval.data) = (struct mdl_data_t *) calloc (sizeof (struct mdl_data_t), 1);
    (yyval.data)->content = (yyvsp[(3) - (4)].dcontent);
  }
    break;

  case 68:

/* Line 1806 of yacc.c  */
#line 407 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (5)].ident));
  }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 417 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (4)].ident));
  }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 423 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (5)].ident));
  }
    break;

  case 72:

/* Line 1806 of yacc.c  */
#line 429 "parse_mdl.y"
    {
  }
    break;

  case 73:

/* Line 1806 of yacc.c  */
#line 434 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (5)].ident));
  }
    break;

  case 74:

/* Line 1806 of yacc.c  */
#line 441 "parse_mdl.y"
    {
  }
    break;

  case 75:

/* Line 1806 of yacc.c  */
#line 446 "parse_mdl.y"
    {
  }
    break;

  case 76:

/* Line 1806 of yacc.c  */
#line 450 "parse_mdl.y"
    { }
    break;

  case 77:

/* Line 1806 of yacc.c  */
#line 451 "parse_mdl.y"
    {
  }
    break;

  case 78:

/* Line 1806 of yacc.c  */
#line 456 "parse_mdl.y"
    {
  }
    break;

  case 79:

/* Line 1806 of yacc.c  */
#line 461 "parse_mdl.y"
    {
  }
    break;

  case 80:

/* Line 1806 of yacc.c  */
#line 466 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (5)].ident));
  }
    break;

  case 81:

/* Line 1806 of yacc.c  */
#line 472 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
  }
    break;

  case 82:

/* Line 1806 of yacc.c  */
#line 478 "parse_mdl.y"
    {
    (yyval.hyptable) = (struct mdl_hyptable_t *) calloc (sizeof (struct mdl_hyptable_t), 1);
    (yyval.hyptable)->name = (yyvsp[(2) - (5)].ident);
    (yyval.hyptable)->data = (yyvsp[(4) - (5)].element);
  }
    break;

  case 83:

/* Line 1806 of yacc.c  */
#line 486 "parse_mdl.y"
    {
    (yyval.dset) = (yyvsp[(3) - (4)].dset);
  }
    break;

  case 84:

/* Line 1806 of yacc.c  */
#line 492 "parse_mdl.y"
    {
    (yyval.dsize) = (struct mdl_datasize_t *) calloc (sizeof (struct mdl_datasize_t), 1);
    (yyval.dsize)->type = (yyvsp[(2) - (5)].ident);
    (yyval.dsize)->size = (int) (yyvsp[(3) - (5)].f);
    (yyval.dsize)->x = (int) (yyvsp[(4) - (5)].f);
    (yyval.dsize)->y = (int) (yyvsp[(5) - (5)].f);
  }
    break;

  case 85:

/* Line 1806 of yacc.c  */
#line 502 "parse_mdl.y"
    {
    (yyval.ident) = (yyvsp[(2) - (2)].ident);
  }
    break;

  case 86:

/* Line 1806 of yacc.c  */
#line 508 "parse_mdl.y"
    {
    (yyval.point) = (struct mdl_point_t *) calloc (sizeof (struct mdl_point_t), 1);
    (yyval.point)->n = (int) (yyvsp[(2) - (6)].f);
    (yyval.point)->x = (int) (yyvsp[(3) - (6)].f);
    (yyval.point)->y = (int) (yyvsp[(4) - (6)].f);
    (yyval.point)->r = (yyvsp[(5) - (6)].f);
    (yyval.point)->i = (yyvsp[(6) - (6)].f);
  }
    break;

  case 87:

/* Line 1806 of yacc.c  */
#line 519 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
    free ((yyvsp[(3) - (3)].ident));
  }
    break;

  case 88:

/* Line 1806 of yacc.c  */
#line 526 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
  }
    break;

  case 89:

/* Line 1806 of yacc.c  */
#line 532 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (4)].ident));
    free ((yyvsp[(3) - (4)].ident));
    free ((yyvsp[(4) - (4)].ident));
  }
    break;

  case 90:

/* Line 1806 of yacc.c  */
#line 537 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
    free ((yyvsp[(3) - (3)].ident));
  }
    break;

  case 91:

/* Line 1806 of yacc.c  */
#line 544 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (7)].ident));
    free ((yyvsp[(4) - (7)].ident));
    free ((yyvsp[(5) - (7)].ident));
    free ((yyvsp[(6) - (7)].ident));
    free ((yyvsp[(7) - (7)].ident));
  }
    break;

  case 92:

/* Line 1806 of yacc.c  */
#line 554 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (7)].ident));
    free ((yyvsp[(4) - (7)].ident));
    free ((yyvsp[(5) - (7)].ident));
    free ((yyvsp[(6) - (7)].ident));
    free ((yyvsp[(7) - (7)].ident));
  }
    break;

  case 93:

/* Line 1806 of yacc.c  */
#line 564 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
  }
    break;

  case 94:

/* Line 1806 of yacc.c  */
#line 570 "parse_mdl.y"
    {
  }
    break;

  case 95:

/* Line 1806 of yacc.c  */
#line 575 "parse_mdl.y"
    {
  }
    break;

  case 96:

/* Line 1806 of yacc.c  */
#line 580 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
    free ((yyvsp[(3) - (3)].ident));
  }
    break;

  case 97:

/* Line 1806 of yacc.c  */
#line 587 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
    free ((yyvsp[(3) - (3)].ident));
  }
    break;

  case 98:

/* Line 1806 of yacc.c  */
#line 594 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (4)].ident));
    free ((yyvsp[(3) - (4)].ident));
    free ((yyvsp[(4) - (4)].ident));
  }
    break;

  case 99:

/* Line 1806 of yacc.c  */
#line 602 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (4)].ident));
    free ((yyvsp[(3) - (4)].ident));
    free ((yyvsp[(4) - (4)].ident));
  }
    break;

  case 100:

/* Line 1806 of yacc.c  */
#line 610 "parse_mdl.y"
    {
    free ((yyvsp[(2) - (3)].ident));
    free ((yyvsp[(3) - (3)].ident));
  }
    break;



/* Line 1806 of yacc.c  */
#line 2570 "parse_mdl.cpp"
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
#line 616 "parse_mdl.y"


int mdl_error (const char * error) {
  fprintf (stderr, "line %d: %s\n", mdl_lineno, error);
  return 0;
}

