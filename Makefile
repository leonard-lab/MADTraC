CC = g++
GCC = gcc
CXXFLAGS = -I. `wx-config --cxxflags` `pkg-config --cflags opencv` -O3 -Wall -DMT_NO_ARTOOLKIT -DMT_NO_CLF 
LDFLAGS = `wx-config --libs` `pkg-config --libs opencv` -L/usr/X11R6/lib -lX11 -lXi -lglut -lGL -lGLU -lm
ARFLAGS = -rs

MT_CORE_SOURCES = $(shell find ./MT/MT_Core \( -name '*.cpp' -or -name '*.c' \) | grep -v "CLFSupport")

MT_CORE_SOURCE_NAMES = $(notdir $(MT_CORE_SOURCES))

MT_CORE_OBJS = $(MT_CORE_SOURCE_NAMES:.cpp=.o)

MT_GUI_SOURCES = $(shell find ./MT/MT_GUI \( -name '*.cpp' -or -name '*.c' \))

MT_GUI_SOURCE_NAMES = $(notdir $(MT_GUI_SOURCES))

MT_GUI_OBJS = $(MT_GUI_SOURCE_NAMES:.cpp=.o)

MT_TRACKING_SOURCES = $(shell find ./MT/MT_Tracking \( -name '*.cpp' \) | grep -v "pyhungarian" | grep -v "Win32Com" | grep -v "GY" | grep -v "YA")

MT_TRACKING_SOURCES_C = $(shell find ./MT/MT_Tracking \( -name '*.c' \) | grep -v "pyhungarian" | grep -v "Win32Com" | grep -v "GY" | grep -v "YA")

MT_TRACKING_SOURCE_NAMES = $(notdir $(MT_TRACKING_SOURCES))

MT_TRACKING_SOURCE_NAMES_C = $(notdir $(MT_TRACKING_SOURCES_C))

MT_TRACKING_OBJS = $(MT_TRACKING_SOURCE_NAMES:.cpp=.o)

MT_TRACKING_OBJS_C = $(MT_TRACKING_SOURCE_NAMES_C:.c=.o)

MT_ROBOT_SOURCES = $(shell find ./MT/MT_Robot \( -name '*.cpp' -or -name '*.c' \) | grep -v "Win32Com")

MT_ROBOT_SOURCE_NAMES = $(notdir $(MT_ROBOT_SOURCES))

MT_ROBOT_OBJS = $(MT_ROBOT_SOURCE_NAMES:.cpp=.o)

all: MT_Core MT_GUI MT_Tracking MT_Robot

MT_Core: MT_Core_SRC
	$(AR) $(ARFLAGS) lib/libMT_Core.a $(MT_CORE_OBJS)

MT_GUI: MT_GUI_SRC
	$(AR) $(ARFLAGS) lib/libMT_GUI.a $(MT_GUI_OBJS)

MT_Tracking: MT_Tracking_SRC MT_Tracking_SRC_C
	$(AR) $(ARFLAGS) lib/libMT_Tracking.a $(MT_TRACKING_OBJS) $(MT_TRACKING_OBJS_C)

MT_Robot: MT_Robot_SRC
	$(AR) $(ARFLAGS) lib/libMT_Robot.a $(MT_ROBOT_OBJS)

MT_Core_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_CORE_SOURCES)

MT_GUI_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_GUI_SOURCES)

MT_Tracking_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_TRACKING_SOURCES)

MT_Tracking_SRC_C:
	$(GCC) $(CXXFLAGS) -c $(MT_TRACKING_SOURCES_C)

MT_Robot_SRC:
	$(CC) $(CXXFLAGS) -c $(MT_ROBOT_SOURCES)
