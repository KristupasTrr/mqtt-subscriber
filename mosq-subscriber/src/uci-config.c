#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <uci.h>

#include "uci-config.h"

struct Topic *get_topics() {

    const char *config_name = "mosq-subscriber";

    struct uci_context *context = uci_alloc_context();
    struct uci_package *package;

    // It is also possible to load the config with uci_lookup_ptr(),
    // or from a file stream using uci_import()
    if (uci_load(context, config_name, &package) != UCI_OK)
    {
        uci_perror(context, "uci_load()");
        uci_free_context(context);
        return NULL;
    }


    int k = 0;
    struct Topic *topics;
    topics = malloc(sizeof(struct Topic) * 1);


    struct uci_element *i, *j;    // Iteration variables
    uci_foreach_element(&package->sections, i)
    {
        struct uci_section *section = uci_to_section(i);
        char *section_type = section->type;

        if (strcmp(section->type, "topic") == 0) {
            if (k >= 1) {
                topics = (struct Topic *) realloc(topics, sizeof(struct Topic) * (k + 1));
            }

            uci_foreach_element(&section->options, j)
            {
                struct uci_option *option = uci_to_option(j);
                char *option_name = option->e.name;

                if(strcmp(option_name, "topic") == 0) {
                    strcpy(topics[k].topic, option->v.string);
                }
                else if (strcmp(option_name, "qos") == 0) {
                    topics[k].qos = atoi(option->v.string);
                }
                else {
                    strcpy(topics[k].direction, option->v.string);
                }
            }
            k++;
        }

        
    }

    uci_free_context(context);

    if (k == 0) {
        return NULL;
    } else {
        topics = (struct Topic *) realloc(topics, sizeof(struct Topic) * (k + 1));
        topics[k].qos = -1;
    }

    return topics;
}
