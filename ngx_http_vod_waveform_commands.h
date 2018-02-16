#define BASE_OFFSET offsetof(ngx_http_vod_loc_conf_t, waveform)

	{ ngx_string("vod_waveform_file_name_prefix"),
	NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_str_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	BASE_OFFSET + offsetof(ngx_http_vod_waveform_loc_conf_t, file_name_prefix),
	NULL },

	{ ngx_string("vod_waveform_interval"),
	NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
	ngx_conf_set_num_slot,
	NGX_HTTP_LOC_CONF_OFFSET,
	BASE_OFFSET + offsetof(ngx_http_vod_waveform_loc_conf_t, interval),
	NULL },

#undef BASE_OFFSET
