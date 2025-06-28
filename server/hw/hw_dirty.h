
#ifndef TWIN_HW_DIRTY_H
#define TWIN_HW_DIRTY_H

inline bool Plain_isDirtyVideo(dat X, dat Y) {
  dat s;
  s = ChangedVideo[Y][0][0];
  if (s == -1 || s > X || ChangedVideo[Y][0][1] < X) {
    s = ChangedVideo[Y][1][0];
    if (s == -1 || s > X || ChangedVideo[Y][1][1] < X)
      return tfalse;
  }
  return ttrue;
}

/* VideoFlip is quite OS and driver independent ;) */
inline void VideoFlip(udat x, udat y) {
  Video[x + y * (ldat)DisplayWidth] ^= TCELL(TCOL(twhite, twhite), 0);
}

#endif /* TWIN_HW_DIRTY_H */
