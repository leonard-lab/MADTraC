#!/bin/bash

# This script is intended to "clean" the MADTraC tree for an svn import (or mass commit,
#   or just to clean up).  It should provide a list of files it is going to delete and
#   allow you to abort.


PROJECT_ROOT=../../..

# look for build directories
BUILD_DIRS=$(find $PROJECT_ROOT -type d -name build)

# look for .DS_Store files (extra files added by Finder
DS_STORES=$(find $PROJECT_ROOT -name ".DS_Store")

# look for vim and emacs autosave files
ASAVE_FILES=$(find $PROJECT_ROOT \( -name ".#*" -or -name "*~" \))

# built apps
BUILT_APPS=$(ls -A $PROJECT_ROOT/trunk/bin)

# copied frameworks
FRAMEWORKS=$(find $PROJECT_ROOT -name "*.framework")

# TEX output
TEX_OUTPUT=$(find $PROJECT_ROOT/trunk/docs/design \( -name "*.log" -o -name "*.aux" -o -name "*.out" -o -name "*.pdf" \) )

ALL_TO_DELETE="$BUILD_DIRS $DS_STORES $ASAVE_FILES $BUILT_APPS $FRAMEWORKS $TEX_OUTPUT poop"

FILES_TO_DELETE=""
DIRS_TO_DELETE=""

for E in $ALL_TO_DELETE
do
    if [ -f $E ]
    then
        FILES_TO_DELETE="$FILES_TO_DELETE $E"
    fi
    if [ -d $E ]
    then
        DIRS_TO_DELETE="$DIRS_TO_DELETE $E"
    fi  
done

echo "Files to be deleted:"
for F in $FILES_TO_DELETE
do
    echo $F
done

echo ""
echo "Directories to be deleted:"
for D in $DIRS_TO_DELETE
do
    echo $D
done

# directories that will be replaced with empty directories
# Doxygen output
DOXY_OUTPUT="$PROJECT_ROOT/trunk/docs/html"
# generated include files
GEN_INC="$PROJECT_ROOT/trunk/include"
# generated libs
GEN_LIB="$PROJECT_ROOT/trunk/lib"

DIRS_TO_EMPTY="$DOXY_OUTPUT $GEN_INC $GEN_LIB"

echo ""
echo "Directories to be emptied"
echo "     (deleted and replaced with an empty directory of the same name):"

for D in $DIRS_TO_EMPTY
do
    echo $D
done

# find out if the user wants us to go ahead, exit, or run interctively
echo ""
echo "Do you want to (y) delete these files and directories, (n) abort,"
read -p "   or (i) interactively ask about each file? (y/n/i):   "

INTERACTIVE=0

if [ "$REPLY" != i ] && [ "$REPLY" != y ]
then
    echo ""
    echo "Goodbye."
    exit 1
fi

if [ "$REPLY" == i ]
then
    INTERACTIVE=1
fi

for E in $FILES_TO_DELETE
do
    if [ $INTERACTIVE -eq 1 ]
    then
        read -p "Delete $E ? (y/n) "
    fi
    if [ $INTERACTIVE -ne 1 ] || [ "$REPLY" == y ]
    then
        rm -rf $E
    fi
done

for E in $DIRS_TO_DELETE
do
    if [ $INTERACTIVE -eq 1 ]
    then
        read -p "Delete directory $E ? (y/n) "
    fi
    if [ $INTERACTIVE -ne 1 ] || [ "$REPLY" == y ]
    then
        rm -rf $E
    fi
done

for E in $DIRS_TO_EMPTY
do
    if [ $INTERACTIVE -eq 1 ]
    then
        read -p "Empty directory $E ? (y/n) "
    fi
    if [ $INTERACTIVE -ne 1 ] || [ "$REPLY" == y ]
    then
        rm -rf $E
        mkdir $E
    fi
done