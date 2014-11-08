EXECUTABLE = graphics_lab
CC = g++
CFLAGS = `pkg-config glfw3 --cflags` `pkg-config glew --cflags` `pkg-config IL --cflags` `pkg-config assimp --cflags` -I ./include -g
LDFLAGS= `pkg-config glfw3 --static --libs` `pkg-config glew --libs` `pkg-config IL --libs` `pkg-config assimp --libs`
SOURCES = BoneAnimation.cpp main.cpp 

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${LDFLAGS}

.cpp.o:
	${CC} ${CFLAGS} -c $< -o $@
