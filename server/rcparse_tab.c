
/*  A Bison parser, made from rcparse.y
 by  GNU Bison version 1.25
  */

#define YYBISON 1  /* Identify Bison output.  */

#define	ADDSCREEN	258
#define	ADDTOMENU	259
#define	ADDTOFUNC	260
#define	BACKGROUND	261
#define	BORDER	262
#define	BUTTON	263
#define	DELETEFUNC	264
#define	DELETEMENU	265
#define	DELETEBUTTON	266
#define	DELETESCREEN	267
#define	EXEC	268
#define	EXECTTY	269
#define	GLOBALFLAGS	270
#define	INTERACTIVE	271
#define	KEY	272
#define	MENU	273
#define	MOUSE	274
#define	MOVE	275
#define	MOVESCREEN	276
#define	NEXT	277
#define	NOP	278
#define	PREV	279
#define	READ	280
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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
    
#include "libTwkeys.h"

#include "twin.h"
#include "methods.h"
#include "data.h"
#include "main.h"
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


    



typedef union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    hwcol	 color;
    node	*_node;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		171
#define	YYFLAG		-32768
#define	YYNTBASE	81

#define YYTRANSLATE(x) ((unsigned)(x) <= 330 ? yytranslate[x] : 112)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    39,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     3,
     4,     2,    57,     2,    58,     2,     2,     2,     2,     2,
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
     2,     2,     2,     2,     2,     1,     2,     5,     6,     7,
     8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
    18,    19,    20,    21,    22,    23,    24,    25,    26,    27,
    28,    29,    30,    31,    32,    33,    34,    35,    36,    37,
    38,    40,    41,    42,    43,    44,    45,    46,    47,    48,
    49,    50,    51,    52,    53,    54,    55,    56,    59,    60,
    61,    62,    63,    64,    65,    66,    67,    68,    69,    70,
    71,    72,    73,    74,    75,    76,    77,    78,    79,    80
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     4,     7,    10,    13,    17,    19,    21,    24,
    25,    27,    30,    33,    37,    40,    44,    48,    53,    57,
    62,    67,    74,    77,    80,    83,    86,    89,    94,    99,
   104,   106,   109,   112,   117,   119,   122,   126,   131,   133,
   136,   139,   141,   144,   150,   154,   155,   157,   159,   162,
   164,   167,   171,   174,   177,   180,   182,   184,   187,   190,
   193,   196,   198,   204,   206,   209,   211,   217,   220,   223,
   226,   230,   233,   237,   240,   242,   245,   247,   249,   251,
   253,   255,   257,   259,   261,   263,   265,   267,   268,   270,
   272,   274,   275,   277,   279,   282,   283,   285,   287,   289,
   291,   293,   295
};

