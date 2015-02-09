COMPILER = clang++
CFLAGS   = -stdlib=libc++ -Wall -std=gnu++1y
LDFLAGS  =
STDLIB	= -I/usr/include/c++/4.9.2 -I/usr/include/x86_64-linux-gnu/c++/4.9
BOOST_DIR = home/nishimura/lib/boost_1_57_0
INCLUDE_B = -I/$(BOOST_DIR)
LIBS    = /$(BOOST_DIR)/stage/lib/libboost_system.a /$(BOOST_DIR)/stage/lib/libboost_filesystem.a /$(BOOST_DIR)/stage/lib/libboost_regex.a 
BIN_DIR  = bin
TARGET	= test
TARGET0   = $(addprefix $(BIN_DIR)/,$(TARGET))
OBJ_DIR   = obj

INCLUDE = $(STDLIB) $(INCLUDE_B)
include ../impl.mk
