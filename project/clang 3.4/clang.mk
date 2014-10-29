COMPILER = clang++
CFLAGS   = -stdlib=libc++ -Wall -std=gnu++1y
LDFLAGS  =
STDLIB	= -I/usr/include/c++/4.8.1 -I/usr/include/i386-linux-gnu/c++/4.8
INCLUDE_B  = -I/usr/include/c++/boost/boost_1_55_0
LIBS     = /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_system.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_filesystem.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_regex.a 
BIN_DIR  = bin
TARGET	= test
TARGET0   = $(addprefix $(BIN_DIR)/,$(TARGET))
OBJ_DIR   = obj

INCLUDE = $(STDLIB) $(INCLUDE_B)
include ../impl.mk
