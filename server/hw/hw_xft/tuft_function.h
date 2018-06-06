static Tutf_function X11_UTF_32_to_charset_function(CONST byte *charset) {
    // this is sufficient for xft fonts which are 16-bit unicode
    return X11_UTF_32_to_UCS_2;
}
