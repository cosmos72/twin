/*
 *  hw_null.c  --  functions to let twin run with no display at all.
 *
 *  Copyright (C) 1999-2000 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "twin.h"
#include "main.h"
#include "data.h"

#include "hw.h"
#include "hw_private.h"


static display_hw null;

/*
 * dummy `null' display:
 * 
 * no keyboard input;
 * no mouse input;
 * no output.
 * 
 * It's *much* more useful than it seems,
 * as it allow twin to run without being attached
 * to some kind of hw display,
 * implementing detach-and-resume.
 */

/* no need for a big display... just wastes CPU and memory */
static void null_DetectSize(udat *x, udat *y) {
    *x = 1;
    *y = 1;
}


/* WARNING: we rely on null_InitHW() to _always_ succeed! */

display_hw *null_InitHW(byte *arg) {
    int fd = open("/dev/null", O_RDWR);
        
    close(0);
    close(1);
    dup2(fd, 0);
    dup2(fd, 1);
    close(fd);
    
    null.KeyboardEvent = (void *)NoOp;
    null.QuitKeyboard  = NoOp;

    All->keyboard_slot = NOSLOT;

    null.SoftMouse = FALSE;
    null.MouseEvent = (void *)NoOp;
    null.QuitMouse = NoOp;
	    
    All->mouse_slot = NOSLOT;
		
    null.DetectSize  = null_DetectSize;
    null.canDragArea = NULL;
    
    null.QuitVideo = NoOp;
    null.FlushVideo = NoOp;
    null.FlushHW = NULL;
    
    null.NeedOldVideo = FALSE;
    null.merge_Threshold = 0;
    null.ExpensiveFlushVideo = FALSE;
	    
    null.ShowMouse = NoOp;
    null.HideMouse = NoOp;

    null.MoveToXY = (void (*)(udat,udat))NoOp;
    null.SetCursorType = (void (*)(uldat))NoOp;
    null.Beep = NoOp;

    null.QuitHW = NoOp;
    
    /* save display size for later restoring (persistence) */
    saveDisplaySize();
    
    return &null;
}

