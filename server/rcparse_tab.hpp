/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
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
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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
    COPY = 297,                    /* COPY  */
    KILL = 298,                    /* KILL  */
    PASTE = 299,                   /* PASTE  */
    QUIT = 300,                    /* QUIT  */
    REFRESH = 301,                 /* REFRESH  */
    WINDOWLIST = 302,              /* WINDOWLIST  */
    FLAG_FUNC = 303,               /* FLAG_FUNC  */
    FOCUS = 304,                   /* FOCUS  */
    MAXIMIZE = 305,                /* MAXIMIZE  */
    FULLSCREEN = 306,              /* FULLSCREEN  */
    LOWER = 307,                   /* LOWER  */
    RAISE = 308,                   /* RAISE  */
    RAISELOWER = 309,              /* RAISELOWER  */
    ROLL = 310,                    /* ROLL  */
    USERFUNC = 311,                /* USERFUNC  */
    FL_ON = 312,                   /* FL_ON  */
    FL_OFF = 313,                  /* FL_OFF  */
    FL_TOGGLE = 314,               /* FL_TOGGLE  */
    FL_ACTIVE = 315,               /* FL_ACTIVE  */
    FL_INACTIVE = 316,             /* FL_INACTIVE  */
    FL_LEFT = 317,                 /* FL_LEFT  */
    FL_RIGHT = 318,                /* FL_RIGHT  */
    GLOBAL_FLAG = 319,             /* GLOBAL_FLAG  */
    ALTFONT = 320,                 /* ALTFONT  */
    CURSOR_ALWAYS = 321,           /* CURSOR_ALWAYS  */
    BLINK = 322,                   /* BLINK  */
    MENU_HIDE = 323,               /* MENU_HIDE  */
    MENU_INFO = 324,               /* MENU_INFO  */
    MENU_RELAX = 325,              /* MENU_RELAX  */
    SCREEN_SCROLL = 326,           /* SCREEN_SCROLL  */
    TERMINALS_UTF8 = 327,          /* TERMINALS_UTF8  */
    SHADOWS = 328,                 /* SHADOWS  */
    BUTTON_PASTE = 329,            /* BUTTON_PASTE  */
    BUTTON_SELECTION = 330,        /* BUTTON_SELECTION  */
    RGB = 331,                     /* RGB  */
    COL_HIGH = 332,                /* COL_HIGH  */
    KBD_FLAG = 333,                /* KBD_FLAG  */
    NUMBER = 334,                  /* NUMBER  */
    STRING = 335                   /* STRING  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{

    tcolor    col;
    ldat      val;
    ldat_list *val_list;
    str       _string;
    byte      imm;
    node      _node;


};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_RCPARSE_TAB_HPP_INCLUDED  */
