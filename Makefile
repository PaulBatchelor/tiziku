.PHONY: ex


CXXFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=1024 \
		   -DMY_CHANNELS_IN=1 -DMY_CHANNELS_OUT=1 -D__PLATFORM_LINUX__ -g 

CFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=1024 \
		   -DMY_CHANNELS_IN=1 -DMY_CHANNELS_OUT=1 -D__PLATFORM_LINUX__ -g 

LDFLAGS=-lpthread -lasound -ldl -lm -ljack -lsndfile -lstdc++

CHUCK_OBJ=`find lib/chuck/ -name "*.o" | egrep -v "libchuck|chuck_main"`

OBJ = lib/libchuck/chuckwrap.o lib/nanovg/nanovg.o

default: tiziku

%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

tiziku: main.c $(OBJ)
	gcc -o $@ $(LDFLAGS) $(CXXFLAGS) main.c $(OBJ) $(CHUCK_OBJ)

hello: hello.c lib/nanovg/nanovg.o
	gcc -I lib/nanovg hello.c -lm -lGL -lglfw lib/nanovg/nanovg.o -o $@

clean:
	rm -rf $(OBJ) tiziku hello
