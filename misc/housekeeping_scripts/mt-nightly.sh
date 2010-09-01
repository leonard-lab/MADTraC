#!/bin/bash

# MADTraC nightly script running currently on poincare.princeton.edu
MT_DIR=~/MT_Safe/MADTraC
MT_WEB_DIR=/var/www/poincare/MADTraC
MT_NIGHTLY_BASE=$MT_WEB_DIR/MADTraC-nightly
EXCLUDES_FILE=$MT_DIR/../excludes.txt

echo =======================================================
echo === Log of mt-nightly for `date`

cd $MT_DIR
echo === git pull
git pull origin master
cd $MT_DIR/docs/doxygen
echo === Creating documentation
doxygen Doxyfile
echo === Creating nightly tarball
cd $MT_DIR
cd ..
find MADTraC -name "*.svn*" > $EXCLUDES_FILE
echo excludes.txt >> $EXCLUDES_FILE
echo *.log >> $EXCLUDES_FILE
echo *docs/html/* >> $EXCLUDES_FILE
rm -f $MT_NIGHTLY_BASE.tar.gz
rm -f $MT_NIGHTLY_BASE.zip
tar -pczf $MT_NIGHTLY_BASE.tar.gz --exclude-from="$EXCLUDES_FILE" MADTraC
echo === Creating nightly zip
zip -r $MT_NIGHTLY_BASE.zip * -x '*.svn*' excludes.txt '*.log' *docs/html/*
echo === Copying to server
cp -R $MT_DIR/web/* $MT_WEB_DIR
cp -R $MT_DIR/docs/html/* $MT_WEB_DIR/docs

echo === End of log for `date` 
echo =======================================================
