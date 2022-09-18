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

#include "stl/fmt.h"

#include <utility> // std:forward()
#include <initializer_list>

enum log_level : unsigned char { INFO, WARNING, ERROR };

to_chars_result logv(log_level level, View<const FmtBase *> args);

inline to_chars_result logl(log_level level, std::initializer_list<const FmtBase *> args) {
  return logv(level, View<const FmtBase *>(args.begin(), args.size()));
}

template <class... T> inline to_chars_result log(log_level level, T &&...args) {
  return logl(level, {&lvalue(fmt(std::forward<T>(args)))...});
}

#endif /* TWIN_LOG_H */
