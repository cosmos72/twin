
/*  A Bison parser, made from rcparse.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ADDSCREEN	257
#define	ADDTOMENU	258
#define	ADDTOFUNC	259
#define	BACKGROUND	260
#define	BORDER	261
#define	BUTTON	262
#define	DELETEFUNC	263
#define	DELETEMENU	264
#define	DELETEBUTTON	265
#define	DELETESCREEN	266
#define	READ	267
#define	EXEC	268
#define	EXECTTY	269
#define	GLOBALFLAGS	270
#define	INTERACTIVE	271
#define	KEY	272
#define	MENU	273
#define	MODULE	274
#define	MOUSE	275
#define	MOVE	276
#define	MOVESCREEN	277
#define	NEXT	278
#define	NOP	279
#define	PREV	280
#define	RESTART	281
#define	RESIZE	282
#define	RESIZESCREEN	283
#define	SCREEN	284
#define	SCROLL	285
#define	SENDTOSCREEN	286
#define	SLEEP	287
#define	STDERR	288
#define	SYNTHETICKEY	289
#define	WAIT	290
#define	WINDOW	291
#define	EASY_FUNC	292
#define	BEEP	293
#define	CENTER	294
#define	CLOSE	295
#define	KILL	296
#define	QUIT	297
#define	REFRESH	298
#define	WINDOWLIST	299
#define	FLAG_FUNC	300
#define	FOCUS	301
#define	MAXIMIZE	302
#define	FULLSCREEN	303
#define	LOWER	304
#define	RAISE	305
#define	RAISELOWER	306
#define	ROLL	307
#define	USERFUNC	308
#define	FL_ON	309
#define	FL_OFF	310
#define	FL_TOGGLE	311
#define	FL_ACTIVE	312
#define	FL_INACTIVE	313
#define	FL_LEFT	314
#define	FL_RIGHT	315
#define	GLOBAL_FLAG	316
#define	ALTFONT	317
#define	ALWAYSCURSOR	318
#define	BLINK	319
#define	EDGESCROLL	320
#define	HIDEMENU	321
#define	MENUINFO	322
#define	SHADOWS	323
#define	PASTEBUTTON	324
#define	SELECTIONBUTTON	325
#define	COLOR	326
#define	COL_HIGH	327
#define	KBD_FLAG	328
#define	NUMBER	329
#define	STRING	330



/*
 * C declarations
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include "Tw/Twkeys.h"

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "main.h"
#include "printk.h"
#include "util.h"
#include "hw.h"
#include "extensions.h"

#include "rctypes.h"

#include "wm.h"
#include "rcrun.h"

#ifdef CONF_THIS_MODULE
# include "version.h"
#endif

#include "rcparse.h"


/* also put here the CONF_* and DEBUG_* used in rcparse.h so that MkDep catches them */
#if defined(DEBUG_MALLOC) || defined(DEBUG_RC) || defined(DEBUG_FORK)
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



typedef union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    hwcol	 color;
    node	 _node;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		174
#define	YYFLAG		-32768
#define	YYNTBASE	82

#define YYTRANSLATE(x) ((unsigned)(x) <= 330 ? yytranslate[x] : 113)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    40,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
     4,     2,    58,     2,    59,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     5,     6,     7,     8,
     9,    10,    11,    12,    13,    14,    15,    16,    17,    18,
    19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
    29,    30,    31,    32,    33,    34,    35,    36,    37,    38,
    39,    41,    42,    43,    44,    45,    46,    47,    48,    49,
    50,    51,    52,    53,    54,    55,    56,    57,    60,    61,
    62,    63,    64,    65,    66,    67,    68,    69,    70,    71,
    72,    73,    74,    75,    76,    77,    78,    79,    80,    81
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,    10,    13,    17,    19,    21,    24,
    25,    27,    30,    33,    37,    40,    44,    48,    53,    57,
    62,    67,    74,    77,    80,    83,    86,    89,    94,    99,
   104,   106,   109,   112,   117,   119,   122,   126,   131,   133,
   136,   139,   141,   144,   150,   154,   155,   157,   159,   162,
   164,   167,   171,   174,   177,   180,   182,   184,   187,   190,
   193,   196,   200,   202,   208,   210,   213,   215,   221,   224,
   227,   230,   234,   237,   241,   244,   246,   249,   251,   253,
   255,   257,   259,   261,   263,   265,   267,   269,   271,   272,
   274,   276,   278,   279,   281,   283,   286,   287,   289,   291,
   293,   295,   297,   299
};

