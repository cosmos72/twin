/*
 *  Twkeys.h  --  keycode #defines for libTw keyboard events
 *
 *  Copyright (C) 2000 by Massimiliano Ghilardi
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Library General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 */

#ifndef _TW_KEYS_H
#define _TW_KEYS_H


#define TW_Tab			0x09
#define TW_Linefeed		0x0A
#define TW_Return		0x0D
#define TW_Escape		0x1B
#define TW_space                0x20

/* no #defines for 0x21...0x7e chars: they are plain ASCII! */

/* Latin 1 */

#define TW_nobreakspace        0xa0
#define TW_exclamdown          0xa1
#define TW_cent        	       0xa2
#define TW_sterling            0xa3
#define TW_currency            0xa4
#define TW_yen                 0xa5
#define TW_brokenbar           0xa6
#define TW_section             0xa7
#define TW_diaeresis           0xa8
#define TW_copyright           0xa9
#define TW_ordfeminine         0xaa
#define TW_guillemotleft       0xab	/* left angle quotation mark */
#define TW_notsign             0xac
#define TW_hyphen              0xad
#define TW_registered          0xae
#define TW_macron              0xaf
#define TW_degree              0xb0
#define TW_plusminus           0xb1
#define TW_twosuperior         0xb2
#define TW_threesuperior       0xb3
#define TW_acute               0xb4
#define TW_mu                  0xb5
#define TW_paragraph           0xb6
#define TW_periodcentered      0xb7
#define TW_cedilla             0xb8
#define TW_onesuperior         0xb9
#define TW_masculine           0xba
#define TW_guillemotright      0xbb	/* right angle quotation mark */
#define TW_onequarter          0xbc
#define TW_onehalf             0xbd
#define TW_threequarters       0xbe
#define TW_questiondown        0xbf
#define TW_Agrave              0xc0
#define TW_Aacute              0xc1
#define TW_Acircumflex         0xc2
#define TW_Atilde              0xc3
#define TW_Adiaeresis          0xc4
#define TW_Aring               0xc5
#define TW_AE                  0xc6
#define TW_Ccedilla            0xc7
#define TW_Egrave              0xc8
#define TW_Eacute              0xc9
#define TW_Ecircumflex         0xca
#define TW_Ediaeresis          0xcb
#define TW_Igrave              0xcc
#define TW_Iacute              0xcd
#define TW_Icircumflex         0xce
#define TW_Idiaeresis          0xcf
#define TW_ETH                 0xd0
#define TW_Ntilde              0xd1
#define TW_Ograve              0xd2
#define TW_Oacute              0xd3
#define TW_Ocircumflex         0xd4
#define TW_Otilde              0xd5
#define TW_Odiaeresis          0xd6
#define TW_multiply            0xd7
#define TW_Ooblique            0xd8
#define TW_Ugrave              0xd9
#define TW_Uacute              0xda
#define TW_Ucircumflex         0xdb
#define TW_Udiaeresis          0xdc
#define TW_Yacute              0xdd
#define TW_THORN               0xde
#define TW_ssharp              0xdf
#define TW_agrave              0xe0
#define TW_aacute              0xe1
#define TW_acircumflex         0xe2
#define TW_atilde              0xe3
#define TW_adiaeresis          0xe4
#define TW_aring               0xe5
#define TW_ae                  0xe6
#define TW_ccedilla            0xe7
#define TW_egrave              0xe8
#define TW_eacute              0xe9
#define TW_ecircumflex         0xea
#define TW_ediaeresis          0xeb
#define TW_igrave              0xec
#define TW_iacute              0xed
#define TW_icircumflex         0xee
#define TW_idiaeresis          0xef
#define TW_eth                 0xf0
#define TW_ntilde              0xf1
#define TW_ograve              0xf2
#define TW_oacute              0xf3
#define TW_ocircumflex         0xf4
#define TW_otilde              0xf5
#define TW_odiaeresis          0xf6
#define TW_division            0xf7
#define TW_oslash              0xf8
#define TW_ugrave              0xf9
#define TW_uacute              0xfa
#define TW_ucircumflex         0xfb
#define TW_udiaeresis          0xfc
#define TW_yacute              0xfd
#define TW_thorn               0xfe
#define TW_ydiaeresis          0xff

/* end Latin 1 */


#define TW_BackSpace		0x7F

