#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

char *db_path = "/etc/config/mqtt-msg.db";

sqlite3 *sql_open(char *dbpath) {
    sqlite3 *db;
    int rc;
    rc = sqlite3_open(dbpath, &db);
    if (rc)
        fprintf(stderr, "ERROR: Can't open database: %s" , sqlite3_errmsg(db));

    return db;
}

int sql_close(sqlite3 *db) {
    return sqlite3_close(db);
}

int sql_check_table_existance(sqlite3 *db, char *table) {
    int rc;
    sqlite3_stmt *stmt;

    char *sql = NULL;
    asprintf(&sql, "SELECT COUNT(TYPE) FROM sqlite_master WHERE TYPE='table' AND NAME='%s';", table);

    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Can't prepare stmt object\n)");
        goto error;
    }
    sqlite3_step(stmt);
    rc = sqlite3_column_int(stmt, 0);

error:
    free(sql);
    sqlite3_finalize(stmt);

    return rc;
}

int sql_add_table(sqlite3 *db, char *table) {
    int rc;
    char *err_msg = 0;
    sqlite3_stmt *stmt;

    char *sql = NULL;
    asprintf(&sql, "CREATE TABLE %s (id INTEGER PRIMARY KEY AUTOINCREMENT, message TEXT NOT NULL, topic TEXT NOT NULL, timestamp DATETIME DEFAULT CURRENT_TIMESTAMP NOT NULL)", table);

    rc = sqlite3_exec(db, sql, 0, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Failed to add table. %s\n", err_msg);
    }

    free(sql);
    sqlite3_finalize(stmt);
    return rc;
}

int sql_add_message(char *table, char *msg, char *topic) {
    int rc;
    char *err_msg = 0;

    sqlite3 *db = sql_open(db_path);
    sqlite3_stmt *stmt = NULL;


    rc = sql_check_table_existance(db, table);
    if (rc == 0) {
        fprintf(stdout, "INFO: Table does not exist\n");
        rc = sql_add_table(db, table);
        if (rc != SQLITE_OK) {
            // print error
            fprintf(stderr, "ERROR: Could not add table\n");
            goto error;
        }
        fprintf(stdout, "SUCCESS: Table added\n");
    } else if (rc != 1) {
        fprintf(stderr, "ERROR: Received error while checking for table\n");
        goto error;
    } else {
        fprintf(stdout, "INFO: Table exists\n");
    }

    char *sql = NULL;
    asprintf(&sql, "INSERT INTO %s(message, topic) VALUES('%s', '%s')", table, msg, topic);

    rc = sqlite3_exec(db, sql, 0, NULL, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "ERROR: Failed to add message. %s\n", err_msg);
    }

    fprintf(stdout, "SUCCESS: Created and added message \n");
    free(sql);
error:
    sqlite3_finalize(stmt);
    sql_close(db);

    return rc;
}
