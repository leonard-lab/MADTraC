# shell script for xcode "Run Script" build phase
#  - environment variables correspond to build settings in xcode project,
#     e.g. ${PRODUCT_NAME} = MT_Core, MT_GUI, etc,
#  - copies all headers from MT/${PRODUCT_NAME} to include dir, preserving
#     directory tree structure, e.g. MT/MT_Core/fileio/xmlsupport.h becomes
#     include/MT/MT_Core/fileio/xmlsupport.hl, excludes MT_${PRODUCT_NAME}.h
#  - creates include/${PRODUCT_NAME}.h
#     * deletes any existing header with this name
#     * wraps the header in #ifdef guards, the guard has the form MT_HAVE_{module},
#       where module is e.g. CORE, GUI, TRACKING, ROBOT...
#     * copies MT/${PRODUCT_NAME}/${PRODUCT_NAME}.h if it exists as a prototype
#     * timepstamps last modification time of prototype header and auto-generated content
#     * defines all of the preprocessor macros defined via any of
#       GCC_PREPROCESSOR_DEFINITIONS, OTHER_CFLAGS, and OTHER_CPLUPLUSFLAGS,
#       guarding each with an #ifndef so as not to clobber any pre-existing
#       values
#     * inserts #include lines for each header copied above
#  - copies the built library file to the lib directory

# shell line to find all headers, but excluding the prototype e.g. MT_Core.h
ALL_HEADERS='find ./MT/${PRODUCT_NAME} \( -name '*.h' -and -not -name ${PRODUCT_NAME}.h \) |  grep -v "pyhungarian.h" | grep -v "Win32Com" | grep -v "YASegmenter.h" '

# copy header files
eval $ALL_HEADERS | cpio -pdvm ./include
# set the permissions correctly on the new headers
chmod -R og-w include/MT

# create the ${PRODUCT_NAME}.h file (e.g. MT_Core.h)
LIB_HEADER=include/${PRODUCT_NAME}.h

# remove the header if it exists
if [ -f $LIB_HEADER ]
then
    rm $LIB_HEADER
fi
touch $LIB_HEADER

# will be e.g. MT_HAVE_CORE, MT_HAVE_GUI, etc.
MOD_NAME=`echo MT_HAVE_${PRODUCT_NAME:3} | tr "[:lower:]" "[:upper:]"`

# add guard defs to the top of the header
echo '#ifndef ' $MOD_NAME >> $LIB_HEADER
echo '#define ' $MOD_NAME >> $LIB_HEADER
echo "" >> $LIB_HEADER

# check to see if a header prototype exists in the source tree
PROTOTYPE_HEADER=./MT/${PRODUCT_NAME}/${PRODUCT_NAME}.h
if [ -f $PROTOTYPE_HEADER ]
then
    cat $PROTOTYPE_HEADER >> $LIB_HEADER
    echo "" >> $LIB_HEADER
    echo '/* Content above copied from prototype ' $PROTOTYPE_HEADER ' */' >> $LIB_HEADER
    echo '/*    which was last modified on' `stat -f "%m" $PROTOTYPE_HEADER | date "+%m/%d/%y"` '*/' >> $LIB_HEADER
fi

echo '/* Content below was inserted by' `basename $0` 'on ' `date "+%m/%d/%y"` '*/' >> $LIB_HEADER

# insert the preprocessor definitions from the xcode project into the header file
echo "" >> $LIB_HEADER

echo '/* Preprocessor definitions inherited from build project */' >> $LIB_HEADER

echo '/* OTHER_CFLAGS: ' ${OTHER_CFLAGS} ' */' >> $LIB_HEADER
echo >> $LIB_HEADER

# looks for entries that look like "-DMY_DEFINITION" or "-DMY_VALUE=42" and converts them
# to #ifndef MY_DEFINITION, #define MY_DEFINITION (or #define MY_VALUE 42), #endif
for def_entry in $OTHER_CFLAGS;
do
    # make sure the first two characters are "-D", indicating a preprocessor definition
    if [ ${def_entry:0:2} == "-D" ]
    then
        # strip off the -D because we don't want it in the #define line
        def_entry=${def_entry:2}
        # also strip off the value if there is one
        echo '#ifndef '${def_entry/=*/} >> $LIB_HEADER
        # the #define doesn't use an equal sign
        echo '    #define '${def_entry/=/   } >> $LIB_HEADER
        echo '#endif' >> $LIB_HEADER
        echo "" >> $LIB_HEADER
    fi
done

echo '/* OTHER_CPLUSPLUSFLAGS: ' ${OTHER_CPLUSPLUSFLAGS} ' */' >> $LIB_HEADER
echo >> $LIB_HEADER

# same as for OTHER_CFLAGS
for def_entry in $OTHER_CPLUSPLUSFLAGS;
do
    if [ ${def_entry:0:2} == "-D" ]
    then
        def_entry=${def_entry:2}
        echo '#ifndef '${def_entry/=*/} >> $LIB_HEADER
        echo '    #define '${def_entry/=/   } >> $LIB_HEADER
        echo '#endif' >> $LIB_HEADER
        echo "" >> $LIB_HEADER
    fi
done

echo '/* GCC_PREPROCESSOR_DEFINITIONS: ' ${GCC_PREPROCESSOR_DEFINITIONS} ' */' >> $LIB_HEADER
echo >> $LIB_HEADER

# similar to OTHER_CFLAGS, but don't look for the "-D"
for def_entry in $GCC_PREPROCESSOR_DEFINITIONS;
do
    def_entry=${def_entry}
    echo '#ifndef '${def_entry/=*/} >> $LIB_HEADER
    echo '    #define '${def_entry/=/   } >> $LIB_HEADER
    echo '#endif' >> $LIB_HEADER
    echo "" >> $LIB_HEADER
done

# add include lines to the header file
echo "" >> $LIB_HEADER
echo '/* Headers for the' ${PRODUCT_NAME} 'module */' >> $LIB_HEADER
for header in `eval $ALL_HEADERS`;
do
    # strips off the preceding ./
    echo '#include "'${header:2}'"' >> $LIB_HEADER
done

# insert the #endif at the end of the file
echo "" >> $LIB_HEADER
echo '#endif /* ' $MOD_NAME ' */' >> $LIB_HEADER

# copy the library file, should be e.g. build/Release/libMTCore.a
LIB_FILE=${TARGET_BUILD_DIR}/${EXECUTABLE_PREFIX}${PRODUCT_NAME}.${EXECUTABLE_EXTENSION}
cp $LIB_FILE lib
