/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/*
 * mapscrn.c
 */

#include <Tw/Tw.h>
#include <Tw/Twerrno.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef TW_HAVE_MEMORY_H
# include <memory.h>
#endif
#ifdef TW_HAVE_FCNTL_H
# include <fcntl.h>
#endif
#ifdef TW_HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif
#ifdef __linux__
# include <sys/kd.h>
#elif !defined (E_TABSZ)
# define E_TABSZ 256
#endif

#include "paths.h"
#include "findfile.h"
#include "kdmapop.h"
#include "utf8.h"
#include "nls.h"

/* the two exported functions */
void saveoldmap(int fd, char *omfil);
void loadnewmap(int fd, char *mfil);

static int ctoi (unsigned char *);

/* search for the map file in these directories (with trailing /) */
static char *mapdirpath[] = { "", DATADIR "/" TRANSDIR "/", 0 };
static char *mapsuffixes[] = { "", ".trans", "_to_uni.trans", ".acm", 0 };

#ifdef MAIN
#include "getfd.h"
#include "m_version.h"

int verbose = 0;

int
main(int argc, char *argv[]) {
	int fd;

	set_progname(argv[0]);

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	if (argc == 2 && !strcmp(argv[1], "-V"))
	    print_version_and_exit();

	if (argc > 1 && !strcmp(argv[1], "-v")) {
		verbose = 1;
		argc--;
		argv++;
	}

	fd = getfd();

	if (argc >= 3 && !strcmp(argv[1], "-o")) {
	    saveoldmap(fd, argv[2]);
	    argc -= 2;
	    argv += 2;
	    if (argc == 1)
	      exit(0);
	}
		
	if (argc != 2) {
		fprintf(stderr, _("usage: %s [-v] [-o map.orig] map-file\n"),
			progname);
		exit(1);
	}

    	if (!TwOpen(NULL)) {
	    fprintf(stderr, _("mapscrn: libTw error: %s\n"), TwStrError(TwErrno));
	    exit(1);
	}
	loadnewmap(fd, argv[1]);

	if (TwErrno) {
	    fprintf(stderr, _("mapscrn: libTw error: %s\n"), TwStrError(TwErrno));
	}
	TwClose();
	
	exit(0);
}
#endif

/*
 * Read two-column file (index, value) with index in 0-255
 * and value in 0-65535. Acceptable representations:
 * decimal (nnn), octal (0nnn), hexadecimal (0xnnn), Unicode (U+xxxx),
 * character ('x').
 * In the character encoding, x may be a single byte binary value
 * (but not space, tab, comma, #) or a single Unicode value coded
 * as a UTF-8 sequence.
 *
 * Store values in ubuf[], and small values also in buf[].
 * Set u to 1 in case a value above 255 or a U+ occurs.
 * Set lineno to line number of first error.
 */
static int
parsemap(FILE *fp, char *buf, unsigned short *ubuf, int *u, int *lineno) {
	char buffer[256];
	int in, on, ln, ret = 0;
	char *p, *q;

	ln = 0;
	while (fgets(buffer,sizeof(buffer)-1,fp)) {
		ln++;
		if (!*u && strstr(buffer, "U+"))
			*u = 1;
		p = strtok(buffer," \t\n");
		if (p && *p != '#') {
			q = strtok(NULL," \t\n#");
			if (q) {
				in = ctoi(p);
				on = ctoi(q);
				if (in >= 0 && in < 256 &&
				    on >= 0 && on < 65536) {
					ubuf[in] = on;
					if (on < 256)
						buf[in] = on;
					else
						*u = 1;
				} else {
					if (!ret)
						*lineno = ln;
					ret = -1;
				}
			}
		}
	}
	return ret;
}

