/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
/*
 * All data is in subdirectories of DATADIR, by default /usr/lib/kbd
 * The following five subdirectories are defined:
 */
#define KEYMAPDIR "keymaps"
#define UNIMAPDIR "unimaps"
#define TRANSDIR "consoletrans"
#define VIDEOMODEDIR "videomodes"
#define FONTDIR "consolefonts"
/* subdir of the latter */
#define PARTIALDIR "partialfonts"
/* obsolete */
#define OLDKEYMAPDIR "keytables"

/*
 * Default keymap, and where the kernel copy of it lives.
 */
#ifdef __sparc__
# define DEFMAP "sunkeymap.map"
# define KERNDIR "/usr/src/linux/drivers/sbus/char"
#else
# define DEFMAP "defkeymap.map"
# define KERNDIR "/usr/src/linux/drivers/char"
#endif

extern FILE *findfile(char *fnam, char **dirpath, char **suffixes);
extern char pathname[];
extern void fpclose(FILE *fp);

extern int verbose;
