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
#include "FFTreader.hpp"


#define SAMPLE_RATE  (44100)
#define FRAMES_PER_BUFFER (512)
#define NUM_CHANNELS    (2)


using namespace std;
#define abs_amp 10000 
#define startchirp 201
#define DEBUG_FLAG  (1)
#define PA_SAMPLE_TYPE  paInt16
#define SAMPLE_SILENCE  (0)
#define PRINTF_S_FORMAT "%d"


void  FFTreader::done(FFTreader::paTestData data, PaError err){
    Pa_Terminate();
    if( data.recordedSamples )       /* Sure it is NULL or valid. */
        free( data.recordedSamples );
    if( err != paNoError )
    {
        fprintf( stderr, "An error occured while using the portaudio stream\n" );
        fprintf( stderr, "Error number: %d\n", err );
        fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
        err = 1;          /* Always return 0 or 1, but no other return codes. */
    }
    exit (EXIT_FAILURE);
}
int FFTreader::recordCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
    paTestData *data = (paTestData*)userData;
    const SAMPLE *rptr = (const SAMPLE*)inputBuffer;
    SAMPLE *wptr = &data->recordedSamples[data->frameIndex * NUM_CHANNELS];
    long framesToCalc;
    long i;
    int finished;
    unsigned long framesLeft = data->maxFrameIndex - data->frameIndex;

    (void) outputBuffer; /* Prevent unused variable warnings. */
    (void) timeInfo;
    (void) statusFlags;
    (void) userData;

    if( framesLeft < framesPerBuffer )
    {
        framesToCalc = framesLeft;
        finished = paComplete;
    }
    else
    {
        framesToCalc = framesPerBuffer;
        finished = paContinue;
    }

    if( inputBuffer == NULL )
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = SAMPLE_SILENCE;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = SAMPLE_SILENCE;  /* right */
        }
    }
    else
    {
        for( i=0; i<framesToCalc; i++ )
        {
            *wptr++ = *rptr++;  /* left */
            if( NUM_CHANNELS == 2 ) *wptr++ = *rptr++;  /* right */
        }
    }
    data->frameIndex += framesToCalc;
    return finished;
}

void FFTreader::record(){
    PaStreamParameters  inputParameters;
    PaStream*           stream;
    PaError             err = paNoError;
    paTestData          data;
    int                 i;
    int                 totalFrames;
    int                 numSamples;
    int                 numBytes;



    data.maxFrameIndex = totalFrames = NUM_SECONDS * SAMPLE_RATE; /* Record for a few seconds. */
    data.frameIndex = 0;
    numSamples = totalFrames * NUM_CHANNELS;
    numBytes = numSamples * sizeof(SAMPLE);
    data.recordedSamples = (SAMPLE *) malloc( numBytes ); /* From now on, recordedSamples is initialised. */
    if( data.recordedSamples == NULL )
    {
        printf("Could not allocate record array.\n");
        done(data,err);
        // goto done;
    }
    for( i=0; i<numSamples; i++ ) data.recordedSamples[i] = 0;

    err = Pa_Initialize();
    if( err != paNoError ) done(data,err);

    inputParameters.device = Pa_GetDefaultInputDevice(); /* default input device */
    if (inputParameters.device == paNoDevice) {
        fprintf(stderr,"Error: No default input device.\n");
        done(data,err);
    }
    inputParameters.channelCount = 2;                    /* stereo input */
    inputParameters.sampleFormat = PA_SAMPLE_TYPE;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;


    err = Pa_OpenStream(
              &stream,
              &inputParameters,
              NULL,                  /* &outputParameters, */
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              recordCallback,
              &data );
    if( err != paNoError ) done(data,err);

    err = Pa_StartStream( stream );
    if( err != paNoError ) done(data,err);
    printf("\n=== Now recording!! Please speak into the microphone. ===\n"); fflush(stdout);

    while( ( err = Pa_IsStreamActive( stream ) ) == 1 )
    {
        Pa_Sleep(1000);
        printf("index = %d\n", data.frameIndex ); fflush(stdout);
    }
    if( err < 0 ) done(data,err);
    err = Pa_CloseStream( stream );
    if( err != paNoError ) done(data,err);
    WAV_Writer writer;
    int ret = Audio_WAV_OpenWriter( &writer, file_path.c_str(), 44100, 2 );
    if( ret < 0)
    {
        printf("Could not open file.");
    }
    else
    {

        ret =  Audio_WAV_WriteShorts( &writer, data.recordedSamples, numSamples );
        ret =  Audio_WAV_CloseWriter( &writer );
        if( ret < 0 ) printf("write error.");
        printf("Wrote data to 'recorded.wav'\n");
    }
}

