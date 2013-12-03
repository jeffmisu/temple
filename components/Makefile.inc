
CC			= g++
LD			= g++

INC_DIRS	= -I$(BASE_DIR) -I$(BASE_DIR)/../common -I$(GCC_INC)
LIB_DIRS	= -L$(GCC_LIB)
LINK_DEPS	=\
	$(BASE_DIR)/obj/FontRenderer.o \
	$(BASE_DIR)/obj/Utility.o \
	$(BASE_DIR)/obj/Component.o \
	$(BASE_DIR)/obj/ComponentBoard.o \
	$(BASE_DIR)/obj/ComponentGlobals.o \
	$(BASE_DIR)/obj/Interface.o \
	-lGL \
	-lfreetype

OBJ_FLAGS	= -c -g
DLL_FLAGS	= -shared

force:
	@true

%.o: %.cpp
	$(CC) -o $@ $(OBJ_FLAGS) $(INC_DIRS) $^
