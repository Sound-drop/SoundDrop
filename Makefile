CXX = clang++

CFLAGS = -O2 -Wall --std=c++14

LIBRARY_SRC = libs
PA_STATIC_LIB = $(LIBRARY_SRC)/portaudio/lib/.libs/libportaudio.a
PA_INCLUDE_DIR = $(LIBRARY_SRC)/portaudio/include

SHELL = /bin/sh
LIBTOOL = $(SHELL) $(LIBRARY_SRC)/portaudio/libtool
LIBS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon

SOUND_OBJS = \
	receiver/write_wav.o \
	receiver/paex_record.o 

all: send_ip

send_ip: send_ip.cpp
	$(CXX) $(CFLAGS) $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) -o $@
sound:bin/sound
bin/sound: $(PA_STATIC_LIB) $(PA_INCLUDE_DIR) $(SOUND_OBJS) 
	$(CXX) -o $@ $(CFLAGS) receiver/*.c $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) $(LIBS)

%.o: %.c
	$(CXX) $(CFLAGS) -c $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) -o $@
# receiver/write_wav.o: receiver/write_wav.h
# 	$(CXX) $(CFLAGS) -c receiver/write_wav.c -Ireceiver -o $@

# receiver/paex_record.o: receiver/paex_record.c
# 	$(CXX) $(CFLAGS) -c $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) -o $@

clean:
	rm -rf send_ip receiver/*.o bin/sound

