#undef TW_HAVE_X11_XIM_XIC
#if defined(XLookupBoth) && defined(XLookupChars) && defined(XLookupKeySym)
# if defined(XNInputStyle) && defined(XIMStatusNothing) && defined(XIMPreeditNothing)
#  if defined(XNClientWindow) && defined(XNFocusWindow)
#   define TW_HAVE_X11_XIM_XIC
#  endif
# endif
#endif
