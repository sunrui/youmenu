//
//  db_seat.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-20.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "db.h"
#include "db_seat.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define t_db_seat "t_db_seat"

int db_seat_init()
{
	dbp_ret query_r;
	const char * query_sql;

	/* ensure t_db_seat exist */
	{
		query_sql =
        "CREATE TABLE IF NOT EXISTS "
        t_db_seat
        " (id INTEGER PRIMARY KEY,"
        "summary TEXT,"
        "qrcode TEXT"
        ")";
        
		query_r = dbp_exec(a_dbp, query_sql);
		assert(query_r == dbp_ok);
	}
    
	return query_r;
}

int db_seat_deinit()
{
	return 1;
}

int db_seat_reset()
{
	dbp_ret query_r;
	const char * query_sql;
    
	/* truncate table t_db_seat */
	{
		query_sql = "TRUNCATE TABLE " t_db_seat;
        
		query_r = dbp_exec(a_dbp, query_sql);
	}
    
	return query_r;
}

int db_seat_add(seat_t * seat)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(seat != NULL);

	query_sql = "INSERT INTO " t_db_seat " (id,summary,qrcode) VALUES (%d,'%s','%s')";
	sprintf(sqlbuf, query_sql, seat->id, seat->summary, seat->qrcode);
    
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
    
	return query_r;
}

int db_seat_del(int id)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(id != -1);
	query_sql = "DELETE FROM " t_db_seat " WHERE id='%d'";
	sprintf(sqlbuf, query_sql, id);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_seat_get(seat_t ** seat)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    int count;
    
    query_sql = "SELECT count(*) as count FROM " t_db_seat;
	sprintf(sqlbuf, query_sql, NULL);

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
    
    if (count == 0)
    {
        dbp_query_reset(a_dbp);
        return 0;
    }
    
    dbp_query_reset(a_dbp);
    query_sql = "SELECT * FROM " t_db_seat;
	sprintf(sqlbuf, query_sql, NULL);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
	{
        seat_t * _seat;
		dbp_row_t * row;
		int fields;
        int i;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 3)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
        _seat = malloc(sizeof(seat_t) * count);
		i = 0;
        
        while (dbp_fetch_row(a_dbp, &row))
		{
            _seat[i].id = atoi(row[0].data);
            strncpy(_seat[i].summary, row[1].data, row[1].length);
            _seat[i].summary[row[1].length] = '\0';
            strncpy(_seat[i].qrcode, row[2].data, row[2].length);
            _seat[i].qrcode[row[2].length] = '\0';
            i++;
            
			dbp_fetch_row_free(a_dbp, row);
		}
        
        *seat = _seat;
	}

	dbp_query_reset(a_dbp);
    
	return count;
}

void db_seat_free(seat_t * seat)
{
    free(seat);
}

int db_seat_get_one(int id, seat_t * seat)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
    query_sql = "SELECT * FROM " t_db_seat " WHERE id=%d";
	sprintf(sqlbuf, query_sql, id);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
    if (seat != NULL)
	{
		dbp_row_t * row;
		int fields;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 3)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
		if (dbp_fetch_row(a_dbp, &row))
		{
            seat->id = atoi(row[0].data);
            strncpy(seat->summary, row[1].data, row[1].length);
            seat->summary[row[3].length] = '\0';
            strncpy(seat->qrcode, row[2].data, row[2].length);
            seat->qrcode[row[2].length] = '\0';
            
			dbp_fetch_row_free(a_dbp, row);
		}
	}

    dbp_query_reset(a_dbp);
    
	return 1;
}
