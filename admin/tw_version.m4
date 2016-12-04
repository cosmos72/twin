m4_define([TW_VERSION], [

[
PACKAGE_VERSION_MAJOR="`echo \"$PACKAGE_VERSION\" | $SED 's|^\([0-9]*\)\..*|\1|g'`"
PACKAGE_VERSION_MINOR="`echo \"$PACKAGE_VERSION\" | $SED 's|^\([0-9]*\)\.\([0-9]*\)\..*|\2|g'`"
PACKAGE_VERSION_PATCH="`echo \"$PACKAGE_VERSION\" | $SED 's|^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\).*|\3|g'`"
PACKAGE_VERSION_EXTRA="`echo \"$PACKAGE_VERSION\" | $SED 's|^\([0-9]*\)\.\([0-9]*\)\.\([0-9]*\)\(.*\)|\4|g'`"
]

AC_SUBST(PACKAGE_VERSION_MAJOR)
AC_SUBST(PACKAGE_VERSION_MINOR)
AC_SUBST(PACKAGE_VERSION_PATCH)
AC_SUBST(PACKAGE_VERSION_EXTRA)

])