static const short yyrhs[] = {    83,
     0,    84,     0,    83,    84,     0,    87,    40,     0,   101,
    40,     0,    15,   103,    40,     0,    40,     0,    40,     0,
    85,    40,     0,     0,    85,     0,     5,   103,     0,     6,
   103,     0,     6,   103,    91,     0,     7,   103,     0,     7,
   103,    88,     0,     8,   103,    97,     0,     8,   103,    97,
    94,     0,     9,   103,   111,     0,     9,   103,   111,    94,
     0,    10,    80,   103,   112,     0,    10,    80,   103,   112,
   106,    80,     0,    11,   103,     0,    12,   103,     0,    13,
    80,     0,    14,   103,     0,    18,    99,     0,    20,   108,
   103,   101,     0,    23,   103,   103,   101,     0,     3,    86,
    89,     4,     0,    90,     0,    89,    90,     0,   101,    85,
     0,     3,    86,    92,     4,     0,    93,     0,    92,    93,
     0,   103,   101,    85,     0,     3,    86,    95,     4,     0,
    96,     0,    95,    96,     0,   103,    85,     0,    80,     0,
    98,    77,     0,    98,    77,    60,    98,    77,     0,    60,
    98,    77,     0,     0,    78,     0,   100,     0,    99,   100,
     0,    67,     0,   107,    67,     0,    74,    80,    80,     0,
   107,    74,     0,    75,    80,     0,    76,    80,     0,   103,
     0,    41,     0,    16,   102,     0,    17,   102,     0,    49,
   110,     0,    19,   104,     0,    22,   103,   110,     0,    21,
     0,   105,   106,    80,   106,    80,     0,    26,     0,    29,
   103,     0,    28,     0,    33,   106,    80,   106,    80,     0,
    34,   103,     0,    35,    80,     0,    36,   102,     0,    37,
   108,   103,     0,    38,   103,     0,    39,   106,    80,     0,
    39,    81,     0,   103,     0,   102,   103,     0,    81,     0,
    80,     0,    33,     0,    21,     0,    24,     0,    30,     0,
    32,     0,    24,     0,    25,     0,    30,     0,    31,     0,
     0,   107,     0,    58,     0,    59,     0,     0,   109,     0,
    79,     0,    79,   107,     0,     0,    60,     0,    61,     0,
    62,     0,    63,     0,    64,     0,    65,     0,    66,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
   160,   163,   164,   167,   168,   169,   170,   173,   174,   177,
   178,   181,   182,   183,   184,   185,   186,   187,   188,   189,
   190,   191,   192,   193,   194,   195,   196,   197,   198,   201,
   204,   205,   208,   211,   214,   215,   218,   221,   224,   225,
   228,   231,   232,   233,   234,   237,   238,   241,   242,   245,
   246,   247,   248,   249,   250,   253,   254,   255,   256,   257,
   258,   260,   261,   262,   264,   265,   266,   267,   268,   269,
   270,   271,   272,   273,   274,   277,   278,   281,   282,   285,
   286,   287,   288,   289,   292,   293,   294,   295,   298,   299,
   302,   303,   306,   307,   310,   311,   314,   315,   316,   317,
   320,   321,   324,   325
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","'('","')'",
"ADDSCREEN","ADDTOMENU","ADDTOFUNC","BACKGROUND","BORDER","BUTTON","DELETEFUNC",
"DELETEMENU","DELETEBUTTON","DELETESCREEN","READ","EXEC","EXECTTY","GLOBALFLAGS",
"INTERACTIVE","KEY","MENU","MODULE","MOUSE","MOVE","MOVESCREEN","NEXT","NOP",
"PREV","RESTART","RESIZE","RESIZESCREEN","SCREEN","SCROLL","SENDTOSCREEN","SLEEP",
"STDERR","SYNTHETICKEY","WAIT","WINDOW","'\\n'","EASY_FUNC","BEEP","CENTER",
"CLOSE","KILL","QUIT","REFRESH","WINDOWLIST","FLAG_FUNC","FOCUS","MAXIMIZE",
"FULLSCREEN","LOWER","RAISE","RAISELOWER","ROLL","USERFUNC","'+'","'-'","FL_ON",
"FL_OFF","FL_TOGGLE","FL_ACTIVE","FL_INACTIVE","FL_LEFT","FL_RIGHT","GLOBAL_FLAG",
"ALTFONT","ALWAYSCURSOR","BLINK","EDGESCROLL","HIDEMENU","MENUINFO","SHADOWS",
"PASTEBUTTON","SELECTIONBUTTON","COLOR","COL_HIGH","KBD_FLAG","NUMBER","STRING",
"rcfile","line_list","line","nl","opt_nl","immediate_line","funcbody_list","_funcbody_list",
"funcbody","menubody_list","_menubody_list","menubody","textbody_list","_textbody_list",
"textbody","color","high","global_list","global_flag","func","string_list","string",
"interactive_mode","move_or_resize","opt_flag","flag","opt_flag_kbd","flag_kbd",
"opt_flag_toggle","flag_active","flag_lr", NULL
};
#endif

