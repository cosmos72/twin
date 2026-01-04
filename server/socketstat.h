/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#define TWScase4(objtype, fieldname, field, fieldtype)                                             \
  case CAT4(TWS_, objtype, _, fieldname):                                                          \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, fieldtype) <= TWS_tcolor || CAT(TWS_, fieldtype) >= TWS_highest ||               \
        AlienSizeof(fieldtype, ctx.Slot)) {                                                        \
      TSF->CAT(TWS_field_, fieldtype) = x->field;                                                  \
      TSF->type = CAT(TWS_, fieldtype);                                                            \
      break;                                                                                       \
    }                                                                                              \
    return tfalse

#define TWScase(objtype, field, fieldtype) TWScase4(objtype, field, field, fieldtype)

#define TWScaseUSE(objtype, use, fieldname, field, fieldtype)                                      \
  case CAT6(TWS_, objtype, _USE_, use, _, fieldname):                                              \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, fieldtype) <= TWS_tcolor || CAT(TWS_, fieldtype) >= TWS_highest ||               \
        AlienSizeof(fieldtype, ctx.Slot)) {                                                        \
      TSF->CAT(TWS_field_, fieldtype) = x->USE.use.field;                                          \
      TSF->type = CAT(TWS_, fieldtype);                                                            \
      break;                                                                                       \
    }                                                                                              \
    return tfalse

#define TWScaseA(objtype, field, n, fieldtype)                                                     \
  case CAT5(TWS_, objtype, _, field, V, n):                                                        \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, fieldtype) <= TWS_tcolor || CAT(TWS_, fieldtype) >= TWS_highest ||               \
        AlienSizeof(fieldtype, ctx.Slot)) {                                                        \
      TSF->CAT(TWS_field_, fieldtype) = x->field[n];                                               \
      TSF->type = CAT(TWS_, fieldtype);                                                            \
      break;                                                                                       \
    }                                                                                              \
    return tfalse

#define TWScaseAUSE(objtype, use, field, n, fieldtype)                                             \
  case CAT8(TWS_, objtype, _USE_, use, _, field, V, n):                                            \
    /* ensure type size WAS negotiated */                                                          \
    if (CAT(TWS_, fieldtype) <= TWS_tcolor || CAT(TWS_, fieldtype) >= TWS_highest ||               \
        AlienSizeof(fieldtype, ctx.Slot)) {                                                        \
      TSF->CAT(TWS_field_, fieldtype) = x->USE.use.field[n];                                       \
      TSF->type = CAT(TWS_, fieldtype);                                                            \
      break;                                                                                       \
    }                                                                                              \
    return tfalse

#define TWScasevec(objtype, field, _type, len) TWScasevec5(objtype, field, field, _type, len)

#define TWScasevec5(objtype, fieldname, field, _type, len)                                         \
  case CAT4(TWS_, objtype, _, fieldname):                                                          \
    if ((TSF->TWS_field_vecV = (void *)x->field)) {                                                \
      TSF->TWS_field_vecL = (uldat)sizeof(_type) * (len);                                          \
    }                                                                                              \
    TSF->type = TWS_vec | CAT(TWS_, _type);                                                        \
    break

#define TWScasevecUSE(objtype, use, field, _type, len)                                             \
  case CAT6(TWS_, objtype, _USE_, use, _, field):                                                  \
    if ((TSF->TWS_field_vecV = (void *)x->USE.use.field))                                          \
      TSF->TWS_field_vecL = (uldat)sizeof(_type) * (len);                                          \
    TSF->type = TWS_vec | CAT(TWS_, _type);                                                        \
    break

#define TWScaseAvec(objtype, field, n, _type, len)                                                 \
  case CAT6(TWS_, objtype, _, field, V, n):                                                        \
    if ((TSF->TWS_field_vecV = (void *)x->field[n]))                                               \
      TSF->TWS_field_vecL = (uldat)sizeof(_type) * (len);                                          \
    TSF->type = TWS_vec | CAT(TWS_, _type);                                                        \
    break

#define TWScaseAvecUSE(objtype, use, field, n, _type, len)                                         \
  case CAT8(TWS_, objtype, _USE_, use, _, field, V, n):                                            \
    if ((TSF->TWS_field_vecV = (void *)x->USE.use.field[n]))                                       \
      TSF->TWS_field_vecL = (uldat)sizeof(_type) * (len);                                          \
    TSF->type = TWS_vec | CAT(TWS_, _type);                                                        \
    break

#define fieldDelta(field) ((udat)(size_t)&(((TobjEntry)0)->field))
#define OFFSET_OF(type, field) ((udat)(size_t)&(((type)0)->field))

