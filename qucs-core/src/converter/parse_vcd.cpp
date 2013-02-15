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
#define yyparse         vcd_parse
#define yylex           vcd_lex
#define yyerror         vcd_error
#define yylval          vcd_lval
#define yychar          vcd_char
#define yydebug         vcd_debug
#define yynerrs         vcd_nerrs


/* Copy the first part of user declarations.  */

/* Line 268 of yacc.c  */
#line 3 "parse_vcd.y"

/*
 * parse_vcd.y - parser for a VCD data file
 *
 * Copyright (C) 2005 Raimund Jacob <raimi@lkcc.org>
 * Copyright (C) 2006, 2008 Stefan Jahn <stefan@lkcc.org>
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
#define YYMAXDEPTH 10000000

#include "check_vcd.h"



/* Line 268 of yacc.c  */
#line 123 "parse_vcd.cpp"

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

/* Line 293 of yacc.c  */
#line 79 "parse_vcd.y"

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



/* Line 293 of yacc.c  */
#line 295 "parse_vcd.cpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 343 of yacc.c  */
#line 307 "parse_vcd.cpp"

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
#define YYFINAL  48
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   89

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  68
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  22
/* YYNRULES -- Number of rules.  */
#define YYNRULES  71
/* YYNRULES -- Number of states.  */
#define YYNSTATES  111

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   315

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      64,    65,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    63,     2,
       2,     2,     2,     2,     2,     2,    66,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,    67,     2,     2,     2,     2,     2,     2,     2,
       2,    61,     2,    62,     2,     2,     2,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint8 yyprhs[] =
{
       0,     0,     3,     6,     7,    10,    13,    16,    19,    23,
      27,    30,    33,    37,    40,    42,    44,    46,    48,    50,
      53,    55,    57,    59,    61,    63,    65,    67,    69,    71,
      77,    78,    82,    88,    92,    94,    96,    98,   100,   102,
     104,   106,   108,   110,   112,   114,   116,   118,   120,   122,
     124,   126,   128,   129,   132,   136,   140,   144,   148,   150,
     153,   156,   157,   160,   162,   164,   167,   169,   171,   173,
     175,   179
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      69,     0,    -1,    70,    81,    -1,    -1,    71,    70,    -1,
       4,     3,    -1,     5,     3,    -1,     6,     3,    -1,     7,
      72,     3,    -1,     8,    74,     3,    -1,     9,     3,    -1,
      11,     3,    -1,    10,    77,     3,    -1,    73,    57,    -1,
      16,    -1,    17,    -1,    18,    -1,    20,    -1,    19,    -1,
      75,    76,    -1,    21,    -1,    28,    -1,    29,    -1,    34,
      -1,    35,    -1,    31,    -1,    32,    -1,    30,    -1,    33,
      -1,    79,    80,    58,    59,    78,    -1,    -1,    61,    55,
      62,    -1,    61,    55,    63,    55,    62,    -1,    64,    55,
      65,    -1,    36,    -1,    37,    -1,    38,    -1,    39,    -1,
      40,    -1,    41,    -1,    42,    -1,    43,    -1,    44,    -1,
      45,    -1,    46,    -1,    47,    -1,    48,    -1,    49,    -1,
      50,    -1,    51,    -1,    52,    -1,    55,    -1,    -1,    82,
      81,    -1,    12,    85,     3,    -1,    13,    85,     3,    -1,
      14,    85,     3,    -1,    15,    85,     3,    -1,    83,    -1,
      84,    85,    -1,    25,    56,    -1,    -1,    86,    85,    -1,
      87,    -1,    89,    -1,    88,    58,    -1,    24,    -1,    21,
      -1,    26,    -1,    23,    -1,    66,    54,    58,    -1,    67,
      53,    58,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   108,   108,   111,   112,   116,   117,   118,   119,   135,
     136,   145,   146,   162,   170,   171,   172,   173,   174,   178,
     185,   186,   187,   191,   192,   193,   194,   195,   196,   200,
     210,   211,   216,   221,   229,   231,   233,   236,   238,   240,
     242,   244,   246,   248,   250,   252,   254,   256,   258,   260,
     262,   267,   270,   271,   275,   276,   277,   278,   281,   288,
     296,   301,   302,   308,   309,   313,   321,   322,   323,   324,
     328,   333
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "t_END", "t_COMMENT", "t_DATE",
  "t_ENDDEFINITIONS", "t_SCOPE", "t_TIMESCALE", "t_UPSCOPE", "t_VAR",
  "t_VERSION", "t_DUMPALL", "t_DUMPOFF", "t_DUMPON", "t_DUMPVARS",
  "s_MODULE", "s_TASK", "s_FUNCTION", "s_FORK", "s_BEGIN", "ONE", "B", "Z",
  "ZERO", "HASHMARK", "X", "R", "TEN", "HUNDRET", "PICO", "MICRO", "NANO",
  "FEMTO", "SECOND", "MILLI", "EVENT", "INTEGER", "PARAMETER", "REAL",
  "REG", "SUPPLY0", "SUPPLY1", "TIME", "TRI", "TRIAND", "TRIOR", "TRIREG",
  "TRI0", "TRI1", "WAND", "WIRE", "WOR", "Real", "Binary",
  "PositiveInteger", "PositiveHugeInteger", "Identifier", "IdentifierCode",
  "Reference", "InvalidCharacter", "'['", "']'", "':'", "'('", "')'",
  "'B'", "'R'", "$accept", "ValueChangeDumpDefinitions", "DeclarationList",
  "Declaration", "ScopeDeclaration", "ScopeType", "TimeScaleDeclaration",
  "TimeScale", "TimeUnit", "VarDeclaration", "BitSelect", "VarType",
  "Size", "SimulationCommandList", "SimulationCommand", "ValueChangeset",
  "SimulationTime", "ValueChangeList", "ValueChange", "ScalarValueChange",
  "Value", "VectorValueChange", 0
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
     315,    91,    93,    58,    40,    41,    66,    82
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    68,    69,    70,    70,    71,    71,    71,    71,    71,
      71,    71,    71,    72,    73,    73,    73,    73,    73,    74,
      75,    75,    75,    76,    76,    76,    76,    76,    76,    77,
      78,    78,    78,    78,    79,    79,    79,    79,    79,    79,
      79,    79,    79,    79,    79,    79,    79,    79,    79,    79,
      79,    80,    81,    81,    82,    82,    82,    82,    82,    83,
      84,    85,    85,    86,    86,    87,    88,    88,    88,    88,
      89,    89
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     2,     2,     2,     2,     3,     3,
       2,     2,     3,     2,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     5,
       0,     3,     5,     3,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     0,     2,     3,     3,     3,     3,     1,     2,
       2,     0,     2,     1,     1,     2,     1,     1,     1,     1,
       3,     3
};

