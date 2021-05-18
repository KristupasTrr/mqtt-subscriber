#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uci.h>

struct Topic {
    char topic[100];
    char direction[5];
    int qos;
};

struct Event {
    char topic[100];
    char account[100];
    char key[255];
    int value_type;
    int comparison_type;
    char comparison_value[255];
    char email[255];
};

struct Email {
    char name[100];
    int secure_conn;
    char smtp_ip[255];
    int smtp_port;
    int crediantials;
    char username[255];
    char password[255];
    char sender_email[255];
};

struct Topic *get_topics();
struct Event *get_events();
struct Email *get_emails();
