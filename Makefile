CXX = clang++
CXXFLAGS = -g -O2 -Wall --std=c++14

LIBRARY_SRC = libs
PA_STATIC_LIB = $(LIBRARY_SRC)/portaudio/lib/.libs/libportaudio.a
PA_INCLUDE_DIR = $(LIBRARY_SRC)/portaudio/include

INCLUDES = -I$(PA_INCLUDE_DIR)
STATIC_LIBS = $(PA_STATIC_LIB)

FLAGS = -framework CoreAudio -framework AudioToolbox -framework AudioUnit -framework Carbon

all: send_ip

send_ip: send_ip.cpp sound_drop.h protocol.h
	$(CXX) $(CXXFLAGS) $< $(STATIC_LIBS) $(INCLUDES) $(FLAGS) -o $@

clean:
	rm -f send_ip
