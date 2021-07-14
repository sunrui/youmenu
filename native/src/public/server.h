//
//  server.h
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-10.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//

#ifndef wx_youmenu_server_h
#define wx_youmenu_server_h

#ifdef __cplusplus
extern "C" {
#endif

int server_startup(int listen_port);
int server_stop();

#ifdef __cplusplus
}
#endif

#endif
