#undef TW_FEATURE_X11_XIM_XIC
#if defined(XLookupBoth) && defined(XLookupChars) && defined(XLookupKeySym)
# if defined(XNInputStyle) && defined(XIMStatusNothing) && defined(XIMPreeditNothing)
#  if defined(XNClientWindow) && defined(XNFocusWindow)
#   define TW_FEATURE_X11_XIM_XIC
#  endif
# endif
#endif

#undef TW_FEATURE_X11_Xutf8LookupString
#if defined(X_HAVE_UTF8_STRING)
# define TW_FEATURE_X11_Xutf8LookupString
#endif
