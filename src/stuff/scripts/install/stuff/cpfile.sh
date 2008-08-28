#!/bin/sh

# file dst user grouop mode

SRC_TOPDIR=`$PREFIX/stuff/opt_get.sh SRC_TOPDIR`
DIST_DIR=`$PREFIX/stuff/opt_get.sh DIST_DIR`

echo -n "Coping file \`$1\`... "
if ( `cp $SRC_TOPDIR$1 $DIST_DIR/webtester$2 > /dev/null 2>&1` ); then
  echo "ok.";
  if ( `test -e "$DIST_DIR/webtester$2"` ); then
    chown $3:$4 "$DIST_DIR/webtester$2";
    chmod $5 "$DIST_DIR/webtester$2";
  fi;
else
  echo "failed!";
  exit -1;
fi

exit 0;
