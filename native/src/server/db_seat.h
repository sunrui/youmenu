//
//  db_seat.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-20.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_db_seat_h
#define wx_youmenu_db_seat_h

#include "identity.h"
#include "db.h"

int db_seat_init();
int db_seat_deinit();
int db_seat_reset();

int db_seat_add(seat_t * seat);
int db_seat_del(int id);
int db_seat_get_one(int id, seat_t * seat);
int db_seat_get(seat_t ** seat);
void db_seat_free(seat_t * seat);

#endif