static const short yyrhs[] = {    82,
     0,    83,     0,    82,    83,     0,    86,    39,     0,   100,
    39,     0,    27,   102,    39,     0,    39,     0,    39,     0,
    84,    39,     0,     0,    84,     0,     5,   102,     0,     6,
   102,     0,     6,   102,    90,     0,     7,   102,     0,     7,
   102,    87,     0,     8,   102,    96,     0,     8,   102,    96,
    93,     0,     9,   102,   110,     0,     9,   102,   110,    93,
     0,    10,    79,   102,   111,     0,    10,    79,   102,   111,
   105,    79,     0,    11,   102,     0,    12,   102,     0,    13,
    79,     0,    14,   102,     0,    17,    98,     0,    19,   107,
   102,   100,     0,    21,   102,   102,   100,     0,     3,    85,
    88,     4,     0,    89,     0,    88,    89,     0,   100,    84,
     0,     3,    85,    91,     4,     0,    92,     0,    91,    92,
     0,   102,   100,    84,     0,     3,    85,    94,     4,     0,
    95,     0,    94,    95,     0,   102,    84,     0,    79,     0,
    97,    76,     0,    97,    76,    59,    97,    76,     0,    59,
    97,    76,     0,     0,    77,     0,    99,     0,    98,    99,
     0,    66,     0,   106,    66,     0,    73,    79,    79,     0,
   106,    73,     0,    74,    79,     0,    75,    79,     0,   102,
     0,    40,     0,    15,   101,     0,    16,   101,     0,    48,
   109,     0,    18,   103,     0,    20,     0,   104,   105,    79,
   105,    79,     0,    24,     0,    28,   102,     0,    26,     0,
    32,   105,    79,   105,    79,     0,    33,   102,     0,    34,
    79,     0,    35,   101,     0,    36,   107,   102,     0,    37,
   102,     0,    38,   105,    79,     0,    38,    80,     0,   102,
     0,   101,   102,     0,    80,     0,    79,     0,    32,     0,
    20,     0,    22,     0,    29,     0,    31,     0,    22,     0,
    23,     0,    29,     0,    30,     0,     0,   106,     0,    57,
     0,    58,     0,     0,   108,     0,    78,     0,    78,   106,
     0,     0,    59,     0,    60,     0,    61,     0,    62,     0,
    63,     0,    64,     0,    65,     0
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
   258,   260,   261,   263,   264,   265,   266,   267,   268,   269,
   270,   271,   272,   273,   276,   277,   280,   281,   284,   285,
   286,   287,   288,   291,   292,   293,   294,   297,   298,   301,
   302,   305,   306,   309,   310,   313,   314,   315,   316,   319,
   320,   323,   324
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","'('","')'",
"ADDSCREEN","ADDTOMENU","ADDTOFUNC","BACKGROUND","BORDER","BUTTON","DELETEFUNC",
"DELETEMENU","DELETEBUTTON","DELETESCREEN","EXEC","EXECTTY","GLOBALFLAGS","INTERACTIVE",
"KEY","MENU","MOUSE","MOVE","MOVESCREEN","NEXT","NOP","PREV","READ","RESTART",
"RESIZE","RESIZESCREEN","SCREEN","SCROLL","SENDTOSCREEN","SLEEP","STDERR","SYNTHETICKEY",
"WAIT","WINDOW","'\\n'","EASY_FUNC","BEEP","CENTER","CLOSE","KILL","QUIT","REFRESH",
"WINDOWLIST","FLAG_FUNC","FOCUS","MAXIMIZE","FULLSCREEN","LOWER","RAISE","RAISELOWER",
"ROLL","USERFUNC","'+'","'-'","FL_ON","FL_OFF","FL_TOGGLE","FL_ACTIVE","FL_INACTIVE",
"FL_LEFT","FL_RIGHT","GLOBAL_FLAG","ALTFONT","ALWAYSCURSOR","BLINK","EDGESCROLL",
"HIDEMENU","MENUINFO","SHADOWS","PASTEBUTTON","SELECTIONBUTTON","COLOR","COL_HIGH",
"KBD_FLAG","NUMBER","STRING","rcfile","line_list","line","nl","opt_nl","immediate_line",
"funcbody_list","_funcbody_list","funcbody","menubody_list","_menubody_list",
"menubody","textbody_list","_textbody_list","textbody","color","high","global_list",
"global_flag","func","string_list","string","interactive_mode","move_or_resize",
"opt_flag","flag","opt_flag_kbd","flag_kbd","opt_flag_toggle","flag_active",
"flag_lr", NULL
};
#endif

static const short yyr1[] = {     0,
    81,    82,    82,    83,    83,    83,    83,    84,    84,    85,
    85,    86,    86,    86,    86,    86,    86,    86,    86,    86,
    86,    86,    86,    86,    86,    86,    86,    86,    86,    87,
    88,    88,    89,    90,    91,    91,    92,    93,    94,    94,
    95,    96,    96,    96,    96,    97,    97,    98,    98,    99,
    99,    99,    99,    99,    99,   100,   100,   100,   100,   100,
   100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
   100,   100,   100,   100,   101,   101,   102,   102,   103,   103,
   103,   103,   103,   104,   104,   104,   104,   105,   105,   106,
   106,   107,   107,   108,   108,   109,   109,   109,   109,   110,
   110,   111,   111
};

