
CC			= g++
LD			= g++

OBJ_FLAGS	= -c -g

OBJ_DIR		= obj

# This is the prefix of the files that define the actual WX app to be run as
# the 'main' function.
APP_FILE_PREFIX	= Temple

# Let's just put this here for the sake of being explicit.
default: all

all: temple

OBJS		= $(APP_FILE_PREFIX).o Utility.o ComponentBoard.o ComponentGroup.o\
	ComponentGlobals.o Component.o Temp.o Interface.o Timer.o FontRenderer.o

temple: $(addprefix $(OBJ_DIR)/, $(OBJS)) components.so
	$(CC) -o $@ $^ $(shell wx-config --cppflags) -I../common $(shell wx-config --libs --gl-libs) -lGL -lglee -g

$(OBJ_DIR)/%.o: %.cpp
	$(BASE_CC)

$(OBJ_DIR)/%.o: ../common/%.cpp
	$(BASE_CC)

clean:
	rm -f temple *.o obj/*.o

BASE_CC		= $(CC) -o $@ $(OBJ_FLAGS) $^ $(shell wx-config --cppflags) -I../common

