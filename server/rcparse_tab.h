/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

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
