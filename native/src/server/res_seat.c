//
//  res_seat.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-24.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "../../eventpack/include/eventpack.h"

#include "db.h"
#include "session.h"

#include <assert.h>
#include <string.h>

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
ep_pack_t * res_seat_list(void * opaque, int fd, ep_pack_t * pack, int * b_req_reclaim)
{
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

        session_dref(session);
    }
    
    pack = ep_pack_new();

    {
        seat_t * _seat;
        int count;
        
        count = db_seat_get(&_seat);
        if (count == 0)
        {
            ep_pack_append_int(pack, "result", dish_r_ok);
            ep_pack_append_int(pack, "count", 0);
            return pack;
        }
        
        ep_pack_append_int(pack, "result", dish_r_ok);
        ep_pack_append_int(pack, "count", count);
        ep_pack_append_start_array(pack, "seat");
        for (int i = 0; i < count; i++)
        {
            ep_pack_append_start_object(pack, "seat_one");
            ep_pack_append_int(pack, "id", _seat[i].id);
            ep_pack_append_string(pack, "summary", _seat[i].summary);
            ep_pack_append_string(pack, "qrcode", _seat[i].qrcode);
            ep_pack_append_finish_object(pack);
        }
        ep_pack_append_finish_array(pack);
    }
    
    return pack;
}