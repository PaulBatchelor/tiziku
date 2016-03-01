.PHONY: ex

NCHAN=2
BUFSIZE=4096

CXXFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=$(BUFSIZE) \
		   -DMY_CHANNELS_IN=1 -DMY_CHANNELS_OUT=$(NCHAN) -D__PLATFORM_LINUX__ -g  -O3

CFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=$(BUFSIZE)  \
		   -DMY_CHANNELS_IN=1 -DMY_CHANNELS_OUT=$(NCHAN) -D__PLATFORM_LINUX__ -g \
		   -I lib/nanovg -O3

LDFLAGS=-lpthread -lasound -ldl -lm -ljack -lsndfile -lstdc++ -lGL -lglfw

CHUCK_OBJ=`find lib/chuck/ -name "*.o" | egrep -v "libchuck|chuck_main"`

#OBJ = graphics.o lib/libchuck/chuckwrap.o lib/nanovg/nanovg.o audio.o 
OBJ = lib/libchuck/chuckwrap.o lib/nanovg/nanovg.o audio.o graphics.o

default: tiziku

%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

tiziku: main.c $(OBJ)
	gcc -o $@ $(LDFLAGS) $(CFLAGS) main.c $(OBJ) $(CHUCK_OBJ)

hello: hello.c lib/nanovg/nanovg.o
	gcc -I lib/nanovg hello.c -lm -lGL -lglfw lib/nanovg/nanovg.o -o $@

clean:
	rm -rf $(OBJ) tiziku hello
