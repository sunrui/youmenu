//
//  req_cook.c
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
void client_cook_accept(connect_t * connect, int seat_id, int dish_id, int number, cook_result * result)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
	
	if (connect == NULL || connect->session == NULL || seat_id < 0 || dish_id < 0 || number < 0)
	{
		*result = cook_r_illegal_parameter;
		return;
	}
    
    if (connect->session->account.type != account_admin && connect->session->account.type != account_cook)
    {
        *result = cook_r_no_permission;
        return;
    }
    
    pack = ep_pack_new();
    ep_pack_append_int(pack, "seat_id", seat_id);
    ep_pack_append_int(pack, "dish_id", dish_id);
    ep_pack_append_int(pack, "number", number);
	
	{
		REQ_MAKE_PARAM(param, ep_id_cook_accept, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			*result = cook_r_no_response;
			return;
		}
	}
	
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret <= 0)
	{
		ep_req_destroy(&handle);
        *result = cook_r_no_response;
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