static int
readnewmapfromfile(int fd, char *mfil, char *buf, unsigned short *ubuf) {
	FILE *fp;
	struct stat stbuf;
	int u = 0;
	int lineno;

	if ((fp = findfile(mfil, mapdirpath, mapsuffixes)) == NULL) {
	        fprintf(stderr, _("mapscrn: cannot open map file _%s_\n"),
			mfil);
		exit(1);
	}
	if (stat(pathname, &stbuf)) {
		perror(pathname);
		fprintf(stderr, _("Cannot stat map file"));
		exit(1);
	}
	if (stbuf.st_size == E_TABSZ) {
		if (verbose)
			printf(_("Loading binary direct-to-font screen map "
				 "from file %s\n"),
			       pathname);
		if (fread(buf,E_TABSZ,1,fp) != 1) {
			fprintf(stderr,
				_("Error reading map from file `%s'\n"),
				pathname);
			exit(1);
		}
	} else if (stbuf.st_size == 2*E_TABSZ) {
		if (verbose)
			printf(_("Loading binary unicode screen map "
				 "from file %s\n"),
			       pathname);
		if (fread(ubuf,2*E_TABSZ,1,fp) != 1) {
			fprintf(stderr,
				_("Error reading map from file `%s'\n"),
				pathname);
			exit(1);
		}
		u = 1;
	} else 	{
		if (verbose)
			printf(_("Loading symbolic screen map from file %s\n"),
			       pathname);
		if (parsemap(fp,buf,ubuf,&u,&lineno)) {
			fprintf(stderr,
				_("Error parsing symbolic map "
				  "from `%s', line %d\n"),
				pathname, lineno);
			exit(1);
		}
	}
	fpclose(fp);
	return u;
}

void
loadnewmap(int fd, char *mfil) {
	unsigned short ubuf[E_TABSZ];
	char buf[E_TABSZ];
	int i, u;

	/* default: trivial straight-to-font */
	for (i=0; i<E_TABSZ; i++) {
		buf[i] = i;
		ubuf[i] = (0xf000 + i);
	}

	u = 0;
	if (mfil)
		u = readnewmapfromfile(fd, mfil, buf, ubuf);

	/* do we need to use loaduniscrnmap() ? */
	if (u) {
#ifdef CONF__UNICODE
		/* yes */
		if (loaduniscrnmap(fd, ubuf))
			exit(1);
#else
		fprintf(stderr,
			_("Error: map from file uses Unicode. Recompile me with unicode support!\n"));
		exit(1);
#endif		
	} else {
		/* no */
		if (loadscrnmap(fd,buf))
			exit(1);
	}
}

/*
 * Read decimal, octal, hexadecimal, Unicode (U+xxxx) or character
 * ('x', x a single byte or a utf8 sequence).
 */
int
ctoi(unsigned char *s) {
	int i;

	if ((strncmp(s,"0x",2) == 0) && 
	    (strspn(s+2,"0123456789abcdefABCDEF") == strlen(s+2)))
		sscanf(s+2,"%x",&i);

	else if ((*s == '0') &&
		 (strspn(s,"01234567") == strlen(s)))
		sscanf(s,"%o",&i);

	else if (strspn(s,"0123456789") == strlen(s)) 
		sscanf(s,"%d",&i);

	else if ((strncmp(s,"U+",2) == 0) && strlen(s) == 6 &&
		 (strspn(s+2,"0123456789abcdefABCDEF") == 4))
		sscanf(s+2,"%x",&i);

	else if ((strlen(s) == 3) && (s[0] == '\'') && (s[2] == '\''))
		i=s[1];

	else if (s[0] == '\'') {
		int err;
		char *s1 = s+1;

		i = from_utf8(&s1, 0, &err);
		if (err || s1[0] != '\'' || s1[1] != 0)
			return -1;
	}

	else return -1;

	return i;
}

void
saveoldmap(int fd, char *omfil) {
	FILE *fp;
	char buf[E_TABSZ];
	unsigned short ubuf[E_TABSZ];
	int i, havemap, haveumap;

	if ((fp = fopen(omfil, "w")) == NULL) {
		perror(omfil);
		exit(1);
	}
	havemap = haveumap = 1;
	if (getscrnmap(fd,buf))
		havemap = 0;
	if (getuniscrnmap(fd,ubuf))
		haveumap = 0;
	if (havemap && haveumap) {
		for (i = 0; i < E_TABSZ; i++) {
			if ((ubuf[i] & ~0xff) != 0xf000) {
				havemap = 0;
				break;
			}
		}
	}
	if (havemap) {
		if (fwrite(buf,sizeof(buf),1,fp) != 1) {
			fprintf(stderr, _("Error writing map to file\n"));
			exit(1);
		}
	} else if (haveumap) {
		if (fwrite(ubuf,sizeof(ubuf),1,fp) != 1) {
			fprintf(stderr, _("Error writing map to file\n"));
			exit(1);
		}
	} else {
		fprintf(stderr, _("Cannot read console map\n"));
		exit(1);
	}
	fclose(fp);

	if (verbose)
		printf(_("Saved screen map in `%s'\n"), omfil);
}
