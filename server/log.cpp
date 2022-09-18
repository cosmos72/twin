/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#include "log.h"
#include "printk.h" // printk_buf[], printk_str()

to_chars_result logv(log_level level, View<const FmtBase *> args) {
  const size_t end = sizeof(printk_buf); // TW_BIGBUFF
  Span<char> out(printk_buf, end);
  size_t written = 0;
  errnum err = SUCCESS;
  for (const FmtBase *arg : args) {
    if (arg) {
      to_chars_result res = arg->write_to(out.span(written, end));
      written += res.written;
      if ((err = res.err) != SUCCESS) {
        break;
      }
    }
  }
  if (written != 0 && err == SUCCESS) {
    printk_str(printk_buf, written);
  }
  return to_chars_result{written, err};
}
