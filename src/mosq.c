#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char *host = "192.168.2.1";
int port = 1883;
int keep_alive = 60;

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    printf("Topic: %s\nQoS: %d\nMessage: %s\n\n", msg->topic, msg->qos, (char *)msg->payload);
}

int main(int argc, char *argv[]) {
    struct mosquitto *mosq;
	int rc;

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
    mosquitto_subscribe(mosq, NULL, "topic/status", 0);

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