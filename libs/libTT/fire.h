/*
 *
 *  Copyright (C) 2002 by Massimiliano Ghilardi
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 */

#ifndef _TT_FIRE_H
#define _TT_FIRE_H


#define FIRE_EVENT_O(call, o, which, value, old_value) \
	FIRE_EVENT(call, o, which, OBJ2ID(value), OBJ2ID(old_value))

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

