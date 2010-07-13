#!/bin/bash

# MADTraC nightly script running currently on poincare.princeton.edu
MT_DIR=~/MT_Safe/MADTraC
MT_WEB_DIR=/var/www/poincare/MADTraC

echo =======================================================
echo === Log of mt-nightly for `date`

cd $MT_DIR
echo === SVN Update
svn update
cd $MT_DIR/docs/doxygen
echo === Creating documentation
doxygen Doxyfile
echo === Creating nightly tarball and zip
tar -pczf $MT_WEB_DIR/latest.tar.gz $MT_DIR/*
zip -r $MT_WEB_DIR/latest.zip $MT_DIR/*
echo === Copying to server
cp -R $MT_DIR/web/* $MT_WEB_DIR
cp -R $MT_DIR/docs/html/* $MT_WEB_DIR/docs

echo === End of log for `date` 
echo =======================================================


