#include <stdio.h>
#include <stdlib.h>
#include "portaudio.h"
#include "write_wav.h"
#include "aquila/global.h"
#include "aquila/source/generator/SquareGenerator.h"
#include "aquila/source/SignalSource.h"
#include "aquila/transform/FftFactory.h"
#include "aquila/tools/TextPlot.h"
#include "aquila/source/WaveFile.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <iostream>
#include <cstdlib>
#define PA_SAMPLE_TYPE  paInt16
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"
/* #define SAMPLE_RATE  (17932) // Test failure to open with this value. */
#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_CHANNELS    (2)
/* #define DITHER_FLAG     (paDitherOff) */
#define DITHER_FLAG     (0) /**/
/** Set to 1 if you want to capture the recording to a file. */
#define WRITE_TO_FILE   (1)
typedef short SAMPLE;

using namespace std;

class FFTreader
{
private:
	const Aquila::WaveFile* wav;
	const std::size_t SIZE = 1024;
	std::size_t END = 0;
	std::size_t sampleFreq = 0; 
	string file_path;
	std::size_t NUM_SECONDS;
	vector<int> findMax(Aquila::SpectrumType spectrum);
	vector<int> freqOfindex(std::size_t start);
	int  findStartingPoint();
	void printStatus(int start, vector<int>& peak);
	int  soundTo16bits(vector<int>& peak);
	vector<string> dataToStrings(vector<vector<int>>& data);
	void record();
	static int recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );
	typedef struct
    {
        int          frameIndex;  /* Index into sample array. */
        int          maxFrameIndex;
        SAMPLE      *recordedSamples;
    }
    paTestData; 
	void done(FFTreader::paTestData data, PaError err);

public:
	typedef unsigned char byteType;
	FFTreader(string file_path, int NUM_SECONDS): file_path(file_path),NUM_SECONDS(NUM_SECONDS){}
	vector<vector<FFTreader::byteType>> parse();
};