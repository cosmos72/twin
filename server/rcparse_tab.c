/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */



/*
 * C declarations
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>
    
#include "twin.h"
#include "methods.h"
#include "data.h"
#include "extreg.h"
#include "hw.h"
#include "main.h"
#include "printk.h"
#include "rctypes.h"
#include "util.h"
#include "version.h"
#include "wm.h"

#include "rcrun.h"       /* needs wm.h, rctypes.h */
#include "rcparse_tab.h" /* needs rcrun.h         */
#include "rcparse.h"     /* needs rcparse_tab.h   */


/*
 * try to get meaningful error messages
 */

#define YYERROR_VERBOSE

#ifdef DEBUG_YACC
# define YYDEBUG 1
  int yydebug = 1;

  static void yyprint(FILE *file, int type, void *value);

# define YYPRINT(file, type, value)   yyprint(file, type, &(value))

#endif

/* twin.h typedefs `msg'... avoid it */
#define msg Msg





# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "rcparse_tab.h".  */
#ifndef YY_YY_RCPARSE_TAB_H_INCLUDED
# define YY_YY_RCPARSE_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    ADDSCREEN = 258,
    ADDTOMENU = 259,
    ADDTOFUNC = 260,
    BACKGROUND = 261,
    BORDER = 262,
    BUTTON = 263,
    DELETEFUNC = 264,
    DELETEMENU = 265,
    DELETEBUTTON = 266,
    DELETESCREEN = 267,
    READ = 268,
    EXEC = 269,
    EXECTTY = 270,
    GLOBALFLAGS = 271,
    INTERACTIVE = 272,
    KEY = 273,
    MENU = 274,
    MODULE = 275,
    MOUSE = 276,
    MOVE = 277,
    MOVESCREEN = 278,
    NEXT = 279,
    NOP = 280,
    PREV = 281,
    RESTART = 282,
    RESIZE = 283,
    RESIZESCREEN = 284,
    SCREEN = 285,
    SCROLL = 286,
    SENDTOSCREEN = 287,
    SLEEP = 288,
    STDERR = 289,
    SYNTHETICKEY = 290,
    WAIT = 291,
    WINDOW = 292,
    EASY_FUNC = 293,
    BEEP = 294,
    CENTER = 295,
    CLOSE = 296,
    KILL = 297,
    QUIT = 298,
    REFRESH = 299,
    WINDOWLIST = 300,
    FLAG_FUNC = 301,
    FOCUS = 302,
    MAXIMIZE = 303,
    FULLSCREEN = 304,
    LOWER = 305,
    RAISE = 306,
    RAISELOWER = 307,
    ROLL = 308,
    USERFUNC = 309,
    FL_ON = 310,
    FL_OFF = 311,
    FL_TOGGLE = 312,
    FL_ACTIVE = 313,
    FL_INACTIVE = 314,
    FL_LEFT = 315,
    FL_RIGHT = 316,
    GLOBAL_FLAG = 317,
    ALTFONT = 318,
    CURSOR_ALWAYS = 319,
    BLINK = 320,
    MENU_HIDE = 321,
    MENU_INFO = 322,
    MENU_RELAX = 323,
    SCREEN_SCROLL = 324,
    TERMINALS_UTF8 = 325,
    SHADOWS = 326,
    BUTTON_PASTE = 327,
    BUTTON_SELECTION = 328,
    COLOR = 329,
    COL_HIGH = 330,
    KBD_FLAG = 331,
    NUMBER = 332,
    STRING = 333
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{


    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    node	 _node;


};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_RCPARSE_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */



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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   275

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  84
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  174

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   333

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      40,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     4,     2,    58,     2,    59,     2,     2,     2,     2,
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
       2,     2,     2,     2,     2,     2,     1,     2,     5,     6,
       7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,    26,
      27,    28,    29,    30,    31,    32,    33,    34,    35,    36,
      37,    38,    39,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      60,    61,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   156,   156,   159,   160,   163,   164,   165,   166,   169,
     170,   173,   174,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   197,   200,   201,   204,   207,   210,   211,   214,   217,
     220,   221,   224,   227,   228,   229,   230,   233,   234,   237,
     238,   241,   242,   243,   244,   245,   246,   249,   250,   251,
     252,   253,   254,   256,   257,   258,   260,   261,   262,   263,
     264,   265,   266,   267,   268,   269,   270,   273,   274,   277,
     278,   281,   282,   283,   284,   285,   288,   289,   290,   291,
     294,   295,   298,   299,   302,   303,   306,   307,   310,   311,
     312,   313,   316,   317,   320,   321
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "'('", "')'", "ADDSCREEN", "ADDTOMENU",
  "ADDTOFUNC", "BACKGROUND", "BORDER", "BUTTON", "DELETEFUNC",
  "DELETEMENU", "DELETEBUTTON", "DELETESCREEN", "READ", "EXEC", "EXECTTY",
  "GLOBALFLAGS", "INTERACTIVE", "KEY", "MENU", "MODULE", "MOUSE", "MOVE",
  "MOVESCREEN", "NEXT", "NOP", "PREV", "RESTART", "RESIZE", "RESIZESCREEN",
  "SCREEN", "SCROLL", "SENDTOSCREEN", "SLEEP", "STDERR", "SYNTHETICKEY",
  "WAIT", "WINDOW", "'\\n'", "EASY_FUNC", "BEEP", "CENTER", "CLOSE",
  "KILL", "QUIT", "REFRESH", "WINDOWLIST", "FLAG_FUNC", "FOCUS",
  "MAXIMIZE", "FULLSCREEN", "LOWER", "RAISE", "RAISELOWER", "ROLL",
  "USERFUNC", "'+'", "'-'", "FL_ON", "FL_OFF", "FL_TOGGLE", "FL_ACTIVE",
  "FL_INACTIVE", "FL_LEFT", "FL_RIGHT", "GLOBAL_FLAG", "ALTFONT",
  "CURSOR_ALWAYS", "BLINK", "MENU_HIDE", "MENU_INFO", "MENU_RELAX",
  "SCREEN_SCROLL", "TERMINALS_UTF8", "SHADOWS", "BUTTON_PASTE",
  "BUTTON_SELECTION", "COLOR", "COL_HIGH", "KBD_FLAG", "NUMBER", "STRING",
  "$accept", "rcfile", "line_list", "line", "nl", "opt_nl",
  "immediate_line", "funcbody_list", "_funcbody_list", "funcbody",
  "menubody_list", "_menubody_list", "menubody", "textbody_list",
  "_textbody_list", "textbody", "color", "high", "global_list",
  "global_flag", "func", "string_list", "string", "interactive_mode",
  "move_or_resize", "opt_flag", "flag", "opt_flag_kbd", "flag_kbd",
  "opt_flag_toggle", "flag_active", "flag_lr", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,    40,    41,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
      10,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,    43,    45,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332,   333
};
# endif

