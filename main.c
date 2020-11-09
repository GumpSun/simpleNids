#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "config.h"
#include "nids.h"
#include "main.h"


void usage(char *name)
{
    printf("\n"
           "Usage: %s [<options>] <name> [<filter>]\n"
           "\n"
           "simpleNids	-i	net interface\n"
           "simpleNids	-f	traffic source\n"
           "simpleNids	-I	ip packets capture\n"
           "simpleNids	-F	ip fragment packets capture\n"
           "simpleNids	-T	tcp packets capture\n"
           "simpleNids	-H	http packets capture\n"
           "simpleNids	-U	udp packets capture\n"
           "simpleNids	-b	base64 output\n"
           "simpleNids	-p	print ouput into kafka\n"
           "simpleNids	-h	help\n"
           "\n",
           name);
    exit(EXIT_FAILURE);
}



int main(int argc, char **argv)
{
    char device_or_pcap[STRING_LENGTH];

    /*simpleNids set*/
    sn_kafka_output = 1;
    sn_ip_capture = 0;
    sn_ip_frag_capture = 0;
    sn_tcp_capture = 0;
    sn_udp_capture = 0;
    sn_http_capture = 0;
    sn_base64_output = 0;
    sn_idc_name = "idcname";

    int sn_result;
    int sn_options;

    const char *short_opt = "i:f:IFTUHbph";
    const struct option long_opt[] =
    {
        {"interface", required_argument, NULL, 'i'},
        {"from", required_argument, NULL, 'f'},
        {"ip-packet", required_argument, NULL, 'I'},
        {"ip-frag-packet", required_argument, NULL, 'F'},
        {"tcp-packet", required_argument, NULL, 'T'},
        {"udp-packet", required_argument, NULL, 'U'},
        {"http-packet", required_argument, NULL, 'H'},
        {"base64", required_argument, NULL, 'b'},
        {"print", required_argument, NULL, 'p'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };

    while ((sn_options = getopt_long(argc, argv, short_opt, long_opt, NULL)) != -1)
    {
        switch (sn_options)
        {
        case 'i':
            strcpy(device_or_pcap, optarg);
            break;
        case 'f':
            sn_idc_name = optarg;
            break;
        case 'I':
            sn_ip_capture = 1;
            break;
        case 'T':
            sn_tcp_capture = 1;
            break;
        case 'U':
            udp_capture = 1;
            break;
        case 'F':
            sn_ip_frag_capture = 1;
            break;
        case 'H':
            sn_http_capture = 1;
            break;
        case 'p':
            sn_kafka_output = 0;
            break;
        case 'b':
            sn_base64_output = 1;
            break;
        case 'h':
            usage(argv[0]);
            return 0;
            break;

        default:
            printf("command line options error\n\n");
            break;
        }
    }

    printf("simpleNids trying to read from '%s'...\n", device_or_pcap);
    sn_result = sm_nids_device(device_or_pcap);

    if (sn_result){printf("simpleNids failed....\n");return (EXIT_FAILURE);}
    else{printf("simpleNids finished...\n");return (EXIT_SUCCESS);}
    return (EXIT_FAILURE);
}
