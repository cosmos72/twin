FIND=find
SED=sed
M4=m4
LN=ln

$FIND -name \*.m4h | \
while read I
do
  O="`echo \"$I\" | $SED 's,/m4/\(.*\).m4h,/\1_m4.h,g'`"
  echo "$M4 -I include < \"$I\" > \"$O\""
  $M4 -I include < "$I" > "$O"
done

for I in md5.c missing.c
do
  echo "$LN -f -n \"server/$I\" \"libs/libTw/$I\""
  $LN -f -n "server/$I" "libs/libTw/$I"
done