#define YYPACT_NINF -119

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-119)))

#define YYTABLE_NINF -91

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     166,   -45,   -45,   -45,   -45,   -45,   -57,   -45,   -45,   -52,
     -45,   -45,   -45,   -45,    -5,    44,   -49,  -119,   -45,   -45,
    -119,  -119,  -119,  -119,   -45,  -119,  -119,   -15,   -45,   -19,
     -45,   -49,   -45,   -37,  -119,  -119,    19,  -119,  -119,    55,
     166,  -119,    27,    42,  -119,   -15,  -119,    80,    83,   -41,
      -3,   -45,  -119,  -119,  -119,  -119,    47,   -45,  -119,   -45,
    -119,  -119,  -119,     6,     7,     8,    -5,  -119,   -27,  -119,
    -119,  -119,  -119,  -119,  -119,   -15,   -45,  -119,    19,   -45,
    -119,    14,  -119,  -119,  -119,   -45,   -45,  -119,  -119,    16,
    -119,  -119,  -119,  -119,  -119,  -119,  -119,  -119,    18,    57,
    -119,    57,  -119,    21,  -119,  -119,   102,    28,  -119,  -119,
     102,     4,  -119,  -119,    37,  -119,  -119,  -119,  -119,  -119,
    -119,   192,  -119,   192,   -15,  -119,  -119,   -15,  -119,    70,
     -45,   192,    48,    57,  -119,    71,  -119,  -119,  -119,   -23,
    -119,  -119,  -119,    50,    51,  -119,    10,  -119,   192,    87,
    -119,    57,  -119,   -45,    21,    52,  -119,  -119,  -119,  -119,
      57,  -119,  -119,    70,    12,  -119,    57,    56,  -119,    70,
    -119,  -119,    70,  -119
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    94,    64,     0,     0,
      86,    87,    66,    68,     0,    88,    89,    90,     0,     0,
       0,    94,     0,    90,     8,    58,    98,    80,    79,     0,
       2,     3,     0,     0,    57,    90,    13,    14,    16,    47,
       0,     0,    24,    25,    26,    27,     0,    59,    77,    60,
      92,    93,    51,     0,     0,     0,    28,    49,     0,    82,
      83,    84,    85,    81,    62,    96,     0,    95,    98,     0,
      67,     0,    91,    70,    71,    72,     0,    74,    76,     0,
      99,   100,   101,    61,     1,     4,     5,     6,     0,    11,
      15,    11,    17,    47,    48,    43,    18,     0,   102,   103,
      20,     0,     7,    78,     0,    55,    56,    50,    52,    54,
      97,     0,    63,     0,    90,    73,    75,    90,     9,    12,
       0,     0,     0,    11,    19,    44,    21,   104,   105,    22,
      53,    29,    30,     0,     0,    10,     0,    36,     0,     0,
      32,     0,    46,     0,    47,     0,    69,    65,    35,    37,
       0,    31,    33,    34,     0,    40,     0,     0,    23,    38,
      39,    41,    42,    45
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
    -119,  -119,  -119,    90,  -118,   -86,  -119,  -119,  -119,   -12,
    -119,  -119,    -8,    29,  -119,   -24,  -119,   -90,  -119,    75,
     -97,    15,    -1,  -119,  -119,   -25,    -9,   111,  -119,    65,
    -119,  -119
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    39,    40,    41,   129,   130,    42,   102,   149,   150,
     100,   146,   147,   134,   164,   165,   106,   107,    66,    67,
      43,    57,    44,    74,    45,    81,    82,    76,    77,    93,
     110,   139
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      46,    47,    48,    49,    50,    68,    52,    53,    89,    55,
      56,    58,    58,   132,   158,   131,   170,    78,    79,   103,
      98,    60,    61,    80,   141,    51,   142,    83,    59,    58,
      54,    87,    75,   163,   151,    60,    61,    37,    38,   104,
     118,   105,   169,    60,    61,    85,    88,   153,   172,   119,
     111,   160,   151,    60,    61,    94,   113,    68,   113,   -90,
     108,   109,    62,    84,   167,    69,   120,    96,    70,   137,
     138,    63,    64,    65,    71,   121,    72,    73,   123,    90,
      91,    92,    97,    99,   113,   125,   101,   112,   114,   115,
     116,   161,    37,    38,    37,    38,   124,   128,   126,   143,
     127,   104,   144,    12,    13,   133,    15,   135,    17,    18,
     145,    20,    21,    22,   155,    23,    24,    25,    26,   140,
      27,    28,    29,    30,    31,    32,    33,   152,    35,   148,
      95,   154,   156,   157,   168,   173,    36,   162,   159,   136,
     171,   117,    86,   122,     0,   148,     0,     0,     0,     0,
       0,     0,   166,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,     0,    37,
      38,     1,     2,     3,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,     0,    23,    24,    25,    26,     0,    27,
      28,    29,    30,    31,    32,    33,    34,    35,    12,    13,
       0,    15,     0,    17,    18,    36,    20,    21,    22,     0,
      23,    24,    25,    26,     0,    27,    28,    29,    30,    31,
      32,    33,     0,    35,     0,     0,     0,     0,     0,     0,
       0,    36,     0,     0,     0,     0,     0,     0,    37,    38,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    37,    38
};

