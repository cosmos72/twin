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
class to_chars_result;

enum log_level /*: unsigned char*/ { INFO, WARNING, ERROR };

class Logger {
public:
  Logger() : written(0) {
  }

  // write internal buffer to messages window
  ~Logger();

  Logger &operator<<(signed char value) {
    return operator<<((long)value);
  }
  Logger &operator<<(short value) {
    return operator<<((long)value);
  }
  Logger &operator<<(int value) {
    return operator<<((long)value);
  }
  Logger &operator<<(long value);

  Logger &operator<<(unsigned char value) {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned short value) {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned int value) {
    return operator<<((unsigned long)value);
  }
  Logger &operator<<(unsigned long value);

  // also accepts string literals, thanks to implicit constructor
  // template<size_t N> View(const T (&addr)[N])
  Logger &operator<<(View<char> value);

  Logger &operator<<(const FmtBase &value);

private:
  Span<char> available_span() const;

private:
  size_t written;
};

inline Logger log(log_level /*level*/) {
  return Logger();
}

#endif /* TWIN_LOG_H */
