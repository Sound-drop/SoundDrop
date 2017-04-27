/**
* <h1>SoundDrop Sender Library</h1>
* This file implements the methods and classes described in
* the sound_drop.h prototype.
*
* @author  Jason Zhao zhao.s.jason@columbia.edu
* @version 1.0
* @since   2017-04-27
*/

#include <stdexcept>
#include <cmath>
#include <iostream>

#include "sound_drop.h"

using namespace std;

/* Callback for PortAudio */
int SoundDrop::paCallback( const void                      *inputBuffer,
						   void                            *outputBuffer,
						   unsigned long                   framesPerBuffer,
						   const PaStreamCallbackTimeInfo  *timeInfo,
						   PaStreamCallbackFlags           statusFlags,
						   void                            *userData) 
{
	SDData *data = (SDData *) userData;
	float *out = (float *) outputBuffer;
	int finished = 0;

	/* avoid unused variable warnings */
	(void) inputBuffer;
	(void) timeInfo;
	(void) statusFlags;

	for(int i = 0; i < framesPerBuffer; i++)
	{
		double val = 0;
		for (double freq : data->sine[data->phase]) {
			val += sin((double) data->phase / freq) / (double) data->sine[data->phase].size();
		}

		*out++ = val; /* left */
		*out++ = val; /* right */
		data->phase += 1;
	}

	if (--(data->total_frames) <= 0) finished = 1;

	return finished;
}

void SoundDrop::load(vector<Packet> packets) {
	/* Clear out any previously set values */
	data.sine.clear();

	/* Calculate total length of transmission */
	int byte_len = 0;
	uint16_t elements = packets.size();
	for (Packet &p : packets) {
		byte_len += p.len;
		byte_len += 2;                    /* Add length field */

		if (p.len % 2 == 1) byte_len++;   /* Round up packet length to even num */
	}

	int word_len = byte_len / 2;
	word_len += 3;                        /* Add start chirp, element count, end chirp */

	int block_len = SR * RATE;
	data.total_frames = static_cast<double>(block_len * ((double) word_len) / (double) FPB);
	data.phase = 0;

	/* Add start chirp */
	for (int i = 0; i < block_len; i++) {
		data.sine.push_back({start_chirp});
	}

	/* Add element count */
	for (int i = 0; i < block_len; i++) {
		vector<double> data_point;
		for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
			if (elements & x) {
				data_point.push_back(encoder[y]);
			}
		}

		data.sine.push_back(move(data_point));
	}

	/* Encode all packets */
	for (Packet &p : packets) {

		/* Encode length of packet */
		vector<double> data_point;
		for (int j = 0; j < block_len; j++) {

			/* Encode bits */
			for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
				if (p.len & x) {
					data_point.push_back(encoder[y]);
				}
			}

			data.sine.push_back(move(data_point));
		}

		/* Encode packet data */
		uint16_t *ptr = (uint16_t *) p.data;
		for (int i = 0; i < p.len / 2; i++) {

			/* Encode a full block length */
			vector<double> data_point;
			for (int j = 0; j < block_len; j++) {

				/* Encode bits */
				for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
					if (*ptr & x) {
						data_point.push_back(encoder[y]);
					}
				}

				data.sine.push_back(move(data_point));
			}

			ptr++;
		}

		/* If odd length data packet, add last byte by itself */
		if (p.len % 2 == 1) {

			*ptr = *ptr & 0x00ff;      /* Zero out second byte of ptr */
			vector<double> data_point;
			for (int j = 0; j < block_len; j++) {

				/* Encode bits */
				for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
					if (*ptr & x) {
						data_point.push_back(encoder[y]);
					}
				}

				data.sine.push_back(move(data_point));
			}

		}

		/* Done encoding packet, onto the next one! */
	}

	/* Add end chirp */
	for (int i = 0; i < block_len; i++) {
		data.sine.push_back({stop_chirp});
	}

	/* All done!  Send transmission when ready... */
}

void SoundDrop::load(vector<vector<uint8_t>> raw_data) {
	/* Clear out any previously set values */
	data.sine.clear();

	/* Calculate total length of transmission */
	int byte_len = 0;
	uint16_t elements = raw_data.size();
	for (vector<uint8_t> &p : raw_data) {
		byte_len += p.size();
		byte_len += 2;                       /* Add length field */

		if (p.size() % 2 == 1) byte_len++;   /* Round up packet length to even num */
	}

	int word_len = byte_len / 2;
	word_len += 3;                           /* Add start chirp, element count, end chirp */

	int block_len = SR * RATE;
	data.total_frames = static_cast<double>(block_len * ((double) word_len) / (double) FPB);
	data.phase = 0;

	/* Add start chirp */
	for (int i = 0; i < block_len; i++) {
		data.sine.push_back({start_chirp});
	}

	/* Add element count */
	for (int i = 0; i < block_len; i++) {
		vector<double> data_point;
		for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
			if (elements & x) {
				data_point.push_back(encoder[y]);
			}
		}

		data.sine.push_back(move(data_point));
	}

	for (vector<uint8_t> &p : raw_data) {

		/* Encode length of packet */
		vector<double> data_point;
		for (int j = 0; j < block_len; j++) {

			/* Encode bits */
			for (uint32_t x = 1, y = 0; x <= (1 << 15); x *= 2, y++) {
				if (p.size() & x) {
					data_point.push_back(encoder[y]);
				}
			}

			data.sine.push_back(move(data_point));
		}

		/* Encode packet data up to even byte */
		for (int i = 0; i < (p.size() / 2) * 2; i += 2) {

			/* Encode a full block length one byte at a time */
			vector<double> data_point;
			for (int j = 0; j < block_len; j++) {

				/* Encode bits */
				for (uint32_t x = 1, y = 0; x <= (1 << 7); x *= 2, y++) {
					if (p[i] & x) {
						data_point.push_back(encoder[y]);
					}

					if (p[i + 1] & x) {
						data_point.push_back(encoder[y + 8]);
					}
				}

				data.sine.push_back(move(data_point));
			}

		}

		/* If odd length data packet, add last byte by itself */
		if (p.size() % 2 == 1) {

			vector<double> data_point;
			for (int j = 0; j < block_len; j++) {

				/* Encode bits */
				for (uint32_t x = 1, y = 0; x <= (1 << 7); x *= 2, y++) {
					if (p[p.size() - 1] & x) {
						data_point.push_back(encoder[y]);
					}
				}

				data.sine.push_back(move(data_point));
			}

		}

		/* Done encoding packet, onto the next one! */
	}

	/* Add end chirp */
	for (int i = 0; i < block_len; i++) {
		data.sine.push_back({stop_chirp});
	}

	/* All done!  Send transmission when ready... */
}

void SoundDrop::send() {
	err = Pa_OpenStream(&stream, NULL, &outParams, SR, FPB, paClipOff, paCallback, &data);
	if (err != paNoError) throw runtime_error("PortAudio failed to open stream");

	err = Pa_StartStream(stream);
	if (err != paNoError) throw runtime_error("PortAudio failed to start stream");

	/* Wait until all data has been sent */
	while ((err = Pa_IsStreamActive(stream)) == 1) Pa_Sleep(100);
	if (err < 0) throw runtime_error("unknown error");

	err = Pa_CloseStream(stream);
	if (err != paNoError) throw runtime_error("PortAudio failed to close stream");

}
