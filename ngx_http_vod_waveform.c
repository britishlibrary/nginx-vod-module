#include <ngx_http.h>
#include "ngx_http_vod_submodule.h"
#include "ngx_http_vod_utils.h"
#include "vod/filters/waveform.h"

#define VOLUME_MAP_TIMESCALE (1000)

static const u_char json_file_ext[] = ".json";
static const u_char dat_file_ext[] = ".dat";
//static const u_char data_content_type[] = "application/octet_stream";
static const u_char json_content_type[] = "application/json";


static ngx_int_t
ngx_http_vod_waveform_init_frame_processor(
	ngx_http_vod_submodule_context_t* submodule_context,
	segment_writer_t* segment_writer,
	ngx_http_vod_frame_processor_t* frame_processor,
	void** frame_processor_state,
	ngx_str_t* output_buffer,
	size_t* response_size,
	ngx_str_t* content_type)
{
	vod_status_t rc;

	rc = waveform_writer_init(
		&submodule_context->request_context,
		&submodule_context->media_set,
		submodule_context->conf->waveform.interval,
		segment_writer->write_tail,
		segment_writer->context,
		frame_processor_state);
	if (rc != VOD_OK)
	{
		ngx_log_debug1(NGX_LOG_DEBUG_HTTP, submodule_context->request_context.log, 0,
			"ngx_http_vod_waveform_init_frame_processor: waveform_writer_init failed %i", rc);
		return ngx_http_vod_status_to_ngx_error(submodule_context->r, rc);
	}

	*frame_processor = (ngx_http_vod_frame_processor_t)waveform_writer_process;
  
	content_type->len = sizeof(json_content_type) - 1;
	content_type->data = (u_char *)json_content_type;

	return NGX_OK;
}

static const ngx_http_vod_request_t waveform_request = {
	REQUEST_FLAG_SINGLE_TRACK | REQUEST_FLAG_PARSE_ALL_CLIPS,
	PARSE_FLAG_FRAMES_ALL | PARSE_FLAG_EXTRA_DATA,
	REQUEST_CLASS_OTHER,
	VOD_CODEC_FLAG(AAC),
	VOLUME_MAP_TIMESCALE,
	NULL,
	ngx_http_vod_waveform_init_frame_processor,
};

static void
ngx_http_vod_waveform_create_loc_conf(
	ngx_conf_t *cf,
	ngx_http_vod_waveform_loc_conf_t *conf)
{
	conf->interval = NGX_CONF_UNSET_UINT;
}

static char *
ngx_http_vod_waveform_merge_loc_conf(
	ngx_conf_t *cf,
	ngx_http_vod_loc_conf_t *base,
	ngx_http_vod_waveform_loc_conf_t *conf,
	ngx_http_vod_waveform_loc_conf_t *prev)
{
	ngx_conf_merge_str_value(conf->file_name_prefix, prev->file_name_prefix, "waveform");
	ngx_conf_merge_uint_value(conf->interval, prev->interval, 1000);
	return NGX_CONF_OK;
}

static int 
ngx_http_vod_waveform_get_file_path_components(ngx_str_t* uri)
{
	return 1;
}

static ngx_int_t
ngx_http_vod_waveform_parse_uri_file_name(
	ngx_http_request_t *r,
	ngx_http_vod_loc_conf_t *conf,
	u_char* start_pos,
	u_char* end_pos,
	request_params_t* request_params,
	const ngx_http_vod_request_t** request)
{
	ngx_int_t rc;
 /* TODO: use .dat or .json formaters */
	if (ngx_http_vod_match_prefix_postfix(start_pos, end_pos, &conf->waveform.file_name_prefix, dat_file_ext))
	{
		start_pos += conf->waveform.file_name_prefix.len;
		end_pos -= (sizeof(dat_file_ext) - 1);
		*request = &waveform_request;
	}
  else if (ngx_http_vod_match_prefix_postfix(start_pos, end_pos, &conf->waveform.file_name_prefix, json_file_ext))
	{
		start_pos += conf->waveform.file_name_prefix.len;
		end_pos -= (sizeof(json_file_ext) - 1);
		*request = &waveform_request;
	}
  else
	{
		ngx_log_error(NGX_LOG_ERR, r->connection->log, 0,
			"ngx_http_vod_waveform_parse_uri_file_name: unidentified request");
		return ngx_http_vod_status_to_ngx_error(r, VOD_BAD_REQUEST);
	}

	// parse the required tracks string
	rc = ngx_http_vod_parse_uri_file_name(r, start_pos, end_pos, 0, request_params);
	if (rc != NGX_OK)
	{
		ngx_log_debug1(NGX_LOG_DEBUG_HTTP, r->connection->log, 0,
			"ngx_http_vod_waveform_parse_uri_file_name: ngx_http_vod_parse_uri_file_name failed %i", rc);
		return rc;
	}

	request_params->tracks_mask[MEDIA_TYPE_VIDEO] = 0;
	request_params->tracks_mask[MEDIA_TYPE_SUBTITLE] = 0;

	return NGX_OK;
}

static ngx_int_t
ngx_http_vod_waveform_parse_drm_info(
	ngx_http_vod_submodule_context_t* submodule_context,
	ngx_str_t* drm_info,
	void** output)
{
	ngx_log_error(NGX_LOG_ERR, submodule_context->request_context.log, 0,
		"ngx_http_vod_waveform_parse_drm_info: unexpected - drm enabled on waveform request");
	return ngx_http_vod_status_to_ngx_error(submodule_context->r, VOD_BAD_REQUEST);
}

DEFINE_SUBMODULE(waveform);
