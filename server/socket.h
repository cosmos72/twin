#ifndef _TW_SOCKET_H
#define _TW_SOCKET_H

typedef struct {
    byte Len, *name;
    void (*F)(void);
} sock_fn;

byte InitSocket(void);
void QuitSocket(void);

#endif /* _TW_SOCKET_H */
