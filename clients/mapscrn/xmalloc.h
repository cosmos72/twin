/* Error-free versions of some libc routines */
extern void *xmalloc(size_t sz);
extern void *xrealloc(void *p, size_t sz);
extern char *xstrdup(char *p);
