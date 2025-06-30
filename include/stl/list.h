/* Copyright (C) 2000-2020 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */
#ifndef TWIN_STL_LIST_H
#define TWIN_STL_LIST_H

/** invasive list of T */
template <class T> class List {

private:
public:
  // CONSTEXPR List() NOTHROW : First(), Last() { }
  // List(const List<T> &other) = default;
  // List<T> &operator=(const List<T> &other) = default;
  // ~List() = default;

  template <class OBJ> //
  void Insert(OBJ obj, OBJ prev, OBJ next, int *objcount = NULL) {
    if (obj->Prev || obj->Next) {
      return;
    }
    if (prev) {
      next = (OBJ)prev->Next;
    } else if (next) {
      prev = (OBJ)next->Prev;
    }
    if ((obj->Prev = prev) != NULL) {
      prev->Next = obj;
    } else {
      First = obj;
    }
    if ((obj->Next = next) != NULL) {
      next->Prev = obj;
    } else {
      Last = obj;
    }
    if (objcount) {
      ++(*objcount);
    }
  }

  template <class OBJ> //
  void Remove(OBJ obj, int *objcount = NULL) {
    if (obj->Prev) {
      obj->Prev->Next = obj->Next;
    } else if (First == obj) {
      First = (T)obj->Next;
    }
    if (obj->Next) {
      obj->Next->Prev = obj->Prev;
    } else if (Last == obj) {
      Last = (T)obj->Prev;
    }
    obj->Prev = obj->Next = NULL;
    if (objcount) {
      --(*objcount);
    }
  }

public:
  T First, Last;
};

#endif /* TWIN_STL_LIST_H */
