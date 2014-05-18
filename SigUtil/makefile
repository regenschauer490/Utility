COMPILER = g++
CFLAGS   = -Wextra -g -MMD -MP -std=gnu++1y
LDFLAGS  =
LIBS     = /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_system.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_filesystem.a /usr/include/c++/boost/boost_1_55_0/stage/lib/libboost_regex.a 
INCLUDE  = -I/usr/include/c++/boost/boost_1_55_0
BIN_DIR  = ../bin
TARGET	= test
TARGET0   = $(addprefix $(BIN_DIR)/,$(TARGET))
OBJ_DIR   = obj
ifeq "$(strip $(OBJ_DIR))" ""
  OBJ_DIR = .
endif

TEST_DIR = example
TEST_FILES = $(wildcard $(TEST_DIR)/*.cpp) 
TEST_FNAMES = $(subst $(TEST_DIR)/,,$(TEST_FILES))
TEST_TMP = $(TEST_FNAMES:.cpp=.o)
TEST_OBJS = $(addprefix $(OBJ_DIR)/,$(TEST_TMP))
MAIN_OBJ = $(addprefix $(OBJ_DIR)/,main.o)
OBJS = $(TEST_OBJS) $(MAIN_OBJ)
DEPENDS  = $(OBJS:.o=.d)

$(TARGET) : $(OBJS) $(LIBS)
	@[ -d $(BIN_DIR) ] || mkdir -p $(BIN_DIR)
	$(COMPILER) -o $(TARGET0) $^ $(LDFLAGS)

$(OBJ_DIR)/main.o : main.cpp
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_DIR)/%.o : example/%.cpp
	@[ -d $(OBJ_DIR) ] || mkdir -p $(OBJ_DIR)
	$(COMPILER) $(CFLAGS) $(INCLUDE) -c $< -o $@

$(OBJ_DIR)/main.o : $(TEST_FILES:.cpp=.h)

$(OBJ_DIR)/%.o : $(addprefix $(TEST_DIR)/,%.h)

 
all: clean $(TARGET)
 
clean:
	rm -f $(OBJS) $(DEPENDS) $(TARGET)
	@rmdir --ignore-fail-on-non-empty `readlink -f $(OBJ_DIR)`
 
-include $(DEPENDS)
