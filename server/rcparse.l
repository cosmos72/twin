%{
/* -*-c-*-
 *
 *  rcparse.l  --  lex back-end of ~/.twinrc parser for twin
 *
 *  Copyright (C) 2000-2001 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "twin.h"
#include "rctypes.h"
#include "rcparse_tab.h"

#define IS_OCTDIGIT(c) ((c) >= '0' && (c) <= '7')
#define IS_HEXDIGIT(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'A' && (c) <= 'F') || ((c) >= 'a' && (c) <= 'f'))
#define HEX2INT(c)     ((c) >= 'a' ? (c) - 'a' + 10 : (c) >= 'A' ? (c) - 'A' + 10 : (c) - '0')

#define MAX_READ_DEPTH 64
static YY_BUFFER_STATE read_stack[MAX_READ_DEPTH];
int read_stack_curr = 0;
byte *file_name[MAX_READ_DEPTH];
int line_no[MAX_READ_DEPTH];

#define LINE_NO line_no[read_stack_curr-1]
#define FILE_NAME file_name[read_stack_curr-1]
    
static int yywrap(void);

static char errbuf[256];

/* twin.h typedefs `msg'... avoid it */
#define msg Msg


#ifdef PURE_PARSER
# define YY_DECL int yylex(YYSTYPE *_yylval, void *yylloc)
# define yylval (*_yylval)
#endif
    
#define YY_SKIP_YYWRAP
#define YY_NO_SCAN_BYTES
#define YY_NO_SCAN_STRING
#define YY_NO_UNPUT
#define YY_NO_SCAN_BUFFER
/*#undef  YY_STACK_USED*/

#define YY_USE_PROTOS

%}

DEC		[0-9]
OCT		[0-7]
HEX		[0-9A-Fa-f]
NONZERO		[1-9]
ALPHA		[A-Za-z_]
ALNUM		{ALPHA}|\-|{DEC}
ID		{ALPHA}{ALNUM}*

DECNUMBER	{NONZERO}{DEC}*
OCTNUMBER	"0"{OCT}*
HEXNUMBER	("0x"|"0X"){HEX}+
NUMBER		({DECNUMBER}|{OCTNUMBER}|{HEXNUMBER})

OCTCHAR		\\{OCTAL}{OCTAL}{OCTAL}
HEXCHAR		\\[xX]{HEX}{HEX}

%%

AddScreen	return ADDSCREEN;
AddToFunc	return ADDTOFUNC;
AddToMenu	return ADDTOMENU;
Background	return BACKGROUND;
Border		return BORDER;
Button		return BUTTON;
DeleteFunc	return DELETEFUNC;
DeleteMenu	return DELETEMENU;
DeleteButton	return DELETEBUTTON;
DeleteScreen	return DELETESCREEN;
Exec		return EXEC;
ExecTty		return EXECTTY;
GlobalFlags	return GLOBALFLAGS;
Interactive	return INTERACTIVE;
Key		return KEY;
Menu		return MENU;
Mouse		return MOUSE;
Module		return MODULE;
Move		return MOVE;
MoveScreen	return MOVESCREEN;
Next		return NEXT;
Prev		return PREV;
Read		return READ;
Restart		return RESTART;
Resize		return RESIZE;
ResizeScreen	return RESIZESCREEN;
Screen		return SCREEN;
Scroll		return SCROLL;
SendToScreen	return SENDTOSCREEN;
Stderr		return STDERR;
Sleep		return SLEEP;
SyntheticKey	return SYNTHETICKEY;
Wait		return WAIT;
Window		return WINDOW;

Focus		{ yylval.val = FOCUS;      return FLAG_FUNC; }
Roll		{ yylval.val = ROLL;       return FLAG_FUNC; }