/* YYDEFACT[STATE-NAME] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      52,     3,     5,     6,     7,    14,    15,    16,    18,    17,
       0,     0,    20,    21,    22,     0,     0,    10,    34,    35,
      36,    37,    38,    39,    40,    41,    42,    43,    44,    45,
      46,    47,    48,    49,    50,     0,     0,    11,     1,    61,
      61,    61,    61,     0,     2,    52,    58,    61,     4,     8,
      13,     9,    27,    25,    26,    28,    23,    24,    19,    12,
      51,     0,    67,    69,    66,    68,     0,     0,     0,    61,
      63,     0,    64,     0,     0,     0,    60,    53,    59,     0,
       0,     0,    54,    62,    65,    55,    56,    57,    30,    70,
      71,     0,     0,    29,     0,     0,    31,     0,    33,     0,
      32
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     9,    10,    11,    20,    21,    25,    26,    68,    45,
     103,    46,    71,    54,    55,    56,    57,    78,    79,    80,
      81,    82
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -54
static const yytype_int8 yypact[] =
{
      32,    -2,    56,    57,    37,   -17,    59,   -22,    60,    64,
      19,    32,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
      62,     9,   -54,   -54,   -54,    65,    17,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,    66,    12,   -54,   -54,   -21,
     -21,   -21,   -21,    14,   -54,    19,   -54,   -21,   -54,   -54,
     -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,    13,   -54,   -54,   -54,   -54,    18,    20,    71,   -21,
     -54,    21,   -54,    72,    73,    74,   -54,   -54,   -54,    22,
      24,    25,   -54,   -54,   -54,   -54,   -54,   -54,    -3,   -54,
     -54,    23,    29,   -54,   -53,    15,   -54,    30,   -54,    26,
     -54
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -54,   -54,    75,   -54,   -54,   -54,   -54,   -54,   -54,   -54,
     -54,   -54,   -54,    34,   -54,   -54,   -54,   -44,   -54,   -54,
     -54,   -54
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      72,    12,    73,    74,    22,    75,    83,    84,    85,   106,
     107,    23,    24,    88,    28,    29,    30,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    49,    50,    51,    52,    93,     1,     2,     3,     4,
       5,     6,     7,     8,    53,    76,    77,    62,    63,    64,
      65,    66,    67,    15,    16,    17,    18,    19,   101,    13,
      14,   102,    27,    47,    48,    59,    60,    70,    61,    69,
      86,    89,    90,    91,    92,    95,    96,    97,   104,    94,
     108,    98,    99,   100,   105,   109,    58,     0,   110,    87
};

#define yypact_value_is_default(yystate) \
  ((yystate) == (-54))

#define yytable_value_is_error(yytable_value) \
  YYID (0)

static const yytype_int8 yycheck[] =
{
      21,     3,    23,    24,    21,    26,    50,    51,    52,    62,
      63,    28,    29,    57,    36,    37,    38,    39,    40,    41,
      42,    43,    44,    45,    46,    47,    48,    49,    50,    51,
      52,    12,    13,    14,    15,    79,     4,     5,     6,     7,
       8,     9,    10,    11,    25,    66,    67,    30,    31,    32,
      33,    34,    35,    16,    17,    18,    19,    20,    61,     3,
       3,    64,     3,     3,     0,     3,    57,    55,     3,     3,
      56,    58,    54,    53,     3,     3,     3,     3,    55,    58,
      65,    59,    58,    58,    55,    55,    11,    -1,    62,    55
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     4,     5,     6,     7,     8,     9,    10,    11,    69,
      70,    71,     3,     3,     3,    16,    17,    18,    19,    20,
      72,    73,    21,    28,    29,    74,    75,     3,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    77,    79,     3,     0,    12,
      13,    14,    15,    25,    81,    82,    83,    84,    70,     3,
      57,     3,    30,    31,    32,    33,    34,    35,    76,     3,
      55,    80,    21,    23,    24,    26,    66,    67,    85,    86,
      87,    88,    89,    85,    85,    85,    56,    81,    85,    58,
      54,    53,     3,    85,    58,     3,     3,     3,    59,    58,
      58,    61,    64,    78,    55,    55,    62,    63,    65,    55,
      62
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
        case 8:

/* Line 1806 of yacc.c  */
#line 119 "parse_vcd.y"
    {
     if (!vcd->scopes) {
       /* no scope defined yet */
       vcd->scopes = (struct vcd_scope *)
	 calloc (1, sizeof (struct vcd_scope));
       vcd->scopes->ident = strdup (VCD_NOSCOPE);
       vcd->scopes->scopes = (yyvsp[(2) - (3)].scope);
       (yyvsp[(2) - (3)].scope)->parent = vcd->scopes;
     } else {
       /* concatenate scope definitions */
       (yyvsp[(2) - (3)].scope)->next = vcd->currentscope->scopes;
       vcd->currentscope->scopes = (yyvsp[(2) - (3)].scope);
       (yyvsp[(2) - (3)].scope)->parent = vcd->currentscope;
     }
     vcd->currentscope = (yyvsp[(2) - (3)].scope);
   }
    break;

  case 10:

