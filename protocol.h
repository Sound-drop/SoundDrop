/**
* <h1>SoundDrop Protocol</h1>
* This protocol is utilized by the SoundDrop library to encode data
* into sound waves.
*
* @author  Jason Zhao zhao.s.jason@columbia.edu
* @version 1.0
* @since   2017-04-27
*/

#ifndef _SD_PROTOCOL_H_
#define _SD_PROTOCOL_H_

#include <vector>

using namespace std;

/* 
 * A simple data encoding protocol for data transmission over audio
 * Audio Spectrum:
 *  - Total: 18500 Hz to 20000 Hz
 *  - Encode binary bit every hundreth frequency from [18.5kHz, 20kHz]
 *     - ie. 0 -> 18.5kHz, 1 -> 18.6kHz, 2 -> 18.7kHz, ...
 */


/* Number used to convert bits to freqs for PortAudio */
static constexpr double magic = 7019.0;

/**
 * encoder is a precomputed table of doubles that represent specific frequencies
 * when played through PortAudio.
 */
static constexpr double encoder[16] = {
	magic / 18500.0,    /* 18.5 kHz, 0000 0000 0000 0001 */
	magic / 18600.0,    /* 18.6 kHz, 0000 0000 0000 0010 */
	magic / 18700.0,    /* 18.7 kHz, 0000 0000 0000 0100 */
	magic / 18800.0,    /* 18.8 kHz, 0000 0000 0000 1000 */
	magic / 18900.0,    /* 18.9 kHz, 0000 0000 0001 0000 */
	magic / 19000.0,    /* 19.0 kHz, 0000 0000 0010 0000 */
	magic / 19100.0,    /* 19.1 kHz, 0000 0000 0100 0000 */
	magic / 19200.0,    /* 19.2 kHz, 0000 0000 1000 0000 */
	magic / 19300.0,    /* 19.3 kHz, 0000 0001 0000 0000 */
	magic / 19400.0,    /* 19.4 kHz, 0000 0010 0000 0000 */
	magic / 19500.0,    /* 19.5 kHz, 0000 0100 0000 0000 */
	magic / 19600.0,    /* 19.6 kHz, 0000 1000 0000 0000 */
	magic / 19700.0,    /* 19.7 kHz, 0001 0000 0000 0000 */
	magic / 19800.0,    /* 19.8 kHz, 0010 0000 0000 0000 */
	magic / 19900.0,    /* 19.9 kHz, 0100 0000 0000 0000 */
	magic / 20000.0     /* 20.0 kHz, 1000 0000 0000 0000 */
};

static constexpr double start_chirp = magic / 20100.0;  /* 20.1 kHz */
static constexpr double stop_chirp  = magic / 20200.0;  /* 20.2 kHz */
static constexpr double bandwidth = 100.0;   /* Hz */
static constexpr double emit_rate = 0.1;     /* Sec */

/**
 * SDData is an internal data structure used to store information about the
 * converted audio data.  SDData is read in the PortAudio callback to actually
 * play the sound.
 */
typedef struct {

	/**
	 * sine is the underlying data structure that holds audio data that is
	 * passed to the PortAudio callback when data is need.  The outermost
	 * vector represents the time domain of the sine wave, while the inner
	 * vector represents the different fundamental frequencies of the complex
	 * sine wave.  The actual complex wave is constructed on-the-fly when
	 * data is needed in the paCallback() function.
	 */
    vector<vector<double>> sine;

    /**
     * Total number of frames.  Each frame is a small buffer for PortAudio.
     * The total number is calculated by dividing the total number of samples
     * by the frames per buffer (SR * RATE * LEN / FPB).  SR is sample rate, 
     * RATE is block transmission rate (100ms), and LEN is the word length
     * of the data.
     */
    double total_frames;

    /**
     * Keeps track of time domain (ie. phase) of the sine wave during wave
     * construction and playback.
     */
    int phase;

} SDData;

/**
 * Packet is the fundamental data transfer unit in the SoundDrop protocol.
 * All data sent through SoundDrop must be wrapped in this struct.
 */
typedef struct {

	/**
     * Byte length of data pointed to byte data pointer.
     */
	uint16_t len;

	/**
     * data is a void pointer to the raw data to be encoded and send.  len
     * bytes will be read from this pointer.
     */
	void *data;

} Packet;

#endif // _SD_PROTOCOL_H_