static const short yyr1[] = {     0,
    82,    83,    83,    84,    84,    84,    84,    85,    85,    86,
    86,    87,    87,    87,    87,    87,    87,    87,    87,    87,
    87,    87,    87,    87,    87,    87,    87,    87,    87,    88,
    89,    89,    90,    91,    92,    92,    93,    94,    95,    95,
    96,    97,    97,    97,    97,    98,    98,    99,    99,   100,
   100,   100,   100,   100,   100,   101,   101,   101,   101,   101,
   101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
   101,   101,   101,   101,   101,   102,   102,   103,   103,   104,
   104,   104,   104,   104,   105,   105,   105,   105,   106,   106,
   107,   107,   108,   108,   109,   109,   110,   110,   110,   110,
   111,   111,   112,   112
};

static const short yyr2[] = {     0,
     1,     1,     2,     2,     2,     3,     1,     1,     2,     0,
     1,     2,     2,     3,     2,     3,     3,     4,     3,     4,
     4,     6,     2,     2,     2,     2,     2,     4,     4,     4,
     1,     2,     2,     4,     1,     2,     3,     4,     1,     2,
     2,     1,     2,     5,     3,     0,     1,     1,     2,     1,
     2,     3,     2,     2,     2,     1,     1,     2,     2,     2,
     2,     3,     1,     5,     1,     2,     1,     5,     2,     2,
     2,     3,     2,     3,     2,     1,     2,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     1,     0,     1,
     1,     1,     0,     1,     1,     2,     0,     1,     1,     1,
     1,     1,     1,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,    93,    63,     0,     0,    85,
    86,    65,    67,     0,    87,    88,    89,     0,     0,     0,
    93,     0,    89,     7,    57,    97,    79,    78,     1,     2,
     0,     0,    56,    89,    12,    13,    15,    46,     0,     0,
    23,    24,    25,    26,     0,    58,    76,    59,    91,    92,
    50,     0,     0,     0,    27,    48,     0,    81,    82,    83,
    84,    80,    61,    95,     0,    94,    97,     0,    66,     0,
    90,    69,    70,    71,     0,    73,    75,     0,    98,    99,
   100,    60,     3,     4,     5,     0,    10,    14,    10,    16,
    46,    47,    42,    17,     0,   101,   102,    19,     0,     6,
    77,     0,    54,    55,    49,    51,    53,    96,     0,    62,
     0,    89,    72,    74,    89,     8,    11,     0,     0,     0,
    10,    18,    43,    20,   103,   104,    21,    52,    28,    29,
     0,     0,     9,     0,    35,     0,     0,    31,     0,    45,
     0,    46,     0,    68,    64,    34,    36,     0,    30,    32,
    33,     0,    39,     0,     0,    22,    37,    38,    40,    41,
    44,     0,     0,     0
};