/* Line 1806 of yacc.c  */
#line 136 "parse_vcd.y"
    {
     if (vcd->currentscope->parent) {
       /* up one scope */
       vcd->currentscope = vcd->currentscope->parent;
     } else {
       fprintf (stderr, "vcd notice, unnecessary $upscope in line %d\n",
		vcd_lineno);
     }
   }
    break;

  case 12:

/* Line 1806 of yacc.c  */
#line 146 "parse_vcd.y"
    {
     if (!vcd->scopes) {
       /* no scope defined yet */
       vcd->scopes = (struct vcd_scope *)
	 calloc (1, sizeof (struct vcd_scope));
       vcd->scopes->ident = strdup (VCD_NOSCOPE);
       vcd->currentscope = vcd->scopes;
     }
     /* concatenate variable definitions */
     (yyvsp[(2) - (3)].vardef)->scope = vcd->currentscope;
     (yyvsp[(2) - (3)].vardef)->next = vcd->currentscope->vardefs;
     vcd->currentscope->vardefs = (yyvsp[(2) - (3)].vardef);
   }
    break;

  case 13:

/* Line 1806 of yacc.c  */
#line 162 "parse_vcd.y"
    {
     (yyval.scope) = (struct vcd_scope *) calloc (1, sizeof (struct vcd_scope));
     (yyval.scope)->type = (yyvsp[(1) - (2)].stype);
     (yyval.scope)->ident = (yyvsp[(2) - (2)].ident);
   }
    break;

  case 14:

/* Line 1806 of yacc.c  */
#line 170 "parse_vcd.y"
    { (yyval.stype) = SCOPE_MODULE;   }
    break;

  case 15:

/* Line 1806 of yacc.c  */
#line 171 "parse_vcd.y"
    { (yyval.stype) = SCOPE_TASK;     }
    break;

  case 16:

/* Line 1806 of yacc.c  */
#line 172 "parse_vcd.y"
    { (yyval.stype) = SCOPE_FUNCTION; }
    break;

  case 17:

/* Line 1806 of yacc.c  */
#line 173 "parse_vcd.y"
    { (yyval.stype) = SCOPE_BEGIN;    }
    break;

  case 18:

/* Line 1806 of yacc.c  */
#line 174 "parse_vcd.y"
    { (yyval.stype) = SCOPE_FORK;     }
    break;

  case 19:

/* Line 1806 of yacc.c  */
#line 178 "parse_vcd.y"
    {
     vcd->t = (yyvsp[(1) - (2)].integer);
     vcd->scale = (yyvsp[(2) - (2)].real);
   }
    break;

  case 20:

/* Line 1806 of yacc.c  */
#line 185 "parse_vcd.y"
    { (yyval.integer) = 1;   }
    break;

  case 21:

/* Line 1806 of yacc.c  */
#line 186 "parse_vcd.y"
    { (yyval.integer) = 10;  }
    break;

  case 22:

/* Line 1806 of yacc.c  */
#line 187 "parse_vcd.y"
    { (yyval.integer) = 100; }
    break;

  case 23:

/* Line 1806 of yacc.c  */
#line 191 "parse_vcd.y"
    { (yyval.real) = 1;     }
    break;

  case 24:

/* Line 1806 of yacc.c  */
#line 192 "parse_vcd.y"
    { (yyval.real) = 1e-3;  }
    break;

  case 25:

/* Line 1806 of yacc.c  */
#line 193 "parse_vcd.y"
    { (yyval.real) = 1e-6;  }
    break;

  case 26:

/* Line 1806 of yacc.c  */
#line 194 "parse_vcd.y"
    { (yyval.real) = 1e-9;  }
    break;

  case 27:

