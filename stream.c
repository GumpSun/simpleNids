#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <nids.h>
#include <time.h>
#include <unistd.h>
#include "zlib.h"
#include "base64.h"
#include "config.h"
#include "http_parser.h"
#include "stream.h"
#include "hash.h"
#include "output.h"

http_parser_settings request_settings;
http_parser_settings response_settings;

//响应内容解压
char* unzip_func(unsigned char* source, unsigned long len)
{
  int err;
  z_stream data_stream;

  uLong compressLen, 
  uLong uncompressLen;
  Byte compress[RESPONSE_DATA_MAXSIZE] = {0};
  Byte uncompress[RESPONSE_DATA_MAXSIZE] = {0};

  compressLen = len;
  uncompressLen = RESPONSE_DATA_MAXSIZE;
  memcpy(compress,(Byte*)source, len);
  
  strcpy((char*)uncompress, "garbage");

  data_stream.zalloc = (alloc_func)0;
  data_stream.zfree = (free_func)0;
  data_stream.opaque = (voidpf)0;
  data_stream.next_in = compress;
  data_stream.avail_in = 0;
  data_stream.next_out = uncompress;

  err = inflateInit2(&data_stream, 47);
  if(err != Z_OK)
  {
    printf("inflateInit2 error:%d", err);
    return NULL;
  }

  while (data_stream.total_out < uncompressLen && data_stream.total_in <compressLen) 
  {
    data_stream.avail_in = data_stream.avail_out = 1;
    err = inflate(&data_stream, Z_NO_FLUSH);
    if(err == Z_STREAM_END) break;
    if(err != Z_OK)
    {
      printf("inflate error:%d",err);
      return NULL;
    }
  }
  err = inflateEnd(&data_stream);
  if(err != Z_OK)
  {
      printf("inflateEnd error:%d",err);
      return NULL;
  }
  char* content = (char*) malloc(sizeof(char) * (data_stream.total_out+1));

  memset(content, '\0', data_stream.total_out+1);
  memcpy(content, (char*)uncompress, data_stream.total_out);
  return content;
}



void sn_streamOpen(struct stream *s, struct tuple4 *addr) {
	memset(s, 0, sizeof(struct stream));
	memcpy(&(s->addr), addr, sizeof(struct tuple4));
	sn_streamClean(s);
	s->tmp = 0;
}


void sn_streamClean(struct stream *s) {
	char buffer[1024];
	http_parser_init(&(s->request_parser), HTTP_REQUEST);
	http_parser_init(&(s->response_parser), HTTP_RESPONSE);
        s->is_http = 0;
        s->request_data_size = 0;
        s->response_data_size = 0;
	s->json = json_object_new_object();
	sprintf(buffer, "%s:%i", int_ntoa(s->addr.saddr), s->addr.source);
	json_object_object_add(s->json, "src", json_object_new_string(buffer));
	sprintf(buffer, "%s:%i", int_ntoa(s->addr.daddr), s->addr.dest);
	json_object_object_add(s->json, "dst", json_object_new_string(buffer));
}

void sn_streamWriteRequest(struct stream *s, char *data, u_int32_t size) {
	if (size != 0) {
		http_parser_execute(&(s->request_parser), &request_settings, data, size);
	}

}

void sn_streamWriteResponse(struct stream *s, char *data, u_int32_t size) {
	if (size != 0) {
		http_parser_execute(&(s->response_parser), &response_settings, data, size);
	}

}

void sn_prepare_output(struct stream *s) {
  char *t;

  if (s->request_data_size > 0) 
  {
    s->request_data[s->request_data_size] = '\0';
    if (base64_output == 1) 
    {
      t = Base64Encode(s->request_data, s->request_data_size);
      json_object_object_add(s->json, "request_body", json_object_new_string(t));
      free(t);
    } 
    else {
      json_object_object_add(s->json, "request_body", json_object_new_string(s->request_data));
    }
  }

  if (s->response_data_size > 0) 
  {
    s->response_data[s->response_data_size] = '\0';
    //response body输出base64编码
    if (base64_output == 1) 
    {
      t = Base64Encode(s->response_data, s->response_data_size);
      json_object_object_add(s->json, "response_body", json_object_new_string(t));
      free(t);
    } else {
      //如果response body是gzip编码,解压后输出
      char* buf = s->response_data;
      if(buf[0] == 31 && buf[1] == -117)
      {
        char* p = unzip_func(buf, s->response_data_size);
        t = Base64Encode(p, strlen(p));
        json_object_object_add(s->json, "response_body", json_object_new_string(t));
        free(p);
        free(t);
      }else{
        t = Base64Encode(s->response_data, s->response_data_size);
        json_object_object_add(s->json, "response_body", json_object_new_string(t));
        free(t);
      }
    }
  }
}


void sn_streamClose(struct stream *s) {
  sn_prepare_output(s);
  if (s->is_http != 0) {
	  json_object_object_add(s->json, "packet_type", json_object_new_string("http"));
	  output(s->json);
  }
  json_object_put(s->json);
}

//sn_stream_init使用的注册函数如下
int on_url(http_parser* _, const char* at, size_t length) {
  char *t;
  struct stream* stream = (struct stream*)_;
  size_t real_length = URL_MAXSIZE > length ? length : URL_MAXSIZE;
  memcpy(&(stream->url), at, real_length);
  stream->url[real_length] = '\0';
  stream->is_http = 1;
  json_object_object_add(stream->json, "request_method", json_object_new_string(http_method_str(stream->request_parser.method)));
  if (base64_output == 1) {
    t = Base64Encode(stream->url, real_length);
    json_object_object_add(stream->json, "request_url", json_object_new_string(t));
    free(t);
  } else {
    json_object_object_add(stream->json, "request_url", json_object_new_string(stream->url));
  }
  return 0;
}

