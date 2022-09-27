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

enum log_level : unsigned char { INFO, WARNING, ERROR };

/// only for internal use by log() - do NOT call!
to_chars_result logv(log_level level, size_t arg_n, /* const FmtBase* */...);

template <class T1 = Void, class T2 = Void, class T3 = Void, class T4 = Void, class T5 = Void,
          class T6 = Void, class T7 = Void, class T8 = Void, class T9 = Void>
inline to_chars_result
log(log_level level, //
    const T1 &arg1 = Void(), const T2 &arg2 = Void(), const T3 &arg3 = Void(),
    const T4 &arg4 = Void(), const T5 &arg5 = Void(), const T6 &arg6 = Void(),
    const T7 &arg7 = Void(), const T8 &arg8 = Void(), const T9 &arg9 = Void()) {

  return logv(level, CountFmtArgs<T1, T2, T3, T4, T5, T6, T7, T8, T9>::value, //
              &lvalue(fmt(arg1)), &lvalue(fmt(arg2)), &lvalue(fmt(arg3)),     //
              &lvalue(fmt(arg4)), &lvalue(fmt(arg5)), &lvalue(fmt(arg6)),     //
              &lvalue(fmt(arg7)), &lvalue(fmt(arg8)), &lvalue(fmt(arg9)));
}

#endif /* TWIN_LOG_H */
