//
//  db.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-23.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "db.h"

#include <assert.h>

dbp_t a_dbp;

int db_init(dbp_conf_t * conf)
{
	assert(a_dbp == NULL);
	a_dbp = dbp_new(DBP_PROVIDER, conf);
	if (a_dbp == NULL)
	{
		assert(0);
		return 0;
	}

    return 1;
}

int db_deinit()
{
    assert(a_dbp != NULL);
	dbp_free(&a_dbp);
	a_dbp = NULL;

	return 1;
}