static const short yyr2[] = {     0,
     1,     1,     2,     2,     2,     3,     1,     1,     2,     0,
     1,     2,     2,     3,     2,     3,     3,     4,     3,     4,
     4,     6,     2,     2,     2,     2,     2,     4,     4,     4,
     1,     2,     2,     4,     1,     2,     3,     4,     1,     2,
     2,     1,     2,     5,     3,     0,     1,     1,     2,     1,
     2,     3,     2,     2,     2,     1,     1,     2,     2,     2,
     2,     1,     5,     1,     2,     1,     5,     2,     2,     2,
     3,     2,     3,     2,     1,     2,     1,     1,     1,     1,
     1,     1,     1,     1,     1,     1,     1,     0,     1,     1,
     1,     0,     1,     1,     2,     0,     1,     1,     1,     1,
     1,     1,     1
};

static const short yydefact[] = {     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,    92,    62,     0,    84,    85,    64,
    66,     0,     0,    86,    87,    88,     0,     0,     0,    92,
     0,    88,     7,    57,    96,    78,    77,     1,     2,     0,
     0,    56,    88,    12,    13,    15,    46,     0,     0,    23,
    24,    25,    26,    58,    75,    59,    90,    91,    50,     0,
     0,     0,    27,    48,     0,    80,    81,    82,    83,    79,
    61,    94,     0,    93,     0,     0,    65,     0,    89,    68,
    69,    70,     0,    72,    74,     0,    97,    98,    99,    60,
     3,     4,     5,     0,    10,    14,    10,    16,    46,    47,
    42,    17,     0,   100,   101,    19,     0,    76,     0,    54,
    55,    49,    51,    53,    95,     0,     0,     6,    88,    71,
    73,    88,     8,    11,     0,     0,     0,    10,    18,    43,
    20,   102,   103,    21,    52,    28,    29,     0,     0,     9,
     0,    35,     0,     0,    31,     0,    45,     0,    46,     0,
    67,    63,    34,    36,     0,    30,    32,    33,     0,    39,
     0,     0,    22,    37,    38,    40,    41,    44,     0,     0,
     0
};

static const short yydefgoto[] = {   169,
    38,    39,   124,   125,    40,    98,   144,   145,    96,   141,
   142,   129,   159,   160,   102,   103,    63,    64,    41,    54,
    42,    71,    43,    78,    79,    73,    74,    90,   106,   134
};

static const short yypact[] = {   154,
    -9,    -9,    -9,    -9,    -9,   -61,    -9,    -9,   -59,    -9,
    -9,    -9,    10,    84,   -55,-32768,    -9,-32768,-32768,-32768,
-32768,    -9,    -9,-32768,-32768,    39,    -9,   -39,    -9,   -55,
    -9,    22,-32768,-32768,    40,-32768,-32768,   154,-32768,    20,
    30,-32768,    39,-32768,    58,    83,   -25,    45,    -9,-32768,
-32768,-32768,-32768,    -9,-32768,    -9,-32768,-32768,-32768,    26,
    35,    38,    10,-32768,   -10,-32768,-32768,-32768,-32768,-32768,
-32768,    39,    -9,-32768,    -9,    79,-32768,    41,-32768,-32768,
-32768,    -9,    -9,-32768,-32768,    42,-32768,-32768,-32768,-32768,
-32768,-32768,-32768,    43,    80,-32768,    80,-32768,    48,-32768,
-32768,   120,    50,-32768,-32768,   120,    47,-32768,    49,-32768,
-32768,-32768,-32768,-32768,-32768,   181,   181,-32768,    39,-32768,
-32768,    39,-32768,    88,    -9,   181,    53,    80,-32768,    71,
-32768,-32768,-32768,     8,-32768,-32768,-32768,    52,    54,-32768,
    11,-32768,   181,     9,-32768,    80,-32768,    -9,    48,    55,
-32768,-32768,-32768,-32768,    80,-32768,-32768,    88,    13,-32768,
    80,    56,-32768,    88,-32768,-32768,    88,-32768,   135,   136,
-32768
};