static const short yydefgoto[] = {   172,
    39,    40,   127,   128,    41,   100,   147,   148,    98,   144,
   145,   132,   162,   163,   104,   105,    65,    66,    42,    56,
    43,    73,    44,    80,    81,    75,    76,    92,   108,   137
};

static const short yypact[] = {   159,
   -18,   -18,   -18,   -18,   -18,   -52,   -18,   -18,   -48,   -18,
   -18,   -18,   -18,   -22,    37,   -41,-32768,   -18,   -18,-32768,
-32768,-32768,-32768,   -18,-32768,-32768,    14,   -18,   -40,   -18,
   -41,   -18,   -25,-32768,-32768,    18,-32768,-32768,   159,-32768,
     2,    20,-32768,    14,-32768,    63,    84,   -39,    12,   -18,
-32768,-32768,-32768,-32768,    45,   -18,-32768,   -18,-32768,-32768,
-32768,     8,    15,    22,   -22,-32768,    -3,-32768,-32768,-32768,
-32768,-32768,-32768,    14,   -18,-32768,    18,   -18,-32768,    25,
-32768,-32768,-32768,   -18,   -18,-32768,-32768,    34,-32768,-32768,
-32768,-32768,-32768,-32768,-32768,    42,    53,-32768,    53,-32768,
    46,-32768,-32768,    93,    23,-32768,-32768,    93,    16,-32768,
-32768,    48,-32768,-32768,-32768,-32768,-32768,-32768,   185,-32768,
   185,    14,-32768,-32768,    14,-32768,    85,   -18,   185,    49,
    53,-32768,    69,-32768,-32768,-32768,   -34,-32768,-32768,-32768,
    50,    52,-32768,     9,-32768,   185,    82,-32768,    53,-32768,
   -18,    46,    54,-32768,-32768,-32768,-32768,    53,-32768,-32768,
    85,    11,-32768,    53,    56,-32768,    85,-32768,-32768,    85,
-32768,   135,   136,-32768
};

static const short yypgoto[] = {-32768,
-32768,    98,  -114,   -80,-32768,-32768,-32768,    -9,-32768,-32768,
    -5,    32,-32768,   -21,-32768,   -87,-32768,    77,   -99,    13,
    -1,-32768,-32768,   -28,    -6,   113,-32768,    68,-32768,-32768
};


#define	YYLAST		266


static const short yytable[] = {    45,
    46,    47,    48,    49,    88,    51,    52,    67,    54,    55,
    57,    57,   156,   130,   168,    96,    77,    78,   129,   139,
   101,   140,    79,    59,    60,    58,    82,    50,    57,   149,
    86,    53,    59,    60,   161,    59,    60,    74,   102,    83,
   103,    94,    84,   167,    61,   -89,   158,   149,   109,   170,
   151,    62,    63,    64,   111,    87,   111,    68,    67,    95,
    69,    37,    38,   116,   165,    97,    70,   118,    71,    72,
   117,    59,    60,   119,   106,   107,   121,    89,    90,    91,
   135,   136,   111,   123,   110,   159,    99,   112,    37,    38,
    37,    38,   126,   141,   113,   131,   142,    12,    13,   133,
    15,   114,    17,    18,   122,    20,    21,    22,   153,    23,
    24,    25,    26,   124,    27,    28,    29,    30,    31,    32,
    33,   125,    35,   102,   143,   150,   146,   138,   152,   154,
    36,   155,   171,   166,   173,   174,    93,   160,   157,   134,
   169,   115,   146,    85,   120,     0,     0,     0,     0,   164,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
   164,    37,    38,     1,     2,     3,     4,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,     0,    23,    24,    25,    26,
     0,    27,    28,    29,    30,    31,    32,    33,    34,    35,
    12,    13,     0,    15,     0,    17,    18,    36,    20,    21,
    22,     0,    23,    24,    25,    26,     0,    27,    28,    29,
    30,    31,    32,    33,     0,    35,     0,     0,     0,     0,
     0,     0,     0,    36,     0,     0,     0,     0,    37,    38,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    37,    38
};

