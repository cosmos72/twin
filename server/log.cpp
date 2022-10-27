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
#include "stl/fmt.h"

Logger::~Logger() NOTHROW {
  try {
    if (written != 0) {
      printk_str(printk_buf, written);
    }
  } catch (...) {
  }
}

Span<char> Logger::available_span() const NOTHROW {
  return Span<char>(printk_buf + written, sizeof(printk_buf) - written);
}

Logger &Logger::operator<<(long value) NOTHROW {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(unsigned long value) NOTHROW {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(View<char> value) NOTHROW {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(const FmtBase &value) NOTHROW {
  written += value.write_to(available_span()).written;
  return *this;
}
