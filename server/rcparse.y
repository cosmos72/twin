/* -*-c-*-
 *
 *  rcparse.y  --  bison ~/.twinrc parser for twin
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

%{

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


%}

/*
 * BISON declarations
 */

%union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    node	 _node;
}

/*
#ifdef PURE_PARSER
%pure_parser
#endif
*/

/* keywords: */


/* tokens not valid in a function: */
%token '(' ')' ADDSCREEN ADDTOMENU ADDTOFUNC
%token BACKGROUND BORDER BUTTON
%token DELETEFUNC DELETEMENU DELETEBUTTON DELETESCREEN
%token READ 

/* tokens valid as first function token */
%token EXEC EXECTTY GLOBALFLAGS INTERACTIVE KEY 
%token MENU MODULE MOUSE MOVE MOVESCREEN NEXT NOP PREV
%token RESTART RESIZE RESIZESCREEN
%token SCREEN SCROLL SENDTOSCREEN SLEEP STDERR
%token SYNTHETICKEY WAIT WINDOW
%token '\n'

%token <val> EASY_FUNC
/* one of or */
%token BEEP CENTER CLOSE KILL QUIT REFRESH WINDOWLIST

%token <val> FLAG_FUNC
/* one of or */
%token FOCUS MAXIMIZE FULLSCREEN LOWER RAISE RAISELOWER ROLL

%token USERFUNC
/* end of tokens valid as first function token */
    
%token '+' '-' FL_ON FL_OFF FL_TOGGLE FL_ACTIVE FL_INACTIVE FL_LEFT FL_RIGHT

%token <val> GLOBAL_FLAG
/* one of */
%token ALTFONT CURSOR_ALWAYS BLINK MENU_HIDE MENU_INFO MENU_RELAX SCREEN_SCROLL SHADOWS
%token BUTTON_PASTE BUTTON_SELECTION

%token <val> COLOR
%token COL_HIGH

%token <val> KBD_FLAG
/* one of KBD_*_FL in twin.h */
    
/* atoms: */
%token <val>    NUMBER
%token <_string> STRING


/* complicated things: */
%type <val>         color high
%type <_string>     string
%type <val>	    interactive_mode move_or_resize
%type <val>         flag opt_flag flag_active opt_flag_toggle flag_lr
%type <val>	    flag_kbd opt_flag_kbd
%type <imm>	    nl opt_nl immediate_line
%type <_node>       func line
%type <_node>       global_flag funcbody menubody textbody
%type <_node>       string_list line_list global_list 
%type <_node>       funcbody_list _funcbody_list
%type <_node>       menubody_list _menubody_list
%type <_node>       textbody_list _textbody_list

%start rcfile

%%

/*
 * Grammar rules
 */

rcfile		: line_list	{ CallList = $1; }
                ;

line_list	:           line { $$ = AddtoNodeList(NULL, $1); }
                | line_list line { $$ = AddtoNodeList( $1 , $2); }
                ;

line		: immediate_line '\n' { $$ = NULL; }
		| func           '\n' { $$ = $1; }
		| READ string    '\n' { set_yy_file(FindFile($2, NULL)); $$ = NULL; }
		| '\n'                { $$ = NULL; }
                ;

nl		:    '\n' {}
		| nl '\n' {}
		;

opt_nl		: /* nothing */ {}
		| nl            {}
		;

immediate_line	: ADDSCREEN  string               { $$ = ImmAddScreen($2); }
		| ADDTOMENU  string               { $$ = MergeMenu($2, NULL); }
		| ADDTOMENU  string menubody_list { $$ = MergeMenu($2,  $3 ); }
		| ADDTOFUNC  string               { $$ = MergeFunc($2, NULL); }
		| ADDTOFUNC  string funcbody_list { $$ = MergeFunc($2,  $3 ); }
		| BACKGROUND string color               { $$ = ImmBackground($2, $3, NULL); }
		| BACKGROUND string color textbody_list { $$ = ImmBackground($2, $3,  $4 ); }
		| BORDER     string flag_active               { $$ = ImmBorder($2, $3, NULL); }
		| BORDER     string flag_active textbody_list { $$ = ImmBorder($2, $3,  $4 ); }
		| BUTTON     NUMBER string flag_lr                 { $$ = ImmButton($2, $3, $4, '+', 0); }
		| BUTTON     NUMBER string flag_lr opt_flag NUMBER { $$ = ImmButton($2, $3, $4, $5, $6); }
		| DELETEFUNC	string { $$ = ImmDeleteFunc($2); }
		| DELETEMENU    string { $$ = ImmDeleteMenu($2); }
		| DELETEBUTTON  NUMBER { $$ = ImmDeleteButton($2); }
		| DELETESCREEN  string { $$ = ImmDeleteScreen($2); }
		| GLOBALFLAGS   global_list { $$ = ImmGlobalFlags($2); }
		| KEY   opt_flag_kbd string func { $$ = BindKey  ($2, $3, $4); }
		| MOUSE string       string func { $$ = BindMouse($2, $3, $4); }
		;

funcbody_list	: '(' opt_nl _funcbody_list ')' { $$ = $3; }
		;

_funcbody_list	:                funcbody { $$ = AddtoNodeList(NULL, $1); }
		| _funcbody_list funcbody { $$ = AddtoNodeList( $1 , $2); }
		;

funcbody	: func nl { $$ = $1; }
		;

menubody_list	: '(' opt_nl _menubody_list ')' { $$ = $3; }
		;

