
#ifndef _TWIN_HW_DIRTY_H
#define _TWIN_HW_DIRTY_H

INLINE byte Plain_isDirtyVideo(dat X, dat Y) {
  dat s;
  s = ChangedVideo[Y][0][0];
  if (s == -1 || s > X || ChangedVideo[Y][0][1] < X) {
    s = ChangedVideo[Y][1][0];
    if (s == -1 || s > X || ChangedVideo[Y][1][1] < X)
      return tfalse;
  }
  return ttrue;
}

/*
 * this is not trivial, as it must consider that areas nearer than
 * merge_Threshold will get merged together (by linux_FlushVideo() for example)
 */
INLINE byte Threshold_isDirtyVideo(dat X, dat Y) {
  dat s1, s2, e1, e2, delta;

  if ((s1 = ChangedVideo[Y][0][0]) == -1)
    return tfalse;

  e1 = ChangedVideo[Y][0][1];
  if (s1 <= X && e1 >= X)
    return ttrue;

  if (e1 < X) {
    /* check the second slot */
    s2 = ChangedVideo[Y][1][0];
    if (s2 != -1) {
      e2 = ChangedVideo[Y][1][1];
      if (s2 <= X && e2 >= X)
        return ttrue;
      if (s2 > X)
        return s2 - e1 < HW->merge_Threshold;
      s1 = s2;
      e1 = e2;
    }
  }

  /* check for slots in the previous/next line */
  if (s1 > X && Y != 0)
    delta = -1;
  else if (e1 < X && Y < DisplayHeight - 1)
    delta = 1;
  else
    return tfalse;

  s2 = ChangedVideo[Y + delta][delta < 0][0];
  if (s2 == -1) {
    s2 = ChangedVideo[Y + delta][delta > 0][0];
    if (s2 == -1)
      return tfalse;
    e2 = ChangedVideo[Y + delta][delta > 0][1];
  } else
    e2 = ChangedVideo[Y + delta][delta < 0][1];

  if (delta < 0)
    return s1 + (DisplayWidth - e2) < HW->merge_Threshold;
  else
    return s2 + (DisplayWidth - e1) < HW->merge_Threshold;
}

/* VideoFlip is quite OS and driver independent ;) */
INLINE void VideoFlip(udat x, udat y) {
  Video[x + y * (ldat)DisplayWidth] ^= HWATTR(COL(WHITE, WHITE), 0);
}

#endif /* _TWIN_HW_DIRTY_H */