#define TW_Home			0x100
#define TW_Left			0x101
#define TW_Up			0x102
#define TW_Right		0x103
#define TW_Down			0x104
#define TW_Prior		0x105
#define TW_Page_Up		TW_Prior
#define TW_Next			0x106
#define TW_Page_Down		TW_Next
#define TW_End			0x107
#define TW_Begin		0x108
#define TW_Insert		0x109
#define TW_Delete		0x10A

#define TW_Pause		0x10B
#define TW_Scroll_Lock		0x10C
#define TW_Sys_Req		0x10D
#define TW_Clear		0x10E

#define TW_Select		0x110
#define TW_Print		0x111
#define TW_Execute		0x112
#define TW_Undo			0x115
#define TW_Redo			0x116
#define TW_Menu			0x117
#define TW_Find			0x118
#define TW_Cancel		0x119
#define TW_Help			0x11A
#define TW_Break		0x11B
#define TW_Caps_Lock		0x11C
#define TW_Mode_switch		TW_Caps_Lock
#define TW_Shift_Lock		0x11D
#define TW_Num_Lock		0x11E

#define TW_KP_Begin		0x11F
#define TW_KP_Home		0x120
#define TW_KP_Left		0x121
#define TW_KP_Up		0x122
#define TW_KP_Right		0x123
#define TW_KP_Down		0x124
#define TW_KP_Prior		0x125
#define TW_KP_Page_Up		TW_KP_Prior
#define TW_KP_Next		0x126
#define TW_KP_Page_Down		TW_KP_Next
#define TW_KP_End		0x127
#define TW_KP_Insert		0x128
#define TW_KP_Delete		0x129
#define TW_KP_Multiply		0x12A
#define TW_KP_Add		0x12B
#define TW_KP_Separator		0x12C
#define TW_KP_Subtract		0x12D
#define TW_KP_Decimal		0x12E
#define TW_KP_Divide		0x12F

#define TW_KP_Equal		0x130
#define TW_KP_F1		0x131
#define TW_KP_F2		0x132
#define TW_KP_F3		0x133
#define TW_KP_F4		0x134

#define TW_KP_Space		0x135
#define TW_KP_Tab		0x136
#define TW_KP_Enter		0x137

#define TW_KP_0			0x140
#define TW_KP_1			0x141
#define TW_KP_2			0x142
#define TW_KP_3			0x143
#define TW_KP_4			0x144
#define TW_KP_5			0x145
#define TW_KP_6			0x146
#define TW_KP_7			0x147
#define TW_KP_8			0x148
#define TW_KP_9			0x149

#define TW_F1			0x150
#define TW_F2			0x151
#define TW_F3			0x152
#define TW_F4			0x153
#define TW_F5			0x154
#define TW_F6			0x155
#define TW_F7			0x156
#define TW_F8			0x157
#define TW_F9			0x158
#define TW_F10			0x159
#define TW_F11			0x15A
#define TW_F12			0x15B
#define TW_F13			0x15C
#define TW_F14			0x15D
#define TW_F15			0x15E
#define TW_F16			0x15F
#define TW_F17			0x160
#define TW_F18			0x161
#define TW_F19			0x162
#define TW_F20			0x163

#define TW_Shift_L		0x164
#define TW_Shift_R		0x165
#define TW_Control_L		0x166
#define TW_Control_R		0x167
#define TW_Meta_L		0x168
#define TW_Meta_R		0x169
#define TW_Alt_L		0x16A
#define TW_Alt_R		0x16B
#define TW_Alt			TW_Alt_L
#define TW_AltGr		TW_Alt_R
#define TW_Super_L		0x16C
#define TW_Super_R		0x16D
#define TW_Hyper_L		0x16E
#define TW_Hyper_R		0x16F

#define TW_EcuSign		0x170
#define TW_ColonSign		0x171
#define TW_CruzeiroSign		0x172
#define TW_FFrancSign		0x173
#define TW_LiraSign		0x174
#define TW_MillSign		0x175
#define TW_NairaSign		0x176
#define TW_PesetaSign		0x177
#define TW_RupeeSign		0x178
#define TW_WonSign		0x179
#define TW_NewSheqelSign	0x17A
#define TW_DongSign		0x17B
#define TW_EuroSign		0x17C

/* a dummy value for xterm-style mouse reporting ASCII sequences */
#define TW_XTermMouse		0x1FE

#define TW_Null			0x1FF

#define MAX_KEY_CODE		(TW_Null+1)


#endif /* _TW_KEYS_H */

