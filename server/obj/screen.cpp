/*
 *  screen.cpp  --  define methods of server class s_widget
 *
 *  Copyright (C) 1993-2019 by Massimiliano Ghilardi
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 */

#include "obj/screen.h"
#include "alloc.h"   // CloneStrL(), CopyMem(), FreeMem()
#include "builtin.h" // Builtin_MsgPort

#include <Tw/datasizes.h> // TW_MAXDAT

screen s_screen::Create(fn_screen Fn, dat NameLen, const char *Name, dat BgWidth, dat BgHeight,
                        const tcell *Bg) {
  screen S = (screen)0;
  size_t size;

  if ((size = (size_t)BgWidth * BgHeight * sizeof(tcell))) {

    if ((S = (screen)s_widget::Create((fn_widget)Fn, Builtin_MsgPort, TW_MAXDAT, TW_MAXDAT, 0,
                                      SCREENFL_USEBG, 0, 0, Bg[0]))) {

      if (!(S->Name = NULL, Name) || (S->Name = CloneStrL(Name, NameLen))) {
        if ((S->USE.B.Bg = (tcell *)AllocMem(size))) {

          S->NameLen = NameLen;
          S->MenuWindow = S->ClickWindow = NULL;
          S->HookW = NULL;
          S->FnHookW = NULL;
          S->USE.B.BgWidth = BgWidth;
          S->USE.B.BgHeight = BgHeight;
          CopyMem(Bg, S->USE.B.Bg, size);
          S->All = NULL;

          return S;
        }
        if (S->Name)
          FreeMem(S->Name);
      }
      Fn->Fn_Widget->Delete((widget)S);
    }
  }
  return NULL;
}
