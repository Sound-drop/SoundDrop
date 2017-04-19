#include <stdio.h>
#include <string.h>
#include <math.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "protocol.h"
#include "portaudio.h"
#include "aquila.h"

using namespace std;

static int paCallback( const void                      *inputBuffer,
					   void                            *outputBuffer,
					   unsigned long                   framesPerBuffer,
					   const PaStreamCallbackTimeInfo  *timeInfo,
					   PaStreamCallbackFlags           statusFlags,
					   void                            *userData) {

	SDData *data = (SDData *) userData;
	float *out = (float *) outputBuffer;
	int finished = 0;

	/* avoid unused variable warnings */
	(void) inputBuffer;
	(void) timeInfo;
	(void) statusFlags;

	for(int i = 0; i < framesPerBuffer; i++)
	{
		float val = 0;
		for (float freq : data->sine[data->phase]) {
			val += (float) sin((float) data->phase / freq);
		}

		*out++ = val; /* left */
		*out++ = val; /* right */
		data->phase += 1;
	}

	if (--(data->total_frames) <= 0) finished = 1;

	return finished;
}

typedef struct { 
	uint8_t n[4]; 
} IP;

IP getIP() {
	string str_addr;
	struct ifaddrs *ifAddrStruct;

	getifaddrs(&ifAddrStruct);

	for (struct ifaddrs *ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
			char str[INET_ADDRSTRLEN];
			struct in_addr *tmp = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp, str, INET_ADDRSTRLEN);

			// Discard localhost
			if (strcmp(str, "127.0.0.1") != 0) {
				str_addr = str;
			}
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}

	cout << str_addr << endl;

	/* Convert string ip address to int */
	IP addr;
	stringstream ss(str_addr);
	string tok;

	int i = 0;
	while (getline(ss, tok, '.')) {
		addr.n[i++] = (uint8_t) stoi(tok, nullptr);
	}

	return addr;
}

int main (int argc, char **argv) {
	/* Initialize PortAudio components to send audio */
	PaStream*           stream;
	PaError             err;
	PaStreamParameters  outParams;
	static const double SR   = 44100.0; /* Sample rate: 44100 Hz */
	static const int    FPB  = 512;     /* Frames per buffer: 46 ms buffers. */
	static const float  RATE = 0.1;     /* Transmission rate: 10 blocks / sec. */

	/* Get data to send */
	IP addr = getIP();
	void *raw_data = (void *) &addr; // TODO: Wrap this all in a class..
	int words = sizeof(IP) / 2;

	cout << "Transmitting " << words * 2 << " bytes..." << endl;

	words += 2; /* size of start and end chirp */

	/* Initialze SoundDrop struct */
	SDData data;
	data.total_frames = static_cast<float>(SR * RATE * ((float) words) / (float) FPB);
	data.phase = 0;

	for (int j = 0; j < SR * RATE; j++) {
		data.sine.push_back({chirp}); /* Start chirp */
	}

	uint16_t *ptr = (uint16_t *) raw_data;
	for (int i = 0; i < words - 2; i++, ptr++) {

		vector<float> data_point;
		for (int j = 0; j < SR * RATE; j++) {

			for (uint32_t x = 1, y = 0; x <= pow(2, 15); x *= 2, y++) {
				if (*ptr & x) {
					data_point.push_back(encoder[y]);
				}
			}

			data.sine.push_back(move(data_point));
		}
	}
	
	for (int j = 0; j < SR * RATE; j++) {
		data.sine.push_back({chirp}); /* End chirp */
	}

	// TODO: Not modern C++ idiomatic, wrap with class handler
	err = Pa_Initialize();
	if (err != paNoError) goto error;

	outParams.device = Pa_GetDefaultOutputDevice(); /* Take the default output device. */
	outParams.channelCount = 2;                     /* Stereo output, most likely supported. */
	outParams.hostApiSpecificStreamInfo = NULL;
	outParams.sampleFormat = paFloat32;             /* 32 bit floating point output. */
	outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;

	cout << "Starting audio..." << endl;

	err = Pa_OpenStream(&stream, NULL, &outParams, SR, FPB, paClipOff, paCallback, &data);
	if (err != paNoError) goto error;

	err = Pa_StartStream(stream);
	if (err != paNoError) goto error;

	while ((err = Pa_IsStreamActive(stream)) == 1) Pa_Sleep(100);
	if (err < 0) goto error;

	err = Pa_CloseStream(stream);
	if (err != paNoError) goto error;

	Pa_Terminate();
	return 0;

error:
	Pa_Terminate();

	fprintf(stderr, "An error occured while using the portaudio stream\n");
	fprintf(stderr, "Error number: %d\n", err);
	fprintf(stderr, "Error message: %s\n", Pa_GetErrorText(err));
	return 0;
}


