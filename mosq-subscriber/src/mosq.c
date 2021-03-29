#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>

#include "db.h"
#include "uci-config.h"

struct Topic *tp;

/* Get and process command line options */
void getopts(int argc, char** argv, char* host, int* port)
{
    int count = 1;

    while (count < argc)
    {
        if (strcmp(argv[count], "-host") == 0)
        {
            if (++count < argc) {
                strcpy(host, argv[count]);
            }
        }
        if (strcmp(argv[count], "-port") == 0) {
            if (++count < argc) {
                *port = atoi(argv[count]);
            }
        }
        count++;
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    int rc;
    printf("Topic: %s\nQoS: %d\nMessage: %s\n\n", msg->topic, msg->qos, (char *)msg->payload);

    int k = 0;
    while (tp[k].qos != -1) {
        if (strcmp(msg->topic, tp[k].topic) == 0) {
            rc = sql_add_message("messages", (char *)msg->payload, (char*)msg->topic);
            break;
        }
    }
}

int main(int argc, char *argv[]) {
    char *host = (char*)malloc(sizeof(char) * 100);
    int port;
    int keep_alive = 60;

    struct mosquitto *mosq;
	int rc;
    
    /* Pass host, topic through argumentus to getopts */
    /* Get host and port */
    getopts(argc, argv, host, &port);

    if (host == NULL || port == NULL) {
        fprintf(stderr, "ERROR: Wrong arguments\n");
        return 1;
    }

    /* Get topics */
    tp = get_topics();

    /* Always initialize lib */
    mosquitto_lib_init();

    /* Create client instance */
    mosq = mosquitto_new(NULL, true, NULL);
    if (mosq == NULL) {
        fprintf(stderr, "ERROR: Out of memory or invalid parameters\n");
        return 1;
    } else {
        fprintf(stdout, "SUCCESS: Mosquitto client instance created\n");
    }

    /* Connect to broker */
    rc = mosquitto_connect(mosq, host, port, keep_alive);
    if (rc != MOSQ_ERR_SUCCESS) {
        mosquitto_destroy(mosq);
        fprintf(stderr, "ERROR: %s\n", mosquitto_strerror(rc));
        return 1;
    } else {
        fprintf(stdout, "SUCCESS: Connected to broker\n");
    }

    /* Subscribe to specific topic */
    if (tp != NULL) {
        int k = 0;
        while (tp[k].qos != -1) {
            mosquitto_subscribe(mosq, NULL, tp[k].topic, tp[k].qos);
            k++;
        }
        
    } else {
        fprintf(stderr, "ERROR: No topics!\n");
    }
    
    /* Set message callback function */
    mosquitto_message_callback_set(mosq, on_message);

    /* Create infinite blocking loop */
    rc = mosquitto_loop_forever(mosq, -1, 1);
    if (rc != MOSQ_ERR_SUCCESS) {
        mosquitto_destroy(mosq);
        fprintf(stderr, "ERROR: %s\n", mosquitto_strerror(rc));
        return 1;
    } else {
        fprintf(stdout, "SUCCESS: Loop started\n");
    }

    /* Free resources */
    mosquitto_lib_cleanup();
    free(tp);
    free(host);

    return 0;
}