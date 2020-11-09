#ifndef CONFIG_H
#define CONFIG_H

/*global options file*/

#define URL_MAXSIZE 2048
#define RESPONSE_HEADER_MAXSIZE 10240
#define REQUEST_HEADER_MAXSIZE 10240
#define REQUEST_DATA_MAXSIZE 10240
#define RESPONSE_DATA_MAXSIZE 40960

#define PACKET_MAXSIZE 0x10000  /* maximum total raw packet size */
#define RESOURCE_MAXSIZE 0x400  /* maximum size for http resource identifier */
#define CONNECTION_BUCKETS 8000 /* number of buckets in connection hash table*/
#define MAX_CONNECTIONS 655350  /* maximum number of simultaneous connections*/

int sn_ip_capture;
int sn_ip_frag_capture;
int sn_tcp_capture;
int sn_udp_capture;
int sn_http_capture;
int sn_kafka_output;
int sn_base64_output;
char *sn_idc_name;

#define STRING_LENGTH 1024

#define KAFKA_HOST "x.x.x.x:9092"
#define KAFKA_TOPIC "simpleNids"

#endif
