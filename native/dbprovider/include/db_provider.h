/*
 * db provider
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#ifndef DB_PROVIDER_H
#define DB_PROVIDER_H

#ifdef  __cplusplus
# define __DB_PROVIDER_BEGIN_DECLS  extern "C" {
# define __DB_PROVIDER_END_DECLS    }
#else
# define __DB_PROVIDER_BEGIN_DECLS
# define __DB_PROVIDER_END_DECLS
#endif

#define DB_PROVIDER_HAVE_SQLITE
//#define DB_PROVIDER_HAVE_MYSQL
//#define DB_PROVIDER_HAVE_ORACLE

__DB_PROVIDER_BEGIN_DECLS

typedef enum dbp_sql
{
#ifdef DB_PROVIDER_HAVE_MYSQL
	dbp_mysql,
#endif
#ifdef DB_PROVIDER_HAVE_ORACLE
	dbp_oracle,
#endif
#ifdef DB_PROVIDER_HAVE_SQLITE
	dbp_sqlite,
#endif
} dbp_sql;

typedef struct dbp_inst * dbp_t;

typedef enum dbp_ret
{
	dbp_ok = 1,
	dbp_error = 0
} dbp_ret;

typedef struct dbp_row
{
	char * data;
	int length;
} dbp_row_t;

typedef enum dbp_query_ret
{
	dbp_query_done,
	dbp_query_row,
	dbp_query_error
} dbp_query_ret;

typedef struct dbp_conf
{
	char ip[17];
	int  port;
	char user[17];
	char pwd[17];
	char db[261];
} dbp_conf_t;

dbp_t dbp_new(dbp_sql provider, dbp_conf_t * conf);

dbp_query_ret dbp_query(dbp_t sql, const char * query_sql);
int dbp_num_fields(dbp_t sql);
/* use this like [while (dbp_fetch_row) ;] */
dbp_ret dbp_fetch_row(dbp_t sql, dbp_row_t ** row);
dbp_ret dbp_fetch_row_free(dbp_t sql, dbp_row_t * row);
dbp_ret dbp_query_reset(dbp_t sql);

dbp_ret dbp_exec(dbp_t sql, const char * query_sql);

void dbp_free(dbp_t * sql);

__DB_PROVIDER_END_DECLS

#endif