int on_request_header_field(http_parser* _, const char* at, size_t length) {
    struct stream* stream = (struct stream*)_;
    size_t real_length = REQUEST_HEADER_MAXSIZE > length ? length : REQUEST_HEADER_MAXSIZE;
    int i;
    memcpy(&(stream->request_cache), "request_", 8);
    memcpy(&(stream->request_cache[8]), at, real_length);
    stream->request_cache[real_length + 8] = '\0';
    for(i = 0; i < real_length + 8; i++) {
		stream->request_cache[i] = tolower(stream->request_cache[i]);

	}

    return 0;
}

int on_request_header_value(http_parser* _, const char* at, size_t length) {
  char value[REQUEST_HEADER_MAXSIZE], *t;
  size_t real_length = REQUEST_HEADER_MAXSIZE > length ? length : REQUEST_HEADER_MAXSIZE;
  struct stream* stream = (struct stream*)_;
  if(stream->request_cache[0] == '\0') return 0;
  memcpy(&value, at, real_length);
  value[real_length] = '\0';
  if (base64_output == 1) {
    t = Base64Encode(value, real_length);
    json_object_object_add(stream->json, stream->request_cache, json_object_new_string(t));
    free(t);
  } else {
    json_object_object_add(stream->json, stream->request_cache, json_object_new_string(value));
  }

  return 0;
}


int on_response_header_field(http_parser* _, const char* at, size_t length) {
    _--;
    struct stream* stream = (struct stream*)_;
    size_t real_length = RESPONSE_HEADER_MAXSIZE > length ? length : RESPONSE_HEADER_MAXSIZE;
    int i;
    memcpy(&(stream->response_cache), "response_", 9);
    memcpy(&(stream->response_cache[9]), at, real_length);
    stream->response_cache[real_length + 9] = '\0';
    for(i = 0; i < real_length + 10; i++) {
		stream->response_cache[i] = tolower(stream->response_cache[i]);

	}

    return 0;
}

int on_response_header_value(http_parser* _, const char* at, size_t length) {
  _--;
  char value[RESPONSE_HEADER_MAXSIZE], *t;
  size_t real_length = RESPONSE_HEADER_MAXSIZE > length ? length : RESPONSE_HEADER_MAXSIZE;
  struct stream* stream = (struct stream*)_;
  if(stream->response_cache[0] == '\0') return 0;
  memcpy(&value, at, real_length);
  value[real_length] = '\0';
  if (base64_output == 1) {
    t = Base64Encode(value, real_length);
    json_object_object_add(stream->json, stream->response_cache, json_object_new_string(t));
    free(t);
  } else {

   /* 去掉response的gzip头，gzip格式的返回报文内存中直接解压 */
    char* gzip = "gzip";
    if(strcmp(gzip,value)==0){
	return 0;
	}
    json_object_object_add(stream->json, stream->response_cache, json_object_new_string(value));
  }
  return 0;
}

int on_status(http_parser* _, const char* at, size_t length) {
  _--;
  struct stream* stream = (struct stream*)_;
  json_object_object_add(stream->json, "response_code", json_object_new_int(stream->response_parser.status_code));
  return 0;
}

int on_request_body(http_parser* _, const char* at, size_t length) {

  struct stream* stream = (struct stream*)_;
  int real_length;

  if (REQUEST_DATA_MAXSIZE > stream->request_data_size) 
  {
    real_length = REQUEST_DATA_MAXSIZE > (stream->request_data_size + length) ? \
    length : (REQUEST_DATA_MAXSIZE - stream->request_data_size);
    memcpy(&(stream->request_data[stream->request_data_size]), at, real_length);
    stream->request_data_size += real_length;
  }
  return 0;
}

int on_response_body(http_parser* _, const char* at, size_t length) {
  _--;
  struct stream* stream = (struct stream*)_;
  int real_length;
  if (RESPONSE_DATA_MAXSIZE > stream->response_data_size) 
  {
    real_length = RESPONSE_DATA_MAXSIZE > (stream->response_data_size + length) ? \
    length : (RESPONSE_DATA_MAXSIZE - stream->response_data_size);
    memcpy(&(stream->response_data[stream->response_data_size]), at, real_length);
    stream->response_data_size += real_length;
  }
  return 0;
}

int session_over(http_parser* _) {
  _--;
  struct stream* stream = (struct stream*)_;
  stream->tmp += 1;
  sn_streamClose(stream);
  sn_streamClean(stream);
  return 0;
}

//
void sn_stream_init() {

  streamCount = 0;
  memset(&request_settings, 0, sizeof(request_settings));
  memset(&response_settings, 0, sizeof(response_settings));

  request_settings.on_url = on_url;
  request_settings.on_header_field = on_request_header_field;
  request_settings.on_header_value = on_request_header_value;
  request_settings.on_body = on_request_body;

  response_settings.on_body = on_response_body;
  response_settings.on_status = on_status;
  response_settings.on_header_field = on_response_header_field;
  response_settings.on_header_value = on_response_header_value;
  response_settings.on_message_complete = session_over;
}

void sn_streamDelete(struct tuple4 *addr) {
	struct stream *s;
	if (!(s = hashDelete(addr))) {
		return;
	}
	sn_streamClose(s);
	free(s);
}
