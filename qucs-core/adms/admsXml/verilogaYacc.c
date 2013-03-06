
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
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
#define YYBISON_VERSION "2.4.1"

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
#define yyparse         verilogaparse
#define yylex           verilogalex
#define yyerror         verilogaerror
#define yylval          verilogalval
#define yychar          verilogachar
#define yydebug         verilogadebug
#define yynerrs         veriloganerrs


/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 3 "verilogaYacc.y"

#define YYDEBUG 1
#define YYPARSE_PARAM myadmsmain
#include "admsVeriloga.h"

#define NEWVARIABLE(l) p_variableprototype myvariableprototype=adms_variableprototype_new(gModule,l,(p_adms)gBlockList->data);

inline static void   Y (p_yaccval myyaccval,p_adms myusrdata) {myyaccval->_usrdata=myusrdata;}
inline static p_adms YY(p_yaccval myyaccval)                  {return myyaccval->_usrdata;}
static char* gNatureAccess=NULL;
static p_number gNatureAbsTol=NULL;
static char* gNatureUnits=NULL;
static char* gNatureidt=NULL;
static char* gNatureddt=NULL;
static p_discipline gDiscipline=NULL;
static p_module gModule=NULL;
static p_analogfunction gAnalogfunction=NULL;
static p_module gInstanceModule=NULL;
static p_node gGND=NULL;
static p_source gSource=NULL;
static p_lexval gLexval=NULL;
static p_contribution gContribution=NULL;
static admse gVariableType=admse_real;
static admse gNodeDirection;
int uid=0;
static p_slist gVariableDeclarationList=NULL;
static p_slist gInstanceVariableList=NULL;
static p_slist gTerminalList=NULL;
static p_slist gBranchAliasList=NULL;
static p_slist gRangeList=NULL;
static p_slist gNodeList=NULL;
static p_slist gAttributeList=NULL;
static p_slist gGlobalAttributeList=NULL;
static p_slist gBlockList=NULL;
static p_slist gBlockVariableList=NULL;
static p_branchalias gBranchAlias=NULL;

static void adms_veriloga_message_fatal_continue(const p_lexval mytoken)
{
  adms_message_fatal_continue(("[%s:%i:%i]: at '%s':\n",mytoken->_f,mytoken->_l,mytoken->_c,mytoken->_string))
}
static void adms_veriloga_message_fatal (const char* message,const p_lexval mytoken)
{
  adms_veriloga_message_fatal_continue(mytoken);
  adms_message_fatal((message))
}
/*
inline static p_variableprototype variableprototype_recursive_lookup_by_id (p_adms myadms,p_lexval mylexval)
{
  if(myadms==(p_adms)gModule)
    return adms_module_list_variable_lookup_by_id(gModule,gModule,mylexval,(p_adms)gModule);
  else if(myadms==(p_adms)gAnalogfunction)
    return adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,mylexval,(p_adms)gAnalogfunction);
  else
  {
    p_slist l;
    for(l=((p_block)myadms)->_variable;l;l=l->next)
      if(!strcmp(((p_variableprototype)l->data)->_lexval->_string,mylexval->_string))
        return (p_variableprototype)l->data;
    return variableprototype_recursive_lookup_by_id((p_adms)((p_block)myadms)->_block,mylexval);
  }
}
*/
inline static p_variable variable_recursive_lookup_by_id (p_adms myadms,p_lexval mylexval)
{
  if(myadms==(p_adms)gModule)
  {
    p_variable myvariable=NULL;
    p_variableprototype myvariableprototype;
    if((myvariableprototype=adms_module_list_variable_lookup_by_id(gModule,gModule,mylexval,(p_adms)gModule)))
    {
      myvariable=adms_variable_new(myvariableprototype);
      adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
    }
    return myvariable;
  }
  else if(myadms==(p_adms)gAnalogfunction)
  {
    p_variable myvariable=NULL;
    p_variableprototype myvariableprototype;
    if((myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,mylexval,(p_adms)gAnalogfunction)))
    {
      myvariable=adms_variable_new(myvariableprototype);
      adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
    }
    return myvariable;
  }
  else
  {
    p_slist l;
    for(l=((p_block)myadms)->_variable;l;l=l->next)
      if(!strcmp(((p_variableprototype)l->data)->_lexval->_string,mylexval->_string))
      {
        p_variableprototype myvariableprototype=(p_variableprototype)l->data;
        p_variable myvariable=adms_variable_new(myvariableprototype);
        adms_slist_push(&myvariableprototype->_instance,(p_adms)myvariable);
        return myvariable;
      }
    return variable_recursive_lookup_by_id((p_adms)((p_block)myadms)->_block,mylexval);
  }
}
static p_nature lookup_nature(const char *myname)
{
  p_slist l;
  for(l=root()->_nature;l;l=l->next)
    if(!strcmp(((p_nature)l->data)->_name,myname))
      return (p_nature)l->data;
  return NULL;
}



/* Line 189 of yacc.c  */
#line 194 "y.tab.c"

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

/* Line 214 of yacc.c  */
#line 119 "verilogaYacc.y"

  p_lexval _lexval;
  p_yaccval _yaccval;



