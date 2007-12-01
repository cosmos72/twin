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




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)

typedef union YYSTYPE {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    node	 _node;
} YYSTYPE;
/* Line 1285 of yacc.c.  */

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;



