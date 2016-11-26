set -x
rm -frv autom4te.cache/ libs/libltdl/ m4/ configure config.cache config.status config.log aclocal.m4
rm -fv admin/aclocal.m4 admin/compile admin/config.guess admin/config.sub admin/depcomp admin/install-sh admin/ltmain.sh admin/missing admin/mkinstalldirs
find -name Makefile.in | xargs -d'\n' rm -fv
