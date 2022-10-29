/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_TYPE_H
#define TWIN_STL_TYPE_H

#include <cstddef> // size_t
#include <new>     // placement new

// --------------------------- TypeId --------------------------------
class TypeId {
protected:
  struct Impl {
    void (*construct)(const void *src, void *dst);
    void (*destroy)(void *addr);
    size_t size;
  };
  const Impl *impl;

  explicit TypeId(const Impl *ptr) : impl(ptr) {
  }

  ~TypeId() {
  }

public:
  bool operator==(const TypeId &other) const {
    return impl == other.impl;
  }

  size_t size() const {
    return impl->size;
  }

  void construct(const void *src, void *dst) {
    impl->construct(src, dst);
  }

  void destroy(void *addr) {
    impl->destroy(addr);
  }

  void copy(const void *src, void *dst) {
    impl->destroy(dst);
    impl->construct(src, dst);
  }
};

// --------------------------- Type<T> --------------------------------
template <class T = void> class Type : public TypeId {
private:
  static const Impl *getImpl() {
    static Impl instance(construct_T, destroy_T, assign_T, sizeof(T));
    return &instance;
  }
  static void construct_T(const void *src, void *dst) {
    new (dst) T(*reinterpret_cast<const T *>(src));
  }
  static void destroy_T(void *addr) {
    reinterpret_cast<T *>(addr)->~T();
  }

public:
  Type() : TypeId(getImpl()) {
  }

  ~Type() {
  }
};

// --------------------------- Type<void> --------------------------------
template <> class Type<void> : public TypeId {
private:
  static const Impl *getImpl() {
    static Impl instance(construct_T, destroy_T, assign_T, 0);
    return &instance;
  }
  static void construct_T(const void *src, void *dst) {
  }
  static void destroy_T(void *addr) {
  }

public:
  Type() : TypeId(getImpl()) {
  }

  ~Type() {
  }
};

#endif /* TWIN_STL_TYPE_H */
