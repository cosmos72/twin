/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
#include <string.h>

#define VERSION "1.06"

char *progname;

TW_INLINE void
set_progname(char *name) {
	char *p;

	p = rindex(name, '/');
	progname = (p ? p+1 : name);
}

TW_INLINE void
print_version_and_exit(void) {
	printf(_("%s from %s\n"), progname, "kbd-" VERSION);
	exit(0);
}
