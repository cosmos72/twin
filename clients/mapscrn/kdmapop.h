/*  
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */
extern int getscrnmap(int fd, char *map);
extern int loadscrnmap(int fd, char *map);
extern int getuniscrnmap(int fd, unsigned short *map);
extern int loaduniscrnmap(int fd, unsigned short *map);
extern int getunimap(int fd, struct unimapdesc *ud);
extern int loadunimap(int fd, struct unimapinit *ui, struct unimapdesc *ud);
