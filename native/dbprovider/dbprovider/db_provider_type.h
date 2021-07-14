/*
 * db provider types definition
 *
 * Copyright (C) 2012 qimery.com
 * @author rui.sun 2012-12-12
 */
#ifndef DB_PROVIDER_TYPE_H
#define DB_PROVIDER_TYPE_H

#include "db_provider.h"

typedef struct dbp_provider_type
{
	dbp_sql provider;

	dbp_t (* pfn_dbp_create_new)(dbp_conf_t * conf);
	dbp_query_ret (* pfn_dbp_query)(dbp_t sql, const char * query_sql);
	int (* pfn_dbp_num_fields)(dbp_t sql);
	dbp_ret (* pfn_dbp_fetch_row)(dbp_t sql, dbp_row_t ** row);
	dbp_ret (* pfn_dbp_fetch_row_free)(dbp_t sql, dbp_row_t * row);
	dbp_ret (* pfn_dbp_query_reset)(dbp_t sql);
	dbp_ret (* pfn_dbp_exec)(dbp_t sql, const char * query_sql);
	void (* pfn_dbp_free)(dbp_t * sql);
} dbp_provider_type_t;

typedef struct dbp_inst
{
	dbp_provider_type_t * type;
	void * real_handle;
} dbp_inst_t;

#endif