CXX = clang++
CFLAGS = -O2 -Wall --std=c++14

LIBRARY_SRC = libs
PA_STATIC_LIB = $(LIBRARY_SRC)/portaudio/lib/.libs/libportaudio.a
PA_INCLUDE_DIR = $(LIBRARY_SRC)/portaudio/include

all: send_ip

send_ip: send_ip.cpp
	$(CXX) $(CFLAGS) $< $(PA_STATIC_LIB) -I$(PA_INCLUDE_DIR) -o $@

clean:
	rm -rf send_ip
