typedef union {
    ldat	 val;
    ldat_list	*val_list;
    str          _string;
    byte         imm;
    hwcol	 color;
    node	*_node;
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


extern YYSTYPE yylval;
