/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* Written by Richard Stallman by simplifying the original so called
   ``semantic'' parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
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
     SHADOWS = 325,
     BUTTON_PASTE = 326,
     BUTTON_SELECTION = 327,
     COLOR = 328,
     COL_HIGH = 329,
     KBD_FLAG = 330,
     NUMBER = 331,
     STRING = 332
   };
#endif
#define ADDSCREEN 258
#define ADDTOMENU 259
#define ADDTOFUNC 260
#define BACKGROUND 261
#define BORDER 262
#define BUTTON 263
#define DELETEFUNC 264
#define DELETEMENU 265
#define DELETEBUTTON 266
#define DELETESCREEN 267
#define READ 268
#define EXEC 269
#define EXECTTY 270
#define GLOBALFLAGS 271
#define INTERACTIVE 272
#define KEY 273
#define MENU 274
#define MODULE 275
#define MOUSE 276
#define MOVE 277
#define MOVESCREEN 278
#define NEXT 279
#define NOP 280
#define PREV 281
#define RESTART 282
#define RESIZE 283
#define RESIZESCREEN 284
#define SCREEN 285
#define SCROLL 286
#define SENDTOSCREEN 287
#define SLEEP 288
#define STDERR 289
#define SYNTHETICKEY 290
#define WAIT 291
#define WINDOW 292
#define EASY_FUNC 293
#define BEEP 294
#define CENTER 295
#define CLOSE 296
#define KILL 297
#define QUIT 298
#define REFRESH 299
#define WINDOWLIST 300
#define FLAG_FUNC 301
#define FOCUS 302
#define MAXIMIZE 303
#define FULLSCREEN 304
#define LOWER 305
#define RAISE 306
#define RAISELOWER 307
#define ROLL 308
#define USERFUNC 309
#define FL_ON 310
#define FL_OFF 311
#define FL_TOGGLE 312
#define FL_ACTIVE 313
#define FL_INACTIVE 314
#define FL_LEFT 315
#define FL_RIGHT 316
#define GLOBAL_FLAG 317
#define ALTFONT 318
#define CURSOR_ALWAYS 319
#define BLINK 320
#define MENU_HIDE 321
#define MENU_INFO 322
#define MENU_RELAX 323
#define SCREEN_SCROLL 324
#define SHADOWS 325
#define BUTTON_PASTE 326
#define BUTTON_SELECTION 327
#define COLOR 328
#define COL_HIGH 329
#define KBD_FLAG 330
#define NUMBER 331
#define STRING 332




/* Copy the first part of user declarations.  */



/*
 * C declarations
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <Tw/Twkeys.h>
#ifdef CONF__UNICODE
# include <Tutf/Tutf.h>
#endif
    
#include "twin.h"
#include "methods.h"
#include "data.h"
#include "main.h"
#include "printk.h"
#include "util.h"
#include "hw.h"
#include "extreg.h"

#include "rctypes.h"

#include "wm.h"
#include "rcrun.h"

#ifdef CONF_THIS_MODULE
# include "version.h"
#endif

#include "rcparse.h"


/* also put here the CONF_* and DEBUG_* used in rcparse.h so that MkDep catches them */
#if defined(DEBUG_RC) || defined(DEBUG_FORK)
#endif

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

#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    node	 _node;
} YYSTYPE;
/* Line 191 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 214 of yacc.c.  */


#if ! defined (yyoverflow) || YYERROR_VERBOSE

# ifndef YYFREE
#  define YYFREE free
# endif
# ifndef YYMALLOC
#  define YYMALLOC malloc
# endif

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   define YYSTACK_ALLOC alloca
#  endif
# else
#  if defined (alloca) || defined (_ALLOCA_H)
#   define YYSTACK_ALLOC alloca
#  else
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning. */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
# else
#  if defined (__STDC__) || defined (__cplusplus)
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   define YYSIZE_T size_t
#  endif
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
# endif
#endif /* ! defined (yyoverflow) || YYERROR_VERBOSE */


