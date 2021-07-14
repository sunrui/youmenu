/*
 * db provider for sqlite3
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#include "db_provider_type.h"

#ifdef DB_PROVIDER_HAVE_SQLITE

#include <sqlite3.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include <pthread.h>

#ifdef WIN32
#include <Windows.h>
#define mkdir(dir, mode) _mkdir(dir)
#define strdup _strdup
#define access _access
#define pthread_mutex_t CRITICAL_SECTION
#define pthread_mutex_init(mutex, attr) InitializeCriticalSection(mutex)
#define pthread_mutex_lock(mutex) EnterCriticalSection(mutex)
#define pthread_mutex_unlock(mutex) LeaveCriticalSection(mutex)
#define pthread_mutex_destroy(mutex) DeleteCriticalSection(mutex)
#endif

typedef struct sqlite_param
{
	sqlite3 * h_sql;
	pthread_mutex_t lock;
	
	sqlite3_stmt * stmt;
	const char * zTail;

	int fields;
	int row;
} sqlite_param_t;

extern const dbp_provider_type_t sqlite_provider;

sqlite3 * dbp_init_sqlite(dbp_conf_t * conf)
{
	static sqlite3 * h_sql;

	assert(conf->db != NULL);
	
	if (access(conf->db, 0) != 0)
	{
		char * path;
		char * tail;
		int r;
		
		r = 0;
		path = strdup(conf->db);
		tail = strrchr(path, '\\');
		if (tail == NULL) tail = strrchr(path, '/');
		if (tail != NULL) *tail = 0;
		if (tail != NULL && access(path, 0) == -1)
		{
			r = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
		}
		free(path);
		if (r != 0)
		{
			assert(0);
			return NULL;
		}
	}
	
	{
		int r;
	
		assert(sqlite3_threadsafe() == 1);
		r = sqlite3_open(conf->db, &h_sql);
		if(r != SQLITE_OK)
		{
			h_sql = NULL;
			assert(0);
			return NULL;
		}
	}
	
	return h_sql;
}

dbp_t dbp_create_new_sqlite(dbp_conf_t * conf)
{
	sqlite_param_t * param;
	dbp_t sql;
	sqlite3 * h_sql;

	h_sql = dbp_init_sqlite(conf);
	if (h_sql == NULL)
	{
		assert(0);
		return NULL;
	}
	
	param = (sqlite_param_t *)calloc(1, sizeof(sqlite_param_t));
	sql = (dbp_t)calloc(1, sizeof(dbp_inst_t));
	param->h_sql = h_sql;
	sql->real_handle = param;
	sql->type = (dbp_provider_type_t *)&sqlite_provider;
	pthread_mutex_init(&param->lock, NULL);

	return sql;
}

dbp_query_ret dbp_query_sqlite(dbp_t sql, const char * query_sql)
{
	sqlite_param_t * param;
	int r;

	assert(sql != NULL && query_sql != NULL);
	param = (sqlite_param_t *)sql->real_handle;
	
	pthread_mutex_lock(&param->lock);
	r = sqlite3_prepare(param->h_sql, query_sql, -1, &param->stmt, &param->zTail);
	param->row = sqlite3_step(param->stmt);
	param->fields = sqlite3_data_count(param->stmt);
	
	if (param->row == SQLITE_DONE)
	{
		return dbp_query_done;
	}
	else if (param->row == SQLITE_ROW)
	{
		return dbp_query_row;
	}
	else
	{
        const char * err = sqlite3_errmsg(param->h_sql);
		assert(err && 0);
		return dbp_query_error;
	}
}

int dbp_num_fields_sqlite(dbp_t sql)
{
	sqlite_param_t * param;

	assert(sql != NULL);
	param = (sqlite_param_t *)sql->real_handle;

	if (sql != NULL && param->stmt != NULL)
	{
		return param->fields;
	}

	return 0;
}

dbp_ret dbp_fetch_row_sqlite(dbp_t sql, dbp_row_t ** row)
{
	sqlite_param_t * param;
	int fields;

	param = (sqlite_param_t *)sql->real_handle;
	if (param->row != SQLITE_ROW)
	{
		return dbp_error;
	}

	fields = dbp_num_fields(sql);
	assert(fields != 0);

	if (row != NULL)
	{
		dbp_row_t * myrow;
		int length;
		int idx;

		myrow = (dbp_row_t *)calloc(1, fields * sizeof(dbp_row_t));
		for (idx = 0; idx < fields; idx++)
		{
			const char * data;

			data = (const char *)sqlite3_column_text(param->stmt, idx);
			assert(data != NULL);
			length = sqlite3_column_bytes(param->stmt, idx);
			myrow[idx].data = (char *)strdup(data);
			myrow[idx].length = length;
		}

		*row = myrow;
	}
	param->row = sqlite3_step(param->stmt);

	return dbp_ok;
}

dbp_ret dbp_fetch_row_free_sqlite(dbp_t sql, dbp_row_t * row)
{
	assert(row != NULL);

	if (row != NULL)
	{
		int fields;
		int idx;
		
		fields = dbp_num_fields(sql);
		assert(fields != 0);
		for (idx = 0; idx < fields; idx++)
		{
			free(row[idx].data);
		}
		free(row);
		return dbp_ok;
	}
	
	assert(0);
	return dbp_error;
}

dbp_ret dbp_query_reset_sqlite(dbp_t sql)
{
	sqlite_param_t * param;
	param = (sqlite_param_t *)sql->real_handle;
	sqlite3_reset(param->stmt);
	sqlite3_finalize(param->stmt);
	pthread_mutex_unlock(&param->lock);
	
	return dbp_ok;
}

dbp_ret db_exec_sqlite(dbp_t sql, const char * query_sql)
{
	sqlite_param_t * param;
	char * err;
	int ret;
	
	param = (sqlite_param_t *)sql->real_handle;
	pthread_mutex_lock(&param->lock);
	ret = sqlite3_exec(param->h_sql, query_sql, NULL, NULL, &err);
	pthread_mutex_unlock(&param->lock);
	assert(ret == SQLITE_OK);
	
	return (ret == SQLITE_OK) ? dbp_ok : dbp_error;
}

void dbp_free_sqlite(dbp_t * sql)
{
	assert(sql != NULL && *sql != NULL);
	if (sql != NULL && *sql != NULL)
	{
		sqlite_param_t * param;
		param = (sqlite_param_t *)(*sql)->real_handle;
		sqlite3_close(param->h_sql);
		
		pthread_mutex_destroy(&param->lock);
		free(param);
		free(*sql);
		*sql = NULL;
	}
}

const dbp_provider_type_t sqlite_provider = {
	dbp_sqlite,
	dbp_create_new_sqlite,
	dbp_query_sqlite,
	dbp_num_fields_sqlite,
    dbp_fetch_row_sqlite,
	dbp_fetch_row_free_sqlite,
	dbp_query_reset_sqlite,
	db_exec_sqlite,
	dbp_free_sqlite
};

#endif