Beep		{ yylval.val = BEEP;       return EASY_FUNC; }
Center		{ yylval.val = CENTER;     return EASY_FUNC; }
Close		{ yylval.val = CLOSE;      return EASY_FUNC; }
FullScreen	{ yylval.val = FULLSCREEN; return EASY_FUNC; }
Kill		{ yylval.val = KILL;       return EASY_FUNC; }
Lower		{ yylval.val = LOWER;      return EASY_FUNC; }
Maximize	{ yylval.val = MAXIMIZE;   return EASY_FUNC; }
Nop		{ yylval.val = NOP;	   return EASY_FUNC; }
Quit		{ yylval.val = QUIT;       return EASY_FUNC; }
Raise		{ yylval.val = RAISE;      return EASY_FUNC; }
RaiseLower	{ yylval.val = RAISELOWER; return EASY_FUNC; }
Refresh		{ yylval.val = REFRESH;    return EASY_FUNC; }
WindowList	{ yylval.val = WINDOWLIST; return EASY_FUNC; }

On		return FL_ON;
Off		return FL_OFF;
Toggle		return FL_TOGGLE;
Active		return FL_ACTIVE;
Inactive	return FL_INACTIVE;
Left		return FL_LEFT;
Right		return FL_RIGHT;
ButtonPaste|PasteButton		return BUTTON_PASTE;
ButtonSelection|SelectionButton	return BUTTON_SELECTION;
Shadows		return SHADOWS;

AltFont		{ yylval.val = ALTFONT;      return GLOBAL_FLAG; }
Blink		{ yylval.val = BLINK;        return GLOBAL_FLAG; }
CursorAlways|AlwaysCursor {
		  yylval.val = CURSOR_ALWAYS;return GLOBAL_FLAG; }
MenuHide|HideMenu {
		  yylval.val = MENU_HIDE;    return GLOBAL_FLAG; }
MenuInfo	{ yylval.val = MENU_INFO;    return GLOBAL_FLAG; }
MenuRelax	{ yylval.val = MENU_RELAX;   return GLOBAL_FLAG; }
ScreenScroll|EdgeScroll	{
		  yylval.val = SCREEN_SCROLL;return GLOBAL_FLAG; }
Alt		{ yylval.val = KBD_ALT_FL;   return KBD_FLAG; }
Ctrl		{ yylval.val = KBD_CTRL_FL;  return KBD_FLAG; }
Shift		{ yylval.val = KBD_SHIFT_FL; return KBD_FLAG; }

Black		{ yylval.val = BLACK;	return COLOR; }
Blue		{ yylval.val = BLUE;	return COLOR; }
Green		{ yylval.val = GREEN;	return COLOR; }
Cyan		{ yylval.val = CYAN;	return COLOR; }
Red		{ yylval.val = RED;	return COLOR; }
Magenta		{ yylval.val = MAGENTA;	return COLOR; }
Yellow		{ yylval.val = YELLOW;	return COLOR; }
White		{ yylval.val = WHITE;	return COLOR; }
Bold|High	return COL_HIGH;

\n		{ LINE_NO++; return '\n'; }

\;		{ return '\n'; }

"#"[^\n]*	/* eat comments */

{ID}		{
		    yylval._string = my_strdup(yytext);
		    return STRING;
		}

{NUMBER}	{
		    yylval.val = strtol(yytext, NULL, 0);
		    return NUMBER;
		}

[\(\)\+\-]	return (unsigned char)yytext[0];

({ALPHA}|{DEC})+ {
		    sprintf(errbuf, "twin: %.200s:%d: invalid identifier `%s'\n",
			    FILE_NAME, LINE_NO, yytext);
		    YY_FATAL_ERROR(errbuf);
		}

