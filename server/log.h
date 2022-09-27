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

enum log_level /*: unsigned char*/ { INFO, WARNING, ERROR };

/// only for internal use by log() - do NOT call!
to_chars_result logv(log_level level, size_t arg_n, /* const FmtBase* */...);

template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4,
                           const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8,
                           const T9 &arg9) {

  return logv(level, CountFmtArgs<T1, T2, T3, T4, T5, T6, T7, T8, T9>::value, //
              &lvalue(fmt(arg1)), &lvalue(fmt(arg2)), &lvalue(fmt(arg3)),     //
              &lvalue(fmt(arg4)), &lvalue(fmt(arg5)), &lvalue(fmt(arg6)),     //
              &lvalue(fmt(arg7)), &lvalue(fmt(arg8)), &lvalue(fmt(arg9)));
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4,
                           const T5 &arg5, const T6 &arg6, const T7 &arg7, const T8 &arg8) {
  return log(level, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, Void());
}
template <class T1, class T2, class T3, class T4, class T5, class T6, class T7>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4,
                           const T5 &arg5, const T6 &arg6, const T7 &arg7) {
  return log(level, arg1, arg2, arg3, arg4, arg5, arg6, arg7, Void(), Void());
}
template <class T1, class T2, class T3, class T4, class T5, class T6>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4,
                           const T5 &arg5, const T6 &arg6) {
  return log(level, arg1, arg2, arg3, arg4, arg5, arg6, Void(), Void(), Void());
}
template <class T1, class T2, class T3, class T4, class T5>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4,
                           const T5 &arg5) {
  return log(level, arg1, arg2, arg3, arg4, arg5, Void(), Void(), Void(), Void());
}
template <class T1, class T2, class T3, class T4>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3, const T4 &arg4) {
  return log(level, arg1, arg2, arg3, arg4, Void(), Void(), Void(), Void(), Void());
}
template <class T1, class T2, class T3>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2, const T3 &arg3) {
  return log(level, arg1, arg2, arg3, Void(), Void(), Void(), Void(), Void(), Void());
}
template <class T1, class T2>
inline to_chars_result log(log_level level, //
                           const T1 &arg1, const T2 &arg2) {
  return log(level, arg1, arg2, Void(), Void(), Void(), Void(), Void(), Void(), Void());
}
template <class T1>
inline to_chars_result log(log_level level, //
                           const T1 &arg1) {
  return logv(level, IsVoid<T1>::value ? 0 : 1, &lvalue(fmt(arg1)));
}
inline to_chars_result log(log_level level) {
  return logv(level, 0);
}

#endif /* TWIN_LOG_H */
