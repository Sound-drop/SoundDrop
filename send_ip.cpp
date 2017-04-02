#include <stdio.h>
#include <string.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <netinet/in.h> 
#include <arpa/inet.h>

#include <iostream>

#include "portaudio.h"
#include "protocol.h"

using namespace std;

static int paCallback(const void                      *inputBuffer,
                      void                            *outputBuffer,
                      unsigned long                   framesPerBuffer,
                      const PaStreamCallbackTimeInfo  *timeInfo,
                      PaStreamCallbackFlags           statusFlags,
                      void                            *userData) {

	int numFrames, finished;
	SDData *data = (SDData *) userData;
	float *out = (float *) outputBuffer;
	(void) inputBuffer; /* Prevent "unused variable" warnings. */

	/* Are we almost at end. */
	if (data->sampsToGo < framesPerBuffer) {
		numFrames = data->sampsToGo;
		finished = 1;
	}
	else {
		numFrames = framesPerBuffer;
		finished = 0;
	}

	for (int i = 0; i < numFrames; i++) {
		*out++ = GeneratePinkNoise( &data->leftPink );
		*out++ = GeneratePinkNoise( &data->rightPink );
	}

	data->sampsToGo -= numFrames;

	return finished;
}

string getIP() {
	string addr;
	struct ifaddrs *ifAddrStruct;

	getifaddrs(&ifAddrStruct);

	for (struct ifaddrs *ifa = ifAddrStruct; ifa != NULL; ifa = ifa->ifa_next) {
		if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
			char str[INET_ADDRSTRLEN];
			struct in_addr *tmp = &((struct sockaddr_in *) ifa->ifa_addr)->sin_addr;
			inet_ntop(AF_INET, tmp, str, INET_ADDRSTRLEN);

			// Discard localhost
			if (strcmp(str, "127.0.0.1") != 0) {
				addr = str;
			}
		}
	}

	if (ifAddrStruct != NULL) {
		freeifaddrs(ifAddrStruct);
	}

	return addr;
}

int main (int argc, char **argv) {
	string addr = getIP();
	int dataLength = addr.size() - 3;
	cout << "IPv4 Address: " << addr << endl;

	// Initialize PortAudio components to send audio
	PaStream*           stream;
	PaError             err;
	SDData              data;
	PaStreamParameters  outParams;
	int                 totalSamples;
	static const double SR  = 44100.0;
	static const int    FPB = 2048; /* Frames per buffer: 46 ms buffers. */

	data.samplesToGo = totalSamples = 0.5 * dataLength * SR; /* Half second pulse for each number */

	// TODO: Not modern C++ idiomatic, wrap with class handler
	err = Pa_Initialize();
	if (err != paNoError) goto error;

	outParams.device = Pa_GetDefaultOutputDevice(); /* Take the default output device. */
	if (outParams.device == paNoDevice) {
		perror("No default output device");
		goto error;
	}
	outParams.channelCount = 2;                     /* Stereo output, most likely supported. */
	outParams.hostApiSpecificStreamInfo = NULL;
	outParams.sampleFormat = paFloat32;             /* 32 bit floating point output. */
	outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;

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


