/*
 * db provider for mysql
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#include "db_provider_type.h"

#ifdef DB_PROVIDER_HAVE_MYSQL

#include <mysql.h>

#include <winsock.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static char dbp_ip[32 + 1];
static int dbp_port;
static char dbp_user[32 + 1];
static char dbp_pwd[32 + 1];
static char sqlite_db[33 + 1];
static const char * unix_socket = NULL; // only used for Linux
static unsigned long client_flag = 0;

typedef struct mydbp_param
{
	MYSQL mysql;
	MYSQL * sock;

	MYdbp_RES * result;
	MYdbp_ROW row;

	char err[256 + 1];
} mydbp_param_t;

extern const dbp_provider_type_t mysql_provider;

void dbp_init_mysql(dbp_config_t * conf)
{
	assert(conf->ip != NULL && conf->port != 0 && 
		conf->user != NULL && conf->pwd != NULL && 
		conf->db != NULL);

	strncpy(dbp_ip, conf->ip, sizeof(dbp_ip) - 1);
	dbp_port = conf->port;
	strncpy(dbp_user, conf->user, sizeof(dbp_user) - 1);
	strncpy(dbp_pwd, conf->pwd, sizeof(dbp_pwd) - 1);
	strncpy(sqlite_db, conf->db, sizeof(sqlite_db) - 1);

	{
		/* notice this function is not thread-safe */
		MYSQL mysql;
		mydbp_init(&mysql);
	}
}

void dbp_clean_mysql()
{
}

dbp_t dbp_create_new_mysql()
{
	mydbp_param_t * param;
	dbp_t sql;
	MYSQL mysql, *sock;

	mydbp_init(&mysql);
	sock = mydbp_real_connect(&mysql, dbp_ip, dbp_user, dbp_pwd, sqlite_db, dbp_port, unix_socket, client_flag);
	if (sock == NULL)
	{
		if (err != NULL)
		{
			strncpy(err, mydbp_error(&mysql), 256 - 1);
		}

		fprintf(stderr, "%s\n", mydbp_error(&mysql));
		return NULL;
	}

	sql = (dbp_t)calloc(1, sizeof(dbp_inst_t));
	param = (mydbp_param_t *)calloc(1, sizeof(mydbp_param_t));
	param->mysql = mysql;
	param->sock = sock;
	sql->real_handle = (int)param;
	sql->type = (dbp_provider_type_t *)&mysql_provider;

	return sql;
}

db_ret dbp_test_connect_mysql(char err[256])
{
	dbp_t sql = dbp_create_new_mysql(err);
	if (sql != NULL)
	{
		dbp_free(&sql);
		return db_ok;
	}

	return db_error;
}

e_dbp_query_result dbp_query_mysql(dbp_t sql, const char * query_sql)
{
	mydbp_param_t * param;
	int r;

	assert(sql != NULL && query_sql != NULL);

	param = (mydbp_param_t *)sql->real_handle;
	r = mydbp_query(&param->mysql, query_sql);
	if (r != 0)
	{
		fprintf(stderr, "%s\n", dbp_error(sql));
		return e_query_error;
	}

	param->result = mydbp_store_result(&param->mysql);
	if (param->result != NULL)
	{
		return e_query_row;
	}
	
	return e_query_done;
}

int dbp_num_fields_mysql(dbp_t sql)
{
	mydbp_param_t * param;

	assert(sql != NULL);
	param = (mydbp_param_t *)sql->real_handle;

	if (sql != NULL && param->result != NULL)
	{
		return mydbp_num_fields(param->result);
	}

	return 0;
}

db_ret dbp_fetch_row_mysql(dbp_t sql, dbp_row_t ** row)
{
	mydbp_param_t * param;
	int fields;

	param = (mydbp_param_t *)sql->real_handle;
	param->row = mydbp_fetch_row(param->result);
	if (param->row == NULL)
	{
		return db_error;
	}

	fields = dbp_num_fields(sql);
	assert(fields != 0);
	if (row != NULL)
	{
		unsigned long * length;
		int idx;
		dbp_row_t * myrow;

		myrow = (dbp_row_t *)calloc(1, fields * sizeof(dbp_row_t));
		length = mydbp_fetch_lengths(param->result);

		for (idx = 0; idx < fields; idx++)
		{
			myrow[idx].data = param->row[idx];
			myrow[idx].length = length[idx];
		}

		*row = myrow;
	}

	return db_ok;
}

db_ret dbp_fetch_row_free_mysql(dbp_t sql, dbp_row_t * row)
{
	assert(row != NULL);

	if (row != NULL)
	{
		free(row);
		return db_ok;
	}

	return db_error;
}

const char * dbp_errstr_mysql(dbp_t sql)
{
	mydbp_param_t * param;

	assert(sql != NULL);

	param = (mydbp_param_t *)sql->real_handle;
	strncpy(param->err, mydbp_error(&param->mysql), sizeof(param->err) - 1);

	return param->err;
}

void dbp_free_mysql(dbp_t * sql)
{
	assert(sql != NULL && *sql != NULL);

	if (sql != NULL && *sql != NULL)
	{
		mydbp_param_t * param;
		param = (mydbp_param_t *)(*sql)->real_handle;
		mydbp_free_result(param->result);
		mydbp_close(&param->mysql);
		free(param);
		free(*sql);
		*sql = NULL;
	}
}

dbp_ret db_exec_mysql(const char * query_sql, char err[256])
{
	return dbp_ok;
}

const dbp_provider_type_t mysql_provider = {
	provider_mysql,
	dbp_init_mysql,
	dbp_clean_mysql,
	dbp_test_connect_mysql,
	dbp_create_new_mysql,
	dbp_query_mysql,
	dbp_num_fields_mysql,
	dbp_fetch_row_mysql,
	dbp_fetch_row_free_mysql,
	dbp_errstr_mysql,
	dbp_free_mysql,
	db_exec_mysql
};

#endif