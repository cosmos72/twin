
#ifndef _TT_FIRE_H
#define _TT_FIRE_H


#define FIRE_EVENT(call, o, which, value, old_value) do { \
    ttany _old_value = (ttany)(old_value); \
    (call); \
    FireChangeEvent((ttcomponent)(o), (which), (ttany)(value), (ttany)(_old_value), (ttopaque)0); \
} while(0)

#define FIRE_EVENTL(call, o, which, value, old_value, len) do { \
    ttany _old_value = (ttany)(old_value); \
    (call); \
    FireChangeEvent((ttcomponent)(o), (which), (ttany)(value), (ttany)(_old_value), (ttopaque)(len)); \
} while(0)


#endif /* _TT_FIRE_H */

