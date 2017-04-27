/**
* <h1>SoundDrop Sender Library Prototype</h1>
* This library utilizes PortAudio library to arbitrary data over sound.
*
* @author  Jason Zhao zhao.s.jason@columbia.edu
* @version 1.0
* @since   2017-04-27
*/

#ifndef _SOUND_DROP_H_
#define _SOUND_DROP_H_

#include "protocol.h"
#include "portaudio.h"

using namespace std;

class SoundDrop {
private:
	/* PortAudio components for sending audio */

	/**
	 * PortAudio audio stream structure
	 */
	PaStream* stream;

	/**
	 * For PortAudio error handling
	 */
	PaError err;

	/**
	 * PortAudio configuration helper struct
	 */
	PaStreamParameters outParams;

	/**
	 * SoundDrop audio data encapsulation struct
	 */
	SDData data;

	/**
	 * Sample rate: 44100 Hz
	 */
	static constexpr double SR = 44100.0;

	/**
	 * Frames per buffer: 46 ms buffers
	 */
	static constexpr int FPB = 512;

	/**
	 * Transmission rate: 10 blocks / sec
	 */
	static constexpr double RATE = 0.1;

	/**
	 * Channel count: 2
	 */
	static constexpr int CH = 2;

	/**
	 * PortAudio callback.  Called by PortAudio when audio data is needed
	 * for playback.  This function constructs a complex sine wave by summing
	 * the sine waves of all composite frequencies at each time sample and 
	 * then normalizing.  Please note, function may be called at interrupt level
	 * so don't use any async-unsafe functions like printf() or malloc().
	 */
	static int paCallback( const void                     *inputBuffer, 
						   void                           *outputBuffer, 
						   unsigned long                  framesPerBuffer, 
						   const PaStreamCallbackTimeInfo *timeInfo, 
						   PaStreamCallbackFlags          statusFlags,
						   void                           *userData        );

public:
	/**
	 * SoundDrop constructor
	 */
	SoundDrop() {
		/* Initialize PortAudio */
		err = Pa_Initialize();
		if (err != paNoError) throw runtime_error("PortAudio initialization failed");

		outParams.device = Pa_GetDefaultOutputDevice(); /* Take the default output device. */
		if (outParams.device == paNoDevice) throw runtime_error("no output device found");

		outParams.channelCount = CH;
		outParams.hostApiSpecificStreamInfo = NULL;
		outParams.sampleFormat = paFloat32;             /* 32 bit floating point output. */
		outParams.suggestedLatency = Pa_GetDeviceInfo(outParams.device)->defaultLowOutputLatency;
	}

	/**
	 * SoundDrop destructor
	 */
	~SoundDrop() {
		Pa_Terminate();
	}

	/**
	 * SoundDrop load function.  This function takes in a vector of packets to 
	 * be parsed and constructed into a SoundDrop data stream compliant with
	 * the SoundDrop protocol.
	 * @param   packets  vector of Packets to be encoded into sound and sent 
	 *                   by SoundDrop
	 * @see     Packet
	 */
	void load(vector<Packet> packets);

	/**
	 * Alternative SoundDrop load function.  This function takes in a vector of 
	 * vectors of uint8_t to be parsed and constructed into a SoundDrop data
	 * stream compliant with the SoundDrop protocol.  This function is more C++
	 * idiomatic because it is type-safe (ie. no void *), but places a heavier
	 * burden on the user, which is why we implemented two load() functions.  
	 * The outer vector is the list of "packets", while the inner vector is 
	 * essentially a Packet struct where the vector length is the byte length of 
	 * the packet and the unsigned chars are the bytes.
	 * @param   raw_data  vector of uint8_t to be encoded into sound and sent by 
	 *                    SoundDrop
	 */
	void load(vector<vector<uint8_t>> raw_data);

	/**
	 * Sends the data loaded into SoundDrop with the load() function.
	 */
	void send();
};

#endif // _SOUND_DROP_H_
