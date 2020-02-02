.PHONY: ex

NCHAN=2
BUFSIZE=512

# CXXFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=$(BUFSIZE) \
# 		   -DMY_CHANNELS_IN=$(NCHAN) -DMY_CHANNELS_OUT=$(NCHAN) -D__PLATFORM_LINUX__ -g  -O3

CFLAGS = -I lib/chuck/ -I lib/libchuck/ -DMY_SRATE=44100 -DMY_BUFFERSIZE=$(BUFSIZE)  \
		   -DMY_CHANNELS_IN=$(NCHAN) -DMY_CHANNELS_OUT=$(NCHAN) -D__PLATFORM_LINUX__ -g \
		   -I lib/nanovg -O3 -Wall

# LDFLAGS=-llua -lpthread -lasound -ldl -lm -ljack -lsndfile -lstdc++ -lGL -lglfw

# CHUCK_OBJ=`find lib/chuck/ -name "*.o" | egrep -v "libchuck|chuck_main"`

#OBJ = graphics.o lib/libchuck/chuckwrap.o lib/nanovg/nanovg.o audio.o 
#OBJ = lib/libchuck/chuckwrap.o lib/nanovg/nanovg.o audio.o graphics.o
OBJS += audio.o graphics.o

LUA_PATH=lib/lua
CORE_O=	lapi.o lcode.o lctype.o ldebug.o ldo.o ldump.o lfunc.o lgc.o llex.o \
	lmem.o lobject.o lopcodes.o lparser.o lstate.o lstring.o ltable.o \
	ltm.o lundump.o lvm.o lzio.o
LIB_O=	lauxlib.o lbaselib.o lbitlib.o lcorolib.o ldblib.o liolib.o \
	lmathlib.o loslib.o lstrlib.o ltablib.o lutf8lib.o loadlib.o linit.o
BASE_O= $(CORE_O) $(LIB_O) $(MYOBJS)

OBJS += $(addprefix $(LUA_PATH)/, $(BASE_O))
CFLAGS+=-DLUA_USE_C89 -DLUA_COMPAT_5_2
CFLAGS+=-I$(LUA_PATH)

OBJS += lib/patchwerk/patchwerk.o
CFLAGS += -Ilib/patchwerk
CFLAGS += -g

LDFLAGS += -lsporth -lsoundpipe -lm -lsndfile -lcairo -lx264

OBJS += wavout.o pwsporth.o chooser.o reg.o glitch.o add.o

default: tiziku

%.o: %.cpp
	g++ -c $(CXXFLAGS) $< -o $@

%.o: %.c
	gcc -c $(CFLAGS) $< -o $@

tiziku: main.c $(OBJS)
	gcc main.c -o $@ $(CFLAGS) $(OBJS) $(LDFLAGS)


NAME=tiziku

$(NAME).mp3: $(NAME).wav
	lame --preset studio $<

$(NAME).mp4: $(NAME).h264 $(NAME).mp3
	ffmpeg -y -i $(NAME).mp3 -i $(NAME).h264 -ac 2 $(NAME).mp4

vid: tiziku
	./tiziku
	$(MAKE) $(NAME).mp4

clean:
	rm -rf $(OBJS) tiziku hello
