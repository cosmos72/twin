/* Copyright (C) 2022 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_LOG_H
#define TWIN_LOG_H

#include "stl/span.h"

class FmtBase;
struct to_chars_result;

enum log_level /*: unsigned char*/ { INFO, WARNING, ERROR };

class Logger {
public:
  Logger() NOTHROW : written(0) {
  }

  // write internal buffer to messages window
  ~Logger() NOTHROW;

  Logger &operator<<(signed char value) NOTHROW {
    return operator<<((long)value);
  }
  Logger &operator<<(short value) NOTHROW {
    return operator<<((long)value);
  }
  Logger &operator<<(int value) NOTHROW {
    return operator<<((long)value);
  }
  Logger &operator<<(long value) NOTHROW;

  Logger &operator<<(unsigned char value) NOTHROW {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned short value) NOTHROW {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned int value) NOTHROW {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned long value) NOTHROW;

  // also accepts string literals, thanks to implicit constructor
  // template<size_t N> View(const T (&addr)[N])
  Logger &operator<<(View<char> value) NOTHROW;

  Logger &operator<<(const FmtBase &value) NOTHROW;

private:
  Span<char> available_span() const NOTHROW;

private:
  size_t written;
};

inline Logger log(log_level /*level*/) {
  return Logger();
}

#endif /* TWIN_LOG_H */