vector<vector<FFTreader::byteType>> FFTreader::parse(){
    

    record();
    Aquila::WaveFile wavData(file_path);
    wav = &wavData;
    END = wav -> getSamplesCount();
    sampleFreq = wav ->getSampleFrequency();
    int right = findStartingPoint();
    int pre_freq = 0, step = sampleFreq/10; 

    vector<int> peak= freqOfindex(right);

    printStatus( right, peak );
    
    if(peak.size()==1 && pre_freq==0 && peak.back() == startchirp){
        //first startchirp chirp
        pre_freq = startchirp;
        right += step;
    }

    //read pkt length
    peak = freqOfindex(right);
    printStatus( right, peak );
    int pkts = soundTo16bits(peak);
    right += step;
    cout <<"Ready to read pkts:"<< pkts<< endl;
    vector<vector<FFTreader::byteType>> ret_data;

    while(pkts-- >0){

        if(right+SIZE > END) break;

        peak = freqOfindex(right);
        printStatus( right, peak );
        int datalen = soundTo16bits(peak);
        right += step;
        cout <<"Ready to read data len :"<< datalen<< endl;
        vector<FFTreader::byteType> pktdata;

        while(datalen > 0){
            if(right+SIZE > END) break;
            peak = freqOfindex(right);
            printStatus( right, peak );
            int content = soundTo16bits(peak);
           

            //if odd, right shift padding
            if(datalen==1) content >>= 8;

            int shift = datalen==1 ? 1 : 2;
            const unsigned short _8bitMask  = 0x00FF;
            
            while(shift-- > 0){
                char d = content & _8bitMask;  
                pktdata.push_back(d);  
                content >>= 8;
            }

            right += step;
            //every data block has two bytes
            datalen -= 2;
            
        }

        ret_data.push_back(pktdata);
    }
    return ret_data;

}
vector<string> FFTreader::dataToStrings(vector<std::vector<int>>& data){

    std::vector<string> ret;
    int pos = 0;

    for(auto& x: data){
        pos++;
        string ret_str = "";
        std::vector<int> ip;
        for(auto tmp :x) {
           if(pos==1){ 
                ip.push_back(tmp);
           }else{ 
                cout << tmp << " ";
                ret_str+=(char) tmp;
           }
        }
        if(ip.size()>0){
                for(int xx=0; xx< ip.size(); xx++){ 
                    if(xx == ip.size()-1) ret_str += std::to_string(ip[xx]);
                    else ret_str+= std::to_string(ip[xx])+'.';
                }
                cout<< "ascii codes";
        }
        cout << endl;
        ret.push_back(ret_str);

    }

    return ret;
}
int FFTreader::soundTo16bits(vector<int>& peak){
    int cur = 0;
    for(auto&x : peak){
        int shift = x - (startchirp-16);
        if(x >= startchirp-16 && x <= startchirp-1) cur |= 1 << shift;
    }
    return cur;
}

void FFTreader::printStatus(int right, vector<int>& peak){

    if(DEBUG_FLAG){

        cout << "@ time " << (double)right/(sampleFreq) << "s"<< endl;
        cout << "Tracked freq (100 Hz): ";
        for(auto&x : peak) cout<< x <<" ";
        cout<<endl;
    }
}
int FFTreader::findStartingPoint(){

    std::size_t start = 0;
    while(start < END){
        vector<int> peak= freqOfindex(start);

        if (peak.size() ==1 && peak.back() == startchirp){
                    break;
        }
        start += SIZE;
    }
    int left = start - SIZE/2, right = start + SIZE/7;
    while(left < right){

        int mid = (left+right)/2;
        // cout << wav.sample(mid) << endl;
        vector<int> peak= freqOfindex(mid);
        if (peak.size() ==1 && peak.back() == startchirp) right = mid;
        else left = mid+1;
    }
    right += SIZE;
    return right;
}

vector<int> FFTreader::findMax(Aquila::SpectrumType spectrum){
    std::size_t halfLength = spectrum.size() / 2;
    std::vector<double> absSpectrum(halfLength);
    double max = 0;
    int peak_freq  = 0;
    std::vector<int> ret;
    
    //search the band of the freq >= 15000
    int start = 0;
    int highpass = startchirp-20;
    for (std::size_t i = start; i < halfLength; ++i)
    {
        absSpectrum[i] = std::abs(spectrum[i]);
        int round_freq = (int)((i-1)*((double)sampleFreq/halfLength)/2 + 50) /100;
        // if(round_freq > highpass) cout << " amp " <<absSpectrum[i] << endl; 


        if(round_freq > highpass && absSpectrum[i-2] < absSpectrum[i-1] && absSpectrum[i-1] > absSpectrum[i] 
            && absSpectrum[i-1] > abs_amp ){
             

             // cout << "original freq " << (i-1)*(sampleFreq/halfLength)/2 << endl;
             if(ret.size() > 0 && ret.back()==round_freq) round_freq++;
             ret.push_back(round_freq);
             // cout << "round freq: "<<round_freq<<endl;
             // cout << " amp " <<absSpectrum[i-1] << endl;
        
        }
        
        if(absSpectrum[i] > max && round_freq > highpass){ 
            max = absSpectrum[i];
            peak_freq = round_freq;
        }
    }
    // cout << " amp " << max << endl; 
    //cout << "peak freq for input with sample size: "<< halfLength*2 << " which needs to be pow of 2" <<endl;
    //cout <<peak_freq << " Hz max amp:" << max << endl;
    //plot(absSpectrum);
    // if(peak_freq < 190) ret.clear();

    return ret;
}

vector<int> FFTreader::freqOfindex(std::size_t start){

    vector<Aquila::SampleType> chunk;
    vector<int> ret;
    for (std::size_t i =start; i< start+SIZE && i < END; ++i)
    {   
        chunk.push_back(wav->sample(i));

    }
   
    if(chunk.size()!=SIZE) return ret; 

    Aquila::SignalSource data(chunk, sampleFreq);

    
    auto fft = Aquila::FftFactory::getFft(SIZE);
    // cout << "\n\nSignal spectrum of time index: "<<start<< endl;
    Aquila::SpectrumType spectrum = fft->fft(data.toArray());
 
    //plt.plotSpectrum(spectrum);
    return findMax(spectrum);
    

}