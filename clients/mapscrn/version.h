#include <string.h>

#define VERSION "1.02"

char *progname;

static inline void
set_progname(char *name) {
	char *p;

	p = rindex(name, '/');
	progname = (p ? p+1 : name);
}

static inline void
print_version_and_exit(void) {
	printf(_("%s from %s\n"), progname, "kbd-" VERSION);
	exit(0);
}
