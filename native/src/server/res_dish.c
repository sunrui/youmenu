//
//  res_dish.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-21.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "../../eventpack/include/eventpack.h"

#include "db.h"
#include "session.h"

#include <assert.h>
#include <string.h>

/*
 * fetch dish
 *
 * @return
 *     dish_r_ok
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
ep_pack_t * res_dish_fetch(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    dish_t * dish;
    int count;
    dish_category category;
    
    /* check permission */
    {
        session_t * session;
        int r;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
			return pack;
		}
        
        if (session->a_type != account_menu && session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
            return pack;
        }
        session_dref(session);
    }
    
    category = dish_category_hot;
    
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
			
			if (strcmp(key, "category") == 0)
			{
                category = (dish_category)ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
		}
	}
    
    pack = ep_pack_new();
	
    count = db_dish_get_by_category(category, &dish);
    if (count == 0)
    {
        ep_pack_append_int(pack, "result", dish_r_ok);
        ep_pack_append_int(pack, "count", 0);
        return pack;
    }
    
    ep_pack_append_int(pack, "result", dish_r_ok);
    ep_pack_append_int(pack, "count", count);
    ep_pack_append_start_array(pack, "dish");
    for (int i = 0; i < count; i++)
    {
        ep_pack_append_start_object(pack, "dish_one");
        ep_pack_append_int(pack, "id", dish[i].id);
        ep_pack_append_int(pack, "category", dish[i].category);
        ep_pack_append_string(pack, "name", dish[i].name);
        ep_pack_append_string(pack, "summary", dish[i].summary);
        ep_pack_append_int(pack, "price_type", dish[i].price_type);
        ep_pack_append_int(pack, "price", dish[i].price);
        ep_pack_append_finish_object(pack);
    }
    ep_pack_append_finish_array(pack);
	db_dish_free(dish);
    
    return pack;
}

/*
 * dish book
 *
 * @return
 *     dish_r_ok
 *     dish_r_ignore_conflict
 *     dish_r_seat_id_invalid
 *     dish_r_dish_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
ep_pack_t * res_dish_book(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    int seat_id;
    int dish_id;
    float jin_or_plate;
	char comment[256];
    
    /* check permission */
    {
        session_t * session;
        int r;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
			return pack;
		}
        
        if (session->a_type != account_menu && session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
            return pack;
        }
        session_dref(session);
    }
    
    seat_id = -1;
    dish_id = -1;
    jin_or_plate = -1.0;
	comment[0] = '\0';
    
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
			else if (strcmp(key, "dish_id") == 0)
			{
                dish_id = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "jin_or_plate") == 0)
			{
                jin_or_plate = ep_pack_iterator_double(iter);
				assert(type == PACKET_DOUBLE);
			}
			else if (strcmp(key, "comment") == 0)
			{
				strncpy(comment, ep_pack_iterator_string(iter), sizeof(comment));
			}
		}
	}
    
    pack = ep_pack_new();
    
    if (seat_id < 0 || dish_id < 0 || jin_or_plate <= 0)
    {
        ep_pack_append_int(pack, "result", dish_r_illegal_parameter);
        return pack;
    }
	
    /* make sure seat_id/dish_id valid */
    {
        int r;
        
        r = db_dish_get_one(dish_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_dish_id_invalid);
            return pack;
        }
        
        r = db_seat_get_one(seat_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_seat_id_invalid);
            return pack;
        }
    }

    {
        book_t book;
        int r;
        
        book.seat_id = seat_id;
        book.dish_id = dish_id;
		book.jin_or_plate = jin_or_plate;
        strncpy(book.comment, comment, strlen(comment));
		book.accept_state = book_accept_state_idle;
		
        r = db_book_add(&book);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_service_unavailable);
            return pack;
        }
    }
    
    ep_pack_append_int(pack, "result", dish_r_ok);
	
    return pack;
}

/*
 * dish cancel
 *
 * @return
 *     dish_r_ok
 *     dish_r_seat_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
ep_pack_t * res_dish_cancel(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    int seat_id;
    
    /* check permission */
    {
        session_t * session;
        int r;
        
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
			return pack;
		}
        
        if (session->a_type != account_menu && session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
            return pack;
        }
        session_dref(session);
    }
    
    seat_id = -1;
    
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
		}
	}
    
    pack = ep_pack_new();
    
    if (seat_id < 0)
    {
        ep_pack_append_int(pack, "result", dish_r_illegal_parameter);
        return pack;
    }

    /* make sure seat_id valid */
    {
        int r;
		
        r = db_seat_get_one(seat_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_seat_id_invalid);
            return pack;
        }
    }
    
    /* cancel this seat_id */
    {
        int r;
        
        r = db_book_del(seat_id);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_service_unavailable);
            return pack;
        }
    }
    
    ep_pack_append_int(pack, "result", dish_r_ok);
    
    return pack;
}

/*
 * dish list
 *
 * @return
 *     dish_r_ok
 *     dish_r_dish_id_invalid
 *     dish_r_no_permission
 *     dish_r_illegal_parameter
 *     dish_r_service_unavailable
 *     dish_r_no_response
 */
ep_pack_t * res_dish_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
    int seat_id;
    
    /* check permission */
    {
        session_t * session;
        int r;
		
		r = session_ref_fd(fd, &session);
		if (r == 0)
		{
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
			return pack;
		}
        
        if (session->a_type != account_menu && session->a_type != account_admin)
        {
            session_dref(session);
            pack = ep_pack_new();
            ep_pack_append_int(pack, "result", dish_r_no_permission);
            return pack;
        }
        session_dref(session);
    }
    
    seat_id = -1;
    
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
		}
	}
    
    pack = ep_pack_new();
    
    if (seat_id < 0)
    {
        ep_pack_append_int(pack, "result", dish_r_illegal_parameter);
        return pack;
    }
    
    /* make sure seat_id valid */
    {
        int r;
		
        r = db_seat_get_one(seat_id, NULL);
        if (r == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_seat_id_invalid);
            return pack;
        }
    }
    
    {
        book_t * _book;
        int count;
        
        count = db_book_get(seat_id, &_book);
        if (count == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_ok);
            ep_pack_append_int(pack, "count", 0);
            return pack;
        }
        
        ep_pack_append_int(pack, "result", dish_r_ok);
        ep_pack_append_int(pack, "count", count);
        ep_pack_append_start_array(pack, "book");
        for (int i = 0; i < count; i++)
        {
            ep_pack_append_start_object(pack, "book_one");
            ep_pack_append_int(pack, "book_id", _book[i].book_id);
            ep_pack_append_int(pack, "seat_id", _book[i].seat_id);
            ep_pack_append_int(pack, "dish_id", _book[i].dish_id);
            ep_pack_append_double(pack, "jin_or_plate", _book[i].jin_or_plate);
            ep_pack_append_string(pack, "comment", _book[i].comment);
            ep_pack_append_int(pack, "accept_state", _book[i].accept_state);
            ep_pack_append_finish_object(pack);
        }
        ep_pack_append_finish_array(pack);
    }
    
    return pack;
}
