#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sqlite3.h>
#include <signal.h>
#include <json-c/json.h>

#include "email.h"
//#include "uci-config.h"
#include "db.h"

struct Topic *tp;
struct Event *events;
struct Email *emails;

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

struct Email *get_email_by_name(const char* name) {
    int k = 0;
    while (emails[k].secure_conn != -1 && emails != NULL) {
        if (strcmp(name, emails[k].name) == 0) {
            return &emails[k];
        }
        k++;
    }
    return NULL;
}

/*
  1: ==
  2: !=
  3: >
  4: >=
  5: <
  6: <=
*/
void execute_topic_events(const char* topic, const char* key, const char* value, enum json_type value_type) {
    int k = 0;
    while (events[k].value_type != -1 && events != NULL) {
        if (strcmp(topic, events[k].topic) == 0 && strcmp(key, events[k].key) == 0) {
            // execute event
            char* msg = NULL;

            // get email
            struct Email *email = get_email_by_name(events[k].account);
            if (email == NULL) {
                fprintf(stderr, "ERROR: NO EMAIL FOUND\n");
                return;
            }
            // if value is string
            if (events[k].value_type == 2 && value_type == json_type_string) {
                // ==
                if (events[k].comparison_type == 1) {
                    if (strcmp(value, events[k].comparison_value) == 0) {
                        // send email
                        asprintf(&msg, "EVENT: EQUAL STRINGS\nEvent value: %s\nReceived value: %s", events[k].comparison_value, value);
                        send_email(msg, events[k].email, email);
                    }
                }
                // !=
                else if (events[k].comparison_type == 2) {
                    if (strcmp(value, events[k].comparison_value) != 0) {
                        // send email
                        asprintf(&msg, "EVENT: NOT EQUAL STRINGS\nEvent value: %s\nReceived value: %s", events[k].comparison_value, value);
                        send_email(msg, events[k].email, email);
                    }
                }
            }

            // if value is double
            else if(events[k].value_type == 1 && (value_type == json_type_int || value_type == json_type_double)) {
                double event_value_converted = atof(events[k].comparison_value);
                double received_value_converted = atof(value);

                // ==
                if (events[k].comparison_type == 1) {
                    if (received_value_converted == event_value_converted) {
                        asprintf(&msg, "EVENT: EQUAL DECIMALS\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }
                // !=
                else if (events[k].comparison_type == 2) {
                    if (received_value_converted != event_value_converted) {
                        asprintf(&msg, "EVENT: NOT EQUAL DECIMALS\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }
                // >
                else if (events[k].comparison_type == 3) {
                    if (received_value_converted > event_value_converted) {
                        // send email
                        asprintf(&msg, "EVENT: RECEIVED VALUE DECIMAL > EVENT VALUE DECIMAL\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }

                // >=
                else if (events[k].comparison_type == 4) {
                    if (received_value_converted >= event_value_converted) {
                        // send email
                        asprintf(&msg, "EVENT: RECEIVED VALUE DECIMAL >= EVENT VALUE DECIMAL\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }

                // <
                else if (events[k].comparison_type == 5) {
                    if (received_value_converted < event_value_converted) {
                        // send email
                        asprintf(&msg, "EVENT: RECEIVED VALUE DECIMAL < EVENT VALUE DECIMAL\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }

                // <=
                else if (events[k].comparison_type == 6) {
                    if (received_value_converted <= event_value_converted) {
                        // send email
                        asprintf(&msg, "EVENT: RECEIVED VALUE DECIMAL <= EVENT VALUE DECIMAL\nEvent value: %f\nReceived value: %f", event_value_converted, received_value_converted);
                        send_email(msg, events[k].email, email);
                    }
                }

            }
            free(msg);
        }
        k++;
    }
}

bool check_for_topic(const char* topic) {
    int k = 0;
    while (tp[k].qos != -1 && tp != NULL) {
        if (strcmp(topic, tp[k].topic) == 0) {
            return true;
        }
        k++;
    }
    return false;
}

void on_message(struct mosquitto *mosq, void *obj, const struct mosquitto_message *msg) {
    int rc;
    printf("Topic: %s\nQoS: %d\nMessage: %s\n\n", msg->topic, msg->qos, (char *)msg->payload);

    bool topic_exists = check_for_topic(msg->topic);

    // add to message database
    sql_add_message("messages", (char *)msg->payload, msg->topic);

    if (topic_exists) {

        struct json_object *jobj;
        // enum json_tokener_error jerr = json_tokener_success;

        jobj = json_tokener_parse((char *)msg->payload);

        const char* key = json_object_get_string(json_object_object_get(jobj, "key"));
        const char* value = json_object_get_string(json_object_object_get(jobj, "value"));
        enum json_type value_type = json_object_get_type(json_object_object_get(jobj, "value"));


        if (key == NULL || value == NULL) {
          fprintf(stderr, "ERROR: Key or value is NULL\n");
          return;
        }

        printf("key: %s\n", key);
        printf("value: %s\n", value);
        printf("value_type: %d\n", value_type);

        execute_topic_events(msg->topic, key, value, value_type);
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

    /* Get topics, events and emails */
    tp = get_topics();
    events = get_events();
    emails = get_emails();

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
    free(events);
    free(emails);
    free(host);

    return 0;
}
