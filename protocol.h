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

double magic = 7019.0;  /* Number used to convert freq to  */

double encoder[16] = {
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

double start_chirp = magic / 20100.0;  /* 20.1 kHz */
double stop_chirp  = magic / 20200.0;  /* 20.2 kHz */
double bandwidth = 100.0;   /* Hz */
double emit_rate = 0.1;     /* Sec */

typedef struct {
    vector<vector<double>> sine;
    double total_frames;
    int phase;
} SDData;

typedef struct {
	uint16_t len;
	void *data;
} Packet;

#endif // _SD_PROTOCOL_H_