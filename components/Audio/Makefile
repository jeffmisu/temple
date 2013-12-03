
INC_DIRS	+=
LINK_DEPS	+= -lportaudio -lpthread -lportsf

all: $(OBJS) WAV.o AudioStream.o InputStream.o OutputStream.o

WAV.o: $(BASE_DIR)/WAV.cpp
	$(OBJ_CMD)

AudioStream.o: $(BASE_DIR)/AudioStream.cpp
	$(OBJ_CMD)

InputStream.o: $(BASE_DIR)/InputStream.cpp
	$(OBJ_CMD)

OutputStream.o: $(BASE_DIR)/OutputStream.cpp
	$(OBJ_CMD)

