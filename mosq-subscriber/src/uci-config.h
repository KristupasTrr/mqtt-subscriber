#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uci.h>

struct Topic {
    char topic[100];
    char direction[5];   
    int qos;
};

struct Topic *get_topics();