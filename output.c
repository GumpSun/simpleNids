#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <librdkafka/rdkafka.h>

#include "output.h"
#include "config.h"

int partition = RD_KAFKA_PARTITION_UA;
rd_kafka_topic_t *rktt;

void sn_init_output() {

    char tmp[16];
    char errstr[512];

    char *brokers = KAFKA_HOST;
    char *topic = KAFKA_TOPIC;
    rd_kafka_conf_t *conf;
    rd_kafka_topic_conf_t *topic_conf;
    
    conf = rd_kafka_conf_new();
    snprintf(tmp, sizeof(tmp), "%i", SIGIO);
    rd_kafka_conf_set(conf, "internal.termination.signal", tmp, NULL, 0);
    topic_conf = rd_kafka_topic_conf_new();

    rd_kafka_t *rkt = NULL;
    if (!(rkt = rd_kafka_new(RD_KAFKA_PRODUCER, conf, errstr, sizeof(errstr))))
    {
        fprintf(stderr, "%% create new producer failed: %s\n", errstr);
        exit(1);
    }

    if (rd_kafka_brokers_add(rkt, brokers) == 0)
    {
        fprintf(stderr, "%% no valid brokers specified\n");
        exit(1);
    }

    rktt = rd_kafka_topic_new(rkt, topic, topic_conf);
}


int output(struct json_object *resJson) {

    time_t tt = time(NULL);
    json_object_object_add(resJson, "time", json_object_new_int(tt));
    json_object_object_add(resJson, "from", json_object_new_string(idc_name));

    const char *kafkaMsg = json_object_to_json_string(resJson);
    if (kafka_output == 1)
    {
        if (rkt == NULL)
            printf("rkt is null\n");
        if (rd_kafka_produce(rkt, partition, RD_KAFKA_MSG_F_COPY, kafkaMsg, strlen(kafkaMsg), NULL, 0, NULL) == -1) {
            fprintf(stderr, "%% produce to topic failed %s "
                "partition %i: %s\n",
                rd_kafka_topic_name(rkt),
                partition,
                rd_kafka_err2str(rd_kafka_errno2err(errno)));
        }
        return 0;
    }
    else
    {
        fprintf(stderr, "%s\n", kafkaMsg);
        return 0;
    }
}
