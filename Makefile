CC = g++
CXXFLAGS = -I. `wx-config --cxxflags` `pkg-config --cflags opencv` -O3 -Wall 
LDFLAGS = `wx-config --libs` `pkg-config --libs opencv` -L/usr/X11R6/lib -lX11 -lXi -lglut -lGL -lGLU -lm
ARFLAGS = -rs

MT_CORE_SOURCES = $(shell find ./MT/MT_Core \( -name '*.cpp' -or -name '*.c' \))

MT_CORE_SOURCE_NAMES = $(notdir $(MT_CORE_SOURCES))

MT_CORE_OBJS = $(MT_CORE_SOURCE_NAMES:.cpp=.o)

all: MT_Core

MT_Core: MT_Core_SRC
	$(AR) $(ARFLAGS) lib/libMT_Core.a $(MT_CORE_OBJS)

MT_GUI:

MT_Tracking:

MT_Robots:

MT_Core_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_CORE_SOURCES)
