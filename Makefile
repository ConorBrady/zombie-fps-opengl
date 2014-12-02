EXECUTABLE = graphics_lab
CC = g++
CFLAGS = -std=c++11 `pkg-config glfw3 --cflags` `pkg-config glew --cflags` `pkg-config IL --cflags` `pkg-config assimp --cflags` -I ./include -g -Wall
LDFLAGS= `pkg-config glfw3 --static --libs` `pkg-config glew --libs` `pkg-config IL --libs` `pkg-config assimp --libs`
SOURCES = \
	animation.cpp \
	bone.cpp \
	bone_animation.cpp \
	bullet.cpp \
	camera.cpp \
	collision_space.cpp \
	gun.cpp \
	landscape.cpp \
	main.cpp \
	mesh.cpp \
	player.cpp \
	shader_loader.cpp \
	window.cpp \
	zombie.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${LDFLAGS}

.cpp.o:
	${CC} ${CFLAGS} -c $< -o $@
