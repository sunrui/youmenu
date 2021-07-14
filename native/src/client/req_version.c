//
//  req_version.c
//  wx.youmenu
//
//  Created by 孙瑞 on 14-4-21.
//  Copyright (c) 2014年 www.honeysense.com. All rights reserved.
//
#include "client.h"
#include "identity.h"
#include "connect.h"

#include <assert.h>
#include <string.h>

/*
 * check newest version from server
 *
 * @return
 *     1 check success
 *     0 no response
 */
int client_check_version(connect_t * connect, platfrom_id pid, int cur_vid, version_status_t * version)
{
	ep_req_t * handle;
	ep_pack_t * pack;
	int ret;
    
	assert(version != NULL);
	memset(version, 0, sizeof(version_status_t));
	
	pack = ep_pack_new();
	ep_pack_append_int(pack, "pid", pid);
    ep_pack_append_int(pack, "cur_ver_id", cur_vid);
    
	{
		REQ_MAKE_PARAM(param, ep_id_version_check, pack);
		handle = ep_req_new(connect->sock, &param, NULL);
		ep_pack_destroy(&pack);
		if (handle == NULL)
		{
			return 0;
		}
	}
    
	ret = ep_req_wait(handle, &pack, REQ_DEF_TIMEOUT);
	if (ret == 0)
	{
		ep_req_destroy(&handle);
		return 0;
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
            
			if (strcmp(key, "new_ver_id") == 0)
			{
				version->new_ver.ver_id = ep_pack_iterator_int(iter);
				assert(type == PACKET_INT);
			}
			else if (strcmp(key, "new_ver_string") == 0)
			{
				strncpy(version->new_ver.string, ep_pack_iterator_string(iter), sizeof(version->new_ver.string));
				assert(type == PACKET_STRING);
			}
			else if (strcmp(key, "url") == 0)
			{
				strncpy(version->url, ep_pack_iterator_string(iter), sizeof(version->url));
				assert(type == PACKET_STRING);
			}
			if (strcmp(key, "update_logs") == 0)
			{
				strncpy(version->update_logs, ep_pack_iterator_string(iter), sizeof(version->update_logs));
				assert(type == PACKET_STRING);
			}
		}
	}
    
	ep_req_destroy(&handle);
    
	return 1;
}