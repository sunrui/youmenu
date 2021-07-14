/*
 * db provider for oracle
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#include "db_provider_type.h"

#ifdef DB_PROVIDER_HAVE_ORACLE

#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
//#include <oracle.h>

static char dbp_ip[32 + 1];
static int dbp_port;
static char dbp_user[32 + 1];
static char dbp_pwd[32 + 1];
static char sqlite_db[33 + 1];

typedef struct oracle_param
{
	char err[256 + 1];
} oracle_param_t;

extern const dbp_provider_type_t oracle_provider;

void dbp_init_oracle(dbp_config_t * conf)
{
	assert(ip != NULL && port != 0 && user != NULL && pwd != NULL && db != NULL);

	strncpy(dbp_ip, ip, sizeof(dbp_ip) - 1);
	dbp_port = port;
	strncpy(dbp_user, user, sizeof(dbp_user) - 1);
	strncpy(dbp_pwd, pwd, sizeof(dbp_pwd) - 1);
	strncpy(sqlite_db, db, sizeof(sqlite_db) - 1);
}

void dbp_clean_oracle()
{
}

dbp_t dbp_create_new_oracle(char err[256])
{
	assert(0 && "no implement db provider for oracle.");
	return NULL;
}

db_ret dbp_test_connect_oracle(char err[256])
{
	dbp_t sql = dbp_create_new_oracle(err);
	if (sql != NULL)
	{
		dbp_free(&sql);
		return db_ok;
	}

	return db_error;
}

e_dbp_query_result dbp_query_oracle(dbp_t sql, const char * query_sql)
{
	return e_query_error;
}

int dbp_num_fields_oracle(dbp_t sql)
{
	return 0;
}

db_ret dbp_fetch_row_oracle(dbp_t sql, dbp_row_t ** row)
{
	return db_error;
}

db_ret dbp_fetch_row_free_oracle(dbp_t sql, dbp_row_t * row)
{
	return db_error;
}

const char * dbp_errstr_oracle(dbp_t sql)
{
	return NULL;
}

void dbp_free_oracle(dbp_t * sql)
{
}

dbp_ret db_exec_oracle(const char * query_sql, char err[256])
{
	return dbp_ok;
}

const dbp_provider_type_t oracle_provider = {
	provider_oracle,
	dbp_init_oracle,
	dbp_clean_oracle,
	dbp_test_connect_oracle,
	dbp_create_new_oracle,
	dbp_query_oracle,
	dbp_num_fields_oracle,
	dbp_fetch_row_oracle,
	dbp_fetch_row_free_oracle,
	dbp_errstr_oracle,
	dbp_free_oracle,
	db_exec_oracle
};

#endif