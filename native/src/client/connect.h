//
//  req_declare.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-19.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_req_declare_h
#define wx_youmenu_req_declare_h

#include "../../eventpack/include/eventpack.h"
#include "timer.h"

typedef struct connect
{
	client_session_t * session;
	ep_client_t * sock;

//    timer_t * timer;
//    st_timer_t * heartbeat;

    pfn_client_disconnect_cb disc_cb;
    void * disc_cb_opaque;
} connect_t;

#endif
