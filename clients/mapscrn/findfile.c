/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include "xmalloc.h"
#include "findfile.h"
#include "nls.h"

char pathname[1024];
static int ispipe;

void fpclose(FILE *fp) {
	if (ispipe)
	     pclose(fp);
	else
	     fclose(fp);
}

#define SIZE(a) (sizeof(a)/sizeof(a[0]))

static struct decompressor {
	char *ext;		/* starts with `.', has no other dots */
	char *cmd;
} decompressors[] = {
	{ ".gz", "gzip -d -c" },
	{ ".bz2", "bzip2 -d -c" },
	{ 0, 0 }
};

static FILE *
pipe_open(struct decompressor *dc) {
	char *pipe_cmd;
	FILE *fp;

	ispipe = 1;
	pipe_cmd = xmalloc(strlen(dc->cmd) + strlen(pathname) + 2);
	sprintf(pipe_cmd, "%s %s", dc->cmd, pathname);
	fp = popen(pipe_cmd, "r");
	if (fp == NULL)
		fprintf(stderr, _("error executing  %s\n"), pipe_cmd);
	return fp;
}

/* If a file PATHNAME exists, then open it.
   If is has a `compressed' extension, then open a pipe reading it */
static FILE *
maybe_pipe_open(void) {
	FILE *fp;
	char *t;
	struct decompressor *dc;

	if ((fp = fopen(pathname, "r")) != NULL) {
	    t = rindex(pathname, '.');
	    if (t) {
		for (dc = &decompressors[0]; dc->cmd; dc++)
		    if (strcmp(t, dc->ext) == 0) {
			fclose(fp);
			return pipe_open(dc);
		    }
	    }
	}
	return fp;
}

static FILE *
findfile_in_dir(char *fnam, char *dir, int recdepth, char **suf) {
	FILE *fp = NULL;
	DIR *d;
	struct dirent *de;
	char *ff, *fdir, *p, *q, **sp;
	struct decompressor *dc;

	ispipe = 0;

	ff = index(fnam, '/');
	if (ff) {
		fdir = xstrdup(fnam);
		fdir[ff-fnam] = 0; 	/* caller guarantees fdir != "" */
	} else
		fdir = 0;		/* just to please gcc */

	d = opendir(dir);
	if (d == NULL)
	    return NULL;
	while ((de = readdir(d)) != NULL) {
	    int okdir;

	    if (strcmp(de->d_name, ".") == 0 ||
		strcmp(de->d_name, "..") == 0)
		continue;

	    if (strlen(dir) + strlen(de->d_name) + 2 > sizeof(pathname))
		continue;

	    okdir = (ff && strcmp(de->d_name, fdir) == 0);

	    if (recdepth || okdir) {
       		struct stat statbuf;
		char *a;

		a = xmalloc(strlen(dir) + strlen(de->d_name) + 2);
		sprintf(a, "%s/%s", dir, de->d_name);
		if (stat(a, &statbuf) == 0 &&
		    S_ISDIR(statbuf.st_mode)) {
			if (okdir)
				fp = findfile_in_dir(ff+1, a, 0, suf);
			if (!fp && recdepth)
				fp = findfile_in_dir(fnam, a, recdepth-1, suf);
			if (fp)
				return fp;
		}
		free(a);
	    }

	    /* Should we be in a subdirectory? */
	    if (ff)
		    continue;

	    /* Does d_name start right? */
	    p = &de->d_name[0];
	    q = fnam;
	    while (*p && *p == *q) p++,q++;
	    if (*q)
		    continue;

	    sprintf(pathname, "%s/%s", dir, de->d_name);

	    /* Does tail consist of a known suffix and possibly
	       a compression suffix? */
	    for(sp = suf; *sp; sp++) {
		    int l;

		    if (!strcmp(p, *sp))
			    return maybe_pipe_open();

		    l = strlen(*sp);
		    if (strncmp(p,*sp,l) == 0) {
			for (dc = &decompressors[0]; dc->cmd; dc++)
			    if (strcmp(p+l, dc->ext) == 0)
				return pipe_open(dc);
		    }
	    }
	}
	return NULL;
}

/* find input file; leave name in pathname[] */
FILE *findfile(char *fnam, char **dirpath, char **suffixes) {
        char **dp, *dir, **sp;
	FILE *fp;
	int dl, recdepth;
	struct decompressor *dc;

	if (strlen(fnam) >= sizeof(pathname))
		return NULL;

	/* Try explicitly given name first */
	strcpy(pathname, fnam);
	fp = maybe_pipe_open();
	if (fp)
		return fp;

	/* Test for full pathname - opening it failed, so need suffix */
	/* (This is just nonsense, for backwards compatibility.) */
	if (*fnam == '/') {
	    for (sp = suffixes; *sp; sp++) {
		if (strlen(fnam) + strlen(*sp) + 1 > sizeof(pathname))
		    continue;
		if (*sp == 0)
		    continue;	/* we tried it already */
		sprintf(pathname, "%s%s", fnam, *sp);
		if((fp = fopen(pathname, "r")) != NULL)
		    return fp;
	    }

	    for (sp = suffixes; *sp; sp++) {
		for (dc = &decompressors[0]; dc->cmd; dc++) {
		    if (strlen(fnam) + strlen(*sp)
			+ strlen(dc->ext) + 1 > sizeof(pathname))
			    continue;
		    sprintf(pathname, "%s%s%s", fnam, *sp, dc->ext);
		    if ((fp = fopen(pathname, "r")) != NULL) {
			    fclose(fp);
			    return pipe_open(dc);
		    }
		}
	    }

	    return NULL;
	}

	/* Search a list of directories and directory hierarchies */
	for (dp = dirpath; *dp; dp++) {

	    /* delete trailing slashes; trailing stars denote recursion */
	    dir = xstrdup(*dp);
	    dl = strlen(dir);
	    recdepth = 0;
	    while (dl && dir[dl-1] == '*') {
		    dir[--dl] = 0;
		    recdepth++;
	    }
	    if (dl == 0) {
		    dir = ".";
	    } else if (dl > 1 && dir[dl-1] == '/') {
		    dir[dl-1] = 0;
	    }

	    fp = findfile_in_dir(fnam, dir, recdepth, suffixes);
	    if (fp)
		    return fp;
	}

	return NULL;
}
