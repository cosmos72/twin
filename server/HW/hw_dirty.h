
#ifndef _TW_HW_DIRTY_H
#define _TW_HW_DIRTY_H


INLINE byte Plain_isDirtyVideo(dat X, dat Y) {
    dat s;
    s = ChangedVideo[Y][0][0];
    if (s == -1 || s > X || ChangedVideo[Y][0][1] < X) {
	s = ChangedVideo[Y][1][0];
	if (s == -1 || s > X || ChangedVideo[Y][1][1] < X)
	    return FALSE;
    }
    return TRUE;
}


/*
 * this is not trivial, as it must consider that areas nearer than
 * merge_Threshold will get merged together (by vcsa_FlushVideo())
 */
INLINE byte Threshold_isDirtyVideo(dat X, dat Y) {
    dat s1, s2, e1, e2, delta;
    
    if ((s1 = ChangedVideo[Y][0][0]) == -1)
	return FALSE;

    e1 = ChangedVideo[Y][0][1];
    if (s1 <= X && e1 >= X)
	return TRUE;

    if (e1 < X) {
	/* check the second slot */
	s2 = ChangedVideo[Y][1][0];
	if (s2 != -1) {
	    e2 = ChangedVideo[Y][1][1];
	    if (s2 <= X && e2 >= X)
		return TRUE;
	    if (s2 > X)
		return s2-e1 < HW->merge_Threshold;
	    s1 = s2;
	    e1 = e2;
	}
    }
    
    /* check for slots in the previous/next line */
    if (s1 > X && Y)
	delta = -1;
    else if (e1 < X && Y < DisplayHeight - 1)
	delta = 1;
    else
	return FALSE;
    
    s2 = ChangedVideo[Y+delta][delta<0][0];
    if (s2 == -1) {
	s2 = ChangedVideo[Y+delta][delta>0][0];
	if (s2 == -1)
	    return FALSE;
	e2 = ChangedVideo[Y+delta][delta>0][1];
    } else
	e2 = ChangedVideo[Y+delta][delta<0][1];
	
    if (delta < 0)
	return s1 + (DisplayWidth - e2) < HW->merge_Threshold;
    else
	return s2 + (DisplayWidth - e1) < HW->merge_Threshold;
}

/* VideoFlip is quite os-independent ;) */
INLINE void VideoFlip(udat x, udat y) {
    uldat pos = x + y * DisplayWidth;
    hwattr h = Video[pos];
    hwcol c = ~HWCOL(h) ^ COL(HIGH,HIGH);

    Video[pos] = HWATTR( c, HWFONT(h));
}

#endif /* _TW_HW_DIRTY_H */

