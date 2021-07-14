//
//  req_seat.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-25.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "client.h"
#include "connect.h"
#include "identity.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

/*
 * seat list
 *
 * @return
 *     seat_r_ok
 *     seat_r_no_permission
 *     seat_r_illegal_parameter
 *     seat_r_service_unavailable
 *     seat_r_no_response
 */
void client_seat_list(connect_t * connect, seat_t ** seat, int * count, seat_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
	
	*seat = NULL;
	*count = 0;
	
	if (connect == NULL || connect->session == NULL)
	{
		*result = seat_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin && connect->session->account.type != account_menu)
    {
        *result = seat_r_no_permission;
        return;
    }
    
	{
		pack = NULL;
		REQ_MAKE_PARAM(param, ep_id_seat_list, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = seat_r_no_response;
			return;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = seat_r_no_response;
        return;
	}
    
    {
        ep_pack_iterator_t iter;
        ep_pack_type type;
        const char * key;
        seat_t * _seat;
        int i;
        
        iter = ep_pack_iterator_get(pack);
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = seat_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "result") != 0)
        {
            ep_req_destroy(&handle);
            *result = seat_r_service_unavailable;
            return;
        }
        
        *result = (seat_result)ep_pack_iterator_int(iter);
        if (*result != dish_r_ok)
        {
            ep_req_destroy(&handle);
            return;
        }
        
        if (!ep_pack_iterator_next(&iter))
        {
            ep_req_destroy(&handle);
            *result = seat_r_service_unavailable;
            return;
        }
        
        type = ep_pack_iterator_type(iter);
        key = ep_pack_iterator_key(iter);
        
        if (type != PACKET_INT || strcmp(key, "count") != 0)
        {
            ep_req_destroy(&handle);
            *result = seat_r_service_unavailable;
            return;
        }
        
        *count = ep_pack_iterator_int(iter);
        if (*count == 0)
        {
            ep_req_destroy(&handle);
            *result = seat_r_ok;
            return;
        }
        
        _seat = (seat_t *)ep_calloc(1, sizeof(seat_t) * (*count));
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
                                _seat[i].id = ep_pack_iterator_int(one_iter);
                                assert(ep_pack_iterator_type(one_iter) == PACKET_INT);
                            }
                            else if (strcmp(key, "summary") == 0)
                            {
                                strncpy(_seat[i].summary, ep_pack_iterator_string(one_iter), sizeof(_seat[i].summary));
                                assert(type == PACKET_STRING);
                            }
                            else if (strcmp(key, "qrcode") == 0)
                            {
                                strncpy(_seat[i].qrcode, ep_pack_iterator_string(one_iter), sizeof(_seat[i].qrcode));
                                assert(type == PACKET_STRING);
                            }
                        }
                        
                        i++;
                    }
                    else
                    {
                        ep_free(_seat);
                        *seat = NULL;
                        
                        *result = seat_r_service_unavailable;
                        ep_req_destroy(&handle);
                        assert(0);
                        return;
                    }
                }
                
                *seat = _seat;
            }
            else
            {
                ep_free(_seat);
                *seat = NULL;
                
                *result = seat_r_service_unavailable;
                ep_req_destroy(&handle);
                assert(0);
                return;
            }
        }
    }
    
    ep_req_destroy(&handle);
    *result = seat_r_ok;
}

void client_seat_list_free(seat_t * seat)
{
	if (seat != NULL)
	{
		ep_free(seat);
	}
}