/* Line 214 of yacc.c  */
#line 288 "y.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 300 "y.tab.c"

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
# if YYENABLE_NLS
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
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
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
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
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

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  20
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   636

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  78
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  111
/* YYNRULES -- Number of rules.  */
#define YYNRULES  249
/* YYNRULES -- Number of states.  */
#define YYNSTATES  504

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    73,     2,    66,     2,    77,    72,     2,
      54,    55,    75,    63,    56,    62,    67,    76,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    60,    52,
      65,    53,    74,    68,    64,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    59,     2,    61,    70,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    57,    71,    58,    69,     2,     2,     2,
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
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    12,    14,    16,    21,
      23,    25,    28,    32,    36,    40,    42,    47,    49,    52,
      57,    63,    68,    73,    74,    76,    80,    83,    86,    88,
      91,    95,    96,   104,   105,   107,   109,   112,   114,   116,
     119,   122,   126,   128,   131,   137,   138,   140,   142,   146,
     148,   150,   153,   155,   158,   160,   162,   164,   169,   173,
     177,   179,   181,   183,   187,   191,   195,   197,   199,   201,
     203,   207,   209,   213,   216,   219,   223,   225,   229,   231,
     238,   243,   248,   253,   259,   265,   267,   269,   272,   276,
     280,   284,   288,   292,   294,   298,   300,   304,   306,   310,
     312,   316,   318,   322,   325,   328,   331,   333,   335,   339,
     341,   345,   352,   354,   356,   359,   362,   367,   374,   376,
     383,   384,   386,   388,   391,   394,   397,   403,   409,   415,
     421,   423,   425,   428,   430,   432,   435,   438,   440,   442,
     444,   448,   451,   453,   456,   458,   460,   462,   464,   470,
     475,   478,   480,   482,   485,   493,   496,   501,   503,   507,
     509,   512,   517,   521,   527,   530,   532,   535,   539,   543,
     547,   549,   553,   555,   562,   566,   571,   578,   583,   589,
     599,   606,   608,   611,   615,   619,   622,   633,   635,   637,
     641,   647,   651,   656,   663,   671,   677,   685,   687,   689,
     693,   695,   697,   699,   705,   707,   711,   716,   718,   722,
     724,   728,   730,   734,   736,   740,   742,   746,   748,   753,
     758,   760,   764,   769,   773,   778,   780,   784,   788,   790,
     794,   798,   800,   804,   808,   812,   814,   817,   820,   823,
     826,   828,   831,   833,   835,   837,   839,   844,   849,   854
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
      79,     0,    -1,    80,    -1,    81,    -1,    80,    81,    -1,
      94,    -1,    82,    -1,    87,    -1,    18,    83,    84,    33,
      -1,    35,    -1,    85,    -1,    84,    85,    -1,    26,    86,
      52,    -1,    51,    86,    52,    -1,    34,    35,    52,    -1,
      35,    -1,     8,    35,    88,    49,    -1,    89,    -1,    88,
      89,    -1,    35,    53,     7,    52,    -1,    35,    53,     7,
      35,    52,    -1,    35,    53,    11,    52,    -1,    35,    53,
      35,    52,    -1,    -1,    91,    -1,    48,    92,    40,    -1,
      48,    20,    -1,    48,    40,    -1,    93,    -1,    92,    93,
      -1,    35,    53,    11,    -1,    -1,    90,    39,    35,    95,
      99,    96,    31,    -1,    -1,   103,    -1,    97,    -1,   103,
      97,    -1,   143,    -1,    98,    -1,    98,   143,    -1,   143,
      98,    -1,    98,   143,    98,    -1,   165,    -1,    98,   165,
      -1,    54,   100,    55,    90,    52,    -1,    -1,   101,    -1,
     102,    -1,   101,    56,   102,    -1,    35,    -1,   104,    -1,
     103,   104,    -1,   106,    -1,   105,   106,    -1,    91,    -1,
     107,    -1,   113,    -1,    17,   127,   129,   128,    -1,    17,
     129,   128,    -1,   127,   130,   128,    -1,   131,    -1,   117,
      -1,    52,    -1,   108,   109,    52,    -1,    30,   110,    52,
      -1,    35,   110,    52,    -1,    47,    -1,    10,    -1,    42,
      -1,   111,    -1,   109,    56,   111,    -1,   112,    -1,   110,
      56,   112,    -1,    35,    90,    -1,    35,    90,    -1,     6,
     116,    52,    -1,   115,    -1,   114,    56,   115,    -1,    35,
      -1,    54,    35,    56,    35,    55,   114,    -1,    54,    35,
      55,   114,    -1,   118,   120,   147,    50,    -1,    16,    46,
     119,    52,    -1,    16,    46,    38,   119,    52,    -1,    16,
      46,    23,   119,    52,    -1,    35,    -1,   121,    -1,   120,
     121,    -1,    47,   122,    52,    -1,    10,   123,    52,    -1,
      42,   124,    52,    -1,    38,   125,    52,    -1,    23,   126,
      52,    -1,    35,    -1,   122,    56,    35,    -1,    35,    -1,
     123,    56,    35,    -1,    35,    -1,   124,    56,    35,    -1,
      35,    -1,   125,    56,    35,    -1,    35,    -1,   126,    56,
      35,    -1,    38,    90,    -1,    23,    90,    -1,    37,    90,
      -1,    52,    -1,   133,    -1,   129,    56,   133,    -1,   134,
      -1,   130,    56,   134,    -1,   132,    35,    53,    35,    90,
      52,    -1,     9,    -1,    14,    -1,   135,    90,    -1,   136,
      90,    -1,   136,    53,   171,   137,    -1,   136,    53,    57,
     145,    58,   137,    -1,    35,    -1,    35,    59,     7,    60,
       7,    61,    -1,    -1,   138,    -1,   139,    -1,   138,   139,
      -1,     5,   140,    -1,    29,   140,    -1,    54,   141,    60,
     142,    55,    -1,    54,   141,    60,   142,    61,    -1,    59,
     141,    60,   142,    55,    -1,    59,   141,    60,   142,    61,
      -1,   171,    -1,   171,    -1,    62,    28,    -1,   171,    -1,
      28,    -1,    63,    28,    -1,    16,   144,    -1,   146,    -1,
     147,    -1,   171,    -1,   145,    56,   171,    -1,    90,   154,
      -1,   157,    -1,   169,    52,    -1,   170,    -1,   160,    -1,
     162,    -1,   161,    -1,    12,    54,   145,    55,    52,    -1,
      12,    54,    55,    52,    -1,    12,    52,    -1,    52,    -1,
     151,    -1,   148,   151,    -1,    64,    54,    35,    54,   149,
      55,    55,    -1,    64,    35,    -1,    64,    54,    35,    55,
      -1,   150,    -1,   149,    56,   150,    -1,    11,    -1,   152,
      41,    -1,   152,    60,    35,    41,    -1,   152,   153,    41,
      -1,   152,    60,    35,   153,    41,    -1,    90,    32,    -1,
     144,    -1,   153,   144,    -1,    38,   155,    52,    -1,    23,
     155,    52,    -1,    37,   155,    52,    -1,   156,    -1,   155,
      56,   156,    -1,    35,    -1,    35,    59,     7,    60,     7,
      61,    -1,   158,    90,    52,    -1,   159,    65,    63,   171,
      -1,    35,    54,    35,    56,    35,    55,    -1,    35,    54,
      35,    55,    -1,    22,    54,   171,    55,   144,    -1,    21,
      54,   169,    52,   171,    52,   169,    55,   144,    -1,    25,
      54,   171,    55,   163,    27,    -1,   164,    -1,   163,   164,
      -1,   172,    60,   144,    -1,    45,    60,   144,    -1,    45,
     144,    -1,   166,    66,    54,   167,    55,    35,    54,   110,
      55,    52,    -1,    35,    -1,   168,    -1,   167,    56,   168,
      -1,    67,    35,    54,   171,    55,    -1,    35,    53,   171,
      -1,    91,    35,    53,   171,    -1,    35,    59,   174,    61,
      53,   171,    -1,    91,    35,    59,   174,    61,    53,   171,
      -1,    15,    54,   171,    55,   144,    -1,    15,    54,   171,
      55,   144,     4,   144,    -1,   174,    -1,   173,    -1,   172,
      56,   173,    -1,   174,    -1,   175,    -1,   176,    -1,   176,
      68,   176,    60,   176,    -1,   177,    -1,   176,    44,   177,
      -1,   176,    69,    70,   177,    -1,   178,    -1,   177,    70,
     178,    -1,   179,    -1,   178,    71,   179,    -1,   180,    -1,
     179,    72,   180,    -1,   181,    -1,   180,    13,   181,    -1,
     182,    -1,   181,    43,   182,    -1,   183,    -1,   182,    53,
      53,   183,    -1,   182,    73,    53,   183,    -1,   184,    -1,
     183,    65,   184,    -1,   183,    65,    53,   184,    -1,   183,
      74,   184,    -1,   183,    74,    53,   184,    -1,   185,    -1,
     184,    24,   185,    -1,   184,    36,   185,    -1,   186,    -1,
     185,    63,   186,    -1,   185,    62,   186,    -1,   187,    -1,
     186,    75,   187,    -1,   186,    76,   187,    -1,   186,    77,
     187,    -1,   188,    -1,    63,   188,    -1,    62,   188,    -1,
      73,   188,    -1,    69,   188,    -1,     7,    -1,     7,    35,
      -1,    19,    -1,    11,    -1,    35,    -1,    12,    -1,    35,
      59,   174,    61,    -1,    12,    54,   172,    55,    -1,    35,
      54,   172,    55,    -1,    54,   174,    55,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   285,   285,   290,   293,   298,   301,   304,   309,   316,
     323,   326,   331,   335,   339,   351,   362,   387,   390,   395,
     406,   440,   452,   479,   481,   486,   489,   497,   502,   505,
     510,   522,   521,   544,   546,   549,   552,   557,   560,   563,
     566,   569,   574,   577,   582,   592,   594,   599,   602,   607,
     617,   620,   625,   628,   634,   641,   644,   647,   650,   653,
     656,   659,   662,   667,   674,   681,   693,   697,   701,   707,
     710,   715,   718,   723,   741,   755,   760,   763,   768,   775,
     800,   823,   831,   837,   845,   853,   864,   867,   872,   875,
     878,   881,   884,   889,   896,   905,   912,   921,   929,   939,
     951,   965,   977,   991,   997,  1003,  1011,  1020,  1023,  1028,
    1031,  1036,  1052,  1055,  1060,  1071,  1082,  1089,  1101,  1110,
    1125,  1127,  1132,  1135,  1140,  1148,  1158,  1166,  1174,  1182,
    1190,  1200,  1204,  1217,  1221,  1232,  1245,  1251,  1255,  1261,
    1268,  1277,  1287,  1291,  1295,  1299,  1303,  1307,  1311,  1322,
    1330,  1338,  1345,  1349,  1357,  1361,  1374,  1389,  1392,  1397,
    1402,  1408,  1415,  1421,  1430,  1444,  1448,  1454,  1467,  1480,
    1495,  1498,  1503,  1509,  1520,  1530,  1542,  1559,  1584,  1593,
    1602,  1612,  1619,  1628,  1636,  1643,  1652,  1675,  1684,  1687,
    1692,  1712,  1727,  1748,  1765,  1790,  1798,  1809,  1818,  1825,
    1834,  1840,  1846,  1850,  1861,  1865,  1873,  1883,  1887,  1897,
    1901,  1911,  1915,  1925,  1929,  1939,  1943,  1953,  1957,  1965,
    1975,  1979,  1987,  1995,  2003,  2013,  2017,  2025,  2035,  2039,
    2047,  2057,  2061,  2069,  2077,  2087,  2091,  2098,  2105,  2112,
    2121,  2126,  2155,  2159,  2165,  2192,  2198,  2206,  2215,  2267
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PREC_IF_THEN", "tk_else", "tk_from",
  "tk_branch", "tk_number", "tk_nature", "tk_aliasparameter", "tk_output",
  "tk_anystring", "tk_dollar_ident", "tk_or", "tk_aliasparam", "tk_if",
  "tk_analog", "tk_parameter", "tk_discipline", "tk_char", "tk_anytext",
  "tk_for", "tk_while", "tk_real", "tk_op_shr", "tk_case", "tk_potential",
  "tk_endcase", "tk_inf", "tk_exclude", "tk_ground", "tk_endmodule",
  "tk_begin", "tk_enddiscipline", "tk_domain", "tk_ident", "tk_op_shl",
  "tk_string", "tk_integer", "tk_module", "tk_endattribute", "tk_end",
  "tk_inout", "tk_and", "tk_bitwise_equr", "tk_default", "tk_function",
  "tk_input", "tk_beginattribute", "tk_endnature", "tk_endfunction",
  "tk_flow", "';'", "'='", "'('", "')'", "','", "'{'", "'}'", "'['", "':'",
  "']'", "'-'", "'+'", "'@'", "'<'", "'#'", "'.'", "'?'", "'~'", "'^'",
  "'|'", "'&'", "'!'", "'>'", "'*'", "'/'", "'%'", "$accept",
  "R_admsParse", "R_l.admsParse", "R_s.admsParse", "R_discipline_member",
  "R_discipline_name", "R_l.discipline_assignment",
  "R_s.discipline_assignment", "R_discipline.naturename",
  "R_nature_member", "R_l.nature_assignment", "R_s.nature_assignment",
  "R_d.attribute.0", "R_d.attribute", "R_l.attribute", "R_s.attribute",
  "R_d.module", "$@1", "R_modulebody", "R_netlist", "R_l.instance",
  "R_d.terminal", "R_l.terminal.0", "R_l.terminal", "R_s.terminal",
  "R_l.declaration", "R_s.declaration.withattribute",
  "R_d.attribute.global", "R_s.declaration", "R_d.node", "R_node.type",
  "R_l.terminalnode", "R_l.node", "R_s.terminalnode", "R_s.node",
  "R_d.branch", "R_l.branchalias", "R_s.branchalias", "R_s.branch",
  "R_d.analogfunction", "R_d.analogfunction.proto",
  "R_d.analogfunction.name", "R_l.analogfunction.declaration",
  "R_s.analogfunction.declaration", "R_l.analogfunction.input.variable",
  "R_l.analogfunction.output.variable",
  "R_l.analogfunction.inout.variable",
  "R_l.analogfunction.integer.variable",
  "R_l.analogfunction.real.variable", "R_variable.type",
  "R_d.variable.end", "R_l.parameter", "R_l.variable",
  "R_d.aliasparameter", "R_d.aliasparameter.token", "R_s.parameter",
  "R_s.variable", "R_s.parameter.name", "R_s.variable.name",
  "R_s.parameter.range", "R_l.interval", "R_s.interval", "R_d.interval",
  "R_interval.inf", "R_interval.sup", "R_analog", "R_analogcode",
  "R_l.expression", "R_analogcode.atomic", "R_analogcode.block",
  "R_analogcode.block.atevent", "R_l.analysis", "R_s.analysis",
  "R_d.block", "R_d.block.begin", "R_l.blockitem", "R_d.blockvariable",
  "R_l.blockvariable", "R_s.blockvariable", "R_d.contribution",
  "R_contribution", "R_source", "R_d.while", "R_d.for", "R_d.case",
  "R_l.case.item", "R_s.case.item", "R_s.instance",
  "R_instance.module.name", "R_l.instance.parameter",
  "R_s.instance.parameter", "R_s.assignment", "R_d.conditional",
  "R_s.expression", "R_l.enode", "R_s.function_expression", "R_expression",
  "R_e.conditional", "R_e.bitwise_equ", "R_e.bitwise_xor",
  "R_e.bitwise_or", "R_e.bitwise_and", "R_e.logical_or", "R_e.logical_and",
  "R_e.comp_equ", "R_e.comp", "R_e.bitwise_shift", "R_e.arithm_add",
  "R_e.arithm_mult", "R_e.unary", "R_e.atomic", 0
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
     305,   306,    59,    61,    40,    41,    44,   123,   125,    91,
      58,    93,    45,    43,    64,    60,    35,    46,    63,   126,
      94,   124,    38,    33,    62,    42,    47,    37
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    78,    79,    80,    80,    81,    81,    81,    82,    83,
      84,    84,    85,    85,    85,    86,    87,    88,    88,    89,
      89,    89,    89,    90,    90,    91,    91,    91,    92,    92,
      93,    95,    94,    96,    96,    96,    96,    97,    97,    97,
      97,    97,    98,    98,    99,   100,   100,   101,   101,   102,
     103,   103,   104,   104,   105,   106,   106,   106,   106,   106,
     106,   106,   106,   107,   107,   107,   108,   108,   108,   109,
     109,   110,   110,   111,   112,   113,   114,   114,   115,   116,
     116,   117,   118,   118,   118,   119,   120,   120,   121,   121,
     121,   121,   121,   122,   122,   123,   123,   124,   124,   125,
     125,   126,   126,   127,   127,   127,   128,   129,   129,   130,
     130,   131,   132,   132,   133,   134,   135,   135,   136,   136,
     137,   137,   138,   138,   139,   139,   140,   140,   140,   140,
     140,   141,   141,   142,   142,   142,   143,   144,   144,   145,
     145,   146,   146,   146,   146,   146,   146,   146,   146,   146,
     146,   146,   147,   147,   148,   148,   148,   149,   149,   150,
     151,   151,   151,   151,   152,   153,   153,   154,   154,   154,
     155,   155,   156,   156,   157,   158,   159,   159,   160,   161,
     162,   163,   163,   164,   164,   164,   165,   166,   167,   167,
     168,   169,   169,   169,   169,   170,   170,   171,   172,   172,
     173,   174,   175,   175,   176,   176,   176,   177,   177,   178,
     178,   179,   179,   180,   180,   181,   181,   182,   182,   182,
     183,   183,   183,   183,   183,   184,   184,   184,   185,   185,
     185,   186,   186,   186,   186,   187,   187,   187,   187,   187,
     188,   188,   188,   188,   188,   188,   188,   188,   188,   188
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     4,     1,
       1,     2,     3,     3,     3,     1,     4,     1,     2,     4,
       5,     4,     4,     0,     1,     3,     2,     2,     1,     2,
       3,     0,     7,     0,     1,     1,     2,     1,     1,     2,
       2,     3,     1,     2,     5,     0,     1,     1,     3,     1,
       1,     2,     1,     2,     1,     1,     1,     4,     3,     3,
       1,     1,     1,     3,     3,     3,     1,     1,     1,     1,
       3,     1,     3,     2,     2,     3,     1,     3,     1,     6,
       4,     4,     4,     5,     5,     1,     1,     2,     3,     3,
       3,     3,     3,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     2,     2,     2,     1,     1,     3,     1,
       3,     6,     1,     1,     2,     2,     4,     6,     1,     6,
       0,     1,     1,     2,     2,     2,     5,     5,     5,     5,
       1,     1,     2,     1,     1,     2,     2,     1,     1,     1,
       3,     2,     1,     2,     1,     1,     1,     1,     5,     4,
       2,     1,     1,     2,     7,     2,     4,     1,     3,     1,
       2,     4,     3,     5,     2,     1,     2,     3,     3,     3,
       1,     3,     1,     6,     3,     4,     6,     4,     5,     9,
       6,     1,     2,     3,     3,     2,    10,     1,     1,     3,
       5,     3,     4,     6,     7,     5,     7,     1,     1,     3,
       1,     1,     1,     5,     1,     3,     4,     1,     3,     1,
       3,     1,     3,     1,     3,     1,     3,     1,     4,     4,
       1,     3,     4,     3,     4,     1,     3,     3,     1,     3,
       3,     1,     3,     3,     3,     1,     2,     2,     2,     2,
       1,     2,     1,     1,     1,     1,     4,     4,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
      23,     0,     0,     0,     0,     2,     3,     6,     7,     0,
      24,     5,     0,     9,     0,    26,     0,    27,     0,    28,
       1,     4,     0,     0,     0,    17,     0,     0,     0,     0,
      10,     0,    25,    29,    31,     0,    16,    18,    15,     0,
       0,     0,     8,    11,    30,     0,     0,     0,     0,    12,
      14,    13,    45,    33,     0,    19,    21,    22,    49,     0,
      46,    47,     0,   112,    67,   113,    23,     0,    23,     0,
     187,    23,    23,    68,    66,    62,    54,     0,    35,    38,
      34,    50,     0,    52,    55,     0,    56,    61,     0,     0,
      60,     0,    37,    42,     0,    20,    23,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   151,     0,     0,
      24,   136,   137,   138,    23,   152,    23,   142,    23,     0,
     145,   147,   146,     0,   144,   118,     0,     0,   107,    23,
       0,   104,    23,     0,    71,     0,   105,   103,    32,    23,
     187,    39,    43,    36,    51,     0,     0,    53,    23,     0,
      69,     0,     0,     0,     0,     0,    23,    86,     0,   109,
      23,     0,    40,     0,     0,    48,     0,    75,   150,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    85,     0,
       0,   155,     0,     0,   164,     0,     0,   141,     0,     0,
     153,   160,     0,   165,    23,     0,     0,   143,     0,     0,
     106,     0,    58,   114,     0,    74,    64,     0,    65,    41,
      73,    63,     0,    95,     0,   101,     0,    99,     0,    97,
       0,    93,     0,    87,     0,     0,    59,   115,     0,     0,
      44,     0,     0,   240,   243,   245,   242,   244,     0,     0,
       0,     0,     0,     0,     0,   139,   197,   201,   202,   204,
     207,   209,   211,   213,   215,   217,   220,   225,   228,   231,
     235,     0,     0,     0,     0,     0,     0,   191,     0,     0,
       0,     0,    82,     0,   172,     0,   170,     0,     0,     0,
       0,    23,   162,   166,   174,     0,     0,    57,   108,     0,
     120,    72,    70,    89,     0,    92,     0,    91,     0,    90,
       0,    88,     0,    81,   110,    23,     0,     0,   188,    78,
      80,    76,     0,   241,     0,     0,     0,     0,   149,   237,
     236,   239,   238,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    23,     0,    23,     0,   177,     0,
       0,    84,    83,     0,   156,     0,   168,     0,   169,   167,
     192,     0,   161,    23,   175,     0,     0,     0,     0,   116,
     121,   122,    96,   102,   100,    98,    94,     0,     0,     0,
       0,     0,     0,     0,   198,   200,     0,     0,   249,   148,
     140,   205,     0,     0,   208,   210,   212,   214,   216,     0,
       0,     0,   221,     0,   223,   226,   227,   230,   229,   232,
     233,   234,   195,     0,   178,    23,     0,   181,     0,     0,
       0,   159,     0,   157,     0,   171,     0,   163,     0,   120,
       0,     0,   124,   130,   125,   123,   111,     0,     0,   189,
      77,    79,   247,     0,   248,   246,     0,   206,   218,   219,
     222,   224,    23,     0,    23,   185,   180,   182,    23,   176,
     193,     0,     0,     0,     0,   119,   117,     0,     0,   131,
     197,     0,     0,     0,   199,   203,   196,     0,   184,   183,
     154,   158,     0,   194,   132,     0,     0,   190,     0,    23,
     173,   134,     0,     0,   133,     0,     0,   179,   135,   126,
     127,   128,   129,   186
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     4,     5,     6,     7,    14,    29,    30,    39,     8,
      24,    25,   109,    10,    18,    19,    11,    45,    77,    78,
      79,    53,    59,    60,    61,    80,    81,    82,    83,    84,
      85,   149,   135,   150,   134,    86,   310,   311,    99,    87,
      88,   180,   156,   157,   222,   214,   220,   218,   216,    89,
     202,   127,   158,    90,    91,   128,   159,   129,   130,   369,
     370,   371,   432,   468,   493,    92,   111,   244,   112,   113,
     114,   422,   423,   115,   116,   194,   187,   275,   276,   117,
     118,   119,   120,   121,   122,   416,   417,    93,    94,   307,
     308,   123,   124,   245,   418,   384,   246,   247,   248,   249,
     250,   251,   252,   253,   254,   255,   256,   257,   258,   259,
     260
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -322
static const yytype_int16 yypact[] =
{
      46,   -24,    61,     2,    45,    33,  -322,  -322,  -322,   151,
    -322,  -322,   146,  -322,     6,  -322,    85,  -322,   100,  -322,
    -322,  -322,   168,   171,   108,  -322,   181,   184,   181,    64,
    -322,   216,  -322,  -322,  -322,    66,  -322,  -322,  -322,   201,
     215,   237,  -322,  -322,  -322,   186,    74,   240,   255,  -322,
    -322,  -322,   281,   535,   266,  -322,  -322,  -322,  -322,   267,
     270,  -322,   275,  -322,  -322,  -322,   425,   150,   283,   301,
     301,   283,   283,  -322,  -322,  -322,  -322,   307,  -322,    89,
     535,  -322,   575,  -322,  -322,   310,  -322,  -322,    23,   315,
    -322,   331,   335,  -322,   294,  -322,   283,   281,   338,   337,
      54,   328,   339,   344,   350,   106,   109,  -322,    79,    99,
     356,  -322,  -322,  -322,   283,  -322,   403,  -322,   283,   362,
    -322,  -322,  -322,   369,  -322,   372,   315,   115,  -322,   283,
     379,  -322,   283,   154,  -322,   156,  -322,  -322,  -322,    14,
    -322,   335,  -322,  -322,  -322,   388,   301,  -322,   283,   166,
    -322,   400,   406,   408,   410,   418,     5,  -322,   191,  -322,
     283,   401,   335,   402,   405,  -322,   239,  -322,  -322,   142,
     357,   120,   357,   357,   357,   423,   357,   424,  -322,   424,
     409,  -322,   429,   430,  -322,   430,   430,  -322,   103,   434,
    -322,  -322,   435,  -322,   427,   417,   411,  -322,   465,   115,
    -322,   315,  -322,  -322,   163,  -322,  -322,   301,  -322,   335,
    -322,  -322,   310,  -322,   193,  -322,   207,  -322,   218,  -322,
     226,  -322,   231,  -322,   426,   315,  -322,  -322,   443,   413,
    -322,   446,   447,   449,  -322,   433,  -322,   135,   357,   438,
      88,    88,    88,    88,   250,  -322,  -322,  -322,    59,   431,
     428,   432,   484,   457,    -5,    11,    20,   279,   116,  -322,
    -322,   448,   110,   356,   450,   454,   456,  -322,   302,   444,
     461,   463,  -322,   293,   459,   232,  -322,   234,   256,   357,
     357,   471,  -322,  -322,  -322,   357,   462,  -322,  -322,   357,
      29,  -322,  -322,  -322,   489,  -322,   491,  -322,   492,  -322,
     493,  -322,   494,  -322,  -322,   283,   496,   322,  -322,  -322,
     476,  -322,   478,  -322,   357,   357,   357,   479,  -322,  -322,
    -322,  -322,  -322,   486,   357,   357,   357,   466,   357,   357,
     357,   357,   357,   487,   497,   188,   261,   357,   357,   357,
     357,   357,   357,   357,    14,   357,    14,   290,  -322,   504,
     500,  -322,  -322,   531,  -322,   539,  -322,   430,  -322,  -322,
    -322,   499,  -322,   473,  -322,   541,   177,   292,   292,  -322,
      29,  -322,  -322,  -322,  -322,  -322,  -322,   502,   503,   521,
     413,   446,   446,   325,  -322,  -322,   330,   501,  -322,  -322,
    -322,   431,    60,   357,   428,   432,   484,   457,    -5,   357,
     357,   357,    20,   357,    20,   279,   279,   116,   116,  -322,
    -322,  -322,   557,   511,  -322,   495,    48,  -322,   257,   509,
     357,  -322,   332,  -322,   506,  -322,   514,  -322,   507,    29,
     360,   360,  -322,  -322,  -322,  -322,  -322,   357,   515,  -322,
    -322,   476,  -322,   357,  -322,  -322,   357,   431,    11,    11,
      20,    20,    14,   120,    14,  -322,  -322,  -322,    14,  -322,
    -322,   516,   531,   567,   357,  -322,  -322,   202,   518,  -322,
     479,   519,   520,   301,  -322,   -19,  -322,   525,  -322,  -322,
    -322,  -322,   527,  -322,  -322,   321,   321,  -322,   341,    14,
    -322,  -322,   227,   123,  -322,   125,   524,  -322,  -322,  -322,
    -322,  -322,  -322,  -322
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -322,  -322,  -322,   581,  -322,  -322,  -322,   561,   565,  -322,
    -322,   570,    16,   -53,  -322,   577,  -322,  -322,  -322,   517,
     -61,  -322,  -322,  -322,   505,  -322,   523,  -322,   522,  -322,
    -322,  -322,   -67,   384,   392,  -322,   219,   225,  -322,  -322,
    -322,    81,  -322,   451,  -322,  -322,  -322,  -322,  -322,   533,
    -128,   482,  -322,  -322,  -322,   414,   386,  -322,   -79,   180,
    -322,   244,   248,   187,   133,   542,  -115,   334,  -322,   464,
    -322,  -322,   162,   512,  -322,   347,  -322,   217,   268,  -322,
    -322,  -322,  -322,  -322,  -322,  -322,   213,   -70,  -322,  -322,
     251,  -168,  -322,  -166,    93,   189,  -164,  -322,  -321,  -311,
     305,   306,   300,   303,   304,    10,  -312,    75,    77,   -66,
    -223
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -24
static const yytype_int16 yytable[] =
{
      76,   193,   133,   264,   261,   392,   265,   266,   267,   142,
     160,    12,   269,   110,   391,   151,     9,   319,   320,   321,
     322,     9,    15,   402,   404,   325,   100,    76,   152,   101,
     226,   162,    26,   151,   367,   102,   103,    16,   290,   104,
      27,     1,    17,   153,   337,    20,   152,   154,   333,   105,
     327,     2,   155,     3,     1,   233,   338,    28,   368,   234,
     235,   153,     3,   110,     2,   154,   107,   236,   334,   108,
     155,   287,   -23,    46,   317,   456,   335,    47,   108,   283,
     209,     3,   447,   237,   131,   336,   110,   136,   137,   450,
      26,   451,   142,   415,     3,   233,    13,    42,    27,   234,
     235,    48,   238,   325,   325,   139,   168,   236,   169,    54,
     240,   241,   164,   360,   181,    28,   361,   242,   263,   364,
     446,   243,   183,   237,   140,   475,    55,   326,   327,   327,
     189,   184,   177,   182,   195,    16,   185,   186,    31,   142,
      32,   110,   238,    23,   178,   203,   160,   179,   205,   233,
     385,   385,   387,   234,   235,   262,   279,    36,   390,   174,
     175,   236,   280,   174,   210,   176,   193,   200,     3,   176,
     233,   201,   189,    68,   234,   235,   227,   237,   499,   413,
     501,    23,   236,   385,   500,   125,   502,    71,    72,   315,
      22,   341,   342,   343,   316,   233,   238,   239,   237,   234,
     235,   433,   433,    34,   240,   241,   206,   236,   208,   233,
     207,   242,   207,   234,   235,   243,    38,   238,   211,    40,
     289,   236,   212,   237,    35,   240,   241,    44,   110,   412,
     484,   414,   242,   324,   233,   429,   243,   237,   234,   235,
      52,   401,   238,   200,   319,   293,   236,   225,   283,   294,
     240,   241,   385,    49,   460,   498,   238,   242,   270,   295,
     271,   243,   237,   296,   469,   469,   470,    50,   233,   320,
     297,   472,   234,   235,   298,   409,   410,   411,   299,   385,
     236,   238,   300,   301,   356,   477,   358,   302,   357,    51,
     357,   110,    56,   110,   231,   232,   237,   233,   483,   233,
     455,   234,   235,   234,   235,   323,   324,    57,   359,   236,
     110,   236,   357,   443,   403,   238,    58,   458,    95,   494,
     494,   377,    96,   240,   241,   237,    97,   237,   233,    98,
     242,     3,   234,   235,   243,   415,   132,   476,   138,   478,
     236,   339,   340,   479,   238,   148,   430,   353,   354,   491,
     125,   431,   240,   241,   240,   241,   237,   348,   349,   242,
     163,   242,   110,   243,   233,   243,   161,   233,   234,   235,
     140,   234,   235,   166,   497,   238,   236,   379,   380,   236,
     442,   443,   170,   240,   492,   444,   443,   461,   462,   167,
     242,   188,   237,   171,   243,   237,   496,   207,   172,   110,
     263,   110,   277,   278,   173,   110,   488,   383,   386,   448,
     449,   238,   405,   406,   238,   100,   407,   408,   101,   240,
     241,   197,   467,   241,   102,   103,   242,   196,   104,   242,
     243,   198,   204,   243,   106,   213,   110,   100,   105,   100,
     101,   215,   101,   217,   191,   219,   102,   103,   102,   103,
     104,     3,   104,   221,   228,   107,   229,   230,   268,   178,
     105,   272,   105,   192,   273,   274,   184,   108,   282,   284,
     281,   106,   286,     3,   285,     3,   303,   107,   305,   107,
     306,   309,   312,   100,   313,   100,   101,   314,   101,   108,
     318,   108,   102,   103,   102,   103,   104,   331,   104,   329,
     332,   328,   345,   344,   330,   350,   105,   100,   105,   346,
     101,   347,   362,   351,   427,   352,   102,   103,   355,     3,
     104,     3,   365,   107,   372,   107,   373,   374,   375,   376,
     105,   378,   381,   382,   388,   108,   393,   108,   389,   419,
     399,    62,   421,     3,    63,    64,   424,   107,   428,    65,
     400,    66,    67,   420,   436,   454,   438,   437,    68,   108,
     426,   452,   445,   453,   459,    69,   463,   464,   465,   473,
      70,   480,    71,    72,   482,   487,   503,    73,   485,   486,
     489,    62,    74,     3,    63,    64,    21,    75,   490,    65,
      43,   145,    67,    41,    37,    33,   292,   143,    68,   291,
     126,   441,   165,   144,   147,    69,   440,   223,   199,   466,
     146,   304,    71,    72,   435,   288,   434,    73,   471,   495,
     224,   141,    74,   366,   481,   425,   190,    75,   363,   457,
     396,   439,   474,   394,   397,   395,   398
};

static const yytype_uint16 yycheck[] =
{
      53,   116,    69,   171,   170,   326,   172,   173,   174,    79,
      89,    35,   176,    66,   325,    10,     0,   240,   241,   242,
     243,     5,    20,   335,   336,    44,    12,    80,    23,    15,
     158,    92,    26,    10,     5,    21,    22,    35,   204,    25,
      34,     8,    40,    38,    24,     0,    23,    42,    53,    35,
      69,    18,    47,    48,     8,     7,    36,    51,    29,    11,
      12,    38,    48,   116,    18,    42,    52,    19,    73,    64,
      47,   199,    39,     7,   238,    27,    65,    11,    64,   194,
     141,    48,   393,    35,    68,    74,   139,    71,    72,   401,
      26,   403,   162,    45,    48,     7,    35,    33,    34,    11,
      12,    35,    54,    44,    44,    16,    52,    19,    54,    35,
      62,    63,    96,   279,    35,    51,   280,    69,   171,   285,
      60,    73,    23,    35,    35,   446,    52,    68,    69,    69,
     114,    32,    23,    54,   118,    35,    37,    38,    53,   209,
      40,   194,    54,    35,    35,   129,   225,    38,   132,     7,
     314,   315,   316,    11,    12,    35,    53,    49,   324,    53,
      54,    19,    59,    53,   148,    59,   281,    52,    48,    59,
       7,    56,   156,    23,    11,    12,   160,    35,    55,   345,
      55,    35,    19,   347,    61,    35,    61,    37,    38,    54,
      39,    75,    76,    77,    59,     7,    54,    55,    35,    11,
      12,   367,   368,    35,    62,    63,    52,    19,    52,     7,
      56,    69,    56,    11,    12,    73,    35,    54,    52,    35,
      57,    19,    56,    35,    53,    62,    63,    11,   281,   344,
      28,   346,    69,    56,     7,    58,    73,    35,    11,    12,
      54,    53,    54,    52,   467,    52,    19,    56,   363,    56,
      62,    63,   416,    52,   420,    28,    54,    69,   177,    52,
     179,    73,    35,    56,   430,   431,   430,    52,     7,   492,
      52,   437,    11,    12,    56,   341,   342,   343,    52,   443,
      19,    54,    56,    52,    52,   453,    52,    56,    56,    52,
      56,   344,    52,   346,    55,    56,    35,     7,   464,     7,
     415,    11,    12,    11,    12,    55,    56,    52,    52,    19,
     363,    19,    56,    56,    53,    54,    35,    60,    52,   485,
     486,   305,    55,    62,    63,    35,    56,    35,     7,    54,
      69,    48,    11,    12,    73,    45,    35,   452,    31,   454,
      19,    62,    63,   458,    54,    35,    54,    54,    55,    28,
      35,    59,    62,    63,    62,    63,    35,    55,    56,    69,
      66,    69,   415,    73,     7,    73,    35,     7,    11,    12,
      35,    11,    12,    35,   489,    54,    19,    55,    56,    19,
      55,    56,    54,    62,    63,    55,    56,    55,    56,    52,
      69,    35,    35,    54,    73,    35,    55,    56,    54,   452,
     453,   454,   185,   186,    54,   458,   473,   314,   315,   399,
     400,    54,   337,   338,    54,    12,   339,   340,    15,    62,
      63,    52,    62,    63,    21,    22,    69,    65,    25,    69,
      73,    59,    53,    73,    46,    35,   489,    12,    35,    12,
      15,    35,    15,    35,    41,    35,    21,    22,    21,    22,
      25,    48,    25,    35,    53,    52,    54,    52,    35,    35,
      35,    52,    35,    60,    35,    35,    32,    64,    41,    52,
      35,    46,     7,    48,    63,    48,    50,    52,    35,    52,
      67,    35,    35,    12,    35,    12,    15,    54,    15,    64,
      52,    64,    21,    22,    21,    22,    25,    13,    25,    71,
      43,    70,    52,    55,    72,    61,    35,    12,    35,    55,
      15,    55,    41,    52,    41,    52,    21,    22,    59,    48,
      25,    48,    60,    52,    35,    52,    35,    35,    35,    35,
      35,    35,    56,    55,    55,    64,    70,    64,    52,    35,
      53,     6,    11,    48,     9,    10,     7,    52,     7,    14,
      53,    16,    17,    53,    52,    60,    35,    54,    23,    64,
      61,     4,    61,    52,    55,    30,    60,    53,    61,    54,
      35,    55,    37,    38,     7,    55,    52,    42,    60,    60,
      55,     6,    47,    48,     9,    10,     5,    52,    61,    14,
      29,    16,    17,    28,    24,    18,   212,    80,    23,   207,
      67,   382,    97,    80,    82,    30,   381,   156,   126,   429,
      35,   225,    37,    38,   370,   201,   368,    42,   431,   486,
     156,    79,    47,   289,   462,   357,   114,    52,   281,   416,
     330,   380,   443,   328,   331,   329,   332
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     8,    18,    48,    79,    80,    81,    82,    87,    90,
      91,    94,    35,    35,    83,    20,    35,    40,    92,    93,
       0,    81,    39,    35,    88,    89,    26,    34,    51,    84,
      85,    53,    40,    93,    35,    53,    49,    89,    35,    86,
      35,    86,    33,    85,    11,    95,     7,    11,    35,    52,
      52,    52,    54,    99,    35,    52,    52,    52,    35,   100,
     101,   102,     6,     9,    10,    14,    16,    17,    23,    30,
      35,    37,    38,    42,    47,    52,    91,    96,    97,    98,
     103,   104,   105,   106,   107,   108,   113,   117,   118,   127,
     131,   132,   143,   165,   166,    52,    55,    56,    54,   116,
      12,    15,    21,    22,    25,    35,    46,    52,    64,    90,
      91,   144,   146,   147,   148,   151,   152,   157,   158,   159,
     160,   161,   162,   169,   170,    35,   127,   129,   133,   135,
     136,    90,    35,   110,   112,   110,    90,    90,    31,    16,
      35,   143,   165,    97,   104,    16,    35,   106,    35,   109,
     111,    10,    23,    38,    42,    47,   120,   121,   130,   134,
     136,    35,    98,    66,    90,   102,    35,    52,    52,    54,
      54,    54,    54,    54,    53,    54,    59,    23,    35,    38,
     119,    35,    54,    23,    32,    37,    38,   154,    35,    90,
     151,    41,    60,   144,   153,    90,    65,    52,    59,   129,
      52,    56,   128,    90,    53,    90,    52,    56,    52,    98,
      90,    52,    56,    35,   123,    35,   126,    35,   125,    35,
     124,    35,   122,   121,   147,    56,   128,    90,    53,    54,
      52,    55,    56,     7,    11,    12,    19,    35,    54,    55,
      62,    63,    69,    73,   145,   171,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   187,
     188,   171,    35,    91,   169,   171,   171,   171,    35,   174,
     119,   119,    52,    35,    35,   155,   156,   155,   155,    53,
      59,    35,    41,   144,    52,    63,     7,   128,   133,    57,
     171,   112,   111,    52,    56,    52,    56,    52,    56,    52,
      56,    52,    56,    50,   134,    35,    67,   167,   168,    35,
     114,   115,    35,    35,    54,    54,    59,   174,    52,   188,
     188,   188,   188,    55,    56,    44,    68,    69,    70,    71,
      72,    13,    43,    53,    73,    65,    74,    24,    36,    62,
      63,    75,    76,    77,    55,    52,    55,    55,    55,    56,
      61,    52,    52,    54,    55,    59,    52,    56,    52,    52,
     171,   174,    41,   153,   171,    60,   145,     5,    29,   137,
     138,   139,    35,    35,    35,    35,    35,    90,    35,    55,
      56,    56,    55,   172,   173,   174,   172,   174,    55,    52,
     171,   177,   176,    70,   178,   179,   180,   181,   182,    53,
      53,    53,   184,    53,   184,   185,   185,   186,   186,   187,
     187,   187,   144,   171,   144,    45,   163,   164,   172,    35,
      53,    11,   149,   150,     7,   156,    61,    41,     7,    58,
      54,    59,   140,   171,   140,   139,    52,    54,    35,   168,
     115,   114,    55,    56,    55,    61,    60,   177,   183,   183,
     184,   184,     4,    52,    60,   144,    27,   164,    60,    55,
     171,    55,    56,    60,    53,    61,   137,    62,   141,   171,
     174,   141,   171,    54,   173,   176,   144,   169,   144,   144,
      55,   150,     7,   171,    28,    60,    60,    55,   110,    55,
      61,    28,    63,   142,   171,   142,    55,   144,    28,    55,
      61,    55,    61,    52
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
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


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
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

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
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



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
  if (yyn == YYPACT_NINF)
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
      if (yyn == 0 || yyn == YYTABLE_NINF)
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

/* Line 1455 of yacc.c  */
#line 286 "verilogaYacc.y"
    {
          ;}
    break;

  case 3:

/* Line 1455 of yacc.c  */
#line 291 "verilogaYacc.y"
    {
          ;}
    break;

  case 4:

/* Line 1455 of yacc.c  */
#line 294 "verilogaYacc.y"
    {
          ;}
    break;

  case 5:

/* Line 1455 of yacc.c  */
#line 299 "verilogaYacc.y"
    {
          ;}
    break;

  case 6:

/* Line 1455 of yacc.c  */
#line 302 "verilogaYacc.y"
    {
          ;}
    break;

  case 7:

/* Line 1455 of yacc.c  */
#line 305 "verilogaYacc.y"
    {
          ;}
    break;

  case 8:

/* Line 1455 of yacc.c  */
#line 310 "verilogaYacc.y"
    {
            adms_admsmain_list_discipline_prepend_once_or_abort(root(),gDiscipline);
            gDiscipline=NULL;
          ;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 317 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            gDiscipline=adms_discipline_new(mylexval1);
          ;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 324 "verilogaYacc.y"
    {
          ;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 327 "verilogaYacc.y"
    {
          ;}
    break;

  case 12:

/* Line 1455 of yacc.c  */
#line 332 "verilogaYacc.y"
    {
            gDiscipline->_potential=(p_nature)YY((yyvsp[(2) - (3)]._yaccval));
          ;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 336 "verilogaYacc.y"
    {
            gDiscipline->_flow=(p_nature)YY((yyvsp[(2) - (3)]._yaccval));
          ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 340 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (3)]._lexval))->_string;
            if(!strcmp(mylexval2,"discrete"))
              gDiscipline->_domain=admse_discrete;
            else if(!strcmp(mylexval2,"continuous"))
              gDiscipline->_domain=admse_continuous;
            else
             adms_veriloga_message_fatal("domain: bad value given - should be either 'discrete' or 'continuous'\n",(yyvsp[(2) - (3)]._lexval));
          ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 352 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            p_nature mynature=lookup_nature(mylexval1);
            if(!mynature)
              adms_veriloga_message_fatal("can't find nature definition\n",(yyvsp[(1) - (1)]._lexval));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mynature);
          ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 363 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (4)]._lexval))->_string;
            p_nature mynature=NULL;
            if(gNatureAccess) 
              mynature=adms_admsmain_list_nature_prepend_by_id_once_or_abort(root(),gNatureAccess);
            else
             adms_veriloga_message_fatal("attribute 'access' in nature definition not found\n",(yyvsp[(2) - (4)]._lexval));
            adms_nature_valueto_name(mynature,mylexval2);
            if(gNatureidt) 
              adms_nature_valueto_idt_name(mynature,gNatureidt);
            if(gNatureddt) 
              adms_nature_valueto_ddt_name(mynature,gNatureddt);
            if(gNatureUnits)
              mynature->_units=gNatureUnits;
            if(gNatureAbsTol)
              mynature->_abstol=gNatureAbsTol;
            gNatureAccess=NULL;
            gNatureAbsTol=NULL;
            gNatureUnits=NULL;
            gNatureidt=NULL;
            gNatureddt=NULL;
          ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 388 "verilogaYacc.y"
    {
          ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 391 "verilogaYacc.y"
    {
          ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 396 "verilogaYacc.y"
    {
            if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,"abstol"))
            {
              if(gNatureAbsTol)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",(yyvsp[(1) - (4)]._lexval));
              gNatureAbsTol=adms_number_new((yyvsp[(3) - (4)]._lexval));
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",(yyvsp[(1) - (4)]._lexval));
          ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 407 "verilogaYacc.y"
    {
            char* mylexval4=((p_lexval)(yyvsp[(4) - (5)]._lexval))->_string;
            admse myunit=admse_1;
            if(!strcmp((yyvsp[(1) - (5)]._lexval)->_string,"abstol"))
            {
              if(gNatureAbsTol)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",(yyvsp[(1) - (5)]._lexval));
              gNatureAbsTol=adms_number_new((yyvsp[(3) - (5)]._lexval));
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",(yyvsp[(1) - (5)]._lexval));
            if(0) {}
            else if(!strcmp(mylexval4,"E")) myunit=admse_E;
            else if(!strcmp(mylexval4,"P")) myunit=admse_P;
            else if(!strcmp(mylexval4,"T")) myunit=admse_T;
            else if(!strcmp(mylexval4,"G")) myunit=admse_G;
            else if(!strcmp(mylexval4,"M")) myunit=admse_M;
            else if(!strcmp(mylexval4,"k")) myunit=admse_k;
            else if(!strcmp(mylexval4,"h")) myunit=admse_h;
            else if(!strcmp(mylexval4,"D")) myunit=admse_D;
            else if(!strcmp(mylexval4,"d")) myunit=admse_d;
            else if(!strcmp(mylexval4,"c")) myunit=admse_c;
            else if(!strcmp(mylexval4,"m")) myunit=admse_m;
            else if(!strcmp(mylexval4,"u")) myunit=admse_u;
            else if(!strcmp(mylexval4,"n")) myunit=admse_n;
            else if(!strcmp(mylexval4,"A")) myunit=admse_A;
            else if(!strcmp(mylexval4,"p")) myunit=admse_p;
            else if(!strcmp(mylexval4,"f")) myunit=admse_f;
            else if(!strcmp(mylexval4,"a")) myunit=admse_a;
            else
              adms_veriloga_message_fatal("can not convert symbol to valid unit\n",(yyvsp[(4) - (5)]._lexval));
            gNatureAbsTol->_scalingunit=myunit;
          ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 441 "verilogaYacc.y"
    {
            char* mylexval3=((p_lexval)(yyvsp[(3) - (4)]._lexval))->_string;
            if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,"units"))
            {
              if(gNatureUnits)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",(yyvsp[(1) - (4)]._lexval));
              gNatureUnits=adms_kclone(mylexval3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",(yyvsp[(1) - (4)]._lexval));
          ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 453 "verilogaYacc.y"
    {
            char* mylexval3=((p_lexval)(yyvsp[(3) - (4)]._lexval))->_string;
            if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,"access"))
            {
              if(gNatureAccess)
                adms_veriloga_message_fatal("nature attribute defined more than once\n",(yyvsp[(1) - (4)]._lexval));
              gNatureAccess=adms_kclone(mylexval3);
            }
            else if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,"idt_nature"))
            {
              if(gNatureidt)
                adms_veriloga_message_fatal("idt_nature attribute defined more than once\n",(yyvsp[(1) - (4)]._lexval));
              gNatureidt=adms_kclone(mylexval3);
            }
            else if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,"ddt_nature"))
            {
              if(gNatureddt)
                adms_veriloga_message_fatal("ddt_nature attribute defined more than once\n",(yyvsp[(1) - (4)]._lexval));
              gNatureddt=adms_kclone(mylexval3);
            }
            else
             adms_veriloga_message_fatal("unknown nature attribute\n",(yyvsp[(1) - (4)]._lexval));
          ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 479 "verilogaYacc.y"
    {
          ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 482 "verilogaYacc.y"
    {
          ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 487 "verilogaYacc.y"
    {
          ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 490 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (2)]._lexval))->_string;
            p_attribute myattribute=adms_attribute_new("ibm");
            p_admst myconstant=adms_admst_newks(adms_kclone(mylexval2));
            myattribute->_value=(p_adms)myconstant;
            adms_slist_push(&gAttributeList,(p_adms)myattribute);
          ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 498 "verilogaYacc.y"
    {
          ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 503 "verilogaYacc.y"
    {
          ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 506 "verilogaYacc.y"
    {
          ;}
    break;

  case 30:

/* Line 1455 of yacc.c  */
#line 511 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (3)]._lexval))->_string;
            char* mylexval3=((p_lexval)(yyvsp[(3) - (3)]._lexval))->_string;
            p_attribute myattribute=adms_attribute_new(mylexval1);
            p_admst myconstant=adms_admst_newks(adms_kclone(mylexval3));
            myattribute->_value=(p_adms)myconstant;
            adms_slist_push(&gAttributeList,(p_adms)myattribute);
          ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 522 "verilogaYacc.y"
    {
            char* mylexval3=((p_lexval)(yyvsp[(3) - (3)]._lexval))->_string;
            p_slist l;
            p_nodealias mynodealias;
            gModule=adms_admsmain_list_module_prepend_by_id_once_or_abort(root(),mylexval3); 
            adms_slist_push(&gBlockList,(p_adms)gModule);
            mynodealias=adms_module_list_nodealias_prepend_by_id_once_or_abort(gModule,gModule,"0"); 
            gGND=adms_module_list_node_prepend_by_id_once_or_abort(gModule,gModule,"GND"); 
            mynodealias->_node=gGND;
            gGND->_location=admse_ground;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gModule->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
          ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 537 "verilogaYacc.y"
    {
            adms_slist_pull(&gBlockList);
            adms_slist_inreverse(&gModule->_assignment);
          ;}
    break;

  case 33:

/* Line 1455 of yacc.c  */
#line 544 "verilogaYacc.y"
    {
          ;}
    break;

  case 34:

/* Line 1455 of yacc.c  */
#line 547 "verilogaYacc.y"
    {
          ;}
    break;

  case 35:

/* Line 1455 of yacc.c  */
#line 550 "verilogaYacc.y"
    {
          ;}
    break;

  case 36:

/* Line 1455 of yacc.c  */
#line 553 "verilogaYacc.y"
    {
          ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 558 "verilogaYacc.y"
    {
          ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 561 "verilogaYacc.y"
    {
          ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 564 "verilogaYacc.y"
    {
          ;}
    break;

  case 40:

/* Line 1455 of yacc.c  */
#line 567 "verilogaYacc.y"
    {
          ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 570 "verilogaYacc.y"
    {
          ;}
    break;

  case 42:

/* Line 1455 of yacc.c  */
#line 575 "verilogaYacc.y"
    {
          ;}
    break;

  case 43:

/* Line 1455 of yacc.c  */
#line 578 "verilogaYacc.y"
    {
          ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 583 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gModule->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
          ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 592 "verilogaYacc.y"
    {
          ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 595 "verilogaYacc.y"
    {
          ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 600 "verilogaYacc.y"
    {
          ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 603 "verilogaYacc.y"
    {
          ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 608 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            p_nodealias mynodealias=adms_module_list_nodealias_prepend_by_id_once_or_abort(gModule,gModule,mylexval1); 
            p_node mynode=adms_module_list_node_prepend_by_id_once_or_abort(gModule,gModule,mylexval1); 
            mynodealias->_node=mynode;
            mynode->_location=admse_external;
          ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 618 "verilogaYacc.y"
    {
          ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 621 "verilogaYacc.y"
    {
          ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 626 "verilogaYacc.y"
    {
          ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 629 "verilogaYacc.y"
    {
            adms_slist_free(gGlobalAttributeList); gGlobalAttributeList=NULL;
          ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 635 "verilogaYacc.y"
    {
            gGlobalAttributeList=gAttributeList;
            gAttributeList=NULL;
          ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 642 "verilogaYacc.y"
    {
          ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 645 "verilogaYacc.y"
    {
          ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 648 "verilogaYacc.y"
    {
          ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 651 "verilogaYacc.y"
    {
          ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 654 "verilogaYacc.y"
    {
          ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 657 "verilogaYacc.y"
    {
          ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 660 "verilogaYacc.y"
    {
          ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 663 "verilogaYacc.y"
    {
          ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 668 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gTerminalList;l;l=l->next)
              ((p_node)l->data)->_direction=gNodeDirection;
            adms_slist_free(gTerminalList); gTerminalList=NULL;
          ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 675 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gNodeList;l;l=l->next)
              ((p_node)l->data)->_location=admse_ground;
            adms_slist_free(gNodeList); gNodeList=NULL;
          ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 682 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (3)]._lexval))->_string;
            char* mydisciplinename=mylexval1;
            p_discipline mydiscipline=adms_admsmain_list_discipline_lookup_by_id(root(),mydisciplinename);
            p_slist l;
            for(l=gNodeList;l;l=l->next)
              ((p_node)l->data)->_discipline=mydiscipline;
            adms_slist_free(gNodeList); gNodeList=NULL;
          ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 694 "verilogaYacc.y"
    {
            gNodeDirection=admse_input;
          ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 698 "verilogaYacc.y"
    {
            gNodeDirection=admse_output;
          ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 702 "verilogaYacc.y"
    {
            gNodeDirection=admse_inout;
          ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 708 "verilogaYacc.y"
    {
          ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 711 "verilogaYacc.y"
    {
          ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 716 "verilogaYacc.y"
    {
          ;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 719 "verilogaYacc.y"
    {
          ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 724 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (2)]._lexval))->_string;
            p_slist l;
            p_node mynode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1);
            if(!mynode)
             adms_veriloga_message_fatal("terminal not found\n",(yyvsp[(1) - (2)]._lexval));
            if(mynode->_location!=admse_external)
             adms_veriloga_message_fatal("node not a terminal\n",(yyvsp[(1) - (2)]._lexval));
            adms_slist_push(&gTerminalList,(p_adms)mynode);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
          ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 742 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (2)]._lexval))->_string;
            p_slist l;
            p_node mynode=adms_module_list_node_prepend_by_id_once_or_ignore(gModule,gModule,mylexval1);
            adms_slist_push(&gNodeList,(p_adms)mynode);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mynode->_attribute,l->data);
          ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 756 "verilogaYacc.y"
    {
          ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 761 "verilogaYacc.y"
    {
          ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 764 "verilogaYacc.y"
    {
          ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 769 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            adms_slist_push(&gBranchAliasList,(p_adms)mylexval1);
          ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 776 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (6)]._lexval))->_string;
            char* mylexval4=((p_lexval)(yyvsp[(4) - (6)]._lexval))->_string;
            p_slist l;
            p_branch mybranch; 
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval2);
            p_node nnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval4);
            mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,nnode); 
            if(!pnode)
             adms_veriloga_message_fatal("node never declared\n",(yyvsp[(2) - (6)]._lexval));
            if(!nnode)
             adms_veriloga_message_fatal("node never declared\n",(yyvsp[(4) - (6)]._lexval));
            for(l=gBranchAliasList;l;l=l->next)
            {
              char*aliasname=(char*)l->data;
              p_branchalias mybranchalias; 
              mybranchalias=adms_module_list_branchalias_prepend_by_id_once_or_abort(gModule,gModule,aliasname); 
              if(mybranchalias) mybranchalias->_branch=mybranch;
            }
            adms_slist_free(gBranchAliasList);
            gBranchAliasList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mybranch->_attribute,l->data);
          ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 801 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (4)]._lexval))->_string;
            p_slist l;
            p_branch mybranch;
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval2);
            if(!pnode)
             adms_veriloga_message_fatal("node never declared\n",(yyvsp[(2) - (4)]._lexval));
            mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,gGND); 
            for(l=gBranchAliasList;l;l=l->next)
            {
              char*aliasname=(char*)l->data;
              p_branchalias mybranchalias; 
              mybranchalias=adms_module_list_branchalias_prepend_by_id_once_or_abort(gModule,gModule,aliasname); 
              if(mybranchalias) mybranchalias->_branch=mybranch;
            }
            adms_slist_free(gBranchAliasList);
            gBranchAliasList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&mybranch->_attribute,l->data);
          ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 824 "verilogaYacc.y"
    {
            adms_slist_pull(&gBlockList);
            gAnalogfunction->_tree=YY((yyvsp[(3) - (4)]._yaccval));
            gAnalogfunction=NULL;
          ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 832 "verilogaYacc.y"
    {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
          ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 838 "verilogaYacc.y"
    {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_type=admse_integer;
            gAnalogfunction->_type=admse_integer; 
          ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 846 "verilogaYacc.y"
    {
            NEWVARIABLE(gAnalogfunction->_lexval)
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
          ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 854 "verilogaYacc.y"
    {
            p_slist l;
            gAnalogfunction=adms_analogfunction_new(gModule,(yyvsp[(1) - (1)]._lexval));
            adms_slist_push(&gBlockList,(p_adms)gAnalogfunction);
            adms_module_list_analogfunction_prepend_once_or_abort(gModule,gAnalogfunction); 
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&gAnalogfunction->_attribute,l->data);
          ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 865 "verilogaYacc.y"
    {
          ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 868 "verilogaYacc.y"
    {
          ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 873 "verilogaYacc.y"
    {
          ;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 876 "verilogaYacc.y"
    {
          ;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 879 "verilogaYacc.y"
    {
          ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 882 "verilogaYacc.y"
    {
          ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 885 "verilogaYacc.y"
    {
          ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 890 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 897 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(3) - (3)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 906 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 913 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(3) - (3)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 922 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 930 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(3) - (3)]._lexval))
            adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
            myvariableprototype->_input=admse_yes;
            myvariableprototype->_output=admse_yes;
            myvariableprototype->_parametertype=admse_analogfunction;
          ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 940 "verilogaYacc.y"
    {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,(yyvsp[(1) - (1)]._lexval),(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_integer;
            else
            {
              NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_integer;
            }
          ;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 952 "verilogaYacc.y"
    {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,(yyvsp[(3) - (3)]._lexval),(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_integer;
            else
            {
              NEWVARIABLE((yyvsp[(3) - (3)]._lexval))
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_integer;
            }
          ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 966 "verilogaYacc.y"
    {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,(yyvsp[(1) - (1)]._lexval),(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_real;
            else
            {
              NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_real;
            }
          ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 978 "verilogaYacc.y"
    {
            p_variableprototype myvariableprototype=adms_analogfunction_list_variable_lookup_by_id(gAnalogfunction,gModule,(yyvsp[(3) - (3)]._lexval),(p_adms)gAnalogfunction);
            if(myvariableprototype)
              myvariableprototype->_type=admse_real;
            else
            {
              NEWVARIABLE((yyvsp[(3) - (3)]._lexval))
              adms_analogfunction_list_variable_prepend_once_or_abort(gAnalogfunction,myvariableprototype); 
              myvariableprototype->_type=admse_real;
            }
          ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 992 "verilogaYacc.y"
    {
            gVariableType=admse_integer;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 998 "verilogaYacc.y"
    {
            gVariableType=admse_real;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1004 "verilogaYacc.y"
    {
            gVariableType=admse_string;
            adms_slist_concat(&gGlobalAttributeList,gAttributeList);
            gAttributeList=NULL;
          ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1012 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gVariableDeclarationList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=gVariableType;
            adms_slist_free(gVariableDeclarationList); gVariableDeclarationList=NULL;
          ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1021 "verilogaYacc.y"
    {
          ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1024 "verilogaYacc.y"
    {
          ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1029 "verilogaYacc.y"
    {
          ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1032 "verilogaYacc.y"
    {
          ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1037 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (6)]._lexval))->_string;
            p_slist l;
            p_variableprototype myvariableprototype=adms_module_list_variable_lookup_by_id(gModule,gModule,(yyvsp[(4) - (6)]._lexval),(p_adms)gModule);
            if(!myvariableprototype)
             adms_veriloga_message_fatal("variable never declared\n",(yyvsp[(4) - (6)]._lexval));
            adms_variableprototype_list_alias_prepend_once_or_abort(myvariableprototype,adms_kclone(mylexval2));
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&myvariableprototype->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&myvariableprototype->_attribute,l->data);
          ;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1053 "verilogaYacc.y"
    {
          ;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1056 "verilogaYacc.y"
    {
          ;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1061 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
          ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1072 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            for(l=gGlobalAttributeList;l;l=l->next)
              adms_slist_push(&((p_variableprototype)gVariableDeclarationList->data)->_attribute,l->data);
          ;}
    break;

  case 116:

/* Line 1455 of yacc.c  */
#line 1083 "verilogaYacc.y"
    {
            ((p_variableprototype)gVariableDeclarationList->data)->_input=admse_yes;
            ((p_variableprototype)gVariableDeclarationList->data)->_default=((p_expression)YY((yyvsp[(3) - (4)]._yaccval)));
            ((p_variableprototype)gVariableDeclarationList->data)->_range=adms_slist_reverse(gRangeList);
            gRangeList=NULL;
          ;}
    break;

  case 117:

/* Line 1455 of yacc.c  */
#line 1090 "verilogaYacc.y"
    {
            p_slist myArgs=(p_slist)YY((yyvsp[(4) - (6)]._yaccval));
            adms_slist_inreverse(&myArgs);
            ((p_variableprototype)gVariableDeclarationList->data)->_input=admse_yes;
            ((p_variableprototype)gVariableDeclarationList->data)->_default=((p_expression)myArgs->data);
            ((p_variableprototype)gVariableDeclarationList->data)->_arraydefault=myArgs;
            ((p_variableprototype)gVariableDeclarationList->data)->_range=adms_slist_reverse(gRangeList);
            gRangeList=NULL;
          ;}
    break;

  case 118:

/* Line 1455 of yacc.c  */
#line 1102 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
            if(adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1))
             adms_veriloga_message_fatal("variable already defined as node\n",(yyvsp[(1) - (1)]._lexval));
            adms_module_list_variable_prepend_once_or_abort(gModule,myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
          ;}
    break;

  case 119:

/* Line 1455 of yacc.c  */
#line 1111 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (6)]._lexval))->_string;
            NEWVARIABLE((yyvsp[(1) - (6)]._lexval))
            if(adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1))
             adms_veriloga_message_fatal("variable already defined as node\n",(yyvsp[(1) - (6)]._lexval));
            adms_module_list_variable_prepend_once_or_abort(gModule,myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
            myvariableprototype->_sizetype=admse_array;
            myvariableprototype->_minsize=adms_number_new((yyvsp[(3) - (6)]._lexval));
            myvariableprototype->_maxsize=adms_number_new((yyvsp[(5) - (6)]._lexval));
          ;}
    break;

  case 120:

/* Line 1455 of yacc.c  */
#line 1125 "verilogaYacc.y"
    {
          ;}
    break;

  case 121:

/* Line 1455 of yacc.c  */
#line 1128 "verilogaYacc.y"
    {
          ;}
    break;

  case 122:

/* Line 1455 of yacc.c  */
#line 1133 "verilogaYacc.y"
    {
          ;}
    break;

  case 123:

/* Line 1455 of yacc.c  */
#line 1136 "verilogaYacc.y"
    {
          ;}
    break;

  case 124:

/* Line 1455 of yacc.c  */
#line 1141 "verilogaYacc.y"
    {
            if(((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_infboundtype==admse_range_bound_value)
              ((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_type=admse_include_value;
            else
              ((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_type=admse_include;
            adms_slist_push(&gRangeList,YY((yyvsp[(2) - (2)]._yaccval)));
          ;}
    break;

  case 125:

/* Line 1455 of yacc.c  */
#line 1149 "verilogaYacc.y"
    {
            if(((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_infboundtype==admse_range_bound_value)
              ((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_type=admse_exclude_value;
            else
              ((p_range)YY((yyvsp[(2) - (2)]._yaccval)))->_type=admse_exclude;
            adms_slist_push(&gRangeList,YY((yyvsp[(2) - (2)]._yaccval)));
          ;}
    break;

  case 126:

/* Line 1455 of yacc.c  */
#line 1159 "verilogaYacc.y"
    {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY((yyvsp[(2) - (5)]._yaccval)),(p_expression)YY((yyvsp[(4) - (5)]._yaccval))); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_exclude;
            myrange->_supboundtype=admse_range_bound_exclude;
            Y((yyval._yaccval),(p_adms)myrange);
          ;}
    break;

  case 127:

/* Line 1455 of yacc.c  */
#line 1167 "verilogaYacc.y"
    {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY((yyvsp[(2) - (5)]._yaccval)),(p_expression)YY((yyvsp[(4) - (5)]._yaccval))); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_exclude;
            myrange->_supboundtype=admse_range_bound_include;
            Y((yyval._yaccval),(p_adms)myrange);
          ;}
    break;

  case 128:

/* Line 1455 of yacc.c  */
#line 1175 "verilogaYacc.y"
    {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY((yyvsp[(2) - (5)]._yaccval)),(p_expression)YY((yyvsp[(4) - (5)]._yaccval))); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_include;
            myrange->_supboundtype=admse_range_bound_exclude;
            Y((yyval._yaccval),(p_adms)myrange);
          ;}
    break;

  case 129:

/* Line 1455 of yacc.c  */
#line 1183 "verilogaYacc.y"
    {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY((yyvsp[(2) - (5)]._yaccval)),(p_expression)YY((yyvsp[(4) - (5)]._yaccval))); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_include;
            myrange->_supboundtype=admse_range_bound_include;
            Y((yyval._yaccval),(p_adms)myrange);
          ;}
    break;

  case 130:

/* Line 1455 of yacc.c  */
#line 1191 "verilogaYacc.y"
    {
            p_range myrange=adms_module_list_range_prepend_by_id_once_or_abort(gModule,gModule,(p_expression)YY((yyvsp[(1) - (1)]._yaccval)),(p_expression)YY((yyvsp[(1) - (1)]._yaccval))); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myrange->_infboundtype=admse_range_bound_value;
            myrange->_supboundtype=admse_range_bound_value;
            Y((yyval._yaccval),(p_adms)myrange);
          ;}
    break;

  case 131:

/* Line 1455 of yacc.c  */
#line 1201 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 132:

/* Line 1455 of yacc.c  */
#line 1205 "verilogaYacc.y"
    {
            p_number mynumber=adms_number_new((yyvsp[(2) - (2)]._lexval)); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("-inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_minus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y((yyval._yaccval),(p_adms)myexpression);
          ;}
    break;

  case 133:

/* Line 1455 of yacc.c  */
#line 1218 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 134:

/* Line 1455 of yacc.c  */
#line 1222 "verilogaYacc.y"
    {
            p_number mynumber=adms_number_new((yyvsp[(1) - (1)]._lexval)); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("+inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_plus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y((yyval._yaccval),(p_adms)myexpression);
          ;}
    break;

  case 135:

/* Line 1455 of yacc.c  */
#line 1233 "verilogaYacc.y"
    {
            p_number mynumber=adms_number_new((yyvsp[(2) - (2)]._lexval)); 
            p_expression myexpression=adms_expression_new(gModule,(p_adms)mynumber); 
            mynumber->_lexval->_string=adms_kclone("+inf");
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myexpression->_infinity=admse_plus;
            myexpression->_hasspecialnumber=adms_kclone("YES");
            Y((yyval._yaccval),(p_adms)myexpression);
          ;}
    break;

  case 136:

/* Line 1455 of yacc.c  */
#line 1246 "verilogaYacc.y"
    {
            gModule->_analog=adms_analog_new(YY((yyvsp[(2) - (2)]._yaccval)));
          ;}
    break;

  case 137:

/* Line 1455 of yacc.c  */
#line 1252 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 138:

/* Line 1455 of yacc.c  */
#line 1256 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 139:

/* Line 1455 of yacc.c  */
#line 1262 "verilogaYacc.y"
    {
            p_slist myArgs=NULL;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY((yyvsp[(1) - (1)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 140:

/* Line 1455 of yacc.c  */
#line 1269 "verilogaYacc.y"
    {
            p_slist myArgs=(p_slist)YY((yyvsp[(1) - (3)]._yaccval));
            (yyval._yaccval)=(yyvsp[(1) - (3)]._yaccval);
            adms_slist_push(&myArgs,YY((yyvsp[(3) - (3)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 141:

/* Line 1455 of yacc.c  */
#line 1278 "verilogaYacc.y"
    {
            p_slist l;
            p_slist lv;
            for(l=gAttributeList;l;l=l->next)
              for(lv=((p_blockvariable)YY((yyvsp[(2) - (2)]._yaccval)))->_variable;lv;lv=lv->next)
                adms_slist_push(&((p_variableprototype)lv->data)->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            (yyval._yaccval)=(yyvsp[(2) - (2)]._yaccval);
          ;}
    break;

  case 142:

/* Line 1455 of yacc.c  */
#line 1288 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 143:

/* Line 1455 of yacc.c  */
#line 1292 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (2)]._yaccval);
          ;}
    break;

  case 144:

/* Line 1455 of yacc.c  */
#line 1296 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 145:

/* Line 1455 of yacc.c  */
#line 1300 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 146:

/* Line 1455 of yacc.c  */
#line 1304 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 147:

/* Line 1455 of yacc.c  */
#line 1308 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 148:

/* Line 1455 of yacc.c  */
#line 1312 "verilogaYacc.y"
    {
            p_function myfunction=adms_function_new((yyvsp[(1) - (5)]._lexval),uid++);
            p_slist myArgs=(p_slist)YY((yyvsp[(3) - (5)]._yaccval));
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&myArgs);
            myfunction->_arguments=myArgs;
            Y((yyval._yaccval),(p_adms)mycallfunction);
          ;}
    break;

  case 149:

/* Line 1455 of yacc.c  */
#line 1323 "verilogaYacc.y"
    {
            p_function myfunction=adms_function_new((yyvsp[(1) - (4)]._lexval),uid++);
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mycallfunction);
          ;}
    break;

  case 150:

/* Line 1455 of yacc.c  */
#line 1331 "verilogaYacc.y"
    {
            p_function myfunction=adms_function_new((yyvsp[(1) - (2)]._lexval),uid++);
            p_callfunction mycallfunction=adms_callfunction_new(gModule,myfunction);
            adms_slist_push(&gModule->_callfunction,(p_adms)mycallfunction);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mycallfunction);
          ;}
    break;

  case 151:

/* Line 1455 of yacc.c  */
#line 1339 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)adms_nilled_new(gModule));
          ;}
    break;

  case 152:

/* Line 1455 of yacc.c  */
#line 1346 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 153:

/* Line 1455 of yacc.c  */
#line 1350 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(2) - (2)]._yaccval);
            adms_lexval_free(((p_block)YY((yyval._yaccval)))->_lexval);
            ((p_block)YY((yyval._yaccval)))->_lexval=(p_lexval)YY((yyvsp[(1) - (2)]._yaccval));
          ;}
    break;

  case 154:

/* Line 1455 of yacc.c  */
#line 1358 "verilogaYacc.y"
    {
            adms_veriloga_message_fatal("@ control not supported\n",(yyvsp[(3) - (7)]._lexval));
          ;}
    break;

  case 155:

/* Line 1455 of yacc.c  */
#line 1362 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (2)]._lexval))->_string;
            char* mypartitionning=adms_kclone(mylexval2);
            if(strcmp(mypartitionning,"initial_model")
              && strcmp(mypartitionning,"initial_instance")
              && strcmp(mypartitionning,"noise")
              && strcmp(mypartitionning,"initial_step")
              && strcmp(mypartitionning,"final_step"))
              adms_veriloga_message_fatal(" @() control not supported\n",(yyvsp[(2) - (2)]._lexval));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)(yyvsp[(2) - (2)]._lexval));
          ;}
    break;

  case 156:

/* Line 1455 of yacc.c  */
#line 1375 "verilogaYacc.y"
    {
            char* mylexval3=((p_lexval)(yyvsp[(3) - (4)]._lexval))->_string;
            char* mypartitionning=adms_kclone(mylexval3);
            if(strcmp(mypartitionning,"initial_model")
              && strcmp(mypartitionning,"initial_instance")
              && strcmp(mypartitionning,"noise")
              && strcmp(mypartitionning,"initial_step")
              && strcmp(mypartitionning,"final_step"))
              adms_veriloga_message_fatal(" @() control not supported\n",(yyvsp[(3) - (4)]._lexval));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)(yyvsp[(3) - (4)]._lexval));
          ;}
    break;

  case 157:

/* Line 1455 of yacc.c  */
#line 1390 "verilogaYacc.y"
    {
          ;}
    break;

  case 158:

/* Line 1455 of yacc.c  */
#line 1393 "verilogaYacc.y"
    {
          ;}
    break;

  case 159:

/* Line 1455 of yacc.c  */
#line 1398 "verilogaYacc.y"
    {
          ;}
    break;

  case 160:

/* Line 1455 of yacc.c  */
#line 1403 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),gBlockList->data);
            adms_slist_pull(&gBlockList);
          ;}
    break;

  case 161:

/* Line 1455 of yacc.c  */
#line 1409 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),gBlockList->data);
            adms_slist_pull(&gBlockList);
            ((p_block)YY((yyval._yaccval)))->_lexval->_string=(yyvsp[(3) - (4)]._lexval)->_string;
          ;}
    break;

  case 162:

/* Line 1455 of yacc.c  */
#line 1416 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),gBlockList->data);
            adms_slist_pull(&gBlockList);
          ;}
    break;

  case 163:

/* Line 1455 of yacc.c  */
#line 1422 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),gBlockList->data);
            adms_slist_pull(&gBlockList);
            ((p_block)YY((yyval._yaccval)))->_lexval->_string=(yyvsp[(3) - (5)]._lexval)->_string;
          ;}
    break;

  case 164:

/* Line 1455 of yacc.c  */
#line 1431 "verilogaYacc.y"
    {
            p_slist l;
            p_block myblock=adms_block_new(gModule,(yyvsp[(2) - (2)]._lexval),gBlockList?((p_block)gBlockList->data):NULL,NULL);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            myblock->_lexval->_string=adms_kclone("");
            adms_slist_push(&gBlockList,(p_adms)myblock);
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&myblock->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            adms_slist_push(&gModule->_block,gBlockList->data);
          ;}
    break;

  case 165:

/* Line 1455 of yacc.c  */
#line 1445 "verilogaYacc.y"
    {
            adms_slist_push(&((p_block)gBlockList->data)->_item,YY((yyvsp[(1) - (1)]._yaccval)));
          ;}
    break;

  case 166:

/* Line 1455 of yacc.c  */
#line 1449 "verilogaYacc.y"
    {
            adms_slist_push(&((p_block)gBlockList->data)->_item,YY((yyvsp[(2) - (2)]._yaccval)));
          ;}
    break;

  case 167:

/* Line 1455 of yacc.c  */
#line 1455 "verilogaYacc.y"
    {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_integer;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y((yyval._yaccval),(p_adms)myblockvariable);
          ;}
    break;

  case 168:

/* Line 1455 of yacc.c  */
#line 1468 "verilogaYacc.y"
    {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_real;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y((yyval._yaccval),(p_adms)myblockvariable);
          ;}
    break;

  case 169:

/* Line 1455 of yacc.c  */
#line 1481 "verilogaYacc.y"
    {
            p_slist l;
            p_blockvariable myblockvariable=adms_blockvariable_new(((p_block)gBlockList->data)); 
            adms_slist_push(&gModule->_blockvariable,(p_adms)myblockvariable); 
            for(l=gBlockVariableList;l;l=l->next)
              ((p_variableprototype)l->data)->_type=admse_string;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&gBlockVariableList);
            myblockvariable->_variable=gBlockVariableList;
            gBlockVariableList=NULL;
            Y((yyval._yaccval),(p_adms)myblockvariable);
          ;}
    break;

  case 170:

/* Line 1455 of yacc.c  */
#line 1496 "verilogaYacc.y"
    {
          ;}
    break;

  case 171:

/* Line 1455 of yacc.c  */
#line 1499 "verilogaYacc.y"
    {
          ;}
    break;

  case 172:

/* Line 1455 of yacc.c  */
#line 1504 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(1) - (1)]._lexval))
            adms_block_list_variable_prepend_once_or_abort(((p_block)gBlockList->data),myvariableprototype); 
            adms_slist_push(&gBlockVariableList,(p_adms)myvariableprototype);
          ;}
    break;

  case 173:

/* Line 1455 of yacc.c  */
#line 1510 "verilogaYacc.y"
    {
            NEWVARIABLE((yyvsp[(1) - (6)]._lexval))
            adms_block_list_variable_prepend_once_or_abort(((p_block)gBlockList->data),myvariableprototype); 
            adms_slist_push(&gVariableDeclarationList,(p_adms)myvariableprototype);
            myvariableprototype->_sizetype=admse_array;
            myvariableprototype->_minsize=adms_number_new((yyvsp[(3) - (6)]._lexval));
            myvariableprototype->_maxsize=adms_number_new((yyvsp[(5) - (6)]._lexval));
          ;}
    break;

  case 174:

/* Line 1455 of yacc.c  */
#line 1521 "verilogaYacc.y"
    {
            p_slist l;
            for(l=gAttributeList;l;l=l->next)
              adms_slist_push(&gContribution->_attribute,l->data);
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            gContribution=NULL;
          ;}
    break;

  case 175:

/* Line 1455 of yacc.c  */
#line 1531 "verilogaYacc.y"
    {
            p_source mysource=(p_source)YY((yyvsp[(1) - (4)]._yaccval));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            gContribution=adms_contribution_new(gModule,mysource,(p_expression)YY((yyvsp[(4) - (4)]._yaccval)),gLexval);
            adms_slist_push(&gModule->_contribution,(p_adms)gContribution);
            Y((yyval._yaccval),(p_adms)gContribution);
            gContribution->_branchalias=gBranchAlias;
            gBranchAlias=NULL;
          ;}
    break;

  case 176:

/* Line 1455 of yacc.c  */
#line 1543 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (6)]._lexval))->_string;
            char* mylexval3=((p_lexval)(yyvsp[(3) - (6)]._lexval))->_string;
            char* mylexval5=((p_lexval)(yyvsp[(5) - (6)]._lexval))->_string;
            p_node Pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval3);
            p_node Nnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval5);
            char* natureID=mylexval1;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),natureID);
            p_branch mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,Pnode,Nnode);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            if(!mynature)
             adms_message_fatal(("[source:error] there is no nature with access %s, missing discipline.h file?\n",natureID))
            gSource=adms_module_list_source_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            gLexval=(yyvsp[(1) - (6)]._lexval);
            Y((yyval._yaccval),(p_adms)gSource);
          ;}
    break;

  case 177:

/* Line 1455 of yacc.c  */
#line 1560 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (4)]._lexval))->_string;
            char* mylexval3=((p_lexval)(yyvsp[(3) - (4)]._lexval))->_string;
            char* natureID=mylexval1;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),natureID);
            p_branchalias branchalias=adms_module_list_branchalias_lookup_by_id(gModule,gModule,mylexval3);
            p_node pnode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval3);
            p_branch mybranch=NULL;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            if(!mynature)
             adms_message_fatal(("[source:error] there is no nature with access %s, please, include discipline.h file\n",natureID))
            if(pnode)
              mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,pnode,gGND);
            else if(branchalias)
              mybranch=branchalias->_branch;
            else
              adms_veriloga_message_fatal("undefined branch or node\n",(yyvsp[(1) - (4)]._lexval));
            gSource=adms_module_list_source_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            gLexval=(yyvsp[(1) - (4)]._lexval);
            gBranchAlias=branchalias;
            Y((yyval._yaccval),(p_adms)gSource);
          ;}
    break;

  case 178:

/* Line 1455 of yacc.c  */
#line 1585 "verilogaYacc.y"
    {
            p_whileloop mywhileloop=adms_whileloop_new(gModule,(p_expression)YY((yyvsp[(3) - (5)]._yaccval)),YY((yyvsp[(5) - (5)]._yaccval)));
            adms_slist_push(&gModule->_whileloop,(p_adms)mywhileloop);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mywhileloop);
          ;}
    break;

  case 179:

/* Line 1455 of yacc.c  */
#line 1594 "verilogaYacc.y"
    {
            p_forloop myforloop=adms_forloop_new(gModule,(p_assignment)YY((yyvsp[(3) - (9)]._yaccval)),(p_expression)YY((yyvsp[(5) - (9)]._yaccval)),(p_assignment)YY((yyvsp[(7) - (9)]._yaccval)),YY((yyvsp[(9) - (9)]._yaccval)));
            adms_slist_push(&gModule->_forloop,(p_adms)myforloop);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myforloop);
          ;}
    break;

  case 180:

/* Line 1455 of yacc.c  */
#line 1603 "verilogaYacc.y"
    {
            p_case mycase=adms_case_new(gModule,(p_expression)YY((yyvsp[(3) - (6)]._yaccval)));
            adms_slist_push(&gModule->_case,(p_adms)mycase);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            mycase->_caseitem=adms_slist_reverse((p_slist)YY((yyvsp[(5) - (6)]._yaccval)));
            Y((yyval._yaccval),(p_adms)mycase);
          ;}
    break;

  case 181:

/* Line 1455 of yacc.c  */
#line 1613 "verilogaYacc.y"
    {
            p_slist myArgs=NULL;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY((yyvsp[(1) - (1)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 182:

/* Line 1455 of yacc.c  */
#line 1620 "verilogaYacc.y"
    {
            p_slist myArgs=(p_slist)YY((yyvsp[(1) - (2)]._yaccval));
            (yyval._yaccval)=(yyvsp[(1) - (2)]._yaccval);
            adms_slist_push(&myArgs,YY((yyvsp[(2) - (2)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 183:

/* Line 1455 of yacc.c  */
#line 1629 "verilogaYacc.y"
    {
            p_slist myArgs=(p_slist)YY((yyvsp[(1) - (3)]._yaccval));
            p_caseitem mycaseitem=adms_caseitem_new(YY((yyvsp[(3) - (3)]._yaccval)));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            mycaseitem->_condition=adms_slist_reverse(myArgs);
            Y((yyval._yaccval),(p_adms)mycaseitem);
          ;}
    break;

  case 184:

/* Line 1455 of yacc.c  */
#line 1637 "verilogaYacc.y"
    {
            p_caseitem mycaseitem=adms_caseitem_new(YY((yyvsp[(3) - (3)]._yaccval)));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            mycaseitem->_defaultcase=admse_yes;
            Y((yyval._yaccval),(p_adms)mycaseitem);
          ;}
    break;

  case 185:

/* Line 1455 of yacc.c  */
#line 1644 "verilogaYacc.y"
    {
            p_caseitem mycaseitem=adms_caseitem_new(YY((yyvsp[(2) - (2)]._yaccval)));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            mycaseitem->_defaultcase=admse_yes;
            Y((yyval._yaccval),(p_adms)mycaseitem);
          ;}
    break;

  case 186:

/* Line 1455 of yacc.c  */
#line 1653 "verilogaYacc.y"
    {
            char* mylexval6=((p_lexval)(yyvsp[(6) - (10)]._lexval))->_string;
            p_instance myinstance;
            p_slist l1;
            p_slist l2;
            myinstance=adms_module_list_instance_prepend_by_id_once_or_abort(gModule,gModule,gInstanceModule,mylexval6);
            adms_slist_inreverse(&gInstanceModule->_node);
            l2=gInstanceModule->_node;
            l2=l2->next; /*GND ignored*/
            for(l1=adms_slist_reverse(gNodeList);l1;l1=l1->next)
            {
              adms_instance_list_terminal_prepend_once_or_abort(myinstance,adms_instancenode_new(((p_node)l1->data),(p_node)l2->data));
              l2=l2->next;
            }
            for(l1=gInstanceVariableList;l1;l1=l1->next)
              adms_instance_list_parameterset_prepend_once_or_abort(myinstance,(p_instanceparameter)l1->data);
            adms_slist_inreverse(&gInstanceModule->_node);
            adms_slist_free(gNodeList);gNodeList=NULL;
            adms_slist_free(gInstanceVariableList);gInstanceVariableList=NULL;
          ;}
    break;

  case 187:

/* Line 1455 of yacc.c  */
#line 1676 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            gInstanceModule=adms_admsmain_list_module_lookup_by_id(root(),mylexval1);
            if(!gInstanceModule)
              adms_message_fatal(("module '%s' not found\n",mylexval1))
          ;}
    break;

  case 188:

/* Line 1455 of yacc.c  */
#line 1685 "verilogaYacc.y"
    {
          ;}
    break;

  case 189:

/* Line 1455 of yacc.c  */
#line 1688 "verilogaYacc.y"
    {
          ;}
    break;

  case 190:

/* Line 1455 of yacc.c  */
#line 1693 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (5)]._lexval))->_string;
            p_variableprototype myvariableprototype=adms_module_list_variable_lookup_by_id(gInstanceModule,gInstanceModule,(yyvsp[(2) - (5)]._lexval),(p_adms)gInstanceModule);
            if(myvariableprototype)
            {
              p_instanceparameter myinstanceparameter;
              myinstanceparameter=adms_instanceparameter_new(myvariableprototype);
              adms_slist_push(&gInstanceVariableList,(p_adms)myinstanceparameter);
              myinstanceparameter->_value=((p_expression)YY((yyvsp[(4) - (5)]._yaccval)));
            }
            else
            {
              adms_veriloga_message_fatal_continue((yyvsp[(2) - (5)]._lexval));
              adms_message_fatal(("[%s.%s.%s]: undefined variable (instance declaration)",
                adms_module_uid(gModule),adms_module_uid(gInstanceModule),mylexval2))
            }
          ;}
    break;

  case 191:

/* Line 1455 of yacc.c  */
#line 1713 "verilogaYacc.y"
    {
            p_assignment myassignment;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(1) - (3)]._lexval));
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",(yyvsp[(1) - (3)]._lexval));
            myvariableprototype=myvariable->_prototype;
            myassignment=adms_assignment_new(gModule,(p_adms)myvariable,(p_expression)YY((yyvsp[(3) - (3)]._yaccval)),(yyvsp[(1) - (3)]._lexval));
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myassignment);
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          ;}
    break;

  case 192:

/* Line 1455 of yacc.c  */
#line 1728 "verilogaYacc.y"
    {
            p_assignment myassignment;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(2) - (4)]._lexval));
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",(yyvsp[(2) - (4)]._lexval));
            myvariableprototype=myvariable->_prototype;
            myassignment=adms_assignment_new(gModule,(p_adms)myvariable,(p_expression)YY((yyvsp[(4) - (4)]._yaccval)),(yyvsp[(2) - (4)]._lexval));
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myassignment);
            {
              p_slist l;
              for(l=gAttributeList;l;l=l->next)
                adms_slist_push(&myassignment->_attribute,l->data);
            }
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          ;}
    break;

  case 193:

/* Line 1455 of yacc.c  */
#line 1749 "verilogaYacc.y"
    {
            p_assignment myassignment;
            p_array myarray;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(1) - (6)]._lexval));
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",(yyvsp[(1) - (6)]._lexval));
            myvariableprototype=myvariable->_prototype;
            myarray=adms_array_new(myvariable,YY((yyvsp[(3) - (6)]._yaccval)));
            myassignment=adms_assignment_new(gModule,(p_adms)myarray,(p_expression)YY((yyvsp[(6) - (6)]._yaccval)),(yyvsp[(1) - (6)]._lexval));
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myassignment);
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          ;}
    break;

  case 194:

/* Line 1455 of yacc.c  */
#line 1766 "verilogaYacc.y"
    {
            p_assignment myassignment;
            p_array myarray;
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(2) - (7)]._lexval));
            p_variableprototype myvariableprototype;
            if(!myvariable)
              adms_veriloga_message_fatal("undefined variable\n",(yyvsp[(2) - (7)]._lexval));
            myvariableprototype=myvariable->_prototype;
            myarray=adms_array_new(myvariable,YY((yyvsp[(4) - (7)]._yaccval)));
            myassignment=adms_assignment_new(gModule,(p_adms)myarray,(p_expression)YY((yyvsp[(7) - (7)]._yaccval)),(yyvsp[(2) - (7)]._lexval));
            adms_slist_push(&gModule->_assignment,(p_adms)myassignment);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myassignment);
            {
              p_slist l;
              for(l=gAttributeList;l;l=l->next)
                adms_slist_push(&myassignment->_attribute,l->data);
            }
            adms_slist_free(gAttributeList); gAttributeList=NULL;
            myvariableprototype->_vcount++;
            myvariableprototype->_vlast=myassignment;
          ;}
    break;

  case 195:

/* Line 1455 of yacc.c  */
#line 1791 "verilogaYacc.y"
    {
            p_expression myexpression=(p_expression)YY((yyvsp[(3) - (5)]._yaccval));
            p_adms mythen=YY((yyvsp[(5) - (5)]._yaccval));
            p_conditional myconditional=adms_conditional_new(gModule,myexpression,mythen,NULL);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myconditional);
          ;}
    break;

  case 196:

/* Line 1455 of yacc.c  */
#line 1799 "verilogaYacc.y"
    {
            p_expression myexpression=(p_expression)YY((yyvsp[(3) - (7)]._yaccval));
            p_adms mythen=YY((yyvsp[(5) - (7)]._yaccval));
            p_adms myelse=YY((yyvsp[(7) - (7)]._yaccval));
            p_conditional myconditional=adms_conditional_new(gModule,myexpression,mythen,myelse);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myconditional);
          ;}
    break;

  case 197:

/* Line 1455 of yacc.c  */
#line 1810 "verilogaYacc.y"
    {
            p_expression myexpression=adms_expression_new(gModule,YY((yyvsp[(1) - (1)]._yaccval))); 
            adms_slist_push(&gModule->_expression,(p_adms)myexpression); 
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
            Y((yyval._yaccval),(p_adms)myexpression);
          ;}
    break;

  case 198:

/* Line 1455 of yacc.c  */
#line 1819 "verilogaYacc.y"
    {
            p_slist myArgs=NULL;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_push(&myArgs,YY((yyvsp[(1) - (1)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 199:

/* Line 1455 of yacc.c  */
#line 1826 "verilogaYacc.y"
    {
            p_slist myArgs=(p_slist)YY((yyvsp[(1) - (3)]._yaccval));
            (yyval._yaccval)=(yyvsp[(1) - (3)]._yaccval);
            adms_slist_push(&myArgs,YY((yyvsp[(3) - (3)]._yaccval)));
            Y((yyval._yaccval),(p_adms)myArgs);
          ;}
    break;

  case 200:

/* Line 1455 of yacc.c  */
#line 1835 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 201:

/* Line 1455 of yacc.c  */
#line 1841 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 202:

/* Line 1455 of yacc.c  */
#line 1847 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 203:

/* Line 1455 of yacc.c  */
#line 1851 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (5)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (5)]._yaccval));
            p_adms m3=YY((yyvsp[(5) - (5)]._yaccval));
            p_mapply_ternary myop=adms_mapply_ternary_new(admse_conditional,m1,m2,m3);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 204:

/* Line 1455 of yacc.c  */
#line 1862 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 205:

/* Line 1455 of yacc.c  */
#line 1866 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_equr,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 206:

/* Line 1455 of yacc.c  */
#line 1874 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (4)]._yaccval));
            p_adms m2=YY((yyvsp[(4) - (4)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_equl,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 207:

/* Line 1455 of yacc.c  */
#line 1884 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 208:

/* Line 1455 of yacc.c  */
#line 1888 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_xor,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 209:

/* Line 1455 of yacc.c  */
#line 1898 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 210:

/* Line 1455 of yacc.c  */
#line 1902 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_or,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 211:

/* Line 1455 of yacc.c  */
#line 1912 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 212:

/* Line 1455 of yacc.c  */
#line 1916 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_bw_and,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 213:

/* Line 1455 of yacc.c  */
#line 1926 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 214:

/* Line 1455 of yacc.c  */
#line 1930 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_or,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 215:

/* Line 1455 of yacc.c  */
#line 1940 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 216:

/* Line 1455 of yacc.c  */
#line 1944 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_and,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 217:

/* Line 1455 of yacc.c  */
#line 1954 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 218:

/* Line 1455 of yacc.c  */
#line 1958 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (4)]._yaccval));
            p_adms m2=YY((yyvsp[(4) - (4)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_equ,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 219:

/* Line 1455 of yacc.c  */
#line 1966 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (4)]._yaccval));
            p_adms m2=YY((yyvsp[(4) - (4)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_notequ,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 220:

/* Line 1455 of yacc.c  */
#line 1976 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 221:

/* Line 1455 of yacc.c  */
#line 1980 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_lt,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 222:

/* Line 1455 of yacc.c  */
#line 1988 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (4)]._yaccval));
            p_adms m2=YY((yyvsp[(4) - (4)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_lt_equ,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 223:

/* Line 1455 of yacc.c  */
#line 1996 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_gt,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 224:

/* Line 1455 of yacc.c  */
#line 2004 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (4)]._yaccval));
            p_adms m2=YY((yyvsp[(4) - (4)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_gt_equ,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 225:

/* Line 1455 of yacc.c  */
#line 2014 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 226:

/* Line 1455 of yacc.c  */
#line 2018 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_shiftr,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 227:

/* Line 1455 of yacc.c  */
#line 2026 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_shiftl,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 228:

/* Line 1455 of yacc.c  */
#line 2036 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 229:

/* Line 1455 of yacc.c  */
#line 2040 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_addp,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 230:

/* Line 1455 of yacc.c  */
#line 2048 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_addm,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 231:

/* Line 1455 of yacc.c  */
#line 2058 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 232:

/* Line 1455 of yacc.c  */
#line 2062 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_multtime,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 233:

/* Line 1455 of yacc.c  */
#line 2070 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_multdiv,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 234:

/* Line 1455 of yacc.c  */
#line 2078 "verilogaYacc.y"
    {
            p_adms m1=YY((yyvsp[(1) - (3)]._yaccval));
            p_adms m2=YY((yyvsp[(3) - (3)]._yaccval));
            p_mapply_binary myop=adms_mapply_binary_new(admse_multmod,m1,m2);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myop);
          ;}
    break;

  case 235:

/* Line 1455 of yacc.c  */
#line 2088 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(1) - (1)]._yaccval);
          ;}
    break;

  case 236:

/* Line 1455 of yacc.c  */
#line 2092 "verilogaYacc.y"
    {
            p_adms m=YY((yyvsp[(2) - (2)]._yaccval));
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_plus,m);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mymathapply);
          ;}
    break;

  case 237:

/* Line 1455 of yacc.c  */
#line 2099 "verilogaYacc.y"
    {
            p_adms m=YY((yyvsp[(2) - (2)]._yaccval));
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_minus,m);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mymathapply);
          ;}
    break;

  case 238:

/* Line 1455 of yacc.c  */
#line 2106 "verilogaYacc.y"
    {
            p_adms m=YY((yyvsp[(2) - (2)]._yaccval));
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_not,m);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mymathapply);
          ;}
    break;

  case 239:

/* Line 1455 of yacc.c  */
#line 2113 "verilogaYacc.y"
    {
            p_adms m=YY((yyvsp[(2) - (2)]._yaccval));
            p_mapply_unary mymathapply=adms_mapply_unary_new(admse_bw_not,m);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)mymathapply);
          ;}
    break;

  case 240:

/* Line 1455 of yacc.c  */
#line 2122 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)adms_number_new((yyvsp[(1) - (1)]._lexval)));
          ;}
    break;

  case 241:

/* Line 1455 of yacc.c  */
#line 2127 "verilogaYacc.y"
    {
            char* mylexval2=((p_lexval)(yyvsp[(2) - (2)]._lexval))->_string;
            p_number mynumber=adms_number_new((yyvsp[(1) - (2)]._lexval));
            int myunit=admse_1;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            if(0) {}
            else if(!strcmp(mylexval2,"E")) myunit=admse_E;
            else if(!strcmp(mylexval2,"P")) myunit=admse_P;
            else if(!strcmp(mylexval2,"T")) myunit=admse_T;
            else if(!strcmp(mylexval2,"G")) myunit=admse_G;
            else if(!strcmp(mylexval2,"M")) myunit=admse_M;
            else if(!strcmp(mylexval2,"k")) myunit=admse_k;
            else if(!strcmp(mylexval2,"h")) myunit=admse_h;
            else if(!strcmp(mylexval2,"D")) myunit=admse_D;
            else if(!strcmp(mylexval2,"d")) myunit=admse_d;
            else if(!strcmp(mylexval2,"c")) myunit=admse_c;
            else if(!strcmp(mylexval2,"m")) myunit=admse_m;
            else if(!strcmp(mylexval2,"u")) myunit=admse_u;
            else if(!strcmp(mylexval2,"n")) myunit=admse_n;
            else if(!strcmp(mylexval2,"A")) myunit=admse_A;
            else if(!strcmp(mylexval2,"p")) myunit=admse_p;
            else if(!strcmp(mylexval2,"f")) myunit=admse_f;
            else if(!strcmp(mylexval2,"a")) myunit=admse_a;
            else
              adms_veriloga_message_fatal(" can not convert symbol to valid unit\n",(yyvsp[(2) - (2)]._lexval));
            mynumber->_scalingunit=myunit;
            Y((yyval._yaccval),(p_adms)mynumber);
          ;}
    break;

  case 242:

/* Line 1455 of yacc.c  */
#line 2156 "verilogaYacc.y"
    {
            adms_veriloga_message_fatal("%s: character are not handled\n",(yyvsp[(1) - (1)]._lexval));
          ;}
    break;

  case 243:

/* Line 1455 of yacc.c  */
#line 2160 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)adms_string_new(mylexval1));
          ;}
    break;

  case 244:

/* Line 1455 of yacc.c  */
#line 2166 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (1)]._lexval))->_string;
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            if(gAnalogfunction)
            {
              p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(1) - (1)]._lexval));
              if(myvariable)
                Y((yyval._yaccval),(p_adms)myvariable);
            }
            else
            {
              p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(1) - (1)]._lexval));
              if(myvariable)
                Y((yyval._yaccval),(p_adms)myvariable);
              else
              {
                p_branchalias mybranchalias=adms_module_list_branchalias_lookup_by_id(gModule,gModule,mylexval1);
                p_node mynode=adms_module_list_node_lookup_by_id(gModule,gModule,mylexval1);
                if(mynode) Y((yyval._yaccval),(p_adms)mynode);
                if(mybranchalias)
                  Y((yyval._yaccval),(p_adms)mybranchalias->_branch);
              }
            }
            if(!YY((yyval._yaccval)))
              adms_veriloga_message_fatal("identifier never declared\n",(yyvsp[(1) - (1)]._lexval));
          ;}
    break;

  case 245:

/* Line 1455 of yacc.c  */
#line 2193 "verilogaYacc.y"
    {
            p_function myfunction=adms_function_new((yyvsp[(1) - (1)]._lexval),uid++);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            Y((yyval._yaccval),(p_adms)myfunction);
          ;}
    break;

  case 246:

/* Line 1455 of yacc.c  */
#line 2199 "verilogaYacc.y"
    {
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            p_variable myvariable=variable_recursive_lookup_by_id(gBlockList->data,(yyvsp[(1) - (4)]._lexval));
            if(!myvariable)
               adms_veriloga_message_fatal("undefined array variable\n",(yyvsp[(1) - (4)]._lexval));
            Y((yyval._yaccval),(p_adms)adms_array_new(myvariable,YY((yyvsp[(3) - (4)]._yaccval))));
          ;}
    break;

  case 247:

/* Line 1455 of yacc.c  */
#line 2207 "verilogaYacc.y"
    {
            p_function myfunction=adms_function_new((yyvsp[(1) - (4)]._lexval),uid++);
            p_slist myArgs=(p_slist)YY((yyvsp[(3) - (4)]._yaccval));
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            adms_slist_inreverse(&myArgs);
            myfunction->_arguments=myArgs;
            Y((yyval._yaccval),(p_adms)myfunction);
          ;}
    break;

  case 248:

/* Line 1455 of yacc.c  */
#line 2216 "verilogaYacc.y"
    {
            char* mylexval1=((p_lexval)(yyvsp[(1) - (4)]._lexval))->_string;
            char* myfunctionname=mylexval1;
            p_slist myArgs=(p_slist)YY((yyvsp[(3) - (4)]._yaccval));
            int narg=adms_slist_length(myArgs);
            p_probe myprobe=NULL;
            p_nature mynature=adms_admsmain_list_nature_lookup_by_id(root(),myfunctionname);
            (yyval._yaccval)=adms_yaccval_new("unknown source file");
            if(mynature && narg==1)
            {
              p_adms mychild0=(p_adms)adms_slist_nth_data(myArgs,0);
              if(mychild0->_datatypename==admse_node)
              {
                p_branch mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,(p_node)mychild0,gGND);
                myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
              }
              else if(mychild0->_datatypename==admse_branch)
              {
                myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,(p_branch)mychild0,mynature);
              }
              else
                adms_veriloga_message_fatal("bad argument (expecting node or branch)\n",(yyvsp[(1) - (4)]._lexval));
            }
            else if(mynature && narg==2)
            {
              p_adms mychild0=(p_adms)adms_slist_nth_data(myArgs,0);
              p_adms mychild1=(p_adms)adms_slist_nth_data(myArgs,1);
              p_branch mybranch;
              if(mychild0->_datatypename!=admse_node)
                adms_veriloga_message_fatal("second argument of probe is not a node\n",(yyvsp[(1) - (4)]._lexval));
              if(mychild1->_datatypename!=admse_node)
                adms_veriloga_message_fatal("first argument of probe is not a node\n",(yyvsp[(1) - (4)]._lexval));
              mybranch=adms_module_list_branch_prepend_by_id_once_or_ignore(gModule,gModule,(p_node)mychild1,((p_node)mychild0));
              myprobe=adms_module_list_probe_prepend_by_id_once_or_ignore(gModule,gModule,mybranch,mynature);
            }
            if(myprobe)
              Y((yyval._yaccval),(p_adms)myprobe);
            else
            {
              p_slist l;
              p_function myfunction=adms_function_new((yyvsp[(1) - (4)]._lexval),uid++);
              for(l=gModule->_analogfunction;l&&(myfunction->_definition==NULL);l=l->next)
              {
                p_analogfunction myanalogfunction=(p_analogfunction)l->data;
                if(!strcmp((yyvsp[(1) - (4)]._lexval)->_string,myanalogfunction->_lexval->_string))
                  myfunction->_definition=myanalogfunction;
              }
              myfunction->_arguments=adms_slist_reverse(myArgs);
              Y((yyval._yaccval),(p_adms)myfunction);
            }
          ;}
    break;

  case 249:

/* Line 1455 of yacc.c  */
#line 2268 "verilogaYacc.y"
    {
            (yyval._yaccval)=(yyvsp[(2) - (3)]._yaccval);
          ;}
    break;



/* Line 1455 of yacc.c  */
#line 5025 "y.tab.c"
      default: break;
    }
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
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
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
      if (yyn != YYPACT_NINF)
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
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
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



/* Line 1675 of yacc.c  */
#line 2272 "verilogaYacc.y"

void adms_veriloga_setint_yydebug(const int val)
{
  yydebug=val;
}

