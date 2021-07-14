//
//  res_cook.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-25.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "../../eventpack/include/eventpack.h"

#include "db.h"
#include "session.h"

#include <assert.h>
#include <string.h>

/*
 * cook accept
 *
 * @return
 *     cook_r_ok
 *     cook_r_seat_id_invalid
 *     cook_r_dish_id_invalid
 *     cook_r_modify_no_more_accept
 *     cook_r_modify_no_exist
 *     cook_r_no_permission
 *     cook_r_illegal_parameter
 *     cook_r_service_unavailable
 *     cook_r_no_response
 */
ep_pack_t * res_cook_accept(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    int seat_id;
    int cook_id;
    int number;
    
    /* check permission */
    {
        session_t * session;
        int r;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", cook_r_ok);
			return pack;
		}
        
        if (session->a_type != account_cook && session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", cook_r_no_permission);
            return pack;
        }
        session_dref(session);
    }
    
    seat_id = -1;
    cook_id = -1;
    number = -1;
    
    if (pack != NULL)
	{
		ep_pack_iterator_t iter;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			ep_pack_type type;
			const char * key;
            
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "seat_id") == 0)
			{
                seat_id = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "cook_id") == 0)
			{
                seat_id = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "number") == 0)
			{
                number = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
		}
	}
    
    pack = ep_pack_new();
    
    if (seat_id < 0 || cook_id < 0 || number < 0)
    {
        ep_pack_append_int(pack, "result", cook_r_illegal_parameter);
        return pack;
    }
    
    /* make sure seat_id/cook_id valid */
    {
        int r;
        
        r = db_dish_get_one(cook_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", cook_r_dish_id_invalid);
            return pack;
        }
        
        r = db_seat_get_one(seat_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", cook_r_seat_id_invalid);
            return pack;
        }
    }
    
//    {
//        book_t book;
//        int r;
//        
//        r = db_book_get_one(seat_id, cook_id, &book);
//        if (r == 0)
//        {
//            ep_pack_append_int(pack, "result", cook_r_modify_no_exist);
//            return pack;
//        }
//        
//        /* check whether left book is less than wish canel numbers */
//        if (book.book_numbers > number && book.book_numbers - book.accept_numbers > number)
//        {
//            ep_pack_append_int(pack, "result", cook_r_modify_no_more_accept);
//            ep_pack_append_int(pack, "book_numbers", book.book_numbers);
//            ep_pack_append_int(pack, "accept_number", book.accept_numbers);
//            return pack;
//        }
//        
//        /* cancel book */
//        if (number == 0)
//        {
//            r = db_book_del_one(seat_id, cook_id);
//            if (r != 0)
//            {
//                ep_pack_append_int(pack, "result", cook_r_service_unavailable);
//                return pack;
//            }
//        }
//        
//        /* modify book */
//        r = db_book_modify_book(seat_id, cook_id, number);
//        if (r == 0)
//        {
//            ep_pack_append_int(pack, "result", cook_r_service_unavailable);
//            return pack;
//        }
//    }
	
    ep_pack_append_int(pack, "result", cook_r_ok);
    
    return pack;
}