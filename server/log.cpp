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

Logger::~Logger() {
  try {
    if (written != 0) {
      printk_str(printk_buf, written);
    }
  } catch (...) {
  }
}

Span<char> Logger::available_span() const {
  return Span<char>(printk_buf + written, sizeof(printk_buf) - written);
}

Logger &Logger::operator<<(long value) {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(unsigned long value) {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(View<char> value) {
  written += to_chars(available_span(), value).written;
  return *this;
}
Logger &Logger::operator<<(const FmtBase &value) {
  written += value.write_to(available_span()).written;
  return *this;
}
