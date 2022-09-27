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

#include <cstdarg>

to_chars_result logv(log_level level, size_t arg_n, /* const FmtBase* */...) {
  std::va_list vargs;
  const size_t end = sizeof(printk_buf); // TW_BIGBUFF
  Span<char> out(printk_buf, end);
  size_t written = 0;
  errnum err = SUCCESS;
  va_start(vargs, arg_n);
  for (size_t i = 0; i < arg_n; ++i) {
    const FmtBase *fmt = va_arg(vargs, const FmtBase *);
    if (fmt) {
      to_chars_result res = fmt->write_to(out.span(written, end));
      written += res.written;
      if ((err = res.err) != SUCCESS) {
        break;
      }
    }
  }
  va_end(vargs);
  if (written != 0 && err == SUCCESS) {
    printk_str(printk_buf, written);
  }
  return to_chars_result(written, err);
}