#if (! defined (yyoverflow) \
     && (! defined (__cplusplus) \
	 || (defined (YYSTYPE_IS_TRIVIAL) && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  short int yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (short int) + sizeof (YYSTYPE))			\
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined (__GNUC__) && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  register YYSIZE_T yyi;		\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (0)
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (0)

#endif

#if defined (__STDC__) || defined (__cplusplus)
   typedef signed char yysigned_char;
#else
   typedef short int yysigned_char;
#endif

/* YYFINAL -- State number of the termination state. */
#define YYFINAL  94
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   273

/* YYNTOKENS -- Number of terminals. */
#define YYNTOKENS  83
/* YYNNTS -- Number of nonterminals. */
#define YYNNTS  32
/* YYNRULES -- Number of rules. */
#define YYNRULES  105
/* YYNRULES -- Number of states. */
#define YYNSTATES  174

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   332

#define YYTRANSLATE(YYX) 						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const unsigned char yytranslate[] =
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
      80,    81,    82
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const unsigned short int yyprhs[] =
{
       0,     0,     3,     5,     7,    10,    13,    16,    20,    22,
      24,    27,    28,    30,    33,    36,    40,    43,    47,    51,
      56,    60,    65,    70,    77,    80,    83,    86,    89,    92,
      97,   102,   107,   109,   112,   115,   120,   122,   125,   129,
     134,   136,   139,   142,   144,   147,   153,   157,   158,   160,
     162,   165,   167,   170,   174,   177,   180,   183,   185,   187,
     190,   193,   196,   199,   203,   205,   211,   213,   216,   218,
     224,   227,   230,   233,   237,   240,   244,   247,   249,   252,
     254,   256,   258,   260,   262,   264,   266,   268,   270,   272,
     274,   275,   277,   279,   281,   282,   284,   286,   289,   290,
     292,   294,   296,   298,   300,   302
};

/* YYRHS -- A `-1'-separated list of the rules' RHS. */
static const yysigned_char yyrhs[] =
{
      84,     0,    -1,    85,    -1,    86,    -1,    85,    86,    -1,
      89,    40,    -1,   103,    40,    -1,    15,   105,    40,    -1,
      40,    -1,    40,    -1,    87,    40,    -1,    -1,    87,    -1,
       5,   105,    -1,     6,   105,    -1,     6,   105,    93,    -1,
       7,   105,    -1,     7,   105,    90,    -1,     8,   105,    99,
      -1,     8,   105,    99,    96,    -1,     9,   105,   113,    -1,
       9,   105,   113,    96,    -1,    10,    81,   105,   114,    -1,
      10,    81,   105,   114,   108,    81,    -1,    11,   105,    -1,
      12,   105,    -1,    13,    81,    -1,    14,   105,    -1,    18,
     101,    -1,    20,   110,   105,   103,    -1,    23,   105,   105,
     103,    -1,     3,    88,    91,     4,    -1,    92,    -1,    91,
      92,    -1,   103,    87,    -1,     3,    88,    94,     4,    -1,
      95,    -1,    94,    95,    -1,   105,   103,    87,    -1,     3,
      88,    97,     4,    -1,    98,    -1,    97,    98,    -1,   105,
      87,    -1,    81,    -1,   100,    78,    -1,   100,    78,    60,
     100,    78,    -1,    60,   100,    78,    -1,    -1,    79,    -1,
     102,    -1,   101,   102,    -1,    67,    -1,   109,    67,    -1,
      75,    81,    81,    -1,   109,    75,    -1,    76,    81,    -1,
      77,    81,    -1,   105,    -1,    41,    -1,    16,   104,    -1,
      17,   104,    -1,    49,   112,    -1,    19,   106,    -1,    22,
     105,   112,    -1,    21,    -1,   107,   108,    81,   108,    81,
      -1,    26,    -1,    29,   105,    -1,    28,    -1,    33,   108,
      81,   108,    81,    -1,    34,   105,    -1,    35,    81,    -1,
      36,   104,    -1,    37,   110,   105,    -1,    38,   105,    -1,
      39,   108,    81,    -1,    39,    82,    -1,   105,    -1,   104,
     105,    -1,    82,    -1,    81,    -1,    33,    -1,    21,    -1,
      24,    -1,    30,    -1,    32,    -1,    24,    -1,    25,    -1,
      30,    -1,    31,    -1,    -1,   109,    -1,    58,    -1,    59,
      -1,    -1,   111,    -1,    80,    -1,    80,   109,    -1,    -1,
      60,    -1,    61,    -1,    62,    -1,    63,    -1,    64,    -1,
      65,    -1,    66,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const unsigned short int yyrline[] =
{
       0,   162,   162,   165,   166,   169,   170,   171,   172,   175,
     176,   179,   180,   183,   184,   185,   186,   187,   188,   189,
     190,   191,   192,   193,   194,   195,   196,   197,   198,   199,
     200,   203,   206,   207,   210,   213,   216,   217,   220,   223,
     226,   227,   230,   233,   234,   235,   236,   239,   240,   243,
     244,   247,   248,   249,   250,   251,   252,   255,   256,   257,
     258,   259,   260,   262,   263,   264,   266,   267,   268,   269,
     270,   271,   272,   273,   274,   275,   276,   279,   280,   283,
     284,   287,   288,   289,   290,   291,   294,   295,   296,   297,
     300,   301,   304,   305,   308,   309,   312,   313,   316,   317,
     318,   319,   322,   323,   326,   327
};
#endif

#if YYDEBUG || YYERROR_VERBOSE
/* YYTNME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals. */
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
  "SCREEN_SCROLL", "SHADOWS", "BUTTON_PASTE", "BUTTON_SELECTION", "COLOR",
  "COL_HIGH", "KBD_FLAG", "NUMBER", "STRING", "$accept", "rcfile",
  "line_list", "line", "nl", "opt_nl", "immediate_line", "funcbody_list",
  "_funcbody_list", "funcbody", "menubody_list", "_menubody_list",
  "menubody", "textbody_list", "_textbody_list", "textbody", "color",
  "high", "global_list", "global_flag", "func", "string_list", "string",
  "interactive_mode", "move_or_resize", "opt_flag", "flag", "opt_flag_kbd",
  "flag_kbd", "opt_flag_toggle", "flag_active", "flag_lr", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const unsigned short int yytoknum[] =
{
       0,   256,   257,    40,    41,   258,   259,   260,   261,   262,
     263,   264,   265,   266,   267,   268,   269,   270,   271,   272,
     273,   274,   275,   276,   277,   278,   279,   280,   281,   282,
     283,   284,   285,   286,   287,   288,   289,   290,   291,   292,
      10,   293,   294,   295,   296,   297,   298,   299,   300,   301,
     302,   303,   304,   305,   306,   307,   308,   309,    43,    45,
     310,   311,   312,   313,   314,   315,   316,   317,   318,   319,
     320,   321,   322,   323,   324,   325,   326,   327,   328,   329,
     330,   331,   332
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const unsigned char yyr1[] =
{
       0,    83,    84,    85,    85,    86,    86,    86,    86,    87,
      87,    88,    88,    89,    89,    89,    89,    89,    89,    89,
      89,    89,    89,    89,    89,    89,    89,    89,    89,    89,
      89,    90,    91,    91,    92,    93,    94,    94,    95,    96,
      97,    97,    98,    99,    99,    99,    99,   100,   100,   101,
     101,   102,   102,   102,   102,   102,   102,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   103,   103,   103,
     103,   103,   103,   103,   103,   103,   103,   104,   104,   105,
     105,   106,   106,   106,   106,   106,   107,   107,   107,   107,
     108,   108,   109,   109,   110,   110,   111,   111,   112,   112,
     112,   112,   113,   113,   114,   114
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const unsigned char yyr2[] =
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

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const unsigned char yydefact[] =
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

/* YYDEFGOTO[NTERM-NUM]. */
static const short int yydefgoto[] =
{
      -1,    39,    40,    41,   129,   130,    42,   102,   149,   150,
     100,   146,   147,   134,   164,   165,   106,   107,    66,    67,
      43,    57,    44,    74,    45,    81,    82,    76,    77,    93,
     110,   139
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -124
static const short int yypact[] =
{
     165,   -28,   -28,   -28,   -28,   -28,   -51,   -28,   -28,   -46,
     -28,   -28,   -28,   -28,   -10,    39,   -39,  -124,   -28,   -28,
    -124,  -124,  -124,  -124,   -28,  -124,  -124,     3,   -28,   -42,
     -28,   -39,   -28,   -37,  -124,  -124,    19,  -124,  -124,    47,
     165,  -124,    30,    36,  -124,     3,  -124,    56,    85,   -41,
      10,   -28,  -124,  -124,  -124,  -124,    49,   -28,  -124,   -28,
    -124,  -124,  -124,     9,    13,    14,   -10,  -124,   -31,  -124,
    -124,  -124,  -124,  -124,  -124,     3,   -28,  -124,    19,   -28,
    -124,    15,  -124,  -124,  -124,   -28,   -28,  -124,  -124,    16,
    -124,  -124,  -124,  -124,  -124,  -124,  -124,  -124,    17,    60,
    -124,    60,  -124,    22,  -124,  -124,   102,    29,  -124,  -124,
     102,    21,  -124,  -124,    38,  -124,  -124,  -124,  -124,  -124,
    -124,   191,  -124,   191,     3,  -124,  -124,     3,  -124,    70,
     -28,   191,    52,    60,  -124,    67,  -124,  -124,  -124,   -26,
    -124,  -124,  -124,    50,    51,  -124,     1,  -124,   191,    87,
    -124,    60,  -124,   -28,    22,    53,  -124,  -124,  -124,  -124,
      60,  -124,  -124,    70,    11,  -124,    60,    55,  -124,    70,
    -124,  -124,    70,  -124
};

/* YYPGOTO[NTERM-NUM].  */
static const yysigned_char yypgoto[] =
{
    -124,  -124,  -124,    95,  -123,   -87,  -124,  -124,  -124,   -12,
    -124,  -124,    -8,    31,  -124,   -24,  -124,   -90,  -124,    73,
     -97,    12,    -1,  -124,  -124,   -25,     2,   111,  -124,    65,
    -124,  -124
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -91
static const short int yytable[] =
{
      46,    47,    48,    49,    50,   158,    52,    53,    89,    55,
      56,    58,    58,   132,   131,   170,    68,    78,    79,   103,
      98,    60,    61,    80,   141,    59,   142,    83,   163,    58,
      51,    87,    60,    61,   151,    54,   118,   169,   104,    84,
     105,    75,    85,   172,   119,    88,   153,    94,    60,    61,
     111,   160,   151,    37,    38,   -90,   113,    62,   113,    99,
      69,    60,    61,    70,   167,    63,    64,    65,    68,    71,
      96,    72,    73,   108,   109,   121,    97,   120,   123,    90,
      91,    92,    37,    38,   113,   125,   137,   138,   101,   112,
     114,   161,    37,    38,   115,   116,   124,   126,   127,   143,
     128,   104,   144,    12,    13,   133,    15,   135,    17,    18,
     145,    20,    21,    22,   155,    23,    24,    25,    26,   140,
      27,    28,    29,    30,    31,    32,    33,   154,    35,   148,
     152,   156,   157,   173,   168,    95,    36,   162,   159,   117,
     171,   136,    86,   122,     0,   148,     0,     0,     0,     0,
       0,     0,   166,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,    37,    38,
       1,     2,     3,     4,     5,     6,     7,     8,     9,    10,
      11,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,     0,    23,    24,    25,    26,     0,    27,    28,
      29,    30,    31,    32,    33,    34,    35,    12,    13,     0,
      15,     0,    17,    18,    36,    20,    21,    22,     0,    23,
      24,    25,    26,     0,    27,    28,    29,    30,    31,    32,
      33,     0,    35,     0,     0,     0,     0,     0,     0,     0,
      36,     0,     0,     0,     0,     0,    37,    38,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    37,    38
};

static const short int yycheck[] =
{
       1,     2,     3,     4,     5,     4,     7,     8,    33,    10,
      11,    12,    13,   103,   101,     4,    14,    18,    19,    60,
      45,    58,    59,    24,   121,    13,   123,    28,   151,    30,
      81,    32,    58,    59,   131,    81,    67,   160,    79,    81,
      81,    80,    30,   166,    75,    82,   133,     0,    58,    59,
      51,   148,   149,    81,    82,    81,    57,    67,    59,     3,
      21,    58,    59,    24,   154,    75,    76,    77,    66,    30,
      40,    32,    33,    63,    64,    76,    40,    75,    79,    60,
      61,    62,    81,    82,    85,    86,    65,    66,     3,    40,
      81,     4,    81,    82,    81,    81,    81,    81,    81,   124,
      40,    79,   127,    16,    17,     3,    19,    78,    21,    22,
      40,    24,    25,    26,   139,    28,    29,    30,    31,    81,
      33,    34,    35,    36,    37,    38,    39,    60,    41,   130,
      78,    81,    81,    78,    81,    40,    49,   149,   146,    66,
     164,   110,    31,    78,    -1,   146,    -1,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   164,    -1,    -1,    -1,    -1,    81,    82,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    -1,    28,    29,    30,    31,    -1,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    16,    17,    -1,
      19,    -1,    21,    22,    49,    24,    25,    26,    -1,    28,
      29,    30,    31,    -1,    33,    34,    35,    36,    37,    38,
      39,    -1,    41,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      49,    -1,    -1,    -1,    -1,    -1,    81,    82,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    81,    82
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const unsigned char yystos[] =
{
       0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    28,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    49,    81,    82,    84,
      85,    86,    89,   103,   105,   107,   105,   105,   105,   105,
     105,    81,   105,   105,    81,   105,   105,   104,   105,   104,
      58,    59,    67,    75,    76,    77,   101,   102,   109,    21,
      24,    30,    32,    33,   106,    80,   110,   111,   105,   105,
     105,   108,   109,   105,    81,   104,   110,   105,    82,   108,
      60,    61,    62,   112,     0,    86,    40,    40,   108,     3,
      93,     3,    90,    60,    79,    81,    99,   100,    63,    64,
     113,   105,    40,   105,    81,    81,    81,   102,    67,    75,
     109,   105,   112,   105,    81,   105,    81,    81,    40,    87,
      88,    88,   100,     3,    96,    78,    96,    65,    66,   114,
      81,   103,   103,   108,   108,    40,    94,    95,   105,    91,
      92,   103,    78,    88,    60,   108,    81,    81,     4,    95,
     103,     4,    92,    87,    97,    98,   105,   100,    81,    87,
       4,    98,    87,    78
};

#if ! defined (YYSIZE_T) && defined (__SIZE_TYPE__)
# define YYSIZE_T __SIZE_TYPE__
#endif
#if ! defined (YYSIZE_T) && defined (size_t)
# define YYSIZE_T size_t
#endif
#if ! defined (YYSIZE_T)
# if defined (__STDC__) || defined (__cplusplus)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# endif
#endif
#if ! defined (YYSIZE_T)
# define YYSIZE_T unsigned int
#endif

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
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { 								\
      yyerror ("syntax error: cannot back up");\
      YYERROR;							\
    }								\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

/* YYLLOC_DEFAULT -- Compute the default location (before the actions
   are run).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)		\
   ((Current).first_line   = (Rhs)[1].first_line,	\
    (Current).first_column = (Rhs)[1].first_column,	\
    (Current).last_line    = (Rhs)[N].last_line,	\
    (Current).last_column  = (Rhs)[N].last_column)
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
} while (0)

# define YYDSYMPRINT(Args)			\
do {						\
  if (yydebug)					\
    yysymprint Args;				\
} while (0)

# define YYDSYMPRINTF(Title, Token, Value, Location)		\
do {								\
  if (yydebug)							\
    {								\
      YYFPRINTF (stderr, "%s ", Title);				\
      yysymprint (stderr, 					\
                  Token, Value);	\
      YYFPRINTF (stderr, "\n");					\
    }								\
} while (0)

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_stack_print (short int *bottom, short int *top)
#else
static void
yy_stack_print (bottom, top)
    short int *bottom;
    short int *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (/* Nothing. */; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yy_reduce_print (int yyrule)
#else
static void
yy_reduce_print (yyrule)
    int yyrule;
#endif
{
  int yyi;
  unsigned int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %u), ",
             yyrule - 1, yylno);
  /* Print the symbols being reduced, and their result.  */
  for (yyi = yyprhs[yyrule]; 0 <= yyrhs[yyi]; yyi++)
    YYFPRINTF (stderr, "%s ", yytname [yyrhs[yyi]]);
  YYFPRINTF (stderr, "-> %s\n", yytname [yyr1[yyrule]]);
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (Rule);		\
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YYDSYMPRINT(Args)
# define YYDSYMPRINTF(Title, Token, Value, Location)
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
   SIZE_MAX < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#if defined (YYMAXDEPTH) && YYMAXDEPTH == 0
# undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined (__GLIBC__) && defined (_STRING_H)
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
#   if defined (__STDC__) || defined (__cplusplus)
yystrlen (const char *yystr)
#   else
yystrlen (yystr)
     const char *yystr;
#   endif
{
  register const char *yys = yystr;

  while (*yys++ != '\0')
    continue;

  return yys - yystr - 1;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined (__GLIBC__) && defined (_STRING_H) && defined (_GNU_SOURCE)
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
#   if defined (__STDC__) || defined (__cplusplus)
yystpcpy (char *yydest, const char *yysrc)
#   else
yystpcpy (yydest, yysrc)
     char *yydest;
     const char *yysrc;
#   endif
{
  register char *yyd = yydest;
  register const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

#endif /* !YYERROR_VERBOSE */



#if YYDEBUG
/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yysymprint (FILE *yyoutput, int yytype, YYSTYPE *yyvaluep)
#else
static void
yysymprint (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  if (yytype < YYNTOKENS)
    {
      YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
# ifdef YYPRINT
      YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
    }
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  switch (yytype)
    {
      default:
        break;
    }
  YYFPRINTF (yyoutput, ")");
}

#endif /* ! YYDEBUG */
/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

#if defined (__STDC__) || defined (__cplusplus)
static void
yydestruct (int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yytype, yyvaluep)
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  /* Pacify ``unused variable'' warnings.  */
  (void) yyvaluep;

  switch (yytype)
    {

      default:
        break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM);
# else
int yyparse ();
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
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
# if defined (__STDC__) || defined (__cplusplus)
int yyparse (void *YYPARSE_PARAM)
# else
int yyparse (YYPARSE_PARAM)
  void *YYPARSE_PARAM;
# endif
#else /* ! YYPARSE_PARAM */
#if defined (__STDC__) || defined (__cplusplus)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  
  register int yystate;
  register int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  short int yyssa[YYINITDEPTH];
  short int *yyss = yyssa;
  register short int *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  register YYSTYPE *yyvsp;



#define YYPOPSTACK   (yyvsp--, yyssp--)

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* When reducing, the number of symbols on the RHS of the reduced
     rule.  */
  int yylen;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

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
     have just been pushed. so pushing a state here evens the stacks.
     */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack. Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	short int *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow ("parser stack overflow",
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyoverflowlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyoverflowlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	short int *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyoverflowlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

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

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

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
      YYDSYMPRINTF ("Next token is", yytoken, &yylval, &yylloc);
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */
  YYDPRINTF ((stderr, "Shifting token %s, ", yytname[yytoken]));

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;


  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  yystate = yyn;
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

    { CallList = yyvsp[0]._node; ;}
    break;

  case 3:

    { yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;}
    break;

  case 4:

    { yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;}
    break;

  case 5:

    { yyval._node = NULL; ;}
    break;

  case 6:

    { yyval._node = yyvsp[-1]._node; ;}
    break;

  case 7:

    { set_yy_file(FindFile(yyvsp[-1]._string, NULL)); yyval._node = NULL; ;}
    break;

  case 8:

    { yyval._node = NULL; ;}
    break;

  case 9:

    {;}
    break;

  case 10:

    {;}
    break;

  case 11:

    {;}
    break;

  case 12:

    {;}
    break;

  case 13:

    { yyval.imm = ImmAddScreen(yyvsp[0]._string); ;}
    break;

  case 14:

    { yyval.imm = MergeMenu(yyvsp[0]._string, NULL); ;}
    break;

  case 15:

    { yyval.imm = MergeMenu(yyvsp[-1]._string,  yyvsp[0]._node ); ;}
    break;

  case 16:

    { yyval.imm = MergeFunc(yyvsp[0]._string, NULL); ;}
    break;

  case 17:

    { yyval.imm = MergeFunc(yyvsp[-1]._string,  yyvsp[0]._node ); ;}
    break;

  case 18:

    { yyval.imm = ImmBackground(yyvsp[-1]._string, yyvsp[0].val, NULL); ;}
    break;

  case 19:

    { yyval.imm = ImmBackground(yyvsp[-2]._string, yyvsp[-1].val,  yyvsp[0]._node ); ;}
    break;

  case 20:

    { yyval.imm = ImmBorder(yyvsp[-1]._string, yyvsp[0].val, NULL); ;}
    break;

  case 21:

    { yyval.imm = ImmBorder(yyvsp[-2]._string, yyvsp[-1].val,  yyvsp[0]._node ); ;}
    break;

  case 22:

    { yyval.imm = ImmButton(yyvsp[-2].val, yyvsp[-1]._string, yyvsp[0].val, '+', 0); ;}
    break;

  case 23:

    { yyval.imm = ImmButton(yyvsp[-4].val, yyvsp[-3]._string, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 24:

    { yyval.imm = ImmDeleteFunc(yyvsp[0]._string); ;}
    break;

  case 25:

    { yyval.imm = ImmDeleteMenu(yyvsp[0]._string); ;}
    break;

  case 26:

    { yyval.imm = ImmDeleteButton(yyvsp[0].val); ;}
    break;

  case 27:

    { yyval.imm = ImmDeleteScreen(yyvsp[0]._string); ;}
    break;

  case 28:

    { yyval.imm = ImmGlobalFlags(yyvsp[0]._node); ;}
    break;

  case 29:

    { yyval.imm = BindKey  (yyvsp[-2].val, yyvsp[-1]._string, yyvsp[0]._node); ;}
    break;

  case 30:

    { yyval.imm = BindMouse(yyvsp[-2]._string, yyvsp[-1]._string, yyvsp[0]._node); ;}
    break;

  case 31:

    { yyval._node = yyvsp[-1]._node; ;}
    break;

  case 32:

    { yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;}
    break;

  case 33:

    { yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;}
    break;

  case 34:

    { yyval._node = yyvsp[-1]._node; ;}
    break;

  case 35:

    { yyval._node = yyvsp[-1]._node; ;}
    break;

  case 36:

    { yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;}
    break;

  case 37:

    { yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;}
    break;

  case 38:

    { yyval._node = MakeNodeBody(yyvsp[-2]._string, yyvsp[-1]._node, NULL); ;}
    break;

  case 39:

    { yyval._node = yyvsp[-1]._node; ;}
    break;

  case 40:

    { yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;}
    break;

  case 41:

    { yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;}
    break;

  case 42:

    { yyval._node = MakeNode(yyvsp[-1]._string); ;}
    break;

  case 43:

    { yyval.val = (hwcol) yyvsp[0].val; ;}
    break;

  case 44:

    { yyval.val = COL(yyvsp[-1].val|yyvsp[0].val, BLACK); ;}
    break;

  case 45:

    { yyval.val = COL(yyvsp[-4].val|yyvsp[-3].val, yyvsp[-1].val|yyvsp[0].val); ;}
    break;

  case 46:

    { yyval.val = COL(WHITE, yyvsp[-1].val|yyvsp[0].val); ;}
    break;

  case 47:

    { yyval.val = (hwcol)0; ;}
    break;

  case 48:

    { yyval.val = HIGH; ;}
    break;

  case 49:

    { yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;}
    break;

  case 50:

    { yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;}
    break;

  case 51:

    { yyval._node = MakeFlagNode(yyvsp[0].val,  0); ;}
    break;

  case 52:

    { yyval._node = MakeFlagNode(yyvsp[0].val, yyvsp[-1].val); ;}
    break;

  case 53:

    { yyval._node = MakeShadowsNode(yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 54:

    { yyval._node = MakeFlagNode(SHADOWS, yyvsp[-1].val); ;}
    break;

  case 55:

    { yyval._node = MakeFlagNode(BUTTON_PASTE, yyvsp[0].val); ;}
    break;

  case 56:

    { yyval._node = MakeFlagNode(BUTTON_SELECTION, yyvsp[0].val); ;}
    break;

  case 57:

    { yyval._node = MakeUserFunc(yyvsp[0]._string); ;}
    break;

  case 58:

    { yyval._node = MakeBuiltinFunc(yyvsp[0].val); ;}
    break;

  case 59:

    { yyval._node = MakeExec(yyvsp[0]._node); ;}
    break;

  case 60:

    { yyval._node = MakeExecTty(yyvsp[0]._node); ;}
    break;

  case 61:

    { yyval._node = MakeFlagNode(yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 62:

    { yyval._node = MakeFlagNode(INTERACTIVE, yyvsp[0].val); ;}
    break;

  case 63:

    { yyval._node = MakeModuleNode(yyvsp[-1]._string, yyvsp[0].val); ;}
    break;

  case 64:

    { yyval._node = MakeBuiltinFunc(MENU); ;}
    break;

  case 65:

    { yyval._node = MakeMoveResizeScroll(yyvsp[-4].val, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 66:

    { yyval._node = MakeWindowNumber('+', 1); ;}
    break;

  case 67:

    { yyval._node = MakeRestartWM(yyvsp[0]._string); ;}
    break;

  case 68:

    { yyval._node = MakeWindowNumber('-', 1); ;}
    break;

  case 69:

    { yyval._node = MakeMoveResizeScroll(SCROLL, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 70:

    { yyval._node = MakeSendToScreen(yyvsp[0]._string); ;}
    break;

  case 71:

    { yyval._node = MakeSleep(yyvsp[0].val); ;}
    break;

  case 72:

    { yyval._node = MakeStderr(yyvsp[0]._node); ;}
    break;

  case 73:

    { yyval._node = MakeSyntheticKey(yyvsp[-1].val, yyvsp[0]._string); ;}
    break;

  case 74:

    { yyval._node = MakeWait(yyvsp[0]._string); ;}
    break;

  case 75:

    { yyval._node = MakeWindowNumber(yyvsp[-1].val, yyvsp[0].val); ;}
    break;

  case 76:

    { yyval._node = MakeWindow(yyvsp[0]._string); ;}
    break;

  case 77:

    { yyval._node = AddtoStringList(NULL, yyvsp[0]._string); ;}
    break;

  case 78:

    { yyval._node = AddtoStringList( yyvsp[-1]._node,  yyvsp[0]._string); ;}
    break;

  case 80:

    { yyval._string = toString(yyvsp[0].val); ;}
    break;

  case 81:

    { yyval.val = SCROLL; ;}
    break;

  case 82:

    { yyval.val = MENU; ;}
    break;

  case 83:

    { yyval.val = MOVE; ;}
    break;

  case 84:

    { yyval.val = RESIZE; ;}
    break;

  case 85:

    { yyval.val = SCREEN; ;}
    break;

  case 86:

    { yyval.val = MOVE; ;}
    break;

  case 87:

    { yyval.val = MOVESCREEN; ;}
    break;

  case 88:

    { yyval.val = RESIZE; ;}
    break;

  case 89:

    { yyval.val = RESIZESCREEN; ;}
    break;

  case 90:

    { yyval.val = 0; ;}
    break;

  case 92:

    { yyval.val = '+'; ;}
    break;

  case 93:

    { yyval.val = '-'; ;}
    break;

  case 94:

    { yyval.val = 0; ;}
    break;

  case 97:

    { yyval.val = yyvsp[-1].val | yyvsp[0].val; ;}
    break;

  case 98:

    { yyval.val = FL_ON; ;}
    break;

  case 99:

    { yyval.val = FL_ON; ;}
    break;

  case 100:

    { yyval.val = FL_OFF; ;}
    break;

  case 101:

    { yyval.val = FL_TOGGLE; ;}
    break;

  case 102:

    { yyval.val = FL_ACTIVE; ;}
    break;

  case 103:

    { yyval.val = FL_INACTIVE; ;}
    break;

  case 104:

    { yyval.val = FL_LEFT; ;}
    break;

  case 105:

    { yyval.val = FL_RIGHT; ;}
    break;


    }

/* Line 1010 of yacc.c.  */


  yyvsp -= yylen;
  yyssp -= yylen;


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
#if YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (YYPACT_NINF < yyn && yyn < YYLAST)
	{
	  YYSIZE_T yysize = 0;
	  int yytype = YYTRANSLATE (yychar);
	  const char* yyprefix;
	  char *yymsg;
	  int yyx;

	  /* Start YYX at -YYN if negative to avoid negative indexes in
	     YYCHECK.  */
	  int yyxbegin = yyn < 0 ? -yyn : 0;

	  /* Stay within bounds of both yycheck and yytname.  */
	  int yychecklim = YYLAST - yyn;
	  int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
	  int yycount = 0;

	  yyprefix = ", expecting ";
	  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	      {
		yysize += yystrlen (yyprefix) + yystrlen (yytname [yyx]);
		yycount += 1;
		if (yycount == 5)
		  {
		    yysize = 0;
		    break;
		  }
	      }
	  yysize += (sizeof ("syntax error, unexpected ")
		     + yystrlen (yytname[yytype]));
	  yymsg = (char *) YYSTACK_ALLOC (yysize);
	  if (yymsg != 0)
	    {
	      char *yyp = yystpcpy (yymsg, "syntax error, unexpected ");
	      yyp = yystpcpy (yyp, yytname[yytype]);

	      if (yycount < 5)
		{
		  yyprefix = ", expecting ";
		  for (yyx = yyxbegin; yyx < yyxend; ++yyx)
		    if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
		      {
			yyp = yystpcpy (yyp, yyprefix);
			yyp = yystpcpy (yyp, yytname[yyx]);
			yyprefix = " or ";
		      }
		}
	      yyerror (yymsg);
	      YYSTACK_FREE (yymsg);
	    }
	  else
	    yyerror ("syntax error; also virtual memory exhausted");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror ("syntax error");
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* If at end of input, pop the error token,
	     then the rest of the stack, then return failure.  */
	  if (yychar == YYEOF)
	     for (;;)
	       {
		 YYPOPSTACK;
		 if (yyssp == yyss)
		   YYABORT;
		 YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
		 yydestruct (yystos[*yyssp], yyvsp);
	       }
        }
      else
	{
	  YYDSYMPRINTF ("Error: discarding", yytoken, &yylval, &yylloc);
	  yydestruct (yytoken, &yylval);
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

#ifdef __GNUC__
  /* Pacify GCC when the user code never invokes YYERROR and the label
     yyerrorlab therefore never appears in user code.  */
  if (0)
     goto yyerrorlab;
#endif

  yyvsp -= yylen;
  yyssp -= yylen;
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

      YYDSYMPRINTF ("Error: popping", yystos[*yyssp], yyvsp, yylsp);
      yydestruct (yystos[yystate], yyvsp);
      YYPOPSTACK;
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  YYDPRINTF ((stderr, "Shifting error token, "));

  *++yyvsp = yylval;


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

#ifndef yyoverflow
/*----------------------------------------------.
| yyoverflowlab -- parser overflow comes here.  |
`----------------------------------------------*/
yyoverflowlab:
  yyerror ("parser stack overflow");
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
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


