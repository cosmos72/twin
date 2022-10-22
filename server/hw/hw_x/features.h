#undef TW_FEATURE_X11_XIM_XIC
#if defined(XLookupBoth) && defined(XLookupChars) && defined(XLookupKeySym)
#if defined(XNInputStyle) && defined(XIMStatusNothing) && defined(XIMPreeditNothing)
#if defined(XNClientWindow) && defined(XNFocusWindow)
#define TW_FEATURE_X11_XIM_XIC
#endif
#endif
#endif

#undef TW_FEATURE_X11_Xutf8LookupString
#if defined(X_HAVE_UTF8_STRING)
#define TW_FEATURE_X11_Xutf8LookupString
#endif

#if HW_X_DRIVER == HW_X11
#define XSYM(name) x11_##name
#elif HW_X_DRIVER == HW_XFT
#define XSYM(name) xft_##name
#else
#error invalid HW_X_DRIVER value: must be either HW_X11 or HW_XFT
#endif

#define XSTR_(s) #s
#define XSTR(s) XSTR_(s)
#define XSYM_STR(s) XSTR(XSYM(s))
