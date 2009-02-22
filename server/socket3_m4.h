/* Copyright (C) 2000 by Massimiliano Ghilardi
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */




/* This file was automatically generated from m4/socket3.m4, do not edit! */




































  case order_FindFunction:
    switch (n) {
      case 2: L = a[1]_any; break;
      case 4: L = a[3]_any; break;
    }
    break;
















  case order_AttachHW:
    switch (n) {
      case 2: L = a[1]_any; break;
    }
    break;

  case order_DetachHW:
    switch (n) {
      case 2: L = a[1]_any; break;
    }
    break;


  case order_SetFontTranslation:
    switch (n) {
      case 1: L = 0x80; break;
    }
    break;

  case order_SetHWFontTranslation:
    switch (n) {
      case 1: L = 0x80; break;
    }
    break;















  case order_DrawWidget:
    switch (n) {
      case 6: L = a[2]_any*a[3]_any; break;
      case 7: L = a[2]_any*a[3]_any; break;
      case 8: L = a[2]_any*a[3]_any; break;
    }
    break;








  case order_RestackChildrenWidget:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;



  case order_CreateGadget:
    switch (n) {
      case 4: L = a[2]_any*a[3]_any; break;
    }
    break;

  case order_CreateButtonGadget:
    switch (n) {
      case 4: L = a[2]_any*a[3]_any; break;
    }
    break;

  case order_WriteTextsGadget:
    switch (n) {
      case 5: L = a[2]_any*a[3]_any; break;
    }
    break;
  case order_WriteHWFontsGadget:
    switch (n) {
      case 5: L = a[2]_any*a[3]_any; break;
    }
    break;

  case order_CreateWindow:
    switch (n) {
      case 2: L = a[1]_any; break;
      case 3: L = a[1]_any; break;
    }
    break;


  case order_WriteAsciiWindow:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;
  case order_WriteStringWindow:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;
  case order_WriteHWFontWindow:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;
  case order_WriteHWAttrWindow:
    switch (n) {
      case 5: L = a[4]_any; break;
    }
    break;


  case order_SetTitleWindow:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;














  case order_RestackChildrenRow:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break; 


  case order_Create4MenuAny:
    switch (n) {
      case 6: L = a[5]_any; break;
    }
    break;




  case order_SetInfoMenu:
    switch (n) {
      case 4: L = a[3]_any; break;
      case 5: L = a[3]_any; break;
    }
    break;

  case order_CreateMsgPort:
    switch (n) {
      case 2: L = a[1]_any; break;
    }
    break;
  case order_FindMsgPort:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;

  case order_BgImageScreen:
    switch (n) {
      case 4: L = a[2]_any*a[3]_any; break;
    }
    break;



















  case order_SendToMsgPort:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;
  case order_BlindSendToMsgPort:
    switch (n) {
      case 3: L = a[2]_any; break;
    }
    break;




  case order_NotifySelection:
    switch (n) {
      case 4: L = TW_MAX_MIMELEN; break;
      case 6: L = a[5]_any; break;
    }
    break;



  case order_OpenExtension:
    switch (n) {
      case 2: L = a[1]_any; break;
    }
    break;
  case order_CallBExtension:
    switch (n) {
      case 3: L = a[2]_any; break;
      case 4: L = 2; break;
    }
    break;




