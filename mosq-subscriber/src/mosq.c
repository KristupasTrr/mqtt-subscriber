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
void getopts(int argc, char** argv, char* host, int* port, int* tls, char* ca, char* cert, char* key)
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
        if (strcmp(argv[count], "-tls") == 0) {
            if (++count < argc) {
                *tls = atoi(argv[count]);
            }
        }
        if (strcmp(argv[count], "-ca") == 0)
        {
            if (++count < argc) {
                strcpy(ca, argv[count]);
            }
        }
        if (strcmp(argv[count], "-cert") == 0)
        {
            if (++count < argc) {
                strcpy(cert, argv[count]);
            }
        }
        if (strcmp(argv[count], "-key") == 0)
        {
            if (++count < argc) {
                strcpy(key, argv[count]);
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
        k++;
    }
}

int main(int argc, char *argv[]) {
    char *host = (char*)malloc(sizeof(char) * 100);
    int port;
    int keep_alive = 60;
    int tls;
    char *cafile = (char*)malloc(sizeof(char) * 100);
    char *certfile = (char*)malloc(sizeof(char) * 100);
    char *keyfile = (char*)malloc(sizeof(char) * 100);

    struct mosquitto *mosq;
	int rc;
    
    /* Pass host, topic through argumentus to getopts */
    /* Get host and port */
    getopts(argc, argv, host, &port, &tls, cafile, certfile, keyfile);

    fprintf(stdout, "CA: %s\n", cafile);
    fprintf(stdout, "Cert: %s\n", certfile);
    fprintf(stdout, "Key: %s\n", keyfile);

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

    /* If SSL provided */
    if (tls) {
        rc = mosquitto_tls_set(mosq, cafile, NULL, certfile, keyfile, NULL);
        if (rc != MOSQ_ERR_SUCCESS) {
            if (rc == MOSQ_ERR_INVAL) {
                fprintf(stderr, "ERROR: Invalid SSL input parameters\n");
            } else if (rc == MOSQ_ERR_NOMEM) {
                fprintf(stderr, "ERROR: Out of memory\n");
            }
            fprintf(stderr, "ERROR: Can not configure SSL\n");
            return 1;
        }
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
            rc = mosquitto_subscribe(mosq, NULL, tp[k].topic, tp[k].qos);
            if (rc != MOSQ_ERR_SUCCESS) {
                fprintf(stderr, "ERROR: Can not subscribe topic (%s)\n", tp[k].topic);
            }
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
        fprintf(stderr, "ERROR: %s\n", mosquitto_strerror(rc));
        mosquitto_destroy(mosq);
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