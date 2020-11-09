#ifndef NIDS_H
#define NIDS_H


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

int sn_nids_device(char *dev);

#endif
