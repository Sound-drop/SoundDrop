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

using namespace std;
class FFTreader
{
private:
	Aquila::WaveFile wav;
	const std::size_t SIZE = 1024;
	const std::size_t END;
	const std::size_t sampleFreq; 
	vector<int> findMax(Aquila::SpectrumType spectrum);
	vector<int> freqOfindex(std::size_t start);
	int  findStartingPoint();
	void printStatus(int start, vector<int>& peak);
	int  soundTo16bits(vector<int>& peak);
	vector<string> dataToStrings(vector<vector<int>>& data);

public:
	typedef unsigned char byteType;
	FFTreader(string file_path): wav(file_path), END(wav.getSamplesCount()),sampleFreq(wav.getSampleFrequency()){}
	vector<vector<FFTreader::byteType>> parse();
};