static const short yypgoto[] = {-32768,
-32768,    99,   -52,   -92,-32768,-32768,-32768,    -6,-32768,-32768,
    -2,    36,-32768,   -18,-32768,   -87,-32768,    81,   -66,    46,
    -1,-32768,-32768,   -24,     1,   113,-32768,-32768,-32768,-32768
};


#define	YYLAST		261


static const short yytable[] = {    44,
    45,    46,    47,    48,   126,    50,    51,    86,    53,    55,
    55,   127,   156,    65,   153,    75,   165,    49,    94,    52,
    76,    77,    72,    11,    12,    80,    14,    55,    16,    84,
    18,    19,    20,    99,    21,   148,    23,    24,    25,    81,
    26,    27,    28,    29,    30,    31,    32,   107,    34,   136,
   137,   100,   108,   101,   108,   113,    35,    56,    92,   146,
    95,   162,   114,    65,    57,    58,    57,    58,    93,    36,
    37,   116,   115,   117,    82,    59,   155,   146,    57,    58,
   108,   120,    60,    61,    62,    97,   -88,    36,    37,    36,
    37,    36,    37,   158,   138,    57,    58,   139,    87,    88,
    89,    85,   164,    66,   109,    67,   104,   105,   167,   150,
   132,   133,    68,   110,    69,    70,   111,   118,   123,   119,
   121,   122,   128,   143,   100,   130,   140,   135,   147,   149,
   151,   168,   152,   163,   170,   171,    91,   157,   154,   143,
   166,   131,    83,   112,     0,     0,   161,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,   161,     1,     2,
     3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
    13,    14,    15,    16,    17,    18,    19,    20,     0,    21,
    22,    23,    24,    25,     0,    26,    27,    28,    29,    30,
    31,    32,    33,    34,     0,    11,    12,     0,    14,     0,
    16,    35,    18,    19,    20,     0,    21,     0,    23,    24,
    25,     0,    26,    27,    28,    29,    30,    31,    32,     0,
    34,     0,     0,     0,     0,     0,     0,     0,    35,     0,
     0,     0,    36,    37,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,     0,     0,    36,
    37
};

static const short yycheck[] = {     1,
     2,     3,     4,     5,    97,     7,     8,    32,    10,    11,
    12,    99,     4,    13,     4,    17,     4,    79,    43,    79,
    22,    23,    78,    15,    16,    27,    18,    29,    20,    31,
    22,    23,    24,    59,    26,   128,    28,    29,    30,    79,
    32,    33,    34,    35,    36,    37,    38,    49,    40,   116,
   117,    77,    54,    79,    56,    66,    48,    12,    39,   126,
     3,   149,    73,    63,    57,    58,    57,    58,    39,    79,
    80,    73,    72,    75,    29,    66,   143,   144,    57,    58,
    82,    83,    73,    74,    75,     3,    79,    79,    80,    79,
    80,    79,    80,   146,   119,    57,    58,   122,    59,    60,
    61,    80,   155,    20,    79,    22,    62,    63,   161,   134,
    64,    65,    29,    79,    31,    32,    79,    39,    39,    79,
    79,    79,     3,   125,    77,    76,    39,    79,    76,    59,
    79,    76,    79,    79,     0,     0,    38,   144,   141,   141,
   159,   106,    30,    63,    -1,    -1,   148,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,   159,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    23,    24,    -1,    26,
    27,    28,    29,    30,    -1,    32,    33,    34,    35,    36,
    37,    38,    39,    40,    -1,    15,    16,    -1,    18,    -1,
    20,    48,    22,    23,    24,    -1,    26,    -1,    28,    29,
    30,    -1,    32,    33,    34,    35,    36,    37,    38,    -1,
    40,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    48,    -1,
    -1,    -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    79,
    80
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */


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
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

#ifndef alloca
#ifdef __GNUC__
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi)
#include <alloca.h>
#else /* not sparc */
#if defined (MSDOS) && !defined (__TURBOC__)
#include <malloc.h>
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
#include <malloc.h>
 #pragma alloca