/* Line 1806 of yacc.c  */
#line 195 "parse_vcd.y"
    { (yyval.real) = 1e-12; }
    break;

  case 28:

/* Line 1806 of yacc.c  */
#line 196 "parse_vcd.y"
    { (yyval.real) = 1e-15; }
    break;

  case 29:

/* Line 1806 of yacc.c  */
#line 200 "parse_vcd.y"
    {
     (yyval.vardef) = (struct vcd_vardef *) calloc (1, sizeof (struct vcd_vardef));
     (yyval.vardef)->type = (yyvsp[(1) - (5)].vtype);
     (yyval.vardef)->size = (yyvsp[(2) - (5)].integer);
     (yyval.vardef)->code = (yyvsp[(3) - (5)].ident);
     (yyval.vardef)->ident = (yyvsp[(4) - (5)].ident);
     (yyval.vardef)->range = (yyvsp[(5) - (5)].range);
   }
    break;

  case 30:

/* Line 1806 of yacc.c  */
#line 210 "parse_vcd.y"
    { (yyval.range) = NULL; }
    break;

  case 31:

/* Line 1806 of yacc.c  */
#line 211 "parse_vcd.y"
    {
      (yyval.range) = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      (yyval.range)->l = -1;
      (yyval.range)->h = (yyvsp[(2) - (3)].integer);
    }
    break;

  case 32:

/* Line 1806 of yacc.c  */
#line 216 "parse_vcd.y"
    {
      (yyval.range) = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      (yyval.range)->l = (yyvsp[(2) - (5)].integer);
      (yyval.range)->h = (yyvsp[(4) - (5)].integer);
    }
    break;

  case 33:

/* Line 1806 of yacc.c  */
#line 221 "parse_vcd.y"
    {
      (yyval.range) = (struct vcd_range *) calloc (1, sizeof (struct vcd_range));
      (yyval.range)->l = (yyvsp[(2) - (3)].integer);
      (yyval.range)->h = -1;
    }
    break;

  case 34:

/* Line 1806 of yacc.c  */
#line 229 "parse_vcd.y"
    { (yyval.vtype) = VAR_EVENT;
     /* a special type to synchronize different statement blocks */ }
    break;

  case 35:

/* Line 1806 of yacc.c  */
#line 231 "parse_vcd.y"
    { (yyval.vtype) = VAR_INTEGER;
     /* signed 32-bit variable */ }
    break;

  case 36:

/* Line 1806 of yacc.c  */
#line 233 "parse_vcd.y"
    { (yyval.vtype) = VAR_PARAMETER;
     /* a named constant - the default value of a parameter can be
	overwritten, when declaring an instance of the associated module */ }
    break;

  case 37:

/* Line 1806 of yacc.c  */
#line 236 "parse_vcd.y"
    { (yyval.vtype) = VAR_REAL;
     /* double-precision floating point */ }
    break;

  case 38:

/* Line 1806 of yacc.c  */
#line 238 "parse_vcd.y"
    { (yyval.vtype) = VAR_REG;
     /* unsigned variable of any bit size */ }
    break;

  case 39:

/* Line 1806 of yacc.c  */
#line 240 "parse_vcd.y"
    { (yyval.vtype) = VAR_SUPPLY0;
     /* constant logic 0 (supply strength) */ }
    break;

  case 40:

/* Line 1806 of yacc.c  */
#line 242 "parse_vcd.y"
    { (yyval.vtype) = VAR_SUPPLY1;
     /* constant logic 1 (supply strength) */ }
    break;

  case 41:

/* Line 1806 of yacc.c  */
#line 244 "parse_vcd.y"
    { (yyval.vtype) = VAR_TIME;
     /* unsigned 64-bit variable */ }
    break;

  case 42:

/* Line 1806 of yacc.c  */
#line 246 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRI;
     /* simple interconnecting wire */ }
    break;

  case 43:

/* Line 1806 of yacc.c  */
#line 248 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRIAND;
     /* wired outputs AND together */ }
    break;

  case 44:

