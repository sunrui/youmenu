/*
 * db provider
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#include "db_provider.h"
#include "db_provider_type.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#ifdef DB_PROVIDER_HAVE_MYSQL
extern const dbp_provider_type_t mysql_provider;
#endif

#ifdef DB_PROVIDER_HAVE_SQLITE
extern const dbp_provider_type_t sqlite_provider;
#endif

#ifdef DB_PROVIDER_HAVE_ORACLE
extern const dbp_provider_type_t oracle_provider;
#endif

static const dbp_provider_type_t * const providers[] =
{
#ifdef DB_PROVIDER_HAVE_SQLITE
	&sqlite_provider,
#endif
#ifdef DB_PROVIDER_HAVE_MYSQL
	&mysql_provider,
#endif
#ifdef DB_PROVIDER_HAVE_ORACLE
	&oracle_provider,
#endif
};

dbp_provider_type_t * dbp_get_type_by_provider(dbp_sql provider)
{
	int count_provider;
	int idx;

	count_provider = sizeof(providers) / sizeof(providers[0]);
	for (idx = 0; idx < count_provider; idx++)
	{
		if (providers[idx]->provider == provider)
		{
			return (dbp_provider_type_t *)providers[idx];
		}
	}

	assert(0 && "not implement specified provider");
	return NULL;
}

dbp_t dbp_new(dbp_sql provider, dbp_conf_t * conf)
{
	dbp_provider_type_t * type;
		
	type = dbp_get_type_by_provider(provider);
	return type->pfn_dbp_create_new(conf);
}

dbp_query_ret dbp_query(dbp_t sql, const char * query_sql)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_query(sql, query_sql);
}

int dbp_num_fields(dbp_t sql)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_num_fields(sql);
}

dbp_ret dbp_fetch_row(dbp_t sql, dbp_row_t ** row)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_fetch_row(sql, row);
}

dbp_ret dbp_fetch_row_free(dbp_t sql, dbp_row_t * row)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_fetch_row_free(sql, row);
}

dbp_ret dbp_query_reset(dbp_t sql)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_query_reset(sql);
}

dbp_ret dbp_exec(dbp_t sql, const char * query_sql)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = sql->type;
	
	return type->pfn_dbp_exec(sql, query_sql);
}

void dbp_free(dbp_t * sql)
{
	dbp_provider_type_t * type;
	assert(sql != NULL);
	type = (*sql)->type;
	
	return type->pfn_dbp_free(sql);
}

//#define TEST_DB_PROVIDER

#ifdef TEST_DB_PROVIDER
void test_dbp_provider()
{
	dbp_config_t conf;
	e_db_provider provider;
	db_ret r;
	int n;
	
	strcpy(conf.ip, "127.0.0.1");
	conf.port = 3306;
	strcpy(conf.user, "root");
	strcpy(conf.pwd, "root");
	strcpy(conf.db, "dbprovider.db");
	
	provider = provider_sqlite;
	dbp_init(provider, &conf);

	r = dbp_test_connect(provider, NULL);
	
	{
		e_dbp_query_result query_r;
		dbp_t sql;
		
		sql = dbp_new(provider, NULL);
		
		query_r = dbp_query(sql, "CREATE TABLE IF NOT EXISTS test(uuid TEXT, age INTEGER)");
		
		query_r = dbp_query(sql, "INSERT INTO test (uuid,age) VALUES ('zhang3',18)");
		query_r = dbp_query(sql, "INSERT INTO test (uuid,age) VALUES ('li4',19)");
		
		query_r = dbp_query(sql, "SELECT * FROM test");
		
		n = dbp_num_fields(sql);
		
		{
			dbp_row_t * row;
			
			while (dbp_fetch_row(sql, &row))
			{
				int i;
				
				for (i = 0; i < n; i++)
				{
					fprintf(stderr, "key = %s(%d).\n", row[i].data, row[i].length);
				}
				
				dbp_fetch_row_free(sql, row);
			}
		}
		
		dbp_free(&sql);
		
		dbp_clean(provider);
	}
}
#endif