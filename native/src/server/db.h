//
//  db.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-10.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_db_h
#define wx_youmenu_db_h

#include "../public/identity.h"
#include "../../dbprovider/include/db_provider.h"

#define DBP_PROVIDER dbp_sqlite
#define DB_DATABASE "youmenu.db"

int db_init(dbp_conf_t * conf);
int db_deinit();

extern dbp_t a_dbp;

#include "db_account.h"
#include "db_dish.h"
#include "db_seat.h"
#include "db_book.h"

#endif
