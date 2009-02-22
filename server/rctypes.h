/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef _TWIN_RCTYPES_H
#define _TWIN_RCTYPES_H


/* types */
typedef byte * str;
typedef struct node * node;
typedef struct ldat_list ldat_list;
typedef struct str_list str_list;

struct node {
    ldat id;
    str name;
    node next, body;
    str data;
    union {
	struct {
	    ldat plus_minus, flag, a, b;
	} f;
	struct {
	    str *argv;
	    uldat argc;
	} v;
	hwcol color;
	ldat ctx;
	node func;
    } x;
};

/*
 * these are modeled after STATE_* and POS_* flags
 */
#define CTX_0		0x0001
#define CTX_1		0x0002
#define CTX_2		0x0004
#define CTX_3		0x0008
#define CTX_4		0x0010
#define CTX_5		0x0020
#define CTX_6		0x0040
#define CTX_7		0x0080
#define CTX_8		0x0100
#define CTX_9		0x0200
#define CTX_BUTTON(n)   (1<<(n))
#define CTX_ANY_BUTTON  0x03FF
#define CTX_CORNER	0x0400
#define CTX_TITLE	0x0800
#define CTX_SIDE	0x1000
#define CTX_BARS	0x2000
#define CTX_WIN		(CTX_TITLE|CTX_SIDE|CTX_BARS)
#define CTX_INSIDE	0x4000

#define CTX_ANY_WIN	0x7FFF

#define CTX_MENU	0x8000 /* the menu bar */
#define CTX_ROOT	0x10000l
#define CTX_ANY		0x1FFFFl

#define CTX_BITS	18 /* this is 1 + # of used bits */

struct ldat_list {
    ldat val;
    struct ldat_list *l;
};

/* stuff exported by shm.c */
#define my_malloc	shm_malloc_or_die
#define my_strdup	shm_strdup_or_die
void *shm_malloc_or_die(size_t size);
byte *shm_strdup_or_die(CONST byte *s);

byte shm_init(size_t len);
byte shm_shrink(void);
void shm_TSR(void);
void shm_TSR_abort(void);
void shm_abort(void);
void shm_quit(void);
byte shm_send(int fd);
byte shm_receive(int fd);
void *shm_getbase(void);
#ifdef DEBUG_MALLOC
void *shm_getend(void);
#endif


/* stuff exported by rcparse.l */
#ifdef PURE_PARSER
 int yylex();
#else
 int yylex(void);
#endif

extern int read_stack_curr;
extern str file_name[];
extern int line_no[];

#define LINE_NO line_no[read_stack_curr-1]
#define FILE_NAME file_name[read_stack_curr-1]

int set_yy_file(char *path);

#endif /* _TWIN_RCTYPES_H */

