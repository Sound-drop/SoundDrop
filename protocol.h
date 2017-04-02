#ifndef _SD_PROTOCOL_H_
#define _SD_PROTOCOL_H_

/* 
 * A simple data encoding protocol for data transmission over audio
 * Audio Spectrum:
 *  - Total: 19500 Hz to 21000 Hz
 *  - Encode hex digit every hundreth frequency from [19.5kHz, 21kHz]
 *     - ie. 0 -> 19.5kHz, 1 -> 19.6kHz, 2 -> 19.7kHz, ...
 */

float encoder[16] = {
	19500.0,    /* 0 */
	19600.0,    /* 1 */
	19700.0,    /* 2 */
	19800.0,    /* 3 */
	19900.0,    /* 4 */
	20000.0,    /* 5 */
	20100.0,    /* 6 */
	20200.0,    /* 7 */
	20300.0,    /* 8 */
	20400.0,    /* 9 */
	20500.0,    /* A */
	20600.0,    /* B */
	20700.0,    /* C */
	20800.0,    /* D */
	20900.0,    /* E */
	21000.0     /* F */
};

float start = 21100.0;
float stop = 21200.0;
float bandwidth = 100.0;


#define MAX_SAMPLES 6400

typedef struct {
	float data[MAX_SAMPLES];
	unsigned int samplesToGo;
} SDData;


#endif // _SD_PROTOCOL_H_