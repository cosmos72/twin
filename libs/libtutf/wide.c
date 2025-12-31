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

#include <Tw/datatypes.h>

#include "wide.h"

unsigned Tutf_is_wide(trune r) {
  if (r <= 4351 || r >= 129785) {
    return 0;
  } else if (is_wide_coarse(r)) {
    return 1;
  } else if (is_narrow_coarse(r)) {
    return 0;
  }
  /** TODO: finish implementing */
  return (unsigned)-1;
}