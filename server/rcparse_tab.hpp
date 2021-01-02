/* A Bison parser, made by GNU Bison 3.7.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2020 Free Software Foundation,
   Inc.

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_RCPARSE_TAB_HPP_INCLUDED
# define YY_YY_RCPARSE_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    ADDSCREEN = 258,               /* ADDSCREEN  */
    ADDTOMENU = 259,               /* ADDTOMENU  */
    ADDTOFUNC = 260,               /* ADDTOFUNC  */
    BACKGROUND = 261,              /* BACKGROUND  */
    BORDER = 262,                  /* BORDER  */
    BUTTON = 263,                  /* BUTTON  */
    DELETEFUNC = 264,              /* DELETEFUNC  */
    DELETEMENU = 265,              /* DELETEMENU  */
    DELETEBUTTON = 266,            /* DELETEBUTTON  */
    DELETESCREEN = 267,            /* DELETESCREEN  */
    READ = 268,                    /* READ  */
    EXEC = 269,                    /* EXEC  */
    EXECTTY = 270,                 /* EXECTTY  */
    GLOBALFLAGS = 271,             /* GLOBALFLAGS  */
    INTERACTIVE = 272,             /* INTERACTIVE  */
    KEY = 273,                     /* KEY  */
    MENU = 274,                    /* MENU  */
    MODULE = 275,                  /* MODULE  */
    MOUSE = 276,                   /* MOUSE  */
    MOVE = 277,                    /* MOVE  */
    MOVESCREEN = 278,              /* MOVESCREEN  */
    NEXT = 279,                    /* NEXT  */
    NOP = 280,                     /* NOP  */
    PREV = 281,                    /* PREV  */
    RESTART = 282,                 /* RESTART  */
    RESIZE = 283,                  /* RESIZE  */
    RESIZESCREEN = 284,            /* RESIZESCREEN  */
    SCREEN = 285,                  /* SCREEN  */
    SCROLL = 286,                  /* SCROLL  */
    SENDTOSCREEN = 287,            /* SENDTOSCREEN  */
    SLEEP = 288,                   /* SLEEP  */
    STDERR = 289,                  /* STDERR  */
    SYNTHETICKEY = 290,            /* SYNTHETICKEY  */
    WAIT = 291,                    /* WAIT  */
    WINDOW = 292,                  /* WINDOW  */
    EASY_FUNC = 293,               /* EASY_FUNC  */
    BEEP = 294,                    /* BEEP  */
    CENTER = 295,                  /* CENTER  */
    CLOSE = 296,                   /* CLOSE  */
    KILL = 297,                    /* KILL  */
    QUIT = 298,                    /* QUIT  */
    REFRESH = 299,                 /* REFRESH  */
    WINDOWLIST = 300,              /* WINDOWLIST  */
    FLAG_FUNC = 301,               /* FLAG_FUNC  */
    FOCUS = 302,                   /* FOCUS  */
    MAXIMIZE = 303,                /* MAXIMIZE  */
    FULLSCREEN = 304,              /* FULLSCREEN  */
    LOWER = 305,                   /* LOWER  */
    RAISE = 306,                   /* RAISE  */
    RAISELOWER = 307,              /* RAISELOWER  */
    ROLL = 308,                    /* ROLL  */
    USERFUNC = 309,                /* USERFUNC  */
    FL_ON = 310,                   /* FL_ON  */
    FL_OFF = 311,                  /* FL_OFF  */
    FL_TOGGLE = 312,               /* FL_TOGGLE  */
    FL_ACTIVE = 313,               /* FL_ACTIVE  */
    FL_INACTIVE = 314,             /* FL_INACTIVE  */
    FL_LEFT = 315,                 /* FL_LEFT  */
    FL_RIGHT = 316,                /* FL_RIGHT  */
    GLOBAL_FLAG = 317,             /* GLOBAL_FLAG  */
    ALTFONT = 318,                 /* ALTFONT  */
    CURSOR_ALWAYS = 319,           /* CURSOR_ALWAYS  */
    BLINK = 320,                   /* BLINK  */
    MENU_HIDE = 321,               /* MENU_HIDE  */
    MENU_INFO = 322,               /* MENU_INFO  */
    MENU_RELAX = 323,              /* MENU_RELAX  */
    SCREEN_SCROLL = 324,           /* SCREEN_SCROLL  */
    TERMINALS_UTF8 = 325,          /* TERMINALS_UTF8  */
    SHADOWS = 326,                 /* SHADOWS  */
    BUTTON_PASTE = 327,            /* BUTTON_PASTE  */
    BUTTON_SELECTION = 328,        /* BUTTON_SELECTION  */
    COLOR = 329,                   /* COLOR  */
    COL_HIGH = 330,                /* COL_HIGH  */
    KBD_FLAG = 331,                /* KBD_FLAG  */
    NUMBER = 332,                  /* NUMBER  */
    STRING = 333                   /* STRING  */
  };
  typedef enum yytokentype yytoken_kind_t;
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

#endif /* !YY_YY_RCPARSE_TAB_HPP_INCLUDED  */
