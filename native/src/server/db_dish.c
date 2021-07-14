//
//  db_dish.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-20.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "db.h"
#include "db_dish.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define t_db_dish "t_db_dish"

int db_dish_init()
{
	dbp_ret query_r;
	const char * query_sql;
	
	/* ensure t_db_dish exist */
	{
		query_sql =
        "CREATE TABLE IF NOT EXISTS "
        t_db_dish
        " (id INTEGER PRIMARY KEY,"
        "category INTEGER,"
        "name TEXT,"
        "summary TEXT,"
        "thumbnail TEXT,"
		"price_type INTEGER,"
        "price INTEGER,"
        "created TIMESTAMPE NOT NULL DEFAULT (datetime('now','localtime'))"
        ")";
        
		query_r = dbp_exec(a_dbp, query_sql);
		assert(query_r == dbp_ok);
	}
    
	return query_r;
}

int db_dish_deinit()
{
	return 1;
}

int db_dish_reset()
{
	dbp_ret query_r;
	const char * query_sql;
    
	/* truncate table t_db_dish */
	{
		query_sql = "TRUNCATE TABLE " t_db_dish;
        
		query_r = dbp_exec(a_dbp, query_sql);
	}
    
	return query_r;
}

int db_dish_add(dish_t * dish)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(dish != NULL);

	query_sql = "INSERT INTO " t_db_dish " (id, category,name,summary,thumbnail,price_type, price) VALUES (%d,%d,'%s','%s','%s',%d,%d)";
	sprintf(sqlbuf, query_sql, dish->id, dish->category, dish->name, dish->summary,
            dish->thumbnail, dish->price_type, dish->price);
    
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
    
	return query_r;
}

int db_dish_del(int id)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(id != -1);
	query_sql = "DELETE FROM " t_db_dish " WHERE id='%d'";
	sprintf(sqlbuf, query_sql, id);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_dish_get_by_category(dish_category category, dish_t ** dish)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    int count;
    
    query_sql = "SELECT count(*) as count FROM " t_db_dish " WHERE category=%d";
	sprintf(sqlbuf, query_sql, category);

	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }

	{
		dbp_row_t * row;
		int fields;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 1)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
		if (dbp_fetch_row(a_dbp, &row))
		{
			count = (account_type)atoi(row[0].data);
			dbp_fetch_row_free(a_dbp, row);
		}
	}
    
    dbp_query_reset(a_dbp);
    if (count == 0)
    {
        return 0;
    }
    
    query_sql = "SELECT * FROM " t_db_dish " WHERE category=%d";
	sprintf(sqlbuf, query_sql, category);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
    if (dish != NULL)
	{
        dish_t * _dish;
		dbp_row_t * row;
		int fields;
        int i;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 8)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
        _dish = calloc(1, sizeof(dish_t) * count);
		i = 0;
        
        while (dbp_fetch_row(a_dbp, &row))
		{
            _dish[i].id = atoi(row[0].data);
            _dish[i].category = atoi(row[1].data);
            strncpy(_dish[i].name, row[2].data, row[2].length);
            _dish[i].name[row[2].length] = '\0';
            strncpy(_dish[i].summary, row[3].data, row[3].length);
            _dish[i].summary[row[3].length] = '\0';
            strncpy(_dish[i].thumbnail, row[4].data, row[4].length);
            _dish[i].thumbnail[row[4].length] = '\0';
            _dish[i].price_type = atoi(row[5].data);
            _dish[i].price = atoi(row[6].data);
            i++;
            
			dbp_fetch_row_free(a_dbp, row);
		}
        
        *dish = _dish;
	}

	dbp_query_reset(a_dbp);
    
	return count;
}

void db_dish_free(dish_t * dish)
{
    free(dish);
}

int db_dish_get_one(int id, dish_t * dish)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
    query_sql = "SELECT * FROM " t_db_dish " WHERE id=%d";
	sprintf(sqlbuf, query_sql, id);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
    if (dish != NULL)
	{
		dbp_row_t * row;
		int fields;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 1)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
		if (dbp_fetch_row(a_dbp, &row))
		{
            dish->id = atoi(row[0].data);
            dish->category = atoi(row[1].data);
            strncpy(dish->name, row[2].data, row[2].length);
            dish->name[row[2].length] = '\0';
            strncpy(dish->summary, row[3].data, row[3].length);
            dish->summary[row[3].length] = '\0';
            strncpy(dish->thumbnail, row[4].data, row[4].length);
            dish->thumbnail[row[4].length] = '\0';
            dish->price_type = atoi(row[5].data);
            dish->price = atoi(row[6].data);
            
			dbp_fetch_row_free(a_dbp, row);
		}
	}

    dbp_query_reset(a_dbp);
    
	return 1;
}
