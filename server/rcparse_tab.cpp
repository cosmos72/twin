/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */


/*
 * C declarations
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <sys/socket.h>

#include <Tw/Twkeys.h>
#include <Tutf/Tutf.h>

#include "twin.h"
#include "algo.h"
#include "alloc.h"
#include "methods.h"
#include "data.h"
#include "hw.h"
#include "main.h"
#include "rctypes.h"
#include "util.h"
#include "version.h"
#include "wm.h"

#include "rcrun.h"         /* needs wm.h, rctypes.h */
#include "rcparse_tab.hpp" /* needs rcrun.h         */
#include "rcparse.h"       /* needs rcparse_tab.hpp */


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



# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "rcparse_tab.hpp"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_3_ = 3,                         /* '('  */
  YYSYMBOL_4_ = 4,                         /* ')'  */
  YYSYMBOL_ADDSCREEN = 5,                  /* ADDSCREEN  */
  YYSYMBOL_ADDTOMENU = 6,                  /* ADDTOMENU  */
  YYSYMBOL_ADDTOFUNC = 7,                  /* ADDTOFUNC  */
  YYSYMBOL_BACKGROUND = 8,                 /* BACKGROUND  */
  YYSYMBOL_BORDER = 9,                     /* BORDER  */
  YYSYMBOL_BUTTON = 10,                    /* BUTTON  */
  YYSYMBOL_DELETEFUNC = 11,                /* DELETEFUNC  */
  YYSYMBOL_DELETEMENU = 12,                /* DELETEMENU  */
  YYSYMBOL_DELETEBUTTON = 13,              /* DELETEBUTTON  */
  YYSYMBOL_DELETESCREEN = 14,              /* DELETESCREEN  */
  YYSYMBOL_READ = 15,                      /* READ  */
  YYSYMBOL_EXEC = 16,                      /* EXEC  */
  YYSYMBOL_EXECTTY = 17,                   /* EXECTTY  */
  YYSYMBOL_GLOBALFLAGS = 18,               /* GLOBALFLAGS  */
  YYSYMBOL_INTERACTIVE = 19,               /* INTERACTIVE  */
  YYSYMBOL_KEY = 20,                       /* KEY  */
  YYSYMBOL_MENU = 21,                      /* MENU  */
  YYSYMBOL_MODULE = 22,                    /* MODULE  */
  YYSYMBOL_MOUSE = 23,                     /* MOUSE  */
  YYSYMBOL_MOVE = 24,                      /* MOVE  */
  YYSYMBOL_MOVESCREEN = 25,                /* MOVESCREEN  */
  YYSYMBOL_NEXT = 26,                      /* NEXT  */
  YYSYMBOL_NOP = 27,                       /* NOP  */
  YYSYMBOL_PREV = 28,                      /* PREV  */
  YYSYMBOL_RESTART = 29,                   /* RESTART  */
  YYSYMBOL_RESIZE = 30,                    /* RESIZE  */
  YYSYMBOL_RESIZESCREEN = 31,              /* RESIZESCREEN  */
  YYSYMBOL_SCREEN = 32,                    /* SCREEN  */
  YYSYMBOL_SCROLL = 33,                    /* SCROLL  */
  YYSYMBOL_SENDTOSCREEN = 34,              /* SENDTOSCREEN  */
  YYSYMBOL_SLEEP = 35,                     /* SLEEP  */
  YYSYMBOL_STDERR = 36,                    /* STDERR  */
  YYSYMBOL_SYNTHETICKEY = 37,              /* SYNTHETICKEY  */
  YYSYMBOL_WAIT = 38,                      /* WAIT  */
  YYSYMBOL_WINDOW = 39,                    /* WINDOW  */
  YYSYMBOL_40_n_ = 40,                     /* '\n'  */
  YYSYMBOL_EASY_FUNC = 41,                 /* EASY_FUNC  */
  YYSYMBOL_BEEP = 42,                      /* BEEP  */
  YYSYMBOL_CENTER = 43,                    /* CENTER  */
  YYSYMBOL_CLOSE = 44,                     /* CLOSE  */
  YYSYMBOL_COPY = 45,                      /* COPY  */
  YYSYMBOL_KILL = 46,                      /* KILL  */
  YYSYMBOL_PASTE = 47,                     /* PASTE  */
  YYSYMBOL_QUIT = 48,                      /* QUIT  */
  YYSYMBOL_REFRESH = 49,                   /* REFRESH  */
  YYSYMBOL_WINDOWLIST = 50,                /* WINDOWLIST  */
  YYSYMBOL_FLAG_FUNC = 51,                 /* FLAG_FUNC  */
  YYSYMBOL_FOCUS = 52,                     /* FOCUS  */
  YYSYMBOL_MAXIMIZE = 53,                  /* MAXIMIZE  */
  YYSYMBOL_FULLSCREEN = 54,                /* FULLSCREEN  */
  YYSYMBOL_LOWER = 55,                     /* LOWER  */
  YYSYMBOL_RAISE = 56,                     /* RAISE  */
  YYSYMBOL_RAISELOWER = 57,                /* RAISELOWER  */
  YYSYMBOL_ROLL = 58,                      /* ROLL  */
  YYSYMBOL_USERFUNC = 59,                  /* USERFUNC  */
  YYSYMBOL_60_ = 60,                       /* '+'  */
  YYSYMBOL_61_ = 61,                       /* '-'  */
  YYSYMBOL_FL_ON = 62,                     /* FL_ON  */
  YYSYMBOL_FL_OFF = 63,                    /* FL_OFF  */
  YYSYMBOL_FL_TOGGLE = 64,                 /* FL_TOGGLE  */
  YYSYMBOL_FL_ACTIVE = 65,                 /* FL_ACTIVE  */
  YYSYMBOL_FL_INACTIVE = 66,               /* FL_INACTIVE  */
  YYSYMBOL_FL_LEFT = 67,                   /* FL_LEFT  */
  YYSYMBOL_FL_RIGHT = 68,                  /* FL_RIGHT  */
  YYSYMBOL_GLOBAL_FLAG = 69,               /* GLOBAL_FLAG  */
  YYSYMBOL_ALTFONT = 70,                   /* ALTFONT  */
  YYSYMBOL_CURSOR_ALWAYS = 71,             /* CURSOR_ALWAYS  */
  YYSYMBOL_BLINK = 72,                     /* BLINK  */
  YYSYMBOL_MENU_HIDE = 73,                 /* MENU_HIDE  */
  YYSYMBOL_MENU_INFO = 74,                 /* MENU_INFO  */
  YYSYMBOL_MENU_RELAX = 75,                /* MENU_RELAX  */
  YYSYMBOL_SCREEN_SCROLL = 76,             /* SCREEN_SCROLL  */
  YYSYMBOL_TERMINALS_UTF8 = 77,            /* TERMINALS_UTF8  */
  YYSYMBOL_SHADOWS = 78,                   /* SHADOWS  */
  YYSYMBOL_BUTTON_PASTE = 79,              /* BUTTON_PASTE  */
  YYSYMBOL_BUTTON_SELECTION = 80,          /* BUTTON_SELECTION  */
  YYSYMBOL_RGB = 81,                       /* RGB  */
  YYSYMBOL_COL_HIGH = 82,                  /* COL_HIGH  */
  YYSYMBOL_KBD_FLAG = 83,                  /* KBD_FLAG  */
  YYSYMBOL_NUMBER = 84,                    /* NUMBER  */
  YYSYMBOL_STRING = 85,                    /* STRING  */
  YYSYMBOL_YYACCEPT = 86,                  /* $accept  */
  YYSYMBOL_rcfile = 87,                    /* rcfile  */
  YYSYMBOL_line_list = 88,                 /* line_list  */
  YYSYMBOL_line = 89,                      /* line  */
  YYSYMBOL_nl = 90,                        /* nl  */
  YYSYMBOL_opt_nl = 91,                    /* opt_nl  */
  YYSYMBOL_immediate_line = 92,            /* immediate_line  */
  YYSYMBOL_funcbody_list = 93,             /* funcbody_list  */
  YYSYMBOL__funcbody_list = 94,            /* _funcbody_list  */
  YYSYMBOL_funcbody = 95,                  /* funcbody  */
  YYSYMBOL_menubody_list = 96,             /* menubody_list  */
  YYSYMBOL__menubody_list = 97,            /* _menubody_list  */
  YYSYMBOL_menubody = 98,                  /* menubody  */
  YYSYMBOL_textbody_list = 99,             /* textbody_list  */
  YYSYMBOL__textbody_list = 100,           /* _textbody_list  */
  YYSYMBOL_textbody = 101,                 /* textbody  */
  YYSYMBOL_color = 102,                    /* color  */
  YYSYMBOL_high = 103,                     /* high  */
  YYSYMBOL_global_list = 104,              /* global_list  */
  YYSYMBOL_global_flag = 105,              /* global_flag  */
  YYSYMBOL_func = 106,                     /* func  */
  YYSYMBOL_string_list = 107,              /* string_list  */
  YYSYMBOL_string = 108,                   /* string  */
  YYSYMBOL_interactive_mode = 109,         /* interactive_mode  */
  YYSYMBOL_move_or_resize = 110,           /* move_or_resize  */
  YYSYMBOL_opt_flag = 111,                 /* opt_flag  */
  YYSYMBOL_flag = 112,                     /* flag  */
  YYSYMBOL_opt_flag_kbd = 113,             /* opt_flag_kbd  */
  YYSYMBOL_flag_kbd = 114,                 /* flag_kbd  */
  YYSYMBOL_opt_flag_toggle = 115,          /* opt_flag_toggle  */
  YYSYMBOL_flag_active = 116,              /* flag_active  */
  YYSYMBOL_flag_lr = 117                   /* flag_lr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_uint8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
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
#define YYLAST   281

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  86
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  32
/* YYNRULES -- Number of rules.  */
#define YYNRULES  105
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  174

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   335


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
      40,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       3,     4,     2,    60,     2,    61,     2,     2,     2,     2,
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
      58,    59,    62,    63,    64,    65,    66,    67,    68,    69,
      70,    71,    72,    73,    74,    75,    76,    77,    78,    79,
      80,    81,    82,    83,    84,    85
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,   154,   154,   157,   158,   161,   162,   163,   164,   167,
     168,   171,   172,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   189,   190,   191,
     192,   195,   198,   199,   202,   205,   208,   209,   212,   215,
     218,   219,   222,   225,   226,   227,   228,   231,   232,   235,
     236,   239,   240,   241,   242,   243,   244,   247,   248,   249,
     250,   251,   252,   254,   255,   256,   258,   259,   260,   261,
     262,   263,   264,   265,   266,   267,   268,   271,   272,   275,
     276,   279,   280,   281,   282,   283,   286,   287,   288,   289,
     292,   293,   296,   297,   300,   301,   304,   305,   308,   309,
     310,   311,   314,   315,   318,   319
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "'('", "')'",
  "ADDSCREEN", "ADDTOMENU", "ADDTOFUNC", "BACKGROUND", "BORDER", "BUTTON",
  "DELETEFUNC", "DELETEMENU", "DELETEBUTTON", "DELETESCREEN", "READ",
  "EXEC", "EXECTTY", "GLOBALFLAGS", "INTERACTIVE", "KEY", "MENU", "MODULE",
  "MOUSE", "MOVE", "MOVESCREEN", "NEXT", "NOP", "PREV", "RESTART",
  "RESIZE", "RESIZESCREEN", "SCREEN", "SCROLL", "SENDTOSCREEN", "SLEEP",
  "STDERR", "SYNTHETICKEY", "WAIT", "WINDOW", "'\\n'", "EASY_FUNC", "BEEP",
  "CENTER", "CLOSE", "COPY", "KILL", "PASTE", "QUIT", "REFRESH",
  "WINDOWLIST", "FLAG_FUNC", "FOCUS", "MAXIMIZE", "FULLSCREEN", "LOWER",
  "RAISE", "RAISELOWER", "ROLL", "USERFUNC", "'+'", "'-'", "FL_ON",
  "FL_OFF", "FL_TOGGLE", "FL_ACTIVE", "FL_INACTIVE", "FL_LEFT", "FL_RIGHT",
  "GLOBAL_FLAG", "ALTFONT", "CURSOR_ALWAYS", "BLINK", "MENU_HIDE",
  "MENU_INFO", "MENU_RELAX", "SCREEN_SCROLL", "TERMINALS_UTF8", "SHADOWS",
  "BUTTON_PASTE", "BUTTON_SELECTION", "RGB", "COL_HIGH", "KBD_FLAG",
  "NUMBER", "STRING", "$accept", "rcfile", "line_list", "line", "nl",
  "opt_nl", "immediate_line", "funcbody_list", "_funcbody_list",
  "funcbody", "menubody_list", "_menubody_list", "menubody",
  "textbody_list", "_textbody_list", "textbody", "color", "high",
  "global_list", "global_flag", "func", "string_list", "string",
  "interactive_mode", "move_or_resize", "opt_flag", "flag", "opt_flag_kbd",
  "flag_kbd", "opt_flag_toggle", "flag_active", "flag_lr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-112)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-91)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int16 yypact[] =
{
     168,   -52,   -52,   -52,   -52,   -52,   -56,   -52,   -52,   -54,
     -52,   -52,   -52,   -52,   -16,    47,   -48,  -112,   -52,   -52,
    -112,  -112,  -112,  -112,   -52,  -112,  -112,    21,   -52,   -41,
     -52,   -48,   -52,   -39,  -112,  -112,    10,  -112,  -112,    54,
     168,  -112,    27,    36,  -112,    21,  -112,    56,    58,   -43,
       4,   -52,  -112,  -112,  -112,  -112,    43,   -52,  -112,   -52,
    -112,  -112,  -112,     5,     6,     8,   -16,  -112,   -31,  -112,
    -112,  -112,  -112,  -112,  -112,    21,   -52,  -112,    10,   -52,
    -112,    13,  -112,  -112,  -112,   -52,   -52,  -112,  -112,    14,
    -112,  -112,  -112,  -112,  -112,  -112,  -112,  -112,    16,    48,
    -112,    48,  -112,    23,  -112,  -112,    98,    26,  -112,  -112,
      98,    19,  -112,  -112,    35,  -112,  -112,  -112,  -112,  -112,
    -112,   196,  -112,   196,    21,  -112,  -112,    21,  -112,    70,
     -52,   196,    46,    48,  -112,    68,  -112,  -112,  -112,   -24,
    -112,  -112,  -112,    49,    50,  -112,     9,  -112,   196,    87,
    -112,    48,  -112,   -52,    23,    51,  -112,  -112,  -112,  -112,
      48,  -112,  -112,    70,    11,  -112,    48,    55,  -112,    70,
    -112,  -112,    70,  -112
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
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
    -112,  -112,  -112,    91,  -111,   -85,  -112,  -112,  -112,   -17,
    -112,  -112,    -7,    30,  -112,   -27,  -112,   -89,  -112,    75,
     -97,    12,    -1,  -112,  -112,   -25,    -9,   111,  -112,    65,
    -112,  -112
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_uint8 yydefgoto[] =
{
       0,    39,    40,    41,   129,   130,    42,   102,   149,   150,
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
      56,    58,    58,   158,   132,   170,   131,    78,    79,   103,
      98,    60,    61,    80,   141,    59,   142,    83,    51,    58,
      54,    87,    37,    38,   151,    75,    60,    61,   118,   104,
     163,   105,    85,    84,    60,    61,    88,   119,   153,   169,
     111,   160,   151,    62,    94,   172,   113,    68,   113,    99,
     -90,   101,    63,    64,    65,   167,   120,    96,    69,   108,
     109,    70,    90,    91,    92,   121,    97,    71,   123,    72,
      73,    60,    61,   112,   113,   125,   137,   138,   128,   114,
     115,   161,   116,    37,    38,    37,    38,   124,   126,   143,
     127,   133,   144,    12,    13,   104,    15,   135,    17,    18,
     145,    20,    21,    22,   155,    23,    24,    25,    26,   140,
      27,    28,    29,    30,    31,    32,    33,   152,    35,   148,
     154,    95,   162,   156,   157,   168,   173,   171,    36,   159,
     136,   117,    86,   122,     0,   148,     0,     0,     0,     0,
       0,     0,   166,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   166,     0,     0,     0,     0,     0,     0,
       0,    37,    38,     1,     2,     3,     4,     5,     6,     7,
       8,     9,    10,    11,    12,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,     0,    23,    24,    25,    26,
       0,    27,    28,    29,    30,    31,    32,    33,    34,    35,
       0,     0,    12,    13,     0,    15,     0,    17,    18,    36,
      20,    21,    22,     0,    23,    24,    25,    26,     0,    27,
      28,    29,    30,    31,    32,    33,     0,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
       0,     0,    37,    38,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      37,    38
};

static const yytype_int16 yycheck[] =
{
       1,     2,     3,     4,     5,    14,     7,     8,    33,    10,
      11,    12,    13,     4,   103,     4,   101,    18,    19,    62,
      45,    60,    61,    24,   121,    13,   123,    28,    84,    30,
      84,    32,    84,    85,   131,    83,    60,    61,    69,    82,
     151,    84,    30,    84,    60,    61,    85,    78,   133,   160,
      51,   148,   149,    69,     0,   166,    57,    66,    59,     3,
      84,     3,    78,    79,    80,   154,    75,    40,    21,    65,
      66,    24,    62,    63,    64,    76,    40,    30,    79,    32,
      33,    60,    61,    40,    85,    86,    67,    68,    40,    84,
      84,     4,    84,    84,    85,    84,    85,    84,    84,   124,
      84,     3,   127,    16,    17,    82,    19,    81,    21,    22,
      40,    24,    25,    26,   139,    28,    29,    30,    31,    84,
      33,    34,    35,    36,    37,    38,    39,    81,    41,   130,
      62,    40,   149,    84,    84,    84,    81,   164,    51,   146,
     110,    66,    31,    78,    -1,   146,    -1,    -1,    -1,    -1,
      -1,    -1,   153,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   164,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    84,    85,     5,     6,     7,     8,     9,    10,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,    -1,    28,    29,    30,    31,
      -1,    33,    34,    35,    36,    37,    38,    39,    40,    41,
      -1,    -1,    16,    17,    -1,    19,    -1,    21,    22,    51,
      24,    25,    26,    -1,    28,    29,    30,    31,    -1,    33,
      34,    35,    36,    37,    38,    39,    -1,    41,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    51,    -1,    -1,
      -1,    -1,    84,    85,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      84,    85
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     5,     6,     7,     8,     9,    10,    11,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      24,    25,    26,    28,    29,    30,    31,    33,    34,    35,
      36,    37,    38,    39,    40,    41,    51,    84,    85,    87,
      88,    89,    92,   106,   108,   110,   108,   108,   108,   108,
     108,    84,   108,   108,    84,   108,   108,   107,   108,   107,
      60,    61,    69,    78,    79,    80,   104,   105,   112,    21,
      24,    30,    32,    33,   109,    83,   113,   114,   108,   108,
     108,   111,   112,   108,    84,   107,   113,   108,    85,   111,
      62,    63,    64,   115,     0,    89,    40,    40,   111,     3,
      96,     3,    93,    62,    82,    84,   102,   103,    65,    66,
     116,   108,    40,   108,    84,    84,    84,   105,    69,    78,
     112,   108,   115,   108,    84,   108,    84,    84,    40,    90,
      91,    91,   103,     3,    99,    81,    99,    67,    68,   117,
      84,   106,   106,   111,   111,    40,    97,    98,   108,    94,
      95,   106,    81,    91,    62,   111,    84,    84,     4,    98,
     106,     4,    95,    90,   100,   101,   108,   103,    84,    90,
       4,   101,    90,    81
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    86,    87,    88,    88,    89,    89,    89,    89,    90,
      90,    91,    91,    92,    92,    92,    92,    92,    92,    92,
      92,    92,    92,    92,    92,    92,    92,    92,    92,    92,
      92,    93,    94,    94,    95,    96,    97,    97,    98,    99,
     100,   100,   101,   102,   102,   102,   102,   103,   103,   104,
     104,   105,   105,   105,   105,   105,   105,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   107,   107,   108,
     108,   109,   109,   109,   109,   109,   110,   110,   110,   110,
     111,   111,   112,   112,   113,   113,   114,   114,   115,   115,
     115,   115,   116,   116,   117,   117
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
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


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
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
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
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
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 2: /* rcfile: line_list  */
                             { CallList = (yyvsp[0]._node); }
    break;

  case 3: /* line_list: line  */
                              { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }
    break;

  case 4: /* line_list: line_list line  */
                                 { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }
    break;

  case 5: /* line: immediate_line '\n'  */
                                  { (yyval._node) = NULL; }
    break;

  case 6: /* line: func '\n'  */
                              { (yyval._node) = (yyvsp[-1]._node); }
    break;

  case 7: /* line: READ string '\n'  */
                              { set_yy_file(FindConfigFile((yyvsp[-1]._string), NULL)); (yyval._node) = NULL; }
    break;

  case 8: /* line: '\n'  */
                              { (yyval._node) = NULL; }
    break;

  case 9: /* nl: '\n'  */
                    {}
    break;

  case 10: /* nl: nl '\n'  */
                  {}
    break;

  case 11: /* opt_nl: %empty  */
                              {}
    break;

  case 12: /* opt_nl: nl  */
                        {}
    break;

  case 13: /* immediate_line: ADDSCREEN string  */
                                                    { (yyval.imm) = ImmAddScreen((yyvsp[0]._string)); }
    break;

  case 14: /* immediate_line: ADDTOMENU string  */
                                          { (yyval.imm) = MergeMenu((yyvsp[0]._string), NULL); }
    break;

  case 15: /* immediate_line: ADDTOMENU string menubody_list  */
                                          { (yyval.imm) = MergeMenu((yyvsp[-1]._string),  (yyvsp[0]._node) ); }
    break;

  case 16: /* immediate_line: ADDTOFUNC string  */
                                          { (yyval.imm) = MergeFunc((yyvsp[0]._string), NULL); }
    break;

  case 17: /* immediate_line: ADDTOFUNC string funcbody_list  */
                                          { (yyval.imm) = MergeFunc((yyvsp[-1]._string),  (yyvsp[0]._node) ); }
    break;

  case 18: /* immediate_line: BACKGROUND string color  */
                                                { (yyval.imm) = ImmBackground((yyvsp[-1]._string), (yyvsp[0].col), NULL); }
    break;

  case 19: /* immediate_line: BACKGROUND string color textbody_list  */
                                                { (yyval.imm) = ImmBackground((yyvsp[-2]._string), (yyvsp[-1].col),  (yyvsp[0]._node) ); }
    break;

  case 20: /* immediate_line: BORDER string flag_active  */
                                                      { (yyval.imm) = ImmBorder((yyvsp[-1]._string), (yyvsp[0].val), NULL); }
    break;

  case 21: /* immediate_line: BORDER string flag_active textbody_list  */
                                                      { (yyval.imm) = ImmBorder((yyvsp[-2]._string), (yyvsp[-1].val),  (yyvsp[0]._node) ); }
    break;

  case 22: /* immediate_line: BUTTON NUMBER string flag_lr  */
                                                           { (yyval.imm) = ImmButton((yyvsp[-2].val), (yyvsp[-1]._string), (yyvsp[0].val), '+', 0); }
    break;

  case 23: /* immediate_line: BUTTON NUMBER string flag_lr opt_flag NUMBER  */
                                                           { (yyval.imm) = ImmButton((yyvsp[-4].val), (yyvsp[-3]._string), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 24: /* immediate_line: DELETEFUNC string  */
                               { (yyval.imm) = ImmDeleteFunc((yyvsp[0]._string)); }
    break;

  case 25: /* immediate_line: DELETEMENU string  */
                               { (yyval.imm) = ImmDeleteMenu((yyvsp[0]._string)); }
    break;

  case 26: /* immediate_line: DELETEBUTTON NUMBER  */
                               { (yyval.imm) = ImmDeleteButton((yyvsp[0].val)); }
    break;

  case 27: /* immediate_line: DELETESCREEN string  */
                               { (yyval.imm) = ImmDeleteScreen((yyvsp[0]._string)); }
    break;

  case 28: /* immediate_line: GLOBALFLAGS global_list  */
                                    { (yyval.imm) = ImmGlobalFlags((yyvsp[0]._node)); }
    break;

  case 29: /* immediate_line: KEY opt_flag_kbd string func  */
                                         { (yyval.imm) = BindKey  ((yyvsp[-2].val), (yyvsp[-1]._string), (yyvsp[0]._node)); }
    break;

  case 30: /* immediate_line: MOUSE string string func  */
                                         { (yyval.imm) = BindMouse((yyvsp[-2]._string), (yyvsp[-1]._string), (yyvsp[0]._node)); }
    break;

  case 31: /* funcbody_list: '(' opt_nl _funcbody_list ')'  */
                                                 { (yyval._node) = (yyvsp[-1]._node); }
    break;

  case 32: /* _funcbody_list: funcbody  */
                                            { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }
    break;

  case 33: /* _funcbody_list: _funcbody_list funcbody  */
                                  { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }
    break;

  case 34: /* funcbody: func nl  */
                      { (yyval._node) = (yyvsp[-1]._node); }
    break;

  case 35: /* menubody_list: '(' opt_nl _menubody_list ')'  */
                                                 { (yyval._node) = (yyvsp[-1]._node); }
    break;

  case 36: /* _menubody_list: menubody  */
                                            { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }
    break;

  case 37: /* _menubody_list: _menubody_list menubody  */
                                  { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }
    break;

  case 38: /* menubody: string func nl  */
                             { (yyval._node) = MakeNodeBody((yyvsp[-2]._string), (yyvsp[-1]._node), NULL); }
    break;

  case 39: /* textbody_list: '(' opt_nl _textbody_list ')'  */
                                                 { (yyval._node) = (yyvsp[-1]._node); }
    break;

  case 40: /* _textbody_list: textbody  */
                                            { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }
    break;

  case 41: /* _textbody_list: _textbody_list textbody  */
                                  { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }
    break;

  case 42: /* textbody: string nl  */
                       { (yyval._node) = MakeNode((yyvsp[-1]._string)); }
    break;

  case 43: /* color: NUMBER  */
                                  { (yyval.col) = TCOL((yyvsp[0].val),     tblack); }
    break;

  case 44: /* color: high RGB  */
                                  { (yyval.col) = TCOL((yyvsp[-1].val)|(yyvsp[0].val),  tblack); }
    break;

  case 45: /* color: high RGB FL_ON high RGB  */
                                  { (yyval.col) = TCOL((yyvsp[-4].val)|(yyvsp[-3].val),  (yyvsp[-1].val)|(yyvsp[0].val)); }
    break;

  case 46: /* color: FL_ON high RGB  */
                                  { (yyval.col) = TCOL(twhite, (yyvsp[-1].val)|(yyvsp[0].val)); }
    break;

  case 47: /* high: %empty  */
                        { (yyval.val) = TRGB0; }
    break;

  case 48: /* high: COL_HIGH  */
                        { (yyval.val) = thigh; }
    break;

  case 49: /* global_list: global_flag  */
                                  { (yyval._node) = AddtoNodeList(NULL, (yyvsp[0]._node)); }
    break;

  case 50: /* global_list: global_list global_flag  */
                                  { (yyval._node) = AddtoNodeList( (yyvsp[-1]._node) , (yyvsp[0]._node)); }
    break;

  case 51: /* global_flag: GLOBAL_FLAG  */
                                  { (yyval._node) = MakeFlagNode((yyvsp[0].val),  0); }
    break;

  case 52: /* global_flag: flag GLOBAL_FLAG  */
                           { (yyval._node) = MakeFlagNode((yyvsp[0].val), (yyvsp[-1].val)); }
    break;

  case 53: /* global_flag: SHADOWS NUMBER NUMBER  */
                                     { (yyval._node) = MakeShadowsNode((yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 54: /* global_flag: flag SHADOWS  */
                           { (yyval._node) = MakeFlagNode(SHADOWS, (yyvsp[-1].val)); }
    break;

  case 55: /* global_flag: BUTTON_PASTE NUMBER  */
                                  { (yyval._node) = MakeFlagNode(BUTTON_PASTE, (yyvsp[0].val)); }
    break;

  case 56: /* global_flag: BUTTON_SELECTION NUMBER  */
                                  { (yyval._node) = MakeFlagNode(BUTTON_SELECTION, (yyvsp[0].val)); }
    break;

  case 57: /* func: string  */
                            { (yyval._node) = MakeUserFunc((yyvsp[0]._string)); }
    break;

  case 58: /* func: EASY_FUNC  */
                           { (yyval._node) = MakeBuiltinFunc((yyvsp[0].val)); }
    break;

  case 59: /* func: EXEC string_list  */
                                 { (yyval._node) = MakeExec((yyvsp[0]._node)); }
    break;

  case 60: /* func: EXECTTY string_list  */
                                 { (yyval._node) = MakeExecTty((yyvsp[0]._node)); }
    break;

  case 61: /* func: FLAG_FUNC opt_flag_toggle  */
                                    { (yyval._node) = MakeFlagNode((yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 62: /* func: INTERACTIVE interactive_mode  */
                    { (yyval._node) = MakeFlagNode(INTERACTIVE, (yyvsp[0].val)); }
    break;

  case 63: /* func: MODULE string opt_flag_toggle  */
                                           { (yyval._node) = MakeModuleNode((yyvsp[-1]._string), (yyvsp[0].val)); }
    break;

  case 64: /* func: MENU  */
                          { (yyval._node) = MakeBuiltinFunc(MENU); }
    break;

  case 65: /* func: move_or_resize opt_flag NUMBER opt_flag NUMBER  */
                    { (yyval._node) = MakeMoveResizeScroll((yyvsp[-4].val), (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 66: /* func: NEXT  */
                          { (yyval._node) = MakeWindowNumber('+', 1); }
    break;

  case 67: /* func: RESTART string  */
                            { (yyval._node) = MakeRestartWM((yyvsp[0]._string)); }
    break;

  case 68: /* func: PREV  */
                          { (yyval._node) = MakeWindowNumber('-', 1); }
    break;

  case 69: /* func: SCROLL opt_flag NUMBER opt_flag NUMBER  */
                                                 { (yyval._node) = MakeMoveResizeScroll(SCROLL, (yyvsp[-3].val), (yyvsp[-2].val), (yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 70: /* func: SENDTOSCREEN string  */
                                 { (yyval._node) = MakeSendToScreen((yyvsp[0]._string)); }
    break;

  case 71: /* func: SLEEP NUMBER  */
                              { (yyval._node) = MakeSleep((yyvsp[0].val)); }
    break;

  case 72: /* func: STDERR string_list  */
                                { (yyval._node) = MakeStderr((yyvsp[0]._node)); }
    break;

  case 73: /* func: SYNTHETICKEY opt_flag_kbd string  */
                                            { (yyval._node) = MakeSyntheticKey((yyvsp[-1].val), (yyvsp[0]._string)); }
    break;

  case 74: /* func: WAIT string  */
                             { (yyval._node) = MakeWait((yyvsp[0]._string)); }
    break;

  case 75: /* func: WINDOW opt_flag NUMBER  */
                                    { (yyval._node) = MakeWindowNumber((yyvsp[-1].val), (yyvsp[0].val)); }
    break;

  case 76: /* func: WINDOW STRING  */
                                   { (yyval._node) = MakeWindow((yyvsp[0]._string)); }
    break;

  case 77: /* string_list: string  */
                                { (yyval._node) = AddtoStringList((node)0, (yyvsp[0]._string)); }
    break;

  case 78: /* string_list: string_list string  */
                             { (yyval._node) = AddtoStringList( (yyvsp[-1]._node),  (yyvsp[0]._string)); }
    break;

  case 80: /* string: NUMBER  */
                 { (yyval._string) = toString((yyvsp[0].val)); }
    break;

  case 81: /* interactive_mode: SCROLL  */
                         { (yyval.val) = SCROLL; }
    break;

  case 82: /* interactive_mode: MENU  */
                 { (yyval.val) = MENU; }
    break;

  case 83: /* interactive_mode: MOVE  */
                 { (yyval.val) = MOVE; }
    break;

  case 84: /* interactive_mode: RESIZE  */
                 { (yyval.val) = RESIZE; }
    break;

  case 85: /* interactive_mode: SCREEN  */
                 { (yyval.val) = SCREEN; }
    break;

  case 86: /* move_or_resize: MOVE  */
                                { (yyval.val) = MOVE; }
    break;

  case 87: /* move_or_resize: MOVESCREEN  */
                        { (yyval.val) = MOVESCREEN; }
    break;

  case 88: /* move_or_resize: RESIZE  */
                    { (yyval.val) = RESIZE; }
    break;

  case 89: /* move_or_resize: RESIZESCREEN  */
                          { (yyval.val) = RESIZESCREEN; }
    break;

  case 90: /* opt_flag: %empty  */
                            { (yyval.val) = 0; }
    break;

  case 92: /* flag: '+'  */
                         { (yyval.val) = '+'; }
    break;

  case 93: /* flag: '-'  */
                     { (yyval.val) = '-'; }
    break;

  case 94: /* opt_flag_kbd: %empty  */
                                { (yyval.val) = 0; }
    break;

  case 97: /* flag_kbd: KBD_FLAG flag  */
                        { (yyval.val) = (yyvsp[-1].val) | (yyvsp[0].val); }
    break;

  case 98: /* opt_flag_toggle: %empty  */
                                   { (yyval.val) = FL_ON; }
    break;

  case 99: /* opt_flag_toggle: FL_ON  */
                       { (yyval.val) = FL_ON; }
    break;

  case 100: /* opt_flag_toggle: FL_OFF  */
                    { (yyval.val) = FL_OFF; }
    break;

  case 101: /* opt_flag_toggle: FL_TOGGLE  */
                       { (yyval.val) = FL_TOGGLE; }
    break;

  case 102: /* flag_active: FL_ACTIVE  */
                              { (yyval.val) = FL_ACTIVE; }
    break;

  case 103: /* flag_active: FL_INACTIVE  */
                         { (yyval.val) = FL_INACTIVE; }
    break;

  case 104: /* flag_lr: FL_LEFT  */
                            { (yyval.val) = FL_LEFT; }
    break;

  case 105: /* flag_lr: FL_RIGHT  */
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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
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
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}



#ifdef DEBUG_YACC
static void yyprint(FILE *file, int type, void *value) {
    if (type == NUMBER) {
      fprintf (file, " %d", ((YYSTYPE *)value)->val);
    } else if (type == STRING) {
      fprintf (file, " \"%s\"", ((YYSTYPE *)value)->_string);
    } else if (type == GLOBAL_FLAG || type == FLAG_FUNC ||
               type == EASY_FUNC) {
      fprintf (file, " %s", TokenName(((YYSTYPE *)value)->val));
    }
}
#endif

static byte rcparse(const char *path) {
    return set_yy_file(path) == 0 && yyparse() == 0;
}
