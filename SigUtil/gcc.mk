COMPILER = g++
CFLAGS   = -Wextra -g -MMD -MP -std=gnu++1y
LDFLAGS  =
INCLUDE  = -I/usr/include/c++/boost/boost_1_55_0
LIBS     = /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_system.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_filesystem.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_regex.a 
BIN_DIR  = ../bin
TARGET	= test
TARGET0   = $(addprefix $(BIN_DIR)/,$(TARGET))
OBJ_DIR   = obj/gcc

include impl.mk
