#include <sqlite3.h>

sqlite3 *sqlopen(char *dbpath);
int sqlclose(sqlite3 *db);
int sql_add_message(char *table, char *msg, char *topic);