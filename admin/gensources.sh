BISON=bison
FIND=find
FLEX=flex
SED=sed
M4=m4
LN=ln

$FIND -name \*.m4h | \
while read i
do
  o="`echo \"$i\" | $SED 's,/m4/\(.*\).m4h,/\1_m4.h,g'`"
  echo "$M4 -I include < \"$i\" > \"$o\""
  $M4 -I include < "$i" > "$o"
done

for i in md5.c missing.c
do
  echo "$LN -f -n \"server/$i\" \"libs/libTw/$i\""
  $LN -f -n "server/$i" "libs/libTw/$i"
done

$FLEX -o server/rcparse_lex.c server/rcparse.l
$BISON --no-lines --defines -o server/rcparse_tab.c server/rcparse.y
