/*
 * database operate - account
 *
 * Copyright (C) 2013 qimery.com
 * @author rui.sun 2013-6-2
 */
#ifndef DB_ACCOUNT_H
#define DB_ACCOUNT_H

#include "identity.h"
#include "public.h"
#include "db.h"

/*
 * @return
 *     1 success
 *     0 failed
 */
int db_account_init();
int db_account_deinit();
int db_account_reset();

int db_account_add(account_t * account);
int db_account_get_one(const char * uid, account_t * account);
int db_account_get(account_t ** account);
void db_account_get_free(account_t * account);
int db_account_pwd_check(const char * uid, const char * pwd);
int db_account_update_pwd(const char * uid, const char * new_pwd);
int db_account_update_nick(const char * uid, const char * nick);
int db_account_update_comment(const char * uid, const char * comment);
int db_account_del(const char * uid);

#endif