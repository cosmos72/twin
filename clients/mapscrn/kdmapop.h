extern int getscrnmap(int fd, char *map);
extern int loadscrnmap(int fd, char *map);
extern int getuniscrnmap(int fd, unsigned short *map);
extern int loaduniscrnmap(int fd, unsigned short *map);
extern int getunimap(int fd, struct unimapdesc *ud);
extern int loadunimap(int fd, struct unimapinit *ui, struct unimapdesc *ud);