static const yytype_int16 yycheck[] =
{
       1,     2,     3,     4,     5,    14,     7,     8,    33,    10,
      11,    12,    13,   103,     4,   101,     4,    18,    19,    60,
      45,    58,    59,    24,   121,    82,   123,    28,    13,    30,
      82,    32,    81,   151,   131,    58,    59,    82,    83,    80,
      67,    82,   160,    58,    59,    30,    83,   133,   166,    76,
      51,   148,   149,    58,    59,     0,    57,    66,    59,    82,
      63,    64,    67,    82,   154,    21,    75,    40,    24,    65,
      66,    76,    77,    78,    30,    76,    32,    33,    79,    60,
      61,    62,    40,     3,    85,    86,     3,    40,    82,    82,
      82,     4,    82,    83,    82,    83,    82,    40,    82,   124,
      82,    80,   127,    16,    17,     3,    19,    79,    21,    22,
      40,    24,    25,    26,   139,    28,    29,    30,    31,    82,
      33,    34,    35,    36,    37,    38,    39,    79,    41,   130,
      40,    60,    82,    82,    82,    79,    49,   149,   146,   110,
     164,    66,    31,    78,    -1,   146,    -1,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   164,    -1,    -1,    -1,    -1,    -1,    82,
      83,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    -1,    28,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    16,    17,
      -1,    19,    -1,    21,    22,    49,    24,    25,    26,    -1,
      28,    29,    30,    31,    -1,    33,    34,    35,    36,    37,
      38,    39,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    49,    -1,    -1,    -1,    -1,    -1,    -1,    82,    83,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    82,    83
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    28,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    49,    82,    83,    85,
      86,    87,    90,   104,   106,   108,   106,   106,   106,   106,
     106,    82,   106,   106,    82,   106,   106,   105,   106,   105,
      58,    59,    67,    76,    77,    78,   102,   103,   110,    21,
      24,    30,    32,    33,   107,    81,   111,   112,   106,   106,
     106,   109,   110,   106,    82,   105,   111,   106,    83,   109,
      60,    61,    62,   113,     0,    87,    40,    40,   109,     3,
      94,     3,    91,    60,    80,    82,   100,   101,    63,    64,
     114,   106,    40,   106,    82,    82,    82,   103,    67,    76,
     110,   106,   113,   106,    82,   106,    82,    82,    40,    88,
      89,    89,   101,     3,    97,    79,    97,    65,    66,   115,
      82,   104,   104,   109,   109,    40,    95,    96,   106,    92,
      93,   104,    79,    89,    60,   109,    82,    82,     4,    96,
     104,     4,    93,    88,    98,    99,   106,   101,    82,    88,
       4,    99,    88,    79
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    84,    85,    86,    86,    87,    87,    87,    87,    88,
      88,    89,    89,    90,    90,    90,    90,    90,    90,    90,
      90,    90,    90,    90,    90,    90,    90,    90,    90,    90,
      90,    91,    92,    92,    93,    94,    95,    95,    96,    97,
      98,    98,    99,   100,   100,   100,   100,   101,   101,   102,
     102,   103,   103,   103,   103,   103,   103,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   104,   104,   104,
     104,   104,   104,   104,   104,   104,   104,   105,   105,   106,
     106,   107,   107,   107,   107,   107,   108,   108,   108,   108,
     109,   109,   110,   110,   111,   111,   112,   112,   113,   113,
     113,   113,   114,   114,   115,   115
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     2,     2,     3,     1,     1,
       2,     0,     1,     2,     2,     3,     2,     3,     3,     4,
       3,     4,     4,     6,     2,     2,     2,     2,     2,     4,
       4,     4,     1,     2,     2,     4,     1,     2,     3,     4,
       1,     2,     2,     1,     2,     5,     3,     0,     1,     1,
       2,     1,     2,     3,     2,     2,     2,     1,     1,     2,
       2,     2,     2,     3,     1,     5,     1,     2,     1,     5,
       2,     2,     2,     3,     2,     3,     2,     1,     2,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     1,     1,     0,     1,     1,     2,     0,     1,
       1,     1,     1,     1,     1,     1
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
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
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
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

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

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

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
      yychar = yylex ();
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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

    { CallList = (yyvsp[0]._node); }

    break;

  case 3:

    { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }

    break;

  case 4:

    { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }

    break;

  case 5:

    { (yyval._node) = NULL; }

    break;

  case 6:

    { (yyval._node) = (yyvsp[-1]._node); }

    break;

  case 7:

    { set_yy_file(FindFile((yyvsp[-1]._string), NULL)); (yyval._node) = NULL; }

    break;

  case 8:

    { (yyval._node) = NULL; }

    break;

  case 9:

    {}

    break;

  case 10:

    {}

    break;

  case 11:

    {}

    break;

  case 12:

    {}

    break;

  case 13:

    { (yyval.imm) = ImmAddScreen((yyvsp[0]._string)); }

    break;

  case 14:

    { (yyval.imm) = MergeMenu((yyvsp[0]._string), NULL); }

    break;

  case 15:

    { (yyval.imm) = MergeMenu((yyvsp[-1]._string),  (yyvsp[0]._node) ); }

    break;

  case 16:

    { (yyval.imm) = MergeFunc((yyvsp[0]._string), NULL); }

    break;

  case 17:

    { (yyval.imm) = MergeFunc((yyvsp[-1]._string),  (yyvsp[0]._node) ); }

    break;

  case 18:

    { (yyval.imm) = ImmBackground((yyvsp[-1]._string), (yyvsp[0].val), NULL); }

    break;

  case 19:

    { (yyval.imm) = ImmBackground((yyvsp[-2]._string), (yyvsp[-1].val),  (yyvsp[0]._node) ); }

    break;

  case 20:

    { (yyval.imm) = ImmBorder((yyvsp[-1]._string), (yyvsp[0].val), NULL); }

    break;

  case 21:

    { (yyval.imm) = ImmBorder((yyvsp[-2]._string), (yyvsp[-1].val),  (yyvsp[0]._node) ); }

    break;

  case 22:

    { (yyval.imm) = ImmButton((yyvsp[-2].val), (yyvsp[-1]._string), (yyvsp[0].val), '+', 0); }

    break;

  case 23:

    { (yyval.imm) = ImmButton((yyvsp[-4].val), (yyvsp[-3]._string), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 24:

    { (yyval.imm) = ImmDeleteFunc((yyvsp[0]._string)); }

    break;

  case 25:

    { (yyval.imm) = ImmDeleteMenu((yyvsp[0]._string)); }

    break;

  case 26:

    { (yyval.imm) = ImmDeleteButton((yyvsp[0].val)); }

    break;

  case 27:

    { (yyval.imm) = ImmDeleteScreen((yyvsp[0]._string)); }

    break;

  case 28:

    { (yyval.imm) = ImmGlobalFlags((yyvsp[0]._node)); }

    break;

  case 29:

    { (yyval.imm) = BindKey  ((yyvsp[-2].val), (yyvsp[-1]._string), (yyvsp[0]._node)); }

    break;

  case 30:

    { (yyval.imm) = BindMouse((yyvsp[-2]._string), (yyvsp[-1]._string), (yyvsp[0]._node)); }

    break;

  case 31:

    { (yyval._node) = (yyvsp[-1]._node); }

    break;

  case 32:

    { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }

    break;

  case 33:

    { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }

    break;

  case 34:

    { (yyval._node) = (yyvsp[-1]._node); }

    break;

  case 35:

    { (yyval._node) = (yyvsp[-1]._node); }

    break;

  case 36:

    { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }

    break;

  case 37:

    { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }

    break;

  case 38:

    { (yyval._node) = MakeNodeBody((yyvsp[-2]._string), (yyvsp[-1]._node), NULL); }

    break;

  case 39:

    { (yyval._node) = (yyvsp[-1]._node); }

    break;

  case 40:

    { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }

    break;

  case 41:

    { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }

    break;

  case 42:

    { (yyval._node) = MakeNode((yyvsp[-1]._string)); }

    break;

  case 43:

    { (yyval.val) = (hwcol) (yyvsp[0].val); }

    break;

  case 44:

    { (yyval.val) = COL((yyvsp[-1].val)|(yyvsp[0].val), BLACK); }

    break;

  case 45:

    { (yyval.val) = COL((yyvsp[-4].val)|(yyvsp[-3].val), (yyvsp[-1].val)|(yyvsp[0].val)); }

    break;

  case 46:

    { (yyval.val) = COL(WHITE, (yyvsp[-1].val)|(yyvsp[0].val)); }

    break;

  case 47:

    { (yyval.val) = (hwcol)0; }

    break;

  case 48:

    { (yyval.val) = HIGH; }

    break;

  case 49:

    { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }

    break;

  case 50:

    { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }

    break;

  case 51:

    { (yyval._node) = MakeFlagNode((yyvsp[0].val),  0); }

    break;

  case 52:

    { (yyval._node) = MakeFlagNode((yyvsp[0].val), (yyvsp[-1].val)); }

    break;

  case 53:

    { (yyval._node) = MakeShadowsNode((yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 54:

    { (yyval._node) = MakeFlagNode(SHADOWS, (yyvsp[-1].val)); }

    break;

  case 55:

    { (yyval._node) = MakeFlagNode(BUTTON_PASTE, (yyvsp[0].val)); }

    break;

  case 56:

    { (yyval._node) = MakeFlagNode(BUTTON_SELECTION, (yyvsp[0].val)); }

    break;

  case 57:

    { (yyval._node) = MakeUserFunc((yyvsp[0]._string)); }

    break;

  case 58:

    { (yyval._node) = MakeBuiltinFunc((yyvsp[0].val)); }

    break;

  case 59:

    { (yyval._node) = MakeExec((yyvsp[0]._node)); }

    break;

  case 60:

    { (yyval._node) = MakeExecTty((yyvsp[0]._node)); }

    break;

  case 61:

    { (yyval._node) = MakeFlagNode((yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 62:

    { (yyval._node) = MakeFlagNode(INTERACTIVE, (yyvsp[0].val)); }

    break;

  case 63:

    { (yyval._node) = MakeModuleNode((yyvsp[-1]._string), (yyvsp[0].val)); }

    break;

  case 64:

    { (yyval._node) = MakeBuiltinFunc(MENU); }

    break;

  case 65:

    { (yyval._node) = MakeMoveResizeScroll((yyvsp[-4].val), (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 66:

    { (yyval._node) = MakeWindowNumber('+', 1); }

    break;

  case 67:

    { (yyval._node) = MakeRestartWM((yyvsp[0]._string)); }

    break;

  case 68:

    { (yyval._node) = MakeWindowNumber('-', 1); }

    break;

  case 69:

    { (yyval._node) = MakeMoveResizeScroll(SCROLL, (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 70:

    { (yyval._node) = MakeSendToScreen((yyvsp[0]._string)); }

    break;

  case 71:

    { (yyval._node) = MakeSleep((yyvsp[0].val)); }

    break;

  case 72:

    { (yyval._node) = MakeStderr((yyvsp[0]._node)); }

    break;

  case 73:

    { (yyval._node) = MakeSyntheticKey((yyvsp[-1].val), (yyvsp[0]._string)); }

    break;

  case 74:

    { (yyval._node) = MakeWait((yyvsp[0]._string)); }

    break;

  case 75:

    { (yyval._node) = MakeWindowNumber((yyvsp[-1].val), (yyvsp[0].val)); }

    break;

  case 76:

    { (yyval._node) = MakeWindow((yyvsp[0]._string)); }

    break;

  case 77:

    { (yyval._node) = AddtoStringList(NULL, (yyvsp[0]._string)); }

    break;

  case 78:

    { (yyval._node) = AddtoStringList( (yyvsp[-1]._node),  (yyvsp[0]._string)); }

    break;

  case 80:

    { (yyval._string) = toString((yyvsp[0].val)); }

    break;

  case 81:

    { (yyval.val) = SCROLL; }

    break;

  case 82:

    { (yyval.val) = MENU; }

    break;

  case 83:

    { (yyval.val) = MOVE; }

    break;

  case 84:

    { (yyval.val) = RESIZE; }

    break;

  case 85:

    { (yyval.val) = SCREEN; }

    break;

  case 86:

    { (yyval.val) = MOVE; }

    break;

  case 87:

    { (yyval.val) = MOVESCREEN; }

    break;

  case 88:

    { (yyval.val) = RESIZE; }

    break;

  case 89:

    { (yyval.val) = RESIZESCREEN; }

    break;

  case 90:

    { (yyval.val) = 0; }

    break;

  case 92:

    { (yyval.val) = '+'; }

    break;

  case 93:

    { (yyval.val) = '-'; }

    break;

  case 94:

    { (yyval.val) = 0; }

    break;

  case 97:

    { (yyval.val) = (yyvsp[-1].val) | (yyvsp[0].val); }

    break;

  case 98:

    { (yyval.val) = FL_ON; }

    break;

  case 99:

    { (yyval.val) = FL_ON; }

    break;

  case 100:

    { (yyval.val) = FL_OFF; }

    break;

  case 101:

    { (yyval.val) = FL_TOGGLE; }

    break;

  case 102:

    { (yyval.val) = FL_ACTIVE; }

    break;

  case 103:

    { (yyval.val) = FL_INACTIVE; }

    break;

  case 104:

    { (yyval.val) = FL_LEFT; }

    break;

  case 105:

    { (yyval.val) = FL_RIGHT; }

    break;



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

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}



#ifdef DEBUG_YACC
static void yyprint(FILE *file, int type, void *value) {
    if (type == NUMBER)
	fprintf (file, " %d", ((YYSTYPE *)value)->val);
    else if (type == STRING)
	fprintf (file, " \"%s\"", ((YYSTYPE *)value)->_string);
    else if (type == GLOBAL_FLAG || type == FLAG_FUNC ||
	type == EASY_FUNC || type == COLOR)
	fprintf (file, " %s", TokenName(((YYSTYPE *)value)->val));
}
#endif

static byte rcparse(byte *path) {
    return set_yy_file(path) == 0 && yyparse() == 0;
}

