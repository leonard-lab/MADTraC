CC = g++
CXXFLAGS = -I. `wx-config --cxxflags` `pkg-config --cflags opencv` -O3 -Wall 
LDFLAGS = `wx-config --libs` `pkg-config --libs opencv`

MT_CORE_SOURCES = `find ./MT/MT_Core \( -name '*.cpp' -or -name '*.c' \)`

MT_CORE_OBJS = $(MT_CORE_SOURCES:.cpp=.o)

all: MT_Core

MT_Core: MT_Core_SRC
	$(CC) $(LDFLAGS) $(MT_CORE_OBJS) -o libMT_Core.a

MT_GUI:

MT_Tracking:

MT_Robots:

MT_Core_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_CORE_SOURCES)