_menubody_list	:                menubody { $$ = AddtoNodeList(NULL, $1); }
		| _menubody_list menubody { $$ = AddtoNodeList( $1 , $2); }
		;

menubody	: string func nl { $$ = MakeNodeBody($1, $2, NULL); }
		;

textbody_list	: '(' opt_nl _textbody_list ')' { $$ = $3; }
		;

_textbody_list	:                textbody { $$ = AddtoNodeList(NULL, $1); }
		| _textbody_list textbody { $$ = AddtoNodeList( $1 , $2); }
		;

textbody	: string nl   { $$ = MakeNode($1); }
		;

color		: NUMBER		      { $$ = (hwcol) $1; }
		| high COLOR		      { $$ = COL($1|$2, BLACK); }
		| high COLOR FL_ON high COLOR { $$ = COL($1|$2, $4|$5); }
		|            FL_ON high COLOR { $$ = COL(WHITE, $2|$3); }
		;

high		: /* nothing */ { $$ = (hwcol)0; }
		| COL_HIGH	{ $$ = HIGH; }
		;

global_list	:             global_flag { $$ = AddtoNodeList(NULL, $1); }
		| global_list global_flag { $$ = AddtoNodeList( $1 , $2); }
		;

global_flag	:      GLOBAL_FLAG { $$ = MakeFlagNode($1,  0); }
		| flag GLOBAL_FLAG { $$ = MakeFlagNode($2, $1); }
		|      SHADOWS NUMBER NUMBER { $$ = MakeShadowsNode($2, $3); }
		| flag SHADOWS     { $$ = MakeFlagNode(SHADOWS, $1); }
		| BUTTON_PASTE NUMBER     { $$ = MakeFlagNode(BUTTON_PASTE, $2); }
		| BUTTON_SELECTION NUMBER { $$ = MakeFlagNode(BUTTON_SELECTION, $2); }
		;

func		: string		{ $$ = MakeUserFunc($1); }
		| EASY_FUNC		{ $$ = MakeBuiltinFunc($1); }
		| EXEC    string_list	{ $$ = MakeExec($2); }
		| EXECTTY string_list	{ $$ = MakeExecTty($2); }
		| FLAG_FUNC opt_flag_toggle { $$ = MakeFlagNode($1, $2); }
		| INTERACTIVE interactive_mode
					{ $$ = MakeFlagNode(INTERACTIVE, $2); }
		| MODULE string	opt_flag_toggle { $$ = MakeModuleNode($2, $3); }
		| MENU			{ $$ = MakeBuiltinFunc(MENU); }
		| move_or_resize opt_flag NUMBER opt_flag NUMBER
					{ $$ = MakeMoveResizeScroll($1, $2, $3, $4, $5); }
		| NEXT			{ $$ = MakeWindowNumber('+', 1); }
		| RESTART string	{ $$ = MakeRestartWM($2); }
		| PREV			{ $$ = MakeWindowNumber('-', 1); }
		| SCROLL opt_flag NUMBER opt_flag NUMBER { $$ = MakeMoveResizeScroll(SCROLL, $2, $3, $4, $5); }
		| SENDTOSCREEN string	{ $$ = MakeSendToScreen($2); }
		| SLEEP NUMBER		{ $$ = MakeSleep($2); }
		| STDERR string_list	{ $$ = MakeStderr($2); }
		| SYNTHETICKEY opt_flag_kbd string  { $$ = MakeSyntheticKey($2, $3); }
		| WAIT string		{ $$ = MakeWait($2); }
		| WINDOW opt_flag NUMBER	{ $$ = MakeWindowNumber($2, $3); }
		| WINDOW STRING			{ $$ = MakeWindow($2); }
		;

string_list	:             string { $$ = AddtoStringList(NULL, $1); }
		| string_list string { $$ = AddtoStringList( $1,  $2); }
		;

string		: STRING
		| NUMBER { $$ = toString($1); }
		;

interactive_mode: SCROLL { $$ = SCROLL; }
		| MENU   { $$ = MENU; }
		| MOVE   { $$ = MOVE; }
		| RESIZE { $$ = RESIZE; }
		| SCREEN { $$ = SCREEN; }
		;

move_or_resize	: MOVE		{ $$ = MOVE; }
		| MOVESCREEN	{ $$ = MOVESCREEN; }
		| RESIZE	{ $$ = RESIZE; }
		| RESIZESCREEN	{ $$ = RESIZESCREEN; }
		;

opt_flag	: /* nothing */ { $$ = 0; }
		| flag
		;

flag		: '+'		{ $$ = '+'; }
		| '-'		{ $$ = '-'; }
		;
		
opt_flag_kbd	: /* nothing */ { $$ = 0; }
		| flag_kbd
		;

flag_kbd	: KBD_FLAG
		| KBD_FLAG flag { $$ = $1 | $2; }
		;

opt_flag_toggle	: /* nothing */ { $$ = FL_ON; }
		| FL_ON		{ $$ = FL_ON; }
		| FL_OFF	{ $$ = FL_OFF; }
		| FL_TOGGLE	{ $$ = FL_TOGGLE; }
		;

flag_active	: FL_ACTIVE	{ $$ = FL_ACTIVE; }
		| FL_INACTIVE	{ $$ = FL_INACTIVE; }
		;

flag_lr		: FL_LEFT	{ $$ = FL_LEFT; }
		| FL_RIGHT	{ $$ = FL_RIGHT; }
		;
		
%%

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

