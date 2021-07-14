//
//  req_dish.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-21.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "client.h"
#include "connect.h"
#include "identity.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

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
void client_dish_fetch(connect_t * connect, dish_category category, dish_t ** dish, int * count, dish_result * result)
{
    ep_req_t * handle;
    ep_pack_t * pack;
    int ret;
    
    if (connect == NULL || connect->session == NULL || dish == NULL)
    {
        *result = dish_r_illegal_parameter;
        return;
    }
    
    if (connect->session->account.type != account_admin && connect->session->account.type != account_menu)
    {
        *result = dish_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_int(pack, "category", category);
    
    {
		REQ_MAKE_PARAM(param, ep_id_dish_fetch, pack);
        handle = ep_req_new(connect->sock, &param, NULL);
        ep_pack_destroy(&pack);
        if (handle == NULL)
        {
            *result = dish_r_no_response;
            return;
        }
    }
    
    ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
    if (ret == 0)
    {
        ep_req_destroy(&handle);
        *result = dish_r_no_response;
        return;
    }

    {
        ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        dish_t * _dish;
        int i;
        
        iter = ep_pack_iterator_get(pack);
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "result") != 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        *result = (dish_result)ep_pack_iterator_int(iter);
        if (*result != dish_r_ok)
        {
            ep_req_destroy(&handle);
            return;
        }
        
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "count") != 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        *count = ep_pack_iterator_int(iter);
        if (*count == 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_ok;
            return;
        }
        
        _dish = (dish_t *)ep_calloc(1, sizeof(dish_t) * (*count));
        i = 0;
        
        while (ep_pack_iterator_next(&iter))
        {
            type = ep_pack_iterator_type(iter);
            key = ep_pack_iterator_key(iter);
            
            if (type == PACKET_ARRAY)
            {
                ep_pack_iterator_t object_iter;

                object_iter = ep_pack_interator_array(iter);
                while (ep_pack_iterator_next(&object_iter))
                {
                    type = ep_pack_iterator_type(object_iter);
                    key = ep_pack_iterator_key(object_iter);
                    
                    if (type == PACKET_OBJECT)
                    {
                        ep_pack_iterator_t one_iter;
                        
                        one_iter = ep_pack_interator_array(object_iter);
                        
                        while (ep_pack_iterator_next(&one_iter))
                        {
                            type = ep_pack_iterator_type(one_iter);
                            key = ep_pack_iterator_key(one_iter);
                            
                            if (strcmp(key, "id") == 0)
                            {
                                _dish[i].id = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "category") == 0)
                            {
                                _dish[i].category = (dish_category)ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "name") == 0)
                            {
                                strcpy(_dish[i].name, ep_pack_iterator_string(one_iter));
                                assert(ep_pack_iterator_type(one_iter) == PACKET_STRING);
                            }
                            else if (strcmp(key, "summary") == 0)
                            {
                                strcpy(_dish[i].summary, ep_pack_iterator_string(one_iter));
                                assert(ep_pack_iterator_type(one_iter) == PACKET_STRING);
                            }
                            else if (strcmp(key, "thumbnail") == 0)
                            {
                                strcpy(_dish[i].thumbnail, ep_pack_iterator_string(one_iter));
                                assert(ep_pack_iterator_type(one_iter) == PACKET_STRING);
                            }
                            else if (strcmp(key, "price_type") == 0)
                            {
                                _dish[i].price_type = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "price") == 0)
                            {
                                _dish[i].price = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                        }
                        
                        i++;
                    }
                    else
                    {
                        ep_free(_dish);
                        *dish = NULL;
                        
                        *result = dish_r_service_unavailable;
                        ep_req_destroy(&handle);
                        assert(0);
                        return;
                    }
                }
                
                *dish = _dish;
            }
            else
            {
                ep_free(_dish);
                *dish = NULL;
                
                *result = dish_r_service_unavailable;
                ep_req_destroy(&handle);
                assert(0);
                return;
            }
        }
    }
    
    ep_req_destroy(&handle);
    *result = dish_r_ok;
}

