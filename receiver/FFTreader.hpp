/**
* <h1>FFT receiver</h1>
* The FFT receiver utlizes portaudio and aquila to fetch and parse sound data.
*
* @author  Mick Lin cl3469@columbia.edu
* @version 1.0
* @since   2017-04-25
*/
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
/**
 * byteType is an alias for unsigned char, which represents a byte (8 bits) of data.
 */
	typedef unsigned char byteType;
/**
 * Constructor with file path for the recorded file and the number of seconds for recording
 * @param  file_path  a path to store the recorded .wav
 * @param  NUM_SECONDS recording time for the wav file
 */
	FFTreader(string file_path, int NUM_SECONDS): file_path(file_path),NUM_SECONDS(NUM_SECONDS){}
/**
 * FFTreader starts to record the sound through microphone and save the data to a wav file.
 * Once the wav file is saved, the aquila FFT parsing module will search and parse the pkts to vector<vector<FFTreader::byteType>>.
 * @return   vector<vector<FFTreader::byteType>>
 * @see FFTreader::byteType
 */
	vector<vector<FFTreader::byteType>> parse();
};