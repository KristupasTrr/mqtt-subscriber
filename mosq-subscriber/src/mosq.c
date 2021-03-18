#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>

#include "db.h"

const char *host = NULL;
int port;
int keep_alive = 60;

/* Get and process command line options */
void getopts(int argc, char** argv)
{
    int count = 1;

    while (count < argc)
    {
        if (strcmp(argv[count], "-host") == 0)
        {
            if (++count < argc) {
                host = argv[count];
            }
        }
        if (strcmp(argv[count], "-port") == 0) {
            if (++count < argc) {
                port = atoi(argv[count]);
            }
        }
        count++;
    }
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    int rc;
    printf("Topic: %s\nQoS: %d\nMessage: %s\n\n", msg->topic, msg->qos, (char *)msg->payload);

    if (strcmp(msg->topic,"topic/print") == 0) {
        rc = sql_print_table("messages");
    }
    else if (strcmp(msg->topic,"topic/add") == 0) {
        rc = sql_add_message("messages", (char *)msg->payload);
    }

}

int main(int argc, char *argv[]) {
    struct mosquitto *mosq;
	int rc;

    /* Get host and port */
    getopts(argc, argv);

    if (host == NULL || port == NULL) {
        fprintf(stderr, "ERROR: Wrong arguments\n");
        return 1;
    }


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
    mosquitto_subscribe(mosq, NULL, "topic/print", 0);
    mosquitto_subscribe(mosq, NULL, "topic/add", 0);

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

    return 0;
}