#define sockAllocListPrevObjs(F, len) sockAllocListDeltaObjs((F), (len), fieldDelta(Prev))
#define sockAllocListNextObjs(F, len) sockAllocListDeltaObjs((F), (len), fieldDelta(Next))
#define sockAllocListParentObjs(F, len) sockAllocListDeltaObjs((F), (len), fieldDelta(Parent))

static tobj *sockAllocListDeltaObjs(Tobj F, topaque *len, udat fdelta) {
  topaque L = 0;
  tobj *_LW, *LW;
  Tobj O = F;
  while (O) {
    L++;
    O = deserialize<Tobj>(O, fdelta);
  }
  if ((_LW = LW = (tobj *)AllocMem(L *= sizeof(tobj)))) {
    *len = L;
    O = F;
    while (O) {
      *LW++ = O->Id;
      O = deserialize<Tobj>(O, fdelta);
    }
  }
  return _LW;
}

static byte sockStatObj(SockCtx &ctx, Tobj o, tsfield TSF) {
  TobjEntry x = (TobjEntry)o;
  (void)ctx;
  switch (TSF->label) {
  case TWS_obj_Id:
    break;
  case TWS_obj_Prev:
    x = x->Prev;
    break;
  case TWS_obj_Next:
    x = x->Next;
    break;
  case TWS_obj_Parent:
    x = x->Parent;
    break;
  case TWS_obj_Prev_List:
    TSF->TWS_field_vecV = sockAllocListPrevObjs(x->Prev, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    return ttrue;
  case TWS_obj_Next_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Next, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    return ttrue;
  case TWS_obj_Parent_List:
    TSF->TWS_field_vecV = sockAllocListParentObjs(x->Parent, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    return ttrue;
  default:
    return tfalse;
  }
  TSF->TWS_field_obj = x;
  TSF->type = TWS_obj;
  return ttrue;
}

static byte sockStatWidget(SockCtx &ctx, Twidget x, tsfield TSF) {
  switch (TSF->label) {
    TWScase4(widget, FirstW, Widgets.First, obj);
    TWScase4(widget, LastW, Widgets.Last, obj);
    TWScase(widget, SelectW, obj);
    TWScase(widget, Left, dat);
    TWScase(widget, Up, dat);
    TWScase(widget, Attr, uldat);
    TWScase(widget, Flags, uldat);
    TWScase(widget, XWidth, dat);
    TWScase(widget, YWidth, dat);
    TWScase(widget, XLogic, ldat);
    TWScase(widget, YLogic, ldat);
    TWScase(widget, O_Prev, obj);
    TWScase(widget, O_Next, obj);
    TWScase(widget, Owner, obj);
    TWScase(widget, USE_Fill, tcell);
  case TWS_widget_ChildrenW_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Widgets.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_widget_O_Prev_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->O_Prev, &TSF->TWS_field_vecL, OFFSET_OF(Twidget, O_Prev));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_widget_O_Next_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->O_Next, &TSF->TWS_field_vecL, OFFSET_OF(Twidget, O_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  /* correct for Tscreen scrolling */
  if (x->Parent && IS_SCREEN(x->Parent)) {
    if (TSF->label == TWS_widget_Left)
      TSF->TWS_field_scalar -= x->Parent->XLogic;
    else if (TSF->label == TWS_widget_Up)
      TSF->TWS_field_scalar -= x->Parent->YLogic;
  }
  return ttrue;
}

static byte sockStatGadget(SockCtx &ctx, Tgadget x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(gadget, ColText, tcolor);
    TWScase(gadget, ColSelect, tcolor);
    TWScase(gadget, ColDisabled, tcolor);
    TWScase(gadget, ColSelectDisabled, tcolor);
    TWScase(gadget, Code, udat);
    TWScase(gadget, Flags, udat);
    TWScase(gadget, G_Prev, obj);
    TWScase(gadget, G_Next, obj);
    TWScase(gadget, Group, obj);
  case TWS_gadget_G_Prev_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->G_Prev, &TSF->TWS_field_vecL, OFFSET_OF(Tgadget, G_Prev));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_gadget_G_Next_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->G_Next, &TSF->TWS_field_vecL, OFFSET_OF(Tgadget, G_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    if (G_USE((Tgadget)x, USETEXT)) {
      switch (TSF->label) {
        TWScaseAvecUSE(gadget, T, Text, 0, trune, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Text, 1, trune, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Text, 2, trune, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Text, 3, trune, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Color, 0, tcolor, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Color, 1, tcolor, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Color, 2, tcolor, x->XWidth * x->YWidth);
        TWScaseAvecUSE(gadget, T, Color, 3, tcolor, x->XWidth * x->YWidth);
      default:
        return tfalse;
      }
    } else
      return tfalse;
    break;
  }
  return ttrue;
}

static byte sockStatWindow(SockCtx &ctx, Twindow x, tsfield TSF) {
  switch (TSF->label) {
  case TWS_widget_Left:
  case TWS_widget_Up:
  case TWS_widget_XWidth:
  case TWS_widget_YWidth:
    sockStatWidget(ctx, (Twidget)x, TSF);
    break;

    TWScase(window, Menu, obj);
    TWScase4(window, NameLen, Name.size(), dat);
    TWScasevec5(window, Name, Name.data(), byte, x->Name.size());
    TWScasevec(window, ColName, tcolor, x->Name.size());
    TWScaseAvec(window, BorderPattern, 0, trune, 9);
    TWScaseAvec(window, BorderPattern, 1, trune, 9);
    TWScase(window, CurX, ldat);
    TWScase(window, CurY, ldat);
    TWScase(window, XstSel, ldat);
    TWScase(window, YstSel, ldat);
    TWScase(window, XendSel, ldat);
    TWScase(window, YendSel, ldat);
    TWScase(window, MenuItem, obj);

    TWScase(window, ColGadgets, tcolor);
    TWScase(window, ColArrows, tcolor);
    TWScase(window, ColBars, tcolor);
    TWScase(window, ColTabs, tcolor);
    TWScase(window, ColBorder, tcolor);
    TWScase(window, ColText, tcolor);
    TWScase(window, ColSelect, tcolor);
    TWScase(window, ColDisabled, tcolor);
    TWScase(window, ColSelectDisabled, tcolor);
    TWScase(window, State, uldat);
    TWScase(window, CursorType, uldat);
    TWScase(window, MinXWidth, dat);
    TWScase(window, MinYWidth, dat);
    TWScase(window, MaxXWidth, dat);
    TWScase(window, MaxYWidth, dat);
    TWScase(window, WLogic, ldat);
    TWScase(window, HLogic, ldat);
  default:
    if (W_USE((Twindow)x, USECONTENTS)) {
      switch (TSF->label) {
        TWScasevecUSE(window, C, Contents, tcell, x->WLogic * x->HLogic);
        TWScaseUSE(window, C, HSplit, HSplit, ldat);
      default:
        return tfalse;
      }
    } else if (W_USE((Twindow)x, USEROWS)) {
      switch (TSF->label) {
        TWScaseUSE(window, R, FirstRow, Rows.First, obj);
        TWScaseUSE(window, R, LastRow, Rows.Last, obj);
      case TWS_window_USE_R_ChildrenRow_List:
        TSF->TWS_field_vecV = sockAllocListNextObjs(x->USE.R.Rows.First, &TSF->TWS_field_vecL);
        TSF->type = TWS_vec | TWS_tobj;
        break;
      default:
        return tfalse;
      }
    } else
      return tfalse;
    break;
  }
  /* correct for window borders */
  if (!(x->Flags & WINDOWFL_BORDERLESS)) {
    if (TSF->label == TWS_widget_Left || TSF->label == TWS_widget_Up)
      TSF->TWS_field_scalar++;
    else if (TSF->label == TWS_widget_XWidth || TSF->label == TWS_widget_YWidth)
      TSF->TWS_field_scalar -= 2;
  }
  return ttrue;
}

static byte sockStatScreen(SockCtx &ctx, Tscreen x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(screen, NameLen, dat);
    TWScasevec(screen, Name, byte, x->NameLen);
  default:
    if (S_USE((Tgadget)x, USEBG)) {
      switch (TSF->label) {
        TWScaseUSE(screen, B, BgWidth, BgWidth, dat);
        TWScaseUSE(screen, B, BgHeight, BgHeight, dat);
        TWScasevecUSE(screen, B, Bg, tcell, x->USE.B.BgWidth * x->USE.B.BgHeight);
      default:
        return tfalse;
      }
    } else
      return tfalse;
    break;
  }
  return ttrue;
}

static byte sockStatGroup(SockCtx &ctx, Tgroup x, tsfield TSF) {
  switch (TSF->label) {
    TWScase4(group, FirstG, Gadgets.First, obj);
    TWScase4(group, LastG, Gadgets.Last, obj);
    TWScase(group, SelectG, obj);
  case TWS_group_ChildrenG_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->Gadgets.First, &TSF->TWS_field_vecL, OFFSET_OF(Tgadget, G_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatRow(SockCtx &ctx, Trow x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(row, Code, udat);
    TWScase(row, Flags, byte);
    TWScase(row, Len, uldat);
    TWScasevec(row, Text, trune, x->Len);
    TWScasevec(row, ColText, tcolor, x->Len);
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatMenuItem(SockCtx &ctx, Tmenuitem x, tsfield TSF) {
  switch (TSF->label) {
    /* missing: */
#if 0
        TWS_menuitem_FlagActive;/*TWS_row_Flags*/
        TWS_menuitem_NameLen;        /*TWS_row_Len*/
        TWS_menuitem_Name;        /*TWS_row_Text*/
#endif
    TWScase(menuitem, Window, obj);
    TWScase(menuitem, Left, dat);
    TWScase(menuitem, ShortCut, dat);
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatMenu(SockCtx &ctx, Tmenu x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(menu, ColItem, tcolor);
    TWScase(menu, ColSelect, tcolor);
    TWScase(menu, ColDisabled, tcolor);
    TWScase(menu, ColSelectDisabled, tcolor);
    TWScase(menu, ColShtCut, tcolor);
    TWScase(menu, ColSelShtCut, tcolor);
    TWScase(menu, CommonItems, byte);
    TWScase(menu, FlagDefColInfo, byte);
    TWScase4(menu, FirstI, Items.First, obj);
    TWScase4(menu, LastI, Items.Last, obj);
    TWScase(menu, SelectI, obj);
  case TWS_menu_ChildrenI_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Items.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatMsgPort(SockCtx &ctx, Tmsgport x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(msgport, WakeUp, byte);
    TWScase(msgport, NameLen, byte);
    TWScasevec(msgport, Name, byte, x->NameLen);
    TWScase4(msgport, FirstMenu, Menus.First, obj);
    TWScase4(msgport, LastMenu, Menus.Last, obj);
    TWScase4(msgport, FirstW, Widgets.First, obj);
    TWScase4(msgport, LastW, Widgets.Last, obj);
    TWScase4(msgport, FirstGroup, Groups.First, obj);
    TWScase4(msgport, LastGroup, Groups.Last, obj);
    TWScase4(msgport, FirstMutex, Mutexes.First, obj);
    TWScase4(msgport, LastMutex, Mutexes.Last, obj);
  case TWS_msgport_ChildrenMenu_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Menus.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_msgport_ChildrenW_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->Widgets.First, &TSF->TWS_field_vecL, OFFSET_OF(Twidget, O_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_msgport_ChildrenGroup_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Groups.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_msgport_ChildrenMutex_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->Mutexes.First, &TSF->TWS_field_vecL, OFFSET_OF(Tmutex, O_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatMutex(SockCtx &ctx, Tmutex x, tsfield TSF) {
  switch (TSF->label) {
    TWScase(mutex, O_Prev, obj);
    TWScase(mutex, O_Next, obj);
    TWScase(mutex, Owner, obj);
    TWScase(mutex, Perm, byte);
    TWScase(mutex, NameLen, byte);
    TWScasevec(mutex, Name, byte, x->NameLen);
  case TWS_mutex_O_Prev_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->O_Prev, &TSF->TWS_field_vecL, OFFSET_OF(Tmutex, O_Prev));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_mutex_O_Next_List:
    TSF->TWS_field_vecV =
        sockAllocListDeltaObjs(x->O_Next, &TSF->TWS_field_vecL, OFFSET_OF(Tmutex, O_Next));
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  return ttrue;
}

static byte sockStatAll(SockCtx &ctx, Tall x, tsfield TSF) {
  switch (TSF->label) {
    TWScase4(all, FirstScreen, Screens.First, obj);
    TWScase4(all, LastScreen, Screens.Last, obj);
    TWScase4(all, FirstMsgPort, MsgPorts.First, obj);
    TWScase4(all, LastMsgPort, MsgPorts.Last, obj);
    TWScase4(all, FirstMutex, Mutexes.First, obj);
    TWScase4(all, LastMutex, Mutexes.Last, obj);
    TWScase(all, DisplayWidth, dat);
    TWScase(all, DisplayHeight, dat);
    TWScase(all, BuiltinMenu, obj);
    TWScase(all, CommonMenu, obj);
  case TWS_all_ChildrenScreen_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Screens.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_all_ChildrenMsgPort_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->MsgPorts.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  case TWS_all_ChildrenMutex_List:
    TSF->TWS_field_vecV = sockAllocListNextObjs(x->Mutexes.First, &TSF->TWS_field_vecL);
    TSF->type = TWS_vec | TWS_tobj;
    break;
  default:
    return tfalse;
  }
  return ttrue;
}

static void sockStat(SockCtx &ctx, Tobj x, udat n, const byte *in) {
  udat i, j;
  tsfield TSF;
  uldat len, q;
  byte *data, ok;

  ok = x && n && (TSF = (tsfield)AllocMem(n * sizeof(*TSF)));

  if (ok) {
    for (i = j = 0; j < n; j++) {
      Pop(in, udat, TSF[i].label);
      switch (x->Id >> class_byte_shift) {
      case Twidget_class_byte:
        ok = sockStatWidget(ctx, (Twidget)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      case Tgadget_class_byte:
        ok = sockStatGadget(ctx, (Tgadget)x, TSF + i) || sockStatWidget(ctx, (Twidget)x, TSF + i) ||
             sockStatObj(ctx, x, TSF + i);
        break;
      case Twindow_class_byte:
        ok = sockStatWindow(ctx, (Twindow)x, TSF + i) || sockStatWidget(ctx, (Twidget)x, TSF + i) ||
             sockStatObj(ctx, x, TSF + i);
        break;
      case Tscreen_class_byte:
        ok = sockStatScreen(ctx, (Tscreen)x, TSF + i) || sockStatWidget(ctx, (Twidget)x, TSF + i) ||
             sockStatObj(ctx, x, TSF + i);
        break;
      case Tgroup_class_byte:
        ok = sockStatGroup(ctx, (Tgroup)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      case Tmenuitem_class_byte:
        ok = sockStatMenuItem(ctx, (Tmenuitem)x, TSF + i) || sockStatRow(ctx, (Trow)x, TSF + i) ||
             sockStatObj(ctx, x, TSF + i);
        break;
      case Tmenu_class_byte:
        ok = sockStatMenu(ctx, (Tmenu)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      case Tmsgport_class_byte:
        ok = sockStatMsgPort(ctx, (Tmsgport)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      case Tmutex_class_byte:
        ok = sockStatMutex(ctx, (Tmutex)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      case Tall_class_byte:
        ok = sockStatAll(ctx, (Tall)x, TSF + i) || sockStatObj(ctx, x, TSF + i);
        break;
      default:
        ok = tfalse;
        break;
      }
      if (ok)
        i++;
    }

    len = 2 * (j = i) * sizeof(udat) + 2 * sizeof(udat);

    for (i = 0; i < j; i++) {
      if (TSF[i].type < TWS_highest) {
        len += TwinMagicData[TSF[i].type];
      } else if (TSF[i].type == TWS_obj) {
        TSF[i].TWS_field_tobj = TSF[i].TWS_field_obj ? TSF[i].TWS_field_obj->Id : NOID;
        len += sizeof(tobj);
      } else {
        if (!TSF[i].TWS_field_vecV) {
          TSF[i].TWS_field_vecL = 0;
        }
        len += TSF[i].TWS_field_vecL + sizeof(uldat);
      }
    }

    ok = sockReply(ctx, OK_MAGIC, len, NULL)           //
         && (data = RemoteWriteGetQueue(ctx.Slot, &q)) //
         && q >= len;

    if (ok) {
      i = 0;
      data += q - len;

      Push(data, udat, j);
      Push(data, udat, i); /* pad */

      for (; i < j; i++) {
        Push(data, udat, TSF[i].label);
        Push(data, udat, TSF[i].type);

        switch (TSF[i].type) {
#if TW_IS_LITTLE_ENDIAN
#define Pushcase(type)                                                                             \
  case CAT(TWS_, type):                                                                            \
    Push(data, type, TSF[i].CAT(TWS_field_, type));                                                \
    break
#else
#define Pushcase(type)                                                                             \
  case CAT(TWS_, type): {                                                                          \
    /* avoid padding problems on big-endian machines */                                            \
    type f = (type)TSF[i].CAT(TWS_field_, type);                                                   \
    Push(data, type, f);                                                                           \
  } break
#endif
          Pushcase(byte);
          Pushcase(udat);
          Pushcase(uldat);
          Pushcase(tcolor);
          Pushcase(topaque);
          Pushcase(tany);
          Pushcase(trune);
          Pushcase(tcell);
          Pushcase(tobj);
#undef Pushcase
        default:
          Push(data, uldat, TSF[i].TWS_field_vecL);
          if (TSF[i].TWS_field_vecL)
            PushV(data, TSF[i].TWS_field_vecL, TSF[i].TWS_field_vecV);
          break;
        }

        if (TSF[i].type == (TWS_vec | TWS_tobj) && TSF[i].TWS_field_vecV)
          FreeMem(TSF[i].TWS_field_vecV);
      }
    }

    FreeMem(TSF);
  }
  if (!ok) {
    sockReply(ctx, 1, 0, NULL);
  }
}
