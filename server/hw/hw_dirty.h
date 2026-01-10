
#ifndef TWIN_HW_DIRTY_H
#define TWIN_HW_DIRTY_H

inline bool plainIsDirtyVideo(dat X, dat Y) {
  dat s;
  s = ChangedVideo[Y][0][0];
  if (s == -1 || s > X || ChangedVideo[Y][0][1] < X) {
    s = ChangedVideo[Y][1][0];
    if (s == -1 || s > X || ChangedVideo[Y][1][1] < X) {
      return false;
    }
  }
  return true;
}

/* VideoFlip is quite OS and driver independent ;) */
inline void VideoFlip(udat x, udat y) {
  tcell &c = Video[x + y * (ldat)DisplayWidth];
  c.color.fg ^= twhite;
  c.color.bg ^= twhite;
}

#endif /* TWIN_HW_DIRTY_H */
