# shell script run by xcode aggregate target
#  - copies the MT.h header

# for some reason the aggregate runs with a working directoy in the
#  build directory rather than the project directory (like the other targets do)
cp "./MT/MT.h" "./include/MT.h"