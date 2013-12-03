
export
BASE_DIR	?= $(shell pwd)/..
GCC_INC		?= C:/CodeBlocks/include
GCC_LIB		?= C:/CodeBlocks/lib

SUBDIRS	= $(shell find -type d -regex "\./.+")

default: all

# Include the Makefiles from all subdirectories. This is how we know what
# the dependencies are.
include Makefile.inc $(shell find ./*/ -name Makefile)

$(SUBDIRS): force
	$(MAKE) -f $(BASE_DIR)/components/Submakefile -C $@

ComponentGraph.cpp: force
	python build_cg.py > $@

components.so: $(SUBDIRS) ComponentGraph.o
	$(CC) -o ../$@ $(DLL_FLAGS) $(LIB_DIRS) $(shell find -regex ".*\.o") $(LINK_DEPS) $(shell wx-config --libs)

all: components.so

clean:
	rm -f ComponentGraph.cpp
	find -name "*.o" -print0 | xargs -0 rm -f _-_
	rm -f ../components.so

test:
	$(CC) -o t.exe $(INC_DIRS) main.cpp ../components.so
