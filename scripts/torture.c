
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <errno.h>
#include <unistd.h>

char *make;
char **conf_list;
int *conf_len;
int conf_n;

unsigned long bitmap; /* support at least up to 32 config entries */

static void *my_malloc(size_t len) {
    void *p;
    
    if ((p = malloc(len)))
	return p;
    exit(1);
}

static void my_dupargv(int argc, char **argv) {
    int i;
    char *s, *d;
    
    conf_len = (int *)my_malloc((argc + 1) * sizeof(int));
    conf_list = (char **)my_malloc((argc + 1) * sizeof(char *));

    for (; (s = *argv); argv++, conf_n++) {
	i = strlen(s);
	conf_len[conf_n] = i + 1;
	d = conf_list[conf_n] = (char *)my_malloc(i + 3);
	memcpy(d, s, i);
	memcpy(d + i, "=n", 3);
    }
    conf_len[conf_n] = 0;
    conf_list[conf_n] = NULL;
}

static int update_conf(void) {
    char *path = "conf/config.status";
    FILE *f = fopen(path, "w");
    char **p;
    int ret = 0;
    
    if (f) {
	ret |= !fprintf(f, "#\n"
			 "#This file is created by 'make torture', do not edit!\n"
			 "#\n"
			 "#Configuration:\n"
			 "#\n");
	for (p = conf_list; *p; p++)
	    ret |= !fprintf(f, "%s\n", *p);
	ret |= !!fclose(f);
	return !ret;
    }
    return 0;
}

static int increase_conf(void) {
    unsigned long b;
    int i;
    
    if (!++bitmap)
	/* overflow */
	return 0;
    
    for (b = bitmap, i = conf_n - 1; i >= 0; i--) {
	conf_list[i][conf_len[i]] = b & 1 ? 'y' : 'n';
	b >>= 1;
    }
    if (b)
	/* overflow */
	return 0;
    return 1;
}
    
#if 0
static int my_system(char *arg) {
    pid_t i;
    
    switch ((i = fork())) {
	
      case 0: /* child */
	fprintf(stderr, "%s %s\n", make, arg);
	fflush(stderr);
	execlp(make, make, arg, NULL);
	/* returned? error! fallthrough... */
      case (pid_t)-1:
	exit(1);
	break;
	
      default:
	/* parent */
	{
	    struct rusage r;
	    pid_t ret;
	    int status;

	    do {
		ret = wait4(i, &status, WNOHANG, &r);
	    } while (ret == (pid_t)-1 && (errno == EINTR || errno == EAGAIN));

	    if (WIFEXITED(status))
		return !WEXITSTATUS(status);
	    break;
	}
    }
    return 0;
}
#endif
static int my_system(char *arg) {
    char *buf = my_malloc(strlen(make) + strlen(arg) + 2);
    
    strcpy(buf, make);
    strcat(buf, " ");
    strcat(buf, arg);
    
    fprintf(stderr, "%s\n", buf);
    fflush(stderr);
    return !system(buf);
}

int main(int argc, char *argv[]) {

    --argc; make = *++argv; 
    my_dupargv(--argc, ++argv);

    do {
	if (!update_conf() || !my_system("nullconfig") || !my_system("all"))
	    return 1;
    } while (increase_conf());
    
    return 0;
}