void client_dish_fetch_free(dish_t ** dish, int count)
{
    for (int i = 0; i < count; i++)
    {
        ep_free(dish[i]);
    }
    
    ep_free(dish);
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
void client_dish_book(connect_t * connect, int seat_id, int dish_id, double jin_or_plate, const char * comment, dish_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;

	if (connect == NULL || connect->session == NULL || seat_id < 0 || dish_id < 0 || jin_or_plate <= 0)
	{
		*result = dish_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin && connect->session->account.type != account_menu)
    {
        *result = dish_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_int(pack, "seat_id", seat_id);
    ep_pack_append_int(pack, "dish_id", dish_id);
    ep_pack_append_double(pack, "jin_or_plate", jin_or_plate);
	
	if (comment != NULL)
	{
		ep_pack_append_string(pack, "comment", comment);
	}

	{
		REQ_MAKE_PARAM(param, ep_id_dish_book, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = dish_r_no_response;
			return;
		}
	}

	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = dish_r_no_response;
        return;
	}
    
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
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
void client_dish_cancel(connect_t * connect, int seat_id, dish_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	if (connect == NULL || connect->session == NULL || seat_id < 0)
	{
		*result = dish_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin && connect->session->account.type != account_menu)
    {
        *result = dish_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_int(pack, "seat_id", seat_id);
    
	{
		REQ_MAKE_PARAM(param, ep_id_dish_cancel, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = dish_r_no_response;
			return;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = dish_r_no_response;
        return;
	}
    
	{
		ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
		
		iter = ep_pack_iterator_get(pack);
		while (ep_pack_iterator_next(&iter))
		{
			type = ep_pack_iterator_type(iter);
			key = ep_pack_iterator_key(iter);
			
			if (strcmp(key, "result") == 0)
			{
				*result = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
				break;
			}
		}
		
		ep_req_destroy(&handle);
	}
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
void client_dish_list(connect_t * connect, int seat_id, book_t ** book, int * count, dish_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	*book = NULL;
	*count = 0;
	
	if (connect == NULL || connect->session == NULL || seat_id < 0)
	{
		*result = dish_r_illegal_parameter;
		return;
	}
	
    if (connect->session->account.type != account_admin && connect->session->account.type != account_menu)
    {
        *result = dish_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_int(pack, "seat_id", seat_id);
    
	{
		REQ_MAKE_PARAM(param, ep_id_dish_list, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = dish_r_no_response;
			return;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = dish_r_no_response;
        return;
	}
    
    {
        ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        book_t * _book;
        int i;
        
        iter = ep_pack_iterator_get(pack);
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "result") != 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        *result = (dish_result)ep_pack_iterator_int(iter);
        if (*result != dish_r_ok)
        {
            ep_req_destroy(&handle);
            return;
        }
        
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "count") != 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_service_unavailable;
            return;
        }
        
        *count = ep_pack_iterator_int(iter);
        if (*count == 0)
        {
            ep_req_destroy(&handle);
            *result = dish_r_ok;
            return;
        }
        
        _book = (book_t *)ep_calloc(1, sizeof(book_t) * (*count));
        i = 0;
        
        while (ep_pack_iterator_next(&iter))
        {
            type = ep_pack_iterator_type(iter);
            key = ep_pack_iterator_key(iter);
            
            if (type == PACKET_ARRAY)
            {
                ep_pack_iterator_t object_iter;
                
                object_iter = ep_pack_interator_array(iter);
                while (ep_pack_iterator_next(&object_iter))
                {
                    type = ep_pack_iterator_type(object_iter);
                    key = ep_pack_iterator_key(object_iter);
                    
                    if (type == PACKET_OBJECT)
                    {
                        ep_pack_iterator_t one_iter;
                        
                        one_iter = ep_pack_interator_array(object_iter);
                        
                        while (ep_pack_iterator_next(&one_iter))
                        {
                            type = ep_pack_iterator_type(one_iter);
                            key = ep_pack_iterator_key(one_iter);
                            
                            if (strcmp(key, "seat_id") == 0)
                            {
                                _book[i].seat_id = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "dish_id") == 0)
                            {
                                _book[i].dish_id = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
							else if (strcmp(key, "jin_or_plate") == 0)
                            {
                                _book[i].jin_or_plate = ep_pack_iterator_double(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_DOUBLE);
                            }
                            else if (strcmp(key, "comment") == 0)
                            {
                                strncpy(_book[i].comment, ep_pack_iterator_string(one_iter), sizeof(_book[i].comment));
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
							else if (strcmp(key, "accept_state") == 0)
							{
								_book[i].dish_id = ep_pack_iterator_int(one_iter);
								assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
							}
						}
						
                        i++;
                    }
                    else
                    {
                        ep_free(_book);
                        *book = NULL;
                        
                        *result = dish_r_service_unavailable;
                        ep_req_destroy(&handle);
                        assert(0);
                        return;
                    }
                }
                
                *book = _book;
            }
            else
            {
                ep_free(_book);
                *book = NULL;
                
                *result = dish_r_service_unavailable;
                ep_req_destroy(&handle);
                assert(0);
                return;
            }
        }
    }
    
    ep_req_destroy(&handle);
    *result = dish_r_ok;
}

void client_dish_list_free(book_t * book)
{
	if (book != NULL)
	{
		ep_free(book);
	}
}