static const short yycheck[] = {     1,
     2,     3,     4,     5,    33,     7,     8,    14,    10,    11,
    12,    13,     4,   101,     4,    44,    18,    19,    99,   119,
    60,   121,    24,    58,    59,    13,    28,    80,    30,   129,
    32,    80,    58,    59,   149,    58,    59,    79,    78,    80,
    80,    40,    30,   158,    67,    80,   146,   147,    50,   164,
   131,    74,    75,    76,    56,    81,    58,    21,    65,    40,
    24,    80,    81,    67,   152,     3,    30,    74,    32,    33,
    74,    58,    59,    75,    63,    64,    78,    60,    61,    62,
    65,    66,    84,    85,    40,     4,     3,    80,    80,    81,
    80,    81,    40,   122,    80,     3,   125,    16,    17,    77,
    19,    80,    21,    22,    80,    24,    25,    26,   137,    28,
    29,    30,    31,    80,    33,    34,    35,    36,    37,    38,
    39,    80,    41,    78,    40,    77,   128,    80,    60,    80,
    49,    80,    77,    80,     0,     0,    39,   147,   144,   108,
   162,    65,   144,    31,    77,    -1,    -1,    -1,    -1,   151,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
   162,    80,    81,     5,     6,     7,     8,     9,    10,    11,
    12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
    22,    23,    24,    25,    26,    -1,    28,    29,    30,    31,
    -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
    16,    17,    -1,    19,    -1,    21,    22,    49,    24,    25,
    26,    -1,    28,    29,    30,    31,    -1,    33,    34,    35,
    36,    37,    38,    39,    -1,    41,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    49,    -1,    -1,    -1,    -1,    80,    81,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    80,    81
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */

/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

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

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif


#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif



/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
YYPARSE_RETURN_TYPE
yyparse (void *);
#else
YYPARSE_RETURN_TYPE
yyparse (void);
#endif
#endif

YYPARSE_RETURN_TYPE
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
#ifndef YYSTACK_USE_ALLOCA
  int yyfree_stacks = 0;
#endif

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
#ifndef YYSTACK_USE_ALLOCA
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
#endif	    
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
{ CallList = yyvsp[0]._node; ;
    break;}
case 2:
{ yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;
    break;}
case 3:
{ yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;
    break;}
case 4:
{ yyval._node = NULL; ;
    break;}
case 5:
{ yyval._node = yyvsp[-1]._node; ;
    break;}
case 6:
{ set_yy_file(findfile(yyvsp[-1]._string, NULL)); yyval._node = NULL; ;
    break;}
case 7:
{ yyval._node = NULL; ;
    break;}
case 8:
{;
    break;}
case 9:
{;
    break;}
case 10:
{;
    break;}
case 11:
{;
    break;}
case 12:
{ yyval.imm = ImmAddScreen(yyvsp[0]._string); ;
    break;}
case 13:
{ yyval.imm = MergeMenu(yyvsp[0]._string, NULL); ;
    break;}
case 14:
{ yyval.imm = MergeMenu(yyvsp[-1]._string,  yyvsp[0]._node ); ;
    break;}
case 15:
{ yyval.imm = MergeFunc(yyvsp[0]._string, NULL); ;
    break;}
case 16:
{ yyval.imm = MergeFunc(yyvsp[-1]._string,  yyvsp[0]._node ); ;
    break;}
case 17:
{ yyval.imm = ImmBackground(yyvsp[-1]._string, yyvsp[0].color, NULL); ;
    break;}
case 18:
{ yyval.imm = ImmBackground(yyvsp[-2]._string, yyvsp[-1].color,  yyvsp[0]._node ); ;
    break;}
case 19:
{ yyval.imm = ImmBorder(yyvsp[-1]._string, yyvsp[0].val, NULL); ;
    break;}
case 20:
{ yyval.imm = ImmBorder(yyvsp[-2]._string, yyvsp[-1].val,  yyvsp[0]._node ); ;
    break;}
case 21:
{ yyval.imm = ImmButton(yyvsp[-2].val, yyvsp[-1]._string, yyvsp[0].val, '+', 0); ;
    break;}
case 22:
{ yyval.imm = ImmButton(yyvsp[-4].val, yyvsp[-3]._string, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 23:
{ yyval.imm = ImmDeleteFunc(yyvsp[0]._string); ;
    break;}
case 24:
{ yyval.imm = ImmDeleteMenu(yyvsp[0]._string); ;
    break;}
case 25:
{ yyval.imm = ImmDeleteButton(yyvsp[0].val); ;
    break;}
case 26:
{ yyval.imm = ImmDeleteScreen(yyvsp[0]._string); ;
    break;}
case 27:
{ yyval.imm = ImmGlobalFlags(yyvsp[0]._node); ;
    break;}
case 28:
{ yyval.imm = BindKey  (yyvsp[-2].val, yyvsp[-1]._string, yyvsp[0]._node); ;
    break;}
case 29:
{ yyval.imm = BindMouse(yyvsp[-2]._string, yyvsp[-1]._string, yyvsp[0]._node); ;
    break;}
case 30:
{ yyval._node = yyvsp[-1]._node; ;
    break;}
case 31:
{ yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;
    break;}
case 32:
{ yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;
    break;}
case 33:
{ yyval._node = yyvsp[-1]._node; ;
    break;}
case 34:
{ yyval._node = yyvsp[-1]._node; ;
    break;}
case 35:
{ yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;
    break;}
case 36:
{ yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;
    break;}
case 37:
{ yyval._node = MakeNodeBody(yyvsp[-2]._string, yyvsp[-1]._node, NULL); ;
    break;}
case 38:
{ yyval._node = yyvsp[-1]._node; ;
    break;}
case 39:
{ yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;
    break;}
case 40:
{ yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;
    break;}
case 41:
{ yyval._node = MakeNode(yyvsp[-1]._string); ;
    break;}
case 42:
{ yyval.color = (hwcol) yyvsp[0].val; ;
    break;}
case 43:
{ yyval.color = COL(yyvsp[-1].color|yyvsp[0].color, BLACK); ;
    break;}
case 44:
{ yyval.color = COL(yyvsp[-4].color|yyvsp[-3].color, yyvsp[-1].color|yyvsp[0].color); ;
    break;}
case 45:
{ yyval.color = COL(WHITE, yyvsp[-1].color|yyvsp[0].color); ;
    break;}
case 46:
{ yyval.color = (hwcol)0; ;
    break;}
case 47:
{ yyval.color = HIGH; ;
    break;}
case 48:
{ yyval._node = AddtoNodeList(NULL, yyvsp[0]._node); ;
    break;}
case 49:
{ yyval._node = AddtoNodeList( yyvsp[-1]._node , yyvsp[0]._node); ;
    break;}
case 50:
{ yyval._node = MakeFlagNode(yyvsp[0].val,  0); ;
    break;}
case 51:
{ yyval._node = MakeFlagNode(yyvsp[0].val, yyvsp[-1].val); ;
    break;}
case 52:
{ yyval._node = MakeShadowsNode(yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 53:
{ yyval._node = MakeFlagNode(SHADOWS, yyvsp[-1].val); ;
    break;}
case 54:
{ yyval._node = MakeFlagNode(PASTEBUTTON, yyvsp[0].val); ;
    break;}
case 55:
{ yyval._node = MakeFlagNode(SELECTIONBUTTON, yyvsp[0].val); ;
    break;}
case 56:
{ yyval._node = MakeUserFunc(yyvsp[0]._string); ;
    break;}
case 57:
{ yyval._node = MakeBuiltinFunc(yyvsp[0].val); ;
    break;}
case 58:
{ yyval._node = MakeExec(yyvsp[0]._node); ;
    break;}
case 59:
{ yyval._node = MakeExecTty(yyvsp[0]._node); ;
    break;}
case 60:
{ yyval._node = MakeFlagNode(yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 61:
{ yyval._node = MakeFlagNode(INTERACTIVE, yyvsp[0].val); ;
    break;}
case 62:
{ yyval._node = MakeModuleNode(yyvsp[-1]._string, yyvsp[0].val); ;
    break;}
case 63:
{ yyval._node = MakeBuiltinFunc(MENU); ;
    break;}
case 64:
{ yyval._node = MakeMoveResizeScroll(yyvsp[-4].val, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 65:
{ yyval._node = MakeWindowNumber('+', 1); ;
    break;}
case 66:
{ yyval._node = MakeRestartWM(yyvsp[0]._string); ;
    break;}
case 67:
{ yyval._node = MakeWindowNumber('-', 1); ;
    break;}
case 68:
{ yyval._node = MakeMoveResizeScroll(SCROLL, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 69:
{ yyval._node = MakeSendToScreen(yyvsp[0]._string); ;
    break;}
case 70:
{ yyval._node = MakeSleep(yyvsp[0].val); ;
    break;}
case 71:
{ yyval._node = MakeStderr(yyvsp[0]._node); ;
    break;}
case 72:
{ yyval._node = MakeSyntheticKey(yyvsp[-1].val, yyvsp[0]._string); ;
    break;}
case 73:
{ yyval._node = MakeWait(yyvsp[0]._string); ;
    break;}
case 74:
{ yyval._node = MakeWindowNumber(yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 75:
{ yyval._node = MakeWindow(yyvsp[0]._string); ;
    break;}
case 76:
{ yyval._node = AddtoStringList(NULL, yyvsp[0]._string); ;
    break;}
case 77:
{ yyval._node = AddtoStringList( yyvsp[-1]._node,  yyvsp[0]._string); ;
    break;}
case 79:
{ yyval._string = toString(yyvsp[0].val); ;
    break;}
case 80:
{ yyval.val = SCROLL; ;
    break;}
case 81:
{ yyval.val = MENU; ;
    break;}
case 82:
{ yyval.val = MOVE; ;
    break;}
case 83:
{ yyval.val = RESIZE; ;
    break;}
case 84:
{ yyval.val = SCREEN; ;
    break;}
case 85:
{ yyval.val = MOVE; ;
    break;}
case 86:
{ yyval.val = MOVESCREEN; ;
    break;}
case 87:
{ yyval.val = RESIZE; ;
    break;}
case 88:
{ yyval.val = RESIZESCREEN; ;
    break;}
case 89:
{ yyval.val = 0; ;
    break;}
case 91:
{ yyval.val = '+'; ;
    break;}
case 92:
{ yyval.val = '-'; ;
    break;}
case 93:
{ yyval.val = 0; ;
    break;}
case 96:
{ yyval.val = yyvsp[-1].val | yyvsp[0].val; ;
    break;}
case 97:
{ yyval.val = FL_ON; ;
    break;}
case 98:
{ yyval.val = FL_ON; ;
    break;}
case 99:
{ yyval.val = FL_OFF; ;
    break;}
case 100:
{ yyval.val = FL_TOGGLE; ;
    break;}
case 101:
{ yyval.val = FL_ACTIVE; ;
    break;}
case 102:
{ yyval.val = FL_INACTIVE; ;
    break;}
case 103:
{ yyval.val = FL_LEFT; ;
    break;}
case 104:
{ yyval.val = FL_RIGHT; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */


  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
#ifndef YYSTACK_USE_ALLOCA
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
#endif    
  return 1;
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

