#ifndef STREAM_H
#define STREAM_H

#include <stdio.h>
#include <stdlib.h>
#include <nids.h>
#include <json/json.h>

#include "config.h"
#include "http_parser.h"

struct stream {
	http_parser request_parser;
	http_parser response_parser;

	char url[URL_MAXSIZE + 1];

	int request_data_size;
	int response_data_size;

	char request_cache[REQUEST_HEADER_MAXSIZE + 1];
	char response_cache[REQUEST_HEADER_MAXSIZE + 1];

	char request_data[REQUEST_DATA_MAXSIZE + 1];
	char response_data[RESPONSE_DATA_MAXSIZE + 1];
	
	struct tuple4 addr;
	int is_http;
	int tmp;
	json_object *json;
};

int streamCount;

void sn_stream_init();
char* unzip_func(unsigned char* source,unsigned long len);
void sn_streamOpen(struct stream *s, struct tuple4 *addr);
void sn_streamWriteRequest(struct stream *stream, char *data, u_int32_t size);
void sn_treamWriteResponse(struct stream *stream, char *data, u_int32_t size);
void sn_streamClose(struct stream *s);
void sn_streamClean(struct stream *s);
void sn_streamDelete(struct tuple4 *addr);

#endif
