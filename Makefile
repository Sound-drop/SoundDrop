CXX = clang++
CXXFLAGS = -g -O2 -Wall --std=c++14

LIBRARY_SRC = libs
PA_STATIC_LIB = $(LIBRARY_SRC)/portaudio/lib/.libs/libportaudio.a
PA_INCLUDE_DIR = $(LIBRARY_SRC)/portaudio/include
AQ_STATIC_LIB = $(LIBRARY_SRC)/aquila/build/libAquila.a $(LIBRARY_SRC)/aquila/build/lib/libOoura_fft.a
AQ_INCLUDE_DIR = $(LIBRARY_SRC)/aquila
LIBSSH_STATIC_LIB = $(LIBRARY_SRC)/libssh/build/src/libssh.dylib # same as static
LIBSSH_INCLUDE_DIR = $(LIBRARY_SRC)/libssh/include

SHELL = /bin/sh
LIBTOOL = $(SHELL) $(LIBRARY_SRC)/portaudio/libtool
LIBS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon

SOUND_OBJS = \
	receiver/write_wav.o \
	receiver/paex_record.o 

INCLUDES = -I$(PA_INCLUDE_DIR) -I$(AQ_INCLUDE_DIR)
STATIC_LIBS = $(PA_STATIC_LIB) $(AQ_STATIC_LIB)

all: send_ip recv

send_ip: send_ip.cpp sound_drop.h protocol.h
	$(CXX) $(CXXFLAGS) $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) $(LIBS) -o $@

transfer: transfer.cpp
	$(CXX) $(CXXFLAGS) $< $(STATIC_LIBS) $(INCLUDES) $(FLAGS) -o $@

recv: receiver/FFTreader.cpp receiver/paex_record.cpp receiver/write_wav.c
	$(CXX) -o $@ $(CXXFLAGS) receiver/FFTreader.cpp receiver/paex_record.cpp receiver/write_wav.c $(STATIC_LIBS) $(INCLUDES) $(LIBS)

# %.o: %.cpp
# 	$(CXX) $(CFLAGS) -c $< $(PA_STATIC_LIB) $(INCLUDES) -o $@

# %.o: %.c
# 	$(CXX) $(CFLAGS) -c $< $(STATIC_LIBS) -I$(INCLUDES) -o $@
clean:
	rm -rf send_ip test transfer *.dSYM receiver/*.o recv
