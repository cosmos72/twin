typedef union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    node	 _node;
} YYSTYPE;
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
#define	CURSOR_ALWAYS	318
#define	BLINK	319
#define	MENU_HIDE	320
#define	MENU_INFO	321
#define	MENU_RELAX	322
#define	SCREEN_SCROLL	323
#define	SHADOWS	324
#define	BUTTON_PASTE	325
#define	BUTTON_SELECTION	326
#define	COLOR	327
#define	COL_HIGH	328
#define	KBD_FLAG	329
#define	NUMBER	330
#define	STRING	331


extern YYSTYPE yylval;
