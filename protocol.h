#ifndef _SD_PROTOCOL_H_
#define _SD_PROTOCOL_H_

#include <vector>

using namespace std;

/* 
 * A simple data encoding protocol for data transmission over audio
 * Audio Spectrum:
 *  - Total: 19500 Hz to 21000 Hz
 *  - Encode hex digit every hundreth frequency from [19.5kHz, 21kHz]
 *     - ie. 0 -> 19.5kHz, 1 -> 19.6kHz, 2 -> 19.7kHz, ...
 */

float encoder[16] = {
	0.359948718,    /* 19.5 kHz, 0000 0000 0000 0001 */
	0.358112245,    /* 19.6 kHz, 0000 0000 0000 0010 */
	0.356294416,    /* 19.7 kHz, 0000 0000 0000 0100 */
	0.354494949,    /* 19.8 kHz, 0000 0000 0000 1000 */
	0.352713568,    /* 19.9 kHz, 0000 0000 0001 0000 */
	0.350950000,    /* 20.0 kHz, 0000 0000 0010 0000 */
	0.349203980,    /* 20.1 kHz, 0000 0000 0100 0000 */
	0.347475248,    /* 20.2 kHz, 0000 0000 1000 0000 */
	0.345763547,    /* 20.3 kHz, 0000 0001 0000 0000 */
	0.344068627,    /* 20.4 kHz, 0000 0010 0000 0000 */
	0.342390244,    /* 20.5 kHz, 0000 0100 0000 0000 */
	0.340728155,    /* 20.6 kHz, 0000 1000 0000 0000 */
	0.339082126,    /* 20.7 kHz, 0001 0000 0000 0000 */
	0.337451923,    /* 20.8 kHz, 0010 0000 0000 0000 */
	0.335837321,    /* 20.9 kHz, 0100 0000 0000 0000 */
	0.334238095     /* 21.0 kHz, 1000 0000 0000 0000 */
};

float chirp     = 0.332654028; /* 21.1 kHz */
float delimiter = 0.331084906; /* 21.2 kHz */
float bandwidth = 100.0;   /* Hz */
float emit_rate = 0.1;     /* Sec */

typedef struct {
    vector<vector<float>> sine;
    float total_frames;
    int phase;
} SDData;

typedef struct {
	uint16_t len;
	void *data;
} Packet;

#endif // _SD_PROTOCOL_H_