#else /* not MSDOS, __TURBOC__, or _AIX */
#ifdef __hpux
#ifdef __cplusplus
extern "C" {
void *alloca (unsigned int);
};
#else /* not __cplusplus */
void *alloca ();
#endif /* not __cplusplus */
#endif /* __hpux */
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc.  */
#endif /* not GNU C.  */
#endif /* alloca not defined.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	return(0)
#define YYABORT 	return(1)
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

#ifndef YYPARSE_RETURN_TYPE
#define YYPARSE_RETURN_TYPE int
#endif

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
YYPARSE_RETURN_TYPE yyparse (void);
#endif

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
     int count;
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
__yy_memcpy (char *to, char *from, int count)
{
  register char *f = from;
  register char *t = to;
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
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
      yyss = (short *) alloca (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1, size * sizeof (*yyssp));
      yyvs = (YYSTYPE *) alloca (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1, size * sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) alloca (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1, size * sizeof (*yylsp));
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
{ yyval._node = MakeBuiltinFunc(MENU); ;
    break;}
case 63:
{ yyval._node = MakeMoveResizeScroll(yyvsp[-4].val, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 64:
{ yyval._node = MakeWindowNumber('+', 1); ;
    break;}
case 65:
{ yyval._node = MakeRestartWM(yyvsp[0]._string); ;
    break;}
case 66:
{ yyval._node = MakeWindowNumber('-', 1); ;
    break;}
case 67:
{ yyval._node = MakeMoveResizeScroll(SCROLL, yyvsp[-3].val, yyvsp[-2].val, yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 68:
{ yyval._node = MakeSendToScreen(yyvsp[0]._string); ;
    break;}
case 69:
{ yyval._node = MakeSleep(yyvsp[0].val); ;
    break;}
case 70:
{ yyval._node = MakeStderr(yyvsp[0]._node); ;
    break;}
case 71:
{ yyval._node = MakeSyntheticKey(yyvsp[-1].val, yyvsp[0]._string); ;
    break;}
case 72:
{ yyval._node = MakeWait(yyvsp[0]._string); ;
    break;}
case 73:
{ yyval._node = MakeWindowNumber(yyvsp[-1].val, yyvsp[0].val); ;
    break;}
case 74:
{ yyval._node = MakeWindow(yyvsp[0]._string); ;
    break;}
case 75:
{ yyval._node = AddtoStringList(NULL, yyvsp[0]._string); ;
    break;}
case 76:
{ yyval._node = AddtoStringList( yyvsp[-1]._node,  yyvsp[0]._string); ;
    break;}
case 78:
{ yyval._string = toString(yyvsp[0].val); ;
    break;}
case 79:
{ yyval.val = SCROLL; ;
    break;}
case 80:
{ yyval.val = MENU; ;
    break;}
case 81:
{ yyval.val = MOVE; ;
    break;}
case 82:
{ yyval.val = RESIZE; ;
    break;}
case 83:
{ yyval.val = SCREEN; ;
    break;}
case 84:
{ yyval.val = MOVE; ;
    break;}
case 85:
{ yyval.val = MOVESCREEN; ;
    break;}
case 86:
{ yyval.val = RESIZE; ;
    break;}
case 87:
{ yyval.val = RESIZESCREEN; ;
    break;}
case 88:
{ yyval.val = 0; ;
    break;}
case 90:
{ yyval.val = '+'; ;
    break;}
case 91:
{ yyval.val = '-'; ;
    break;}
case 92:
{ yyval.val = 0; ;
    break;}
case 95:
{ yyval.val = yyvsp[-1].val | yyvsp[0].val; ;
    break;}
case 96:
{ yyval.val = FL_ON; ;
    break;}
case 97:
{ yyval.val = FL_ON; ;
    break;}
case 98:
{ yyval.val = FL_OFF; ;
    break;}
case 99:
{ yyval.val = FL_TOGGLE; ;
    break;}
case 100:
{ yyval.val = FL_ACTIVE; ;
    break;}
case 101:
{ yyval.val = FL_INACTIVE; ;
    break;}
case 102:
{ yyval.val = FL_LEFT; ;
    break;}
case 103:
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

