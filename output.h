#ifndef OUTPUT_H
#define OUTPUT_H
#include <json/json.h>

#define int_ntoa(x)     inet_ntoa(*((struct in_addr *)&x))

void sn_init_output();
int output(struct json_object *resJson);

#endif
