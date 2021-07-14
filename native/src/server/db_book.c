//
//  db_book.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-23.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "db.h"
#include "db_book.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define t_db_book "t_db_book"

int db_book_init()
{
	dbp_ret query_r;
	const char * query_sql;

	/* ensure t_db_book exist */
	{
		query_sql =
        "CREATE TABLE IF NOT EXISTS "
        t_db_book
        " (book_id INTEGER PRIMARY KEY,"
        "seat_id INTEGER,"
        "dish_id INTEGER,"
        "jin_or_plate REAL,"
        "comment TEXT,"
        "accept_state int"
        ")";
        
		query_r = dbp_exec(a_dbp, query_sql);
		assert(query_r == dbp_ok);
	}
    
	return query_r;
}

int db_book_deinit()
{
	return 1;
}

int db_book_reset()
{
	dbp_ret query_r;
	const char * query_sql;
    
	/* truncate table t_db_book */
	{
		query_sql = "TRUNCATE TABLE " t_db_book;
        
		query_r = dbp_exec(a_dbp, query_sql);
	}
    
	return query_r;
}

int db_book_add(book_t * book)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(book != NULL);
	query_sql = "INSERT INTO " t_db_book " (seat_id,dish_id,jin_or_plate,comment,accept_state) VALUES (%d,%d,%0.2f,'%s',%d)";
	sprintf(sqlbuf, query_sql, book->seat_id, book->dish_id, book->jin_or_plate, book->comment, book->accept_state);
    
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
    
	return query_r;
}

int db_book_del(int book_id)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(book_id >= 0);
	query_sql = "DELETE FROM " t_db_book " WHERE book_id=%d";
	sprintf(sqlbuf, query_sql, book_id);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_book_get_one(int book_id, book_t * book)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    
	assert(book_id >= 0);
    query_sql = "SELECT book_id,seat_id,dish_id,jin_or_plate,comment,accept_state FROM "
			t_db_book " WHERE book_id=%d";
	sprintf(sqlbuf, query_sql, book_id);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
    if (book != NULL)
	{
		dbp_row_t * row;
		int fields;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 6)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
        while (dbp_fetch_row(a_dbp, &row))
		{
			book->book_id = atoi(row[0].data);
            book->seat_id = atoi(row[1].data);
            book->dish_id = atoi(row[2].data);
            book->jin_or_plate = atof(row[3].data);
			strncpy(book->comment, row[4].data, row[4].length);
			book->comment[row[4].length] = '\0';
            book->accept_state = atof(row[5].data);
            
			dbp_fetch_row_free(a_dbp, row);
		}
	}
    
	dbp_query_reset(a_dbp);
    return 1;
}

int db_book_get(int seat_id, book_t ** book)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
    int count;
    
	assert(seat_id >= 0);
    query_sql = "SELECT count(*) as count FROM " t_db_book " WHERE seat_id = %d";
	sprintf(sqlbuf, query_sql, seat_id);

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
    
    query_sql = "SELECT book_id,seat_id,dish_id,jin_or_plate,comment,accept_state FROM "
				t_db_book " WHERE seat_id = %d";
	sprintf(sqlbuf, query_sql, seat_id);
    
	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
	{
        book_t * _book;
		dbp_row_t * row;
		int fields;
        int i;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 6)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
        _book = (book_t *)calloc(1, sizeof(book_t) * count);
		i = 0;
        
        while (dbp_fetch_row(a_dbp, &row))
		{
			_book[i].book_id = atoi(row[0].data);
            _book[i].seat_id = atoi(row[1].data);
            _book[i].dish_id = atoi(row[2].data);
            _book[i].jin_or_plate = atof(row[3].data);
			strncpy(_book[i].comment, row[4].data, row[4].length);
			_book[i].comment[row[4].length] = '\0';
            _book[i].accept_state = atof(row[5].data);
            
			dbp_fetch_row_free(a_dbp, row);
			
            i++;
            
			dbp_fetch_row_free(a_dbp, row);
		}
        
        *book = _book;
	}

	dbp_query_reset(a_dbp);
    
	return count;
}

void db_book_free(book_t * book)
{
    free(book);
}

int db_book_modify_book(int seat_id, int dish_id, int number)
{
    dbp_ret query_r;
    const char * query_sql;
  	char sqlbuf[4096];
    
	assert(seat_id >= 0 && dish_id >= 0);
   	query_sql = "UPDATE " t_db_book " SET book_numbers=%d WHERE seat_id=%d AND dish_id=%d";
 	sprintf(sqlbuf, query_sql, number, seat_id, dish_id);
 	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
    
    return query_r;
}

int db_book_modify_accept(int seat_id, int dish_id, int number)
{
    dbp_ret query_r;
    const char * query_sql;
  	char sqlbuf[4096];
    
	assert(seat_id >= 0 && dish_id >= 0);
   	query_sql = "UPDATE " t_db_book " SET accept_numbers=%d WHERE seat_id=%d AND dish_id=%d";
 	sprintf(sqlbuf, query_sql, number, seat_id, dish_id);
 	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
    
    return query_r;
}