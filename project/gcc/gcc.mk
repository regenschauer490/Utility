COMPILER = g++
CFLAGS   = -Wextra -g -MMD -MP -std=gnu++1y
LDFLAGS  =
#BOOST_DIR = usr/include/c++/boost/boost_1_55_0
BOOST_DIR = home/sigure/boost/boost_1_57_0
INCLUDE = -I/$(BOOST_DIR)
LIBS    = /$(BOOST_DIR)/stage/lib/libboost_system.a /$(BOOST_DIR)/stage/lib/libboost_filesystem.a /$(BOOST_DIR)/stage/lib/libboost_regex.a 
OBJ_DIR = ./obj
BIN_DIR = ./bin
TARGET	= test
TARGET0 = $(addprefix $(BIN_DIR)/,$(TARGET))

include ../impl.mk
