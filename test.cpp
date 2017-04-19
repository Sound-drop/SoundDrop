/** @file patest_mono.c
	@ingroup test_src
	@brief Play a monophonic sine wave using the Portable Audio api for several seconds.
	@author Phil Burk  http://www.softsynth.com
*/
/*
 * $Id$
 *
 * Authors:
 *    Ross Bencina <rossb@audiomulch.com>
 *    Phil Burk <philburk@softsynth.com>
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */

#include <stdio.h>
#include <math.h>
#include "portaudio.h"

#include "aquila.h"

#include <iostream>
#include <vector>

using namespace std;

#define SAMPLE_RATE        44100
#define FRAMES_PER_BUFFER  512
#define MAGIC_NUMBER       7019
#define OUTPUT_DEVICE      Pa_GetDefaultOutputDevice()

typedef struct {
    vector<vector<float>> sine;
    float total_frames;
    int phase;
} paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    paTestData *data = (paTestData*)userData;
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
            if (freq) {
                //cerr << "freq=" << freq << endl;
                val += (float) sin((float) data->phase / freq);
            }
        }

        // Normalize the values
        //val /= data->sine.size();

        *out++ = val; /* left */
        *out++ = val; /* right */
        data->phase += 1;
    }

    if (--(data->total_frames) <= 0) finished = 1;

    return finished;
}

/*******************************************************************/
int main(void);
int main(void)
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;

    printf("PortAudio Test: output MONO sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);

    /*
    const size_t SIZE = TABLE_SIZE;
    const Aquila::FrequencyType wave = SR, f1 = 1000.0, f2 = 700.0;

    Aquila::SineGenerator sine1(wave);
    sine1.setAmplitude(32).setFrequency(f1).generate(SIZE);
    Aquila::SineGenerator sine2(wave);
    sine2.setAmplitude(0).setFrequency(f2).generate(SIZE);
    auto sum = sine1 + sine2;

    float *ptr = (float *) sum.toArray();

    float max = 0;
    for (int i = 0; i < SIZE; i++) {
        if (*ptr > max) {
            max = *ptr;
        }
    }

    cout << "max=" << max << endl;

    for (int i = 0; i < SIZE; i++, ptr++) {
        cout << *ptr << endl;
        data.sine[i] = *ptr / max;
    }
    */

    float data_length = 100.0;
    float emit_rate = 1;

    paTestData data;
    data.total_frames = (float) SAMPLE_RATE * data_length * emit_rate / (float) FRAMES_PER_BUFFER;
    data.phase = 0;

    /* initialise sinusoidal wavetable */
    for (int i = 1; i <= data_length; i++) {
        for (int j = 0; j < SAMPLE_RATE * emit_rate; j++) {
            //data.sine.push_back({0.359947692, 0.358111224, 0.356293401, 0.350949, 0.334237143});
            data.sine.push_back({0.359947692, 0.358111224, 0.356293401, 0.350949, 0.334237143});
        }
    }

    cout << "size(data.sine)=" << data.sine.size() << endl;
    cout << "size(data.sine[i])=" << data.sine[0].size() << endl;
    
    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = OUTPUT_DEVICE;
    outputParameters.channelCount = 2;       /* MONO output */
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              SAMPLE_RATE,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              patestCallback,
              &data );
    if( err != paNoError ) goto error;

    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;
    
    while ((err = Pa_IsStreamActive(stream)) == 1) Pa_Sleep(100);
    if (err < 0) goto error;

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;
    
    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;
    
    Pa_Terminate();
    printf("Test finished.\n");
    return err;
error:
    Pa_Terminate();
    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}

