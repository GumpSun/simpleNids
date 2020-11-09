#include <nids.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <json/json.h>

#include "config.h"
#include "output.h"
#include "hash.h"
#include "stream.h"

/* 使用libnids库接口实现解析*/

#define int_ntoa(x) inet_ntoa(*((struct in_addr *)&x))
struct ethernet_head
{
    u_char dest_mac[6];
    u_char source_mac[6];
    u_short type;
};
struct ip_head
{
    u_char version_header_len;
    u_char ip_tos;
    u_short total_len;
    u_short id;
    u_short off_set;
    u_char ttl;
    u_char protocol;
    u_short check_sum;
    u_char source_ip[4];
    u_char dest_ip[4];
};

void ipFunc(struct ip *pkt, int len) {
    struct json_object *packetJson = json_object_new_object();

    json_object_object_add(packetJson, "src", json_object_new_string(inet_ntoa(pkt->ip_src)));
    json_object_object_add(packetJson, "dst", json_object_new_string(inet_ntoa(pkt->ip_dst)));
    json_object_object_add(packetJson, "total_length", json_object_new_int(len));
    json_object_object_add(packetJson, "protocol", json_object_new_int(pkt->ip_p));
    json_object_object_add(packetJson, "packet_type", json_object_new_string("ip"));

    output(packetJson);
    json_object_put(packetJson);
}

void ipFragFunc(struct ip *pkt, int len) {
    struct json_object *packetJson = json_object_new_object();

    json_object_object_add(packetJson, "src", json_object_new_string(inet_ntoa(pkt->ip_src)));
    json_object_object_add(packetJson, "dst", json_object_new_string(inet_ntoa(pkt->ip_dst)));
    json_object_object_add(packetJson, "total_length", json_object_new_int(len));
    json_object_object_add(packetJson, "protocol", json_object_new_int(pkt->ip_p));
    json_object_object_add(packetJson, "packet_type", json_object_new_string("ip_frag"));

    output(packetJson);
    json_object_put(packetJson);
}

void udpFunc(struct tuple4 *addr, u_char *data, int len, struct ip *pkt) {
    struct json_object *packetJson = json_object_new_object();
    char buffer[1024];
    sprintf(buffer, "%s:%i", int_ntoa(addr->saddr), addr->source);

    json_object_object_add(packetJson, "src", json_object_new_string(buffer));
    sprintf(buffer, "%s:%i", int_ntoa(addr->daddr), addr->dest);
    json_object_object_add(packetJson, "dst", json_object_new_string(buffer));
    json_object_object_add(packetJson, "total_length", json_object_new_int(pkt->ip_len));
    json_object_object_add(packetJson, "data", json_object_new_string((char *)data));
    json_object_object_add(packetJson, "packet_type", json_object_new_string("udp"));

    output(packetJson);
    json_object_put(packetJson);
}

void tcpFunc(struct tcp_stream *ts, void **param) {
    //fprintf(stderr, "TODO: full tcp capture\n");
}

void httpFunc(struct tcp_stream *ts, void **param) {
    struct stream *s;
    struct half_stream *hs;
    struct tuple4 a;

    char buffer[65535];

    memcpy(&a, &ts->addr, sizeof(struct tuple4));

    switch (ts->nids_state) {

        case NIDS_JUST_EST:
            ts->client.collect++;
            ts->server.collect++;

            if ((s = hashFind(&a))) 
            {
                if (!(s = hashDelete(&(s->addr)))) 
                {
                    return;
                }
                json_object_put(s->json);
                free(s);
            }

            if (!(s = malloc(sizeof(struct stream)))) 
            {
                return;
            }

            if(!hashAdd(&a, s)) 
            {
                free(s);
                return;
            }
            sn_streamOpen(s, &a);
            break;

        case NIDS_DATA:
            if (!(s = hashFind(&a))) 
            {
                break;
            }
            if (ts->client.count_new) 
            {
                hs = &ts->client;
                memcpy(buffer, (char *)hs->data, hs->count_new);
                sn_streamWriteResponse(s, buffer, hs->count_new);
            } else {
                hs = &ts->server;
                memcpy(buffer, (char *)hs->data, hs->count_new);
                sn_streamWriteRequest(s, buffer, hs->count_new);
            }
            break;

        default:
            sn_streamDelete(&a);
    }
}

static void hc_syslog(int type, int errnum, struct ip *iph, void *data)
{
    return;
}

int sn_nids_run()
{
    struct nids_chksum_ctl temp;
    temp.netaddr = 0;
    temp.mask = 0;
    temp.action = 1;

    nids_register_chksum_ctl(&temp, 1);

    nids_params.n_tcp_streams = (MAX_CONNECTIONS * 2) / 3 + 1;
    nids_params.syslog = hc_syslog;

    if (!nids_init())
        return (-1);

    if (sn_ip_capture == 1) {
        nids_register_ip(ipFunc);
    }

    if (sn_ip_frag_capture == 1) {
        nids_register_ip_frag(ipFragFunc);
    }

    if (sn_udp_capture == 1) {
        nids_register_udp(udpFunc);
    }

    if (sn_tcp_capture == 1) {
        nids_register_tcp(tcpFunc);
    }
    if (sn_kafka_output == 1) {
        sn_init_output();
    }
    if (sn_http_capture == 1) {
        hashInit();
        sn_stream_init();
        nids_register_tcp(httpFunc);
    }
    fprintf(stderr, "start simpleNids ...\n");
    nids_run();
    fprintf(stderr, "finish simpleNids ....\n");
    return (0);
}

int sn_nids_device(char *dev) {
    nids_params.device = dev;
    return (sn_nids_run());
}