\"([^\"]|(\\.))*\" {
			int len = strlen(yytext + 1);
			char c, *p = yytext + 1;
			char *res = (char *)my_malloc(len), *s = res;
			
			while ((c = *p)) {
			    if (c != '\\' || !p[1])
				*s++ = *p++;
			    else switch(*++p) {
			      case 'a': *s++ = '\a'; break;
			      case 'b': *s++ = '\b'; break;
			      case 'e': *s++ = '\033'; break;
			      case 'f': *s++ = '\f'; break;
			      case 'n': *s++ = '\n'; break;
			      case 'r': *s++ = '\r'; break;
			      case 't': *s++ = '\t'; break;
			      case 'v': *s++ = '\v'; break;
			      case '0': *s++ = '\0'; break;
			      case 'u':
				/* FIXME: unicode is ok, but s is a (char *), not (hwfont *) ! */
				if (IS_HEXDIGIT(p[1]) &&
				    IS_HEXDIGIT(p[2]) &&
				    IS_HEXDIGIT(p[3]) &&
				    IS_HEXDIGIT(p[4])) {
				    *s++ = (((HEX2INT(p[1]) << 4) | HEX2INT(p[2])) << 8)
				        | ((HEX2INT(p[3]) << 4) | HEX2INT(p[2]));
				    p += 5;
				}
				break;
			      case 'x':
				if (IS_HEXDIGIT(p[1]) &&
				    IS_HEXDIGIT(p[2])) {
				    *s++ = (HEX2INT(p[1]) << 4)	| HEX2INT(p[2]);
				    p += 3;
				}
				break;
			      default:
				if (IS_OCTDIGIT(p[0]) &&
				    IS_OCTDIGIT(p[1]) &&
				    IS_OCTDIGIT(p[2])) {
				    *s++ = ((p[0] - '0') << 6)
					| ((p[1] - '0') << 3)
					|  (p[2] - '0');
				    p += 3;
				} else if (*p == '0')
				    *s++ = '\0', p++;
				else
				    *s++ = *p++;
				break;
			    }
			}
			s[-1] = '\0'; /* overwrite final \" */
			yylval._string = res;
			return STRING;
		}

\"([^\"]|(\\.))* {
		    char *buf = AllocMem(256 + strlen(yytext));
		    if (buf) {
			sprintf(buf, "twin: %.200s:%d: unterminated string:\n%s\n",
				FILE_NAME, LINE_NO, yytext);
		    } else
			buf = "twin: unterminated string\n";
		    YY_FATAL_ERROR(buf);
		}
		
[\t\r\f\v ]+	/* eat whitespace */


.		{
		    unsigned char ch = yytext[0];
		    sprintf(errbuf, "twin: %.200s:%d: illegal character 0x%02X",
			    FILE_NAME, LINE_NO, ch);
                    if (ch >= 32 && ch < 127) {
			sprintf(errbuf + strlen(errbuf), " `%c'\n", ch);
                    } else
			strcat(errbuf, "\n");
		    YY_FATAL_ERROR(errbuf);
		}

%%


int set_yy_file(char *path) {
    uldat len;
    FILE *f;
	
    if (!path)
	return 1;
    
    if (read_stack_curr >= MAX_READ_DEPTH) {
	fprintf(stderr, "twin: %s:%d: `Read' commands nested too deeply!\n",
		FILE_NAME, LINE_NO);
	return 0;
    }

    if (!(f = fopen(path, "r")))
	return 1;

    len = LenStr(path) + 1;
    
    read_stack[read_stack_curr++] = YY_CURRENT_BUFFER;
    LINE_NO = 1;
    FILE_NAME = my_malloc(len);
    CopyMem(path, FILE_NAME, len);

    yy_switch_to_buffer(yy_create_buffer(yyin = f, YY_BUF_SIZE));
    BEGIN(INITIAL);
    return 0;
}

static int yywrap(void) {
    if ( read_stack_curr > 0 ) {
	fclose(yyin);
	yy_delete_buffer( YY_CURRENT_BUFFER );
	yy_switch_to_buffer(read_stack[--read_stack_curr]);
	BEGIN(INITIAL);
    }
    return read_stack_curr <= 0;
}

