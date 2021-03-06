EXECUTABLE = zombie_fps
CC = g++
CFLAGS = \
		-std=c++11 \
		`pkg-config glfw3 --cflags` \
		`pkg-config glew --cflags` \
		`pkg-config IL --cflags` \
		`pkg-config assimp --cflags` \
		-I ./include -I/usr/local/include/ -g -Wall
LDFLAGS= \
 		`pkg-config glfw3 --static --libs` \
 		`pkg-config glew --libs` \
		`pkg-config IL --libs` \
		`pkg-config assimp --libs` \
		-L/usr/local/lib -lirrklang

SOURCES = \
	audible_space.cpp \
	bullet.cpp \
	camera.cpp \
	collision_space.cpp \
	gun.cpp \
	landscape.cpp \
	main.cpp \
	mesh.cpp \
	notification_center.cpp \
	player.cpp \
	score_manager.cpp \
	shader_loader.cpp \
	window.cpp \
	zombie.cpp \
	zombie_manager.cpp \
	lib/text/text.cpp

OBJECTS=$(SOURCES:.cpp=.o)

all: $(SOURCES) $(EXECUTABLE)

${EXECUTABLE}: ${OBJECTS}
	${CC} ${OBJECTS} -o $@ ${LDFLAGS}

.cpp.o:
	${CC} ${CFLAGS} -c $< -o $@
