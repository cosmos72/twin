/*
 * mapscrn.c - version 0.98
 */

#include <stdio.h>
#include <memory.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/kd.h>

#include "libTw.h"

#include "paths.h"
#include "findfile.h"
#include "nls.h"

/* the two exported functions */
void loadnewmap(char *mfil);

static int parsemap (FILE *, char*);
static int ctoi (unsigned char *);

/* search for the map file in these directories (with trailing /) */
static char *mapdirpath[] = { "", DATADIR "/" TRANSDIR "/", 0 };
static char *mapsuffixes[] = { "", 0 };

#include "version.h"

int verbose = 0;

int
main(int argc, char *argv[]) {

	set_progname(argv[0]);

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	if (argc == 2 && !strcmp(argv[1], "-V"))
	    print_version_and_exit();

	if (argc != 2) {
		fprintf(stderr, _("usage: %s map-file\n"),
			progname);
		exit(1);
	}
    
    	if (!TwOpen(NULL)) {
	    fprintf(stderr, _("mapscrn: libTw error: %s\n"), TwStrError(TwErrno));
	    exit(1);
	}
	
	loadnewmap(argv[1]);

	if (TwErrno) {
	    fprintf(stderr, _("mapscrn: libTw error: %s\n"), TwStrError(TwErrno));
	}
    
	TwClose();
    
	exit(0);
}

void
loadnewmap(char *mfil) {
	FILE *fp;
	struct stat stbuf;
	char buf[E_TABSZ];

	if ((fp = findfile(mfil, mapdirpath, mapsuffixes)) == NULL) {
	        fprintf(stderr, _("mapscrn: cannot open map file `%s'\n"), mfil);
		exit(1);
	}
	if (stat(pathname, &stbuf)) {
		perror(pathname);
		fprintf(stderr, _("Cannot stat map file"));
		exit(1);
	}
	if (stbuf.st_size != E_TABSZ) {
		fprintf(stderr,
			_("Loading symbolic screen map from file %s\n"),
			pathname);

		if (parsemap(fp,buf)) {
			fprintf(stderr, _("Error parsing symbolic map\n"));
			exit(1);
		}
	} else 	{
		fprintf(stderr, _("Loading binary screen map from file %s\n"),
			pathname);

		if (fread(buf,E_TABSZ,1,fp) != 1) {
			fprintf(stderr, _("Cannot read map from file"));
			exit(1);
		}
	}
	fpclose(fp);

	TwSetFontTranslation(buf+0x80);

	if (verbose)
	  printf(_("Loaded screen map from `%s'\n"), mfil);
}

static int
parsemap(FILE *fp, char buf[]) {
  char buffer[256];
  int in, on;
  char *p, *q;

  for (in=0; in<256; in++) buf[in]=in;

  while (fgets(buffer,sizeof(buffer)-1,fp)) {
      p = strtok(buffer," \t\n");
      if (p && *p != '#') {
	  q = strtok(NULL," \t\n#");
	  if (q) {
	      in = ctoi(p);
	      on = ctoi(q);
	      if (in >= 0 && on >= 0) buf[in] = on;
	  }
      }
  }
  return(0);
}

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

  else if ((strlen(s) == 3) && (s[0] == '\'') && (s[2] == '\''))
    i=s[1];

  else return(-1);

  if (i < 0 || i > 255) {
      fprintf(stderr, _("mapscrn: format error detected in `%s'\n"), s);
      exit(1);
  }

  return(i);
}

