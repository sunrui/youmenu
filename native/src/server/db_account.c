/*
 * database operate - account
 *
 * Copyright (C) 2013 qimery.com
 * @author rui.sun 2013-6-2
 */
#include "db.h"

#include "../utils/sha.h"

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define t_db_account "t_db_account"

/* encrypt our pwd */
void db_encrypt_pwd(const char * pwd, char hashpwd[40 + 1])
{
#ifdef DEBUG
	strcpy(hashpwd, pwd);
#else
	char tmp_sha[20];
	char tmp[8];
	
	SHA(pwd, (int)strlen(pwd), (uint8_t *)tmp_sha);
	SHA(tmp_sha, sizeof(tmp_sha), (uint8_t *)tmp_sha);
	hashpwd[0] = hashpwd[20] = '\0';
	
	for (int n = 19; n >= 0; n--)
	{
		snprintf(tmp, sizeof(tmp), "%02X", (unsigned char)tmp_sha[n]);
		strcat(hashpwd, tmp);
	}
#endif
}

int db_account_init()
{
	dbp_ret query_r;
	const char * query_sql;

	/* ensure t_db_account exist */
	{
		query_sql =
			"CREATE TABLE IF NOT EXISTS "
			t_db_account
			" (uid TEXT PRIMARY KEY,"
			"type INTEGER,"
			"pwd TEXT,"
			"nick TEXT,"
			"comment TEXT"
			")";

		query_r = dbp_exec(a_dbp, query_sql);
		assert(query_r == dbp_ok);
	}

	return query_r;
}

int db_account_deinit()
{
	return 1;
}

int db_account_reset()
{
	dbp_ret query_r;
	const char * query_sql;

	/* truncate table t_db_account */
	{
		query_sql = "TRUNCATE TABLE " t_db_account;

		query_r = dbp_exec(a_dbp, query_sql);
		assert(query_r == dbp_ok);
	}

	return query_r;
}

int db_account_add(account_t * account)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
	char hashpwd[40 + 1];

	assert(account != NULL);
	
	/* word to lower */
	{
		char * p_uid = account->uid;
		
		do
		{
			*p_uid = tolower(*p_uid);
		} while (*p_uid++);
	}
	
	db_encrypt_pwd(account->pwd, hashpwd);
	query_sql = "INSERT INTO " t_db_account " (uid,type,pwd,nick,comment) VALUES ('%s',%d,'%s','%s','%s')";
	sprintf(sqlbuf, query_sql, account->uid, account->type, hashpwd,
		account->nick, account->comment);

	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_account_get_one(const char * uid, account_t * account)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];

	assert(uid != NULL);
	query_sql = "SELECT * FROM " t_db_account " WHERE uid='%s'";
	sprintf(sqlbuf, query_sql, uid);

	query_r = dbp_query(a_dbp, sqlbuf);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
	}

	if (account != NULL)
	{
		dbp_row_t * row;
		int fields;

		fields = dbp_num_fields(a_dbp);
		if (fields != 5)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}

		if (dbp_fetch_row(a_dbp, &row))
		{
			strncpy(account->uid, row[0].data, row[0].length);
            account->uid[row[0].length] = '\0';
			account->type = (account_type)atoi(row[1].data);
			strncpy(account->pwd, row[2].data, row[2].length);
            account->pwd[row[2].length] = '\0';
			strncpy(account->nick, row[3].data, row[3].length);
            account->nick[row[3].length] = '\0';
			strncpy(account->comment, row[4].data, row[4].length);
            account->comment[row[4].length] = '\0';

			dbp_fetch_row_free(a_dbp, row);
		}
	}
	
	dbp_query_reset(a_dbp);

	return 1;
}

int db_account_get(account_t ** account)
{
	dbp_query_ret query_r;
	const char * query_sql;
	int count;
    
    query_sql = "SELECT count(*) as count FROM " t_db_account;
	query_r = dbp_query(a_dbp, query_sql);
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
			count = atoi(row[0].data);
			dbp_fetch_row_free(a_dbp, row);
		}
	}
    
    dbp_query_reset(a_dbp);
    if (count == 0)
    {
        return 0;
    }
    
    query_sql = "SELECT * FROM " t_db_account;
	query_r = dbp_query(a_dbp, query_sql);
	assert(query_r != dbp_query_error);
	if (query_r != dbp_query_row)
	{
		dbp_query_reset(a_dbp);
		return 0;
    }
    
	{
        account_t * _account;
		dbp_row_t * row;
		int fields;
        int i;
        
		fields = dbp_num_fields(a_dbp);
		if (fields != 5)
		{
			assert(0);
			dbp_query_reset(a_dbp);
			return 0;
		}
        
        _account = (account_t *)calloc(1, sizeof(account_t) * (count));
		i = 0;
        
        while (dbp_fetch_row(a_dbp, &row))
		{
			strncpy(_account[i].uid, row[0].data, row[0].length);
            _account[i].uid[row[0].length] = '\0';
			_account[i].type = (account_type)atoi(row[1].data);
			strncpy(_account[i].pwd, row[2].data, row[2].length);
            _account[i].pwd[row[2].length] = '\0';
			strncpy(_account[i].nick, row[3].data, row[3].length);
            _account[i].nick[row[3].length] = '\0';
			strncpy(_account[i].comment, row[4].data, row[4].length);
            _account[i].comment[row[4].length] = '\0';
            
            i++;
            
			dbp_fetch_row_free(a_dbp, row);
		}
        
        *account = _account;
	}
    
	dbp_query_reset(a_dbp);
    
	return count;
}

void db_account_get_free(account_t * account)
{
    free(account);
}

int db_account_pwd_check(const char * uid, const char * pwd)
{
	dbp_query_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
	char hashpwd[40 + 1];

	assert(uid != NULL && pwd != NULL);
	db_encrypt_pwd(pwd, hashpwd);
	query_sql = "SELECT * FROM " t_db_account " WHERE uid='%s' AND pwd='%s'";
	sprintf(sqlbuf, query_sql, uid, hashpwd);
	query_r = dbp_query(a_dbp, sqlbuf);
	dbp_query_reset(a_dbp);

	return query_r == dbp_query_row;
}

int db_account_update_pwd(const char * uid, const char * new_pwd)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
	char hashpwd[40 + 1];

	assert(uid != NULL && new_pwd != NULL);
	db_encrypt_pwd(new_pwd, hashpwd);
	query_sql = "UPDATE " t_db_account " SET pwd='%s' WHERE uid='%s'";
	sprintf(sqlbuf, query_sql, hashpwd, uid);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_account_update_nick(const char * uid, const char * nick)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];
	
	assert(uid != NULL && nick != NULL);
	query_sql = "UPDATE " t_db_account " SET nick='%s' WHERE uid='%s'";
	sprintf(sqlbuf, query_sql, nick, uid);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);
	
	return query_r;
}

int db_account_update_comment(const char * uid, const char * comment)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];

	assert(uid != NULL && comment != NULL);
	query_sql = "UPDATE " t_db_account " SET comment='%s' WHERE uid='%s'";
	sprintf(sqlbuf, query_sql, comment, uid);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}

int db_account_del(const char * uid)
{
	dbp_ret query_r;
	const char * query_sql;
	char sqlbuf[4096];

	assert(uid != NULL);
	query_sql = "DELETE FROM " t_db_account " WHERE uid='%s'";
	sprintf(sqlbuf, query_sql, uid);
	query_r = dbp_exec(a_dbp, sqlbuf);
    assert(query_r == dbp_ok);

	return query_r;
}