/* Line 1806 of yacc.c  */
#line 250 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRIOR;
     /* wired outputs OR together */ }
    break;

  case 45:

/* Line 1806 of yacc.c  */
#line 252 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRIREG;
     /* stores last value when tri-stated (capacitance strength) */ }
    break;

  case 46:

/* Line 1806 of yacc.c  */
#line 254 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRI0;
     /* pulls down when tri-stated */ }
    break;

  case 47:

/* Line 1806 of yacc.c  */
#line 256 "parse_vcd.y"
    { (yyval.vtype) = VAR_TRI1;
     /* pulls up when tri-stated */ }
    break;

  case 48:

/* Line 1806 of yacc.c  */
#line 258 "parse_vcd.y"
    { (yyval.vtype) = VAR_WAND;
     /* wired outputs AND together */ }
    break;

  case 49:

/* Line 1806 of yacc.c  */
#line 260 "parse_vcd.y"
    { (yyval.vtype) = VAR_WIRE;
     /* simple interconnecting wire */ }
    break;

  case 50:

/* Line 1806 of yacc.c  */
#line 262 "parse_vcd.y"
    { (yyval.vtype) = VAR_WOR;
     /* wired outputs OR together */ }
    break;

  case 57:

/* Line 1806 of yacc.c  */
#line 278 "parse_vcd.y"
    {
      vcd->changesets->changes = (yyvsp[(2) - (3)].change);
  }
    break;

  case 58:

/* Line 1806 of yacc.c  */
#line 281 "parse_vcd.y"
    {
      (yyvsp[(1) - (1)].changeset)->next = vcd->changesets;
      vcd->changesets = (yyvsp[(1) - (1)].changeset);
  }
    break;

  case 59:

/* Line 1806 of yacc.c  */
#line 288 "parse_vcd.y"
    {
      (yyval.changeset) = (struct vcd_changeset *) calloc (1, sizeof (struct vcd_changeset));
      (yyval.changeset)->t = (yyvsp[(1) - (2)].real);
      (yyval.changeset)->changes = (yyvsp[(2) - (2)].change);
    }
    break;

  case 60:

/* Line 1806 of yacc.c  */
#line 296 "parse_vcd.y"
    {
      (yyval.real) = (yyvsp[(2) - (2)].real);
    }
    break;

  case 61:

/* Line 1806 of yacc.c  */
#line 301 "parse_vcd.y"
    { (yyval.change) = NULL; }
    break;

  case 62:

/* Line 1806 of yacc.c  */
#line 302 "parse_vcd.y"
    {
      (yyvsp[(1) - (2)].change)->next = (yyvsp[(2) - (2)].change);
    }
    break;

  case 65:

/* Line 1806 of yacc.c  */
#line 313 "parse_vcd.y"
    {
      (yyval.change) = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      (yyval.change)->value = (yyvsp[(1) - (2)].value);
      (yyval.change)->code = (yyvsp[(2) - (2)].ident);
    }
    break;

  case 70:

/* Line 1806 of yacc.c  */
#line 328 "parse_vcd.y"
    {
      (yyval.change) = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      (yyval.change)->value = (yyvsp[(2) - (3)].value);
      (yyval.change)->code = (yyvsp[(3) - (3)].ident);
    }
    break;

  case 71:

/* Line 1806 of yacc.c  */
#line 333 "parse_vcd.y"
    {
      (yyval.change) = (struct vcd_change *) calloc (1, sizeof (struct vcd_change));
      (yyval.change)->value = (yyvsp[(2) - (3)].value);
      (yyval.change)->code = (yyvsp[(3) - (3)].ident);
      (yyval.change)->isreal = 1;
    }
    break;



/* Line 1806 of yacc.c  */
#line 2096 "parse_vcd.cpp"
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
#line 342 "parse_vcd.y"


int vcd_error (const char * error) {
  fprintf (stderr, "line %d: %s\n", vcd_lineno, error);
  return 0;
}

