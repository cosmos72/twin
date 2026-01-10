#!/bin/sh

BISON=bison
FLEX=flex
LN=ln
SCHEME='chezscheme --script'

$SCHEME include/scheme/Tw/common.scm  > include/Tw/common_gen.h
$SCHEME include/scheme/Tw/common1.scm > include/Tw/common1_gen.h
$SCHEME include/scheme/Tw/proto.scm   > include/Tw/proto_gen.h
$SCHEME include/scheme/Tw/proto1.scm  > include/Tw/proto1_gen.h
$SCHEME include/scheme/Tw/libtw1.scm  > libs/libtw/libtw1_gen.h
$SCHEME include/scheme/Tw/libtw2.scm  > libs/libtw/libtw2_gen.h
$SCHEME include/scheme/socklist.scm   > include/socklist_gen.h
$SCHEME include/scheme/socket1.scm    > server/socket1_gen.h
$SCHEME include/scheme/socket2.scm    > server/socket2_gen.h
$SCHEME include/scheme/socket3.scm    > server/socket3_gen.h

for i in md5.c missing.c
do
  echo "$LN -f -n \"server/$i\"pp \"libs/libtw/$i\""
  $LN -f -n "server/$i"pp "libs/libtw/$i"
done

echo "cd server"
cd server

echo "$FLEX -o rcparse_lex.cpp rcparse.l"
$FLEX -o rcparse_lex.cpp rcparse.l

echo "$BISON --no-lines --defines -o rcparse_tab.cpp rcparse.y"
$BISON --no-lines --defines -o rcparse_tab.cpp rcparse.y

echo "cd .."
cd ..
