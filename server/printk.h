#ifndef _TWIN_PRINTK_H
#define _TWIN_PRINTK_H

int printk(CONST byte *format, ...);
void printk_str(int len, CONST byte *s);
int printk_receive_fd(int fd);
int flushk(void);

byte RegisterPrintk(int fd);
void UnRegisterPrintk(void);


#endif /* _TWIN_PRINTK_H */
