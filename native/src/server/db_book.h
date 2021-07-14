//
//  db_book.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-23.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_db_book_h
#define wx_youmenu_db_book_h

#include "identity.h"
#include "db.h"

int db_book_init();
int db_book_deinit();
int db_book_reset();

int db_book_add(book_t * book);
int db_book_del(int book_id);
int db_book_get_one(int book_id, book_t * book);
int db_book_get(int seat_id, book_t ** book);
void db_book_free(book_t * book);

#endif
