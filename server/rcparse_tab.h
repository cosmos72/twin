typedef union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    hwcol	 color;
    node	 _node;
} YYSTYPE;
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
#define	READ	268
#define	EXEC	269
#define	EXECTTY	270
#define	GLOBALFLAGS	271
#define	INTERACTIVE	272
#define	KEY	273
#define	MENU	274
#define	MODULE	275
#define	MOUSE	276
#define	MOVE	277
#define	MOVESCREEN	278
#define	NEXT	279
#define	NOP	280
#define	PREV	281
#define	RESTART	282
#define	RESIZE	283
#define	RESIZESCREEN	284
#define	SCREEN	285
#define	SCROLL	286
#define	SENDTOSCREEN	287
#define	SLEEP	288
#define	STDERR	289
#define	SYNTHETICKEY	290
#define	WAIT	291
#define	WINDOW	292
#define	EASY_FUNC	293
#define	BEEP	294
#define	CENTER	295
#define	CLOSE	296
#define	KILL	297
#define	QUIT	298
#define	REFRESH	299
#define	WINDOWLIST	300
#define	FLAG_FUNC	301
#define	FOCUS	302
#define	MAXIMIZE	303
#define	FULLSCREEN	304
#define	LOWER	305
#define	RAISE	306
#define	RAISELOWER	307
#define	ROLL	308
#define	USERFUNC	309
#define	FL_ON	310
#define	FL_OFF	311
#define	FL_TOGGLE	312
#define	FL_ACTIVE	313
#define	FL_INACTIVE	314
#define	FL_LEFT	315
#define	FL_RIGHT	316
#define	GLOBAL_FLAG	317
#define	ALTFONT	318
#define	ALWAYSCURSOR	319
#define	BLINK	320
#define	EDGESCROLL	321
#define	HIDEMENU	322
#define	MENUINFO	323
#define	SHADOWS	324
#define	PASTEBUTTON	325
#define	SELECTIONBUTTON	326
#define	COLOR	327
#define	COL_HIGH	328
#define	KBD_FLAG	329
#define	NUMBER	330
#define	STRING	331


extern YYSTYPE yylval;
