#ifndef __WAVEFORM_H__
#define __WAVEFORM_H__

// includes
#include "../media_set.h"
#include <libavcodec/avcodec.h>

// constants
#define WAVEFORM_INPUT_SAMPLE_FORMAT (AV_SAMPLE_FMT_FLTP)

// audio filter encoder functions
vod_status_t waveform_encoder_init(
	request_context_t* request_context,
	uint32_t timescale,
	vod_array_t* frames_array,
	void** result);

vod_status_t waveform_encoder_update_media_info(
	void* context,
	media_info_t* media_info);

vod_status_t waveform_encoder_write_frame(
	void* context,
	AVFrame* frame);

// writer functions
vod_status_t waveform_writer_init(
	request_context_t* request_context,
	media_set_t* media_set,
	uint32_t interval,
	write_callback_t write_callback,
	void* write_context,
	void** result);

vod_status_t waveform_writer_process(
	void* state);

#endif // __WAVEFORM_H__
