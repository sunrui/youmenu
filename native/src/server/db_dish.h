//
//  db_dish.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-20.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_db_dish_h
#define wx_youmenu_db_dish_h

#include "identity.h"
#include "db.h"

int db_dish_init();
int db_dish_deinit();
int db_dish_reset();

int db_dish_add(dish_t * dish);
int db_dish_del(int id);
int db_dish_get_one(int id, dish_t * dish);
int db_dish_get_by_category(dish_category category, dish_t ** dish);
void db_dish_free(dish_t * dish);

#endif
